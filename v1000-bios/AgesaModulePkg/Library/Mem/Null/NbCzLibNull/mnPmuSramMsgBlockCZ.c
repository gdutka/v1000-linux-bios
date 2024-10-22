/* $NoKeywords:$ */
/**
 * @file
 *
 * mnPmuSramMsgBlockCZ.c
 *
 * Common Northbridge  functions for CZ
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/CZ)
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
#include "mt.h"
#include "mnreg.h"
#include "mnpmu.h"
#include "merrhdl.h"
#include "mport.h"
#include "mnPmuSramMsgBlockCZ.h"
#include "Library/AmdHeapLib.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE LIBRARY_MEM_NULL_NBCZLIBNULL_MNPMUSRAMMSGBLOCKCZ_FILECODE

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
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *    Load the PMU SRAM Message Block to DMEM.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return  TRUE - Load the PMU SRAM Message Block successfully.
 *     @return  FALSE - Fail to load the PMU SRAM Message Block.
 */

BOOLEAN
MemNWritePmuSramMsgBlockCZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    Read the PMU SRAM Message Block from DMEM.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return  TRUE - Read the PMU SRAM Message Block successfully.
 *     @return  FALSE - Failed to read the PMU SRAM Message Block.
 */

BOOLEAN
MemNReadPmuSramMsgBlockCZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    Initialize the PMU SRAM Message Block buffer.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return  TRUE - initialize the PMU SRAM Message buffer successfully.
 *     @return  FALSE - Fail to initialize the PMU SRAM Message buffer.
 */

BOOLEAN
MemNInitPmuSramMsgBlockCZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    Free the PMU SRAM Message Block buffer.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return  TRUE - Free PMU SRAM Message buffer successfully.
 *     @return  FALSE - Fail to free PMU SRAM Message buffer.
 */

BOOLEAN
MemNPostPmuSramMsgBlockCZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return TRUE;
}

