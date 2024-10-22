/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _APCB_MEM_PLATFORM_TUNING_H_
#define _APCB_MEM_PLATFORM_TUNING_H_
//
// @APCB_START
//

//
// @MACRO
//

//////////////////////////////
//
// Platform Tuning Definitions
//
//////////////////////////////
/// Memory Pstate
typedef enum {
  MEMORY_PSTATE0,   ///< Memory Pstate 0
  MEMORY_PSTATE1,   ///< Memory Pstate 1
  MEMORY_PSTATE2,   ///< Memory Pstate 2
  MEMORY_PSTATE3,   ///< Memory Pstate 3
} MEM_PSTATE;

/// PMU SEQUENCE_CTRL
#define SMB_TESTEnable    (1 << 0)
#define SMB_ShortReset    (1 << 1)
#define SMB_ShortPattern  (1 << 2)
#define SMB_InputVref     (1 << 3)
#define SMB_SkipVrefUpd   (1 << 4)
#define SMB_PER_RANK_TMG    0x01
#define SMB_2T_TMG          0x01
#define SMB_CSMUX_45_ADDR   0x04
#define SMB_CSMUX_67_ADDR   0x08
#define SMB_TMG_MUX_67_ADDR 0x10
#define SMB_DRAM_TYPE_DDR4_UDIMM  0x02
#define SMB_SEQ_DEVINIT     (1 << 0)      // [0] DevInit - Device/phy initialization. Should always be set.
#define SMB_SEQ_WRLVL       (1 << 1)      // [1] WrLvl - Write leveling
#define SMB_SEQ_RXEN        (1 << 2)      // [2] RxEn - Read gate training
#define SMB_SEQ_RDDQS1D     (1 << 3)      // [3] RdDQS1D - 1d read dqs training
#define SMB_SEQ_WRDQ1D      (1 << 4)      // [4] WrDQ1D - 1d write dq training
#define SMB_SEQ_RD2D        (1 << 5)      // [5] rd2D - 2d read dqs training
#define SMB_SEQ_WR2D        (1 << 6)      // [6] wr2D - 2d write dq training
#define SMB_SEQ_LCDLCAL     (1 << 7)      // [7] LcdlCal - LCDL offset calibration training
#define SMB_SEQ_RDDESKEW    (1 << 8)      // [8] RdDeskew - Per lane read dq deskew training
#define SMB_SEQ_MXRDLAT     (1 << 9)      // [9] MxRdLat - Max read latency training
#define SMB_SEQ_DWL         (1 << 10)     // [10] DWL - LDRIMM DRAM Write Leveling
#define SMB_SEQ_MREP        (1 << 11)     // [11] MREP - LRDIMM DRAM Interface MDQ Receive Enable Phase training
#define SMB_SEQ_MRD         (1 << 12)     // [12] MRD - DRAM-to-DB Read Delay training
#define SMB_SEQ_MWD         (1 << 13)     // [13] MWD - DB-to-DRAM Write Delay training
#define SMB_DFE 0x01                      // [0]  1 = Run rx2D with DFE, 0 = Run rx2D with DFE off
#define SMB_FFE 0x01                      // [0]  1 = Run rx2D with FFE, 0 = Run rx2D with FFE off

//
// PLATFORM_TUNING_ENTRY
//
//
// @APCB_ENUM
//
typedef enum _PLATFORM_TUNING_ENTRY {
   PLATFORM_TUNING_ENTRY_START = 0x1000,
   // Enablement
   PLATFORM_TUNING_CHIPSEL_INTLV,
   PLATFORM_TUNING_ECC,
   PLATFORM_TUNING_PARITY,
   PLATFORM_TUNING_MEM_BUS_LIMIT,
   PLATFORM_TUNING_MBIST_ENABLE,
   PLATFORM_TUNING_MBIST_TEST_MODE,
   PLATFORM_TUNING_MBIST_AGGRESSOR_ON,
   PLATFORM_TUNING_MBIST_PER_BIT_SLAVE_DIE_REPORT,
   PLATFORM_TUNING_POST_PACKAGE_REPAIR_ENABLE,
   PLATFORM_TUNING_POST_PACKAGE_REPAIR_ALL_BANKS,

   // Performace
   PLATFORM_TUNING_BANK_GROUP_SWAP,
   PLATFORM_TUNING_ODTS_CMD_THROTTLE_ENABLE,
   PLATFORM_TUNING_AUTO_REFFINE_GRAN_MODE,
   PLATFORM_TUNING_ADDR_CMD_PARITY_RETRY,
   PLATFORM_TUNING_RCD_PARITY_ENABLE,
   PLATFORM_TUNING_WR_CRC_ENABLE,
   PLATFORM_TUNING_WR_CRC_RETRY,
   PLATFORM_TUNING_ECC_SYMBOL_SIZE,
   PLATFORM_TUNING_DDR4_UECC_RETRY,

   // Power
   PLATFORM_TUNING_ENABLE_POWER_DOWN,
   PLATFORM_TUNING_MEMORY_ALL_CLOCKS_ON,
   PLATFORM_TUNING_POWER_DOWN_MODE,
   PLATFORM_TUNING_TEMPERATURE_CONTROLLED_REFRESH_ENABLE,

   // Platform
   PLATFORM_TUNING_CMD_BUS_TMG,
   PLATFORM_TUNING_DATA_BUS_TMG,

   // Memory Discovery
   PLATFORM_TUNING_SPD_VALUE_OVERRIDE,
   PLATFORM_TUNING_SPD_CRC_IGNORE,

   // UMC Configuration
   PLATFORM_TUNING_GEAR_DOWN_EN,
   PLATFORM_TUNING_CMD_2T_EN,
   PLATFORM_TUNING_WR_PREAMBLE,
   PLATFORM_TUNING_RD_PREAMBLE,
   PLATFORM_TUNING_ODT_PAT_CTRL_CS0,
   PLATFORM_TUNING_ODT_PAT_CTRL_CS1,
   PLATFORM_TUNING_ODT_PAT_CTRL_CS2,
   PLATFORM_TUNING_ODT_PAT_CTRL_CS3,
   PLATFORM_TUNING_TX_EQ_IMPEDANCE_DQ,
   PLATFORM_TUNING_TX_IMPEDANCE_DQ,
   PLATFORM_TUNING_TX_EQ_HI_IMPEDANCE_DQ,
   PLATFORM_TUNING_TX_ODT_DRV_STREN,
   PLATFORM_TUNING_TRCDWR,
   PLATFORM_TUNING_TRCDRD,
   PLATFORM_TUNING_TRAS,
   PLATFORM_TUNING_TCL,
   PLATFORM_TUNING_TRP,
   PLATFORM_TUNING_TRTP,
   PLATFORM_TUNING_TRRDDLR,
   PLATFORM_TUNING_TRRDL,
   PLATFORM_TUNING_TRRDS,
   PLATFORM_TUNING_TFAWDLR,
   PLATFORM_TUNING_TFAWSLR,
   PLATFORM_TUNING_TFAW,
   PLATFORM_TUNING_TWTRL,
   PLATFORM_TUNING_TWTRS,
   PLATFORM_TUNING_TCWL,
   PLATFORM_TUNING_TWR,
   PLATFORM_TUNING_TRCPAGE,
   PLATFORM_TUNING_TRFC,
   PLATFORM_TUNING_TRFC2,
   PLATFORM_TUNING_TRFC4,
   PLATFORM_TUNING_TSTAG,
   PLATFORM_TUNING_TSTAGLR,
   PLATFORM_TUNING_TSTAG2LR,
   PLATFORM_TUNING_TSTAG4LR,
   PLATFORM_TUNING_TRDRDBAN,
   PLATFORM_TUNING_TRDRDSCL,
   PLATFORM_TUNING_TRDRDSCDLR,
   PLATFORM_TUNING_TRDRDSC,
   PLATFORM_TUNING_TRDRDSD,
   PLATFORM_TUNING_TRDRDDD,
   PLATFORM_TUNING_TWRWRBAN,
   PLATFORM_TUNING_TWRWRSCL,
   PLATFORM_TUNING_TWRWRSCDLR,
   PLATFORM_TUNING_TWRWRSC,
   PLATFORM_TUNING_TWRWRSD,
   PLATFORM_TUNING_TWRWRDD,
   PLATFORM_TUNING_TWRRDSCDLR,
   PLATFORM_TUNING_TRDWR,
   PLATFORM_TUNING_TWRRD,

   // PHY Configuration
   PLATFORM_TUNING_ADDR_CMD_SETUP,
   PLATFORM_TUNING_CS_ODT_SETUP,
   PLATFORM_TUNING_CKE_SETUP,
   PLATFORM_TUNING_CLK_DRV_STREN,
   PLATFORM_TUNING_ADDR_CMD_DRV_STREN,
   PLATFORM_TUNING_CS_ODT_DRV_STREN,
   PLATFORM_TUNING_CKE_DRV_STREN,
   /// PHY Regs start
   PLATFORM_TUNING_PHY_REG_START,
   PLATFORM_TUNING_RX_ENABLE_DELAY,
   PLATFORM_TUNING_TX_DQ_DELAY,
   PLATFORM_TUNING_TX_DQS_DELAY,
   PLATFORM_TUNING_RX_PB_DELAY,
   PLATFORM_TUNING_RX_CLK_DELAY,
   PLATFORM_TUNING_VREF_DAC0,
   PLATFORM_TUNING_VREF_DAC1,
   PLATFORM_TUNING_VREF_IN_GLOBAL,
   PLATFORM_TUNING_DQ_DQS_RCV_CTRL,
   PLATFORM_TUNING_PHY_REG_END,
   /// PHY Regs end

   // 1D training input
   PLATFORM_TUNING_1D_TRAIN_MR0,
   PLATFORM_TUNING_1D_TRAIN_MR1,
   PLATFORM_TUNING_1D_TRAIN_MR2,
   PLATFORM_TUNING_1D_TRAIN_MR3,
   PLATFORM_TUNING_1D_TRAIN_MR4,
   PLATFORM_TUNING_1D_TRAIN_MR5,
   PLATFORM_TUNING_1D_TRAIN_MR6,
   PLATFORM_TUNING_1D_TRAIN_PHY_VREF,
   PLATFORM_TUNING_1D_TRAIN_SEQUENCE_CTRL,
   PLATFORM_TUNING_1D_TRAIN_DFI_MRL_MARGIN,
   PLATFORM_TUNING_1D_CUSTOM_MSG,
   PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL0,
   PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL1,
   PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL2,
   PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL3,
   PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL4,
   PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL5,
   PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL6,
   PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL7,
   PLATFORM_TUNING_1D_TRAIN_ALT_WCAS_L,

   // 2D training input
   PLATFORM_TUNING_2D_TRAIN_MR0,
   PLATFORM_TUNING_2D_TRAIN_MR1,
   PLATFORM_TUNING_2D_TRAIN_MR2,
   PLATFORM_TUNING_2D_TRAIN_MR3,
   PLATFORM_TUNING_2D_TRAIN_MR4,
   PLATFORM_TUNING_2D_TRAIN_MR5,
   PLATFORM_TUNING_2D_TRAIN_MR6,
   PLATFORM_TUNING_2D_TRAIN_PHY_VREF,
   PLATFORM_TUNING_2D_TRAIN_SEQUENCE_CTRL,
   PLATFORM_TUNING_2D_TRAIN_DFI_MRL_MARGIN,
   PLATFORM_TUNING_2D_TRAIN_RX2D_TRAIN_OPT,
   PLATFORM_TUNING_2D_TRAIN_TX2D_TRAIN_OPT,
   PLATFORM_TUNING_2D_CUSTOM_MSG,
   PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL0,
   PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL1,
   PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL2,
   PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL3,
   PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL4,
   PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL5,
   PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL6,
   PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL7,
   PLATFORM_TUNING_2D_TRAIN_ALT_WCAS_L,

   //Validity
   PLATFORM_TUNING_VALIDITY_DRAM_MFG_ID,
   PLATFORM_TUNING_VALIDITY_MODULE_MFG_ID,
   PLATFORM_TUNING_VALIDITY_DRAM_STEPPING,
   PLATFORM_TUNING_VALIDITY_TERMINATOR,

   // END! END! END!
   PLATFORM_TUNING_ENTRY_END,
   PLATFORM_TUNING_TERMINATOR = 0xFEEF
} PLATFORM_TUNING_ENTRY;

typedef UINT8 PTO_ENTRY;

//
// @CONSTANT
//

#define OP_MINUS 0x11
#define OP_PLUS 0x22
#define OP_OVERRIDE 0x33

//
// @MACRO
//


//////////////////////////
//
// USER INTERFACE MACRO(S)
//
//////////////////////////
#define DW_TO_DB(dw) \
 ((UINT8)(((UINT16) (dw)) & 0xFF)), ((UINT8)((((UINT16) (dw)) >> 8) & 0xFF))

#define DD_TO_DB(dd) \
 ((UINT8)(((UINT32) (dd)) & 0xFF)), ((UINT8)((((UINT32) (dd)) >> 8) & 0xFF)), ((UINT8)((((UINT32) (dd)) >> 16) & 0xFF)), ((UINT8)((((UINT32) (dd)) >> 24) & 0xFF))

#define NIBBLE_TO_DB(nibble0, nibble1) \
 ((UINT8)(((UINT8) (nibble0) & 0x0F) | (((UINT8) (nibble1) << 4) & 0xF0)))


#define MAKE_PTO_TERMINATE() DW_TO_DB(PLATFORM_TUNING_TERMINATOR)
//
// Enablement entrys
//
#define MAKE_PTO_CHIPSEL_INTLV_ENTRY(SocketBitMap, ChannelBitMap, Enable) \
  DW_TO_DB(PLATFORM_TUNING_CHIPSEL_INTLV), 6, ANY_SOCKET, ANY_CHANNEL, (Enable)
// Ecc
#define MAKE_PTO_ECC_ENTRY(SocketBitMap, ChannelBitMap, Enable) \
   DW_TO_DB(PLATFORM_TUNING_ECC), 6, ANY_SOCKET, ANY_CHANNEL, (Enable)
// Parity
#define MAKE_PTO_PARITY_ENTRY(SocketBitMap, ChannelBitMap, Enable) \
   DW_TO_DB(PLATFORM_TUNING_PARITY), 6, ANY_SOCKET, ANY_CHANNEL, (Enable)
// Mem Bus Limit
#define MAKE_PTO_MEM_BUS_LIMIT_ENTRY(SocketBitMap, ChannelBitMap, Speed) \
   DW_TO_DB(PLATFORM_TUNING_MEM_BUS_LIMIT), 7, ANY_SOCKET, ANY_CHANNEL, DW_TO_DB((Speed))
// Mbist Enable
#define MAKE_PTO_MBIST_ENABLE_ENTRY(SocketBitMap, ChannelBitMap, Enable) \
   DW_TO_DB(PLATFORM_TUNING_MBIST_ENABLE), 6, ANY_SOCKET, ANY_CHANNEL, (Enable)
// Mbist Test Mode
#define MAKE_PTO_MBIST_TEST_MODE_ENTRY(SocketBitMap, ChannelBitMap, Mode) \
   DW_TO_DB(PLATFORM_TUNING_MBIST_TEST_MODE), 6, ANY_SOCKET, ANY_CHANNEL, (Mode)
// Mbist Aggressor On
#define MAKE_PTO_MBIST_AGGRESSOR_ON_ENTRY(SocketBitMap, ChannelBitMap, Enable) \
   DW_TO_DB(PLATFORM_TUNING_MBIST_AGGRESSOR_ON), 6, ANY_SOCKET, ANY_CHANNEL, (Enable)
// Mbist Per Bit Slave Die Report
#define MAKE_PTO_MBIST_PER_BIT_SLAVE_DIE_REPORT_ENTRY(SocketBitMap, ChannelBitMap, Value8) \
   DW_TO_DB(PLATFORM_TUNING_MBIST_PER_BIT_SLAVE_DIE_REPORT), 6, ANY_SOCKET, (ChannelBitMap), (Value8)
// Post Package Repair Enable
#define MAKE_PTO_POST_PACKAGE_REPAIR_ENABLE_ENTRY(SocketBitMap, ChannelBitMap, Enable) \
   DW_TO_DB(PLATFORM_TUNING_POST_PACKAGE_REPAIR_ENABLE), 6, ANY_SOCKET, ANY_CHANNEL, (Enable)
// Post Package Repair All Banks
#define MAKE_PTO_POST_PACKAGE_REPAIR_ALL_BANKS_ENTRY(SocketBitMap, ChannelBitMap, Enable) \
   DW_TO_DB(PLATFORM_TUNING_POST_PACKAGE_REPAIR_ALL_BANKS), 6, ANY_SOCKET, ANY_CHANNEL, (Enable)

//
// Performance entrys
//
// Bank Group Swap
#define MAKE_PTO_BANK_GROUP_SWAP_ENTRY(SocketBitMap, ChannelBitMap, Enable) \
   DW_TO_DB(PLATFORM_TUNING_BANK_GROUP_SWAP), 6, ANY_SOCKET, ANY_CHANNEL, (Enable)
// Odts Command Throttle Enable
#define MAKE_PTO_ODTS_CMD_THROTTLE_ENABLE_ENTRY(SocketBitMap, ChannelBitMap, Enable) \
   DW_TO_DB(PLATFORM_TUNING_ODTS_CMD_THROTTLE_ENABLE), 6, ANY_SOCKET, ANY_CHANNEL, (Enable)
// Auto Reffine Gran Mode
#define MAKE_PTO_AUTO_REFFINE_GRAN_MODE_ENTRY(SocketBitMap, ChannelBitMap, Value) \
   DW_TO_DB(PLATFORM_TUNING_AUTO_REFFINE_GRAN_MODE), 6, ANY_SOCKET, ANY_CHANNEL, (Value)
// Address Command Parity Retry
#define MAKE_PTO_ADDR_CMD_PARITY_RETRY_ENTRY(SocketBitMap, ChannelBitMap, Enable) \
   DW_TO_DB(PLATFORM_TUNING_ADDR_CMD_PARITY_RETRY), 6, ANY_SOCKET, ANY_CHANNEL, (Enable)
// Rcd Parity
#define MAKE_PTO_RCD_PARITY_ENABLE_ENTRY(SocketBitMap, ChannelBitMap, Enable) \
   DW_TO_DB(PLATFORM_TUNING_RCD_PARITY_ENABLE), 6, ANY_SOCKET, ANY_CHANNEL, (Enable)
// WrCrc Enable
#define MAKE_PTO_WR_CRC_ENABLE_ENTRY(SocketBitMap, ChannelBitMap, Enable) \
   DW_TO_DB(PLATFORM_TUNING_WR_CRC_ENABLE), 6, ANY_SOCKET, ANY_CHANNEL, (Enable)
// WrCrc Retry
#define MAKE_PTO_WR_CRC_RETRY_ENTRY(SocketBitMap, ChannelBitMap, Enable) \
   DW_TO_DB(PLATFORM_TUNING_WR_CRC_RETRY), 6, ANY_SOCKET, ANY_CHANNEL, (Enable)
// Ecc symbol size
#define MAKE_PTO_ECC_SYMBOL_SIZE_ENTRY(SocketBitMap, ChannelBitMap, Value) \
   DW_TO_DB(PLATFORM_TUNING_ECC_SYMBOL_SIZE), 6, ANY_SOCKET, ANY_CHANNEL, (Value)

//
// Power entrys
//
// Enable power down
#define MAKE_PTO_ENABLE_POWER_DOWN_ENTRY(SocketBitMap, ChannelBitMap, Value) \
   DW_TO_DB(PLATFORM_TUNING_ENABLE_POWER_DOWN), 6, ANY_SOCKET, ANY_CHANNEL, (Value)
// Memory all clocks on
#define MAKE_PTO_ENABLE_TUNING_MEMORY_ALL_CLOCKS_ON_ENTRY(SocketBitMap, ChannelBitMap, Value) \
   DW_TO_DB(PLATFORM_TUNING_MEMORY_ALL_CLOCKS_ON), 6, ANY_SOCKET, ANY_CHANNEL, (Value)
// Power down mode
#define MAKE_PTO_ENABLE_POWER_DOWN_MODE_ENTRY(SocketBitMap, ChannelBitMap, Value) \
   DW_TO_DB(PLATFORM_TUNING_POWER_DOWN_MODE), 6, ANY_SOCKET, ANY_CHANNEL, (Value)
// Temperature controlled refresh enable
#define MAKE_PTO_TEMPERATURE_CONTROLLED_REFRESH_ENABLE_ENTRY(SocketBitMap, ChannelBitMap, Value) \
   DW_TO_DB(PLATFORM_TUNING_TEMPERATURE_CONTROLLED_REFRESH_ENABLE), 6, ANY_SOCKET, ANY_CHANNEL, (Value)

//
// Platform entrys
//
// Command Bus Timing
#define MAKE_PTO_CMD_BUS_TMG_ENTRY(SocketBitMap, ChannelBitMap, DimmPerCh, SpeedBitMap, Width, VddIo, Dimm0Rank, Dimm1Rank, SlowMode, AddrCmdCtrl, CkeStrength, CsOdtStrength, AddrCmdStrength, ClkStrength) \
   DW_TO_DB(PLATFORM_TUNING_CMD_BUS_TMG), 22, (SocketBitMap), (ChannelBitMap), (DimmPerCh), DD_TO_DB((SpeedBitMap)), (Width), (VddIo),\
   NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (SlowMode), DD_TO_DB((AddrCmdCtrl)), (CkeStrength), (CsOdtStrength), (AddrCmdStrength), (ClkStrength)
// Data Bus Timing
#define MAKE_PTO_DATA_BUS_TMG_ENTRY(SocketBitMap, ChannelBitMap, DimmPerCh, SpeedBitMap, Width, VddIo, Dimm0Rank, Dimm1Rank, RttNom, RttWr, RttPark, DqDrvStrength, DqsDrvStrength, OdtDrvStrength) \
  DW_TO_DB(PLATFORM_TUNING_DATA_BUS_TMG), 19, (SocketBitMap), (ChannelBitMap), (DimmPerCh), DD_TO_DB((SpeedBitMap)), (Width), (VddIo),\
  NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (RttNom), (RttWr), (RttPark), (DqDrvStrength), (DqsDrvStrength), (OdtDrvStrength)

//
// Memory Discovery entrys
//
// SPD Value Override
#define MAKE_PTO_SPD_VALUE_OVERRIDE_ENTRY(SocketBitMap, ChannelBitMap, DimmBitMap, Offset, Value) \
   DW_TO_DB(PLATFORM_TUNING_SPD_VALUE_OVERRIDE), 9, (SocketBitMap), (ChannelBitMap), (DimmBitMap), DW_TO_DB(Offset), (Value)
// SPD Crc Ignore
#define MAKE_PTO_SPD_CRC_IGNORE_ENTRY(SocketBitMap, ChannelBitMap, Dimm, Enable) \
   DW_TO_DB(PLATFORM_TUNING_SPD_CRC_IGNORE), 7, ANY_SOCKET, ANY_CHANNEL, ALL_DIMMS, (Enable)

//
// UMC Configuration entrys
//
// Gear Down Enable
#define MAKE_PTO_GEAR_DOWN_ENABLE_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Enable) \
   DW_TO_DB(PLATFORM_TUNING_GEAR_DOWN_EN), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Enable)
// Command 2T Enable
#define MAKE_PTO_CMD_2T_ENABLE_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Enable) \
   DW_TO_DB(PLATFORM_TUNING_CMD_2T_EN), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Enable)
// Wr preamble
#define MAKE_PTO_WR_PREAMBLE_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_WR_PREAMBLE), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// Rd preamble
#define MAKE_PTO_RD_PREAMBLE_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_RD_PREAMBLE), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// Odt pattern ctrl cs0
#define MAKE_PTO_ODT_PAT_CTRL_CS0_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value32) \
   DW_TO_DB(PLATFORM_TUNING_ODT_PAT_CTRL_CS0), 11, (SocketBitMap), (ChannelBitMap), 0xFF, NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DD_TO_DB((Value32))
// Odt pattern ctrl cs1
#define MAKE_PTO_ODT_PAT_CTRL_CS1_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value32) \
   DW_TO_DB(PLATFORM_TUNING_ODT_PAT_CTRL_CS1), 11, (SocketBitMap), (ChannelBitMap), 0xFF, NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DD_TO_DB((Value32))
// Odt pattern ctrl cs2
#define MAKE_PTO_ODT_PAT_CTRL_CS2_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value32) \
   DW_TO_DB(PLATFORM_TUNING_ODT_PAT_CTRL_CS2), 11, (SocketBitMap), (ChannelBitMap), 0xFF, NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DD_TO_DB((Value32))
// Odt pattern ctrl cs3
#define MAKE_PTO_ODT_PAT_CTRL_CS3_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value32) \
   DW_TO_DB(PLATFORM_TUNING_ODT_PAT_CTRL_CS3), 11, (SocketBitMap), (ChannelBitMap), 0xFF, NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DD_TO_DB((Value32))
// Tx eq impedance Dq
#define MAKE_PTO_TX_EQ_IMPEDANCE_DQ_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_TX_EQ_IMPEDANCE_DQ), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// Tx impedance Dq
#define MAKE_PTO_TX_IMPEDANCE_DQ_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_TX_IMPEDANCE_DQ), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// Tx eq hi impedance Dq
#define MAKE_PTO_TX_EQ_HI_IMPEDANCE_DQ_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_TX_EQ_HI_IMPEDANCE_DQ), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// Tx odt drive strength
#define MAKE_PTO_TX_ODT_DRV_STREN_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_TX_ODT_DRV_STREN), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// Trcdwr
#define MAKE_PTO_TRCDWR_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TRCDWR), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// Trcdrd
#define MAKE_PTO_TRCDRD_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TRCDRD), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// Tras
#define MAKE_PTO_TRAS_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TRAS), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// Tcl
#define MAKE_PTO_TCL_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TCL), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// Trp
#define MAKE_PTO_TRP_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TRP), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// Trtp
#define MAKE_PTO_TRTP_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TRTP), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// TrrdDlr
#define MAKE_PTO_TRRDDLR_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TRRDDLR), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// TrrdL
#define MAKE_PTO_TRRDL_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TRRDL), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// TrrdS
#define MAKE_PTO_TRRDS_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TRRDS), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// TfawDlr
#define MAKE_PTO_TFAWDLR_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TFAWDLR), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// TfawSlr
#define MAKE_PTO_TFAWSLR_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TFAWSLR), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// Tfaw
#define MAKE_PTO_TFAW_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TFAW), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// TwtrL
#define MAKE_PTO_TWTRL_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TWTRL), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// TwtrS
#define MAKE_PTO_TWTRS_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TWTRS), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// Tcwl
#define MAKE_PTO_TCWL_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TCWL), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// Twr
#define MAKE_PTO_TWR_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TWR), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// Trcpage
#define MAKE_PTO_TRCPAGE_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TRCPAGE), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// Trfc
#define MAKE_PTO_TRFC_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16InMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TRFC), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16InMemClk))
// Trfc2
#define MAKE_PTO_TRFC2_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16InMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TRFC2), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16InMemClk))
// Trfc4
#define MAKE_PTO_TRFC4_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16InMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TRFC4), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16InMemClk))
// Tstag
#define MAKE_PTO_TSTAG_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TSTAG), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// TstagLr
#define MAKE_PTO_TSTAGLR_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TSTAGLR), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// Tstag2Lr
#define MAKE_PTO_TSTAG2LR_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TSTAG2LR), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// Tstag4Lr
#define MAKE_PTO_TSTAG4LR_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TSTAG4LR), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// TrdrdBan
#define MAKE_PTO_TRDRDBAN_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TRDRDBAN), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// TrdrdScL
#define MAKE_PTO_TRDRDSCL_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TRDRDSCL), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// TrdrdScDlr
#define MAKE_PTO_TRDRDSCDLR_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TRDRDSCDLR), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// TrdrdSc
#define MAKE_PTO_TRDRDSC_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TRDRDSC), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// TrdrdSd
#define MAKE_PTO_TRDRDSD_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TRDRDSD), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// TrdrdDd
#define MAKE_PTO_TRDRDDD_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TRDRDDD), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// TwrwrBan
#define MAKE_PTO_TWRWRBAN_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TWRWRBAN), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// TwrwrScL
#define MAKE_PTO_TWRWRSCL_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TWRWRSCL), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// TwrwrScDlr
#define MAKE_PTO_TWRWRSCDLR_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TWRWRSCDLR), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// TwrwrSc
#define MAKE_PTO_TWRWRSC_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TWRWRSC), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// TwrwrSd
#define MAKE_PTO_TWRWRSD_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TWRWRSD), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// TwrwrDd
#define MAKE_PTO_TWRWRDD_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TWRWRDD), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// TwrrdScDlr
#define MAKE_PTO_TWRRDSCDLR_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TWRRDSCDLR), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// Trdwr
#define MAKE_PTO_TRDWR_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TRDWR), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// Twrrd
#define MAKE_PTO_TWRRD_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TWRRD), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
// DDR4 UECC retry
#define MAKE_PTO_DDR4_UECC_RETRY_ENTRY(SocketBitMap, ChannelBitMap, Value) \
   DW_TO_DB(PLATFORM_TUNING_DDR4_UECC_RETRY), 8, ANY_SOCKET, ANY_CHANNEL, 0xFF/*MemPstate*/, 0xFF/*NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)*/, (Value)

//
// PHY Configuration entrys
//
// Address Command Setup
#define MAKE_PTO_ADDR_CMD_SETUP_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_ADDR_CMD_SETUP), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// Cs ODT Setup
#define MAKE_PTO_CS_ODT_SETUP_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_CS_ODT_SETUP), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// Cke Setup
#define MAKE_PTO_CKE_SETUP_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_CKE_SETUP), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// Clk drive strength
#define MAKE_PTO_CLK_DRV_STREN_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_CLK_DRV_STREN), 8, (SocketBitMap), (ChannelBitMap), 0xFF, NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// Address command drive strength
#define MAKE_PTO_ADDR_CMD_DRV_STREN_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_ADDR_CMD_DRV_STREN), 8, (SocketBitMap), (ChannelBitMap), 0xFF, NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// Cs ODT drive strength
#define MAKE_PTO_CS_ODT_DRV_STREN_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_CS_ODT_DRV_STREN), 8, (SocketBitMap), (ChannelBitMap), 0xFF, NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// Cke drive strength
#define MAKE_PTO_CKE_DRV_STREN_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_CKE_DRV_STREN), 8, (SocketBitMap), (ChannelBitMap), 0xFF, NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// Rx enable delay
#define MAKE_PTO_RX_ENABLE_DELAY_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Operation, Dimm, Byte, Nibble, Value16) \
   DW_TO_DB(PLATFORM_TUNING_RX_ENABLE_DELAY), 14, (SocketBitMap), (ChannelBitMap), (MemPstate), 0xFF/*NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)*/,\
   (Operation), (Dimm), (Byte), (Nibble), 0xFF/*Dq*/, DW_TO_DB((Value16))
// Tx Dq delay
#define MAKE_PTO_TX_DQ_DELAY_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Operation, Dimm, Byte, Dq, Value16) \
   DW_TO_DB(PLATFORM_TUNING_TX_DQ_DELAY), 14, (SocketBitMap), (ChannelBitMap), (MemPstate), 0xFF/*NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)*/,\
   (Operation), (Dimm), (Byte), 0xFF/*Nibble*/, (Dq), DW_TO_DB((Value16))
// Tx Dqs delay
#define MAKE_PTO_TX_DQS_DELAY_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Operation, Dimm, Byte, Nibble, Value16) \
   DW_TO_DB(PLATFORM_TUNING_TX_DQS_DELAY), 14, (SocketBitMap), (ChannelBitMap), (MemPstate), 0xFF/*NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)*/,\
   (Operation), (Dimm), (Byte), (Nibble), 0xFF/*Dq*/, DW_TO_DB((Value16))
// Rx PB delay
#define MAKE_PTO_RX_PB_DELAY_ENTRY(SocketBitMap, ChannelBitMap, Operation, Dimm, Byte, Dq, Value16) \
   DW_TO_DB(PLATFORM_TUNING_RX_PB_DELAY), 14, (SocketBitMap), (ChannelBitMap), 0/*MemPstate*/, 0xFF/*NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)*/,\
   (Operation), (Dimm), (Byte), 0xFF/*Nibble*/, (Dq), DW_TO_DB((Value16))
// Rx clk delay
#define MAKE_PTO_RX_CLK_DELAY_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Operation, Dimm, Byte, Nibble, Value16) \
   DW_TO_DB(PLATFORM_TUNING_RX_CLK_DELAY), 14, (SocketBitMap), (ChannelBitMap), (MemPstate), 0xFF/*NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)*/,\
   (Operation), (Dimm), (Byte), (Nibble), 0xFF/*Dq*/, DW_TO_DB((Value16))
// Vref DAC0
#define MAKE_PTO_VREF_DAC_0_ENTRY(SocketBitMap, ChannelBitMap, Operation, Byte, Dq, Value16) \
   DW_TO_DB(PLATFORM_TUNING_VREF_DAC0), 14, (SocketBitMap), (ChannelBitMap), 0/*MemPstate*/, 0xFF/*NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)*/,\
   (Operation), 0xFF/*Dimm*/, (Byte), 0xFF/*Nibble*/, (Dq), DW_TO_DB((Value16))
// Vref DAC1
#define MAKE_PTO_VREF_DAC_1_ENTRY(SocketBitMap, ChannelBitMap, Operation, Byte, Dq, Value16) \
   DW_TO_DB(PLATFORM_TUNING_VREF_DAC1), 14, (SocketBitMap), (ChannelBitMap), 0/*MemPstate*/, 0xFF/*NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)*/,\
   (Operation), 0xFF/*Dimm*/, (Byte), 0xFF/*Nibble*/, (Dq), DW_TO_DB((Value16))
// Vref In Global
#define MAKE_PTO_VREF_IN_GLOBAL_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Operation, Value16) \
   DW_TO_DB(PLATFORM_TUNING_VREF_IN_GLOBAL), 14, (SocketBitMap), (ChannelBitMap), (MemPstate), 0xFF/*NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)*/,\
   (Operation), 0xFF/*Dimm*/, 0xFF/*Byte*/, 0xFF/*Nibble*/, 0xFF/*Dq*/, DW_TO_DB((Value16))
// DqDqsRcvCtrl
#define MAKE_PTO_DQ_DQS_RCV_CTRL_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Byte, Nibble, Value16) \
   DW_TO_DB(PLATFORM_TUNING_DQ_DQS_RCV_CTRL), 14, (SocketBitMap), (ChannelBitMap), (MemPstate), 0xFF/*NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)*/,\
   OP_OVERRIDE, 0xFF/*Dimm*/, (Byte), (Nibble), 0xFF/*Dq*/, DW_TO_DB((Value16))

//
// 1D training input entrys
//
// 1D Train MR0
#define MAKE_PTO_1D_TRAIN_MR0_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_MR0), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
// 1D Train MR1
#define MAKE_PTO_1D_TRAIN_MR1_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_MR1), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
// 1D Train MR2
#define MAKE_PTO_1D_TRAIN_MR2_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_MR2), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
// 1D Train MR3
#define MAKE_PTO_1D_TRAIN_MR3_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_MR3), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
// 1D Train MR4
#define MAKE_PTO_1D_TRAIN_MR4_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_MR4), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
// 1D Train MR5
#define MAKE_PTO_1D_TRAIN_MR5_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_MR5), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
// 1D Train MR6
#define MAKE_PTO_1D_TRAIN_MR6_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_MR6), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
// 1D Train Phy Vref
#define MAKE_PTO_1D_TRAIN_PHY_VREF_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_PHY_VREF), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// 1D Train Sequence Control
#define MAKE_PTO_1D_TRAIN_SEQUENCE_CTRL_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_SEQUENCE_CTRL), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
// 1D Train DFI MRL Margin
#define MAKE_PTO_1D_TRAIN_DFI_MRL_MARGIN_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_DFI_MRL_MARGIN), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// 1D Custom Message
#define MAKE_PTO_1D_CUSTOM_MSG_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Offset, Value) \
   DW_TO_DB(PLATFORM_TUNING_1D_CUSTOM_MSG), 10, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Offset)),\
   (Value)
// 1D Train AcsmOdtCtrl0
#define MAKE_PTO_1D_TRAIN_ACSM_ODT_CTRL0_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL0), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// 1D Train AcsmOdtCtrl1
#define MAKE_PTO_1D_TRAIN_ACSM_ODT_CTRL1_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL1), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// 1D Train AcsmOdtCtrl2
#define MAKE_PTO_1D_TRAIN_ACSM_ODT_CTRL2_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL2), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// 1D Train AcsmOdtCtrl3
#define MAKE_PTO_1D_TRAIN_ACSM_ODT_CTRL3_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL3), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// 1D Train AcsmOdtCtrl4
#define MAKE_PTO_1D_TRAIN_ACSM_ODT_CTRL4_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL4), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// 1D Train AcsmOdtCtrl5
#define MAKE_PTO_1D_TRAIN_ACSM_ODT_CTRL5_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL5), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// 1D Train AcsmOdtCtrl6
#define MAKE_PTO_1D_TRAIN_ACSM_ODT_CTRL6_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL6), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// 1D Train AcsmOdtCtrl7
#define MAKE_PTO_1D_TRAIN_ACSM_ODT_CTRL7_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL7), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// 1D Train ALT_WCAS_L
#define MAKE_PTO_1D_TRAIN_ALT_WCAS_L_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_ALT_WCAS_L), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)

//
// 2D training input entrys
//
// 2D Train MR0
#define MAKE_PTO_2D_TRAIN_MR0_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_MR0), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
// 2D Train MR1
#define MAKE_PTO_2D_TRAIN_MR1_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_MR1), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
// 2D Train MR2
#define MAKE_PTO_2D_TRAIN_MR2_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_MR2), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
// 2D Train MR3
#define MAKE_PTO_2D_TRAIN_MR3_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_MR3), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
// 2D Train MR4
#define MAKE_PTO_2D_TRAIN_MR4_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_MR4), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
// 2D Train MR5
#define MAKE_PTO_2D_TRAIN_MR5_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_MR5), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
// 2D Train MR6
#define MAKE_PTO_2D_TRAIN_MR6_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_MR6), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
// 2D Train Phy Vref
#define MAKE_PTO_2D_TRAIN_PHY_VREF_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_PHY_VREF), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// 2D Train Sequence Control
#define MAKE_PTO_2D_TRAIN_SEQUENCE_CTRL_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_SEQUENCE_CTRL), 9,(SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
// 2D Train DFI MRL Margin
#define MAKE_PTO_2D_TRAIN_DFI_MRL_MARGIN_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_DFI_MRL_MARGIN), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// 2D Train RX2D Train Opt
#define MAKE_PTO_2D_TRAIN_RX2D_TRAIN_OPT_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_RX2D_TRAIN_OPT), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// 2D Train TX2D Train Opt
#define MAKE_PTO_2D_TRAIN_TX2D_TRAIN_OPT_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_TX2D_TRAIN_OPT), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// 2D Custom Message
#define MAKE_PTO_2D_CUSTOM_MSG_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Offset, Value) \
   DW_TO_DB(PLATFORM_TUNING_2D_CUSTOM_MSG), 10, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Offset)),\
   (Value)
// 2D Train AcsmOdtCtrl0
#define MAKE_PTO_2D_TRAIN_ACSM_ODT_CTRL0_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL0), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// 2D Train AcsmOdtCtrl1
#define MAKE_PTO_2D_TRAIN_ACSM_ODT_CTRL1_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL1), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// 2D Train AcsmOdtCtrl2
#define MAKE_PTO_2D_TRAIN_ACSM_ODT_CTRL2_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL2), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// 2D Train AcsmOdtCtrl3
#define MAKE_PTO_2D_TRAIN_ACSM_ODT_CTRL3_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL3), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// 2D Train AcsmOdtCtrl4
#define MAKE_PTO_2D_TRAIN_ACSM_ODT_CTRL4_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL4), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// 2D Train AcsmOdtCtrl5
#define MAKE_PTO_2D_TRAIN_ACSM_ODT_CTRL5_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL5), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// 2D Train AcsmOdtCtrl6
#define MAKE_PTO_2D_TRAIN_ACSM_ODT_CTRL6_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL6), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// 2D Train AcsmOdtCtrl7
#define MAKE_PTO_2D_TRAIN_ACSM_ODT_CTRL7_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL7), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
// 2D Train ALT_WCAS_L
#define MAKE_PTO_2D_TRAIN_ALT_WCAS_L_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_ALT_WCAS_L), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)

//
// Validity macros
//
// DramManufacturersIDCode
#define MAKE_PTO_VALIDITY_DRAM_MFG_ID_ENTRY(SocketBitMap, ChannelBitMap, DramMfgId) \
   DW_TO_DB(PLATFORM_TUNING_VALIDITY_DRAM_MFG_ID), 7, (SocketBitMap), (ChannelBitMap), DW_TO_DB((DramMfgId))
// ModuleManufacturersIDCode
#define MAKE_PTO_VALIDITY_MODULE_MFG_ID_ENTRY(SocketBitMap, ChannelBitMap, ModuleMfgId) \
   DW_TO_DB(PLATFORM_TUNING_VALIDITY_MODULE_MFG_ID), 7, (SocketBitMap), (ChannelBitMap), DW_TO_DB((ModuleMfgId))
// DramStepping
#define MAKE_PTO_VALIDITY_DRAM_STEPPING_ENTRY(SocketBitMap, ChannelBitMap, DramStepping) \
   DW_TO_DB(PLATFORM_TUNING_VALIDITY_DRAM_STEPPING), 7, (SocketBitMap), (ChannelBitMap), DW_TO_DB((DramStepping))
// Vality Check End
#define MAKE_PTO_VALIDITY_END() \
   DW_TO_DB(PLATFORM_TUNING_VALIDITY_TERMINATOR), 3

//
// PLATFORM TUNING C STRUCTURE DEFINITIONS
//

//
// Enablement entrys
//
// Chipsel Interleave
//#define MAKE_PTO_CHIPSEL_INTLV_ENTRY(SocketBitMap, ChannelBitMap, Enable) \
//  DW_TO_DB(PLATFORM_TUNING_CHIPSEL_INTLV), 6, ANY_SOCKET, ANY_CHANNEL, (Enable)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   Enable;      ///< Enable
} PTO_CHIPSEL_INTLV_ENTRY;

// Ecc
//#define MAKE_PTO_ECC_ENTRY(SocketBitMap, ChannelBitMap, Enable) \
//   DW_TO_DB(PLATFORM_TUNING_ECC), 6, ANY_SOCKET, ANY_CHANNEL, (Enable)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   Enable;      ///< Enable
} PTO_ECC_ENTRY;

// Parity
//#define MAKE_PTO_PARITY_ENTRY(SocketBitMap, ChannelBitMap, Enable) \
//   DW_TO_DB(PLATFORM_TUNING_PARITY), 6, ANY_SOCKET, ANY_CHANNEL, (Enable)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   Enable;      ///< Enable
} PTO_PARITY_ENTRY;

// Mem Bus Limit
//#define MAKE_PTO_MEM_BUS_LIMIT_ENTRY(SocketBitMap, ChannelBitMap, Speed) \
//   DW_TO_DB(PLATFORM_TUNING_MEM_BUS_LIMIT), 7, ANY_SOCKET, ANY_CHANNEL, DW_TO_DB((Speed))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT16   Speed;      ///< Speed
} PTO_MEM_BUS_LIMIT_ENTRY;

// Mbist Enable
//#define MAKE_PTO_MBIST_ENABLE_ENTRY(SocketBitMap, ChannelBitMap, Enable) \
//   DW_TO_DB(PLATFORM_TUNING_MBIST_ENABLE), 6, ANY_SOCKET, ANY_CHANNEL, (Enable)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   Enable;      ///< Enable
} PTO_MBIST_ENABLE_ENTRY;

// Mbist Test Mode
//#define MAKE_PTO_MBIST_TEST_MODE_ENTRY(SocketBitMap, ChannelBitMap, Mode) \
//   DW_TO_DB(PLATFORM_TUNING_MBIST_TEST_MODE), 6, ANY_SOCKET, ANY_CHANNEL, (Mode)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   Mode;      ///< Mode
} PTO_MBIST_TEST_MODE_ENTRY;

// Mbist Aggressor On
//#define MAKE_PTO_MBIST_AGGRESSOR_ON_ENTRY(SocketBitMap, ChannelBitMap, Enable) \
//   DW_TO_DB(PLATFORM_TUNING_MBIST_AGGRESSOR_ON), 6, ANY_SOCKET, ANY_CHANNEL, (Enable)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   Enable;      ///< Enable
} PTO_MBIST_AGGRESSOR_ON_ENTRY;

// Mbist Per Bit Slave Die Report
//#define MAKE_PTO_MBIST_PER_BIT_SLAVE_DIE_REPORT_ENTRY(SocketBitMap, ChannelBitMap, Value8) \
//   DW_TO_DB(PLATFORM_TUNING_MBIST_PER_BIT_SLAVE_DIE_REPORT), 6, ANY_SOCKET, (ChannelBitMap), (Value8)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   Value8;      ///< Value8
} PTO_MBIST_PER_BIT_SLAVE_DIE_REPORT_ENTRY;

// Post Package Repair Enable
//#define MAKE_PTO_POST_PACKAGE_REPAIR_ENABLE_ENTRY(SocketBitMap, ChannelBitMap, Enable) \
//   DW_TO_DB(PLATFORM_TUNING_POST_PACKAGE_REPAIR_ENABLE), 6, ANY_SOCKET, ANY_CHANNEL, (Enable)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   Enable;      ///< Enable
} PTO_POST_PACKAGE_REPAIR_ENABLE_ENTRY;

// Post Package Repair All Banks
//#define MAKE_PTO_POST_PACKAGE_REPAIR_ALL_BANKS_ENTRY(SocketBitMap, ChannelBitMap, Enable) \
//   DW_TO_DB(PLATFORM_TUNING_POST_PACKAGE_REPAIR_ALL_BANKS), 6, ANY_SOCKET, ANY_CHANNEL, (Enable)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   Enable;      ///< Enable
} PTO_POST_PACKAGE_REPAIR_ALL_BANKS_ENTRY;

//
// Performance entrys
//
// Bank Group Swap
//#define MAKE_PTO_BANK_GROUP_SWAP_ENTRY(SocketBitMap, ChannelBitMap, Enable) \
//   DW_TO_DB(PLATFORM_TUNING_BANK_GROUP_SWAP), 6, ANY_SOCKET, ANY_CHANNEL, (Enable)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   Enable;      ///< Enable
} PTO_BANK_GROUP_SWAP_ENTRY;

// Odts Command Throttle Enable
//#define MAKE_PTO_ODTS_CMD_THROTTLE_ENABLE_ENTRY(SocketBitMap, ChannelBitMap, Enable) \
//   DW_TO_DB(PLATFORM_TUNING_ODTS_CMD_THROTTLE_ENABLE), 6, ANY_SOCKET, ANY_CHANNEL, (Enable)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   Enable;      ///< Enable
} PTO_ODTS_CMD_THROTTLE_ENABLE_ENTRY;

// Auto Reffine Gran Mode
//#define MAKE_PTO_AUTO_REFFINE_GRAN_MODE_ENTRY(SocketBitMap, ChannelBitMap, Value) \
//   DW_TO_DB(PLATFORM_TUNING_AUTO_REFFINE_GRAN_MODE), 6, ANY_SOCKET, ANY_CHANNEL, (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   Value;      ///< Value
} PTO_AUTO_REFFINE_GRAN_MODE_ENTRY;

// Address Command Parity Retry
//#define MAKE_PTO_ADDR_CMD_PARITY_RETRY_ENTRY(SocketBitMap, ChannelBitMap, Enable) \
//   DW_TO_DB(PLATFORM_TUNING_ADDR_CMD_PARITY_RETRY), 6, ANY_SOCKET, ANY_CHANNEL, (Enable)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   Enable;      ///< Enable
} PTO_ADDR_CMD_PARITY_RETRY_ENTRY;

// Rcd Parity
//#define MAKE_PTO_RCD_PARITY_ENABLE_ENTRY(SocketBitMap, ChannelBitMap, Enable) \
//   DW_TO_DB(PLATFORM_TUNING_RCD_PARITY_ENABLE), 6, ANY_SOCKET, ANY_CHANNEL, (Enable)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   Enable;      ///< Enable
} PTO_RCD_PARITY_ENABLE_ENTRY;

// WrCrc Enable
//#define MAKE_PTO_WR_CRC_ENABLE_ENTRY(SocketBitMap, ChannelBitMap, Enable) \
//   DW_TO_DB(PLATFORM_TUNING_WR_CRC_ENABLE), 6, ANY_SOCKET, ANY_CHANNEL, (Enable)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   Enable;      ///< Enable
} PTO_WR_CRC_ENABLE_ENTRY;

// WrCrc Retry
//#define MAKE_PTO_WR_CRC_RETRY_ENTRY(SocketBitMap, ChannelBitMap, Enable) \
//   DW_TO_DB(PLATFORM_TUNING_WR_CRC_RETRY), 6, ANY_SOCKET, ANY_CHANNEL, (Enable)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   Enable;      ///< Enable
} PTO_WR_CRC_RETRY_ENTRY;

// Ecc symbol size
//#define MAKE_PTO_ECC_SYMBOL_SIZE_ENTRY(SocketBitMap, ChannelBitMap, Value) \
//   DW_TO_DB(PLATFORM_TUNING_ECC_SYMBOL_SIZE), 6, ANY_SOCKET, ANY_CHANNEL, (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   Value;      ///< Value
} PTO_ECC_SYMBOL_SIZE_ENTRY;

//
// Power entrys
//
// Enable power down
//#define MAKE_PTO_ENABLE_POWER_DOWN_ENTRY(SocketBitMap, ChannelBitMap, Value) \
//   DW_TO_DB(PLATFORM_TUNING_ENABLE_POWER_DOWN), 6, ANY_SOCKET, ANY_CHANNEL, (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   Value;      ///< Value
} PTO_ENABLE_POWER_DOWN_ENTRY;

// Memory all clocks on
//#define MAKE_PTO_ENABLE_TUNING_MEMORY_ALL_CLOCKS_ON_ENTRY(SocketBitMap, ChannelBitMap, Value) \
//   DW_TO_DB(PLATFORM_TUNING_MEMORY_ALL_CLOCKS_ON), 6, ANY_SOCKET, ANY_CHANNEL, (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   Value;      ///< Value
} PTO_ENABLE_TUNING_MEMORY_ALL_CLOCKS_ON_ENTRY;

// Power down mode
//#define MAKE_PTO_ENABLE_POWER_DOWN_MODE_ENTRY(SocketBitMap, ChannelBitMap, Value) \
//   DW_TO_DB(PLATFORM_TUNING_POWER_DOWN_MODE), 6, ANY_SOCKET, ANY_CHANNEL, (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   Value;      ///< Value
} PTO_ENABLE_POWER_DOWN_MODE_ENTRY;

// Temperature controlled refresh enable
//#define MAKE_PTO_TEMPERATURE_CONTROLLED_REFRESH_ENABLE_ENTRY(SocketBitMap, ChannelBitMap, Value) \
//   DW_TO_DB(PLATFORM_TUNING_TEMPERATURE_CONTROLLED_REFRESH_ENABLE), 6, ANY_SOCKET, ANY_CHANNEL, (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   Value;      ///< Value
} PTO_TEMPERATURE_CONTROLLED_REFRESH_ENABLE_ENTRY;

//
// Platform entrys
//
// Command Bus Timing
//#define MAKE_PTO_CMD_BUS_TMG_ENTRY(SocketBitMap, ChannelBitMap, DimmPerCh, SpeedBitMap, Width, VddIo, Dimm0Rank, Dimm1Rank, SlowMode, AddrCmdCtrl, CkeStrength, CsOdtStrength, AddrCmdStrength, ClkStrength) \
//   DW_TO_DB(PLATFORM_TUNING_CMD_BUS_TMG), 22, (SocketBitMap), (ChannelBitMap), (DimmPerCh), DD_TO_DB((SpeedBitMap)), (Width), (VddIo),\
//   NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (SlowMode), DD_TO_DB((AddrCmdCtrl)), (CkeStrength), (CsOdtStrength), (AddrCmdStrength), (ClkStrength)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   DimmPerCh;     ///< DimmPerCh
  UINT32 SpeedBitMap;    ///< SpeedBitMap
  UINT8   Width;              ///< Width
  UINT8   VddIo;              ///< VddIo
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   SlowMode;       ///< SlowMode
  UINT32 AddrCmdCtrl;   ///< AddrCmdCtrl
  UINT8   CkeStrength;   ///< CkeStrength
  UINT8   CsOdtStrength;   ///< CsOdtStrength
  UINT8   AddrCmdStrength;   ///< AddrCmdStrength
  UINT8   ClkStrength;   ///< ClkStrength
} PTO_CMD_BUS_TMG_ENTRY;

// Data Bus Timing
//#define MAKE_PTO_DATA_BUS_TMG_ENTRY(SocketBitMap, ChannelBitMap, DimmPerCh, SpeedBitMap, Width, VddIo, Dimm0Rank, Dimm1Rank, RttNom, RttWr, RttPark, DqDrvStrength, DqsDrvStrength, OdtDrvStrength) \
//  DW_TO_DB(PLATFORM_TUNING_DATA_BUS_TMG), 19, (SocketBitMap), (ChannelBitMap), (DimmPerCh), DD_TO_DB((SpeedBitMap)), (Width), (VddIo),\
//  NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (RttNom), (RttWr), (RttPark), (DqDrvStrength), (DqsDrvStrength), (OdtDrvStrength)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   DimmPerCh;     ///< DimmPerCh
  UINT32 SpeedBitMap;    ///< SpeedBitMap
  UINT8   Width;              ///< Width
  UINT8   VddIo;              ///< VddIo
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   RttNom;       ///< RttNom
  UINT8   RttWr;         ///< RttWr
  UINT8   RttPark;      ///< RttPark
  UINT8   DqDrvStrength;   ///< DqDrvStrength
  UINT8   DqsDrvStrength;   ///< DqsDrvStrength
  UINT8   OdtDrvStrength;   ///< OdtDrvStrength
} PTO_DATA_BUS_TMG_ENTRY;

//
// Memory Discovery entrys
//
// SPD Value Override
//#define MAKE_PTO_SPD_VALUE_OVERRIDE_ENTRY(SocketBitMap, ChannelBitMap, Dimm, Offset, Value) \
//   DW_TO_DB(PLATFORM_TUNING_SPD_VALUE_OVERRIDE), 9, (SocketBitMap), (ChannelBitMap), (Dimm), DW_TO_DB(Offset), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   Dimm;              ///< Dimm
  UINT16 Offset;              ///< Offset
  UINT8   Value;              ///< Value
} PTO_SPD_VALUE_OVERRIDE_ENTRY;

// SPD Crc Ignore
//#define MAKE_PTO_SPD_CRC_IGNORE_ENTRY(SocketBitMap, ChannelBitMap, Dimm, Enable) \
//   DW_TO_DB(PLATFORM_TUNING_SPD_CRC_IGNORE), 7, ANY_SOCKET, ANY_CHANNEL, ALL_DIMMS, (Enable)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   Dimm;              ///< Dimm
  UINT8   Enable;              ///< Enable
} PTO_SPD_CRC_IGNORE_ENTRY;

//
// UMC Configuration entrys
//
// Gear Down Enable
//#define MAKE_PTO_GEAR_DOWN_ENABLE_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Enable) \
//   DW_TO_DB(PLATFORM_TUNING_GEAR_DOWN_EN), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Enable)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Enable;              ///< Enable
} PTO_GEAR_DOWN_ENABLE_ENTRY;

// Command 2T Enable
//#define MAKE_PTO_CMD_2T_ENABLE_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Enable) \
//   DW_TO_DB(PLATFORM_TUNING_CMD_2T_EN), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Enable)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Enable;              ///< Enable
} PTO_CMD_2T_ENABLE_ENTRY;

// Wr preamble
//#define MAKE_PTO_WR_PREAMBLE_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_WR_PREAMBLE), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;              ///< Value
} PTO_WR_PREAMBLE_ENTRY;

// Rd preamble
#define MAKE_PTO_RD_PREAMBLE_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
   DW_TO_DB(PLATFORM_TUNING_RD_PREAMBLE), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;              ///< Value
} PTO_RD_PREAMBLE_ENTRY;

// Odt pattern ctrl cs0
//#define MAKE_PTO_ODT_PAT_CTRL_CS0_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value32) \
//   DW_TO_DB(PLATFORM_TUNING_ODT_PAT_CTRL_CS0), 11, (SocketBitMap), (ChannelBitMap), 0xFF, NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DD_TO_DB((Value32))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT32   Value32;              ///< Value32
} PTO_ODT_PAT_CTRL_CS0_ENTRY;

// Odt pattern ctrl cs1
//#define MAKE_PTO_ODT_PAT_CTRL_CS1_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value32) \
//   DW_TO_DB(PLATFORM_TUNING_ODT_PAT_CTRL_CS1), 11, (SocketBitMap), (ChannelBitMap), 0xFF, NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DD_TO_DB((Value32))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT32   Value32;              ///< Value32
} PTO_ODT_PAT_CTRL_CS1_ENTRY;

// Odt pattern ctrl cs2
//#define MAKE_PTO_ODT_PAT_CTRL_CS2_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value32) \
//   DW_TO_DB(PLATFORM_TUNING_ODT_PAT_CTRL_CS2), 11, (SocketBitMap), (ChannelBitMap), 0xFF, NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DD_TO_DB((Value32))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT32   Value32;              ///< Value32
} PTO_ODT_PAT_CTRL_CS2_ENTRY;

// Odt pattern ctrl cs3
//#define MAKE_PTO_ODT_PAT_CTRL_CS3_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value32) \
//   DW_TO_DB(PLATFORM_TUNING_ODT_PAT_CTRL_CS3), 11, (SocketBitMap), (ChannelBitMap), 0xFF, NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DD_TO_DB((Value32))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT32   Value32;         ///< Value32
} PTO_ODT_PAT_CTRL_CS3_ENTRY;

// Tx eq impedance Dq
//#define MAKE_PTO_TX_EQ_IMPEDANCE_DQ_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_TX_EQ_IMPEDANCE_DQ), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;         ///< Value
} PTO_TX_EQ_IMPEDANCE_DQ_ENTRY;

// Tx odt drive strength
//#define MAKE_PTO_TX_ODT_DRV_STREN_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_TX_ODT_DRV_STREN), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;         ///< Value
} PTO_TX_ODT_DRV_STREN_ENTRY;

// Trcdwr
//#define MAKE_PTO_TRCDWR_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TRCDWR), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TRCDWR_ENTRY;

// Trcdrd
//#define MAKE_PTO_TRCDRD_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TRCDRD), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TRCDRD_ENTRY;

// Tras
//#define MAKE_PTO_TRAS_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TRAS), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TRAS_ENTRY;

// Tcl
//#define MAKE_PTO_TCL_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TCL), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TCL_ENTRY;

// Trp
//#define MAKE_PTO_TRP_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TRP), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TRP_ENTRY;

// Trtp
//#define MAKE_PTO_TRTP_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TRTP), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TRTP_ENTRY;

// TrrdDlr
//#define MAKE_PTO_TRRDDLR_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TRRDDLR), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TRRDDLR_ENTRY;

// TrrdL
//#define MAKE_PTO_TRRDL_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TRRDL), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TRRDL_ENTRY;

// TrrdS
//#define MAKE_PTO_TRRDS_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TRRDS), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TRRDS_ENTRY;

// TfawDlr
//#define MAKE_PTO_TFAWDLR_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TFAWDLR), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TFAWDLR_ENTRY;

// TfawSlr
//#define MAKE_PTO_TFAWSLR_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TFAWSLR), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TFAWSLR_ENTRY;

// Tfaw
//#define MAKE_PTO_TFAW_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TFAW), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TFAW_ENTRY;

// TwtrL
//#define MAKE_PTO_TWTRL_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TWTRL), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TWTRL_ENTRY;

// TwtrS
//#define MAKE_PTO_TWTRS_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TWTRS), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TWTRS_ENTRY;

// Tcwl
//#define MAKE_PTO_TCWL_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TCWL), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TCWL_ENTRY;

// Twr
//#define MAKE_PTO_TWR_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TWR), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TWR_ENTRY;

// Trcpage
//#define MAKE_PTO_TRCPAGE_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TRCPAGE), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TRCPAGE_ENTRY;

// Trfc
//#define MAKE_PTO_TRFC_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16InMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TRFC), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16InMemClk))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT16   Value16InMemClk;  ///< Value16InMemClk
} PTO_TRFC_ENTRY;

// Trfc2
//#define MAKE_PTO_TRFC2_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16InMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TRFC2), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16InMemClk))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT16   Value16InMemClk;  ///< Value16InMemClk
} PTO_TRFC2_ENTRY;

// Trfc4
//#define MAKE_PTO_TRFC4_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16InMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TRFC4), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16InMemClk))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT16   Value16InMemClk;  ///< Value16InMemClk
} PTO_TRFC4_ENTRY;

// Tstag
//#define MAKE_PTO_TSTAG_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TSTAG), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TSTAG_ENTRY;

// TstagLr
//#define MAKE_PTO_TSTAGLR_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TSTAGLR), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TSTAGLR_ENTRY;

// Tstag2Lr
//#define MAKE_PTO_TSTAG2LR_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TSTAG2LR), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TSTAG2LR_ENTRY;

// Tstag4Lr
//#define MAKE_PTO_TSTAG4LR_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TSTAG4LR), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TSTAG4LR_ENTRY;

// TrdrdBan
//#define MAKE_PTO_TRDRDBAN_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TRDRDBAN), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TRDRDBAN_ENTRY;

// TrdrdScL
//#define MAKE_PTO_TRDRDSCL_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TRDRDSCL), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TRDRDSCL_ENTRY;

// TrdrdScDlr
//#define MAKE_PTO_TRDRDSCDLR_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TRDRDSCDLR), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TRDRDSCDLR_ENTRY;

// TrdrdSc
//#define MAKE_PTO_TRDRDSC_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TRDRDSC), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TRDRDSC_ENTRY;

// TrdrdSd
//#define MAKE_PTO_TRDRDSD_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TRDRDSD), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TRDRDSD_ENTRY;

// TrdrdDd
//#define MAKE_PTO_TRDRDDD_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TRDRDDD), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TRDRDDD_ENTRY;

// TwrwrBan
//#define MAKE_PTO_TWRWRBAN_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TWRWRBAN), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TWRWRBAN_ENTRY;

// TwrwrScL
//#define MAKE_PTO_TWRWRSCL_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TWRWRSCL), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TWRWRSCL_ENTRY;

// TwrwrScDlr
//#define MAKE_PTO_TWRWRSCDLR_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TWRWRSCDLR), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TWRWRSCDLR_ENTRY;

// TwrwrSc
//#define MAKE_PTO_TWRWRSC_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TWRWRSC), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TWRWRSC_ENTRY;

// TwrwrSd
//#define MAKE_PTO_TWRWRSD_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TWRWRSD), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TWRWRSD_ENTRY;

// TwrwrDd
//#define MAKE_PTO_TWRWRDD_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TWRWRDD), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TWRWRDD_ENTRY;

// TwrrdScDlr
//#define MAKE_PTO_TWRRDSCDLR_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TWRRDSCDLR), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TWRRDSCDLR_ENTRY;

// Trdwr
//#define MAKE_PTO_TRDWR_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
//   DW_TO_DB(PLATFORM_TUNING_TRDWR), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TRDWR_ENTRY;

// Twrrd
#define MAKE_PTO_TWRRD_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, ValueInMemClk) \
   DW_TO_DB(PLATFORM_TUNING_TWRRD), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (ValueInMemClk)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   ValueInMemClk;  ///< ValueInMemClk
} PTO_TWRRD_ENTRY;

// DDR4 UECC retry
//#define MAKE_PTO_DDR4_UECC_RETRY_ENTRY(SocketBitMap, ChannelBitMap, Value) \
//   DW_TO_DB(PLATFORM_TUNING_DDR4_UECC_RETRY), 8, ANY_SOCKET, ANY_CHANNEL, 0xFF/*MemPstate*/, 0xFF/*NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)*/, (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_DDR4_UECC_RETRY_ENTRY;

//
// PHY Configuration entrys
//
// Address Command Setup
//#define MAKE_PTO_ADDR_CMD_SETUP_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_ADDR_CMD_SETUP), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_ADDR_CMD_SETUP_ENTRY;

// Cs ODT Setup
//#define MAKE_PTO_CS_ODT_SETUP_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_CS_ODT_SETUP), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_CS_ODT_SETUP_ENTRY;

// Cke Setup
//#define MAKE_PTO_CKE_SETUP_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_CKE_SETUP), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_CKE_SETUP_ENTRY;

// Clk drive strength
//#define MAKE_PTO_CLK_DRV_STREN_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_CLK_DRV_STREN), 8, (SocketBitMap), (ChannelBitMap), 0xFF, NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_CLK_DRV_STREN_ENTRY;

// Address command drive strength
//#define MAKE_PTO_ADDR_CMD_DRV_STREN_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_ADDR_CMD_DRV_STREN), 8, (SocketBitMap), (ChannelBitMap), 0xFF, NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_ADDR_CMD_DRV_STREN_ENTRY;

// Cs ODT drive strength
//#define MAKE_PTO_CS_ODT_DRV_STREN_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_CS_ODT_DRV_STREN), 8, (SocketBitMap), (ChannelBitMap), 0xFF, NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_CS_ODT_DRV_STREN_ENTRY;

// Cke drive strength
//#define MAKE_PTO_CKE_DRV_STREN_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_CKE_DRV_STREN), 8, (SocketBitMap), (ChannelBitMap), 0xFF, NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_CKE_DRV_STREN_ENTRY;

// Rx enable delay
//#define MAKE_PTO_RX_ENABLE_DELAY_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Operation, Dimm, Byte, Nibble, Value16) \
//   DW_TO_DB(PLATFORM_TUNING_RX_ENABLE_DELAY), 14, (SocketBitMap), (ChannelBitMap), (MemPstate), 0xFF/*NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)*/,\
//   (Operation), (Dimm), (Byte), (Nibble), 0xFF/*Dq*/, DW_TO_DB((Value16))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Operation;         ///< Operation
  UINT8   Dimm;              ///< Dimm
  UINT8   Byte;                ///< Byte
  UINT8   Nibble;              ///< Nibble
  UINT8   Dq;                   ///< Dq
  UINT16   Value16;         ///< Value16
} PTO_RX_ENABLE_DELAY_ENTRY;

// Tx Dq delay
//#define MAKE_PTO_TX_DQ_DELAY_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Operation, Dimm, Byte, Dq, Value16) \
//   DW_TO_DB(PLATFORM_TUNING_TX_DQ_DELAY), 14, (SocketBitMap), (ChannelBitMap), (MemPstate), 0xFF/*NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)*/,\
//   (Operation), (Dimm), (Byte), 0xFF/*Nibble*/, (Dq), DW_TO_DB((Value16))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Operation;         ///< Operation
  UINT8   Dimm;              ///< Dimm
  UINT8   Byte;                ///< Byte
  UINT8   Nibble;              ///< Nibble
  UINT8   Dq;                   ///< Dq
  UINT16   Value16;         ///< Value16
} PTO_TX_DQ_DELAY_ENTRY;

// Rx PB delay
//#define MAKE_PTO_RX_PB_DELAY_ENTRY(SocketBitMap, ChannelBitMap, Operation, Dimm, Byte, Dq, Value16) \
//   DW_TO_DB(PLATFORM_TUNING_RX_PB_DELAY), 14, (SocketBitMap), (ChannelBitMap), 0/*MemPstate*/, 0xFF/*NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)*/,\
//   (Operation), (Dimm), (Byte), 0xFF/*Nibble*/, (Dq), DW_TO_DB((Value16))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Operation;         ///< Operation
  UINT8   Dimm;              ///< Dimm
  UINT8   Byte;                ///< Byte
  UINT8   Nibble;              ///< Nibble
  UINT8   Dq;                   ///< Dq
  UINT16   Value16;         ///< Value16
} PTO_RX_PB_DELAY_ENTRY;

// Rx clk delay
//#define MAKE_PTO_RX_CLK_DELAY_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Operation, Dimm, Byte, Nibble, Value16) \
//   DW_TO_DB(PLATFORM_TUNING_RX_CLK_DELAY), 14, (SocketBitMap), (ChannelBitMap), (MemPstate), 0xFF/*NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)*/,\
//   (Operation), (Dimm), (Byte), (Nibble), 0xFF/*Dq*/, DW_TO_DB((Value16))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Operation;         ///< Operation
  UINT8   Dimm;              ///< Dimm
  UINT8   Byte;                ///< Byte
  UINT8   Nibble;              ///< Nibble
  UINT8   Dq;                   ///< Dq
  UINT16   Value16;         ///< Value16
} PTO_RX_CLK_DELAY_ENTRY;

// Vref DAC0
//#define MAKE_PTO_VREF_DAC_0_ENTRY(SocketBitMap, ChannelBitMap, Operation, Byte, Dq, Value16) \
//   DW_TO_DB(PLATFORM_TUNING_VREF_DAC0), 14, (SocketBitMap), (ChannelBitMap), 0/*MemPstate*/, 0xFF/*NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)*/,\
//   (Operation), 0xFF/*Dimm*/, (Byte), 0xFF/*Nibble*/, (Dq), DW_TO_DB((Value16))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Operation;         ///< Operation
  UINT8   Dimm;              ///< Dimm
  UINT8   Byte;                ///< Byte
  UINT8   Nibble;              ///< Nibble
  UINT8   Dq;                   ///< Dq
  UINT16   Value16;         ///< Value16
} PTO_VREF_DAC_0_ENTRY;

// Vref DAC1
//#define MAKE_PTO_VREF_DAC_1_ENTRY(SocketBitMap, ChannelBitMap, Operation, Byte, Dq, Value16) \
//   DW_TO_DB(PLATFORM_TUNING_VREF_DAC1), 14, (SocketBitMap), (ChannelBitMap), 0/*MemPstate*/, 0xFF/*NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)*/,\
//   (Operation), 0xFF/*Dimm*/, (Byte), 0xFF/*Nibble*/, (Dq), DW_TO_DB((Value16))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Operation;         ///< Operation
  UINT8   Dimm;              ///< Dimm
  UINT8   Byte;                ///< Byte
  UINT8   Nibble;              ///< Nibble
  UINT8   Dq;                   ///< Dq
  UINT16   Value16;         ///< Value16
} PTO_VREF_DAC_1_ENTRY;

// Vref In Global
//#define MAKE_PTO_VREF_IN_GLOBAL_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Operation, Value16) \
//   DW_TO_DB(PLATFORM_TUNING_VREF_IN_GLOBAL), 14, (SocketBitMap), (ChannelBitMap), (MemPstate), 0xFF/*NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)*/,\
//   (Operation), 0xFF/*Dimm*/, 0xFF/*Byte*/, 0xFF/*Nibble*/, 0xFF/*Dq*/, DW_TO_DB((Value16))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Operation;         ///< Operation
  UINT8   Dimm;              ///< Dimm
  UINT8   Byte;                ///< Byte
  UINT8   Nibble;              ///< Nibble
  UINT8   Dq;                   ///< Dq
  UINT16   Value16;         ///< Value16
} PTO_VREF_IN_GLOBAL_ENTRY;

// 1D training input entrys
//
// 1D Train MR0
//#define MAKE_PTO_1D_TRAIN_MR0_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
//   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_MR0), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT16  Value16;  ///< Value16
} PTO_1D_TRAIN_MR0_ENTRY;

// 1D Train MR1
//#define MAKE_PTO_1D_TRAIN_MR1_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
//   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_MR1), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT16  Value16;  ///< Value16
} PTO_1D_TRAIN_MR1_ENTRY;

// 1D Train MR2
//#define MAKE_PTO_1D_TRAIN_MR2_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
//   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_MR2), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT16  Value16;  ///< Value16
} PTO_1D_TRAIN_MR2_ENTRY;

// 1D Train MR3
//#define MAKE_PTO_1D_TRAIN_MR3_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
//   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_MR3), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT16  Value16;  ///< Value16
} PTO_1D_TRAIN_MR3_ENTRY;

// 1D Train MR4
//#define MAKE_PTO_1D_TRAIN_MR4_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
//   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_MR4), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT16  Value16;  ///< Value16
} PTO_1D_TRAIN_MR4_ENTRY;

// 1D Train MR5
//#define MAKE_PTO_1D_TRAIN_MR5_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
//   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_MR5), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT16  Value16;  ///< Value16
} PTO_1D_TRAIN_MR5_ENTRY;

// 1D Train MR6
//#define MAKE_PTO_1D_TRAIN_MR6_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
//   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_MR6), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT16  Value16;  ///< Value16
} PTO_1D_TRAIN_MR6_ENTRY;

// 1D Train Phy Vref
//#define MAKE_PTO_1D_TRAIN_PHY_VREF_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_PHY_VREF), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_1D_TRAIN_PHY_VREF_ENTRY;

// 1D Train Sequence Control
//#define MAKE_PTO_1D_TRAIN_SEQUENCE_CTRL_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
//   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_SEQUENCE_CTRL), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT16  Value16;  ///< Value16
} PTO_1D_TRAIN_SEQUENCE_CTRL_ENTRY;

// 1D Train DFI MRL Margin
//#define MAKE_PTO_1D_TRAIN_DFI_MRL_MARGIN_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_DFI_MRL_MARGIN), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_1D_TRAIN_DFI_MRL_MARGIN_ENTRY;

// 1D Custom Message
//#define MAKE_PTO_1D_CUSTOM_MSG_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Offset, Value) \
//   DW_TO_DB(PLATFORM_TUNING_1D_CUSTOM_MSG), 10, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Offset)),\
//   (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT16 Offset;              ///< Offset
  UINT8   Value;              ///< Value
} PTO_1D_CUSTOM_MSG_ENTRY;

// 1D Train AcsmOdtCtrl0
//#define MAKE_PTO_1D_TRAIN_ACSM_ODT_CTRL0_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL0), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_1D_TRAIN_ACSM_ODT_CTRL0_ENTRY;

// 1D Train AcsmOdtCtrl1
//#define MAKE_PTO_1D_TRAIN_ACSM_ODT_CTRL1_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL1), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_1D_TRAIN_ACSM_ODT_CTRL1_ENTRY;

// 1D Train AcsmOdtCtrl2
//#define MAKE_PTO_1D_TRAIN_ACSM_ODT_CTRL2_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL2), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_1D_TRAIN_ACSM_ODT_CTRL2_ENTRY;

// 1D Train AcsmOdtCtrl3
//#define MAKE_PTO_1D_TRAIN_ACSM_ODT_CTRL3_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL3), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_1D_TRAIN_ACSM_ODT_CTRL3_ENTRY;

// 1D Train AcsmOdtCtrl4
//#define MAKE_PTO_1D_TRAIN_ACSM_ODT_CTRL4_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL4), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_1D_TRAIN_ACSM_ODT_CTRL4_ENTRY;

// 1D Train AcsmOdtCtrl5
//#define MAKE_PTO_1D_TRAIN_ACSM_ODT_CTRL5_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL5), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_1D_TRAIN_ACSM_ODT_CTRL5_ENTRY;

// 1D Train AcsmOdtCtrl6
//#define MAKE_PTO_1D_TRAIN_ACSM_ODT_CTRL6_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL6), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_1D_TRAIN_ACSM_ODT_CTRL6_ENTRY;

// 1D Train AcsmOdtCtrl7
//#define MAKE_PTO_1D_TRAIN_ACSM_ODT_CTRL7_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL7), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_1D_TRAIN_ACSM_ODT_CTRL7_ENTRY;

// 1D Train ALT_WCAS_L
//#define MAKE_PTO_1D_TRAIN_ALT_WCAS_L_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_1D_TRAIN_ALT_WCAS_L), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_1D_TRAIN_ALT_WCAS_L_ENTRY;

//
// 2D training input entrys
//
// 2D Train MR0
//#define MAKE_PTO_2D_TRAIN_MR0_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
//   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_MR0), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT16  Value16;  ///< Value16
} PTO_2D_TRAIN_MR0_ENTRY;

// 2D Train MR1
//#define MAKE_PTO_2D_TRAIN_MR1_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
//   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_MR1), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT16  Value16;  ///< Value16
} PTO_2D_TRAIN_MR1_ENTRY;

// 2D Train MR2
//#define MAKE_PTO_2D_TRAIN_MR2_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
//   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_MR2), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT16  Value16;  ///< Value16
} PTO_2D_TRAIN_MR2_ENTRY;

// 2D Train MR3
//#define MAKE_PTO_2D_TRAIN_MR3_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
//   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_MR3), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT16  Value16;  ///< Value16
} PTO_2D_TRAIN_MR3_ENTRY;

// 2D Train MR4
//#define MAKE_PTO_2D_TRAIN_MR4_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
//   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_MR4), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT16  Value16;  ///< Value16
} PTO_2D_TRAIN_MR4_ENTRY;

// 2D Train MR5
//#define MAKE_PTO_2D_TRAIN_MR5_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
//   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_MR5), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT16  Value16;  ///< Value16
} PTO_2D_TRAIN_MR5_ENTRY;

// 2D Train MR6
//#define MAKE_PTO_2D_TRAIN_MR6_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
//   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_MR6), 9, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT16  Value16;  ///< Value16
} PTO_2D_TRAIN_MR6_ENTRY;

// 2D Train Phy Vref
//#define MAKE_PTO_2D_TRAIN_PHY_VREF_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_PHY_VREF), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_2D_TRAIN_PHY_VREF_ENTRY;

// 2D Train Sequence Control
//#define MAKE_PTO_2D_TRAIN_SEQUENCE_CTRL_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value16) \
//   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_SEQUENCE_CTRL), 9,(SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Value16))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT16  Value16;  ///< Value16
} PTO_2D_TRAIN_SEQUENCE_CTRL_ENTRY;

// 2D Train DFI MRL Margin
//#define MAKE_PTO_2D_TRAIN_DFI_MRL_MARGIN_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_DFI_MRL_MARGIN), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_2D_TRAIN_DFI_MRL_MARGIN_ENTRY;

// 2D Train RX2D Train Opt
//#define MAKE_PTO_2D_TRAIN_RX2D_TRAIN_OPT_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_RX2D_TRAIN_OPT), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_2D_TRAIN_RX2D_TRAIN_OPT_ENTRY;

// 2D Train TX2D Train Opt
//#define MAKE_PTO_2D_TRAIN_TX2D_TRAIN_OPT_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_TX2D_TRAIN_OPT), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_2D_TRAIN_TX2D_TRAIN_OPT_ENTRY;

// 2D Custom Message
//#define MAKE_PTO_2D_CUSTOM_MSG_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Offset, Value) \
//   DW_TO_DB(PLATFORM_TUNING_2D_CUSTOM_MSG), 10, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), DW_TO_DB((Offset)),\
//   (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT16 Offset;              ///< Offset
  UINT8   Value;              ///< Value
} PTO_2D_CUSTOM_MSG_ENTRY;

// 2D Train AcsmOdtCtrl0
//#define MAKE_PTO_2D_TRAIN_ACSM_ODT_CTRL0_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL0), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_2D_TRAIN_ACSM_ODT_CTRL0_ENTRY;

// 2D Train AcsmOdtCtrl1
//#define MAKE_PTO_2D_TRAIN_ACSM_ODT_CTRL1_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL1), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_2D_TRAIN_ACSM_ODT_CTRL1_ENTRY;

// 2D Train AcsmOdtCtrl2
//#define MAKE_PTO_2D_TRAIN_ACSM_ODT_CTRL2_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL2), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_2D_TRAIN_ACSM_ODT_CTRL2_ENTRY;

// 2D Train AcsmOdtCtrl3
//#define MAKE_PTO_2D_TRAIN_ACSM_ODT_CTRL3_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL3), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_2D_TRAIN_ACSM_ODT_CTRL3_ENTRY;

// 2D Train AcsmOdtCtrl4
//#define MAKE_PTO_2D_TRAIN_ACSM_ODT_CTRL4_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL4), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_2D_TRAIN_ACSM_ODT_CTRL4_ENTRY;

// 2D Train AcsmOdtCtrl5
//#define MAKE_PTO_2D_TRAIN_ACSM_ODT_CTRL5_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL5), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_2D_TRAIN_ACSM_ODT_CTRL5_ENTRY;

// 2D Train AcsmOdtCtrl6
//#define MAKE_PTO_2D_TRAIN_ACSM_ODT_CTRL6_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL6), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_2D_TRAIN_ACSM_ODT_CTRL6_ENTRY;

// 2D Train AcsmOdtCtrl7
//#define MAKE_PTO_2D_TRAIN_ACSM_ODT_CTRL7_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL7), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_2D_TRAIN_ACSM_ODT_CTRL7_ENTRY;

// 2D Train ALT_WCAS_L
//#define MAKE_PTO_2D_TRAIN_ALT_WCAS_L_ENTRY(SocketBitMap, ChannelBitMap, MemPstate, Dimm0Rank, Dimm1Rank, Value) \
//   DW_TO_DB(PLATFORM_TUNING_2D_TRAIN_ALT_WCAS_L), 8, (SocketBitMap), (ChannelBitMap), (MemPstate), NIBBLE_TO_DB((Dimm0Rank), (Dimm1Rank)), (Value)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT8   MemPstate;       ///< MemPstate
  UINT8   Dimm0Rank:4; ///< Dimm0Rank
  UINT8   Dimm1Rank:4; ///< Dimm1Rank
  UINT8   Value;  ///< Value
} PTO_2D_TRAIN_ALT_WCAS_L_ENTRY;

// Vality Check End
//#define MAKE_PTO_VALIDITY_END() \
//   DW_TO_DB(PLATFORM_TUNING_VALIDITY_TERMINATOR), 3
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
} PTO_VALIDITY_END;

// DramManufacturersIDCode
//#define MAKE_PTO_VALIDITY_DRAM_MFG_ID_ENTRY(SocketBitMap, ChannelBitMap, DramMfgId) \
//   DW_TO_DB(PLATFORM_TUNING_VALIDITY_DRAM_MFG_ID), 7, (SocketBitMap), (ChannelBitMap), DW_TO_DB((DramMfgId))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT16  DramMfgId;      ///< DramMfgId
} PTO_VALIDITY_DRAM_MFG_ID_ENTRY;

// DramStepping
//#define MAKE_PTO_VALIDITY_DRAM_STEPPING_ENTRY(SocketBitMap, ChannelBitMap, DramStepping) \
//   DW_TO_DB(PLATFORM_TUNING_VALIDITY_DRAM_STEPPING), 7, (SocketBitMap), (ChannelBitMap), DW_TO_DB((DramStepping))
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
  UINT8   NumOfByte;      ///< Number of bytes used for this macro
  UINT8   SocketBitMap;   ///< SocketBitMap
  UINT8   ChannelBitMap;  ///< ChannelBitMap
  UINT16  DramStepping;   ///< DramStepping
} PTO_VALIDITY_DRAM_STEPPING_ENTRY;

// Table Termination
//#define MAKE_PTO_TERMINATE() DW_TO_DB(PLATFORM_TUNING_TERMINATOR)
typedef struct {
  UINT16  PTEType;        ///< Platform Tuning Entry Type
                          ///< @RANGE: PLATFORM_TUNING_ENTRY
} PTO_TERMINATE_ENTRY;

typedef union {
  PTO_CHIPSEL_INTLV_ENTRY PtoChipselIntlv; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_CHIPSEL_INTLV
  PTO_ECC_ENTRY PtoEcc; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_ECC
  PTO_MEM_BUS_LIMIT_ENTRY PtoMemBusLimit; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_MEM_BUS_LIMIT
  PTO_MBIST_ENABLE_ENTRY PtoMbistEnable; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_MBIST_ENABLE
  PTO_MBIST_TEST_MODE_ENTRY PtoMbistTestMode; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_MBIST_TEST_MODE
  PTO_MBIST_AGGRESSOR_ON_ENTRY PtoMbistAggressorOn; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_MBIST_AGGRESSOR_ON
  PTO_MBIST_PER_BIT_SLAVE_DIE_REPORT_ENTRY PtoMbistPerBitSlaveDieReport; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_MBIST_PER_BIT_SLAVE_DIE_REPORT
  PTO_POST_PACKAGE_REPAIR_ENABLE_ENTRY PtoPostPackageRepairEn; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_POST_PACKAGE_REPAIR_ENABLE
  PTO_POST_PACKAGE_REPAIR_ALL_BANKS_ENTRY PtoPostPackageRepairAllBanks; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_POST_PACKAGE_REPAIR_ALL_BANKS
  PTO_BANK_GROUP_SWAP_ENTRY PtoBankGroupSwap; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_BANK_GROUP_SWAP
  PTO_ODTS_CMD_THROTTLE_ENABLE_ENTRY PtoOdtsCmdThrottleEn; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_ODTS_CMD_THROTTLE_ENABLE
  PTO_AUTO_REFFINE_GRAN_MODE_ENTRY PtoAutoReffineGranMode; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_AUTO_REFFINE_GRAN_MODE
  PTO_ADDR_CMD_PARITY_RETRY_ENTRY PtoAddrCmdParityRetry; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_ADDR_CMD_PARITY_RETRY
  PTO_RCD_PARITY_ENABLE_ENTRY PtoRcdParityEn; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_RCD_PARITY_ENABLE
  PTO_WR_CRC_ENABLE_ENTRY PtoWrCrcEn; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_WR_CRC_ENABLE
  PTO_WR_CRC_RETRY_ENTRY PtoWrCrcRetry; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_WR_CRC_RETRY
  PTO_ECC_SYMBOL_SIZE_ENTRY PtoEccSymbolSize; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_ECC_SYMBOL_SIZE
  PTO_ENABLE_POWER_DOWN_ENTRY PtoEnPowerDown; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_ENABLE_POWER_DOWN
  PTO_ENABLE_TUNING_MEMORY_ALL_CLOCKS_ON_ENTRY PtoEnMemAllClocks; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_MEMORY_ALL_CLOCKS_ON
  PTO_ENABLE_POWER_DOWN_MODE_ENTRY PtoEnPowerDownMode; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_POWER_DOWN_MODE
  PTO_TEMPERATURE_CONTROLLED_REFRESH_ENABLE_ENTRY PtoTempCntrlRefreshEn; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TEMPERATURE_CONTROLLED_REFRESH_ENABLE
  PTO_CMD_BUS_TMG_ENTRY PtoCmdBusTmg; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_CMD_BUS_TMG
  PTO_DATA_BUS_TMG_ENTRY PtoDataBusTmg; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_DATA_BUS_TMG
  PTO_SPD_VALUE_OVERRIDE_ENTRY PtoSpdValOverride; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_SPD_VALUE_OVERRIDE
  PTO_SPD_CRC_IGNORE_ENTRY PtoSpdCrcIgnore; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_SPD_CRC_IGNORE
  PTO_GEAR_DOWN_ENABLE_ENTRY PtoGearDownEn; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_GEAR_DOWN_EN
  PTO_CMD_2T_ENABLE_ENTRY PtoCmd2tEn; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_CMD_2T_EN
  PTO_WR_PREAMBLE_ENTRY PtoWrPreamble; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_WR_PREAMBLE
  PTO_RD_PREAMBLE_ENTRY PtoRdPreamble; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_RD_PREAMBLE
  PTO_ODT_PAT_CTRL_CS0_ENTRY PtoOdtPatCtrlCs0; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_ODT_PAT_CTRL_CS0
  PTO_ODT_PAT_CTRL_CS1_ENTRY PtoOdtPatCtrlCs1; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_ODT_PAT_CTRL_CS1
  PTO_ODT_PAT_CTRL_CS2_ENTRY PtoOdtPatCtrlCs2; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_ODT_PAT_CTRL_CS2
  PTO_ODT_PAT_CTRL_CS3_ENTRY PtoOdtPatCtrlCs3; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_ODT_PAT_CTRL_CS3
  PTO_TX_EQ_IMPEDANCE_DQ_ENTRY PtoTxEqImpedDq; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TX_EQ_IMPEDANCE_DQ
  PTO_TX_ODT_DRV_STREN_ENTRY PtoTxOdtDrvStren; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TX_ODT_DRV_STREN
  PTO_TRCDWR_ENTRY PtoTrcdwr; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TRCDWR
  PTO_TRCDRD_ENTRY PtoTrcdrd; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TRCDRD
  PTO_TRAS_ENTRY PtoTras; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TRAS
  PTO_TCL_ENTRY PtoTcl; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TCL
  PTO_TRP_ENTRY PtoTrp; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TRP
  PTO_TRTP_ENTRY PtoTrtp; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TRTP
  PTO_TRRDDLR_ENTRY PtoTrrddlr; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TRRDDLR
  PTO_TRRDL_ENTRY PtoTrrdl; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TRRDL
  PTO_TRRDS_ENTRY PtoTrrds; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TRRDS
  PTO_TFAWDLR_ENTRY PtoTfawdlr; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TFAWDLR
  PTO_TFAWSLR_ENTRY PtoTfawslr; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TFAWSLR
  PTO_TFAW_ENTRY PtoTfaw; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TFAW
  PTO_TWTRL_ENTRY PtoTwtrl; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TWTRL
  PTO_TWTRS_ENTRY PtoTwtrs; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TWTRS
  PTO_TCWL_ENTRY PtoTcwl; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TCWL
  PTO_TWR_ENTRY PtoTwr; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TWR
  PTO_TRCPAGE_ENTRY PtoTrcpage; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TRCPAGE
  PTO_TRFC_ENTRY PtoTrfc; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TRFC
  PTO_TRFC2_ENTRY PtoTrfc2; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TRFC2
  PTO_TRFC4_ENTRY PtoTrfc4; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TRFC4
  PTO_TSTAG_ENTRY PtoTstag; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TSTAG
  PTO_TSTAGLR_ENTRY Ptostaglr; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TSTAGLR
  PTO_TSTAG2LR_ENTRY Ptostag2lr; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TSTAG2LR
  PTO_TSTAG4LR_ENTRY Ptostag4lr; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TSTAG4LR
  PTO_TRDRDBAN_ENTRY PtoTrdrdban; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TRDRDBAN
  PTO_TRDRDSCL_ENTRY PtoTrdrdscl; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TRDRDSCL
  PTO_TRDRDSCDLR_ENTRY PtoTrdrdscdlr; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TRDRDSCDLR
  PTO_TRDRDSC_ENTRY PtoTrdrdsc; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TRDRDSC
  PTO_TRDRDSD_ENTRY PtoTrdrdsd; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TRDRDSD
  PTO_TRDRDDD_ENTRY PtoTrdrddd; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TRDRDDD
  PTO_TWRWRBAN_ENTRY PtoTwrwrban; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TWRWRBAN
  PTO_TWRWRSCL_ENTRY PtoTwrwrscl; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TWRWRSCL
  PTO_TWRWRSCDLR_ENTRY PtoTwrwrscdlr; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TWRWRSCDLR
  PTO_TWRWRSC_ENTRY PtoTwrwrsc; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TWRWRSC
  PTO_TWRWRSD_ENTRY PtoTwrwrsd; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TWRWRSD
  PTO_TWRWRDD_ENTRY PtoTwrwrdd; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TWRWRDD
  PTO_TWRRDSCDLR_ENTRY PtoTwrrdscdlr; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TWRRDSCDLR
  PTO_TRDWR_ENTRY PtoTrdwr; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TRDWR
  PTO_TWRRD_ENTRY PtoTwrrd; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TWRRD
  PTO_DDR4_UECC_RETRY_ENTRY PtoDdr4UeccRetry; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_DDR4_UECC_RETRY
  PTO_ADDR_CMD_SETUP_ENTRY PtoAddrCmdSetup; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_ADDR_CMD_SETUP
  PTO_CS_ODT_SETUP_ENTRY PtoCsOdtSetup; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_CS_ODT_SETUP
  PTO_CKE_SETUP_ENTRY PtoCkeSetup; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_CKE_SETUP
  PTO_CLK_DRV_STREN_ENTRY PtoClkDrvStren; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_CLK_DRV_STREN
  PTO_ADDR_CMD_DRV_STREN_ENTRY PtoAddCmdDrvStren; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_ADDR_CMD_DRV_STREN
  PTO_CS_ODT_DRV_STREN_ENTRY PtoCsOdtDrvStren; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_CS_ODT_DRV_STREN
  PTO_CKE_DRV_STREN_ENTRY PtoCkeDrvStren; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_CKE_DRV_STREN
  PTO_RX_ENABLE_DELAY_ENTRY PtoRxEnable; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_RX_ENABLE_DELAY
  PTO_TX_DQ_DELAY_ENTRY PtoTxDqDly; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TX_DQ_DELAY
  PTO_RX_PB_DELAY_ENTRY PtoRxPbDly; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_RX_PB_DELAY
  PTO_RX_CLK_DELAY_ENTRY PtoRxClkDly; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_RX_CLK_DELAY
  PTO_VREF_DAC_0_ENTRY PtoVrefDAC0; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_VREF_DAC0
  PTO_VREF_DAC_1_ENTRY PtoVrefDAC1; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_VREF_DAC1
  PTO_VREF_IN_GLOBAL_ENTRY PtoVrefInGlobal; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_VREF_IN_GLOBAL
  PTO_1D_TRAIN_MR0_ENTRY Pto1dTrainMr0; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_1D_TRAIN_MR0
  PTO_1D_TRAIN_MR1_ENTRY Pto1dTrainMr1; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_1D_TRAIN_MR1
  PTO_1D_TRAIN_MR2_ENTRY Pto1dTrainMr2; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_1D_TRAIN_MR2
  PTO_1D_TRAIN_MR3_ENTRY Pto1dTrainMr3; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_1D_TRAIN_MR3
  PTO_1D_TRAIN_MR4_ENTRY Pto1dTrainMr4; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_1D_TRAIN_MR4
  PTO_1D_TRAIN_MR5_ENTRY Pto1dTrainMr5; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_1D_TRAIN_MR5
  PTO_1D_TRAIN_MR6_ENTRY Pto1dTrainMr6; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_1D_TRAIN_MR6
  PTO_1D_TRAIN_PHY_VREF_ENTRY Pto1dTrainPhyVref; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_1D_TRAIN_PHY_VREF
  PTO_1D_TRAIN_SEQUENCE_CTRL_ENTRY Pto1dTrainSeqCtrl; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_1D_TRAIN_SEQUENCE_CTRL
  PTO_1D_TRAIN_DFI_MRL_MARGIN_ENTRY Pto1dTrainDfiMrlMargin; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_1D_TRAIN_DFI_MRL_MARGIN
  PTO_1D_CUSTOM_MSG_ENTRY Pto1dCustomMsg; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_1D_CUSTOM_MSG
  PTO_1D_TRAIN_ACSM_ODT_CTRL0_ENTRY Pto1dTrainAcsmOdtCtrl0; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL0
  PTO_1D_TRAIN_ACSM_ODT_CTRL1_ENTRY Pto1dTrainAcsmOdtCtrl1; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL1
  PTO_1D_TRAIN_ACSM_ODT_CTRL2_ENTRY Pto1dTrainAcsmOdtCtrl2; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL2
  PTO_1D_TRAIN_ACSM_ODT_CTRL3_ENTRY Pto1dTrainAcsmOdtCtrl3; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL3
  PTO_1D_TRAIN_ACSM_ODT_CTRL4_ENTRY Pto1dTrainAcsmOdtCtrl4; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL4
  PTO_1D_TRAIN_ACSM_ODT_CTRL5_ENTRY Pto1dTrainAcsmOdtCtrl5; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL5
  PTO_1D_TRAIN_ACSM_ODT_CTRL6_ENTRY Pto1dTrainAcsmOdtCtrl6; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL6
  PTO_1D_TRAIN_ACSM_ODT_CTRL7_ENTRY Pto1dTrainAcsmOdtCtrl7; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_1D_TRAIN_ACSM_ODT_CTRL7
  PTO_1D_TRAIN_ALT_WCAS_L_ENTRY Pto1dTrainAltWcsL; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_1D_TRAIN_ALT_WCAS_L
  PTO_2D_TRAIN_MR0_ENTRY Pto2dTrainMr0; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_2D_TRAIN_MR0
  PTO_2D_TRAIN_MR1_ENTRY Pto2dTrainMr1; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_2D_TRAIN_MR1
  PTO_2D_TRAIN_MR2_ENTRY Pto2dTrainMr2; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_2D_TRAIN_MR2
  PTO_2D_TRAIN_MR3_ENTRY Pto2dTrainMr3; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_2D_TRAIN_MR3
  PTO_2D_TRAIN_MR4_ENTRY Pto2dTrainMr4; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_2D_TRAIN_MR4
  PTO_2D_TRAIN_MR5_ENTRY Pto2dTrainMr5; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_2D_TRAIN_MR5
  PTO_2D_TRAIN_MR6_ENTRY Pto2dTrainMr6; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_2D_TRAIN_MR6
  PTO_2D_TRAIN_PHY_VREF_ENTRY Pto2dTrainPhyVref; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_2D_TRAIN_PHY_VREF
  PTO_2D_TRAIN_SEQUENCE_CTRL_ENTRY Pto2dTrainSeqCtrl; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_2D_TRAIN_SEQUENCE_CTRL
  PTO_2D_TRAIN_DFI_MRL_MARGIN_ENTRY Pto2dTrainDfiMrlMargin; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_2D_TRAIN_DFI_MRL_MARGIN
  PTO_2D_TRAIN_RX2D_TRAIN_OPT_ENTRY Pto2dTrainRx2dOpt; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_2D_TRAIN_RX2D_TRAIN_OPT
  PTO_2D_TRAIN_TX2D_TRAIN_OPT_ENTRY Pto2dTrainTx2dOpt; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_2D_TRAIN_TX2D_TRAIN_OPT
  PTO_2D_CUSTOM_MSG_ENTRY Pto2dCustomMsg; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_2D_CUSTOM_MSG
  PTO_2D_TRAIN_ACSM_ODT_CTRL0_ENTRY Pto2dTrainAcsmOdtCtrl0; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL0
  PTO_2D_TRAIN_ACSM_ODT_CTRL1_ENTRY Pto2dTrainAcsmOdtCtrl1; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL1
  PTO_2D_TRAIN_ACSM_ODT_CTRL2_ENTRY Pto2dTrainAcsmOdtCtrl2; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL2
  PTO_2D_TRAIN_ACSM_ODT_CTRL3_ENTRY Pto2dTrainAcsmOdtCtrl3; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL3
  PTO_2D_TRAIN_ACSM_ODT_CTRL4_ENTRY Pto2dTrainAcsmOdtCtrl4; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL4
  PTO_2D_TRAIN_ACSM_ODT_CTRL5_ENTRY Pto2dTrainAcsmOdtCtrl5; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL5
  PTO_2D_TRAIN_ACSM_ODT_CTRL6_ENTRY Pto2dTrainAcsmOdtCtrl6; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL6
  PTO_2D_TRAIN_ACSM_ODT_CTRL7_ENTRY Pto2dTrainAcsmOdtCtrl7; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_2D_TRAIN_ACSM_ODT_CTRL7
  PTO_2D_TRAIN_ALT_WCAS_L_ENTRY Pto2dTrainAltWcsL; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_2D_TRAIN_ALT_WCAS_L
  PTO_VALIDITY_END PtoValidtyEnd; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_VALIDITY_TERMINATOR
  PTO_VALIDITY_DRAM_MFG_ID_ENTRY  	PtoValDramMfgId; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_VALIDITY_DRAM_MFG_ID
  PTO_VALIDITY_DRAM_STEPPING_ENTRY  PtoValDramStepping; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_VALIDITY_DRAM_STEPPING
  PTO_TERMINATE_ENTRY  PtoTerminate; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_TERMINATOR
} MEM_PLATFORM_TUNING;

/// PLATFORM_TUNING_VALIDITY_DRAM_MFG_ID
/** MEM_PLATFORM_TUNING
*    @GROUPID: APCB_GROUP_MEMORY
*    @TYPEID: APCB_MEM_TYPE_PLATFORM_TUNING
*    @ALIGNMENT: 1
*    This data structure is used to pass memory MEM_PLATFORM_TUNING data
*/
typedef struct {
  MEM_PLATFORM_TUNING       MemPTE1st;
  MEM_PLATFORM_TUNING       MemPTE[];
} PLATFORM_TUNING_ENTRY_ARRAY;

#endif // _APCB_MEM_PLATFORM_TUNING_H_


