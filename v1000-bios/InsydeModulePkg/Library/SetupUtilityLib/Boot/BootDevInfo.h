/** @file
  Header file for boot device information relative functions

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

#ifndef _BOOT_DEV_INFO_H_
#define _BOOT_DEV_INFO_H_

#include <Library/SetupUtilityLib.h>
#include <IndustryStandard/Pci22.h>
#include "SetupUtilityLibCommon.h"
#include "BbsType.h"
#include "BootMenu.h"

EFI_STATUS
BootDevInfoInit (
  VOID
  );

VOID
BootDevInfoShutdown (
  VOID
  );

EFI_STATUS
BootDevInfoSortByKernelConfig (
  IN KERNEL_CONFIGURATION                   *KernelConfig
  );

EFI_STATUS
BootDevInfoSortByBootOrder (
  VOID
  );

EFI_STATUS
BootDevInfoSetToVariable (
  VOID
  );

UINT16
GetBootTypeOrderCount (
  IN KERNEL_CONFIGURATION                   *KernelConfig
  );

extern BOOT_DEV_INFO                        *mBootDevInfo;
extern UINTN                                mBootDevInfoCount;
#endif
