/* $NoKeywords:$ */
/**
 * @file
 *
 * mfemp.c
 *
 * Feature EMP initialization functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/ECC)
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
#include "Library/AmdBaseLib.h"
#include "Library/AmdMemBaseV5Lib.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEM_FEATECCLIB_MFEMP_FILECODE
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
IsPowerOfTwo (
  IN       UINT32 TestNumber
  );

BOOLEAN
MemFInitEMP (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

/*
 *-----------------------------------------------------------------------------
 *                                EXPORTED FUNCTIONS
 *
 *-----------------------------------------------------------------------------
 */

extern BUILD_OPT_CFG UserOptions;

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function initializes EMP (Enhanced Memory Protection)
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  This feature is enabled.
 *     @return          FALSE - This feature is not enabled.
 */

BOOLEAN
MemFInitEMP (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MEM_PARAMETER_STRUCT *RefPtr;
  DIE_STRUCT *MCTPtr;

  ASSERT (NBPtr != NULL);

  RefPtr = NBPtr->RefPtr;
  MCTPtr = NBPtr->MCTPtr;

  if (RefPtr->EnableEccFeature) {
    if (NBPtr->GetBitField (NBPtr, BFEnhMemProtCap) == 0) {
      PutEventLog (AGESA_WARNING, MEM_WARNING_EMP_NOT_SUPPORTED, 0, 0, 0, 0, &NBPtr->MemPtr->StdHeader);
      MCTPtr->ErrStatus[EsbEMPNotSupported] = TRUE;
    } else if (RefPtr->EnableChannelIntlv || RefPtr->EnableBankIntlv || RefPtr->EnableBankSwizzle) {
      PutEventLog (AGESA_WARNING, MEM_WARNING_EMP_CONFLICT, 0, 0, 0, 0, &NBPtr->MemPtr->StdHeader);
      MCTPtr->ErrStatus[EsbEMPConflict] = TRUE;
    } else if ((!MCTPtr->GangedMode) &&
               (!IsPowerOfTwo (MCTPtr->DctData[0].Timings.DctMemSize) &&
                !IsPowerOfTwo (MCTPtr->DctData[1].Timings.DctMemSize))) {
      PutEventLog (AGESA_WARNING, MEM_WARNING_EMP_NOT_ENABLED, 0, 0, 0, 0, &NBPtr->MemPtr->StdHeader);
      MCTPtr->ErrStatus[EsbEMPDis] = TRUE;
    } else {
      // Reduce memory size to 7/8 of the original memory size
      ASSERT ((MCTPtr->NodeMemSize % 8) == 0);
      NBPtr->SetBitField (NBPtr, BFDramHoleValid, 0);
      MCTPtr->NodeMemSize = (MCTPtr->NodeMemSize / 8) * 7;
      NBPtr->HtMemMapInit (NBPtr);
      NBPtr->CpuMemTyping (NBPtr);

      // Enable EMP
      NBPtr->SetBitField (NBPtr, BFDramEccEn, 1);

      // Scrub CTL settings for Dcache, L2
      NBPtr->SetBitField (NBPtr, BFL2Scrub, UserOptions.CfgScrubL2Rate);
      NBPtr->SetBitField (NBPtr, BFDcacheScrub, UserOptions.CfgScrubDcRate);

      NBPtr->SetBitField (NBPtr, BFSyncOnUcEccEn, UserOptions.CfgEccSyncFlood);
      return TRUE;
    }
  }
  return FALSE;
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
 *      This function checks to see if the input is power of two.
 *
 *     @param[in]    TestNumber  - Value to check for power of two
 *
 *     @return          TRUE     - is power of two
 *                      FALSE    - is not power of two
 */
BOOLEAN
STATIC
IsPowerOfTwo (
  IN       UINT32 TestNumber
  )
{
  return (BOOLEAN) ((TestNumber & (TestNumber - 1)) == 0);
}

