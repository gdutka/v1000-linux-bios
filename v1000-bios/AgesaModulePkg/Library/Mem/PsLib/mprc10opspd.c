/* $NoKeywords:$ */
/**
 * @file
 *
 * mprc10opspd.c
 *
 * A sub-engine which extracts RC10 operating speed value for RDIMM configuration.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps)
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
#define FILECODE LIBRARY_MEM_PSLIB_MPRC10OPSPD_FILECODE


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
MemPGetRC10OpSpd (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_PSC_TABLE_BLOCK *EntryOfTables
  );
/* -----------------------------------------------------------------------------*/
/**
 *
 *    A sub-function which extracts RC10 operating speed value from a input table and stores extracted
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
MemPGetRC10OpSpd (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_PSC_TABLE_BLOCK *EntryOfTables
  )
{
  UINT8 i;
  UINT8 TableSize;
  UINT32 CurDDRrate;
  SOC_LOGICAL_ID LogicalCpuid;
  UINT8 PackageType;
  UINT8 PsoMaskRC10OpSpeed;
  PSCFG_OPSPD_ENTRY *TblPtr;
  CH_DEF_STRUCT *CurrentChannel;

  CurrentChannel = NBPtr->ChannelPtr;

  if (CurrentChannel->RegDimmPresent == 0) {
    return TRUE;
  }

  TblPtr = NULL;
  TableSize = 0;
  PackageType = 0;
  LogicalCpuid.Family = AMD_FAMILY_UNKNOWN;

  i = 0;
  // Obtain table pointer, table size, Logical Cpuid and PSC type according to NB type and package type.
  while (EntryOfTables->TblEntryOfRC10OpSpeed[i] != NULL) {
    LogicalCpuid = (EntryOfTables->TblEntryOfRC10OpSpeed[i])->Header.LogicalCpuid;
    PackageType = (EntryOfTables->TblEntryOfRC10OpSpeed[i])->Header.PackageType;
    //
    // Determine if this is the expected NB Type
    //
    if (MemPIsIdSupported (NBPtr, LogicalCpuid, PackageType)) {
      TblPtr = (PSCFG_OPSPD_ENTRY *) ((EntryOfTables->TblEntryOfRC10OpSpeed[i])->TBLPtr);
      TableSize = (EntryOfTables->TblEntryOfRC10OpSpeed[i])->TableSize;
      break;
    }
    i++;
  }

  // Check whether no table entry is found.
  if (EntryOfTables->TblEntryOfRC10OpSpeed[i] == NULL) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\nNo RC10 Op Speed table\n");
    return FALSE;
  }

  CurDDRrate = (UINT32) (1 << (CurrentChannel->DCTPtr->Timings.Speed / 66));

  for (i = 0; i < TableSize; i++) {
    if ((TblPtr->DDRrate & CurDDRrate) != 0) {
      NBPtr->PsPtr->RC10OpSpd = TblPtr->OPSPD;
      break;
    }
    TblPtr++;
  }

  //
  // If there is no entry, check if overriding value existed. If not, return FALSE.
  //
  PsoMaskRC10OpSpeed = (UINT8) MemPProceedTblDrvOverride (NBPtr, NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_TBLDRV_RC10_OPSPEED);
  if ((PsoMaskRC10OpSpeed == 0) && (i == TableSize)) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\nNo RC10 Op Speed entries\n");
    PutEventLog (AGESA_ERROR, MEM_ERROR_RC10_OP_SPEED_NOT_FOUND, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
    SetMemError (AGESA_ERROR, NBPtr->MCTPtr);
    if (!NBPtr->MemPtr->ErrorHandling (NBPtr->MCTPtr, NBPtr->Dct, EXCLUDE_ALL_CHIPSEL, &NBPtr->MemPtr->StdHeader)) {
      ASSERT (FALSE);
    }
    return FALSE;
  }

  return TRUE;
}
