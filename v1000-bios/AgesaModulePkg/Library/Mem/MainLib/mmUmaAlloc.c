/* $NoKeywords:$ */
/**
 * @file
 *
 * mmUmaAlloc.c
 *
 * Main Memory Feature implementation file for UMA allocation.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
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
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "mport.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE LIBRARY_MEM_MAINLIB_MMUMAALLOC_FILECODE
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
MemMUmaAlloc (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  );

/*-----------------------------------------------------------------------------
*                                EXPORTED FUNCTIONS
*
*-----------------------------------------------------------------------------
*/

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *    UMA allocation mechanism.
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 */
BOOLEAN
MemMUmaAlloc (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  UINT32 TOM;
  UINT32 TOM2;
  UINT32 UmaSize;
  UINT32 TopOfChIntlv;
  UINT32 DctSelHi;
  UINT32 UmaAlignment;
  UINT32 UmaAbove4GBase;
  UINT32 UmaBelow4GBase;
  BOOLEAN DctSelIntLvEn;
  BOOLEAN UmaAbove4GEn;
  S_UINT64 SMsr;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  UMA_INFO *UmaInfoPtr;
  UINT8 Node;

  MEM_DATA_STRUCT *MemPtr;
  MEM_NB_BLOCK  *NBPtr;
  MEM_PARAMETER_STRUCT *RefPtr;

  MemPtr = MemMainPtr->MemPtr;
  NBPtr = &(MemMainPtr->NBPtr[BSP_DIE]);
  RefPtr = NBPtr->RefPtr;

  TOM2 = 0;
  SMsr.Value64 = 0;
  UmaAbove4GBase = 0;
  RefPtr->UmaBase = 0;
  UmaAlignment = (UINT32) RefPtr->UmaAlignment;
  UmaAbove4GEn = RefPtr->UmaAbove4G;
  DctSelIntLvEn = (NBPtr->GetBitField (NBPtr, BFDctSelIntLvEn) != 0) ? TRUE : FALSE;
  TopOfChIntlv = NBPtr->GetBitField (NBPtr, BFDctSelBaseAddr) << (27 - 16);
  DctSelHi = NBPtr->GetBitField (NBPtr, BFDctSelHi);

  // Allocate heap for UMA_INFO
  AllocHeapParams.RequestedBufferSize = sizeof (UMA_INFO);
  AllocHeapParams.BufferHandle = AMD_UMA_INFO_HANDLE;
  AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
  if (AGESA_SUCCESS != HeapAllocateBuffer (&AllocHeapParams, &MemPtr->StdHeader)) {
    ASSERT(FALSE); // Could not allocate heap for Uma information.
    return FALSE;
  }
  UmaInfoPtr = (UMA_INFO *) AllocHeapParams.BufferPtr;
  // Default all the fields of UMA_INFO
  UmaInfoPtr->UmaMode = (UINT8) UMA_NONE;
  UmaInfoPtr->UmaSize = 0;
  UmaInfoPtr->UmaBase = 0;
  UmaInfoPtr->UmaAttributes = 0;
  UmaInfoPtr->MemClock = NBPtr->DCTPtr->Timings.TargetSpeed;

  switch (RefPtr->UmaMode) {
  case UMA_NONE:
    UmaSize = 0;
    break;
  case UMA_SPECIFIED:
    UmaSize = RefPtr->UmaSize;
    break;
  case UMA_AUTO:
    UmaSize = NBPtr->GetUmaSize (NBPtr);
    break;
  default:
    UmaSize = 0;
    IDS_ERROR_TRAP;
  }

  if (UmaSize != 0) {
    //TOM scaled from [47:0] to [47:16]
    SMsr.Value64 = AsmReadMsr64 (TOP_MEM);
    TOM = (SMsr.Values32.lo >> 16) | (SMsr.Values32.hi << (32 - 16));

    UmaBelow4GBase = (TOM - UmaSize) & UmaAlignment;
    // Initialize Ref->UmaBase to UmaBelow4GBase
    RefPtr->UmaBase = UmaBelow4GBase;

    // Uma Above 4G support
    if (UmaAbove4GEn) {
      //TOM2 scaled from [47:0] to [47:16]
      SMsr.Value64 = AsmReadMsr64 (TOP_MEM2);
      TOM2 = (SMsr.Values32.lo >> 16) | (SMsr.Values32.hi << (32 - 16));
      if (TOM2 != 0) {
        UmaAbove4GBase = (TOM2 - UmaSize) & UmaAlignment;
        //Set UmaAbove4GBase to 0 if UmaAbove4GBase is below 4GB
        if (UmaAbove4GBase < _4GB_RJ16) {
          UmaAbove4GBase = 0;
        }
        if (UmaAbove4GBase != 0) {
          RefPtr->UmaBase = UmaAbove4GBase;
          // 1. TopOfChIntlv == 0 indicates that whole DCT0 and DCT1 memory are interleaved.
          // 2. TopOfChIntlv >= TOM tells us :
          //   -All or portion of Uma region that above 4G is NOT interleaved.
          //   -Whole Uma region that below 4G is interleaved.
          if (DctSelIntLvEn && (TopOfChIntlv >= TOM)) {
            RefPtr->UmaBase = UmaBelow4GBase;
          }
        }
      }
    }

    UmaInfoPtr->UmaMode = (UINT8) (RefPtr->UmaMode);
    UmaInfoPtr->UmaBase = (UINT64) ((UINT64) RefPtr->UmaBase << 16);

    if (RefPtr->UmaBase >= _4GB_RJ16) {
      // UmaSize might be extended if it is 128MB or 256MB .. aligned, so update it.
      RefPtr->UmaSize = TOM2 - UmaAbove4GBase;
      if (DctSelIntLvEn && (TopOfChIntlv == 0)) {
        UmaInfoPtr->UmaAttributes = UMA_ATTRIBUTE_INTERLEAVE | UMA_ATTRIBUTE_ON_DCT0 | UMA_ATTRIBUTE_ON_DCT1;
      } else {
        // Entire UMA region is in the high DCT
        UmaInfoPtr->UmaAttributes = (DctSelHi == 0) ? UMA_ATTRIBUTE_ON_DCT0 : UMA_ATTRIBUTE_ON_DCT1;
      }
    } else {
      // UmaSize might be extended if it is 128MB or 256MB .. aligned, so update it.
      RefPtr->UmaSize = TOM - UmaBelow4GBase;
      if (DctSelIntLvEn && ((TopOfChIntlv == 0) || (TopOfChIntlv >= TOM))) {
        UmaInfoPtr->UmaAttributes = UMA_ATTRIBUTE_INTERLEAVE | UMA_ATTRIBUTE_ON_DCT0 | UMA_ATTRIBUTE_ON_DCT1;
      } else {
        if (UmaBelow4GBase >= TopOfChIntlv) {
          // Entire UMA region is in the high DCT
          UmaInfoPtr->UmaAttributes = (DctSelHi == 0) ? UMA_ATTRIBUTE_ON_DCT0 : UMA_ATTRIBUTE_ON_DCT1;
        } else if (TopOfChIntlv >= TOM) {
          // Entire UMA region is in the low DCT
          UmaInfoPtr->UmaAttributes = (DctSelHi == 1) ? UMA_ATTRIBUTE_ON_DCT0 : UMA_ATTRIBUTE_ON_DCT1;
        } else {
          // UMA region is in both DCT0 and DCT1
          UmaInfoPtr->UmaAttributes = UMA_ATTRIBUTE_ON_DCT0 | UMA_ATTRIBUTE_ON_DCT1;
        }
      }
    }
    UmaInfoPtr->UmaSize = (RefPtr->UmaSize) << 16;
    IDS_HDT_CONSOLE (MEM_FLOW, "\tUMA is allocated:\n\t\tBase: %x0000\n\t\tSize: %x0000\n", RefPtr->UmaBase, RefPtr->UmaSize);
    NBPtr->FamilySpecificHook[FixupUmaInfo] (NBPtr, UmaInfoPtr);
  } else {
    IDS_HDT_CONSOLE (MEM_FLOW, "\tNo UMA region is allocated !");
    // Update the results
    RefPtr->UmaSize = 0;
    RefPtr->UmaBase = 0;
  }

  // Wrap UMA with ECC exclusion range so that MCT recognizes the data written by GMC
  if (MemMainPtr->mmSharedPtr->AllECC == TRUE) {
    MemMDisableScrubber (MemMainPtr);
    for (Node = 0; Node < MemMainPtr->DieCount; Node ++) {
      NBPtr = &(MemMainPtr->NBPtr[Node]);
      NBPtr->MemNSetEccExclusionRange (NBPtr);
    }
    MemMRestoreScrubber (MemMainPtr);
  }

  return TRUE;
}


