/** @file

;*******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#ifndef _PLATFORM_USB_LEGACY_ENABL_
#define _PLATFORM_USB_LEGACY_ENABL_

//
// Local function prototypes
//
#define ESC_KEY   0x01
#define DEL_KEY   0x53
#define F1_KEY    0x3b
#define F2_KEY    0x3c
#define F3_KEY    0x3d
#define F4_KEY    0x3e
#define F5_KEY    0x3f
#define F6_KEY    0x40
#define F7_KEY    0x41
#define F8_KEY    0x42
#define F9_KEY    0x43
#define F10_KEY   0x44
#define F11_KEY   0x85
#define F12_KEY   0x86

EFI_STATUS
UsbLegacyPlatformInstall (
  VOID
  );

#endif
