/** @file
  Boot menu relative functions

;******************************************************************************
;* Copyright (c) 2012 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "BootMenu.h"
#include <Library/PrintLib.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/BootOptionPolicy.h>
#include <Protocol/UsbIo.h>
#include <Protocol/BlockIo.h>


/**
 Sync boot device info to boot config.

 @retval EFI_SUCCESS              Sync boot device info successfully.
 @retval Other                    Fail to get browser data.
**/
STATIC
EFI_STATUS
BootDevInfoSyncToBootConfig (
  VOID
  )
{
  UINT16                                    Index;
  UINT16                                    EfiDevCount;
  UINT16                                    LegacyDevCount;
  EFI_STATUS                                Status;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  UINT16                                    LegacyBootDevTypeCount;
  UINT16                                    TypeIndex;
  UINT16                                    DevCount;
  UINT8                                     DevType;
  UINT16                                    Offset;
  KERNEL_CONFIGURATION                      *KernelConfig;
  BBS_TYPE_TABLE                            *BbsTypeTable;

  //
  // Update device switch of boot config from boot device info.
  //
  EfiDevCount    = 0;
  LegacyDevCount = 0;
  for (Index = 0; Index < mBootDevInfoCount; Index++) {
    if (mBootDevInfo[Index].IsEfiBootDev) {
      mBootConfig.EfiBootDevSwitch[EfiDevCount++] = mBootDevInfo[Index].IsActive;
    } else {
      mBootConfig.LegacyAdvBootDevSwitch[LegacyDevCount++] = mBootDevInfo[Index].IsActive;
    }
  }

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  KernelConfig           = (KERNEL_CONFIGURATION *) SuBrowser->SCBuffer;
  LegacyBootDevTypeCount = GetBootTypeOrderCount (KernelConfig);

  for (TypeIndex = 0; TypeIndex < LegacyBootDevTypeCount; TypeIndex++) {
    BbsTypeTable = GetBbsTypeTableByDevType (KernelConfig->BootTypeOrder[TypeIndex]);
    if (BbsTypeTable == NULL || BbsTypeTable->KeyBootDeviceBase < KEY_BOOT_DEVICE_TYPE_BASE) {
      continue;
    }

    Offset   = BbsTypeTable->KeyBootDeviceBase - KEY_BOOT_DEVICE_TYPE_BASE;
    DevType  = BbsTypeTable->DeviceType;
    DevCount = 0;

    for (Index = 0; Index < mBootDevInfoCount; Index++) {
      if (!mBootDevInfo[Index].IsEfiBootDev &&
          mBootDevInfo[Index].DevType == DevType) {
        mBootConfig.LegacyTypeDevSwitch[Offset + DevCount] = mBootDevInfo[Index].IsActive;
        DevCount++;
      }
    }
  }

  return EFI_SUCCESS;
}

/**
 Set active value to the specific boot option number.

 @param[in] BootOptionNum         Boot option number
 @param[in] IsActive              Active value

 @retval EFI_SUCCESS              Set active value successfully.
 @retval EFI_NOT_FOUND            Can not find the corresponding boot device info by boot option number.
**/
EFI_STATUS
BootDevInfoSetActiveValue (
  IN UINT16                                 BootOptionNum,
  IN BOOLEAN                                IsActive
  )
{
  UINT16                                    Index;

  if (mBootDevInfo == NULL) {
    return EFI_NOT_FOUND;
  }

  for (Index = 0; Index < mBootDevInfoCount; Index++) {
    if (mBootDevInfo[Index].BootOptionNum == BootOptionNum) {
      mBootDevInfo[Index].IsActive = IsActive;
      BootDevInfoSyncToBootConfig ();
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**
 Get boot device info by question ID.

 @return The pointer of boot device info or NULL if not found.
**/
BOOT_DEV_INFO *
GetBootDevInfo (
  IN EFI_QUESTION_ID                        QuestionId
  )
{
  UINT16                                    KeyBase;
  UINT16                                    TargetIndex;
  UINT16                                    Index;
  UINT16                                    Count;
  EFI_STATUS                                Status;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  UINT16                                    LegacyBootDevTypeCount;
  UINT16                                    TypeIndex;
  KERNEL_CONFIGURATION                      *KernelConfig;
  BBS_TYPE_TABLE                            *BbsTypeTable;

  KeyBase = KEY_EFI_BOOT_DEVICE_BASE;
  if ((QuestionId >= KeyBase) &&
      (QuestionId <  KeyBase + MAX_BOOT_DEVICES_NUMBER)) {
    TargetIndex  = QuestionId - KeyBase;

    for (Index = 0, Count = 0; Index < mBootDevInfoCount; Index++) {
      if (mBootDevInfo[Index].IsEfiBootDev) {
        if (Count == TargetIndex) {
          return &mBootDevInfo[Index];
        }
        Count++;
      }
    }
  }

  KeyBase = KEY_ADV_LEGACY_BOOT_BASE;
  if ((QuestionId >= KeyBase) &&
      (QuestionId <  KeyBase + MAX_LEGACY_BOOT_DEV_NUM)) {
    TargetIndex  = QuestionId - KeyBase;

    for (Index = 0, Count = 0; Index < mBootDevInfoCount; Index++) {
      if (!mBootDevInfo[Index].IsEfiBootDev) {
        if (Count == TargetIndex) {
          return &mBootDevInfo[Index];
        }
        Count++;
      }
    }
  }

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  KernelConfig           = (KERNEL_CONFIGURATION *) SuBrowser->SCBuffer;
  LegacyBootDevTypeCount = GetBootTypeOrderCount (KernelConfig);

  for (TypeIndex = 0; TypeIndex < LegacyBootDevTypeCount; TypeIndex++) {
    BbsTypeTable = GetBbsTypeTableByDevType (KernelConfig->BootTypeOrder[TypeIndex]);
    if (BbsTypeTable == NULL || BbsTypeTable->KeyBootDeviceBase < KEY_BOOT_DEVICE_TYPE_BASE) {
      continue;
    }

    KeyBase = BbsTypeTable->KeyBootDeviceBase;
    if (!((QuestionId >= KeyBase) &&
          (QuestionId <  KeyBase + BbsTypeTable->DevInfoCount))) {
      continue;
    }

    TargetIndex  = QuestionId - KeyBase;

    for (Index = 0, Count = 0; Index < mBootDevInfoCount; Index++) {
      if (!mBootDevInfo[Index].IsEfiBootDev &&
          mBootDevInfo[Index].DevType == BbsTypeTable->DeviceType) {
        if (Count == TargetIndex) {
          return &mBootDevInfo[Index];
        }
        Count++;
      }
    }
  }

  return NULL;
}

/**
 Sync current boot order to boot config varstore.
**/
VOID
SyncBootOrderToBootConfig (
  VOID
  )
{
  BootDevInfoSortByBootOrder ();
  BootDevInfoSyncToBootConfig ();
}

/**
 Initialize boot configuration value.

 @param[in] SuBrowser                  Pointer point to SETUP_UTILITY_BROWSER_DATA instance

 @retval EFI_SUCCESS                   Initialize boot configuration successfully.
**/
EFI_STATUS
BootConfigInit (
  IN SETUP_UTILITY_BROWSER_DATA         *SuBrowser
  )
{
  UINTN                                 Index;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  KERNEL_CONFIGURATION                  *KernelConfig;

  SUCInfo      = SuBrowser->SUCInfo;
  KernelConfig = (KERNEL_CONFIGURATION *) SuBrowser->SCBuffer;

  BootDevInfoSyncToBootConfig ();
  mBootConfig.HaveLegacyBootDevTypeOrder = (GetBootTypeOrderCount (KernelConfig) != 0) ? 1 : 0;
  mBootConfig.HaveLegacyBootDev          = (SUCInfo->LegacyBootDeviceNum         != 0) ? 1 : 0;
  mBootConfig.HaveEfiBootDev             = (SUCInfo->EfiBootDeviceNum            != 0) ? 1 : 0;
  for (Index = 0; Index < mBbsTypeTableCount; Index++) {
    mBootConfig.NoBootDevs[Index]= (mBbsTypeTable[Index].DevInfoCount != 0) ? 1 : 0;
  }

  return EFI_SUCCESS;
}

/**
  Load the default value of boot configuration varstore.

  @retval EFI_SUCCESS              Load the default value successfully.
  @retval Other                    Fail to initialize boot device switch info.
**/
EFI_STATUS
BootConfigLoadDefault (
  VOID
  )
{
  BootDevInfoInit ();
  InitBbsTypeTable ();
  return BootDevInfoSyncToBootConfig ();
}

/**
  Save the current settings of boot configuration varstore.

  @retval EFI_SUCCESS              Save the current settings successfully.
  @retval Other                    Fail to set boot option variable by device switch info.
**/
EFI_STATUS
BootConfigSaveSetting (
  VOID
  )
{
  return BootDevInfoSetToVariable ();
}

/**
  Clean space character in the front and back of input string.

  @param Str             Input string

**/
VOID
CleanSpaceChar (
  IN    CHAR16    *Str
  )
{
  UINTN     StrLength;
  UINTN     Start;
  UINTN     End;
  UINTN     ValidLength;
  CHAR16    SpaceChar = ' ';

  if (Str == NULL) {
    return;
  }

  StrLength = StrLen (Str);
  if (StrLength == 0) {
    return;
  }

  Start = 0;
  End   = StrLength - 1;

  while (Str[Start] == SpaceChar) {
    Start++;
  }
  if (Start == StrLength) {
    //
    // All chars are space char, no need to remove space chars.
    //
    return;
  }

  while (Str[End] == SpaceChar) {
    End--;
  }

  ValidLength = End - Start + 1;
  if (ValidLength < StrLength) {
    gBS->CopyMem (&Str[0], &Str[Start], ValidLength * sizeof(CHAR16));
    gBS->SetMem (&Str[ValidLength], (StrLength - ValidLength) * sizeof(CHAR16), 0);
  }
}

/**
  Check whether the EFI_COMPONENT_NAME_PROTOCOL instance is valid for use.

  @param[in] ComponentName      A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance
  @param[in] Handle             The handle of Block IO device

  @retval TRUE                  The protocol is valid
  @retval FALSE                 The protocol is invalid

**/
BOOLEAN
IsValidComponentNameProtocol (
  IN  EFI_COMPONENT_NAME_PROTOCOL    *ComponentName,
  IN  EFI_HANDLE                     Handle
  )
{
  EFI_STATUS                     Status;
  CHAR16                         *String1;
  CHAR16                         *String2;

  //
  // Some incomplete ComponentName protocols return success regardless of invalid inputs, which brings the wrong device name received.
  // Try the cases that gives illegal inputs to filter these incomplete ComponentName protocol.
  //
  // Give illegal inputs (ControllerHandle = NULL)
  //
  Status = ComponentName->GetControllerName (
                            ComponentName,
                            NULL,
                            Handle,
                            ComponentName->SupportedLanguages,
                            &String1
                            );
  if (EFI_ERROR (Status)) {
    return TRUE;
  }

  //
  // Give illegal inputs (ControllerHandle = NULL, ChildHandle = NULL)
  //
  Status = ComponentName->GetControllerName (
                            ComponentName,
                            NULL,
                            NULL,
                            ComponentName->SupportedLanguages,
                            &String2
                            );
  if (EFI_ERROR (Status)) {
    return TRUE;
  }

  //
  // The protocol that return a fixed data in the above cases is considered invalid
  //
  return (CompareMem (String1, String2, StrLen (String1)) != 0);
}

/**
 Get device name from Component Name (2) protocol.

 @param[in] ControllerHandle             The handle of a controller
 @param[in] BlockIoHandle                The handle of a Block IO device
 @param[in] ComponentNameProtocolGuid    A pointer to an EFI_GUID. It points to Component Name (2) protocol GUID
 @param[in] ComponentNameHandleCount     The number of Component Name (2) protocol handles
 @param[in] ComponentNameHandleList      A pointer to an array of Component Name (2) protocol handles

 @return A pointer to device name or NULL if not found.
**/
CHAR16 *
GetDevNameFromComponentNameProtocol (
  IN EFI_HANDLE                          ControllerHandle,
  IN EFI_HANDLE                          BlockIoHandle,
  IN EFI_GUID                            *ComponentNameProtocolGuid,
  IN UINTN                               ComponentNameHandleCount,
  IN EFI_HANDLE                          *ComponentNameHandleList
  )
{
  UINTN                                  Index;
  EFI_COMPONENT_NAME_PROTOCOL            *ComponentName;
  CHAR16                                 *DeviceName;
  EFI_STATUS                             Status;

  for (Index = 0; Index < ComponentNameHandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    ComponentNameHandleList[Index],
                    ComponentNameProtocolGuid,
                    (VOID **) &ComponentName
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = ComponentName->GetControllerName (
                               ComponentName,
                               ControllerHandle,
                               BlockIoHandle,
                               ComponentName->SupportedLanguages,
                               &DeviceName
                               );
    if (EFI_ERROR (Status)) {
      //
      // For some Single-LUN storages, BlockIo protocol may be installed on the ControllerHandle instead of a new handle created.
      // Trying the BlockIo handle as the ControllerHandle for retrieving the device name.
      //
      Status = ComponentName->GetControllerName (
                                 ComponentName,
                                 BlockIoHandle,
                                 BlockIoHandle,
                                 ComponentName->SupportedLanguages,
                                 &DeviceName
                                 );
    }
    if (EFI_ERROR (Status)) {
      continue;
    }
    if (!IsValidComponentNameProtocol (ComponentName, BlockIoHandle)) {
      continue;
    }
    return DeviceName;
  }

  return NULL;
}

/**
  Get all hardware boot device information (BlockIo device path and device name).

  @param HwBootDeviceInfoCount   Hardware boot device information count
  @param HwBootDeviceInfo        Array pointer of hardware boot device information

  @retval EFI_SUCCESS            Get information successfully
  @return Other                  Invalid input parameter.
                                 Locate protocol fail or get usb device information fail

**/
EFI_STATUS
GetAllHwBootDeviceInfo (
  OUT UINTN                          *HwBootDeviceInfoCount,
  OUT HARDWARE_BOOT_DEVICE_INFO      **HwBootDeviceInfo
  )
{
  EFI_STATUS                     Status;
  UINTN                          HandleCount;
  EFI_HANDLE                     *HandleBuffer;
  UINTN                          Index;
  EFI_DEVICE_PATH_PROTOCOL       *BlkIoDevicePath;
  EFI_DEVICE_PATH_PROTOCOL       *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL       *AppendedDevicePath;
  EFI_DEVICE_PATH_PROTOCOL       *ControllerDevicePath;
  EFI_HANDLE                     ControllerHandle;
  CHAR16                         *DeviceName;
  UINTN                          ComponentNameHandleCount;
  UINTN                          ComponentName2HandleCount;
  EFI_HANDLE                     *ComponentNameHandleBuffer;
  EFI_HANDLE                     *ComponentName2HandleBuffer;
  UINTN                          HwBootDeviceInfoLength;
  UINTN                          Increment;
  BOOLEAN                        StopSearching;

  if (HwBootDeviceInfoCount == NULL || HwBootDeviceInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *HwBootDeviceInfoCount = 0;
  *HwBootDeviceInfo = NULL;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiBlockIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiComponentNameProtocolGuid , NULL, &ComponentNameHandleCount , &ComponentNameHandleBuffer);
  if (EFI_ERROR (Status)) {
    ComponentNameHandleCount   = 0;
    ComponentNameHandleBuffer  = NULL;
  }
  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiComponentName2ProtocolGuid, NULL, &ComponentName2HandleCount, &ComponentName2HandleBuffer);
  if (EFI_ERROR (Status)) {
    ComponentName2HandleCount  = 0;
    ComponentName2HandleBuffer = NULL;
  }
  if (ComponentNameHandleCount == 0 && ComponentName2HandleCount == 0) {
    goto Exit;
  }

  Increment = 10;
  HwBootDeviceInfoLength = 0;
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID *) &BlkIoDevicePath
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    //
    // Skip media level BlkIo instance
    //
    TempDevicePath = BlkIoDevicePath;
    while (!IsDevicePathEnd (TempDevicePath) && DevicePathType (TempDevicePath) != MEDIA_DEVICE_PATH) {
      TempDevicePath = NextDevicePathNode (TempDevicePath);
    }
    if (!IsDevicePathEnd (TempDevicePath)) {
      continue;
    }

    //
    // Find contoller handle and retrieve the device name
    //
    StopSearching    = FALSE;
    ControllerHandle = HandleBuffer[Index];
    while (!StopSearching) {
      Status = gBS->HandleProtocol (
                      ControllerHandle,
                      &gEfiDevicePathProtocolGuid,
                      (VOID *)&TempDevicePath
                      );
      if (EFI_ERROR (Status)) {
        break;
      }

      AppendedDevicePath   = NULL;
      ControllerDevicePath = NULL;
      while (!IsDevicePathEnd (NextDevicePathNode (TempDevicePath))) {
        ControllerDevicePath = AppendDevicePathNode (AppendedDevicePath, TempDevicePath);
        if (AppendedDevicePath != NULL) {
          FreePool (AppendedDevicePath);
        }
        AppendedDevicePath = ControllerDevicePath;
        TempDevicePath = NextDevicePathNode (TempDevicePath);
      }

      //
      // Stop the searching at PCI controller device path
      //
      StopSearching = ((DevicePathType (TempDevicePath) == HARDWARE_DEVICE_PATH) &&
                      (DevicePathSubType (TempDevicePath) == HW_PCI_DP));

      Status = gBS->LocateDevicePath (
                      &gEfiDevicePathProtocolGuid,
                      &ControllerDevicePath,
                      &ControllerHandle
                      );
      if (AppendedDevicePath != NULL) {
        FreePool (AppendedDevicePath);
      }
      if (EFI_ERROR (Status)) {
        break;
      }

      DeviceName = GetDevNameFromComponentNameProtocol (ControllerHandle, HandleBuffer[Index], &gEfiComponentName2ProtocolGuid, ComponentName2HandleCount, ComponentName2HandleBuffer);
      if (DeviceName == NULL) {
        DeviceName = GetDevNameFromComponentNameProtocol (ControllerHandle, HandleBuffer[Index], &gEfiComponentNameProtocolGuid, ComponentNameHandleCount, ComponentNameHandleBuffer);
        if (DeviceName == NULL) {
          continue;
        }
      }

      if ((*HwBootDeviceInfoCount) >= HwBootDeviceInfoLength) {
        *HwBootDeviceInfo = ReallocatePool (
                              HwBootDeviceInfoLength * sizeof(HARDWARE_BOOT_DEVICE_INFO),
                              (HwBootDeviceInfoLength + Increment) * sizeof(HARDWARE_BOOT_DEVICE_INFO),
                              *HwBootDeviceInfo
                              );
        if (*HwBootDeviceInfo == NULL) {
          Status = EFI_OUT_OF_RESOURCES;
          goto Exit;
        }
        HwBootDeviceInfoLength += Increment;
      }
      (*HwBootDeviceInfo)[*HwBootDeviceInfoCount].BlockIoDevicePath = BlkIoDevicePath;
      (*HwBootDeviceInfo)[*HwBootDeviceInfoCount].HwDeviceName      = AllocateCopyPool (StrSize (DeviceName), DeviceName);
      CleanSpaceChar ((*HwBootDeviceInfo)[*HwBootDeviceInfoCount].HwDeviceName);

      (*HwBootDeviceInfoCount)++;
      break;
    }
  }

Exit:
  if (HandleCount != 0) {
    FreePool (HandleBuffer);
  }
  if (ComponentNameHandleCount != 0) {
    FreePool (ComponentNameHandleBuffer);
  }
  if (ComponentName2HandleBuffer != NULL) {
    FreePool (ComponentName2HandleBuffer);
  }
  if (*HwBootDeviceInfoCount == 0) {
    Status = EFI_NOT_FOUND;
  }

  return Status;
}

/**
  Check whether there is a instance in BlockIoDevicePath, which contain multi device path
  instances, has the same partition node with HardDriveDevicePath device path

  @param BlockIoDevicePath       Multi device path instances which need to check
  @param HardDriveDevicePath     A device path which starts with a hard drive media device path.

  @retval TRUE                   There is a matched device path instance
  @retval FALSE                  There is no matched device path instance

**/
BOOLEAN
MatchPartitionDevicePathNode (
  IN  EFI_DEVICE_PATH_PROTOCOL   *BlockIoDevicePath,
  IN  HARDDRIVE_DEVICE_PATH      *HardDriveDevicePath
  )
{
  HARDDRIVE_DEVICE_PATH     *Node;

  if ((BlockIoDevicePath == NULL) || (HardDriveDevicePath == NULL)) {
    return FALSE;
  }

  //
  // Match all the partition device path nodes including the nested partition nodes
  //
  while (!IsDevicePathEnd (BlockIoDevicePath)) {
    if ((DevicePathType (BlockIoDevicePath) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (BlockIoDevicePath) == MEDIA_HARDDRIVE_DP)
        ) {
      //
      // See if the harddrive device path in blockio matches the orig Hard Drive Node
      //
      Node = (HARDDRIVE_DEVICE_PATH *) BlockIoDevicePath;

      //
      // Match Signature and PartitionNumber.
      // Unused bytes in Signature are initiaized with zeros.
      //
      if ((Node->PartitionNumber == HardDriveDevicePath->PartitionNumber) &&
          (Node->MBRType == HardDriveDevicePath->MBRType) &&
          (Node->SignatureType == HardDriveDevicePath->SignatureType) &&
          (CompareMem (Node->Signature, HardDriveDevicePath->Signature, sizeof (Node->Signature)) == 0)) {
        return TRUE;
      }
    }

    BlockIoDevicePath = NextDevicePathNode (BlockIoDevicePath);
  }

  return FALSE;
}

/**
  If input is a hard drive device path, append it to corresponding BlockIo device path.
  If input is not a hard drive device path, output NULL.

  @param HardDriveDevicePath     Input device path

  @return pointer to the device path which combines BlockIo and hard disk device path
          or NULL if it is not hard disk device.

**/
EFI_DEVICE_PATH_PROTOCOL *
AppendHardDrivePathToBlkIoDevicePath (
  IN  HARDDRIVE_DEVICE_PATH      *HardDriveDevicePath
  )
{
  EFI_STATUS                Status;
  UINTN                     BlockIoHandleCount;
  EFI_HANDLE                *BlockIoBuffer;
  EFI_DEVICE_PATH_PROTOCOL  *BlockIoDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *NewDevicePath;
  UINTN                     Index;

  if (HardDriveDevicePath == NULL) {
    return NULL;
  }

  if (!((DevicePathType (&HardDriveDevicePath->Header) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (&HardDriveDevicePath->Header) == MEDIA_HARDDRIVE_DP))) {
    return NULL;
  }

  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiBlockIoProtocolGuid, NULL, &BlockIoHandleCount, &BlockIoBuffer);
  if (EFI_ERROR (Status) || BlockIoHandleCount == 0) {
    return NULL;
  }

  for (Index = 0; Index < BlockIoHandleCount; Index++) {
    Status = gBS->HandleProtocol (BlockIoBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID *) &BlockIoDevicePath);
    if (EFI_ERROR (Status) || BlockIoDevicePath == NULL) {
      continue;
    }

    if (MatchPartitionDevicePathNode (BlockIoDevicePath, HardDriveDevicePath)) {
      //
      // Combine the Block IO and Hard Drive Device path together.
      //
      DevicePath = NextDevicePathNode ((EFI_DEVICE_PATH_PROTOCOL *) HardDriveDevicePath);
      NewDevicePath = AppendDevicePath (BlockIoDevicePath, DevicePath);

      FreePool (BlockIoBuffer);
      return NewDevicePath;
    }
  }

  FreePool (BlockIoBuffer);
  return NULL;
}

/**
  Compare with each BlockIo device path.

  @param  HardDriveDevicePath    BlockIo device path
  @param  DevicePath             BlockIo device path or hard drive device path

  @retval TRUE                   BlockIo device paths are the same
  @retval FALSE                  BlockIo device paths are different

**/
BOOLEAN
CompareBlockIoDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL *BlockIoDevicePath,
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL *NewDevicePath;
  EFI_DEVICE_PATH_PROTOCOL *TempDevicePath;
  BOOLEAN                  Match;

  Match = TRUE;

  if (BlockIoDevicePath == NULL || DevicePath == NULL || IsDevicePathEnd (BlockIoDevicePath)) {
    return FALSE;
  }
  NewDevicePath = AppendHardDrivePathToBlkIoDevicePath ((HARDDRIVE_DEVICE_PATH *) DevicePath);
  if (NewDevicePath == NULL) {
    NewDevicePath = DevicePath;
  }

  TempDevicePath = NewDevicePath;
  while (!IsDevicePathEnd (BlockIoDevicePath)) {
    if (CompareMem (BlockIoDevicePath, TempDevicePath, DevicePathNodeLength (BlockIoDevicePath)) != 0) {
      Match = FALSE;
      break;
    }

    BlockIoDevicePath = NextDevicePathNode (BlockIoDevicePath);
    TempDevicePath = NextDevicePathNode (TempDevicePath);
  }

  if (NewDevicePath != DevicePath) {
    FreePool (NewDevicePath);
  }

  if (Match) {
    return TRUE;
  }

  return FALSE;
}

/**
  Append device name to boot option description.

  @param  EfiBootDevCount        Number of EFI boot option
  @param  EfiBootDevInfo         Array pointer of BOOT_DEV_INFO which store each boot option information

  @retval EFI_SUCCESS            Append device name successfully
  @retval EFI_INVALID_PARAMETER  Invalid input parameter

**/
EFI_STATUS
BootOptionStrAppendDeviceName (
  IN     UINT16                      EfiBootDevCount,
  IN OUT BOOT_DEV_INFO               *EfiBootDevInfo
  )
{
  UINTN                              Index;
  UINTN                              Index2;
  HARDWARE_BOOT_DEVICE_INFO          *HwBootDeviceInfo;
  UINTN                              HwBootDeviceCount;
  CHAR16                             *String;
  EFI_DEVICE_PATH_PROTOCOL           *DevicePath;
  UINTN                              Size;
  CHAR16                             *HwDeviceName;
  CHAR16                             NoDeviceStr[] = L"No Device";
  UINTN                              DeviceCount;
  UINT8                              *Ptr;
  UINT8                              *VarData;

  if (EfiBootDevCount == 0 || EfiBootDevInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  GetAllHwBootDeviceInfo (&HwBootDeviceCount, &HwBootDeviceInfo);

  for (Index = 0; Index < EfiBootDevCount; Index++) {
    DevicePath = EfiBootDevInfo[Index].DevicePath;
    if (!((DevicePathType (DevicePath) == MEDIA_DEVICE_PATH &&
           DevicePathSubType (DevicePath) == MEDIA_HARDDRIVE_DP) ||
          (DevicePathType (DevicePath) == MESSAGING_DEVICE_PATH &&
           DevicePathSubType (DevicePath) == MSG_USB_CLASS_DP))) {
      continue;
    }

    VarData      = NULL;
    HwDeviceName = NULL;
    DeviceCount  = 1;
    if (DevicePathType (DevicePath) == MESSAGING_DEVICE_PATH &&
        DevicePathSubType (DevicePath) == MSG_USB_CLASS_DP) {
      //
      // For Windows To Go, assign no device string as device name if device number is 0.
      //
      VarData = CommonGetVariableData (
                  L"WindowsToGo",
                  &gEfiGenericVariableGuid
                  );
      if (VarData != NULL) {
        Ptr         = VarData;
        DevicePath  = (EFI_DEVICE_PATH_PROTOCOL *) Ptr;
        Ptr        += GetDevicePathSize (DevicePath);
        DeviceCount = *(UINT16 *) Ptr;
      } else {
        DeviceCount = 0;
      }

      if (DeviceCount == 0) {
        HwDeviceName = NoDeviceStr;
      }
    }

    //
    // Append device name to boot option description if BlkIo device path is match.
    //
    if (HwDeviceName == NULL && HwBootDeviceInfo != NULL) {
      for (Index2 = 0; Index2 < HwBootDeviceCount; Index2++) {
        if (CompareBlockIoDevicePath (HwBootDeviceInfo[Index2].BlockIoDevicePath, DevicePath)) {
          HwDeviceName = HwBootDeviceInfo[Index2].HwDeviceName;
          break;
        }
      }
    }

    if (HwDeviceName != NULL) {
      if (DeviceCount > 1) {
        Size = StrSize (EfiBootDevInfo[Index].pString) + StrSize (L" (") + StrSize (HwDeviceName) + StrSize (L",...") + StrSize (L")");
      } else {
        Size = StrSize (EfiBootDevInfo[Index].pString) + StrSize (L" (") + StrSize (HwDeviceName) + StrSize (L")");
      }
      String = AllocateZeroPool (Size);
      if (String != NULL) {
        StrCatS (String, Size / sizeof (CHAR16), EfiBootDevInfo[Index].pString);
        StrCatS (String, Size / sizeof (CHAR16), L" (");
        StrCatS (String, Size / sizeof (CHAR16), HwDeviceName);
        if (DeviceCount > 1) {
          StrCatS (String, Size / sizeof (CHAR16), L",...");
        }
        StrCatS (String, Size / sizeof (CHAR16), L")");

        FreePool (EfiBootDevInfo[Index].pString);
        EfiBootDevInfo[Index].pString = String;
      }
    }

    if (VarData != NULL) {
      FreePool (VarData);
    }
  }

  if (HwBootDeviceCount != 0 && HwBootDeviceInfo != NULL) {
    for (Index2 = 0; Index2 < HwBootDeviceCount; Index2++) {
      FreePool (HwBootDeviceInfo[Index2].HwDeviceName);
    }
    FreePool (HwBootDeviceInfo);
  }

  return EFI_SUCCESS;
}

/**
 Update action/checkbox opcodes in specific label by input boot device info list.

 @param[in] HiiHandle                          HII handle for HII get string function
 @param[in] LastToken                          Pointer to the start string token number
 @param[in] BootDevInfoList                    Pointer to the boot device info list
 @param[in] BootDevInfoCount                   The number of boot device info in the list
 @param[in] DeviceKeyBase                      Question ID base value
 @param[in] BootDevLabel                       VFR label value
 @param[in] FormId                             Form ID
 @param[in] Offset                             Offset value in the varstorage
 @param[in] Opcode                             Target opcode (action or checkbox)

 @retval EFI_SUCCESS                           Process item movement successfully
 @retval EFI_OUT_OF_RESOURCES                  Create HII opcode handle failed.
 @retval Other                                 Fail to get browser data.
**/
STATIC
EFI_STATUS
UpdateNewForm (
  IN EFI_HII_HANDLE                            HiiHandle,
  IN EFI_STRING_ID                             *LastToken,
  IN BOOT_DEV_INFO                             *BootDevInfoList,
  IN UINTN                                     BootDevInfoCount,
  IN UINT16                                    DeviceKeyBase,
  IN UINT16                                    BootDevLabel,
  IN EFI_FORM_ID                               FormId,
  IN UINT16                                    Offset,
  IN UINT8                                     Opcode
  )
{
  UINTN                                        Index;
  EFI_STRING_ID                                TempLastToken;
  SETUP_UTILITY_BROWSER_DATA                   *SuBrowser;
  EFI_STATUS                                   Status;
  EFI_IFR_GUID_LABEL                           *StartLabel;
  EFI_IFR_CHECKBOX                             *CheckboxOpCode;
  EFI_IFR_ACTION                               *ActionOpCode;
  VOID                                         *OpCodeHandle;
  UINT8                                        IfrFlag;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  OpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (OpCodeHandle != NULL);
  if (OpCodeHandle == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (OpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = BootDevLabel;

  if (BootDevInfoList == NULL) {
    goto Done;
  }

  IfrFlag = EFI_IFR_CHECKBOX_DEFAULT_MFG;

  for (Index = 0; Index < BootDevInfoCount; Index++) {
    TempLastToken = HiiSetString (HiiHandle, 0, BootDevInfoList[Index].pString, NULL);
    if (Index == 0) {
      *LastToken = TempLastToken;
    }

    switch (Opcode) {

    case EFI_IFR_CHECKBOX_OP:
      CheckboxOpCode = (EFI_IFR_CHECKBOX *) HiiCreateCheckBoxOpCode (
                                              OpCodeHandle,
                                              (EFI_QUESTION_ID) (DeviceKeyBase + Index),
                                              BOOT_VARSTORE_ID,
                                              (UINT16) (Offset + Index),
                                              TempLastToken,
                                              0,
                                              EFI_IFR_FLAG_CALLBACK,
                                              0,
                                              NULL
                                              );
      CheckboxOpCode->Question.Flags |= IfrFlag;
      break;

    case EFI_IFR_ACTION_OP:
      ActionOpCode = (EFI_IFR_ACTION *) HiiCreateActionOpCode (
                                          OpCodeHandle,
                                          (EFI_QUESTION_ID) (DeviceKeyBase + Index),
                                          TempLastToken,
                                          0,
                                          0,
                                          0
                                          );
      ActionOpCode->Question.Flags |= IfrFlag;
      break;
    }
  }

Done:
  HiiUpdateForm (
    HiiHandle,
    NULL,
    FormId,
    OpCodeHandle,
    NULL
    );

  HiiFreeOpCodeHandle (OpCodeHandle);
  return EFI_SUCCESS;
}

/**
 Get the device info list of legacy boot type

 @param[in]  HiiHandle                      HII handle for HII get string function
 @param[in]  KernelConfig                   Pointer to kernel configuration
 @param[out] LegacyBootTypeDevInfoCount     Number of device info in the list
 @param[out] BootTypeOrderDevInfo           Double pointer to the device info list of legacy boot type

 @retval EFI_SUCCESS                        Get the device info list successfully.
 @retval EFI_OUT_OF_RESOURCES               Allocate pool fail
**/
EFI_STATUS
GetLegacyBootTypeDevInfoList (
  IN  EFI_HII_HANDLE                        HiiHandle,
  IN  KERNEL_CONFIGURATION                  *KernelConfig,
  OUT UINT16                                *LegacyBootTypeDevInfoCount,
  OUT BOOT_DEV_INFO                         **LegacyBootTypeDevInfoList
  )
{
  UINT16                                    Index;
  UINT16                                    BootTypeCount;
  BBS_TYPE_TABLE                            *BbsTypeTable;
  BOOT_DEV_INFO                             *InfoList;

  BootTypeCount = GetBootTypeOrderCount (KernelConfig);

  InfoList = AllocateZeroPool (sizeof (BOOT_DEV_INFO) * BootTypeCount);
  if (InfoList == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < BootTypeCount; Index++) {
    BbsTypeTable = GetBbsTypeTableByDevType (KernelConfig->BootTypeOrder[Index]);
    if (BbsTypeTable != NULL) {
      InfoList[Index].pString = HiiGetString (HiiHandle, BbsTypeTable->StrToken, NULL);
    }
  }

  *LegacyBootTypeDevInfoList  = InfoList;
  *LegacyBootTypeDevInfoCount = BootTypeCount;
  return EFI_SUCCESS;
}

/**
 Get device info list by boot type (EFI or Legacy).

 @param[in]  BootType                       Boot type (EFI or Legacy)
 @param[out] BootDevInfoCount               Pointer to the number of boot device info in the list
 @param[out] BootDevInfoList                Double pointer to the boot devices info list

 @retval EFI_SUCCESS                        Get device info list successfully.
 @retval EFI_OUT_OF_RESOURCES               Allocate pool fail.
**/
EFI_STATUS
GetDevInfoListByBootType (
  IN  UINT16                                BootType,
  OUT UINT16                                *BootDevInfoCount,
  OUT BOOT_DEV_INFO                         **BootDevInfoList
  )
{
  UINTN                                     Index;
  UINT16                                    InfoCount;
  BOOT_DEV_INFO                             *InfoList;
  BOOLEAN                                   IsEfiBootDev;

  IsEfiBootDev = (BOOLEAN) (BootType == EFI_BOOT_DEV);

  for (Index = 0, InfoCount = 0; Index < mBootDevInfoCount; Index++) {
    if (mBootDevInfo[Index].IsEfiBootDev == IsEfiBootDev) {
      InfoCount++;
    }
  }
  if (InfoCount == 0) {
    *BootDevInfoCount = 0;
    *BootDevInfoList  = NULL;
    return EFI_SUCCESS;
  }

  InfoList = AllocateZeroPool (sizeof(BOOT_DEV_INFO) * InfoCount);
  if (InfoList == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0, InfoCount = 0; Index < mBootDevInfoCount; Index++) {
    if (mBootDevInfo[Index].IsEfiBootDev == IsEfiBootDev) {
      CopyMem (
        &InfoList[InfoCount],
        &mBootDevInfo[Index],
        sizeof (BOOT_DEV_INFO)
        );
      if (mBootDevInfo[Index].pString != NULL) {
        InfoList[InfoCount].pString = AllocateCopyPool (
                                        StrSize (mBootDevInfo[Index].pString),
                                        mBootDevInfo[Index].pString
                                        );
      }
      InfoCount++;
    }
  }

  *BootDevInfoCount = InfoCount;
  *BootDevInfoList  = InfoList;
  return EFI_SUCCESS;
}

/**
 Free the device info list

 @param[in] DevInfoCount      Number of device info in the list
 @param[in] DevInfoList       Pointer to the device info list
**/
VOID
FreeDevInfoList (
  IN UINT16                                 DevInfoCount,
  IN BOOT_DEV_INFO                          *DevInfoList
  )
{
  UINTN                                     Index;

  if (DevInfoList == NULL) {
    return;
  }

  for (Index = 0; Index < DevInfoCount; Index++) {
    if (DevInfoList[Index].pString != NULL) {
      FreePool (DevInfoList[Index].pString);
    }
  }
  FreePool (DevInfoList);
}

/**
 Update legacy boot type order label

 @param[in]      HiiHandle                  HII handle for HII get string function
 @param[in]      KernelConfig               Pointer to kernel configuration
 @param[in, out] SUCInfo                    Pointer to setup utility configuration

 @retval EFI_SUCCESS                        Update legacy boot type order label successfully.
**/
EFI_STATUS
UpdateLegacyBootTypeOrderLabel (
  IN     EFI_HII_HANDLE                     HiiHandle,
  IN     KERNEL_CONFIGURATION               *KernelConfig,
  IN OUT SETUP_UTILITY_CONFIGURATION        *SUCInfo
  )
{
  EFI_STATUS                                Status;
  BOOT_DEV_INFO                             *LegacyBootTypeDevInfoList;
  UINT16                                    LegacyBootTypeDevInfoCount;

  Status = GetLegacyBootTypeDevInfoList (HiiHandle, KernelConfig, &LegacyBootTypeDevInfoCount, &LegacyBootTypeDevInfoList);
  if (EFI_ERROR (Status)) {
    LegacyBootTypeDevInfoList  = NULL;
    LegacyBootTypeDevInfoCount = 0;
  }

  SUCInfo->BootTypeTokenRecord = 0;
  UpdateNewForm (
    HiiHandle,
    &SUCInfo->BootTypeTokenRecord,
    LegacyBootTypeDevInfoList,
    LegacyBootTypeDevInfoCount,
    (UINT16) KEY_BOOT_TYPE_ORDER_BASE,
    (UINT16) BOOT_ORDER_LABEL,
    BOOT_DEVICE_LEG_NOR_BOOT_ID,
    0,
    EFI_IFR_ACTION_OP
    );

  FreeDevInfoList (LegacyBootTypeDevInfoCount, LegacyBootTypeDevInfoList);
  return EFI_SUCCESS;
}

/**
 Update the strings of legacy boot type order

 @param[in]      HiiHandle                  HII handle for HII get string function
 @param[in]      KernelConfig               Pointer to kernel configuration
 @param[in, out] SUCInfo                    Pointer to setup utility configuration

 @retval EFI_SUCCESS                        Update the strings of legacy boot type order successfully.
 @retval Other                              Get device info list failed.
**/
EFI_STATUS
UpdateLegacyBootTypeOrderStrings (
  IN     EFI_HII_HANDLE                     HiiHandle,
  IN     KERNEL_CONFIGURATION               *KernelConfig,
  IN OUT SETUP_UTILITY_CONFIGURATION        *SUCInfo
  )
{
  EFI_STATUS                                Status;
  BOOT_DEV_INFO                             *LegacyBootTypeDevInfoList;
  UINT16                                    LegacyBootTypeDevInfoCount;
  UINT16                                    Index;
  EFI_STRING_ID                             Token;

  Status = GetLegacyBootTypeDevInfoList (HiiHandle, KernelConfig, &LegacyBootTypeDevInfoCount, &LegacyBootTypeDevInfoList);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Token = SUCInfo->BootTypeTokenRecord;
  if (LegacyBootTypeDevInfoList != NULL) {
    for (Index = 0; Index < LegacyBootTypeDevInfoCount; Index++, Token++) {
      HiiSetString (HiiHandle, Token, LegacyBootTypeDevInfoList[Index].pString, NULL);
    }
  }

  FreeDevInfoList (LegacyBootTypeDevInfoCount, LegacyBootTypeDevInfoList);
  return EFI_SUCCESS;
}

/**
 Update EFI boot order label

 @param[in]      HiiHandle                  HII handle for HII get string function
 @param[in, out] SUCInfo                    Pointer to setup utility configuration

 @retval EFI_SUCCESS                        Update EFI boot order label successfully.
**/
EFI_STATUS
UpdateEfiBootOrderLabel (
  IN     EFI_HII_HANDLE                     HiiHandle,
  IN OUT SETUP_UTILITY_CONFIGURATION        *SUCInfo
  )
{
  EFI_STATUS                                Status;
  BOOT_DEV_INFO                             *EfiBootDevInfoList;
  UINT16                                    EfiBootDevInfoCount;

  Status = GetDevInfoListByBootType (EFI_BOOT_DEV, &EfiBootDevInfoCount, &EfiBootDevInfoList);
  if (EFI_ERROR (Status)) {
    EfiBootDevInfoList  = NULL;
    EfiBootDevInfoCount = 0;
  }
  ASSERT (EfiBootDevInfoCount <= MAX_BOOT_DEVICES_NUMBER);

  SUCInfo->EfiBootDeviceNum = EfiBootDevInfoCount;
  SUCInfo->EfiTokenRecord   = 0;
  BootOptionStrAppendDeviceName (EfiBootDevInfoCount, EfiBootDevInfoList);
  UpdateNewForm (
    HiiHandle,
    &SUCInfo->EfiTokenRecord,
    EfiBootDevInfoList,
    EfiBootDevInfoCount,
    (UINT16) KEY_EFI_BOOT_DEVICE_BASE,
    (UINT16) EFI_BOOT_DEVICE_LABEL,
    BOOT_DEVICE_EFI_FORM_ID,
    (UINT16) ((UINTN) mBootConfig.EfiBootDevSwitch - (UINTN) &mBootConfig),
    EFI_IFR_CHECKBOX_OP
    );

  FreeDevInfoList (EfiBootDevInfoCount, EfiBootDevInfoList);
  return EFI_SUCCESS;
}

/**
 Update the strings of EFI boot order

 @param[in]      HiiHandle                  HII handle for HII get string function
 @param[in, out] SUCInfo                    Pointer to setup utility configuration

 @retval EFI_SUCCESS                        Update the strings of EFI boot order successfully.
 @retval Other                              Get device info list failed.
**/
EFI_STATUS
UpdateEfiBootOrderStrings (
  IN     EFI_HII_HANDLE                     HiiHandle,
  IN OUT SETUP_UTILITY_CONFIGURATION        *SUCInfo
  )
{
  EFI_STATUS                                Status;
  BOOT_DEV_INFO                             *EfiBootDevInfoList;
  UINT16                                    EfiBootDevInfoCount;
  UINT16                                    Index;
  EFI_STRING_ID                             Token;

  Status = GetDevInfoListByBootType (EFI_BOOT_DEV, &EfiBootDevInfoCount, &EfiBootDevInfoList);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Token = SUCInfo->EfiTokenRecord;
  if (EfiBootDevInfoList != NULL) {
    BootOptionStrAppendDeviceName (EfiBootDevInfoCount, EfiBootDevInfoList);

    for (Index = 0; Index < EfiBootDevInfoCount; Index++, Token++) {
      HiiSetString (HiiHandle, Token, EfiBootDevInfoList[Index].pString, NULL);
    }
  }

  FreeDevInfoList (EfiBootDevInfoCount, EfiBootDevInfoList);
  return EFI_SUCCESS;
}

/**
 Update legacy advance boot order label

 @param[in]      HiiHandle                  HII handle for HII get string function
 @param[in, out] SUCInfo                    Pointer to setup utility configuration

 @retval EFI_SUCCESS                        Update legacy advance boot order label successfully.
**/
EFI_STATUS
UpdateLegacyAdvBootOrderLabel (
  IN     EFI_HII_HANDLE                     HiiHandle,
  IN OUT SETUP_UTILITY_CONFIGURATION        *SUCInfo
  )
{
  EFI_STATUS                                Status;
  BOOT_DEV_INFO                             *LegacyBootDevInfoList;
  UINT16                                    LegacyBootDevInfoCount;

  Status = GetDevInfoListByBootType (LEGACY_BOOT_DEV, &LegacyBootDevInfoCount, &LegacyBootDevInfoList);
  if (EFI_ERROR (Status)) {
    LegacyBootDevInfoList  = NULL;
    LegacyBootDevInfoCount = 0;
  }
  ASSERT (LegacyBootDevInfoCount <= MAX_LEGACY_BOOT_DEV_NUM);

  SUCInfo->LegacyBootDeviceNum      = LegacyBootDevInfoCount;
  SUCInfo->LegacyAdvanceTokenRecord = 0;
  UpdateNewForm (
    HiiHandle,
    &SUCInfo->LegacyAdvanceTokenRecord,
    LegacyBootDevInfoList,
    LegacyBootDevInfoCount,
    (UINT16) KEY_ADV_LEGACY_BOOT_BASE,
    (UINT16) BOOT_LEGACY_ADV_BOOT_LABEL,
    BOOT_OPTION_FORM_ID,
    (UINT16) ((UINTN) mBootConfig.LegacyAdvBootDevSwitch - (UINTN) &mBootConfig),
    EFI_IFR_CHECKBOX_OP
    );

  FreeDevInfoList (LegacyBootDevInfoCount, LegacyBootDevInfoList);
  return EFI_SUCCESS;
}

/**
 Update the strings of legacy advance boot order

 @param[in]      HiiHandle                  HII handle for HII get string function
 @param[in, out] SUCInfo                    Pointer to setup utility configuration

 @retval EFI_SUCCESS                        Update the strings of legacy advance boot order successfully.
 @retval Other                              Get device info list failed.
**/
EFI_STATUS
UpdateLegacyAdvBootOrderStrings (
  IN     EFI_HII_HANDLE                     HiiHandle,
  IN OUT SETUP_UTILITY_CONFIGURATION        *SUCInfo
  )
{
  EFI_STATUS                                Status;
  BOOT_DEV_INFO                             *LegacyBootDevInfoList;
  UINT16                                    LegacyBootDevInfoCount;
  UINT16                                    Index;
  EFI_STRING_ID                             Token;

  Status = GetDevInfoListByBootType (LEGACY_BOOT_DEV, &LegacyBootDevInfoCount, &LegacyBootDevInfoList);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Token = SUCInfo->LegacyAdvanceTokenRecord;
  if (LegacyBootDevInfoList != NULL) {
    for (Index = 0; Index < LegacyBootDevInfoCount; Index++, Token++) {
      HiiSetString (HiiHandle, Token, LegacyBootDevInfoList[Index].pString, NULL);
    }
  }

  FreeDevInfoList (LegacyBootDevInfoCount, LegacyBootDevInfoList);
  return EFI_SUCCESS;
}

/**
 Update legacy normal boot order label

 @param[in]      HiiHandle                  HII handle for HII get string function
 @param[in, out] SUCInfo                    Pointer to setup utility configuration

 @retval EFI_SUCCESS                        Update legacy normal boot order label successfully.
**/
EFI_STATUS
UpdateLegacyNormalBootOrderLabels (
  IN     EFI_HII_HANDLE                     HiiHandle,
  IN OUT SETUP_UTILITY_CONFIGURATION        *SUCInfo
  )
{
  UINTN                                     Index;
  UINT16                                    Offset;

  for (Index = 0; Index < mBbsTypeTableCount; Index++) {
    if (mBbsTypeTable[Index].KeyBootDeviceBase >= KEY_BOOT_DEVICE_TYPE_BASE) {
      Offset  = ((UINT16) (UINTN) &mBootConfig.LegacyTypeDevSwitch[mBbsTypeTable[Index].KeyBootDeviceBase - KEY_BOOT_DEVICE_TYPE_BASE]);
      Offset -= (UINT16) (UINTN) &mBootConfig;
    } else {
      Offset = 0;
    }

    SUCInfo->LegacyNormalTokenRecord[Index] = 0;
    UpdateNewForm (
      HiiHandle,
      &SUCInfo->LegacyNormalTokenRecord[Index],
      mBbsTypeTable[Index].DevInfoList,
      mBbsTypeTable[Index].DevInfoCount,
      mBbsTypeTable[Index].KeyBootDeviceBase,
      mBbsTypeTable[Index].BootDevicesLabel,
      mBbsTypeTable[Index].FormId,
      Offset,
      EFI_IFR_CHECKBOX_OP
      );
  }

  return EFI_SUCCESS;
}

/**
 Update the strings of legacy normal boot order

 @param[in]      HiiHandle                  HII handle for HII get string function
 @param[in, out] SUCInfo                    Pointer to setup utility configuration

 @retval EFI_SUCCESS                        Update the strings of legacy normal boot order successfully.
**/
EFI_STATUS
UpdateLegacyNormalBootOrderStrings (
  IN     EFI_HII_HANDLE                     HiiHandle,
  IN OUT SETUP_UTILITY_CONFIGURATION        *SUCInfo
  )
{
  UINTN                                     Index;
  EFI_STRING_ID                             Token;
  UINTN                                     DevIndex;

  for(Index = 0; Index < mBbsTypeTableCount; Index++) {
    if (mBbsTypeTable[Index].DevInfoCount == 0) {
      continue;
    }

    Token = SUCInfo->LegacyNormalTokenRecord[Index];
    for (DevIndex = 0; DevIndex < mBbsTypeTable[Index].DevInfoCount; DevIndex++, Token++) {
      HiiSetString (HiiHandle, Token, mBbsTypeTable[Index].DevInfoList[DevIndex].pString, NULL);
    }
  }

  return EFI_SUCCESS;
}

/**
 Update hidden boot type order label by ordered list opcode

 @param[in] HiiHandle                      Hii hanlde for Boot page
 @param[in] KernelConfig                   Pointer to kernel config instance

 @retval EFI_SUCCESS                       Update boot type order label successfully
 @retval EFI_OUT_OF_RESOURCES              Create HII opcode failed
**/
EFI_STATUS
UpdateHiddenBootTypeOrderLabel (
  IN EFI_HII_HANDLE                         HiiHandle,
  IN KERNEL_CONFIGURATION                   *KernelConfig
  )
{
  VOID                                      *OpCodeHandle;
  VOID                                      *OptionsOpCodeHandle;
  EFI_IFR_GUID_LABEL                        *StartLabel;
  UINT8                                     Count;
  UINTN                                     Index;
  UINTN                                     TableIndex;
  EFI_STRING_ID                             Token;
  UINT16                                    Offset;

  OpCodeHandle        = HiiAllocateOpCodeHandle ();
  OptionsOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (OpCodeHandle != NULL && OptionsOpCodeHandle != NULL);
  if (OpCodeHandle == NULL || OptionsOpCodeHandle == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (OpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = HIDDEN_BOOT_TYPE_ORDER_LABEL;

  Count = 0;
  for (Index = 0; Index < sizeof(KernelConfig->BootTypeOrder) && KernelConfig->BootTypeOrder[Index] != 0; Index++) {
    for (TableIndex = 0, Token = 0; TableIndex < mBbsTypeTableCount; TableIndex++) {
      if (mBbsTypeTable[TableIndex].DeviceType == KernelConfig->BootTypeOrder[Index]) {
        Token = mBbsTypeTable[TableIndex].StrToken;
        break;
      }
    }
    if (TableIndex == mBbsTypeTableCount) {
      continue;
    }

    HiiCreateOneOfOptionOpCode (
      OptionsOpCodeHandle,
      Token,
      0,
      EFI_IFR_NUMERIC_SIZE_1,
      KernelConfig->BootTypeOrder[Index]
      );
    Count++;
  }

  Offset = (UINT16) ((UINTN) KernelConfig->BootTypeOrder - (UINTN) KernelConfig);
  HiiCreateOrderedListOpCode (
     OpCodeHandle,
     KEY_HIDDEN_BOOT_TYPE_ORDER,
     CONFIGURATION_VARSTORE_ID,
     Offset,
     STRING_TOKEN (STR_BOOT_TYPE_ORDER_OPTIONS),
     STRING_TOKEN (STR_BOOT_TYPE_ORDER_OPTIONS),
     EFI_IFR_FLAG_CALLBACK,
     0,
     EFI_IFR_NUMERIC_SIZE_1,
     Count,
     OptionsOpCodeHandle,
     NULL
     );

  HiiUpdateForm (
    HiiHandle,
    NULL,
    BOOT_HIDDEN_BOOT_TYPE_ORDER_ID,
    OpCodeHandle,
    NULL
    );

  HiiFreeOpCodeHandle (OpCodeHandle);
  HiiFreeOpCodeHandle (OptionsOpCodeHandle);
  return EFI_SUCCESS;
}

/**
 Update all of device relative stirngs

 @param[in] HiiHandle                       Hii hanlde for Boot page
 @param[in] KernelConfig                    Pointer to kernel configuration instance

 @retval EFI_SUCCESS                        Update device relative strings successful.
 @retval Other                              Fail to get browser data.
**/
EFI_STATUS
SetupUtilityLibUpdateDeviceString (
  IN EFI_HII_HANDLE                         HiiHandle,
  IN KERNEL_CONFIGURATION                   *KernelConfig
  )
{
  EFI_STATUS                                Status;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo = SuBrowser->SUCInfo;

  if (SUCInfo->AdvBootDeviceNum == 0) {
    return EFI_SUCCESS;
  }

  BootDevInfoSortByBootOrder ();
  BootDevInfoSortByKernelConfig (KernelConfig);
  SortBbsTypeTable (SUCInfo->BootOrder, SUCInfo->AdvBootDeviceNum);

  UpdateLegacyBootTypeOrderStrings (HiiHandle, KernelConfig, SUCInfo);
  UpdateLegacyNormalBootOrderStrings (HiiHandle, SUCInfo);
  UpdateLegacyAdvBootOrderStrings (HiiHandle, SUCInfo);
  UpdateEfiBootOrderStrings (HiiHandle, SUCInfo);

  return EFI_SUCCESS;
}

/**
 Update boot order related menu option in action/checkbox opcode in platform to boot page

 @param[in] BootHiiHandle                   Hii hanlde for Boot page
 @param[in] KernelConfig                    Pointer to kernel configuration instance

 @retval EFI_SUCCESS                        Update boot menu successful.
 @retval Other                              Fail to get browser data.
**/
EFI_STATUS
UpdateBootOrderLabels (
  IN     EFI_HII_HANDLE                         BootHiiHandle,
  IN OUT KERNEL_CONFIGURATION                   *KernelConfig
  )
{
  EFI_STATUS                            Status;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo = SuBrowser->SUCInfo;
  ZeroMem (&mBootConfig, sizeof (mBootConfig));

  if (SUCInfo->AdvBootDeviceNum == 0) {
    return EFI_SUCCESS;
  }

  InitBbsTypeTable ();

  UpdateLegacyBootTypeOrderLabel (BootHiiHandle, KernelConfig, SUCInfo);
  UpdateLegacyNormalBootOrderLabels (BootHiiHandle, SUCInfo);
  UpdateLegacyAdvBootOrderLabel (BootHiiHandle, SUCInfo);
  UpdateEfiBootOrderLabel (BootHiiHandle, SUCInfo);
  UpdateHiddenBootTypeOrderLabel (BootHiiHandle, KernelConfig);

  return EFI_SUCCESS;
}

/**
  Move one boot type in the BootTypeOrder.

  @param  UpShift                TRUE : move LowerType upon UpperType
                                 FALSE: move UpperType below LowerType
  @param  UpperType              Upper boot type
  @param  LowerType              Lower boot type
  @param  BootTypeOrder          Pointer to boot type order
  @param  TypeCount              Boot type count

**/
VOID
MoveBootType (
  IN     BOOLEAN   UpShift,
  IN     UINT8     UpperType,
  IN     UINT8     LowerType,
  IN OUT UINT8     *BootTypeOrder,
  IN     UINTN     TypeCount
  )
{
  UINT8            *NewBootTypeOrder;
  UINTN            Index;
  UINTN            Count;

  if (TypeCount == 0) {
    return;
  }

  NewBootTypeOrder = AllocateZeroPool (TypeCount * sizeof (UINT8));
  if (NewBootTypeOrder == NULL) {
    return;
  }

  Count = 0;
  for (Index = 0; Index < TypeCount; Index++) {
    if (BootTypeOrder[Index] != UpperType &&
        BootTypeOrder[Index] != LowerType) {
      NewBootTypeOrder[Count++] = BootTypeOrder[Index];
      continue;
    }

    if ((UpShift  && BootTypeOrder[Index] == UpperType) ||
        (!UpShift && BootTypeOrder[Index] == LowerType)) {
      NewBootTypeOrder[Count++] = LowerType;
      NewBootTypeOrder[Count++] = UpperType;
    }
  }

  CopyMem (BootTypeOrder, NewBootTypeOrder, Count * sizeof (UINT8));
  FreePool (NewBootTypeOrder);
}

/**
 Sync boot order to boot type order

 @param[in]      UpShift                    User is moving item up (TRUE) or down (FALSE)
 @param[in]      BootDevNum                 Boot device number
 @param[in]      BootOrder                  Pointer to boot order
 @param[in, out] KernelConfig               Pointer to kernel configuration instance

 @retval EFI_SUCCESS                        Sync boot order to boot type order successfully
 @retval EFI_OUT_OF_RESOURCES               Allocate pool failed
**/
EFI_STATUS
SyncBootOrderToBootTypeOrder (
  IN     BOOLEAN                            UpShift,
  IN     UINT16                             BootDevNum,
  IN     UINT16                             *BootOrder,
  IN OUT KERNEL_CONFIGURATION               *KernelConfig
  )
{
  UINTN                                     NewBootTypeCount;
  UINT8                                     *NewBootTypeOrder;
  UINT8                                     *OldBootTypeOrder;
  UINTN                                     OldBootTypeCount;
  UINT16                                    Index;
  UINTN                                     TypeIndex;
  BBS_TYPE_TABLE                            *BbsTypeTable;
  UINT8                                     MoveType[2];
  UINT16                                    BootTypeCount;

  NewBootTypeOrder = AllocatePool (sizeof (KernelConfig->BootTypeOrder));
  if (NewBootTypeOrder == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  NewBootTypeCount = 0;
  for (Index = 0; Index < BootDevNum; Index++) {
    BbsTypeTable = GetBbsTypeTableByBootOptionNum (BootOrder[Index]);
    if (BbsTypeTable == NULL) {
      continue;
    }

    for (TypeIndex = 0; TypeIndex < NewBootTypeCount; TypeIndex++) {
      if (NewBootTypeOrder[TypeIndex] == BbsTypeTable->DeviceType) {
        break;
      }
    }
    if (TypeIndex == NewBootTypeCount) {
      NewBootTypeOrder[NewBootTypeCount++] = BbsTypeTable->DeviceType;
    }
  }

  //
  // First, find out oringial boot type order which show in Boot/Legacy/Advance.
  // Then comparing with new boot type order to find out which one boot type shift.
  //
  OldBootTypeOrder = AllocateZeroPool (sizeof (KernelConfig->BootTypeOrder));
  if (OldBootTypeOrder == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  BootTypeCount    = GetBootTypeOrderCount (KernelConfig);
  OldBootTypeCount = 0;
  for (Index = 0; Index < BootTypeCount; Index++) {
    for (TypeIndex = 0; TypeIndex < NewBootTypeCount; TypeIndex++) {
      if (KernelConfig->BootTypeOrder[Index] == NewBootTypeOrder[TypeIndex]) {
        OldBootTypeOrder[OldBootTypeCount++] = KernelConfig->BootTypeOrder[Index];
        break;
      }
    }
    if (OldBootTypeCount == NewBootTypeCount) {
      break;
    }
  }

  TypeIndex = 0;
  for (Index = 0; Index < NewBootTypeCount; Index++) {
    if (OldBootTypeOrder[Index] != NewBootTypeOrder[Index]) {
      MoveType[TypeIndex++] = OldBootTypeOrder[Index];
    }
    if (TypeIndex == 2) {
      MoveBootType (
        UpShift,
        MoveType[0],
        MoveType[1],
        KernelConfig->BootTypeOrder,
        BootTypeCount
        );
      break;
    }
  }

  FreePool (OldBootTypeOrder);
  FreePool (NewBootTypeOrder);
  return EFI_SUCCESS;
}

/**
 Sync boot order from boot type order

 @param[in]      SUCInfo                    Pointer to setup utility configuration instance
 @param[in]      KernelConfig               Pointer to kernel configuration instance
 @param[in]      BootDevNum                 Boot device number
 @param[in, out] BootOrder                  Pointer to boot order

 @retval EFI_SUCCESS                        Sync boot order from boot type order successfully
**/
EFI_STATUS
SyncBootOrderFromBootTypeOrder (
  IN     SETUP_UTILITY_CONFIGURATION        *SUCInfo,
  IN     KERNEL_CONFIGURATION               *KernelConfig,
  IN     UINT16                             BootDevNum,
  IN OUT UINT16                             *BootOrder
  )
{
  UINTN                                     Index;
  UINTN                                     Count;
  UINTN                                     DevIndex;
  UINTN                                     OrderIndex;
  BBS_TYPE_TABLE                            *BbsTypeTable;

  OrderIndex = (KernelConfig->BootNormalPriority == EFI_FIRST) ? SUCInfo->EfiBootDeviceNum : 0;
  Count      = GetBootTypeOrderCount (KernelConfig);

  for (Index = 0; Index < Count; Index++) {
    BbsTypeTable = GetBbsTypeTableByDevType (KernelConfig->BootTypeOrder[Index]);
    if (BbsTypeTable == NULL) {
      continue;
    }

    for (DevIndex = 0; DevIndex < BbsTypeTable->DevInfoCount; DevIndex++) {
 	    BootOrder[OrderIndex++] = BbsTypeTable->DevInfoList[DevIndex].BootOptionNum;
    }
  }

  return EFI_SUCCESS;
}

/**
 Process item movement by user action and update the BootOrder, BootTypeOrder if need, mBbsTypeTable if need and boot config.

 @param[in]      This                           Pointer to config access protocol instance
 @param[in]      QuestionId                     Question ID
 @param[in]      UpShift                        User is moving item up (TRUE) or down (FALSE)
 @param[in, out] KernelConfig                   Pointer to kernel configuration instance

 @retval EFI_SUCCESS                            Process item movement successfully
 @retval EFI_INVALID_PARAMETER                  Question ID is invalid
 @retval Other                                  Fail to get browser data.
**/
EFI_STATUS
Shiftitem (
  IN     CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN     EFI_QUESTION_ID                        QuestionId,
  IN     BOOLEAN                                UpShift,
  IN OUT KERNEL_CONFIGURATION                   *KernelConfig
  )
{
  CHAR16                                        *FirstUpdateStr;
  CHAR16                                        *SecondUpdateStr;
  EFI_STRING_ID                                 FirstToken;
  EFI_STRING_ID                                 SecondToken;
  UINT16                                        Temp;
  UINTN                                         Index;
  UINT16                                        BootTypeCount;
  UINT16                                        *BootOrder;
  EFI_STATUS                                    Status;
  UINTN                                         OrderIndex;
  UINTN                                         BootMenuType;
  SETUP_UTILITY_CONFIGURATION                   *SUCInfo;
  EFI_CALLBACK_INFO                             *BootCallBackInfo;
  SETUP_UTILITY_BROWSER_DATA                    *SuBrowser;
  BBS_TYPE_TABLE                                *BbsTypeTable;
  BBS_TYPE_TABLE                                *CurrentBbsTypeTable;
  UINT16                                        DeviceIndex;
  BOOLEAN                                       IsFirstItem;
  BOOLEAN                                       IsLastItem;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  BootCallBackInfo = EFI_CALLBACK_INFO_FROM_THIS (This);
  SUCInfo          = SuBrowser->SUCInfo;
  BootTypeCount    = GetBootTypeOrderCount (KernelConfig);

  if ((QuestionId >= KEY_BOOT_TYPE_ORDER_BASE) &&
      (QuestionId  < (KEY_BOOT_TYPE_ORDER_BASE + BootTypeCount))) {
    BootMenuType = LEGACY_DEVICE_TYPE_MENU;
    DeviceIndex  = QuestionId - KEY_BOOT_TYPE_ORDER_BASE;
    FirstToken   = SUCInfo->BootTypeTokenRecord + DeviceIndex;
    IsFirstItem  = (BOOLEAN) (QuestionId - KEY_BOOT_TYPE_ORDER_BASE == 0);
    IsLastItem   = (BOOLEAN) (QuestionId - KEY_BOOT_TYPE_ORDER_BASE + 1 == BootTypeCount);
    OrderIndex   = DeviceIndex;
  } else if ((QuestionId >= KEY_BOOT_DEVICE_BASE) &&
             (QuestionId  < KEY_BOOT_DEVICE_BASE + SUCInfo->AdvBootDeviceNum)) {
    BootMenuType = ADV_BOOT_MENU;
    DeviceIndex  = QuestionId - KEY_BOOT_DEVICE_BASE;
    FirstToken   = SUCInfo->AdvanceTokenRecord + DeviceIndex;
    IsFirstItem  = (BOOLEAN) (QuestionId - KEY_BOOT_DEVICE_BASE == 0);
    IsLastItem   = (BOOLEAN) (QuestionId - KEY_BOOT_DEVICE_BASE + 1 == SUCInfo->AdvBootDeviceNum);
    OrderIndex   = DeviceIndex;
  } else if ((QuestionId >= KEY_EFI_BOOT_DEVICE_BASE) &&
             (QuestionId  < KEY_EFI_BOOT_DEVICE_BASE + SUCInfo->EfiBootDeviceNum)) {
    BootMenuType = EFI_BOOT_MENU;
    DeviceIndex  = QuestionId - KEY_EFI_BOOT_DEVICE_BASE;
    FirstToken   = SUCInfo->EfiTokenRecord + DeviceIndex;
    IsFirstItem  = (BOOLEAN) (QuestionId - KEY_EFI_BOOT_DEVICE_BASE == 0);
    IsLastItem   = (BOOLEAN) (QuestionId - KEY_EFI_BOOT_DEVICE_BASE + 1 == SUCInfo->EfiBootDeviceNum);
    OrderIndex   = (KernelConfig->BootNormalPriority == LEGACY_FIRST) ? DeviceIndex + SUCInfo->LegacyBootDeviceNum : DeviceIndex;
  } else if ((QuestionId >= KEY_ADV_LEGACY_BOOT_BASE) &&
             (QuestionId  < KEY_ADV_LEGACY_BOOT_BASE + SUCInfo->LegacyBootDeviceNum)) {
    BootMenuType = LEGACY_ADV_MENU;
    DeviceIndex  = QuestionId - KEY_ADV_LEGACY_BOOT_BASE;
    FirstToken   = SUCInfo->LegacyAdvanceTokenRecord + DeviceIndex;
    IsFirstItem  = (BOOLEAN) (QuestionId - KEY_ADV_LEGACY_BOOT_BASE == 0);
    IsLastItem   = (BOOLEAN) (QuestionId - KEY_ADV_LEGACY_BOOT_BASE + 1 == SUCInfo->LegacyBootDeviceNum);
    OrderIndex   = (KernelConfig->BootNormalPriority == EFI_FIRST) ? DeviceIndex + SUCInfo->EfiBootDeviceNum : DeviceIndex;
  } else {
    for (Index = 0, BbsTypeTable = NULL; Index < mBbsTypeTableCount; Index++) {
      BbsTypeTable = &mBbsTypeTable[Index];
      if ((BbsTypeTable->KeyBootDeviceBase != 0) &&
          (QuestionId >= BbsTypeTable->KeyBootDeviceBase) &&
          (QuestionId  < BbsTypeTable->KeyBootDeviceBase + BbsTypeTable->DevInfoCount)) {
        break;
      }
    }
    if (Index == mBbsTypeTableCount) {
      return EFI_INVALID_PARAMETER;
    }

    BootMenuType = LEGACY_NORMAL_MENU;
    DeviceIndex  = QuestionId - BbsTypeTable->KeyBootDeviceBase;
    FirstToken   = SUCInfo->LegacyNormalTokenRecord[Index] + DeviceIndex;
    IsFirstItem  = (BOOLEAN) (QuestionId - BbsTypeTable->KeyBootDeviceBase == 0);
    IsLastItem   = (BOOLEAN) ((UINTN) (QuestionId - BbsTypeTable->KeyBootDeviceBase + 1) == BbsTypeTable->DevInfoCount);
    OrderIndex   = (KernelConfig->BootNormalPriority == EFI_FIRST) ? SUCInfo->EfiBootDeviceNum : 0;

    for (Index = 0; Index < BootTypeCount; Index++) {
      CurrentBbsTypeTable = GetBbsTypeTableByDevType (KernelConfig->BootTypeOrder[Index]);
      if (CurrentBbsTypeTable == NULL) {
        continue;
      }
      if (CurrentBbsTypeTable == BbsTypeTable) {
        break;
      }
      OrderIndex += CurrentBbsTypeTable->DevInfoCount;
    }
    OrderIndex += DeviceIndex;
  }

  if ((UpShift && IsFirstItem) || (!UpShift && IsLastItem)) {
    return EFI_INVALID_PARAMETER;
  }

  BootOrder = SUCInfo->BootOrder;
  if (UpShift) {
    SecondToken = FirstToken - 1;
    if (BootMenuType != LEGACY_DEVICE_TYPE_MENU) {
      Temp                      = BootOrder[OrderIndex - 1];
      BootOrder[OrderIndex - 1] = BootOrder[OrderIndex];
      BootOrder[OrderIndex]     = Temp;
    } else {
      Temp                                        = KernelConfig->BootTypeOrder[OrderIndex - 1];
      KernelConfig->BootTypeOrder[OrderIndex - 1] = KernelConfig->BootTypeOrder[OrderIndex];
      KernelConfig->BootTypeOrder[OrderIndex]     = (UINT8) Temp;
    }
  } else {
    SecondToken = FirstToken + 1;
    if (BootMenuType != LEGACY_DEVICE_TYPE_MENU) {
      Temp                      = BootOrder[OrderIndex + 1];
      BootOrder[OrderIndex + 1] = BootOrder[OrderIndex];
      BootOrder[OrderIndex]     = Temp;
    } else {
      Temp                                        = KernelConfig->BootTypeOrder[OrderIndex + 1];
      KernelConfig->BootTypeOrder[OrderIndex + 1] = KernelConfig->BootTypeOrder[OrderIndex];
      KernelConfig->BootTypeOrder[OrderIndex]     = (UINT8) Temp;
    }
  }

  SecondUpdateStr = HiiGetString (BootCallBackInfo->HiiHandle, (EFI_STRING_ID) SecondToken, NULL);
  FirstUpdateStr  = HiiGetString (BootCallBackInfo->HiiHandle, (EFI_STRING_ID) FirstToken , NULL);
  HiiSetString (BootCallBackInfo->HiiHandle, (EFI_STRING_ID) FirstToken , SecondUpdateStr, NULL);
  HiiSetString (BootCallBackInfo->HiiHandle, (EFI_STRING_ID) SecondToken, FirstUpdateStr , NULL);
  FreePool (FirstUpdateStr);
  FreePool (SecondUpdateStr);

  switch (BootMenuType) {

  case ADV_BOOT_MENU:
  case LEGACY_ADV_MENU:
    SyncBootOrderToBootTypeOrder (UpShift, SUCInfo->AdvBootDeviceNum, BootOrder, KernelConfig);

  case LEGACY_NORMAL_MENU:
    SortBbsTypeTable (BootOrder, SUCInfo->AdvBootDeviceNum);
    break;

  case LEGACY_DEVICE_TYPE_MENU:
    SyncBootOrderFromBootTypeOrder (SUCInfo, KernelConfig, SUCInfo->AdvBootDeviceNum, BootOrder);
    break;

  default :
    break;
  }

  SyncBootOrderToBootConfig ();
  return EFI_SUCCESS;
}

/**
  Check the Bootxxxx number of device is EFI device or legacy device.

  @param  NumberOfDevice         The device number in BootOrder variable (ex: Boot0001, then NumberOfDevice is 1)

  @retval TRUE                   The device is EFI device.
  @retval FALSE                  The device is legacy device.

**/
BOOLEAN
IsEfiDevice (
  IN     UINT16    NumberOfDevice
  )
{
  UINT16                    BootOptionName[20];
  UINT8                     *BootOption;
  UINT8                     *WorkingPtr;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  BOOLEAN                   IsEfiDevice;

  UnicodeSPrint (
    BootOptionName,
    sizeof (BootOptionName),
    L"Boot%04x",
    NumberOfDevice
    );

  BootOption = CommonGetVariableData (
                 BootOptionName,
                 &gEfiGlobalVariableGuid
                 );
  ASSERT (BootOption != NULL);
  if (BootOption == NULL) {
    return FALSE;
  }

  //
  // Find device path in Bootxxxx variable
  //
  WorkingPtr = BootOption;
  WorkingPtr += sizeof (UINT32);
  WorkingPtr += sizeof (UINT16);
  WorkingPtr += (UINTN) StrSize ((UINT16 *) WorkingPtr);
  DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) WorkingPtr;

  IsEfiDevice = TRUE;
  if ((BBS_DEVICE_PATH == DevicePath->Type) && (BBS_BBS_DP == DevicePath->SubType)) {
    IsEfiDevice = FALSE;
  }

  FreePool (BootOption);
  return IsEfiDevice;
}

/**
  Check if the boot option is a EFI shell boot option which is put in ROM image.

  @param  DevicePath             The device path need to be processed.

  @retval TRUE                   It is a EFI shell boot option in ROM image.
  @retval FALSE                  It is not a EFI shell boot option in ROM image.

**/
STATIC
BOOLEAN
IsEfiShellBootOption (
  IN  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL     *LastDeviceNode;
  EFI_DEVICE_PATH_PROTOCOL     *WorkDevicePath;
  BOOLEAN                      IsEfiShell;
  EFI_GUID                     *FileGuid;

  IsEfiShell = FALSE;
  WorkDevicePath = DevicePath;
  LastDeviceNode = DevicePath;

  while (!IsDevicePathEnd (WorkDevicePath)) {
    LastDeviceNode = WorkDevicePath;
    WorkDevicePath = NextDevicePathNode (WorkDevicePath);
  }
  FileGuid = EfiGetNameGuidFromFwVolDevicePathNode ((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) LastDeviceNode);
  if (FileGuid != NULL && (CompareGuid (FileGuid, PcdGetPtr(PcdShellFile)) || CompareGuid (FileGuid, PcdGetPtr(PcdMiniShellFile)))) {
    IsEfiShell = TRUE;
  }

  return IsEfiShell;
}

/**
  According to input parameter to adjust boot order to EFI device first or legacy device first.

  @param  EfiDeviceFirst         TRUE : Indicate EFI device first.
                                 FALSE: Indicate legacy device first.
  @param  DeviceCount            Total device count
  @param  BootOrder              Pointer to BootOrder.

  @retval EFI_SUCCESS            Adjust boot order successful.
  @retval EFI_INVALID_PARAMETER  Input parameter is invalid.

**/
EFI_STATUS
AdjustBootOrder (
  IN      BOOLEAN      EfiDeviceFirst,
  IN      UINTN        DeviceCount,
  IN OUT  UINT16       *BootOrder
  )
{
  UINTN               BootOrderIndex;
  UINT16              *EfiDevicOrder;
  UINTN               EfiDeviceIndex;
  UINT16              *LegacyDeviceOrder;
  UINTN               LegacyDeviceIndex;

  if (DeviceCount == 0 || BootOrder == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  EfiDevicOrder     = AllocateZeroPool (DeviceCount * sizeof (CHAR16));
  if (EfiDevicOrder == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  LegacyDeviceOrder = AllocateZeroPool (DeviceCount * sizeof (CHAR16));
  if (LegacyDeviceOrder == NULL) {
    FreePool (EfiDevicOrder);
    return EFI_OUT_OF_RESOURCES;
  }
  EfiDeviceIndex    = 0;
  LegacyDeviceIndex = 0;

  //
  // According boot type (EFI or legacy) to put boot order respective buffer
  //
  for (BootOrderIndex = 0; BootOrderIndex < DeviceCount; BootOrderIndex++) {
    if (IsEfiDevice (BootOrder[BootOrderIndex])) {
      EfiDevicOrder[EfiDeviceIndex++] = BootOrder[BootOrderIndex];
    } else {
      LegacyDeviceOrder[LegacyDeviceIndex++] = BootOrder[BootOrderIndex];
    }
  }

  //
  // Adjust boot order depend on EFI device first or legacy device first
  //
  if (EfiDeviceFirst) {
    CopyMem (BootOrder, EfiDevicOrder, EfiDeviceIndex * sizeof (CHAR16));
    CopyMem (&BootOrder[EfiDeviceIndex], LegacyDeviceOrder, LegacyDeviceIndex * sizeof (CHAR16));
  } else {
    CopyMem (BootOrder, LegacyDeviceOrder, LegacyDeviceIndex * sizeof (CHAR16));
    CopyMem (&BootOrder[LegacyDeviceIndex], EfiDevicOrder, EfiDeviceIndex * sizeof (CHAR16));
  }

  FreePool (EfiDevicOrder);
  FreePool (LegacyDeviceOrder);
  return EFI_SUCCESS;
}

/**
  According to EFI device priority to sort all of EFI devices.

  @param  DeviceCount            Total EFI device count
  @param  BootOrder              Pointer to EFI device BootOrder.

  @retval EFI_SUCCESS            Change to auto boot order policy successful.
  @retval EFI_INVALID_PARAMETER  Input parameter is invalid.
  @retval EFI_UNSUPPORTED        doesn't support auto EFI Boot option oreder algorithm

**/
EFI_STATUS
SortEfiDevices (
  IN      UINTN        DeviceCount,
  IN OUT  UINT16       *BootOrder
  )
{
  UINTN                               Index;
  UINTN                               SortIndex;
  UINT16                              BootOptionName[20];
  UINTN                               BootOptionSize;
  UINT8                               *BootOption;
  UINT8                               *WorkingPtr;
  EFI_DEVICE_PATH_PROTOCOL            *DevicePath;
  EFI_BOOT_ORDER_PRIORITY             *BootPriority;
  UINT16                              Size;
  EFI_STATUS                          Status;
  UINT16                              BootNumber;
  UINT8                               BootPriorityLevel;
  EFI_BOOT_OPTION_POLICY_PROTOCOL     *BootOptionPolicy;
  BOOLEAN                             CreateByOS;

  Status = gBS->LocateProtocol (
                &gEfiBootOptionPolicyProtocolGuid,
                NULL,
                (VOID **)  &BootOptionPolicy
                );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  BootPriority = AllocateZeroPool (DeviceCount * sizeof (EFI_BOOT_ORDER_PRIORITY));
  if (BootPriority == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  for (Index = 0; Index < DeviceCount; Index++) {
    UnicodeSPrint (
      BootOptionName,
      sizeof (BootOptionName),
      L"Boot%04x",
      BootOrder[Index]
      );
    BootOption     = NULL;
    BootOptionSize = 0;
    Status = CommonGetVariableDataAndSize (
               BootOptionName,
               &gEfiGlobalVariableGuid,
               &BootOptionSize,
               (VOID **) &BootOption
               );
    ASSERT (BootOption != NULL);
    if (BootOption == NULL) {
      return EFI_NOT_FOUND;
    }

    //
    // Find device path in Bootxxxx variable
    //
    Size = 0;
    WorkingPtr = BootOption;
    WorkingPtr += sizeof (UINT32) ;
    Size += sizeof (UINT32) + (UINT16) (*WorkingPtr);
    WorkingPtr += sizeof (UINT16) ;
    Size += sizeof (UINT16) + (UINT16) StrSize ((UINT16 *) WorkingPtr);
    WorkingPtr += StrSize ((UINT16 *) WorkingPtr);
    DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) WorkingPtr;
    WorkingPtr += GetDevicePathSize (DevicePath);
    CreateByOS = TRUE;

    if (((BootOptionSize - Size == 2) && (AsciiStrnCmp ((CHAR8 *) WorkingPtr, "RC", 2) == 0)) ||
        (IsEfiShellBootOption (DevicePath) && (BootOptionSize - Size >= 2) && (AsciiStrnCmp ((CHAR8 *) WorkingPtr, "RC", 2) == 0))) {
      CreateByOS = FALSE;
    }
    Status = BootOptionPolicy->GetEfiOptionPriority (
                                 BootOptionPolicy,
                                 CreateByOS,
                                 DevicePath,
                                 &BootPriority[Index]
                                 );
    ASSERT_EFI_ERROR (Status);
    FreePool (BootOption);
  }
  //
  // Sort EFI boot options based on EFI boot option priority policy
  //
  for (Index = 0; Index < DeviceCount; Index++) {
    for (SortIndex = Index + 1; SortIndex < DeviceCount; SortIndex++) {
      if (BootPriority[Index] > BootPriority[SortIndex]) {
        BootPriorityLevel = BootPriority[Index];
        BootPriority[Index] = BootPriority[SortIndex];
        BootPriority[SortIndex] = BootPriorityLevel;
        BootNumber = BootOrder[Index];
        BootOrder[Index] = BootOrder[SortIndex];
        BootOrder[SortIndex] = BootNumber;
      }
    }
  }

  FreePool (BootPriority);

  return EFI_SUCCESS;
}

/**
  Change boot order to suit auto boot option order policy.

  @param  DeviceCount            Total device count
  @param  BootOrder              Pointer to BootOrder.

  @retval EFI_SUCCESS            Change to auto boot order policy successful.
  @retval EFI_INVALID_PARAMETER  Input parameter is invalid.

**/
EFI_STATUS
ChangeToAutoBootOrder (
  IN      UINTN        DeviceCount,
  IN OUT  UINT16       *BootOrder
  )
{
  UINTN            EfiDeviceNum;
  UINTN            EfiStartIndex;
  UINT16           *EfiDeviceOrder;
  UINTN            Index;

  if (DeviceCount == 0 || BootOrder == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  EfiDeviceNum = 0;
  EfiStartIndex = 0;
  for (Index = 0; Index < DeviceCount; Index++) {
    if (IsEfiDevice (BootOrder[Index])) {
      if (EfiDeviceNum == 0) {
        EfiStartIndex = Index;
      }
      EfiDeviceNum++;
    }
  }

  if (EfiDeviceNum == 0) {
    return EFI_SUCCESS;
  }
  EfiDeviceOrder = AllocateZeroPool (EfiDeviceNum * sizeof (CHAR16));
  if (EfiDeviceOrder == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CopyMem (EfiDeviceOrder, &BootOrder[EfiStartIndex], EfiDeviceNum * sizeof (UINT16));
  //
  // According to EFI device priority to sort EFI devices
  //
  SortEfiDevices (EfiDeviceNum, EfiDeviceOrder);

  //
  // copy sorted EFI device order to boot order
  //
  CopyMem (&BootOrder[EfiStartIndex], EfiDeviceOrder, EfiDeviceNum * sizeof (UINT16));

  FreePool (EfiDeviceOrder);

  return EFI_SUCCESS;
}

