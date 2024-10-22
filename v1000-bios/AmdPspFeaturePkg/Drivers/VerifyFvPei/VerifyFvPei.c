/** @file

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

**/

#include <PiPei.h>
#include <Uefi.h>

#include <Ppi/FirmwareVolume.h>
#include <Ppi/FirmwareVolumeInfo.h>
#include <Ppi/FirmwareVolumeInfo2.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseCryptLib.h>
#include <Library/PostCodeLib.h>
#include <Library/PerformanceLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Guid/FirmwareFileSystem3.h>

#define GET_OCCUPIED_SIZE(ActualSize, Alignment) \
  ((ActualSize) + (((Alignment) - ((ActualSize) & ((Alignment) - 1))) & ((Alignment) - 1)))

EFI_STATUS
EFIAPI
FirmwareVolmeInfoPpiNotifyCallback (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  );

EFI_PEI_NOTIFY_DESCRIPTOR mNotifyOnFvInfoList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gEfiPeiFirmwareVolumeInfoPpiGuid,
    FirmwareVolmeInfoPpiNotifyCallback 
  },
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiFirmwareVolumeInfo2PpiGuid,
    FirmwareVolmeInfoPpiNotifyCallback 
  }
};

EFI_FIRMWARE_VOLUME_HEADER  *mVerifiedFvAddress[FixedPcdGet32 (PcdPeiCoreMaxFvSupported)];
UINT32 mVerifiedFvIndex = 0;

EFI_PEI_FIRMWARE_VOLUME_INFO2_PPI mDiscoveredFvInfo[FixedPcdGet32 (PcdPeiCoreMaxFvSupported)];
UINT32 mDiscoveredFvIndex = 0;

EFI_STATUS
EFIAPI
FirmwareVolmeInfoPpiNotifyCallback (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  )
{
  EFI_PEI_FIRMWARE_VOLUME_INFO2_PPI     FvInfo2Ppi;
  EFI_PEI_FIRMWARE_VOLUME_PPI           *FvPpi;
  EFI_STATUS                            Status;
  EFI_PEI_FV_HANDLE                     FvHandle;
  EFI_PEI_FILE_HANDLE                   FileHandle;
  EFI_GUID                              *ParentFileName;
  UINTN                                 FvIndex;
  BOOLEAN                               Match;

  ASSERT (mDiscoveredFvIndex < FixedPcdGet32 (PcdPeiCoreMaxFvSupported));

  if (CompareGuid (NotifyDescriptor->Guid, &gEfiPeiFirmwareVolumeInfo2PpiGuid)) {
    //
    // It is FvInfo2PPI.
    //
    CopyMem (&FvInfo2Ppi, Ppi, sizeof (EFI_PEI_FIRMWARE_VOLUME_INFO2_PPI));
  } else {
    //
    // It is FvInfoPPI.
    //
    CopyMem (&FvInfo2Ppi, Ppi, sizeof (EFI_PEI_FIRMWARE_VOLUME_INFO_PPI));
    FvInfo2Ppi.AuthenticationStatus = 0;
  }

  //
  // Locate the corresponding FV_PPI according to founded FV's format guid.
  //
  Status = PeiServicesLocatePpi (
             &FvInfo2Ppi.FvFormat,
             0,
             NULL,
             (VOID**)&FvPpi
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Fail to process FV %p because no corresponding EFI_FIRMWARE_VOLUME_PPI is found!\n", FvInfo2Ppi.FvInfo));
    return EFI_SUCCESS;
  }

  //
  // This firmware volume contains firmware file system,
  // so need to check if it is unknow Fv.
  //
  FvHandle       = FvInfo2Ppi.FvInfo;
  ParentFileName = FvInfo2Ppi.ParentFileName;
  do {
    FileHandle = NULL;
    for (FvIndex = 0; FvIndex < mDiscoveredFvIndex; FvIndex ++) {
      Status = PeiServicesFfsFindFileByName (ParentFileName, mDiscoveredFvInfo[FvIndex].FvInfo, (VOID **)&FileHandle);
      if (!EFI_ERROR (Status)) {
        FvHandle       = mDiscoveredFvInfo[FvIndex].FvInfo;
        ParentFileName = mDiscoveredFvInfo[FvIndex].ParentFileName;
      }
    }
  } while (FileHandle != NULL);

  for (FvIndex = 0, Match = FALSE; FvIndex < mVerifiedFvIndex; FvIndex ++) {
    if (mVerifiedFvAddress[FvIndex] == FvHandle) {
      Match = TRUE;
      break;
    }
  }

  if (!Match) {
    DEBUG ((EFI_D_ERROR, "An illegal FV %p that may contain firmware file system!\n", FvHandle));
    CpuDeadLoop ();
  }

  //
  // Process new found FV.
  // Add new FV into the Discovered FV list.
  //
  for (FvIndex = 0; FvIndex < mDiscoveredFvIndex; FvIndex ++) {
    if (mDiscoveredFvInfo[FvIndex].FvInfo == FvInfo2Ppi.FvInfo) {
      return EFI_SUCCESS;
    }
  }

  //
  // Update Discovered FV list.
  //
  if (mDiscoveredFvIndex < FixedPcdGet32 (PcdPeiCoreMaxFvSupported)) {
    CopyMem (&mDiscoveredFvInfo[mDiscoveredFvIndex], &FvInfo2Ppi, sizeof (EFI_PEI_FIRMWARE_VOLUME_INFO2_PPI));
    mDiscoveredFvIndex++;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
RegisterTrustedFv (
  IN EFI_FIRMWARE_VOLUME_HEADER  *CurrentFvAddress
  )
{

  ASSERT (mVerifiedFvIndex < FixedPcdGet32 (PcdPeiCoreMaxFvSupported));

  mVerifiedFvAddress[mVerifiedFvIndex] = CurrentFvAddress;

  if (mVerifiedFvIndex < FixedPcdGet32 (PcdPeiCoreMaxFvSupported)) {
    mVerifiedFvIndex++;
  }

  return EFI_SUCCESS;
}

/**
 SHA256 HASH calculation

 @param [in]   Message          The message data to be calculated
 @param [in]   MessageSize      The size in byte of the message data
 @param [out]  Digest           The caclulated HASH digest

 @retval EFI_SUCCESS            The HASH value is calculated
 @retval EFI_SECURITY_VIOLATION  Failed to calculate the HASH

**/
EFI_STATUS
CalculateSha256Hash (
  IN UINT8                      *Message,
  IN UINTN                      MessageSize,
  OUT UINT8                     *Digest
  )
{
  VOID              *HashCtx;
  UINTN             CtxSize;
  EFI_STATUS        Status;

  SetMem (Digest, SHA256_DIGEST_SIZE, 0);
  CtxSize = Sha256GetContextSize ();
  HashCtx = NULL;
  HashCtx = AllocatePool (CtxSize);
  if (HashCtx == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (!Sha256Init (HashCtx)) {
    Status = EFI_SECURITY_VIOLATION;
    goto Done;
  }
  if(!Sha256Update (HashCtx, Message, MessageSize)) {
    Status = EFI_SECURITY_VIOLATION;
    goto Done;
  }
  if(!Sha256Final (HashCtx, Digest)) {
    Status = EFI_SECURITY_VIOLATION;
  } else {
    Status = EFI_SUCCESS;
  }

Done:
  FreePool (HashCtx);
  return Status;
}

/**
  Verify a Firmware volume.

  @param CurrentFvAddress   Pointer to the current Firmware Volume under consideration

  @retval EFI_SUCCESS       Firmware Volume is legal

**/
EFI_STATUS
VerifyFv (
  IN EFI_FIRMWARE_VOLUME_HEADER  *CurrentFvAddress,
  IN EFI_PHYSICAL_ADDRESS        CurrentFvHashAddress
  )
{
  EFI_STATUS                            Status;
  EFI_FIRMWARE_VOLUME_HEADER            *NewFvBuffer;
  EFI_FIRMWARE_VOLUME_HEADER            *FwVolHeader;
  EFI_PEI_FILE_HANDLE                   FileHandle;
  EFI_FFS_FILE_HEADER                   *FfsFileHeader;
  EFI_PEI_FIRMWARE_VOLUME_PPI           *FvPpi;
  UINT32                                FileLength;
  UINT32                                FileOccupiedSize;
  UINTN                                 FvOccupiedSize;
  BOOLEAN                               IsFfs3Fv;
  UINT8                                 HashValue[SHA256_DIGEST_SIZE];
  EFI_HOB_FIRMWARE_VOLUME               *FvHob;

  FwVolHeader = CurrentFvAddress;

  IsFfs3Fv = CompareGuid (&FwVolHeader->FileSystemGuid, &gEfiFirmwareFileSystem3Guid);

  Status = PeiServicesLocatePpi (
             &FwVolHeader->FileSystemGuid,
             0,
             NULL,
             (VOID**)&FvPpi
             );
  ASSERT_EFI_ERROR (Status);

  //
  // Look for the last FFS file in this FV.
  //
  FileHandle       = NULL;
  FfsFileHeader    = NULL;
  FileOccupiedSize = 0;
  do {
    Status = FvPpi->FindFileByType (
                      FvPpi,
                      EFI_FV_FILETYPE_ALL,
                      CurrentFvAddress,
                      &FileHandle
                      );
    if (!EFI_ERROR (Status)) {
      FfsFileHeader = FileHandle;
    }
  } while (FileHandle != NULL);

  //
  // Calculate all files occupied size.
  //
  if (IS_FFS_FILE2 (FfsFileHeader)) {
    if (!IsFfs3Fv) {
      DEBUG ((EFI_D_ERROR, "It is a FFS3 formatted file: %g in a non-FFS3 formatted FV.\n", &(FfsFileHeader)->Name));
    }
    FileLength = FFS_FILE2_SIZE (FfsFileHeader);
    ASSERT (FileLength > 0x00FFFFFF);
  } else {
    FileLength = FFS_FILE_SIZE (FfsFileHeader);
  }
  //
  // FileLength is adjusted to FileOccupiedSize as it is 8 byte aligned.
  //
  FileOccupiedSize = GET_OCCUPIED_SIZE (FileLength, 8);
  FvOccupiedSize = (UINT32) ((UINT8 *)FfsFileHeader - (UINT8 *)FwVolHeader) + FileOccupiedSize;
  ASSERT (FvOccupiedSize <= (UINTN)FwVolHeader->FvLength);

  //
  // Policy check if support relocate mode.
  //
  NewFvBuffer = AllocatePages (EFI_SIZE_TO_PAGES ((UINTN)FwVolHeader->FvLength));
  if (NewFvBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  if (NewFvBuffer != FwVolHeader) {
    SetMem (NewFvBuffer, (UINTN)FwVolHeader->FvLength, 0xFF);
    CopyMem (NewFvBuffer, FwVolHeader, FvOccupiedSize);
    //
    // Replace original Fv hob.
    //
    FvHob = (EFI_HOB_FIRMWARE_VOLUME *)GetFirstHob (EFI_HOB_TYPE_FV);
    while (FvHob != NULL) {
      if ((EFI_PEI_FV_HANDLE)(UINTN)FvHob->BaseAddress == FwVolHeader) {
        FvHob->BaseAddress = (EFI_PHYSICAL_ADDRESS)(UINTN)NewFvBuffer;
        FvHob->Length      = NewFvBuffer->FvLength;
        break;
      }
      FvHob = (EFI_HOB_FIRMWARE_VOLUME *)GetNextHob (EFI_HOB_TYPE_FV, (VOID *)((UINTN)FvHob + FvHob->Header.HobLength));
    }
  }

  //
  // Verify HASH of FvMainCompact to continue the chain of trust before image extract and execute.
  // Calculate the HASH of FvMain_Compact.
  //
  Status = CalculateSha256Hash ((UINT8*)NewFvBuffer, (UINTN)FvOccupiedSize, HashValue);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Compare hash data.
  //
  if (CompareMem ((UINT8*)HashValue, (UINT8*)(UINTN)CurrentFvHashAddress, sizeof (HashValue)) != 0) {
    DEBUG ((EFI_D_ERROR, "HASH of Fv %p verify fail.\n", NewFvBuffer));
    return EFI_SECURITY_VIOLATION;
  }

  //
  // Registed Verified FvAddress.
  //
  RegisterTrustedFv (NewFvBuffer);
  
  return EFI_SUCCESS;
}

/**
  Initialize Discovered Fv List.

  @param PrivateData     - Pointer to PEI_CORE_INSTANCE.
  @param SecCoreData     - Pointer to EFI_SEC_PEI_HAND_OFF.
**/
VOID 
InitializeFvList (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                                Status;
  EFI_PEI_FIRMWARE_VOLUME_INFO2_PPI         *FvInfo2Ppi;
  EFI_FIRMWARE_VOLUME_HEADER                *FvHeader;
  UINTN                                     Instance;
  UINTN                                     FvIndex;

  //
  // Update RTM to Discovered FV list.
  //
  mDiscoveredFvIndex = 0;
  Instance           = 0;
  FvHeader           = NULL;
  do {
    Status = PeiServicesFfsFindNextVolume (Instance++, &FvHeader);
    if (!EFI_ERROR (Status)) {
      CopyGuid (&mDiscoveredFvInfo[mDiscoveredFvIndex].FvFormat, &FvHeader->FileSystemGuid);
      mDiscoveredFvInfo[mDiscoveredFvIndex].FvInfo               = FvHeader;
      mDiscoveredFvInfo[mDiscoveredFvIndex].FvInfoSize           = (UINT32)FvHeader->FvLength;
      mDiscoveredFvInfo[mDiscoveredFvIndex].ParentFvName         = NULL;
      mDiscoveredFvInfo[mDiscoveredFvIndex].ParentFileName       = NULL;
      mDiscoveredFvInfo[mDiscoveredFvIndex].AuthenticationStatus = 0;
      if (mDiscoveredFvIndex < FixedPcdGet32 (PcdPeiCoreMaxFvSupported)) {
        mDiscoveredFvIndex++;
      }
    }
  } while (FvHeader != NULL);

  //
  // Update Discovered FV list.
  //
  Instance = 0;
  while (Instance < mDiscoveredFvIndex) {
    FvInfo2Ppi = NULL;
    Status = PeiServicesLocatePpi (
               &gEfiPeiFirmwareVolumeInfo2PpiGuid,
               Instance,
               NULL,
               (VOID**)&FvInfo2Ppi
               );
    if (FvInfo2Ppi != NULL) {
      for (FvIndex = 0; FvIndex < mDiscoveredFvIndex; FvIndex ++) {
        if (mDiscoveredFvInfo[FvIndex].FvInfo == FvInfo2Ppi->FvInfo) {
          CopyMem (&mDiscoveredFvInfo[FvIndex], FvInfo2Ppi, sizeof (EFI_PEI_FIRMWARE_VOLUME_INFO2_PPI));
        }
      }
    }
    Status = PeiServicesLocatePpi (
               &gEfiPeiFirmwareVolumeInfoPpiGuid,
               Instance,
               NULL,
               (VOID**)&FvInfo2Ppi
               );
    if (FvInfo2Ppi != NULL) {
      for (FvIndex = 0; FvIndex < mDiscoveredFvIndex; FvIndex ++) {
        if (mDiscoveredFvInfo[FvIndex].FvInfo == FvInfo2Ppi->FvInfo) {
          CopyMem (&mDiscoveredFvInfo[FvIndex], FvInfo2Ppi, sizeof (EFI_PEI_FIRMWARE_VOLUME_INFO_PPI));
          mDiscoveredFvInfo[FvIndex].AuthenticationStatus = 0;
        }
      }
    }
    Instance ++;
  }

  //
  // BFV should be always Ttrusted Fv.
  //
  RegisterTrustedFv (mDiscoveredFvInfo[0].FvInfo);
}

/**
  Entry point function for the PEIM

  @param FileHandle      Handle of the file being invoked.
  @param PeiServices     Describes the list of possible PEI Services.

  @return EFI_SUCCESS    If we installed our PPI

**/
EFI_STATUS
EFIAPI
VerifyFvPeimEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                                Status;
  EFI_BOOT_MODE                             BootMode;
  EFI_FIRMWARE_VOLUME_HEADER                *FvHeader;
  EFI_PHYSICAL_ADDRESS                      FvHashBase;

  //
  // Get boot mode and ensure that it is shadowed to permanent memory
  //
  BootMode = GetBootModeHob ();

  if (BootMode != BOOT_ON_S3_RESUME) {
    Status = PeiServicesRegisterForShadow (FileHandle);
    if (Status == EFI_SUCCESS) {
      //
      // EFI_SUCESS means it is the first time to call register for shadow.
      //
      return Status;
    }
    //
    // Ensure that it is shadowed to permanent memory.
    //
    ASSERT (Status == EFI_ALREADY_STARTED);
  } else {
    //
    // In S3 path, skip shadow logic. no measurement is required
    //
    return EFI_UNSUPPORTED;
  }

  //
  // Do verify for given FV buffer.
  //
  FvHeader   = (EFI_FIRMWARE_VOLUME_HEADER*) PcdGet32 (PcdFlashFvMainBase);
  FvHashBase = PcdGet32 (PcdFlashFvMainHashBase);
  Status = VerifyFv (FvHeader, FvHashBase);
  if (EFI_ERROR(Status)) {
    //
    // If we get here, Hash data may not match!
    //
    DEBUG ((EFI_D_ERROR, "Fail to verify FV which address is 0x%11p", FvHeader));
    CpuDeadLoop ();
  }

  InitializeFvList (PeiServices);

  //
  // Post a call-back for the FvInfoPPI and FvInfo2PPI services to 
  // verify any exposed Fvs have already been measured.
  //
  Status = PeiServicesNotifyPpi (mNotifyOnFvInfoList);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
