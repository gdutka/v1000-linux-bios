/* $NoKeywords:$ */
/**
 * @file
 *
 * mtsdi3.c
 *
 * Technology Software DRAM Init for DDR3
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech/DDR3)
 * @e \$Revision: 309090 $ @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 **/
/*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/


/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */



#include "AGESA.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "mt.h"
#include "mt3.h"
#include "mtsdi3.h"
#include "mtrci3.h"
#include "merrhdl.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)
#define FILECODE LIBRARY_MEM_NULL_TECHDDR3LIBNULL_MTSDI3_FILECODE
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

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */



/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function initiates software DRAM init for  both DCTs
 *   at the same time.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

BOOLEAN
MemTDramInitSw3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function calculates the EMRS1 value
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]        Wl - Indicates if WL mode should be enabled
 *     @param[in]        TargetDIMM - DIMM target for WL
 */

VOID
MemTEMRS13 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       BOOLEAN Wl,
  IN       UINT8 TargetDIMM
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function calculates the EMRS2 value
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemTEMRS23 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function calculates the EMRS3 value
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemTEMRS33 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This sets MRS value
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemTMRS3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This send all MR commands to a rank in sequence 2-3-1-0
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       ChipSel    - Target Chip Select
 */

VOID
MemTSendAllMRCmds3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 ChipSel
  )
{

}

