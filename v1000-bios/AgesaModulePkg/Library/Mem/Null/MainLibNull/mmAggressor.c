/* $NoKeywords:$ */
/**
 * @file
 *
 * mmAggressor.c
 *
 * Main Memory Feature implementation file for aggressor initialization.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
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
#include "OptionMemory.h"
#include "Library/AmdBaseLib.h"
#include "Library/IdsLib.h"
#include "mport.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "Ids.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEM_NULL_MAINLIBNULL_MMAGGRESSOR_FILECODE

extern MEM_FEAT_BLOCK_MAIN MemFeatMain;

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
VOID
STATIC
MemMInitializeAggressorData (
  IN OUT   MEM_MAIN_DATA_BLOCK *mmPtr
  );

/*-----------------------------------------------------------------------------
*                                EXPORTED FUNCTIONS
*
*-----------------------------------------------------------------------------
*/
BOOLEAN
MemMAggressor (
  IN OUT   MEM_MAIN_DATA_BLOCK *mmPtr
  );
/* -----------------------------------------------------------------------------*/
/**
 *
 *  Find Aggressor Chipselects on all DCTs/Nodes relative to the
 *   current node.
 *
 *     @param[in,out]   *mmPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */
BOOLEAN
MemMAggressor (
  IN OUT   MEM_MAIN_DATA_BLOCK *mmPtr
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
 *  Determine Aggressor Chipselects for all DCTs on all nodes relative to the
 *   current node.
 *
 *     @param[in,out]   *mmPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 */
VOID
STATIC
MemMInitializeAggressorData (
  IN OUT   MEM_MAIN_DATA_BLOCK *mmPtr
  )
{

}

