/* $NoKeywords:$ */
/**
 * @file
 *
 * mn.h
 *
 * Common Northbridge
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
 * @e \$Revision: 318536 $ @e \$Date: 2015-05-12 07:46:27 +0800 (Tue, 12 May 2015) $
 *
 **/
/*****************************************************************************
  *
  * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
  *
  *******************************************************************************
  *
 */

#ifndef _MN_H_
#define _MN_H_

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
#define _32MB_RJ16          0x0200
#define _16MB_RJ16          0x0100
#define _4GB_RJ16           (((UINT32) 4) << (30 - 16))
#define _1TB_RJ16           (((UINT32) 1) << (40 - 16))
#define HT_REGION_BASE_RJ16 ((UINT32)0x00FD0000ul)

#define DCT_ACCESS_WRITE (UINT32) 0x40000000ul
#define MTRR_VALID  11

#define NB_ACCESS       0
#define DCT_PHY_ACCESS  1
#define DCT_EXTRA       2

#define DCT_PHY_DIRECT  0xF1

#define VT_MSK_VALUE 0
#define VT_ARRAY     1
/*---------------------------------------------
 *   TSEFO - Type Start End Function Offset
 *
 *   31:30   Type of access        (2-bits)
 *   29:29   Special               (1-bit)
 *   28:28   Phy Direct            (1-bit)
 *   27:27   Whole Register Access (1-bit)
 *   26:26   Linked                (1-bit)
 *   25:21   Start bit             (5-bits)
 *   20:16   End bit               (5-bits)
 *   15:00   Function_Offset/Index (16-bits)
 *---------------------------------------------
 */
typedef UINT32 TSEFO;

/**
  MAKE_TSEFO(TableName, a, b, c, d, BitFieldIndex):

  @param[in]    TableName
  @param[in]    BitFieldIndex
  @param[in]    a  Type of access.
  @param[in]    b  Index of register (can be in Function_Offset format).
  @param[in]    c  Highest bit of the bit field.
  @param[in]    d  Lowest bit of the bit field.

  @return   TSEFO  Access params encrypted in TSEFO format.
--*/
#define MAKE_TSEFO(TableName, a, b, c, d, BitFieldIndex) \
TableName[BitFieldIndex] = ( \
  (a == DCT_PHY_DIRECT) ? ( \
    (((UINT32) DCT_PHY_ACCESS) << 30) | (((UINT32) 1) << 28) | (((UINT32) b) & 0xFFFF) | (\
      ((c == 15) && (d == 0)) ? ( \
        (((UINT32) 1) << 27) | (((UINT32) b) & 0xF0000) \
      ) : ( \
        (c >= d) ? ( \
          (((UINT32) c) << 21) | (((UINT32) d) << 16) \
        ) : ( \
          (((UINT32) d) << 21) | (((UINT32) c) << 16) \
        ) \
      ) \
    ) \
  ) : ( \
    (((UINT32) a) << 30) | (((UINT32) b) & 0xFFFFFFF) | ( \
      (((UINT32) b) >> 16) ? ( \
        (((UINT32) 1) << 29) \
      ) : ( \
        (c >= d) ? ( \
          (((UINT32) c) << 21) | (((UINT32) d) << 16) \
        ) : ( \
          (((UINT32) d) << 21) | (((UINT32) c) << 16) \
        ) \
      ) \
    ) \
  ) \
)

/**
  LINK_TSEFO(TableName, LowerBitFieldIndex, HigherBitFieldIndex):
  This is one way link: any write to LowerBitFieldIndex would write to HigherBitFieldIndex,
                        but NOT the other way around.
  Requirement: LowerBitFieldIndex must be declared *right* before HigherBitFieldIndex.

  @param[in]    TableName
  @param[in]    LowerBitFieldIndex
  @param[in]    HigherBitFieldIndex

  @return   TSEFO  Access params encrypted in TSEFO format.
--*/
#define LINK_TSEFO(TableName, LowerBitFieldIndex, HigherBitFieldIndex) { \
  ASSERT (LowerBitFieldIndex == (HigherBitFieldIndex - 1)) ; \
  TableName[LowerBitFieldIndex] = TableName[LowerBitFieldIndex] | (((UINT32) 1) << 26); \
}

// Indicate when a bitfield has multiple memory Pstate copy
#define MULTI_MPSTATE_COPY_TSEFO(TableName, BitFieldName) \
  TableName[BitFieldName] = TableName[BitFieldName] | (((UINT32) 1) << 29)

#define TSEFO_TYPE(x)  ((UINT8) (((UINT32) (x) >> 30) & 0x03))
#define TSEFO_START(x) ((UINT8) (((UINT32) (x) >> 21) & 0x1F))
#define TSEFO_END(x)   ((UINT8) (((UINT32) (x) >> 16) & 0x1F))
#define TSEFO_OFFSET(x) ((UINT32) (x) & 0xFFFF)
#define TSEFO_LINKED(x) ((UINT8) (((UINT32) (x) >> 26) & 0x01))
#define TSEFO_DIRECT_EN(x) ((UINT8) (((UINT32) (x) >> 28) & 0x01))
#define TSEFO_WHOLE_REG_ACCESS(x) ((UINT8) (((UINT32) (x) >> 27) & 0x01))
#define _FN(x, y) (((UINT32) (x) << 12) + (UINT32) (y))
#define TSEFO_MULTI_MPSTATE_COPY(x) ((UINT8) (((UINT32) (x) >> 29) & 1))
#define _NOT_USED_ 0

/* */
#define B0_DLY   0
#define B1_DLY   1
#define B2_DLY   2
#define B3_DLY   3
#define B4_DLY   4
#define B5_DLY   5
#define B6_DLY   6
#define B7_DLY   7
#define ECC_DLY  8

#define DDR2_TRAIN_FLOW 0
#define DDR3_TRAIN_FLOW 1

#define DEFAULT_MRL_RETRAIN_TIMES 5
#define DEFAULT_PMU_RETRAIN_TIMES 10

//
// Minimum Data Eye width in consecutive 32nds of a UI of
// valid data
//
#define MIN_RD_DATAEYE_WIDTH_NB 4
#define MIN_WR_DATAEYE_WIDTH_NB 4

//
// RELIABLE READ/WRITE MODE DEFINITIONS
//
#define PRECHARGE_ALL_BANKS     0xFF           ///< Use to specify PrechargeAll Command to Precharge Cmd Function
#define CMD_TGT_A               0x00           ///< Issue Commands to Command Target A
#define CMD_TGT_AB              0x01           ///< Issue Commands to Command Targets A and B
#define CMD_TYPE_READ           0x00           ///< Read Command
#define CMD_TYPE_WRITE          0x01           ///< Write Command
#define CMD_TYPE_WR_RD          0x02           ///< Alternating Write and Read Commands
#define CPG_BANK_ADDRESS_A      0x0            ///< Dimm Bank address used in Reliable RD/RW mode training
#define CPG_BANK_ADDRESS_B      0x1            ///< Dimm Bank address used in Reliable RD/RW mode training
#define CPG_BANK_ADDRESS_B_MRL  0x4            ///< Dimm Bank address used in MultiRow MRL Reliable RD/RW mode training
#define CPG_ROW_ADDRESS_A       0x0            ///< Dimm Row address used in Reliable RD/RW mode training
#define CPG_ROW_ADDRESS_B       0x0            ///< Dimm Row address used in Reliable RD/RW mode training
#define CPG_COL_ADDRESS_A       0x0            ///< Dimm Column address used in Reliable RD/RW mode training
#define CPG_COL_ADDRESS_B       0x0            ///< Dimm Column address used in Reliable RD/RW mode training
#define CPG_COMPARE_MASK_LOW    0x00000000ul  ///< Dram DQMask[31:0] used to mask comparison on reads.  1=ignore
#define CPG_COMPARE_MASK_HI     0x00000000ul  ///< Dram DQMask[63:32] used to mask comparison on reads.  1=ignore
#define CPG_COMPARE_MASK_ECC    0x00          ///< Dram EccMask used to mask comparison on reads.  1=ignore
#define PRBS_SEED_32            0x062221ul     ///< Data PRBS Seed
#define PRBS_SEED_64            0x066665ul     ///< Data PRBS Seed
#define PRBS_SEED_128           0x026666ul     ///< Data PRBS Seed
#define PRBS_SEED_256           0x044443ul     ///< Data PRBS Seed


/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

/// Memory Bus Nibble List for Write CRC DQ Mapping
typedef enum DQ_MAP_NIBBLES {
  DQ_0_3,    ///< DQ 0-3
  DQ_4_7,    ///< DQ 4-7
  DQ_8_11,   ///< DQ 8-3
  DQ_12_15,  ///< DQ 12-15
  DQ_16_19,  ///< DQ 16-19
  DQ_20_23,  ///< DQ 20-23
  DQ_24_27,  ///< DQ 24-27
  DQ_28_31,  ///< DQ 28-31
  DQ_32_35,  ///< DQ 32-35
  DQ_36_39,  ///< DQ 36-39
  DQ_40_43,  ///< DQ 40-43
  DQ_44_47,  ///< DQ 44-47
  DQ_48_51,  ///< DQ 48-51
  DQ_52_55,  ///< DQ 52-55
  DQ_56_59,  ///< DQ 56-49
  DQ_60_63,  ///< DQ 60-63
  DQ_64_67,  ///< DQ 64-67
  DQ_68_71   ///< DQ 68-71
} DQ_MAP_NIBBLES;

/// Structure for Reliable Read/Write Mode Data
/// These are values that may need to be referenced by the low level functions
/// during training and are initialized at the beginning of a particular type of training.
typedef struct _RRW_SETTINGS {
  UINT8 CmdTgt;                          ///< Value to program into CmdTgt
  UINT8 TgtBankAddressA;                 ///< Target A Bank address
  UINT32 TgtRowAddressA;                 ///< Target A Row address
  UINT32 TgtColAddressA;                 ///< Target A Column address
  UINT8 TgtBankAddressB;                 ///< Target B Bank address
  UINT32 TgtRowAddressB;                 ///< Target B Row address
  UINT32 TgtColAddressB;                 ///< Target B Column address
  UINT32 CompareMaskLow;                 ///< Compare Mask Bits 31:0
  UINT32 CompareMaskHigh;                ///< Compare Mask Bits 63:32
  UINT8 CompareMaskEcc;                  ///< Compare Mask Ecc
  UINT32 DataPrbsSeed;                   ///< PRBS Seed value
  UINT8 DataPatGenSel;                   ///< Data Pattern Generator
} RRW_SETTINGS;

#define PRBS23_I   0                     ///< Data Pattern Generator value for PRBS23 I
#define PRBS23_II  1                     ///< Data Pattern Generator value for PRBS23 II
#define PRBS23_III 2                     ///< Data Pattern Generator value for PRBS23 III

/// DQS training related delays
typedef enum {
  AccessRcvEnDly,                       ///< Receiver enable delay
  AccessWrDatDly,                       ///< Write data delay
  AccessRdDqsDly,                       ///< Read DQS delay
  AccessWrDqsDly,                       ///< Write DQS delay
  AccessPhRecDly,                       ///< Phase recovery delay
  AccessRdDqs2dDly,                     ///< Read DQS 2D delay
  AccessWrDat2dDly,                     ///< Write data 2D delay
} TRN_DLY_TYPE;

/// Training patterns for position training
typedef enum {
  POS_PATTERN_72B,                      ///< 72 bit pattern
  POS_PATTERN_256B,                     ///< 256 bit pattern
} POS_TRN_PATTERN_TYPE;

/// ODT mode
typedef enum {
  MISSION_MODE,                         ///< ODT during mission mode
  WRITE_LEVELING_MODE                   ///< ODT during write leveling
} ODT_MODE;

/*
 *   DRBN - Dimm-Rank-Byte-Nibble
 *   31:12   Reserved
 *   11:09   Dimm       (3-bits)
 *   08      Rank       (1-bit)
 *   07:05   Reserved
 *   04:01   Byte       (4-bits)
 *   00      Nibble     (1-bit)
 */
typedef UINT32 DRBN;
#define MAKE_DRBN(dimm, rank, byte, nibble) ((((UINT32) (dimm)) << 9) | (((UINT32) (rank)) << 8) | \
(((UINT32) (byte)) << 1) | ((UINT32) (nibble)) )
#define DIMM_BYTE_ACCESS(dimm, byte) ((((UINT32) (dimm)) << 9) | (((UINT32) (byte)) << 1))
#define CS_NBBL_ACCESS(cs, nibble) ((((UINT32) (cs)) << 8) | ((UINT32) (nibble)))
#define DIMM_NBBL_ACCESS(dimm, nibble) ((((UINT32) (dimm)) << 9) | ((UINT32) (nibble)))
#define DRBN_DIMM(x) ((UINT8) (((UINT32) (x) >> 9) & 0x07))
#define DRBN_RANK(x) ((UINT8) (((UINT32) (x) >> 8) & 0x01))
#define DRBN_BYTE(x) ((UINT8) (((UINT32) (x) >> 1) & 0x0F))
#define DRBN_NBBL(x) ((UINT8) (((UINT32) (x)) & 0x01))
#define DRBN_DIMM_NBBL(x) ((UINT8) (((UINT32) (x)) & 0x1F))

/* Dimm Type mask */
#define DT_X4       0x01
#define DT_X8       0x02
#define DT_X16      0x04
#define DT_SR       0x10
#define DT_DR       0x20
#define DT_QR       0x40

#define DT_ANY_X4   0x71
#define DT_ANY_X8   0x72
#define DT_ANY_X16  0x74
#define DT_ANY_SR   0x17
#define DT_ANY_DR   0x27
#define DT_ANY_QR   0x47
#define DT_ANY_SR_DR    (DT_ANY_SR | DT_ANY_DR)
#define DT_ANY          (DT_ANY_SR | DT_ANY_DR | DT_ANY_QR)

/// Delay Scaling Info Struct - Describes number of delay increments per UI of a delay type
///
typedef struct _TRN_DLY_PARMS {
  UINT8  Min;    ///< Minimum Value
  UINT8  Max;    ///< Maximum Value
  UINT8  Mask;   ///< Mask to be applied (i.e. 0xFF if adjustable by one, 0xFE if adjustable by 2, etc.)
} TRN_DLY_PARMS;

/// Structure for certain data saving needed for DCT.
typedef struct {
  UINT8  RcvEnDlyCounts[8]; ///< DQS Receiver Enable Delay counts
  UINT32 PhRecReg[3];     ///< 3 Phase recovery control registers
  BOOLEAN Is2Dx4;         ///< Check if 2D training is enabled and x4 DIMMs are installed
} MEM_DCT_CACHE;

/// Structure for table driven support.
typedef struct _MEM_TBL_ALIAS {
  UINT8  time;            ///< Modification time.
  UINT8  node:4;          ///< Node on which to make modification.
  UINT8  dct:4;           ///< DCT on which to make modification.
  UINT8  dimm:4;          ///< Dimm on which to make modification.
  UINT8  attr:3;          ///< Attribute of modification.
  UINT8  vtype:1;         ///< Flag indicating value type.
  UINT32 bfindex;         ///< Bit field index that need to be modified.
  union {                 ///< Union is defined to easy select between single and multiple bytelane cases.
    struct {              ///< Sub-struct used for one bytelane value.
      UINT16 bytelane:16; ///< Bytelane on which to make modification.
      UINT32 value;       ///< Modified value.
      UINT8 reserved[3];  ///< Reserved for this purpose
    } s;                  ///< single value to one or multiple bytelanes
    UINT16 bytelanevalue[18]; ///< Array to specify individual bytelane values
  } data;
} MEM_TABLE_ALIAS;

/// Structure for Platform Specific Block.
typedef struct _MEM_PS_BLOCK {
  UINT8 DramTerm; ///< Dram Term
  UINT8 QR_DramTerm; ///< Dram Term for QR
  UINT8 DynamicDramTerm; ///< Dynamic Dram Term
  UINT8 NumOfReg[MAX_DIMMS_PER_CHANNEL]; ///< Number of registers on each RDIMM (From SPD)
  UINT8 MR0WR;  ///< MR0WR
  UINT8 MR0CL31;  ///< MR0[CL][3:1]
  UINT8 MR0CL0;  ///< MR0CL[0]
  UINT8 RttNom[8];  ///< RttNom value for maximum 8 chipsels per channel
  UINT8 RttWr[8];   ///< RttWr value for maximum 8 chipsels per channel
  UINT8 RttPark[8]; ///< RttPark value for maximum 8 chipsels per channel
  UINT8 F0RC8;  ///< F0RC8
  UINT8 F1RC0;  ///< F1RC0
  UINT8 F1RC1;  ///< F1RC1
  UINT8 F1RC2;  ///< F1RC2
  UINT8 RC10OpSpd;  ///< RC10[OperatingSpeed]
  UINT8 LrdimmRowAddrBits[MAX_DIMMS_PER_CHANNEL]; ///< Effective Row address bits used by LRDIMMS
  UINT16 SpeedLimit[VOLT1_25_ENCODED_VAL + 1];  ///< SpeedLimit of individual VDDIO
  UINT8 WLSeedVal;  ///< Seed value of WL training extracted from PSC table
  UINT16 HWRxENSeedVal;  ///< Seed value of HW RxEn training extracted from PSC table
  BOOLEAN ProcessorOnDieTerminationOff;           ///< processor ODT off
                                                  ///< FALSE = Phy enables receiver pad termination
                                                  ///< TRUE = Phy disables receiver pad termination
  UINT8  SpdDramType;     ///< DramType from SPD
  UINT8  SpdModuleType;   ///< ModuleType from SPD

  UINT8  CkeStrength;     ///< CKE drive strength
  UINT8  CsOdtStrength;   ///< CS ODT drive strength
  UINT8  AddrCmdStrength; ///< Addr Cmd drive strength
  UINT8  ClkStrength;     ///< CLK drive strength
  UINT8  DqStrength;      ///< Data drive strength
  UINT8  DqsStrength;     ///< DQS drive strength
  UINT8  OdtStrength;     ///< ODT drive strength
  UINT8  RxDlyOp:1;       ///< RxDly Operator
  UINT8  RxDlyOffset:7;   ///< RxDly Offset
  UINT8  TxDlyOp:1;       ///< TxDly Operator
  UINT8  TxDlyOffset:7;   ///< TxDly Offset
  UINT8  VrefOp:1;        ///< Vref Operator
  UINT8  VrefOffset:7;    ///< Vref Offset

  /* PUBLIC functions */
  BOOLEAN (*MemPDoPs) (struct _MEM_NB_BLOCK *NBPtr); ///< Function that gets Form factor info.
  VOID (*MemPGetPORFreqLimit) (struct _MEM_NB_BLOCK *NBPtr); ///< Function that gets the speed limit of a dimm population.
  BOOLEAN (*MemPGetPass1Seeds) (struct _MEM_NB_BLOCK *NBPtr); ///< Function that gets pass1 seeds of WL and RxEn training.

  /* Bitmaps used in looking up PS tables */
  UINT8  NumOfDimmSlots; ///< See enum type NOD_SUPPORTED
  UINT8  DimmType;       ///< See enum type DIMM_TYPE
  UINT16 RankType;       ///< Rank type of all populated DIMMs. See MemPGetPsRankType.
  UINT8  MotherboardLayer; ///< See enum type MB_LAYER_TYPE
  UINT8  MotherboardPower; ///< See enum type MB_POWER_TYPE
  VOID   *PmuSramMsgBlockPtr;  ///< Pointer to PMU SRAM MSG Block for this channel
} MEM_PS_BLOCK;

/// Structure parameters needed in frequency change of client NB.
typedef struct _MEM_FREQ_CHANGE_PARAM {
  UINT16 PllLockTimeDefault; ///< Default PllLockTime
  UINT8  RdPtrInit667orHigher; ///< RdPtrInit for frequency 667MHz and higher
  UINT8  RdPtrInitLower667; ///< RdPtrInit for frequency lower than 667MHz
  UINT8  NclkPeriodMul2x; ///< Multiplier for NclkPeriod in parial sum calculation x 2
  UINT8  MemClkPeriodMul2x; ///< Multiplier for MemClkPeriod in parial sum calculation x 2
  UINT8  SyncTimeMul4x;         ///< Multiplier for SyncTime
  UINT16 TDataProp800orHigher;  ///< TDataProp for frequency 800MHz or higher
  UINT16 TDataPropLower800;     ///< TDataProp for frequency lower than 800MHz
} MEM_FREQ_CHANGE_PARAM;

/// List for NB items that are supported
typedef enum {
  SetSpareEn,                      ///< Sets spare enable
  CheckSpareEn,                    ///< Spare enabled
  SetDllShutDown,                  ///< Sets DllShutDown
  CheckEccDLLPwrDnConfig,          ///< Checks to determine if EccDLLPwrDnConf needs to be adjusted
  DimmBasedOnSpeed,                ///< Checks to determine if Dimm number needs to be adjusted based on speed
  CheckMaxDramRate,                ///< Checks to determine the maximum rate
  Check1GAlign,                    ///< Checks to determine if 1 GB alignment is supported
  DramModeBeforeDimmPres,          ///< Check to determine if DRAM mode needs to be set before dimm presence
  DramModeAfterDimmPres,           ///< Check to determine if DRAM mode needs to be set after dimm presence
  CheckClearOnDimmMirror,          ///< Check to determine if we need to clear on DIMM mirror
  CheckDisDllShutdownSR,           ///< Check to determine if DisDllShutdown needs to be set
  CheckMemClkCSPresent,            ///< Check to determine if chipselect needs to be set based on disabled memclocks
  CheckChangeAvgValue,             ///< Check to determine if we need to change average value
  CheckMaxRdDqsDlyPtr,             ///< Check to determine change Max Rd Dqs Delay
  CheckPhyFenceTraining,           ///< Check to determine if we need to Phy Fence training
  CheckGetMCTSysAddr,              ///< Check to determine if we need to GetMCTSysAddr
  CheckSendAllMRCmds,              ///< Check to determine if we need to SendAllMRCmds
  CheckFindPSOverideWithSocket,    ///< Check to determine if we need to Find PSOveride With Socket
  CheckFindPSDct,                  ///< Check to determine if we need to Find PSOveride With DCT
  CheckODTControls,                ///< Check to determine if we need to set ODT controls
  CheckDummyCLRead,                ///< Check to determine if an extra dummy read is required
  CheckDllStdBy,                   ///< Check to determine if setting DLL stand by is required
  CheckSlewWithMarginImprv,        ///< Check to determine if setting of Slew With MarginImprv is required
  CheckSlewWithoutMarginImprv,     ///< Check to determine if setting of Slew Without MarginImprv is required
  CheckDllSpeedUp,                 ///< Check to determine if setting of Dll SpeedUp is required
  CheckDllRegDis,                  ///< Check to determine if setting of DLL Regulator Disable is required
  FenceTrnBeforeDramInit,          ///< Check to determine if fence training has been done before Dram init
  WLSeedAdjust,                    ///< Check to determine if WL seed needs to be adjusted
  UnifiedNbFence,                  ///< Check to determine if Phy fence is of Unified NB
  AdjustTwr,                       ///< Check to determine if Twr needs to be adjusted
  ChannelPDMode,                   ///< Check to determine if channel power down mode is the only that is supported
  ForceEnMemHoleRemapping,         ///< Check to determine if we need to force enabling memory hole remapping
  AdjustTrdrdSD,                   ///< Check to determine if we need to adjust TrdrdSD
  ReverseMaxRdLatTrain,            ///< Check to determine if reverse (pass to fail) algorithm is supported for MaxRdLat training
  SkipErrTrain,                    ///< Check to determine if skip error training is supported
  DramSrHys,                       ///< Check to determine if DRAM SR hysteresis is supported
  PchgPDMode,                      ///< Check to determine if Precharge powerdown mode is supported
  EccByteTraining,                 ///< Check to determine if DRAM ECC Byte training
  CheckDrvImpCtrl,                 ///< Check to determine if we need to set DrvImpCtrl
  CheckDramTerm,                   ///< Check to determine if we need to set DramTerm
  CheckDramTermDyn,                ///< Check to determine if we need to set DramTermDyn
  CheckQoff,                       ///< Check to determine if we need to set Qoff
  CheckSetSameDctODTsEn,           ///< Check to defermine if we need to set "ODTsEn" the same on each DCT
  WLNegativeDelay,                 ///< Check to determine if the NB can tolerate a negtive WL delay value
  SchedDlySlot1Extra,              ///< Check to determine if DataTxSchedDly Slot1 equation in slowMode to subtract an extra MEMCLK
  TwoStageDramInit,                ///< Check to determine if we need to seperate Draminit into 2 stages. The first one processes info on all nodes. The second one does Dram Init.
  ExtraPclkInMaxRdLat,             ///< Check to determine if an extra PCLK is needed for MaxRdLat
  CsrPhyPllPdEn,                   ///< Check to determine if CSR Phy PLL Powerdown is enabled or not
  AdjustTrc,                       ///< Check to determine if we need to adjust Trc
  ProgramCsrComparator,            ///< Check to determine if we need to program CsrComparator with the same value as D18F2x09C_x0D0F_0[7:0]1F[RxVioLvl]
  EnProcOdtAdvForUDIMM,            ///< Check to determine if we need to always enable ProcOdtAdv for UDIMM
  SetTDqsForx8DimmOnly,            ///< Only set MR1[TDQS] for x8 DIMMs when x4 and x8 DIMMs are both present on a channel
  WlRttNomFor1of3Cfg,              ///< Set Rtt_Nom = Rtt_Wr in one of three DIMMs per channel configurations
  PerformanceOnly,                 ///< Only support performance policy, does not support battery life policy
  AdjustTrp,                       ///< Check to determin if Trp needs to be adjusted
  DllStaggerEn,                    ///< Check to determin if Dll Stagger should be turned on
  RxChnMntClksEn,                  ///< Check to determin if Receive Channel Maintenance Clock Enable should be turned on
  ForcePhyToM0,                    ///< Force Phy to M0
  G5DimmInD3Socket,                ///< Check to determine if DDR3 DIMM socket will be used for GDDR5 DIMMs as well
  MasterDct1Relocate,              ///< Check to determine if Master DCT for channel is DCT 3 instead of DCT 1
  PerDimmAggressors2D,             ///< Use only the even Chipselect of each installed DIMM as an Aggressor
  OptimizedPatternWrite2D,         ///< Optimize 2D training to Write Pattern Once, read many times.
  AMPIsEnabled,                    ///< Check to detemine if AMP is exactly enabled.
  ScrubberEn,                      ///< Check to determine if DRAM scrubber is already enabled
  SwitchRdDqsDlyForMaxRdLatency,   ///< Use the different RdDqsDly value for MaxRdLatency calculation before/after DQS training
  SelectMotherboardLayer,          ///< Check to determine if multiple motherboard layer design applies
  SelectMotherboardPower,          ///< Check to determine if multiple motherboard power design applies
  DqsRoutingTypeDependence,        ///< Check to determine if the PS tables depend upon the DQS routing type
  EnumSize                         ///< Size of list
} NB_SUPPORTED;

/// List for family specific functions that are supported
typedef enum {
  BeforePhyFenceTraining,          ///< Family specific tasks before Phy Fence Training
  BeforeMemClkFreqVal,             ///< hook before setting MemClkFreqVal bit
  AfterMemClkFreqVal,              ///< Override PllMult and PllDiv
  OverridePllMult,                 ///< Override PllMult
  OverridePllDiv,                  ///< Override PllDiv
  BeforeMemClr,                    ///< Before MemClr
  SendMrsCmdsPerCs,                ///< Send MRS commands per CS
  SetupHwTrainingEngine,           ///< Setup Hardware training engine for specific training type
  OverrideRcvEnSeed,               ///< Override seed for hardware based RcvEn training
  AddlMaxRdLatTrain,               ///< Perform additional MaxRdLat training if needed
  ForceAutoComp,                   ///< Force Auto Comp
  DetectMemPllError,               ///< Detect MemPll Divide by 3 bug
  ReEnablePhyComp,                 ///< Re-Enable Phy Compensation after RcvEn Training
  ExtractWLODT,                    ///< Extract WL ODT value thr given ODT pattern
  DCTSelectSwitch,                 ///< Select DCT when we switch DCT
  ScrubberErratum,                 ///< Erratum for setting scrubber rate
  MR0_PPD,                         ///< Override MR0[PPD]
  GetDdrMaxRate,                   ///< Interpret DdrMaxRate with Familiy-specific encoding
  ExitPhyAssistedTraining,         ///< Perform family specific tasks when exiting phy assisted training
  AfterSaveRestore,                ///< Action after save/restore execution
  OverrideDataTxFifoWrDly,         ///< Override DataTxFifoWrDly based on training result of WrDatDly
  OverrideRcvEnSeedPassN,          ///< Override seed for hardware based RcvEn training where N greater than 0
  AfterMemClkFreqChg,              ///< Reprogram DIMMs' buffers after MEMCLK frequency change
  AdjustTxpdll,                    ///< Adjust Txpdll value to encoded register value
  CalcWrDqDqsEarly,                ///< Calculate WrDqDqsEarly
  TrainWlPerNibble,                ///< Train Write Leveling per nibble
  TrainWlPerNibbleAdjustWLDly,     ///< Train WL per nibble and adjust the WL delay
  TrainWlPerNibbleSeed,             ///< Save the seed for WL nibble based training
  TrainRxEnPerNibble,              ///< Train Rx Enable Training per nibble
  TrainRxEnAdjustDlyPerNibble,     ///< Train Rx Enable Training nibble and adjust the RxEn delay
  TrainRxEnGetAvgDlyPerNibble,     ///< Display Rx Enable Training average nibble value for each BL
  InitPerNibbleTrn,                ///< Initiates Per Nibble Training.
  BeforeSetCsTri,                  ///< Modify CS tri-state bit map.
  ForceRdDqsPhaseB,                ///< Force RdDqsDly to phase B
  ProgramPOdtOff,                  ///< Disable/enable receiver pad termination
  SetDqsODT,                       ///< Set DQS ODT
  DisLowPwrDrvStr,                 ///< Hook to skip setting LowPowerDriveStrengthEn
  AdjustRdDqsDlyOffset,            ///< Adjust the bit offset of the RdDqsDly Bit Bitfield before writing and after reading
  ResetRxFifoPtr,                  ///< Reset RxFifo pointer during Read DQS training
  EnableParityAfterMemRst,         ///< Enable DRAM Address Parity after memory reset.
  FinalizeVDDIO,                   ///< Finalize VDDIO
  TrainingNibbleZero,              ///< Check for see Nibble zero is being trained (individually or with x8 training)
  ProgOdtControl,                  ///< Calculate RdOdtTrnOnDly and RdOdtOnDuration
  SetSkewMemClk,                   ///< Set SkewMemClk
  OverrideWLSeed,                  ///< Override WL seed
  AdjustCSIntLvLowAddr,            ///< Adjust CS interleaving low address
  Adjust2DVrefStepSize,            ///< Adjusts the step size for Vref during 2D RdDqs training
  Adjust2DDelayStepSize,           ///< Adjusts the step size for RdDqs during 2D RdDqs training
  ReleaseNbPstate,                 ///< Release NB P-state
  InitializeRxEnSeedlessTraining,  ///< Initializes RxEn Seedless Training
  TrackRxEnSeedlessRdWrNoWindBLError,           ///< Track Bytelane Errors resulting from No window for RxEn Seedless Training
  TrackRxEnSeedlessRdWrSmallWindBLError,        ///< Track Bytelane Errors resulting from Small window for RxEn Seedless Training
  InitialzeRxEnSeedlessByteLaneError,           ///< Initializes ByteLaneError to False for RxEn Seedless Training
  InitExtMMIOAddr,                 ///< Initializes extended MMIO address space
  MemPstateStageChange,            ///< handle training when multiple memory pstate is supported
  ProgramFence2RxDll,              ///< program RxDll in a different register
  RdDqsDlyRestartChk,              ///< Check to see if we need to restart RdDqsDly
  BeforeWrDatTrn,                  ///< Check to see if special handling is needed before WrDatDly Training
  ForceLvDimmVoltage,              ///< Force LVDIMM voltage to 1.5V
  BfAfExcludeDimm,                 ///< Workaround before and after excluding dimms
  AdjustWrDqsBeforeSeedScaling,    ///< For some family, negative WL is compensated and WrDqs needs to be adjusted before seed scaling
  OverridePrevPassRcvEnDly,        ///< Check to determine if we need override PrevPassRcvEnDly
  AdjustRdPtrInit,                 ///< Adjust RdPtrInit value according to certain conditions
  Adjust2DPhaseMaskBasedOnEcc,     ///< Adjusts the Phase Mask Based on ECC
  FixupSysAddr,                    ///< Adjust physical address before identifying DIMM.
  RegAccessFence,                  ///< Make sure previous phy registers writes are done
  WLMR1,                           ///< Check to see if we need to do special things when sending MR1 during WL
  FixupUmaInfo,                    ///< Fix up UMA info for a specific family
  RelocatePscTblEntryByMotherBoardLayer, ///< Relocate the PSC table entry pointer by mother board layer design
  AmpVoltageDisp,                     ///< To display AMP voltage values
  AdjustHwRcvEnSeedGross,          ///< Adjust the SeedGross value for hardware receiver enable training under certaion conditions
  ForceEccSymbolSize,              ///< Force the ECC Symbol Size to x4 or x8
  AdjustRdDqsDlyForMaxRdLat,       ///< Adjust RdDqsDly used for MaxRdLatency calculation
  SetMaxRdLatBasedOnSeededRxEnDly, ///< Prior to DQS Receiver Enable Training, Set MaxRdLatency value based on seeded value of RxEnDly
  /// 2D Read/Write Training Feature Functions
  RdWr2DTraining,                  ///< Main Common 2D Rd/Wr Training Fucntion
  CheckRdWr2DTrainingPerConfig,    ///< Determine if 2D Training should be run on the current config
  RdWr2DSelectIntExtVref,          ///< 2D Training function to select internal or external Vref
  RdWr2DProgramVref,               ///< 2D Training function to Program Vref
  RdWr2DScaleVref,                 ///< 2D Training function to scale Vref to the memory controller register
  RdWr2DProgramDelays,             ///< 2D Training function program delay values
  RdWr2DProgramMaxVref,            ///< 2D Training function to set final vref
  RdWr2DDataCollection,            ///< 2D Training Data Collection
  RdWr2DInitVictim,                ///< 2D Training Victim Initialization
  RdWr2DInitVictimChipSel,         ///< 2D Training Initialize Victim CS to be trained
  RdWr2DStartVictim,               ///< 2D Training Start Victim Data pattern
  RdWr2DFinalizeVictim,            ///< 2D Training Finalize Victim Traffic
  RdWr2DCompareInPhase,            ///< 2D Training In-Phase comparison
  RdWr2DCompare180Phase,           ///< 2D Training 180 Degree ou-of-phase comparison
  RdWr2DProgramDataPattern,        ///< 2D Training Program Data Pattern
  /// End 2D Read/Write Training Functions
  TurnOnAggressorChannels,         ///< Enable the Aggressor channels.
  StartAggressor,                  ///< Start Aggressor pattern generation.
  StopAggressor,                   ///< Stop Aggressor pattern generation.
  DisableMemHoleMapping,           ///< Disable Memory Hole Settings
  RestoreMemHoleMapping,           ///< Enable Memory Hole Settings
  PhyInitVref,                     ///< Program VrefSel during phy init
  DllPDBypassClkAddrCmd,           ///< DllPDBypass function to enable bypass mode for Clk/Addr/Cmd
  DllPDBypassTxRxEn,               ///< DllPDBypass function to bypass Tx/Rx En
  EnableLowPowerRcvr,              ///< DllPDBypass function to enable low power receiver
  DllPDBypassWL,                   ///< DllPDBypass function to skip write leveling
  DllPDBypassAdjustRxEnDly,        ///< DllPDBypass function to adjust Rx En delay
  IntermediateMemclkFreqVal,       ///< Workaround for some families needing intermediate frequency valid
  DramScrubErratum792,             ///< Workaround for some families requiring disabling the DramScrubRate and DramScrubRedir
  NumberOfHooks                    ///< Size of list
} FAMILY_SPECIFIC_FUNC_INDEX;

///< Entry for SPD Timing
typedef struct {
  BIT_FIELD_NAME BitField;  ///< Bit field name of the timing
  UINT8 Min; ///< Minimum value for timing
  UINT8 Max; ///< Maximum value for timing
  UINT8 Bias; ///< Bias from actual value
  UINT8 Ratio_x2; ///< Actual value will be multiplied by (Ratio_x2/2)
} CTENTRY;

/// Structure for northbridge block.
typedef struct _MEM_NB_BLOCK {
  MEM_DATA_STRUCT *MemPtr; ///< Point to MEM_DATA_STRUCT.
  MEM_PARAMETER_STRUCT *RefPtr; ///< Point to MEM_PARAMETER_STRUCT.
  DIE_STRUCT *MCTPtr;             ///< point to current Node's MCT struct
  DCT_STRUCT *DCTPtr;             ///< point to current Node's DCT struct
  DCT_STRUCT *AllDCTPtr;          ///< point to all Node's DCT structs
  CH_DEF_STRUCT *ChannelPtr;      ///< point to current channel data
  SPD_DEF_STRUCT *SPDPtr;         ///< Point to SPD data for current DCT.
  struct _MEM_TECH_BLOCK *TechPtr; ///< point to technology block.
  struct _MEM_FEAT_BLOCK_NB *FeatPtr; ///< point to NB Specific feature block.
  struct _MEM_SHARED_DATA *SharedPtr;     ///< Pointer to Memory scratchpad area
  struct _MEM_NB_BLOCK    *AdjacentDieNBPtr;     ///< Pointer to Adjacent Die In same socket
  BOOLEAN  DieEnabled[MAX_NODES_SUPPORTED];///< Indicates the Dies that are enabled
  SPD_DEF_STRUCT *AllNodeSPDPtr; ///< Point to SPD data for the system.
  DIE_STRUCT *AllNodeMCTPtr;      ///< point to all Node's MCT structs
  UINT8 DimmToBeUsed; ///< Dimm to be used in recovery mode.
  MEM_PS_BLOCK *PsPtr; ///< point to platform specific block
  MEM_PS_BLOCK *PSBlock; ///< point to the first platform specific block on this node.
  MEM_FREQ_CHANGE_PARAM *FreqChangeParam; ///< pointer to parameter of frequency change.
  VOID *PlatSpecFlowPtr;           ///< Pointer to the Platform Specific Flow Block

  PCI_ADDR PciAddr;                ///< PCI address for this node
  TSEFO *NBRegTable;               ///< contains all bit field definitions

  UINT8 Node; ///< current node.
  UINT8 Dct; ///< current DCT.
  UINT8 Channel; ///< current channel.
  UINT8 DctCount; ///< number of DCTs on the current NB.
  UINT8 ChannelCount; ///< number of channels per DCT of the current NB.
  UINT8 NodeCount; ///< number of Nodes supported
  UINT8 CsPerDelay;   ///< number of CS controlled per set of delay registers.
  UINT8 CsPerChannel; ///< number of CS per channel.
  BOOLEAN Ganged; ///< mode for current MCT controller.
  POS_TRN_PATTERN_TYPE PosTrnPattern; ///< specifies the pattern that should be used for position training.
  BOOLEAN MemCleared; ///< memory clear flag.
  UINT32  CPGInit; ///< continuous pattern generation flag.
  UINT16  StartupSpeed; ///< startup speed for DDR.
  UINT16  M1Speed; ///< DDR Speed for Memory PState 1
  UINT16  RcvrEnDlyLimit; ///< maximum value that RcvrEnDly field can take.
  UINT32 McaNbCtlReg; ///< reserve MCA reports.
  UINT32 VarMtrrHiMsk;   ///< variable MTRR mask for upper 32 bits.
  UINT32 CsRegMsk;       ///< mask for CS base register
  UINT32 NBClkFreq; ///< Current NB Clock frequency
  UINT8  DefDctSelIntLvAddr; ///< Default DctSelIntLvAddr
  UINT8  TrainingSequenceIndex; ///< Index into the Training Sequence
  UINT8  RdDqsDlyForMaxRdLat; ///Maximum value of Read DQS Delay used in MaxRdLatency calculation
  RRW_SETTINGS RrwSettings;  ///<Settings for Reliable Read/Write mode
  UINT8 TotalRdDQSDlyRange;   ///< Max number of RdDQS Delays
  UINT8 TotalWrDatDlyRange;   ///< Max number of WrDat Delays
  INT16 MinRxEnSeedGross; ///< Minimum value of the Receiver Enable
  INT16 MaxRxEnSeedTotal; ///< Maximum value of the Receiver Enable
  UINT8 TotalMaxVrefRange;       ///< Max number of Vref settings
  UINT8 MaxSeedCount; ///< Max number of Data patterns to be generated
  UINT32 TotalBitTimes2DRdTraining;  ///< Total Bit Times for 2D Read Training
  UINT32 TotalBitTimes2DWrTraining;  ///< Total Bit Times for 2D Write Training
  UINT8 Vref;            ///< Vref setting
  UINT8 RdDqsDly;        ///< RdDQSDly setting
  UINT16 MaxFreqVDDIO[VOLT1_25 + 1]; ///< Max Frequency each voltage supports.
  UINT32 PhaseLaneMask;   ///< Lane Mask for Inphase and 180 phase registers
  UINT8 MaxDiamondStep;  ///< Maximum Diamond step Size
  UINT8 DiamondWidthRd;  ///< Diamond Width for 2D Rd Training
  UINT8 DiamondWidthWr;  ///< Diamond Width for 2D Wr Training
  UINT8 RdRolloverMultiple;  ///< 2D Training Read Rollover Multiple.
  UINT8 CurrentAggressorCSTarget[MAX_CHANNELS_PER_SOCKET]; ///< Current Aggressor CS targeted
  UINT8 MaxAggressorCSEnabled[MAX_CHANNELS_PER_SOCKET]; ///< Maximum Number of Aggressor CS targeted
  UINT8 MaxAggressorDimms[MAX_CHANNELS_PER_SOCKET];  ///< Maximum Number of Aggressor CS DIMMs
  UINT8 InitialAggressorCSTarget[MAX_CHANNELS_PER_SOCKET];  ///< Initial Number of the first CS Aggressor
  BOOLEAN OrigDisAutoRefreshState;      ///< Original state of Dis Auto Refresh
  BOOLEAN Execute1dMaxRdLatTraining;    ///< Indicates if 1D training should be executed
  BOOLEAN Override2DTraining;                  ///< 2D training has been overriden
  VOID* PmuFirmwareImageTable;                  ///< Table of FW Images. Assigned when Memory type is known.
  UINT8 PmuFirmwareImageTableSize;             ///< Number of entries in the Image Table;
  UINT8 PmuFirmwareImage;                      ///< PMU firmware image to be use.
  BOOLEAN SelfRefreshAbort;                    ///< Self Refresh Abort Enable

  MEM_DCT_CACHE DctCache[MAX_CHANNELS_PER_SOCKET]; ///< Allocate space for MCT_DCT_CACHE.
  MEM_DCT_CACHE *DctCachePtr;    ///< pointer to current Node's Node struct

  /* Temporary storage */
  BOOLEAN   ClToNbFlag;        ///< is used to restore ClLinesToNbDis bit after memory
  UINT8     NbFreqChgState;    ///< is used as a state index in NB frequency change state machine
  UINT32    NbPsCtlReg;        ///< is used to save/restore NB Pstate control register
  MEM_PSTATE MemPstate;        ///< is used to save current memory Pstate context
  NB_PSTATE  NbPstate;         ///< is used to save current NB Pstate context
  MEM_PSTATE_STAGE MemPstateStage; ///< is used to save the current stage status of memory pstate
  UINT8     DqsRoutingType;    ///< is used to save the routing type of DQS signal
  RDDQSDLY_RTN_STAT   RdDqsDlyRetrnStat;    ///< is used to check if RdDqsDly training needs to be restarted
  CONST UINT32 *RecModeDefRegArray;   ///< points to an array of default register values that are set for recovery mode
  struct _MEM_DRAM_INFO  *DevInfoArray;   ///< points to an array of DRAM device info
  UINT32    DisDramScrub;      ///< is used to save scrubber status
  UINT32    LgcyMmioHoleEnMap; ///< is used to save LgcyMmioHoleEn for all the ranges
  UINT32    DctIntLvEnMap;     ///< Used to save DctIntLvEn for all the DCT ranges
  UINT32    MctPrefecherDisMap;///< Used to save PrefIoDis and PrefCpuDis

  ///< Determines if code should be executed on a give NB
  BOOLEAN IsSupported[EnumSize];
  BOOLEAN (*FamilySpecificHook[NumberOfHooks]) (struct _MEM_NB_BLOCK *NBPtr, VOID *OptParam);   ///< This array of pointers point to
                                                                                                ///< family specific functions.

  /* PUBLIC functions */
  VOID (*SwitchDCT) (struct _MEM_NB_BLOCK *NBPtr, UINT8 DCT); ///< Switch to current DCT.
  VOID (*SwitchChannel) (struct _MEM_NB_BLOCK *NBPtr, UINT8 Channel); ///< Switch to current channel.
  VOID (*SetMaxLatency) (struct _MEM_NB_BLOCK *NBPtr, UINT16 MaxRcvEnDly); ///< Set Max Rd Latency.
  VOID (*getMaxLatParams) (struct _MEM_NB_BLOCK *NBPtr, UINT16 MaxDlyForMaxRdLat, UINT16 *MinDly, UINT16 *MaxDly, UINT16 *DlyBias); ///< retrieves the Max latency parameters.
  BOOLEAN (*GetSysAddr) (struct _MEM_NB_BLOCK *NBPtr, UINT8 Receiver, UINT32 *Addr); ///< Get system address for training dimm.
  BOOLEAN (*RankEnabled) (struct _MEM_NB_BLOCK *NBPtr, UINT8 Receiver); ///< Check if a rank has been enabled.
  BOOLEAN (*InitializeMCT) (struct _MEM_NB_BLOCK *NBPtr); ///< MCT initialization.
  BOOLEAN (*FinalizeMCT) (struct _MEM_NB_BLOCK *NBPtr); ///< sets final values in BUCFG and BUCFG2.
  BOOLEAN (*InitMCT) (struct _MEM_NB_BLOCK *NBPtr); ///< main entry call for memory initialization.
  VOID (*SendMrsCmd) (struct _MEM_NB_BLOCK *NBPtr); ///< send MRS command.
  VOID (*sendZQCmd) (struct _MEM_NB_BLOCK *NBPtr); ///< send ZQ command.
  VOID (*TrainingFlow) (struct _MEM_NB_BLOCK *NBPtr); ///< Set the training flow control
  VOID (*WritePattern) (struct _MEM_NB_BLOCK *NBPtr, UINT32 Address, UINT8 Pattern[], UINT16 ClCount); ///< Write training pattern.
  VOID (*ReadPattern) (struct _MEM_NB_BLOCK *NBPtr, UINT8 Buffer[], UINT32 Address, UINT16 ClCount); ///< Read training pattern.
  VOID (*GenHwRcvEnReads) (struct _MEM_NB_BLOCK *NBPtr, UINT32 Address);  ///< generates a continuous burst of reads during HW RcvEn training.
  UINT16 (*CompareTestPattern) (struct _MEM_NB_BLOCK *NBPtr, UINT8 Buffer[], UINT8 Pattern[], UINT16 ByteCount); ///< Compare training pattern.
  UINT16 (*InsDlyCompareTestPattern) (struct _MEM_NB_BLOCK *NBPtr, UINT8 Buffer[], UINT8 Pattern[], UINT16 ByteCount); ///< Compare training pattern using 1 beat offset to check for insertion delay
  BOOLEAN (*StitchMemory) (struct _MEM_NB_BLOCK *NBPtr); ///< combines all the memory into a contiguous map.
  VOID (*ProgramCycTimings) (struct _MEM_NB_BLOCK *NBPtr); ///< programs the memory controller with SPD timings.
  BOOLEAN (*AutoConfig) (struct _MEM_NB_BLOCK *NBPtr); ///< programs the memory controller with configuration parameters
  BOOLEAN (*PlatformSpec) (struct _MEM_NB_BLOCK *NBPtr); ///< programs platform specific parameters.
  VOID (*DisableDCT) (struct _MEM_NB_BLOCK *NBPtr); ///< disable a DCT if no dimm presents.
  VOID (*StartupDCT) (struct _MEM_NB_BLOCK *NBPtr); ///< start a DCT.
  VOID (*SyncTargetSpeed) (struct _MEM_NB_BLOCK *NBPtr); ///< Check and sync the target speed of all channels of this node.
  VOID (*ChangeFrequency) (struct _MEM_NB_BLOCK *NBPtr); ///< Frequency change sequence.
  BOOLEAN (*RampUpFrequency) (struct _MEM_NB_BLOCK *NBPtr); ///< Change frequency to the next supported level.
  BOOLEAN (*ChangeNbFrequency) (struct _MEM_NB_BLOCK *NBPtr); ///< Change NB frequency.
  VOID (*PhyFenceTraining) (struct _MEM_NB_BLOCK *NBPtr); ///< Phy fence training.
  BOOLEAN (*SyncDctsReady) (struct _MEM_NB_BLOCK *NBPtr); ///< Synchronize DCTs.
  BOOLEAN (*HtMemMapInit) (struct _MEM_NB_BLOCK *NBPtr); ///< Memory map initialization.
  VOID (*SyncAddrMapToAllNodes) (struct _MEM_NB_BLOCK *NBPtr); ///< copies the Node 0 map to all the other nodes.
  BOOLEAN (*CpuMemTyping) (struct _MEM_NB_BLOCK *NBPtr); ///< MTRR and TOM setting.
  VOID (*BeforeDqsTraining) (struct _MEM_NB_BLOCK *NBPtr); ///< processes needed before DQS training.
  VOID (*AfterDqsTraining) (struct _MEM_NB_BLOCK *NBPtr); ///< processes needed after DQS training.
  BOOLEAN (*OtherTiming) (struct _MEM_NB_BLOCK *NBPtr); ///< setting non-spd timing.
  VOID (*UMAMemTyping) (struct _MEM_NB_BLOCK *NBPtr); ///< MTRR and TOM setting needed for UMA platform.
  VOID (*Feature) (struct _MEM_NB_BLOCK *NBPtr); ///< Feature support.
  UINT8 (*GetSocketRelativeChannel) (struct _MEM_NB_BLOCK *NBPtr, UINT8 Dct, UINT8 Channel); ///< Get channel number relative to a socket.
  VOID (*SetDramOdtRec) (struct _MEM_NB_BLOCK *NBPtr, ODT_MODE OdtMode, UINT8 ChipSelect, UINT8 TargetCS); ///< Set Dram ODT.
  UINT32 (*GetSysAddrRec) ( VOID ); ///< Get system address for training.
  VOID (*SwitchNodeRec) (struct _MEM_NB_BLOCK *NBPtr, UINT8 NodeID); ///< Switch to current node.
  VOID (*TechBlockSwitch) (struct _MEM_NB_BLOCK *NBPtr); ///< Selects appropriate Tech functions for the NB.
  VOID (*SetEccSymbolSize) (struct _MEM_NB_BLOCK *NBPtr); ///< Set Ecc Symbol Size.
  VOID (*GetTrainDlyParms) (struct _MEM_NB_BLOCK *NBPtr, TRN_DLY_TYPE TrnDly, TRN_DLY_PARMS *Parms); ///< Retrieve Specific Delay range info for current NB under current conditions.
  AGESA_STATUS (*TrainingPatternInit) (struct _MEM_NB_BLOCK *NBPtr); ///< Initialize the training Pattern
  AGESA_STATUS (*TrainingPatternFinalize) (struct _MEM_NB_BLOCK *NBPtr); ///< Finalize the training Pattern
  BOOLEAN (*GetApproximateWriteDatDelay) (struct _MEM_NB_BLOCK *NBPtr, UINT8 Index, UINT8 *Value); ///< Retrieve the next WrDat Delay Approximation
  VOID (*FlushPattern) (struct _MEM_NB_BLOCK *NBPtr, UINT32 Address, UINT16 ClCount);  ///<Flush the training pattern
  UINT8 (*MinDataEyeWidth) (struct _MEM_NB_BLOCK *NBPtr);  ///<Get Min Data Eye Width in 32nds of a UI
  VOID (*MemNCapSpeedBatteryLife) (struct _MEM_NB_BLOCK *NBPtr); ///< Caps speed based on battery life check.
  UINT32 (*GetUmaSize) (struct _MEM_NB_BLOCK *NBPtr); ///< Get Uma Size
  UINT8 (*GetMemClkFreqId) (struct _MEM_NB_BLOCK *NBPtr, UINT16 Speed); ///< Translate MemClk frequency in MHz to MemClkFreq value
  VOID (*EnableSwapIntlvRgn) (struct _MEM_NB_BLOCK *NBPtr, UINT32 Base, UINT32 Limit); ///< Enable swapped interleaving region
  BOOLEAN (*ChangeNbFrequencyWrap) (struct _MEM_NB_BLOCK *NBPr, UINT32 NBPstate); ///< Wrapper for NB Pstate change function
  VOID (*WaitXMemClks) (struct _MEM_NB_BLOCK *NBPr, UINT32 MemClkCount); ///< Waits a specified number of MemClks
  UINT16 (*GetMemClkFreqInCurrentContext) (struct _MEM_NB_BLOCK *NBPtr); ///< Obtains the memory frequency in the current context
  VOID (*ProgramNbPsDependentRegs) (struct _MEM_NB_BLOCK *NBPtr); ///< Programs NB Pstate dependent registers
  VOID (*AllocateC6Storage) (struct _MEM_NB_BLOCK *NBPtr); ///< Allocates DRAM region for Core C6
  BOOLEAN (*MemS3PspDetect) (struct _MEM_NB_BLOCK *NBPtr); ///< PSP detection
  BOOLEAN (*MemS3PspPlatformSecureBootEn) (struct _MEM_NB_BLOCK *NBPtr); //< PSP Platform Secure Boot detection
  BOOLEAN (*MemRunningOnPsp) (struct _MEM_NB_BLOCK *NBPtr); ///< PSP detection

  /* PUBLIC Get/Set register field functions */
  UINT32 (*GetBitField) (struct _MEM_NB_BLOCK *NBPtr, UINTN FieldName); ///< Pci register bit field read.
  VOID (*SetBitField) (struct _MEM_NB_BLOCK *NBPtr, UINTN FieldName, UINT32 Value); ///< Pci register bit field write.
  BOOLEAN (*BrdcstCheck) (struct _MEM_NB_BLOCK *NBPtr, UINTN FieldName, UINT32 Value); ///< Pci register bit field broadcast read.
  VOID (*BrdcstSet) (struct _MEM_NB_BLOCK *NBPtr, UINTN FieldName, UINT32 Value); ///< Pci register bit field broadcast write.
  VOID (*PollBitField) (struct _MEM_NB_BLOCK *NBPtr, UINTN FieldName, UINT32 Field, UINT32 MicroSecond, BOOLEAN IfBroadCast); ///< Poll a Pci register bitfield.
  UINT32 (*GetTrainDly) (struct _MEM_NB_BLOCK *NBPtr, TRN_DLY_TYPE TrnDly, DRBN DrbnVar); ///< Training register bit field read.
  VOID (*SetTrainDly) (struct _MEM_NB_BLOCK *NBPtr, TRN_DLY_TYPE TrnDly, DRBN DrbnVar, UINT16 Value); ///< Training register bit field write.
  AGESA_STATUS (*InitRecovery) (struct _MEM_NB_BLOCK *NBPtr); ///< Recover mode memory init
  VOID (*MemRecNInitializeMctNb) (struct _MEM_NB_BLOCK *NBPtr); ///< Initialize MCT changes
  VOID (*MemRecNFinalizeMctNb) (struct _MEM_NB_BLOCK *NBPtr); ///< Finalize MCT changes
  VOID (*MemNInitPhyComp) (struct _MEM_NB_BLOCK *NBPtr); ///< Init Phy compensation
  VOID (*MemNBeforeDramInitNb) (struct _MEM_NB_BLOCK *NBPtr); ///< Before Dram init
  BOOLEAN (*MemNIsIdSupportedNb) (SOC_LOGICAL_ID *LogicalIdPtr); ///< Determines if a given CPU id is supported
  BOOLEAN (*MemNPlatformSpecificFormFactorInitNb) (struct _MEM_NB_BLOCK *NBPtr); ///< Platform specific functions
  VOID (*MemNSetOtherTimingNb) (struct _MEM_NB_BLOCK *NBPtr); ///< Set non-spd timings
  VOID (*MemNBeforePlatformSpecNb) (struct _MEM_NB_BLOCK *NBPtr); ///< Apply settings prior to platform specific settings
  UINT32 (*MemNCmnGetSetFieldNb) (struct _MEM_NB_BLOCK *NBPtr, UINT8 IsSet, UINTN FieldName, UINT32 Field); ///< Sets a register value
  UINT32 (*MemNcmnGetSetTrainDly) (struct _MEM_NB_BLOCK *NBPtr, UINT8 IsSet, TRN_DLY_TYPE TrnDly, DRBN DrbnVar, UINT16 Field); ///< Sets a training delay field
  VOID (*MemPPhyFenceTrainingNb) (struct _MEM_NB_BLOCK *NBPtr); ///< Phy Fence training
  VOID (*MemPNodeMemBoundaryNb) (struct _MEM_NB_BLOCK *NBPtr, UINT32 *NodeSysLimit); ///< Phy Fence training
  UINT32 (*MemRecNCmnGetSetFieldNb) (struct _MEM_NB_BLOCK *NBPtr, UINT8 IsSet, UINTN FieldName, UINT32 Field); ///< This functions sets bit fields in recover mode
  UINT32 (*MemRecNcmnGetSetTrainDlyNb) (struct _MEM_NB_BLOCK *NBPtr, UINT8 IsSet, TRN_DLY_TYPE TrnDly, DRBN DrbnVar, UINT16 Field); ///< This functions sets bit fields in recover mode
  VOID (*MemRecNSwitchDctNb) (struct _MEM_NB_BLOCK *NBPtr, UINT8 NodeID); ///< S3 Exit self refresh register
  VOID (*MemNPFenceAdjustNb) (struct _MEM_NB_BLOCK *NBPtr, INT16 *Value16); ///< Adjust Avg PRE value of Phy fence training
  VOID (*MemNPrepareRcvrEnDlySeed) (struct _MEM_NB_BLOCK *NBPtr); ///< Seed valude for HW RxEn training
  UINT8 (*MemNGetDramTerm) (struct _MEM_NB_BLOCK *NBPtr, UINT8 ChipSel); ///< Dram Term value
  UINT8 (*MemNGetDynDramTerm) (struct _MEM_NB_BLOCK *NBPtr, UINT8 ChipSel); ///< Dynamic Dram Term value
  VOID (*MemNSaveMR0) (struct _MEM_NB_BLOCK *NBPtr, UINT32 MrsAddress); ///< Save MR0 during memory initialization
  UINT32 (*MemNGetMR0CL) (struct _MEM_NB_BLOCK *NBPtr); ///< MR0[CL] value
  UINT32 (*MemNGetMR0WR) (struct _MEM_NB_BLOCK *NBPtr); ///< MR0[WR] value
  UINT32 (*MemNGetMR2CWL) (struct _MEM_NB_BLOCK *NBPtr); ///< MR2[CWL] value
  UINT32 (*InPhaseCompareRdDqs2DPattern) (struct _MEM_NB_BLOCK *NBPtr, UINT8 Buffer[], UINT8 Pattern[], UINT16 ByteCount); ///< InPhase Compare training pattern for RdDQS 2D training
  UINT32 (*Phase180CompareRdDqs2DPattern) (struct _MEM_NB_BLOCK *NBPtr, UINT8 Buffer[], UINT8 Pattern[], UINT16 ByteCount); ///< 180 Compare training pattern for RdDQS 2D training
  VOID (*AgressorContinuousWrites) (struct _MEM_NB_BLOCK *NBPtr, UINT8 SeedCount, BOOLEAN TurnOnInfinite); ///< Enables/Disables continuous writes on unused agressor channels
  UINT32 (*GetPrbs2dRdDqsSeed) (struct _MEM_NB_BLOCK *NBPtr, UINT8 SeedCount); ///< Prbs Seed for 2d RdDqs training
  VOID (*DisableInfiniteWritePattern) (struct _MEM_NB_BLOCK *NBPtr); ///< Disables infinite Write training pattern.
  VOID (*EnableInfiniteWritePattern) (struct _MEM_NB_BLOCK *NBPtr); ///< Enables infinite training pattern.
  VOID (*InitializeRdDqs2dVictimContinuousWrites) (struct _MEM_NB_BLOCK *NBPtr); ///< Initializes victim infinite training pattern for RdDqs2d Training.
  VOID (*FinalizeRdDqs2dVictimContinuousWrites) (struct _MEM_NB_BLOCK *NBPtr); ///< Finalizes victim infinite training pattern for RdDqs2d Training.
  VOID (*InitializeRdDqs2dVictimChipSelContinuousWrites) (struct _MEM_NB_BLOCK *NBPtr); ///< Initializes victim infinite training pattern for RdDqs2d Training for victim CS.
  VOID (*StartRdDqs2dVictimContinuousWrites) (struct _MEM_NB_BLOCK *NBPtr , UINT8 SeedCount); ///< Starts victim infinite training pattern for RdDqs2d Training.
  BOOLEAN (*ConfigureDCTForGeneral) (struct _MEM_NB_BLOCK *NBPtr); ///< Configure DCT For General use.
  BOOLEAN (*ConfigureDCTForTraining) (struct _MEM_NB_BLOCK *NBPtr); ///< Configure DCT For Training.
  BOOLEAN (*ConfigureDCTNonExplicitSeq) (struct _MEM_NB_BLOCK *NBPtr); ///< Configure DCT for Non Explicit Seq.
  BOOLEAN (*SynchronizeChannels) (struct _MEM_NB_BLOCK *NBPtr); ///< Synchronize Channels.
  BOOLEAN (*ConfigureDCTNormal) (struct _MEM_NB_BLOCK *NBPtr); ///< Configure DCT for Normal operation.
  BOOLEAN (*MemN2DRdDQSDataCollection) (struct _MEM_NB_BLOCK *NBPtr); ///< 2D training data collection method
  UINT32 (*MemNGetMemoryWidth) (struct _MEM_NB_BLOCK *NBPtr); ///< Get memory width
  VOID (*MemNDisableScrubber) (struct _MEM_NB_BLOCK *NBPtr); ///< Disable scrubber prior to any write access to additonal address space
  VOID (*MemNRestoreScrubber) (struct _MEM_NB_BLOCK *NBPtr); ///< Restore scrubber settings after write access to additonal address space
  VOID (*MemNSetEccExclusionRange) (struct _MEM_NB_BLOCK *NBPtr);  ///< Set the ECC exclusion range
  VOID (*MemNDisableMemClk) (struct _MEM_NB_BLOCK *NBPtr);  ///< Disable unused Memory clocks
  VOID (*MemNTristateUnusedChipSelects) (struct _MEM_NB_BLOCK *NBPtr);  ///< Tristate unused chipselects
  VOID (*GetCSIntLvAddr) (struct _MEM_NB_BLOCK *NBPtr, UINT8 BankAddrMap, UINT8 *LowBit, UINT8 *HiBit); ///< Get Chip select interleave address.
} MEM_NB_BLOCK;

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */
VOID
MemNCmnInitDefaultsNb (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

VOID
MemNInitNBDataNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNSwitchDCTNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Dct
  );

VOID
MemNSwitchChannelNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Channel
  );

UINT32
MemNGetBitFieldNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINTN FieldName
  );

VOID
MemNSetBitFieldNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINTN FieldName,
  IN       UINT32 Field
  );

BOOLEAN
MemNBrdcstCheckNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINTN FieldName,
  IN       UINT32 Field
  );

VOID
MemNBrdcstSetNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINTN FieldName,
  IN       UINT32 Field
  );


UINT32
MemNGetTrainDlyNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       TRN_DLY_TYPE TrnDly,
  IN       DRBN DrbnVar
  );

VOID
MemNSetTrainDlyNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       TRN_DLY_TYPE TrnDly,
  IN       DRBN DrbnVar,
  IN       UINT16 Field
  );

BOOLEAN
MemNRankEnabledNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Receiver
  );

UINT8 MemNGetSocketRelativeChannelNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Dct,
  IN       UINT8 Channel
  );

BOOLEAN
MemNGetMCTSysAddrNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Receiver,
  OUT      UINT32 *AddrPtr
  );

BOOLEAN
MemNStitchMemoryNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNDisableDCTUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNStartupDCTUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNSendZQCmdNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNSwapBitsUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNTrainPhyFenceNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT32
MemNcmnGetSetTrainDlyUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 IsSet,
  IN       TRN_DLY_TYPE TrnDly,
  IN       DRBN DrbnVar,
  IN       UINT16 Field
  );

VOID
MemNSyncTargetSpeedNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNCPUMemTypingNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNUMAMemTypingNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNSyncAddrMapToAllNodesNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNInitMCTNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNPollBitFieldNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINTN FieldName,
  IN       UINT32 Field,
  IN       UINT32 MicroSecond,
  IN       BOOLEAN IfBroadCast
  );

VOID
MemNSetEccSymbolSizeNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNDQSTiming3Nb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

AGESA_STATUS
MemNTrainingPatternInitNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNGetApproximateWriteDatDelayNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Index,
  IN OUT   UINT8 *Value
  );

AGESA_STATUS
MemNTrainingPatternFinalizeNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNFlushPatternNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address,
  IN       UINT16 ClCount
  );

UINT8
MemNCSPerDelayNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT8
MemNMinDataEyeWidthNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT16
MemNCompareTestPatternNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT16 ByteCount
  );

UINT16
MemNInsDlyCompareTestPatternNb (
  IN       MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT16 ByteCount
  );

BOOLEAN
MemNSetMTRRUmaRegionUCNb  (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 *BasePtr,
  IN OUT   UINT32 *LimitPtr
  );

UINT8
MemNGetMemClkFreqIdUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 Speed
  );

UINT16
MemNGetMemClkFreqUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 FreqId
  );

BOOLEAN
MemNGetPlatformCfgNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNPlatformSpecUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNDisableMemClkUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNTristateUnusedChipSelectsNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNProgramPlatformSpecNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

INT16
MemNCalcCDDNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       TRN_DLY_TYPE TrnDlyType1,
  IN       TRN_DLY_TYPE TrnDlyType2,
  IN       BOOLEAN SameDimm,
  IN       BOOLEAN DiffDimm
  );

VOID
MemNChangeFrequencyUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNPhyVoltageLevelNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNPhyFenceTrainingUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNEnableSwapIntlvRgnNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN          UINT32 Base,
  IN          UINT32 Limit
  );

VOID
MemNPrepareRcvrEnDlySeedNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNChangeNbFrequencyUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNWaitXMemClksNb (
  IN OUT   MEM_NB_BLOCK  *NBPtr,
  IN       UINT32 MemClkCount
  );

BOOLEAN
memNSequenceDDR3Nb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT16
GetTrainDlyFromHeapNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       TRN_DLY_TYPE TrnDlyType,
  IN       DRBN Drbn
  );

BOOLEAN
MemNTrainingFlowUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNSetupHwTrainingEngineUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       VOID* OptParam
  );

BOOLEAN
MemNDctCfgSelectUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       VOID *Dct
  );

BOOLEAN
MemNGetMaxDdrRateUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       VOID *DdrMaxRate
  );

VOID
MemNSetTxpNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNCalcWrDqDqsEarlyUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

VOID
MemNGetTrainDlyParmsUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       TRN_DLY_TYPE TrnDly,
  IN OUT   TRN_DLY_PARMS *Parms
  );

BOOLEAN
MemNPlatformSpecificFormFactorInitTblDrvNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNChangeNbFrequencyWrapUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 NBPstate
  );

VOID
MemNForcePhyToM0Unb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNProgramCycTimingsUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNSetSkewMemClkUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

VOID
MemNSendMrsCmdUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT8
MemNGetDramTermTblDrvNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 ChipSel
  );

UINT8
MemNGetDynDramTermTblDrvNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 ChipSel
  );

UINT32
MemNGetMR2CWLUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT32
MemNGetMR0CLTblDrvNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT32
MemNGetMR0WRTblDrvNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNRampUpFrequencyUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNAfterSaveRestoreUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemNAdjustRdDqsDlyOffsetUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Offset
  );

VOID
MemNC6AdjustMSRs (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNC6AndGnbAcpEngineAdjustMSRs (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNInitializeRxEnSeedlessTrainingUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemNTrackRxEnSeedlessRdWrNoWindBLErrorUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemNTrackRxEnSeedlessRdWrSmallWindBLErrorUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemNInitialzeRxEnSeedlessByteLaneErrorUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

VOID
MemNPhyPowerSavingMPstateUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNChangeMemPStateContextNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_PSTATE MemPstate
  );

VOID
MemNDramPowerMngTimingNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNAllocateNBRegTableNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       NB_REG_TAB_HANDLE  Handle
  );

VOID
MemTResetRcvFifoUnb (
  IN OUT   struct _MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dummy
  );

UINT32
MemNInPhaseCompareRdDqs2DPatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT16 ByteCount
  );

UINT32
MemN180CompareRdDqs2DPatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT16 ByteCount
  );

VOID
MemNAgressorContinuousWritesUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 SeedCount,
  IN       BOOLEAN TurnOnInfinite
  );

UINT32
MemNGetPrbs2dRdDqsSeedUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 SeedCount
  );

BOOLEAN
MemNAdjust2DPhaseMaskBasedOnEccUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

VOID
MemNRrwPrechargeCmd (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 ChipSelect,
  IN       UINT8 Bank
  );

VOID
MemNRrwActivateCmd (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 ChipSelect,
  IN       UINT8 Bank,
  IN       UINT32 RowAddress
  );

VOID
MemNStartRdDqs2dVictimContinuousWritesUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 SeedCount
  );

VOID
MemNInitializeRdDqs2dVictimChipSelContinuousWritesUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNFinalizeRdDqs2dVictimContinuousWritesUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNInitializeRdDqs2dVictimContinuousWritesUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNAdjustWrDqsBeforeSeedScalingUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *WrDqsBias
  );

BOOLEAN
MemNDefaultFamilyHookNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

VOID
MemNBrdcstSetUnConditionalNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINTN FieldName,
  IN       UINT32 Field
  );

BOOLEAN
MemN2DRdDQSEyeRimSearchUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNContReadPatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT32 Address,
  IN       UINT16 ClCount
  );

VOID
MemNContWritePatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address,
  IN       UINT8 Pattern[],
  IN       UINT16 ClCount
  );

UINT16
MemNCompareTestPatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT16 ByteCount
  );

UINT32
MemNGetMemoryWidthUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNModdifyMtrrFixDramModEn (
  IN       UINT32             MsrAddress,
  IN       UINT64             *Value,
  IN       VOID               *StdHeader
  );

BOOLEAN
MemNAmpVoltageDispUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemNAdjustRdDqsDlyForMaxRdLatUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

VOID
MemNSetEccExclusionRangeUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT32
MemNGetNbClkFreqByPstateUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 NBPstate
  );

VOID
MemNAdjustDdrSpeedUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNIntermediateMemclkFreqValUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

VOID
MemNGenHwRcvEnReadsUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address
  );

VOID
MemNEnableInfiniteWritePatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNDisableInfiniteWritePatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNInitDqsTrainRcvrEnHwNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNDisableDqsTrainRcvrEnHwNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNInitCPGUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT16
MemNInsDlyCompareTestPatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT16 ByteCount
  );

VOID
MemNGetPORFreqLimitTblDrvNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT8
MemTGetDQMappingByte (
  IN DQ_MAP_NIBBLES Nibble
  );

UINT16
MemNS3GetMemClkFreqUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 FreqId
  );
#endif  /* _MN_H_ */


