/** @file
 DXE Chipset Services Library.

 This file contains only one function that is DxeCsSvcInstallPciRomSwitchTextMode().
 The function DxeCsSvcInstallPciRomSwitchTextMode() use chipset services to determine
 should to switch to text mode.

***************************************************************************
* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

#include <Protocol/BootLogo.h>

/**
 Called by LegacyBiosPlatform when legacy bios is going to install option Rom.
 The chipset platform library can determine if it requires to switch to text
 mode while installing option rom for specific device.

 @param[in]         DeviceHandle        Handle of device OPROM is associated with.
 @param[in, out]    ForceSwitchTextMode Initial value contains the upper level's switch text
                                        mode policy. Set to TRUE if it requires to switch to
                                        text mode.

 @retval            EFI_SUCCESS           Function returns successfully
 @retval            EFI_INVALID_PARAMETER The parameters Address has an invalid value.
                                          1. DeviceHandle is NULL.
*/
EFI_STATUS
InstallPciRomSwitchTextMode (
  IN      EFI_HANDLE                    DeviceHandle,
  IN OUT  BOOLEAN                       *ForceSwitchTextMode
  )
{
  EFI_STATUS                            Status;
  EFI_BOOT_LOGO_PROTOCOL                *BootLogo;

  *ForceSwitchTextMode = TRUE;

  //
  // Because force to set text mode when dispatch OPROM, so Boot Logo is corrupted,
  // for BGRT it need to reset boot logo status to re-drew logo.
  //
  if (*ForceSwitchTextMode == TRUE) {
    BootLogo = NULL;
    Status = gBS->LocateProtocol (&gEfiBootLogoProtocolGuid, NULL, (VOID **) &BootLogo);
    if (!EFI_ERROR (Status) && (BootLogo != NULL)) {
      BootLogo->SetBootLogo (BootLogo, NULL, 0, 0, 0, 0);
    }
  }

  return EFI_SUCCESS;
}
