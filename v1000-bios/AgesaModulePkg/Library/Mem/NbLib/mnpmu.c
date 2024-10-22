/* $NoKeywords:$ */
/**
 * @file
 *
 * mnpmu.c
 *
 * Common Northbridge function for PMU initialization
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB)
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
#include "Library/IdsLib.h"
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mu.h"
#include "mnreg.h"
#include "mnpmu.h"
#include "merrhdl.h"
#include "cpuRegisters.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE LIBRARY_MEM_NBLIB_MNPMU_FILECODE

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
 *      This function resets the PMU
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNPmuResetNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  // 1. Program D18F2x9C_x0002_0099_dct[3:0][PmuReset,PmuStall] = 1,1.
  MemNSetBitFieldNb (NBPtr, BFPmuReset, 1);
  MemNSetBitFieldNb (NBPtr, BFPmuStall, 1);

  // 2. Program D18F2x9C_x0002_000E_dct[3:0][PhyDisable]=0.
  MemNSetBitFieldNb (NBPtr, BFPhyDisable, 0);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *     This function commands PMU to start DRAM initialization and/or DRAM training
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNStartPmuNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\tStart PMU on image%d...\n", NBPtr->PmuFirmwareImage);
  // Initialize the protocol bit prior use.
  MemNSetBitFieldNb (NBPtr, BFRdy, 1);
  MemNSetBitFieldNb (NBPtr, BFRdy2, 1);

  // Clear PmuStall to start PMU
  MemNSetBitFieldNb (NBPtr, BFPmuStall, 0);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *     This function waits for PMU to complete
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE - PMU completes
 *     @return          FALSE - PMU fails
 */

BOOLEAN
MemNPendOnPmuCompletionNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT16 UsMsg;
  UINT16 Us2Msg;
  INT16 Us2MsgIndex;
  INT16 Count;

  UsMsg = 0;
  Us2Msg = 0;
  Count = 0;

  while (UsMsg != USMSG_PMUQEMPTY) {
    // 1. Wait until D18F2x9C_x0002_0004_dct[3:0][UsRdy]==0.
    MemNPollBitFieldNb (NBPtr, BFUsRdy, 0, PCI_ACCESS_TIMEOUT, FALSE);

    // 2. Read D18F2x9C_x0002_0032_dct[3:0][Message]
    UsMsg = (UINT16) MemNGetBitFieldNb (NBPtr, BFUsMessage);
    IDS_HDT_CONSOLE (MEM_PMU, "\t\t\tUsMsg: %x\n", UsMsg);

    Us2MsgIndex = 0;

    // Print out debug data if available
    if (UsMsg == USMSG_US2MSGRDY) {
      IDS_HDT_CONSOLE (MEM_PMU, "\t\t\t\tStream message through US mailbox 2\n");
      do {
        // 1. Wait until D18F2x9C_x0002_0004_dct[3:0][Us2Rdy]==0.
        MemNPollBitFieldNb (NBPtr, BFUs2Rdy, 0, PCI_ACCESS_TIMEOUT, FALSE);

        // 2. Read D18F2x9C_x0002_0034_dct[3:0][Message]
        Us2Msg = (UINT16) MemNGetBitFieldNb (NBPtr, BFUs2Message);
        IDS_HDT_CONSOLE (MEM_PMU, "\t\t\t\tUs2Msg : %x\n", Us2Msg);

        if (Us2MsgIndex == 0) {
          // The first item received is the COUNT.
          Count = (INT16)Us2Msg;
          IDS_HDT_CONSOLE (MEM_PMU, "\t\t\t\tUs2Msg COUNT: %x\n", Count);
        }

        Us2MsgIndex++;

        // 3. Program D18F2x9C_x0002_0035_dct[3:0][Rdy] = 1.
        MemNSetBitFieldNb (NBPtr, BFRdy2, 1);

        // 4. Decrement COUNT and loop to step 1 until COUNT == -1.
      } while (--Count >= 0);
    }

    // 3. Program D18F2x9C_x0002_0033_dct[3:0][Rdy] = 1.
    MemNSetBitFieldNb (NBPtr, BFRdy, 1);

    if (UsMsg == USMSG_FAIL) {
      return FALSE;
    }
  }

  return TRUE;
}

