/* $NoKeywords:$ */
/**
 * @file
 *
 * mpUcz4.c
 *
 * Platform specific settings for CZ DDR4 UDIMM system
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps/CZ)
 * @e \$Revision: 334519 $ @e \$Date: 2016-01-15 15:48:51 +0800 (Fri, 15 Jan 2016) $
 *
 **/
/*****************************************************************************
  *
  * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
  *
  *******************************************************************************
  *
 */

#include "AGESA.h"
#include "mport.h"
#include "ma.h"
#include "mm.h"
#include "mn.h"
#include "mp.h"
#include "PlatformMemoryConfiguration.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEM_PSCZLIB_MPUCZ4_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/*
 *-----------------------------------------------------------------------------
 *                                EXPORTED FUNCTIONS
 *
 *-----------------------------------------------------------------------------
 */
// DDR4 Max Freq UDIMM
// Format :
// DimmPerCh,   Dimms,   SR,   DR,   QR,   Speed1_5V,   Speed1_35V,   Speed1_25V
//
STATIC CONST PSCFG_MAXFREQ_ENTRY ROMDATA MaxFreqCZUDIMMD4[] = {
  // DimmPerCh, Dimms, SR, DR, Speed1_2V
  {_1DIMM, 1, 1, 0, 0, DDR2400_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY},
  {_1DIMM, 1, 0, 1, 0, DDR2400_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY},
  {_2DIMM, 1, 1, 0, 0, DDR2400_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY},
  {_2DIMM, 1, 0, 1, 0, DDR2133_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY},
  {_2DIMM, 2, 2, 0, 0, DDR1866_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY},
  {_2DIMM, 2, 1, 1, 0, DDR1866_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY},
  {_2DIMM, 2, 0, 2, 0, DDR1866_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY},
};
CONST PSC_TBL_ENTRY CZMaxFreqTblEntUD4 = {
   {PSCFG_MAXFREQ, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_ALL}, PT_DONT_CARE, DDR4_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_ROUTING_ANY},
   sizeof (MaxFreqCZUDIMMD4) / sizeof (PSCFG_MAXFREQ_ENTRY),
   (VOID *)&MaxFreqCZUDIMMD4
};

// CAD Bus configuration for UDIMM (DQS routed in tree)
// Format :
// DimmPerCh, DDRrate,   VDDIO,                Dimm0,             Dimm1,  Slow, AddrCmdTmg,  CKE, CS/ODT, AddrCmd, CLK driver strength
//
STATIC CONST PSCFG_CADBUS_ENTRY ROMDATA CadBusCZUDIMMTreeD4[] = {
  {1, DDR1333 +
      DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,            V1_2,              DIMM_SR,                NP,   0,   0x00000000, 0x1F,   0x1F,    0x1F, 0x1F},
  {1, DDR1333,            V1_2,              DIMM_DR,                NP,   0,   0x00360000, 0x1F,   0x1F,    0x1F, 0x1F},
  {1, DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,            V1_2,              DIMM_DR,                NP,   1,   0x00000000, 0x1F,   0x1F,    0x1F, 0x1F},
  {2, DDR1333 +
      DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,            V1_2,                   NP,           DIMM_SR,   0,   0x00000000, 0x1F,   0x1F,    0x1F, 0x1F},
  {2, DDR1333,            V1_2,                   NP,           DIMM_DR,   0,   0x00360000, 0x1F,   0x1F,    0x1F, 0x1F},
  {2, DDR1333 +
      DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,            V1_2,              DIMM_SR,                NP,   0,   0x00000000, 0x1F,   0x1F,    0x1F, 0x1F},
  {2, DDR1333 +
      DDR1600 +
      DDR1866,            V1_2,    DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR,   1,   0x00000000, 0x1F,   0x1F,    0x3F, 0x1F},
  {2, DDR1333,            V1_2,              DIMM_DR,                NP,   0,   0x00360000, 0x1F,   0x1F,    0x1F, 0x1F},
  {2, DDR1600 +
      DDR1866 +
      DDR2133,            V1_2,                   NP,           DIMM_DR,   1,   0x00000000, 0x1F,   0x1F,    0x1F, 0x1F},
  {2, DDR1600 +
      DDR1866 +
      DDR2133,            V1_2,              DIMM_DR,                NP,   1,   0x00000000, 0x1F,   0x1F,    0x1F, 0x1F},
  {2, DDR2133,            V1_2,              DIMM_SR,           DIMM_SR,   1,   0x00000000, 0x1F,   0x1F,    0x3F, 0x1F},
};
CONST PSC_TBL_ENTRY CZCadBusTblEnUTreeD4 = {
   {PSCFG_CADBUS, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_CZ_A0 | AMD_REV_F15_CZ_A1}, PT_DONT_CARE, DDR4_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_TREE},
   sizeof (CadBusCZUDIMMTreeD4) / sizeof (PSCFG_CADBUS_ENTRY),
   (VOID *)&CadBusCZUDIMMTreeD4
};

// Data Bus configuration for UDIMM (DQS routed in tree) (CZ A0 Only)
// Format :
// DimmPerCh, DDRrate,   VDDIO,      Dimm0,   Dimm1,   RttNom,    RttWr,    RttPark,   DQ,  DQS,  ODT strength
//
STATIC CONST PSCFG_DATABUS_ENTRY_D4 ROMDATA DataBusCZUDIMMTreeD4_A0[] = {
  {1, DDR1333,            V1_2,    DIMM_SR,      NP, ODT_OFF, RTTWR_OFF,  RTTPRK_48, 0x70, 0x70, 0xF},
  {1, DDR1333,            V1_2,    DIMM_DR,      NP, ODT_OFF,  RTTWR_80, RTTPRK_240, 0x70, 0x70, 0xF},
  {1, DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,            V1_2,    DIMM_SR,      NP, ODT_OFF, RTTWR_OFF,  RTTPRK_48, 0x7F, 0x7F, 0xF},
  {1, DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,             V1_2,   DIMM_DR,      NP, ODT_OFF,  RTTWR_80, RTTPRK_240, 0x7F, 0x7F, 0xF},
  {2, DDR1333,             V1_2,        NP, DIMM_SR, ODT_OFF, RTTWR_OFF,  RTTPRK_48, 0x70, 0x70, 0xF},
  {2, DDR1333,             V1_2,        NP, DIMM_DR, ODT_OFF,  RTTWR_80, RTTPRK_240, 0x70, 0x70, 0xF},
  {2, DDR1333,             V1_2,   DIMM_SR,      NP, ODT_OFF, RTTWR_OFF,  RTTPRK_48, 0x70, 0x70, 0xF},
  {2, DDR1333 +
      DDR1600 +
      DDR1866 +
      DDR2133,             V1_2,   DIMM_SR, DIMM_SR,  ODT_40,  RTTWR_80, RTTPRK_OFF, 0x75, 0x75, 0xF},
  {2, DDR1333 +
      DDR1600 +
      DDR1866,             V1_2,   DIMM_SR, DIMM_DR,  ODT_40,  RTTWR_80,  RTTPRK_40, 0x75, 0x75, 0xF},
  {2, DDR1333,             V1_2,   DIMM_DR,      NP, ODT_OFF,  RTTWR_80, RTTPRK_240, 0x70, 0x70, 0xF},
  {2, DDR1333 +
      DDR1600 +
      DDR1866,             V1_2,   DIMM_DR, DIMM_SR,  ODT_40,  RTTWR_80,  RTTPRK_40, 0x75, 0x75, 0xF},
  {2, DDR1333 +
      DDR1600 +
      DDR1866,             V1_2,   DIMM_DR, DIMM_DR,  ODT_40,  RTTWR_80, RTTPRK_OFF, 0x75, 0x75, 0xF},
  {2, DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,             V1_2,        NP, DIMM_SR, ODT_OFF, RTTWR_OFF,  RTTPRK_48, 0x7F, 0x7F, 0xF},
  {2, DDR1600 +
      DDR1866 +
      DDR2133,             V1_2,        NP, DIMM_DR, ODT_OFF,  RTTWR_80, RTTPRK_240, 0x7F, 0x7F, 0xF},
  {2, DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,             V1_2,   DIMM_SR,      NP, ODT_OFF, RTTWR_OFF,  RTTPRK_48, 0x7F, 0x7F, 0xF},
  {2, DDR1600 +
      DDR1866 +
      DDR2133,             V1_2,   DIMM_DR,      NP, ODT_OFF,  RTTWR_80, RTTPRK_240, 0x7F, 0x7F, 0xF}
};
CONST PSC_TBL_ENTRY CZDataBusTblEnUTreeD4_A0 = {
   {PSCFG_DATABUS, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_CZ_A0}, PT_DONT_CARE, DDR4_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_TREE},
   sizeof (DataBusCZUDIMMTreeD4_A0) / sizeof (PSCFG_DATABUS_ENTRY_D4),
   (VOID *)&DataBusCZUDIMMTreeD4_A0
};

// Data Bus configuration for UDIMM (DQS routed in tree) (CZ A1 or Later)
// Format :
// DimmPerCh, DDRrate,   VDDIO,              Dimm0,             Dimm1,  RttNom,     RttWr,   RttPark,   DQ,  DQS, ODT strength
//
STATIC CONST PSCFG_DATABUS_ENTRY_D4 ROMDATA DataBusCZUDIMMTreeD4[] = {
  {1, DDR1333,             V1_2,           DIMM_SR,                NP, ODT_OFF, RTTWR_OFF,  RTTPRK_48, 0x70, 0x70, 0x30},
  {1, DDR1333,             V1_2,           DIMM_DR,                NP, ODT_OFF,  RTTWR_80, RTTPRK_240, 0x70, 0x70, 0x30},
  {1, DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,             V1_2,           DIMM_SR,                NP, ODT_OFF, RTTWR_OFF,  RTTPRK_48, 0x7F, 0x7F, 0x30},
  {1, DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,             V1_2,           DIMM_DR,                NP, ODT_OFF,  RTTWR_80, RTTPRK_240, 0x7F, 0x7F, 0x30},
  {2, DDR1333,             V1_2,                NP,           DIMM_SR, ODT_OFF, RTTWR_OFF,  RTTPRK_48, 0x70, 0x70, 0x30},
  {2, DDR1333,             V1_2,                NP,           DIMM_DR, ODT_OFF,  RTTWR_80, RTTPRK_240, 0x70, 0x70, 0x30},
  {2, DDR1333,             V1_2,           DIMM_SR,                NP, ODT_OFF, RTTWR_OFF,  RTTPRK_48, 0x70, 0x70, 0x30},
  {2, DDR1333 +
      DDR1600 +
      DDR1866,             V1_2, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR,  ODT_40,  RTTWR_80, RTTPRK_OFF, 0x75, 0x75, 0x30},
  {2, DDR1333,             V1_2,           DIMM_DR,                NP, ODT_OFF,  RTTWR_80, RTTPRK_240, 0x70, 0x70, 0x30},
  {2, DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,             V1_2,                NP,           DIMM_SR, ODT_OFF, RTTWR_OFF,  RTTPRK_48, 0x7F, 0x7F, 0x30},
  {2, DDR1600 +
      DDR1866 +
      DDR2133,             V1_2,                NP,           DIMM_DR, ODT_OFF,  RTTWR_80, RTTPRK_240, 0x7F, 0x7F, 0x30},
  {2, DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,             V1_2,           DIMM_SR,                NP, ODT_OFF, RTTWR_OFF,  RTTPRK_48, 0x7F, 0x7F, 0x30},
  {2, DDR1600 +
      DDR1866 +
      DDR2133,             V1_2,           DIMM_DR,                NP, ODT_OFF,  RTTWR_80, RTTPRK_240, 0x7F, 0x7F, 0x30},
  {2, DDR2133,
                           V1_2,           DIMM_SR,           DIMM_SR,  ODT_40,  RTTWR_80, RTTPRK_OFF, 0x75, 0x75, 0x30},
};
CONST PSC_TBL_ENTRY CZDataBusTblEnUTreeD4 = {
   {PSCFG_DATABUS, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_CZ_A1}, PT_DONT_CARE, DDR4_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_TREE},
   sizeof (DataBusCZUDIMMTreeD4) / sizeof (PSCFG_DATABUS_ENTRY_D4),
   (VOID *)&DataBusCZUDIMMTreeD4
};

// CAD Bus configuration for UDIMM (DQS routed in daisy chain)
// Format :
// DimmPerCh, DDRrate,   VDDIO,                Dimm0,             Dimm1,  Slow, AddrCmdTmg,  CKE, CS/ODT, AddrCmd, CLK driver strength
//
STATIC CONST PSCFG_CADBUS_ENTRY ROMDATA CadBusCZUDIMMDCD4[] = {
  {1, DDR1333 +
      DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,            V1_2,              DIMM_SR,                NP,   0,   0x00000000, 0x1F,   0x1F,    0x1F, 0x1F},
  {1, DDR1333,            V1_2,              DIMM_DR,                NP,   0,   0x00360000, 0x1F,   0x1F,    0x1F, 0x1F},
  {1, DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,            V1_2,              DIMM_DR,                NP,   1,   0x00000000, 0x1F,   0x1F,    0x1F, 0x1F},
  {2, DDR1333 +
      DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,            V1_2,                   NP,           DIMM_SR,   0,   0x00000000, 0x1F,   0x1F,    0x1F, 0x1F},
  {2, DDR1333,            V1_2,                   NP,           DIMM_DR,   0,   0x00360000, 0x1F,   0x1F,    0x1F, 0x1F},
  {2, DDR1333 +
      DDR1600 +
      DDR1866,            V1_2,    DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR,   1,   0x00000000, 0x1F,   0x1F,    0x3F, 0x1F},
  {2, DDR1600 +
      DDR1866 +
      DDR2133,            V1_2,                   NP,           DIMM_DR,   1,   0x00000000, 0x1F,   0x1F,    0x1F, 0x1F},
  {2, DDR2133,            V1_2,              DIMM_SR,           DIMM_SR,   1,   0x00000000, 0x1F,   0x1F,    0x3F, 0x1F},
};
CONST PSC_TBL_ENTRY CZCadBusTblEnUDCD4 = {
   {PSCFG_CADBUS, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_CZ_A0 | AMD_REV_F15_CZ_A1}, PT_DONT_CARE, DDR4_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_DAISY_CHAIN},
   sizeof (CadBusCZUDIMMDCD4) / sizeof (PSCFG_CADBUS_ENTRY),
   (VOID *)&CadBusCZUDIMMDCD4
};

// Data Bus configuration for UDIMM (DQS routed in daisy chain) (CZ A0 Only)
// Format :
// DimmPerCh, DDRrate,   VDDIO,      Dimm0,   Dimm1,   RttNom,    RttWr,    RttPark,   DQ,  DQS,  ODT strength
//
STATIC CONST PSCFG_DATABUS_ENTRY_D4 ROMDATA DataBusCZUDIMMDCD4_A0[] = {
  {1, DDR1333,            V1_2,    DIMM_SR,      NP, ODT_OFF, RTTWR_OFF,  RTTPRK_48, 0x70, 0x70, 0xF},
  {1, DDR1333,            V1_2,    DIMM_DR,      NP, ODT_OFF,  RTTWR_80, RTTPRK_240, 0x70, 0x70, 0xF},
  {1, DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,            V1_2,    DIMM_SR,      NP, ODT_OFF, RTTWR_OFF,  RTTPRK_48, 0x7F, 0x7F, 0xF},
  {1, DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,             V1_2,   DIMM_DR,      NP, ODT_OFF,  RTTWR_80, RTTPRK_240, 0x7F, 0x7F, 0xF},
  {2, DDR1333,             V1_2,        NP, DIMM_SR, ODT_OFF, RTTWR_OFF,  RTTPRK_48, 0x70, 0x70, 0xF},
  {2, DDR1333,             V1_2,        NP, DIMM_DR, ODT_OFF,  RTTWR_80, RTTPRK_240, 0x70, 0x70, 0xF},
  {2, DDR1333 +
      DDR1600 +
      DDR1866 +
      DDR2133,             V1_2,   DIMM_SR, DIMM_SR,  ODT_40,  RTTWR_80, RTTPRK_OFF, 0x75, 0x75, 0xF},
  {2, DDR1333 +
      DDR1600 +
      DDR1866,             V1_2,   DIMM_SR, DIMM_DR,  ODT_40,  RTTWR_80,  RTTPRK_40, 0x75, 0x75, 0xF},
  {2, DDR1333 +
      DDR1600 +
      DDR1866,             V1_2,   DIMM_DR, DIMM_SR,  ODT_40,  RTTWR_80,  RTTPRK_40, 0x75, 0x75, 0xF},
  {2, DDR1333 +
      DDR1600 +
      DDR1866,             V1_2,   DIMM_DR, DIMM_DR,  ODT_40,  RTTWR_80, RTTPRK_OFF, 0x75, 0x75, 0xF},
  {2, DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,             V1_2,        NP, DIMM_SR, ODT_OFF, RTTWR_OFF,  RTTPRK_48, 0x7F, 0x7F, 0xF},
  {2, DDR1600 +
      DDR1866 +
      DDR2133,             V1_2,        NP, DIMM_DR, ODT_OFF,  RTTWR_80, RTTPRK_240, 0x7F, 0x7F, 0xF},
};
CONST PSC_TBL_ENTRY CZDataBusTblEnUDCD4_A0 = {
   {PSCFG_DATABUS, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_CZ_A0}, PT_DONT_CARE, DDR4_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_DAISY_CHAIN},
   sizeof (DataBusCZUDIMMDCD4_A0) / sizeof (PSCFG_DATABUS_ENTRY_D4),
   (VOID *)&DataBusCZUDIMMDCD4_A0
};

// Data Bus configuration for UDIMM (DQS routed in daisy chain) (CZ A1 or Later)
// Format :
// DimmPerCh, DDRrate,   VDDIO,              Dimm0,             Dimm1,  RttNom,     RttWr,   RttPark,   DQ,  DQS, ODT strength
//
STATIC CONST PSCFG_DATABUS_ENTRY_D4 ROMDATA DataBusCZUDIMMDCD4[] = {
  {1, DDR1333,             V1_2,           DIMM_SR,                NP, ODT_OFF, RTTWR_OFF,  RTTPRK_48, 0x70, 0x70, 0x30},
  {1, DDR1333,             V1_2,           DIMM_DR,                NP, ODT_OFF,  RTTWR_80, RTTPRK_240, 0x70, 0x70, 0x30},
  {1, DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,             V1_2,           DIMM_SR,                NP, ODT_OFF, RTTWR_OFF,  RTTPRK_48, 0x7F, 0x7F, 0x30},
  {1, DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,             V1_2,           DIMM_DR,                NP, ODT_OFF,  RTTWR_80, RTTPRK_240, 0x7F, 0x7F, 0x30},
  {2, DDR1333,             V1_2,                NP,           DIMM_SR, ODT_OFF, RTTWR_OFF,  RTTPRK_48, 0x70, 0x70, 0x30},
  {2, DDR1333,             V1_2,                NP,           DIMM_DR, ODT_OFF,  RTTWR_80, RTTPRK_240, 0x70, 0x70, 0x30},
  {2, DDR1333 +
      DDR1600 +
      DDR1866,             V1_2, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR,  ODT_40,  RTTWR_80, RTTPRK_OFF, 0x75, 0x75, 0x30},
  {2, DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,             V1_2,                NP,           DIMM_SR, ODT_OFF, RTTWR_OFF,  RTTPRK_48, 0x7F, 0x7F, 0x30},
  {2, DDR1600 +
      DDR1866 +
      DDR2133,             V1_2,                NP,           DIMM_DR, ODT_OFF,  RTTWR_80, RTTPRK_240, 0x7F, 0x7F, 0x30},
  {2, DDR2133,             V1_2,           DIMM_SR,           DIMM_SR,  ODT_40,  RTTWR_80, RTTPRK_OFF, 0x75, 0x75, 0x30},
};
CONST PSC_TBL_ENTRY CZDataBusTblEnUDCD4 = {
   {PSCFG_DATABUS, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_CZ_A1}, PT_DONT_CARE, DDR4_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_DAISY_CHAIN},
   sizeof (DataBusCZUDIMMDCD4) / sizeof (PSCFG_DATABUS_ENTRY_D4),
   (VOID *)&DataBusCZUDIMMDCD4
};

// CAD Bus configuration for UDIMM (DQS routed in tree)
// Format :
// DimmPerCh, DDRrate,   VDDIO,                Dimm0,             Dimm1,  Slow, AddrCmdTmg,  CKE, CS/ODT, AddrCmd, CLK driver strength
//
STATIC CONST PSCFG_CADBUS_ENTRY ROMDATA CadBusBRUDIMMTreeD4[] = {
  {1, DDR1333 +
      DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,            V1_2,              DIMM_SR,                NP,   0,   0x00000000, 0x1F,   0x1F,    0x1F, 0x1F},
  {1, DDR1333,            V1_2,              DIMM_DR,                NP,   0,   0x00360000, 0x1F,   0x1F,    0x1F, 0x1F},
  {1, DDR1600 +
      DDR1866 +
      DDR2133,            V1_2,              DIMM_DR,                NP,   1,   0x00000000, 0x1F,   0x1F,    0x1F, 0x1F},
  {1, DDR2400,            V1_2,              DIMM_DR,                NP,   1,   0x0030342F, 0x1F,   0x1F,    0x1F, 0x1F},
  {2, DDR1333 +
      DDR1600 +
      DDR1866 +
      DDR2133,            V1_2,                   NP,           DIMM_SR,   0,   0x00000000, 0x1F,   0x1F,    0x1F, 0x1F},
  {2, DDR2400,            V1_2,                   NP,           DIMM_SR,   0,   0x0030342F, 0x1F,   0x1F,    0x1F, 0x1F},
  {2, DDR1333,            V1_2,                   NP,           DIMM_DR,   0,   0x00360000, 0x1F,   0x1F,    0x1F, 0x1F},
  {2, DDR1333 +
      DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,            V1_2,              DIMM_SR,                NP,   0,   0x00000000, 0x1F,   0x1F,    0x1F, 0x1F},
  {2, DDR1333 +
      DDR1600 +
      DDR1866,            V1_2,    DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR,   1,   0x00000000, 0x1F,   0x1F,    0x3F, 0x1F},
  {2, DDR1333,            V1_2,              DIMM_DR,                NP,   1,   0x00360000, 0x1F,   0x1F,    0x1F, 0x1F},
  {2, DDR1600 +
      DDR1866 +
      DDR2133,            V1_2,                   NP,           DIMM_DR,   1,   0x00000000, 0x1F,   0x1F,    0x1F, 0x1F},
  {2, DDR1600 +
      DDR1866 +
      DDR2133,            V1_2,              DIMM_DR,                NP,   1,   0x00000000, 0x1F,   0x1F,    0x1F, 0x1F},
  {2, DDR2133,            V1_2,              DIMM_SR,           DIMM_SR,   1,   0x00000000, 0x1F,   0x1F,    0x3F, 0x1F},
};
CONST PSC_TBL_ENTRY BRCadBusTblEnUTreeD4 = {
   {PSCFG_CADBUS, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_BR_ALL}, PT_DONT_CARE, DDR4_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_TREE},
   sizeof (CadBusBRUDIMMTreeD4) / sizeof (PSCFG_CADBUS_ENTRY),
   (VOID *)&CadBusBRUDIMMTreeD4
};

// Data Bus configuration for UDIMM  (DQS routed in tree)
// Format :
// DimmPerCh, DDRrate,    VDDIO,             Dimm0,             Dimm1,     RttNom,      RttWr,    RttPark,   DQ,  DQS, ODT strength
//
STATIC CONST PSCFG_DATABUS_ENTRY_D4 ROMDATA DataBusBRUDIMMTreeD4[] = {
  {1, DDR1333,             V1_2,           DIMM_SR,                NP,    ODT_OFF,  RTTWR_OFF,  RTTPRK_48, 0x70, 0x70, 0x30},
  {1, DDR1333,             V1_2,           DIMM_DR,                NP,    ODT_OFF,   RTTWR_80, RTTPRK_240, 0x70, 0x70, 0x30},
  {1, DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,             V1_2,           DIMM_SR,                NP,    ODT_OFF,  RTTWR_OFF,  RTTPRK_48, 0x7F, 0x7F, 0x30},
  {1, DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,             V1_2,           DIMM_DR,                NP,    ODT_OFF,   RTTWR_80, RTTPRK_240, 0x7F, 0x7F, 0x30},
  {2, DDR1333,             V1_2,                NP,           DIMM_SR,    ODT_OFF,  RTTWR_OFF,  RTTPRK_48, 0x70, 0x70, 0x30},
  {2, DDR1333,             V1_2,                NP,           DIMM_DR,    ODT_OFF,   RTTWR_80, RTTPRK_240, 0x70, 0x70, 0x30},
  {2, DDR1333,             V1_2,           DIMM_SR,                NP,    ODT_OFF,  RTTWR_OFF,  RTTPRK_60, 0x70, 0x70, 0x30},
  {2, DDR1333 +
      DDR1600 +
      DDR1866,             V1_2, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR,     ODT_40,   RTTWR_80, RTTPRK_OFF, 0x75, 0x75, 0x30},
  {2, DDR1333,             V1_2,           DIMM_DR,                NP,    ODT_OFF,   RTTWR_80, RTTPRK_240, 0x70, 0x70, 0x30},
  {2, DDR1600 +
      DDR1866 +
      DDR2133,             V1_2,                NP,           DIMM_SR,    ODT_OFF,  RTTWR_OFF,  RTTPRK_48, 0x7F, 0x7F, 0x30},
  {2, DDR2400,             V1_2,                NP,           DIMM_SR,    ODT_OFF,  RTTWR_OFF,  RTTPRK_48, 0x7F, 0x7F, 0x30},
  {2, DDR1600 +
      DDR1866,             V1_2,                NP,           DIMM_DR,    ODT_OFF,   RTTWR_80, RTTPRK_240, 0x7F, 0x7F, 0x30},
  {2, DDR2133,             V1_2,                NP,           DIMM_DR,  RTTNOM_48,   RTTWR_80, RTTPRK_240, 0x7F, 0x7F, 0x30},
  {2, DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,             V1_2,           DIMM_SR,                NP,    ODT_OFF,  RTTWR_OFF,  RTTPRK_60, 0x7F, 0x7F, 0x30},
  {2, DDR1600 +
      DDR1866 +
      DDR2133,             V1_2,           DIMM_DR,                NP,    ODT_OFF,   RTTWR_80, RTTPRK_240, 0x7F, 0x7F, 0x30},
  {2, DDR2133,             V1_2,           DIMM_SR,           DIMM_SR,     ODT_40,   RTTWR_80, RTTPRK_OFF, 0x75, 0x75, 0x30},
};
CONST PSC_TBL_ENTRY BRDataBusTblEnUTreeD4 = {
   {PSCFG_DATABUS, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_BR_ALL}, PT_DONT_CARE, DDR4_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_TREE},
   sizeof (DataBusBRUDIMMTreeD4) / sizeof (PSCFG_DATABUS_ENTRY_D4),
   (VOID *)&DataBusBRUDIMMTreeD4
};

// CAD Bus configuration for UDIMM (DQS routed in daisy chain)
// Format :
// DimmPerCh, DDRrate,   VDDIO,                Dimm0,             Dimm1,  Slow, AddrCmdTmg,  CKE, CS/ODT, AddrCmd, CLK driver strength
//
STATIC CONST PSCFG_CADBUS_ENTRY ROMDATA CadBusBRUDIMMDCD4[] = {
  {1, DDR1333 +
      DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,            V1_2,              DIMM_SR,                NP,   0,   0x00000000, 0x1F,   0x1F,    0x1F, 0x1F},
  {1, DDR1333,            V1_2,              DIMM_DR,                NP,   0,   0x00360000, 0x1F,   0x1F,    0x1F, 0x1F},
  {1, DDR1600 +
      DDR1866 +
      DDR2133,            V1_2,              DIMM_DR,                NP,   1,   0x00000000, 0x1F,   0x1F,    0x1F, 0x1F},
  {1, DDR2400,            V1_2,              DIMM_DR,                NP,   1,   0x0030342F, 0x1F,   0x1F,    0x1F, 0x1F},
  {2, DDR1333 +
      DDR1600 +
      DDR1866 +
      DDR2133,            V1_2,                   NP,           DIMM_SR,   0,   0x00000000, 0x1F,   0x1F,    0x1F, 0x1F},
  {2, DDR2400,            V1_2,                   NP,           DIMM_SR,   0,   0x0030342F, 0x1F,   0x1F,    0x1F, 0x1F},
  {2, DDR1333,            V1_2,                   NP,           DIMM_DR,   0,   0x00360000, 0x1F,   0x1F,    0x1F, 0x1F},
  {2, DDR1333 +
      DDR1600 +
      DDR1866,            V1_2,    DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR,   1,   0x00000000, 0x1F,   0x1F,    0x3F, 0x1F},
  {2, DDR1600 +
      DDR1866 +
      DDR2133,            V1_2,                   NP,           DIMM_DR,   1,   0x00000000, 0x1F,   0x1F,    0x1F, 0x1F},
  {2, DDR2133,            V1_2,              DIMM_SR,           DIMM_SR,   1,   0x00000000, 0x1F,   0x1F,    0x3F, 0x1F},
};
CONST PSC_TBL_ENTRY BRCadBusTblEnUDCD4 = {
   {PSCFG_CADBUS, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_BR_ALL}, PT_DONT_CARE, DDR4_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_DAISY_CHAIN},
   sizeof (CadBusBRUDIMMDCD4) / sizeof (PSCFG_CADBUS_ENTRY),
   (VOID *)&CadBusBRUDIMMDCD4
};

// Data Bus configuration for UDIMM  (DQS routed in daisy chain)
// Format :
// DimmPerCh, DDRrate,    VDDIO,             Dimm0,             Dimm1,     RttNom,      RttWr,    RttPark,   DQ,  DQS, ODT strength, RxDly Op, RxDly Offset, TxDly Op, TxDly Offset, Vref Op, Vref Offset
//
STATIC CONST PSCFG_DATABUS_ENTRY_D4 ROMDATA DataBusBRUDIMMDCD4[] = {
  {1, DDR1333,             V1_2,           DIMM_SR,                NP,    ODT_OFF,  RTTWR_OFF,  RTTPRK_48, 0x70, 0x70, 0x30},
  {1, DDR1333,             V1_2,           DIMM_DR,                NP,    ODT_OFF,   RTTWR_80, RTTPRK_240, 0x70, 0x70, 0x30},
  {1, DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,             V1_2,           DIMM_SR,                NP,    ODT_OFF,  RTTWR_OFF,  RTTPRK_48, 0x7F, 0x7F, 0x30},
  {1, DDR1600 +
      DDR1866 +
      DDR2133 +
      DDR2400,             V1_2,           DIMM_DR,                NP,    ODT_OFF,   RTTWR_80, RTTPRK_240, 0x7F, 0x7F, 0x30},
  {2, DDR1333,             V1_2,                NP,           DIMM_SR,    ODT_OFF,  RTTWR_OFF,  RTTPRK_48, 0x70, 0x70, 0x30},
  {2, DDR1333,             V1_2,                NP,           DIMM_DR,    ODT_OFF,   RTTWR_80, RTTPRK_240, 0x70, 0x70, 0x30},
  {2, DDR1333 +
      DDR1600 +
      DDR1866,             V1_2,           DIMM_SR,           DIMM_SR,    ODT_40,    RTTWR_80, RTTPRK_OFF, 0x75, 0x75, 0xF,  OP_ADD,  2,  OP_SUB,  4,  OP_ADD,  2},
  {2, DDR1333 +
      DDR1600 +
      DDR1866,             V1_2,           DIMM_SR,           DIMM_DR,    ODT_40,    RTTWR_80, RTTPRK_OFF, 0x75, 0x75, 0xF,  OP_ADD,  2,  OP_SUB,  4,  OP_ADD,  2},
  {2, DDR1333 +
      DDR1600 +
      DDR1866,             V1_2,           DIMM_DR,           DIMM_SR,    ODT_40,    RTTWR_80, RTTPRK_OFF, 0x75, 0x75, 0xF,  OP_ADD,  2,  OP_SUB,  4,  OP_ADD,  2},
  {2, DDR1333 +
      DDR1600 +
      DDR1866,             V1_2,           DIMM_DR,           DIMM_DR,    ODT_40,   RTTWR_120, RTTPRK_OFF, 0x75, 0x75, 0xC,  OP_ADD,  0,  OP_SUB,  4,  OP_ADD,  2},
  {2, DDR1600 +
      DDR1866 +
      DDR2133,             V1_2,                NP,           DIMM_SR,   ODT_OFF,   RTTWR_OFF,  RTTPRK_48, 0x7F, 0x7F, 0x30},
  {2, DDR2400,             V1_2,                NP,           DIMM_SR,   ODT_OFF,   RTTWR_OFF,  RTTPRK_48, 0x7F, 0x7F, 0x30},
  {2, DDR1600 +
      DDR1866,             V1_2,                NP,           DIMM_DR,   ODT_OFF,    RTTWR_80, RTTPRK_240, 0x7F, 0x7F, 0x30},
  {2, DDR2133,             V1_2,                NP,           DIMM_DR, RTTNOM_48,    RTTWR_OFF, RTTWR_OFF, 0x7F, 0x7F, 0x30},
  {2, DDR2133,             V1_2,           DIMM_SR,           DIMM_SR,    ODT_40,    RTTWR_80, RTTPRK_OFF, 0x75, 0x75, 0xF,  OP_ADD,  2,  OP_SUB,  4,  OP_ADD,  2},
};
CONST PSC_TBL_ENTRY BRDataBusTblEnUDCD4 = {
   {PSCFG_DATABUS, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_BR_ALL}, PT_DONT_CARE, DDR4_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_DAISY_CHAIN},
   sizeof (DataBusBRUDIMMDCD4) / sizeof (PSCFG_DATABUS_ENTRY_D4),
   (VOID *)&DataBusBRUDIMMDCD4
};

