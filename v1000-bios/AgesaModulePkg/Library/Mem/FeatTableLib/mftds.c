/* $NoKeywords:$ */
/**
 * @file
 *
 * mftds.c
 *
 * Northbridge table drive support file for DR
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/TABLE)
 * @e \$Revision: 324073 $ @e \$Date: 2015-08-11 09:45:24 +0800 (Tue, 11 Aug 2015) $
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
#include "Library/AmdBaseLib.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mftds.h"
#include "OptionMemory.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEM_FEATTABLELIB_MFTDS_FILECODE

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */
#define MAX_BYTELANES_PER_CHANNEL   (8 + 1)  ///< Max Bytelanes per channel

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

VOID
SetTableValues (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_TABLE_ALIAS MTPtr
  );

/*-----------------------------------------------------------------------------
 *
 *   This function initializes bit field translation table
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_TABLE_ALIAS structure
 *     @param[in]       time     - Indicate the timing for the register which is written.
 *
 *     @return          None
 * ----------------------------------------------------------------------------
 */
VOID
MemFInitTableDrive (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8     time
  )
{
  MEM_TABLE_ALIAS *MTPtr;
  MEM_TABLE_ALIAS *IdsMTPtr;
  BOOLEAN PlatformOvr;
  BOOLEAN IdsOvr;

  ASSERT (NBPtr != NULL);
  IdsMTPtr = NULL;
  MTPtr = (MEM_TABLE_ALIAS *) NBPtr->RefPtr->TableBasedAlterations;

  IDS_SKIP_HOOK (IDS_GET_DRAM_TABLE, &IdsMTPtr, &(NBPtr->MemPtr->StdHeader)) {
    IDS_OPTION_HOOK (IDS_INIT_DRAM_TABLE, NBPtr, &(NBPtr->MemPtr->StdHeader));
    IDS_OPTION_HOOK (IDS_GET_DRAM_TABLE, &IdsMTPtr, &(NBPtr->MemPtr->StdHeader));
  }

  PlatformOvr = SetTableValuesLoop (NBPtr, MTPtr, time);
  IdsOvr = SetTableValuesLoop (NBPtr, IdsMTPtr, time);
  IDS_HDT_CONSOLE_DEBUG_CODE (
    if (PlatformOvr || IdsOvr) {
      if (PlatformOvr) {
        IDS_HDT_CONSOLE (MEM_FLOW, "\nPlatform overrides applied[%02x]", time);
      } else {
        IDS_HDT_CONSOLE (MEM_FLOW, "\nIDS overrides applied[%02x]", time);
      }
      IDS_HDT_CONSOLE (MEM_FLOW, "\n\n");
    }
  )
  IDS_OPTION_HOOK (IDS_MT_BASE + time, NBPtr, &(NBPtr->MemPtr->StdHeader));
}

/*-----------------------------------------------------------------------------
 *
 *   This function initializes bit field translation table
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *MTPtr   - Pointer to the MEM_TABLE_ALIAS structure
 *     @param[in]       time     - Indicate the timing for the register which is written.
 *
 *     @return          BOOLEAN  - TRUE:  Values were set
 *                                 FALSE: No values were set
 *
 * ----------------------------------------------------------------------------
 */
BOOLEAN
SetTableValuesLoop (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_TABLE_ALIAS *MTPtr,
  IN       UINT8     time
  )
{
  UINT8 i;
  UINT8 CurDct;
  BOOLEAN ValueSet;

  ValueSet = FALSE;
  if (MTPtr != NULL) {
    CurDct = NBPtr->Dct;
    for (i = 0; MTPtr[i].time != MTEnd; i++) {
      if ((MTPtr[i].attr != MTAuto) && (MTPtr[i].time == time)) {
        SetTableValues (NBPtr, MTPtr[i]);
        ValueSet = TRUE;
      }
    }
    NBPtr->SwitchDCT (NBPtr, CurDct);
  }
  return ValueSet;
}

/*-----------------------------------------------------------------------------
 *
 *   Engine for setting Table Value.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       MTPtr    - Pointer to the MEM_TABLE_ALIAS structure
 *
 *     @return      None
 * ----------------------------------------------------------------------------
 */
VOID
SetTableValues (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_TABLE_ALIAS MTPtr
  )
{
  UINT8 AccessType;
  UINT16 ByteLane;
  UINT8 Dct;
  UINT8 i;
  UINT8 j;
  UINT32 TempVal[36];
  UINT32 Temp2Val[72];
  UINT8 *DqsSavePtr;
  UINT8  DqsOffset;
  BOOLEAN SaveDqs;

  AccessType = 0;
  DqsSavePtr = NULL;
  SaveDqs = TRUE;

  ASSERT (MTPtr.time <= MTValidTimePointLimit);
  ASSERT (MTPtr.attr <= MTOr);
  ASSERT (MTPtr.node <= MTNodes);
  ASSERT (MTPtr.dct <= MTDcts);
  ASSERT (MTPtr.dimm <= MTDIMMs);
  ASSERT (MTPtr.data.s.bytelane <= MTBLs);

  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    if ((MTPtr.dct == MTDcts) || (MTPtr.dct == Dct)) {
      NBPtr->SwitchDCT (NBPtr, Dct);
      switch (MTPtr.bfindex) {
      case BFRcvEnDly:
        AccessType = AccessRcvEnDly;
        DqsSavePtr = NULL;
        break;
      case BFWrDatDly:
        AccessType = AccessWrDatDly;
        DqsSavePtr = NBPtr->ChannelPtr->WrDatDlys;
        break;
      case BFRdDqsDly:
        AccessType = AccessRdDqsDly;
        DqsSavePtr = NBPtr->ChannelPtr->RdDqsDlys;
        break;
      case BFWrDqsDly:
        AccessType = AccessWrDqsDly;
        DqsSavePtr = NBPtr->ChannelPtr->WrDqsDlys;
        break;
      case BFRdDqs2dDly:
        AccessType = AccessRdDqs2dDly;
        SaveDqs = FALSE;
        break;
      case BFWrDat2dDly:
        AccessType = AccessWrDat2dDly;
        break;
      case BFPhRecDly:
        AccessType = AccessPhRecDly;
        SaveDqs = FALSE;
        break;
      default:
        AccessType = 0xFF;
        break;
      }
      if (AccessType == 0xFF) {
        if (MTPtr.attr == MTOverride) {
          NBPtr->SetBitField (NBPtr, MTPtr.bfindex, MTPtr.data.s.value);
        }
        if (MTPtr.attr == MTSubtract) {
          NBPtr->SetBitField (NBPtr, MTPtr.bfindex, NBPtr->GetBitField (NBPtr, MTPtr.bfindex) - MTPtr.data.s.value);
        }
        if (MTPtr.attr == MTAdd) {
          NBPtr->SetBitField (NBPtr, MTPtr.bfindex, NBPtr->GetBitField (NBPtr, MTPtr.bfindex) + MTPtr.data.s.value);
        }
        if (MTPtr.attr == MTAnd) {
          NBPtr->SetBitField (NBPtr, MTPtr.bfindex, (NBPtr->GetBitField (NBPtr, MTPtr.bfindex) & MTPtr.data.s.value));
        }
        if (MTPtr.attr == MTOr) {
          NBPtr->SetBitField (NBPtr, MTPtr.bfindex, (NBPtr->GetBitField (NBPtr, MTPtr.bfindex) | MTPtr.data.s.value));
        }
      } else {
        // Store the DQS data first
        for (i = 0; i < NBPtr->CsPerChannel; i = i + NBPtr->CsPerDelay) {
          if (AccessType == AccessRdDqs2dDly || AccessType == AccessWrDat2dDly) {
            for (j = 0; j < MAX_NUMBER_LANES; j++) {
              Temp2Val[i * MAX_NUMBER_LANES + j] = NBPtr->GetTrainDly (NBPtr, AccessType, DIMM_NBBL_ACCESS (i / NBPtr->CsPerDelay, j));
            }
          } else {
            for (j = 0; j < MAX_BYTELANES_PER_CHANNEL; j++) {
              TempVal[i / NBPtr->CsPerDelay * MAX_BYTELANES_PER_CHANNEL + j] = NBPtr->GetTrainDly (NBPtr, AccessType, DIMM_BYTE_ACCESS (i / NBPtr->CsPerDelay, j));
            }
          }
        }
        //
        // Single Value with Bytleane mask option
        //   Indicated by the vtype flag
        //
        if (MTPtr.vtype == VT_MSK_VALUE) {
          // set the value which defined in Memory table.
          for (i = 0; i < NBPtr->CsPerChannel; i = i + NBPtr->CsPerDelay) {
            ByteLane = MTPtr.data.s.bytelane;
            if ((MTPtr.dimm == MTDIMMs) || ((MTPtr.dimm * NBPtr->CsPerDelay) == i)) {
              if (AccessType == AccessRdDqs2dDly) {
                for (j = 0; j < MAX_NUMBER_LANES; j++) {
                  DqsOffset = (i * MAX_NUMBER_LANES + j);
                  if ((ByteLane & (UINT16)1) != 0) {
                    if (MTPtr.attr == MTOverride) {
                      Temp2Val[DqsOffset] = (UINT16)MTPtr.data.s.value;
                    }
                    if (MTPtr.attr == MTSubtract) {
                      Temp2Val[DqsOffset] -= (UINT16)MTPtr.data.s.value;
                    }
                    if (MTPtr.attr == MTAdd) {
                      Temp2Val[DqsOffset] += (UINT16)MTPtr.data.s.value;
                    }
                    NBPtr->SetTrainDly (NBPtr, AccessType, DIMM_NBBL_ACCESS (i / NBPtr->CsPerDelay, j), (UINT16)Temp2Val[DqsOffset]);
                    if (SaveDqs) {
                      if (DqsSavePtr == NULL) {
                        NBPtr->ChannelPtr->RcvEnDlys[DqsOffset] = (UINT16)Temp2Val[DqsOffset];
                      } else {
                        DqsSavePtr[DqsOffset] = (UINT8)Temp2Val[DqsOffset];
                      }
                    }
                  }
                  ByteLane = ByteLane >> (UINT16)1;
                }
              } else if (AccessType == AccessWrDat2dDly) {
                for (j = 0; j < MAX_NUMBER_LANES; j++) {
                  DqsOffset = (i * MAX_NUMBER_LANES + j);
                  if ((ByteLane & (UINT16)1) != 0) {
                    if (MTPtr.attr == MTOverride) {
                      Temp2Val[DqsOffset] = (UINT16)MTPtr.data.s.value;
                    }
                    if (MTPtr.attr == MTSubtract) {
                      Temp2Val[DqsOffset] -= (UINT16)MTPtr.data.s.value;
                    }
                    if (MTPtr.attr == MTAdd) {
                      Temp2Val[DqsOffset] += (UINT16)MTPtr.data.s.value;
                    }
                    NBPtr->SetTrainDly (NBPtr, AccessType, DIMM_NBBL_ACCESS (i / NBPtr->CsPerDelay, j), (UINT16)Temp2Val[DqsOffset]);
                  }
                  ByteLane = ByteLane >> (UINT16)1;
                }
              } else {
                for (j = 0; j < MAX_BYTELANES_PER_CHANNEL; j++) {
                  DqsOffset = (i / NBPtr->CsPerDelay * MAX_BYTELANES_PER_CHANNEL + j);
                  if ((ByteLane & (UINT16)1) != 0) {
                    if (MTPtr.attr == MTOverride) {
                      TempVal[DqsOffset] = (UINT16)MTPtr.data.s.value;
                    }
                    if (MTPtr.attr == MTSubtract) {
                      TempVal[DqsOffset] -= (UINT16)MTPtr.data.s.value;
                    }
                    if (MTPtr.attr == MTAdd) {
                      TempVal[DqsOffset] += (UINT16)MTPtr.data.s.value;
                    }
                    NBPtr->SetTrainDly (NBPtr, AccessType, DIMM_BYTE_ACCESS (i / NBPtr->CsPerDelay, j), (UINT16)TempVal[DqsOffset]);
                    if (SaveDqs) {
                      if (DqsSavePtr == NULL) {
                        NBPtr->ChannelPtr->RcvEnDlys[DqsOffset] = (UINT16)TempVal[DqsOffset];
                      } else {
                        DqsSavePtr[DqsOffset] = (UINT8)TempVal[DqsOffset];
                      }
                    }
                  }
                  ByteLane = ByteLane >> (UINT16)1;
                }
              }
            }
          }
        } else {
          // Multiple values specified in a byte array
          for (i = 0; i < NBPtr->CsPerChannel; i = i + NBPtr->CsPerDelay) {
            if ((MTPtr.dimm == MTDIMMs) || ((MTPtr.dimm * NBPtr->CsPerDelay) == i)) {
              if (AccessType == AccessRdDqs2dDly) {
                for (j = 0; j < MAX_NUMBER_LANES; j++) {
                  DqsOffset = (i * MAX_NUMBER_LANES + j);
                  if (MTPtr.attr == MTOverride) {
                    Temp2Val[DqsOffset] = MTPtr.data.bytelanevalue[j];
                  }
                  if (MTPtr.attr == MTSubtract) {
                    Temp2Val[DqsOffset] -= MTPtr.data.bytelanevalue[j];
                  }
                  if (MTPtr.attr == MTAdd) {
                    Temp2Val[DqsOffset] += MTPtr.data.bytelanevalue[j];
                  }
                  NBPtr->SetTrainDly (NBPtr, AccessType, DIMM_NBBL_ACCESS (i / NBPtr->CsPerDelay, j), (UINT16)Temp2Val[DqsOffset]);
                  if (SaveDqs) {
                    if (DqsSavePtr == NULL) {
                      NBPtr->ChannelPtr->RcvEnDlys[DqsOffset] = (UINT16)Temp2Val[DqsOffset];
                    } else {
                      DqsSavePtr[DqsOffset] = (UINT8)Temp2Val[DqsOffset];
                    }
                  }
                }
              }  else if (AccessType == AccessWrDat2dDly) {
                for (j = 0; j < MAX_NUMBER_LANES; j++) {
                  DqsOffset = (i * MAX_NUMBER_LANES + j);
                  if (MTPtr.attr == MTOverride) {
                    Temp2Val[DqsOffset] = MTPtr.data.bytelanevalue[j];
                  }
                  if (MTPtr.attr == MTSubtract) {
                    Temp2Val[DqsOffset] -= MTPtr.data.bytelanevalue[j];
                  }
                  if (MTPtr.attr == MTAdd) {
                    Temp2Val[DqsOffset] += MTPtr.data.bytelanevalue[j];
                  }
                  NBPtr->SetTrainDly (NBPtr, AccessType, DIMM_NBBL_ACCESS (i / NBPtr->CsPerDelay, j), (UINT16)Temp2Val[DqsOffset]);
                }
              } else {
                for (j = 0; j < MAX_BYTELANES_PER_CHANNEL; j++) {
                  DqsOffset = (i / NBPtr->CsPerDelay * MAX_BYTELANES_PER_CHANNEL + j);
                  if (MTPtr.attr == MTOverride) {
                    TempVal[DqsOffset] = MTPtr.data.bytelanevalue[j];
                  }
                  if (MTPtr.attr == MTSubtract) {
                    TempVal[DqsOffset] -= MTPtr.data.bytelanevalue[j];
                  }
                  if (MTPtr.attr == MTAdd) {
                    TempVal[DqsOffset] += MTPtr.data.bytelanevalue[j];
                  }
                  NBPtr->SetTrainDly (NBPtr, AccessType, DIMM_BYTE_ACCESS (i / NBPtr->CsPerDelay, j), (UINT16)TempVal[DqsOffset]);
                  if (SaveDqs) {
                    if (DqsSavePtr == NULL) {
                      NBPtr->ChannelPtr->RcvEnDlys[DqsOffset] = (UINT16)TempVal[DqsOffset];
                    } else {
                      DqsSavePtr[DqsOffset] = (UINT8)TempVal[DqsOffset];
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}







