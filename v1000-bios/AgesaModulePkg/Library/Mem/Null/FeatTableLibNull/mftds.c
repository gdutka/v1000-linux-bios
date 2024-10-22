/* $NoKeywords:$ */
/**
 * @file
 *
 * mftds.c
 *
 * Northbridge table drive support file for DR
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/TABLE)
 * @e \$Revision: 324073 $ @e \$Date: 2015-08-11 09:45:24 +0800 (Tue, 11 Aug 2015) $
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
#include "Library/AmdBaseLib.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mftds.h"
#include "OptionMemory.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEM_NULL_FEATTABLELIBNULL_MFTDS_FILECODE

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */
#define MAX_BYTELANES_PER_CHANNEL   (8 + 1)  ///< Max Bytelanes per channel

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

VOID
SetTableValues (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_TABLE_ALIAS MTPtr
  );

BOOLEAN
SetTableValuesLoop (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_TABLE_ALIAS *MTPtr,
  IN       UINT8     time
  );

/*-----------------------------------------------------------------------------
 *
 *   This function initializes bit field translation table
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_TABLE_ALIAS structure
 *     @param[in]       time     - Indicate the timing for the register which is written.
 *
 *     @return          None
 * ----------------------------------------------------------------------------
 */
VOID
MemFInitTableDrive (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8     time
  )
{

}

/*-----------------------------------------------------------------------------
 *
 *   This function initializes bit field translation table
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *MTPtr   - Pointer to the MEM_TABLE_ALIAS structure
 *     @param[in]       time     - Indicate the timing for the register which is written.
 *
 *     @return          BOOLEAN  - TRUE:  Values were set
 *                                 FALSE: No values were set
 *
 * ----------------------------------------------------------------------------
 */
BOOLEAN
SetTableValuesLoop (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_TABLE_ALIAS *MTPtr,
  IN       UINT8     time
  )
{
  return TRUE;
}

/*-----------------------------------------------------------------------------
 *
 *   Engine for setting Table Value.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       MTPtr    - Pointer to the MEM_TABLE_ALIAS structure
 *
 *     @return      None
 * ----------------------------------------------------------------------------
 */
VOID
SetTableValues (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_TABLE_ALIAS MTPtr
  )
{

}







