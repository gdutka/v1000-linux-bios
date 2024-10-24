/* $NoKeywords:$ */
/**
 * @file
 *
 * mpodtpat.c
 *
 * A sub-engine which extracts ODT pattern value.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps)
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
#include "Library/AmdMemBaseV5Lib.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "mu.h"
#include "ma.h"
#include "mp.h"
#include "merrhdl.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)
#define FILECODE LIBRARY_MEM_PSLIB_MPODTPAT_FILECODE


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
MemPGetODTPattern (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_PSC_TABLE_BLOCK *EntryOfTables
  );
/* -----------------------------------------------------------------------------*/
/**
 *
 *    A sub-function which extracts ODT Pattern value from a input table and stores extracted
 *    value to a specific address.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *EntryOfTables     - Pointer to MEM_PSC_TABLE_BLOCK
 *
 *     @return          TRUE - Table values can be extracted per dimm population and ranks type.
 *     @return          FALSE - Table values cannot be extracted per dimm population and ranks type.
 *
 */
BOOLEAN
MemPGetODTPattern (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_PSC_TABLE_BLOCK *EntryOfTables
  )
{
  UINT8 i;
  UINT16 RankTypeInTable;
  UINT16 RankTypeOfPopulatedDimm;
  UINT8 MaxDimmPerCh;
  UINT8 NOD;
  UINT8 TableSize;
  DIMM_TYPE DimmType;
  UINT8 PsoMaskOdtPat;
  SOC_LOGICAL_ID LogicalCpuid;
  UINT8 PackageType;
  PSCFG_3D_ODTPAT_ENTRY *TblPtr;
  CH_DEF_STRUCT *CurrentChannel;

  CurrentChannel = NBPtr->ChannelPtr;

  TblPtr = NULL;
  TableSize = 0;
  PackageType = 0;
  LogicalCpuid.Family = AMD_FAMILY_UNKNOWN;
  MaxDimmPerCh = GetMaxDimmsPerChannel (NBPtr->RefPtr->PlatformMemoryConfiguration, NBPtr->MCTPtr->SocketId, CurrentChannel->ChannelID);
  NOD = (UINT8) 1 << (MaxDimmPerCh - 1);

  if (CurrentChannel->RegDimmPresent != 0) {
    DimmType = RDIMM_TYPE;
  } else if (CurrentChannel->SODimmPresent != 0) {
    DimmType = SODIMM_TYPE;
  } else if (CurrentChannel->LrDimmPresent != 0) {
    DimmType = LRDIMM_TYPE;
  } else {
    DimmType = UDIMM_TYPE;
  }

  i = 0;
  // Obtain table pointer, table size, Logical Cpuid and PSC type according to Dimm, NB and package type.
  while (EntryOfTables->TblEntryOfODTPattern[i] != NULL) {
    if (((EntryOfTables->TblEntryOfODTPattern[i])->Header.DimmType & DimmType) != 0) {
      if (((EntryOfTables->TblEntryOfODTPattern[i])->Header.NumOfDimm & NOD) != 0) {
        if ((EntryOfTables->TblEntryOfODTPattern[i])->Header.TechType == CurrentChannel->TechType) {
          //
          // Determine if this is the expected NB Type
          //
          LogicalCpuid = (EntryOfTables->TblEntryOfODTPattern[i])->Header.LogicalCpuid;
          PackageType = (EntryOfTables->TblEntryOfODTPattern[i])->Header.PackageType;
          if (MemPIsIdSupported (NBPtr, LogicalCpuid, PackageType)) {
            TblPtr = (PSCFG_3D_ODTPAT_ENTRY *) ((EntryOfTables->TblEntryOfODTPattern[i])->TBLPtr);
            TableSize = (EntryOfTables->TblEntryOfODTPattern[i])->TableSize;
            break;
          }
        }
      }
    }
    i++;
  }

  // Check whether no table entry is found.
  if (EntryOfTables->TblEntryOfODTPattern[i] == NULL) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\nNo ODT table\n");
    return FALSE;
  }

  RankTypeOfPopulatedDimm = MemPGetPsRankType (CurrentChannel);

  for (i = 0; i < TableSize; i++) {
    MemPConstructRankTypeMap ((UINT16) TblPtr->Dimm0, (UINT16) TblPtr->Dimm1, (UINT16) TblPtr->Dimm2, &RankTypeInTable);
    if ((RankTypeInTable & RankTypeOfPopulatedDimm) == RankTypeOfPopulatedDimm) {
      CurrentChannel->PhyRODTCSHigh = TblPtr->RdODTCSHigh;
      CurrentChannel->PhyRODTCSLow = TblPtr->RdODTCSLow;
      CurrentChannel->PhyWODTCSHigh = TblPtr->WrODTCSHigh;
      CurrentChannel->PhyWODTCSLow = TblPtr->WrODTCSLow;

      //WL ODT
      if (MemNGetBitFieldNb (NBPtr, BFPerRankTimingEn) == 0) {
        CurrentChannel->PhyWLODT[0] = (UINT8) (CurrentChannel->PhyWODTCSLow & 0x0F);
        CurrentChannel->PhyWLODT[1] = (UINT8) ((CurrentChannel->PhyWODTCSLow  >> 16) & 0x0F);
        CurrentChannel->PhyWLODT[2] = (UINT8) (CurrentChannel->PhyWODTCSHigh & 0x0F);
        CurrentChannel->PhyWLODT[3] = (UINT8) ((CurrentChannel->PhyWODTCSHigh >> 16) & 0x0F);
      } else {
        *(UINT32 *) &(CurrentChannel->PhyWLODT[0]) = CurrentChannel->PhyWODTCSLow & 0xF0F0F0F;
      }

      break;
    }
    TblPtr++;
  }

  //
  // If there is no entry, check if overriding value existed. If not, return FALSE
  //
  PsoMaskOdtPat = (UINT8) MemPProceedTblDrvOverride (NBPtr, NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_TBLDRV_ODTPATTERN);
  if ((PsoMaskOdtPat == 0) && (i == TableSize)) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\nNo ODT entries\n");
    PutEventLog (AGESA_ERROR, MEM_ERROR_ODT_PATTERN_NOT_FOUND, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
    SetMemError (AGESA_ERROR, NBPtr->MCTPtr);
    if (!NBPtr->MemPtr->ErrorHandling (NBPtr->MCTPtr, NBPtr->Dct, EXCLUDE_ALL_CHIPSEL, &NBPtr->MemPtr->StdHeader)) {
      ASSERT (FALSE);
    }
    return FALSE;
  }

  return TRUE;
}
