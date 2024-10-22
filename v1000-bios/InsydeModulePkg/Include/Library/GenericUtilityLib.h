/** @file
  GenericUtilityLib

;******************************************************************************
;* Copyright (c) 2012 - 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _GENERIC_UTILITY_LIB_H_
#define _GENERIC_UTILITY_LIB_H_

#include <Protocol/LegacyBios.h>
#include <Protocol/SimpleFileSystem.h>
#include <Guid/FileSystemVolumeLabelInfo.h>
#include <Guid/FileInfo.h>
#include <Guid/InsydeModuleHii.h>
#include <Guid/BdsHii.h>
#include <Library/DevicePathLib.h>

#define IS_HW_VENDOR_DEVICE_PATH(DevicePath)   ((BOOLEAN) ((DevicePathType (DevicePath) == HARDWARE_DEVICE_PATH) && (DevicePathSubType (DevicePath) == HW_VENDOR_DP)))

VOID
EFIAPI
CallBootManager (
  VOID
  );

UINTN
EFIAPI
CallDeviceManager (
  VOID
  );

VOID
EFIAPI
CallSecureBootMgr (
  VOID
  );

EFI_STATUS
EFIAPI
BdsStartBootMaint (
  VOID
  );

EFI_FILE_HANDLE
EFIAPI
UtilityLibOpenRoot (
  IN EFI_HANDLE                DeviceHandle
  );

EFI_FILE_SYSTEM_VOLUME_LABEL *
EFIAPI
UtilityLibFileSystemVolumeLabelInfo (
  IN EFI_FILE_HANDLE           FHand
  );

EFI_FILE_INFO *
EFIAPI
UtilityLibFileInfo (
  IN EFI_FILE_HANDLE           FHand
  );

EFI_STATUS
EFIAPI
UtilityLibDeleteVariable (
  IN CHAR16                    *VarName,
  IN EFI_GUID                  *VarGuid
  );

CHAR16 *
EFIAPI
UtilityLibStrDuplicate (
  IN CHAR16                    *Src
  );

BOOLEAN
EFIAPI
IsPasswordExist (
  VOID
  );

EFI_STATUS
EFIAPI
LockPasswordState (
  VOID
  );

EFI_STATUS
EFIAPI
UnlockPasswordState (
  IN  CHAR16                   *Password,
  IN  UINTN                    PasswordLength
  );

EFI_STATUS
EFIAPI
CheckPassword (
  VOID
  );

EFI_STATUS
EFIAPI
LaunchBootOptionByDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  );

EFI_STATUS
EFIAPI
ClearOsIndicationsBits (
  IN UINT64                    ClearBits
  );

BOOLEAN
EFIAPI
DoesOsIndicateBootToFwUI (
  VOID
  );

BOOLEAN
EFIAPI
IsHwVendorDevicePath (
  IN EFI_HANDLE                Handle
  );

extern H2O_BDS_LOAD_OPTION_VENDOR_DEVICE_PATH      gH2OBootManagerDevicePath;
extern H2O_BDS_LOAD_OPTION_VENDOR_DEVICE_PATH      gH2OBootMaintenanceDevicePath;
extern H2O_BDS_LOAD_OPTION_VENDOR_DEVICE_PATH      gH2ODeviceManagerDevicePath;
extern H2O_BDS_LOAD_OPTION_VENDOR_DEVICE_PATH      gH2OSecureBootMgrDevicePath;
extern H2O_BDS_LOAD_OPTION_VENDOR_DEVICE_PATH      gH2OFrontPageDevicePath;

#endif
