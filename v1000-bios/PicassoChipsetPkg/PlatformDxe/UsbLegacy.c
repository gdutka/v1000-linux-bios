/** @file
  The Usb Legacy Protocol is used provide a standard interface to the
  UsbLegacy code for platform modifications of operation.

;*******************************************************************************
;* Copyright (c) 2013 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#include "Platform.h"
#include "UsbLegacy.h"

#include <Protocol/UsbLegacyPlatform.h>

//
// Local  prototypes
//
EFI_USB_LEGACY_PLATFORM_PROTOCOL    mUsbLegacyPlatform;

/**

  Return all platform information that can modify USB legacy operation

  @param  This                Protocol instance pointer.
  @param  UsbLegacyModifiers  List of keys to monitor from. This includes both
          
  @retval EFI_SUCCESS         Modifiers exist.
  @retval EFI_NOT_FOUND       Modifiers not not exist.
  
**/
EFI_STATUS
GetUsbPlatformOptions (
  IN   EFI_USB_LEGACY_PLATFORM_PROTOCOL  *This,
  OUT  USB_LEGACY_MODIFIERS              *UsbLegacyModifiers
  )
{
  EFI_STATUS                      Status;
  KERNEL_CONFIGURATION            KernelSetup;

  UsbLegacyModifiers->UsbLegacyEnable           = 0x00;
  UsbLegacyModifiers->UsbZip                    = 0x00;
  UsbLegacyModifiers->UsbZipEmulation           = 0x00;
  UsbLegacyModifiers->UsbFixedDiskWpBootSector  = 0x00;
  UsbLegacyModifiers->UsbBoot                   = 0x00;

  Status = GetKernelConfiguration (&KernelSetup);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // Check SETUP for behavior modifications
  //
  if (KernelSetup.LegacyUsbSupport == 0x01) {
    UsbLegacyModifiers->UsbLegacyEnable = 0x01;
  }
  if (KernelSetup.LegacyUsbSupport == 0x02) {
    UsbLegacyModifiers->UsbUefiOnly = 0x01;
  }
  UsbLegacyModifiers->UsbZip = 0x02;

  UsbLegacyModifiers->UsbZipEmulation = KernelSetup.UsbZipEmulation;

  UsbLegacyModifiers->UsbFixedDiskWpBootSector = 0x00;

  //
  // Set UsbBoot default value Support USB boot
  //
  if (KernelSetup.UsbBoot == 0) {
    UsbLegacyModifiers->UsbBoot = 0x02;//DEFINE_USB_BOOT_ENABLE;
  } else if (KernelSetup.UsbBoot == 1) {
    UsbLegacyModifiers->UsbBoot = 0x01;//DEFINE_USB_BOOT_DISABLED;
  }

  //
  // Set Support USB2.0 Device
  //
  if (KernelSetup.HighSpeedUsb != 0) {
     UsbLegacyModifiers->UsbEhciSupport = 0x01;
  }

  Status = OemSvcSetUsbLegacyPlatformOptions (UsbLegacyModifiers);
  DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "DxeOemSvcChipsetLib OemSvcSetUsbLegacyPlatformOptions, Status : %r\n", Status ) );

  return EFI_SUCCESS;
}

/**

  Return all platform information that can modify USB legacy operation

  @param  This                Protocol instance pointer.
  @param  KeyList             List of keys to monitor from. This includes both USB & PS2 keyboard inputs.
  @param  KeyListSize         Size of KeyList in bytes          
          
  @retval EFI_SUCCESS         Keys are to be monitored.
  @retval EFI_NOT_FOUND       No keys are to be monitored.
  
**/
EFI_STATUS
GetPlatformMonitorKeyOptions (
  IN   EFI_USB_LEGACY_PLATFORM_PROTOCOL *This,
  OUT  KEY_ELEMENT                    **KeyList,
  OUT  UINTN                          *KeyListSize
  )
{
  UINT8                    Count;
  KEY_ELEMENT              EndEntry;

  Count = 0;
  ZeroMem (&EndEntry, sizeof(KEY_ELEMENT));

  //
  // Return KeyList if it exists else return
  // *KeyList = NULL;
  // *KeyListSize = 0;
  //
  *KeyList = (KEY_ELEMENT *)PcdGetPtr (PcdPlatformKeyList);
  while (CompareMem (&EndEntry, &(*KeyList)[Count],sizeof(KEY_ELEMENT)) != 0) {
    Count++;
  }
  *KeyListSize = Count * sizeof(KEY_ELEMENT);
  
  return EFI_SUCCESS;
}

/**

  Install Driver to produce USB Legacy platform protocol.

  @param  This                Protocol instance pointer.
  @param  KeyList             List of keys to monitor from. This includes both USB & PS2 keyboard inputs.
  @param  KeyListSize         Size of KeyList in bytes          
          
  @retval EFI_SUCCESS         USB Legacy Platform protocol installed
  @retval Other               No protocol installed, unload driver
  
**/
EFI_STATUS
UsbLegacyPlatformInstall (
  )
{
  EFI_STATUS                           Status;
  EFI_HANDLE                           Handle;
  //
  // Grab a copy of all the protocols we depend on. Any error would
  // be a dispatcher bug!.
  //
  mUsbLegacyPlatform.GetUsbPlatformOptions  = GetUsbPlatformOptions;
  mUsbLegacyPlatform.GetPlatformMonitorKeyOptions  = GetPlatformMonitorKeyOptions;

  //
  // Make a new handle and install the protocol
  //
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiUsbLegacyPlatformProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mUsbLegacyPlatform
                  );
  return Status;
}
