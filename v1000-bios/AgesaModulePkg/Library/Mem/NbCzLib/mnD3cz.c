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

#define FILECODE LIBRARY_MEM_NBCZLIB_MND3CZ_FILECODE

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
  //
  // Assign DDR3 PMU FW Image Table
  //
  NBPtr->PmuFirmwareImageTable = PmuFirmwareD3ImageTableCZ;
  NBPtr->PmuFirmwareImageTableSize = PmuFirmwareD3ImageTableSizeCZ;
  //
  // Initialize Platform Specific Flow Block
  //
  NBPtr->PlatSpecFlowPtr = &memPlatSpecFlowD3CZ;
  // ---------------------------------------------------------------------------
  // Common functions
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Misc functions
  // ---------------------------------------------------------------------------
  NBPtr->ProgramCycTimings = MemNProgramCycTimingsD3CZ;
  NBPtr->GetCSIntLvAddr = MemNGetCSIntLvAddrD3CZ;

  // ---------------------------------------------------------------------------
  // Training functions
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Family specific flags
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Family specific hooks
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Initialize PS block's functions
  // ---------------------------------------------------------------------------
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
  UINT8   Dct;

  // Set DramType and disable unused DCTs
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_CZ; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);

    MemNSetBitFieldNb (NBPtr, BFDramType, DRAM_TYPE_DDR3_CZ);
  }

  // After disable unused DCTs, change the number of DCTs to 2 for DDR3 mode
  NBPtr->DctCount = MAX_D3_DCTS_PER_NODE_CZ;
  // Program bit fields before memory init
  MemNSetBitFieldNb (NBPtr, BFEnSplitMctDatBuffers, 1);
}



