/* $NoKeywords:$ */
/**
 * @file
 *
 * mmflowd34cz.c
 *
 * Main Memory initialization sequence for DDR3 and DDR4 for CZ
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main/CZ)
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
#include "Library/AmdMemBaseV5Lib.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "mport.h"
#include "mm.h"
#include "mn.h"
#include "mncz.h"
#include "mnD3cz.h"
#include "mnD4cz.h"
#include "mt.h"
#include "mt3.h"
#include "mt4.h"
#include "mu.h"
#include "mnreg.h"
#include "mnpmu.h"
#include "mnPmuSramMsgBlockCZ.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE LIBRARY_MEM_MAINCZLIB_MMFLOWD34CZ_FILECODE

#include "mftds.h"


extern MEM_FLOW_CFG MemMD3FlowCZ;
extern MEM_FLOW_CFG MemMD4FlowCZ;

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
 *
 *      This function defines the memory initialization flow for
 *      systems that support DDR3 and DDR4 and CZ processor.
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 *     @return          AGESA_STATUS
 *                          - AGESA_FATAL
 *                          - AGESA_CRITICAL
 *                          - AGESA_SUCCESS
 */
AGESA_STATUS
MemMFlowD34CZ (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  MEM_NB_BLOCK  *NBPtr;
  MEM_TECH_BLOCK *TechPtr;
  MEM_DATA_STRUCT *MemPtr;
  TECHNOLOGY_TYPE *TechTypePtr;
  MEM_OPTION_INSTALL_STRUCT *OptionMemoryInstallPtr;

  NBPtr = MemMainPtr->NBPtr;
  TechPtr = MemMainPtr->TechPtr;
  MemPtr = MemMainPtr->MemPtr;
  OptionMemoryInstallPtr = (MEM_OPTION_INSTALL_STRUCT *)NBPtr->MemPtr->OptionMemoryInstallPtr;

  GetLogicalIdOfSocket (MemPtr->DiesPerSystem[BSP_DIE].SocketId, &(MemPtr->DiesPerSystem[BSP_DIE].LogicalCpuid), &(MemPtr->StdHeader));

  //
  TechTypePtr = (TECHNOLOGY_TYPE *) FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_MEM_TECH, NBPtr->MCTPtr->SocketId, 0, 0, NULL, NULL);
  if (TechTypePtr != NULL) {
    //
    // DDR4
    //
    if (*TechTypePtr == DDR4_TECHNOLOGY) {
      MemNInitNBDataD4CZ (NBPtr);
    }
    //
    // DDR3
    //
    if (*TechTypePtr == DDR3_TECHNOLOGY) {
      MemNInitNBDataD3CZ (NBPtr);
    }
  }
  //----------------------------------------------------------------
  // Memory Context Restore
  //----------------------------------------------------------------
  if (OptionMemoryInstallPtr->MemFeatMain.MemRestore (MemMainPtr)) {

    // Memory DMI support
    if (!OptionMemoryInstallPtr->MemFeatMain.MemDmi (MemMainPtr)) {
      return AGESA_CRITICAL;
    }

    // Memory CRAT support
    if (!OptionMemoryInstallPtr->MemFeatMain.MemCrat (MemMainPtr)) {
      return AGESA_CRITICAL;
    }

    return AGESA_SUCCESS;
  }

  MemFInitTableDrive (&NBPtr[BSP_DIE], MTBeforeInitializeMCT);

  // Clear DisDllShutdownSR prior any P-State changes.
  MemNConfigureDisDllShutdownSrCZ (NBPtr);

  //----------------------------------------------------------------
  // Force NB-Pstate to NBP0
  //----------------------------------------------------------------
  MemNChangeNbFrequencyWrapUnb (NBPtr, 0);

  //
  // Check Installed DRAM Technology Type
  //
  TechTypePtr = (TECHNOLOGY_TYPE *) FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_MEM_TECH, NBPtr->MCTPtr->SocketId, 0, 0, NULL, NULL);
  if (TechTypePtr != NULL) {
    //
    // DDR4
    //
    if (*TechTypePtr == DDR4_TECHNOLOGY) {
      if (NBPtr->MemPtr->ParameterListPtr->DimmTypeDddr4Capable == TRUE) {
        IDS_HDT_CONSOLE (MEM_FLOW, "\nAnalyze DDR4 DIMM SPD Data\n");
        if (MemTDIMMPresence4 (NBPtr[BSP_DIE].TechPtr) && (NBPtr[BSP_DIE].MCTPtr->DimmPresent != 0)) {
          //
          // Setup D4 Platform Specific Pointers here.
          //
          MemNInitNBDataD4CZ (NBPtr);
          return MemMD4FlowCZ (MemMainPtr);
        } else {
          //
          // DDR4 requested but No DDR4 DIMMs found
          //
          IDS_HDT_CONSOLE (MEM_FLOW, "\nNo DDR4 DIMMs found.\n");
          PutEventLog (AGESA_FATAL, MEM_ERROR_NO_DIMM_FOUND_ON_SYSTEM, 0, 0, 0, 0, &(MemMainPtr->MemPtr->StdHeader));
          return AGESA_FATAL;
        }
      } else {
        return FALSE;
      }
    }
    //
    // DDR3
    //
    if (*TechTypePtr == DDR3_TECHNOLOGY) {
      if (NBPtr->MemPtr->ParameterListPtr->DimmTypeDddr3Capable == TRUE) {
        IDS_HDT_CONSOLE (MEM_FLOW, "\nAnalyze DDR3 DIMM SPD Data\n");
        if (MemTDIMMPresence3 (NBPtr[BSP_DIE].TechPtr) && (NBPtr[BSP_DIE].MCTPtr->DimmPresent != 0)) {
          //
          // Setup D3 Platform Specific Pointers here.
          //
          MemNInitNBDataD3CZ (NBPtr);
          return MemMD3FlowCZ (MemMainPtr);
        } else {
          //
          // DDR3 requested but No DDR3 DIMMs found
          //
          IDS_HDT_CONSOLE (MEM_FLOW, "\nNo DDR3 DIMMs found.\n");
          PutEventLog (AGESA_FATAL, MEM_ERROR_NO_DIMM_FOUND_ON_SYSTEM, 0, 0, 0, 0, &(MemMainPtr->MemPtr->StdHeader));
          return AGESA_FATAL;
        }
      } else {
        return FALSE;
      }
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\nNo Valid Dimms of the supported types found.\n");
    return AGESA_FATAL; // UNSUPPORTED DIMMs requested
  } else {
    IDS_HDT_CONSOLE (MEM_FLOW, "Platform BIOS with Combo DDR3/DDR4 Support must define DRAM Technology Type in the Platform Configuration Table\n");
    return AGESA_FATAL;
  }
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

