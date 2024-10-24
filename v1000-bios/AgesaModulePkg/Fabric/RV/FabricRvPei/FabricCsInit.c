/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Fabric Coherent Slave initialization.
 *
 * This funtion initializes the CS devices in the fabric.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "Porting.h"
#include "AMD.h"
#include <Library/BaseFabricTopologyLib.h>
#include <FabricRegistersRV.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/AmdIdsHookLib.h>
#include "Filecode.h"
#include "FabricCsInit.h"

#define FILECODE FABRIC_RV_FABRICRVPEI_FABRICCSINIT_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
CONST UINTN ROMDATA RvCsInstanceIds[RV_NUM_CS_BLOCKS] = {
  RV_CS0_INSTANCE_ID,
  RV_CS1_INSTANCE_ID
};

/* -----------------------------------------------------------------------------*/
/**
 *
 *  FabricCsInit
 *
 *  Description:
 *    This funtion initializes the CS devices in the fabric.
 *
 */
VOID
FabricCsInit (
  IN       CONST EFI_PEI_SERVICES       **PeiServices,
  IN       AMD_PEI_SOC_LOGICAL_ID_PPI   *SocLogicalIdPpi
  )
{
  UINTN                       i;
  CS_CFG_A0_REGISTER          CsCfgA0;
  CS_CFG_A1_REGISTER          CsCfgA1;
  CS_CFG_A2_REGISTER          CsCfgA2;
  DRAM_CONT_MON_REGISTER      DramContMon;
  CS_MODE_CTRL_A0_REGISTER    CsModeCtrlA0;
  SPEC_DRAM_RD_MON_REGISTER   SpecDramRdMon;
  DRAM_BASE_ADDRESS_REGISTER  DramBaseAddr;
  COMMON_32BITS_REGISTER      CmnReg;

  for (i = 0; i < RV_NUM_CS_BLOCKS; i++) {
    CsCfgA0.Value = FabricRegisterAccRead (0, 0, CS_CFG_A0_FUNC, CS_CFG_A0_REG, RvCsInstanceIds[i]);
    CsCfgA0.Field.DisAddrCollNcRdVc6 = 1;
    FabricRegisterAccWrite (0, 0, CS_CFG_A0_FUNC, CS_CFG_A0_REG, RvCsInstanceIds[i], CsCfgA0.Value, FALSE);

    CsCfgA1.Value = FabricRegisterAccRead (0, 0, CS_CFG_A1_FUNC, CS_CFG_A1_REG, RvCsInstanceIds[i]);
    CsCfgA1.Field.EnHoldTgtDnForDramWr = 0;
    FabricRegisterAccWrite (0, 0, CS_CFG_A1_FUNC, CS_CFG_A1_REG, RvCsInstanceIds[i], CsCfgA1.Value, FALSE);

    CsModeCtrlA0.Value = FabricRegisterAccRead (0, 0, CS_MODE_CTRL_A0_FUNC, CS_MODE_CTRL_A0_REG, RvCsInstanceIds[i]);
    CsModeCtrlA0.Field.ForceEAInOrder = 1;
    IDS_HOOK (IDS_HOOK_DF_CS_INIT1, NULL, (VOID *) &CsModeCtrlA0);
    FabricRegisterAccWrite (0, 0, CS_MODE_CTRL_A0_FUNC, CS_MODE_CTRL_A0_REG, RvCsInstanceIds[i], CsModeCtrlA0.Value, FALSE);

    CsCfgA2.Value = FabricRegisterAccRead (0, 0, CS_CFG_A2_FUNC, CS_CFG_A2_REG, RvCsInstanceIds[i]);
    CsCfgA2.Field.EnSkipGpuPrbEncNonZeroKey = 1;
    CsCfgA2.Field.DisReqBypEA = 1;
    IDS_HOOK (IDS_HOOK_DF_CS_INIT2, NULL, (VOID *) &CsCfgA2);
    FabricRegisterAccWrite (0, 0, CS_CFG_A2_FUNC, CS_CFG_A2_REG, RvCsInstanceIds[i], CsCfgA2.Value, FALSE);
  }

  DramContMon.Value = 0;
  DramContMon.Field.UmcRdThresh = 6;
  DramContMon.Field.SpecDramRdSampleWin = 5;
  DramContMon.Field.UmcRdSampleWin = 0;
  DramContMon.Field.DramContentionThresh = 0xC;
  DramContMon.Field.DramContentionMonEn = 1;
  FabricRegisterAccWrite (0, 0, DRAM_CONT_MON_FUNC, DRAM_CONT_MON_REG, FABRIC_REG_ACC_BC, DramContMon.Value, FALSE);

  SpecDramRdMon.Value = 0;
  SpecDramRdMon.Field.UmcRdThreshHi = 0x14;
  SpecDramRdMon.Field.UmcRdThreshLow = 0x6;
  SpecDramRdMon.Field.SpecDramRdMissThresh = 6;
  SpecDramRdMon.Field.SpecDramRdContentionThresh = 0xC;
  SpecDramRdMon.Field.SpecDramRdMonEn = 1;
  FabricRegisterAccWrite (0, 0, SPEC_DRAM_RD_MON_FUNC, SPEC_DRAM_RD_MON_REG, FABRIC_REG_ACC_BC, SpecDramRdMon.Value, FALSE);

  for (i = 0; i < RV_NUM_CS_BLOCKS; i++) {
    DramBaseAddr.Value = FabricRegisterAccRead (0, 0, DRAMBASEADDR0_FUNC, DRAMBASEADDR0_REG, RvCsInstanceIds[i]);
    if (DramBaseAddr.Field.AddrRngVal == 0) {
      CsCfgA2.Value = FabricRegisterAccRead (0, 0, CS_CFG_A2_FUNC, CS_CFG_A2_REG, RvCsInstanceIds[i]);
      CsCfgA2.Field.InclSdpCredRlsAllDrained = 0;
      FabricRegisterAccWrite (0, 0, CS_CFG_A2_FUNC, CS_CFG_A2_REG, RvCsInstanceIds[i], CsCfgA2.Value, FALSE);
    }
  }

  CmnReg.Value = FabricRegisterAccRead (0, 0, 0x2, 0x50, FABRIC_REG_ACC_BC);
  CmnReg.Field.Bit0 = 0;
  FabricRegisterAccWrite (0, 0, 0x2, 0x50, FABRIC_REG_ACC_BC, CmnReg.Value, FALSE);
}


