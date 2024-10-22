/* $NoKeywords:$ */
/**
 * @file
 *
 * CCX Set MCA Library - RV
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
#include <CcxRegistersRv.h>
#include <Filecode.h>

#define FILECODE LIBRARY_CCXSETMCARVLIB_CCXSETMCARVLIB_FILECODE

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
  MCA_MISC_MSR    McaMisc;
  UINT32          McaMiscAddr;

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

    // write all MCA_MISC0[BlkPtr] = 0 in all MCA banks except MCA_MISC0_UMC (Bank 15 & 16)
    McaMiscAddr = MSR_MCA_MISC_BANK0 + 0x10 * McaIndex;
    if ((McaMiscAddr != 0xC00020F3) && (McaMiscAddr != 0xC0002103)) {
      McaMisc.Value = AsmReadMsr64 (McaMiscAddr);
      McaMisc.Field.BlkPtr = 0;
      AsmWriteMsr64 (McaMiscAddr, McaMisc.Value);
    }
  }

  // MCA_MISC1_UMC0
  McaMisc.Value = AsmReadMsr64 (0x0C00020FA);
  McaMisc.Field.BlkPtr = 0;
  AsmWriteMsr64 (0x0C00020FA, McaMisc.Value);

  // MCA_MISC1_UMC1
  McaMisc.Value = AsmReadMsr64 (0x0C000210A);
  McaMisc.Field.BlkPtr = 0;
  AsmWriteMsr64 (0x0C000210A, McaMisc.Value);

  // McaIntrCfg (C000_0410)
  // bit [3] = 1
  AsmMsrOr64 (0xC0000410, BIT3);

  // MCA_CTL_MASK_LS (C001_0400)
  // bit [10] = 1
  // bit [9]  = 1
  AsmMsrOr64 (0xC0010400, (BIT10 | BIT9));

  // MCA_CTL_MASK_FP (C001_0406)
  // bit [6]  = 1
  AsmMsrOr64 (0xC0010406, BIT6);

  // MCA_CTL_MASK_L3
  // Should be run by core 0 of every DIE
  if (CcxIsDiePrimary (NULL)) {
    for (McaIndex = 0; McaIndex < 8; McaIndex++) {
      AsmMsrOr64 ((0xC0010407 + McaIndex), BIT7);
    }
  }
}


