/* $NoKeywords:$ */
/**
 * @file
 *
 * mttecc3.c
 *
 * Technology ECC byte support for registered DDR3
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech/DDR3)
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
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE LIBRARY_MEM_TECHDDR3LIB_MTTECC3_FILECODE
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

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sets the DQS ECC timings for registered DDR3
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemTSetDQSEccTmgsRDdr3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8 Dct;
  UINT8 Dimm;
  UINT8 i;
  UINT8 *WrDqsDly;
  UINT16 *RcvEnDly;
  UINT8 *RdDqsDly;
  UINT8 *WrDatDly;
  UINT8 EccByte;
  INT16 TempValue;

  MEM_NB_BLOCK *NBPtr;
  CH_DEF_STRUCT *ChannelPtr;

  EccByte = TechPtr->MaxByteLanes ();
  NBPtr = TechPtr->NBPtr;

  if (NBPtr->MCTPtr->NodeMemSize) {
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      NBPtr->SwitchDCT (NBPtr, Dct);
      if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
        ChannelPtr = NBPtr->ChannelPtr;
        for (Dimm = 0; Dimm < MAX_DIMMS_PER_CHANNEL; Dimm++) {
          if (NBPtr->DCTPtr->Timings.CsEnabled & ((UINT16)3 << (Dimm * 2))) {
            i = Dimm * TechPtr->DlyTableWidth ();
            WrDqsDly = &ChannelPtr->WrDqsDlys[i];
            RcvEnDly = &ChannelPtr->RcvEnDlys[i];
            RdDqsDly = &ChannelPtr->RdDqsDlys[i];
            WrDatDly = &ChannelPtr->WrDatDlys[i];
            // Receiver DQS Enable:
            // Receiver DQS enable for ECC bytelane = Receiver DQS enable for bytelane 3 -
            //                                        [write DQS for bytelane 3 - write DQS for ECC]

            TempValue = (INT16) RcvEnDly[3] - (INT16) (WrDqsDly[3] - WrDqsDly[EccByte]);
            if (TempValue < 0) {
              TempValue = 0;
            }
            RcvEnDly[EccByte] = (UINT16) TempValue;

            // Read DQS:
            // Read DQS for ECC bytelane = read DQS of byte lane 3
            //
            RdDqsDly[EccByte] = RdDqsDly[3];

            // Write Data:
            // Write Data for ECC bytelane = Write DQS for ECC +
            //                               [write data for bytelane 3 - Write DQS for bytelane 3]
            TempValue = (INT16) (WrDqsDly[EccByte] + (INT8) (WrDatDly[3] - WrDqsDly[3]));
            if (TempValue < 0) {
              TempValue = 0;
            }
            WrDatDly[EccByte] = (UINT8) TempValue;

            NBPtr->SetTrainDly (NBPtr, AccessRcvEnDly, DIMM_BYTE_ACCESS (Dimm, EccByte), RcvEnDly[EccByte]);
            NBPtr->SetTrainDly (NBPtr, AccessRdDqsDly, DIMM_BYTE_ACCESS (Dimm, EccByte), RdDqsDly[EccByte]);
            NBPtr->SetTrainDly (NBPtr, AccessWrDatDly, DIMM_BYTE_ACCESS (Dimm, EccByte), WrDatDly[EccByte]);
          }
        }
      }
    }
  }
  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}
