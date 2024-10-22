/* $NoKeywords:$ */
/**
 * @file
 *
 * mttwl3.c
 *
 * Technology Phy assisted write levelization for DDR3
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
#include "Library/AmdBaseLib.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "mt.h"
#include "mp.h"
#include "mtsdi3.h"
#include "mtlrdimm3.h"
#include "merrhdl.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE LIBRARY_MEM_NULL_TECHDDR3LIBNULL_MTTWL3_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
extern MEM_PSC_FLOW_BLOCK* memPlatSpecFlowArray[];

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

BOOLEAN
STATIC
MemTWriteLevelizationHw3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Pass
  );

VOID
STATIC
MemTWLPerDimmHw3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dimm,
  IN       UINT8 Pass
  );

VOID
STATIC
MemTPrepareDIMMs3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 TargetDIMM,
  IN       BOOLEAN Wl
  );

VOID
STATIC
MemTProcConfig3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dimm,
  IN       UINT8 Pass
  );

VOID
STATIC
MemTBeginWLTrain3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dimm
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */


/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function executes first pass of Phy assisted write levelization
 *      for a specific node (DDR800).
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemTWriteLevelizationHw3Pass1 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function executes second pass of Phy assisted write levelization
 *      for a specific node (DDR1066 and above).
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemTWriteLevelizationHw3Pass2 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function prepares for Phy assisted training.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemTPreparePhyAssistedTraining (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function revert to normal settings when exiting from Phy assisted training.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemTExitPhyAssistedTraining (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return TRUE;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function executed hardware based write levelization for a specific die
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in] Pass - Pass number (1 (400Mhz) or 2 (>400Mhz))
 *
 *     @pre   Auto refresh and ZQCL must be disabled
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
STATIC
MemTWriteLevelizationHw3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Pass
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function initializes per DIMM write levelization
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in] Dimm - DIMM to be trained
 *     @param[in] Pass - Pass number (1 (400Mhz) or 2 (>400Mhz))
 *
 */

VOID
STATIC
MemTWLPerDimmHw3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dimm,
  IN       UINT8 Pass
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function prepares the DIMMS for Write Levelization
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in] TargetDIMM - DIMM to be trained
 *     @param[in] Wl - Indicates if WL mode should be enabled
 *
 */

VOID
STATIC
MemTPrepareDIMMs3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 TargetDIMM,
  IN       BOOLEAN Wl
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function programs seed values for Write Levelization
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in] Dimm - DIMM to be trained
 *     @param[in] Pass - Pass for WL training (1 - 400Mhz or 2 - >400Mhz)
 *
 */

VOID
STATIC
MemTProcConfig3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dimm,
  IN       UINT8 Pass
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function begins WL training for a specific DIMM
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in] Dimm - DIMM to be trained
 *
 */

VOID
STATIC
MemTBeginWLTrain3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dimm
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function programs register after Phy assisted training is finish.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemTExitPhyAssistedTrainingClient3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return TRUE;
}

