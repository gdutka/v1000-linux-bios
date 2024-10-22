/** @file
  This protocol will be installed before PXE display information, and uninstall when display function end.
  Project can register protocol install notify to control PXE display information function.

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

#ifndef _PXE_DEVICE_INFO_PROTOCOL_H
#define _PXE_DEVICE_INFO_PROTOCOL_H

//
// PXE device info protocol GUID name
//
#define H2O_PXE_DEVICE_INFO_GUID \
  { 0xd12827a6, 0x5ae1, 0x4c25, { 0x96, 0x11, 0x1c, 0x1f, 0x1a, 0xec, 0x91, 0x6d } }

//
// If using Information of strings, please duplicate it first.
// It will be freed if project provide information string in ProjectHookDeviceInformation
//
typedef struct _H2O_PXE_DEVICE_INFO_PROTOCOL {
  EFI_HANDLE                           PxeBootHandle;
  EFI_HANDLE                           UndiControllerHandle;
  EFI_HANDLE                           DeviceHandle;
  CHAR16                               *MacString;
  CHAR16                               *DeviceInfo;            // This string will be freed if project provide string in ProjectHookDeviceInformation
  CHAR16                               *UndiControllerInfo;    // This string will be freed if project provide string in ProjectHookDeviceInformation
  BOOLEAN                              DisplayEnables;
  CHAR16                               *ProjectHookDeviceInformation;
} H2O_PXE_DEVICE_INFO_PROTOCOL;

extern EFI_GUID gH2oPxeDeviceInfoProtocolGuid;

#endif