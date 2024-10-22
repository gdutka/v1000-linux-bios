/* $NoKeywords:$ */
/**
 * @file
 *
 * mpdatacfgD4.c
 *
 * Sub-engine to extract DDR4 data bus settings from recommended table
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

#define FILECODE LIBRARY_MEM_PSLIB_MPDATACFGD4_FILECODE

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
 *     Look up data Bus config tables and return the pointer to the matched entry.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *EntryOfTables     - Pointer to MEM_PSC_TABLE_BLOCK
 *
 *     @return          TRUE - Table values can be extracted per dimm population and ranks type.
 *     @return          FALSE - Table values cannot be extracted per dimm population and ranks type.
 *
 */
BOOLEAN
MemPLookupDataBusCfgTabs4 (
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
  UINT8   Chipsel;
  PSCFG_DATABUS_ENTRY_D4 *TblPtr;
  CH_DEF_STRUCT *CurrentChannel;

  CurrentChannel = NBPtr->ChannelPtr;

  if (CurrentChannel->TechType != DDR4_TECHNOLOGY) {
    return FALSE;
  }

  TblPtr = (PSCFG_DATABUS_ENTRY_D4 *) MemPGetTableEntry (NBPtr, EntryOfTables->TblEntryOfDramTerm, &TableSize);
  IDS_HDT_CONSOLE(MEM_FLOW,"\t\tLooking up data bus config entries\n");

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
              for (Chipsel = 0; Chipsel < MAX_CS_PER_CHANNEL; Chipsel++) {
                NBPtr->PsPtr->RttNom[Chipsel] = (UINT8) TblPtr->RttNom;
                NBPtr->PsPtr->RttWr[Chipsel] = (UINT8) TblPtr->RttWr;
                NBPtr->PsPtr->RttPark[Chipsel] = (UINT8) TblPtr->RttPark;

              }
              NBPtr->PsPtr->DqStrength  = (UINT8) TblPtr->DqStrength;
              NBPtr->PsPtr->DqsStrength = (UINT8) TblPtr->DqsStrength;
              NBPtr->PsPtr->OdtStrength = (UINT8) TblPtr->OdtStrength;
              NBPtr->PsPtr->RxDlyOp     = TblPtr->RxDlyOp;
              NBPtr->PsPtr->RxDlyOffset = TblPtr->RxDlyOffset;
              NBPtr->PsPtr->TxDlyOp     = TblPtr->TxDlyOp;
              NBPtr->PsPtr->TxDlyOffset = TblPtr->TxDlyOffset;
              NBPtr->PsPtr->VrefOp      = TblPtr->VrefOp;
              NBPtr->PsPtr->VrefOffset  = TblPtr->VrefOffset;
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
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tNo data bus config entries\n");
  } else {
    IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t\tData bus config entry found:\n");
    IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t\t\tRttnom: %x Rttwr: %x RttPark: %x DQStr: %x DQSStr %x ODTStr: %x\n\n",
                     TblPtr->RttNom, TblPtr->RttWr, TblPtr->RttPark,
                     NBPtr->PsPtr->DqStrength, NBPtr->PsPtr->DqsStrength, NBPtr->PsPtr->OdtStrength);
    return TRUE;
  }

  if (NBPtr->SharedPtr->VoltageMap != VDDIO_DETERMINED) {
    return TRUE;
  }
  PutEventLog (AGESA_ERROR, MEM_ERROR_DATA_BUS_CFG_NOT_FOUND, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
  SetMemError (AGESA_ERROR, NBPtr->MCTPtr);
  if (!NBPtr->MemPtr->ErrorHandling (NBPtr->MCTPtr, NBPtr->Dct, EXCLUDE_ALL_CHIPSEL, &NBPtr->MemPtr->StdHeader)) {
    ASSERT (FALSE);
  }
  return FALSE;
}


