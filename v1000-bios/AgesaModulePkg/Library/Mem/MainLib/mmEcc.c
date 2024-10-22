/* $NoKeywords:$ */
/**
 * @file
 *
 * mmEcc.c
 *
 * Main Memory Feature implementation file for ECC Initialization
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


#include "Porting.h"
#include "AGESA.h"
#include "Ids.h"
#include "Library/AmdBaseLib.h"
#include "Library/IdsLib.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "ma.h"
#include "mfmemclr.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE LIBRARY_MEM_MAINLIB_MMECC_FILECODE
/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

BOOLEAN
MemMEcc (
  IN OUT   MEM_MAIN_DATA_BLOCK *mmPtr
  );

/*-----------------------------------------------------------------------------
*                                EXPORTED FUNCTIONS
*
*-----------------------------------------------------------------------------
*/
/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *
 *
 *     @param[in,out]   *mmPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */
BOOLEAN
MemMEcc (
  IN OUT   MEM_MAIN_DATA_BLOCK *mmPtr
  )
{
  UINT8 Die;
  MEM_SHARED_DATA *SharedPtr;
  MEM_PARAMETER_STRUCT *RefPtr;
  BOOLEAN RetVal;

  RetVal = TRUE;
  RefPtr = mmPtr->MemPtr->ParameterListPtr;
  SharedPtr = mmPtr->mmSharedPtr;

  //
  // Run Northbridge-specific ECC initialization feature for each die.
  //
  SharedPtr->AllECC = FALSE;
  if (RefPtr->EnableEccFeature) {
    SharedPtr->AllECC = TRUE;
    AGESA_TESTPOINT (TpProcMemEccInitialization, &(mmPtr->MemPtr->StdHeader));

    for (Die = 0 ; Die < mmPtr->DieCount ; Die ++ ) {
      mmPtr->NBPtr[Die].FeatPtr->CheckEcc (&(mmPtr->NBPtr[Die]));
      RetVal &= (BOOLEAN) (mmPtr->NBPtr[Die].MCTPtr->ErrCode < AGESA_FATAL);
    }
    if (SharedPtr->AllECC == TRUE) {
      RefPtr->GStatus[GsbAllECCDimms] = TRUE;
      // Sync mem clear before setting scrub rate.
      for (Die = 0; Die < mmPtr->DieCount; Die++) {
        MemFMctMemClr_Sync (&(mmPtr->NBPtr[Die]));
      }
    }
  }
  // Scrubber control
  for (Die = 0 ; Die < mmPtr->DieCount ; Die ++ ) {
    mmPtr->NBPtr[Die].FeatPtr->InitEcc (&(mmPtr->NBPtr[Die]));
  }
  return RetVal;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function disable DRAM scrubber
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 */
VOID
MemMDisableScrubber (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  UINT8 Node;
  MEM_SHARED_DATA *SharedPtr;
  MEM_NB_BLOCK  *NBArray;

  SharedPtr = MemMainPtr->mmSharedPtr;
  NBArray = MemMainPtr->NBPtr;
  if (SharedPtr->AllECC == TRUE) {
    for (Node = 0; Node < MemMainPtr->DieCount; Node ++) {
      NBArray[Node].MemNDisableScrubber (&NBArray[Node]);
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function restore the settings of DRAM scrubber
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 */
VOID
MemMRestoreScrubber (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  UINT8 Node;
  MEM_SHARED_DATA *SharedPtr;
  MEM_NB_BLOCK  *NBArray;

  SharedPtr = MemMainPtr->mmSharedPtr;
  NBArray = MemMainPtr->NBPtr;
  if (SharedPtr->AllECC == TRUE) {
    for (Node = 0; Node < MemMainPtr->DieCount; Node ++) {
      NBArray[Node].MemNRestoreScrubber (&NBArray[Node]);
    }
  }
}

