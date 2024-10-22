/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
 
#ifndef _APCB_V3_TOKEN_UID_H_
#define _APCB_V3_TOKEN_UID_H_
//
// @APCB_START
//

//
// @APCB_TOKEN
//

/** BOOLEAN_TRUE_FALSE_VAL
*   Value range for boolean type with True and False usage
*/
typedef enum {
  _FALSE = 0,      ///< False
  _TRUE = 1,       ///< True
} BOOLEAN_TRUE_FALSE_VAL;


/** BOOLEAN_TRUE_FALSE_VAL
*   Value range for boolean type with Enable and Disable usage
*/
typedef enum {
  DISABLE = 0,      ///< Disable
  ENABLE = 1,       ///< Enable
} BOOLEAN_ENABLE_DISABLE_VAL;

//
// @APCB_TOKEN
//

// -------------------------------------------------------------------------------------------------
/// @GROUPID: APCB_GROUP_TOKEN
/// @TYPEID: APCB_TYPE_TOKEN_1BYTE
/// @SIZE: 1


//
// @APCB_TOKEN
//

/// @GROUPID: APCB_GROUP_TOKEN
/// @TYPEID: APCB_TYPE_TOKEN_2BYTES
/// @SIZE: 2
/// @DESC: PSP_SYSHUB_WDT_INTERVAL
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_PSP_SYSHUB_WDT_INTERVAL                            0xEDB5E4C9


//
// @APCB_TOKEN
//

/// @GROUPID: APCB_GROUP_TOKEN
/// @TYPEID: APCB_TYPE_TOKEN_1BYTE
/// @SIZE: 1
/// @DESC: CCX_SEV_ASID_COUNT
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CCX_SEV_ASID_COUNT                                 0x55876720

/// @DESC: XTRIG7_WORKAROUND
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_XTRIG7_WORKAROUND                                  0x2762E77B

/// @DESC: DF_CHAN_INTLV_HASH
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_DF_CHAN_INTLV_HASH                                 0x09299638

/// @DESC: MEM_MBIST_AGGRESSORS_CHNL
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_MEM_MBIST_AGGRESSORS_CHNL                          0xDCD1444A

/// @DESC: MEM_MBIST_PATTERN_LENGTH
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_MEM_MBIST_PATTERN_LENGTH                           0xAE7BAEDD

/// @DESC: MEM_MBIST_PATTERN_SELECT
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_MEM_MBIST_PATTERN_SELECT                           0xF527EBF8

/// @DESC: MEM_MBIST_AGGR_STATIC_LANE_SEL_L32
/// @RANGE: 0~0xFFFFFFFF = UITN32 Valid Range
  #define APCB_TOKEN_UID_MEM_MBIST_AGGR_STATIC_LANE_SEL_L32                 0x745218AD

/// @DESC: MEM_MBIST_AGGR_STATIC_LANE_SEL_U32
/// @RANGE: 0~0xFFFFFFFF = UITN32 Valid Range
  #define APCB_TOKEN_UID_MEM_MBIST_AGGR_STATIC_LANE_SEL_U32                 0xFAC9F48F

/// @DESC: MEM_MBIST_AGGR_STATIC_LANE_SEL_ECC
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_MEM_MBIST_AGGR_STATIC_LANE_SEL_ECC                 0x57122E99

/// @DESC: MEM_MBIST_AGGR_STATIC_LANE_VAL
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_MEM_MBIST_AGGR_STATIC_LANE_VAL                     0x4474D416

/// @DESC: MEM_MBIST_TGT_STATIC_LANE_SEL_L32
/// @RANGE: 0~0xFFFFFFFF = UITN32 Valid Range
  #define APCB_TOKEN_UID_MEM_MBIST_TGT_STATIC_LANE_SEL_L32                  0x81880D15

/// @DESC: MEM_MBIST_TGT_STATIC_LANE_SEL_U32
/// @RANGE: 0~0xFFFFFFFF = UITN32 Valid Range
  #define APCB_TOKEN_UID_MEM_MBIST_TGT_STATIC_LANE_SEL_U32                  0xAF669F33

/// @DESC: MEM_MBIST_TGT_STATIC_LANE_SEL_ECC
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_MEM_MBIST_TGT_STATIC_LANE_SEL_ECC                  0xA6E92CEE

/// @DESC: MEM_MBIST_TGT_STATIC_LANE_VAL
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_MEM_MBIST_TGT_STATIC_LANE_VAL                      0x4D7E0206

/// @DESC: MEM_MBIST_DATA_EYE_TYPE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_MEM_MBIST_DATA_EYE_TYPE                            0x4E2EDC1B

/// @DESC: MEM_MBIST_WORST_CASE_GRAN
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_MEM_MBIST_WORST_CASE_GRAN                          0x23B0B6A1

/// @DESC: MEM_MBIST_READ_DATA_EYE_VOLTAGE_STEP
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_MEM_MBIST_READ_DATA_EYE_VOLTAGE_STEP               0x35D6A4F8

/// @DESC: MEM_MBIST_READ_DATA_EYE_TIMING_STEP
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_MEM_MBIST_READ_DATA_EYE_TIMING_STEP                0x58CCD28A

/// @DESC: MEM_MBIST_WRITE_DATA_EYE_VOLTAGE_STEP
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_MEM_MBIST_WRITE_DATA_EYE_VOLTAGE_STEP              0xCDA61022

/// @DESC: MEM_MBIST_WRITE_DATA_EYE_TIMING_STEP
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_MEM_MBIST_WRITE_DATA_EYE_TIMING_STEP               0xD9025142

/// @DESC: FCH_CONSOLE_OUT_BASIC_ENABLE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_FCH_CONSOLE_OUT_BASIC_ENABLE                       0xA0903F98

/// @DESC: _DF_SYNC_FLOOD_PROP
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_DF_SYNC_FLOOD_PROP                                 0x49639134



// =================================================================================================
//
/// @APCB_TOKEN
//
// -------------------------------------------------------------------------------------------------
/// @GROUPID: APCB_GROUP_TOKEN
/// @TYPEID: APCB_TYPE_TOKEN_BOOLEAN
/// @SIZE: 1

/// @DESC: PSP TP Port control
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_PSP_TP_PORT                                        0x0460ABE8  // bool

/// @DESC: PSP_ERROR_DISPLAY
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_PSP_ERROR_DISPLAY                                  0xDC33FF21  // bool

/// @DESC: PSP_EVENT_LOG_DISPLAY
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_PSP_EVENT_LOG_DISPLAY                              0x0C473E1C  // bool

/// @DESC: PSP_STOP_ON_ERROR
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_PSP_STOP_ON_ERROR                                  0xE7024A21  // bool

/// @DESC: MEMHOLEREMAPPING
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_MEMHOLEREMAPPING                                   0x6A133AC5  // bool

/// @DESC: LIMITMEMORYTOBELOW1TB
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_LIMITMEMORYTOBELOW1TB                              0x5E71E6D8  // bool

/// @DESC: ENABLECHIPSELECTINTLV
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_ENABLECHIPSELECTINTLV                              0x6F81A115  // bool

/// @DESC: ENABLECHANNELINTLV
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_ENABLECHANNELINTLV                                 0x48254F73  // bool

/// @DESC: ENABLEECCFEATURE
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_ENABLEECCFEATURE                                   0xFA35F040  // bool

/// @DESC: ENABLEPOWERDOWN
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_ENABLEPOWERDOWN                                    0xBBB185A2  // bool

/// @DESC: ENABLEPARITY
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_ENABLEPARITY                                       0x3CB8CBD2  // bool

/// @DESC: ENABLEBANKSWIZZLE
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_ENABLEBANKSWIZZLE                                  0x6414D160  // bool

/// @DESC: ENABLEMEMCLR
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_ENABLEMEMCLR                                       0xC6ACDB37  // bool

/// @DESC: MEMRESTORECTL
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_MEMRESTORECTL                                      0xFEDB01F8  // bool

/// @DESC: ISCAPSULEMODE
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_ISCAPSULEMODE                                      0x96176308  // bool

/// @DESC: AMPENABLE
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_AMPENABLE                                          0x592CB3CA  // bool

/// @DESC: DRAMDOUBLEREFRESHRATE
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_DRAMDOUBLEREFRESHRATE                              0x44D40026  // bool

/// @DESC: ECCREDIRECTION
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_ECCREDIRECTION                                     0xDEDE0E09  // bool

/// @DESC: ECCSYNCFLOOD
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_ECCSYNCFLOOD                                       0x88BD40C2  // bool

/// @DESC: DQSTRAININGCONTROL
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_DQSTRAININGCONTROL                                 0x3CAAA3FA  // bool

/// @DESC: UMAABOVE4G
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_UMAABOVE4G                                         0x77E41D2A  // bool

/// @DESC: MEMORYALLCLOCKSON
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_MEMORYALLCLOCKSON                                  0xB95E0555  // bool

/// @DESC: IGNORESPDCHECKSUM
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_IGNORESPDCHECKSUM                                  0x7D369DBC  // bool

/// @DESC: MEMORYMODEUNGANGED
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_MEMORYMODEUNGANGED                                 0x03CE1180  // bool

/// @DESC: MEMORYQUADRANKCAPABLE
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_MEMORYQUADRANKCAPABLE                              0xE6DFD3DC  // bool

/// @DESC: MEMORYRDIMMCAPABLE
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_MEMORYRDIMMCAPABLE                                 0x81726666  // bool

/// @DESC: MEMORYLRDIMMCAPABLE
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_MEMORYLRDIMMCAPABLE                                0x014FBF20  // bool

/// @DESC: MEMORYUDIMMCAPABLE
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_MEMORYUDIMMCAPABLE                                 0x3CF8A8EC  // bool

/// @DESC: MEMORYSODIMMCAPABLE
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_MEMORYSODIMMCAPABLE                                0x7C61C187  // bool

/// @DESC: DIMMTYPEDDDR4CAPABLE
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_DIMMTYPEDDDR4CAPABLE                               0xF1C4BFBF  // bool

/// @DESC: DIMMTYPEDDDR3CAPABLE
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_DIMMTYPEDDDR3CAPABLE                               0x0789210C  // bool

/// @DESC: DIMMTYPELPDDDR3CAPABLE
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_DIMMTYPELPDDDR3CAPABLE                             0xAD96AA30  // bool

/// @DESC: ENABLEZQRESET
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_ENABLEZQRESET                                      0x541BF318  // bool

/// @DESC: ENABLEBANKGROUPSWAP
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_ENABLEBANKGROUPSWAP                                0x46920968  // bool

/// @DESC: ODTSCMDTHROTEN
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_ODTSCMDTHROTEN                                     0xC0736395  // bool

/// @DESC: SWCMDTHROTEN
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_SWCMDTHROTEN                                       0xA29C1CF9  // bool

/// @DESC: FORCEPWRDOWNTHROTEN
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_FORCEPWRDOWNTHROTEN                                0x10849D6C  // bool

/// @DESC: ENABLEMEMPSTATE
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_ENABLEMEMPSTATE                                    0x56B93947  // bool

/// @DESC: DDRROUTEBALANCEDTEE
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_DDRROUTEBALANCEDTEE                                0xE68C363D  // bool

/// @DESC: MEM_TEMP_CONTROLLED_REFRESH_EN
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_MEM_TEMP_CONTROLLED_REFRESH_EN                     0xF051E1C4  // bool

/// @DESC: ENABLEBANKGROUPSWAPALT
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_ENABLEBANKGROUPSWAPALT                             0xA89D1BE8  // bool

/// @DESC: UECC_RETRY_DDR4
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_UECC_RETRY_DDR4                                    0xBFF00125  // bool

/// @DESC: PCIE_RESET_CONTROL
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_PCIE_RESET_CONTROL                                 0xF7BB3451  // bool , data in x86, SSP code doesn't use.

/// @DESC: PCIE_RESET_PIN_SELECT
/// @RANGE: 0~0x4 = UITN8 Valid Range
  #define APCB_TOKEN_UID_PCIE_RESET_PIN_SELECT                              0x8C0B2DE9  // uint8

/// @DESC: DF_PROBE_FILTER_ENABLE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_DF_PROBE_FILTER                                    0x6597C573  // uint8

/// @DESC: MEM_MBIST_AGGR_STATIC_LANE_CTRL
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_MEM_MBIST_AGGR_STATIC_LANE_CTRL                    0x77E6F2C9

/// @DESC: MEM_AGESA_EXTENDED_MEM_TEST_EN
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_MEM_AGESA_EXTENDED_MEM_TEST_EN                     0x83FFCED3

/// @DESC: DRAMDOUBLEREFRESHRATEEN
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_DRAMDOUBLEREFRESHRATEEN                            0x974E8E7C  // unused

/// @DESC: CBS_CMN_MEM_CTRLLER_DRAM_ECC_EN_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_CTRLLER_DRAM_ECC_EN_DDR4               0x9C6D6993

/// @DESC: POST_PACKAGE_REPAIR_ENABLE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_POST_PACKAGE_REPAIR_ENABLE                         0xCDC03E4E

/// @DESC: PSP_ENABLE_DEBUG_MODE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_PSP_ENABLE_DEBUG_MODE                              0xD1091CD0  // uint8

/// @DESC: MEM_MBIST_TGT_STATIC_LANE_CTRL
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_MEM_MBIST_TGT_STATIC_LANE_CTRL                     0xE1CC135E

/// @DESC: MEM_TEMP_CONTROLLED_EXTENDED_REFRESH_EN
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_MEM_TEMP_CONTROLLED_EXTENDED_REFRESH_EN            0xF402F423

/// @DESC: DF_GMI2_REPLAY
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_DF_GMI2_REPLAY                                     0xDDBEC696

/// @DESC: CCX PPIN opt-in
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_CCX_PPIN_OPT_IN                                    0x6A6700FD  // bool

// -------------------------------------------------------------------------------------------------
/// @GROUPID: APCB_GROUP_TOKEN
/// @TYPEID: APCB_TYPE_TOKEN_1BYTE
/// @SIZE: 1

/// @DESC: DF_GMI_ENCRYPT
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_DF_GMI_ENCRYPT                                     0x08A45920  // uint8

/// @DESC: DF_XGMI_ENCRYPT
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_DF_XGMI_ENCRYPT                                    0x6BD32F1C  // uint8

/// @DESC: DF_SAVE_RESTORE_MEM_ENCRYPT
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_DF_SAVE_RESTORE_MEM_ENCRYPT                        0x7B3D1F75  // uint8

/// @DESC: DF_SYS_STORAGE_AT_TOP_OF_MEM
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_DF_SYS_STORAGE_AT_TOP_OF_MEM                       0x249E08D5  // uint8

/// @DESC: DF_BOTTOMIO
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_DF_BOTTOMIO                                        0x8FB98529  // uint8

/// @DESC: DIMMSENSORRESOLUTION
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_DIMMSENSORRESOLUTION                               0x831AF313  // uint8

/// @DESC: AUTOREFFINEGRANMODE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_AUTOREFFINEGRANMODE                                0x190305DF  // uint8

/// @DESC: SOLDERDOWNDRAM
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_SOLDERDOWNDRAM                                     0x5B8F861D  // uint8 , not used in SSP

/// @DESC: MEM_MBIST_SUBTEST_TYPE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_MEM_MBIST_SUBTEST_TYPE                             0x772A107C  // uint8

/// @DESC: MEM_MBIST_AGGRESOR_ON
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_MEM_MBIST_AGGRESOR_ON                              0x032361C4  // uint8

/// @DESC: MEM_CPU_VREF_RANGE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_MEM_CPU_VREF_RANGE                                 0x7627CB6D  // uint8

/// @DESC: MEM_DRAM_VREF_RANGE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_MEM_DRAM_VREF_RANGE                                0xA8769655  // uint8

/// @DESC: MEM_NVDIMM_POWER_SOURCE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_MEM_NVDIMM_POWER_SOURCE                            0x286D0075  // uint8

/// @DESC: MEM_PS_ERROR_HANDLING
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_MEM_PS_ERROR_HANDLING                              0x6C4CCF38  // uint8

/// @DESC: MEM_MBIST_PER_BIT_SLAVE_DIE_REPORT
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_MEM_MBIST_PER_BIT_SLAVE_DIE_REPORT                 0xCFF56411  // uint8

/// @DESC: BMC_SOCKET_NUMBER
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_BMC_SOCKET_NUMBER                                  0x846573F9  // uint8

/// @DESC: BMC_START_LANE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_BMC_START_LANE                                     0xB88D87DF  // uint8

/// @DESC: BMC_END_LANE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_BMC_END_LANE                                       0x143F3963  // uint8

/// @DESC: BMC_DEVICE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_BMC_DEVICE                                         0xD5BC5FC9  // uint8

/// @DESC: BMC_FUNCTION
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_BMC_FUNCTION                                       0x1DE4DD61  // uint8

/// @DESC: FCH_CONSOLE_OUT_ENABLE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_FCH_CONSOLE_OUT_ENABLE                             0xDDB759DA  // uint8

/// @DESC: FCH_CONSOLE_OUT_SERIAL_PORT
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_FCH_CONSOLE_OUT_SERIAL_PORT                        0xFFF9F34D  // uint8

/// @DESC: Data Fabric Memory Interleaving Control
/// @RANGE: DF_MEM_INTERLEAVING_VAL
  #define APCB_TOKEN_UID_DF_MEM_INTERLEAVING                                0xCE0176EF  // uint8

/// @DESC: MEM_DATA_POISON
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_MEM_DATA_POISON                                    0x48959473  // bool

/// @DESC: MEM_DATA_SCRAMBLE
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_MEM_DATA_SCRAMBLE                                  0x98ACA5B4  // bool

/// @DESC: MEM_MBIST_HALT_ON_ERROR
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_MEM_MBIST_HALT_ON_ERROR                            0xB1940F25  // bool

/// @DESC: MEM_TSME_ENABLE
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_MEM_TSME_ENABLE                                    0xD1FA6660  // bool

/// @DESC: MEM_MBIST_TEST_ENABLE
/// @RANGE: BOOLEAN_TRUE_FALSE_VAL
  #define APCB_TOKEN_UID_MEM_MBIST_TEST_ENABLE                              0xDF5502C8  // bool

#if 0
///         0 = DF_MEM_INTLV_NONE
///         1 = DF_MEM_INTLV_CHANNEL
///         2 = DF_MEM_INTLV_DIE
///         3 = DF_MEM_INTLV_SOCKET
  #define   DF_MEM_INTLV_SOCKET  3
  #define   DF_MEM_INTLV_DIE     2
  #define   DF_MEM_INTLV_CHANNEL 1
  #define   DF_MEM_INTLV_NONE    0
#endif
/** DF_MEM_INTERLEAVING_VAL
*  Value range for APCB_TOKEN_UID_DF_MEM_INTERLEAVING
*/
typedef enum {
  DF_MEM_INTLV_NONE     = 0,      ///< No interleave
  DF_MEM_INTLV_CHANNEL  = 1,      ///< Channel interleave
  DF_MEM_INTLV_DIE      = 2,      ///< Die interleave
  DF_MEM_INTLV_SOCKET   = 3,      ///< Socket interleave
  DF_MEM_INTLV_AUTO     = 7,      ///< Auto interleave
} DF_MEM_INTERLEAVING_VAL;

//#define macro(
//#define conts (A * B *C) ///< @VALUE:3

//
/// @APCB_TOKEN
//
/// @DESC: Data Fabric DRAM Interleave Size
///        Desc line2
/// @RANGE: DF_DRAM_INTLV_SIZE_VAL
  #define APCB_TOKEN_UID_DF_MEM_INTERLEAVING_SIZE                           0x2606C42E

#if 0
///         0 = DF_DRAM_INTLV_SIZE_256
///         1 = DF_DRAM_INTLV_SIZE_512
///         2 = DF_DRAM_INTLV_SIZE_1024
///         3 = DF_DRAM_INTLV_SIZE_2048
  #define   DF_DRAM_INTLV_SIZE_256   0
  #define   DF_DRAM_INTLV_SIZE_512   1
  #define   DF_DRAM_INTLV_SIZE_1024  2
  #define   DF_DRAM_INTLV_SIZE_2048  3
#endif
/** DF_DRAM_INTLV_SIZE_VAL
*  Value range for APCB_TOKEN_UID_DF_MEM_INTERLEAVING_SIZE
*/
typedef enum {
  DF_DRAM_INTLV_SIZE_256 = 0,     ///< Interleave size 256 bytes
  DF_DRAM_INTLV_SIZE_512 = 1,     ///< Interleave size 512 bytes
  DF_DRAM_INTLV_SIZE_1024 = 2,    ///< Interleave size 1024 bytes
  DF_DRAM_INTLV_SIZE_2048 = 3,    ///< Interleave size 2048 bytes
} DF_DRAM_INTLV_SIZE_VAL;


/// @DESC: DF_ENABLE_CHAN_INTLV_HASH
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_DF_ENABLE_CHAN_INTLV_HASH                          0x4951B181  // uint8

/// @DESC: DF_MEM_CLEAR
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_DF_MEM_CLEAR                                       0x9D177E57  // uint8 in code, uint8 and bool in data



//
/// @APCB_TOKEN
//
/// @DESC: Data Fabric DRAM Interleave Size
/// @RANGE: DF_DRAM_INTLV_SIZE_VAL
  #define APCB_TOKEN_UID_DF3_XGMI2_LINK_CFG                                 0xB0B6AD3E  // uint8

///         0 = DF_DRAM_INTLV_SIZE_256
///         1 = DF_DRAM_INTLV_SIZE_512
///         2 = DF_DRAM_INTLV_SIZE_1024
///         3 = DF_DRAM_INTLV_SIZE_2048
#define   DF3_XGMI2_LINK_CFG_2LINK  0
#define   DF3_XGMI2_LINK_CFG_3LINK  1
#define   DF3_XGMI2_LINK_CFG_4LINK  2

/// @DESC: UMAMODE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_UMAMODE                                            0x1FB35295  // uint8

/// @DESC: FORCETRAINMODE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_FORCETRAINMODE                                     0x95744B84  // uint8

/// @DESC: DIMMTYPEUSEDINMIXEDCONFIG
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_DIMMTYPEUSEDINMIXEDCONFIG                          0xFAB5C903  // uint8

/// @DESC: PMUTRAINMODE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_PMUTRAINMODE                                       0xBD4A6AFC  // uint8

/// @DESC: ODTSCMDTHROTCYC
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_ODTSCMDTHROTCYC                                    0x69318E90  // uint8

/// @DESC: SWCMDTHROTCYC
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_SWCMDTHROTCYC                                      0xDCEC8FCB  // uint8



// -------------------------------------------------------------------------------------------------
/// @GROUPID: APCB_GROUP_TOKEN
/// @TYPEID: APCB_TYPE_TOKEN_2BYTES
/// @SIZE: 2

/// @DESC: BOTTOMIO
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_BOTTOMIO                                           0x092F4F14  // uint16

/// @DESC: SCRUBDRAMRATE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_SCRUBDRAMRATE                                      0x9ADDDD6B  // uint16

/// @DESC: SCRUBL2RATE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_SCRUBL2RATE                                        0x2266C144  // uint16

/// @DESC: SCRUBL3RATE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_SCRUBL3RATE                                        0xC0279AE0  // uint16

/// @DESC: SCRUBICRATE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_SCRUBICRATE                                        0x99639EE4  // uint16

/// @DESC: SCRUBDCRATE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_SCRUBDCRATE                                        0xB398DAA0  // uint16

/// @DESC: ECCSYMBOLSIZE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_ECCSYMBOLSIZE                                      0x302D5C04  // uint16

/// @DESC: DIMMSENSORCONF
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_DIMMSENSORCONF                                     0x51E7B610  // uint16

/// @DESC: DIMMSENSORUPPER
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_DIMMSENSORUPPER                                    0xB5AF557A  // uint16

/// @DESC: DIMMSENSORLOWER
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_DIMMSENSORLOWER                                    0xC5EA38A0  // uint16

/// @DESC: DIMMSENSORCRITICAL
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_DIMMSENSORCRITICAL                                 0x38E9BF5D  // uint16

/// @DESC: TEST
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_TEST                                               0x11D0001E  // uint16

// -------------------------------------------------------------------------------------------------
/// @GROUPID: APCB_GROUP_TOKEN
/// @TYPEID: APCB_TYPE_TOKEN_4BYTES
/// @SIZE: 4

/// @DESC: CCX_MIN_SEV_ASID
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CCX_MIN_SEV_ASID                                   0xA7C33753  // uint32

/// @DESC: DF_PCI_MMIO_SIZE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_DF_PCI_MMIO_SIZE                                   0x3D9B7D7B  // uint32

/// @DESC: DF_CAKE_CRC_THRESH_PERF_BOUNDS
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_DF_CAKE_CRC_THRESH_PERF_BOUNDS                     0x9258CF45  // uint32

/// @DESC: USERTIMINGMODE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_USERTIMINGMODE                                     0xFC560D7D  // uint32

/// @DESC: MEMCLOCKVALUE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_MEMCLOCKVALUE                                      0xCC83F65F  // uint32

/// @DESC: UMASIZE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_UMASIZE                                            0x37B1F8CF  // uint32

/// @DESC: UMAALIGNMENT
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_UMAALIGNMENT                                       0x57DDF512  // uint32

/// @DESC: MEMORYBUSFREQUENCYLIMIT
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_MEMORYBUSFREQUENCYLIMIT                            0x34970A3C  // uint32

/// @DESC: POWERDOWNMODE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_POWERDOWNMODE                                      0x23DD2705  // uint32




// CBS CMN APCB Tokens

// =================================================================================================
//
/// @APCB_TOKEN
//
// -------------------------------------------------------------------------------------------------
/// @GROUPID: APCB_GROUP_TOKEN
/// @TYPEID: APCB_TYPE_TOKEN_BOOLEAN
/// @SIZE: 1

/// @DESC: CBS_CMN_MEM_MAPPING_BANK_INTERLEAVE_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_MAPPING_BANK_INTERLEAVE_DDR4           0x77A2B63A  // bool in code, uint8 in x86 data

/// @DESC: CBS_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_DDR4           0x229EDFE4  // bool in code, uint8 in x86 data

/// @DESC: CBS_CMN_MEM_ADDRESS_HASH_BANK_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_ADDRESS_HASH_BANK_DDR4                 0x0EF08D04  // bool in code, uint8 in x86 data

/// @DESC: CBS_CMN_MEM_ADDRESS_HASH_CS_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_ADDRESS_HASH_CS_DDR4                   0xA01AB8A0  // bool in code, uint8 in x86 data

/// @DESC: CBS_CMN_MEM_SPD_READ_OPTIMIZATION_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_SPD_READ_OPTIMIZATION_DDR4             0x8D3AB10E  // bool in code, uint8 in x86 data

/// @DESC: CBS_CMN_MEM_MBIST_EN
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_MBIST_EN                               0x0D0AE86F  // uint8

/// @DESC: CBS_CMN_MEM_UECC_RETRY_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_UECC_RETRY_DDR4                        0x42216E4B  // Duplicate with APCB_TOKEN_UID_UECC_RETRY_DDR4

/// @DESC: CBS_CMN_MEM_DATA_POISONING_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_DATA_POISONING_DDR4                    0x598E8661  // uint8 in x86 data, not used in code

// -------------------------------------------------------------------------------------------------
/// @GROUPID: APCB_GROUP_TOKEN
/// @TYPEID: APCB_TYPE_TOKEN_1BYTE
/// @SIZE: 1

/// @DESC: CBS_CMN_MEM_CTRLLER_BANK_SWIZZLE_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_CTRLLER_BANK_SWIZZLE_DDR4              0xA701EEF7  // uint8 in x86 data

/// @DESC: CBS_CMN_MEM_ADDRESS_HASHING_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_ADDRESS_HASHING_DDR4                   0xDB22DC8F  // uint8 in x86 data

/// @DESC: CBS_CMN_MEM_SPEED_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_SPEED_DDR4                             0xE0604CE9  // uint8

/// @DESC: CBS_CMN_MEM_TIMING_TCL_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TCL_DDR4                        0xB2D12C4E  // uint8

/// @DESC: CBS_CMN_MEM_TIMING_TRCDRD_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TRCDRD_DDR4                     0xB22E12F7  // uint8

/// @DESC: CBS_CMN_MEM_TIMING_TRCDWR_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TRCDWR_DDR4                     0xB3840F07  // uint8

/// @DESC: CBS_CMN_MEM_TIMING_TRP_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TRP_DDR4                        0xA2D655C8  // uint8

/// @DESC: CBS_CMN_MEM_TIMING_TRAS_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TRAS_DDR4                       0x03E98A84  // uint8

/// @DESC: CBS_CMN_MEM_TSME_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TSME_DDR4                              0xBC3F4EED  // uint8 in x86 data, not used in code

/// @DESC: CBS_CMN_MEM_OVERCLOCK_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_OVERCLOCK_DDR4                         0x593292BE  // uint8 in x86 data, parent control in code

/// @DESC: CBS_CMN_GNB_GFX_UMA_MODE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_GNB_GFX_UMA_MODE                           0xAC3A0B3D  // uint8 in x86 data, not used in SSP

/// @DESC: CBS_CMN_GNB_GFX_UMA_VERSION
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_GNB_GFX_UMA_VERSION                        0xF27223BF  // uint8 in x86 data, not used in SSP

/// @DESC: CBS_CMN_GNB_GFX_DISPLAY_RESOLUTION
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_GNB_GFX_DISPLAY_RESOLUTION                 0x8BEA3A1A  // uint8 in x86 data, not used in SSP

/// @DESC: CBS_CMN_MEM_TIMING_TRC_CTRL_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TRC_CTRL_DDR4                   0xBBC7849A  // uint8 in x86 data, not used in code

/// @DESC: CBS_CMN_MEM_TIMING_TRC_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TRC_DDR4                        0x5B082A2B  // uint8 in data

/// @DESC: CBS_CMN_MEM_TIMING_TRRD_S_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TRRD_S_DDR4                     0x50B2D463  // uint8

/// @DESC: CBS_CMN_MEM_TIMING_TRRD_L_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TRRD_L_DDR4                     0xFC23535E  // uint8

/// @DESC: CBS_CMN_MEM_TIMING_TFAW_CTRL_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TFAW_CTRL_DDR4                  0xB36B7473  // uint8 in x86 data, used as 2nd level parent control in code

/// @DESC: CBS_CMN_MEM_TIMING_TFAW_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TFAW_DDR4                       0xDE1D6178  // uint8

/// @DESC: CBS_CMN_MEM_TIMING_TWTR_S_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TWTR_S_DDR4                     0xEB6A9FE9  // uint8

/// @DESC: CBS_CMN_MEM_TIMING_TWTR_L_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TWTR_L_DDR4                     0x5782D479  // uint8

/// @DESC: CBS_CMN_MEM_TIMING_TWR_CTRL_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TWR_CTRL_DDR4                   0x5C26F15B  // uint8

/// @DESC: CBS_CMN_MEM_TIMING_TWR_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TWR_DDR4                        0x4FC9F17E  // uint8

/// @DESC: CBS_CMN_MEM_TIMING_TRCPAGE_CTRL_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TRCPAGE_CTRL_DDR4               0x2BC2CA69  // uint8

/// @DESC: CBS_CMN_MEM_TIMING_TRCPAGE_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TRCPAGE_DDR4                    0x1A82F1F6  // uint8 (new data), uint16 (old data in x86), uint8 in code.

/// @DESC: CBS_CMN_MEM_TIMING_TRDRD_SC_L_CTRL_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TRDRD_SC_L_CTRL_DDR4            0xC71A49AA  // uint8 in x86 data, 2nd level parent control in code

/// @DESC: CBS_CMN_MEM_TIMING_TRDRD_SC_L_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TRDRD_SC_L_DDR4                 0x6A594A63  // uint8

/// @DESC: CBS_CMN_MEM_TIMING_TWRWR_SC_L_CTRL_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TWRWR_SC_L_CTRL_DDR4            0x91371469  // uint8 , 2nd level parent control in code

/// @DESC: CBS_CMN_MEM_TIMING_TWRWR_SC_L_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TWRWR_SC_L_DDR4                 0xA148AB5F  // uint8 in x86 data, uint32 varible in code

/// @DESC: CBS_CMN_MEM_TIMING_TRFC_CTRL_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TRFC_CTRL_DDR4                  0x86ED48B5  // uint8 , 2nd level parent control in code

/// @DESC: CBS_CMN_MEM_TIMING_TRFC2_CTRL_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TRFC2_CTRL_DDR4                 0x6D8E4884  // uint8 , 2nd level parent control in code

/// @DESC: CBS_CMN_MEM_TIMING_TRFC4_CTRL_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TRFC4_CTRL_DDR4                 0xFBB240FD  // uint8 , 2nd level parent control in code

/// @DESC: CBS_CMN_MEM_OVERCLOCK_FAIL_CNT
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_OVERCLOCK_FAIL_CNT                     0x92FECF1C  // uint8

/// @DESC: CBS_CMN_MEM_CTRLLER_PROC_ODT_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_CTRLLER_PROC_ODT_DDR4                  0xE74CFD9B  // uint8


// -------------------------------------------------------------------------------------------------
/// @GROUPID: APCB_GROUP_TOKEN
/// @TYPEID: APCB_TYPE_TOKEN_2BYTES
/// @SIZE: 2

/// @DESC: CBS_CMN_MEM_TIMING_TRFC_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TRFC_DDR4                       0x49714C55  // uint16

/// @DESC: CBS_CMN_MEM_TIMING_TRFC2_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TRFC2_DDR4                      0xEF6CF7B3  // uint16

/// @DESC: CBS_CMN_MEM_TIMING_TRFC4_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TRFC4_DDR4                      0x4A05C5E2  // uint16




// -------------------------------------------------------------------------------------------------
/// @GROUPID: APCB_GROUP_TOKEN
/// @TYPEID: APCB_TYPE_TOKEN_4BYTES
/// @SIZE: 4



/// @DESC: CBS_CMN_GNB_GFX_UMA_FRAME_BUFFER_SIZE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_GNB_GFX_UMA_FRAME_BUFFER_SIZE              0xD3C40980  // uint32 in x86 data, not used in SSP

/// @DESC: CBS_CMN_MEM_MBIST_SUB_TEST
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_MBIST_SUB_TEST                         0x5BFB50B8  // uint in x86 data, not used in code



// Below token need to be classified by type ID and size

/// @DESC: CBS_CMN_MEM_TIMING_TRDRD_SC_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TRDRD_SC_DDR4                   0x2E199CAA

/// @DESC: CBS_CMN_MEM_TIMING_TRDRD_SD_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TRDRD_SD_DDR4                   0x6AD7647F

/// @DESC: CBS_CMN_MEM_TIMING_TRDRD_DD_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TRDRD_DD_DDR4                   0xA1E46AFC

/// @DESC: CBS_CMN_MEM_TIMING_TWRWR_SC_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TWRWR_SC_DDR4                   0x4DE9DD1F

/// @DESC: CBS_CMN_MEM_TIMING_TWRWR_SD_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TWRWR_SD_DDR4                   0xA95E4B3B

/// @DESC: CBS_CMN_MEM_TIMING_TWRWR_DD_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TWRWR_DD_DDR4                   0x9E720EC8

/// @DESC: CBS_CMN_MEM_TIMING_TRDWR_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TRDWR_DDR4                      0x7636900E

/// @DESC: CBS_CMN_MEM_TIMING_TWRRD_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TWRRD_DDR4                      0x8FD5A3D2

/// @DESC: CBS_CMN_MEM_TIMING_TRTP_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TRTP_DDR4                       0x88B9F33B

/// @DESC: CBS_CMN_MEM_TIMING_TCWL_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TCWL_DDR4                       0xBDBA463D

/// @DESC: CBS_CMN_MEM_DATA_BUS_CONFIG_CTL_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_DATA_BUS_CONFIG_CTL_DDR4               0x9CC11046

/// @DESC: CBS_CMN_MEM_CTRLLER_RTT_NOM_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_CTRLLER_RTT_NOM_DDR4                   0xBFFF404B

/// @DESC: CBS_CMN_MEM_CTRLLER_RTT_WR_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_CTRLLER_RTT_WR_DDR4                    0xA80353D7

/// @DESC: CBS_CMN_MEM_CTRLLER_RTT_PARK_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_CTRLLER_RTT_PARK_DDR4                  0x58BF5D3E

/// @DESC: CBS_CMN_MEM_TIMING_TCKE_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_TIMING_TCKE_DDR4                       0x5EF92FB0

/// @DESC: CBS_CMN_MEM_CTRLLER_DATA_SCRAMBLE_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_CTRLLER_DATA_SCRAMBLE_DDR4             0x6301BD18

/// @DESC: CBS_CMN_CLDO_VDDP_CTL
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_CLDO_VDDP_CTL                              0xE033C709

/// @DESC: CBS_CMN_CLDOVDD_PVOLTAGE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_CLDOVDD_PVOLTAGE                           0x7915D708

/// @DESC: CBS_CMN_MEM_CTRLLER2_T_MODE_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_CTRLLER2_T_MODE_DDR4                   0x0677966C

/// @DESC: CBS_CMN_MEM_GEAR_DOWN_MODE_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_GEAR_DOWN_MODE_DDR4                    0x68444D72

/// @DESC: CBS_CMN_MEM_MBIST_AGGRESSORS
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_MBIST_AGGRESSORS                       0xD98B8639

/// @DESC: CBS_CMN_MEM_MBIST_PER_BIT_SLAVE_DIE_REPORT
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_MBIST_PER_BIT_SLAVE_DIE_REPORT         0xF4943BAD

/// @DESC: CBS_CMN_MEM_CTRLLER_PWR_DN_EN_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_CTRLLER_PWR_DN_EN_DDR4                 0x2D3BB9D8

/// @DESC: CBS_CMN_MEM_CAD_BUS_TIMING_CTL_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_CAD_BUS_TIMING_CTL_DDR4                0x264B7625

/// @DESC: CBS_CMN_MEM_ADDR_CMD_SETUP_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_ADDR_CMD_SETUP_DDR4                    0x1E974F51

/// @DESC: CBS_CMN_MEM_CS_ODT_SETUP_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_CS_ODT_SETUP_DDR4                      0x3B1B99E9

/// @DESC: CBS_CMN_MEM_CKE_SETUP_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_CKE_SETUP_DDR4                         0x37D13B99

/// @DESC: CBS_CMN_MEM_CAD_BUS_DRV_STREN_CTL_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_CAD_BUS_DRV_STREN_CTL_DDR4             0xDB047BEB

/// @DESC: CBS_CMN_MEM_CAD_BUS_CLK_DRV_STREN_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_CAD_BUS_CLK_DRV_STREN_DDR4             0xD8ADC426

/// @DESC: CBS_CMN_MEM_CAD_BUS_ADDR_CMD_DRV_STREN_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_CAD_BUS_ADDR_CMD_DRV_STREN_DDR4        0xF91E8072

/// @DESC: CBS_CMN_MEM_CAD_BUS_CS_ODT_DRV_STREN_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_CAD_BUS_CS_ODT_DRV_STREN_DDR4          0x7BBE9AF1

/// @DESC: CBS_CMN_MEM_CAD_BUS_CKE_DRV_STREN_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_CAD_BUS_CKE_DRV_STREN_DDR4             0x1014364F

/// @DESC: CBS_CPU_RELAXED_EDC_THROTTLING
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CPU_RELAXED_EDC_THROTTLING                     0x7AE392CE

/// @DESC: CBS_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_ALT_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_ALT_DDR4       0xAE6507DA

/// @DESC: CBS_DF_CMN_SYNC_FLOOD_PROP
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_DF_CMN_SYNC_FLOOD_PROP                         0x55CE1C39

/// @DESC: CBS_CMN_MEM_OVERCLOCK_DIMM_CONFIG_CHECK
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_OVERCLOCK_DIMM_CONFIG_CHECK            0x3EE00E21

/// @DESC: CBS_CMN_MEM_CTRLLER_DRAM_ECC_SYMBOL_SIZE_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_CTRLLER_DRAM_ECC_SYMBOL_SIZE_DDR4      0x325AEF95

/// @DESC: CBS_CMN_CPU_SMU_PSP_PROD_MODE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_CPU_SMU_PSP_PROD_MODE                      0x8C341AAF


// Below are new added from 0030 to 0050RC3 base.

// -------------------------------------------------------------------------------------------------
/// @GROUPID: APCB_GROUP_TOKEN
/// @TYPEID: APCB_TYPE_TOKEN_1BYTE
/// @SIZE: 1

/// @DESC: CBS_DF_CMN_CAKE_CRC_PERF_BOUNDS
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_DF_CMN_CAKE_CRC_PERF_BOUNDS                    0x22D1B1EE

/// @DESC: CBS_CMN_MEM_MBIST_TESTMODE
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_MBIST_TESTMODE                         0x4D0C053F

/// @DESC: CCX_P0_SETTING
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CCX_P0_SETTING                                     0x83CF6A10

/// @DESC: CCX_P0_FID
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CCX_P0_FID                                         0x6B492DDA

/// @DESC: CCX_P0_DID
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CCX_P0_DID                                         0x09C5BCCC

/// @DESC: CCX_P0_VID
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CCX_P0_VID                                         0x65370189

/// @DESC: CBS_CMN_MEM_ADDRESS_HASH_RM_DDR4
/// @RANGE: 0~0xFF = UITN8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_ADDRESS_HASH_RM_DDR4                   0x50A6D6CA





// ==============================
// New Added

  #define APCB_TOKEN_UID_DF_DRAM_NPS                                        0x2CF3DAC9

  #define APCB_TOKEN_UID_DF_INVERT_DRAM_MAP                                 0x6574B2C0

  #define APCB_TOKEN_UID_CBS_CMN_MEM_AGESA_EXTENDED_MEMORY_TEST             0x7256F5C0

/// @DESC: 3LINK_XGMI_MAX_SPEED
/// @RANGE: 0~0xFF = UINT8 Valid Range
  #define APCB_TOKEN_UID_DF_3LINK_MAX_XGMI_SPEED                            0x53BA449B

/// @DESC: 4LINK_XGMI_MAX_SPEED
/// @RANGE: 0~0xFF = UINT8 Valid Range
  #define APCB_TOKEN_UID_DF_4LINK_MAX_XGMI_SPEED                            0x3F307CB3

// debug items
  #define APCB_TOKEN_UID_CBS_DBG_MEM_EARLY_PAGE_ACTIVATE_DDR4               0x3D1FB3FF

  #define APCB_TOKEN_UID_CPU_SYNC_FLOOD_ON_MCA                              0x96664127

  #define APCB_TOKEN_UID_DF_SPF_WAY_CTRL                                    0x71094904

/// @DESC: MEM_DIS_MEM_ERR_INJ
/// @RANGE: BOOLEAN_ENABLE_1_DISABLE_0_VAILDVAL_INTERNAL
  #define APCB_TOKEN_UID_CBS_CMN_MEM_DIS_MEM_ERR_INJ                        0xFF756025

/// @DESC: MEM_RCD_PARITY_DDR4
/// @RANGE: BOOLEAN_ENABLE_1_DISABLE_0_VAILDVAL_INTERNAL
  #define APCB_TOKEN_UID_CBS_CMN_MEM_RCD_PARITY_DDR4                        0x7E23FE78

/// @DESC: MEM_ADDR_CMD_PARITY_RETRY_DDR4
/// @RANGE: BOOLEAN_ENABLE_1_DISABLE_0_VAILDVAL_INTERNAL
  #define APCB_TOKEN_UID_CBS_CMN_MEM_ADDR_CMD_PARITY_RETRY_DDR4             0xBE8BEBCE

/// @DESC: MEM_ADDR_CMD_PARITY_ERROR_MAX_REPLAY_D
/// @RANGE: 0x00~0x3F = UINT8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_ADDR_CMD_PARITY_ERROR_MAX_REPLAY_DDR4  0x04E6A482

/// @DESC: MEM_CTRLLER_WR_CRC_EN_DDR4
/// @RANGE: BOOLEAN_ENABLE_1_DISABLE_0_VAILDVAL_INTERNAL
  #define APCB_TOKEN_UID_CBS_CMN_MEM_CTRLLER_WR_CRC_EN_DDR4                 0x94451A4B

/// @DESC: MEM_WRITE_CRC_RETRY_DDR4
/// @RANGE: BOOLEAN_ENABLE_1_DISABLE_0_VAILDVAL_INTERNAL
  #define APCB_TOKEN_UID_CBS_CMN_MEM_WRITE_CRC_RETRY_DDR4                   0x25FB6EA6

/// @DESC: MEM_WRITE_CRC_ERROR_MAX_REPLAY_DDR4
/// @RANGE: 0x00~0x3F = UINT8 Valid Range
  #define APCB_TOKEN_UID_CBS_CMN_MEM_WRITE_CRC_ERROR_MAX_REPLAY_DDR4        0x74A08BEC

#endif // _APCB_V3_TOKEN_UID_H_

