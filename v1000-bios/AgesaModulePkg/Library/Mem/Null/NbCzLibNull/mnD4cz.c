/* $NoKeywords:$ */
/**
 * @file
 *
 * mnD4cz.c
 *
 * DDR4 Specific Northbridge functions for CZ
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/CZ)
 * @e \$Revision$ @e \$Date$
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
///@todo
//#include "Library/AmdBaseLib.h"
#include "OptionMemory.h"
#include "mport.h"
#include "mm.h"
#include "mn.h"
#include "mnreg.h"
#include "mnpmu.h"
#include "mncz.h"
#include "mnD4cz.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE LIBRARY_MEM_NULL_NBCZLIBNULL_MND4CZ_FILECODE

extern ROMDATA PMU_FIRMWARE PmuFirmwareD4ImageTableCZ[];
extern ROMDATA UINT8 PmuFirmwareD4ImageTableSizeCZ;
extern MEM_PSC_FLOW_BLOCK memPlatSpecFlowD4CZ;
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
 *   This function initializes member DDR4 Technology Specific functions and
 *    variables of NB block.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNInitNBDataD4CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function programs DDR mode to DDR4 for all DCTs
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNSetDdrModeD4CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}



