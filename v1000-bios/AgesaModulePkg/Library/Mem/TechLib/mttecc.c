/* $NoKeywords:$ */
/**
 * @file
 *
 * mttecc.c
 *
 * Technology ECC byte support
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech)
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
#include "Library/IdsLib.h"
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE LIBRARY_MEM_TECHLIB_MTTECC_FILECODE
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

VOID
STATIC
MemTCalcDQSEccTmg (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dimm,
  IN       UINT8 Type,
  IN OUT   VOID  *DlyArray
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sets the DQS ECC timings
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemTSetDQSEccTmgs (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8 Dct;
  UINT8 Dimm;
  UINT8 i;

  MEM_NB_BLOCK *NBPtr;
  CH_DEF_STRUCT *ChannelPtr;

  NBPtr = TechPtr->NBPtr;
  if (NBPtr->MCTPtr->NodeMemSize) {
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      NBPtr->SwitchDCT (NBPtr, Dct);
      if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
        ChannelPtr = NBPtr->ChannelPtr;
        for (Dimm = 0; Dimm < MAX_DIMMS_PER_CHANNEL; Dimm++) {
          if (NBPtr->DCTPtr->Timings.CsEnabled & ((UINT16)1 << (Dimm * 2))) {
            i = Dimm * TechPtr->DlyTableWidth ();
            MemTCalcDQSEccTmg (TechPtr, Dimm, AccessRcvEnDly, &ChannelPtr->RcvEnDlys[i]);
            MemTCalcDQSEccTmg (TechPtr, Dimm, AccessRdDqsDly, &ChannelPtr->RdDqsDlys[i]);
            MemTCalcDQSEccTmg (TechPtr, Dimm, AccessWrDatDly, &ChannelPtr->WrDatDlys[i]);
          }
        }
      }
    }
  }
  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function calculates the DQS ECC timings
 *
 *     @param[in,out]   *TechPtr - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       Dimm - Dimm number
 *     @param[in]       Type - Type of DQS timing
 *     @param[in,out]   *DlyArray - Pointer to the array of delays per this Dimm
 *
 */

VOID
STATIC
MemTCalcDQSEccTmg (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dimm,
  IN       UINT8 Type,
  IN OUT   VOID  *DlyArray
  )
{
  UINT8 i;
  UINT8 j;
  UINT8 Scale;
  UINT8 EccByte;
  UINT16 ByteiDly;
  UINT16 BytejDly;
  UINT16 EccDly;
  UINT8 *WrDqsDly;
  MEM_NB_BLOCK *NBPtr;
  CH_DEF_STRUCT *ChannelPtr;

  NBPtr = TechPtr->NBPtr;
  ChannelPtr = NBPtr->ChannelPtr;

  EccByte = TechPtr->MaxByteLanes ();
  i = (UINT8) (ChannelPtr->DctEccDqsLike & 0xFF);
  j = (UINT8) (ChannelPtr->DctEccDqsLike >> 8);
  Scale = ChannelPtr->DctEccDqsScale;
  WrDqsDly = &ChannelPtr->WrDqsDlys[Dimm * TechPtr->DlyTableWidth ()];

  if (Type == AccessRcvEnDly) {
    ByteiDly = ((UINT16 *) DlyArray)[i];
    BytejDly = ((UINT16 *) DlyArray)[j];
  } else {
    ByteiDly = ((UINT8 *) DlyArray)[i];
    BytejDly = ((UINT8 *) DlyArray)[j];
  }

  //
  // For WrDatDly, Subtract TxDqs Delay to get
  //  TxDq-TxDqs Delta, which is what should be averaged.
  //
  if (Type == AccessWrDatDly) {
    ByteiDly = ByteiDly - WrDqsDly[i];
    BytejDly = BytejDly - WrDqsDly[j];
  }

  if (BytejDly > ByteiDly) {
    EccDly = ByteiDly + (UINT8) (((UINT16) (BytejDly - ByteiDly) * Scale + 0x77) / 0xFF);
    //                                                            Round up --^
  } else {
    EccDly = BytejDly + (UINT8) (((UINT16) (ByteiDly - BytejDly) * (0xFF - Scale) + 0x77) / 0xFF);
    //                                                                     Round up --^
  }

  if (Type == AccessRcvEnDly) {
    ((UINT16 *) DlyArray)[EccByte] = EccDly;
  } else {
    ((UINT8 *) DlyArray)[EccByte] = (UINT8) EccDly;
  }

  //
  // For WrDatDly, Add back the TxDqs value for ECC bytelane
  //
  if (Type == AccessWrDatDly) {
    EccDly = EccDly + WrDqsDly[EccByte];
  }

  NBPtr->SetTrainDly (NBPtr, Type, DIMM_BYTE_ACCESS (Dimm, EccByte), EccDly);
}

