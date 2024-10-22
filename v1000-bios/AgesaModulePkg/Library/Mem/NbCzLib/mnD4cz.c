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
#include "Library/AmdMemBaseV5Lib.h"
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

#define FILECODE LIBRARY_MEM_NBCZLIB_MND4CZ_FILECODE

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
  //
  // DDR4 Startup Speed and M1 Speed are both DDR1333
  //
  NBPtr->StartupSpeed = DDR1333_FREQUENCY;
  NBPtr->M1Speed = DDR1333_FREQUENCY;
  //
  // Assign DDR4 PMU FW Image Table
  //
  NBPtr->PmuFirmwareImageTable = PmuFirmwareD4ImageTableCZ;
  NBPtr->PmuFirmwareImageTableSize = PmuFirmwareD4ImageTableSizeCZ;
  //
  // Initialize Platform Specific Flow Block
  //
  NBPtr->PlatSpecFlowPtr = &memPlatSpecFlowD4CZ;
  // ---------------------------------------------------------------------------
  // Common functions
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Misc functions
  // ---------------------------------------------------------------------------
  NBPtr->ProgramCycTimings = MemNProgramCycTimingsD4CZ;
  NBPtr->GetCSIntLvAddr = MemNGetCSIntLvAddrD4CZ;

  // ---------------------------------------------------------------------------
  // Training functions
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Family specific flags
  // ---------------------------------------------------------------------------
  NBPtr->SelfRefreshAbort = FALSE;

  if (NBPtr->RefPtr->DramTempControlledRefreshEn && NBPtr->RefPtr->EnableExtendedTemperatureRange) {
    //
    // Force CfgDramDoubleRefreshRateEn = TRUE as it is required for DDR4 Temperature Controlled
    //  refresh in the Extended Temperature Range.
    //
    if (NBPtr->RefPtr->DramDoubleRefreshRateEn != TRUE) {
      NBPtr->RefPtr->DramDoubleRefreshRateEn = TRUE;
      ///@todo
      //PutEventLog (AGESA_ALERT, MEM_ALERT_DRAM_DOUBLE_REFRESH_RATE_ENABLED, 0, 0, 0, 0, &NBPtr->MemPtr->StdHeader);
      SetMemError (AGESA_ALERT, NBPtr->MCTPtr);
    }
  }

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
  UINT8   Dct;

  // Set DramType and disable unused DCTs
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_CZ; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);

    MemNSetBitFieldNb (NBPtr, BFDramType, DRAM_TYPE_DDR4_CZ);
  }

  // After disable unused DCTs, change the number of DCTs to 2 for DDR4 mode
  NBPtr->DctCount = MAX_D4_DCTS_PER_NODE_CZ;
  // Program bit fields before memory init
  MemNSetBitFieldNb (NBPtr, BFEnSplitMctDatBuffers, 1);
}



