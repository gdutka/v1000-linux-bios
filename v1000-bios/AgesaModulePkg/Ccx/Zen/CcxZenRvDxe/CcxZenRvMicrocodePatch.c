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

  CcxZenRvMicrocodePatch.c
  Load microcode

Abstract:
--*/

#include <AGESA.h>
#include <Library/CcxMicrocodePatchLib.h>
#include "Filecode.h"

#define FILECODE CCX_ZEN_CCXZENRVDXE_CCXZENRVMICROCODEPATCH_FILECODE


/*++

Routine Description:

  Zen - Zeppelin Microcode Patch loading

Arguments:
 *  @param[in,out] UcodePatchAddr        - The selected UcodePatch address, return 0 if not found
 *  @param[in] StdHeader                 - Config handle for library and services.
Returns:

--*/
VOID
CcxZenRvMicrocodeInit (
  IN OUT   UINT64             *UcodePatchAddr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  LoadMicrocodePatch (UcodePatchAddr, StdHeader);
}

