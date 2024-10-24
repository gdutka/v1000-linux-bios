/* $NoKeywords:$ */
/**
 * @file
 *
 * mplribt.c
 *
 * A sub-engine which extracts F0RC8, F1RC0, F1RC1 and F1RC2 value for LRDIMM configuration.
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
#define FILECODE LIBRARY_MEM_PSLIB_MPLRIBT_FILECODE


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
MemPGetLRIBT (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_PSC_TABLE_BLOCK *EntryOfTables
  );
/* -----------------------------------------------------------------------------*/
/**
 *
 *    A sub-function which extracts LRDIMM F0RC8, F1RC0, F1RC1 and F1RC2 value from a input
 *    table and stores extracted value to a specific address.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *EntryOfTables     - Pointer to MEM_PSC_TABLE_BLOCK
 *
 *     @return          TRUE - Succeed in extracting the table value
 *     @return          FALSE - Fail to extract the table value
 *
 */
BOOLEAN
MemPGetLRIBT (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_PSC_TABLE_BLOCK *EntryOfTables
  )
{
  UINT8 i;
  UINT8 MaxDimmPerCh;
  UINT8 NOD;
  UINT8 TableSize;
  UINT32 CurDDRrate;
  UINT8 DDRVoltage;
  UINT16 RankTypeOfPopulatedDimm;
  UINT16 RankTypeInTable;
  UINT8 PsoMaskLRIBT;
  SOC_LOGICAL_ID LogicalCpuid;
  UINT8 PackageType;
  PSCFG_L_IBT_ENTRY *TblPtr;
  CH_DEF_STRUCT *CurrentChannel;

  CurrentChannel = NBPtr->ChannelPtr;

  if (CurrentChannel->LrDimmPresent == 0) {
    return TRUE;
  }

  TblPtr = NULL;
  TableSize = 0;
  PackageType = 0;
  LogicalCpuid.Family = AMD_FAMILY_UNKNOWN;
  MaxDimmPerCh = GetMaxDimmsPerChannel (NBPtr->RefPtr->PlatformMemoryConfiguration, NBPtr->MCTPtr->SocketId, CurrentChannel->ChannelID);
  NOD = (UINT8) 1 << (MaxDimmPerCh - 1);

  i = 0;
  // Obtain table pointer, table size, Logical Cpuid and PSC type according to NB type and package type.
  while (EntryOfTables->TblEntryOfLRIBT[i] != NULL) {
    if (((EntryOfTables->TblEntryOfLRIBT[i])->Header.NumOfDimm & NOD) != 0) {
      LogicalCpuid = (EntryOfTables->TblEntryOfLRIBT[i])->Header.LogicalCpuid;
      PackageType = (EntryOfTables->TblEntryOfLRIBT[i])->Header.PackageType;
      //
      // Determine if this is the expected NB Type
      //
      if (MemPIsIdSupported (NBPtr, LogicalCpuid, PackageType)) {
        TblPtr = (PSCFG_L_IBT_ENTRY *) ((EntryOfTables->TblEntryOfLRIBT[i])->TBLPtr);
        TableSize = (EntryOfTables->TblEntryOfLRIBT[i])->TableSize;
        break;
      }
    }
    i++;
  }

  // Check whether no table entry is found.
  if (EntryOfTables->TblEntryOfLRIBT[i] == NULL) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\nNo LRDIMM IBT table\n");
    return FALSE;
  }

  CurDDRrate = (UINT32) (1 << (CurrentChannel->DCTPtr->Timings.Speed / 66));
  DDRVoltage = (UINT8) (1 << CONVERT_VDDIO_TO_ENCODED (NBPtr->RefPtr->DDRVoltage, CurrentChannel->TechType));
  RankTypeOfPopulatedDimm = MemPGetPsRankType (CurrentChannel);

  for (i = 0; i < TableSize; i++) {
    MemPConstructRankTypeMap ((UINT16) TblPtr->Dimm0, (UINT16) TblPtr->Dimm1, (UINT16) TblPtr->Dimm2, &RankTypeInTable);
    if ((TblPtr->DimmPerCh & NOD) != 0) {
      if ((TblPtr->DDRrate & CurDDRrate) != 0) {
        if ((TblPtr->VDDIO & DDRVoltage) != 0) {
          if ((RankTypeInTable & RankTypeOfPopulatedDimm) == RankTypeOfPopulatedDimm) {
            NBPtr->PsPtr->F0RC8 = (UINT8) TblPtr->F0RC8;
            NBPtr->PsPtr->F1RC0 = (UINT8) TblPtr->F1RC0;
            NBPtr->PsPtr->F1RC1 = (UINT8) TblPtr->F1RC1;
            NBPtr->PsPtr->F1RC2 = (UINT8) TblPtr->F1RC2;
            break;
          }
        }
      }
    }
    TblPtr++;
  }
  //
  // If there is no entry, check if overriding value existed. If not, return FALSE
  //
  PsoMaskLRIBT = (UINT8) MemPProceedTblDrvOverride (NBPtr, NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_TBLDRV_LRDIMM_IBT);
  if ((PsoMaskLRIBT == 0) && (i == TableSize)) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\nNo LRDIMM IBT entries\n");
    PutEventLog (AGESA_ERROR, MEM_ERROR_LR_IBT_NOT_FOUND, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
    SetMemError (AGESA_ERROR, NBPtr->MCTPtr);
    if (!NBPtr->MemPtr->ErrorHandling (NBPtr->MCTPtr, NBPtr->Dct, EXCLUDE_ALL_CHIPSEL, &NBPtr->MemPtr->StdHeader)) {
      ASSERT (FALSE);
    }
    return FALSE;
  }

  return TRUE;
}

