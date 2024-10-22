/** @file
  Include file for InstallVerbTable PEIM.

;******************************************************************************
;* Copyright (c) 2013 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _INSTALL_VERB_TABLE_DXE_H_
#define _INSTALL_VERB_TABLE_DXE_H_

#include <Guid/AzaliaPolicy.h>

#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/KernelConfigLib.h>

#include <Protocol/FchDxeCallout.h>

#include <ChipsetSetupConfig.h>

//
// Function Definition
//
VOID
EFIAPI
FchCalloutDxeAzaliaInitCallBack (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );

STATIC
EFI_STATUS
SetAzaliaPolicy (
  IN KERNEL_CONFIGURATION  *SetupVariable
  );
#endif
