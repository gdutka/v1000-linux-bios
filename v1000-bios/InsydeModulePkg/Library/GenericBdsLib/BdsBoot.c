/** @file
  GenericBdsLib

;******************************************************************************
;* Copyright (c) 2012 - 2020, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  BDS Lib functions which relate with create or process the boot option.

Copyright (c) 2004 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "InternalBdsLib.h"
#include "String.h"
#include "BdsBootOption.h"
#include "BdsCheckFunctions.h"
#include "BdsDummyBootOption.h"
#include <Guid/ReturnFromImage.h>
#include <Guid/ConsoleOutDevice.h>
#include <Guid/Gpt.h>
#include <Guid/BdsDefaultBootOptions.h>

EFI_HII_HANDLE                    gBdsLibStringPackHandle = NULL;
STATIC EFI_EVENT                  mBdsServicesEvent;
H2O_BDS_SERVICES_PROTOCOL         *gBdsServices;

GENERIC_BDS_LIB_GLOBAL_DATA   *mGenericBdsLibGlobalData = NULL;
EFI_GUID  mGenericBdsLibGlobalDataProtocolGuid = {0xda8a55e4, 0x4940, 0x4af9, {0x9f, 0x36, 0x2d, 0xb4, 0x3e, 0xc2, 0x82, 0x59}};

/**
  Initialize GenericBdsLib global data.

  @retval EFI_SUCCESS               Initialize global data successful.
  @retval EFI_OUT_OF_RESOURCES      Allocate memory failed.
  @retval Other                     Install protocol failed.
**/
STATIC
EFI_STATUS
InitGenericBdsGlobalData (
  VOID
  )
{
  EFI_STATUS              Status;
  EFI_HANDLE              Handle;

  Status = gBS->LocateProtocol (
                  &mGenericBdsLibGlobalDataProtocolGuid,
                  NULL,
                  (VOID **) &mGenericBdsLibGlobalData
                  );
  if (!EFI_ERROR (Status)) {
    return Status;
  }

  mGenericBdsLibGlobalData = AllocateZeroPool (sizeof(GENERIC_BDS_LIB_GLOBAL_DATA));
  if (mGenericBdsLibGlobalData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  mGenericBdsLibGlobalData->EnumBootDevice             = FALSE;
  mGenericBdsLibGlobalData->EnableBootOrderHook        = FALSE;
  mGenericBdsLibGlobalData->UefiFastBootEnabled        = TRUE;
  mGenericBdsLibGlobalData->ResetReminderFeatureSwitch = TRUE;
  mGenericBdsLibGlobalData->ResetRequired              = FALSE;
  mGenericBdsLibGlobalData->PreviousHandlesNum         = 0;
  mGenericBdsLibGlobalData->PreviousHandles            = NULL;
  InitializeListHead (&mGenericBdsLibGlobalData->UsbShortFormInfoList);

  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &mGenericBdsLibGlobalDataProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  mGenericBdsLibGlobalData
                  );
  if (EFI_ERROR (Status)) {
    FreePool (mGenericBdsLibGlobalData);
    mGenericBdsLibGlobalData = NULL;
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Notification function for gH2OBdsServicesProtocolGuid handler Protocol

  @param[in] EFI_EVENT              Event of the notification
  @param[in] Context                not used in this function
**/
STATIC
VOID
EFIAPI
BdsServicesCallBack (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS        Status;

  Status = gBS->LocateProtocol (
                  &gH2OBdsServicesProtocolGuid,
                  NULL,
                  (VOID **) &gBdsServices
                  );
  if (Status != EFI_SUCCESS) {
    return;
  }
  gBS->CloseEvent (Event);
}

/**
  The constructor function register UNI strings into imageHandle and register event to
  locate gH2OBdsServicesProtocolGuid protocol.

  It will ASSERT() if that operation fails and it will always return EFI_SUCCESS.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.

  @retval EFI_SUCCESS   The constructor successfully added string package.
  @retval Other value   The constructor can't add string package.
**/
EFI_STATUS
EFIAPI
GenericBdsLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  VOID                                *Registration;
  EFI_STATUS                          Status;

  Status = InitGenericBdsGlobalData ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  gBdsLibStringPackHandle = HiiAddPackages (
                              &gBdsLibStringPackageGuid,
                              ImageHandle,
                              GenericBdsLibStrings,
                              NULL
                              );

  ASSERT (gBdsLibStringPackHandle != NULL);
  if (gBdsLibStringPackHandle == NULL) {
    return EFI_ABORTED;
  }

  UpdateBvdtToHii (gBdsLibStringPackHandle);

  mBdsServicesEvent = EfiCreateProtocolNotifyEvent (
                        &gH2OBdsServicesProtocolGuid,
                        TPL_CALLBACK,
                        BdsServicesCallBack,
                        NULL,
                        &Registration
                        );
  return EFI_SUCCESS;
}

/**
  The destructor function to close mBdsServicesEvent event.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.

  @retval EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
GenericBdsLibDestructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  if (mBdsServicesEvent != NULL) {
    gBS->CloseEvent (mBdsServicesEvent);
  }
  return EFI_SUCCESS;
}

/**
  Un-support the CSM Opt-out when switch disabled.

  @param  Event                The triggered event.

  @retval EFI_UNSUPPORTED
**/
EFI_STATUS
LegacyBiosDependency (
  IN EFI_EVENT            Event
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Boot the legacy system with the boot option

  @param  Option                 The legacy boot option which have BBS device path

  @retval EFI_UNSUPPORTED        There is no legacybios protocol, do not support
                                 legacy boot.
  @retval EFI_STATUS             Return the status of LegacyBios->LegacyBoot ().
**/
EFI_STATUS
BdsLibDoLegacyBoot (
  IN  BDS_COMMON_OPTION           *Option
  )
{
  EFI_STATUS                Status;
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;

  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **) &LegacyBios);
  if (EFI_ERROR (Status)) {
    //
    // If no LegacyBios protocol we do not support legacy boot
    //
    return EFI_UNSUPPORTED;
  }
  //
  // Notes: if we separate the int 19, then we don't need to refresh BBS
  //
  // to set BBS Table priority
  //
  SetBbsPriority (LegacyBios, Option);
  //
  // Write boot to OS performance data for legacy boot.
  //
  WRITE_BOOT_TO_OS_PERFORMANCE_DATA;

  DEBUG ((DEBUG_INFO | DEBUG_LOAD, "Legacy Boot: %S\n", Option->Description));
  return LegacyBios->LegacyBoot (
                      LegacyBios,
                      (BBS_BBS_DEVICE_PATH *) Option->DevicePath,
                      Option->LoadOptionsSize,
                      Option->LoadOptions
                      );
}

/**
 Create USB WWID device path node.

 @param[in] UsbIoHandle              USB Io protocol handle

 @return A Pointer to the USB WWID device path node or NULL if not found.
**/
STATIC
USB_WWID_DEVICE_PATH *
CreateUsbWwidDevPathNode (
  IN EFI_HANDLE                UsbIoHandle
  )
{
  EFI_STATUS                   Status;
  EFI_USB_IO_PROTOCOL          *UsbIo;
  EFI_USB_INTERFACE_DESCRIPTOR IfDesc;
  EFI_USB_DEVICE_DESCRIPTOR    DevDesc;
  UINT16                       *LangIdTable;
  UINT16                       TableSize;
  UINT16                       Index;
  CHAR16                       *SerialNumberStr;
  UINTN                        SerialNumberStrLen;
  USB_WWID_DEVICE_PATH         *UsbWwid;

  Status = gBS->HandleProtocol (UsbIoHandle, &gEfiUsbIoProtocolGuid, (VOID **) &UsbIo);
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  Status = UsbIo->UsbGetInterfaceDescriptor (UsbIo, &IfDesc);
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  Status = UsbIo->UsbGetDeviceDescriptor (UsbIo, &DevDesc);
  if (EFI_ERROR (Status) || DevDesc.StrSerialNumber == 0) {
    return NULL;
  }

  TableSize   = 0;
  LangIdTable = NULL;
  Status = UsbIo->UsbGetSupportedLanguages (UsbIo, &LangIdTable, &TableSize);
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  SerialNumberStr = NULL;
  for (Index = 0; Index < TableSize / sizeof (UINT16); Index++) {
    Status = UsbIo->UsbGetStringDescriptor (
                      UsbIo,
                      LangIdTable[Index],
                      DevDesc.StrSerialNumber,
                      &SerialNumberStr
                      );
    if (!EFI_ERROR (Status)) {
      break;
    }
  }
  if (SerialNumberStr == NULL) {
    return NULL;
  }

  SerialNumberStrLen = StrLen (SerialNumberStr);
  UsbWwid = (USB_WWID_DEVICE_PATH *) CreateDeviceNode (
                                       MESSAGING_DEVICE_PATH,
                                       MSG_USB_WWID_DP,
                                       (UINT16) (sizeof (USB_WWID_DEVICE_PATH) + SerialNumberStrLen * sizeof (CHAR16))
                                       );
  if (UsbWwid == NULL) {
    FreePool (SerialNumberStr);
    return NULL;
  }

  UsbWwid->InterfaceNumber = IfDesc.InterfaceNumber;
  UsbWwid->VendorId        = DevDesc.IdVendor;
  UsbWwid->ProductId       = DevDesc.IdProduct;
  CopyMem (UsbWwid + 1, SerialNumberStr, SerialNumberStrLen * sizeof (CHAR16));

  FreePool (SerialNumberStr);
  return UsbWwid;
}

/**
 Refresh USB short form device path info list.

 @retval EFI_SUCCESS                 Refresh USB short form device path info list successfully.
 @retval EFI_OUT_OF_RESOURCES        Lack of memory resource.
 @retval Others                      There is no simple file system protocol instance on system.
**/
STATIC
EFI_STATUS
RefreshUsbShortFormDevPathList (
  VOID
  )
{
  LIST_ENTRY                           *Link;
  GENERIC_BDS_LIB_USB_SHORT_FORM_INFO  *UsbShortFormInfo;
  EFI_STATUS                           Status;
  EFI_HANDLE                           *HandleList;
  UINTN                                HandleCount;
  UINTN                                Index;
  UINTN                                FileSysDevPathSize;
  EFI_DEVICE_PATH_PROTOCOL             *FileSysDevPath;
  EFI_DEVICE_PATH_PROTOCOL             *RemaindingDevPath;
  EFI_HANDLE                           UsbIoHandle;
  BOOLEAN                              Found;
  USB_WWID_DEVICE_PATH                 *UsbWwid;

  //
  // Remove nonexisting USB device from info list.
  //
  Link = GetFirstNode (&mGenericBdsLibGlobalData->UsbShortFormInfoList);
  while (!IsNull (&mGenericBdsLibGlobalData->UsbShortFormInfoList, Link)) {
    UsbShortFormInfo = (GENERIC_BDS_LIB_USB_SHORT_FORM_INFO *) Link;
    Link             = GetNextNode (&mGenericBdsLibGlobalData->UsbShortFormInfoList, Link);

    RemaindingDevPath = UsbShortFormInfo->FileSysDevPath;
    Status = gBS->LocateDevicePath (&gEfiSimpleFileSystemProtocolGuid, &RemaindingDevPath, &UsbIoHandle);
    if (EFI_ERROR (Status) || !IsDevicePathEnd (RemaindingDevPath)) {
      RemoveEntryList (&UsbShortFormInfo->Link);
      FreePool (UsbShortFormInfo->FileSysDevPath);
      FreePool (UsbShortFormInfo->UsbWwid);
      FreePool (UsbShortFormInfo);
    }
  }

  //
  // Add new USB device in info list.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleList
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (HandleList[Index], &gEfiDevicePathProtocolGuid, (VOID **) &FileSysDevPath);
    if (EFI_ERROR (Status)) {
      continue;
    }

    RemaindingDevPath = FileSysDevPath;
    Status = gBS->LocateDevicePath (&gEfiUsbIoProtocolGuid, &RemaindingDevPath, &UsbIoHandle);
    if (EFI_ERROR (Status)) {
      continue;
    }

    Found = FALSE;
    UsbShortFormInfo = NULL;
    FileSysDevPathSize = GetDevicePathSize (FileSysDevPath);
    Link = GetFirstNode (&mGenericBdsLibGlobalData->UsbShortFormInfoList);
    while (!IsNull (&mGenericBdsLibGlobalData->UsbShortFormInfoList, Link)) {
      UsbShortFormInfo = (GENERIC_BDS_LIB_USB_SHORT_FORM_INFO *) Link;
      if (CompareMem (UsbShortFormInfo->FileSysDevPath, FileSysDevPath, FileSysDevPathSize) == 0) {
        Found = TRUE;
        break;
      }

      Link = GetNextNode (&mGenericBdsLibGlobalData->UsbShortFormInfoList, Link);
    }
    if (Found) {
      continue;
    }

    UsbWwid = CreateUsbWwidDevPathNode (UsbIoHandle);
    if (UsbWwid == NULL) {
      continue;
    }

    UsbShortFormInfo = AllocateZeroPool (sizeof (GENERIC_BDS_LIB_USB_SHORT_FORM_INFO));
    if (UsbShortFormInfo == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    UsbShortFormInfo->FileSysDevPath = AllocateCopyPool (FileSysDevPathSize, FileSysDevPath);
    if (UsbShortFormInfo->FileSysDevPath == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    UsbShortFormInfo->UsbIoRemaindingDevPathOffset = (UINTN) RemaindingDevPath - (UINTN) FileSysDevPath;
    UsbShortFormInfo->UsbWwid                      = UsbWwid;
    InsertTailList (&mGenericBdsLibGlobalData->UsbShortFormInfoList, &UsbShortFormInfo->Link);
  }

  FreePool (HandleList);
  return EFI_SUCCESS;
}

/**
 Transfer input device path to USB short form device path.

 @param[in] DevPath                  Pointer to device path

 @return A Pointer to the USB short form device path or NULL if failed to transfer.
**/
STATIC
EFI_DEVICE_PATH_PROTOCOL *
GetUsbShortFormDevPath (
  IN EFI_DEVICE_PATH_PROTOCOL          *DevPath
  )
{
  LIST_ENTRY                           *Link;
  GENERIC_BDS_LIB_USB_SHORT_FORM_INFO  *UsbShortFormInfo;
  BOOLEAN                              Found;
  UINT8                                *RemaindingDevPathPtr;
  UINTN                                RemaindingDevPathSize;
  UINTN                                UsbWwidSize;
  UINT8                                *ShortformDevPathPtr;

  RefreshUsbShortFormDevPathList ();

  Found = FALSE;
  UsbShortFormInfo = NULL;
  Link = GetFirstNode (&mGenericBdsLibGlobalData->UsbShortFormInfoList);
  while (!IsNull (&mGenericBdsLibGlobalData->UsbShortFormInfoList, Link)) {
    UsbShortFormInfo = (GENERIC_BDS_LIB_USB_SHORT_FORM_INFO *) Link;
    if (CompareMem (
          DevPath,
          UsbShortFormInfo->FileSysDevPath,
          GetDevicePathSize (UsbShortFormInfo->FileSysDevPath) - sizeof(EFI_DEVICE_PATH_PROTOCOL)
          ) == 0) {
      Found = TRUE;
      break;
    }

    Link = GetNextNode (&mGenericBdsLibGlobalData->UsbShortFormInfoList, Link);
  }
  if (!Found) {
    return NULL;
  }

  RemaindingDevPathPtr  = (UINT8 *) DevPath + UsbShortFormInfo->UsbIoRemaindingDevPathOffset;
  RemaindingDevPathSize = GetDevicePathSize (DevPath) - UsbShortFormInfo->UsbIoRemaindingDevPathOffset;
  UsbWwidSize           = DevicePathNodeLength (UsbShortFormInfo->UsbWwid);
  ShortformDevPathPtr = AllocatePool (UsbWwidSize + RemaindingDevPathSize);
  if (ShortformDevPathPtr == NULL) {
    return NULL;
  }

  CopyMem (ShortformDevPathPtr              , UsbShortFormInfo->UsbWwid, UsbWwidSize);
  CopyMem (ShortformDevPathPtr + UsbWwidSize, RemaindingDevPathPtr     , RemaindingDevPathSize);
  return (EFI_DEVICE_PATH_PROTOCOL *) ShortformDevPathPtr;
}

/**
 Get the device path that matches the specified short-form device path that starts with the USB WWID device path.

 @param[in] UsbShortFormDevPath        Pointer to the USB short-form device path instance

 @return A pointer to device path which matches the specified USB short-form device path or NULL if failed to transfer.
**/
EFI_DEVICE_PATH_PROTOCOL *
EFIAPI
BdsLibExpandUsbShortFormDevPath (
  IN EFI_DEVICE_PATH_PROTOCOL          *UsbShortFormDevPath
  )
{
  LIST_ENTRY                           *Link;
  GENERIC_BDS_LIB_USB_SHORT_FORM_INFO  *UsbShortFormInfo;
  BOOLEAN                              Found;
  EFI_DEVICE_PATH_PROTOCOL             *RemaindingDevPath;
  UINTN                                RemaindingDevPathSize;
  UINTN                                UsbWwidSize;
  UINT8                                *DevPathPtr;

  if (!IS_USB_SHORT_FORM_DEVICE_PATH (UsbShortFormDevPath)) {
    return NULL;
  }

  RefreshUsbShortFormDevPathList ();

  Found = FALSE;
  UsbShortFormInfo = NULL;
  UsbWwidSize = DevicePathNodeLength (UsbShortFormDevPath);
  Link = GetFirstNode (&mGenericBdsLibGlobalData->UsbShortFormInfoList);
  while (!IsNull (&mGenericBdsLibGlobalData->UsbShortFormInfoList, Link)) {
    UsbShortFormInfo = (GENERIC_BDS_LIB_USB_SHORT_FORM_INFO *) Link;
    if (DevicePathNodeLength (UsbShortFormInfo->UsbWwid) == UsbWwidSize &&
        CompareMem (UsbShortFormInfo->UsbWwid, UsbShortFormDevPath, UsbWwidSize) == 0) {
      Found = TRUE;
      break;
    }

    Link = GetNextNode (&mGenericBdsLibGlobalData->UsbShortFormInfoList, Link);
  }
  if (!Found) {
    return NULL;
  }

  RemaindingDevPath     = NextDevicePathNode (UsbShortFormDevPath);
  RemaindingDevPathSize = GetDevicePathSize (RemaindingDevPath);
  DevPathPtr            = AllocatePool (UsbShortFormInfo->UsbIoRemaindingDevPathOffset + RemaindingDevPathSize);
  if (DevPathPtr == NULL) {
    return NULL;
  }

  CopyMem (
    DevPathPtr,
    UsbShortFormInfo->FileSysDevPath,
    UsbShortFormInfo->UsbIoRemaindingDevPathOffset
    );
  CopyMem (
    DevPathPtr + UsbShortFormInfo->UsbIoRemaindingDevPathOffset,
    RemaindingDevPath,
    RemaindingDevPathSize
    );

  return (EFI_DEVICE_PATH_PROTOCOL *) DevPathPtr;
}

/**
  Expand a device path that starts with a hard drive media device path node to be a
  full device path that includes the full hardware path to the device. We need
  to do this so it can be booted. As an optimization the front match (the part point
  to the partition node. E.g. ACPI() /PCI()/ATA()/Partition() ) is saved in a variable
  so a connect all is not required on every boot. All successful history device path
  which point to partition node (the front part) will be saved.

  @param  HardDriveDevicePath    EFI Device Path to boot, if it starts with a hard
                                 drive media device path.
  @return A Pointer to the full device path or NULL if a valid Hard Drive device path
          cannot be found.
**/
EFI_DEVICE_PATH_PROTOCOL *
EFIAPI
BdsExpandPartitionPartialDevicePathToFull (
  IN  HARDDRIVE_DEVICE_PATH      *HardDriveDevicePath
  )
{
  EFI_STATUS                Status;
  UINTN                     BlockIoHandleCount;
  EFI_HANDLE                *BlockIoBuffer;
  EFI_DEVICE_PATH_PROTOCOL  *FullDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *BlockIoDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  UINTN                     Index;
  UINTN                     InstanceNum;
  EFI_DEVICE_PATH_PROTOCOL  *CachedDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *TempNewDevicePath;
  UINTN                     CachedDevicePathSize;
  BOOLEAN                   DeviceExist;
  BOOLEAN                   NeedAdjust;
  EFI_DEVICE_PATH_PROTOCOL  *Instance;
  UINTN                     Size;

  if (!((DevicePathType (&HardDriveDevicePath->Header) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (&HardDriveDevicePath->Header) == MEDIA_HARDDRIVE_DP))) {
    return NULL;
  }

  FullDevicePath = NULL;
  CachedDevicePath = NULL;
  //
  // Check if there is prestore HD_BOOT_DEVICE_PATH_VARIABLE_NAME variable.
  // If exist, search the front path which point to partition node in the variable instants.
  // If fail to find or HD_BOOT_DEVICE_PATH_VARIABLE_NAME not exist, reconnect all and search in all system
  //
  if (FeaturePcdGet (PcdHdBootDevPathVarSupported)) {
    CachedDevicePath = BdsLibGetVariableAndSize (
                        HD_BOOT_DEVICE_PATH_VARIABLE_NAME,
                        &gHdBootDevicePathVariablGuid,
                        &CachedDevicePathSize
                        );

    if (CachedDevicePath != NULL) {
      TempNewDevicePath = CachedDevicePath;
      DeviceExist = FALSE;
      NeedAdjust = FALSE;
      do {
        //
        // Check every instance of the variable
        // First, check whether the instance contain the partition node, which is needed for distinguishing  multi
        // partial partition boot option. Second, check whether the instance could be connected.
        //
        Instance  = GetNextDevicePathInstance (&TempNewDevicePath, &Size);
        if (MatchPartitionDevicePathNode (Instance, HardDriveDevicePath)) {
          //
          // Connect the device path instance, the device path point to hard drive media device path node
          // e.g. ACPI() /PCI()/ATA()/Partition()
          //
          Status = BdsLibConnectDevicePath (Instance);
          if (!EFI_ERROR (Status)) {
            DeviceExist = TRUE;
            break;
          }
        }
        //
        // Come here means the first instance is not matched
        //
        NeedAdjust = TRUE;
        FreePool(Instance);
      } while (TempNewDevicePath != NULL);

      if (DeviceExist) {
        //
        // Find the matched device path.
        // Append the file path information from the boot option and return the fully expanded device path.
        //
        DevicePath     = NextDevicePathNode ((EFI_DEVICE_PATH_PROTOCOL *) HardDriveDevicePath);
        FullDevicePath = AppendDevicePath (Instance, DevicePath);

        //
        // Adjust the HD_BOOT_DEVICE_PATH_VARIABLE_NAME instances sequence if the matched one is not first one.
        //
        if (NeedAdjust) {
          //
          // First delete the matched instance.
          //
          TempNewDevicePath = CachedDevicePath;
          CachedDevicePath  = BdsLibDelPartMatchInstance (CachedDevicePath, Instance );
          FreePool (TempNewDevicePath);

          //
          // Second, append the remaining path after the matched instance
          //
          TempNewDevicePath = CachedDevicePath;
          CachedDevicePath = AppendDevicePathInstance (Instance, CachedDevicePath );
          FreePool (TempNewDevicePath);
          //
          // Save the matching Device Path so we don't need to do a connect all next time
          //
          Status = gRT->SetVariable (
                          HD_BOOT_DEVICE_PATH_VARIABLE_NAME,
                          &gHdBootDevicePathVariablGuid,
                          EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                          GetDevicePathSize (CachedDevicePath),
                          CachedDevicePath
                          );
        }

        FreePool (Instance);
        FreePool (CachedDevicePath);
        return FullDevicePath;
      }
    }

    //
    // If we get here we fail to find or HD_BOOT_DEVICE_PATH_VARIABLE_NAME not exist, and now we need
    // to search all devices in the system for a matched partition
    //
    BdsLibConnectAllDriversToAllControllers ();
  }

  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiBlockIoProtocolGuid, NULL, &BlockIoHandleCount, &BlockIoBuffer);
  if (EFI_ERROR (Status) || BlockIoHandleCount == 0 || BlockIoBuffer == NULL) {
    //
    // If there was an error or there are no device handles that support
    // the BLOCK_IO Protocol, then return.
    //
    return NULL;
  }
  //
  // Loop through all the device handles that support the BLOCK_IO Protocol
  //
  for (Index = 0; Index < BlockIoHandleCount; Index++) {

    Status = gBS->HandleProtocol (BlockIoBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID *) &BlockIoDevicePath);
    if (EFI_ERROR (Status) || BlockIoDevicePath == NULL) {
      continue;
    }

    if (MatchPartitionDevicePathNode (BlockIoDevicePath, HardDriveDevicePath)) {
      //
      // Find the matched partition device path
      //
      DevicePath    = NextDevicePathNode ((EFI_DEVICE_PATH_PROTOCOL *) HardDriveDevicePath);
      FullDevicePath = AppendDevicePath (BlockIoDevicePath, DevicePath);

      if (FeaturePcdGet (PcdHdBootDevPathVarSupported)) {
        //
        // Save the matched partition device path in HD_BOOT_DEVICE_PATH_VARIABLE_NAME variable
        //
        if (CachedDevicePath != NULL) {
          //
          // Save the matched partition device path as first instance of HD_BOOT_DEVICE_PATH_VARIABLE_NAME variable
          //
          if (BdsLibMatchDevicePaths (CachedDevicePath, BlockIoDevicePath)) {
            TempNewDevicePath = CachedDevicePath;
            CachedDevicePath = BdsLibDelPartMatchInstance (CachedDevicePath, BlockIoDevicePath);
            FreePool(TempNewDevicePath);

            TempNewDevicePath = CachedDevicePath;
            CachedDevicePath = AppendDevicePathInstance (BlockIoDevicePath, CachedDevicePath);
            if (TempNewDevicePath != NULL) {
              FreePool(TempNewDevicePath);
            }
          } else {
            TempNewDevicePath = CachedDevicePath;
            CachedDevicePath = AppendDevicePathInstance (BlockIoDevicePath, CachedDevicePath);
            FreePool(TempNewDevicePath);
          }
          //
          // Here limit the device path instance number to 12, which is max number for a system support 3 IDE controller
          // If the user try to boot many OS in different HDs or partitions, in theory,
          // the HD_BOOT_DEVICE_PATH_VARIABLE_NAME variable maybe become larger and larger.
          //
          InstanceNum = 0;
          ASSERT (CachedDevicePath != NULL);
          if (CachedDevicePath == NULL) {
            break;
          }
          TempNewDevicePath = CachedDevicePath;
          while (!IsDevicePathEnd (TempNewDevicePath)) {
            TempNewDevicePath = NextDevicePathNode (TempNewDevicePath);
            //
            // Parse one instance
            //
            while (!IsDevicePathEndType (TempNewDevicePath)) {
              TempNewDevicePath = NextDevicePathNode (TempNewDevicePath);
            }
            InstanceNum++;
            //
            // If the CachedDevicePath variable contain too much instance, only remain 12 instances.
            //
            if (InstanceNum >= 12) {
              SetDevicePathEndNode (TempNewDevicePath);
              break;
            }
          }
        } else {
          CachedDevicePath = DuplicateDevicePath (BlockIoDevicePath);
        }

        //
        // Save the matching Device Path so we don't need to do a connect all next time
        //
        Status = gRT->SetVariable (
                        HD_BOOT_DEVICE_PATH_VARIABLE_NAME,
                        &gHdBootDevicePathVariablGuid,
                        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                        GetDevicePathSize (CachedDevicePath),
                        CachedDevicePath
                        );
      }
      break;
    }
  }

  if (CachedDevicePath != NULL) {
    FreePool (CachedDevicePath);
  }
  if (BlockIoBuffer != NULL) {
    FreePool (BlockIoBuffer);
  }
  return FullDevicePath;
}

/**
  Check whether there is a instance in BlockIoDevicePath, which contain multi device path
  instances, has the same partition node with HardDriveDevicePath device path

  @param  BlockIoDevicePath      Multi device path instances which need to check
  @param  HardDriveDevicePath    A device path which starts with a hard drive media
                                 device path.

  @retval TRUE                   There is a matched device path instance.
  @retval FALSE                  There is no matched device path instance.
**/
BOOLEAN
EFIAPI
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
  Delete the boot option that content was duplicated.

**/
VOID
BdsLibDeleteRedundantOption (
  )
{
  UINT16                    *BootOrder;
  UINT16                    *TmpBootOrder;
  UINTN                     BootOrderSize;
  UINTN                     SearchIndex;
  UINTN                     Index;
  UINTN                     BootOptionCnt;
  UINT16                    BootIndex[BOOT_OPTION_MAX_CHAR];
  UINT8                     **BootOptionPool;
  UINTN                     *BootOptionSizePool;
  BOOLEAN                   BootOrderChanged;
  BOOLEAN                   SameBootOption;

  BootOrder          = NULL;
  TmpBootOrder       = NULL;
  BootOptionPool     = NULL;
  BootOptionSizePool = NULL;
  BootOrderChanged   = FALSE;
  BootOptionCnt      = 0;
  BootOrderSize      = 0;
  //
  // Get all boot option
  //
  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  TmpBootOrder = AllocateZeroPool (BootOrderSize);
  if (NULL == BootOrder || TmpBootOrder == NULL) {
    goto EXIT;
  }
  CopyMem (TmpBootOrder, BootOrder, BootOrderSize);
  BootOptionCnt      = BootOrderSize / sizeof (UINT16);
  if (BootOptionCnt == 1) {
    goto EXIT;
  }

  BootOptionSizePool = AllocateZeroPool (BootOptionCnt * sizeof (UINTN));
  BootOptionPool     = AllocateZeroPool (BootOptionCnt * sizeof (UINT8 *));
  if (BootOptionSizePool == NULL || BootOptionPool == NULL) {
    goto EXIT;
  }
  Index = 0;
  while (Index < BootOptionCnt) {
    UnicodeSPrint (BootIndex, sizeof (BootIndex), L"Boot%04x", BootOrder[Index]);
    BootOptionPool[Index] = BdsLibGetVariableAndSize (
                              BootIndex,
                              &gEfiGlobalVariableGuid,
                              (BootOptionSizePool + Index)
                              );
    if (NULL == BootOptionPool[Index]) {
      goto EXIT;
    }
    Index++;
  }

  //
  // Scan and delete redundant option
  //
  for (Index = 0; Index < BootOptionCnt; Index++) {
    if (BootOptionSizePool[Index] == 0) {
      continue;
    }
    for (SearchIndex = Index + 1; SearchIndex < BootOptionCnt; SearchIndex++) {
      SameBootOption = IsSameBootOption (
                         BootOptionPool[Index],
                         BootOptionSizePool[Index],
                         BootOptionPool[SearchIndex],
                         BootOptionSizePool[SearchIndex]
                         );
      if (SameBootOption) {
        BootOrderChanged = TRUE;
        BdsLibDeleteBootOption (TmpBootOrder[SearchIndex], BootOrder, &BootOrderSize);
        BootOptionSizePool[SearchIndex] = 0;
      }
    }
  }

  //
  // Restore BootOrder if need.
  //
  if (BootOrderChanged) {
    gRT->SetVariable (
           L"BootOrder",
           &gEfiGlobalVariableGuid,
           EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
           BootOrderSize,
           BootOrder
           );
  }

EXIT:
  if (TmpBootOrder != NULL) {
    FreePool (TmpBootOrder);
  }
  if (BootOptionSizePool != NULL) {
    FreePool (BootOptionSizePool);
  }
  if (BootOptionPool != NULL) {
    for (Index = 0; Index < BootOptionCnt; Index++) {
      if (BootOptionPool[Index] != NULL) {
        FreePool (BootOptionPool[Index]);
      }
    }
    FreePool (BootOptionPool);
  }
  if (BootOrder != NULL) {
    FreePool (BootOrder);
  }
}

/**
  Delete the boot option associated with the handle passed in.

  @param  Handle                 The handle which present the device path to create
                                 boot option

  @retval EFI_SUCCESS            Delete the boot option success
  @retval EFI_NOT_FOUND          If the Device Path is not found in the system
  @retval EFI_OUT_OF_RESOURCES   Lack of memory resource
  @retval Other                  Error return value from SetVariable()
**/
EFI_STATUS
BdsLibDeleteOptionFromHandle (
  IN  EFI_HANDLE                 Handle
  )
{
  UINT16                    *BootOrder;
  UINT8                     *BootOptionVar;
  UINTN                     BootOrderSize;
  UINTN                     BootOptionSize;
  EFI_STATUS                Status;
  UINTN                     Index;
  UINT16                    BootOption[BOOT_OPTION_MAX_CHAR];
  UINTN                     DevicePathSize;
  UINTN                     OptionDevicePathSize;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *OptionDevicePath;
  UINT8                     *TempPtr;
  EFI_DEVICE_PATH_PROTOCOL  *ExpandedDevPath;
  BOOLEAN                   Found;

  Status        = EFI_SUCCESS;
  BootOrder     = NULL;
  BootOrderSize = 0;

  //
  // Check "BootOrder" variable, if no, means there is no any boot order.
  //
  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (BootOrder == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Convert device handle to device path protocol instance
  //
  DevicePath = DevicePathFromHandle (Handle);
  if (DevicePath == NULL) {
    return EFI_NOT_FOUND;
  }
  DevicePathSize = GetDevicePathSize (DevicePath);

  //
  // Loop all boot order variable and find the matching device path
  //
  Found = FALSE;
  Index = 0;
  while (Index < BootOrderSize / sizeof (UINT16)) {
    UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[Index]);
    BootOptionVar = BdsLibGetVariableAndSize (
                      BootOption,
                      &gEfiGlobalVariableGuid,
                      &BootOptionSize
                      );

    if (BootOptionVar == NULL) {
      FreePool (BootOrder);
      return EFI_OUT_OF_RESOURCES;
    }

    TempPtr = BootOptionVar;
    TempPtr += sizeof (UINT32) + sizeof (UINT16);
    TempPtr += StrSize ((CHAR16 *) TempPtr);
    OptionDevicePath = (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;
    ExpandedDevPath = (IS_USB_SHORT_FORM_DEVICE_PATH (OptionDevicePath)) ? BdsLibExpandUsbShortFormDevPath (OptionDevicePath) : NULL;
    if (ExpandedDevPath != NULL) {
      OptionDevicePath = ExpandedDevPath;
    }
    OptionDevicePathSize = GetDevicePathSize (OptionDevicePath);

    //
    // Check whether the device path match
    //
    if ((OptionDevicePathSize == DevicePathSize) &&
        (CompareMem (DevicePath, OptionDevicePath, DevicePathSize) == 0)) {
      Found = TRUE;
    }

    FreePool (BootOptionVar);
    if (ExpandedDevPath != NULL) {
      FreePool (ExpandedDevPath);
    }

    if (Found) {
      BdsLibDeleteBootOption (BootOrder[Index], BootOrder, &BootOrderSize);
      break;
    }

    Index++;
  }

  //
  // Adjust number of boot option for "BootOrder" variable.
  //
  Status = gRT->SetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  BootOrderSize,
                  BootOrder
                  );

  FreePool (BootOrder);

  return Status;
}

/**
  According to input parameter to adjust boot order to EFI device first or
  legacy device first.

  @param  EfiDeviceFirst       TRUE : Indicate EFI device first.
                               FALSE: Indicate legacy device first.
  @param  DeviceCount          Total device count
  @param  BootOrder            Pointer to BootOrder.

  @retval EFI_SUCCESS           Adjust boot order successful.
  @retval EFI_INVALID_PARAMETER Input parameter is invalid.
**/
EFI_STATUS
AdjustBootOrder (
  IN      BOOLEAN      EfiDeviceFirst,
  IN      UINTN        DeviceCount,
  IN OUT  UINT16       *BootOrder
  )
{
  UINTN               BootOrderIndex;
  UINT16              *EfiDeviceOrder;
  UINTN               EfiDeviceIndex;
  UINT16              *LegacyDeviceOrder;
  UINTN               LegacyDeviceIndex;

  if (DeviceCount == 0 || BootOrder == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  EfiDeviceOrder = AllocateZeroPool (DeviceCount * sizeof (CHAR16));
  if (EfiDeviceOrder == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  LegacyDeviceOrder = AllocateZeroPool (DeviceCount * sizeof (CHAR16));
  if (LegacyDeviceOrder == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  EfiDeviceIndex = 0;
  LegacyDeviceIndex = 0;

  //
  // According boot type (EFI or legacy) to put boot order respective buffer
  //
  for (BootOrderIndex = 0; BootOrderIndex < DeviceCount; BootOrderIndex++) {
    if (IsEfiDevice (BootOrder[BootOrderIndex])) {
      EfiDeviceOrder[EfiDeviceIndex++] = BootOrder[BootOrderIndex];
    } else {
      LegacyDeviceOrder[LegacyDeviceIndex++] = BootOrder[BootOrderIndex];
    }
  }

  //
  // Adjust boot order depend on EFI device first or legacy device first
  //
  if (EfiDeviceFirst) {
    CopyMem (BootOrder, EfiDeviceOrder, EfiDeviceIndex * sizeof (CHAR16));
    CopyMem (&BootOrder[EfiDeviceIndex], LegacyDeviceOrder, LegacyDeviceIndex * sizeof (CHAR16));
  } else {
    CopyMem (BootOrder, LegacyDeviceOrder, LegacyDeviceIndex * sizeof (CHAR16));
    CopyMem (&BootOrder[LegacyDeviceIndex], EfiDeviceOrder, EfiDeviceIndex * sizeof (CHAR16));
  }

  FreePool (EfiDeviceOrder);
  FreePool (LegacyDeviceOrder);
  return EFI_SUCCESS;
}

/**
  Based on the input oprom storage device table, check if the device path belongs to oprom storage device.

  @param  DevicePath           The list of device patch
  @param  OpromStorageDev      Pointer of array which contains oprom storage device information
  @param  OpromStorageDevCount Number of oprom storage device

  @retval TRUE                 Device path belongs to oprom storage device.
  @retval FALSE                otherwise
**/
BOOLEAN
IsOpromStorageDev (
  IN EFI_DEVICE_PATH_PROTOCOL      *DevicePath,
  IN OPROM_STORAGE_DEVICE_INFO     *OpromStorageDev,
  IN UINTN                         OpromStorageDevCount
  )
{
  EFI_STATUS                   Status;
  EFI_HANDLE                   PciIoHandle;
  EFI_PCI_IO_PROTOCOL          *PciIo;
  UINTN                        Segment;
  UINTN                        Bus;
  UINTN                        Device;
  UINTN                        Function;
  UINTN                        Index;

  if (OpromStorageDev == NULL || OpromStorageDevCount == 0) {
    return FALSE;
  }

  Status = gBS->LocateDevicePath (
                  &gEfiPciIoProtocolGuid,
                  &DevicePath,
                  &PciIoHandle
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  Status = gBS->HandleProtocol (
                  PciIoHandle,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  Status = PciIo->GetLocation (PciIo, &Segment, &Bus, &Device, &Function);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  for (Index = 0; Index < OpromStorageDevCount; Index++) {
    if (Segment  == OpromStorageDev[Index].Segment &&
        Bus      == OpromStorageDev[Index].Bus &&
        Device   == OpromStorageDev[Index].Device &&
        Function == OpromStorageDev[Index].Function) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Collect option ROM storage device information.

  @param  OpromStorageDev      Pointer of array which contains oprom storage device information
  @param  OpromStorageDevCount Number of oprom storage device

  @retval EFI_SUCCESS             Get Oprom storage device information successfully.
  @retval EFI_INVALID_PARAMETER   Input parameter is NULL.
  @retval Other                   LocateHandleBuffer or AllocatePool fail.
**/
EFI_STATUS
BdsLibGetOpromStorageDevInfo (
  OUT OPROM_STORAGE_DEVICE_INFO **OpromStorageDev,
  OUT UINTN                     *OpromStorageDevCount
  )
{
  EFI_STATUS                       Status;
  UINTN                            NumberPciIoHandles;
  EFI_HANDLE                       *PciIoHandles;
  EFI_PCI_IO_PROTOCOL              *PciIo;
  UINT8                            Class;
  UINTN                            Index;
  EFI_PCI_IO_PROTOCOL              *OpromStorageDevTable[MAX_OPTION_ROM_STORAGE_DEVICE];

  if (OpromStorageDev == NULL || OpromStorageDevCount == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *OpromStorageDev = NULL;
  *OpromStorageDevCount = 0;
  NumberPciIoHandles = 0;
  PciIoHandles = NULL;

  //
  // Find all storage devices which are controlled by option ROM.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &NumberPciIoHandles,
                  &PciIoHandles
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  for (Index = 0; Index < NumberPciIoHandles; Index++) {
    Status = gBS->HandleProtocol (PciIoHandles[Index], &gEfiPciIoProtocolGuid, (VOID **) &PciIo);
    if (EFI_ERROR (Status) || PciIo->RomSize == 0) {
      continue;
    }

    Status = PciIo->Pci.Read (
                          PciIo,
                          EfiPciWidthUint8,
                          PCI_CLASSCODE_OFFSET + 2,
                          1,
                          &Class
                          );
    if (EFI_ERROR (Status) || Class != PCI_CLASS_MASS_STORAGE) {
      continue;
    }

    if (*OpromStorageDevCount == MAX_OPTION_ROM_STORAGE_DEVICE) {
      DEBUG((EFI_D_ERROR, "WARNING: Unable to get all option ROM storage device information!\n"));
      break;
    }

    OpromStorageDevTable[*OpromStorageDevCount] = PciIo;
    (*OpromStorageDevCount)++;
  }

  if (NumberPciIoHandles) {
    FreePool (PciIoHandles);
  }

  //
  // From the table of oprom storage device, get info one by one.
  //
  if (*OpromStorageDevCount) {
    *OpromStorageDev = AllocatePool (*OpromStorageDevCount * sizeof (OPROM_STORAGE_DEVICE_INFO));
    if (*OpromStorageDev == NULL) {
      *OpromStorageDevCount = 0;
      return EFI_OUT_OF_RESOURCES;
    }

    for (Index = 0; Index < *OpromStorageDevCount; Index++) {
      PciIo = OpromStorageDevTable[Index];
      PciIo->GetLocation (
               PciIo,
               &((*OpromStorageDev)[Index].Segment),
               &((*OpromStorageDev)[Index].Bus),
               &((*OpromStorageDev)[Index].Device),
               &((*OpromStorageDev)[Index].Function)
               );
    }
  }

  return EFI_SUCCESS;
}

/**
  According input boot order index to delete BIOS created boot option which partition device path is same
  as input boot option. If input boot option is created by BIOS, this function will delete input boot option
  directly.

  @param[in]      BootOrderIndex      Input boot order index.
  @param[in]      PositionPolicy      The new created boot order policy.
  @param[in, out] BootOrder           Boot order list.
  @param[in, out] BootOrderSize       Boot order size by byte.
  @param[in]      BiosCreatedBootNum  The boot option number which is created by BIOS.

  @retval EFI_SUCCESS                 BIOS created boot option is deleted and the delete boot order index is returned.
  @retval EFI_INVALID_PARAMETER       BootOrder is NULL or BiosCreatedIndex is NULL or corresponding Boot#### is incorrect.
  @retval EFI_NOT_FOUND               Cannot find BIOS created boot option.
  @retval EFI_OUT_OF_RESOURCES        Lack of memory resource.
**/
EFI_STATUS
DeleteBiosCreateOption (
  IN      UINTN       BootOrderIndex,
  IN      UINT16      PositionPolicy,
  IN OUT  UINT16      *BootOrder,
  IN OUT  UINTN       *BootOrderSize,
  OUT     UINT16       *BiosCreatedBootNum
  )
{
  UINT16                    BootOption[BOOT_OPTION_MAX_CHAR];
  UINT8                     *BootOptionVar;
  UINTN                     VariableSize;
  UINT16                    BootOrderNum;
  UINT16                    Index;
  EFI_DEVICE_PATH_PROTOCOL  *OptionDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *WorkingDevicePath;
  BOOLEAN                   BiosCreatedFound;
  UINTN                     PcdTokenNum;

  if (BootOrder == NULL || BootOrderSize == NULL || BiosCreatedBootNum == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[BootOrderIndex]);
  BootOptionVar = BdsLibGetVariableAndSize (
                    BootOption,
                    &gEfiGlobalVariableGuid,
                    &VariableSize
                    );
  if (BootOptionVar == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  BiosCreatedFound = FALSE;
  OptionDevicePath = NULL;
  if (BdsLibIsBiosCreatedOption (BootOptionVar, VariableSize)) {
    FreePool (BootOptionVar);
    *BiosCreatedBootNum = BootOrder[BootOrderIndex];
    BdsLibDeleteBootOption (BootOrder[BootOrderIndex], BootOrder, BootOrderSize);
    BiosCreatedFound = TRUE;
  } else {
    FreePool (BootOptionVar);
    OptionDevicePath = BdsLibGetDevicePathFromBootOption (BootOrder[BootOrderIndex]);
    if (OptionDevicePath == NULL) {
      return EFI_INVALID_PARAMETER;
    }

    PcdTokenNum  = GetGenericOsPcdByFilePath (OptionDevicePath);
    BootOrderNum = (UINT16) (*BootOrderSize / sizeof (UINT16));
    for (Index = 0; Index < BootOrderNum; Index++) {
      if (Index == BootOrderIndex) {
        continue;
      }
      UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[Index]);
      BootOptionVar = BdsLibGetVariableAndSize (
                        BootOption,
                        &gEfiGlobalVariableGuid,
                        &VariableSize
                        );
      if (BootOptionVar == NULL || !BdsLibIsBiosCreatedOption (BootOptionVar, VariableSize)) {
        if (BootOptionVar != NULL) {
          FreePool (BootOptionVar);
        }
        continue;
      }

      WorkingDevicePath = BdsLibGetDevicePathFromBootOption (BootOrder[Index]);
      if (WorkingDevicePath == NULL) {
         FreePool (BootOptionVar);
         continue;
      }
      if (MatchPartitionDevicePathNode (OptionDevicePath, (HARDDRIVE_DEVICE_PATH *) WorkingDevicePath) &&
          (BdsLibMatchFilePathDevicePathNode (OptionDevicePath, WorkingDevicePath) ||
           IsFilePathInGenericOsPcd (PcdTokenNum, WorkingDevicePath))) {
        BiosCreatedFound = TRUE;
        *BiosCreatedBootNum = BootOrder[Index];
      }

      FreePool (BootOptionVar);
      FreePool (WorkingDevicePath);
      if (BiosCreatedFound) {
        UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[Index]);
        gRT->SetVariable (
               BootOption,
               &gEfiGlobalVariableGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
               0,
               NULL
               );
        if (PositionPolicy == IN_AUTO) {
          //
          // Keep OS created EFI Boot option priority, if NewPositionPolicy is auto
          //
          CopyMem (
            &BootOrder[Index],
            BootOrder + Index + 1,
            *BootOrderSize - (Index + 1) * sizeof (UINT16)
            );
        } else {
          //
          // Keep recovery EFI Boot option priority, if NewPositionPolicy isn't auto
          //
          BootOrder[Index] = BootOrder[BootOrderIndex];
          CopyMem (
            &BootOrder[BootOrderIndex],
            BootOrder + BootOrderIndex + 1,
            *BootOrderSize - (BootOrderIndex + 1) * sizeof (UINT16)
            );
        }
         *BootOrderSize -= sizeof (UINT16);
        break;
      }
    }
  }

  if (OptionDevicePath != NULL) {
    FreePool (OptionDevicePath);
  }

  return BiosCreatedFound ? EFI_SUCCESS : EFI_NOT_FOUND;
}

/**
  Delete all invalid EFI boot options.

  @retval EFI_SUCCESS            Delete all invalid boot option success
  @retval EFI_NOT_FOUND          Variable "BootOrder" is not found
  @retval EFI_OUT_OF_RESOURCES   Lack of memory resource
  @retval Other                  Error return value from SetVariable()
**/
EFI_STATUS
BdsDeleteAllInvalidEfiBootOption (
  OUT BOOLEAN                   *WindowsToGoBootVarExist
  )
{
  UINT16                        *BootOrder;
  UINT8                         *BootOptionVar;
  UINTN                         BootOrderSize;
  UINTN                         BootOptionSize;
  EFI_STATUS                    Status;
  UINTN                         Index;
  UINT16                        BootOption[BOOT_OPTION_MAX_CHAR];
  EFI_DEVICE_PATH_PROTOCOL      *OptionDevicePath;
  UINT8                         *TempPtr;
  CHAR16                        *Description;
  BOOLEAN                       NeedDelete;
  EFI_DEVICE_PATH_PROTOCOL      *NewDevicePath;
  UINT16                        BiosCreatedBootNum;
  KERNEL_CONFIGURATION          SystemConfiguration;
  EFI_STATUS                    SystemConfigStatus;
  EFI_FILE_HANDLE               FileHandle;
  UINTN                         Size;
  OPROM_STORAGE_DEVICE_INFO     *OpromStorageDev;
  UINTN                         OpromStorageDevCount;
  UINT8                         *DisableOpromStorageDevBoot;
  UINTN                         VariableSize;
  UINTN                         OptionDevicePathSize;
  UINT8                         *OptionalData;
  UINT32                        HashValue;
  BOOLEAN                       IsAppOption;
  UINTN                         BootType;
  BOOLEAN                       IsUsbBootSupported;
  BOOLEAN                       IsNetworkBootSupported;
  BOOLEAN                       IsWindowsToGoBootOption;
  EFI_DEVICE_PATH_PROTOCOL      *ExpandedDevPath;

  *WindowsToGoBootVarExist = FALSE;

  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (NULL == BootOrder) {
    return EFI_NOT_FOUND;
  }

  OpromStorageDev = NULL;
  OpromStorageDevCount = 0;
  DisableOpromStorageDevBoot = BdsLibGetVariableAndSize (
                                 L"DisableOpromStorageDevBoot",
                                 &gEfiGenericVariableGuid,
                                 &Size
                                 );
  if (DisableOpromStorageDevBoot != NULL) {
    BdsLibGetOpromStorageDevInfo (&OpromStorageDev, &OpromStorageDevCount);
  }

  SystemConfigStatus     = GetKernelConfiguration (&SystemConfiguration);
  BootType               = BdsLibGetBootType ();
  IsUsbBootSupported     = (BOOLEAN) (EFI_ERROR (SystemConfigStatus) || SystemConfiguration.UsbBoot == 0);
  IsNetworkBootSupported = (BOOLEAN) (!EFI_ERROR (SystemConfigStatus) && SystemConfiguration.PxeBootToLan != 0);

  Index = 0;
  while (Index < BootOrderSize / sizeof (UINT16)) {
    if (FeaturePcdGet (PcdAutoCreateDummyBootOption) && BdsLibIsDummyBootOption (BootOrder[Index])) {
      Index++;
      continue;
    }
    UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[Index]);
    BootOptionVar = BdsLibGetVariableAndSize (
                      BootOption,
                      &gEfiGlobalVariableGuid,
                      &VariableSize
                      );
    if (NULL == BootOptionVar) {
      //
      //If the Boot Device is not exit, we should dynamically adjust the BootOrder
      //
      BdsLibUpdateInvalidBootOrder (&BootOrder, Index, &BootOrderSize);
      if (BootOrder == NULL) {
        break;
      }
      continue;
    }

    if (!ValidateOption (BootOptionVar, VariableSize)) {
      BdsLibDeleteBootOption (BootOrder[Index], BootOrder, &BootOrderSize);
      FreePool (BootOptionVar);
      continue;
    }

    TempPtr = BootOptionVar;
    IsAppOption = (BOOLEAN) ((*(UINT32 *) BootOptionVar & LOAD_OPTION_CATEGORY_APP) == LOAD_OPTION_CATEGORY_APP);
    TempPtr += sizeof (UINT32) + sizeof (UINT16);
    Description = (CHAR16 *) TempPtr;
    TempPtr += StrSize ((CHAR16 *) TempPtr);
    OptionDevicePath = (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;
    OptionDevicePathSize = GetDevicePathSize (OptionDevicePath);
    BootOptionSize = (UINTN) (TempPtr - BootOptionVar) + OptionDevicePathSize;
    OptionalData = BootOptionVar + BootOptionSize;

    ExpandedDevPath = IS_USB_SHORT_FORM_DEVICE_PATH(OptionDevicePath) ? BdsLibExpandUsbShortFormDevPath (OptionDevicePath) : NULL;
    if (ExpandedDevPath != NULL) {
      OptionDevicePath = ExpandedDevPath;
    }
    IsWindowsToGoBootOption = IsWindowsToGoBootOptionDevPath (OptionDevicePath);

    if (IsLegacyBootOptionDevPath (OptionDevicePath)) {
      //
      // Skip legacy boot option (BBS boot device)
      //
      NeedDelete = FALSE;
    } else if (BootType == LEGACY_BOOT_TYPE && !IsAppOption) {
      //
      // SCU disable EFI Boot
      //
      NeedDelete = TRUE;
    } else if (IsValidHwVendorBootOptionDevPath (OptionDevicePath)) {
      NeedDelete = FALSE;
    } else if (IsWindowsToGoBootOption) {
      NeedDelete = FALSE;
    } else if (!IsUsbBootSupported && IsUsbDevicePath (OptionDevicePath)) {
      NeedDelete = TRUE;
    } else if (IsFvFileBootOptionDevPath (OptionDevicePath)) {
      //
      // Skip boot option for FV file if read file successfully.
      //
      NeedDelete = TRUE;
      if (IsValidFvFileBootOptionDevPath (OptionDevicePath)) {
        NeedDelete = FALSE;
      } else if (VariableSize - BootOptionSize == SHELL_OPTIONAL_DATA_SIZE) {
        gBS->CopyMem (&HashValue, &OptionalData[2], 4);
        Status = UpdateShellDevicePath ((CHAR16 *) BootOption, *((UINT32 *) BootOptionVar), Description, HashValue);
        if (!EFI_ERROR (Status)) {
          NeedDelete = FALSE;
        }
      }
    } else if (IsGenericUefiBootOsDevPath (OptionDevicePath)) {
      NewDevicePath = BdsExpandPartitionPartialDevicePathToFull ((HARDDRIVE_DEVICE_PATH *) OptionDevicePath);
      if (NewDevicePath == NULL) {
        NewDevicePath = OptionDevicePath;
      }

      if (IsOpromStorageDev (NewDevicePath, OpromStorageDev, OpromStorageDevCount)) {
        FileHandle = NULL;
      } else {
        Status = BdsLibOpenFileFromDevicePath (
                   NewDevicePath,
                   EFI_FILE_MODE_READ,
                   0,
                   &FileHandle
                   );
        if (EFI_ERROR (Status)) {
          FileHandle = NULL;
        }
      }
      if (NewDevicePath != OptionDevicePath) {
        FreePool (NewDevicePath);
      }

      NeedDelete = TRUE;
      if (FileHandle != NULL) {
        NeedDelete = FALSE;
        FileHandle->Close (FileHandle);
        if (!BdsLibIsBiosCreatedOption (BootOptionVar, VariableSize)) {
          Status = DeleteBiosCreateOption (Index, SystemConfiguration.NewPositionPolicy, BootOrder, &BootOrderSize, &BiosCreatedBootNum);
          if (!EFI_ERROR (Status)) {
            if (ReadCmos8 (LastBootDevice) == (UINT8) BiosCreatedBootNum) {
              WriteCmos8 (LastBootDevice, (UINT8) BootOrder[Index]);
            }
            FreePool (BootOptionVar);
            continue;
          }
        }
      }
    } else if (IsValidRemovableBootOptionDevPath (OptionDevicePath) &&
               !IsOpromStorageDev (OptionDevicePath, OpromStorageDev, OpromStorageDevCount)) {
      NeedDelete = FALSE;
    } else if (IsNetworkBootSupported && IsValidNetworkBootOption (&SystemConfiguration, OptionDevicePath)) {
      NeedDelete = FALSE;
    } else {
      NeedDelete = TRUE;
    }

    if (FeaturePcdGet (PcdH2OBdsCpBootDeviceEnumCheckBootOptionSupported)) {
      TriggerCpBootDeviceEnumCheckBootOption (BootOption, &gEfiGlobalVariableGuid, &NeedDelete);
    }

    FreePool (BootOptionVar);
    if (ExpandedDevPath != NULL) {
      FreePool (ExpandedDevPath);
    }

    if (NeedDelete) {
      BdsLibDeleteBootOption (BootOrder[Index], BootOrder, &BootOrderSize);
      continue;
    }

    if (IsWindowsToGoBootOption) {
      *WindowsToGoBootVarExist = TRUE;
    }

    Index++;
  }

  if (!EFI_ERROR (SystemConfigStatus)) {
    AdjustBootOrder (
      !((BOOLEAN) SystemConfiguration.BootNormalPriority),
      BootOrderSize / sizeof (UINT16),
      BootOrder
      );
  }

  Status = gRT->SetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  BootOrderSize,
                  BootOrder
                  );

  if (BootOrder != NULL) {
    FreePool (BootOrder);
  }
  if (DisableOpromStorageDevBoot != NULL) {
    FreePool (DisableOpromStorageDevBoot);

    if (OpromStorageDevCount) {
      FreePool (OpromStorageDev);
    }
  }
  return Status;
}


/**
  Get size of buffer to get protocol's handles.

  @param[in]  ProtocolGuid  The protocol GUID of handles.
  @param[out] BufferSize    Buffer size to get protocol handles.

  @retval     EFI_SUCCESS   Get size successful.
  @retval     other         Error status from gBS->LocateHandle function.
**/
EFI_STATUS
GetHandleBufferSize (
  IN   EFI_GUID              *ProtocolGuid,
  OUT  UINTN                 *BufferSize
  )
{
  EFI_STATUS                 Status;
  UINTN                      TmpBufferSize;

  TmpBufferSize = 0;
  Status = gBS->LocateHandle (
                  ByProtocol,
                  ProtocolGuid,
                  NULL,
                  &TmpBufferSize,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    if (Status == EFI_NOT_FOUND) {
      TmpBufferSize = 0;
    } else if (Status != EFI_BUFFER_TOO_SMALL) {
      return Status;
    }
  }

  /// Status will be EFI_NOT_FOUND or EFI_BUFFER_TOO_SMALL
  *BufferSize = TmpBufferSize;
  return EFI_SUCCESS;
}


/**
  Determine whether there is a added or removed EFI device on system

  @retval TRUE   There is a added or removed EFI device on system
  @retval FALSE  There is no added or removed EFI device on system
**/
STATIC
BOOLEAN
IsEfiDevAddedOrRemoved (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINTN                         FileSysHandlesNum;
  UINTN                         LoadFileHandlesNum;
  UINTN                         TotalHandlesNum;
  EFI_HANDLE                    *ProtocolHandles;
  UINTN                         RecordIndex;
  UINTN                         Index;
  BOOLEAN                       DevIsAddedOrRemoved;

  FileSysHandlesNum = 0;
  Status = GetHandleBufferSize (
             &gEfiSimpleFileSystemProtocolGuid,
             &FileSysHandlesNum
             );
  if (EFI_ERROR (Status)) {
    return TRUE;
  }
  LoadFileHandlesNum = 0;
  Status = GetHandleBufferSize (
             &gEfiLoadFileProtocolGuid,
             &LoadFileHandlesNum
             );
  if (EFI_ERROR (Status)) {
    return TRUE;
  }
  TotalHandlesNum = FileSysHandlesNum + LoadFileHandlesNum;
  if (TotalHandlesNum != 0) {
    ProtocolHandles = AllocateZeroPool (sizeof (EFI_HANDLE) * TotalHandlesNum);
    if (ProtocolHandles == NULL) {
      return TRUE;
    }
  } else {
    ProtocolHandles = NULL;
  }

  if (FileSysHandlesNum != 0) {
    gBS->LocateHandle (
           ByProtocol,
           &gEfiSimpleFileSystemProtocolGuid,
           NULL,
           &FileSysHandlesNum,
           ProtocolHandles
           );
  }

  if (LoadFileHandlesNum != 0) {
    gBS->LocateHandle (
           ByProtocol,
           &gEfiLoadFileProtocolGuid,
           NULL,
           &LoadFileHandlesNum,
           (ProtocolHandles + FileSysHandlesNum)
           );
  }

  if (TotalHandlesNum == mGenericBdsLibGlobalData->PreviousHandlesNum) {
    DevIsAddedOrRemoved = FALSE;
    for (Index = 0; Index < TotalHandlesNum; Index++) {
      for (RecordIndex = 0; RecordIndex < mGenericBdsLibGlobalData->PreviousHandlesNum; RecordIndex++) {
        if (mGenericBdsLibGlobalData->PreviousHandles[RecordIndex] == ProtocolHandles[Index]) {
          break;
        }
      }

      if (RecordIndex == mGenericBdsLibGlobalData->PreviousHandlesNum) {
        DevIsAddedOrRemoved = TRUE;
        break;
      }
    }
  } else {
    DevIsAddedOrRemoved = TRUE;
  }

  if (mGenericBdsLibGlobalData->PreviousHandles != NULL) {
    FreePool (mGenericBdsLibGlobalData->PreviousHandles);
  }
  mGenericBdsLibGlobalData->PreviousHandlesNum = TotalHandlesNum;
  mGenericBdsLibGlobalData->PreviousHandles    = (TotalHandlesNum > 0) ? ProtocolHandles : NULL;

  return DevIsAddedOrRemoved;
}

/**
 Internal function to trigger BOOT_DEVICE_ENUM_AFTER checkpoint.
**/
STATIC
VOID
TriggerCpBootDeviceEnumAfter (
  VOID
  )
{
  H2O_BDS_CP_BOOT_DEVICE_ENUM_AFTER_DATA     BootDevEnumAfterData;

  BootDevEnumAfterData.Size   = sizeof (H2O_BDS_CP_BOOT_DEVICE_ENUM_AFTER_DATA);
  BootDevEnumAfterData.Status = H2O_CP_TASK_NORMAL;

  DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpBootDeviceEnumAfterGuid));
  H2OCpTrigger (&gH2OBdsCpBootDeviceEnumAfterGuid, &BootDevEnumAfterData);
  DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", BootDevEnumAfterData.Status));
}

/**
 Internal function to trigger BOOT_DEVICE_ENUM_BEFORE checkpoint.
**/
STATIC
VOID
TriggerCpBootDeviceEnumBefore (
  VOID
  )
{
  H2O_BDS_CP_BOOT_DEVICE_ENUM_BEFORE_DATA      BootDevEnumBeforeData;

  BootDevEnumBeforeData.Size   = sizeof (H2O_BDS_CP_BOOT_DEVICE_ENUM_BEFORE_DATA);
  BootDevEnumBeforeData.Status = H2O_CP_TASK_NORMAL;

  DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpBootDeviceEnumBeforeGuid));
  H2OCpTrigger (&gH2OBdsCpBootDeviceEnumBeforeGuid, &BootDevEnumBeforeData);
  DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", BootDevEnumBeforeData.Status));
}

/**
 Internal function to trigger BOOT_DEVICE_ENUM_CHECK_BOOT_OPTION checkpoint.

 @param[in]      VariableName    Pointer to boot option variable name.
 @param[in]      VariableGuid    Pointer to boot option variable GUID.
 @param[in, out] Invalid         Pointer to flag to specify this boot option variable is invalid or not.

 @retval EFI_SUCCESS             Trigger BOOT_DEVICE_ENUM_CHECK_BOOT_OPTION checkpoint successfully.
 @retval Other                   Fail to convert boot option variable to load option data.
**/
EFI_STATUS
TriggerCpBootDeviceEnumCheckBootOption (
  IN     CHAR16                                       *VariableName,
  IN     EFI_GUID                                     *VariableGuid,
  IN OUT BOOLEAN                                      *Invalid
  )
{
  EFI_STATUS                                          Status;
  H2O_BDS_LOAD_OPTION                                 *BdsLoadOption;
  H2O_BDS_CP_BOOT_DEVICE_ENUM_CHECK_BOOT_OPTION_DATA  BootDevEnumCheckBootOptionData;

  Status = gBdsServices->ConvertVarToLoadOption (gBdsServices, VariableName, VariableGuid, &BdsLoadOption);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  BootDevEnumCheckBootOptionData.Size       = sizeof (H2O_BDS_CP_BOOT_DEVICE_ENUM_CHECK_BOOT_OPTION_DATA);
  BootDevEnumCheckBootOptionData.Status     = H2O_CP_TASK_NORMAL;
  BootDevEnumCheckBootOptionData.BootOption = BdsLoadOption;
  BootDevEnumCheckBootOptionData.Valid      = !(*Invalid);

  DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpBootDeviceEnumCheckBootOptionGuid));
  H2OCpTrigger (&gH2OBdsCpBootDeviceEnumCheckBootOptionGuid, &BootDevEnumCheckBootOptionData);
  DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", BootDevEnumCheckBootOptionData.Status));

  if (BootDevEnumCheckBootOptionData.Status == H2O_CP_TASK_UPDATE) {
    *Invalid = !(BootDevEnumCheckBootOptionData.Valid);
  }

  gBdsServices->FreeLoadOption (gBdsServices, BdsLoadOption);
  return EFI_SUCCESS;
}

/**
  For EFI boot option, BDS separate them as six types:
  1. Network - The boot option points to the SimpleNetworkProtocol device.
               Bds will try to automatically create this type boot option when enumerate.
  2. Shell   - The boot option points to internal flash shell.
               Bds will try to automatically create this type boot option when enumerate.
  3. Removable BlockIo      - The boot option only points to the removable media
                              device, like USB flash disk, DVD, Floppy etc.
                              These device should contain a *removable* blockIo
                              protocol in their device handle.
                              Bds will try to automatically create this type boot option
                              when enumerate.
  4. Fixed BlockIo          - The boot option only points to a Fixed blockIo device,
                              like HardDisk.
                              These device should contain a *fixed* blockIo
                              protocol in their device handle.
                              BDS will skip fixed blockIo devices, and NOT
                              automatically create boot option for them. But BDS
                              will help to delete those fixed blockIo boot option,
                              whose description rule conflict with other auto-created
                              boot options.
  5. Non-BlockIo Simplefile - The boot option points to a device whose handle
                              has SimpleFileSystem Protocol, but has no blockio
                              protocol. These devices do not offer blockIo
                              protocol, but BDS still can get the
                              \EFI\BOOT\boot{machinename}.EFI by SimpleFileSystem
                              Protocol.
  6. File    - The boot option points to a file. These boot options are usually
               created by user manually or OS loader. BDS will not delete or modify
               these boot options.

  This function will enumerate all possible boot device in the system, and
  automatically create boot options for Network, Shell, Removable BlockIo,
  and Non-BlockIo Simplefile devices.
  It will only execute once of every boot.

  @param  BdsBootOptionList      The header of the link list which indexed all
                                 current boot options. Deprecated.

  @retval EFI_SUCCESS            Finished all the boot device enumerate and create
                                 the boot option base on that boot device

  @retval EFI_OUT_OF_RESOURCES   Failed to enumerate the boot device and create the boot option list
**/
EFI_STATUS
BdsLibEnumerateAllBootOption (
  IN     BOOLEAN             ForceEnumerateAll,
  IN OUT LIST_ENTRY          *BdsBootOptionList OPTIONAL
  )
{
  EFI_STATUS                    Status;
  KERNEL_CONFIGURATION          SystemConfiguration;
  EFI_STATUS                    SystemConfigStatus;
  BOOLEAN                       WindowsToGoBootVarExist;
  CHAR8                         *PlatLang;
  CHAR8                         *LastLang;
  UINTN                         BootType;
  BOOLEAN                       IsUsbBootSupported;
  BOOLEAN                       IsNetworkBootSupported;

  POST_CODE (BDS_ENUMERATE_ALL_BOOT_OPTION);
  DEBUG_CODE (
    if (BdsBootOptionList != NULL) {
      DEBUG ((DEBUG_INFO, "BdsLibEnumerateAllBootOption(): BdsBootOptionList is deprecated. Please upgrade caller code to input NULL and use GetBootList() to get boot option list.\n"));
    }
  );

  if (FeaturePcdGet (PcdH2OBdsCpBootDeviceEnumBeforeSupported)) {
    TriggerCpBootDeviceEnumBefore ();
  }

  Status   = EFI_SUCCESS;
  PlatLang = NULL;
  LastLang = NULL;
  BootType = BdsLibGetBootType ();
  BdsLibDeleteInvalidBootOptions ();
  //
  // If the boot device enumerate happened, just get the boot device from the boot order variable
  //
  if (!IsEfiDevAddedOrRemoved () && mGenericBdsLibGlobalData->EnumBootDevice && !ForceEnumerateAll) {
    //
    // No new bootable device. Get PlatformLang variable and compare with LastEnumLang variable to check if there is a
    // need to update the boot option description or not. If there is no PlatformLang variable, enumeration has occurred
    // but no language change has happened, so there is no need to update the boot option description.
    //
    GetVariable2 (LAST_ENUM_LANGUAGE_VARIABLE_NAME, &gLastEnumLangGuid, (VOID **)&LastLang, NULL);
    GetEfiGlobalVariable2 (L"PlatformLang",  (VOID **)&PlatLang, NULL);
    if (PlatLang == NULL) {
      goto Exit;
    }

    if ((LastLang != NULL) && (AsciiStrCmp (LastLang, PlatLang) == 0)) {
      SetAllBootOptionsConnected ();
      goto Exit;
    } else {
      gRT->SetVariable (
             LAST_ENUM_LANGUAGE_VARIABLE_NAME,
             &gLastEnumLangGuid,
             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
             AsciiStrSize (PlatLang),
             PlatLang
             );
    }
  }

  //
  // Notes:
  // this dirty code is to get the legacy boot option from the BBS table and create to variable as the EFI boot option,
  // it should be removed after the CSM can provide legacy boot option directly
  //
  if (BootType != EFI_BOOT_TYPE) {
    REFRESH_LEGACY_BOOT_OPTIONS;

    if (BootType == LEGACY_BOOT_TYPE) {
      BdsLibRemovedBootOption (FALSE);
    }
  } else {
    BdsLibRemovedBootOption (TRUE);
  }

  if (FeaturePcdGet (PcdAutoCreateDummyBootOption) && BootType != LEGACY_BOOT_TYPE) {
    CreateDummyBootOptions ();
    SyncBootOrder ();
  }

  BdsLibDeleteRedundantOption ();

  if (mGenericBdsLibGlobalData->UefiFastBootEnabled) {
    mGenericBdsLibGlobalData->UefiFastBootEnabled = BdsLibIsWin8FastBootActive ();
  }

  if (BootType != LEGACY_BOOT_TYPE && !mGenericBdsLibGlobalData->EnableBootOrderHook) {
    if (IsVirtualBootOrder ()) {
      BdsLibEnableBootOrderHook ();
    }
    mGenericBdsLibGlobalData->EnableBootOrderHook = TRUE;
  }

  if (mGenericBdsLibGlobalData->UefiFastBootEnabled) {
    //
    // UEFI fast boot feature is only enabled for first time enumeration.
    //
    mGenericBdsLibGlobalData->UefiFastBootEnabled = FALSE;
    SetTargetHddConnected ();
    goto Exit;
  }

  BdsDeleteAllInvalidEfiBootOption (&WindowsToGoBootVarExist);

  InitializeListHead (&mWindowsToGoDeviceList);
  if (WindowsToGoBootVarExist) {
    UpdateWindowsToGoList ();
  }

  //
  // SCU disable EFI Boot
  //
  if (BootType == LEGACY_BOOT_TYPE) {
    mGenericBdsLibGlobalData->EnumBootDevice = TRUE;
    SetAllBootOptionsConnected ();
    goto Exit;
  }

  //
  // Enumerate all boot options
  //
  SystemConfigStatus = GetKernelConfiguration (&SystemConfiguration);
  ASSERT_EFI_ERROR (SystemConfigStatus);
  IsUsbBootSupported     = (BOOLEAN) (EFI_ERROR (SystemConfigStatus) || SystemConfiguration.UsbBoot == 0);
  IsNetworkBootSupported = (BOOLEAN) (!EFI_ERROR (SystemConfigStatus) && SystemConfiguration.PxeBootToLan != 0);

  EnumerateAllSimpleFileSysBootOption (WindowsToGoBootVarExist, IsUsbBootSupported);
  if (IsNetworkBootSupported) {
    EnumerateAllNetworkBootOption ();
  }
  EnumerateAllShellBootOption ();

  //
  // Make sure every boot only have one time boot device enumerate
  //
  if (SystemConfiguration.Win8FastBoot == 0 &&
      BootType == EFI_BOOT_TYPE) {
    UpdateTargetHddVariable ();
  }
  SetAllBootOptionsConnected ();
  mGenericBdsLibGlobalData->EnumBootDevice = TRUE;

Exit:
  if (FeaturePcdGet (PcdH2OBdsCpBootDeviceEnumAfterSupported)) {
    TriggerCpBootDeviceEnumAfter ();
  }

  if (BdsBootOptionList != NULL) {
    Status = BdsLibBuildOptionFromVar (BdsBootOptionList, L"BootOrder");
  }
  if (FeaturePcdGet (PcdAutoCreateDummyBootOption) && BootType != LEGACY_BOOT_TYPE) {
    BdsLibSyncPhysicalBootOrder ();
  }

  if (LastLang != NULL) {
    FreePool (LastLang);
  }
  if (PlatLang != NULL) {
    FreePool (PlatLang);
  }
  return Status;
}

/**
  Build the boot option with the handle parsed in

  @param  Handle                 The handle which present the device path to create
                                 boot option
  @param  BdsBootOptionList      The header of the link list which indexed all
                                 current boot options. Deprecated.
  @param  String                 The description of the boot option.
**/
VOID
EFIAPI
BdsLibBuildOptionFromHandle (
  IN  EFI_HANDLE                 Handle,
  IN  LIST_ENTRY                 *BdsBootOptionList OPTIONAL,
  IN  CHAR16                     *String
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  CHAR16                    *TempString;
  CHAR16                    *CustomizedDescription;
  EFI_STATUS                OemSvcStatus;
  EFI_DEVICE_PATH_PROTOCOL  *UsbShortFormDevPath;

  DEBUG_CODE (
    if (BdsBootOptionList != NULL) {
      DEBUG ((DEBUG_INFO, "BdsLibBuildOptionFromHandle(): BdsBootOptionList is deprecated. Please upgrade caller code to input NULL.\n"));
    }
  );

  DevicePath = DevicePathFromHandle (Handle);
  if (String == NULL) {
    TempString  = DevicePathToStr (DevicePath);
  } else {
    TempString = String;
  }

  ASSERT (TempString != NULL);
  if (TempString == NULL) {
    return;
  }

  CustomizedDescription = AllocateCopyPool (StrSize (TempString), TempString);
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcDxeUpdateDescriptionOfBootOption \n"));
  OemSvcStatus = OemSvcDxeUpdateDescriptionOfBootOption (DevicePath, NULL, NULL, &CustomizedDescription);
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcDxeUpdateDescriptionOfBootOption Status: %r\n", OemSvcStatus));
  if (CustomizedDescription != NULL) {
    if (TempString != String) {
      FreePool (TempString);
    }
    TempString = CustomizedDescription;
  }

  //
  // Create and register new boot option
  //
  UsbShortFormDevPath = NULL;
  if (FeaturePcdGet (PcdH2OBdsUsbCreateShortFormBootOption)) {
    if (IsUsbDevicePath (DevicePath)) {
      UsbShortFormDevPath = GetUsbShortFormDevPath (DevicePath);
      if (UsbShortFormDevPath != NULL) {
        DevicePath = UsbShortFormDevPath;
      }
    }
  }

  BdsLibRegisterNewOption (NULL, DevicePath, TempString, L"BootOrder", (UINT8 *) "RC", 2);
  if (TempString != String) {
    FreePool (TempString);
  }
  if (UsbShortFormDevPath != NULL) {
    FreePool (UsbShortFormDevPath);
  }
}

EFI_STATUS
SetBbsPriority (
  IN  EFI_LEGACY_BIOS_PROTOCOL          *LegacyBios,
  IN  BDS_COMMON_OPTION                 *Option
  )
{
  BBS_TABLE                     *LocalBbsTable;
  UINT16                        BbsIndex;
  UINT16                        BootOption[10];
  UINTN                         BootOptionSize;
  UINT8                         *Ptr, *BootOptionVar;
  UINT16                        *OptionOrder;
  UINTN                         OptionOrderSize;
  UINT16                        PriorityIndex;
  BOOLEAN                       Flag;  // TRUE for Option->BootCurrent is the highest priority
  UINT16                        DevPathSize;
  CHAR16                        *BootDesc;
  UINT16                        Index;

  //
  // Read the BootOrder variable.
  //
  OptionOrder = BdsLibGetVariableAndSize (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  &OptionOrderSize
                  );
  if (OptionOrder == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Flag = FALSE;

  //
  // Set BBS priority according OptionOrder variable
  //
  Index = 0;
  for (PriorityIndex = 0; PriorityIndex < OptionOrderSize / sizeof (UINT16); PriorityIndex++) {
    UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", OptionOrder[PriorityIndex]);

    BootOptionVar = BdsLibGetVariableAndSize (
                      BootOption,
                      &gEfiGlobalVariableGuid,
                      &BootOptionSize
                      );
    if (BootOptionVar == NULL) {
      continue;
    }
    //
    // Skip the native boot options(EFI shell...)
    //
    Ptr = BootOptionVar + sizeof(UINT32) + sizeof(UINT16) + StrSize ((CHAR16 *)(BootOptionVar + 6));
    if (*Ptr != BBS_DEVICE_PATH) {
      continue;
    }

    Ptr = BootOptionVar;
    Ptr += sizeof (UINT32);

    DevPathSize = *((UINT16 *) Ptr);
    Ptr += sizeof (UINT16);

    BootDesc = (CHAR16*) Ptr;
    Ptr += StrSize (BootDesc);

    Ptr += DevPathSize;
    Ptr += sizeof (BBS_TABLE);

    BbsIndex = *(UINT16 *)Ptr;

    LegacyBios->GetBbsInfo (LegacyBios, NULL, NULL, NULL, &LocalBbsTable);

    LocalBbsTable[BbsIndex].BootPriority = Index;
    Index++;
    //
    // Pull Option->BootCurrent up to the highest priority
    //
    if (!Flag) {
      if (Option->BootCurrent == OptionOrder[PriorityIndex]) {
        LocalBbsTable[BbsIndex].BootPriority = 0;
        Flag = TRUE;
      } else {
        LocalBbsTable[BbsIndex].BootPriority++;
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Return the bootable media handle.
  First, check the device is connected
  Second, check whether the device path point to a device which support SimpleFileSystemProtocol,
  Third, detect the the default boot file in the Media, and return the removable Media handle.

  @param  DevicePath  Device Path to a  bootable device

  @return  The bootable media handle. If the media on the DevicePath is not bootable, NULL will return.
**/
EFI_HANDLE
EFIAPI
BdsLibGetBootableHandle (
  IN  EFI_DEVICE_PATH_PROTOCOL      *DevicePath
  )
{
  EFI_STATUS                      Status;
  EFI_TPL                         OldTpl;
  EFI_DEVICE_PATH_PROTOCOL        *UpdatedDevicePath;
  EFI_DEVICE_PATH_PROTOCOL        *DupDevicePath;
  EFI_HANDLE                      Handle;
  EFI_BLOCK_IO_PROTOCOL           *BlockIo;
  VOID                            *Buffer;
  EFI_DEVICE_PATH_PROTOCOL        *TempDevicePath;
  UINTN                           Size;
  UINTN                           TempSize;
  EFI_HANDLE                      ReturnHandle;
  EFI_HANDLE                      *SimpleFileSystemHandles;

  UINTN                           NumberSimpleFileSystemHandles;
  UINTN                           Index;
  EFI_IMAGE_DOS_HEADER            DosHeader;
  EFI_IMAGE_OPTIONAL_HEADER_UNION       HdrData;
  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION   Hdr;

  if (!IsDevicePathValid (DevicePath, 0)) {
    ASSERT (FALSE);
    return NULL;
  }

  UpdatedDevicePath = DevicePath;

  //
  // Enter to critical section to protect the acquired BlockIo instance
  // from getting released due to the USB mass storage hotplug event
  //
  OldTpl = gBS->RaiseTPL (TPL_CALLBACK);

  //
  // Check whether the device is connected
  //
  Status = gBS->LocateDevicePath (&gEfiBlockIoProtocolGuid, &UpdatedDevicePath, &Handle);
  if (EFI_ERROR (Status)) {
    //
    // Skip the case that the boot option point to a simple file protocol which does not consume block Io protocol,
    //
    Status = gBS->LocateDevicePath (&gEfiSimpleFileSystemProtocolGuid, &UpdatedDevicePath, &Handle);
    if (EFI_ERROR (Status)) {
      //
      // Fail to find the proper BlockIo and simple file protocol, maybe because device not present,  we need to connect it firstly
      //
      UpdatedDevicePath = DevicePath;
      Status            = gBS->LocateDevicePath (&gEfiDevicePathProtocolGuid, &UpdatedDevicePath, &Handle);
      gBS->ConnectController (Handle, NULL, NULL, TRUE);
    }
  } else {
    //
    // For removable device boot option, its contained device path only point to the removable device handle,
    // should make sure all its children handles (its child partion or media handles) are created and connected.
    //
    gBS->ConnectController (Handle, NULL, NULL, TRUE);
    //
    // Get BlockIo protocol and check removable attribute
    //
    Status = gBS->HandleProtocol (Handle, &gEfiBlockIoProtocolGuid, (VOID **)&BlockIo);
    ASSERT_EFI_ERROR (Status);

    //
    // Issue a dummy read to the device to check for media change.
    // When the removable media is changed, any Block IO read/write will
    // cause the BlockIo protocol be reinstalled and EFI_MEDIA_CHANGED is
    // returned. After the Block IO protocol is reinstalled, subsequent
    // Block IO read/write will success.
    //
    Buffer = AllocatePool (BlockIo->Media->BlockSize);
    if (Buffer != NULL) {
      BlockIo->ReadBlocks (
               BlockIo,
               BlockIo->Media->MediaId,
               0,
               BlockIo->Media->BlockSize,
               Buffer
               );
      FreePool(Buffer);
    }
  }

  //
  // Detect the the default boot file from removable Media
  //

  //
  // If fail to get bootable handle specified by a USB boot option, the BDS should try to find other bootable device in the same USB bus
  // Try to locate the USB node device path first, if fail then use its previous PCI node to search
  //
  DupDevicePath = DuplicateDevicePath (DevicePath);
  ASSERT (DupDevicePath != NULL);

  UpdatedDevicePath = DupDevicePath;
  Status = gBS->LocateDevicePath (&gEfiDevicePathProtocolGuid, &UpdatedDevicePath, &Handle);
  //
  // if the resulting device path point to a usb node, and the usb node is a dummy node, should only let device path only point to the previous Pci node
  // Acpi()/Pci()/Usb() --> Acpi()/Pci()
  //
  if ((DevicePathType (UpdatedDevicePath) == MESSAGING_DEVICE_PATH) &&
      (DevicePathSubType (UpdatedDevicePath) == MSG_USB_DP)) {
    //
    // Remove the usb node, let the device path only point to PCI node
    //
    SetDevicePathEndNode (UpdatedDevicePath);
    UpdatedDevicePath = DupDevicePath;
  } else {
    UpdatedDevicePath = DevicePath;
  }

  //
  // Get the device path size of boot option
  //
  Size = GetDevicePathSize(UpdatedDevicePath) - sizeof (EFI_DEVICE_PATH_PROTOCOL); // minus the end node
  ReturnHandle = NULL;
  gBS->LocateHandleBuffer (
      ByProtocol,
      &gEfiSimpleFileSystemProtocolGuid,
      NULL,
      &NumberSimpleFileSystemHandles,
      &SimpleFileSystemHandles
      );
  for (Index = 0; Index < NumberSimpleFileSystemHandles; Index++) {
    //
    // Get the device path size of SimpleFileSystem handle
    //
    TempDevicePath = DevicePathFromHandle (SimpleFileSystemHandles[Index]);
    TempSize = GetDevicePathSize (TempDevicePath)- sizeof (EFI_DEVICE_PATH_PROTOCOL); // minus the end node
    //
    // Check whether the device path of boot option is part of the  SimpleFileSystem handle's device path
    //
    if (Size <= TempSize && CompareMem (TempDevicePath, UpdatedDevicePath, Size)==0) {
      //
      // Load the default boot file \EFI\BOOT\boot{machinename}.EFI from removable Media
      //  machinename is ia32, ia64, x64, ...
      //
      Hdr.Union = &HdrData;
      Status = BdsLibGetImageHeader (
                 SimpleFileSystemHandles[Index],
                 EFI_REMOVABLE_MEDIA_FILE_NAME,
                 &DosHeader,
                 Hdr
                 );
      if (!EFI_ERROR (Status) &&
        EFI_IMAGE_MACHINE_TYPE_SUPPORTED (Hdr.Pe32->FileHeader.Machine) &&
        Hdr.Pe32->OptionalHeader.Subsystem == EFI_IMAGE_SUBSYSTEM_EFI_APPLICATION) {
        ReturnHandle = SimpleFileSystemHandles[Index];
        break;
      }
    }
  }

  FreePool(DupDevicePath);

  if (SimpleFileSystemHandles != NULL) {
    FreePool(SimpleFileSystemHandles);
  }

  gBS->RestoreTPL (OldTpl);

  return ReturnHandle;
}

/**
  Check to see if the network cable is plugged in. If the DevicePath is not
  connected it will be connected.

  @param  DevicePath             Device Path to check

  @retval TRUE                   DevicePath points to an Network that is connected
  @retval FALSE                  DevicePath does not point to a bootable network
**/
BOOLEAN
BdsLibNetworkBootWithMediaPresent (
  IN  EFI_DEVICE_PATH_PROTOCOL      *DevicePath
  )
{
  EFI_STATUS                      Status;
  EFI_DEVICE_PATH_PROTOCOL        *UpdatedDevicePath;
  EFI_HANDLE                      Handle;
  EFI_SIMPLE_NETWORK_PROTOCOL     *Snp;
  BOOLEAN                         MediaPresent;
  UINT32                          InterruptStatus;

  MediaPresent = FALSE;

  UpdatedDevicePath = DevicePath;
  //
  // Locate Load File Protocol for PXE boot option first
  //
  Status = gBS->LocateDevicePath (&gEfiLoadFileProtocolGuid, &UpdatedDevicePath, &Handle);
  if (EFI_ERROR (Status)) {
    //
    // Device not present so see if we need to connect it
    //
    Status = BdsLibConnectDevicePath (DevicePath);
    if (!EFI_ERROR (Status)) {
      //
      // This one should work after we did the connect
      //
      Status = gBS->LocateDevicePath (&gEfiLoadFileProtocolGuid, &UpdatedDevicePath, &Handle);
    }
  }

  if (!EFI_ERROR (Status)) {
    Status = gBS->HandleProtocol (Handle, &gEfiSimpleNetworkProtocolGuid, (VOID **)&Snp);
    if (EFI_ERROR (Status)) {
      //
      // Failed to open SNP from this handle, try to get SNP from parent handle
      //
      UpdatedDevicePath = DevicePathFromHandle (Handle);
      if (UpdatedDevicePath != NULL) {
        Status = gBS->LocateDevicePath (&gEfiSimpleNetworkProtocolGuid, &UpdatedDevicePath, &Handle);
        if (!EFI_ERROR (Status)) {
          //
          // SNP handle found, get SNP from it
          //
          Status = gBS->HandleProtocol (Handle, &gEfiSimpleNetworkProtocolGuid, (VOID **) &Snp);
        }
      }
    }

    if (!EFI_ERROR (Status)) {
      if (Snp->Mode->MediaPresentSupported) {
        if (Snp->Mode->State == EfiSimpleNetworkInitialized) {
          //
          // Invoke Snp->GetStatus() to refresh the media status
          //
          Snp->GetStatus (Snp, &InterruptStatus, NULL);

          //
          // In case some one else is using the SNP check to see if it's connected
          //
          MediaPresent = Snp->Mode->MediaPresent;
        } else {
          //
          // No one is using SNP so we need to Start and Initialize so
          // MediaPresent will be valid.
          //
          Status = Snp->Start (Snp);
          if (!EFI_ERROR (Status)) {
            Status = Snp->Initialize (Snp, 0, 0);
            if (!EFI_ERROR (Status)) {
              MediaPresent = Snp->Mode->MediaPresent;
              Snp->Shutdown (Snp);
            }
            Snp->Stop (Snp);
          }
        }
      } else {
        MediaPresent = TRUE;
      }
    }
  }

  return MediaPresent;
}

/**
  Check whether the Device path in a boot option point to a valid bootable device,
  And if CheckMedia is true, check the device is ready to boot now.

  @param  DevPath     the Device path in a boot option
  @param  CheckMedia  if true, check the device is ready to boot now.

  @retval TRUE        the Device path  is valid
  @retval FALSE       the Device path  is invalid .
**/
BOOLEAN
EFIAPI
BdsLibIsValidEFIBootOptDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevPath,
  IN BOOLEAN                      CheckMedia
  )
{
  return BdsLibIsValidEFIBootOptDevicePathExt (DevPath, CheckMedia, NULL);
}

/**
  Check whether the Device path in a boot option point to a valid bootable device,
  And if CheckMedia is true, check the device is ready to boot now.
  If Description is not NULL and the device path point to a fixed BlockIo
  device, check the description whether conflict with other auto-created
  boot options.

  @param  DevPath     the Device path in a boot option
  @param  CheckMedia  if true, check the device is ready to boot now.
  @param  Description the description in a boot option

  @retval TRUE        the Device path  is valid
  @retval FALSE       the Device path  is invalid .
**/
BOOLEAN
EFIAPI
BdsLibIsValidEFIBootOptDevicePathExt (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevPath,
  IN BOOLEAN                      CheckMedia,
  IN CHAR16                       *Description
  )
{
  EFI_STATUS                Status;
  EFI_HANDLE                Handle;
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *LastDeviceNode;
  EFI_BLOCK_IO_PROTOCOL     *BlockIo;

  TempDevicePath = DevPath;
  LastDeviceNode = DevPath;

  //
  // Check if it's a valid boot option for network boot device.
  // Check if there is EfiLoadFileProtocol installed.
  // If yes, that means there is a boot option for network.
  //
  Status = gBS->LocateDevicePath (
                  &gEfiLoadFileProtocolGuid,
                  &TempDevicePath,
                  &Handle
                  );
  if (EFI_ERROR (Status)) {
    //
    // Device not present so see if we need to connect it
    //
    TempDevicePath = DevPath;
    BdsLibConnectDevicePath (TempDevicePath);
    Status = gBS->LocateDevicePath (
                    &gEfiLoadFileProtocolGuid,
                    &TempDevicePath,
                    &Handle
                    );
  }

  if (!EFI_ERROR (Status)) {
    if (!IsDevicePathEnd (TempDevicePath)) {
      //
      // LoadFile protocol is not installed on handle with exactly the same DevPath
      //
      return FALSE;
    }

    if (CheckMedia) {
      //
      // Test if it is ready to boot now
      //
      if (BdsLibNetworkBootWithMediaPresent(DevPath)) {
        return TRUE;
      }
    } else {
      return TRUE;
    }
  }

  //
  // If the boot option point to a file, it is a valid EFI boot option,
  // and assume it is ready to boot now
  //
  while (!IsDevicePathEnd (TempDevicePath)) {
    //
    // If there is USB Class or USB WWID device path node, treat it as valid EFI
    // Boot Option. BdsExpandUsbShortFormDevicePath () will be used to expand it
    // to full device path.
    //
    if ((DevicePathType (TempDevicePath) == MESSAGING_DEVICE_PATH) &&
        ((DevicePathSubType (TempDevicePath) == MSG_USB_CLASS_DP) ||
         (DevicePathSubType (TempDevicePath) == MSG_USB_WWID_DP))) {
      return TRUE;
    }

    LastDeviceNode = TempDevicePath;
    TempDevicePath = NextDevicePathNode (TempDevicePath);
  }
  if ((DevicePathType (LastDeviceNode) == MEDIA_DEVICE_PATH) &&
    (DevicePathSubType (LastDeviceNode) == MEDIA_FILEPATH_DP)) {
    return TRUE;
  }

  //
  // Check if it's a valid boot option for internal FV application
  //
  if (EfiGetNameGuidFromFwVolDevicePathNode ((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) LastDeviceNode) != NULL) {
    //
    // If the boot option point to internal FV application, make sure it is valid
    //
    TempDevicePath = DevPath;
    Status = BdsLibUpdateFvFileDevicePath (
               &TempDevicePath,
               EfiGetNameGuidFromFwVolDevicePathNode ((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) LastDeviceNode)
               );
    if (Status == EFI_ALREADY_STARTED) {
      return TRUE;
    } else {
      if (Status == EFI_SUCCESS) {
        FreePool (TempDevicePath);
      }
      return FALSE;
    }
  }

  //
  // If the boot option point to a blockIO device:
  //    if it is a removable blockIo device, it is valid.
  //    if it is a fixed blockIo device, check its description confliction.
  //
  TempDevicePath = DevPath;
  Status = gBS->LocateDevicePath (&gEfiBlockIoProtocolGuid, &TempDevicePath, &Handle);
  if (EFI_ERROR (Status)) {
    //
    // Device not present so see if we need to connect it
    //
    Status = BdsLibConnectDevicePath (DevPath);
    if (!EFI_ERROR (Status)) {
      //
      // Try again to get the Block Io protocol after we did the connect
      //
      TempDevicePath = DevPath;
      Status = gBS->LocateDevicePath (&gEfiBlockIoProtocolGuid, &TempDevicePath, &Handle);
    }
  }

  if (!EFI_ERROR (Status)) {
    Status = gBS->HandleProtocol (Handle, &gEfiBlockIoProtocolGuid, (VOID **)&BlockIo);
    if (!EFI_ERROR (Status)) {
      if (CheckMedia) {
        //
        // Test if it is ready to boot now
        //
        if (BdsLibGetBootableHandle (DevPath) != NULL) {
          return TRUE;
        }
      } else {
        return TRUE;
      }
    }
  } else {
    //
    // if the boot option point to a simple file protocol which does not consume block Io protocol, it is also a valid EFI boot option,
    //
    Status = gBS->LocateDevicePath (&gEfiSimpleFileSystemProtocolGuid, &TempDevicePath, &Handle);
    if (!EFI_ERROR (Status)) {
      if (CheckMedia) {
        //
        // Test if it is ready to boot now
        //
        if (BdsLibGetBootableHandle (DevPath) != NULL) {
          return TRUE;
        }
      } else {
        return TRUE;
      }
    }
  }
  return FALSE;
}

/**
  According to a file guild, check a Fv file device path is valid. If it is invalid,
  try to return the valid device path.
  FV address maybe changes for memory layout adjust from time to time, use this function
  could promise the Fv file device path is right.

  @param  DevicePath             on input, the Fv file device path need to check on
                                 output, the updated valid Fv file device path
  @param  FileGuid               the Fv file guild

  @retval EFI_INVALID_PARAMETER  the input DevicePath or FileGuid is invalid
                                 parameter
  @retval EFI_UNSUPPORTED        the input DevicePath does not contain Fv file
                                 guild at all
  @retval EFI_ALREADY_STARTED    the input DevicePath has pointed to Fv file, it is
                                 valid
  @retval EFI_SUCCESS            has successfully updated the invalid DevicePath,
                                 and return the updated device path in DevicePath
**/
EFI_STATUS
EFIAPI
BdsLibUpdateFvFileDevicePath (
  IN  OUT EFI_DEVICE_PATH_PROTOCOL      ** DevicePath,
  IN  EFI_GUID                          *FileGuid
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *LastDeviceNode;
  EFI_STATUS                    Status;
  EFI_GUID                      *GuidPoint;
  UINTN                         Index;
  UINTN                         FvHandleCount;
  EFI_HANDLE                    *FvHandleBuffer;
  EFI_FV_FILETYPE               Type;
  UINTN                         Size;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINT32                        AuthenticationStatus;
  BOOLEAN                       FindFvFile;
  EFI_LOADED_IMAGE_PROTOCOL     *LoadedImage;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH FvFileNode;
  EFI_HANDLE                    FoundFvHandle;
  EFI_DEVICE_PATH_PROTOCOL      *NewDevicePath;

  if ((DevicePath == NULL) || (*DevicePath == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  if (FileGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check whether the device path point to the default the input Fv file
  //
  TempDevicePath = *DevicePath;
  LastDeviceNode = TempDevicePath;
  while (!IsDevicePathEnd (TempDevicePath)) {
     LastDeviceNode = TempDevicePath;
     TempDevicePath = NextDevicePathNode (TempDevicePath);
  }
  GuidPoint = EfiGetNameGuidFromFwVolDevicePathNode (
                (MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) LastDeviceNode
                );
  if (GuidPoint == NULL) {
    //
    // if this option does not points to a Fv file, just return EFI_UNSUPPORTED
    //
    return EFI_UNSUPPORTED;
  }
  if (!CompareGuid (GuidPoint, FileGuid)) {
    //
    // If the Fv file is not the input file guid, just return EFI_UNSUPPORTED
    //
    return EFI_UNSUPPORTED;
  }

  //
  // Check whether the input Fv file device path is valid
  //
  TempDevicePath = *DevicePath;
  FoundFvHandle = NULL;
  Status = gBS->LocateDevicePath (
                  &gEfiFirmwareVolume2ProtocolGuid,
                  &TempDevicePath,
                  &FoundFvHandle
                  );
  if (!EFI_ERROR (Status)) {
    Status = gBS->HandleProtocol (
                    FoundFvHandle,
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **) &Fv
                    );
    if (!EFI_ERROR (Status)) {
      //
      // Set FV ReadFile Buffer as NULL, only need to check whether input Fv file exist there
      //
      Status = Fv->ReadFile (
                    Fv,
                    FileGuid,
                    NULL,
                    &Size,
                    &Type,
                    &Attributes,
                    &AuthenticationStatus
                    );
      if (!EFI_ERROR (Status)) {
        return EFI_ALREADY_STARTED;
      }
    }
  }

  //
  // Look for the input wanted FV file in current FV
  // First, try to look for in Bds own FV. Bds and input wanted FV file usually are in the same FV
  //
  FindFvFile = FALSE;
  FoundFvHandle = NULL;
  Status = gBS->HandleProtocol (
             gImageHandle,
             &gEfiLoadedImageProtocolGuid,
             (VOID **) &LoadedImage
             );
  if (!EFI_ERROR (Status)) {
    Status = gBS->HandleProtocol (
                    LoadedImage->DeviceHandle,
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **) &Fv
                    );
    if (!EFI_ERROR (Status)) {
      Status = Fv->ReadFile (
                    Fv,
                    FileGuid,
                    NULL,
                    &Size,
                    &Type,
                    &Attributes,
                    &AuthenticationStatus
                    );
      if (!EFI_ERROR (Status)) {
        FindFvFile = TRUE;
        FoundFvHandle = LoadedImage->DeviceHandle;
      }
    }
  }
  //
  // Second, if fail to find, try to enumerate all FV
  //
  if (!FindFvFile) {
    FvHandleBuffer = NULL;
    gBS->LocateHandleBuffer (
          ByProtocol,
          &gEfiFirmwareVolume2ProtocolGuid,
          NULL,
          &FvHandleCount,
          &FvHandleBuffer
          );
    for (Index = 0; Index < FvHandleCount; Index++) {
      gBS->HandleProtocol (
            FvHandleBuffer[Index],
            &gEfiFirmwareVolume2ProtocolGuid,
            (VOID **) &Fv
            );

      Status = Fv->ReadFile (
                    Fv,
                    FileGuid,
                    NULL,
                    &Size,
                    &Type,
                    &Attributes,
                    &AuthenticationStatus
                    );
      if (EFI_ERROR (Status)) {
        //
        // Skip if input Fv file not in the FV
        //
        continue;
      }
      FindFvFile = TRUE;
      FoundFvHandle = FvHandleBuffer[Index];
      break;
    }

    if (FvHandleBuffer != NULL) {
      FreePool (FvHandleBuffer);
    }
  }

  if (FindFvFile) {
    //
    // Build the shell device path
    //
    NewDevicePath = DevicePathFromHandle (FoundFvHandle);
    EfiInitializeFwVolDevicepathNode (&FvFileNode, FileGuid);
    NewDevicePath = AppendDevicePathNode (NewDevicePath, (EFI_DEVICE_PATH_PROTOCOL *) &FvFileNode);
    *DevicePath = NewDevicePath;
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}

/**
  According Input value to determine remove legacy boot device or EFI
  boot device.

  @param  RemovedLegacy     TRUE: Remove all of legacy boot devices
                            FALSE: Remove all of EFI boot devices

  @retval EFI_SUCCESS            Remove boot devices successful.
  @retval EFI_NOT_FOUND          Cannot find any boot device.
  @retval EFI_OUT_OF_RESOURCES   Boot Order and Boot option does not synchronization
**/
EFI_STATUS
BdsLibRemovedBootOption (
  IN  BOOLEAN                           RemovedLegacy
  )
{
  UINT16                    *BootOrder;
  UINT8                     *BootOptionVar;
  UINTN                     BootOrderSize;
  UINTN                     BootOptionSize;
  EFI_STATUS                Status;
  UINTN                     Index;
  UINTN                     Index2;
  UINT16                    BootOption[BOOT_OPTION_MAX_CHAR];

  BOOLEAN                   IsLegacyBootOption;
  BBS_TABLE                 *BbsEntry;
  UINT16                    BbsIndex;
  BOOLEAN                   NeedDelete;

  Status        = EFI_SUCCESS;
  BootOrder     = NULL;
  BootOrderSize = 0;

  //
  // delete physical and virtual boot order variable if system is in legacy mode.
  //
  if (FeaturePcdGet (PcdAutoCreateDummyBootOption) && !RemovedLegacy) {
    Status = gRT->SetVariable (
                    PHYSICAL_BOOT_ORDER_NAME,
                    &gEfiGenericVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    0,
                    NULL
                    );
  }

  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (NULL == BootOrder) {
    return EFI_NOT_FOUND;
  }

  Index = 0;
  while (Index < BootOrderSize / sizeof (UINT16)) {
    UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[Index]);
    BootOptionVar = BdsLibGetVariableAndSize (
                      BootOption,
                      &gEfiGlobalVariableGuid,
                      &BootOptionSize
                      );
    if (NULL == BootOptionVar) {
      FreePool (BootOrder);
      return EFI_OUT_OF_RESOURCES;
    }

    IsLegacyBootOption = BdsLibIsLegacyBootOption (BootOptionVar, &BbsEntry, &BbsIndex);

    NeedDelete = TRUE;
    if (RemovedLegacy && !IsLegacyBootOption) {
      NeedDelete = FALSE;
    }

    if (!RemovedLegacy && IsLegacyBootOption) {
      NeedDelete = FALSE;
    }

    if (FeaturePcdGet (PcdH2OBdsCpBootDeviceEnumCheckBootOptionSupported)) {
      TriggerCpBootDeviceEnumCheckBootOption (BootOption, &gEfiGlobalVariableGuid, &NeedDelete);
    }

    //
    // Delete this invalid boot option "Boot####"
    //
    if (NeedDelete) {
      Status = gRT->SetVariable (
                      BootOption,
                      &gEfiGlobalVariableGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      0,
                      NULL
                      );
      //
      // Mark this boot option in boot order as deleted
      //
      BootOrder[Index] = 0xffff;
    }

    FreePool (BootOptionVar);
    Index++;
  }

  //
  // Adjust boot order array
  //
  Index2 = 0;
  for (Index = 0; Index < BootOrderSize / sizeof (UINT16); Index++) {
    if (BootOrder[Index] != 0xffff) {
      BootOrder[Index2] = BootOrder[Index];
      Index2 ++;
    }
  }
  Status = gRT->SetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  Index2 * sizeof (UINT16),
                  BootOrder
                  );

  FreePool (BootOrder);

  return Status;
}

/**
  For a bootable Device path, return its boot type

  @param  DevicePath    The bootable device Path to check

  @return UINT32 Boot type :
    //
    // If the device path contains any media deviec path node, it is media boot type
    // For the floppy node, handle it as media node
    //
    BDS_EFI_MEDIA_HD_BOOT
    BDS_EFI_MEDIA_CDROM_BOOT
    BDS_EFI_ACPI_FLOPPY_BOOT
    //
    // If the device path not contains any media deviec path node,  and
    // its last device path node point to a message device path node, it is
    // a message boot type
    //
    BDS_EFI_MESSAGE_ATAPI_BOOT
    BDS_EFI_MESSAGE_SCSI_BOOT
    BDS_EFI_MESSAGE_USB_DEVICE_BOOT
    BDS_EFI_MESSAGE_MISC_BOOT
    //
    // Legacy boot type
    //
    BDS_LEGACY_BBS_BOOT
    //
    // If a EFI Removable BlockIO device path not point to a media and message device,
    // it is unsupported
    //
    BDS_EFI_UNSUPPORT
**/
UINT32
BdsLibGetBootTypeFromDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
  )
{
  ACPI_HID_DEVICE_PATH          *Acpi;
  EFI_DEVICE_PATH_PROTOCOL      *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *LastDeviceNode;
  UINT32                        BootType;

  if (NULL == DevicePath) {
    return BDS_EFI_UNSUPPORT;
  }

  TempDevicePath = DevicePath;

  while (!IsDevicePathEndType (TempDevicePath)) {
    switch (DevicePathType (TempDevicePath)) {

    case BBS_DEVICE_PATH:
       return BDS_LEGACY_BBS_BOOT;

    case MEDIA_DEVICE_PATH:
      if (DevicePathSubType (TempDevicePath) == MEDIA_HARDDRIVE_DP) {
        return BDS_EFI_MEDIA_HD_BOOT;
      } else if (DevicePathSubType (TempDevicePath) == MEDIA_CDROM_DP) {
        return BDS_EFI_MEDIA_CDROM_BOOT;
      } else if (DevicePathSubType (TempDevicePath) == MEDIA_PIWG_FW_FILE_DP) {
        return BDS_EFI_MEDIA_FV_FILEPATH_BOOT;
      }
      break;

    case ACPI_DEVICE_PATH:
      Acpi = (ACPI_HID_DEVICE_PATH *) TempDevicePath;
      if (EISA_ID_TO_NUM (Acpi->HID) == 0x0604) {
        return BDS_EFI_ACPI_FLOPPY_BOOT;
      }
      break;

    case HARDWARE_DEVICE_PATH:
      if (DevicePathSubType (TempDevicePath) == HW_CONTROLLER_DP) {
        //
        // Get the next device path node
        //
        LastDeviceNode = NextDevicePathNode (TempDevicePath);
        //
        // Multi-partition devices
        //
        if (DevicePathSubType (LastDeviceNode) == MSG_DEVICE_LOGICAL_UNIT_DP) {
          return BDS_EFI_SDHC_BOOT;
        }
        //
        // Single-partition devices
        //
      if (!IsDevicePathEndType (LastDeviceNode)) {
          return BDS_EFI_SDHC_BOOT;
        }
      }
      break;

    case MESSAGING_DEVICE_PATH:

      //
      // check message device path is USB device first.
      //
      if (DevicePathSubType(TempDevicePath) == MSG_USB_DP ||
          DevicePathSubType(TempDevicePath) == MSG_USB_WWID_DP) {
        //
        // Check if it is USB LAN.
        //
        LastDeviceNode = TempDevicePath;
        while (!IsDevicePathEndType (LastDeviceNode)) {
          if (DevicePathType (LastDeviceNode) == MESSAGING_DEVICE_PATH &&
              DevicePathSubType(LastDeviceNode) == MSG_MAC_ADDR_DP) {
            return BDS_EFI_MESSAGE_MAC_BOOT;
          }
          LastDeviceNode = NextDevicePathNode (LastDeviceNode);
        }
        return BDS_EFI_MESSAGE_USB_DEVICE_BOOT;
      }

      //
      // check message device path is ISCSI device.
      //
      if (DevicePathSubType(TempDevicePath) == MSG_ISCSI_DP) {
        return BDS_EFI_MESSAGE_ISCSI_BOOT;
      }

      //
      // Check message device path is SD/MMC device.
      //
      if ((DevicePathSubType (TempDevicePath) == MSG_SD_DP) ||
          (DevicePathSubType (TempDevicePath) == MSG_EMMC_DP)) {
        return BDS_EFI_SDHC_BOOT;
      }

      //
      // Get the last device path node
      //
      LastDeviceNode = NextDevicePathNode (TempDevicePath);

      if (DevicePathSubType(LastDeviceNode) == MSG_DEVICE_LOGICAL_UNIT_DP) {
        //
        // if the next node type is Device Logical Unit, which specify the Logical Unit Number (LUN),
        // skit it
        //
          LastDeviceNode = NextDevicePathNode (LastDeviceNode);
      }

      //
      // if the device path not only point to driver device, it is not a messaging device path,
      //
      if (!IsDevicePathEndType (LastDeviceNode)) {
        break;
      }

      switch (DevicePathSubType (TempDevicePath)) {
      case MSG_ATAPI_DP:
        BootType = BDS_EFI_MESSAGE_ATAPI_BOOT;
        break;

      case MSG_SCSI_DP:
        BootType = BDS_EFI_MESSAGE_SCSI_BOOT;
        break;

      case MSG_SATA_DP:
        BootType = BDS_EFI_MESSAGE_SATA_BOOT;
        break;

      case MSG_MAC_ADDR_DP:
      case MSG_VLAN_DP:
      case MSG_IPv4_DP:
      case MSG_IPv6_DP:
        BootType = BDS_EFI_MESSAGE_MAC_BOOT;
        break;

      default:
        BootType = BDS_EFI_MESSAGE_MISC_BOOT;
        break;
      }
      return BootType;

    default:
      break;
    }
    TempDevicePath = NextDevicePathNode (TempDevicePath);
  }

  return BDS_EFI_UNSUPPORT;
}

/**
  According to device path to open file .

  @param  DevicePath    Pointer to EFI_DEVICE_PATH_PROTOCOL
  @param  OpenMode      The mode to open the file. The only valid combinations that the
                        file may be opened with are: Read, Read/Write, or Create/Read/
                        Write. See "Related Definitions" below.
  @param  Attributes    Only valid for EFI_FILE_MODE_CREATE, in which case these
                        are the attribute bits for the newly created file. See "Related
                        Definitions" below.
  @param  NewHandle     A pointer to the location to return the opened handle for the new
                        file. See the type EFI_FILE_PROTOCOL description.

  @retval EFI_SUCCESS            The file was opened.
  @retval EFI_NOT_FOUND          The specified file could not be found on the device.
  @retval EFI_NO_MEDIA           The device has no medium.
  @retval EFI_MEDIA_CHANGED      The device has a different medium in it or the medium is no
                                 longer supported.
  @retval EFI_DEVICE_ERROR       The device reported an error.
  @retval EFI_VOLUME_CORRUPTED   The file system structures are corrupted.
  @retval EFI_WRITE_PROTECTED    An attempt was made to create a file, or open a file for write
                                 when the media is write-protected.
  @retval EFI_ACCESS_DENIED      The service denied access to the file.
  @retval EFI_OUT_OF_RESOURCES   Not enough resources were available to open the file.
  @retval EFI_VOLUME_FULL        The volume is full.
**/
EFI_STATUS
BdsLibOpenFileFromDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  IN  UINT64                     OpenMode,
  IN  UINT64                     Attributes,
  OUT EFI_FILE_HANDLE            *NewHandle
  )
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        DeviceHandle;
  FILEPATH_DEVICE_PATH              *FilePathNode;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL   *Volume;
  EFI_FILE_HANDLE                   FileHandle;
  EFI_FILE_HANDLE                   LastHandle;
  FILEPATH_DEVICE_PATH              *OriginalFilePathNode;

  FilePathNode = (FILEPATH_DEVICE_PATH *) DevicePath;
  Volume       = NULL;
  *NewHandle   = NULL;

  Status = gBS->LocateDevicePath (
                  &gEfiSimpleFileSystemProtocolGuid,
                  (EFI_DEVICE_PATH_PROTOCOL **) &FilePathNode,
                  &DeviceHandle
                  );
  if (!EFI_ERROR (Status)) {
    Status = gBS->HandleProtocol (DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (VOID **) &Volume);
  }

  if (EFI_ERROR (Status) || Volume == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = Volume->OpenVolume (Volume, &FileHandle);
  if (!EFI_ERROR (Status)) {
    //
    // Duplicate the device path to avoid the access to unaligned device path node.
    // Because the device path consists of one or more FILE PATH MEDIA DEVICE PATH
    // nodes, It assures the fields in device path nodes are 2 byte aligned.
    //
    FilePathNode = (FILEPATH_DEVICE_PATH *) DuplicateDevicePath (
                                              (EFI_DEVICE_PATH_PROTOCOL *)(UINTN)FilePathNode
                                              );
    if (FilePathNode == NULL) {
      FileHandle->Close (FileHandle);
      Status = EFI_OUT_OF_RESOURCES;
    } else {
      OriginalFilePathNode = FilePathNode;
      //
      // Parse each MEDIA_FILEPATH_DP node. There may be more than one, since the
      //  directory information and filename can be seperate. The goal is to inch
      //  our way down each device path node and close the previous node
      //
      while (!IsDevicePathEnd (&FilePathNode->Header)) {
        if (DevicePathType (&FilePathNode->Header) != MEDIA_DEVICE_PATH ||
            DevicePathSubType (&FilePathNode->Header) != MEDIA_FILEPATH_DP) {
          Status = EFI_UNSUPPORTED;
        }

        if (EFI_ERROR (Status)) {
          //
          // Exit loop on Error
          //
          break;
        }

        LastHandle = FileHandle;
        FileHandle = NULL;
        Status = LastHandle->Open (
                               LastHandle,
                               &FileHandle,
                               FilePathNode->PathName,
                               OpenMode,
                               Attributes
                               );

        //
        // Close the previous node
        //
        LastHandle->Close (LastHandle);
        FilePathNode = (FILEPATH_DEVICE_PATH *) NextDevicePathNode (&FilePathNode->Header);
      }
      //
      // Free the allocated memory pool
      //
      FreePool (OriginalFilePathNode);
    }
  }

  if (Status == EFI_SUCCESS) {
    *NewHandle = FileHandle;
  }

  return Status;
}

VOID
SignalImageReturns (
  VOID
  )
{
  EFI_HANDLE                        SignalHandle;
  EFI_HANDLE                        *HandleBuffer;
  UINTN                             NumberOfHandles;
  EFI_STATUS                        Status;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gReturnFromImageGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    SignalHandle = NULL;
    Status = gBS->InstallProtocolInterface (
                    &SignalHandle,
                    &gReturnFromImageGuid,
                    EFI_NATIVE_INTERFACE,
                    NULL
                    );
  } else {
    Status = gBS->ReinstallProtocolInterface (
                    HandleBuffer[0],
                    &gReturnFromImageGuid,
                    NULL,
                    NULL
                    );
    FreePool (HandleBuffer);
  }
  //
  // Clear enumerate boot device flag to allow system re-enumerate all boot options.
  // This action is to make sure all of device drivers are loaded in shell environment
  // can be scanned if user returns from shell environment.
  //
  mGenericBdsLibGlobalData->EnumBootDevice = FALSE;
}

