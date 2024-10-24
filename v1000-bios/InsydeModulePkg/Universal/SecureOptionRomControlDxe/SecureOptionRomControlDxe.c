/** @file

  Secure Option ROM Control implementation.

;******************************************************************************
;* Copyright (c) 2014 - 2021, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  PCI Rom supporting funtions implementation for PCI Bus module.

Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <SecureOptionRomControlDxe.h>
#include <SecureOptionRomControlSetup/SecureOptionRomControlSetup.h>

H2O_SECURITY_ARCH_PROTOCOL mH2OSecurityArch = {
                             SecurOptionRomControlAuthenticate
                           };

BOOLEAN                    mOpRomListInitialized = FALSE;
BOOLEAN                    mOpRomPolicyInitialized = FALSE;
EFI_LIST_ENTRY             mOpRomListHeader;
UINT16                     mCurrentOpRomNum;
UINT16                     mLockOpRomVarNum;

EFI_SECURITY2_FILE_AUTHENTICATION mOrgFileAuthentication;

/**
  Check if the device path is for VGA device.

  @param[in]  DevicePath        Pointer to the Device Path protocol of the option ROM.

  @retval     TRUE              The input device path is for VGA device.
              FALSE              The input device path is not for VGA device.

**/
BOOLEAN
EFIAPI
IsVgaDeviceFromDevicePath (
  IN CONST  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL                                *DevPath;
  EFI_DEVICE_PATH_PROTOCOL                                *DevPathPtr;
  EFI_STATUS                                              Status = EFI_NOT_FOUND;
  EFI_HANDLE                                              Handle;
  EFI_PCI_IO_PROTOCOL                                     *PciIo;
  UINT16                                                  ClassCode;
  BOOLEAN                                                 IsVgaDevice;

  DevPath = DuplicateDevicePath (DevicePath);
  DevPathPtr = DevPath;

  PciIo = NULL;
  Status = gBS->LocateDevicePath (&gEfiPciIoProtocolGuid, &DevPath, &Handle);
  if (!EFI_ERROR (Status)) {
    Status = gBS->HandleProtocol (Handle, &gEfiPciIoProtocolGuid, (VOID **)&PciIo);
  }
  FreePool (DevPathPtr);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  IsVgaDevice = FALSE;
  Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, 0xA, 1, &ClassCode);
  if (!EFI_ERROR (Status)) {
    if ((ClassCode == 0x0001) || (ClassCode == 0x0300)) {
      IsVgaDevice = TRUE;
    }
  }

  return IsVgaDevice;
}

/**
  Check if the device path is from PCI slot device.

  @param[in]  DevicePath        Pointer to the Device Path protocol of the option ROM.

  @retval     TRUE              The input device path is for PCI slot device.
              FALSE             The input device path is not for PCI slot device.

**/
BOOLEAN
EFIAPI
IsSupportedPciSlotDevicePath (
  IN CONST  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
  )
{
  EFI_STATUS                         Status;
  UINTN                              TokenNum;
  EFI_DEVICE_PATH_PROTOCOL           *WorkingDevicePath;
  EFI_DEVICE_PATH_PROTOCOL           *PlatformDevPath;
  EFI_DEVICE_PATH_PROTOCOL           *TempDevicePath;
  EFI_DEVICE_PATH_FROM_TEXT_PROTOCOL *DevPathFromText;
  BOOLEAN                            Match;
  EFI_GUID                           TargetTokenSpaceGuid;
  UINT32                             DeviceAttribCount;

  if (!IsDevicePathValid(DevicePath, 0) || DevicePath == NULL) {
    return FALSE;
  }

  Status = gBS->LocateProtocol (
                  &gEfiDevicePathFromTextProtocolGuid,
                  NULL,
                  (VOID **) &DevPathFromText
                  );
  if (EFI_ERROR(Status)) {
    return FALSE;
  }

  TargetTokenSpaceGuid = gH2ODeviceInfoTokenSpaceGuid;
  TempDevicePath = DuplicateDevicePath(DevicePath);

  Status = EFI_NOT_FOUND;
  TokenNum = LibPcdGetNextToken(&TargetTokenSpaceGuid, 0);
  while (TokenNum != 0) {
    WorkingDevicePath = TempDevicePath;
    if (IsDevicePathPci(TokenNum) != TRUE) {
      TokenNum = LibPcdGetNextToken(&TargetTokenSpaceGuid, TokenNum);
      continue;
    }
    Match = TRUE;
    Status = GetDeviceInfo(TokenNum, &PlatformDevPath, &DeviceAttribCount);
    if (EFI_ERROR (Status)) {
      TokenNum = LibPcdGetNextToken(&TargetTokenSpaceGuid, TokenNum);
      continue;
    }

    while (!IsDevicePathEnd (PlatformDevPath)) {
      if (CompareMem (WorkingDevicePath, PlatformDevPath, DevicePathNodeLength (WorkingDevicePath)) != 0) {
        Match = FALSE;
      }
      WorkingDevicePath = NextDevicePathNode (WorkingDevicePath);
      PlatformDevPath = NextDevicePathNode (PlatformDevPath);
    }

    if (!Match) {
      TokenNum = LibPcdGetNextToken(&TargetTokenSpaceGuid, TokenNum);
    } else {
      //
      // Find the device path is included in the pre-defined PCD gH2ODeviceInfoTokenSpaceGuid.
      //
      Status = EFI_SUCCESS;
      break;
    }
  }

  FreePool (TempDevicePath);
  if (Status == EFI_SUCCESS) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Calculate the HASH for device ROM image.

  @param[in]  RomImage           Pointer to the Option Rom.
  @param[in]  RomSize            Size of Option Rom.
  @param[out]  HashValue         Pointer to the Option Rom HASH value.

  @retval EFI_SUCCESS            Get the HASH value successfully.
  @retval Others                 Get the HASH value failure.

**/
EFI_STATUS
EFIAPI
GetHashValue (
  IN  VOID                            *RomImage,
  IN  UINTN                           RomSize,
  OUT UINT8                           *HashValue
  )
{
  VOID                        *Sha256Context;
  UINTN                       Sha256ContextSize;
  BOOLEAN                     Sha256Sts;

  if (HashValue == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Sha256Sts = FALSE;
  Sha256ContextSize = 0;
  Sha256ContextSize = Sha256GetContextSize ();
  Sha256Context = AllocateZeroPool(Sha256ContextSize);
  if (Sha256Context == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Sha256Sts = Sha256Init (Sha256Context);
  if (Sha256Sts == FALSE) {
    return EFI_UNSUPPORTED;
  }

  Sha256Sts = Sha256Update (Sha256Context, RomImage, RomSize);
  if (Sha256Sts == FALSE) {
    return EFI_UNSUPPORTED;
  }

  Sha256Sts = Sha256Final (Sha256Context, HashValue);
  if (Sha256Sts == FALSE) {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

/**
  Load the EFI Image from Option ROM.

  @param[in]  PciIo         PCI IO protocol.
  @param[in]  FilePath      The file path of the EFI Image
  @param[out] BufferSize    On input the size of Buffer in bytes. On output with a return
                            code of EFI_SUCCESS, the amount of data transferred to Buffer.
                            On output with a return code of EFI_BUFFER_TOO_SMALL,
                            the size of Buffer required to retrieve the requested file.
  @param[in]  Buffer        The memory buffer to transfer the file to. If Buffer is NULL,
                            then no the size of the requested file is returned in BufferSize.

  @retval EFI_SUCCESS           The file was loaded.
  @retval EFI_INVALID_PARAMETER FilePath is not a valid device path, or
                                BufferSize is NULL.
  @retval EFI_NOT_FOUND         Not found PCI Option Rom on PCI device.
  @retval EFI_DEVICE_ERROR      Failed to decompress PCI Option Rom image.
  @retval EFI_BUFFER_TOO_SMALL  The BufferSize is too small to read the current directory entry.
                                BufferSize has been updated with the size needed to complete the request.
**/
EFI_STATUS
EFIAPI
LocalLoadFile (
  IN EFI_PCI_IO_PROTOCOL      *PciIo,
  IN EFI_DEVICE_PATH_PROTOCOL *FilePath,
  IN OUT UINTN                *BufferSize,
  IN VOID                     *Buffer      OPTIONAL
  )
{
  EFI_STATUS                                Status;
  MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH   *EfiOpRomImageNode;
  EFI_PCI_EXPANSION_ROM_HEADER              *EfiRomHeader;
  PCI_DATA_STRUCTURE                        *Pcir;
  UINT32                                    ImageSize;
  UINT8                                     *ImageBuffer;
  UINT32                                    ImageLength;
  UINT32                                    DestinationSize;
  UINT32                                    ScratchSize;
  VOID                                      *Scratch;
  EFI_DECOMPRESS_PROTOCOL                   *Decompress;

  EfiOpRomImageNode = (MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH *) FilePath;
  if ((EfiOpRomImageNode == NULL) ||
      (DevicePathType (FilePath) != MEDIA_DEVICE_PATH) ||
      (DevicePathSubType (FilePath) != MEDIA_RELATIVE_OFFSET_RANGE_DP) ||
      (DevicePathNodeLength (FilePath) != sizeof (MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH)) ||
      (!IsDevicePathEnd (NextDevicePathNode (FilePath))) ||
      (EfiOpRomImageNode->StartingOffset > EfiOpRomImageNode->EndingOffset) ||
      (EfiOpRomImageNode->EndingOffset >= PciIo->RomSize) ||
      (BufferSize == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  EfiRomHeader = (EFI_PCI_EXPANSION_ROM_HEADER *)((UINT8 *) (PciIo->RomImage) + (EfiOpRomImageNode->StartingOffset));
  if (EfiRomHeader->Signature != PCI_EXPANSION_ROM_HEADER_SIGNATURE) {
    return EFI_NOT_FOUND;
  }

  Pcir = (PCI_DATA_STRUCTURE *) ((UINT8 *) EfiRomHeader + EfiRomHeader->PcirOffset);

  if ((Pcir->CodeType == PCI_CODE_TYPE_EFI_IMAGE) &&
      (EfiRomHeader->EfiSignature == EFI_PCI_EXPANSION_ROM_HEADER_EFISIGNATURE) &&
      ((EfiRomHeader->EfiSubsystem == EFI_IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER) ||
       (EfiRomHeader->EfiSubsystem == EFI_IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER)) &&
      (EfiRomHeader->CompressionType <= EFI_PCI_EXPANSION_ROM_HEADER_COMPRESSED)) {
    ImageSize   = (UINT32) EfiRomHeader->InitializationSize * 512;
    ImageBuffer = (UINT8*) EfiRomHeader + EfiRomHeader->EfiImageHeaderOffset;
    ImageLength = ImageSize - EfiRomHeader->EfiImageHeaderOffset;

    if (EfiRomHeader->CompressionType != EFI_PCI_EXPANSION_ROM_HEADER_COMPRESSED) {
      //
      // Uncompressed: Copy the EFI Image directly to user's buffer
      //
      if (Buffer == NULL || *BufferSize < ImageLength) {
        *BufferSize = ImageLength;
        return EFI_BUFFER_TOO_SMALL;
      }

      *BufferSize = ImageLength;
      CopyMem (Buffer, ImageBuffer, ImageLength);
      return EFI_SUCCESS;
    } else {
      //
      // Compressed: Uncompress before copying
      //
      Status = gBS->LocateProtocol (&gEfiDecompressProtocolGuid, NULL, (VOID **) &Decompress);
      if (EFI_ERROR (Status)) {
        return EFI_DEVICE_ERROR;
      }
      Status = Decompress->GetInfo (
                             Decompress,
                             ImageBuffer,
                             ImageLength,
                             &DestinationSize,
                             &ScratchSize
                             );
      if (EFI_ERROR (Status)) {
        return EFI_DEVICE_ERROR;
      }

      if (Buffer == NULL || *BufferSize < DestinationSize) {
        *BufferSize = DestinationSize;
        return EFI_BUFFER_TOO_SMALL;
      }

      *BufferSize = DestinationSize;
      Scratch = AllocateZeroPool (ScratchSize);
      if (Scratch == NULL) {
        return EFI_DEVICE_ERROR;
      }

      Status = Decompress->Decompress (
                             Decompress,
                             ImageBuffer,
                             ImageLength,
                             Buffer,
                             DestinationSize,
                             Scratch,
                             ScratchSize
                             );
      FreePool (Scratch);
      if (EFI_ERROR (Status)) {
        return EFI_DEVICE_ERROR;
      }
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**
  Validate the content of the option ROM option content from variable.

  @param[in]   *OpRomOption       - The point of option ROM option from OpRomXXXX variable.

  @retval EFI_SUCCESS             - The content is valid.
  @retval EFI_UNSUPPORTED         - The content is invalid

**/
EFI_STATUS
EFIAPI
ValidateOpRomVariable (
  IN OPTION_ROM_OPTION            *OpRomOption
  )
{
  DEBUG ((DEBUG_INFO, "Option ROM VendorID: 0x%04x\n", OpRomOption->VendorId));
  DEBUG ((DEBUG_INFO, "           DeviceID: 0x%04x\n", OpRomOption->DeviceId));

  if ((OpRomOption->Attribute != OP_ROM_ACTIVE) && (OpRomOption->Attribute != OP_ROM_INACTIVE)) {
    DEBUG ((DEBUG_ERROR, "ERROR Format >> Attribute: %d\n", OpRomOption->Attribute));
    return EFI_UNSUPPORTED;
  } else {
    DEBUG ((DEBUG_INFO, "           Attribute: %d\n", OpRomOption->Attribute));
  }

  if ((OpRomOption->FoundRomType & (~(HAVE_LEGACY_ROM | HAVE_UNSIGNED_EFI_ROM | HAVE_SIGNED_EFI_ROM))) != 0) {
    DEBUG ((DEBUG_ERROR, "ERROR Format >> FoundRomType: %d\n", OpRomOption->FoundRomType));
    return EFI_UNSUPPORTED;
  } else {
    DEBUG ((DEBUG_INFO, "           FoundRomType: %d\n", OpRomOption->FoundRomType));
  }

  if (OpRomOption->OpRomPolicy >= INVALID_SUPPORT_OPTION_ROM) {
    DEBUG ((DEBUG_ERROR, "ERROR Format >> OpRomPolicy: %d\n", OpRomOption->OpRomPolicy));
    return EFI_UNSUPPORTED;
  } else {
    DEBUG ((DEBUG_INFO, "           OpRomPolicy: %d\n", OpRomOption->OpRomPolicy));
  }

  return EFI_SUCCESS;
}

/**
  Build the OpRom#### option from the VariableName and the build OpRom#### will
  also be linked to BdsCommonOptionList.

  @param[in,out] *OpRomList   - The header of the OpRom#### option link list
  @param[in]     OptionNumber - The number for the OpRom#### variable.

  @retval OPTION_ROM_LIST_OPTION  - Get the option just been created
  @retval NULL                    - Failed to get the new option

**/
OPTION_ROM_LIST_OPTION *
EFIAPI
PciOpRomVariableToOption (
  IN OUT EFI_LIST_ENTRY               *OpRomList,
  IN     UINT16                       OptionNumber
  )
{
  EFI_STATUS                   Status;
  UINTN                        VariableSize;
  UINT8                        *Variable;
  OPTION_ROM_LIST_OPTION       *Option;
  CHAR16                       OptionName[20];
  OPTION_ROM_OPTION            *OpRomOption;

  UnicodeSPrint (OptionName, sizeof (OptionName), L"OpRom%04x", OptionNumber);
  //
  // Read the variable. We will never free this data.
  //
  Variable = NULL;
  Status = CommonGetVariableDataAndSize (
             OptionName,
             &gH2OSecureOptionRomControlFormsetGuid,
             &VariableSize,
             (VOID **) &Variable
             );
  if (EFI_ERROR (Status) || (Variable == NULL)) {
    DEBUG ((DEBUG_ERROR, "PciOpRomVariableToOption(): Get OpRom%04x: %r.\n", OptionNumber, Status));
    return NULL;
  }

  //
  // Validate the variable content.
  //
  OpRomOption = (OPTION_ROM_OPTION *) Variable;
  Status = ValidateOpRomVariable (OpRomOption);
  if (EFI_ERROR (Status)) {
    FreePool (Variable);
    return NULL;
  }

  Option = NULL;
  Option = AllocateZeroPool (sizeof (OPTION_ROM_LIST_OPTION) + OpRomOption->DevicePathSize);
  if (Option == NULL) {
    FreePool (Variable);
    return NULL;
  }
  Option->Signature = OPTION_ROM_LIST_OPTION_SIGNATURE;
  CopyMem (&Option->OpRomOption, Variable, VariableSize);
  Option->OptionNumber = OptionNumber;
  InsertTailList (OpRomList, &Option->Link);
  FreePool (Variable);
  return Option;
}

/**
  This function uses PCI device path, PCI Vendor ID, Device ID to get supported Option ROM type from option ROM list.

  @param[in]  DevicePath             - Device Path for the PCI device.
  @param[in]  VendorId               - PCI Vendor ID.
  @param[in]  DeviceId               - PCI Device ID.
  @param[in]  RomHash                - Pointer to the HASH value of PCI Device ROM.
  @param[out] SupportedPolicy        - Pointer to save supported option ROM type.

  @retval EFI_SUCCESS            - Get supported ROM image type successful.
  @retval EFI_NOT_FOUND          - Cannot find Supported type from option ROM list
  @retval EFI_INVALID_PARAMETER  - Any input parameter is invalid.

**/
EFI_STATUS
EFIAPI
GetOpRomPolicyForDevice (
  IN CONST  EFI_DEVICE_PATH_PROTOCOL          *DevicePath,
  IN        UINT16                             VendorId,
  IN        UINT16                             DeviceId,
  IN        UINT8                              *RomHash,
  OUT       UINT8                              *SupportedPolicy
 )
{
  OPTION_ROM_LIST_OPTION       *Option;
  EFI_LIST_ENTRY               *Link;
  EFI_DEVICE_PATH_PROTOCOL     *PlatformDevPath;
  EFI_DEVICE_PATH_PROTOCOL     *WorkingDevicePath;
  BOOLEAN                      MatchDevicePath;
  BOOLEAN                      MatchRomHash;

  if (VendorId == 0xFFFF || DeviceId == 0xFFFF || SupportedPolicy == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!mOpRomListInitialized) {
    return EFI_NOT_FOUND;
  }

  MatchDevicePath = FALSE;
  MatchRomHash = FALSE;
  for (Link = mOpRomListHeader.ForwardLink; Link != &mOpRomListHeader; Link = Link->ForwardLink) {
    Option = CR (Link, OPTION_ROM_LIST_OPTION, Link, OPTION_ROM_LIST_OPTION_SIGNATURE);
    if (FeaturePcdGet (PcdH2OPciOptionRomSecurityControlBySlot)) {
      //
      // Slot-based checking.
      //

      //
      // Compare the device path.
      //
      PlatformDevPath = (EFI_DEVICE_PATH_PROTOCOL *) Option->OpRomOption.DevicePath;
      WorkingDevicePath = (EFI_DEVICE_PATH_PROTOCOL*) DevicePath;
      if(!IsDevicePathEnd (PlatformDevPath)) {
        MatchDevicePath = TRUE;
      }
      while (!IsDevicePathEnd (PlatformDevPath)) {
        if (CompareMem (PlatformDevPath, WorkingDevicePath, DevicePathNodeLength (PlatformDevPath)) != 0) {
          MatchDevicePath = FALSE;
          break;
        }
        WorkingDevicePath = NextDevicePathNode (WorkingDevicePath);
        PlatformDevPath = NextDevicePathNode (PlatformDevPath);
      }

      //
      // Compare the HASH of ROM.
      //
      if ((MatchDevicePath == TRUE) && CompareMem (&Option->OpRomOption.HashValue, RomHash, OPROM_CONTROL_HASH_SIZE) == 0) {
        MatchRomHash = TRUE;
      }

      if ((MatchDevicePath == TRUE) && (MatchRomHash == TRUE)) {
        *SupportedPolicy = Option->OpRomOption.OpRomPolicy;
        return EFI_SUCCESS;
      }
    } else {
      //
      // ID-based checking.
      //
      if (Option->OpRomOption.VendorId == VendorId && Option->OpRomOption.DeviceId == DeviceId) {
        *SupportedPolicy = Option->OpRomOption.OpRomPolicy;
        return EFI_SUCCESS;
      }
    }
  }
  return EFI_NOT_FOUND;
}

/**
  According to vendor id and device id to check the option ROM policy exists or not.

  @param[in]  DevicePath  - Device Path of the PCI device.
  @param[in]  VendorId    - PCI Vendor ID.
  @param[in]  DeviceId    - PCI Device ID.
  @param[out] Option      - On return, a pointer to matched OPTION_ROM_LIST_OPTION instance.
                            If Option ROM policy doesn't exist, the option is set to NULL.

  @retval TRUE       - The option ROM policy exists.
  @retval FALSE      - The option ROM policy doesn't exists.

**/
BOOLEAN
EFIAPI
DoesOpRomPolicyExist (
  IN CONST  EFI_DEVICE_PATH_PROTOCOL           *DevicePath,
  IN        UINT16                             VendorId,
  IN        UINT16                             DeviceId,
  OUT       OPTION_ROM_LIST_OPTION             **OpRomOption
  )
{
  OPTION_ROM_LIST_OPTION       *Option;
  EFI_LIST_ENTRY               *Link;
  EFI_DEVICE_PATH_PROTOCOL     *PlatformDevPath;
  EFI_DEVICE_PATH_PROTOCOL     *WorkingDevicePath;
  BOOLEAN                      Match;

  if (!mOpRomListInitialized) {
    return FALSE;
  }
  Match = FALSE;
  for (Link = mOpRomListHeader.ForwardLink; Link != &mOpRomListHeader; Link = Link->ForwardLink) {
    Option = CR (Link, OPTION_ROM_LIST_OPTION, Link, OPTION_ROM_LIST_OPTION_SIGNATURE);
    if (FeaturePcdGet (PcdH2OPciOptionRomSecurityControlBySlot)) {
      //
      // Slot-based checking.
      //
      PlatformDevPath = (EFI_DEVICE_PATH_PROTOCOL *) Option->OpRomOption.DevicePath;
      if(!IsDevicePathEnd (PlatformDevPath)) {
        Match = TRUE;
      }
      WorkingDevicePath = (EFI_DEVICE_PATH_PROTOCOL*)DevicePath;
      while (!IsDevicePathEnd (PlatformDevPath)) {
        if (CompareMem (PlatformDevPath, WorkingDevicePath, DevicePathNodeLength (PlatformDevPath)) != 0) {
          Match = FALSE;
          break;
        }
        WorkingDevicePath = NextDevicePathNode (WorkingDevicePath);
        PlatformDevPath = NextDevicePathNode (PlatformDevPath);
      }
      if (Match == TRUE) {
        *OpRomOption = Option;
        return Match;
      }
    } else {
      //
      // ID-based checking.
      //
      if (Option->OpRomOption.VendorId == VendorId && Option->OpRomOption.DeviceId == DeviceId) {
        *OpRomOption = Option;
        return TRUE;
      }
    }
  }

  return FALSE;
}

/**
  Lock variable to sensitive variable.

  @param[in] CurOpRomNum  - Pointer to current OP ROM option number

  @retval EFI_SUCCESS   - Lock Option Rom variable to sensitive variable successfully.

**/
EFI_STATUS
LockSensitiveVariables (
  IN  UINT16                  CurOpRomNum
  )
{
  EFI_STATUS                    Status;
  EDKII_VARIABLE_LOCK_PROTOCOL  *VariableLock;
  CHAR16                        OptionName[20];
  UINTN                         Index;
  UINT16                        OptionRomNum;

  Status = gBS->LocateProtocol (&gEdkiiVariableLockProtocolGuid, NULL, (VOID **) &VariableLock);
  if (!EFI_ERROR (Status)) {
    //
    // Lock related variable to sensitive variable.
    //
    Status = VariableLock->RequestToLock (VariableLock, L"SecOpRomConfig", &gH2OSecureOptionRomControlFormsetGuid);
    DEBUG ((DEBUG_INFO, "VariableLock for \"SecOpRomConfig\": %r\n", Status));

    Status = VariableLock->RequestToLock (VariableLock, L"OpRomOrder", &gH2OSecureOptionRomControlFormsetGuid);
    DEBUG ((DEBUG_INFO, "VariableLock for \"OpRomOrder\": %r\n", Status));

    if (CurOpRomNum > PcdGet16(PcdH2OPciOptionRomControlNum)) {
      OptionRomNum = CurOpRomNum;
    } else {
      OptionRomNum = PcdGet16(PcdH2OPciOptionRomControlNum);
    }

    for (Index = 0; Index < OptionRomNum; Index++) {
      UnicodeSPrint (OptionName, sizeof (OptionName), L"OpRom%04x", Index);
      Status = VariableLock->RequestToLock (VariableLock, OptionName, &gH2OSecureOptionRomControlFormsetGuid);
      DEBUG ((DEBUG_INFO, "VariableLock for \"%s\": %r\n", OptionName, Status));
    }

    mLockOpRomVarNum = OptionRomNum;
  }

  return Status;
}

/**
  Get new Option ROM option number for OpRom#### variable.

  @param[out] NewOptionNum  - Pointer to new OP ROM option number

  @retval EFI_SUCCESS   - Get New OP ROM option number

**/
EFI_STATUS
EFIAPI
GetNewOpRomOptionNo (
  OUT UINT16             *NewOptionNum
  )
{
  UINT16                     OptionNum;
  UINT16                     Index;
  EFI_LIST_ENTRY             *Link;
  OPTION_ROM_LIST_OPTION     *Option;
  BOOLEAN                    OptionNumFound;

  OptionNum = 0;
  for (Link = mOpRomListHeader.ForwardLink; Link != &mOpRomListHeader; Link = Link->ForwardLink) {
    OptionNum++;
  }
  for (Index = 0; Index < OptionNum; Index++) {
    OptionNumFound = FALSE;
    for (Link = mOpRomListHeader.ForwardLink; Link != &mOpRomListHeader; Link = Link->ForwardLink) {
      Option = CR (Link, OPTION_ROM_LIST_OPTION, Link, OPTION_ROM_LIST_OPTION_SIGNATURE);
      if (Option->OptionNumber == Index) {
        OptionNumFound = TRUE;
      }
    }
    if (!OptionNumFound) {
      break;
    }
  }
  *NewOptionNum = Index;
  return EFI_SUCCESS;
}

/**
 Function to compare 2 value.

 @param[in] Buffer1    Pointer to value to compare (UINTN*).
 @param[in] Buffer2    Pointer to second value to compare (UINTN*).

 @retval 0             Buffer1 equal to Buffer2.
 @retval <0            Buffer1 is less than Buffer2.
 @retval >0            Buffer1 is greater than Buffer2.
**/
INTN
EFIAPI
ValueCompare (
  IN CONST VOID                        *Buffer1,
  IN CONST VOID                        *Buffer2
  )
{
  if (*((UINTN *) Buffer1) == *((UINTN *) Buffer2)) {
    return 0;
  } else if (*((UINTN *) Buffer1) < *((UINTN *) Buffer2)) {
    return -1;
  } else {
    return 1;
  }
}

/**
  Get gH2ODeviceInfoTokenSpaceGuid PCD list and add to Rom list.

  @param[in] RomOptionList        - Pointer to the Option Rom list.
  @param[in] CurrentOptionRomNum  - Current Option Rom number.

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
GetDefaultOptionRomPolicyList (
  IN EFI_LIST_ENTRY      *RomOptionList,
  IN UINT16              *CurrentOptionRomNum
  )
{
  EFI_STATUS                     Status;
  UINTN                          TokenNum;
  UINTN                          *TokenBuf;
  UINTN                          TokenCount;
  UINTN                          TokenIndex;
  OPTION_ROM_LIST_OPTION         *Option;
  EFI_LIST_ENTRY                 *Link;
  OPTION_ROM_LIST_OPTION         *OptionLink;
  BOOLEAN                        Match;
  EFI_DEVICE_PATH_PROTOCOL       *PlatformDevPath;
  EFI_DEVICE_PATH_PROTOCOL       *WorkingDevicePath;
  EFI_DEVICE_PATH_PROTOCOL       *TempDevicePath;
  UINT32                         DeviceAttribCount;
  CHAR16                         *DevicePathStr;
  EFI_GUID                       TargetTokenSpaceGuid;
  EFI_LIST_ENTRY                 *TempRomOptionList;

  TempRomOptionList = (EFI_LIST_ENTRY *) RomOptionList;
  TokenBuf = AllocateZeroPool(sizeof (UINTN) * MAX_OPTION_ROM_POLICY);
  if (TokenBuf == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Get the Device Info list
  //
  TargetTokenSpaceGuid = gH2ODeviceInfoTokenSpaceGuid;
  TokenNum = 0;
  TokenCount = 0;
  do {
    TokenNum = LibPcdGetNextToken(&TargetTokenSpaceGuid, TokenNum);
    if (TokenNum == 0) {
      break;
    }
    TokenBuf[TokenCount] = TokenNum;
    TokenCount++;
  } while (TRUE);

  if (TokenCount == 0) {
    FreePool (TokenBuf);
    return EFI_SUCCESS;
  }

  //
  // Sort the device info list
  //
  if (TokenCount > 1) {
    PerformQuickSort (TokenBuf, TokenCount, sizeof(UINTN), ValueCompare);
  }

  for (TokenIndex = 0; TokenIndex < TokenCount; TokenIndex++) {
    TokenNum = TokenBuf[TokenIndex];
    if (IsDevicePathPci(TokenNum) != TRUE) {
      continue;
    }
    Status = GetDeviceInfo(TokenNum, &WorkingDevicePath, &DeviceAttribCount);
    if (EFI_ERROR (Status)) {
      continue;
    }

    Match = FALSE;
    TempDevicePath = DuplicateDevicePath(WorkingDevicePath);
    if (!IsListEmpty (RomOptionList)) {
      Link = GetFirstNode (RomOptionList);
      while (1) {
        OptionLink = CR (Link, OPTION_ROM_LIST_OPTION, Link, OPTION_ROM_LIST_OPTION_SIGNATURE);
        PlatformDevPath = (EFI_DEVICE_PATH_PROTOCOL *) OptionLink->OpRomOption.DevicePath;
        if (!IsDevicePathEnd (PlatformDevPath)) {
          Match = TRUE;
        }
        while (!IsDevicePathEnd (PlatformDevPath)) {
          DevicePathStr = NULL;
          DevicePathStr = ConvertDeviceNodeToText(WorkingDevicePath, TRUE, TRUE);
          if (DevicePathStr != NULL) {
            DEBUG ((DEBUG_INFO, "WorkingDevicePath DevicePathStr: %s\n", DevicePathStr));
            FreePool (DevicePathStr);
          } else {
            DEBUG ((DEBUG_INFO, "WorkingDevicePath DevicePathStr is NULL\n"));
          }
          DevicePathStr = NULL;
          DevicePathStr = ConvertDeviceNodeToText(PlatformDevPath, TRUE, TRUE);
          if (DevicePathStr != NULL) {
            DEBUG ((DEBUG_INFO, "PlatformDevPath DevicePathStr: %s\n", DevicePathStr));
            FreePool (DevicePathStr);
          } else {
            DEBUG ((DEBUG_INFO, "PlatformDevPath DevicePathStr is NULL\n"));
          }
          if (CompareMem (PlatformDevPath, WorkingDevicePath, DevicePathNodeLength (PlatformDevPath)) != 0) {
            Match = FALSE;
            DEBUG ((DEBUG_INFO, "Match is false, break.\n"));
            break;
          }
          WorkingDevicePath = NextDevicePathNode (WorkingDevicePath);
          PlatformDevPath = NextDevicePathNode (PlatformDevPath);
        }

        if ((Match == TRUE) || IsNodeAtEnd (RomOptionList, Link)) {
          break;
        } else {
          Link = GetNextNode (RomOptionList, Link);
          WorkingDevicePath = TempDevicePath;
          DEBUG ((DEBUG_INFO, "\n"));
        }
      }
    } else {
      DEBUG ((DEBUG_INFO, "List is Empty.\n"));
    }
    if (Match) {
      DEBUG ((DEBUG_INFO, "Get the same Data in list.\n"));
    } else {
      DEBUG ((DEBUG_INFO, "Get not the same Data in list.\n"));
      DEBUG ((DEBUG_INFO, "*CurrentOptionRomNum: 0x%x\n", *CurrentOptionRomNum));
      Option = AllocateZeroPool (sizeof (OPTION_ROM_LIST_OPTION) + GetDevicePathSize (TempDevicePath));
      GetNewOpRomOptionNo (&(Option->OptionNumber));
      Option->OpRomOption.VendorId = 0x0000;
      Option->OpRomOption.DeviceId = 0x0000;
      Option->Signature = OPTION_ROM_LIST_OPTION_SIGNATURE;
      Option->OpRomOption.Attribute = OP_ROM_INACTIVE;
      Option->OpRomOption.FoundRomType = 0;
      Option->OpRomOption.OpRomPolicy = UNSUPPORT_ALL_OPTION_ROM;
      Option->OpRomOption.DevicePathSize = GetDevicePathSize (TempDevicePath);
      CopyMem (&(Option->OpRomOption.DevicePath), TempDevicePath, Option->OpRomOption.DevicePathSize);
      InsertTailList (RomOptionList, &Option->Link);
      (*CurrentOptionRomNum)++;
    }

    FreePool (TempDevicePath);
  }

  RomOptionList = (EFI_LIST_ENTRY *) TempRomOptionList;

  FreePool (TokenBuf);
  return EFI_SUCCESS;
}

/**
  This function uses OpRom#### and OpRomOrder varialb to initialize
  option ROM list.

  @param[in]  Event        Event whose notification function is being invoked
  @param[in]  Context      Pointer to the notification function's context

  @retval EFI_SUCCESS         - Initialize option ROM list successful.
  @retval EFI_ALREADY_STARTED - The option ROM list has been initialized

**/
VOID
EFIAPI
InitializeOptionRomList (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINT16                        *OptionRomOrder;
  UINTN                         OptionRomOrderSize;
  UINTN                         Index;
  OPTION_ROM_LIST_OPTION        *Option;
  VOID                          *Interface;
  UINT16                        CurrentOptionRomNum;

  DEBUG ((DEBUG_INFO, "InitializeOptionRomList() Start.\n"));
  Status = gBS->LocateProtocol (&gEfiVariableArchProtocolGuid, NULL, (VOID **) &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }

  if (mOpRomListInitialized) {
    return;
  }

  InitializeListHead (&mOpRomListHeader);
  mOpRomListInitialized = TRUE;

  OptionRomOrder = NULL;
  Status = CommonGetVariableDataAndSize(
             L"OpRomOrder",
             &gH2OSecureOptionRomControlFormsetGuid,
             &OptionRomOrderSize,
             (VOID **) &OptionRomOrder
             );
  if (EFI_ERROR (Status) || (OptionRomOrder == NULL)) {
    DEBUG ((DEBUG_INFO, "InitializeOptionRomList() Get OpRomOrder: %r.\n", Status));
    //
    // Lock sensitive variables with CurrentOptionRomNum = 0.
    //
    CurrentOptionRomNum = 0;

    if (FeaturePcdGet (PcdH2OPciOptionRomSecurityControlBySlot)) {
      GetDefaultOptionRomPolicyList (&mOpRomListHeader, &CurrentOptionRomNum);
      UpdateOptionRomListToVariable ();
    }

    LockSensitiveVariables (CurrentOptionRomNum);
    return;
  }

  CurrentOptionRomNum = (UINT16)(OptionRomOrderSize / sizeof (UINT16));

  for (Index = 0; Index < CurrentOptionRomNum; Index++) {
    Option = PciOpRomVariableToOption (&mOpRomListHeader, OptionRomOrder[Index]);
    //
    // Set OpRom Attribute to inactive to initialize all of OpRom options
    //
    if (Option != NULL && !mOpRomPolicyInitialized) {
      Option->OpRomOption.Attribute = OP_ROM_INACTIVE;
      Option->OpRomOption.FoundRomType = 0;
    }
  }

  if (FeaturePcdGet (PcdH2OPciOptionRomSecurityControlBySlot)) {
    GetDefaultOptionRomPolicyList (&mOpRomListHeader, &CurrentOptionRomNum);
    UpdateOptionRomListToVariable ();
  }

  if (OptionRomOrder != NULL) {
    FreePool (OptionRomOrder);
  }

  //
  // Lock sensitive variables.
  //
  LockSensitiveVariables (CurrentOptionRomNum);

  DEBUG ((DEBUG_INFO, "InitializeOptionRomList() End.\n"));
  return;
}

/**

  Add Specific PCI Rom to option ROM list.
  @param[in] DevicePath       - Device Path of the PCI device.
  @param[in] VendorId         - PCI Vendor ID.
  @param[in] DeviceId         - PCI Device ID.
  @param[in] FoundRomType     - ROM type which contained in specific ROM image
  @param[in] OptionRomPolicy  - The option ROM policy for this type PCI option ROM
  @param[in] Active           - This option is active or not.
  @param[in] RomHash          - HASH value of the ROM image.

  @retval EFI_SUCCESS            - Add option ROM to option ROM list successful.
  @retval EFI_NOT_READY          - The option ROM list isn't initialized
  @retval EFI_INVALID_PARAMETER  - Any input parameter is invalid.

**/
EFI_STATUS
EFIAPI
AddRomImageToList (
  IN CONST  EFI_DEVICE_PATH_PROTOCOL           *DevicePath,
  IN        UINT16                             VendorId,
  IN        UINT16                             DeviceId,
  IN        UINT8                              FoundRomType,
  IN        UINT8                              OptionRomPolicy,
  IN        BOOLEAN                            Active,
  IN        UINT8                              *RomHash
  )
{
  OPTION_ROM_LIST_OPTION       *Option;
  UINTN                        PathSize;
  EFI_STATUS                   Status;

  Status = EFI_SUCCESS;
  if (VendorId == 0xFFFF || DeviceId == 0xFFFF) {
    return EFI_INVALID_PARAMETER;
  }

  if (!mOpRomListInitialized) {
    return EFI_NOT_READY;
  }

  if (DoesOpRomPolicyExist (DevicePath, VendorId, DeviceId, &Option)) {
    Option->OpRomOption.VendorId = VendorId;
    Option->OpRomOption.DeviceId = DeviceId;
    Option->OpRomOption.FoundRomType |= FoundRomType;
    Option->OpRomOption.OpRomPolicy = OptionRomPolicy;
    if ((Active == TRUE) && ((Option->OpRomOption.Attribute & OP_ROM_INACTIVE) == OP_ROM_INACTIVE)) {
      Option->OpRomOption.Attribute = OP_ROM_ACTIVE;
    }
    if (FeaturePcdGet (PcdH2OPciOptionRomSecurityControlBySlot)) {
      CopyMem (&Option->OpRomOption.HashValue, RomHash, OPROM_CONTROL_HASH_SIZE);
    }
  } else {
    if (FeaturePcdGet (PcdH2OPciOptionRomSecurityControlBySlot)) {
      PathSize = GetDevicePathSize (DevicePath);
    } else {
      PathSize = 0;
    }

    Option = AllocateZeroPool (sizeof (OPTION_ROM_LIST_OPTION) + PathSize);
    if (Option == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    Option->Signature = OPTION_ROM_LIST_OPTION_SIGNATURE;
    Option->OpRomOption.Attribute = (UINT8)((Active == TRUE) ? OP_ROM_ACTIVE : OP_ROM_INACTIVE);
    Option->OpRomOption.FoundRomType = FoundRomType;
    Option->OpRomOption.OpRomPolicy = OptionRomPolicy;
    Option->OpRomOption.VendorId = VendorId;
    Option->OpRomOption.DeviceId = DeviceId;
    Option->OpRomOption.DevicePathSize = PathSize;
    if (FeaturePcdGet (PcdH2OPciOptionRomSecurityControlBySlot)) {
      CopyMem (&Option->OpRomOption.HashValue, RomHash, OPROM_CONTROL_HASH_SIZE);
      CopyMem (&Option->OpRomOption.DevicePath, DevicePath, PathSize);
    }

    GetNewOpRomOptionNo (&Option->OptionNumber);
    InsertTailList (&mOpRomListHeader, &Option->Link);
  }

  Status = UpdateOptionRomListToVariable ();
  return Status;
}

/**

  This function uses option list to save OpRom#### and OpRomOrder variable.

  @param None

  @retval EFI_SUCCESS         - Update OpRom#### and OpRomOrder variable Successful.
  @retval EFI_NOT_READY       - The option ROM list hasn't been initialized

**/
EFI_STATUS
EFIAPI
UpdateOptionRomListToVariable (
  VOID
  )
{
  UINT16                        OptionNum;
  UINT16                        Index;
  UINT16                        *OptionRomOrder;
  EFI_LIST_ENTRY                *Link;
  OPTION_ROM_LIST_OPTION        *Option;
  CHAR16                        OptionName[20];
  EFI_STATUS                    Status;
  UINTN                         OptionRomOrderSize;
  UINT16                        *OpRomOrder;

  if (!mOpRomListInitialized) {
    return EFI_NOT_READY;
  }

  OptionNum = 0;
  for (Link = mOpRomListHeader.ForwardLink; Link != &mOpRomListHeader; Link = Link->ForwardLink) {
    Option = CR (Link, OPTION_ROM_LIST_OPTION, Link, OPTION_ROM_LIST_OPTION_SIGNATURE);
    UnicodeSPrint (OptionName, sizeof (OptionName), L"OpRom%04x", Option->OptionNumber);
    Status = SetVariableToSensitiveVariable (
               OptionName,
               &gH2OSecureOptionRomControlFormsetGuid,
               EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
               sizeof (OPTION_ROM_OPTION) + Option->OpRomOption.DevicePathSize,
               &(Option->OpRomOption)
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "SetVariableToSensitiveVariable %s: %r\n", OptionName, Status));
    }

    OptionNum++;
  }

  DEBUG ((DEBUG_INFO, "UpdateOptionRomListToVariable() OptionNum: %x\n", OptionNum));
  mCurrentOpRomNum = OptionNum;

  OptionRomOrder = AllocateZeroPool (OptionNum * sizeof (UINT16));
  if (OptionRomOrder == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Index = 0;
  for (Link = mOpRomListHeader.ForwardLink; Link != &mOpRomListHeader; Link = Link->ForwardLink) {
    Option = CR (Link, OPTION_ROM_LIST_OPTION, Link, OPTION_ROM_LIST_OPTION_SIGNATURE);
    OptionRomOrder[Index++] = Option->OptionNumber;
  }

  Status = SetVariableToSensitiveVariable (
             L"OpRomOrder",
             &gH2OSecureOptionRomControlFormsetGuid,
             EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
             OptionNum * sizeof (UINT16),
             OptionRomOrder
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "SetVariableToSensitiveVariable \"OpRomOrder\": %r\n", Status));
  } else {
    DEBUG_CODE (
      Status = CommonGetVariableDataAndSize(
                 L"OpRomOrder",
                 &gH2OSecureOptionRomControlFormsetGuid,
                 &OptionRomOrderSize,
                 (VOID **) &OpRomOrder
                 );
      if (EFI_ERROR (Status) || (OpRomOrder == NULL)) {
        DEBUG ((DEBUG_INFO, "GetVariable OpRomOrder: %r.\n", Status));
      } else {
        DEBUG ((DEBUG_INFO, "OptionRomOrderSize : 0x%x.\n", OptionRomOrderSize));
        for (Index = 0; Index < (OptionRomOrderSize / sizeof(UINT16)); Index++) {
          DEBUG ((DEBUG_INFO, "OpRomOrder[%d] = %d\n", Index, OpRomOrder[Index]));
        }
        FreePool (OpRomOrder);
      }
    );
  }

  mOpRomPolicyInitialized = TRUE;
  FreePool (OptionRomOrder);

  return Status;
}

/**

  This function will free all of allocated option ROM list.

  @param None

  @retval EFI_SUCCESS         - All of allocated option ROM list has been freed.

**/
EFI_STATUS
EFIAPI
FreeOptionRomList (
  VOID
  )
{
  OPTION_ROM_LIST_OPTION       *Option;
  EFI_LIST_ENTRY               *Link;

  if (!mOpRomListInitialized) {
    return EFI_SUCCESS;
  }
  while (!IsListEmpty (&mOpRomListHeader)) {
    Link = mOpRomListHeader.ForwardLink;
    Option = CR (Link, OPTION_ROM_LIST_OPTION, Link, OPTION_ROM_LIST_OPTION_SIGNATURE);
    RemoveEntryList (Link);
    FreePool (Option);
  }
  mOpRomListInitialized = FALSE;
  return EFI_SUCCESS;
}

/**

  This function uses PCI Vendor ID, Device ID, and RomType to check if the Option ROM type is supported.

  @param[in]      DevicePath             - Device Path of PCI device.
  @param[in]      VendorId               - PCI Vendor ID.
  @param[in]      DeviceId               - PCI Device ID.
  @param[in]      RomType                - The Rom Type of the image.
  @param[in,out]  RomSupported           - TRUE : The ROM type is supported.
                                         - FALSE: The ROM type is unsupported.

  @retval EFI_SUCCESS                    - Successfully checking if the ROM type is supported.

**/
EFI_STATUS
EFIAPI
CheckOpRomSupportPolicy (
  IN CONST  EFI_DEVICE_PATH_PROTOCOL           *DevicePath,
  IN        UINT16                             VendorId,
  IN        UINT16                             DeviceId,
  IN        UINT8                              RomType,
  IN OUT    BOOLEAN                            *RomSupported
 )
{
  EFI_STATUS                                Status;
  SECURE_OPROM_CONTROL_CONFIGURATION        *Config;
  UINTN                                     VarSize;
  UINT8                                     SupportedPolicy;
  BOOLEAN                                   IsSupportedPciSlot;
  BOOLEAN                                   IsVgaPciDevice;
  UINT8                                     OpRomHash[OPROM_CONTROL_HASH_SIZE] = {0};
  EFI_DEVICE_PATH_PROTOCOL                  *DevPath;
  EFI_HANDLE                                Handle;
  EFI_PCI_IO_PROTOCOL                       *PciIo;

  if (VendorId == 0xFFFF || DeviceId == 0xFFFF || RomType == 0) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get HASH value of the option ROM area.
  //
  if (FeaturePcdGet (PcdH2OPciOptionRomSecurityControlBySlot)) {
    DevPath = DuplicateDevicePath (DevicePath);
    PciIo = NULL;
    Status = gBS->LocateDevicePath (&gEfiPciIoProtocolGuid, &DevPath, &Handle);
    if (!EFI_ERROR (Status)) {
      Status = gBS->HandleProtocol (Handle, &gEfiPciIoProtocolGuid, (VOID **)&PciIo);
      if (!EFI_ERROR (Status)) {
        Status = GetHashValue (PciIo->RomImage, PciIo->RomSize, OpRomHash);
      }
    }
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }
  }


  //
  // Try to get option ROM policy from OpRom#### variable. If we can get OP ROM policy
  // for this specific vendor ID and device ID PCI device, return successful directly.
  //
  SupportedPolicy = 0;
  Status = GetOpRomPolicyForDevice (DevicePath, VendorId, DeviceId, OpRomHash, &SupportedPolicy);
  if (!EFI_ERROR (Status)) {
    //
    // Get the supported policy,
    // and then check the RomType contained in ROM and the supported policy type from OpRom#### variable.
    //
    *RomSupported = IsSupportedRom (RomType, SupportedPolicy);
    AddRomImageToList (DevicePath, VendorId, DeviceId, RomType, SupportedPolicy, *RomSupported, OpRomHash);
    return Status;
  }

  //
  // Cannot find option ROM policy from OpRom#### variable
  //
  if (FeaturePcdGet (PcdH2OPciOptionRomSecurityControlBySlot)) {
    IsVgaPciDevice = IsVgaDeviceFromDevicePath (DevicePath);
    IsSupportedPciSlot = IsSupportedPciSlotDevicePath (DevicePath);
    if (IsSupportedPciSlot == TRUE) {
      if (IsVgaPciDevice == TRUE) {
        //
        // Force the VGA device option Rom be supported for the initialization.
        // Otherwise, the platform would not show the display
        // because chipset code would disable the IGD when VGA PCI device is installed.
        //
        SupportedPolicy = SUPPORT_ALL_OPTION_ROM;
        *RomSupported = TRUE;
      } else {
        SupportedPolicy = UNSUPPORT_ALL_OPTION_ROM;
        *RomSupported = FALSE;
      }
      Status = EFI_SUCCESS;
      AddRomImageToList (DevicePath, VendorId, DeviceId, RomType, SupportedPolicy, *RomSupported, OpRomHash);
      return Status;
    }
  }

  //
  // For Non-slot PCI device option ROM,
  // If can not find option ROM policy from OpRom#### variable, try to use default policy.
  //
  Status = CommonGetVariableDataAndSize(
             L"SecOpRomConfig",
             &gH2OSecureOptionRomControlFormsetGuid,
             &VarSize,
             (VOID **) &Config
             );
  if (!EFI_ERROR (Status)) {
    if (Config->OptionRomPolicy != OPTION_ROM_POLICY_AUTO) {
      SupportedPolicy = Config->OptionRomPolicy - 1;
    } else {
      //
      // OptionRomPolicy is OPTION_ROM_POLICY_AUTO: Option ROM Control is NOT enabled.
      //
      SupportedPolicy = SUPPORT_ALL_OPTION_ROM;
      Status = EFI_UNSUPPORTED;
    }

    *RomSupported = IsSupportedRom (RomType, SupportedPolicy);
    AddRomImageToList (DevicePath, VendorId, DeviceId, RomType, SupportedPolicy, *RomSupported, OpRomHash);
  }

  return Status;
}

/**

  According to Contained ROM type and supported ROM type to check this ROM is
  supported by current policy.

  @param[in] ContaindRomType   - All of the ROM type which contained in specific ROM image.
  @param[in] SupportedRomType  - Supported ROM type for specific PCI device (Distingusished by
                             Vendor ID and Device ID).

  @retval TRUE              - Platform can support this ROM image.
  @retval FALSE             - Platform cannot support this ROM image.

**/
BOOLEAN
EFIAPI
IsSupportedRom (
  IN  UINT8     ContaindRomType,
  IN  UINT8     SupportedRomType
  )
{
  EFI_STATUS                    Status;
  BOOLEAN                       IsSupported;
  KERNEL_CONFIGURATION          KernelConfig;


  Status = GetKernelConfiguration (&KernelConfig);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  IsSupported = FALSE;
  switch (SupportedRomType) {

  case SUPPORT_ALL_OPTION_ROM:
    if (((KernelConfig.BootType == DUAL_BOOT_TYPE  ) && ((ContaindRomType & (HAVE_LEGACY_ROM | HAVE_UNSIGNED_EFI_ROM | HAVE_SIGNED_EFI_ROM)) != 0)) ||
        ((KernelConfig.BootType == LEGACY_BOOT_TYPE) && ((ContaindRomType & HAVE_LEGACY_ROM) != 0)) ||
        ((KernelConfig.BootType == EFI_BOOT_TYPE   ) && ((ContaindRomType & (HAVE_UNSIGNED_EFI_ROM | HAVE_SIGNED_EFI_ROM)) != 0))) {
      IsSupported = TRUE;
    }
    break;

  case SUPPORT_LEGACY_OPTION_ROM:
    if ((KernelConfig.BootType == DUAL_BOOT_TYPE) ||
        (KernelConfig.BootType == LEGACY_BOOT_TYPE)) {
      if ((ContaindRomType & HAVE_LEGACY_ROM) != 0) {
        IsSupported = TRUE;
      }
    }
    break;

  case SUPPORT_UNSIGNED_EFI_OPTION_ROM:
    if ((KernelConfig.BootType == DUAL_BOOT_TYPE) ||
        (KernelConfig.BootType == EFI_BOOT_TYPE)) {
      if ((ContaindRomType & HAVE_UNSIGNED_EFI_ROM) != 0) {
        IsSupported = TRUE;
      }
    }
    break;

  case SUPPORT_SIGNED_EFI_OPTION_ROM:
    if ((KernelConfig.BootType == DUAL_BOOT_TYPE) ||
        (KernelConfig.BootType == EFI_BOOT_TYPE)) {
      if ((ContaindRomType & HAVE_SIGNED_EFI_ROM) != 0) {
        IsSupported = TRUE;
      }
    }
    break;
  }
  return IsSupported;
}

/**
 Get the device path node for given device path type and device path sub-type

 @param [in] DevicePath         The device path to be processed
 @param [in] DevicePathType     The device path type
 @param [in] DevicePathSubType  The device path sub type

 @return  The retrieved device path node with the given DevicePathType and DevicePathSubType

**/
EFI_DEVICE_PATH_PROTOCOL *
EFIAPI
LocateDevicePathNode(
  IN OUT CONST  EFI_DEVICE_PATH_PROTOCOL             *DevicePath,
  IN            UINT8                                DevPathType,
  IN            UINT8                                DevPathSubType
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *DevPath;

  if (DevicePath == NULL)
    return NULL;

  DevPath = DuplicateDevicePath (DevicePath);

  while (!IsDevicePathEnd(DevPath)) {
    if (DevPath->Type == DevPathType && DevPath->SubType == DevPathSubType) {
      return DevPath;
    }
    DevPath = NextDevicePathNode(DevPath);
  }

  return NULL;

}

/**

  Get the Vendor ID, Device ID, and ROM type of this ROM from Device Path.

  @param[in]     DevicePath             - The file path of the Option ROM Image.
  @param[in,out] VendorId               - PCI Vendor ID.
  @param[in,out] DeviceId               - PCI Device ID.
  @param[in,out] RomType                - The Rom Type of the image.

  @retval EFI_SUCCESS                   Get the information of the option ROM.

**/
EFI_STATUS
EFIAPI
GetInfoFromDevicePath (
  IN CONST  EFI_DEVICE_PATH_PROTOCOL           *DevicePath,
  IN OUT    UINT16                             *VendorId,
  IN OUT    UINT16                             *DeviceId,
  IN OUT    UINT8                              *RomType
  )
{
  MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH                 *PciOptionRomImageDevicePath;
  EFI_DEVICE_PATH_PROTOCOL                                *DevPath;
  EFI_DEVICE_PATH_PROTOCOL                                *DevPathPtr;
  UINT64                                                  StartingOffset;
  UINT64                                                  EndingOffset;
  EFI_STATUS                                              Status = EFI_NOT_FOUND;
  EFI_HANDLE                                              Handle;
  EFI_PCI_IO_PROTOCOL                                     *PciIo;
  UINT32                                                  Buffer;

  DevPath = DuplicateDevicePath (DevicePath);
  DevPathPtr = DevPath;

  //
  // 1. Get the Option Rom DEVICE PATH.
  //
  PciOptionRomImageDevicePath = (MEDIA_RELATIVE_OFFSET_RANGE_DEVICE_PATH *) LocateDevicePathNode (
                                                                              DevicePath,
                                                                              MEDIA_DEVICE_PATH,
                                                                              MEDIA_RELATIVE_OFFSET_RANGE_DP
                                                                              );
  if (PciOptionRomImageDevicePath == NULL) {
    return Status;
  }

  StartingOffset = PciOptionRomImageDevicePath->StartingOffset;
  EndingOffset = PciOptionRomImageDevicePath->EndingOffset;

  //
  // 2. Locate device path for PCI IO protocol.
  //
  PciIo = NULL;
  Status = gBS->LocateDevicePath (&gEfiPciIoProtocolGuid, &DevPath, &Handle);
  if (!EFI_ERROR (Status)) {
    Status = gBS->HandleProtocol (Handle, &gEfiPciIoProtocolGuid, (VOID **)&PciIo);
  }
  FreePool (DevPathPtr);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 0, 1, &Buffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // 3. Check if it is UEFI Option ROM
  //
  Status = GetUefiRomType (PciIo, (EFI_DEVICE_PATH_PROTOCOL *)PciOptionRomImageDevicePath, RomType);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  *VendorId = (UINT16)(Buffer & 0xFFFF);
  *DeviceId = (UINT16)((Buffer >> 16) & 0xFFFF);
  DEBUG ((DEBUG_INFO, "GetInfoFromDevicePath()   VenID: 0x%X DevID: 0x%X RomType: %x\n", *VendorId, *DeviceId, *RomType));

  return EFI_SUCCESS;

}

/**

  Get the ROM type of this ROM: Signed or Unsigned ROM type.

  @param[in]     PciIoDevice            - Pointer PCI_IO_DEVICE structure.
  @param[in]     EfiOpRomImageFilePath  - The file path of the EFI Image.
  @param[in,out] RomType                - The Rom Type of the image.s

  @retval EFI_SUCCESS                Get the ROM type.

**/
EFI_STATUS
EFIAPI
GetUefiRomType (
  IN     EFI_PCI_IO_PROTOCOL      *PciIo,
  IN     EFI_DEVICE_PATH_PROTOCOL *EfiOpRomImageFilePath,
  IN OUT UINT8                    *RomType
  )
{
  EFI_STATUS                               Status;
  UINT8                                    *Buffer;
  UINTN                                    BufferSize;
  EFI_IMAGE_DOS_HEADER                     *DosHdr;
  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION      NtHeader;
  EFI_IMAGE_DATA_DIRECTORY                 *SecDataDir;
  UINTN                                    PeCoffHeaderOffset;
  UINT16                                   Magic;
  BOOLEAN                                  IsSignedEfi;

  Buffer = NULL;
  Status = LocalLoadFile (
             PciIo,
             EfiOpRomImageFilePath,
             &BufferSize,
             (VOID *) Buffer
             );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Buffer = AllocateZeroPool (BufferSize);
    ASSERT (Buffer != NULL);
    Status = LocalLoadFile (
               PciIo,
               EfiOpRomImageFilePath,
               &BufferSize,
               (VOID *) Buffer
               );
  }
  IsSignedEfi = FALSE;
  if (!EFI_ERROR (Status) && Buffer != NULL) {
    DosHdr = (EFI_IMAGE_DOS_HEADER *) Buffer;
    if (DosHdr->e_magic == EFI_IMAGE_DOS_SIGNATURE) {
      //
      // DOS image header is present,
      // so read the PE header after the DOS image header.
      //
      PeCoffHeaderOffset = DosHdr->e_lfanew;
    } else {
      PeCoffHeaderOffset = 0;
    }
    //
    // Check PE/COFF image.
    //
    NtHeader.Pe32 = (EFI_IMAGE_NT_HEADERS32 *) (Buffer + PeCoffHeaderOffset);
    if (NtHeader.Pe32->Signature == EFI_IMAGE_NT_SIGNATURE) {
      Magic = NtHeader.Pe32->OptionalHeader.Magic;
      if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
        //
        // Use PE32 offset.
        //
        SecDataDir = (EFI_IMAGE_DATA_DIRECTORY *) &NtHeader.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY];
      } else {
        //
        // Use PE32+ offset.
        //
        SecDataDir = (EFI_IMAGE_DATA_DIRECTORY *) &NtHeader.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY];
      }

      if (SecDataDir->Size != 0) {
        //
        // This image is signed.
        //
        IsSignedEfi = TRUE;
      }
    }
  }

  if (IsSignedEfi == TRUE) {
    *RomType |= HAVE_SIGNED_EFI_ROM;
  } else {
    *RomType |= HAVE_UNSIGNED_EFI_ROM;
  }
  return EFI_SUCCESS;
}

/**
 Free the option ROM list.

 @retval EFI_SUCCESS
*/
EFI_STATUS
EFIAPI
SecurOptionRomControlReadyToBootCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{

  DEBUG ((DEBUG_INFO, "ReadyToBoot event to free the Option ROM list.\n"));
  FreeOptionRomList ();

  //
  // Check if the pre-lock sensitive variable number is enough or not.
  //
  if (mCurrentOpRomNum > mLockOpRomVarNum) {
    DEBUG ((DEBUG_ERROR, "The pre-lock sensitive variable number is not enough.\n"));
    DEBUG ((DEBUG_ERROR, "Reset system to lock enough number of sensitive variables.\n"));
    gRT->ResetSystem (EfiResetWarm, EFI_SUCCESS, 0, NULL);
  }

  gBS->CloseEvent (Event);
  return EFI_SUCCESS;
}

/**
  Check the input condition of the option ROM to determine if the option ROM is available.

  @param  DevicePath       A pointer to the device path of the file that is
                           being dispatched.
  @param  VendorId         Vendor ID for the PCI device.
  @param  DeviceId         Device ID for the PCI device.
  @param  IsLegacyOpRom    To determine the Option ROM type: Legacy or UEFI.

  @retval SEC_FORCE_SUCCESS       The Option ROM image is allowed to run.
  @retval SEC_FORCE_DENIED        The Option ROM image is not allowed to run.
  @retval SEC_DO_NOTHING          Continue to run as default behavior.

**/
H2O_SECURITY_ARCH_STATUS
EFIAPI
SecurOptionRomControlAuthenticate (
  IN CONST  EFI_DEVICE_PATH_PROTOCOL                     *DevicePath,
  IN        UINT16                                       VendorId,
  IN        UINT16                                       DeviceId,
  IN        BOOLEAN                                      IsLegacyOpRom
  )
{
  EFI_STATUS                                              Status;
  UINT8                                                   RomType;
  UINT16                                                  VenId;
  UINT16                                                  DevId;
  BOOLEAN                                                 RomSupported;
  SECURE_OPROM_CONTROL_CONFIGURATION                      *Config;
  UINTN                                                   VarSize;

  Status = CommonGetVariableDataAndSize(
             L"SecOpRomConfig",
             &gH2OSecureOptionRomControlFormsetGuid,
             &VarSize,
             (VOID **) &Config
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "SecurOptionRomControlAuthenticate() RomSupported: SEC_DO_NOTHING\n"));
    return SEC_DO_NOTHING;
  }

  //
  // 1. Get related information.
  //
  RomType = 0;
  if (IsLegacyOpRom) {
    //
    // Legacy option ROM.
    //
    VenId = VendorId;
    DevId = DeviceId;
    RomType = HAVE_LEGACY_ROM;
  } else {
    //
    // UEFI option ROM
    //
    Status = GetInfoFromDevicePath (DevicePath, &VenId, &DevId, &RomType);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "SecurOptionRomControlAuthenticate() RomSupported: SEC_DO_NOTHING\n"));
      return SEC_DO_NOTHING;
    }
  }

  //
  // 2.1 Check if the ROM type is supported.
  //
  Status = CheckOpRomSupportPolicy (DevicePath, VenId, DevId, RomType, &RomSupported);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "SecurOptionRomControlAuthenticate() RomSupported: SEC_DO_NOTHING\n"));
    return SEC_DO_NOTHING;
  }

  DEBUG ((DEBUG_INFO, "SecurOptionRomControlAuthenticate() RomSupported: %s.\n", RomSupported ? L"Force Access" : L"Force Denied"));
  return RomSupported ? SEC_FORCE_SUCCESS : SEC_FORCE_DENIED;
}


/**
  Check H2O Security Arch protocol for option rom dispathing policy.

  @param  DevicePath              A pointer to the device path of the file that is
                                  being dispatched.

  @retval EFI_SUCCESS             The Option ROM image is available.
  @retval EFI_ACCESS_DENIED       The Option ROM image is unavailable.
  @retval EFI_UNSUPPORTED         Can't locate the H2OSecurityArch protocol.

**/
EFI_STATUS
EFIAPI
CheckH2OSecurityArchPolicy (
  IN CONST EFI_DEVICE_PATH_PROTOCOL    *DevicePath
  )
{
  EFI_STATUS                           Status;
  H2O_SECURITY_ARCH_STATUS             SecStatus = SEC_DO_NOTHING;
  H2O_SECURITY_ARCH_PROTOCOL           *H2OSecArchProtocol;
  EFI_HANDLE                           *HandleBuffer;
  UINTN                                NumberOfHandles;
  UINTN                                Index;

  //
  // Locate H2OSecurityArch protocol.
  //
  HandleBuffer = NULL;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gH2OSecurityArchProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gH2OSecurityArchProtocolGuid,
                    (VOID **)&H2OSecArchProtocol
                    );
    if (!EFI_ERROR (Status)) {
      SecStatus = H2OSecArchProtocol->FileAuthentication(DevicePath, 0, 0, FALSE);
      if (SecStatus == SEC_DO_NOTHING) {
        continue;
      } else {
        //
        // The H2OSecurityArch protocol returned the specific behavior.
        // - SEC_FORCE_SUCCESS
        // - SEC_FORCE_DENIED
        //
        break;
      }
    }
  }

  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
    HandleBuffer = NULL;
  }

  if (SecStatus == SEC_FORCE_DENIED) {
    return EFI_ACCESS_DENIED;
  } else if (SecStatus == SEC_FORCE_SUCCESS) {
    return EFI_SUCCESS;
  } else {
    // SEC_DO_NOTHING
    return EFI_UNSUPPORTED;
  }

}

/**
  This is to measure and/or verify a UEFI image.
  First, it will check H2O Security Arch protocol for Option ROM dispatch policy,
  if the return status doesn't match EFI_SUCCESS or EFI_ACCESS_DENIED,
  then process it with original EFI Security2 protocol behavior.

  This service abstracts the invocation of Trusted Computing Group (TCG) measured boot, UEFI
  Secure boot, and UEFI User Identity infrastructure. For the former two, the DXE Foundation
  invokes the FileAuthentication() with a DevicePath and corresponding image in
  FileBuffer memory. The TCG measurement code will record the FileBuffer contents into the
  appropriate PCR. The image verification logic will confirm the integrity and provenance of the
  image in FileBuffer of length FileSize . The origin of the image will be DevicePath in
  these cases.
  If the FileBuffer is NULL, the interface will determine if the DevicePath can be connected
  in order to support the User Identification policy.

  @param  This             The EFI_SECURITY2_ARCH_PROTOCOL instance.
  @param  File             A pointer to the device path of the file that is
                           being dispatched. This will optionally be used for logging.
  @param  FileBuffer       A pointer to the buffer with the UEFI file image.
  @param  FileSize         The size of the file.
  @param  BootPolicy       A boot policy that was used to call LoadImage() UEFI service. If
                           FileAuthentication() is invoked not from the LoadImage(),
                           BootPolicy must be set to FALSE.

  @retval EFI_SUCCESS             The file specified by DevicePath and non-NULL
                                  FileBuffer did authenticate, and the platform policy dictates
                                  that the DXE Foundation may use the file.
  @retval EFI_SUCCESS             The device path specified by NULL device path DevicePath
                                  and non-NULL FileBuffer did authenticate, and the platform
                                  policy dictates that the DXE Foundation may execute the image in
                                  FileBuffer.
  @retval EFI_SUCCESS             FileBuffer is NULL and current user has permission to start
                                  UEFI device drivers on the device path specified by DevicePath.
  @retval EFI_SECURITY_VIOLATION  The file specified by DevicePath and FileBuffer did not
                                  authenticate, and the platform policy dictates that the file should be
                                  placed in the untrusted state. The image has been added to the file
                                  execution table.
  @retval EFI_ACCESS_DENIED       The file specified by File and FileBuffer did not
                                  authenticate, and the platform policy dictates that the DXE
                                  Foundation many not use File.
  @retval EFI_SECURITY_VIOLATION  FileBuffer is NULL and the user has no
                                  permission to start UEFI device drivers on the device path specified
                                  by DevicePath.
  @retval EFI_SECURITY_VIOLATION  FileBuffer is not NULL and the user has no permission to load
                                  drivers from the device path specified by DevicePath. The
                                  image has been added into the list of the deferred images.
**/
EFI_STATUS
EFIAPI
HookSecurity2StubAuthenticate (
  IN CONST  EFI_SECURITY2_ARCH_PROTOCOL       *This,
  IN CONST  EFI_DEVICE_PATH_PROTOCOL          *File,
  IN        VOID                              *FileBuffer,
  IN        UINTN                             FileSize,
  IN        BOOLEAN                           BootPolicy
  )
{
  EFI_STATUS                          Status;

  Status = CheckH2OSecurityArchPolicy (File);
  if (Status == EFI_SUCCESS) {
    //
    // The Option ROM image is available, so force using it.
    //
    return EFI_SUCCESS;
  } else if (Status == EFI_ACCESS_DENIED) {
    //
    // The Option ROM image is unavailable.
    //
    return EFI_ACCESS_DENIED;
  }

  //
  // Use original EFI Security2 protocol.
  //
  return mOrgFileAuthentication (
           This,
           File,
           FileBuffer,
           FileSize,
           BootPolicy
           );
}

/**

 Nofify function when EFI Security2 Arch protocol is installed.

 @retval VOID.
*/
VOID
EFIAPI
Security2ArchProtocolNotifyFunction (
  IN EFI_EVENT     Event,
  IN VOID          *Context
  )
{
  EFI_STATUS                            Status;
  EFI_SECURITY2_ARCH_PROTOCOL           *Security2;

  Status = gBS->LocateProtocol (&gEfiSecurity2ArchProtocolGuid, NULL, (VOID **) &Security2);
  if (!EFI_ERROR (Status)) {
    //
    // Hook the original FileAuthentiacation function of EFI Security2 protocol.
    //
    mOrgFileAuthentication = Security2->FileAuthentication;
    Security2->FileAuthentication = HookSecurity2StubAuthenticate;

    gBS->CloseEvent (Event);
  }

  return;
}

/**
  Hook EFI Security2 protocol.

  @param   VOID

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
HookSecurity2Protocol (
  VOID
  )
{
  EFI_STATUS                            Status;
  EFI_SECURITY2_ARCH_PROTOCOL           *Security2;
  VOID                                  *Registration;

  Status = gBS->LocateProtocol (&gEfiSecurity2ArchProtocolGuid, NULL, (VOID **) &Security2);
  if (EFI_ERROR (Status)) {
    //
    // Can't locate EFI Security 2 protocol, create notify event for it.
    //
    EfiCreateProtocolNotifyEvent (
      &gEfiSecurity2ArchProtocolGuid,
      TPL_CALLBACK,
      Security2ArchProtocolNotifyFunction,
      NULL,
      &Registration
      );
    return EFI_SUCCESS;
  }

  //
  // Hook the original FileAuthentiacation function of EFI Security2 protocol.
  //
  mOrgFileAuthentication = Security2->FileAuthentication;
  Security2->FileAuthentication = HookSecurity2StubAuthenticate;

  return EFI_SUCCESS;
}

/**
  Installs Secure Option ROM Control Protocol.

  @param  ImageHandle  The image handle of this driver.
  @param  SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS   Install the sample Security Architectural Protocol successfully.

**/
EFI_STATUS
EFIAPI
SecureOptionRomControlInitialize (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            Handle = NULL;
  EFI_EVENT                             ReadyToBootEvent;

  //
  // Initialieze the Setup related functions.
  //
  if (PcdGetBool (PcdH2OPciOptionRomSecurityControlSetupSupported) == TRUE){
    SetupInit (ImageHandle, SystemTable);
  }

  //
  // Initialize the option ROM list.
  //
  InitializeOptionRomList();

  //
  // Install the H2O Security Architectural Protocol onto a new handle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gH2OSecurityArchProtocolGuid,
                  &mH2OSecurityArch,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    FreeOptionRomList ();
    return Status;
  }

  //
  // Create a ReadyToBoot event to free the Option ROM list.
  //
  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             SecurOptionRomControlReadyToBootCallback,
             NULL,
             &ReadyToBootEvent
             );
  DEBUG ((DEBUG_INFO, "Create a ReadyToBoot event to free the Option ROM list: %r\n", Status));

  //
  // Hook the original EFI Security2 Arch Protocol.
  //
  Status = HookSecurity2Protocol ();
  DEBUG ((DEBUG_INFO, "HookSecurity2Protocol(): %r\n", Status));

  return EFI_SUCCESS;
}


