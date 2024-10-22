/* $NoKeywords:$ */
/**
 * @file
 *
 * mpcz3.c
 *
 * Platform specific settings for CZ
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

CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)



#define FILECODE LIBRARY_MEM_PSCZLIB_MPCZ3_FILECODE

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
// Common tables of CZ platform specific configuration
//

// ODT pattern
// Format:
//  Dimm0, Dimm1  RdODTCSHigh,   RdODTCSLow,   WrODTCSHigh,   WrODTCSLow
//
STATIC CONST PSCFG_2D_ODTPAT_ENTRY CZOdtPat3[] = {
  {NP, DIMM_SR, 0x00000000, 0x00000000, 0x00000000, 0x00020000},
  {NP, DIMM_DR, 0x00000000, 0x00000000, 0x00000000, 0x02080000},
  {DIMM_SR, NP, 0x00000000, 0x00000000, 0x00000000, 0x00000001},
  {DIMM_DR, NP, 0x00000000, 0x00000000, 0x00000000, 0x00000104},
  {DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, 0x00000000, 0x01010202, 0x00000000, 0x09030603}
};
CONST PSC_TBL_ENTRY CZOdtPatTblEnt3 = {
   {PSCFG_ODT_PAT_2D, DT_DONT_CARE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_ROUTING_ANY},
   sizeof (CZOdtPat3) / sizeof (PSCFG_2D_ODTPAT_ENTRY),
   (VOID *)&CZOdtPat3
};

