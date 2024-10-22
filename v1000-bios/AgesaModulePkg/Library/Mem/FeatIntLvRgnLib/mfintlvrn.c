/* $NoKeywords:$ */
/**
 * @file
 *
 * mfrintlv.c
 *
 * Feature Region interleaving support
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/Intlvrgn)
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
#include "Library/AmdHeapLib.h"
#include "mport.h"
#include "mm.h"
#include "mn.h"
#include "Library/AmdMemFeatIntLvRgnLib.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEM_FEATINTLVRGNLIB_MFINTLVRN_FILECODE

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define _4GB_RJ27 ((UINT32)4 << (30 - 27))
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
 *   MemFInterleaveRegion:
 *
 *  Applies region interleaving if both DCTs have different size of memory, and
 *  the channel interleaving region doesn't have UMA covered.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemFInterleaveRegion (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 TOM;
  UINT32 TOM2;
  UINT32 TOMused;
  UINT32 UmaBase;
  UINT32 DctSelBase;
  S_UINT64 SMsr;
  LOCATE_HEAP_PTR LocHeap;
  UMA_INFO *UmaInfoPtr;

  MEM_DATA_STRUCT *MemPtr;
  MEM_PARAMETER_STRUCT *RefPtr;

  MemPtr = NBPtr->MemPtr;
  RefPtr = NBPtr->RefPtr;

  UmaBase = (UINT32) RefPtr->UmaBase >> (27 - 16);

  //TOM scaled from [47:0] to [47:27]
  SMsr.Value64 = AsmReadMsr64 (TOP_MEM);
  SMsr.Values32.lo += (16 << 20);  // Add 16MB to gain back C6 region if C6 is enabled
  TOM = (SMsr.Values32.lo >> 27) | (SMsr.Values32.hi << (32 - 27));

  //TOM2 scaled from [47:0] to [47:27]
  SMsr.Value64 = AsmReadMsr64 (TOP_MEM2);
  TOM2 = (SMsr.Values32.lo >> 27) | (SMsr.Values32.hi << (32 - 27));

  TOMused = (UmaBase >= _4GB_RJ27) ? TOM2 : TOM;

  if (UmaBase != 0) {
    //Check if channel interleaving is enabled ? if so, go to next step.
    if (NBPtr->GetBitField (NBPtr, BFDctSelIntLvEn) == 1) {
      DctSelBase = NBPtr->GetBitField (NBPtr, BFDctSelBaseAddr);
      //Skip if DctSelBase is equal to 0, because DCT0 has as the same memory size as DCT1.
      if (DctSelBase != 0) {
        //We need not enable swapped interleaved region when channel interleaving region has covered all of the UMA.
        if (DctSelBase < TOMused) {
          NBPtr->EnableSwapIntlvRgn (NBPtr, UmaBase, TOMused);

          // Set UMA attribute to interleaved after interleaved region has been swapped
          LocHeap.BufferHandle = AMD_UMA_INFO_HANDLE;
          if (HeapLocateBuffer (&LocHeap, &(NBPtr->MemPtr->StdHeader)) == AGESA_SUCCESS) {
            UmaInfoPtr = (UMA_INFO *) LocHeap.BufferPtr;
            UmaInfoPtr->UmaAttributes = UMA_ATTRIBUTE_INTERLEAVE | UMA_ATTRIBUTE_ON_DCT0 | UMA_ATTRIBUTE_ON_DCT1;
          } else {
            ASSERT (FALSE);
          }
        }
      }
    }
  }
}



