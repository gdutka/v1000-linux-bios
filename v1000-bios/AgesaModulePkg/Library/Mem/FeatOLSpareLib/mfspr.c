/* $NoKeywords:$ */
/**
 * @file
 *
 * mfspr.c
 *
 * Feature enable online spare
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/Olspare)
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
#include "mm.h"
#include "mn.h"
#include "Library/AmdMemFeatOLSpareLib.h"
#include "Library/AmdBaseLib.h"
#include "Library/AmdMemBaseV5Lib.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEM_FEATOLSPARELIB_MFSPR_FILECODE

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
 *  Enable online spare on current node if it is requested.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  This feature is enabled.
 *     @return          FALSE - This feature is not enabled.
 */

BOOLEAN
MemFOnlineSpare (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;
  UINT8 q;
  UINT8  Value8;
  BOOLEAN Flag;
  BOOLEAN OnlineSprEnabled[MAX_CHANNELS_PER_SOCKET];

  MEM_PARAMETER_STRUCT *RefPtr;
  DIE_STRUCT *MCTPtr;

  ASSERT (NBPtr != NULL);

  RefPtr = NBPtr->RefPtr;
  Flag = FALSE;
  if (RefPtr->EnableOnLineSpareCtl != 0) {
    RefPtr->GStatus[GsbEnDIMMSpareNW] = TRUE;
    MCTPtr = NBPtr->MCTPtr;

    // Check if online spare can be enabled on current node
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      ASSERT (Dct < sizeof (OnlineSprEnabled));
      NBPtr->SwitchDCT (NBPtr, Dct);
      OnlineSprEnabled[Dct] = FALSE;
      if ((MCTPtr->GangedMode == 0) || (MCTPtr->Dct == 0)) {
        if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
          // Make sure at least two chip-selects are available
          Value8 = HighBitSet32 (NBPtr->DCTPtr->Timings.CsEnabled);
          if (Value8 > LowBitSet32 (NBPtr->DCTPtr->Timings.CsEnabled)) {
            OnlineSprEnabled[Dct] = TRUE;
            Flag = TRUE;
          } else {
            PutEventLog (AGESA_ERROR, MEM_ERROR_DIMM_SPARING_NOT_ENABLED, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
            MCTPtr->ErrStatus[EsbSpareDis] = TRUE;
          }
        }
      }
    }

    // If we don't have spared rank on any DCT, we don't run the rest part of the code.
    if (!Flag) {
      return FALSE;
    }

    MCTPtr->NodeMemSize = 0;
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      NBPtr->SwitchDCT (NBPtr, Dct);
      if (OnlineSprEnabled[Dct]) {
        // Only run StitchMemory if we need to set a spare rank.
        NBPtr->DCTPtr->Timings.DctMemSize = 0;
        for (q = 0; q < MAX_CS_PER_CHANNEL; q++) {
          NBPtr->SetBitField (NBPtr, BFCSBaseAddr0Reg + q, 0);
        }
        Flag = NBPtr->StitchMemory (NBPtr);
        ASSERT (Flag == TRUE);
      } else if ((MCTPtr->GangedMode == 0) && (NBPtr->DCTPtr->Timings.DctMemSize != 0)) {
        // Otherwise, need to adjust the memory size on the node.
        MCTPtr->NodeMemSize += NBPtr->DCTPtr->Timings.DctMemSize;
        MCTPtr->NodeSysLimit = MCTPtr->NodeMemSize - 1;
      }
    }
    return TRUE;
  } else {
    return FALSE;
  }
}

