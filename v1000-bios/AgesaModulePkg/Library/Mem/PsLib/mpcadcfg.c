/* $NoKeywords:$ */
/**
 * @file
 *
 * mpcadcfg.c
 *
 * Sub-engine to extract CAD bus settings from recommended table
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps)
 * @e \$Revision: 322133 $ @e \$Date: 2015-07-07 20:04:05 +0800 (Tue, 07 Jul 2015) $
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

#define FILECODE LIBRARY_MEM_PSLIB_MPCADCFG_FILECODE


/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define _DONT_CARE 0xFF

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
 *     Look up CAD Bus config tables and return the pointer to the matched entry.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *EntryOfTables     - Pointer to MEM_PSC_TABLE_BLOCK
 *
 *     @return          TRUE - Table values can be extracted per dimm population and ranks type.
 *     @return          FALSE - Table values cannot be extracted per dimm population and ranks type.
 *
 */
BOOLEAN
MemPLookupCadBusCfgTabs (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_PSC_TABLE_BLOCK *EntryOfTables
  )
{
  UINT8   i;
  UINT8   TableSize;
  UINT32  CurDDRrate;
  UINT8   DDRVoltage;
  UINT16  RankTypeInTable;
  UINT8   PsoMaskSAO;
  PSCFG_CADBUS_ENTRY *TblPtr;
  CH_DEF_STRUCT *CurrentChannel;

  CurrentChannel = NBPtr->ChannelPtr;

  TblPtr = (PSCFG_CADBUS_ENTRY *) MemPGetTableEntry (NBPtr, EntryOfTables->TblEntryOfSAO, &TableSize);

  IDS_HDT_CONSOLE(MEM_FLOW,"\t\t\tLooking up CAD bus entries\n");
  if (TblPtr != NULL) {
    CurDDRrate = (UINT32) (1 << (CurrentChannel->DCTPtr->Timings.Speed / 66));
    DDRVoltage = (UINT8) (1 << CONVERT_VDDIO_TO_ENCODED (NBPtr->RefPtr->DDRVoltage, CurrentChannel->TechType));
    IDS_HDT_CONSOLE(MEM_FLOW,"\t\t\t            NumDimmSlots   DDR Rate     VDDIO      Rank Type\n");
    IDS_HDT_CONSOLE(MEM_FLOW,"\t\t\t Current ->      %d         %08x       %02x         %x\n",
      NBPtr->PsPtr->NumOfDimmSlots, CurDDRrate, DDRVoltage, NBPtr->PsPtr->RankType);
    for (i = 0; i < TableSize; i++) {
      RankTypeInTable = ((UINT16) TblPtr->Dimm0) | ((UINT16) TblPtr->Dimm1 << 4) | (NP << 8) | (NP << 12);
      IDS_HDT_CONSOLE(MEM_FLOW,"\t\t\t    %d            %d         %08x       %02x         %04x\n",
        i, TblPtr->DimmPerCh, TblPtr->DDRrate, TblPtr->VDDIO, RankTypeInTable);
      if ((TblPtr->DimmPerCh & NBPtr->PsPtr->NumOfDimmSlots) != 0) {
        if ((TblPtr->DDRrate & CurDDRrate) != 0) {
          if ((TblPtr->VDDIO & DDRVoltage) != 0) {
            if ((RankTypeInTable & NBPtr->PsPtr->RankType) == NBPtr->PsPtr->RankType) {
              if (NBPtr->MemPstate == MEMORY_PSTATE1) {
                CurrentChannel->DctAddrTmgPs1 = TblPtr->AddrCmdCtl;
                IDS_OPTION_HOOK (IDS_MEM_ADDR_CMD_TMG, &(CurrentChannel->DctAddrTmgPs1), &(NBPtr->MemPtr->StdHeader));
                CurrentChannel->SlowModePs1 = (TblPtr->SlowMode == 1) ? TRUE : FALSE;
              } else {
                CurrentChannel->DctAddrTmg = TblPtr->AddrCmdCtl;
                IDS_OPTION_HOOK (IDS_MEM_ADDR_CMD_TMG, &(CurrentChannel->DctAddrTmg), &(NBPtr->MemPtr->StdHeader));
                CurrentChannel->SlowMode = (TblPtr->SlowMode == 1) ? TRUE : FALSE;
              }
              NBPtr->PsPtr->CkeStrength     = TblPtr->CkeStrength;
              NBPtr->PsPtr->CsOdtStrength   = TblPtr->CsOdtStrength;
              NBPtr->PsPtr->AddrCmdStrength = TblPtr->AddrCmdStrength;
              NBPtr->PsPtr->ClkStrength     = TblPtr->ClkStrength;
              break;
            }
          }
        }
      }
      TblPtr++;
    }
  } else {
    i = TableSize = 0;
  }

  PsoMaskSAO = (UINT8) MemPProceedTblDrvOverride (NBPtr, NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_TBLDRV_SLOWACCMODE);
  PsoMaskSAO &= (UINT8) MemPProceedTblDrvOverride (NBPtr, NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_TBLDRV_ADDRTMG);
  if ((PsoMaskSAO == 0) && (i == TableSize)) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tNo CAD bus config entries\n");
  } else {
    IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t\tCAD bus config entry found:\n");
    IDS_HDT_CONSOLE(MEM_FLOW, "\n\t\t\t\tSlowAccMode: %x  AddrCmdTmg: %x  CKEDrvStr: %x  CSODTDrvStr: %x  CLKDrvStr: %x\n\n",
                  TblPtr->SlowMode, TblPtr->AddrCmdCtl, NBPtr->PsPtr->CkeStrength,
                  NBPtr->PsPtr->CsOdtStrength, NBPtr->PsPtr->AddrCmdStrength, NBPtr->PsPtr->ClkStrength);
    return TRUE;
  }

  if (NBPtr->SharedPtr->VoltageMap != VDDIO_DETERMINED) {
    return TRUE;
  }
  PutEventLog (AGESA_ERROR, MEM_ERROR_CAD_BUS_TMG_NOT_FOUND, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
  SetMemError (AGESA_ERROR, NBPtr->MCTPtr);
  if (!NBPtr->MemPtr->ErrorHandling (NBPtr->MCTPtr, NBPtr->Dct, EXCLUDE_ALL_CHIPSEL, &NBPtr->MemPtr->StdHeader)) {
    ASSERT (FALSE);
  }

  return FALSE;
}

