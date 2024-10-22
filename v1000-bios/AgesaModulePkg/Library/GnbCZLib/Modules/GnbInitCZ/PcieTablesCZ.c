/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe init tables.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 313364 $   @e \$Date: 2015-02-21 04:31:07 +0800 (Sat, 21 Feb 2015) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbRegistersCZ.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           T A B L E S
 *----------------------------------------------------------------------------------------
 */


STATIC PCIE_HOST_REGISTER_ENTRY ROMDATA CoreInitTable [] = {
  {
    D0F0xE4_CORE_0020_ADDRESS,
    D0F0xE4_CORE_0020_CiRcOrderingDis_MASK,
    (0x1 << D0F0xE4_CORE_0020_CiRcOrderingDis_OFFSET)
  },
  {
    D0F0xE4_CORE_0010_ADDRESS,
    D0F0xE4_CORE_0010_RxUmiAdjPayloadSize_MASK,
    (0x4 << D0F0xE4_CORE_0010_RxUmiAdjPayloadSize_OFFSET)
  },
  {
    D0F0xE4_CORE_001C_ADDRESS,
    D0F0xE4_CORE_001C_TxArbRoundRobinEn_MASK |
    D0F0xE4_CORE_001C_TxArbSlvLimit_MASK |
    D0F0xE4_CORE_001C_TxArbMstLimit_MASK,
    (0x1 << D0F0xE4_CORE_001C_TxArbRoundRobinEn_OFFSET) |
    (0x4 << D0F0xE4_CORE_001C_TxArbSlvLimit_OFFSET) |
    (0x4 << D0F0xE4_CORE_001C_TxArbMstLimit_OFFSET)
  },
  {
    D0F0xE4_CORE_001C_ADDRESS,
    D0F0xE4_CORE_001C_SlvMemLsEn_MASK |
    D0F0xE4_CORE_001C_MstMemLsEn_MASK |
    D0F0xE4_CORE_001C_ReplayMemLsEn_MASK,
    (0x1 << D0F0xE4_CORE_001C_SlvMemLsEn_OFFSET) |
    (0x1 << D0F0xE4_CORE_001C_MstMemLsEn_OFFSET) |
    (0x1 << D0F0xE4_CORE_001C_ReplayMemLsEn_OFFSET)
  },
  {
    D0F0xE4_CORE_0040_ADDRESS,
    D0F0xE4_CORE_0040_PElecIdleMode_MASK,
    (0x1 << D0F0xE4_CORE_0040_PElecIdleMode_OFFSET)
  },
  {
    D0F0xE4_CORE_0002_ADDRESS,
    D0F0xE4_CORE_0002_HwDebug_0__MASK,
    (0x1 << D0F0xE4_CORE_0002_HwDebug_0__OFFSET)
  },
  {
    D0F0xE4_CORE_00C1_ADDRESS,
    D0F0xE4_CORE_00C1_StrapLinkBwNotificationCapEn_MASK |
    D0F0xE4_CORE_00C1_StrapGen2Compliance_MASK |
    D0F0xE4_CORE_00C1_StrapGen3Compliance_MASK,
    (0x1 << D0F0xE4_CORE_00C1_StrapLinkBwNotificationCapEn_OFFSET) |
    (0x1 << D0F0xE4_CORE_00C1_StrapGen2Compliance_OFFSET) |
    (0x1 << D0F0xE4_CORE_00C1_StrapGen3Compliance_OFFSET)
  },
  {
    D0F0xE4_CORE_00B0_ADDRESS,
    D0F0xE4_CORE_00B0_StrapF0MsiEn_MASK |
    D0F0xE4_CORE_00B0_StrapF0AerEn_MASK,
    (0x1 << D0F0xE4_CORE_00B0_StrapF0MsiEn_OFFSET)
  },
  {
    D0F0xE4_CORE_00B1_ADDRESS,
    D0F0xE4_CORE_00B1_StrapF1MsiEn_MASK |
    D0F0xE4_CORE_00B1_StrapF1AerEn_MASK,
    (0x1 << D0F0xE4_CORE_00B1_StrapF1MsiEn_OFFSET) | (0x1 << D0F0xE4_CORE_00B1_StrapF1AerEn_OFFSET)
  },
  {
    D0F0xE4_CORE_00B2_ADDRESS,
    D0F0xE4_CORE_00B2_StrapF2MsiEn_MASK |
    D0F0xE4_CORE_00B2_StrapF2AerEn_MASK,
    (0x1 << D0F0xE4_CORE_00B2_StrapF2MsiEn_OFFSET) | (0x1 << D0F0xE4_CORE_00B2_StrapF2AerEn_OFFSET)
  }
};

CONST PCIE_HOST_REGISTER_TABLE_HEADER ROMDATA CoreInitTableCZ  = {
  &CoreInitTable[0],
  sizeof (CoreInitTable) / sizeof (PCIE_HOST_REGISTER_ENTRY)
};


STATIC PCIE_HOST_REGISTER_ENTRY ROMDATA PhyLaneInitEarlyTable [] = {
  {
    D0F0xE4_PHY_000A_ADDRESS,
    D0F0xE4_PHY_000A_adapt_cfg_mode_MASK,
    (0x163ul << D0F0xE4_PHY_000A_adapt_cfg_mode_OFFSET)
  }
};

CONST PCIE_HOST_REGISTER_TABLE_HEADER ROMDATA PhyLaneInitEarlyTableCZ  = {
  &PhyLaneInitEarlyTable[0],
  sizeof (PhyLaneInitEarlyTable) / sizeof (PCIE_HOST_REGISTER_ENTRY)
};


STATIC PCIE_HOST_REGISTER_ENTRY ROMDATA PhyWrapperInitEarlyTable [] = {
  {
    D0F0xE4_PHY_620A_ADDRESS,
    D0F0xE4_PHY_620A_adapt_cfg_fom_ber_MASK |
    D0F0xE4_PHY_620A_adapt_cfg_oc_time_MASK |
    D0F0xE4_PHY_620A_adapt_cfg_cdr_time_MASK |
    D0F0xE4_PHY_620A_adapt_cfg_leq_time_MASK |
    D0F0xE4_PHY_620A_adapt_cfg_dfe_time_MASK |
    D0F0xE4_PHY_620A_adapt_cfg_fom_time_MASK,
    (0x3ul << D0F0xE4_PHY_620A_adapt_cfg_fom_ber_OFFSET) |
    (0xcul << D0F0xE4_PHY_620A_adapt_cfg_oc_time_OFFSET) |
    (0x3ul << D0F0xE4_PHY_620A_adapt_cfg_cdr_time_OFFSET) |
    (0x7ul << D0F0xE4_PHY_620A_adapt_cfg_leq_time_OFFSET) |
    (0x9ul << D0F0xE4_PHY_620A_adapt_cfg_dfe_time_OFFSET) |
    (0xcul << D0F0xE4_PHY_620A_adapt_cfg_fom_time_OFFSET)
  },
  {
    D0F0xE4_PHY_620B_ADDRESS,
    D0F0xE4_PHY_620B_adapt_cfg_leq_loop_gain_MASK |
    D0F0xE4_PHY_620B_adapt_cfg_ofc_loop_gain_MASK |
    D0F0xE4_PHY_620B_adapt_cfg_fom_loop_gain_MASK |
    D0F0xE4_PHY_620B_adapt_cfg_dfe_ref_loop_gain_MASK |
    D0F0xE4_PHY_620B_adapt_cfg_dfe_tap_loop_gain_MASK |
    D0F0xE4_PHY_620B_adapt_cfg_pi_off_range_rt_MASK |
    D0F0xE4_PHY_620B_adapt_cfg_pi_off_range_lt_MASK,
    (0x3ul << D0F0xE4_PHY_620B_adapt_cfg_leq_loop_gain_OFFSET) |
    (0xcul << D0F0xE4_PHY_620B_adapt_cfg_ofc_loop_gain_OFFSET) |
    (0xcul << D0F0xE4_PHY_620B_adapt_cfg_fom_loop_gain_OFFSET) |
    (0xcul << D0F0xE4_PHY_620B_adapt_cfg_dfe_ref_loop_gain_OFFSET) |
    (0xcul << D0F0xE4_PHY_620B_adapt_cfg_dfe_tap_loop_gain_OFFSET) |
    (0x4ul << D0F0xE4_PHY_620B_adapt_cfg_pi_off_range_rt_OFFSET) |
    (0x4ul << D0F0xE4_PHY_620B_adapt_cfg_pi_off_range_lt_OFFSET)
  }
};

CONST PCIE_HOST_REGISTER_TABLE_HEADER ROMDATA PhyWrapperInitEarlyTableCZ  = {
  &PhyWrapperInitEarlyTable[0],
  sizeof (PhyWrapperInitEarlyTable) / sizeof (PCIE_HOST_REGISTER_ENTRY)
};

STATIC PCIE_PORT_REGISTER_ENTRY ROMDATA PortInitEarlyTable [] = {
  {
    DxFxxE4_x70_ADDRESS,
    DxFxxE4_x70_RxRcbCplTimeoutMode_MASK,
    (0x1 << DxFxxE4_x70_RxRcbCplTimeoutMode_OFFSET)
  },
  {
    DxFxxE4_xA0_ADDRESS,
    DxFxxE4_xA0_Lc16xClearTxPipe_MASK | DxFxxE4_xA0_LcL1ImmediateAck_MASK | DxFxxE4_xA0_LcL0sInactivity_MASK,
    (0x3 << DxFxxE4_xA0_Lc16xClearTxPipe_OFFSET) |
    (0x1 << DxFxxE4_xA0_LcL1ImmediateAck_OFFSET) |
    (0x6 << DxFxxE4_xA0_LcL0sInactivity_OFFSET)
  },
  {
    DxFxxE4_xA1_ADDRESS,
    DxFxxE4_xA1_LcDontGotoL0sifL1Armed_MASK,
    (0x1 << DxFxxE4_xA1_LcDontGotoL0sifL1Armed_OFFSET)
  },
  {
    DxFxxE4_xA2_ADDRESS,
    DxFxxE4_xA2_LcRenegotiateEn_MASK |
    DxFxxE4_xA2_LcUpconfigureSupport_MASK |
    DxFxxE4_xA2_LcDynLanesPwrState_MASK,
    (0x1 << DxFxxE4_xA2_LcRenegotiateEn_OFFSET) |
    (0x1 << DxFxxE4_xA2_LcUpconfigureSupport_OFFSET) |
    (0x3 << DxFxxE4_xA2_LcDynLanesPwrState_OFFSET)
  },
  {
    DxFxxE4_xA3_ADDRESS,
    DxFxxE4_xA3_LcXmitNFts_MASK,
    (0x36 << DxFxxE4_xA3_LcXmitNFts_OFFSET)
  },
  {
    DxFxxE4_xA3_ADDRESS,
    DxFxxE4_xA3_LcXmitNFtsOverrideEn_MASK,
    (0x1 << DxFxxE4_xA3_LcXmitNFtsOverrideEn_OFFSET)
  },
  {
    DxFxxE4_xB5_ADDRESS,
    DxFxxE4_xB5_LcGoToRecovery_MASK,
    0x1 << DxFxxE4_xB5_LcGoToRecovery_OFFSET
  },
  {
    DxFxxE4_xB1_ADDRESS,
    DxFxxE4_xB1_LcDeassertRxEnInL0s_MASK | DxFxxE4_xB1_LcBlockElIdleinL0_MASK | DxFxxE4_xB1_LcElecIdleMode_MASK,
    (0x1 << DxFxxE4_xB1_LcDeassertRxEnInL0s_OFFSET) |
    (0x1 << DxFxxE4_xB1_LcBlockElIdleinL0_OFFSET) |
    (0x1 << DxFxxE4_xB1_LcElecIdleMode_OFFSET)
  },
  {
    DxFxxE4_xB6_ADDRESS,
    DxFxxE4_xB6_LcEqSearchMode_MASK,
    (0x1 << DxFxxE4_xB6_LcEqSearchMode_OFFSET)
  },
  {
    DxFxxE4_xC0_ADDRESS,
    DxFxxE4_xC0_StrapLaneNegotiation_MASK,
    (0x1 << DxFxxE4_xC0_StrapLaneNegotiation_OFFSET)
  },
  {
    DxFxxE4_xDA_ADDRESS,
    DxFxxE4_xDA_RegHpgiPresenceDetectStateChangeEn_MASK,
    (0x1 << DxFxxE4_xDA_RegHpgiPresenceDetectStateChangeEn_OFFSET)
  }
};

CONST PCIE_PORT_REGISTER_TABLE_HEADER ROMDATA PortInitEarlyTableCZ  = {
  &PortInitEarlyTable[0],
  sizeof (PortInitEarlyTable) / sizeof (PCIE_PORT_REGISTER_ENTRY)
};


STATIC PCIE_PORT_REGISTER_ENTRY ROMDATA PortInitMidTable [] = {
  {
    DxFxxE4_xC0_ADDRESS,
    DxFxxE4_xC0_StrapAutoRcSpeedNegotiationDis_MASK,
    (0x1 << DxFxxE4_xC0_StrapAutoRcSpeedNegotiationDis_OFFSET)
  }
};

CONST PCIE_PORT_REGISTER_TABLE_HEADER ROMDATA PortInitMidTableCZ  = {
  &PortInitMidTable[0],
  sizeof (PortInitMidTable) / sizeof (PCIE_PORT_REGISTER_ENTRY)
};

