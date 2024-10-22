/** @file
  PlatformBdsLib

;******************************************************************************
;* Copyright (c) 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "OemHotKey.h"

/**
  Platform Oem HotKey Callback Function

  @param  Selection       HotKey Selection
  @param  Timeout         
  @param  BootMode        
  @param  NoBootDevices   

  @retval EFI_SUCCESS   

**/
EFI_STATUS
OemHotKeyCallback (
  IN UINT16                                    Selection,
  IN UINT16                                    Timeout,
  IN EFI_BOOT_MODE                             BootMode,
  IN BOOLEAN                                   NoBootDevices
  )
{
  EFI_STATUS                            Status;
  LIST_ENTRY                            BdsBootOptionList;

  if ((IsHotKeyDetected () || DoesOsIndicateBootToFwUI ()) && BdsLibIsWin8FastBootActive ()) {
    BdsLibConnectUsbHID ();
    BdsLibConnectAll ();
    if (BdsLibGetBootType () != LEGACY_BOOT_TYPE) {
      BdsLibRestoreBootOrderFromPhysicalBootOrder ();
    }
  }

  Status = EFI_SUCCESS;
  Status = BdsLibStartSetupUtility (TRUE);

  switch (Selection) {

//[-start-140325-IB13080003-add]//
  case SETUP_HOT_KEY:
    //
    // Display SetupUtility
    //
    BdsLibStartSetupUtility (FALSE);
    break;

  case DEVICE_MANAGER_HOT_KEY:
    LaunchBootOptionByDevicePath ((EFI_DEVICE_PATH_PROTOCOL *) &gH2ODeviceManagerDevicePath);
    break;

  case BOOT_MANAGER_HOT_KEY:
    //
    // User chose to run the Boot Manager
    //
    LaunchBootOptionByDevicePath ((EFI_DEVICE_PATH_PROTOCOL *) &gH2OBootManagerDevicePath);
    break;

  case BOOT_MAINTAIN_HOT_KEY:
    if (BdsLibGetBootType () != LEGACY_BOOT_TYPE) {
      //
      // Display the Boot Maintenance Manager
      //
      LaunchBootOptionByDevicePath ((EFI_DEVICE_PATH_PROTOCOL *) &gH2OBootMaintenanceDevicePath);
    } else {
      return EFI_UNSUPPORTED;
    }
    break;

  case SECURE_BOOT_HOT_KEY:
    if (BdsLibGetBootType () != LEGACY_BOOT_TYPE) {
      //
      // Display SetupUtility
      //
      LaunchBootOptionByDevicePath ((EFI_DEVICE_PATH_PROTOCOL *) &gH2OSecureBootMgrDevicePath);
    } else {
      return EFI_UNSUPPORTED;
    }
    break;
    
//[-end-140325-IB13080003-add]//
  case FRONT_PAGE_HOT_KEY:
    if (FeaturePcdGet(PcdFrontPageSupported)) {
      Timeout = 0xFFFF;
      InitializeListHead (&BdsBootOptionList);
      BdsLibEnumerateAllBootOption (TRUE, &BdsBootOptionList);
      LaunchBootOptionByDevicePath ((EFI_DEVICE_PATH_PROTOCOL *) &gH2OFrontPageDevicePath);
      break;
    }

    break;


  }
  
  return EFI_SUCCESS;
}  

