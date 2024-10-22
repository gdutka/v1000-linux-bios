/* $NoKeywords:$ */
/**
 * @file
 *
 * mnD3cz.c
 *
 * DDR3 Specific Northbridge functions for CZ
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/CZ)
 * @e \$Revision: 312538 $ @e \$Date: 2015-02-09 16:53:54 +0800 (Mon, 09 Feb 2015) $
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
#include "mport.h"
#include "mm.h"
#include "mn.h"
#include "mnreg.h"
#include "mnpmu.h"
#include "mncz.h"
#include "mnD3cz.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE LIBRARY_MEM_NULL_NBCZLIBNULL_MND3CZ_FILECODE

extern ROMDATA PMU_FIRMWARE PmuFirmwareD3ImageTableCZ[];
extern ROMDATA UINT8 PmuFirmwareD3ImageTableSizeCZ;
extern MEM_PSC_FLOW_BLOCK memPlatSpecFlowD3CZ;
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
 *   This function initializes member DDR3 Technology Specific functions and
 *    variables of NB block.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNInitNBDataD3CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function programs DDR mode to DDR3 for all DCTs
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNSetDdrModeD3CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}



