/* $NoKeywords:$ */
/**
 * @file
 *
 * CCX Set MCA Library - VH
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
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

  CcxSetMcaLib.c
  Set MCA registers

Abstract:
--*/

#include <AGESA.h>
#include <Library/BaseLib.h>
#include <Library/CcxRolesLib.h>
#include <CcxRegistersVh.h>
#include <Filecode.h>

#define FILECODE LIBRARY_CCXSETMCAVHLIB_CCXSETMCAVHLIB_FILECODE

/* -----------------------------------------------------------------------------*/
/**
 *
 *  CcxSetMca
 *
 *  Description:
 *    This routine sets all MCA registers
 *
 */
VOID
CcxSetMca (
  )
{
  UINT8           McaIndex;
  UINT8           McaCount;
  MCA_CONFIG_MSR  McaCfg;

  // MSR_0000_0179[7:0][Count]
  McaCount = (AsmReadMsr64 (0x00000179) & 0xFF);
  for (McaIndex = 0; McaIndex < McaCount; McaIndex++) {
    McaCfg.Value = AsmReadMsr64 (MSR_MCA_CFG_BANK0 + 0x10 * McaIndex);

    // MCi_CONFIG[32][McaXEnable] = 1 if MGC_CAP[Count] <= 32, else 0
    McaCfg.Field.McaXEnable = (McaCount <= 32) ? 1 : 0;

    // MCi_CONFIG[33][TransparentErrorLoggingEnable] = 0 if supported
    if (McaCfg.Field.TransparentErrorLoggingSupported == 1) {
      McaCfg.Field.TransparentErrorLoggingEnable = 0;
    }

    // MCi_CONFIG[34][LogDeferredInMcaStat] = 1 if supported
    if (McaCfg.Field.DeferredErrorLoggingSupported == 1) {
      McaCfg.Field.LogDeferredInMcaStat = 1;
    }
    AsmWriteMsr64 ((MSR_MCA_CFG_BANK0 + 0x10 * McaIndex), McaCfg.Value);
  }

  // MCA_CTL_MASK_L3
  // Should be run by core 0 of every DIE
  if (CcxIsDiePrimary (NULL)) {
    for (McaIndex = 0; McaIndex < 8; McaIndex++) {
      AsmMsrOr64 ((0xC0010407 + McaIndex), BIT7);
      AsmMsrOr64 (0xC0010419, (BIT9 | BIT8 | BIT7 | BIT6));
    }
  }
}


