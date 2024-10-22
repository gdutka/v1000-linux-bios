/** @file
  Header file of BDS check related functions

;******************************************************************************
;* Copyright (c) 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#ifndef _BDS_CHECK_FUNCTIONS_H_
#define _BDS_CHECK_FUNCTIONS_H_

#include "InternalBdsLib.h"

BOOLEAN
IsLegacyBootOptionDevPath (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  );

BOOLEAN
IsUsbDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  );

BOOLEAN
IsValidHwVendorBootOptionDevPath (
  IN EFI_DEVICE_PATH_PROTOCOL   *OptionDevicePath
  );

BOOLEAN
IsFvFileBootOptionDevPath (
  IN EFI_DEVICE_PATH_PROTOCOL   *OptionDevicePath
  );

BOOLEAN
IsValidFvFileBootOptionDevPath (
  IN EFI_DEVICE_PATH_PROTOCOL   *OptionDevicePath
  );

BOOLEAN
IsWindowsToGoBootOptionDevPath (
  IN EFI_DEVICE_PATH_PROTOCOL   *OptionDevicePath
  );

#endif
