/* $NoKeywords:$ */
/**
 * @file},
 *
 * mpScz4.c},
 *
 * Platform specific settings for CZ DDR4 SO-DIMM system
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA},
 * @e sub-project: (Mem/Ps/CZ)
 * @e \$Revision: 325367 $ @e \$Date: 2015-08-27 09:46:56 +0800 (Thu, 27 Aug 2015) $
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

#define FILECODE LIBRARY_MEM_NULL_PSCZLIBNULL_MPSCZ4_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
// For A1 or Greater tables
#define AMD_REV_F15_CZ_GTE_A1    (~AMD_REV_F15_CZ_A0 & AMD_REV_F15_ALL)

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

STATIC CONST PSCFG_MAXFREQ_ENTRY ROMDATA MaxFreqCZSODIMMD4[] = {
  {0},
};
CONST PSC_TBL_ENTRY CZMaxFreqTblEntSOD4 = {
   {PSCFG_MAXFREQ, SODIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_CZ_A0 | AMD_REV_F15_CZ_A1}, PT_DONT_CARE, DDR4_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_ROUTING_ANY},
   sizeof (MaxFreqCZSODIMMD4) / sizeof (PSCFG_MAXFREQ_ENTRY),
   (VOID *)&MaxFreqCZSODIMMD4
};

// DDR4 Max Freq. for normal SODIMM configuration
// Format :
// DimmPerCh,   Dimms,   SR,   DR,   QR,   Speed1_2V,   Speed_TBD1,   Speed_TBD2
//
STATIC CONST PSCFG_MAXFREQ_ENTRY ROMDATA MaxFreqBRSODIMMD4[] = {
  {0},
};
CONST PSC_TBL_ENTRY BRMaxFreqTblEntSOD4 = {
   {PSCFG_MAXFREQ, SODIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_BR_ALL}, PT_DONT_CARE, DDR4_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_ROUTING_ANY},
   sizeof (MaxFreqBRSODIMMD4) / sizeof (PSCFG_MAXFREQ_ENTRY),
   (VOID *)&MaxFreqBRSODIMMD4
};

// CAD Bus configuration for SO-DIMM (DQS routed in tree)
// Format :
// DimmPerCh, DDRrate,   VDDIO,      Dimm0,   Dimm1,  Slow, AddrCmdTmg, CKE, CS/ODT, AddrCmd, CLK driver strength
//
STATIC CONST PSCFG_CADBUS_ENTRY ROMDATA CadBusCZSODIMMTree4[] = {
  {0},
};

CONST PSC_TBL_ENTRY CZCadBusTblEnSOTree4 = {
   {PSCFG_CADBUS, SODIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_ALL}, PT_DONT_CARE, DDR4_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_TREE},
   sizeof (CadBusCZSODIMMTree4) / sizeof (PSCFG_CADBUS_ENTRY),
   (VOID *)&CadBusCZSODIMMTree4
};


// Data Bus configuration for SO-DIMM (DQS routed in tree) (CZ A0 Only)
// Format :
// DimmPerCh, DDRrate,                      VDDIO, Dimm0,   Dimm1,                       RttNom,     RttWr,     Rtt_Park,   DQ,   DQS, ODT strength
//
STATIC CONST PSCFG_DATABUS_ENTRY_D4 ROMDATA DataBusCZSODIMMTree4_A0[] = {
  {0},

};

CONST PSC_TBL_ENTRY CZDataBusTblEnSOTree4_A0 = {
   {PSCFG_DATABUS, SODIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_CZ_A0}, PT_DONT_CARE, DDR4_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_TREE},
   sizeof (DataBusCZSODIMMTree4_A0) / sizeof (PSCFG_DATABUS_ENTRY_D4),
   (VOID *)&DataBusCZSODIMMTree4_A0
};

// Data Bus configuration for SO-DIMM  (DQS routed in tree) (CZ A1 or Later)
// Format :
// DimmPerCh, DDRrate,                      VDDIO, Dimm0,   Dimm1,                       RttNom,     RttWr,     Rtt_Park,   DQ,   DQS, ODT strength
//
STATIC CONST PSCFG_DATABUS_ENTRY_D4 ROMDATA DataBusCZSODIMMTree4[] = {
  {0},
};

CONST PSC_TBL_ENTRY CZDataBusTblEnSOTree4 = {
   {PSCFG_DATABUS, SODIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_CZ_GTE_A1}, PT_DONT_CARE, DDR4_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_TREE},
   sizeof (DataBusCZSODIMMTree4) / sizeof (PSCFG_DATABUS_ENTRY_D4),
   (VOID *)&DataBusCZSODIMMTree4
};

// CAD Bus configuration for SO-DIMM (DQS routed in daisy chain)
// Format :
// DimmPerCh, DDRrate,   VDDIO,      Dimm0,   Dimm1,  Slow, AddrCmdTmg, CKE, CS/ODT, AddrCmd, CLK driver strength
//
STATIC CONST PSCFG_CADBUS_ENTRY ROMDATA CadBusCZSODIMMDC4[] = {
  {0},
};

CONST PSC_TBL_ENTRY CZCadBusTblEnSODC4 = {
   {PSCFG_CADBUS, SODIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_ALL}, PT_DONT_CARE, DDR4_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_DAISY_CHAIN},
   sizeof (CadBusCZSODIMMDC4) / sizeof (PSCFG_CADBUS_ENTRY),
   (VOID *)&CadBusCZSODIMMDC4
};


// Data Bus configuration for SO-DIMM (DQS routed in daisy chain) (CZ A0 Only)
// Format :
// DimmPerCh, DDRrate,                      VDDIO, Dimm0,   Dimm1,                       RttNom,     RttWr,     Rtt_Park,   DQ,   DQS, ODT strength
//
STATIC CONST PSCFG_DATABUS_ENTRY_D4 ROMDATA DataBusCZSODIMMDC4_A0[] = {
  {0},

};

CONST PSC_TBL_ENTRY CZDataBusTblEnSODC4_A0 = {
   {PSCFG_DATABUS, SODIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_CZ_A0}, PT_DONT_CARE, DDR4_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_DAISY_CHAIN},
   sizeof (DataBusCZSODIMMDC4_A0) / sizeof (PSCFG_DATABUS_ENTRY_D4),
   (VOID *)&DataBusCZSODIMMDC4_A0
};

// Data Bus configuration for SO-DIMM  (DQS routed in daisy chain) (CZ A1 or Later)
// Format :
// DimmPerCh, DDRrate,                      VDDIO, Dimm0,   Dimm1,                       RttNom,     RttWr,     Rtt_Park,   DQ,   DQS, ODT strength
//
STATIC CONST PSCFG_DATABUS_ENTRY_D4 ROMDATA DataBusCZSODIMMDC4[] = {
  {0},
};

CONST PSC_TBL_ENTRY CZDataBusTblEnSODC4 = {
   {PSCFG_DATABUS, SODIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_CZ_GTE_A1}, PT_DONT_CARE, DDR4_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_DAISY_CHAIN},
   sizeof (DataBusCZSODIMMDC4) / sizeof (PSCFG_DATABUS_ENTRY_D4),
   (VOID *)&DataBusCZSODIMMDC4
};

