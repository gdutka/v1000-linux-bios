/** @file
  Header file of BDS boot option related functions

;******************************************************************************
;* Copyright (c) 2017 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _BDS_BOOT_OPTION_H_
#define _BDS_BOOT_OPTION_H_

#include "InternalBdsLib.h"
#include "BdsCheckFunctions.h"

EFI_STATUS
EnumerateAllNetworkBootOption (
  VOID
  );

EFI_STATUS
EnumerateAllShellBootOption (
  VOID
  );

EFI_STATUS
EnumerateAllSimpleFileSysBootOption (
  IN     BOOLEAN                WindowsToGoBootVarExist,
  IN     BOOLEAN                IsUsbBootSupported
  );

UINTN
GetGenericOsPcdByFilePath (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevPath
  );

BOOLEAN
IsValidNetworkBootOption (
  IN KERNEL_CONFIGURATION       *SystemConfiguration,
  IN EFI_DEVICE_PATH_PROTOCOL   *OptionDevicePath
  );

BOOLEAN
IsNonRemovableHdd (
  IN EFI_HANDLE                 Handle
  );

BOOLEAN
IsGenericUefiBootOs (
  IN EFI_HANDLE                 Handle,
  IN BOOLEAN                    WindowsToGoEnabled
  );

BOOLEAN
IsGenericUefiBootOsDevPath (
  IN EFI_DEVICE_PATH_PROTOCOL   *OptionDevicePath
  );

BOOLEAN
IsFilePathInGenericOsPcd (
  IN UINTN                      PcdTokenNum,
  IN EFI_DEVICE_PATH_PROTOCOL   *DevPath
  );

BOOLEAN
IsValidRemovableBootOptionDevPath (
  IN EFI_DEVICE_PATH_PROTOCOL   *OptionDevicePath
  );

BOOLEAN
IsSameBootOption (
  IN UINT8                      *FirstBootOption,
  IN UINTN                      FirstBootOptionSize,
  IN UINT8                      *SecondBootOption,
  IN UINTN                      SecondBootOptionSize
  );

EFI_STATUS
UpdateShellDevicePath (
  IN CHAR16                     *OptionName,
  IN UINT32                     Attribute,
  IN CHAR16                     *Description,
  IN UINT32                     BootOptionHashValue
  );

VOID
UpdateWindowsToGoList (
  VOID
  );

extern LIST_ENTRY               mWindowsToGoDeviceList;
#endif
