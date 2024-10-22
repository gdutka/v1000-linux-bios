/* $NoKeywords:$ */
/**
 * @file
 *
 * mpScz3.c
 *
 * Platform specific settings for CZ DDR3 SO-DIMM system
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps/CZ)
 * @e \$Revision: 311790 $ @e \$Date: 2015-01-27 13:03:49 +0800 (Tue, 27 Jan 2015) $
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

#define FILECODE LIBRARY_MEM_NULL_PSCZLIBNULL_MPSCZ3_FILECODE
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

// DDR3 Max Freq. for normal SODIMM configuration
// Format :
// DimmPerCh,   Dimms,   SR,   DR,   QR,   Speed1_5V,   Speed1_35V,   Speed1_25V
//
STATIC CONST PSCFG_MAXFREQ_ENTRY ROMDATA MaxFreqCZSODIMMD3[] = {
  {0},
};
CONST PSC_TBL_ENTRY CZMaxFreqTblEntSOD3 = {
   {PSCFG_MAXFREQ, SODIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_ROUTING_ANY},
   sizeof (MaxFreqCZSODIMMD3) / sizeof (PSCFG_MAXFREQ_ENTRY),
   (VOID *)&MaxFreqCZSODIMMD3
};

// CAD Bus configuration for SO-DIMM (Routed in tree)
// Format :
// DimmPerCh, DDRrate,   VDDIO,      Dimm0,   Dimm1,  Slow, AddrCmdTmg, CKE, CS/ODT, AddrCmd, CLK driver strength
//
STATIC CONST PSCFG_CADBUS_ENTRY ROMDATA CadBusCZTree3[] = {
  {0},
};
CONST PSC_TBL_ENTRY CZCadBusTblEnSOTree3 = {
   {PSCFG_CADBUS, SODIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_TREE},
   sizeof (CadBusCZTree3) / sizeof (PSCFG_CADBUS_ENTRY),
   (VOID *)&CadBusCZTree3
};

// Data Bus configuration for SO-DIMM (Routed in tree)
// Format :
// DimmPerCh, DDRrate,   VDDIO,      Dimm0,   Dimm1,   RttNom,  RttWr,   DQ,   DQS,  ODT strength
//
STATIC CONST PSCFG_DATABUS_ENTRY_D3 ROMDATA DataBusCZSODIMMTree3[] = {
  {0},
};
CONST PSC_TBL_ENTRY CZDataBusTblEnSOTree3 = {
   {PSCFG_DATABUS, SODIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_TREE},
   sizeof (DataBusCZSODIMMTree3) / sizeof (PSCFG_DATABUS_ENTRY_D3),
   (VOID *)&DataBusCZSODIMMTree3
};

// CAD Bus configuration for SO-DIMM (Routed in daisy chain)
// Format :
// DimmPerCh, DDRrate,   VDDIO,      Dimm0,   Dimm1,  Slow, AddrCmdTmg, CKE, CS/ODT, AddrCmd, CLK driver strength
//
STATIC CONST PSCFG_CADBUS_ENTRY ROMDATA CadBusCZDC3[] = {
  {0},
};
CONST PSC_TBL_ENTRY CZCadBusTblEnSODC3 = {
   {PSCFG_CADBUS, SODIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_DAISY_CHAIN},
   sizeof (CadBusCZDC3) / sizeof (PSCFG_CADBUS_ENTRY),
   (VOID *)&CadBusCZDC3
};

// Data Bus configuration for SO-DIMM (Routed in daisy chain)
// Format :
//
STATIC CONST PSCFG_DATABUS_ENTRY_D3 ROMDATA DataBusCZSODIMMDC3[] = {
  {0},
};
CONST PSC_TBL_ENTRY CZDataBusTblEnSODC3 = {
   {PSCFG_DATABUS, SODIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_DAISY_CHAIN},
   sizeof (DataBusCZSODIMMDC3) / sizeof (PSCFG_DATABUS_ENTRY_D3),
   (VOID *)&DataBusCZSODIMMDC3
};

