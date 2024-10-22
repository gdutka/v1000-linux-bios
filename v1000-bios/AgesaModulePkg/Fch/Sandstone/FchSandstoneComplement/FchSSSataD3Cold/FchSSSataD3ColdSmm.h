/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH SMM Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project   FCH SMM Driver
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _FCH_SS_SATA_D3COLD_H_
#define _FCH_SS_SATA_D3COLD_H_

#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>
#include "GnbDxio.h"
#include <Library/NbioSmuV10Lib.h>
#include <Protocol/FchSmmSxDispatch2.h>
#include <Protocol/FchSmmSwDispatch2.h>
#include <Library/FchSmmLib.h>
#include "FchPlatform.h"

EFI_STATUS
EFIAPI
AmdSmiSataD3ColdEntryCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SW_REGISTER_CONTEXT *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
);

EFI_STATUS
EFIAPI
AmdSmiSataD3ColdExitCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SW_REGISTER_CONTEXT *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
);

EFI_STATUS
FchSataD3ColdSmmEntryPoint (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
);

#endif

