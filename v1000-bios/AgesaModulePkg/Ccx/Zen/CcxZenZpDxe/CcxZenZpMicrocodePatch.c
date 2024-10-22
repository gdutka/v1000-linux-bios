/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX Zen - Zeppelin API, and related functions.
 *
 * Contains code that initializes the core complex
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CCX
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
/*++
Module Name:

  CcxZenZpMicrocodePatch.c
  Load microcode

Abstract:
--*/

#include <Library/UefiLib.h>
#include <AGESA.h>
#include <Library/CcxMicrocodePatchLib.h>
#include <Library/BaseCoreLogicalIdLib.h>
#include <Library/AmdIdsHookLib.h>
#include "CcxZenZpMicrocodePatch.h"
#include "IdsLib.h"
#include "Filecode.h"

#define FILECODE CCX_ZEN_CCXZENZPDXE_CCXZENZPMICROCODEPATCH_FILECODE

/*++

Routine Description:

  Zen - Zeppelin Microcode Patch loading

Arguments:
 *  @param[in,out] UcodePatchAddr        - The selected UcodePatch address, return 0 if not found
 *  @param[in] StdHeader                 - Config handle for library and services.
Returns:

--*/
VOID
CcxZenZpMicrocodeInit (
  IN OUT   UINT64             *UcodePatchAddr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  SOC_LOGICAL_ID LogicalId;

  BaseGetLogicalIdOnExecutingCore (&LogicalId);

  if ((LogicalId.Family == AMD_FAMILY_17_ZP) && ((LogicalId.Revision & AMD_REV_F17_ZP_LTE_B1) != 0)) {
    AsmMsrOr64 (MSR_LS_CFG2, BIT7);
  }

  LoadMicrocodePatch (UcodePatchAddr, StdHeader);

  if ((LogicalId.Family == AMD_FAMILY_17_ZP) && ((LogicalId.Revision & AMD_REV_F17_ZP_LTE_B1) != 0)) {
    AsmWbinvd ();
    AsmMsrAnd64 (MSR_LS_CFG2, ~((UINT64) BIT7));
  }

  if ((LogicalId.Family == AMD_FAMILY_17_ZP) && ((LogicalId.Revision & AMD_REV_F17_ZP_B1) != 0)) {
    if (AsmReadMsr64 (0x0000008B) >= 0x08001118) {
      AsmMsrOr64 (0xC001100B, BIT0);
    }
  }
}

