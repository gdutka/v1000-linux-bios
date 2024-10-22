/* $NoKeywords:$ */
/**
 * @file
 *
 * mt.c
 *
 * Common Technology file
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech)
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
#include "mport.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE LIBRARY_MEM_NULL_TECHLIBNULL_MT_FILECODE


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
BOOLEAN
STATIC
MemTDefaultTechnologyHook (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  );
/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function is the default return for non-training technology features
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 */
BOOLEAN
MemTFeatDef (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function sets the TestFail bit for all CS that fail training.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 */
VOID
MemTMarkTrainFail (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function sets the initial controller environment before training.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemTBeginTraining (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function sets the final controller environment after training.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemTEndTraining (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function sets all the bytelanes/nibbles to the same delay value
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       Dly - Delay value to set
 *
 */

VOID
MemTSetDQSDelayAllCSR (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dly
  )
{

}
/*-----------------------------------------------------------------------------
 *
 *
 *     This function is used to intialize common technology functions
 *
 *     @param[in,out]  *TechPtr     - Pointer to the MEM_TECH_BLOCK
 * ----------------------------------------------------------------------------
 */
VOID
MemTCommonTechInit (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{

}
/*-----------------------------------------------------------------------------
 *
 *
 *     This function is an empty function used to intialize TechnologySpecificHook array
 *
 *     @param[in,out]  *TechPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  OptParam   - Optional parameter
 *
 *     @return  FALSE - always
 * ----------------------------------------------------------------------------
 */
BOOLEAN
STATIC
MemTDefaultTechnologyHook (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  )
{
  return FALSE;
}

