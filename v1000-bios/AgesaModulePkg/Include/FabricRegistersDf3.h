/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Register Definitions for Data Fabric 3.0 based products
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _FABRIC_REGISTERS_DF3_H_
#define _FABRIC_REGISTERS_DF3_H_
#pragma pack (push, 1)

/* Fabric Block Instance Count Register */
#define FABBLKINSTCOUNT_FUNC 0x0
#define FABBLKINSTCOUNT_REG  0x040
#define FABBLKINSTCOUNT_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FABBLKINSTCOUNT_FUNC, FABBLKINSTCOUNT_REG))

/// Fabric Block Instance Count Register
typedef union {
  struct {                           ///< Bitfields of Fabric Block Instance Count Register
    UINT32 BlkInstCount:8;           ///< Block Instance Count
    UINT32 :24;                      ///< Reserved
  } Field;
  UINT32  Value;
} FABRIC_BLK_INST_COUNT_REGISTER;

/* VGA Enable Register */
#define VGA_EN_FUNC 0x0
#define VGA_EN_REG  0x080
#define VGA_EN_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, VGA_EN_FUNC, VGA_EN_REG))

/// VGA Enable Register
typedef union {
  struct {                           ///< Bitfields of VGA Enable Register
    UINT32 VgaEn_VE:1;               ///< VGA Enable
    UINT32 VgaEn_NP:1;               ///< Non-Posted
    UINT32 VgaEn_CpuDis:1;           ///< CpuDis
    UINT32 :1;                       ///< Reserved
    UINT32 VgaEn_DstFabricID:10;     ///< Destination Fabric ID
    UINT32 :18;                      ///< Reserved
  } Field;
  UINT32  Value;
} VGA_EN_REGISTER;

/* Configuration Address Control Register */
#define CFGADDRESSCTRL_FUNC 0x0
#define CFGADDRESSCTRL_REG  0x084
#define CFGADDRESSCTRL_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, CFGADDRESSCTRL_FUNC, CFGADDRESSCTRL_REG))

/// Configuration Address Control Register
typedef union {
  struct {                           ///< Bitfields of Configuration Address Control Register
    UINT32 SecBusNum:8;              ///< Secondary Bus Number
    UINT32 :24;                      ///< Reserved
  } Field;
  UINT32  Value;
} CFG_ADDRESS_CTRL_REGISTER;

/* Configuration Address Maps Register */
#define CFGADDRESSMAP_FUNC 0x0
#define CFGADDRESSMAP0_REG  0x0A0
#define CFGADDRESSMAP0_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, CFGADDRESSMAP_FUNC, CFGADDRESSMAP0_REG))

/// Configuration Address Maps Register
typedef union {
  struct {                           ///< Bitfields of Configuration Address Maps Register
    UINT32 RE:1;                     ///< Read Enable
    UINT32 WE:1;                     ///< Write Enable
    UINT32 :2;                       ///< Reserved
    UINT32 DstFabricID:10;           ///< Destination Fabric ID
    UINT32 :2;                       ///< Reserved
    UINT32 BusNumBase:8;             ///< Bus Number Base
    UINT32 BusNumLimit:8;            ///< Bus Number Limit
  } Field;
  UINT32  Value;
} CFG_ADDRESS_MAP_REGISTER;

/* DRAM Hole Control Register */
#define DRAMHOLECTRL_FUNC 0x0
#define DRAMHOLECTRL_REG  0x104
#define DRAMHOLECTRL_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, DRAMHOLECTRL_FUNC, DRAMHOLECTRL_REG)
#define DF_DramHoleControl    DRAMHOLECTRL_PCI_ADDR

/// DRAM Hole Control Register
typedef union {
  struct {                           ///< Bitfields of DRAM Hole Control Register
    UINT32 DramHoleValid:1;          ///< DRAM Hole Valid
    UINT32 :23;                      ///< Reserved
    UINT32 DramHoleBase:8;           ///< DRAM Hole Base
  } Field;
  UINT32  Value;
} DRAM_HOLE_CONTROL_REGISTER;

/* DRAM Base Address Register */
#define DRAMBASEADDR0_FUNC 0x0
#define DRAMBASEADDR0_REG  0x110
#define DRAMBASEADDR0_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, DRAMBASEADDR0_FUNC, DRAMBASEADDR0_REG)
#define DF_DramBaseAddress0    DRAMBASEADDR0_PCI_ADDR

#define DRAM_REGION_REGISTER_OFFSET  0x8

#define DRAM_INTLV_ADDR_SEL_SHIFT         8
#define DRAM_INTLV_ADDR_SEL_MASK          0xFF
#define DRAM_INTLV_ADDR_SEL_SUPPORT_MASK  0x0F

/// DRAM Base Address Register
typedef union {
  struct {                           ///< Bitfields of DRAM Base Address Register
    UINT32 AddrRngVal:1;             ///< Address Range Valid
    UINT32 LgcyMmioHoleEn:1;         ///< Legacy MMIO Hole Enable
    UINT32 IntLvNumChan:4;           ///< Interleave Number Of Channels
    UINT32 IntLvNumDies:2;           ///< Interleave Number Of Dies
    UINT32 IntLvNumSockets:1;        ///< Interleave Number Of Sockets
    UINT32 IntLvAddrSel:3;           ///< Interleave Address Select
    UINT32 DramBaseAddr:20;          ///< DRAM Base Address
  } Field;
  UINT32  Value;
} DRAM_BASE_ADDRESS_REGISTER;

/* DRAM Limit Address Register */
#define DRAMLIMITADDR0_FUNC 0x0
#define DRAMLIMITADDR0_REG  0x114
#define DRAMLIMITADDR0_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, DRAMLIMITADDR0_FUNC, DRAMLIMITADDR0_REG)
#define DF_DramLimitAddress0    DRAMLIMITADDR0_PCI_ADDR

/// DRAM Limit Address Register
typedef union {
  struct {                           ///< Bitfields of DRAM Limit Address Register
    UINT32 DstFabricID:10;           ///< Destination Fabric ID
    UINT32 :2;                       ///< Reserved
    UINT32 DramLimitAddr:20;         ///< DRAM Limit Address
  } Field;
  UINT32  Value;
} DRAM_LIMIT_ADDRESS_REGISTER;


/* DF Global Clock Gater Register */
#define DF_GLBL_CLK_GATER_FUNC 0x0
#define DF_GLBL_CLK_GATER_REG 0x3F0
#define DF_GLBL_CLK_GATER_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, DF_GLBL_CLK_GATER_FUNC, DF_GLBL_CLK_GATER_REG))

/// DF Global Clock Gater Register
typedef union {
  struct {                                ///< Bitfields of DF Global Clock Gater Register
    UINT32 MGCGMode:4;                    ///< Mid-grain clock gating mode
    UINT32 :28;                           ///< Reserved
  } Field;
  UINT32 Value;
} DF_GLBL_CLK_GATER_REGISTER;

/* DF Global Control Register */
#define DF_GLBL_CTRL_FUNC 0x0
#define DF_GLBL_CTRL_REG 0x3F8
#define DF_GLBL_CTRL_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, DF_GLBL_CTRL_FUNC, DF_GLBL_CTRL_REG))

/// DF Global Control Register
typedef union {
  struct {                                ///< Bitfields of DF Global Control Register
    UINT32 :3;                            ///< Reserved
    UINT32 DisImmSyncFloodOnFatalErr:1;   ///< Disable immediate sync flood on fatal error
    UINT32 WDTBaseSel:2;                  ///< Watchdog timer base select
    UINT32 :2;                            ///< Reserved
    UINT32 PIEWDTCntSel:3;                ///< PIE watchdog timer count select
    UINT32 :1;                            ///< Reserved
    UINT32 IOMWDTCntSel:3;                ///< IO master watchdog timer count select
    UINT32 :1;                            ///< Reserved
    UINT32 CCMWDTCntSel:3;                ///< Core coherent master watchdog timer count select
    UINT32 :1;                            ///< Reserved
    UINT32 GlbHashIntlvCtl64K:1;          ///< Global hash interleave 64K control
    UINT32 GlbHashIntlvCtl2M:1;           ///< Global hash interleave 2M control
    UINT32 GlbHashIntlvCtl1G:1;           ///< Global hash interleave 1G control
    UINT32 :9;                            ///< Reserved
  } Field;
  UINT32 Value;
} DF_GLBL_CTRL_REGISTER;

/* System Configuration Register */
#define SYSCFG_FUNC 0x1
#define SYSCFG_REG  0x200
#define SYSCFG_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, SYSCFG_FUNC, SYSCFG_REG))

/// System Configuration Register
typedef union {
  struct {                             ///< Bitfields of System Configuration Register
    UINT32 DiePresent:8;               ///< Local Die Present
    UINT32 :3;                         ///< Reserved
    UINT32 DieType:8;                  ///< Die type map
    UINT32 :4;                         ///< Reserved
    UINT32 MyDieType:2;                ///< Local die type
    UINT32 OtherDieType:2;             ///< Other die type
    UINT32 OtherSocketPresent:1;       ///< Other socket present
    UINT32 MyNodeId:3;                 ///< Node ID
    UINT32 :1;                         ///< Reserved
  } Field;
  UINT32  Value;
} SYS_CFG_REGISTER;

/* System Component Count Register */
#define SYSCOMPCOUNT_FUNC 0x1
#define SYSCOMPCOUNT_REG  0x204
#define SYSCOMPCOUNT_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, SYSCOMPCOUNT_FUNC, SYSCOMPCOUNT_REG))

/// System Configuration Register
typedef union {
  struct {                             ///< Bitfields of System Configuration Register
    UINT32 PIECount:8;                 ///< PIE Count
    UINT32 GCMCount:8;                 ///< GCM Count
    UINT32 IOMCount:8;                 ///< IOMS Count
    UINT32 :8;                         ///< Reserved
  } Field;
  UINT32  Value;
} SYS_COMP_COUNT_REGISTER;

/* PCS Enable Register */
#define PCSENABLE_FUNC 0x1
#define PCSENABLE_REG  0x330
#define PCSENABLE_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, PCSENABLE_FUNC, PCSENABLE_REG))

/// PCS Enable Register
typedef union {
  struct {
    UINT32 CakePcsEnable:6;          ///< CAKE PCS Enable
    UINT32 :2;                       ///< Reserved
    UINT32 CcmPcsEnable:8;           ///< CCM PCS Enable
    UINT32 :16;                      ///< Reserved
  } Field;
  UINT32  Value;
} PCSENABLE_REGISTER;

/* Coherent Slave Configuration A1 Register */
#define CSCFGA1_FUNC 0x2
#define CSCFGA1_REG 0x044
#define CSCFGA1_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, CSCFGA1_FUNC, CSCFGA1_REG))

/// Coherent Slave Configuration A1 Register
typedef union {
  struct {                                ///< Bitfields of DRAM Contention Monitor Register
    UINT32 :14;                           ///< Reserved
    UINT32 DisAddrCollNcRdVc6:1;          ///< Disable address collision in VC6
    UINT32 :17;                           ///< Reserved
  } Field;
  UINT32 Value;
} CS_CFG_A1_REGISTER;

/* Coherent Slave Configuration A2 Register */
#define CSCFGA2_FUNC 0x2
#define CSCFGA2_REG 0x048
#define CSCFGA2_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, CSCFGA2_FUNC, CSCFGA2_REG))

/// Coherent Slave Configuration A2 Register
typedef union {
  struct {                                ///< Bitfields of DRAM Contention Monitor Register
    UINT32 :1;                            ///< Reserved
    UINT32 DisReqBypEA:1;                 ///< Disable request bypass EA
    UINT32 :2;                            ///< Reserved
    UINT32 RdRspBypSatLmt:4;              ///< Read Response Bypass Saturation Limit
    UINT32 :9;                            ///< Reserved
    UINT32 DisTgtSt:1;                    ///< Disable Target Starts
    UINT32 :5;                            ///< Reserved
    UINT32 EnTgtStUseRspNd:1;             ///< Enable Target Starts use response no data
    UINT32 :1;                            ///< Reserved
    UINT32 EnTgtDnUseRspNd:1;             ///< Enable TgtDn use response no data
    UINT32 FrcTgtDnUseRspNd:1;            ///< Force TgtDn use response no data
    UINT32 :4;                            ///< Reserved
    UINT32 DisTgtStRmt:1;                 ///< Disable Target Starts to remote dies
  } Field;
  UINT32 Value;
} CS_CFG_A2_REGISTER;

/* DRAM Contention Monitor Register */
#define DRAM_CONT_MON_FUNC 0x2
#define DRAM_CONT_MON_REG 0x068
#define DRAM_CONT_MON_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, DRAM_CONT_MON_FUNC, DRAM_CONT_MON_REG))

/// DRAM Contention Monitor Register
typedef union {
  struct {                                ///< Bitfields of DRAM Contention Monitor Register
    UINT32 DramContentionMonEn:1;         ///< DRAM contention monitor enable
    UINT32 DramContentionThresh:5;        ///< DRAM contention threshold
    UINT32 :2;                            ///< Reserved
    UINT32 UmcRdSampleWin:4;              ///< UMC read sample window
    UINT32 SpecDramRdSampleWin:4;         ///< Speculative DRAM read sample window
    UINT32 UmcRdThresh:8;                 ///< UMC read threshold
    UINT32 :8;                            ///< Reserved
  } Field;
  UINT32 Value;
} DRAM_CONT_MON_REGISTER;

/* Speculative DRAM Read Monitor Register */
#define SPEC_DRAM_RD_MON_FUNC 0x2
#define SPEC_DRAM_RD_MON_REG 0x06C
#define SPEC_DRAM_RD_MON_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, SPEC_DRAM_RD_MON_FUNC, SPEC_DRAM_RD_MON_REG))

/// Speculative DRAM Read Monitor Register
typedef union {
  struct {                                ///< Bitfields of Speculative DRAM Read Monitor Register
    UINT32 SpecDramRdMonEn:1;             ///< Speculative DRAM read monitor enable
    UINT32 SpecDramRdContentionThresh:5;  ///< Speculative DRAM read contention threshold
    UINT32 :2;                            ///< Reserved
    UINT32 SpecDramRdMissThresh:8;        ///< Speculative DRAM read miss threshold
    UINT32 UmcRdThreshLow:8;              ///< UMC read threshold low
    UINT32 UmcRdThreshHi:8;               ///< UMC read threshold high
  } Field;
  UINT32 Value;
} SPEC_DRAM_RD_MON_REGISTER;

/* IOM Control Register */
#define IOM_CTRL_FUNC 0x3
#define IOM_CTRL_REG 0x04C
#define IOM_CTRL_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, IOM_CTRL_FUNC, IOM_CTRL_REG))

/// IOM Control Register
typedef union {
  struct {                              ///< Bitfields of IOM Control Register
    UINT32 EnIomActIntSysLmt:1;         ///< Enable limit on total number of active IOM
    UINT32 IomActIntSysLmt:2;           ///< Total number of active or outstanding IOM
    UINT32 :1;                          ///< Reserved
    UINT32 FrcIomNoSdpRdRspRule:1;      ///< Disable SDP Read Response ordering
    UINT32 FrcIomSdpVc7ToFtiVc4:1;      ///< Force mapping
    UINT32 FrcIomSdpVc2To7BLZero:1;     ///< Force BlockLevel
    UINT32 FrcIomSdpVc2To7Ordered:1;    ///< Treat commands as if PassPW = 0
    UINT32 FrcIomRspPPWPassPWOne:1;     ///< Force RspPWW and PPW bit to 1
    UINT32 :23;                         ///< Reserved
  } Field;
  UINT32 Value;
} IOM_CTRL_REGISTER;

/* CCM Configuration Register */
#define CCM_CFG_FUNC 0x3
#define CCM_CFG_REG 0x104
#define CCM_CFG_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, CCM_CFG_FUNC, CCM_CFG_REG))

/// CCM Configuration Register
typedef union {
  struct {                                  ///< Bitfields of CCM Configuration Register
    UINT32 DisAllMemCancel:1;               ///< Disable MemCancel response.
    UINT32 DisRemoteMemCancel:1;            ///< Disable MemCancel to remote die.
    UINT32 DisPrbReqByp:1;                  ///< Disable Probe Request bypass path.
    UINT32 DisPrbRspByp:1;                  ///< Disable Probe Response bypass path.
    UINT32 DisCcmReqQByp:1;                 ///< Disable Request queue (REQQ) bypass path
    UINT32 DisCcmRspDQByp:1;                ///< Disable Read Response Bypass.
    UINT32 EnEarlyPrbRspDataReturn:1;       ///< Enable return of Probe Response dirty data to SDP without waiting for all system responses to be accumulated.
    UINT32 RspQRdRspWaterMark:6;            ///< Number of responses waiting to be scheduled to the data scheduler before the bypass path is disabled until the count falls below this threshold watermark.
    UINT32 FrcIoPassPW:1;                   ///< Enable PassPW bit always set.
    UINT32 FrcVc0RspPassPW:1;               ///< Enable Response PassPW always set
    UINT32 ForceCacheBlkRdZeroOrder:1;      ///< Force all cache block commands to fetch data in Zero word order.
    UINT32 DsPReqLmt:6;                     ///< Number of downstream Posted requests issued by CCM which may be outstanding on IO links.
    UINT32 DisSpecDramRead:1;               ///< Disable Speculative Dram Reads to Remote Die.
    UINT32 DisFastTPRWrHiPri:1;             ///< Disable high priority for Fast TPR writes
    UINT32 FrcMemPassPW:1;                  ///< Pass PW for sized requests to DRAM
    UINT32 ForceRdBlkLToC:1;                ///< Force RdBlkL to be issued as RdBlk to FTI.
    UINT32 DisMstAbtRsp:1;                  ///< Disable Master Abort Response
    UINT32 DisPciCfgMstAbtRsp:1;            ///< Disable PCI Config Master Abort Response
    UINT32 DisProtErrRsp:1;                 ///< Disable protocol violation on FTI Error Response
    UINT32 DisCcmRspDataByp:1;              ///< Disable Response Data Bypass
    UINT32 DisReTagSelection:1;             ///< ReTag selection algorithm
    UINT32 DisPrbAsTgtSt:1;                 ///< Probe as TgtStart
  } Field;
  UINT32 Value;
} CCM_CFG_REGISTER;

/* CCM Configuration 1 Register */
#define CCM_CFG1_FUNC 0x3
#define CCM_CFG1_REG 0x108
#define CCM_CFG1_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, CCM_CFG1_FUNC, CCM_CFG1_REG))

/// CCM Configuration 1 Register
typedef union {
  struct {                                  ///< Bitfields of CCM Configuration 1 Register
    UINT32 DisReqQLPOrLLPreEmptiveByp:1;    ///< Disable Pre-emptive Bypass Path
    UINT32 DisReqQPickOutOfOrder:1;         ///< Disable out-of-order request picking
    UINT32 DisRspQPickOutOfOrder:1;         ///< Disable out-of-order response picking
    UINT32 DisPrbQPickOutOfOrder:1;         ///< Disable out-of-order probe picking
    UINT32 ForceCohSzRdWr:1;                ///< Force non-coherent commands as coherent versions
    UINT32 :1;                              ///< Reserved
    UINT32 EnExtPciGt8BTgtAbt:1;            ///< Enable extended PCI accesses
    UINT32 DisReqQResAntiStrv:1;            ///< Disable anti-starvation widget
    UINT32 DisFid0OrderMatch:1;             ///< Treat all CS as separate targets
    UINT32 SPVQDeallocStallEn:1;            ///< SPVQ deallocation
    UINT32 PrbPickStallSdpEn:1;             ///< PRBQ probe request picker is stalled
    UINT32 PrbPickStallFtiEn:1;             ///< PRBQ probe response picker is stalled
    UINT32 DatPickStallSdpEn:1;             ///< ORIGDQ picker is stalled
    UINT32 DatPickStallFtiEn:1;             ///< RSPDQ picker is stalled
    UINT32 DatDeallocStallSdpEn:1;          ///< ORIGDQ deallocation is stalled
    UINT32 DatDeallocStallFtiEn:1;          ///< RSPDQ deallocation is stalled
    UINT32 DisCVQPickOutOfOrder:1;          ///< CVQ picker
    UINT32 DisSPVQPickOutOfOrder:1;         ///< SPVQ picker
    UINT32 DisFtiReqByp:1;                  ///< FTI req bypass
    UINT32 ForceRequestOrdering:1;          ///< Force TgtStart/TgtDone ordering
    UINT32 AlarmClockCntSel:3;              ///< CCM alarm clock count select
    UINT32 AlarmClockBaseSel:2;             ///< CCM alarm clock timebase select
    UINT32 DisFullZeroConv:1;               ///< Disable conversion
    UINT32 :2;                              ///< Reserved
    UINT32 DisFtiParErr:1;                  ///< Disables parity error checking for FTI
    UINT32 DisRspdParErr:1;                 ///< Disables parity error checking for RSPD
    UINT32 DisSdpParErr:1;                  ///< Disables parity error checking for SPD
    UINT32 DisOrigdParErr:1;                ///< Disables parity error checking for ORIGD
  } Field;
  UINT32 Value;
} CCM_CFG1_REGISTER;

/* CCX Enable Register */
#define CCXENABLE_FUNC 0x3
#define CCXENABLE_REG  0x12C
#define CCXENABLE_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, CCXENABLE_FUNC, CCXENABLE_REG))

/// CCX Enable Register
typedef union {
  struct {
    UINT32  CCXEn:2;                          ///< CCX Enable
    UINT32  :30;                              ///< Reserved
  } Field;
  UINT32   Value;
} CCX_ENABLE_REGISTER;

/* CAKE Response Compression Control Register */
#define CAKE_RSP_COMP_CTRL_FUNC 0x3
#define CAKE_RSP_COMP_CTRL_REG 0x280
#define CAKE_RSP_COMP_CTRL_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, CAKE_RSP_COMP_CTRL_FUNC, CAKE_RSP_COMP_CTRL_REG))

/* CAKE Request Compression Control Register */
#define CAKE_REQ_COMP_CTRL_FUNC 0x3
#define CAKE_REQ_COMP_CTRL_REG 0x284
#define CAKE_REQ_COMP_CTRL_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, CAKE_REQ_COMP_CTRL_FUNC, CAKE_REQ_COMP_CTRL_REG))

/* CAKE System Configuration Register */
#define CAKE_SYS_CFG_FUNC 0x3
#define CAKE_SYS_CFG_REG 0x288
#define CAKE_SYS_CFG_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, CAKE_SYS_CFG_FUNC, CAKE_SYS_CFG_REG))

/// CAKE System Configuration Register
typedef union {
  struct {                                  ///< Bitfields of CAKE System Configuration Register
    UINT32 ConnDieId:4;                     ///< Connected Die ID
    UINT32 ConnRemoteXgmiDieId:4;           ///< Connected Remote XGMI Die ID
    UINT32 ConnLocalXgmiDieId:4;            ///< Connected Local XGMI Die ID
    UINT32 GmiConnType:2;                   ///< GMI Connection Type
    UINT32 FtiToGmiDatBypEn:1;              ///< FTI to GMI Data Bypass Enable
    UINT32 GmiToFtiDatBypEn:1;              ///< GMI to FTI Data Bypass Enable
    UINT32 GmiPackFlushFull:1;              ///< GMI Pack Flush Full
    UINT32 CakeDisblTillRst:1;              ///< CAKE Disable Until Reset
    UINT32 CakeGmiSwapEn:1;                 ///< CAKE GMI swap
    UINT32 CakeGmi128:1;                    ///< CAKE GMI 128
    UINT32 NeibPriUpEn:1;                   ///< Neighbor Pri Up
    UINT32 NeibPriVal:2;                    ///< Neighbor Pri Valid
    UINT32 RemoteNeibPriUpEn:1;             ///< Remote Pri Up
    UINT32 RemotePriVal:2;                  ///< Remote Pri Valid
    UINT32 PassThruPriUpEn:1;               ///< Pass thru Pri Up
    UINT32 PassThruPriVal:2;                ///< Pass thru Pri Valid
    UINT32 CakePriClr:1;                    ///< CAKE Pri clear
    UINT32 :2;                              ///< Reserved
  } Field;
  UINT32 Value;
} CAKE_SYS_CFG_REGISTER;

/* IOM Configuration 0 Register */
#define IOM_CFG_FUNC 0x3
#define IOM_CFG_REG 0x300
#define IOM_CFG_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, IOM_CFG_FUNC, IOM_CFG_REG))

/// IOM Configuration 0 Register
typedef union {
  struct {                                ///< Bitfields of IOM Configuration 0 Register
    UINT32 :1;                            ///< Reserved
    UINT32 DisBusLock:1;                  ///< Disable blocking commands for BusLock
    UINT32 EnReqBusLock:1;                ///< Block all NP commands
    UINT32 DisDbgPickStallFtiRsp:1;       ///< Disable DebugStall pick for Source Done
    UINT32 DisDbgPickStallSdpWrRsp:1;     ///< Disable DebugStall pick for SDP write
    UINT32 DisDbgPickStallSdpRdRsp:1;     ///< Disable DebugStall pick for SDP Read Response
    UINT32 SrcDnCsWrSzAnyPPW:1;           ///< Generate SrcDone for SDP request
    UINT32 SrcDnCsWrSzAnyBL:1;            ///< Generate SrcDone regardless of BlockLevel
    UINT32 DisSrcDnOnVc2ToVc7:1;          ///< Disable SrcDone for SDP from VC2 to VC7
    UINT32 AlarmClockBaseSel:2;           ///< IOM ordering AlarmClock timebase select
    UINT32 AlarmClockCntSel:3;            ///< IOM ordering AlarmClock Count select
    UINT32 IomHeartBeatEn:1;              ///< Configure IOM heartbeat monitoring
    UINT32 LoopBackIosRsp:1;              ///< Loop IOS reponse indication
    UINT32 ForceCohSzRdWr:1;              ///< Force probes to be issued
    UINT32 DisIoChnWrOrd:1;               ///< Disable IOS destination requests order
    UINT32 FrcIosRspPriZero:1;            ///< Force IOS reposnse to be low priority
    UINT32 DisTgtStartShortCut:1;         ///< Disable Target Start shortcut
    UINT32 DisFrcThrottleDisOnLoadStep:1; ///< Disable Force Throttle
    UINT32 LatMonTgtSel:3;                ///< Select performance Latency Monitor Target
    UINT32 DisIosRspBypass:1;             ///< Disable IOS response bypass path
    UINT32 EnAutoClrRspWtPriorOp:1;       ///< Enable automatic clearing of response wait code
    UINT32 DevMsgIoChnWrOnly:1;           ///< Allow device message requests to set chain bit
    UINT32 DisTokRestoreFromRDL:1;        ///< Disable token restore
    UINT32 FrcIomOrdReqBL1Or0AsBL2:1;     ///< Force IOM order
    UINT32 DisMrq0BypCntUId:1;            ///< Disable Mrq0
    UINT32 DisMrq0BypCntPrt:1;            ///< Disable Mrq0
    UINT32 FrcIomOrdReqAsPPWZero:1;       ///< Force IOM order
  } Field;
  UINT32 Value;
} IOM_CFG_REGISTER;

/* Core Coherent Master Configuration Access Control */
#define CORE_MASTER_ACC_CTRL_FUNC 0x4
#define CORE_MASTER_ACC_CTRL_REG  0x44
#define CORE_MASTER_ACC_CTRL_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, CORE_MASTER_ACC_CTRL_FUNC, CORE_MASTER_ACC_CTRL_REG))

/// Core Coherent Master Configuration Access Control
typedef union {
  struct {                              ///< Bitfields of Core Coherent Master Configuration Access Control
    UINT32 EnableCf8ExtCfg:1;           ///< Enable PCI extended configuration register
    UINT32 DisPciCfgReg:1;              ///< Disable CFC/CF8 Accesses to IO space
    UINT32 :30;                         ///< Reserved
  } Field;
  UINT32 Value;
} CORE_MASTER_ACC_CTRL_REGISTER;

/* Fabric Indirect Config Access Address 3 Register */
#define FICAA3_FUNC 0x4
#define FICAA3_REG 0x05C
#define FICAA3_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FICAA3_FUNC, FICAA3_REG))

/// Fabric Indirect Config Access Address 3 Register
typedef union {
  struct {                           ///< Bitfields of Fabric Indirect Config Access Address 3 Register
    UINT32 CfgRegInstAccEn:1;        ///< CfgRegInstAccEn
    UINT32 :1;                       ///< Reserved
    UINT32 IndCfgAccRegNum:9;        ///< IndCfgAccRegNum
    UINT32 IndCfgAccFuncNum:3;       ///< IndCfgAccFuncNum
    UINT32 SixtyFourBitRegEn:1;      ///< SixtyFourBitRegEn
    UINT32 :1;                       ///< Reserved
    UINT32 CfgRegInstID:8;           ///< CfgRegInstID
    UINT32 :8;                       ///< Reserved
  } Field;                           ///<
  UINT32 Value;                      ///<
} FABRIC_IND_CFG_ACCESS_ADDR_REGISTER;

/* Fabric Indirect Config Access Data Low 3 Register */
#define FICAD3_LO_FUNC 0x4
#define FICAD3_LO_REG 0x98
#define FICAD3_LO_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FICAD3_LO_FUNC, FICAD3_LO_REG))

/* Fabric Indirect Config Access Data High 3 Register */
#define FICAD3_HI_FUNC 0x4
#define FICAD3_HI_REG 0x9C
#define FICAD3_HI_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, FICAD3_HI_FUNC, FICAD3_HI_REG))

/* Master Request Control Register */
#define MASTER_REQ_CTRL_FUNC 0x4
#define MASTER_REQ_CTRL_REG 0x0C0
#define MASTER_REQ_CTRL_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, MASTER_REQ_CTRL_FUNC, MASTER_REQ_CTRL_REG))

/// Master Request Control Register
typedef union {
  struct {                              ///< Bitfields of Master Request Control Register
    UINT32 :4;                          ///< Reserved
    UINT32 MstDramActReqLmt:8;          ///< Master to DRAM Active Request Limit
    UINT32 MstIoNpActReqLmt:6;          ///< Master to IO Non-Posted Active Request Limit
    UINT32 :8;                          ///< Reserved
    UINT32 MstIoReqThrottle:5;          ///< IO request throttle rate
    UINT32 :1;                          ///< Reserved
  } Field;
  UINT32 Value;
} MASTER_REQ_CTRL_REGISTER;

/* Master Request Priority Register */
#define MASTER_REQ_PRI_FUNC 0x4
#define MASTER_REQ_PRI_REG 0x0C4
#define MASTER_REQ_PRI_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, MASTER_REQ_PRI_FUNC, MASTER_REQ_PRI_REG))

/// Master Request Priority Register
typedef union {
  struct {                             ///< Bitfields of Master Request Priority Register
    UINT32 UseSdpReqPri:1;             ///< Use SDP request priority
    UINT32 :3;                         ///< Reserved
    UINT32 DefRdReqPri:4;              ///< Default read request priority
    UINT32 DefWrReqPri:4;              ///< Default write request priority
    UINT32 :4;                         ///< Reserved
    UINT32 LPRdReqPri:4;               ///< Lone potato read request priority
    UINT32 ElvPriRdBlkC:4;             ///< Elevated RdBlkC request priority
    UINT32 ElvPriRdBlkX:4;             ///< Elevated RdBlkX request priority
    UINT32 :4;                         ///< Reserved
  } Field;
  UINT32 Value;
} MASTER_REQ_PRI_REGISTER;

/* Master Light Load And Lone Potato Request Priority Threshold Register */
#define MASTER_LL_LP_REQ_PRI_THRESH_FUNC 0x4
#define MASTER_LL_LP_REQ_PRI_THRESH_REG 0x0C8
#define MASTER_LL_LP_REQ_PRI_THRESH_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, MASTER_LL_LP_REQ_PRI_THRESH_FUNC, MASTER_LL_LP_REQ_PRI_THRESH_REG))

/// Master Light Load And Lone Potato Request Priority Threshold Register
typedef union {
  struct {                             ///< Bitfields of Master Light Load And Lone Potato Request Priority Threshold Register
    UINT32 ElvPriRdBlkXThr:5;          ///< Elevated RdBlkX command watermark
    UINT32 :2;                         ///< Reserved
    UINT32 FrcElvPriRdBlkX:1;          ///< Force RdBlkX priority
    UINT32 ElvPriRdBlkCThr:5;          ///< Elevated RdBlkC command watermark
    UINT32 :2;                         ///< Reserved
    UINT32 FrcElvPriRdBlkC:1;          ///< Force RdBlkC priority
    UINT32 LPElvPriReqThr:3;           ///< Lone potato elevated priority request threshold
    UINT32 :1;                         ///< Reserved
    UINT32 LPCumElvPriReqThr:5;        ///< Lone potato cumulative elevated priority request threshold
    UINT32 LPPreEmptiveBypSatThr:3;    ///< Lone potato preemptive bypass saturation threshold
    UINT32 :4;                         ///< Reserved
  } Field;
  UINT32 Value;
} MASTER_LL_LP_REQ_PRI_THRESH_REGISTER;

/* Cstate Control Register */
#define CSTATE_CTRL_FUNC 0x5
#define CSTATE_CTRL_REG 0x300
#define CSTATE_CTRL_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, CSTATE_CTRL_FUNC, CSTATE_CTRL_REG))

/// Cstate Control Register
typedef union {
  struct {                             ///< Bitfields of Cstate Control Register
    UINT32 DfInCstate:1;               ///< DF in CState
    UINT32 AllClientsIdle:1;           ///< All clients idle
    UINT32 AnyGmiDown:1;               ///< Any GMI down
    UINT32 DfRegsNotRestored:1;        ///< DF registers not restored
    UINT32 PickLoadStepBusy:1;         ///< Pick load step busy
    UINT32 DfCstateDisable:1;          ///< DF CState disable
    UINT32 DfCstateEntryDisable:1;     ///< DF CState entry disable
    UINT32 DfCstateAsyncWakeDisable:1; ///< DF CState asyncrhonous wake disable
    UINT32 DfCstateMmStutterOptEn:1;   ///< DF CState MM stutter
    UINT32 DfCstateMmPwrGateOptEn:1;   ///< DF CState MM power gate
    UINT32 DfCstateDceElptModeEn:1;    ///< DF CState DCE eLPT mode
    UINT32 DfCstatePwrGateEn:1;        ///< DF CState power gate
    UINT32 DfCstateClkPwrDnEn:1;       ///< DF CState clock power down
    UINT32 DfCstateSelfRefrEn:1;       ///< DF CState self refresh
    UINT32 DfCstateGmiPwrDnEn:1;       ///< DF CState GMI power down
    UINT32 DfCstateXgmiPwrDnEn:1;      ///< DF CState xGMI power down
    UINT32 PerDieDfCstateEn:1;         ///< Per die DF CState
    UINT32 :15;                        ///< Reserved
  } Field;
  UINT32 Value;
} CSTATE_CTRL_REGISTER;

/* Pstate S3/D3 Control Register */
#define PSTATE_S3D3_CTRL_FUNC 0x5
#define PSTATE_S3D3_CTRL_REG 0x304
#define PSTATE_S3D3_CTRL_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, PSTATE_S3D3_CTRL_FUNC, PSTATE_S3D3_CTRL_REG))

/// Pstate S3/D3 Control Register
typedef union {
  struct {                                ///< Bitfields of Pstate S3/D3 Control Register
    UINT32 DfPstateChgBusy:1;             ///< DF Pstate change busy
    UINT32 DfS3EntryBusy:1;               ///< DF S3 entry busy
    UINT32 DfPstateChgReq:1;              ///< DF Pstate change request
    UINT32 ExpectDfPstateChg:1;           ///< Expect DF Pstate change
    UINT32 DfS3EntryReq:1;                ///< DF S3 entry request
    UINT32 NvdimmSelfRefrEn:1;            ///< NV DIMM self refresh enable
    UINT32 DfPstateChgDisableClkChg:1;    ///< DF Pstate change disable clock change
    UINT32 DfPstateChgDisableQuiesce:1;   ///< DF Pstate change disable quiesce
    UINT32 DfPstateChgDisableMstateChg:1; ///< DF Pstate change disable Mstate change
    UINT32 DfPstateChgDisableGmiChg:1;    ///< DF Pstate change disable GMI change
    UINT32 DfPstateChgDisableXgmiChg:1;   ///< DF Pstate change disable xGMI change
    UINT32 DfSaveStateStart:1;            ///< DF save state start
    UINT32 DfSaveStateDone:1;             ///< DF save state done
    UINT32 DfRestoreStateStart:1;         ///< DF restore state start
    UINT32 DfRestoreStateDone:1;          ///< DF restore state done
    UINT32 RecalibrateE12gPhyPllOnDfPstateChg:1; ///< Recal E12g phy PLL on DF Pstate change
    UINT32 :16;                           ///< Reserved
  } Field;
  UINT32 Value;
} PSTATE_S3D3_CTRL_REGISTER;

/* Multi-Die Hysteresis Control Register */
#define MULTIDIE_HYST_CTRL_FUNC 0x5
#define MULTIDIE_HYST_CTRL_REG 0x354
#define MULTIDIE_HYST_CTRL_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, MULTIDIE_HYST_CTRL_FUNC, MULTIDIE_HYST_CTRL_REG))

/// Multi-Die Hysteresis Control Register
typedef union {
  struct {                             ///< Bitfields of Multi-Die Hysteresis Control Register
    UINT32 GmiDisconHyst:8;            ///< GMI disconnect hysteresis
    UINT32 XgmiDisconHyst:8;           ///< xGMI disconnect hysteresis
    UINT32 PerDieDfCstateHyst:8;       ///< Per die DF Cstate hysteresis
    UINT32 PerDieDfCstateHystScale:5;  ///< Per die DF Cstate hysteresis scale
    UINT32 :3;                         ///< Reserved
  } Field;
  UINT32 Value;
} MULTIDIE_HYST_CTRL_REGISTER;

/* Load Step Hysteresis Control Register */
#define LOAD_STEP_HYST_CTRL_FUNC 0x5
#define LOAD_STEP_HYST_CTRL_REG 0x358
#define LOAD_STEP_HYST_CTRL_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, LOAD_STEP_HYST_CTRL_FUNC, LOAD_STEP_HYST_CTRL_REG))

/// Load Step Hysteresis Control Register
typedef union {
  struct {                             ///< Bitfields of Load Step Hysteresis Control Register
    UINT32 ClkLoadStepHyst:6;          ///< Clock load step hysteresis
    UINT32 ClkLoadStepHystScale:2;     ///< Clock load step hysteresis scale
    UINT32 GmiLoadStepHyst:6;          ///< GMI load step hysteresis
    UINT32 GmiLoadStepHystScale:2;     ///< GMI load step hysteresis scale
    UINT32 PickLoadStepHyst:6;         ///< Pick load step hysteresis
    UINT32 PickLoadStepHystScale:2;    ///< Pick load step hysteresis scale
    UINT32 PickLoadStepThrottle0:4;    ///< Pick load step throttle 0
    UINT32 PickLoadStepThrottle1:4;    ///< Pick load step throttle 1
  } Field;
  UINT32 Value;
} LOAD_STEP_HYST_CTRL_REGISTER;

/* DRAM Scrubber Base Address Register */
#define DRAMSCRUBBASEADDR_FUNC 0x6
#define DRAMSCRUBBASEADDR_REG  0x040
#define DRAMSCRUBBASEADDR_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, DRAMSCRUBBASEADDR_FUNC, DRAMSCRUBBASEADDR_REG)

/// DRAM Scrubber Base Address Register
typedef union {
  struct {                             ///< Bitfields of DRAM Scrubber Base Address Register
    UINT32 DramScrubEn:1;              ///< DRAM Scrub Enable
    UINT32 DramScrubValid:1;           ///< DRAM Scrub Valid
    UINT32 :2;                         ///< Reserved
    UINT32 DramScrubBaseAddr:28;       ///< DRAM Scrub Base Address
  } Fields;
  UINT32  Value;
} DRAM_SCRUB_BASE_ADDR_REGISTER;

/* DRAM Scrubber Limit Address Register */
#define DRAMSCRUBLIMITADDR_FUNC 0x6
#define DRAMSCRUBLIMITADDR_REG  0x044
#define DRAMSCRUBLIMITADDR_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, DRAMSCRUBLIMITADDR_FUNC, DRAMSCRUBLIMITADDR_REG)

/// DRAM Scrubber Limit Address Register
typedef union {
  struct {                             ///< Bitfields of DRAM Scrubber Limit Address Register
    UINT32 DramScrubRate:4;            ///< DRAM Scrub Rate
    UINT32 DramScrubLimitAddr:28;      ///< DRAM Scrub Limit Address
  } Fields;
  UINT32  Value;
} DRAM_SCRUB_LIMIT_ADDR_REGISTER;

/* DRAM Scrubber Address Low Register */
#define DRAMSCRUBADDRLO_FUNC 0x6
#define DRAMSCRUBADDRLO_REG  0x048
#define DRAMSCRUBADDRLO_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, DRAMSCRUBADDRLO_FUNC, DRAMSCRUBADDRLO_REG)

/// DRAM Scrubber Address Low Register
typedef union {
  struct {                             ///< Bitfields of DRAM Scrubber Address Low Register
    UINT32 MemInitEnable:1;            ///< MemInitEnable
    UINT32 MemInitDone:1;              ///< MemInitDone
    UINT32 DisMemInit:1;               ///< DisMemInit
    UINT32 :3;                         ///< Reserved
    UINT32 SeqScrubAddrLo:26;          ///< SeqScrubAddrLo
  } Fields;
  UINT32  Value;
} DRAM_SCRUB_ADDR_LO_REGISTER;

/* DRAM Scrubber Address High Register */
#define DRAMSCRUBADDRHI_FUNC 0x6
#define DRAMSCRUBADDRHI_REG  0x04C
#define DRAMSCRUBADDRHI_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, DRAMSCRUBADDRHI_FUNC, DRAMSCRUBADDRHI_REG)

/// DRAM Scrubber Address High Register
typedef union {
  struct {                             ///< Bitfields of DRAM Scrubber Address High Register
    UINT32 SeqScrubAddrHi:16;          ///< SeqScrubAddrHi
    UINT32 :8;                         ///< Reserved
    UINT32 SeqScrubMode:2;             ///< SeqScrubMode
    UINT32 :2;                         ///< Reserved
    UINT32 NumSeqScrubsCstateExit:4;   ///< Number of sequential scrubs to issue on Cstate exit
  } Fields;
  UINT32  Value;
} DRAM_SCRUB_ADDR_HI_REGISTER;

/* DRAM Scrubber Base Address 1 Register */
#define DRAMSCRUBBASEADDR1_FUNC 0x6
#define DRAMSCRUBBASEADDR1_REG  0x050
#define DRAMSCRUBBASEADDR1_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, DRAMSCRUBBASEADDR1_FUNC, DRAMSCRUBBASEADDR1_REG)

/// DRAM Scrubber Base Address Register 1
typedef union {
  struct {                             ///< Bitfields of DRAM Scrubber Base Address Register
    UINT32 :1;                         ///< Reserved
    UINT32 DramScrubValid:1;           ///< DRAM Scrub Valid
    UINT32 :2;                         ///< Reserved
    UINT32 DramScrubBaseAddr:28;       ///< DRAM Scrub Base Address
  } Fields;
  UINT32  Value;
} DRAM_SCRUB_BASE_ADDR1_REGISTER;

/* DRAM Scrubber Limit Address 1 Register */
#define DRAMSCRUBLIMITADDR1_FUNC 0x6
#define DRAMSCRUBLIMITADDR1_REG  0x054
#define DRAMSCRUBLIMITADDR1_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, DRAMSCRUBLIMITADDR1_FUNC, DRAMSCRUBLIMITADDR1_REG)

/// DRAM Scrubber Limit Address 1 Register
typedef union {
  struct {                             ///< Bitfields of DRAM Scrubber Limit Address Register
    UINT32 :4;                         ///< Reserved
    UINT32 DramScrubLimitAddr:28;      ///< DRAM Scrub Limit Address
  } Fields;
  UINT32  Value;
} DRAM_SCRUB_LIMIT_ADDR1_REGISTER;

/* DRAM Scrubber Address Low 1 Register */
#define DRAMSCRUBADDRLO1_FUNC 0x6
#define DRAMSCRUBADDRLO1_REG  0x058
#define DRAMSCRUBADDRLO1_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, DRAMSCRUBADDRLO1_FUNC, DRAMSCRUBADDRLO1_REG)

/// DRAM Scrubber Address Low 1 Register
typedef union {
  struct {                             ///< Bitfields of DRAM Scrubber Address Low Register
    UINT32 :6;                         ///< Reserved
    UINT32 SeqScrubAddrLo:26;          ///< SeqScrubAddrLo
  } Fields;
  UINT32  Value;
} DRAM_SCRUB_ADDR_LO1_REGISTER;

/* DRAM Scrubber Address High 1 Register */
#define DRAMSCRUBADDRHI1_FUNC 0x6
#define DRAMSCRUBADDRHI1_REG  0x05C
#define DRAMSCRUBADDRHI1_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, DRAMSCRUBADDRHI1_FUNC, DRAMSCRUBADDRHI1_REG)

/// DRAM Scrubber Address High 1 Register
typedef union {
  struct {                             ///< Bitfields of DRAM Scrubber Address High Register
    UINT32 SeqScrubAddrHi:16;          ///< SeqScrubAddrHi
    UINT32 :16;                        ///< Reserved
  } Fields;
  UINT32  Value;
} DRAM_SCRUB_ADDR_HI1_REGISTER;

/* DRAM Scrubber Base Address 2 Register */
#define DRAMSCRUBBASEADDR2_FUNC 0x6
#define DRAMSCRUBBASEADDR2_REG  0x060
#define DRAMSCRUBBASEADDR2_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, DRAMSCRUBBASEADDR2_FUNC, DRAMSCRUBBASEADDR2_REG)

/// DRAM Scrubber Base Address Register 2
typedef union {
  struct {                             ///< Bitfields of DRAM Scrubber Base Address Register
    UINT32 :1;                         ///< Reserved
    UINT32 DramScrubValid:1;           ///< DRAM Scrub Valid
    UINT32 :2;                         ///< Reserved
    UINT32 DramScrubBaseAddr:28;       ///< DRAM Scrub Base Address
  } Fields;
  UINT32  Value;
} DRAM_SCRUB_BASE_ADDR2_REGISTER;

/* DRAM Scrubber Limit Address 2 Register */
#define DRAMSCRUBLIMITADDR2_FUNC 0x6
#define DRAMSCRUBLIMITADDR2_REG  0x064
#define DRAMSCRUBLIMITADDR2_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, DRAMSCRUBLIMITADDR2_FUNC, DRAMSCRUBLIMITADDR2_REG)

/// DRAM Scrubber Limit Address 2 Register
typedef union {
  struct {                             ///< Bitfields of DRAM Scrubber Limit Address Register
    UINT32 :4;                         ///< Reserved
    UINT32 DramScrubLimitAddr:28;      ///< DRAM Scrub Limit Address
  } Fields;
  UINT32  Value;
} DRAM_SCRUB_LIMIT_ADDR2_REGISTER;

/* DRAM Scrubber Address Low 2 Register */
#define DRAMSCRUBADDRLO2_FUNC 0x6
#define DRAMSCRUBADDRLO2_REG  0x068
#define DRAMSCRUBADDRLO2_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, DRAMSCRUBADDRLO2_FUNC, DRAMSCRUBADDRLO2_REG)

/// DRAM Scrubber Address Low 2 Register
typedef union {
  struct {                             ///< Bitfields of DRAM Scrubber Address Low Register
    UINT32 :6;                         ///< Reserved
    UINT32 SeqScrubAddrLo:26;          ///< SeqScrubAddrLo
  } Fields;
  UINT32  Value;
} DRAM_SCRUB_ADDR_LO2_REGISTER;

/* DRAM Scrubber Address High 2 Register */
#define DRAMSCRUBADDRHI2_FUNC 0x6
#define DRAMSCRUBADDRHI2_REG  0x06C
#define DRAMSCRUBADDRHI2_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, DRAMSCRUBADDRHI2_FUNC, DRAMSCRUBADDRHI2_REG)

/// DRAM Scrubber Address High 2 Register
typedef union {
  struct {                             ///< Bitfields of DRAM Scrubber Address High Register
    UINT32 SeqScrubAddrHi:16;          ///< SeqScrubAddrHi
    UINT32 :16;                        ///< Reserved
  } Fields;
  UINT32  Value;
} DRAM_SCRUB_ADDR_HI2_REGISTER;

/* DRAM Scrubber Base Address 3 Register */
#define DRAMSCRUBBASEADDR3_FUNC 0x6
#define DRAMSCRUBBASEADDR3_REG  0x070
#define DRAMSCRUBBASEADDR3_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, DRAMSCRUBBASEADDR3_FUNC, DRAMSCRUBBASEADDR3_REG)

/// DRAM Scrubber Base Address Register 3
typedef union {
  struct {                             ///< Bitfields of DRAM Scrubber Base Address Register
    UINT32 :1;                         ///< Reserved
    UINT32 DramScrubValid:1;           ///< DRAM Scrub Valid
    UINT32 :2;                         ///< Reserved
    UINT32 DramScrubBaseAddr:28;       ///< DRAM Scrub Base Address
  } Fields;
  UINT32  Value;
} DRAM_SCRUB_BASE_ADDR3_REGISTER;

/* DRAM Scrubber Limit Address 3 Register */
#define DRAMSCRUBLIMITADDR3_FUNC 0x6
#define DRAMSCRUBLIMITADDR3_REG  0x074
#define DRAMSCRUBLIMITADDR3_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, DRAMSCRUBLIMITADDR3_FUNC, DRAMSCRUBLIMITADDR3_REG)

/// DRAM Scrubber Limit Address 3 Register
typedef union {
  struct {                             ///< Bitfields of DRAM Scrubber Limit Address Register
    UINT32 :4;                         ///< Reserved
    UINT32 DramScrubLimitAddr:28;      ///< DRAM Scrub Limit Address
  } Fields;
  UINT32  Value;
} DRAM_SCRUB_LIMIT_ADDR3_REGISTER;

/* DRAM Scrubber Address Low 3 Register */
#define DRAMSCRUBADDRLO3_FUNC 0x6
#define DRAMSCRUBADDRLO3_REG  0x078
#define DRAMSCRUBADDRLO3_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, DRAMSCRUBADDRLO3_FUNC, DRAMSCRUBADDRLO3_REG)

/// DRAM Scrubber Address Low 3 Register
typedef union {
  struct {                             ///< Bitfields of DRAM Scrubber Address Low Register
    UINT32 :6;                         ///< Reserved
    UINT32 SeqScrubAddrLo:26;          ///< SeqScrubAddrLo
  } Fields;
  UINT32  Value;
} DRAM_SCRUB_ADDR_LO3_REGISTER;

/* DRAM Scrubber Address High 3 Register */
#define DRAMSCRUBADDRHI3_FUNC 0x6
#define DRAMSCRUBADDRHI3_REG  0x07C
#define DRAMSCRUBADDRHI3_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, DRAMSCRUBADDRHI1_FUNC, DRAMSCRUBADDRHI1_REG)

/// DRAM Scrubber Address High 3 Register
typedef union {
  struct {                             ///< Bitfields of DRAM Scrubber Address High Register
    UINT32 SeqScrubAddrHi:16;          ///< SeqScrubAddrHi
    UINT32 :16;                        ///< Reserved
  } Fields;
  UINT32  Value;
} DRAM_SCRUB_ADDR_HI3_REGISTER;

/* Redirect Scrubber Control Register */
#define REDIRECTSCRUBCTRL_FUNC 0x6
#define REDIRECTSCRUBCTRL_REG  0x0B8
#define REDIRECTSCRUBCTRL_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, REDIRECTSCRUBCTRL_FUNC, REDIRECTSCRUBCTRL_REG)

/// DRAM Scrubber Address High Register
typedef union {
  struct {                             ///< Bitfields of DRAM Scrubber Address High Register
    UINT32 RedirScrubMode:2;           ///< Redirect scrub mode
    UINT32 RedirScrubReqLmt:2;         ///< Redirect Scrub limits
    UINT32 :28;                        ///< Reserved
  } Fields;
  UINT32  Value;
} REDIRECT_SCRUB_CTRL_REGISTER;

/* Trace Capture Buffer Control Register */
#define TCBCNTL_FUNC 0x6
#define TCBCNTL_REG  0x0DC
#define TCBCNTL_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, TCBCNTL_FUNC, TCBCNTL_REG)

/// Trace Capture Buffer Control Register
typedef union {
  struct {                             ///< Bitfields of Trace Capture Buffer Control Register
    UINT32 TcbEn:1;                    ///< Enable Trace Capture Buffer.
    UINT32 TraceToDramEn:1;            ///< Enable Trace Capture Buffer Trace to DRAM mode.
    UINT32 AccTscMode:1;               ///< Enable Accurate TSC Mode.
    UINT32 TraceInbDat:1;              ///< This field determines which data channel is presented to Trace Capture Buffer for FrontEndD.
    UINT32 :1;                         ///< Reserved
    UINT32 TraceReqChanFullUnitId:1;   ///< This field determines whether to trace full UnitId or full Attr field for Request channel.
    UINT32 TraceDramAddrMapSel:4;      ///< This field points to the AddrMap instance which is used to map the Trace Dram Address.
    UINT32 TscOverflowPktEn:1;         ///< TSC overflow info packet
    UINT32 TscOverflowChanSel:2;       ///< This field defines the Trace Capture Buffer Channel which is fed a TSC overflow in the event of an overflow.
    UINT32 TraceWbVcEn:1;              ///< WriteBack VC trace control.
    UINT32 TraceQWSel:2;               ///< Specify which part of the Trace Capture Buffer data value is stored with the data header.
    UINT32 :2;                         ///< Reserved
    UINT32 TraceQosPri:2;              ///< This field determines the priority with which the trace writes are issued over FTI.
    UINT32 TscAltReset:1;              ///< TSC alternate reset value
    UINT32 TscCreditlessCmds:1;        ///< TSC creditless commands
    UINT32 TraceUpperAddrBits:1;       ///< Trace upper address bits
    UINT32 TraceDatByteEn:1;           ///< Trace data byte enable
    UINT32 :7;                         ///< Reserved
    UINT32 TcbCntlLock:1;              ///< Trace Capture Buffer Control Register write lock.
  } Field;
  UINT32  Value;
} TCB_CNTL_REGISTER;

/* Hardware Assert Status Low Register */
#define HWA_STS_LOW_FUNC 0x6
#define HWA_STS_LOW_REG 0x1F0
#define HWA_STS_LOW_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, HWA_STS_LOW_FUNC, HWA_STS_LOW_REG))

/// Hardware Assert Status Low Register
typedef union {
  struct {                             ///< Bitfields of Hardware Assert Status Low Register
    UINT32 HWAssert0:1;                ///< Hardware Assert 0
    UINT32 HWAssert1:1;                ///< Hardware Assert 1
    UINT32 HWAssert2:1;                ///< Hardware Assert 2
    UINT32 HWAssert3:1;                ///< Hardware Assert 3
    UINT32 HWAssert4:1;                ///< Hardware Assert 4
    UINT32 HWAssert5:1;                ///< Hardware Assert 5
    UINT32 HWAssert6:1;                ///< Hardware Assert 6
    UINT32 HWAssert7:1;                ///< Hardware Assert 7
    UINT32 HWAssert8:1;                ///< Hardware Assert 8
    UINT32 HWAssert9:1;                ///< Hardware Assert 9
    UINT32 HWAssert10:1;               ///< Hardware Assert 10
    UINT32 HWAssert11:1;               ///< Hardware Assert 11
    UINT32 HWAssert12:1;               ///< Hardware Assert 12
    UINT32 HWAssert13:1;               ///< Hardware Assert 13
    UINT32 HWAssert14:1;               ///< Hardware Assert 14
    UINT32 HWAssert15:1;               ///< Hardware Assert 15
    UINT32 HWAssert16:1;               ///< Hardware Assert 16
    UINT32 HWAssert17:1;               ///< Hardware Assert 17
    UINT32 HWAssert18:1;               ///< Hardware Assert 18
    UINT32 HWAssert19:1;               ///< Hardware Assert 19
    UINT32 HWAssert20:1;               ///< Hardware Assert 20
    UINT32 HWAssert21:1;               ///< Hardware Assert 21
    UINT32 HWAssert22:1;               ///< Hardware Assert 22
    UINT32 HWAssert23:1;               ///< Hardware Assert 23
    UINT32 HWAssert24:1;               ///< Hardware Assert 24
    UINT32 HWAssert25:1;               ///< Hardware Assert 25
    UINT32 HWAssert26:1;               ///< Hardware Assert 26
    UINT32 HWAssert27:1;               ///< Hardware Assert 27
    UINT32 HWAssert28:1;               ///< Hardware Assert 28
    UINT32 HWAssert29:1;               ///< Hardware Assert 29
    UINT32 HWAssert30:1;               ///< Hardware Assert 30
    UINT32 HWAssert31:1;               ///< Hardware Assert 31
  } Field;
  UINT32 Value;
} HWA_STS_LOW_REGISTER;

/* Hardware Assert Status High Register */
#define HWA_STS_HI_FUNC 0x6
#define HWA_STS_HI_REG 0x1F4
#define HWA_STS_HI_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, HWA_STS_HI_FUNC, HWA_STS_HI_REG))

/// Hardware Assert Status High Register
typedef union {
  struct {                             ///< Bitfields of Hardware Assert Status High Register
    UINT32 HWAssert0:1;                ///< Hardware Assert 0
    UINT32 HWAssert1:1;                ///< Hardware Assert 1
    UINT32 HWAssert2:1;                ///< Hardware Assert 2
    UINT32 HWAssert3:1;                ///< Hardware Assert 3
    UINT32 HWAssert4:1;                ///< Hardware Assert 4
    UINT32 HWAssert5:1;                ///< Hardware Assert 5
    UINT32 HWAssert6:1;                ///< Hardware Assert 6
    UINT32 HWAssert7:1;                ///< Hardware Assert 7
    UINT32 HWAssert8:1;                ///< Hardware Assert 8
    UINT32 HWAssert9:1;                ///< Hardware Assert 9
    UINT32 HWAssert10:1;               ///< Hardware Assert 10
    UINT32 HWAssert11:1;               ///< Hardware Assert 11
    UINT32 HWAssert12:1;               ///< Hardware Assert 12
    UINT32 HWAssert13:1;               ///< Hardware Assert 13
    UINT32 HWAssert14:1;               ///< Hardware Assert 14
    UINT32 HWAssert15:1;               ///< Hardware Assert 15
    UINT32 HWAssert16:1;               ///< Hardware Assert 16
    UINT32 HWAssert17:1;               ///< Hardware Assert 17
    UINT32 HWAssert18:1;               ///< Hardware Assert 18
    UINT32 HWAssert19:1;               ///< Hardware Assert 19
    UINT32 HWAssert20:1;               ///< Hardware Assert 20
    UINT32 HWAssert21:1;               ///< Hardware Assert 21
    UINT32 HWAssert22:1;               ///< Hardware Assert 22
    UINT32 HWAssert23:1;               ///< Hardware Assert 23
    UINT32 HWAssert24:1;               ///< Hardware Assert 24
    UINT32 HWAssert25:1;               ///< Hardware Assert 25
    UINT32 HWAssert26:1;               ///< Hardware Assert 26
    UINT32 HWAssert27:1;               ///< Hardware Assert 27
    UINT32 HWAssert28:1;               ///< Hardware Assert 28
    UINT32 HWAssert29:1;               ///< Hardware Assert 29
    UINT32 HWAssert30:1;               ///< Hardware Assert 30
    UINT32 HWAssert31:1;               ///< Hardware Assert 31
  } Field;
  UINT32 Value;
} HWA_STS_HI_REGISTER;

/* Hardware Assert Status Low Register */
#define HWA_MASK_LOW_FUNC 0x6
#define HWA_MASK_LOW_REG 0x1F8
#define HWA_MASK_LOW_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, HWA_MASK_LOW_FUNC, HWA_MASK_LOW_REG))

/// Hardware Assert Mask Low Register
typedef union {
  struct {                             ///< Bitfields of Hardware Assert Mask Low Register
    UINT32 HWAssertMsk0:1;             ///< Hardware Assert Mask 0
    UINT32 HWAssertMsk1:1;             ///< Hardware Assert Mask 1
    UINT32 HWAssertMsk2:1;             ///< Hardware Assert Mask 2
    UINT32 HWAssertMsk3:1;             ///< Hardware Assert Mask 3
    UINT32 HWAssertMsk4:1;             ///< Hardware Assert Mask 4
    UINT32 HWAssertMsk5:1;             ///< Hardware Assert Mask 5
    UINT32 HWAssertMsk6:1;             ///< Hardware Assert Mask 6
    UINT32 HWAssertMsk7:1;             ///< Hardware Assert Mask 7
    UINT32 HWAssertMsk8:1;             ///< Hardware Assert Mask 8
    UINT32 HWAssertMsk9:1;             ///< Hardware Assert Mask 9
    UINT32 HWAssertMsk10:1;            ///< Hardware Assert Mask 10
    UINT32 HWAssertMsk11:1;            ///< Hardware Assert Mask 11
    UINT32 HWAssertMsk12:1;            ///< Hardware Assert Mask 12
    UINT32 HWAssertMsk13:1;            ///< Hardware Assert Mask 13
    UINT32 HWAssertMsk14:1;            ///< Hardware Assert Mask 14
    UINT32 HWAssertMsk15:1;            ///< Hardware Assert Mask 15
    UINT32 HWAssertMsk16:1;            ///< Hardware Assert Mask 16
    UINT32 HWAssertMsk17:1;            ///< Hardware Assert Mask 17
    UINT32 HWAssertMsk18:1;            ///< Hardware Assert Mask 18
    UINT32 HWAssertMsk19:1;            ///< Hardware Assert Mask 19
    UINT32 HWAssertMsk20:1;            ///< Hardware Assert Mask 20
    UINT32 HWAssertMsk21:1;            ///< Hardware Assert Mask 21
    UINT32 HWAssertMsk22:1;            ///< Hardware Assert Mask 22
    UINT32 HWAssertMsk23:1;            ///< Hardware Assert Mask 23
    UINT32 HWAssertMsk24:1;            ///< Hardware Assert Mask 24
    UINT32 HWAssertMsk25:1;            ///< Hardware Assert Mask 25
    UINT32 HWAssertMsk26:1;            ///< Hardware Assert Mask 26
    UINT32 HWAssertMsk27:1;            ///< Hardware Assert Mask 27
    UINT32 HWAssertMsk28:1;            ///< Hardware Assert Mask 28
    UINT32 HWAssertMsk29:1;            ///< Hardware Assert Mask 29
    UINT32 HWAssertMsk30:1;            ///< Hardware Assert Mask 30
    UINT32 HWAssertMsk31:1;            ///< Hardware Assert Mask 31
  } Field;
  UINT32 Value;
} HWA_MASK_LOW_REGISTER;

/* Hardware Assert Mask High Register */
#define HWA_MASK_HI_FUNC 0x6
#define HWA_MASK_HI_REG 0x1FC
#define HWA_MASK_HI_PCI_ADDR (MAKE_SBDFO (0, 0, 0x18, HWA_MASK_HI_FUNC, HWA_MASK_HI_REG))

/// Hardware Assert Mask High Register
typedef union {
  struct {                             ///< Bitfields of Hardware Assert Mask High Register
    UINT32 HWAssertMsk0:1;             ///< Hardware Assert Mask 0
    UINT32 HWAssertMsk1:1;             ///< Hardware Assert Mask 1
    UINT32 HWAssertMsk2:1;             ///< Hardware Assert Mask 2
    UINT32 HWAssertMsk3:1;             ///< Hardware Assert Mask 3
    UINT32 HWAssertMsk4:1;             ///< Hardware Assert Mask 4
    UINT32 HWAssertMsk5:1;             ///< Hardware Assert Mask 5
    UINT32 HWAssertMsk6:1;             ///< Hardware Assert Mask 6
    UINT32 HWAssertMsk7:1;             ///< Hardware Assert Mask 7
    UINT32 HWAssertMsk8:1;             ///< Hardware Assert Mask 8
    UINT32 HWAssertMsk9:1;             ///< Hardware Assert Mask 9
    UINT32 HWAssertMsk10:1;            ///< Hardware Assert Mask 10
    UINT32 HWAssertMsk11:1;            ///< Hardware Assert Mask 11
    UINT32 HWAssertMsk12:1;            ///< Hardware Assert Mask 12
    UINT32 HWAssertMsk13:1;            ///< Hardware Assert Mask 13
    UINT32 HWAssertMsk14:1;            ///< Hardware Assert Mask 14
    UINT32 HWAssertMsk15:1;            ///< Hardware Assert Mask 15
    UINT32 HWAssertMsk16:1;            ///< Hardware Assert Mask 16
    UINT32 HWAssertMsk17:1;            ///< Hardware Assert Mask 17
    UINT32 HWAssertMsk18:1;            ///< Hardware Assert Mask 18
    UINT32 HWAssertMsk19:1;            ///< Hardware Assert Mask 19
    UINT32 HWAssertMsk20:1;            ///< Hardware Assert Mask 20
    UINT32 HWAssertMsk21:1;            ///< Hardware Assert Mask 21
    UINT32 HWAssertMsk22:1;            ///< Hardware Assert Mask 22
    UINT32 HWAssertMsk23:1;            ///< Hardware Assert Mask 23
    UINT32 HWAssertMsk24:1;            ///< Hardware Assert Mask 24
    UINT32 HWAssertMsk25:1;            ///< Hardware Assert Mask 25
    UINT32 HWAssertMsk26:1;            ///< Hardware Assert Mask 26
    UINT32 HWAssertMsk27:1;            ///< Hardware Assert Mask 27
    UINT32 HWAssertMsk28:1;            ///< Hardware Assert Mask 28
    UINT32 HWAssertMsk29:1;            ///< Hardware Assert Mask 29
    UINT32 HWAssertMsk30:1;            ///< Hardware Assert Mask 30
    UINT32 HWAssertMsk31:1;            ///< Hardware Assert Mask 31
  } Field;
  UINT32 Value;
} HWA_MASK_HI_REGISTER;

#pragma pack (pop)
#endif /* _FABRIC_REGISTERS_DF3_H_ */

