/* $NoKeywords:$ */
/**
 * @file
 *
 * mt.c
 *
 * Common Technology file
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

#include <Library/BaseLib.h>
#include "AGESA.h"
#include "Library/AmdBaseLib.h"
#include "mport.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE LIBRARY_MEM_TECHLIB_MT_FILECODE

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
MemTDefaultTechnologyHook (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  );
/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function is the default return for non-training technology features
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 */
BOOLEAN
MemTFeatDef (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function sets the TestFail bit for all CS that fail training.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 */
VOID
MemTMarkTrainFail (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  MEM_NB_BLOCK *NBPtr;
  UINT8 Dct;
  UINT8 ChipSel;

  NBPtr = TechPtr->NBPtr;
  for (Dct = 0; Dct < NBPtr->DctCount; Dct ++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    NBPtr->DCTPtr->Timings.CsEnabled &= ~NBPtr->DCTPtr->Timings.CsTrainFail;
    for (ChipSel = 0; ChipSel < MAX_CS_PER_CHANNEL; ChipSel ++) {
      if ((NBPtr->DCTPtr->Timings.CsTrainFail & ((UINT16)1 << ChipSel)) != 0) {
        NBPtr->SetBitField (NBPtr, (BFCSBaseAddr0Reg + ChipSel), (UINT32)1 << BFTestFail);
      }
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function sets the initial controller environment before training.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemTBeginTraining (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  S_UINT64 SMsr;
  MEM_DATA_STRUCT *MemPtr;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;
  MemPtr = NBPtr->MemPtr;

  TechPtr->CR4reg = AsmReadCr4 ();
  AsmWriteCr4 (TechPtr->CR4reg | ((UINT32)1 << 9)); // enable SSE2

  SMsr.Value64 = AsmReadMsr64 (HWCR);            // HWCR
  TechPtr->HwcrLo = SMsr.Values32.lo;
  SMsr.Values32.lo |= 0x00020000;                  // turn on HWCR.wrap32dis
  SMsr.Values32.lo &= 0xFFFF7FFF;                  // turn off HWCR.SSEDIS
  AsmWriteMsr64 (HWCR, SMsr.Value64);

  TechPtr->DramEcc = (UINT8) NBPtr->GetBitField (NBPtr, BFDramEccEn);
  NBPtr->SetBitField (NBPtr, BFDramEccEn, 0); // Disable ECC
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function sets the final controller environment after training.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemTEndTraining (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  S_UINT64 SMsr;
  MEM_DATA_STRUCT *MemPtr;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;
  MemPtr = NBPtr->MemPtr;

  AsmWriteCr4 (TechPtr->CR4reg);

  SMsr.Value64 = AsmReadMsr64 (HWCR);
  SMsr.Values32.lo = TechPtr->HwcrLo;
  AsmWriteMsr64 (HWCR, SMsr.Value64);

  NBPtr->SetBitField (NBPtr, BFDramEccEn, TechPtr->DramEcc);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function sets all the bytelanes/nibbles to the same delay value
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       Dly - Delay value to set
 *
 */

VOID
MemTSetDQSDelayAllCSR (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dly
  )
{
  UINT8 i;
  UINT8 MaxBytelanes;
  MaxBytelanes = (TechPtr->NBPtr->MCTPtr->Status[SbEccDimms] && TechPtr->NBPtr->IsSupported[EccByteTraining]) ? 9 : 8;

  for (i = 0; i <  MaxBytelanes; i++) {
    TechPtr->SetDQSDelayCSR (TechPtr, i, Dly);
  }
  TechPtr->NBPtr->FamilySpecificHook[RegAccessFence] (TechPtr->NBPtr, NULL);
}
/*-----------------------------------------------------------------------------
 *
 *
 *     This function is used to intialize common technology functions
 *
 *     @param[in,out]  *TechPtr     - Pointer to the MEM_TECH_BLOCK
 * ----------------------------------------------------------------------------
 */
VOID
MemTCommonTechInit (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8 i;
  for (i = 0; i < NumberOfTechHooks; i++) {
    TechPtr->TechnologySpecificHook[i] = MemTDefaultTechnologyHook;
  }
}
/*-----------------------------------------------------------------------------
 *
 *
 *     This function is an empty function used to intialize TechnologySpecificHook array
 *
 *     @param[in,out]  *TechPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  OptParam   - Optional parameter
 *
 *     @return  FALSE - always
 * ----------------------------------------------------------------------------
 */
BOOLEAN
STATIC
MemTDefaultTechnologyHook (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  )
{
  return FALSE;
}

