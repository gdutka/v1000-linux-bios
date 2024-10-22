/**
 * @file
 *
 * IOMMU init tables
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 12:28:05 -0600 (Tue, 09 Dec 2014) $
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
#include  <GnbDxio.h>
#include  <Library/GnbCommonLib.h>
#include  <Library/NbioTable.h>
#include  <GnbRegistersRV.h>
#include  <Filecode.h>

#define FILECODE  NBIO_NBIOBASE_AMDNBIOBASERVPEI_GNBIOMMUTABLESRV_FILECODE

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


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

GNB_TABLE ROMDATA GnbIommuEarlyInitTableRV [] = {
//---------------------------------------------------------------------------

  GNB_ENTRY_TERMINATE
};

GNB_TABLE ROMDATA GnbIommuEnvInitTableRV [] = {
//---------------------------------------------------------------------------
// Set SB location for IOMMU
  GNB_ENTRY_COPY (
    L1_SB_LOCATION_TYPE,
    L1_SB_LOCATION_ADDRESS,
    0,
    32,
    SB_LOCATION_TYPE,
    SB_LOCATION_ADDRESS,
    0,
    32
    ),
  GNB_ENTRY_COPY (
    L2_SB_LOCATION_TYPE,
    L2_SB_LOCATION_ADDRESS,
    0,
    32,
    SB_LOCATION_TYPE,
    SB_LOCATION_ADDRESS,
    0,
    32
    ),

//---------------------------------------------------------------------------
//<Embedded_Override_Start> 
// Disable INTx in IOMMU
  GNB_ENTRY_RMW (
    IOMMU_CONTROL_W_TYPE,
    IOMMU_CONTROL_W_ADDRESS,
    IOMMU_CONTROL_W_INTERRUPT_PIN_W_MASK,
    0x0 << IOMMU_CONTROL_W_INTERRUPT_PIN_W_OFFSET
    ),
//<Embedded_Override_End>
  //---------------------------------------------------------------------------
//programs IOHC::CFG_IOHC_PCI[CFG_IOHC_PCI_Dev0Fn2RegEn] = 1h
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_DISABLED,
    CFG_IOHC_PCI_TYPE,
    CFG_IOHC_PCI_ADDRESS,
    SMN_00000034_CFG_IOHC_PCI_Dev0Fn2RegEn_MASK,
    (0x1 << SMN_00000034_CFG_IOHC_PCI_Dev0Fn2RegEn_OFFSET)
    ),
//Program IOMMU::L2::IOMMU_CAP_MISC[IOMMU_HT_ATS_RESV] = 0h
  GNB_ENTRY_RMW (
    IOMMU_CAP_MISC_TYPE,
    IOMMU_CAP_MISC_ADDRESS,
    SMN_00000050_IOMMU_HT_ATS_RESV_MASK,
    (0 << SMN_00000050_IOMMU_HT_ATS_RESV_OFFSET)
    ),

//---------------------------------------------------------------------------
//IOMMU L1 Initialization
//// IOAGR
  GNB_ENTRY_RMW (
    L1_CNTRL_4_AGR_TYPE,
    L1_CNTRL_4_AGR_ADDRESS,
    IOMMUL1IOAGR_0C8_ATS_multiple_resp_en_MASK |
    IOMMUL1IOAGR_0C8_Timeout_pulse_ext_En_MASK,
    (0x1 << IOMMUL1IOAGR_0C8_ATS_multiple_resp_en_OFFSET) |
    (0x1 << IOMMUL1IOAGR_0C8_Timeout_pulse_ext_En_OFFSET)
    ),
  GNB_ENTRY_RMW (
    L1_DEBUG_1_AGR_TYPE,
    L1_DEBUG_1_AGR_ADDRESS,
    IOMMUL1IOAGR_01C_REG_enable_nw_MASK |
    IOMMUL1IOAGR_01C_REG_disble_eco_invaldation_0_MASK,
    (0x1 << IOMMUL1IOAGR_01C_REG_enable_nw_OFFSET) |
    (0x1 << IOMMUL1IOAGR_01C_REG_disble_eco_invaldation_0_OFFSET)
    ),
  GNB_ENTRY_RMW (
    L1_FEATURE_CNTRL_AGR_TYPE,
    L1_FEATURE_CNTRL_AGR_ADDRESS,
    BIT1 |
    BIT2,
    1 << 1 |
    1 << 2
    ),
///@todo this should be based on data parity error handling
//  GNB_ENTRY_RMW (
//    L1_CNTRL_2_AGR_TYPE,
//    L1_CNTRL_2_AGR_ADDRESS,
//    1,
//    0x01
//    ),
//// PCIE0
  GNB_ENTRY_RMW (
    L1_CNTRL_4_PCIE0_TYPE,
    L1_CNTRL_4_PCIE0_ADDRESS,
    IOMMUL1PCIE0_0C8_ATS_multiple_resp_en_MASK |
    IOMMUL1PCIE0_0C8_Timeout_pulse_ext_En_MASK,
    (0x1 << IOMMUL1PCIE0_0C8_ATS_multiple_resp_en_OFFSET) |
    (0x1 << IOMMUL1PCIE0_0C8_Timeout_pulse_ext_En_OFFSET)
    ),
  GNB_ENTRY_RMW (
    L1_DEBUG_1_PCIE0_TYPE,
    L1_DEBUG_1_PCIE0_ADDRESS,
    IOMMUL1PCIE0_01C_REG_enable_nw_MASK |
    IOMMUL1PCIE0_01C_REG_disble_eco_invaldation_0_MASK,
    (0x1 << IOMMUL1PCIE0_01C_REG_enable_nw_OFFSET) |
    (0x1 << IOMMUL1PCIE0_01C_REG_disble_eco_invaldation_0_OFFSET)
    ),
  GNB_ENTRY_RMW (
    L1_FEATURE_CNTRL_PCIE0_TYPE,
    L1_FEATURE_CNTRL_PCIE0_ADDRESS,
    BIT1 |
    BIT2,
    1 << 1 |
    1 << 2
    ),
///@todo this should be based on data parity error handling
//  GNB_ENTRY_RMW (
//    L1_CNTRL_2_PCIE0_TYPE,
//    L1_CNTRL_2_PCIE0_ADDRESS,
//    1,
//    0x01
//    ),
////---------------------------------------------------------------------------
//// IOMMU L2 Initialization
  GNB_ENTRY_RMW (
    L2_ERR_RULE_CONTROL_0_TYPE,
    L2_ERR_RULE_CONTROL_0_ADDRESS,
    SMN_00000200_ERRRuleLock0_MASK,
    0x1 << SMN_00000200_ERRRuleDisable0_OFFSET
    ),
  GNB_ENTRY_RMW (
    L2_ERR_RULE_CONTROL_3_TYPE,
    L2_ERR_RULE_CONTROL_3_ADDRESS,
    SMN_157000C0_ERRRuleLock1_MASK,
    0x1 << SMN_157000C0_ERRRuleLock1_OFFSET
    ),
  GNB_ENTRY_RMW (
    L2_L2A_PGSIZE_CONTROL_TYPE,
    L2_L2A_PGSIZE_CONTROL_ADDRESS,
    SMN_157000D0_L2AREG_HOST_PGSIZE_MASK |
    SMN_157000D0_L2AREG_GST_PGSIZE_MASK,
    (0x49 << SMN_157000D0_L2AREG_HOST_PGSIZE_OFFSET) |
    (0x49 << SMN_157000D0_L2AREG_GST_PGSIZE_OFFSET)
    ),
  GNB_ENTRY_RMW (
    L2_L2B_PGSIZE_CONTROL_TYPE,
    L2_L2B_PGSIZE_CONTROL_ADDRESS,
    SMN_00000250_L2BREG_HOST_PGSIZE_MASK |
    SMN_00000250_L2BREG_GST_PGSIZE_MASK,
    (0x49 << SMN_00000250_L2BREG_HOST_PGSIZE_OFFSET) |
    (0x49 << SMN_00000250_L2BREG_GST_PGSIZE_OFFSET)
    ),
  GNB_ENTRY_RMW (
    L2_CONTROL_5_TYPE,
    L2_CONTROL_5_ADDRESS,
    SMN_00000130_GST_partial_ptc_cntrl_MASK,
    (0x49 << SMN_00000130_GST_partial_ptc_cntrl_OFFSET)
    ),
  GNB_ENTRY_RMW (
    L2_DEBUG_3_TYPE,
    L2_DEBUG_3_ADDRESS,
    SMN_0000011C_REG_atomic_filter_en_MASK |
    SMN_0000011C_REG_mask_l2_nw_MASK |
    SMN_0000011C_REG_ats_iw_MASK,
    (0x1 << SMN_0000011C_REG_atomic_filter_en_OFFSET) |
    (0x1 << SMN_0000011C_REG_mask_l2_nw_OFFSET) |
    (0x1 << SMN_0000011C_REG_ats_iw_OFFSET)
    ),
  GNB_ENTRY_RMW (
    L2_CP_CONTROL_TYPE,
    L2_CP_CONTROL_ADDRESS,
    SMN_00000158_CPFlushOnWait_MASK |
    SMN_00000158_CPFlushOnInv_MASK,
    (0x1 << SMN_00000158_CPFlushOnWait_OFFSET) |
    (0x0 << SMN_00000158_CPFlushOnInv_OFFSET)
    ),
  GNB_ENTRY_RMW (
    L2B_UPDATE_FILTER_CNTL_TYPE,
    L2B_UPDATE_FILTER_CNTL_ADDRESS,
    SMN_0000014C_L2b_Update_Filter_Bypass_MASK,
    (0x0 << SMN_0000014C_L2b_Update_Filter_Bypass_OFFSET)
    ),
  GNB_ENTRY_RMW (
    L2A_UPDATE_FILTER_CNTL_TYPE,
    L2A_UPDATE_FILTER_CNTL_ADDRESS,
    SMN_15700088_L2a_Update_Filter_Bypass_MASK,
    (0x0 << SMN_15700088_L2a_Update_Filter_Bypass_OFFSET)
    ),
  GNB_ENTRY_RMW (
    L2_TW_CONTROL_TYPE,
    L2_TW_CONTROL_ADDRESS,
    SMN_00000150_TWPrefetchRange_MASK |
    SMN_00000150_TWPrefetchEn_MASK,
    (0x7 << SMN_00000150_TWPrefetchRange_OFFSET) |
    (0x1 << SMN_00000150_TWPrefetchEn_OFFSET)
    ),
//---------------------------------------------------------------------------
// IOMMU Initialization
// IOMMU L2 clock gating
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_L2_CLOCK_GATING,
    L2_L2A_CK_GATE_CONTROL_TYPE,
    L2_L2A_CK_GATE_CONTROL_ADDRESS,
    IOMMUL2A_0CC_CKGateL2ARegsDisable_MASK | IOMMUL2A_0CC_CKGateL2ADynamicDisable_MASK | IOMMUL2A_0CC_CKGateL2ACacheDisable_MASK,
    (0x0 << IOMMUL2A_0CC_CKGateL2ARegsDisable_OFFSET) |
    (0x0 << IOMMUL2A_0CC_CKGateL2ADynamicDisable_OFFSET) |
    (0x0 << IOMMUL2A_0CC_CKGateL2ACacheDisable_OFFSET)
    ),
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_L2_CLOCK_GATING,
    L2_L2B_CK_GATE_CONTROL_TYPE,
    L2_L2B_CK_GATE_CONTROL_ADDRESS,
    SMN_00000240_CKGateL2BRegsDisable_MASK | SMN_00000240_CKGateL2BDynamicDisable_MASK |
    SMN_00000240_CKGateL2BMiscDisable_MASK | SMN_00000240_CKGateL2BCacheDisable_MASK,
    (0x0 << SMN_00000240_CKGateL2BRegsDisable_OFFSET) |
    (0x0 << SMN_00000240_CKGateL2BDynamicDisable_OFFSET) |
    (0x0 << SMN_00000240_CKGateL2BMiscDisable_OFFSET) |
    (0x0 << SMN_00000240_CKGateL2BCacheDisable_OFFSET)
    ),
// IOMMU L1 clock gating
  // IOAGR
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_L1_CLOCK_GATING,
    L1_CLKCNTRL_0_AGR_TYPE,
    L1_CLKCNTRL_0_AGR_ADDRESS,
    IOMMUL1IOAGR_0CC_L1_DMA_CLKGATE_EN_MASK | IOMMUL1IOAGR_0CC_L1_CACHE_CLKGATE_EN_MASK | IOMMUL1IOAGR_0CC_L1_CPSLV_CLKGATE_EN_MASK |
    IOMMUL1IOAGR_0CC_L1_PERF_CLKGATE_EN_MASK | IOMMUL1IOAGR_0CC_L1_MEMORY_CLKGATE_EN_MASK | IOMMUL1IOAGR_0CC_L1_REG_CLKGATE_EN_MASK |
    IOMMUL1IOAGR_0CC_L1_HOSTREQ_CLKGATE_EN_MASK | IOMMUL1IOAGR_0CC_L1_DMARSP_CLKGATE_EN_MASK | IOMMUL1IOAGR_0CC_L1_HOSTRSP_CLKGATE_EN_MASK |
    IOMMUL1IOAGR_0CC_L1_L2_CLKGATE_EN_MASK | IOMMUL1IOAGR_0CC_L1_CLKGATE_HYSTERESIS_MASK,
    (0x1 << IOMMUL1IOAGR_0CC_L1_DMA_CLKGATE_EN_OFFSET) |
    (0x1 << IOMMUL1IOAGR_0CC_L1_CACHE_CLKGATE_EN_OFFSET) |
    (0x0 << IOMMUL1IOAGR_0CC_L1_CPSLV_CLKGATE_EN_OFFSET) |
    (0x1 << IOMMUL1IOAGR_0CC_L1_PERF_CLKGATE_EN_OFFSET) |
    (0x1 << IOMMUL1IOAGR_0CC_L1_MEMORY_CLKGATE_EN_OFFSET) |
    (0x1 << IOMMUL1IOAGR_0CC_L1_REG_CLKGATE_EN_OFFSET) |
    (0x1 << IOMMUL1IOAGR_0CC_L1_HOSTREQ_CLKGATE_EN_OFFSET) |
    (0x1 << IOMMUL1IOAGR_0CC_L1_DMARSP_CLKGATE_EN_OFFSET) |
    (0x1 << IOMMUL1IOAGR_0CC_L1_HOSTRSP_CLKGATE_EN_OFFSET) |
    (0x1 << IOMMUL1IOAGR_0CC_L1_L2_CLKGATE_EN_OFFSET) |
    (0x20 << IOMMUL1IOAGR_0CC_L1_CLKGATE_HYSTERESIS_OFFSET)
    ),
  // PCIE0
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_L1_CLOCK_GATING,
    L1_CLKCNTRL_0_PCIE0_TYPE,
    L1_CLKCNTRL_0_PCIE0_ADDRESS,
    IOMMUL1PCIE0_0CC_L1_DMA_CLKGATE_EN_MASK | IOMMUL1PCIE0_0CC_L1_CACHE_CLKGATE_EN_MASK | IOMMUL1PCIE0_0CC_L1_CPSLV_CLKGATE_EN_MASK |
    IOMMUL1PCIE0_0CC_L1_PERF_CLKGATE_EN_MASK | IOMMUL1PCIE0_0CC_L1_MEMORY_CLKGATE_EN_MASK | IOMMUL1PCIE0_0CC_L1_REG_CLKGATE_EN_MASK |
    IOMMUL1PCIE0_0CC_L1_HOSTREQ_CLKGATE_EN_MASK | IOMMUL1PCIE0_0CC_L1_DMARSP_CLKGATE_EN_MASK | IOMMUL1PCIE0_0CC_L1_HOSTRSP_CLKGATE_EN_MASK |
    IOMMUL1PCIE0_0CC_L1_L2_CLKGATE_EN_MASK | IOMMUL1PCIE0_0CC_L1_CLKGATE_HYSTERESIS_MASK,
    (0x1 << IOMMUL1PCIE0_0CC_L1_DMA_CLKGATE_EN_OFFSET) |
    (0x1 << IOMMUL1PCIE0_0CC_L1_CACHE_CLKGATE_EN_OFFSET) |
    (0x1 << IOMMUL1PCIE0_0CC_L1_CPSLV_CLKGATE_EN_OFFSET) |
    (0x1 << IOMMUL1PCIE0_0CC_L1_PERF_CLKGATE_EN_OFFSET) |
    (0x1 << IOMMUL1PCIE0_0CC_L1_MEMORY_CLKGATE_EN_OFFSET) |
    (0x1 << IOMMUL1PCIE0_0CC_L1_REG_CLKGATE_EN_OFFSET) |
    (0x1 << IOMMUL1PCIE0_0CC_L1_HOSTREQ_CLKGATE_EN_OFFSET) |
    (0x1 << IOMMUL1PCIE0_0CC_L1_DMARSP_CLKGATE_EN_OFFSET) |
    (0x1 << IOMMUL1PCIE0_0CC_L1_HOSTRSP_CLKGATE_EN_OFFSET) |
    (0x1 << IOMMUL1PCIE0_0CC_L1_L2_CLKGATE_EN_OFFSET) |
    (0x20 << IOMMUL1PCIE0_0CC_L1_CLKGATE_HYSTERESIS_OFFSET)
    ),
//---------------------------------------------------------------------------
// Configure IOMMU Power Island
// Hide IOMMU function if disabled
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_DISABLED,
    CFG_IOHC_PCI_TYPE,
    CFG_IOHC_PCI_ADDRESS,
    SMN_00000034_CFG_IOHC_PCI_Dev0Fn2RegEn_MASK,
    0x0
    ),
  // Iommu
  GNB_ENTRY_RMW (
    L2_TW_CONTROL_TYPE,
    L2_TW_CONTROL_ADDRESS,
    SMN_00000150_TWPrefetchRange_MASK |
    SMN_00000150_TWPrefetchEn_MASK,
    (0x7 << SMN_00000150_TWPrefetchRange_OFFSET) |
    (0x1 << SMN_00000150_TWPrefetchEn_OFFSET)
    ),
  // PCIE0
  GNB_ENTRY_RMW (
    TYPE_SMN,
    0x1470001c,
    IOMMUL1IOAGR_01C_REG_eco_allow_update_dis_MASK,
    (0x1 << IOMMUL1IOAGR_01C_REG_Block_On_4K_wait_dis_OFFSET)
    ),
  // PCIE0
  GNB_ENTRY_RMW (
    TYPE_SMN,
    0x1480001c,
    IOMMUL1IOAGR_01C_REG_eco_allow_update_dis_MASK,
    (0x1 << IOMMUL1IOAGR_01C_REG_Block_On_4K_wait_dis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    0x14700034,
    IOMMUL1IOAGR_01C_REG_eco_allow_update_dis_MASK,
    (0x1 << IOMMUL1IOAGR_01C_REG_Block_On_4K_wait_dis_OFFSET)
    ),
  // PCIE0
  GNB_ENTRY_RMW (
    TYPE_SMN,
    0x13f01200,
    0xFF,
    0x1
    ),
  GNB_ENTRY_TERMINATE
};

