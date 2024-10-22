/* $NoKeywords:$ */
/**
 * @file
 *
 * mpUcz3.c
 *
 * Platform specific settings for CZ DDR3 UDIMM system
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

#define FILECODE LIBRARY_MEM_NULL_PSCZLIBNULL_MPUCZ3_FILECODE
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

//
STATIC CONST PSCFG_MAXFREQ_ENTRY ROMDATA MaxFreqCZUDIMMD3[] = {
  {0}
};
CONST PSC_TBL_ENTRY CZMaxFreqTblEntUD3 = {
   {PSCFG_MAXFREQ, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_ROUTING_ANY},
   sizeof (MaxFreqCZUDIMMD3) / sizeof (PSCFG_MAXFREQ_ENTRY),
   (VOID *)&MaxFreqCZUDIMMD3
};

// CAD Bus configuration for UDIMM (DQS routed in tree)
// Format :
// DimmPerCh, DDRrate,   VDDIO,      Dimm0,   Dimm1,  Slow, AddrCmdTmg, CKE, CS/ODT, AddrCmd, CLK driver strength
//
STATIC CONST PSCFG_CADBUS_ENTRY ROMDATA CadBusCZUDIMMTree3[] = {
  {0},
};
CONST PSC_TBL_ENTRY CZCadBusTblEnUTree3 = {
   {PSCFG_CADBUS, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_TREE},
   sizeof (CadBusCZUDIMMTree3) / sizeof (PSCFG_CADBUS_ENTRY),
   (VOID *)&CadBusCZUDIMMTree3
};

// Data Bus configuration for UDIMM (DQS routed in tree)
// Format :
// DimmPerCh, DDRrate,   VDDIO,      Dimm0,   Dimm1,   RttNom,  RttWr,   DQ,   DQS,  ODT strength
//
STATIC CONST PSCFG_DATABUS_ENTRY_D3 ROMDATA DataBusCZUDIMMTree3[] = {
  {0},
};
CONST PSC_TBL_ENTRY CZDataBusTblEnUTree3 = {
   {PSCFG_DATABUS, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_TREE},
   sizeof (DataBusCZUDIMMTree3) / sizeof (PSCFG_DATABUS_ENTRY_D3),
   (VOID *)&DataBusCZUDIMMTree3
};

// CAD Bus configuration for UDIMM (DQS routed in daisy chain)
// Format :
// DimmPerCh, DDRrate,   VDDIO,      Dimm0,   Dimm1,  Slow, AddrCmdTmg, CKE, CS/ODT, AddrCmd, CLK driver strength
//
STATIC CONST PSCFG_CADBUS_ENTRY ROMDATA CadBusCZUDIMMDC3[] = {
  {0},
};
CONST PSC_TBL_ENTRY CZCadBusTblEnUDC3 = {
   {PSCFG_CADBUS, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_DAISY_CHAIN},
   sizeof (CadBusCZUDIMMDC3) / sizeof (PSCFG_CADBUS_ENTRY),
   (VOID *)&CadBusCZUDIMMDC3
};

// Data Bus configuration for SO-DIMM (DQS routed in daisy chain)
// Format :
// DimmPerCh, DDRrate,   VDDIO,      Dimm0,   Dimm1,   RttNom,  RttWr,   DQ,   DQS,  ODT strength
//
STATIC CONST PSCFG_DATABUS_ENTRY_D3 ROMDATA DataBusCZUDIMMDC3[] = {
  {0},
};
CONST PSC_TBL_ENTRY CZDataBusTblEnUDC3 = {
   {PSCFG_DATABUS, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_DAISY_CHAIN},
   sizeof (DataBusCZUDIMMDC3) / sizeof (PSCFG_DATABUS_ENTRY_D3),
   (VOID *)&DataBusCZUDIMMDC3
};

