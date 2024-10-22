/** @file
  Header file for device manager driver

;******************************************************************************
;* Copyright (c) 2012 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _DEVICE_MANAGER_DXE_H_
#define _DEVICE_MANAGER_DXE_H_

#include <Uefi.h>
#include <Protocol/DevicePath.h>
#include <Protocol/FormBrowser2.h>
#include <Protocol/LoadFile.h>
#include <Guid/BdsHii.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/GenericBdsLib.h>
#include <Library/GenericUtilityLib.h>
#include <Library/HiiExLib.h>
#include <Library/PcdLib.h>

EFI_STATUS
EFIAPI
DeviceManagerLoadFile (
  IN EFI_LOAD_FILE_PROTOCOL                  *This,
  IN EFI_DEVICE_PATH_PROTOCOL                *FilePath,
  IN BOOLEAN                                 BootPolicy,
  IN OUT UINTN                               *BufferSize,
  IN VOID                                    *Buffer OPTIONAL
  );

#endif
