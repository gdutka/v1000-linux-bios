/* $NoKeywords:$ */
/**
 * @file
 *
 * merrhdl.c
 *
 * Memory error handling
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
#include "Library/AmdBaseLib.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "Library/AmdHeapLib.h"
#include "merrhdl.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE LIBRARY_MEM_MAINLIB_MERRHDL_FILECODE

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
 *  This function handle errors occur in memory code.
 *
 *
 *     @param[in,out]   *MCTPtr - pointer to DIE_STRUCT.
 *     @param[in,out]   DCT - DCT that needs to be handled.
 *     @param[in,out]   ChipSelMask - Chip select mask that needs to be handled
 *     @param[in,out]   *StdHeader - pointer to AMD_CONFIG_PARAMS
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */
BOOLEAN
MemErrHandle (
  IN       DIE_STRUCT *MCTPtr,
  IN       UINT8  DCT,
  IN       UINT16  ChipSelMask,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  BOOLEAN ErrorRecovery;
  BOOLEAN IgnoreErr;
  DCT_STRUCT *DCTPtr;
  UINT8 CurrentDCT;
  LOCATE_HEAP_PTR LocHeap;
  MEM_NB_BLOCK *NBPtr;
  MEM_MAIN_DATA_BLOCK mmData;

  DCTPtr = MCTPtr->DctData;
  ErrorRecovery = TRUE;
  IgnoreErr = FALSE;
  IDS_OPTION_HOOK (IDS_MEM_ERROR_RECOVERY, &ErrorRecovery, StdHeader);

  if (ErrorRecovery) {
    if (DCT == EXCLUDE_ALL_DCT) {
      // Exclude all DCTs on a node
      for (CurrentDCT = 0; CurrentDCT < MCTPtr->DctCount; CurrentDCT++) {
        DCTPtr[CurrentDCT].Timings.CsTestFail = DCTPtr[CurrentDCT].Timings.CsPresent;
      }
    } else if (ChipSelMask == EXCLUDE_ALL_CHIPSEL) {
      // Exclude the specified DCT
      DCTPtr[DCT].Timings.CsTestFail = DCTPtr[DCT].Timings.CsPresent;
    } else {
      // Exclude the chip select that has been marked out
      DCTPtr[DCT].Timings.CsTestFail |= ChipSelMask & DCTPtr[DCT].Timings.CsPresent;
      IDS_OPTION_HOOK (IDS_LOADCARD_ERROR_RECOVERY, &DCTPtr[DCT], StdHeader);
    }

    // Exclude the failed dimm to recovery from error
    if (MCTPtr->NodeMemSize != 0) {
      LocHeap.BufferHandle = AMD_MEM_AUTO_HANDLE;
      if (HeapLocateBuffer (&LocHeap, StdHeader) == AGESA_SUCCESS) {
        // NB block has already been constructed by main block.
        // No need to construct it here.
        NBPtr = (MEM_NB_BLOCK *)LocHeap.BufferPtr;
        if (!NBPtr->SharedPtr->NodeMap[MCTPtr->NodeId].IsValid) {
          // Memory map has not been calculated, no need to remap memory across node here.
          // Only need to remap memory within the node.
          NBPtr = &NBPtr[MCTPtr->NodeId];
          NBPtr->FeatPtr->ExcludeDIMM (NBPtr);
        } else {
          // Need to remap memory across the whole system.
          mmData.MemPtr = NBPtr->MemPtr;
          mmData.mmSharedPtr = NBPtr->SharedPtr;
          mmData.NBPtr = NBPtr;
          mmData.TechPtr = (MEM_TECH_BLOCK *) (&NBPtr[NBPtr->MemPtr->DieCount]);
          mmData.DieCount = NBPtr->MemPtr->DieCount;
          if (!((MEM_OPTION_INSTALL_STRUCT *)NBPtr->MemPtr->OptionMemoryInstallPtr)->MemFeatMain.ExcludeDIMM (&mmData)) {
            return FALSE;
          }
        }
      }
      // If allocation fails, that means the code is not running at BSP.
      // Parallel training is in process.
      // Remap for parallel training will be done when control returns to BSP.
    }
    return TRUE;
  } else {
    IDS_OPTION_HOOK (IDS_MEM_IGNORE_ERROR, &IgnoreErr, StdHeader);
    if (IgnoreErr) {
      return TRUE;
    }
    SetMemError (AGESA_FATAL, MCTPtr);
    // ErrorRecovery is FALSE
    return FALSE;
  }
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */


