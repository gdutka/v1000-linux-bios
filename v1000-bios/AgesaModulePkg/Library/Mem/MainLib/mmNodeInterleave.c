/* $NoKeywords:$ */
/**
 * @file
 *
 * mmNodeInterleave.c
 *
 * Main Memory Feature implementation file for Node Interleaving
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
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "Library/AmdBaseLib.h"
#include "Library/AmdMemBaseV5Lib.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEM_MAINLIB_MMNODEINTERLEAVE_FILECODE

extern MEM_FEAT_BLOCK_MAIN MemFeatMain;
/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
BOOLEAN
MemMInterleaveNodes (
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
 *  Check and enable node interleaving on all nodes.
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */
BOOLEAN
MemMInterleaveNodes (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  UINT8   Node;
  UINT8   NodeCnt;
  BOOLEAN RetVal;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = MemMainPtr->NBPtr;
  NodeCnt = 0;
  RetVal = TRUE;

  if (NBPtr->RefPtr->EnableNodeIntlv) {
    if (!MemFeatMain.MemClr (MemMainPtr)) {
      PutEventLog (AGESA_WARNING, MEM_WARNING_NODE_INTERLEAVING_NOT_ENABLED, 0, 0, 0, 0, &NBPtr->MemPtr->StdHeader);
      SetMemError (AGESA_WARNING, NBPtr->MCTPtr);
      return FALSE;
    }

    MemMainPtr->mmSharedPtr->NodeIntlv.IsValid = FALSE;
    MemMainPtr->mmSharedPtr->NodeIntlv.NodeIntlvSel = 0;

    for (Node = 0; Node < MemMainPtr->DieCount; Node++) {
      if (!NBPtr[Node].FeatPtr->CheckInterleaveNodes (&NBPtr[Node])) {
        break;
      }
      if (NBPtr[Node].MCTPtr->NodeMemSize != 0) {
        NodeCnt ++;
      }
    }

    if ((Node == MemMainPtr->DieCount) && (NodeCnt != 0) && ((NodeCnt & (NodeCnt - 1)) == 0)) {
      MemMainPtr->mmSharedPtr->NodeIntlv.NodeCnt = NodeCnt;
      for (Node = 0; Node < MemMainPtr->DieCount; Node++) {
        if (NBPtr[Node].MCTPtr->NodeMemSize != 0) {
          NBPtr[Node].FeatPtr->InterleaveNodes (&NBPtr[Node]);
        }
      }
      for (Node = 0; Node < MemMainPtr->DieCount; Node++) {
        NBPtr[Node].SyncAddrMapToAllNodes (&NBPtr[Node]);
        RetVal &= (BOOLEAN) (NBPtr[Node].MCTPtr->ErrCode < AGESA_FATAL);
      }
    } else {
      //
      // If all nodes cannot be interleaved
      //
      PutEventLog (AGESA_WARNING, MEM_WARNING_NODE_INTERLEAVING_NOT_ENABLED, 0, 0, 0, 0, &NBPtr->MemPtr->StdHeader);
      SetMemError (AGESA_WARNING, NBPtr->MCTPtr);
    }
  }

  return RetVal;
}

