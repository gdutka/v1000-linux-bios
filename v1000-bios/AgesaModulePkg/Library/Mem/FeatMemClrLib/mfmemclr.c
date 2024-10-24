/* $NoKeywords:$ */
/**
 * @file
 *
 * mfmemclr.c
 *
 * Feature function for memory clear operation
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/Memclr)
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




#include "AGESA.h"
#include "mm.h"
#include "mn.h"
#include "mfmemclr.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "merrhdl.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEM_FEATMEMCLRLIB_MFMEMCLR_FILECODE

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

/*
 *-----------------------------------------------------------------------------
 *                                EXPORTED FUNCTIONS
 *
 *-----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   Initiates memory clear operation on one node with Dram on it.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

BOOLEAN
MemFMctMemClr_Init (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  AGESA_TESTPOINT (TpProcMemMemClr, &NBPtr->MemPtr->StdHeader);
  IDS_PERF_TIMESTAMP (TP_BEGINMEMFMCTMEMCLR_INIT, &(NBPtr->MemPtr->StdHeader));
  if (NBPtr->RefPtr->EnableMemClr == TRUE) {
    if (NBPtr->MCTPtr->NodeMemSize != 0) {
      if (!NBPtr->MemCleared) {
        NBPtr->PollBitField (NBPtr, BFMemClrBusy, 0, SPECIAL_PCI_ACCESS_TIMEOUT, FALSE);
        if (NBPtr->GetBitField (NBPtr, BFDramEnabled) == 1) {
          NBPtr->FamilySpecificHook[BeforeMemClr] (NBPtr, NBPtr);
          NBPtr->SetBitField (NBPtr, BFDramBaseAddr, 0);
          NBPtr->SetBitField (NBPtr, BFMemClrInit, 1);
        }
      }
    }
  }
  IDS_PERF_TIMESTAMP (TP_ENDNMEMFMCTMEMCLR_INIT, &(NBPtr->MemPtr->StdHeader));
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   Ensures memory clear operation has completed on one node with Dram on it.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

BOOLEAN
MemFMctMemClr_Sync (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 MicroSecondToWait;

  MicroSecondToWait = 0;
  if (NBPtr->RefPtr->EnableMemClr == TRUE) {
    if (NBPtr->MCTPtr->NodeMemSize != 0) {
      // Calculate Timeout value:
      // Timeout (in microsecond) = Memory Size * 1.5 ns / 8 Byte * 4 (Margin) * 1000 (change millisecond to us)
      // NodeMemSize is system address right shifted by 16, so shift it 4 bits to right to convert it to MB.
      // 1.5 / 8 * 4 * 1000 = 750
      MicroSecondToWait = (NBPtr->MCTPtr->NodeMemSize >> 4) * 750;

      if (!NBPtr->MemCleared) {
        NBPtr->PollBitField (NBPtr, BFMemClrBusy, 0, MicroSecondToWait, FALSE);
        NBPtr->PollBitField (NBPtr, BFMemCleared, 1, MicroSecondToWait, FALSE);
        NBPtr->SetBitField (NBPtr, BFDramBaseAddr, NBPtr->MCTPtr->NodeSysBase >> (27 - 16));
        NBPtr->MemCleared = TRUE;
      }
    }
  }
  return TRUE;
}


