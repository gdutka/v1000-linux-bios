/* $NoKeywords:$ */
/**
 * @file
 *
 * mm.h
 *
 * Common main functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
 * @e \$Revision: 322891 $ @e \$Date: 2015-07-20 15:13:49 +0800 (Mon, 20 Jul 2015) $
 *
 **/
/*****************************************************************************
  *
  * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
  *
  *******************************************************************************
  *
 */

#ifndef _MM_H_
#define _MM_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */

#define ALLOC_SOCKET_STRUCT_HANDLE    0
#define ALLOC_DIE_STRUCT_HANDLE       1
#define ALLOC_DCT_STRUCT_HANDLE       2
#define ALLOC_CHL_STRUCT_HANDLE       3
#define ALLOC_PLATFORM_PTR_HANDLE     4
#define ALLOC_FORM_FACTOR_HANDLE      5
#define ALLOC_TRN_DATA_HANDLE         6
#define ALLOC_DIMM_DATA_HANDLE        7
#define ALLOC_PAR_TRN_HANDLE          8
#define ALLOC_NB_REG_TABLE            9
#define ALLOC_DEVICE_INFO             10
#define ALLOC_DATAEYES_HANDLE         11

#define GENERATE_MEM_HANDLE(type, x, y, z) (\
  AMD_MEM_MISC_HANDLES_START + (((type) << 18) + ((x) << 12) + ((y) << 6) + (z)) \
)

/// Heap handle for each supported family's NB register table
typedef enum {
  NbRegTabDR,           ///< Heap handle for DR NB register table
  NbRegTabDA,           ///< Heap handle for DA NB register table
  NbRegTabC32,          ///< Heap handle for C32 NB register table
  NbRegTabHY,           ///< Heap handle for HY NB register table
  NbRegTabKR,           ///< Heap handle for KR NB register table
  NbRegTabLN,           ///< Heap handle for LN NB register table
  NbRegTabON,           ///< Heap handle for ON NB register table
  NbRegTabOR,           ///< Heap handle for OR NB register table
  NbRegTabTN,           ///< Heap handle for TN NB register table
  NbRegTabKV,           ///< Heap handle for KV NB register table
  NbRegTabKB,           ///< Heap handle for KB NB register table
  NbRegTabML,           ///< Heap handle for ML NB register table
  NbRegTabCZ,           ///< Heap handle for CZ NB register table
  NbRegTabST,           ///< Heap handle for ST NB register table
  NumberOfNbRegTables   ///< Number of families that have NB register tables
} NB_REG_TAB_HANDLE;


//
// DDR3 VDDIO Range
//
#define VOLT1_5_ENCODED_VAL             0
#define VOLT1_35_ENCODED_VAL            1
#define VOLT1_25_ENCODED_VAL            2
//
// DDR4 VDDIO Range
//
#define VOLT1_2_ENCODED_VAL             0
#define VOLT1_TBD1_ENCODED_VAL          1
#define VOLT1_TBD2_ENCODED_VAL          2

#define VOLT_DDR3_ENCODED_LIMIT         3  // Maximum Encoded value for DDR3
#define VOLT_DDR4_ENCODED_LIMIT         1  // Maximum Encoded value for DDR4, change to support additional voltages.
#define VOLT_STARTING_ENCODED_VAL       0  // Lowest value for all technologies
#define VOLT1_5_MILLIVOLTS 1500
#define VOLT1_35_MILLIVOLTS 1350
#define VOLT1_25_MILLIVOLTS 1250
#define VOLT1_2_MILLIVOLTS 1200
#define VOLT_UNSUPPORTED_ENCODED_VAL  0xFF
//
// Macro to display Voltage String
//
#define GET_VDDIO_STRING(Vddio, TechType) (\
  (TechType == DDR3_TECHNOLOGY) ?\
  ((Vddio == VOLT1_5) ?  "1.5V" :\
  ((Vddio == VOLT1_35) ? "1.35V" :\
  ((Vddio == VOLT1_25) ? "1.25V" :\
    "Unsupported Voltage"))) :\
  (TechType == DDR4_TECHNOLOGY) ?\
  ((Vddio == VOLT1_2) ? "1.2V" :\
    "Unsupported Voltage") :\
    "Unsupported Voltage")

#define GET_ENCODED_VDDIO_STRING(EncodedVal, TechType) (\
  (TechType == DDR3_TECHNOLOGY) ?\
  ((EncodedVal == VOLT1_5_ENCODED_VAL) ?  "1.5V" :\
  ((EncodedVal == VOLT1_35_ENCODED_VAL) ? "1.35V" :\
  ((EncodedVal == VOLT1_25_ENCODED_VAL) ? "1.25V" :\
    "Unsupported Voltage"))) :\
  (TechType == DDR4_TECHNOLOGY) ?\
  ((EncodedVal == VOLT1_2_ENCODED_VAL) ?  "1.2V" :\
    "Unsupported Voltage") :\
    "Unsupported Voltage")
//
// Macro to determine the limit value for each DRAM type (for looping)
//
#define GET_ENCODED_VOLTAGE_LIMIT(TechType) (\
  (TechType == DDR3_TECHNOLOGY) ?\
    VOLT_DDR3_ENCODED_LIMIT :\
  (TechType == DDR4_TECHNOLOGY) ?\
    VOLT_DDR4_ENCODED_LIMIT :\
    VOLT_STARTING_ENCODED_VAL)
//
// Conversion Macro from Interface DDRVoltage value to internal index
//
#define CONVERT_VDDIO_TO_ENCODED(VddIo, TechType) (\
  (TechType == DDR3_TECHNOLOGY) ?\
  ((VddIo == VOLT1_5) ? VOLT1_5_ENCODED_VAL :\
  ((VddIo == VOLT1_35) ? VOLT1_35_ENCODED_VAL :\
  ((VddIo == VOLT1_25) ? VOLT1_25_ENCODED_VAL :\
    VOLT_UNSUPPORTED_ENCODED_VAL))):\
  (TechType == DDR4_TECHNOLOGY) ?\
  ((VddIo == VOLT1_2) ? VOLT1_2_ENCODED_VAL :\
    VOLT_UNSUPPORTED_ENCODED_VAL) :\
    VOLT_UNSUPPORTED_ENCODED_VAL)
//
// Conversion Macro from internal index to Interface DDRVoltage
//
#define CONVERT_ENCODED_TO_VDDIO(EncodedVal, TechType) (\
  (TechType == DDR3_TECHNOLOGY) ?\
  ((EncodedVal == VOLT1_5_ENCODED_VAL) ? VOLT1_5 :\
  ((EncodedVal == VOLT1_35_ENCODED_VAL) ? VOLT1_35 :\
  ((EncodedVal == VOLT1_25_ENCODED_VAL) ? VOLT1_25 :\
    VOLT_UNSUPPORTED))) :\
  (TechType == DDR4_TECHNOLOGY) ?\
  ((EncodedVal == VOLT1_2_ENCODED_VAL) ? VOLT1_2 :\
    VOLT_UNSUPPORTED) :\
    VOLT_UNSUPPORTED)
//
// Conversion Macro from internal index to Interface DDRVoltage in Millivolts
//
#define CONVERT_ENCODED_TO_VDDIO_MILLIVOLTS(EncodedVal, TechType) (\
  (TechType == DDR3_TECHNOLOGY) ?\
  ((EncodedVal == VOLT1_5_ENCODED_VAL) ? VOLT1_5_MILLIVOLTS :\
  ((EncodedVal == VOLT1_35_ENCODED_VAL) ? VOLT1_35_MILLIVOLTS :\
  ((EncodedVal == VOLT1_25_ENCODED_VAL) ? VOLT1_25_MILLIVOLTS :\
    0))) :\
  (TechType == DDR4_TECHNOLOGY) ?\
  ((EncodedVal == VOLT1_2_ENCODED_VAL) ? VOLT1_2_MILLIVOLTS :\
  0) :\
  0)

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

/// Bit field names used in memory initialization
typedef enum {
  BFDevVendorIDReg,                     ///< Bit field DevVendorIDReg
  BFNodeID,                             ///< Bit field NodeID
  BFNodeCnt,                            ///< Bit field NodeCnt

  BFSmcindIndex,                        ///< Bit field SmcindIndex
  BFSmcindData,                         ///< Bit field SmcindData

  BFDramBaseReg0,                       ///< Bit field DramBaseReg0
  BFDramBaseReg1,                       ///< Bit field DramBaseReg1
  BFDramBaseReg2,                       ///< Bit field DramBaseReg2
  BFDramBaseReg3,                       ///< Bit field DramBaseReg3
  BFDramBaseReg4,                       ///< Bit field DramBaseReg4
  BFDramBaseReg5,                       ///< Bit field DramBaseReg5
  BFDramBaseReg6,                       ///< Bit field DramBaseReg6
  BFDramBaseReg7,                       ///< Bit field DramBaseReg7

  BFDramLimitReg0,                      ///< Bit field DramLimitReg0
  BFDramLimitReg1,                      ///< Bit field DramLimitReg1
  BFDramLimitReg2,                      ///< Bit field DramLimitReg2
  BFDramLimitReg3,                      ///< Bit field DramLimitReg3
  BFDramLimitReg4,                      ///< Bit field DramLimitReg4
  BFDramLimitReg5,                      ///< Bit field DramLimitReg5
  BFDramLimitReg6,                      ///< Bit field DramLimitReg6
  BFDramLimitReg7,                      ///< Bit field DramLimitReg7

  BFDramBaseHiReg0,                     ///< Bit field DramBaseHiReg0
  BFDramBaseHiReg1,                     ///< Bit field DramBaseHiReg1
  BFDramBaseHiReg2,                     ///< Bit field DramBaseHiReg2
  BFDramBaseHiReg3,                     ///< Bit field DramBaseHiReg3
  BFDramBaseHiReg4,                     ///< Bit field DramBaseHiReg4
  BFDramBaseHiReg5,                     ///< Bit field DramBaseHiReg5
  BFDramBaseHiReg6,                     ///< Bit field DramBaseHiReg6
  BFDramBaseHiReg7,                     ///< Bit field DramBaseHiReg7

  BFDramLimitHiReg0,                    ///< Bit field DramLimitHiReg0
  BFDramLimitHiReg1,                    ///< Bit field DramLimitHiReg1
  BFDramLimitHiReg2,                    ///< Bit field DramLimitHiReg2
  BFDramLimitHiReg3,                    ///< Bit field DramLimitHiReg3
  BFDramLimitHiReg4,                    ///< Bit field DramLimitHiReg4
  BFDramLimitHiReg5,                    ///< Bit field DramLimitHiReg5
  BFDramLimitHiReg6,                    ///< Bit field DramLimitHiReg6
  BFDramLimitHiReg7,                    ///< Bit field DramLimitHiReg7

  BFDramHoleAddrReg,                    ///< Bit field DramHoleAddrReg

  BFDramRngRE0,                         ///< Bit field DramBaseReg0 RE
  BFDramRngRE1,                         ///< Bit field DramBaseReg1 RE
  BFDramRngRE2,                         ///< Bit field DramBaseReg2 RE
  BFDramRngRE3,                         ///< Bit field DramBaseReg3 RE
  BFDramRngRE4,                         ///< Bit field DramBaseReg4 RE
  BFDramRngRE5,                         ///< Bit field DramBaseReg5 RE
  BFDramRngRE6,                         ///< Bit field DramBaseReg6 RE
  BFDramRngRE7,                         ///< Bit field DramBaseReg7 RE

  BFDramRngWE0,                         ///< Bit field DramBaseReg0 WE
  BFDramRngWE1,                         ///< Bit field DramBaseReg1 WE
  BFDramRngWE2,                         ///< Bit field DramBaseReg2 WE
  BFDramRngWE3,                         ///< Bit field DramBaseReg3 WE
  BFDramRngWE4,                         ///< Bit field DramBaseReg4 WE
  BFDramRngWE5,                         ///< Bit field DramBaseReg5 WE
  BFDramRngWE6,                         ///< Bit field DramBaseReg6 WE
  BFDramRngWE7,                         ///< Bit field DramBaseReg7 WE

  BFDramRngDstNode0,                    ///< Bit field Dram Range0 DstNode
  BFDramRngDstNode1,                    ///< Bit field Dram Range1 DstNode
  BFDramRngDstNode2,                    ///< Bit field Dram Range2 DstNode
  BFDramRngDstNode3,                    ///< Bit field Dram Range3 DstNode
  BFDramRngDstNode4,                    ///< Bit field Dram Range4 DstNode
  BFDramRngDstNode5,                    ///< Bit field Dram Range5 DstNode
  BFDramRngDstNode6,                    ///< Bit field Dram Range6 DstNode
  BFDramRngDstNode7,                    ///< Bit field Dram Range7 DstNode

  BFCSBaseAddr0Reg,                     ///< Bit field CSBaseAddr0Reg
  BFCSBaseAddr1Reg,                     ///< Bit field CSBaseAddr1Reg
  BFCSBaseAddr2Reg,                     ///< Bit field CSBaseAddr2Reg
  BFCSBaseAddr3Reg,                     ///< Bit field CSBaseAddr3Reg
  BFCSBaseAddr4Reg,                     ///< Bit field CSBaseAddr4Reg
  BFCSBaseAddr5Reg,                     ///< Bit field CSBaseAddr5Reg
  BFCSBaseAddr6Reg,                     ///< Bit field CSBaseAddr6Reg
  BFCSBaseAddr7Reg,                     ///< Bit field CSBaseAddr7Reg

  BFCSMask0Reg,                         ///< Bit field CSMask0Reg
  BFCSMask1Reg,                         ///< Bit field CSMask1Reg
  BFCSMask2Reg,                         ///< Bit field CSMask2Reg
  BFCSMask3Reg,                         ///< Bit field CSMask3Reg

  BFRankDef0,                           ///< Bit field RankDef 0
  BFRankDef1,                           ///< Bit field RankDef 1
  BFRankDef2,                           ///< Bit field RankDef 2
  BFRankDef3,                           ///< Bit field RankDef 3

  BFDramControlReg,                     ///< Bit field DramControlReg
  BFDramInitRegReg,                     ///< Bit field DramInitRegReg
  BFDramBankAddrReg,                    ///< Bit field DramBankAddrReg
  BFDramMRSReg,                         ///< Bit field DramMRSReg
  BFDramTimingLoReg,                    ///< Bit field DramTimingLoReg
  BFDramTimingHiReg,                    ///< Bit field DramTimingHiReg
  BFDramConfigLoReg,                    ///< Bit field DramConfigLoReg
  BFDramConfigHiReg,                    ///< Bit field DramConfigHiReg
  BFDctAddlOffsetReg,                   ///< Bit field DctAddlOffsetReg
  BFDctAddlDataReg,                     ///< Bit field DctAddlDataReg
  BFDctAccessDone,                      ///< Bit field DctAccessDone
  BFDctAccessError,                     ///< Bit field DctAccessError
  BFDctExtraOffsetReg,                  ///< Bit field DctExtraOffsetReg
  BFDctExtraDataReg,                    ///< Bit field DctExtraDataReg
  BFDctExtraAccessDone,                 ///< Bit field DctExtraAccessDone
  BFDramConfigMiscReg,                  ///< Bit field DramConfigMiscReg
  BFDramCtrlMiscReg2,                   ///< Bit field DramCtrlMiscReg2
  BFMctCfgHiReg,                        ///< Bit field MctCfgHiReg
  BFTrcBufAdrPtrHi,                     ///< Bit field BFTrcBufAdrPtr
  BFTrcBufDramLimitHi,                  ///< Bit field BFTrcBufDramLimit
  BFTrcBufDramBaseHi,                   ///< Bit field BFTrcBufDramBase
  BFMctCfgLoReg,                        ///< Bit field MctCfgLoReg
  BFExtMctCfgLoReg,                     ///< Bit field ExtMctCfgLoReg
  BFExtMctCfgHiReg,                     ///< Bit field ExtMctCfgHiReg
  BFDcqBwThrotWm,                       ///< Bit field DcqBwThrotWm
  BFPrefFiveConf,                       ///< Bit field PrefFiveConf
  BFPrefFourConf,                       ///< Bit field PrefFourConf
  BFDcqBwThrotWm1,                      ///< Bit field DcqBwThrotWm1
  BFDcqBwThrotWm2,                      ///< Bit field DcqBwThrotWm2

  BFCSMapCKE,                           ///< Bit field CSMapCKE
  BFTrdrdBan,                           ///< Bit field TrdrdBan
  BFTrcpage,                            ///< Bit field Trcpage
  BFTzqcs,                              ///< Bit field Tzqcs
  BFTzqoper,                            ///< Bit field Tzqoper

  BFDramHoleBase,                       ///< Bit field DramHoleBase
  BFDramHoleOffset,                     ///< Bit field DramHoleOffset
  BFDramMemHoistValid,                  ///< Bit field DramMemHoistValid
  BFDramHtHoleValid,                    ///< Bit field BFDramHtHoleValid
  BFDramHoleValid,                      ///< Bit field DramHoleValid
  BFDramBaseAddr,                       ///< Bit field DramBaseAddr
  BFDramIntlvSel,                       ///< Bit field DramIntlvSel
  BFDramLimitAddr,                      ///< Bit field DramLimitAddr
  BFDramCtrlBaseReg0,                   ///< Bit field DramCtrlBaseReg0
  BFLgcyMmioHoleEn0,                    ///< Bit field LgcyMmioHoleEn0
  BFDramCtrlLimitReg0,                  ///< Bit field DramCtrlLimitReg0
  BFDramIntlvEn,                        ///< Bit field DramIntlvEn
  BFMemPsSel,                           ///< Bit field MemPsSel
  BFDctCfgSel,                          ///< Bit field DctCfgSel
  BFRcvParErr,                          ///< Bit field RcvParErr

  BFDctBaseReg0,                        ///< Bit field DctBaseReg0
  BFDctBaseReg1,                        ///< Bit field DctBaseReg1
  BFDctBaseReg2,                        ///< Bit field DctBaseReg2
  BFDctBaseReg3,                        ///< Bit field DctBaseReg3

  BFDctLimitReg0,                       ///< Bit field DctLimitReg0
  BFDctLimitReg1,                       ///< Bit field DctLimitReg1
  BFDctLimitReg2,                       ///< Bit field DctLimitReg2
  BFDctLimitReg3,                       ///< Bit field DctLimitReg3

  BFDctHighAddressOffsetReg0,           ///< Bit field DctHighAddressOffsetReg0
  BFDctHighAddressOffsetReg1,           ///< Bit field DctHighAddressOffsetReg1
  BFDctHighAddressOffsetReg2,           ///< Bit field DctHighAddressOffsetReg2
  BFDctHighAddressOffsetReg3,           ///< Bit field DctHighAddressOffsetReg3

  BFDctTriChannelInterleaveReg0,        ///< Bit field DctTriChannelInterleaveReg0
  BFDctTriChannelInterleaveReg1,        ///< Bit field DctTriChannelInterleaveReg1
  BFDctTriChannelInterleaveReg2,        ///< Bit field DctTriChannelInterleaveReg2
  BFDctTriChannelInterleaveReg3,        ///< Bit field DctTriChannelInterleaveReg3
  BFDctTriChannelInterleaveReg4,        ///< Bit field DctTriChannelInterleaveReg4
  BFDctTriChannelInterleaveReg5,        ///< Bit field DctTriChannelInterleaveReg5
  BFDctTriChannelInterleaveReg6,        ///< Bit field DctTriChannelInterleaveReg6
  BFDctTriChannelInterleaveReg7,        ///< Bit field DctTriChannelInterleaveReg7

  BF3DStackHeight0,                     ///< Bit field 3DStackHeight0
  BF3DStackHeight1,                     ///< Bit field 3DStackHeight1
  BF3DStackHeight2,                     ///< Bit field 3DStackHeight2
  BF3DStackHeight3,                     ///< Bit field 3DStackHeight3

  BFIdleCycLowLimit,                    ///< Bit field IdleCycLowLimit
  BFIdleCycLimit,                       ///< Bit field IdleCycLimit
  BFPendRefPayback,                     ///< Bit field PendRefPayback
  BFProcOdtDis,                         ///< Bit field ProcOdtDis



  BFMcaNbCtlReg,                        ///< Bit field McaNbCtlReg
  BFDramEccEn,                          ///< Bit field DramEccEn
  BFEccCapable,                         ///< Bit field Ecc Capable
  BFSyncOnUcEccEn,                      ///< Bit field SyncOnUcEccEn
  BFEccSymbolSize,                      ///< Bit field EccSymbolSize
  BFMcaNbStatusLoReg,                   ///< Bit field McaNbStatusLoReg
  BFMcaNbStatusHiReg,                   ///< Bit field McaNbStatusHiReg
  BFDramScrub,                          ///< Bit field DramScrub
  BFL2Scrub,                            ///< Bit field L2Scrub
  BFDcacheScrub,                        ///< Bit field DcacheScrub
  BFL3Scrub,                            ///< Bit field L3Scrub
  BFScrubReDirEn,                       ///< Bit field ScrubReDirEn
  BFScrubAddrLoReg,                     ///< Bit field ScrubAddrLoReg
  BFScrubAddrHiReg,                     ///< Bit field ScrubAddrHiReg
  BFDisDramScrub,                       ///< Bit field DisDramScrub
  BFC1ClkDivisor,                       ///< Bit field C1ClkDivisor
  BFDisDatMsk,                          ///< Bit field DisDatMsk
  BFNbFid,                              ///< Bit field NbFid
  BFL3Capable,                          ///< Bit field L3Capable
  BFDisableL3,                          ///< Bit field DisableL3
  BFEnhMemProtCap,                      ///< Bit field EnhMemProtCap
  BFNbPsForceReq,                       ///< Bit field NbPsForceReq
  BFNbPsCtrlDis,                        ///< Bit field NbPsCtrlDis
  BFNbPsCap,                            ///< Bit field NbPsCap

  BFNonSPDHi,                           ///< Bit field NonSPDHi
  BFRdPtrInit,                          ///< Bit field RdPtrInit
  BFAltVidC3MemClkTriEn,                ///< Bit field AltVidC3MemClkTriEn
  BFDqsRcvEnTrain,                      ///< Bit field DqsRcvEnTrain
  BFEarlyArbEn,                         ///< Bit field EarlyArbEn
  BFMaxLatency,                         ///< Bit field either MaxRdLat or MaxAsyncLat
  BFMaxLatency0,                        ///< Bit field either MaxRdLat or MaxAsyncLat for NB P0
  BFMaxLatency1,                        ///< Bit field either MaxRdLat or MaxAsyncLat for NB P1
  BFMaxLatency2,                        ///< Bit field either MaxRdLat or MaxAsyncLat for NB P2
  BFMaxLatency3,                        ///< Bit field either MaxRdLat or MaxAsyncLat for NB P3

  BFMrsAddress,                         ///< Bit field MrsAddress
  BFMrsBank,                            ///< Bit field MrsBank
  BFMrsChipSel,                         ///< Bit field MrsChipSel
  BFSendPchgAll,                        ///< Bit field SendPchgAll
  BFSendAutoRefresh,                    ///< Bit field SendAutoRefresh
  BFSendMrsCmd,                         ///< Bit field SendMrsCmd
  BFDeassertMemRstX,                    ///< Bit field DeassertMemRstX
  BFAssertCke,                          ///< Bit field AssertCke
  BFSendZQCmd,                          ///< Bit field SendZQCmd
  BFSendCtrlWord,                       ///< Bit field SendCtrlWord
  BFEnDramInit,                         ///< Bit field EnDramInit
  BFMrsLevel,                           ///< Bit field MrsLevel
  BFMrsQoff,                            ///< Bit field MrsQoff
  BFMrsAddressHi,                       ///< Bit field MrsAddress [17:13]

  BFBurstCtrl,                          ///< Bit field BurstCtrl
  BFDrvImpCtrl,                         ///< Bit field DrvImpCtrl
  BFDramTerm_DDR3,                      ///< Bit field DramTerm_DDR3
  BFDramTermDyn,                        ///< Bit field DramTermDyn
  BFQoff,                               ///< Bit field Qoff
  BFASR,                                ///< Bit field ASR
  BFSRT,                                ///< Bit field SRT
  BFTcwl,                               ///< Bit field Tcwl
  BFPchgPDModeSel,                      ///< Bit field PchgPDModeSel
  BFLowPowerDefault,                    ///< Bit field LowPowerDefault

  BFTvrefdq,                            ///< Bit field Tvrefdq
  BFTcl,                                ///< Bit field Tcl
  BFTrcd,                               ///< Bit field Trcd
  BFTrp,                                ///< Bit field Trp
  BFTrtp,                               ///< Bit field Trtp
  BFTras,                               ///< Bit field Tras
  BFTrc,                                ///< Bit field Trc
  BFTwr,                                ///< Bit field Twr
  BFTrrd,                               ///< Bit field Trrd
  BFTrrdL,                              ///< Bit field TrrdL
  BFMemClkDis,                          ///< Bit field MemClkDis
  BFDramTiming0,                        ///< Bit field BFDramTiming0
  BFDramTiming1,                        ///< Bit field BFDramTiming1
  BFDramTiming2,                        ///< Bit field BFDramTiming2
  BFDramTiming3,                        ///< Bit field BFDramTiming3
  BFDramTiming4,                        ///< Bit field BFDramTiming4
  BFDramTiming5,                        ///< Bit field BFDramTiming5
  BFDramTiming6,                        ///< Bit field BFDramTiming6
  BFDramTiming10,                       ///< Bit field BFDramTiming10
  BFDramNBP0,                           ///< Bit field BFDramNBP0

  BFNonSPD,                             ///< Bit field NonSPD
  BFTrdrdSdScL,                         ///< Bit field TrdrdSdScL
  BFTwrwrSdScL,                         ///< Bit field TwrwrSdScL
  BFTrwtWB,                             ///< Bit field TrwtWB
  BFTrwtTO,                             ///< Bit field TrwtTO
  BFTwtr,                               ///< Bit field Twtr
  BFTwtrL,                              ///< Bit field TwtrL
  BFTwrrd,                              ///< Bit field Twrrd
  BFTwrrdHi,                            ///< Bit field TwrrdHi
  BFTwrwr,                              ///< Bit field Twrwr
  BFTwrwrHi,                            ///< Bit field TwrwrHi
  BFTrdrdSD,                            ///< Bit field TrdrdSD
  BFTwrwrSD,                            ///< Bit field TwrwrSD
  BFTwrrdSD,                            ///< Bit field TwrrdSD
  BFTmod,                               ///< Bit field Tmod
  BFTmrd,                               ///< Bit field Tmrd
  BFTdllk,                              ///< Bit field Tdllk
  BFRdOdtTrnOnDly,                      ///< Bit field RdOdtTrnOnDly
  BFRdOdtOnDuration,                    ///< Bit field RdOdtOnDuration
  BFWrOdtTrnOnDly,                      ///< Bit field WrOdtTrnOnDly
  BFWrOdtOnDuration,                    ///< Bit field WrOdtOnDuration
  BFPrtlChPDDynDly,                     ///< Bit field PrtlChPDDynDly

  BFAggrPDDelay,                        ///< Bit field AggrPDDelay
  BFPchgPDEnDelay,                      ///< Bit field PchgPDEnDelay

  BFTrdrd,                              ///< Bit field Trdrd
  BFTrdrdHi,                            ///< Bit field TrdrdHi
  BFTxs,                                ///< Bit field Txs
  BFTref,                               ///< Bit field Tref
  BFDisAutoRefresh,                     ///< Bit field DisAutoRefresh
  BFTrfc0,                              ///< Bit field Trfc0
  BFTrfc1,                              ///< Bit field Trfc1
  BFTrfc2,                              ///< Bit field Trfc2
  BFTrfc3,                              ///< Bit field Trfc3

  BFInitDram,                           ///< Bit field InitDram
  BFExitSelfRef,                        ///< Bit field ExitSelfRef
  BFDramTerm,                           ///< Bit field DramTerm
  BFParEn,                              ///< Bit field ParEn
  BFBurstLength32,                      ///< Bit field BurstLength32
  BFWidth128,                           ///< Bit field Width128
  BFX4Dimm,                             ///< Bit field X4Dimm
  BFDimmEccEn,                          ///< Bit field DimmEccEn
  BFUnBuffDimm,                         ///< Bit field UnBuffDimm
  BFEnterSelfRef,                       ///< Bit field EnterSelfRef
  BFDynPageCloseEn,                     ///< Bit field DynPageCloseEn
  BFIdleCycInit,                        ///< Bit field IdleCycInit
  BFFreqChgInProg,                      ///< Bit field FreqChgInProg
  BFForceAutoPchg,                      ///< Bit field ForceAutoPchg
  BFStagRefEn,                          ///< Bit field StagRefEn
  BFPendRefPaybackS3En,                 ///< Bit field PendRefPaybackS3En
  BFEnDispAutoPrecharge,                ///< Bit field EnDispAutoPrecharge
  BFDisDllShutdownSR,                   ///< Bit field DisDllShutdownSR
  BFDisSscClkGateData,                  ///< Bit field DisSscClkGateData
  BFDisSscClkGateCmdAddr,               ///< Bit field DisSscClkGateCmdAddr
  BFDisSimulRdWr,                       ///< Bit field DisSimulRdWr

  BFMemClkFreq,                         ///< Bit field MemClkFreq
  BFMemClkFreqVal,                      ///< Bit field MemClkFreqVal
  BFDdr3Mode,                           ///< Bit field Ddr3Mode
  BFLegacyBiosMode,                     ///< Bit field LegacyBiosMode
  BFZqcsInterval,                       ///< Bit field ZqcsInterval
  BFRDqsEn,                             ///< Bit field RDqsEn
  BFDisDramInterface,                   ///< Bit field DisDramInterface
  BFPowerDownEn,                        ///< Bit field PowerDownEn
  BFPowerDownMode,                      ///< Bit field PowerDownMode
  BFFourRankSoDimm,                     ///< Bit field FourRankSoDimm
  BFDcqArbBypassEn,                     ///< Bit field DcqArbBypassEn
  BFFourRankRDimm,                      ///< Bit field FourRankRDimm
  BFSlowAccessMode,                     ///< Bit field SlowAccessMode
  BFBankSwizzleMode,                    ///< Bit field BankSwizzleMode
  BFDcqBypassMax,                       ///< Bit field DcqBypassMax
  BFOn3RdCasStallMode,                  ///< Bit field On3RdCasStallMode
  BFFourActWindow,                      ///< Bit field FourActWindow
  BFDphyMemPsSelEn,                     ///< Bit field BFDphyMemPsSelEn
  BFBankGroupSwap,                      ///< Bit field BFBankGroupSwap

  BFODTSEn,                             ///< Bit field ODTSEn
  BFODTSDoubleRefreshRate,              ///< Bit field ODTSDoubleRefreshRate
  BFCmdThrottleMode,                    ///< Bit field CmdThrottleMode
  BFBwCapEn,                            ///< Bit field BwCapEn

  BFDdr3FourSocketCh,                   ///< Bit field Ddr3FourSocketCh
  BFSubMemclkRegDly,                    ///< Bit field SubMemclkRegDly
  BFOdtSwizzle,                         ///< Bit field OdtSwizzle
  BFProgOdtEn,                          ///< Bit field ProgOdtEn
  BFCtrlWordCS,                         ///< Bit field CtrlWordCS
  BFRefChCmdMgtDis,                     ///< Bit field RefChCmdMgtDis
  BFFastSelfRefEntryDis,                ///< Bit field FastSelfRefEntryDis
  BFOppWrEnhDis,                        ///< Bit field OppWrEnhDis
  BFPrtlChPDEnhEn,                      ///< Bit field PrtlChPDEnhEn
  BFAggrPDEn,                           ///< Bit field AggrPDEn
  BFDataTxFifoWrDly,                    ///< Bit field DataTxFifoWrDly
  BFDataTxFifoWrDly0,                   ///< Bit field DataTxFifoWrDly for NB P0
  BFDataTxFifoWrDly1,                   ///< Bit field DataTxFifoWrDly for NB P1
  BFDataTxFifoWrDly2,                   ///< Bit field DataTxFifoWrDly for NB P2
  BFDataTxFifoWrDly3,                   ///< Bit field DataTxFifoWrDly for NB P3
  BFWrDqDqsEarly,                       ///< Bit field WrDqDqsEarly
  BFCSMux45,                            ///< Bit field CSMux45
  BFCSMux67,                            ///< Bit field CSMux67
  BFLrDimmMrsCtrl,                      ///< Bit field LrDimmMrsCtrl
  BFExtendedParityEn,                   ///< Bit field ExtendedParityEn
  BFLrDimmEnhRefEn,                     ///< Bit field LrDimmEnhRefEn
  BFCSTimingMux67,                      ///< Bit field CSTimingMux67
  BFLrDimmErrOutMonEn,                  ///< Bit field LrDimmErrOutMonEn

  BFIntLvRgnSwapEn,                     ///< Bit field IntLvRgnSwapEn
  BFIntLvRgnBaseAddr,                   ///< Bit field IntLvRgnBaseAddr
  BFIntLvRgnLmtAddr,                    ///< Bit field IntLvRgnLmtAddr
  BFIntLvRgnSize,                       ///< Bit field IntLvRgnSize

  BFDctSelHiRngEn,                      ///< Bit field DctSelHiRngEn
  BFDctSelHi,                           ///< Bit field DctSelHi
  BFDctSelIntLvEn,                      ///< Bit field DctSelIntLvEn
  BFMemClrInit,                         ///< Bit field MemClrInit
  BFDctGangEn,                          ///< Bit field DctGangEn
  BFDctDatIntLv,                        ///< Bit field DctDatIntLv
  BFDctSelIntLvAddr,                    ///< Bit field DctSelIntLvAddr
  BFDctSelIntLvAddrHi,                  ///< Bit field DctSelIntLvAddrHi
  BFParallelMemClrEn,                   ///< Bit field ParallelMemClrEn
  BFDramEnabled,                        ///< Bit field DramEnabled
  BFMemClrBusy,                         ///< Bit field MemClrBusy
  BFMemCleared,                         ///< Bit field MemCleared
  BFDctSelBaseAddr,                     ///< Bit field DctSelBaseAddr
  BFDctSelBaseOffset,                   ///< Bit field DctSelBaseOffset
  BFBankSwapAddr8En,                    ///< Bit field DctSelBankSwap

  BFAdapPrefMissRatio,                  ///< Bit field AdapPrefMissRatio
  BFAdapPrefPosStep,                    ///< Bit field AdapPrefPosStep
  BFAdapPrefNegStep,                    ///< Bit field AdapPrefNegStep
  BFCohPrefPrbLmt,                      ///< Bit field CohPrefPrbLmt

  BFFlushWrOnS3StpGnt,                  ///< Bit field FlushWrOnS3StpGnt

  BFPrefDramTrainDone,                  ///< Bit field PrefDramTrainDone
  BFWrDramTrainMode,                    ///< Bit field WrDramTrainMode
  BFMctPrefReqLimit,                    ///< Bit field MctPrefReqLimit
  BFPrefDramTrainMode,                  ///< Bit field PrefDramTrainMode
  BFDctWrLimit,                         ///< Bit field DctWrLimit
  BFMctWrLimit,                         ///< Bit field MctWrLimit
  BFDramTrainPdbDis,                    ///< Bit field DramTrainPdbDis
  BFTrainLength,                        ///< Bit field TrainLength
  BFRdTrainGo,                          ///< Bit field RdTrainGo
  BFWrTrainGo,                          ///< Bit field WrTrainGo
  BFWrTrainAdrPtrLo,                    ///< Bit field WrTrainAdrPtrLo
  BFWrTrainAdrPtrHi,                    ///< Bit field WrTrainAdrPtrHi
  BFWrTrainBufAddr,                     ///< Bit field WrTrainBufAddr
  BFWrTrainBufDat,                      ///< Bit field WrTrainBufDat
  BFFlushWr,                            ///< Bit field FlushWr
  BFFlushWrOnStpGnt,                    ///< Bit field FlushWrOnStpGnt
  BFPrefCpuDis,                         ///< Bit field PrefCpuDis
  BFPrefIoDis,                          ///< Bit field PrefIoDis
  BFTrainCmpSts,                        ///< Bit field TrainCmpSts
  BFTrainCmpSts2,                       ///< Bit field TrainCmpSts2
  BFTcbModeEn,                          ///< Bit field TcbModeEn
  BFTraceModeEn,                        ///< Bit field TraceModeEn

  BFAddrCmdDrvStren,                    ///< Bit field AddrCmdDrvStren
  BFDataDrvStren,                       ///< Bit field DataDrvStren
  BFCkeDrvStren,                        ///< Bit field CkeDrvStren
  BFCsOdtDrvStren,                      ///< Bit field CsOdtDrvStren
  BFClkDrvStren,                        ///< Bit field ClkDrvStren
  BFDqsDrvStren,                        ///< Bit field DqsDrvStren
  BFProcOdt,                              ///< Bit field ProcOdt
  BFODCControl,                         ///< Bit field ODCControl
  BFAddrTmgControl,                     ///< Bit field AddrTmgControl
  BFAddrCmdFineDelay,                   ///< Bit field AddrCmdFineDelay

  BFWrtLvTrEn,                          ///< Bit field WrtLvTrEn
  BFWrtLvTrMode,                        ///< Bit field WrtLvTrMode
  BFPhyFenceTrEn,                       ///< Bit field PhyFenceTrEn
  BFTrDimmSel,                          ///< Bit field TrDimmSel
  BFTrNibbleSel,                        ///< Bit field TrNibbleSel
  BFFenceTrSel,                         ///< Bit field FenceTrSel
  BFWrLvOdt,                            ///< Bit field WrLvOdt
  BFWrLvOdtEn,                          ///< Bit field WrLvOdtEn
  BFDqsRcvTrEn,                         ///< Bit field DqsRcvTrEn
  BFDisAutoComp,                        ///< Bit field DisAutoComp
  BFWrtLvErr,                           ///< Bit field WrtLvErr
  BFODTAssertionCtl,                    ///< Bit field ODTAssertionCtl
  BFNibbleTrainModeEn,                  ///< Bit field NibbleTrainModeEn
  BFRankTrainModeEn,                    ///< Bit field RankTrainModeEn
  BFPllMult,                            ///< Bit field PllMult
  BFPllDiv,                             ///< Bit field PllDiv
  BFDramPhyCtlReg,                      ///< Bit field Dram Phy Control Register

  BFDramPhyStatusReg,                   ///< Bit field DramPhyStatusReg

  BFDQMappingDimm0Reg0,                 ///< Bit field for DQ Mapping DIMM 0 Register 0
  BFDQMappingDimm0Reg1,                 ///< Bit field for DQ Mapping DIMM 0 Register 1
  BFDQMappingDimm0Reg2,                 ///< Bit field for DQ Mapping DIMM 0 Register 2
  BFDQMappingDimm0Reg3,                 ///< Bit field for DQ Mapping DIMM 0 Register 3
  BFDQMappingDimm1Reg0,                 ///< Bit field for DQ Mapping DIMM 1 Register 0
  BFDQMappingDimm1Reg1,                 ///< Bit field for DQ Mapping DIMM 1 Register 1
  BFDQMappingDimm1Reg2,                 ///< Bit field for DQ Mapping DIMM 1 Register 2
  BFDQMappingDimm1Reg3,                 ///< Bit field for DQ Mapping DIMM 1 Register 3
  BFDQMappingDimm2Reg0,                 ///< Bit field for DQ Mapping DIMM 2 Register 0
  BFDQMappingDimm2Reg1,                 ///< Bit field for DQ Mapping DIMM 2 Register 1
  BFDQMappingDimm2Reg2,                 ///< Bit field for DQ Mapping DIMM 2 Register 2
  BFDQMappingDimm2Reg3,                 ///< Bit field for DQ Mapping DIMM 2 Register 3
  BFDQMappingDimm3Reg0,                 ///< Bit field for DQ Mapping DIMM 3 Register 0
  BFDQMappingDimm3Reg1,                 ///< Bit field for DQ Mapping DIMM 3 Register 1
  BFDQMappingDimm3Reg2,                 ///< Bit field for DQ Mapping DIMM 3 Register 2
  BFDQMappingDimm3Reg3,                 ///< Bit field for DQ Mapping DIMM 3 Register 3

  BFD3Cmp2PCal,                         ///< Bit field D3Cmp2PCal
  BFD3Cmp2NCal,                         ///< Bit field D3Cmp2NCal
  BFD3Cmp1PCal,                         ///< Bit field D3Cmp1PCal
  BFD3Cmp1NCal,                         ///< Bit field D3Cmp1NCal
  BFD3Cmp0PCal,                         ///< Bit field D3Cmp0PCal
  BFD3Cmp0NCal,                         ///< Bit field D3Cmp0NCal

  BFPhyFence,                           ///< Bit field PhyFence
  BFODTTri,                             ///< Bit field ODTTri
  BFCKETri,                             ///< Bit field CKETri
  BFChipSelTri,                         ///< Bit field ChipSelTri
  BFPhyRODTCSLow,                       ///< Bit field PhyRODTCSLow
  BFPhyRODTCSHigh,                      ///< Bit field PhyRODTCSHigh
  BFPhyWODTCSLow,                       ///< Bit field PhyWODTCSLow
  BFPhyWODTCSHigh,                      ///< Bit field PhyWODTCSHigh
  BFUSPLLCtlAll,                        ///< Bit field USPLLCtlAll
  BFDSPLLCtlAll,                        ///< Bit field DSPLLCtlAll
  BFUSNibbleAlignEn,                    ///< Bit field USNibbleAlignEn
  BFChnLinitClkEn,                      ///< Bit field ChnLinitClkEn

  BFTSLinkSelect,                       ///< Bit field TSLinkSelect
  BFTS2BitLockEn,                       ///< Bit field TS2BitLockEn
  BFTS2En,                              ///< Bit field TS2En
  BFTS1En,                              ///< Bit field TS1En
  BFTS0LinkStarEn,                      ///< Bit field TS0LinkStarEn
  BFTS0En,                              ///< Bit field TS0En

  BFLinkTrainData,                      ///< Bit field LinkTrainData

  BFRstRxFifoPtrs,                      ///< Bit field RstRxFifoPtrs
  BFRxFifoPtrInit,                      ///< Bit field RxFifoPtrInit
  BFRstTxFifoPtrs,                      ///< Bit field RstTxFifoPtrs
  BFTxFifoPtrInit,                      ///< Bit field TxFifoPtrInit

  BFLpbkCount,                          ///< Bit field LpbkCount
  BFLpbkMap,                            ///< Bit field LpbkMap
  BFSendLpbkMaintCmd,                   ///< Bit field SendLpbkMaintCmd
  BFLpbkData,                           ///< Bit field LpbkData

  BFMbRdPtrEn,                          ///< Bit field MbRdPtrEn
  BFLnkLpBkLat,                         ///< Bit field LnkLpBkLat
  BFLpbkRndTripLatDone,                 ///< Bit field LpbkRndTripLatDone
  BFLnkLatTrainEn,                      ///< Bit field LnkLatTrainEn

  BFDsPhyReset,                         ///< Bit field DsPhyReset
  BFLinkReset,                          ///< Bit field LinkReset

  BFPllLockTime,                        ///< Bit field PllLockTime
  BFPllRegWaitTime,                     ///< Bit field PllRegWaitTime
  BFNclkFreqDone,                       ///< Bit field NclkFreqDone
  BFNbPs0NclkDiv,                       ///< Bit field NbPs0NclkDiv
  BFNbPs1NclkDiv,                       ///< Bit field NbPs1NclkDiv
  BFNbPsCsrAccSel,                      ///< Bit field NbPsCsrAccSel
  BFNbPsDbgEn,                          ///< Bit field NbPsDbgEn
  BFNclkRampWithDllRelock,              ///< Bit field NclkRampWithDllRelock

  BFOnLineSpareControl,                 ///< Bit field OnLineSpareControl
  BFDdrMaxRate,                         ///< Bit field DdrMaxRate

  BFPhyDctCfgSelMode,                   ///< Bit field PhyDctCfgSelMode
  BFNbPstateDis,                        ///< Bit field NbPstateDis
  BFNbPsSel,                            ///< Bit field NbPsSel
  BFNbPstateCtlReg,                     ///< Bit field NB Pstate Control register
  BFSwNbPstateLoDis,                    ///< Bit field SwNbPstateLoDis
  BFNbPstateLo,                         ///< Bit field NbPstateLo
  BFNbPstateHi,                         ///< Bit field NbPstateHi
  BFNbPstateMaxVal,                     ///< Bit field NbPstateMaxVal
  BFCurNbPstate,                        ///< Bit field NbCurNbPstate
  BFCurNbVid,                           ///< Bit field CurNbVid
  BFCurNbVidHi,                         ///< Bit field CurNbVidHi
  BFCurNbPstateLo,                      ///< Bit field CurNbPstateLo
  BFNbPstateReqBusy,                    ///< Bit field NbPstateReqBusy
  BFCurNbDid,                           ///< Bit field CurNbDid
  BFCurNbFid,                           ///< Bit field CurNbFid
  BFDctSelBankSwap,                     ///< Bit Field DctSelBankSwap

  BFC6Base,                             ///< Bit field C6Base
  BFC6DramLock,                         ///< Bit field C6DramLock
  BFCC6SaveEn,                          ///< Bit field CC6SaveEn
  BFCoreStateSaveDestNode,              ///< Bit field CoreStateSaveDestNode

  BFRxPtrInitReq,                       ///< Bit field RxPtrInitReq
  BFAddrCmdTriEn,                       ///< Bit field AddrCmdTriEn
  BFForceCasToSlot0,                    ///< Bit field ForceCasToSlot0
  BFDisCutThroughMode,                  ///< Bit field DisCutThroughMode
  BFDbeSkidBufDis,                      ///< Bit field DbeSkidBufDis
  BFDbeGskMemClkAlignMode,              ///< Bit field DbeGskMemClkAlignMode
  BFEnCpuSerRdBehindNpIoWr,             ///< Bit field EnCpuSerRdBehindNpIoWr
  BFDRAMPhyDLLControl,                  ///< Bit field DRAMPhyDLLControl
  BFRxDLLWakeupTime,                    ///< Bit field RxDllWakeupTime
  BFRxCPUpdPeriod,                      ///< Bit field RxCPUpdPeriod
  BFRxMaxDurDllNoLock,                  ///< Bit field RxMaxDurDllNoLock
  BFTxDLLWakeupTime,                    ///< Bit field TxDllWakeupTime
  BFTxCPUpdPeriod,                      ///< Bit field TxCPUpdPeriod
  BFTxMaxDurDllNoLock,                  ///< Bit field TxMaxDurDllNoLock
  BFEnRxPadStandby,                     ///< Bit field EnRxPadStandby
  BFMaxSkipErrTrain,                    ///< Bit field MaxSkipErrTrain
  BFSlotSel,                            ///< Bit field SlotSel
  BFSlot1ExtraClkEn,                    ///< Bit field Slot1ExtraClkEn
  BFPtrInitReq,                         ///< Bit field PtrInitReq
  BFDccPgSsBusDis,                      ///< Bit field DccPgSsBusDis
  BFGsyncDis,                           ///< Bit field GsyncDis
  BFChanVal,                            ///< Bit field ChanVal
  BFDctCfgBcEn,                         ///< Bit field DctCfgBcEn

  BFMemTempHot,                         ///< Bit field MemTempHot
  BFDoubleTrefRateEn,                   ///< Bit field DoubleTrefRateEn

  BFAcpiPwrStsCtrlHi,                   ///< Bit field BFAcpiPwrStsCtrlHi
  BFDramSrHysEn,                        ///< Bit field BFDramSrHysEn
  BFDramSrHys,                          ///< Bit field BFDramSrHys
  BFMemTriStateEn,                      ///< Bit field BFMemTriStateEn
  BFDramSrEn,                           ///< Bit field BFDramSrEn

  BFDeassertCke,                        ///< Bit field BFDeassertCke
  BFFourRankRDimm0,                     ///< Bit field BFFourRankRDimm0
  BFFourRankRDimm1,                     ///< Bit field BFFourRankRDimm1
  BFTwrwrSdSc,                          ///< Bit field BFTwrwrSdSc
  BFTwrwrSdDc,                          ///< Bit field BFTwrwrSdDc
  BFTwrwrDd,                            ///< Bit field BFTwrwrDd
  BFTrdrdSdSc,                          ///< Bit field BFTrdrdSdSc
  BFTrdrdSdDc,                          ///< Bit field BFTrdrdSdDc
  BFTrdrdDd,                            ///< Bit field BFTrdrdDd
  BFTstag0,                             ///< Bit field BFTstag0
  BFTstag1,                             ///< Bit field BFTstag1
  BFTstag2,                             ///< Bit field BFTstag2
  BFTstag3,                             ///< Bit field BFTstag3

  BFDataPatGenSel,                      ///< Bit field DataPatGenSel
  BFActPchgGenEn,                       ///< Bit field ActPchgGenEn
  BFShmooRdDqsDly,                      ///< Bit field ShmooRdDqsDly

  BFReverseLoopbackEn,                  ///< Bit field BFReverseLoopbackEn
  BFCmdSendInProg,                      ///< Bit field CmdSendInProg
  BFSendCmd,                            ///< Bit field SendCmd
  BFTestStatus,                         ///< Bit field TestStatus
  BFCmdTgt,                             ///< Bit field CmdTgt
  BFCmdType,                            ///< Bit field CmdType
  BFStopOnErr,                          ///< Bit field StopOnErr
  BFResetAllErr,                        ///< Bit field ResetAllErr
  BFCmdTestEnable,                      ///< Bit field CmdTestEnable
  BFTgtChipSelectA,                     ///< Bit field TgtChipSelectA
  BFTgtBankA,                           ///< Bit field TgtBankA
  BFTgtAddressHiA,                      ///< Bit field TgtAddressHiA
  BFTgtAddressA,                        ///< Bit field TgtAddressA
  BFTgtChipSelectB,                     ///< Bit field TgtChipSelectB
  BFTgtBankB,                           ///< Bit field TgtBankB
  BFTgtAddressHiB,                      ///< Bit field TgtAddressHiB
  BFTgtAddressB,                        ///< Bit field TgtAddressB
  BFBubbleCnt2,                         ///< Bit field BubbleCnt2
  BFBubbleCnt,                          ///< Bit field BubbleCnt
  BFCmdStreamLen,                       ///< Bit field CmdStreamLen
  BFCmdCount,                           ///< Bit field CmdCount
  BFErrDqNum,                           ///< Bit field ErrDQNum
  BFErrCnt,                             ///< Bit field ErrCnt
  BFNibbleErrSts,                       ///< Bit field NibbleErrSts
  BFNibbleErr180Sts,                    ///< Bit field NibbleErr180Sts
  BFDataPrbsSeed,                       ///< Bit field DataPrbsSeed
  BFDramDqMaskLow,                      ///< Bit field DramDqMaskLow
  BFDramDqMaskHigh,                     ///< Bit field DramDqMaskHigh
  BFDramEccMask,                        ///< Bit field DramEccMask
  BFDQPatOvrEn0,                        ///< Bit field DQPatOvrEn0
  BFDQPatOvrEn1,                        ///< Bit field DQPatOvrEn1
  BFXorPatOvr,                          ///< Bit field XorPatOvr
  BFPatOvrVal,                          ///< Bit field PatOvrVal
  BFEccPatOvrEn,                        ///< Bit field EccPatOvrEn
  BFSendActCmd,                         ///< Bit field SendActCmd
  BFSendPchgCmd,                        ///< Bit field SendPchgCmd
  BFCmdChipSelect,                      ///< Bit field CmdChipSelect
  BFCmdBank,                            ///< Bit field CmdBank
  BFCmdAddress,                         ///< Bit field CmdAddress
  BFErrBeatNum,                         ///< Bit Field ErrBeatNum
  BFErrCmdNum,                          ///< Bit field BFErrCmdNum
  BFDQErrLow,                           ///< Bit field DQSErrLow
  BFDQErrHigh,                          ///< Bit field DQSErrHigh
  BFEccErr,                             ///< Bit field EccErr
  BFFastMstateDis,                      ///< Bit field FastMstateDis
  BFDramUserDataPattern0,               ///< Bit field DramUserDataPattern0
  BFDramUserDataPattern1,               ///< Bit field DramUserDataPattern1
  BFDramUserDataPattern2,               ///< Bit field DramUserDataPattern2
  BFDramUserDataPattern3,               ///< Bit field DramUserDataPattern3
  BFActPchgSeq,                         ///< Bit field ActPchgSeq
  BFActPchgCmdMin,                      ///< Bit field ActPchgCmdMin
  BFDramCommandReg4,                    ///< Bit field DramCommandReg4
  BFDramCommandReg5,                    ///< Bit field DramCommandReg5
  BFNibbleErrStsSel,                    ///< Bit field NibbleErrStsSel
  BFErrStsDly,                          ///< Bit field ErrStsDly
  BFErrStsDly180,                       ///< Bit field ErrStsDly180

  /* Bit fields for workarounds */
  BFErr263,                             ///< Bit field Err263
  BFErr350,                             ///< Bit field Err350
  BFErr322I,                            ///< Bit field Err322I
  BFErr322II,                           ///< Bit field Err322II
  BFErratum468WorkaroundNotRequired,    ///< Bit field Erratum468WorkaroundNotRequired

  /* Bit fields for Phy */
  BFEccDLLConf,                         ///< Bit field EccDLLConf
  BFProcOdtAdv,                         ///< Bit field ProcOdtAdv
  BFEccDLLPwrDnConf,                    ///< Bit field EccDLLPwrDnConf
  BFDataCtlPipePclkGateEn,              ///< Bit field DataCtlPipePclkGateEn
  BFPchgPdPclkGateEn,                   ///< Bit field PchgPdPclkGateEn
  BFTxPclkGateEn,                       ///< Bit field TxPclkGateEn
  BFPhyPLLLockTime,                     ///< Bit field PhyPLLLockTime
  BFPhyDLLLockTime,                     ///< Bit field PhyDLLLockTime
  BFSkewMemClk,                         ///< Bit field SkewMemClk
  BFPhyDLLControl,                      ///< Bit field BFPhyDLLControl
  BFPhy0x0D080F0C,                      ///< Bit field BFPhy0x0D080F0C
  BFPhy0x0D080F10,                      ///< Bit field BFPhy0x0D080F10
  BFPhy0x0D080F11,                      ///< Bit field BFPhy0x0D080F11
  BFPhy0x0D088F30,                      ///< Bit field BFPhy0x0D088F30
  BFPhy0x0D08C030,                      ///< Bit field BFPhy0x0D08C030
  BFPhy0x0D082F30,                      ///< Bit field BFPhy0x0D082F30
  BFDiffTimingEn,                       ///< Bit Field DiffTimingEn
  BFFence,                              ///< Bit Field Fence
  BFDelay,                              ///< Bit Field Delay
  BFFenceValue,                         ///< Bit Field FenceValue

  BFTxP,                                ///< Bit Field TxP
  BFTxN,                                ///< Bit Field TxN
  BFPhy0x0D0F0F0B,                      ///< Bit Field BFPhy0x0D0F0F0B
  BFPhy0x0D0F0F07,                      ///< Bit Field BFPhy0x0D0F0F07
  BFPhy0x0D0F0F03,                      ///< Bit Field BFPhy0x0D0F0F03

  BFPhy0x0D040F3E,                      ///< Bit field BFPhy0x0D040F3E
  BFPhy0x0D042F3E,                      ///< Bit field BFPhy0x0D042F3E
  BFPhy0x0D048F3E,                      ///< Bit field BFPhy0x0D048F3E
  BFPhy0x0D04DF3E,                      ///< Bit field BFPhy0x0D04DF3E

  BFPhyClkDllFine0,                   ///< Bit field ClkDllFineDly 0
  BFPhyClkDllFine1,                   ///< Bit field ClkDllFineDly 1

  BFPhyClkConfig0,                    ///< Bit field ClkConfig0
  BFPhyClkConfig1,                    ///< Bit field ClkConfig1
  BFPhyClkConfig2,                    ///< Bit field ClkConfig2
  BFPhyClkConfig3,                    ///< Bit field ClkConfig3

  BFPhy0x0D0F0F13,                      ///< Bit field BFPhy0x0D0F0F13
  BFPhy0x0D0F0F13Bit0to7,               ///< Bit field BFPhy0x0D0F0F13Bit0to7
  BFPhy0x0D0F0830,                      ///< Bit field BFPhy0x0D0F0830
  BFPhy0x0D07812F,                      ///< Bit field BFPhy0x0D0F8108

  BFDataRxVioLvl,                       ///< Bit field DataRxVioLvl
  BFClkRxVioLvl,                        ///< Bit field ClkRxVioLvl
  BFCmdRxVioLvl,                        ///< Bit field CmdRxVioLvl
  BFAddrRxVioLvl,                       ///< Bit field AddrRxVioLvl
  BFCmpVioLvl,                          ///< Bit field CmpVioLvl
  BFCsrComparator,                      ///< Bit field CsrComparator
  BFAlwaysEnDllClks,                    ///< Bit field AlwaysEnDllClks
  BFPhy0x0D0FE00A,                      ///< Bit field Phy0x0D0FE00A
  BFPllPdMode,                          ///< Bit fields SelCsrPllPdMode and CsrPhySrPllPdMode

  BFDataFence2,                         ///< Bit field DataFence2
  BFClkFence2,                          ///< Bit field ClkFence2
  BFCmdFence2,                          ///< Bit field CmdFence2
  BFAddrFence2,                         ///< Bit field AddrFence2

  BFDataByteDMConf,                     ///< Bit field DataByteDMConf

  BFAddrCmdTri,                         ///< Bit field BFAddrCmdTri
  BFLowPowerDrvStrengthEn,              ///< Bit field BFLowPowerDrvStrengthEn
  BFLevel,                              ///< Bit field Level

  BFDbeGskFifoNumerator,                ///< Bit field DbeGskFifoNumerator
  BFDbeGskFifoDenominator,              ///< Bit field DbeGskFifoDenominator
  BFDataTxFifoSchedDlyNegSlot0,         ///< Bit field DataTxFifoSchedDlyNegSlot0
  BFDataTxFifoSchedDlyNegSlot1,         ///< Bit field DataTxFifoSchedDlyNegSlot1
  BFDataTxFifoSchedDlySlot0,            ///< Bit field DataTxFifoSchedDlySlot0
  BFDataTxFifoSchedDlySlot1,            ///< Bit field DataTxFifoSchedDlySlot1

  BFDisablePredriverCal,                ///< Bit field DisablePredriverCal
  BFDataByteTxPreDriverCal,             ///< Bit field DataByteTxPreDriverCal
  BFPOdtOff,                            ///< Bit field POdtOff
  BFDataByteTxPreDriverCal2Pad1,        ///< Bit field DataByteTxPreDriverCal2Pad1
  BFDataByteTxPreDriverCal2Pad2,        ///< Bit field DataByteTxPreDriverCal2Pad2
  BFCmdAddr0TxPreDriverCal2Pad1,        ///< Bit field CmdAddr0TxPreDriverCal2Pad1
  BFCmdAddr0TxPreDriverCal2Pad2,        ///< Bit field CmdAddr0TxPreDriverCal2Pad2
  BFCmdAddr1TxPreDriverCal2Pad1,        ///< Bit field CmdAddr1TxPreDriverCal2Pad1
  BFCmdAddr1TxPreDriverCal2Pad2,        ///< Bit field CmdAddr1TxPreDriverCal2Pad2
  BFAddrTxPreDriverCal2Pad1,            ///< Bit field AddrTxPreDriverCal2Pad1
  BFAddrTxPreDriverCal2Pad2,            ///< Bit field AddrTxPreDriverCal2Pad2
  BFAddrTxPreDriverCal2Pad3,            ///< Bit field AddrTxPreDriverCal2Pad3
  BFAddrTxPreDriverCal2Pad4,            ///< Bit field AddrTxPreDriverCal2Pad4
  BFCmdAddr0TxPreDriverCalPad0,         ///< Bit field CmdAddr0TxPreDriverCalPad0
  BFCmdAddr1TxPreDriverCalPad0,         ///< Bit field CmdAddr1TxPreDriverCalPad0
  BFAddrTxPreDriverCalPad0,             ///< Bit field AddrTxPreDriverCalPad0
  BFClock0TxPreDriverCalPad0,           ///< Bit field Clock0TxPreDriverCalPad0
  BFClock1TxPreDriverCalPad0,           ///< Bit field Clock1TxPreDriverCalPad0
  BFClock2TxPreDriverCalPad0,           ///< Bit field Clock2TxPreDriverCalPad0
  BFPNOdtCal,                           ///< Bit field P/NOdtCal
  BFPNDrvCal,                           ///< Bit field P/NDrvCal
  BFCalVal,                             ///< Bit field CalVal
  BFPStateToAccess,                     ///< Bit field PStateToAccess

  BFTxp,                                ///< Bit field Txp
  BFTxpdll,                             ///< Bit field Txpdll
  BFDramPwrMngm1Reg,                    ///< Bit field DRAM Power Management 1 register
  BFL3ScrbRedirDis,                     ///< Bit field L3ScrbRedirDis
  BFDQOdt03,                            ///< Bit field DQ Odt 0-3
  BFDQOdt47,                            ///< Bit field DQ Odt 4-7
  BFTriDM,                              ///< Bit field TriDM

  BFTcksrx,                             ///< Bit field Tcksrx
  BFTcksre,                             ///< Bit field Tcksre
  BFTckesr,                             ///< Bit field Tckesr
  BFTcpded,                             ///< Bit field Tcpded
  BFTpd,                                ///< Bit field Tpd

  BFNBPerfEventCounter3Low,             ///< Bit field Northbridge Performance Event Counter 3 Low
  BFNBPerfEventCounter3High,            ///< Bit field Northbridge Performance Event Counter 3 High
  BFFixedErrataSkipPorFreqCap,          ///< Bit field FixedErrataSkipPorFreqCap
  BFPerRankTimingEn,                    ///< Bit field PerRankTimingEn
  BFMemPhyPllPdMode,                    ///< Bit field MemPhyPllPdMode
  BFBankSwap,                           ///< Bit field BankSwap
  BFBwCapCmdThrottleMode,               ///< Bit field BwCapCmdThrottleMode
  BFRxChMntClkEn,                       ///< Bit field RxChMntClkEn
  BFBlockRxDqsLock,                     ///< Bit field BlockRxDqsLock
  BFRxSsbMntClkEn,                      ///< Bit field RxSsbMntClkEn
  BFPhyPSMasterChannel,                 ///< Bit field PhyPSMasterChannel

  BFObsrvVrefSel,                        ///< Bit field BFObsrvVrefSel
  BFVrefSel,                             ///< Bit field VrefSel
  BFVrefDAC,                             ///< Bit field VrefDAC
  BFDataByteRxDqsDLLDimm0Lane0,          ///< Bit field DataByteRxDqsDLL DIMM0 Lane 0
  BFDataByteRxDqsDLLDimm0Lane1,          ///< Bit field DataByteRxDqsDLL DIMM0 Lane 1
  BFDataByteRxDqsDLLDimm0Lane2,          ///< Bit field DataByteRxDqsDLL DIMM0 Lane 2
  BFDataByteRxDqsDLLDimm0Lane3,          ///< Bit field DataByteRxDqsDLL DIMM0 Lane 3
  BFDataByteRxDqsDLLDimm0Lane4,          ///< Bit field DataByteRxDqsDLL DIMM0 Lane 4
  BFDataByteRxDqsDLLDimm0Lane5,          ///< Bit field DataByteRxDqsDLL DIMM0 Lane 5
  BFDataByteRxDqsDLLDimm0Lane6,          ///< Bit field DataByteRxDqsDLL DIMM0 Lane 6
  BFDataByteRxDqsDLLDimm0Lane7,          ///< Bit field DataByteRxDqsDLL DIMM0 Lane 7
  BFDataByteRxDqsDLLDimm0Lane8,          ///< Bit field DataByteRxDqsDLL DIMM0 Lane 8
  BFDataByteRxDqsDLLDimm0Lane9,          ///< Bit field DataByteRxDqsDLL DIMM0 Lane 9
  BFDataByteRxDqsDLLDimm0LaneA,          ///< Bit field DataByteRxDqsDLL DIMM0 Lane A
  BFDataByteRxDqsDLLDimm0LaneB,          ///< Bit field DataByteRxDqsDLL DIMM0 Lane B
  BFDataByteRxDqsDLLDimm0LaneC,          ///< Bit field DataByteRxDqsDLL DIMM0 Lane C
  BFDataByteRxDqsDLLDimm0LaneD,          ///< Bit field DataByteRxDqsDLL DIMM0 Lane D
  BFDataByteRxDqsDLLDimm0LaneE,          ///< Bit field DataByteRxDqsDLL DIMM0 Lane E
  BFDataByteRxDqsDLLDimm0LaneF,          ///< Bit field DataByteRxDqsDLL DIMM0 Lane F
  BFDataByteRxDqsDLLDimm0Lane10,         ///< Bit field DataByteRxDqsDLL DIMM0 Lane 10
  BFDataByteRxDqsDLLDimm0Lane11,         ///< Bit field DataByteRxDqsDLL DIMM0 Lane 11
  BFDataByteRxDqsDLLDimm0Broadcast,      ///< Bit field DataByteRxDqsDLL DIMM0 Broadcast
  BFDataByteRxDqsDLLDimm1Lane0,          ///< Bit field DataByteRxDqsDLL DIMM1 Lane 0
  BFDataByteRxDqsDLLDimm1Lane1,          ///< Bit field DataByteRxDqsDLL DIMM1 Lane 1
  BFDataByteRxDqsDLLDimm1Lane2,          ///< Bit field DataByteRxDqsDLL DIMM1 Lane 2
  BFDataByteRxDqsDLLDimm1Lane3,          ///< Bit field DataByteRxDqsDLL DIMM1 Lane 3
  BFDataByteRxDqsDLLDimm1Lane4,          ///< Bit field DataByteRxDqsDLL DIMM1 Lane 4
  BFDataByteRxDqsDLLDimm1Lane5,          ///< Bit field DataByteRxDqsDLL DIMM1 Lane 5
  BFDataByteRxDqsDLLDimm1Lane6,          ///< Bit field DataByteRxDqsDLL DIMM1 Lane 6
  BFDataByteRxDqsDLLDimm1Lane7,          ///< Bit field DataByteRxDqsDLL DIMM1 Lane 7
  BFDataByteRxDqsDLLDimm1Lane8,          ///< Bit field DataByteRxDqsDLL DIMM1 Lane 8
  BFDataByteRxDqsDLLDimm1Lane9,          ///< Bit field DataByteRxDqsDLL DIMM1 Lane 9
  BFDataByteRxDqsDLLDimm1LaneA,          ///< Bit field DataByteRxDqsDLL DIMM1 Lane A
  BFDataByteRxDqsDLLDimm1LaneB,          ///< Bit field DataByteRxDqsDLL DIMM1 Lane B
  BFDataByteRxDqsDLLDimm1LaneC,          ///< Bit field DataByteRxDqsDLL DIMM1 Lane C
  BFDataByteRxDqsDLLDimm1LaneD,          ///< Bit field DataByteRxDqsDLL DIMM1 Lane D
  BFDataByteRxDqsDLLDimm1LaneE,          ///< Bit field DataByteRxDqsDLL DIMM1 Lane E
  BFDataByteRxDqsDLLDimm1LaneF,          ///< Bit field DataByteRxDqsDLL DIMM1 Lane F
  BFDataByteRxDqsDLLDimm1Lane10,         ///< Bit field DataByteRxDqsDLL DIMM1 Lane 10
  BFDataByteRxDqsDLLDimm1Lane11,         ///< Bit field DataByteRxDqsDLL DIMM1 Lane 11
  BFDataByteRxDqsDLLDimm1Broadcast,      ///< Bit field DataByteRxDqsDLL DIMM1 Broadcast
  BFDataByteRxDqsDLLDimm2Lane0,          ///< Bit field DataByteRxDqsDLL DIMM2 Lane 0
  BFDataByteRxDqsDLLDimm2Lane1,          ///< Bit field DataByteRxDqsDLL DIMM2 Lane 1
  BFDataByteRxDqsDLLDimm2Lane2,          ///< Bit field DataByteRxDqsDLL DIMM2 Lane 2
  BFDataByteRxDqsDLLDimm2Lane3,          ///< Bit field DataByteRxDqsDLL DIMM2 Lane 3
  BFDataByteRxDqsDLLDimm2Lane4,          ///< Bit field DataByteRxDqsDLL DIMM2 Lane 4
  BFDataByteRxDqsDLLDimm2Lane5,          ///< Bit field DataByteRxDqsDLL DIMM2 Lane 5
  BFDataByteRxDqsDLLDimm2Lane6,          ///< Bit field DataByteRxDqsDLL DIMM2 Lane 6
  BFDataByteRxDqsDLLDimm2Lane7,          ///< Bit field DataByteRxDqsDLL DIMM2 Lane 7
  BFDataByteRxDqsDLLDimm2Lane8,          ///< Bit field DataByteRxDqsDLL DIMM2 Lane 8
  BFDataByteRxDqsDLLDimm2Lane9,          ///< Bit field DataByteRxDqsDLL DIMM2 Lane 9
  BFDataByteRxDqsDLLDimm2LaneA,          ///< Bit field DataByteRxDqsDLL DIMM2 Lane A
  BFDataByteRxDqsDLLDimm2LaneB,          ///< Bit field DataByteRxDqsDLL DIMM2 Lane B
  BFDataByteRxDqsDLLDimm2LaneC,          ///< Bit field DataByteRxDqsDLL DIMM2 Lane C
  BFDataByteRxDqsDLLDimm2LaneD,          ///< Bit field DataByteRxDqsDLL DIMM2 Lane D
  BFDataByteRxDqsDLLDimm2LaneE,          ///< Bit field DataByteRxDqsDLL DIMM2 Lane E
  BFDataByteRxDqsDLLDimm2LaneF,          ///< Bit field DataByteRxDqsDLL DIMM2 Lane F
  BFDataByteRxDqsDLLDimm2Lane10,         ///< Bit field DataByteRxDqsDLL DIMM2 Lane 10
  BFDataByteRxDqsDLLDimm2Lane11,         ///< Bit field DataByteRxDqsDLL DIMM2 Lane 11
  BFDataByteRxDqsDLLDimm2Broadcast,      ///< Bit field DataByteRxDqsDLL DIMM2 Broadcast
  BFDataByteRxDqsDLLDimm3Lane0,          ///< Bit field DataByteRxDqsDLL DIMM3 Lane 0
  BFDataByteRxDqsDLLDimm3Lane1,          ///< Bit field DataByteRxDqsDLL DIMM3 Lane 1
  BFDataByteRxDqsDLLDimm3Lane2,          ///< Bit field DataByteRxDqsDLL DIMM3 Lane 2
  BFDataByteRxDqsDLLDimm3Lane3,          ///< Bit field DataByteRxDqsDLL DIMM3 Lane 3
  BFDataByteRxDqsDLLDimm3Lane4,          ///< Bit field DataByteRxDqsDLL DIMM3 Lane 4
  BFDataByteRxDqsDLLDimm3Lane5,          ///< Bit field DataByteRxDqsDLL DIMM3 Lane 5
  BFDataByteRxDqsDLLDimm3Lane6,          ///< Bit field DataByteRxDqsDLL DIMM3 Lane 6
  BFDataByteRxDqsDLLDimm3Lane7,          ///< Bit field DataByteRxDqsDLL DIMM3 Lane 7
  BFDataByteRxDqsDLLDimm3Lane8,          ///< Bit field DataByteRxDqsDLL DIMM3 Lane 8
  BFDataByteRxDqsDLLDimm3Lane9,          ///< Bit field DataByteRxDqsDLL DIMM3 Lane 9
  BFDataByteRxDqsDLLDimm3LaneA,          ///< Bit field DataByteRxDqsDLL DIMM3 Lane A
  BFDataByteRxDqsDLLDimm3LaneB,          ///< Bit field DataByteRxDqsDLL DIMM3 Lane B
  BFDataByteRxDqsDLLDimm3LaneC,          ///< Bit field DataByteRxDqsDLL DIMM3 Lane C
  BFDataByteRxDqsDLLDimm3LaneD,          ///< Bit field DataByteRxDqsDLL DIMM3 Lane D
  BFDataByteRxDqsDLLDimm3LaneE,          ///< Bit field DataByteRxDqsDLL DIMM3 Lane E
  BFDataByteRxDqsDLLDimm3LaneF,          ///< Bit field DataByteRxDqsDLL DIMM3 Lane F
  BFDataByteRxDqsDLLDimm3Lane10,         ///< Bit field DataByteRxDqsDLL DIMM3 Lane 10
  BFDataByteRxDqsDLLDimm3Lane11,         ///< Bit field DataByteRxDqsDLL DIMM3 Lane 11
  BFDataByteRxDqsDLLDimm3Broadcast,      ///< Bit field DataByteRxDqsDLL DIMM3 Broadcast
  BFDataByteDllPowerMgnByte0,           ///< Bit field DataByteDllPowerManagement for Byte 0
  BFDataByteDllPowerMgnByte1,           ///< Bit field DataByteDllPowerManagement for Byte 1
  BFDataByteDllPowerMgnByte2,           ///< Bit field DataByteDllPowerManagement for Byte 2
  BFDataByteDllPowerMgnByte3,           ///< Bit field DataByteDllPowerManagement for Byte 3
  BFDataByteDllPowerMgnByte4,           ///< Bit field DataByteDllPowerManagement for Byte 4
  BFDataByteDllPowerMgnByte5,           ///< Bit field DataByteDllPowerManagement for Byte 5
  BFDataByteDllPowerMgnByte6,           ///< Bit field DataByteDllPowerManagement for Byte 6
  BFDataByteDllPowerMgnByte7,           ///< Bit field DataByteDllPowerManagement for Byte 7
  BFDataByteDllPowerMgnByte8,           ///< Bit field DataByteDllPowerManagement for Byte ECC
  BFDataByteDllPowerMgnByteAll,         ///< Bit field DataByteDllPowerManagement for all bytes
  BFIoSkewMode,                         ///< Bit field for IoSkewMode
  BFLfsrRollOver,                       ///< Bit field for LfsrRollOver
  BFM1MemClkFreq,                       ///< Bit field M1MemClkFreq
  BFRate,                               ///< Bit field Rate
  BFFence2,                             ///< Bit field Fence2
  BFReducedLoop,                        ///< Bit field ReducedLoop

  BFNbVid0,                             ///< Bit field NbVid for NB Pstate 0
  BFNbVid0Hi,                           ///< Bit field 7th bit of NbVid for NB Pstate 0
  BFNbVid1,                             ///< Bit field NbVid for NB Pstate 1
  BFNbVid1Hi,                           ///< Bit field 7th bit of NbVid for NB Pstate 1
  BFNbVid2,                             ///< Bit field NbVid for NB Pstate 2
  BFNbVid2Hi,                           ///< Bit field 7th bit of NbVid for NB Pstate 2
  BFNbVid3,                             ///< Bit field NbVid for NB Pstate 3
  BFNbVid3Hi,                           ///< Bit field 7th bit of NbVid for NB Pstate 3

  BFMemPstate0,                         ///< Bit field MemPstate for NB Pstate 0
  BFMemPstate1,                         ///< Bit field MemPstate for NB Pstate 1
  BFMemPstate2,                         ///< Bit field MemPstate for NB Pstate 2
  BFMemPstate3,                         ///< Bit field MemPstate for NB Pstate 3
  BFMemPstateDis,                       ///< Bit field MemPstateDis
  BFMultiNodeCpu,                        ///< Bit field for MultiNodeCpu( MCM capability)

  BFNbFid0,                             ///< Bit field NbFid for NB Pstate 0
  BFNbFid1,                             ///< Bit field NbFid for NB Pstate 1
  BFNbFid2,                             ///< Bit field NbFid for NB Pstate 2
  BFNbFid3,                             ///< Bit field NbFid for NB Pstate 3

  BFNbDid0,                             ///< Bit field NbDid for NB Pstate 0
  BFNbDid1,                             ///< Bit field NbDid for NB Pstate 1
  BFNbDid2,                             ///< Bit field NbDid for NB Pstate 2
  BFNbDid3,                             ///< Bit field NbDid for NB Pstate 3

  BFRxBypass3rd4thStg,                  ///< Bit field RxBypass3rd4thStg
  BFRx4thStgEn,                         ///< Bit field Rx4thStgEn
  BFDllPDBypass,                        ///< Bit field DllPDBypass
  BFDllCSRBiasTrim,                     ///< Bit field DllCSRBiasTrim
  BFClkDllPDBypass,                     ///< Bit field ClkDllPDBypass
  BFCmdDllPDBypass,                     ///< Bit field CmdDllPDBypass
  BFAddrDllPDBypass,                    ///< Bit field AddrDllPDBypass
  BFRxDqInsDly,                         ///< Bit feild RxDqInsDly
  BFLowPowerRcvrEn,                     ///< Bit feild LowPowerRcvrEn
  BFDllNoLock,                          ///< Bit field DllNoLock
  BFEnSplitDctLimits,                   ///< Bit field EnSplitDctLimits
  BFEnSplitMctDatBuffers,               ///< Bit field EnSplitMctDatBuffers
  BFGmcTokenLimit,                      ///< Bit fieid GmcTokenLimit
  BFMctTokenLimit,                      ///< Bit field MctTokenLimit
  BFGmcToDctControl1,                   ///< Bit field GmcToDctControl1
  BFDllCSRBisaTrimDByte,                ///< Bit field DllCSRBisaTrimDByte
  BFDllCSRBisaTrimClk,                  ///< Bit field DllCSRBisaTrimClk
  BFDllCSRBisaTrimCsOdt,                ///< Bit field DllCSRBisaTrimCsOdt
  BFDllCSRBisaTrimAByte2,               ///< Bit field DllCSRBisaTrimAByte2
  BFReduceLoop,                         ///< Bit field ReduceLoop
  BFEffArbDis,                          ///< Bit field EffArbDis
  BFDramCommand0,                       ///< Bit field for DramCommand0 register
  BFDramCommand2,                       ///< Bit field for DramCommand2 register
  BFDramLoopbackAndTrainingControl,     ///< Bit field for DramLoopbackAndTrainingControl register
  BFCurMemPstate,                       ///< Bit field CurMemPstate
  BFDisMctHldRegSkidBuffer,             ///< Bit field DisMctHldRegSkidBuffer
  BFFreeOpbdDcqStallThresh,             ///< Bit field FreeOpbdDcqStallThresh

  BFMxMr0,                              ///< Bit field for MxMr0
  BFMxMr1,                              ///< Bit field for MxMr1
  BFMxMr2,                              ///< Bit field for MxMr2
  BFMxMr3,                              ///< Bit field for MxMr3
  BFMxMr4,                              ///< Bit field for MxMr4
  BFMxMr5,                              ///< Bit field for MxMr5
  BFMxMr6,                              ///< Bit field for MxMr6
  BFMxMrsEn,                            ///< Bit field for MxMrsEn
  BFVrefTrainMrsEn,                     ///< Bit field for VrefTrainMrsEn
  BFVrefTrain3Mrs6,                     ///< Bit field for VrefTrain3Mrs6

  BFUrgentTknDis,                       ///< Bit field for "UrgentTknDis"
  BFUrGmcMinTokens,                     ///< Bit field for "UrGmcMinTokens"
  BFUrGmcTokenLimit,                    ///< Bit field for "UrGmcTokenLimit"
  BFUrMctTokenLimit,                    ///< Bit field for "UrMctTokenLimit"
  BFUrMctMinTokens,                     ///< Bit field for "UrMctMinTokens"

  BFChAM1FenceSave,                     ///< Bit field for "ChAM1FenceSave"
  BFChBM1FenceSave,                     ///< Bit field for "ChBM1FenceSave"

  BFMctEccDisLatOptEn,                  ///< Bit field for "MctEccDisLatOptEn"
  BFCpuElevPrioDis,                     ///< Bit field for "CpuElevPrioDis"
  BFNbOffsetTrim,                       ///< Bit field for "NbOffsetTrim"
  BFECCExclusionBaseLow,                ///< Bit field for ECCExclusionBaseLow
  BFECCExclusionBaseHigh,               ///< Bit field for ECCExclusionBaseHigh
  BFECCExclusionLimitLow,               ///< Bit field for ECCExclusionLimitLow
  BFECCExclusionLimitHigh,              ///< Bit field for ECCExclusionLimitHigh
  BFCpuElevPrioPeriod,                  ///< Bit field for CpuElevPrioPeriod
  BFSbRdPtrInit,                        ///< Bit field for SbRdPtrInit
  BFCmdRdPtrInit,                       ///< Bit field for CmdRdPtrInit

  BFChannelEn,                          ///< Bit field ChannelEn
  BFDctEn,                              ///< Bit field DctEn
  BFDramSprLock,                        ///< Bit field DramSprLock
  BFBgMirrorDis,                        ///< Bit field BgMirrorDis
  BFDramType,                           ///< Bit field DramType
  BFPmuReset,                           ///< Bit field PmuReset
  BFPmuStall,                           ///< Bit field PmuStall
  BFD4ModeMByte,                        ///< Bit field D4Mode Master
  BFMemResetL,                          ///< Bit field MemReset_L
  BFAcsmCkeOride,                       ///< Bit field AcsmCkeOride
  BFAcsmCkeEnb,                         ///< Bit field AcsmCkeEnb
  BFDbiOnResetDrvEn,                    ///< Bit field DbiOnResetDrvEn
  BFDbiOnResetVal,                      ///< Bit field DbiOnResetVal

  BFGnbVidReqDis,                       ///< Bit field GndVidReqDis
  BFNbVidUseGnb,                        ///< Bit field NbVidUseGnb
  BFVidChgBusy,                         ///< Bit field VidChgBusy

  BFTrrdlG5,                            ///< Bit field Trrdl for GDDR5
  BFTrrdsG5,                            ///< Bit field Trrds for GDDR5
  BFTrcG5,                              ///< Bit field Trc for GDDR5
  BFTrcdWrG5,                           ///< Bit field TrcdWr for GDDR5
  BFTrcdRdG5,                           ///< Bit field TrcdRd for GDDR5
  BFTrasG5,                             ///< Bit field Tras for GDDR5
  BFTrtplG5,                            ///< Bit field Trtpl for GDDR5
  BFTrtpsG5,                            ///< Bit field Trtps for GDDR5
  BFTwrG5,                              ///< Bit field Twr for GDDR5
  BFTrpG5,                              ///< Bit field Trp for GDDR5
  BFT32awG5,                            ///< Bit field T32aw for GDDR5
  BFTppdG5,                             ///< Bit field Tppd for GDDR5
  BFTfawG5,                             ///< Bit field Tfaw for GDDR5
  BFTrwtTOG5,                           ///< Bit field TrwtTO for GDDR5
  BFTccdlG5,                            ///< Bit field Tccdl for GDDR5
  BFTccdsG5,                            ///< Bit field Tccds for GDDR5
  BFTwtrlG5,                            ///< Bit field Twtrl for GDDR5
  BFTwtrsG5,                            ///< Bit field Twtrs for GDDR5
  BFTcrcwlG5,                           ///< Bit field Tcrcwl for GDDR5
  BFTwlG5,                              ///< Bit field Twl for GDDR5
  BFTcrcrlG5,                           ///< Bit field Tcrcrl for GDDR5
  BFTclG5,                              ///< Bit field Tcl for GDDR5
  BFTrfcG5,                             ///< Bit field Trfc for GDDR5
  BFTrefG5,                             ///< Bit field Tref for GDDR5
  BFMemClkFreqValG5,                    ///< Bit field MemClkFreqVal for GDDR5
  BFMemClkFreqG5,                       ///< Bit field MemClkFreq for GDDR5
  BFTmrdG5,                             ///< Bit field Tmrd for GDDR5
  BFTcksrxG5,                           ///< Bit field Tcksrx for GDDR5
  BFTcksreG5,                           ///< Bit field Tcksre for GDDR5
  BFTckeG5,                             ///< Bit field Tcke for GDDR5
  BFTpdG5,                              ///< Bit field Tpd for GDDR5
  BFAggrPDDelayG5,                      ///< Bit field AggrPDDelay for GDDR5
  BFPchgPDEnDelayG5,                    ///< Bit field PchgPDEnDelay for GDDR5
  BFTxpnG5,                             ///< Bit field Txpn

  BFBankGrpEn,                        ///< Bit field BankGrpEn for GDDR5
  BFPptInterval,                      ///< Bit field PptInterval for GDDR5
  BFPptEn,                            ///< Bit field PptEn for GDDR5
  BFStartWCK,                         ///< Bit field StartWCK for GDDR5
  BFWrCrcEn,                          ///< Bit field WrCrcEn
  BFRdCrcEn,                          ///< Bit field RdCrcEn
  BFWDBIDis,                          ///< Bit field WDBIDis for GDDR5
  BFRDBIDis,                          ///< Bit field RDBIDis for GDDR5
  BFABILowLimit,                      ///< Bit field ABILowLimit for GDDR5
  BFABIDis,                           ///< Bit field ABIDis for GDDR5
  BFDataScrambleEn,                   ///< Bit field DataScrambleEn
  BFRdEdcLatency,                     ///< Bit field RdEdcLatency for GDDR5
  BFWrEdcLatency,                     ///< Bit field WrEdcLatency for GDDR5

  BFTclAdj,                             ///< Bit field TclAdj
  BFPhyTclAdj,                          ///< Bit field PhyTclAdj
  BFPhyDisable,                         ///< Bit field PhyDisable
  BFD4ModeAbyte,                        ///< Bit field D4ModeAbyte
  BFD4ModeDbyte,                        ///< Bit field BFD4ModeDbyte
  BFMajorModeAbyte,                     ///< Bit field MajorModeAbyte
  BFPowerDownRcvrAbyte,                 ///< Bit field PowerDownRcvrAbyte
  BFAVoltageLevel,                      ///< Bit field AVoltageLevel
  BFMajorModeDbyte,                     ///< Bit field MajorModeDbyte
  BFExtVrefRange,                       ///< Bit field ExtVrefRange
  BFAcsmDdrMode,                        ///< Bit field AcsmDdrMode
  BFPowerDownRcvrDbyte,                 ///< Bit field PowerDownRcvrDbyte
  BFVoltageLevel,                       ///< Bit field VoltageLevel
  BFCalD4D3,                            ///< Bit field CalD4D3
  BFG5Mode,                             ///< Bit field G5_Mode
  BFG5ModeAbyte,                        ///< Bit field G5_Mode ABYTE
  BFG5ModeDbyte,                        ///< Bit field G5_Mode DBYTE
  BFG5ModeAbit,                         ///< Bit field G5_Mode ABIT
  BFCalG5D3,                            ///< Bit field CalG5D3
  BFCalCmptrResTrim,                    ///< Bit field CalCmptrResTrim
  BFDsRdy,                              ///< Bit field Rdy for downstream mailbox
  BFDsMessage,                          ///< Bit field Message for downstream mailbox
  BFUsRdy,                              ///< Bit field UsRdy for upstream mailbox
  BFUsMessage,                          ///< Bit field Message for upstream mailbox
  BFRdy,                                ///< Bit field Rdy for upstream mailbox
  BFUs2Rdy,                             ///< Bit field UsRdy for upstream mailbox 2
  BFUs2Message,                         ///< Bit field Message for upstream mailbox 2
  BFRdy2,                               ///< Bit field Rdy for upstream mailbox 2
  BFPMUClkDiv,                          ///< Bit field PMUClkDiv
  BFDisPredriverCal,                    ///< Bit field DisPredriverCal
  BFPhyClkCtl,                          ///< Bit field PhyClkCtl
  BFCadMaxDurDllNoLock,                 ///< Bit field CAD MaxDurDllNoLock
  BFDataMaxDurDllNoLock,                ///< Bit field Data MaxDurDllNoLock
  BFBaseModel,                          ///< Bit field BaseModel
  BFExtModel,                           ///< Bit field ExtModel
  BFMajorMode,                          ///< Bit field MajorMode
  BFVrefHspeed,                         ///< Bit field VrefHspeed
  BFEQStrenHiP,                         ///< Bit field EQStrenHiP

  // Reserved
  BFReserved01,                         ///< Reserved 01
  BFReserved02,                         ///< Reserved 02
  BFReserved03,                         ///< Reserved 03
  BFReserved04,                         ///< Reserved 04
  BFReserved05,                         ///< Reserved 05
  BFReserved06,                         ///< Reserved 06
  BFReserved07,                         ///< Reserved 07
  BFReserved08,                         ///< Reserved 08
  BFReserved09,                         ///< Reserved 09
  BFReserved10,                         ///< Reserved 10

  BFReserved11,                         ///< Reserved 11
  BFReserved12,                         ///< Reserved 12
  BFReserved13,                         ///< Reserved 13
  BFReserved14,                         ///< Reserved 14
  BFReserved15,                         ///< Reserved 15
  BFReserved16,                         ///< Reserved 16
  BFReserved17,                         ///< Reserved 17
  BFReserved18,                         ///< Reserved 18
  BFReserved19,                         ///< Reserved 19
  BFReserved20,                         ///< Reserved 20
  BFIdsCmnMemReg,                       ///< Reserved for ids only, the value may dynamic change
  BFDctSelBaseAddrReg,                  ///< Bit field DctSelBaseAddrReg
  BFDctSelBaseOffsetReg,                ///< Bit field DctSelBaseOffsetReg
  BFPDPhyPSDis,                         ///< Bit field PDPhyPSDis F2xA8[19]
  BFGlobalVrefInSel,                    ///< Bit field GlobalVrefInSel
  BFGlobalVrefOut0Sel,                  ///< Bit field GlobalVrefOut0Sel
  BFGlobalVrefOut1Sel,                  ///< Bit field GlobalVrefOut1Sel
  BFGlobalVrefOut2Sel,                  ///< Bit field GlobalVrefOut2Sel
  BFGlobalVrefOut3Sel,                  ///< Bit field GlobalVrefOut3Sel
  BFCadDllPumpPeriod,                   ///< Bit field CAD DllPumpPeriod
  BFDataDllPumpPeriod,                  ///< Bit field Dbyte DllPumpPeriod
  BFCadDllPumpCount,                   ///< Bit field CAD DllPumpCount
  BFDataDllPumpCount,                  ///< Bit field Dbyte DllPumpCount
  BFVddpVddrLowVoltSupp,                ///< F3x1FC ScratchFuse Bit 21
  BFLgcyMmioHoleEn1,                    ///< Bit field LgcyMmioHoleEn1
  BFLgcyMmioHoleEn2,                    ///< Bit field LgcyMmioHoleEn2
  BFLgcyMmioHoleEn3,                    ///< Bit field LgcyMmioHoleEn3
  BFDctIntLvEn0,                        ///< Bit field DctIntLvEn0
  BFDctIntLvEn1,                        ///< Bit field DctIntLvEn1
  BFDctIntLvEn2,                        ///< Bit field DctIntLvEn2
  BFDctIntLvEn3,                        ///< Bit field DctIntLvEn3
  BFCalRun,                             ///< Bit field CalRun
  BFCalOnce,                            ///< Bit field CalOnce
  BFCalInitMode,                        ///< Bit field CalInitMode
  BFPmuSramSd,                          ///< Bit field PmuSramSd
  BFDctClkGateEn,                       ///< Bit DctClkGateEn
  BFDctPwrGateEn,                       ///< Bit DctPwrGateEn
  /* End of accessible list --- entries below this line are for private use ------------*/
  BFEndOfList,                          ///< End of bit field list

  // Only for Table Drive Support define.
  BFRcvEnDly,       ///< F2x[1,0]9C_x[2B:10] Dram DQS Receiver Enable Timing Control Registers
  BFWrDatDly,       ///< F2x[1, 0]9C_x[302:301, 202:201, 102:101, 02:01] DRAM Write Data Timing [High:Low] Registers
  BFRdDqsDly,       ///< F2x[1, 0]9C_x[306:305, 206:205, 106:105, 06:05] DRAM Read DQS Timing Control [High:Low] Registers
  BFWrDqsDly,       ///< F2x[1, 0]9C_x[4A:30] DRAM DQS Write Timing Control Registers
  BFPhRecDly,       ///< F2x[1, 0]9C_x[51:50] DRAM Phase Recovery Control Register [High:Low] Registers
  BFRdDqs2dDly,     ///< F2x[1, 0]9C_x0D0F_0[F,8:0]2[3:0]
  BFWrDat2dDly,     ///< F2x[1, 0]9C_x0000_0[3:0]0[3:1]
  /* Do not define any entries beyond this point */
  BFAbsLimit                            ///< Beyond this point is reserved for bit field manipulation

} BIT_FIELD_NAME;

/// Bit field aliases
#define BFMainPllOpFreqId       BFNbFid
#define BFNbDid                 BFNbPs0NclkDiv
#define BFRdDramTrainMode       BFPrefDramTrainMode
#define BFThrottleEn            BFCmdThrottleMode
#define BFIntlvRegionEn         BFIntLvRgnSwapEn
#define BFIntlvRegionBase       BFIntLvRgnBaseAddr
#define BFIntlvRegionLimit      BFIntLvRgnLmtAddr
#define BFRdOdtPatReg           BFPhyRODTCSLow
#define BFWrOdtPatReg           BFPhyWODTCSLow
#define BFLockDramCfg           BFC6DramLock
#define BFRstRcvFifo            BFTwr
#define BFDramCmd2Reg           BFCmdBank
#define BFDramODTCtlReg         BFRdOdtTrnOnDly

/// Bit field names per DRAM CS base address register
typedef enum {
  BFCSEnable = 0,               ///< Chip select enable
  BFSpare = 1,                  ///< Spare rank
  BFTestFail = 2,               ///< Memory test failed
  BFOnDimmMirror = 3            ///< on-DIMM mirroring enable
} CS_BASE_BIT_FIELD;

/// Flag for exclude dimm
typedef enum {
  NORMAL,                       ///< Normal mode, exclude the dimm if there is new dimm failure
  TRAINING,                     ///< Training mode, exclude dimms that fail during training after training is done
  END_TRAINING                  ///< End training mode, exclude all dimms that failed during training
} DIMM_EXCLUDE_FLAG;

#define BSP_DIE   0
#define MAX_NODES_SUPPORTED     1   ///< Maximum number of nodes in the system.
#define MAX_CS_PER_CHANNEL      8   ///< Max CS per channel
#define MAX_BYTELANES 8   ///< 8 byte lanes
#define MAX_DELAYS    9   ///< 8 data bytes + 1 ECC byte
#define MAX_DIMMS     4   ///< 4 DIMMs per channel

#define VDDIO_DETERMINED      0xFF  ///< VDDIO has been determined yet. Further processing is not needed.

/// Indicators for voltage stages
typedef enum {
  VOLT_NORMAL_STAGE = 0,    ///< Indicate normal voltage stage
  VOLT_AMP_STAGE            ///< Indicate AMP voltage stage
} VOLT_STAGE;

///
/// MEM_SHARED_DATA
/// This structure defines the shared data area that is used by the memory
/// code to share data between different northbridge objects. Each substructure
/// in the data area defines how this data area is used by a different purpose.
///
/// There should only be one instance of this struct created for all of the memory
/// code to use.
///
typedef struct _MEM_SHARED_DATA {

  // System memory map data
  UINT32  CurrentNodeSysBase;       ///< Base[47:16] (system address) DRAM base address for current node.
  /// Memory map data for each node
  BOOLEAN AllECC; ///< ECC support on the system
  DIMM_EXCLUDE_FLAG DimmExcludeFlag;  ///< Control the exclude dimm behavior
  UINT8 VoltageMap; ///< The commonly supported voltage map in the system

  UINT8   TopNode;   ///< Node that has its memory  mapped to TOPMEM/TOPMEM2
  BOOLEAN C6Enabled; ///< TRUE if   C6  is  enabled
  BOOLEAN AcpEngineEnabled; ///< TRUE if ACP Engine is enabled
  BOOLEAN LastPmuTrainFail; ///< Used to indicate the result of the last PMU training
  UINT8   TimesPmuRetrain; ///< Used to save the remaining chances of PMU training retry

  /// Data structure for NB register table
  struct {
    UINT64  FamilyId;               ///< LogicalCpuid.Family
    UINT32  *NBRegTable;            ///< Pointer to allocated buffer for NBRegTable
  } NBRegMap[MAX_NODES_SUPPORTED];

  /// Data structure for node map
  struct {
    BOOLEAN IsValid;                ///< TRUE if this node contains memory.
    UINT32  SysBase;                ///< Base[47:16] (system address) DRAM base address of this node.
    UINT32  SysLimit;               ///< Base[47:16] (system address) DRAM limit address of this node.
  } NodeMap[MAX_NODES_SUPPORTED];
  BOOLEAN UndoHoistingAbove1TB;     ///< Undo hoisting above 1TB

  /// Data structure for node interleave feature
  struct {
    BOOLEAN IsValid;                ///< TRUE if the data in this structure is valid.
    UINT8   NodeCnt;                ///< Number of nodes in the system.
    UINT32  NodeMemSize;            ///< Total memory of this node.
    UINT32  Dct0MemSize;            ///< Total memory of this DCT 0.
    UINT8   NodeIntlvSel;           ///< Index to each node.
  } NodeIntlv;

  /// Common Voltage Min/Max Values
  UINT8 CommonSmallestMaxNegVref;   ///< Common smallest negative Vref across platfrom
  UINT8 CommonSmallestMaxPosVref;   ///< Common smallest positive Vref across platform

} MEM_SHARED_DATA;

///
/// MEM_MAIN_DATA_BLOCK
///
typedef struct _MEM_MAIN_DATA_BLOCK {
  struct _MEM_DATA_STRUCT *MemPtr;          ///< Pointer to customer shared data
  struct _MEM_NB_BLOCK    *NBPtr;           ///< Pointer to array of NB Blocks
  struct _MEM_TECH_BLOCK  *TechPtr;         ///< Pointer to array of Tech Blocks
  struct _MEM_SHARED_DATA *mmSharedPtr;     ///< Pointer to shared data area.
  UINT8           DieCount;                 ///< Total number of Dies installed
} MEM_MAIN_DATA_BLOCK;

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */


/*
node: Indicates the Node
- Value ranges from 0-7, 0xF: for all nodes
- Size - 4 Bits

dct: Indicate the DRAM Controller
- Value is 0, 1 (0xF: apply setting to all DCTs)
- Size - 4 Bits

dimm: This values specifies which DIMM register will be applied
- The value varies from 0 to 3, 0xF: all DIMMs
- Size - 4 Bits

attr - Indicates if the value needs to be added, subtracted, overridden or Auto (not changed)
- 0: Do not change the current value in the register
- 1: Use the value provided in the table to override the current value in the register (the one that AGESA initially determined)
- 2: Add the value provided in the table as an offset to the current value in the register (the one that AGESA initially determined)
- 3: Subtract the value provided in the table as an offset to the current value in the register (the one that AGESA initially determined)
- Size - 2 Bits

time - Indicate the timing for the register which is written.
- 0:  Write register value before Dram init
- 1:  Write register value before memory training
- 2:  Write register value after memory training
- Size - 1 Bits

bytelane: bytelane number
- This determines specifies which bytelane register will be applied
- Bit0 =1 - set value into Bytelane0
- Bit1 =1 - set value into Bytelane1
- Bit2 =1 - set value into Bytelane2
...
...
- 0xFFFF: all bytelane
- Size - 16 Bits.

bfIndex: Indicate the bitfield index
- Size - 16 Bits

value - Value to be used
- This can be an offset (sub or Add) or an override value.
- Size - DWORD
*/

// Sample code
// NBACCESS (MTBeforeDInit, MTNodes,  MTDct0, BFCSBaseAddr5Reg, MTOverride,   0x400001),
// NBACCESS (MTBeforeTrn,   MTNodes,  MTDct1, BFCSBaseAddr7Reg, MTOverride,   0xFFFFFFFF),
// DQSACCESS (MTAfterTrn,    MTNodes,  MTDcts, MTDIMM0,  MTBL1+MTBL2,    BFRcvEnDly, MTSubtract,   2),
// DQSACCESS (MTAfterTrn,    MTNodes,  MTDct1, MTDIMM1,  MTBLNOECC,      BFRcvEnDly, MTAdd,        1),

#define ENDMEMTDS  0, 0, 0, 0, 0, 0, 0xFFFFFFFFul, 0

#define NBACCESS(time, node, dct, bitfield, attr, value) \
{ (time), \
  ((node) & 0x0F) | ((dct) << 4), \
  (((attr) & 0x07) << 4) | (VT_MSK_VALUE << 7) , \
  (UINT8)((bitfield) & 0x000000FF), \
  (UINT8)(((bitfield) >> 8)  & 0x000000FF), \
  (UINT8)(((bitfield) >> 16) & 0x000000FF), \
  (UINT8)(((bitfield) >> 24) & 0x000000FF), \
  0, 0, \
  (UINT8)((value) & 0x000000FF), \
  (UINT8)(((value) >> 8)  & 0x000000FF), \
  (UINT8)(((value) >> 16) & 0x000000FF), \
  (UINT8)(((value) >> 24) & 0x000000FF), \
  0, 0, 0 \
}

#define DQSACCESS(time, node, dct, dimm, bitfield, attr, b0, b1, b2, b3, b4, b5, b6, b7, b8) \
{ (time), \
  ((node) & 0x0F) | ((dct) << 4), \
  (((dimm) & 0x0F)  | ((attr) & 0x07) << 4) | (VT_ARRAY << 7) , \
  (UINT8)((bitfield) & 0x000000FF), \
  (UINT8)(((bitfield) >> 8)  & 0x000000FF), \
  (UINT8)(((bitfield) >> 16) & 0x000000FF), \
  (UINT8)(((bitfield) >> 24) & 0x000000FF), \
  (b0), (b1), (b2), (b3), (b4), (b5), (b6), (b7), (b8) \
}

#define DQS2DACCESS(time, node, dct, dimm, bitfield, attr, l0, l1, l2, l3, l4, l5, l6, l7, l8, l9, l10, l11, l12, l13, l14, l15, l16, l17) \
{ (time), \
  ((node) & 0x0F) | ((dct) << 4), \
  (((dimm) & 0x0F)  | ((attr) & 0x07) << 4) | (VT_ARRAY << 7) , \
  (UINT8)((bitfield) & 0x000000FF), \
  (UINT8)(((bitfield) >> 8)  & 0x000000FF), \
  (UINT8)(((bitfield) >> 16) & 0x000000FF), \
  (UINT8)(((bitfield) >> 24) & 0x000000FF), \
  (UINT8)((l0) & 0xFF), \
  (UINT8)(((l0) >> 8) & 0xFF), \
  (UINT8)((l1) & 0xFF), \
  (UINT8)(((l1) >> 8) & 0xFF), \
  (UINT8)((l2) & 0xFF), \
  (UINT8)(((l2) >> 8) & 0xFF), \
  (UINT8)((l3) & 0xFF), \
  (UINT8)(((l3) >> 8) & 0xFF), \
  (UINT8)((l4) & 0xFF), \
  (UINT8)(((l4) >> 8) & 0xFF), \
  (UINT8)((l5) & 0xFF), \
  (UINT8)(((l5) >> 8) & 0xFF), \
  (UINT8)((l6) & 0xFF), \
  (UINT8)(((l6) >> 8) & 0xFF), \
  (UINT8)((l7) & 0xFF), \
  (UINT8)(((l7) >> 8) & 0xFF), \
  (UINT8)((l8) & 0xFF), \
  (UINT8)(((l8) >> 8) & 0xFF), \
  (UINT8)((l9) & 0xFF), \
  (UINT8)(((l9) >> 8) & 0xFF), \
  (UINT8)((l10) & 0xFF), \
  (UINT8)(((l10) >> 8) & 0xFF), \
  (UINT8)((l11) & 0xFF), \
  (UINT8)(((l11) >> 8) & 0xFF), \
  (UINT8)((l12) & 0xFF), \
  (UINT8)(((l12) >> 8) & 0xFF), \
  (UINT8)((l13) & 0xFF), \
  (UINT8)(((l13) >> 8) & 0xFF), \
  (UINT8)((l14) & 0xFF), \
  (UINT8)(((l14) >> 8) & 0xFF), \
  (UINT8)((l15) & 0xFF), \
  (UINT8)(((l15) >> 8) & 0xFF), \
  (UINT8)((l16) & 0xFF), \
  (UINT8)(((l16) >> 8) & 0xFF), \
  (UINT8)((l17) & 0xFF), \
  (UINT8)(((l17) >> 8) & 0xFF) \
}
/// Type of modification supported by table driven support.
typedef enum {
  MTAuto,                       ///<  Do not change the current value in the register
  MTOverride,                   ///<  Use the value provided in the table to override the current value in the register
  MTSubtract,                   ///<  Subtract the value provided in the table as an offset to the current value in the register
  MTAdd,                         ///<  Add the value provided in the table as an offset to the current value in the reg
  MTAnd,                         ///<  And the value provided in the table as an offset to the current value in the reg
  MTOr                         ///<  Or the value provided in the table as an offset to the current value in the reg
} MTAttr;

/// Time for table driven support to make modification.
/// NOTE please keep the order of below MT points
typedef enum {
  MTBeforeInitializeMCT = 0,        ///<  00 Before InitializeMCT
  MTAfterAutoCycTiming,             ///<  01 After Auto Cycle Timing
  MTAfterPlatformSpec,              ///<  02 After Platform Specific Configuration
  MTBeforeDInit,                    ///<  03 Before Dram init
  MTBeforeTrn,                      ///<  04 Before memory training
  MTAfterMemPstate1PartialTrn,      ///<  05 After partial training at starting frequency
  MTAfterTrn,                       ///<  06 After memory training
  MTAfterSwWLTrn,                   ///<  07 After SW Based WL Training
  MTAfterHwWLTrnP1,                 ///<  08 After HW Based WL Training Part 1
  MTAfterHwRxEnTrnP1,               ///<  09 After HW Based Receiver Enable Training Part 1
  MTAfterFreqChg,                   ///<  10 After each frequency change
  MTAfterHwWLTrnP2,                 ///<  11 After HW Based WL Training Part 2
  MTAfterHwRxEnTrnP2,               ///<  12 After HW Based Receiver Enable Training Part 2
  MTAfterSwRxEnTrn,                 ///<  13 After SW Based Receiver Enable Training
  MTAfterDqsRwPosTrn,               ///<  14 After DQS Read/Write Position Training
  MTAfterMaxRdLatTrn,               ///<  15 After Max Read Latency Training
  MTAfterNbPstateChange,            ///<  16 After programming NB Pstate dependent registers
  MTAfterInterleave,                ///<  17 After Programming Interleave registers
  MTAfterSettingMemoryPstate1,      ///<  18 After programming registers for memory pstate 1
  MTAfterFinalizeMCT,               ///<  19 After Finalize MCT Programming

  MTValidTimePointLimit,        ///< Mark the upper bound of the supported time points
  MTEnd = 0xFF ///< End of enum define.
} MTTime;

/// Node on which modification should be made by table driven support.
typedef enum {
  MTNode0, ///< Node 0.
  MTNode1, ///< Node 1.
  MTNode2, ///< Node 2.
  MTNode3, ///< Node 3.
  MTNode4, ///< Node 4.
  MTNode5, ///< Node 5.
  MTNode6, ///< Node 6.
  MTNode7, ///< Node 7.
  MTNodes = 0xF                   ///<  all nodes
} MTNode;

/// DCT on which modification should be made by table driven support.
typedef enum {
  MTDct0, ///< DCT 0.
  MTDct1, ///< DCT 1.
  MTDct2, ///< DCT 2.
  MTDct3, ///< DCT 3.
  MTDcts = 0x7,                   ///<  all dcts
} MTDct;

/// Dimm on which modification should be made by table driven support.
typedef enum {
  MTDIMM0, ///< Dimm 0.
  MTDIMM1, ///< Dimm 1.
  MTDIMM2, ///< Dimm 2.
  MTDIMM3, ///< Dimm 3.
  MTDIMMs = 0xF,                  ///<  all Dimms
} MTDIMM;

/// Bytelane on which modification should be made by table driven support.
typedef enum {
  MTBL0 = 0x1,                    ///<  set the value into Bytelane0
  MTBL1 = 0x2,                    ///<  set the value into Bytelane1
  MTBL2 = 0x4,                    ///<  set the value into Bytelane2
  MTBL3 = 0x8,                    ///<  set the value into Bytelane3
  MTBL4 = 0x10, ///< set the value into Bytelane4
  MTBL5 = 0x20, ///< set the value into Bytelane5
  MTBL6 = 0x40, ///< set the value into Bytelane6
  MTBL7 = 0x80, ///< set the value into Bytelane7
  MTBL8 = 0x100,                  ///<  set the value into ECC
  MTBLNOECC = 0xFF,               ///<  all Bytelanes except ECC
  MTBLs = 0xFFFF,                 ///<  all Bytelanes
} MTBL;

/// Values used to indicate which type of training is being done.
typedef enum {
  TRN_RCVR_ENABLE,                     ///< Reciever Enable Training
  TRN_DQS_POSITION,                    ///< Read/Write DQS Position training
  TRN_MAX_READ_LATENCY                 ///< Max read Latency training
} TRAINING_TYPE;

/// Memory Pstate
typedef enum {
  MEMORY_PSTATE0,   ///< Memory Pstate 0
  MEMORY_PSTATE1,   ///< Memory Pstate 1
} MEM_PSTATE;

/// Memory Pstate
typedef enum {
  NB_PSTATE0,   ///< NB Pstate 0
  NB_PSTATE1,   ///< NB Pstate 1
  NB_PSTATE2,   ///< NB Pstate 2
  NB_PSTATE3,   ///< NB Pstate 3
} NB_PSTATE;

/// Memory Pstate Training Stage
typedef enum {
  MEMORY_PSTATE_NONE = 0,           ///< Memory Pstate processing dummy stage
  MEMORY_PSTATE_1ST_STAGE = 0xF1,   ///< Memory Pstate processing stage 1, in which full training is done at DDR667
  MEMORY_PSTATE_2ND_STAGE,          ///< Memory Pstate processing stage 2, in which partial trainig will be done at DDR800 - target speed
  MEMORY_PSTATE_3RD_STAGE,          ///< Memory Pstate processing stage 3, in which full training will be done at target frequency and MaxRdLatency training will start
  MEMORY_PSTATE_S3_STAGE            ///< Memory Pstate Processing in S3.
} MEM_PSTATE_STAGE;

/// RdDqsDly Retrain status
typedef enum {
  RDDQSDLY_RTN_NEEDED = 0xF1,       ///< RdDqsDly retrain may be needed
  RDDQSDLY_RTN_SUSPEND,             ///< RdDqsDly retrain is suspected
  RDDQSDLY_RTN_ONGOING              ///< RdDqsDly retrain condition is just detected
} RDDQSDLY_RTN_STAT;

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */
AGESA_STATUS
AmdMemAuto (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

AGESA_STATUS
MemAmdFinalize (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

AGESA_STATUS
MemSocketScan (
  IN OUT   MEM_MAIN_DATA_BLOCK *mmPtr
  );

VOID
SetMemError (
  IN       AGESA_STATUS  Errorval,
  IN OUT   DIE_STRUCT *MCTPtr
  );

VOID
memDefRet ( VOID );

BOOLEAN
memDefTrue ( VOID );

BOOLEAN
memDefFalse ( VOID );

AGESA_STATUS
memDefRetSuccess ( VOID );

VOID
memDefaultUndefined ( VOID );

VOID
MemFS3Wait10ns (
  IN       UINT32 Count,
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

VOID
AmdMemFunctionListDef (
  IN OUT   VOID *pMemData
  );

VOID
MemMDisableScrubber (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  );

VOID
MemMRestoreScrubber (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  );
#endif  /* _MM_H_ */



