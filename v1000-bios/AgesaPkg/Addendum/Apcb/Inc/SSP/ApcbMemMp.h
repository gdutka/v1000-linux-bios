/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _MP_H_
#define _MP_H_
//
// @APCB_START
//


///Mp.h

/// Type of an entry for Dram Term table
typedef struct {
  _4BYTE_ALIGN UINT32 Speed; ///< BitMap for the supported speed
  _1BYTE_ALIGN UINT8  Dimms; ///< BitMap for supported number of dimm
  _1BYTE_ALIGN UINT8  QR_Dimms; ///< BitMap for supported number of QR dimm
  _1BYTE_ALIGN UINT8  DramTerm; ///< DramTerm value
  _1BYTE_ALIGN UINT8  QR_DramTerm; ///< DramTerm value for QR
  _1BYTE_ALIGN UINT8  DynamicDramTerm; ///< Dynamic DramTerm
} DRAM_TERM_ENTRY;

/// Type of an entry for POR speed limit table
typedef struct {
  _2BYTE_ALIGN UINT16   DIMMRankType;  ///< Bitmap of Ranks
  _1BYTE_ALIGN UINT8    Dimms;  ///< Number of dimm
  _2BYTE_ALIGN UINT16    SpeedLimit_1_5V; ///< POR speed limit for 1.5V
  _2BYTE_ALIGN UINT16    SpeedLimit_1_35V; ///< POR speed limit for 1.35V
  _2BYTE_ALIGN UINT16    SpeedLimit_1_25V; ///< POR speed limit for 1.25V
} POR_SPEED_LIMIT;

/// UDIMM&RDIMM Max. Frequency
typedef union {
  struct {                       ///< PSCFG_MAXFREQ_ENTRY
    _1BYTE_ALIGN UINT8 DimmPerCh;             ///< Dimm slot per chanel
    _2BYTE_ALIGN UINT16 Dimms;              ///< Number of Dimms on a channel
    _2BYTE_ALIGN UINT16 SR;                 ///< Number of single-rank Dimm
    _2BYTE_ALIGN UINT16 DR;                 ///< Number of dual-rank Dimm
    _2BYTE_ALIGN UINT16 QR;                 ///< Number of quad-rank Dimm
    _2BYTE_ALIGN UINT16 Speed1_5V;            ///< Speed limit with voltage 1.5V
    _2BYTE_ALIGN UINT16 Speed1_35V;           ///< Speed limit with voltage 1.35V
    _2BYTE_ALIGN UINT16 Speed1_25V;           ///< Speed limit with voltage 1.25V
  } _MAXFREQ_ENTRY;
  struct {
    _1BYTE_ALIGN UINT8 DimmSlotPerCh;
    _2BYTE_ALIGN UINT16 CDN;                  ///< Condition
    _2BYTE_ALIGN UINT16 CDN1;                  ///< Condition
    _2BYTE_ALIGN UINT16 CDN2;                  ///< Condition
    _2BYTE_ALIGN UINT16 CDN3;                  ///< Condition
    _2BYTE_ALIGN UINT16 Speed[3];             ///< Speed limit
  } MAXFREQ_ENTRY;
} PSCFG_MAXFREQ_ENTRY;

/** PSCFG_MAXFREQ_ENTRY
*    @GROUPID: APCB_GROUP_MEMORY (0x1704)
*    @TYPEID: APCB_MEM_TYPE_PS_UDIMM_DDR4_MAX_FREQ (0x44)
*    This data structure is used to pass memory PSCFG_MAXFREQ_ENTRY data
*
*    @GROUPID: APCB_GROUP_MEMORY (0x1704)
*    @TYPEID: APCB_MEM_TYPE_PS_RDIMM_DDR4_MAX_FREQ (0x49)
*    This data structure is used to pass memory PSCFG_MAXFREQ_ENTRY data
*
*    @GROUPID: APCB_GROUP_MEMORY (0x1704)
*    @TYPEID: APCB_MEM_TYPE_PS_3DS_RDIMM_DDR4_MAX_FREQ (0x4B)
*    This data structure is used to pass memory PSCFG_MAXFREQ_ENTRY data
*
*    @GROUPID: APCB_GROUP_MEMORY (0x1704)
*    @TYPEID: APCB_MEM_TYPE_PS_UDIMM_DDR4_STRETCH_FREQ (0x45)
*    This data structure is used to pass memory PSCFG_STRETCHFREQ_ENTRY data
*
*    @GROUPID: APCB_GROUP_MEMORY (0x1704)
*    @TYPEID: APCB_MEM_TYPE_PS_RDIMM_DDR4_STRETCH_FREQ (0x4A)
*    This data structure is used to pass memory PSCFG_STRETCHFREQ_ENTRY data
*/
typedef struct {
  PSCFG_MAXFREQ_ENTRY         PscfgMaxFreqEntry1st;
  PSCFG_MAXFREQ_ENTRY         PscfgMaxFreqEntry[];
} PSCFG_MAXFREQ_ENTRY_STRUCTURE;

///  LRDIMM Max. Frequency
typedef union {
  struct {                       ///< PSCFG_LR_MAXFREQ_ENTRY
    _1BYTE_ALIGN UINT8 DimmPerCh;             ///< Dimm slot per chanel
    _2BYTE_ALIGN UINT16 Dimms;              ///< Number of Dimms on a channel
    _2BYTE_ALIGN UINT16 LR;                ///< Number of LR-DIMM
    _2BYTE_ALIGN UINT16 Speed1_5V;            ///< Speed limit with voltage 1.5V
    _2BYTE_ALIGN UINT16 Speed1_35V;           ///< Speed limit with voltage 1.35V
    _2BYTE_ALIGN UINT16 Speed1_25V;           ///< Speed limit with voltage 1.25V
  } _LR_MAXFREQ_ENTRY;
  struct {
    _1BYTE_ALIGN UINT8 DimmSlotPerCh;
    _2BYTE_ALIGN UINT16 CDN;
    _2BYTE_ALIGN UINT16 CDN1;                  ///< Condition
    _2BYTE_ALIGN UINT16 CDN2;                  ///< Condition
    _2BYTE_ALIGN UINT16 CDN3;                  ///< Condition
    _2BYTE_ALIGN UINT16 Speed[3];
  } LR_MAXFREQ_ENTRY;
} PSCFG_LR_MAXFREQ_ENTRY;

/** PSCFG_LR_MAXFREQ_ENTRY
*    @GROUPID: APCB_GROUP_MEMORY (0x1704)
*    @TYPEID: APCB_MEM_TYPE_PS_LRDIMM_DDR4_MAX_FREQ (0x57)
*    This data structure is used to pass memory PSCFG_MAXFREQ_ENTRY data
*/
typedef struct {
  PSCFG_LR_MAXFREQ_ENTRY         PscfgLrMaxFreqEntry1st;
  PSCFG_LR_MAXFREQ_ENTRY         PscfgLrMaxFreqEntry[];
} PSCFG_LR_MAXFREQ_ENTRY_STRUCTURE;
//-----------------------------------------------------------------------------

/// UDIMM&RDIMM RttNom and RttWr
typedef struct {
  _8BYTE_ALIGN UINT64 DimmPerCh;          ///< Dimm slot per chanel
  _8BYTE_ALIGN UINT64 DDRrate;           ///< Bitmap of DDR rate
  _8BYTE_ALIGN UINT64 VDDIO;              ///< Bitmap of VDDIO
  _8BYTE_ALIGN UINT64 Dimm0;              ///< Bitmap of rank type of Dimm0
  _8BYTE_ALIGN UINT64 Dimm1;              ///< Bitmap of rank type of Dimm1
  _8BYTE_ALIGN UINT64 Dimm2;              ///< Bitmap of rank type of Dimm2
  _8BYTE_ALIGN UINT64 Dimm;               ///< Bitmap of rank type of Dimm
  _8BYTE_ALIGN UINT64 Rank;               ///< Bitmap of rank
  _1BYTE_ALIGN UINT8 RttNom;              ///< Dram term
  _1BYTE_ALIGN UINT8 RttWr;               ///< Dynamic dram term
} PSCFG_RTT_ENTRY;

/// LRDIMM RttNom and RttWr
typedef struct {
  _8BYTE_ALIGN UINT64 DimmPerCh;          ///< Dimm slot per chanel
  _8BYTE_ALIGN UINT64 DDRrate;           ///< Bitmap of DDR rate
  _8BYTE_ALIGN UINT64 VDDIO;              ///< Bitmap of VDDIO
  _8BYTE_ALIGN UINT64 Dimm0;              ///< Dimm0 population
  _8BYTE_ALIGN UINT64 Dimm1;              ///< Dimm1 population
  _8BYTE_ALIGN UINT64 Dimm2;             ///< Dimm2 population
  _1BYTE_ALIGN UINT8 RttNom;              ///< Dram term
  _1BYTE_ALIGN UINT8 RttWr;               ///< Dynamic dram term
} PSCFG_LR_RTT_ENTRY;

/// UDIMM&RDIMM&LRDIMM ODT pattern OF 1 DPC
typedef struct {
  _4BYTE_ALIGN UINT32 Dimm0;              ///< Bitmap of dimm0 rank type or dimm0 population of LRDIMM
  _4BYTE_ALIGN UINT32 ODTPatCS0;          ///< ODT Pattern for CS0
  _4BYTE_ALIGN UINT32 ODTPatCS1;          ///< ODT Pattern for CS1
  _4BYTE_ALIGN UINT32 ODTPatCS2;          ///< ODT Pattern for CS2
  _4BYTE_ALIGN UINT32 ODTPatCS3;          ///< ODT Pattern for CS3
} PSCFG_1D_ODTPAT_ENTRY;

/// UDIMM&RDIMM&LRDIMM ODT pattern OF 2 DPC
typedef struct {
  _4BYTE_ALIGN UINT32 Dimm0:4;            ///< Bitmap of dimm0 rank type or dimm0 population of LRDIMM
  _4BYTE_ALIGN UINT32 Dimm1:28;           ///< Bitmap of dimm1 rank type or dimm1 population of LRDIMM
  _4BYTE_ALIGN UINT32 ODTPatCS0;          ///< ODT Pattern for CS0
  _4BYTE_ALIGN UINT32 ODTPatCS1;          ///< ODT Pattern for CS1
  _4BYTE_ALIGN UINT32 ODTPatCS2;          ///< ODT Pattern for CS2
  _4BYTE_ALIGN UINT32 ODTPatCS3;          ///< ODT Pattern for CS3
} PSCFG_2D_ODTPAT_ENTRY;

/** PSCFG_2D_ODTPAT_ENTRY
*    @GROUPID: APCB_GROUP_MEMORY (0x1704)
*    @TYPEID: APCB_MEM_TYPE_PS_UDIMM_DDR4_ODT_PAT (0x41)
*    This data structure is used to pass memory PS_UDIMM_DDR4_ODT_PAT data
*
*    @GROUPID: APCB_GROUP_MEMORY (0x1704)
*    @TYPEID: APCB_MEM_TYPE_PS_RDIMM_DDR4_ODT_PAT (0x46)
*    This data structure is used to pass memory PS_UDIMM_DDR4_ODT_PAT data
*
*    @GROUPID: APCB_GROUP_MEMORY (0x1704)
*    @TYPEID: APCB_MEM_TYPE_PS_LRDIMM_DDR4_ODT_PAT (0x54)
*    This data structure is used to pass memory PS_UDIMM_DDR4_ODT_PAT data
*/
typedef struct {
  PSCFG_2D_ODTPAT_ENTRY       PscfgTwodOdtPatEntry1st;
  PSCFG_2D_ODTPAT_ENTRY       PscfgTwodOdtPatEntry[];
} PSCFG_TWO_D_ODTPAT;

/// UDIMM&RDIMM&LRDIMM ODT pattern OF 3 DPC
typedef struct {
  _4BYTE_ALIGN UINT32 Dimm0:4;            ///< Bitmap of dimm0 rank type or dimm0 population of LRDIMM
  _4BYTE_ALIGN UINT32 Dimm1:4;            ///< Bitmap of dimm1 rank type or dimm1 population of LRDIMM
  _4BYTE_ALIGN UINT32 Dimm2:24;           ///< Bitmap of dimm2 rank type or dimm2 population of LRDIMM
  _4BYTE_ALIGN UINT32 ODTPatCS0;          ///< ODT Pattern for CS0
  _4BYTE_ALIGN UINT32 ODTPatCS1;          ///< ODT Pattern for CS1
  _4BYTE_ALIGN UINT32 ODTPatCS2;          ///< ODT Pattern for CS2
  _4BYTE_ALIGN UINT32 ODTPatCS3;          ///< ODT Pattern for CS3
} PSCFG_3D_ODTPAT_ENTRY;

/// UDIMM&RDIMM&LRDIMM SlowMode, AddrTmgCtl and ODC
typedef struct {
  _8BYTE_ALIGN UINT64 DimmPerCh;          ///< Dimm slot per channel
  _8BYTE_ALIGN UINT64 DDRrate;           ///< Bitmap of DDR rate
  _8BYTE_ALIGN UINT64 VDDIO;              ///< Bitmap of VDDIO
  _8BYTE_ALIGN UINT64 Dimm0;              ///< Bitmap of dimm0 rank type or dimm0 population of LRDIMM
  _8BYTE_ALIGN UINT64 Dimm1;              ///< Bitmap of dimm1 rank type or dimm1 population of LRDIMM
  _8BYTE_ALIGN UINT64 Dimm2;             ///< Bitmap of dimm2 rank type or dimm2 population of LRDIMM
  _8BYTE_ALIGN UINT64 SlowMode;           ///< SlowMode
  _4BYTE_ALIGN UINT32 AddTmgCtl;            ///< AddTmgCtl
  _4BYTE_ALIGN UINT32 ODC;                  ///< ODC
  _1BYTE_ALIGN UINT8 POdtOff;               ///< POdtOff
} PSCFG_SAO_ENTRY;

/// UDIMM&RDIMM&LRDIMM 2D training config entry
typedef struct {
  _8BYTE_ALIGN UINT64 DimmPerCh;          ///< Dimm per channel
  _8BYTE_ALIGN UINT64 DDRrate;           ///< Bitmap of DDR rate
  _8BYTE_ALIGN UINT64 VDDIO;              ///< Bitmap of VDDIO
  _8BYTE_ALIGN UINT64 Dimm0;              ///< Bitmap of dimm0 rank type or dimm0 population of LRDIMM
  _8BYTE_ALIGN UINT64 Dimm1;              ///< Bitmap of dimm1 rank type or dimm1 population of LRDIMM
  _8BYTE_ALIGN UINT64 Dimm2;             ///< Bitmap of dimm2 rank type or dimm2 population of LRDIMM
  _8BYTE_ALIGN UINT64 Enable2D;           ///< SlowMode
} PSCFG_S2D_ENTRY;

/// UDIMM&RDIMM MR0[WR]
typedef struct {
  _1BYTE_ALIGN UINT8 Timing;                ///< Fn2_22C_dct[1:0][Twr]
  _1BYTE_ALIGN UINT8 Value;                 ///< MR0[WR] : bit0 - bit2 available
} PSCFG_MR0WR_ENTRY;

/// UDIMM&RDIMM MR0[CL]
typedef struct {
  _1BYTE_ALIGN UINT8 Timing;                ///< Fn2_200_dct[1:0][Tcl]
  _1BYTE_ALIGN UINT8 Value;               ///< MR0[CL] : bit0 - bit2 CL[3:1]
  _1BYTE_ALIGN UINT8 Value1;              ///< MR0[CL] : bit3 CL[0]
} PSCFG_MR0CL_ENTRY;

/// UDIMM&RDIMM MR2[IBT]
typedef struct {
  _8BYTE_ALIGN UINT64 DimmPerCh;          ///< Dimm per channel
  _8BYTE_ALIGN UINT64 DDRrate;           ///< Bitmap of DDR rate
  _8BYTE_ALIGN UINT64 VDDIO;              ///< Bitmap of VDDIO
  _8BYTE_ALIGN UINT64 Dimm0;              ///< Bitmap of dimm0 rank type
  _8BYTE_ALIGN UINT64 Dimm1;              ///< Bitmap of dimm1 rank type
  _8BYTE_ALIGN UINT64 Dimm2;              ///< Bitmap of dimm2 rank type
  _8BYTE_ALIGN UINT64 Dimm;               ///< Bitmap of rank type of Dimm
  _8BYTE_ALIGN UINT64 NumOfReg;           ///< Number of registers
  _8BYTE_ALIGN UINT64 IBT;                 ///< MR2[IBT] value
} PSCFG_MR2IBT_ENTRY;

/// UDIMM&RDIMM&LRDIMM Operating Speed
typedef struct {
  _4BYTE_ALIGN UINT32 DDRrate;              ///< Bitmap of DDR rate
  _1BYTE_ALIGN UINT8 OPSPD;                 ///< RC10[OperatingSpeed]
} PSCFG_OPSPD_ENTRY;

/// LRDIMM IBT
typedef struct {
  _8BYTE_ALIGN UINT64 DimmPerCh;          ///< Dimm per channel
  _8BYTE_ALIGN UINT64 DDRrate;           ///< Bitmap of DDR rate
  _8BYTE_ALIGN UINT64 VDDIO;              ///< Bitmap of VDDIO
  _8BYTE_ALIGN UINT64 Dimm0;              ///< Dimm0 population
  _8BYTE_ALIGN UINT64 Dimm1;              ///< Dimm1 population
  _8BYTE_ALIGN UINT64 Dimm2;              ///< Dimm2 population
  _8BYTE_ALIGN UINT64 F0RC8;              ///< F0RC8
  _8BYTE_ALIGN UINT64 F1RC0;              ///< F1RC0
  _8BYTE_ALIGN UINT64 F1RC1;              ///< F1RC1
  _8BYTE_ALIGN UINT64 F1RC2;              ///< F1RC2
} PSCFG_L_IBT_ENTRY;

/// LRDIMM F0RC13[NumPhysicalRanks]
typedef struct {
  _1BYTE_ALIGN UINT8 NumRanks : 3;            ///< NumRanks
  _1BYTE_ALIGN UINT8 NumPhyRanks : 5;         ///< NumPhyRanks
} PSCFG_L_NPR_ENTRY;

/// LRDIMM F0RC13[NumLogicalRanks]
typedef struct {
  _2BYTE_ALIGN UINT16 NumPhyRanks;        ///< NumPhyRanks
  _2BYTE_ALIGN UINT16 DramCap;            ///< DramCap
  _2BYTE_ALIGN UINT16 NumDimmSlot;        ///< NumDimmSlot
  _1BYTE_ALIGN UINT8 NumLogRanks;         ///< NumLogRanks
} PSCFG_L_NLR_ENTRY;

/// UDIMM&RDIMM&LRDIMM pass1 seed entry
typedef struct {
  _1BYTE_ALIGN UINT8 DimmPerCh;             ///< Dimm per channel
  _1BYTE_ALIGN UINT8 Channel;               ///< Channel#
  _2BYTE_ALIGN UINT16 SeedVal;              ///< Seed value
} PSCFG_SEED_ENTRY;

/// Platform specific configuration types
typedef enum {
  PSCFG_MAXFREQ,               ///< PSCFG_MAXFREQ
  PSCFG_LR_MAXFREQ,            ///< PSCFG_LR_MAXFREQ
  PSCFG_RTT,                   ///< PSCFG_RTT
  PSCFG_LR_RTT,                ///< PSCFG_LR_RTT
  PSCFG_ODT_PAT_1D,            ///< PSCFG_ODT_PAT_1D
  PSCFG_ODT_PAT_2D,            ///< PSCFG_ODT_PAT_2D
  PSCFG_ODT_PAT_3D,            ///< PSCFG_ODT_PAT_3D
  PSCFG_LR_ODT_PAT_1D,         ///< PSCFG_LR_ODT_PAT_1D
  PSCFG_LR_ODT_PAT_2D,         ///< PSCFG_LR_ODT_PAT_2D
  PSCFG_LR_ODT_PAT_3D,         ///< PSCFG_LR_ODT_PAT_3D
  PSCFG_SAO,                   ///< PSCFG_SAO
  PSCFG_LR_SAO,                ///< PSCFG_LR_SAO
  PSCFG_MR0WR,                 ///< PSCFG_MR0WR
  PSCFG_MR0CL,                 ///< PSCFG_MR0CL
  PSCFG_RC2IBT,                ///< PSCFG_RC2IBT
  PSCFG_RC10OPSPD,             ///< PSCFG_RC10OPSPD
  PSCFG_LR_IBT,                ///< PSCFG_LR_IBT
  PSCFG_LR_NPR,                ///< PSCFG_LR_NPR
  PSCFG_LR_NLR,                ///< PSCFG_LR_NLR
  PSCFG_S2D,                   ///< PSCFG_S2D
  PSCFG_WL_PASS1_SEED,         ///< PSCFG_WL_PASS1_SEED
  PSCFG_HWRXEN_PASS1_SEED,     ///< PSCFG_HWRXEN_SEED
  PSCFG_CADBUS,                ///< PSCFG_CADBUS
  PSCFG_CADBUS_DRAMDN,         ///< PSCFG_CADBUS_DRAMDN
  PSCFG_DATABUS,               ///< PSCFG_DATABUS
  PSCFG_DATABUS_DRAMDN,        ///< PSCFG_DATABUS_DRAMDN

                 // The type of general table entries could be added between
                 // PSCFG_GEN_START and PSCFG_GEN_END so that the PSCGen routine
                 // is able to look for the entries per the PSCType.
  PSCFG_GEN_START,             ///< PSCFG_GEN_START
  PSCFG_CLKDIS,                ///< PSCFG_CLKDIS
  PSCFG_CKETRI,                ///< PSCFG_CKETRI
  PSCFG_ODTTRI,                ///< PSCFG_ODTTRI
  PSCFG_CSTRI,                 ///< PSCFG_CSTRI
  PSCFG_GEN_END                ///< PSCFG_GEN_END
} PSCFG_TYPE;

/// Dimm types
typedef enum {
  UDIMM_TYPE = 0x01,           ///< UDIMM_TYPE
  RDIMM_TYPE = 0x02,           ///< RDIMM_TYPE
  SODIMM_TYPE = 0x04,          ///< SODIMM_TYPE
  LRDIMM_TYPE = 0x08,          ///< LRDIMM_TYPE
  SODWN_SODIMM_TYPE = 0x10,    ///< SODWN_SODIMM_TYPE
  DT_DONT_CARE = 0xFF          ///< DT_DONT_CARE
} DIMM_TYPE;

/// Number of DRAM devices or DIMM slots
typedef enum {
  _1DIMM = 0x01,               ///< _1DIMM
  _2DIMM = 0x02,               ///< _2DIMM
  _3DIMM = 0x04,               ///< _3DIMM
  _4DIMM = 0x08,               ///< _4DIMM
  _DIMM_NONE = 0xF0,           ///< _DIMM_NONE (no DIMM slot)
  NOD_DONT_CARE = 0xFF         ///< NOD_DONT_CARE
} NOD_SUPPORTED;

/// Motherboard layer type
typedef enum {
  _4LAYERS = 0x01,             ///< 4 Layers
  _6LAYERS = 0x02,             ///< 6 Layers
  MBL_DONT_CARE = 0xFF         ///< MBL_DONT_CARE
} MB_LAYER_TYPE;

/// Motherboard power type
typedef enum {
  LPM = 0x01,                  ///< Low power motherboard
  HPM = 0x02,                  ///< High power motherboard
  MBP_DONT_CARE = 0xFF         ///< MBP_DONT_CARE
} MB_POWER_TYPE;


/// Structures use to pass system Logical CPU-ID
typedef struct {
  IN OUT   UINT16 Family;             ///< Indicates logical ID Family
  IN OUT   UINT16 Revision;           ///< Indicates logical ID Revision
} APCB_SOC_LOGICAL_ID;

/// Table header related definitions
typedef struct {
  PSCFG_TYPE PSCType;           ///< PSC Type
  DIMM_TYPE DimmType;           ///< Dimm Type
  NOD_SUPPORTED NumOfDimm;      ///< Numbef of dimm
  APCB_SOC_LOGICAL_ID LogicalCpuid;  ///< Logical Cpuid
  _1BYTE_ALIGN UINT8 PackageType;            ///< Package Type
  TECHNOLOGY_TYPE TechType;     ///< Technology type
  MB_LAYER_TYPE MotherboardLayer; ///< Motherboard layer type
  MB_POWER_TYPE MotherboardPower; ///< Motherboard power type
} PSC_TBL_HEADER;

/// Table entry
typedef struct {
  PSC_TBL_HEADER Header;        ///< PSC_TBL_HEADER
  _1BYTE_ALIGN UINT8 TableSize;              ///< Table size
  VOID *TBLPtr;                 ///< Pointer of the table
} PSC_TBL_ENTRY;

#define PT_DONT_CARE 0xFF
#define NP 1
#define V1_5  1
#define V1_35  2
#define V1_25  4
#define V1_2   1
#define V_TBD1 2
#define V_TBD2 4

#define VOLT_ALL (7)
#define DIMM_SR 2
#define DIMM_DR 4
#define DIMM_QR 8
#define DIMM_LR 2
#define R0 1
#define R1 2
#define R2 4
#define R3 8
#define CH_A 0x01
#define CH_B 0x02
#define CH_C 0x04
#define CH_D 0x08
#define CH_ALL 0x0F
#define DEVWIDTH_16 1
#define DEVWIDTH_32 2

/// CAD Bus configuration
typedef struct {
  _4BYTE_ALIGN UINT32 DimmPerCh;     ///< Bitmap of Dimm slot per chanel
  _4BYTE_ALIGN UINT32 DDRrate;      ///< Bitmap of DDR rate
  _4BYTE_ALIGN UINT32 VDDIO;         ///< Bitmap of VDDIO
  _4BYTE_ALIGN UINT32 Dimm0;         ///< Bitmap of rank type of Dimm0
  _4BYTE_ALIGN UINT32 Dimm1;         ///< Bitmap of rank type of Dimm1

  _4BYTE_ALIGN UINT16 GearDownMode;  ///< GearDownMode
  _4BYTE_ALIGN UINT16 SlowMode;      ///< SlowMode
  _4BYTE_ALIGN UINT32 AddrCmdCtl;   ///< AddrCmdCtl

  _1BYTE_ALIGN UINT8  CkeStrength;     ///< CKE drive strength
  _1BYTE_ALIGN UINT8  CsOdtStrength;   ///< CS ODT drive strength
  _1BYTE_ALIGN UINT8  AddrCmdStrength; ///< Addr Cmd drive strength
  _1BYTE_ALIGN UINT8  ClkStrength;     ///< CLK drive strength
} PSCFG_CADBUS_ENTRY;

/** PSCFG_CADBUS_ENTRY
*    @GROUPID: APCB_GROUP_MEMORY (0x1704)
*    @TYPEID: APCB_MEM_TYPE_PS_UDIMM_DDR4_CAD_BUS (0x42)
*    This data structure is used to pass memory PSCFG_CADBUS_ENTRY data
*
*    @GROUPID: APCB_GROUP_MEMORY (0x1704)
*    @TYPEID: APCB_MEM_TYPE_PS_RDIMM_DDR4_CAD_BUS (0x47)
*    This data structure is used to pass memory PSCFG_CADBUS_ENTRY data
*
*    @GROUPID: APCB_GROUP_MEMORY (0x1704)
*    @TYPEID: APCB_MEM_TYPE_PS_LRDIMM_DDR4_CAD_BUS (0x55)
*    This data structure is used to pass memory PSCFG_CADBUS_ENTRY data
*/
typedef struct {
  PSCFG_CADBUS_ENTRY          PscfgCadbusEntry1st;
  PSCFG_CADBUS_ENTRY          PscfgCadbusEntry[];
} PSCFG_CADBUS_ENTRY_STRUCTURE;

/// CAD Bus configuration for Soldered Down DRAM
typedef struct {
  _4BYTE_ALIGN UINT32 DimmPerCh;     ///< Bitmap of Dimm slot per chanel
  _4BYTE_ALIGN UINT32 DDRrate;       ///< Bitmap of DDR rate
  _4BYTE_ALIGN UINT32 VDDIO;         ///< Bitmap of VDDIO
  _4BYTE_ALIGN UINT32 Dimm0;         ///< Bitmap of rank type of Dimm0
  _4BYTE_ALIGN UINT32 DevWidth;      ///< Device Width

  _4BYTE_ALIGN UINT32 SlowMode;      ///< SlowMode
  _4BYTE_ALIGN UINT32 AddrCmdCtl;    ///< AddrCmdCtl

  _1BYTE_ALIGN UINT8  CkeStrength;     ///< CKE drive strength
  _1BYTE_ALIGN UINT8  CsOdtStrength;   ///< CS ODT drive strength
  _1BYTE_ALIGN UINT8  AddrCmdStrength; ///< Addr Cmd drive strength
  _1BYTE_ALIGN UINT8  ClkStrength;     ///< CLK drive strength
} PSCFG_DRAMDN_CADBUS_ENTRY;

/// On die termination encoding
typedef enum {
  ODT_OFF = 0,     ///<  0 On die termination disabled
  ODT_60 = 1,     ///<  1 60 ohms
  ODT_120 = 2,     ///<  2 120 ohms
  ODT_40 = 3,     ///<  3 40 ohms
  ODT_20 = 4,     ///<  4 20 ohms
  ODT_30 = 5,     ///<  5 30 ohms
} ODT_ENC;

/// DDR4 Rtt_Nom termination encoding
typedef enum {
  RTTNOM_OFF = 0, ///<  0 Rtt_Nom Disabled
  RTTNOM_60 = 1, ///<  1 60 ohms
  RTTNOM_120 = 2, ///<  2 120 ohms
  RTTNOM_40 = 3, ///<  3 40 ohms
  RTTNOM_240 = 4, ///<  4 240 ohms
  RTTNOM_48 = 5, ///<  5 48 ohms
  RTTNOM_80 = 6, ///<  5 80 ohms
  RTTNOM_34 = 7, ///<  5 34 ohms
} D4_RTT_NOM_ENC;

/// DDR4 Rtt_Wr termination encoding
typedef enum {
  RTTWR_OFF = 0, ///<  0 Rtt_Nom Disabled
  RTTWR_120 = 1, ///< 1 120 ohms
  RTTWR_240 = 2, ///<  2 24 ohms
  RTTWR_HIZ = 3, ///<  3 Hi-Z
  RTTWR_80 = 4, ///<  4 80 ohms
} D4_RTT_WR_ENC;

/// DDR4 Rtt_Park termination encoding
typedef enum {
  RTTPRK_OFF = 0, ///<  0 Rtt_Park Disabled
  RTTPRK_60 = 1, ///<  1 60 ohms
  RTTPRK_120 = 2, ///<  2 120 ohms
  RTTPRK_40 = 3, ///<  3 40 ohms
  RTTPRK_240 = 4, ///<  4 240 ohms
  RTTPRK_48 = 5, ///<  5 48 ohms
  RTTPRK_80 = 6, ///<  5 80 ohms
  RTTPRK_34 = 7, ///<  5 34 ohms
} D4_RTT_PRK_ENC;

/// Data Bus configuration
typedef struct {
  _4BYTE_ALIGN UINT32 DimmPerCh;   ///< Bitmap of Dimm slot per chanel
  _4BYTE_ALIGN UINT32 DDRrate;    ///< Bitmap of DDR rate
  _4BYTE_ALIGN UINT32 VDDIO;       ///< Bitmap of VDDIO
  _4BYTE_ALIGN UINT32 Dimm0;       ///< Bitmap of rank type of Dimm0
  _4BYTE_ALIGN UINT32 DevWidth;    ///< Device Width

  _4BYTE_ALIGN UINT32 RttNom;      ///< Rtt_Nom
  _4BYTE_ALIGN UINT32 RttWr;       ///< Rtt_Wr
  _4BYTE_ALIGN UINT32 DqStrength;  ///< Data drive strength
  _4BYTE_ALIGN UINT32 DqsStrength; ///< DQS drive strength
  _4BYTE_ALIGN UINT32 OdtStrength; ///< ODT drive strength
} PSCFG_DRAMDN_DATABUS_ENTRY;

/// DDR4 Data Bus configuration
typedef struct {
  _4BYTE_ALIGN UINT8 DimmPerCh;    ///< Bitmap of Dimm slot per chanel
  _4BYTE_ALIGN UINT32 DDRrate;     ///< Bitmap of DDR rate
  _4BYTE_ALIGN UINT32 VDDIO;       ///< Bitmap of VDDIO
  _4BYTE_ALIGN UINT32 Dimm0;       ///< Bitmap of rank type of Dimm0
  _4BYTE_ALIGN UINT32 Dimm1;       ///< Bitmap of rank type of Dimm1

  _4BYTE_ALIGN UINT32 RttNom;      ///< Rtt_Nom
  _4BYTE_ALIGN UINT32 RttWr;       ///< Rtt_Wr
  _4BYTE_ALIGN UINT32 RttPark;     ///< Rtt_Park
  _4BYTE_ALIGN UINT32 DqStrength;  ///< Data drive strength
  _4BYTE_ALIGN UINT32 DqsStrength; ///< DQS drive strength
  _4BYTE_ALIGN UINT32 OdtStrength; ///< ODT drive strength
  _4BYTE_ALIGN UINT32 PMUPhyVref;  ///< Message Block Seed for Host Vref
  _4BYTE_ALIGN UINT32 VrefDq;      ///< Message Block Seed for DRAM MR6 Vref
} PSCFG_DATABUS_ENTRY_D4;

/** PSCFG_DATABUS_ENTRY_D4
*    @GROUPID: APCB_GROUP_MEMORY (0x1704)
*    @TYPEID: APCB_MEM_TYPE_PS_UDIMM_DDR4_DATA_BUS (0x43)
*    APCB_GROUP_MEMORY  APCB_MEM_TYPE_PS_UDIMM_DDR4_DATA_BUS
*    DDR4 Data Bus configuration
*
*    @GROUPID: APCB_GROUP_MEMORY (0x1704)
*    @TYPEID: APCB_MEM_TYPE_PS_RDIMM_DDR4_DATA_BUS (0x48)
*    APCB_GROUP_MEMORY  APCB_MEM_TYPE_PS_RDIMM_DDR4_DATA_BUS
*    DDR4 Data Bus configuration
*
*    @GROUPID: APCB_GROUP_MEMORY (0x1704)
*    @TYPEID: APCB_MEM_TYPE_PS_LRDIMM_DDR4_DATA_BUS (0x56)
*    APCB_GROUP_MEMORY  APCB_MEM_TYPE_PS_LRDIMM_DDR4_DATA_BUS
*    DDR4 Data Bus configuration
*/
typedef struct {
  PSCFG_DATABUS_ENTRY_D4          PscfgDatabusEntryD41st;
  PSCFG_DATABUS_ENTRY_D4          PscfgDatabusEntryD4[];
} PSCFG_DATABUS_ENTRY_D4_STRUCTURE;
//end of Mp.h

#endif // _MP_H_

