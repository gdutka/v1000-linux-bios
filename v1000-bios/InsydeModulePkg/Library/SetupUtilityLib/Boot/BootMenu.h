/** @file
  Header file for Boot Menu relative function

;******************************************************************************
;* Copyright (c) 2012 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SETUP_BOOT_MENU_H_
#define _SETUP_BOOT_MENU_H_

#include <Library/SetupUtilityLib.h>
#include <Library/PrintLib.h>
#include "SetupUtilityLibCommon.h"
#include "BbsType.h"
#include "Boot.h"
#include "BootDevInfo.h"

#define MIN_ALIGNMENT_SIZE  4
#define ALIGN_SIZE(a)       ((a % MIN_ALIGNMENT_SIZE) ? MIN_ALIGNMENT_SIZE - (a % MIN_ALIGNMENT_SIZE) : 0)

//
// The following functions are used in Boot menu internal
//
EFI_STATUS
Shiftitem (
  IN     CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN     EFI_QUESTION_ID                        QuestionId,
  IN     BOOLEAN                                UpShift,
  IN OUT KERNEL_CONFIGURATION                   *KernelConfig
  );

EFI_STATUS
UpdateBootOrderLabels (
  IN     EFI_HII_HANDLE                      BootHiiHandle,
  IN OUT KERNEL_CONFIGURATION                *KernelConfig
  );

EFI_STATUS
AdjustBootOrder (
  IN      BOOLEAN      EfiDeviceFirst,
  IN      UINTN        DeviceCount,
  IN OUT  UINT16       *BootOrder
  );

BOOT_DEV_INFO *
GetBootDevInfo (
  IN EFI_QUESTION_ID                        QuestionId
  );

EFI_STATUS
BootConfigInit (
  IN SETUP_UTILITY_BROWSER_DATA             *SuBrowser
  );

EFI_STATUS
BootConfigSaveSetting (
  VOID
  );

#endif
