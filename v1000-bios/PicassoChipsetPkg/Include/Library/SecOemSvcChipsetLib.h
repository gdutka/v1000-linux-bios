/** @file
  Definition for Oem Services Default Lib.

;******************************************************************************
;* Copyright (c) 2015, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SEC_OEM_SVC_CHIPSET_LIB_H_
#define _SEC_OEM_SVC_CHIPSET_LIB_H_

#include <Uefi.h>


//
// OemSvc function prototypes
//
EFI_STATUS
OemSvcInitializeInsydeDebugger (
  VOID
  );

EFI_STATUS
OemSvcOemSecPlatformMain (
  IN OUT  EFI_SEC_PEI_HAND_OFF    *SecCoreData,
  IN OUT  EFI_PEI_PPI_DESCRIPTOR  *PpiList
  );

#endif
