/* $NoKeywords:$ */
/**
 * @file
 *
 * ApcbCommon.h
 *
 * AGESA PSP Customization Block common definitions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
 * @e \$Revision: 192403 $ @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 **/
/*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/

#ifndef _APCB_COMMON_H_
#define _APCB_COMMON_H_

typedef enum {
  APCB_ID_CONFIG_CCX_MIN_SEV_ASID,
  APCB_ID_CONFIG_CCX_SEV_ASID_COUNT,
  APCB_ID_CONFIG_CCX_XTRIG7_WORKAROUND,
  APCB_ID_CONFIG_DF_GMI_ENCRYPT,
  APCB_ID_CONFIG_DF_XGMI_ENCRYPT,
  APCB_ID_CONFIG_DF_SAVE_RESTORE_MEM_ENCRYPT,
  APCB_ID_CONFIG_DF_SYS_STORAGE_AT_TOP_OF_MEM,
  APCB_ID_CONFIG_DF_PROBE_FILTER_ENABLE,
  APCB_ID_CONFIG_DF_BOTTOMIO,
  APCB_ID_CONFIG_DF_MEM_INTERLEAVING,
  APCB_ID_CONFIG_DF_DRAM_INTERLEAVE_SIZE,
  APCB_ID_CONFIG_DF_ENABLE_CHAN_INTLV_HASH,
  APCB_ID_CONFIG_DF_PCI_MMIO_SIZE,
  APCB_ID_CONFIG_DF_CAKE_CRC_THRESH_PERF_BOUNDS,
  APCB_ID_CONFIG_DF_MEM_CLEAR,
  APCB_ID_CONFIG_DF3_XGMI2_LINK_CFG,
  APCB_ID_CONFIG_DF_GMI2_REPLAY,
  APCB_ID_CONFIG_DF_DRAM_NPS,
  APCB_ID_CONFIG_DF_INVERT_DRAM_MAP,
  APCB_ID_CONFIG_MEMHOLEREMAPPING,
  APCB_ID_CONFIG_LIMITMEMORYTOBELOW1TB,
  APCB_ID_CONFIG_USERTIMINGMODE,
  APCB_ID_CONFIG_MEMCLOCKVALUE,
  APCB_ID_CONFIG_ENABLECHIPSELECTINTLV,
  APCB_ID_CONFIG_ENABLEECCFEATURE,
  APCB_ID_CONFIG_ENABLEPOWERDOWN,
  APCB_ID_CONFIG_ENABLEPARITY,
  APCB_ID_CONFIG_ENABLEBANKSWIZZLE,
  APCB_ID_CONFIG_ENABLEMEMCLR,
  APCB_ID_CONFIG_UMAMODE,
  APCB_ID_CONFIG_UMASIZE,
  APCB_ID_CONFIG_MEMRESTORECTL,
  APCB_ID_CONFIG_SAVEMEMCONTEXTCTL,
  APCB_ID_CONFIG_ISCAPSULEMODE,
  APCB_ID_CONFIG_FORCETRAINMODE,
  APCB_ID_CONFIG_DIMMTYPEUSEDINMIXEDCONFIG,
  APCB_ID_CONFIG_AMPENABLE,
  APCB_ID_CONFIG_DRAMDOUBLEREFRESHRATE,
  APCB_ID_CONFIG_PMUTRAINMODE,
  APCB_ID_CONFIG_ECCREDIRECTION,
  APCB_ID_CONFIG_SCRUBDRAMRATE,
  APCB_ID_CONFIG_SCRUBL2RATE,
  APCB_ID_CONFIG_SCRUBL3RATE,
  APCB_ID_CONFIG_SCRUBICRATE,
  APCB_ID_CONFIG_SCRUBDCRATE,
  APCB_ID_CONFIG_ECCSYNCFLOOD,
  APCB_ID_CONFIG_ECCSYMBOLSIZE,
  APCB_ID_CONFIG_DQSTRAININGCONTROL,
  APCB_ID_CONFIG_UMAABOVE4G,
  APCB_ID_CONFIG_UMAALIGNMENT,
  APCB_ID_CONFIG_MEMORYALLCLOCKSON,
  APCB_ID_CONFIG_MEMORYBUSFREQUENCYLIMIT,
  APCB_ID_CONFIG_POWERDOWNMODE,
  APCB_ID_CONFIG_IGNORESPDCHECKSUM,
  APCB_ID_CONFIG_MEMORYMODEUNGANGED,
  APCB_ID_CONFIG_MEMORYQUADRANKCAPABLE,
  APCB_ID_CONFIG_MEMORYRDIMMCAPABLE,
  APCB_ID_CONFIG_MEMORYLRDIMMCAPABLE,
  APCB_ID_CONFIG_MEMORYUDIMMCAPABLE,
  APCB_ID_CONFIG_MEMORYSODIMMCAPABLE,
  APCB_ID_CONFIG_DRAMDOUBLEREFRESHRATEEN,
  APCB_ID_CONFIG_DIMMTYPEDDDR4CAPABLE,
  APCB_ID_CONFIG_DIMMTYPEDDDR3CAPABLE,
  APCB_ID_CONFIG_DIMMTYPELPDDDR3CAPABLE,
  APCB_ID_CONFIG_ENABLEZQRESET,
  APCB_ID_CONFIG_ENABLEBANKGROUPSWAP,
  APCB_ID_CONFIG_ODTSCMDTHROTEN,
  APCB_ID_CONFIG_SWCMDTHROTEN,
  APCB_ID_CONFIG_FORCEPWRDOWNTHROTEN,
  APCB_ID_CONFIG_ODTSCMDTHROTCYC,
  APCB_ID_CONFIG_SWCMDTHROTCYC,
  APCB_ID_CONFIG_DIMMSENSORCONF,
  APCB_ID_CONFIG_DIMMSENSORUPPER,
  APCB_ID_CONFIG_DIMMSENSORLOWER,
  APCB_ID_CONFIG_DIMMSENSORCRITICAL,
  APCB_ID_CONFIG_DIMMSENSORRESOLUTION,
  APCB_ID_CONFIG_AUTOREFFINEGRANMODE,
  APCB_ID_CONFIG_ENABLEMEMPSTATE,
  APCB_ID_CONFIG_SOLDERDOWNDRAM,
  APCB_ID_CONFIG_DDRROUTEBALANCEDTEE,
  APCB_ID_CONFIG_MEM_MBIST_TEST_ENABLE,
  APCB_ID_CONFIG_MEM_MBIST_SUBTEST_TYPE,
  APCB_ID_CONFIG_MEM_MBIST_AGGRESOR_ON,
  APCB_ID_CONFIG_MEM_MBIST_HALT_ON_ERROR,
  APCB_ID_CONFIG_MEM_CPU_VREF_RANGE,
  APCB_ID_CONFIG_MEM_DRAM_VREF_RANGE,
  APCB_ID_CONFIG_MEM_TSME_ENABLE,
  APCB_ID_CONFIG_MEM_NVDIMM_POWER_SOURCE,
  APCB_ID_CONFIG_MEM_DATA_POISON,
  APCB_ID_CONFIG_MEM_DATA_SCRAMBLE,
  APCB_ID_CONFIG_BMC_SOCKET_NUMBER,
  APCB_ID_CONFIG_BMC_START_LANE,
  APCB_ID_CONFIG_BMC_END_LANE,
  APCB_ID_CONFIG_BMC_DEVICE,
  APCB_ID_CONFIG_BMC_FUNCTION,
  APCB_ID_CONFIG_FCH_CONSOLE_OUT_ENABLE,
  APCB_ID_CONFIG_FCH_CONSOLE_OUT_SERIAL_PORT
} APCB_COMMON_CONFIG_ID;

typedef enum {
  APCB_ID_CBS_DF_CMN_GMI_ENCRYPTION,
  APCB_ID_CBS_DF_CMN_X_GMI_ENCRYPTION,
  APCB_ID_CBS_DF_CMN_CC6_MEM_ENCRYPTION,
  APCB_ID_CBS_DF_CMN_CC6_ALLOCATION_SCHEME,
  APCB_ID_CBS_DF_CMN_SYS_PROBE_FILTER,
  APCB_ID_CBS_DF_CMN_MEM_INTLV,
  APCB_ID_CBS_DF_CMN_MEM_INTLV_SIZE,
  APCB_ID_CBS_DF_CMN_CHNL_INTLV_HASH,
  APCB_ID_CBS_CMN_MEM_MAPPING_BANK_INTERLEAVE_DDR4,
  APCB_ID_CBS_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_DDR4,
  APCB_ID_CBS_CMN_MEM_CTRLLER_BANK_SWIZZLE_DDR4,
  APCB_ID_CBS_CMN_MEM_ADDRESS_HASHING_DDR4,
  APCB_ID_CBS_CMN_MEM_SPEED_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TCL_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TRCDRD_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TRCDWR_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TRP_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TRAS_DDR4,
  APCB_ID_CBS_CMN_MEM_DATA_POISONING_DDR4,
  APCB_ID_CBS_CMN_MEM_ADDRESS_HASH_BANK_DDR4,
  APCB_ID_CBS_CMN_MEM_ADDRESS_HASH_CS_DDR4,
  APCB_ID_CBS_CMN_MEM_TSME_DDR4,
  APCB_ID_CBS_CMN_MEM_OVERCLOCK_DDR4,
  APCB_ID_CBS_CMN_GNB_GFX_UMA_MODE,
  APCB_ID_CBS_CMN_GNB_GFX_UMA_VERSION,
  APCB_ID_CBS_CMN_GNB_GFX_DISPLAY_RESOLUTION,
  APCB_ID_CBS_CMN_GNB_GFX_UMA_FRAME_BUFFER_SIZE,
  APCB_ID_CBS_CMN_GNB_SOC_OVERCLOCK_VID,
  APCB_ID_CBS_CMN_GNB_FCLK_OC_FLAG,
  APCB_ID_CBS_CMN_CPU_SEV_ASID_SPACE_LIMIT,
  APCB_ID_CBS_CMN_MEM_SPD_READ_OPTIMIZATION_DDR4,
  APCB_ID_CBS_CMN_MEM_MBIST_EN,
  APCB_ID_CBS_CMN_MEM_MBIST_SUB_TEST,
  APCB_ID_CBS_DF_CMN_MEM_CLEAR,
  APCB_ID_CBS_CMN_MEM_TIMING_TRC_CTRL_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TRC_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TRRD_S_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TRRD_L_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TFAW_CTRL_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TFAW_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TWTR_S_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TWTR_L_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TWR_CTRL_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TWR_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TRCPAGE_CTRL_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TRCPAGE_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TRDRD_SC_L_CTRL_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TRDRD_SC_L_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TWRWR_SC_L_CTRL_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TWRWR_SC_L_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TRFC_CTRL_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TRFC_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TRFC2_CTRL_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TRFC2_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TRFC4_CTRL_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TRFC4_DDR4,
  APCB_ID_CBS_CMN_MEM_OVERCLOCK_FAIL_CNT,
  APCB_ID_CBS_CMN_MEM_CTRLLER_PROC_ODT_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TRDRD_SC_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TRDRD_SD_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TRDRD_DD_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TWRWR_SC_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TWRWR_SD_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TWRWR_DD_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TRDWR_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TWRRD_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TRTP_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TCWL_DDR4,
  APCB_ID_CBS_CMN_MEM_DATA_BUS_CONFIG_CTL_DDR4,
  APCB_ID_CBS_CMN_MEM_CTRLLER_RTT_NOM_DDR4,
  APCB_ID_CBS_CMN_MEM_CTRLLER_RTT_WR_DDR4,
  APCB_ID_CBS_CMN_MEM_CTRLLER_RTT_PARK_DDR4,
  APCB_ID_CBS_CMN_MEM_TIMING_TCKE_DDR4,
  APCB_ID_CBS_CMN_MEM_CTRLLER_DATA_SCRAMBLE_DDR4,
  APCB_ID_CBS_CMN_CLDO_VDDP_CTL,
  APCB_ID_CBS_CMN_CLDOVDD_PVOLTAGE,
  APCB_ID_CBS_CMN_MEM_CTRLLER2_T_MODE_DDR4,
  APCB_ID_CBS_CMN_MEM_GEAR_DOWN_MODE_DDR4,
  APCB_ID_CBS_CMN_MEM_MBIST_AGGRESSORS,
  APCB_ID_CBS_CMN_MEM_MBIST_PER_BIT_SLAVE_DIE_REPORT,
  APCB_ID_CBS_CMN_MEM_CTRLLER_PWR_DN_EN_DDR4,
  APCB_ID_CBS_CMN_MEM_CAD_BUS_TIMING_CTL_DDR4,
  APCB_ID_CBS_CMN_MEM_ADDR_CMD_SETUP_DDR4,
  APCB_ID_CBS_CMN_MEM_CS_ODT_SETUP_DDR4,
  APCB_ID_CBS_CMN_MEM_CKE_SETUP_DDR4,
  APCB_ID_CBS_CMN_MEM_CAD_BUS_DRV_STREN_CTL_DDR4,
  APCB_ID_CBS_CMN_MEM_CAD_BUS_CLK_DRV_STREN_DDR4,
  APCB_ID_CBS_CMN_MEM_CAD_BUS_ADDR_CMD_DRV_STREN_DDR4,
  APCB_ID_CBS_CMN_MEM_CAD_BUS_CS_ODT_DRV_STREN_DDR4,
  APCB_ID_CBS_CMN_MEM_CAD_BUS_CKE_DRV_STREN_DDR4,
  APCB_ID_CBS_CPU_RELAXED_EDC_THROTTLING,
  APCB_ID_CBS_CMN_PERSISTENT_LOW,
  APCB_ID_CBS_CMN_PERSISTENT_HIGH,
  APCB_ID_CBS_CMN_PERSISTENT_SIZE_LOW,
  APCB_ID_CBS_CMN_PERSISTENT_SIZE_HIGH
} APCB_COMMON_CBS_EXT_ID;

typedef struct _TOKEN_PAIR {
  UINT16 ApcbCommonId;
  UINT16 ApcbToken;
} TOKEN_PAIR;

//
// DDR Post Package Repair
//
#define DRAM_POST_PKG_SOFT_REPAIR 0
#define DRAM_POST_PKG_HARD_REPAIR 1
#define DRAM_POST_PKG_VALID_REPAIR 1
#define DRAM_POST_PKG_INVALID_REPAIR 0

typedef struct _DRRP_REPAIR_ENTRY {
  UINT32 Bank:5;                           ///< [4:0] Bank {BG 3:2,BA 1:0}
  UINT32 RankMultiplier:3;                 ///< [7:5] Rank Multiplier
  UINT32 Device:5;                         ///< [12:8] Device width
                                           ///<        Device = 4, Repair DIMMs with device width x4 only
                                           ///<        Device = 8, Repair DIMMs with device width x8 only
                                           ///<        Device = 16, Repair DIMMs with device width x16 only
                                           ///<        Device = 0x1F - Ignore device width and repair specific devices based on TargetDevice
  UINT32 ChipSelect:2;                     ///< [14:13] ChipSelect
  UINT32 Column:10;                        ///< [24:15] Column Address
  UINT32 RepairType:1;                     ///< [25:25] Hard/soft repair (0=hard, 1=soft)
  UINT32 Valid:1;                          ///< [26:26] Valid entry
  UINT32 TargetDevice:5;                   ///< [31:27] Target Device (only valid if Device = 0x1F)
                                           ///<         x4 (0-17 (with ECC)), x8 (0-8 (with ECC))
  UINT32 Row:18;                           ///< [18:0] Row Address
  UINT32 Socket:3;                         ///< [21:19] Socket
  UINT32 Channel:3;                        ///< [21:19] Channel
  UINT32 Reserverd1:8;                     ///< pad to 32 bit
} DRRP_REPAIR_ENTRY;

#endif //_APCB_COMMON_H_




