/* $NoKeywords:$ */
/**
 * @file
 *
 * mfchi.c
 *
 * Feature Channel interleaving support
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/Chintlv)
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
#include "mm.h"
#include "mn.h"
#include "Library/AmdMemFeatChIntLvLib.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEM_FEATCHINTLVLIB_MFCHI_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define _4GB_ (0x10000ul >> 10)

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
 *   MemFInterleaveChannels:
 *
 *  Applies DIMM channel interleaving if enabled, if not ganged mode, and
 *  there are valid dimms in both channels.  Called once per Node.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  This feature is enabled.
 *     @return          FALSE - This feature is not enabled.
 */

BOOLEAN
MemFInterleaveChannels (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 DramBase;
  UINT32 DctSelBase;
  UINT32 HoleSize;
  UINT32 HoleBase;
  UINT32 HoleOffset;
  UINT32 Dct0Size;
  UINT32 Dct1Size;
  UINT32 SmallerDct;
  UINT8 DctSelIntLvAddr;
  UINT8 DctSelHi;
  UINT8 DctSelHiRngEn;
  UINT32 HoleValid;

  MEM_PARAMETER_STRUCT *RefPtr;
  DIE_STRUCT *MCTPtr;

  ASSERT (NBPtr != NULL);

  RefPtr = NBPtr->RefPtr;

  DctSelIntLvAddr = NBPtr->DefDctSelIntLvAddr;
  if (RefPtr->EnableChannelIntlv) {
    HoleSize = 0;
    HoleBase = 0;
    if (RefPtr->GStatus[GsbSoftHole] || RefPtr->GStatus[GsbHWHole]) {
      // HoleBase scaled from [47:16] to [47:26]
      HoleBase = RefPtr->HoleBase >> 10;
      HoleSize = _4GB_ - HoleBase;
    }

    MCTPtr = NBPtr->MCTPtr;

    HoleValid = NBPtr->GetBitField (NBPtr, BFDramHoleValid);
    if ((!MCTPtr->GangedMode) &&
        (MCTPtr->DctData[0].Timings.DctMemSize != 0) &&
        (MCTPtr->DctData[1].Timings.DctMemSize != 0)) {
      // DramBase scaled [47:16] to [47:26]
      DramBase = MCTPtr->NodeSysBase >> 10;
      // Scale NodeSysLimit [47:16] to [47:26]
      Dct1Size = (MCTPtr->NodeSysLimit + 1) >> 10;
      Dct0Size = NBPtr->GetBitField (NBPtr, BFDctSelBaseOffset);
      if ((Dct0Size >= _4GB_) && (DramBase < HoleBase)) {
        Dct0Size -= HoleSize;
      }
      if ((Dct1Size >= _4GB_) && (DramBase < HoleBase)) {
        Dct1Size -= HoleSize;
      }
      Dct1Size -= Dct0Size;
      Dct0Size -= DramBase;

       // Select the bigger size DCT to put in DctSelHi
      DctSelHiRngEn = 1;
      DctSelHi = 0;
      SmallerDct = Dct1Size;
      if (Dct1Size == Dct0Size) {
        SmallerDct = 0;
        DctSelHiRngEn = 0;
      } else if (Dct1Size > Dct0Size) {
        SmallerDct = Dct0Size;
        DctSelHi = 1;
      }

      if (SmallerDct != 0) {
        DctSelBase = (SmallerDct * 2) + DramBase;
      } else {
        DctSelBase = 0;
      }
      if ((DctSelBase >= HoleBase) && (DramBase < HoleBase)) {
        DctSelBase += HoleSize;
      }
      IDS_OPTION_HOOK (IDS_CHANNEL_INTERLEAVE, &DctSelIntLvAddr, &(NBPtr->MemPtr->StdHeader));
      NBPtr->SetBitField (NBPtr, BFDctSelBaseAddr, DctSelBase >> 1);
      NBPtr->SetBitField (NBPtr, BFDctSelHiRngEn, DctSelHiRngEn);
      NBPtr->SetBitField (NBPtr, BFDctSelHi, DctSelHi);
      NBPtr->SetBitField (NBPtr, BFDctSelIntLvAddr, DctSelIntLvAddr);
      NBPtr->SetBitField (NBPtr, BFDctSelIntLvEn, 1);

       // DctSelBaseOffset = DctSelBaseAddr - Interleaved region
      NBPtr->SetBitField (NBPtr, BFDctSelBaseOffset, DctSelBase - SmallerDct);

       // Adjust DramHoleOffset
      if (HoleValid != 0) {
        HoleOffset = DramBase;
        if ((DctSelBase < HoleBase) && (DctSelBase != 0)) {
          HoleOffset += (DctSelBase - DramBase) >> 1;
        }
        HoleOffset += HoleSize;
        NBPtr->SetBitField (NBPtr, BFDramHoleOffset, HoleOffset << 3);
      }
    } else {
      //
      // Channel Interleaving is requested but cannot be enabled
      //
      PutEventLog (AGESA_WARNING, MEM_WARNING_CHANNEL_INTERLEAVING_NOT_ENABLED, NBPtr->Node, 0, 0, 0, &NBPtr->MemPtr->StdHeader);
      SetMemError (AGESA_WARNING, MCTPtr);
    }

    return TRUE;
  } else {
    return FALSE;
  }
}

