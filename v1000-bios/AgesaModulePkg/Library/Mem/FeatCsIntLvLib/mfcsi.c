/* $NoKeywords:$ */
/**
 * @file
 *
 * mfcsi.c
 *
 * Feature bank interleaving support (AKA Chip Select Interleaving )
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/Csintlv)
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


/* This file contains functions for Chip Select interleaving */



#include "AGESA.h"
#include "Library/AmdBaseLib.h"
#include "Library/AmdMemBaseV5Lib.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "Library/AmdMemFeatCsIntLvLib.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEM_FEATCSINTLVLIB_MFCSI_FILECODE
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
STATIC
MemFDctInterleaveBanks (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
STATIC
CsIntSwap (
  IN OUT   UINT32 *BaseMaskRegPtr,
  IN       UINT8 EnChipSels,
  IN       UINT8 LoBit,
  IN       UINT8 HiBit
  );

BOOLEAN
MemFUndoInterleaveBanks (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

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
 *      This function Applies DIMM bank (chip-select) interleaving if enabled
 *      and if all criteria are met.  Interleaves chip-selects on page boundaries.
 *      This function calls subfunctions that sets up CS interleaving on multiple Sockets
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  This feature is enabled.
 *     @return          FALSE - This feature is not enabled.
 */

BOOLEAN
MemFInterleaveBanks (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;
  BOOLEAN RetFlag;

  ASSERT (NBPtr != NULL);

  RetFlag = FALSE;
  if (NBPtr->RefPtr->EnableBankIntlv) {
    if (NBPtr->MCTPtr->NodeMemSize) {
      for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
        NBPtr->SwitchDCT (NBPtr, Dct);
        RetFlag |= MemFDctInterleaveBanks (NBPtr);
      }
    }
  }
  return RetFlag;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *     This function checks if bank interleaving has been enabled or not. If yes, it will
 *     undo bank interleaving. Otherwise, it does nothing.
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  Bank interleaving has been enabled.
 *     @return          FALSE - Bank interleaving has not been enabled.
 */

BOOLEAN
MemFUndoInterleaveBanks (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Cs;
  UINT8 Dct;
  UINT32 CSMask;
  BOOLEAN CSIntlvEnabled;
  BOOLEAN RetFlag;

  ASSERT (NBPtr != NULL);

  RetFlag = FALSE;

  if (NBPtr->RefPtr->EnableBankIntlv) {
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      NBPtr->SwitchDCT (NBPtr, Dct);
      if (NBPtr->DCTPtr->Timings.DctMemSize) {
        CSIntlvEnabled = FALSE;
        for (Cs = 0; Cs < MAX_CS_PER_CHANNEL; Cs++) {
          if ((NBPtr->GetBitField (NBPtr, BFCSBaseAddr0Reg + Cs) & 1) != 0) {
            CSMask = NBPtr->GetBitField (NBPtr, BFCSMask0Reg + (Cs / 2));
            if (((CSMask >> 5) & 0x1FF) != 0x1FF) {
              CSIntlvEnabled = TRUE;
              break;
            }
          }
        }
        if (CSIntlvEnabled) {
          IDS_HDT_CONSOLE(MEM_FLOW,"\t\tUndo CS Interleave\n");
          MemFDctInterleaveBanks (NBPtr);
          RetFlag = TRUE;
        }
      }
    }
  }
  return RetFlag;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function Applies DIMM bank (chip-select) interleaving if enabled
 *      and if all criteria are met.  Interleaves chip-selects on page boundaries.
 *      This function is run once per Socket
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  Register bits have been swapped.
 *     @return          FALSE - Register bits have not been swapped.
 *
 */

BOOLEAN
STATIC
MemFDctInterleaveBanks (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Cs;
  UINT8 EnChipSels;
  UINT8 BankEncd;
  UINT8 BankEncd0;
  UINT8 i;
  UINT8 j;
  UINT32 BankAddrReg;
  UINT32 BaseRegS0;
  UINT32 BaseRegS1;
  UINT32 MaskReg;
  UINT8 Offset;
  UINT8 Dct;

  ASSERT (NBPtr != NULL);

  Dct = NBPtr->Dct;

  // Check if CS interleaving can be enabled
  EnChipSels = 0;
  BankEncd0 = 0xFF;
  Offset = 0;
  for (Cs = 0; Cs < MAX_CS_PER_CHANNEL; Cs++) {
    if ((NBPtr->GetBitField (NBPtr, BFCSBaseAddr0Reg + Cs) & 3) != 0) {
      BankAddrReg = NBPtr->GetBitField (NBPtr, BFDramBankAddrReg);
      BankEncd = (UINT8) ((BankAddrReg >> ((Cs / 2) * 4)) & 0xF);
      if (BankEncd0 == 0xFF) {
        BankEncd0 = BankEncd;
      } else if (BankEncd0 != BankEncd) {
        break;
      }
      if ((NBPtr->GetBitField (NBPtr, BFCSBaseAddr0Reg + Cs) & 1) != 0) {
        EnChipSels++;
      }
    }
  }

  // Swap Dram Base/Mask Addr to enable CS interleaving
  if ((Cs == MAX_CS_PER_CHANNEL) && ((EnChipSels == 2) || (EnChipSels == 4) || (EnChipSels == 8))) {
    NBPtr->TechPtr->GetCSIntLvAddr (NBPtr->TechPtr, BankEncd0, &i, &j);
    NBPtr->DCTPtr->BankAddrMap = BankEncd0;
    NBPtr->DCTPtr->EnabledChipSels = EnChipSels;
    // Family specific CS interleaving low address adjustment
    NBPtr->FamilySpecificHook[AdjustCSIntLvLowAddr] (NBPtr, &i);

    if (NBPtr->MCTPtr->Status[Sb128bitmode]) {
      i++;
      j++;
    }

    for (Cs = 0; Cs < MAX_CS_PER_CHANNEL; Cs += 2) {
      //
      // LRDIMMS - Add an offset to the bit positions specified based on D18F2x[6C:60]_dct[1:0][RankDef] as follows:
      //    RankDef=0xb: 0 RankDef=10b: 1 RankDef=11b: 2
      //    Using RankMult information:  Lo/HiBit <<= (Mult >> 1)
      //
      if (NBPtr->MCTPtr->Status[SbLrdimms]) {
        Offset = ((NBPtr->ChannelPtr->LrDimmRankMult[Cs >> 1]) >> 1);
      }
      BaseRegS0 = NBPtr->GetBitField (NBPtr, BFCSBaseAddr0Reg + Cs);
      BaseRegS1 = NBPtr->GetBitField (NBPtr, BFCSBaseAddr0Reg + Cs + 1);
      if (((BaseRegS0 | BaseRegS1) & 1) != 0) {
        // Swap Mask register bits
        MaskReg = NBPtr->GetBitField (NBPtr, BFCSMask0Reg + (Cs / 2));
        IDS_HDT_CONSOLE(MEM_FLOW,"\t\tDimm %d Mask    : %X --> ",(Cs / 2), MaskReg);
        CsIntSwap (&MaskReg, EnChipSels, (i + Offset), (j + Offset));
        IDS_HDT_CONSOLE(MEM_FLOW,"%X\n",MaskReg);
        NBPtr->SetBitField (NBPtr, BFCSMask0Reg + (Cs / 2), MaskReg);

        // Swap Base register bits
        IDS_HDT_CONSOLE(MEM_FLOW,"\t\tCS %d Base Addr : %X --> ", Cs, BaseRegS0);
        CsIntSwap (&BaseRegS0, EnChipSels, (i + Offset), (j + Offset));
        IDS_HDT_CONSOLE(MEM_FLOW,"%X\n", BaseRegS0);
        NBPtr->SetBitField (NBPtr, BFCSBaseAddr0Reg + Cs, BaseRegS0);
        IDS_HDT_CONSOLE_DEBUG_CODE(
          if (( BaseRegS1 & 1) != 0) {
            IDS_HDT_CONSOLE(MEM_FLOW,"\t\tCS %d Base Addr : %X --> ", (Cs + 1), BaseRegS1);
          }
        )
        CsIntSwap (&BaseRegS1, EnChipSels, (i + Offset), (j + Offset));
        IDS_HDT_CONSOLE_DEBUG_CODE(
          if (( BaseRegS1 & 1) != 0) {
            IDS_HDT_CONSOLE(MEM_FLOW,"%X\n", BaseRegS1);
          }
        )
        NBPtr->SetBitField (NBPtr, BFCSBaseAddr0Reg + Cs + 1, BaseRegS1);
      }
    }
    //
    // Bank Interleaving is requested and has been enabled as well
    //
    NBPtr->MCTPtr->DctData[Dct].BkIntDis = FALSE;
    return TRUE;
  } else {
    //
    // Bank Interleaving is requested but cannot be enabled
    //
    PutEventLog (AGESA_WARNING, MEM_WARNING_BANK_INTERLEAVING_NOT_ENABLED, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
    SetMemError (AGESA_WARNING, NBPtr->MCTPtr);
    NBPtr->MCTPtr->DctData[Dct].BkIntDis = TRUE;
  }
  return FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This supporting function swaps Chip selects
 *
 *     @param[in,out]  *BaseMaskRegPtr   - Pointer to the Mask Register
 *     @param[in]      *EnChipSels    - Chip Selects to Enable
 *     @param[in]      *LoBit         - Lowest Bit
 *     @param[in]      *HiBit         - Highest Bit
 *
 *
 */

VOID
STATIC
CsIntSwap (
  IN OUT   UINT32 *BaseMaskRegPtr,
  IN       UINT8 EnChipSels,
  IN       UINT8 LoBit,
  IN       UINT8 HiBit
  )
{
  UINT8 BitDelta;
  UINT32 TempHi;
  UINT32 TempLo;
  UINT32 AddrLoMask;
  UINT32 AddrHiMask;

  ASSERT (BaseMaskRegPtr != NULL);
  ASSERT (HiBit > LoBit);

  BitDelta = HiBit - LoBit;
  AddrLoMask = (((UINT32)EnChipSels) - 1) << LoBit;
  AddrHiMask = AddrLoMask << BitDelta;

  TempHi = TempLo = *BaseMaskRegPtr;
  TempLo &= AddrLoMask;
  TempLo <<= BitDelta;     // move lower bits to upper bit position
  TempHi &= AddrHiMask;
  TempHi >>= BitDelta;     // move upper bits to lower bit position

  *BaseMaskRegPtr &= ~AddrLoMask;
  *BaseMaskRegPtr &= ~AddrHiMask;
  *BaseMaskRegPtr |= TempLo | TempHi;
}

