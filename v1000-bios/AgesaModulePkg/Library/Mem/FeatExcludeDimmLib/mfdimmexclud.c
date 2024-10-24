/* $NoKeywords:$ */
/**
 * @file
 *
 * mfdimmexclud.c
 *
 * Feature DIMM exclude.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/EXCLUDIMM)
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
#include "mt.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEM_FEATEXCLUDEDIMMLIB_MFDIMMEXCLUD_FILECODE

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
BOOLEAN
MemFRASExcludeDIMM (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *  Check and disable Chip selects that fail training for each node.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  This feature is enabled.
 *     @return          FALSE - This feature is not enabled.
 */
BOOLEAN
MemFRASExcludeDIMM (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;
  UINT8 ReserveDCT;
  UINT8 q;
  BOOLEAN Flag;
  BOOLEAN IsCSIntlvEnabled;
  UINT16 CsTestFail;
  DIE_STRUCT *MCTPtr;
  BOOLEAN RetVal;

  ASSERT (NBPtr != NULL);
  ReserveDCT = NBPtr->Dct;
  CsTestFail = 0;
  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.CsTestFail != 0) {
      // When there is no new failed dimm that needs to be excluded, then no need to go through the process.
      switch (NBPtr->SharedPtr->DimmExcludeFlag) {
      case NORMAL:
        // See there is new dimm that needs to be excluded
        if ((NBPtr->DCTPtr->Timings.CsTestFail & NBPtr->DCTPtr->Timings.CsEnabled) != 0) {
          CsTestFail |= NBPtr->DCTPtr->Timings.CsTestFail;
        }
        break;
      case TRAINING:
        // Do not do any dimm excluding during training
        // Dimm exclude will be done at the end of training
        break;
      case END_TRAINING:
        // Exclude all dimms that have failures during training
        if ((NBPtr->DCTPtr->Timings.CsTrainFail != 0) ||
            ((NBPtr->DCTPtr->Timings.CsTestFail & NBPtr->DCTPtr->Timings.CsEnabled) != 0)) {
          CsTestFail |= NBPtr->DCTPtr->Timings.CsTestFail;
        }
        break;
      default:
        IDS_ERROR_TRAP;
      }
    }
  }

  if (CsTestFail != 0) {
    IsCSIntlvEnabled = FALSE;
    MCTPtr = NBPtr->MCTPtr;
    MCTPtr->NodeMemSize = 0;
    NBPtr->SharedPtr->NodeMap[NBPtr->Node].IsValid = FALSE;
    NBPtr->SharedPtr->NodeMap[NBPtr->Node].SysBase = 0;
    NBPtr->SharedPtr->NodeMap[NBPtr->Node].SysLimit = 0;
    NBPtr->SetBitField (NBPtr, BFDramBaseAddr, 0);
    NBPtr->SetBitField (NBPtr, BFDramLimitAddr, 0);

    if (MCTPtr->GangedMode) {
      // if ganged mode, disable all pairs of CS that fail.
      NBPtr->DCTPtr->Timings.CsTestFail |= CsTestFail;
    }

    // if chip select interleaving has been enabled, need to undo it before remapping memory
    if (NBPtr->FeatPtr->UndoInterleaveBanks (NBPtr)) {
      IsCSIntlvEnabled = TRUE;
    }

    Flag = TRUE;
    NBPtr->FamilySpecificHook[BfAfExcludeDimm] (NBPtr, &Flag);
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      NBPtr->SwitchDCT (NBPtr, Dct);
      if (!MCTPtr->GangedMode || (MCTPtr->Dct == 0)) {
        if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
          NBPtr->DCTPtr->Timings.DctMemSize = 0;

          NBPtr->DCTPtr->Timings.CsEnabled = 0;
          for (q = 0; q < MAX_CS_PER_CHANNEL; q++) {
            NBPtr->SetBitField (NBPtr, BFCSBaseAddr0Reg + q, 0);
          }

          // Set F2x94[DisDramInterface] = 1 if all chip selects fail training on the DCT
          if ((NBPtr->DCTPtr->Timings.CsPresent & ~NBPtr->DCTPtr->Timings.CsTestFail) == 0) {
            NBPtr->DisableDCT (NBPtr);
          }

          Flag = NBPtr->StitchMemory (NBPtr);
          ASSERT (Flag == TRUE);
        }
      }
    }
    Flag = FALSE;
    NBPtr->FamilySpecificHook[BfAfExcludeDimm] (NBPtr, &Flag);

    // Re-enable chip select interleaving when remapping is done.
    if (IsCSIntlvEnabled) {
      NBPtr->FeatPtr->InterleaveBanks (NBPtr);
    }

    RetVal = TRUE;
  } else {
    RetVal = FALSE;
  }
  NBPtr->SwitchDCT (NBPtr, ReserveDCT);
  return RetVal;
}


