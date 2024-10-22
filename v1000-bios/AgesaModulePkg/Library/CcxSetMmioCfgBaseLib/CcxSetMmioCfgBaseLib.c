/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Ccx set MMIO configuration space base address LIB
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

  CcxSetMmioCfgBaseLib.c
  Set MMIO configuration space base address

Abstract:
--*/

#include <AGESA.h>
#include <Library/BaseLib.h>
#include <cpuRegisters.h>
#include <Filecode.h>
#include <Library/CcxRolesLib.h>

#define FILECODE LIBRARY_CCXSETMMIOCFGBASELIB_CCXSETMMIOCFGBASELIB_FILECODE

/* -----------------------------------------------------------------------------*/
/**
 *
 *  CcxSetMmioCfgBaseLib
 *
 *  Description:
 *    This routine sets MSR_MMIO_Cfg_Base register
 *
 */
VOID
CcxSetMmioCfgBaseLib (
  )
{
  MMIO_CFG_BASE_MSR MmioCfgBase;

  if (CcxIsBsp (NULL)) {
    MmioCfgBase.Value = AsmReadMsr64 (MSR_MMIO_Cfg_Base);
    if (MmioCfgBase.Field.Enable == 0) {
      // Set [47:20][MmioCfgBaseAddr]
      // Set [0][Enable]
      // [5:2][BusRange] has been set by ABL
      MmioCfgBase.Field.MmioCfgBaseAddr = RShiftU64 (PcdGet64 (PcdPciExpressBaseAddress), 20);
      MmioCfgBase.Field.Enable = 1;
      AsmWriteMsr64 (MSR_MMIO_Cfg_Base, MmioCfgBase.Value);
    }
  }
}


