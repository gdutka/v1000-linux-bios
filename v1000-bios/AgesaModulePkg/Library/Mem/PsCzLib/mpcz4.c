/* $NoKeywords:$ */
/**
 * @file
 *
 * mpcz4.c
 *
 * Platform specific settings for CZ with DDR4
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps/CZ)
 * @e \$Revision: 329897 $ @e \$Date: 2015-10-29 19:48:28 +0800 (Thu, 29 Oct 2015) $
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



#define FILECODE LIBRARY_MEM_PSCZLIB_MPCZ4_FILECODE
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
//  Dimm0, Dimm1     RdODTCSHigh, RdODTCSLow, WrODTCSHigh,  WrODTCSLow
//
STATIC CONST PSCFG_2D_ODTPAT_ENTRY CZOdtPat4[] = {
  {NP,      DIMM_SR,   0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  {NP,      DIMM_DR,   0x00000000, 0x00000000, 0x00000000, 0x08020000 },
  {DIMM_SR, NP,        0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  {DIMM_DR, NP,        0x00000000, 0x00000000, 0x00000000, 0x00000000 },
  {DIMM_SR, DIMM_SR,   0x00000000, 0x00010002, 0x00000000, 0x00030003 },
  {DIMM_SR, DIMM_DR,   0x00000000, 0x01010002, 0x00000000, 0x09030003 },
  {DIMM_DR, DIMM_SR,   0x00000000, 0x00010202, 0x00000000, 0x00030603 },
  {DIMM_DR, DIMM_DR,   0x00000000, 0x01010202, 0x00000000, 0x09030603 },
};
CONST PSC_TBL_ENTRY CZOdtPatTblEnt4 = {
   {PSCFG_ODT_PAT_2D, DT_DONT_CARE, NOD_DONT_CARE, {AMD_FAMILY_15_CZ, AMD_REV_F15_ALL}, PT_DONT_CARE, DDR4_TECHNOLOGY, MBL_DONT_CARE, MBP_DONT_CARE, DQS_ROUTING_ANY},
   sizeof (CZOdtPat4) / sizeof (PSCFG_2D_ODTPAT_ENTRY),
   (VOID *)&CZOdtPat4
};

