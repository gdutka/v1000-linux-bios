/* $NoKeywords:$ */
/**
 * @file
 *
 * mnmrsd3.c
 *
 * DDR3 Mode Register Handling Routines
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB)
 * @e \$Revision: 313709 $ @e \$Date: 2015-02-26 11:22:47 +0800 (Thu, 26 Feb 2015) $
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
#include "merrhdl.h"
#include "cpuRegisters.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE LIBRARY_MEM_NBLIB_MNMRSD3_FILECODE

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
 *     This function calculates the value of MR0
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          MR0 value
 *
 */

UINT32
MemNCalcD3MR0 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 MrsAddress;
  UINT32 TclEnc;
  UINT32 TwrEnc;
  UINT32 Ppd;

  // program MrsAddress[1:0]=burst length and control method
  // (BL):based on F2x[1,0]84[BurstCtrl]
  MrsAddress = MemNGetBitFieldNb (NBPtr, BFBurstCtrl);

  // program MrsAddress[3]=1 (BT):interleaved
  MrsAddress |= (UINT16) 1 << 3;

  // program MrsAddress[6:4,2]=read CAS latency
  TclEnc = MemNGetBitFieldNb (NBPtr, BFTcl) - 4;
  MrsAddress |= (TclEnc >> 3) << 2;
  MrsAddress |= (TclEnc & 7) << 4;

  // program MrsAddress[11:9]=write recovery for auto-precharge
  TwrEnc = MemNGetBitFieldNb (NBPtr, BFTwr);
  TwrEnc = (TwrEnc >= 10) ? ((TwrEnc + 1) / 2) : (TwrEnc - 4);
  MrsAddress |= TwrEnc << 9;

  // program MrsAddress[12] (PPD):based on F2x[1,0]84[PChgPDModeSel]
  Ppd = MemNGetBitFieldNb (NBPtr, BFPchgPDModeSel);
  MrsAddress |= Ppd << 12;

  return MrsAddress;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function calculates the value of MR1
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       ChipSel - Rank to be trained
 *
 *     @return          MR1 value
 *
 */

UINT32
MemNCalcD3MR1 (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 ChipSel
  )
{
  UINT32 MrsAddress;
  UINT32 RttNom;

  MrsAddress = 0;

  // program MrsAddress[5,1]=output driver impedance control (DIC):
  MrsAddress |= ((UINT16) 1 << 1);

  // program MrsAddress[9,6,2]=nominal termination resistance of ODT (RTT):
  // Different CS may have different RTT.
  RttNom = NBPtr->PsPtr->RttNom[ChipSel];
  IDS_OPTION_HOOK (IDS_MEM_DRAM_TERM, &RttNom, &NBPtr->MemPtr->StdHeader);
  MrsAddress |= (((RttNom >> 2) & 1) << 9) | (((RttNom >> 1) & 1) << 6) | ((RttNom & 1) << 2);

  // program MrsAddress[12]=output disable (QOFF): 0

  // program MrsAddress[11]=TDQS:
  if ((NBPtr->DCTPtr->Timings.Dimmx4Present != 0) && (NBPtr->DCTPtr->Timings.Dimmx8Present != 0)) {
    if ((NBPtr->DCTPtr->Timings.Dimmx8Present & ((UINT8) 1 << (ChipSel >> 1))) != 0) {
      MrsAddress |= ((UINT16) 1 << 11);
    }
  }

  return MrsAddress;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function calculates the value of MR2
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       ChipSel - Rank to be trained
 *
 *     @return          MR2 value
 *
 */

UINT32
MemNCalcD3MR2 (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 ChipSel
  )
{
  CH_DEF_STRUCT *ChannelPtr;
  UINT32 MrsAddress;
  UINT32 RttWr;
  UINT8  ChannelASR;
  UINT8  ChannelSRT;

  ChannelPtr = NBPtr->ChannelPtr;

  // program MrsAddress[5:3]=CAS write latency (CWL):
  MrsAddress = (MemNGetBitFieldNb (NBPtr, BFTcwl) - 5) << 3;

  // program MrsAddress[6]=auto self refresh method (ASR):
  // program MrsAddress[7]=self refresh temperature range (SRT):
  ChannelASR = ((ChannelPtr->DimmASRPresent & ChannelPtr->ChDimmValid) == ChannelPtr->ChDimmValid) ? 1:0;
  ChannelSRT = ((ChannelPtr->DimmSRTPresent & ChannelPtr->ChDimmValid) == ChannelPtr->ChDimmValid) ? 1:0;
  if (ChannelASR == 1) {
    MrsAddress |= (1 << 6);
  } else if (NBPtr->RefPtr->EnableExtendedTemperatureRange) {
    MrsAddress |= (ChannelSRT << 7);
  }

  // program MrsAddress[10:9]=dynamic termination during writes (RTT_WR):
  RttWr = NBPtr->PsPtr->RttWr[ChipSel];
  IDS_OPTION_HOOK (IDS_MEM_DYN_DRAM_TERM, &RttWr, &NBPtr->MemPtr->StdHeader);
  MrsAddress |= RttWr << 9;

  return MrsAddress;
}


