/* $NoKeywords:$ */
/**
 * @file
 *
 * mmMemclr.c
 *
 * Main Memory Feature implementation file for Memory Clear.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
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
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "Ids.h"
#include "Library/IdsLib.h"
#include "mfmemclr.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEM_MAINLIB_MMMEMCLR_FILECODE
/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
BOOLEAN
MemMMctMemClr (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
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
 *   Initiates/synchronizes memory clear on all nodes with Dram on it.
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */
BOOLEAN
MemMMctMemClr (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  UINT8   Node;
  UINT8   NodeCnt;
  BOOLEAN RetVal;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = MemMainPtr->NBPtr;
  NodeCnt = MemMainPtr->DieCount;
  RetVal = TRUE;

  IDS_OPTION_HOOK (IDS_BEFORE_MEMCLR, NULL, &NBPtr->MemPtr->StdHeader);

  IDS_HDT_CONSOLE (MEM_FLOW, "Mem Clear\n");
  for (Node = 0; Node < NodeCnt; Node++) {
    NBPtr->FamilySpecificHook[DisableMemHoleMapping] (&NBPtr[Node], NULL);
  }

  for (Node = 0; Node < NodeCnt; Node++) {
    MemFMctMemClr_Init (&NBPtr[Node]);
  }

  for (Node = 0; Node < NodeCnt; Node++) {
    MemFMctMemClr_Sync (&NBPtr[Node]);
    RetVal &= (BOOLEAN) (NBPtr[Node].MCTPtr->ErrCode < AGESA_FATAL);
  }

  for (Node = 0; Node < NodeCnt; Node++) {
    NBPtr->FamilySpecificHook[RestoreMemHoleMapping] (&NBPtr[Node], NULL);
  }

  return RetVal;
}
