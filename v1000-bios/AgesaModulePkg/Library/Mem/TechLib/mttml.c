/* $NoKeywords:$ */
/**
 * @file
 *
 * mttml.c
 *
 * Technology Max Latency Training support
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech)
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
#include "Library/AmdMemBaseV5Lib.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "merrhdl.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE LIBRARY_MEM_TECHLIB_MTTML_FILECODE
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
 *      This function trains Max latency for all dies
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemTTrainMaxLatency (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT32 TestAddrRJ16;
  UINT8 Dct;
  UINT8 ChipSel;
  UINT8  *PatternBufPtr;
  UINT8  *TestBufferPtr;
  UINT8  CurrentNbPstate;
  UINT16 CalcMaxLatDly;
  UINT16 MaxLatDly;
  UINT16 MaxLatLimit;
  UINT16 Margin;
  UINT16 CurTest;
  UINT16 _CL_;
  UINT8 TimesFail;
  UINT8 TimesRetrain;
  UINT16 i;
  MEM_DATA_STRUCT *MemPtr;
  DIE_STRUCT *MCTPtr;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;
  MCTPtr = NBPtr->MCTPtr;
  MemPtr = NBPtr->MemPtr;
  TechPtr->TrainingType = TRN_MAX_READ_LATENCY;
  TimesRetrain = DEFAULT_TRAINING_TIMES;
  IDS_OPTION_HOOK (IDS_MEM_RETRAIN_TIMES, &TimesRetrain, &MemPtr->StdHeader);

  IDS_HDT_CONSOLE (MEM_STATUS, "\nStart MaxRdLat training\n");
  // Set environment settings before training
  AGESA_TESTPOINT (TpProcMemMaxRdLatencyTraining, &(MemPtr->StdHeader));

  MemTBeginTraining (TechPtr);
  //
  // Initialize the Training Pattern
  //
  if (AGESA_SUCCESS != NBPtr->TrainingPatternInit (NBPtr)) {
    return (BOOLEAN) (MCTPtr->ErrCode < AGESA_FATAL);
  }
  TechPtr->PatternLength = (MCTPtr->Status[Sb128bitmode]) ? 6 : 3;
  //
  // Setup hardware training engine (if applicable)
  //
  NBPtr->FamilySpecificHook[SetupHwTrainingEngine] (NBPtr, &TechPtr->TrainingType);
  //
  // Adjust RdDqsDly used for MaxRdLatency calculation
  //
  NBPtr->FamilySpecificHook[AdjustRdDqsDlyForMaxRdLat] (NBPtr, NULL);

  MaxLatDly = 0;
  _CL_ = TechPtr->PatternLength;
  PatternBufPtr = TechPtr->PatternBufPtr;
  TestBufferPtr = TechPtr->TestBufPtr;
  //
  // Begin max latency training
  //
  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    if (MCTPtr->Status[Sb128bitmode] && (Dct != 0)) {
      break;
    }

    NBPtr->SwitchDCT (NBPtr, Dct);

    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      IDS_HDT_CONSOLE (MEM_STATUS, "\tDct %d\n", Dct);
      if (TechPtr->FindMaxDlyForMaxRdLat (TechPtr, &ChipSel)) {
        TechPtr->ChipSel = ChipSel;
        if (NBPtr->GetSysAddr (NBPtr, ChipSel, &TestAddrRJ16)) {
          IDS_HDT_CONSOLE (MEM_STATUS, "\t\tCS %d\n", ChipSel);
          IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tWrite to address: %04x0000\n", TestAddrRJ16);

          // Write the test patterns
          AGESA_TESTPOINT (TpProcMemMaxRdLatWritePattern, &(MemPtr->StdHeader));
          NBPtr->WritePattern (NBPtr, TestAddrRJ16, PatternBufPtr, _CL_);

          // Sweep max latency delays
          NBPtr->getMaxLatParams (NBPtr, TechPtr->MaxDlyForMaxRdLat, &CalcMaxLatDly, &MaxLatLimit, &Margin);
          AGESA_TESTPOINT (TpProcMemMaxRdLatStartSweep, &(MemPtr->StdHeader));

          TimesFail = 0;
          ERROR_HANDLE_RETRAIN_BEGIN (TimesFail, TimesRetrain)
          {
            MaxLatDly = CalcMaxLatDly;
            for (i = 0; i < (MaxLatLimit - CalcMaxLatDly); i++) {
              NBPtr->SetBitField (NBPtr, BFMaxLatency, MaxLatDly);
              IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tDly %3x", MaxLatDly);
              TechPtr->ResetDCTWrPtr (TechPtr, 6);

              AGESA_TESTPOINT (TpProcMemMaxRdLatReadPattern, &(MemPtr->StdHeader));
              NBPtr->ReadPattern (NBPtr, TestBufferPtr, TestAddrRJ16, _CL_);
              AGESA_TESTPOINT (TpProcMemMaxRdLatTestPattern, &(MemPtr->StdHeader));
              CurTest = NBPtr->CompareTestPattern (NBPtr, TestBufferPtr, PatternBufPtr, _CL_ * 64);
              NBPtr->FlushPattern (NBPtr, TestAddrRJ16, _CL_);

              if (NBPtr->IsSupported[ReverseMaxRdLatTrain]) {
                // Reverse training decrements MaxLatDly whenever the test passes
                // and uses the last passing MaxLatDly as left edge
                if (CurTest == 0xFFFF) {
                  IDS_HDT_CONSOLE (MEM_FLOW, "  P");
                  if (MaxLatDly == 0) {
                    break;
                  } else {
                    MaxLatDly--;
                  }
                }
              } else {
                // Traditional training increments MaxLatDly until the test passes
                // and uses it as left edge
                if (CurTest == 0xFFFF) {
                  IDS_HDT_CONSOLE (MEM_FLOW, "  P");
                  break;
                } else {
                  MaxLatDly++;
                }
              }
              IDS_HDT_CONSOLE (MEM_FLOW, "\n");
            } // End of delay sweep
            ERROR_HANDLE_RETRAIN_END ((MaxLatDly >= MaxLatLimit), TimesFail)
          }

          AGESA_TESTPOINT (TpProcMemMaxRdLatSetDelay, &(MemPtr->StdHeader));

          if (MaxLatDly >= MaxLatLimit) {
            PutEventLog (AGESA_ERROR, MEM_ERROR_MAX_LAT_NO_WINDOW, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
            SetMemError (AGESA_ERROR, MCTPtr);
            NBPtr->DCTPtr->Timings.CsTrainFail |= NBPtr->DCTPtr->Timings.CsPresent;
            MCTPtr->ChannelTrainFail |= (UINT32)1 << Dct;
            if (!NBPtr->MemPtr->ErrorHandling (MCTPtr, NBPtr->Dct, EXCLUDE_ALL_CHIPSEL, &NBPtr->MemPtr->StdHeader)) {
              ASSERT (FALSE);
              return FALSE;
            }
          } else {
            NBPtr->FamilySpecificHook[AddlMaxRdLatTrain] (NBPtr, &TestAddrRJ16);

            IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\tFirst Passing MRL: %03x", MaxLatDly);
            MaxLatDly = MaxLatDly + Margin;
            if (NBPtr->IsSupported[ReverseMaxRdLatTrain]) {
              MaxLatDly++;  // Add 1 to get back to the last passing value
            }
            // Set final delays
            CurrentNbPstate = (UINT8) MemNGetBitFieldNb (NBPtr, BFCurNbPstate);
            ASSERT (CurrentNbPstate <= 3);
            NBPtr->ChannelPtr->DctMaxRdLat [CurrentNbPstate] = MaxLatDly;
            NBPtr->SetBitField (NBPtr, BFMaxLatency, MaxLatDly);
            IDS_HDT_CONSOLE (MEM_FLOW, "\t\tFinal MaxRdLat: %03x\n", MaxLatDly);

          }
        }
      }
    }
  }

  // Restore environment settings after training
  MemTEndTraining (TechPtr);
  IDS_HDT_CONSOLE (MEM_FLOW, "\nEnd MaxRdLat training\n\n");
  //
  // Finalize the Pattern
  //
  NBPtr->TrainingPatternFinalize (NBPtr);
  return (BOOLEAN) (MCTPtr->ErrCode < AGESA_FATAL);
}

