/* $NoKeywords:$ */
/**
 * @file
 *
 * mpmr0.c
 *
 * A sub-engine which extracts MR0[WR] and MR0[CL] value.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps)
 * @e \$Revision: 311625 $ @e \$Date: 2015-01-26 10:35:21 +0800 (Mon, 26 Jan 2015) $
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
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "ma.h"
#include "mp.h"
#include "merrhdl.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)
#define FILECODE LIBRARY_MEM_PSLIB_MPMR0_FILECODE


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
BOOLEAN
MemPGetMR0WrCL (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_PSC_TABLE_BLOCK *EntryOfTables
  );
/* -----------------------------------------------------------------------------*/
/**
 *
 *    A sub-function which extracts MR0[WR] or MR0[CL] value from a input table and store the
 *    value to a specific address.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *EntryOfTables     - Pointer to MEM_PSC_TABLE_BLOCK
 *
 *     @return          TRUE - Succeed in extracting the table value
 *     @return          FALSE - Fail to extract the table value
 *
 */
BOOLEAN
MemPGetMR0WrCL (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_PSC_TABLE_BLOCK *EntryOfTables
  )
{

  UINT8 i;
  UINT8 j;
  UINT8 p;
  UINT32 Value32;
  UINT8 TableSize;
  PSCFG_TYPE Type;
  SOC_LOGICAL_ID LogicalCpuid;
  UINT8 PackageType;
  UINT8 PsoMaskMR0;
  PSCFG_MR0CL_ENTRY *TblPtr;
  PSC_TBL_ENTRY **ptr;

  TblPtr = NULL;
  TableSize = 0;
  PsoMaskMR0 = 0;

  // Extract MR0[WR] value, then MR0[CL] value
  for (i = 0; i < 2; i++) {
    if (i == 0) {
      ptr = EntryOfTables->TblEntryOfMR0WR;
      Type = PSCFG_MR0WR;
    } else {
      ptr = EntryOfTables->TblEntryOfMR0CL;
      Type = PSCFG_MR0CL;
    }

    p = 0;
    // Obtain table pointer, table size, Logical Cpuid and PSC type according to Dimm, NB and package type.
    while (ptr[p] != NULL) {
      //
      // Determine if this is the expected NB Type
      //
      LogicalCpuid = (ptr[p])->Header.LogicalCpuid;
      PackageType = (ptr[p])->Header.PackageType;
      if (MemPIsIdSupported (NBPtr, LogicalCpuid, PackageType)) {
        TblPtr = (PSCFG_MR0CL_ENTRY *) ((ptr[p])->TBLPtr);
        TableSize = (ptr[p])->TableSize;
        break;
      }
      p++;
    }

    // Check whether no table entry is found.
    if (ptr[p] == NULL) {
      IDS_HDT_CONSOLE (MEM_FLOW, "\nNo MR0 table\n");
      return FALSE;
    }

    Value32 = (Type == PSCFG_MR0WR) ? NBPtr->GetBitField (NBPtr, BFTwr) : NBPtr->GetBitField (NBPtr, BFTcl);
    for (j = 0; j < TableSize; j++, TblPtr++) {
      if (Value32 == (UINT32) TblPtr->Timing) {
        if (Type == PSCFG_MR0WR) {
          NBPtr->PsPtr->MR0WR = (UINT8) TblPtr->Value;
          break;
        } else {
          NBPtr->PsPtr->MR0CL31 = (UINT8) TblPtr->Value;
          NBPtr->PsPtr->MR0CL0 = (UINT8) TblPtr->Value1;
          break;
        }
      }
    }

    //
    // If there is no entry, check if overriding value existed. If not, return FALSE.
    //
    PsoMaskMR0 = (UINT8) MemPProceedTblDrvOverride (NBPtr, NBPtr->RefPtr->PlatformMemoryConfiguration, ((i == 0) ? PSO_TBLDRV_MR0_WR : PSO_TBLDRV_MR0_CL));
    if ((PsoMaskMR0 == 0) && (j == TableSize)) {
      IDS_HDT_CONSOLE (MEM_FLOW, (i == 0) ? "\nNo MR0[WR] entries\n" : "\nNo MR0[CL] entries\n");
      PutEventLog (AGESA_ERROR, MEM_ERROR_MR0_NOT_FOUND, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
      SetMemError (AGESA_ERROR, NBPtr->MCTPtr);
      if (!NBPtr->MemPtr->ErrorHandling (NBPtr->MCTPtr, NBPtr->Dct, EXCLUDE_ALL_CHIPSEL, &NBPtr->MemPtr->StdHeader)) {
        ASSERT (FALSE);
      }
      return FALSE;
    }
  }

  return TRUE;
}
