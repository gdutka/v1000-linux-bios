/**
 * @file
 *
 * mtlrdimm3.c
 *
 * Technology initialization and control workd support for DDR3 LRDIMMS
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech/DDR3)
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
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "mt.h"
#include "mt3.h"
#include "mtspd3.h"
#include "mtrci3.h"
#include "mtsdi3.h"
#include "mtlrdimm3.h"
#include "merrhdl.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)
#define FILECODE LIBRARY_MEM_TECHDDR3LIB_MTLRDIMM3_FILECODE
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
MemTSendMBCtlWord3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Fn,
  IN       UINT8 Rcw,
  IN       UINT8 Value
  );

VOID
STATIC
MemTSendExtMBCtlWord3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT16 Addr,
  IN       UINT16 Data,
  IN       UINT8 Len
  );

UINT8
STATIC
MemTGetSpecialMBCtlWord3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dimm,
  IN       UINT8 Fn,
  IN       UINT8 Rc
  );

BOOLEAN
STATIC
MemTLrDimmControlRegInit3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
STATIC
MemTLrDimmFreqChgCtrlWrd3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
STATIC
MemTWLPrepareLrdimm3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *Wl
  );

BOOLEAN
STATIC
MemTSendAllMRCmdsLR3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *CsPtr
  );

VOID
STATIC
MemTEMRS1Lr3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 ChipSel,
  IN       UINT8 PhyRank
  );

VOID
STATIC
MemTEMRS2Lr3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 ChipSel
  );


BOOLEAN
STATIC
MemTLrdimmRankMultiplication (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *DimmID
  );

BOOLEAN
STATIC
MemTLrdimmBuf2DramTrain3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
STATIC
MemTLrdimmSyncTrainedDlys (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
STATIC
MemTLrdimmPresence (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *DimmID
  );

UINT32
STATIC
MemTLrDimmGetBufferID (
  IN OUT  MEM_TECH_BLOCK *TechPtr,
  IN      UINT8 Dimm
  );

VOID
STATIC
MemTLrdimmInitHook (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       LRDIMM_HOOK_ENTRYPOINT Entrypoint,
  IN       UINT8 Dimm,
  IN OUT   VOID  *OptParam
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function initializes LRDIMM functions.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

BOOLEAN
MemTLrdimmConstructor3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  TechPtr->TechnologySpecificHook[LrdimmSendAllMRCmds]      = MemTSendAllMRCmdsLR3;
  TechPtr->TechnologySpecificHook[LrdimmControlRegInit]     = MemTLrDimmControlRegInit3;
  TechPtr->TechnologySpecificHook[LrdimmFreqChgCtrlWrd]     = MemTLrDimmFreqChgCtrlWrd3;
  TechPtr->TechnologySpecificHook[WlTrainingPrepareLrdimm]  = MemTWLPrepareLrdimm3;
  TechPtr->TechnologySpecificHook[LrdimmRankMultiplication] = MemTLrdimmRankMultiplication;
  TechPtr->TechnologySpecificHook[LrdimmBuf2DramTrain]      = MemTLrdimmBuf2DramTrain3;
  TechPtr->TechnologySpecificHook[LrdimmSyncTrainedDlys]    = MemTLrdimmSyncTrainedDlys;
  TechPtr->TechnologySpecificHook[LrdimmPresence]           = MemTLrdimmPresence;
  return TRUE;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sends a Control word command to an LRDIMM Memory Buffer
 *
 *     @param[in,out] *TechPtr - Pointer to the MEM_TECH_BLOCK
 *     @param[in]     Fn       - control word function
 *     @param[in]     Rcw      - control word number
 *     @param[in]     Value    - value to send
 *
 */

VOID
STATIC
MemTSendMBCtlWord3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Fn,
  IN       UINT8 Rcw,
  IN       UINT8 Value
  )
{
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;

  ASSERT (Rcw != RCW_FN_SELECT);  // RC7 can only be used for function select
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tF%dRC%d = %x\n", Fn, Rcw, Value);
  //
  // Select the MB Function by sending the Fn number
  //  to the Function Select Control Word
  //
  MemUWait10ns (800, NBPtr->MemPtr);
  MemTSendCtlWord3 (TechPtr, RCW_FN_SELECT, Fn);
  //
  // Send the value to the control word
  //
  MemUWait10ns (800, NBPtr->MemPtr);
  MemTSendCtlWord3 (TechPtr, Rcw, Value);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sends a an Extended Control word command to an LRDIMM Memory Buffer
 *
 *     @param[in,out] *TechPtr - Pointer to the MEM_TECH_BLOCK
 *     @param[in]     Addr     - Extended Control Word Address
 *                                 Addr[15:12] Extended Control Workd Function Select
 *                                 Addr[11:0] Extended Control Word CSR Address
 *     @param[in]     Data     - value to send
 *     @param[in]     Len      - Length of data.  1 or 2 bytes
 *
 */

VOID
STATIC
MemTSendExtMBCtlWord3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT16 Addr,
  IN       UINT16 Data,
  IN       UINT8 Len
  )
{
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;

  ASSERT ((Len == 1) || (Len == 2));
  if (Len == 2 ) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\tExtRC_x%04x = %04x\n", Addr, Data);
  } else {
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\tExtRC_x%04x = %02x\n", Addr, (UINT8) (Data & 0xFF) );
  }
  //
  // Select the MB Function by sending the Fn number
  //  to the Function Select Control Word
  //
  MemUWait10ns (800, NBPtr->MemPtr);
  MemTSendCtlWord3 (TechPtr, RCW_FN_SELECT, 13);
  //
  // Send address via control words
  //
  MemUWait10ns (800, NBPtr->MemPtr);
  MemTSendCtlWord3 (TechPtr, 9, (UINT8) (Addr >> 12));
  MemUWait10ns (800, NBPtr->MemPtr);
  MemTSendCtlWord3 (TechPtr, 10, (UINT8) (Addr & 0xF));
  MemUWait10ns (800, NBPtr->MemPtr);
  MemTSendCtlWord3 (TechPtr, 11, (UINT8) ((Addr >> 4) & 0x0F));
  MemUWait10ns (800, NBPtr->MemPtr);
  MemTSendCtlWord3 (TechPtr, 12, (UINT8) ((Addr >> 8) & 0x0F));
  //
  // Send the Lower Byte of Data
  //
  MemUWait10ns (800, NBPtr->MemPtr);
  MemTSendCtlWord3 (TechPtr, 14, (UINT8) (Data & 0xF));
  MemUWait10ns (800, NBPtr->MemPtr);
  MemTSendCtlWord3 (TechPtr, 15, (UINT8) ((Data >> 4) & 0x0F));
  //
  // Send the Upper Byte of Data
  //
  if (Len == 2) {
    MemUWait10ns (800, NBPtr->MemPtr);
    MemTSendCtlWord3 (TechPtr, 10, (UINT8) ((Addr & 0xF) + 1));
    MemUWait10ns (800, NBPtr->MemPtr);
    MemTSendCtlWord3 (TechPtr, 14, (UINT8) ((Data >> 8) & 0xF));
    MemUWait10ns (800, NBPtr->MemPtr);
    MemTSendCtlWord3 (TechPtr, 15, (UINT8) ((Data >> 12) & 0xF));
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function gets the value of special RCW
 *
 *     @param[in,out] *TechPtr - Pointer to the MEM_TECH_BLOCK
 *     @param[in]     Dimm     - Physical LR DIMM number
 *     @param[in]     Fn       - control word function
 *     @param[in]     Rc       - control word number
 *
 *     @return      Special RCW value
 *
 */

UINT8
STATIC
MemTGetSpecialMBCtlWord3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dimm,
  IN       UINT8 Fn,
  IN       UINT8 Rc
  )
{
  CONST UINT8   F0RC13PhyRankTab[] = {3, 2, 0, 1, 0};
  UINT8         PhyRanks;
  UINT8         LogRanks;
  UINT8         DramCap;
  UINT8         Value8;
  UINT8         *SpdBufferPtr;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;

  MemTGetDimmSpdBuffer3 (TechPtr, &SpdBufferPtr, Dimm);

  Value8 = 0;
  switch (Fn) {
  case 0:
    switch (Rc) {
    case 8:
      // F0RC8
      Value8 = NBPtr->PsPtr->F0RC8;
      break;
    case 10:
      // F0RC10
      // 2:0 OperatingSpeed: operating speed. BIOS: Table 88.
      if (NBPtr->DCTPtr->Timings.Speed == DDR667_FREQUENCY) {
        Value8 = 0;
      } else {
        Value8 = (UINT8) (NBPtr->DCTPtr->Timings.Speed / 133) - 3;
      }
      break;
    case 11:
      // F0RC11
      // 3:2 ParityCalculation: partiy calculation. BIOS: Table.
      // 1:0 OperatingVoltage: operating voltage. BIOS: IF(VDDIO == 1.5) THEN 00b ELSEIF (VDDIO ==
      //     1.35) THEN 01b ELSE 10b ENDIF.
      DramCap = SpdBufferPtr[SPD_DENSITY] & 0xF;
      if (NBPtr->PsPtr->LrdimmRowAddrBits[Dimm] > 16) {
        Value8 = 8;
      } else {
        Value8 = 4;
      }
      Value8 |= CONVERT_VDDIO_TO_ENCODED (NBPtr->RefPtr->DDRVoltage, NBPtr->ChannelPtr->TechType);
      break;
    case 13:
      // F0RC13
      // 3:2 NumLogicalRanks: partiy calculation. BIOS: Table 90.
      // 1:0 NumPhysicalRanks: operating voltage. BIOS: Table 89.
      LogRanks = NBPtr->ChannelPtr->LrDimmLogicalRanks[Dimm] >> 1;
      PhyRanks = F0RC13PhyRankTab[(SpdBufferPtr[SPD_RANKS] >> 3) & 7];
      Value8 = (LogRanks << 2) | PhyRanks;
      break;
    case 14:
      // F0RC14
      // 3 DramBusWidth: DRAM bus width. BIOS: IF (DeviceWidth==0) THEN 0 ELSE 1 ENDIF.
      // 2 MRSCommandControl: MRS command control. BIOS: IF (F0RC15[RankMultiplicationControl]
      //   > 0) THEN 1 ELSE 0 ENDIF.
      // 1 RefreshPrechargeCommandControl: refresh and precharge command control. BIOS: IF
      //   (F0RC15[RankMultiplicationControl] > 0) THEN D18F2xA8_dct[1:0][LrDimmEnhRefEn] ELSE 0 ENDIF.
      // 0 AddressMirror: address mirror. BIOS: RankMap. See D18F2x[5C:40]_dct[1:0][OnDimmMirror].
      if ((SpdBufferPtr[SPD_DEV_WIDTH] & 7) != 0) {
        Value8 |= 8;
      }
      if (NBPtr->ChannelPtr->LrDimmRankMult[Dimm] > 1) {
        Value8 |= 4;
        if (NBPtr->GetBitField (NBPtr, BFLrDimmEnhRefEn) == 1) {
          Value8 |= 2;
        }
      }
      if ((SpdBufferPtr[SPD_ADDRMAP] & 1) != 0) {
        Value8 |= 1;
      }
      break;
    case 15:
      // F0RC15
      // 3:0 RankMultiplicationControl: rank multiplication control. BIOS: Table 91.
      DramCap = SpdBufferPtr[SPD_DENSITY] & 0xF;
      ASSERT ((DramCap >= 2) && (DramCap <= 4));            // BKDG only lists 1Gb, 2Gb, and 4Gb
      switch (NBPtr->ChannelPtr->LrDimmRankMult[Dimm]) {
      case 1:
        Value8 = 0;
        break;
      case 2:
        Value8 = DramCap - 1;
        break;
      case 4:
        Value8 = DramCap + 3;
        break;
      default:
        ASSERT (FALSE);
      }
      break;
    default:
      ASSERT (FALSE);
    }
    break;
  case 1:
    switch (Rc) {
    case 0:
      // F1RC0
      Value8 = NBPtr->PsPtr->F1RC0;
      Value8 |= (UINT8) NBPtr->GetBitField (NBPtr, BFCSTimingMux67) << 3;
      break;
    case 1:
      // F1RC1
      Value8 = NBPtr->PsPtr->F1RC1;
      break;
    case 2:
      // F1RC2
      Value8 = NBPtr->PsPtr->F1RC2;
      break;
    case 9:
      // F1RC9
      if (NBPtr->GetBitField (NBPtr, BFLrDimmEnhRefEn) == 0) {
        Value8 = 1;
      }
      break;
    default:
      ASSERT (FALSE);
    }
    break;
  case 3:
    switch (Rc) {
    case 0:
      // F3RC0
      // 3   TDQSControl: TDQS control. BIOS: 0.
      // 2:0 RttNom: RttNom. BIOS: Table 57, Table 60
      Value8 = NBPtr->PsPtr->RttNom[Dimm << 1];
      break;
    case 1:
      // F3RC1
      // 3   Vref: Vref. BIOS: 0.
      // 2:0 RttWr: RttWr. BIOS: Table 57, Table 60.
      Value8 = NBPtr->PsPtr->RttWr[Dimm << 1];
      break;
    case 6:
      // F3RC6
      // IF (D18F2x90_dct[1:0][X4Dimm] == 0) THEN 1 ELSE 0
      if (NBPtr->GetBitField (NBPtr, BFX4Dimm) == 0) {
        Value8 = 8;
      }
      break;
    default:
      ASSERT (FALSE);
    }
    break;
  default:
    ASSERT (FALSE);
  }

  return Value8;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sends LRDIMM Control Words to all LRDIMMS
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out]   OptParam   - Optional parameter
 *
 *     @return    TRUE
 */

BOOLEAN
STATIC
MemTLrDimmControlRegInit3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  )
{
  CONST UINT8 RCWInitTable[] = {
    //  RCW,  Mask, SPD
    F0, RC0,  0x00, SPD_NONE,
    F0, RC1,  0x00, SPD_NONE,
    F0, RC2,  0x03, SPD_67,
    F0, RC10, 0x00, SPECIAL_CASE,
    F0, RC11, 0x00, SPECIAL_CASE,

    F1, RC8,  0x0F, SPD_69,
    F1, RC11, 0xF0, SPD_69,
    F1, RC12, 0x0F, SPD_70,
    F1, RC13, 0xF0, SPD_70,
    F1, RC14, 0x0F, SPD_71,
    F1, RC15, 0xF0, SPD_71,

    WAIT_6US, 0, 0, 0,

    F0, RC3,  0xF0, SPD_67,
    F0, RC4,  0x0F, SPD_68,
    F0, RC5,  0xF0, SPD_68,

    F0, RC6,  0x00, SPD_NONE,
    F0, RC8,  0x00, SPECIAL_CASE,
    F0, RC9,  0x0C, SPD_NONE,
    F0, RC13, 0x00, SPECIAL_CASE,
    F0, RC14, 0x00, SPECIAL_CASE,
    F0, RC15, 0x00, SPECIAL_CASE,

    F1, RC0,  0x00, SPECIAL_CASE,
    F1, RC1,  0x00, SPECIAL_CASE,
    F1, RC2,  0x00, SPECIAL_CASE,
    F1, RC3,  0x00, SPD_NONE,
    F1, RC9,  0x00, SPECIAL_CASE,
    F1, RC10, 0x00, SPD_NONE,

    F2, RC0,  0x00, SPD_NONE,
    F2, RC1,  0x00, SPD_NONE,
    F2, RC2,  0x0F, SPD_NONE,
    F2, RC3,  0x00, SPD_NONE,

    F3, RC0,  0x00, SPECIAL_CASE,
    F3, RC1,  0x00, SPECIAL_CASE,
    F3, RC2,  0x01, SPD_NONE,
    F3, RC6,  0x00, SPECIAL_CASE
    //
    // F3 RC[8,9] are programmed in MDQ RC loop
    //
    // F[10:3] RC[11,10] are programmed in QxODT RC loop
    //
    // F[15,14] RC[15:0] are programmed in personality RC loop
  };

  UINT8           Dimm;
  UINT16          i;
  UINT16          DimmMask;
  UINT8           Fn;
  UINT8           Rc;
  UINT8           Mask;
  UINT8           Spd;
  UINT8           *SpdBufferPtr;
  UINT8           FreqDiffOffset;
  UINT8           Value8;
  UINT32           Temp32;
  MEM_DATA_STRUCT *MemPtr;
  MEM_NB_BLOCK    *NBPtr;

  NBPtr = TechPtr->NBPtr;
  MemPtr = NBPtr->MemPtr;

  if (NBPtr->MCTPtr->Status[SbLrdimms]) {
    for (Dimm = 0; Dimm < MAX_DIMMS_PER_CHANNEL; Dimm++) {
      DimmMask = (UINT16)1 << Dimm;
      if ((NBPtr->ChannelPtr->LrDimmPresent & DimmMask) != 0) {
        //
        // Select the Target Chipselects
        //
        NBPtr->SetBitField (NBPtr, BFMrsChipSel, (Dimm << 1));
        NBPtr->SetBitField (NBPtr, BFCtrlWordCS, 3 << (Dimm << 1));

        MemTGetDimmSpdBuffer3 (TechPtr, &SpdBufferPtr, Dimm);

        IDS_HDT_CONSOLE (MEM_FLOW, "\t\tSending LRDIMM Control Words: Dimm %02x\n", Dimm);

        for (i = 0; i < sizeof (RCWInitTable) ; i += 4) {
          Fn   = RCWInitTable[i];
          Rc   = RCWInitTable[i + 1];
          Mask = RCWInitTable[i + 2];
          Spd  = RCWInitTable[i + 3];

          if (Fn == WAIT_6US) {
            MemUWait10ns (600, MemPtr);   // wait 6us for TSTAB
          } else {
            if (Spd == SPD_NONE) {
              Value8 = Mask;
            } else if (Spd == SPECIAL_CASE) {
              Value8 = MemTGetSpecialMBCtlWord3 (TechPtr, Dimm, Fn, Rc);
            } else {
              Value8 = (Mask > 0x0F) ? ((SpdBufferPtr[Spd] & Mask) >> 4) : (SpdBufferPtr[Spd] & Mask);
            }
            MemTSendMBCtlWord3 (TechPtr, Fn, Rc, Value8);
          }
        }

        FreqDiffOffset = (UINT8) (SPD_FREQ_DIFF_OFFSET * (((NBPtr->DCTPtr->Timings.Speed / 133) - 3) / 2));
        //
        // Send RCW to program MDQ termination and drive strength
        //
        for (Rc = 8; Rc <= 9; Rc++) {
          Value8 = SpdBufferPtr[SPD_MDQ_800_1066 + FreqDiffOffset];
          Value8 = (Rc == 9) ? (Value8 >> 4) : Value8;
          MemTSendMBCtlWord3 (TechPtr, 3, Rc, Value8 & 0x07);
        }
        //
        // Send RCW to program QxODT
        //
        for (Fn = 3; Fn <= 10; Fn ++) {
          for (Rc = 10; Rc <= 11; Rc++) {
            Value8 = SpdBufferPtr[SPD_QXODT_800_1066 + FreqDiffOffset + ((Fn - 3) >> 1)];
            Value8 = (Rc == 11) ? (Value8 >> 4) : (Value8 & 0x0F);
            Value8 = ((Fn & 1) == 0) ? (Value8 >> 2) : (Value8 & 0x03);
            MemTSendMBCtlWord3 (TechPtr, Fn, Rc, Value8);
          }
        }

        MemTLrdimmInitHook (TechPtr, AFTER_TSTAB, Dimm, 0);
        //
        // Send Personality bytes from SPD
        //
        for (i = 0; i < 15; i ++) {
          Value8 = SpdBufferPtr[SPD_PERSONALITY_BYTE + i];
          Fn = (UINT8) (14 + (i >> 3));
          Rc = (UINT8) ((i << 1) & 0x0F);
          if ( i == 0) {
            Value8 |= 0x01;
          } else if ( i > 10) {
            Rc += 2;
          }
          MemTSendMBCtlWord3 (TechPtr, Fn, Rc, (Value8 & 0x0F));
          if (i == 3) {
            Fn++;
          } else {
            Rc++;
          }
          MemTSendMBCtlWord3 (TechPtr, Fn, Rc, (Value8 >> 4));
        }
        //
        // Send Extended Control Words to Buffer
        //
        // ExtRC_xAC
        //
        MemTSendExtMBCtlWord3 (TechPtr, 0x00AC, 0, 1);
        //
        // ExtRC_xB8-BF
        //
        Value8 = SpdBufferPtr[SPD_MR1_MR2_800_1066 + FreqDiffOffset];
        for (i = 0x00B8; i < 0x00C0; i++) {
          MemTSendExtMBCtlWord3 (TechPtr, i, Value8, 1);
          if (i == 0xB9) {
            //
            // Phys ranks > 1, Rtt_nom = 0
            //
            Value8 &= 0xE3;
          }
        }
        // ExtRC_xC8
        Value8 = (UINT8) (NBPtr->MemNGetMR0CL (NBPtr) & 0x000000FF);
        Value8 = ((Value8 & 0xE0) | ((Value8 & 0x04) << 1));
        Value8 |= 1 << 2; // RBT
        Value8 |= NBPtr->GetBitField (NBPtr, BFBurstCtrl); // BL
        MemTSendExtMBCtlWord3 (TechPtr, 0x00C8 , Value8, 1);
        // ExtRC_xC9
        // PPD
        Value8 = (UINT8) (NBPtr->GetBitField (NBPtr, BFPchgPDModeSel) & 0x000000FF);
        NBPtr->FamilySpecificHook[MR0_PPD] (NBPtr, &Value8);
        IDS_OPTION_HOOK (IDS_MEM_MR0, &Value8, &TechPtr->NBPtr->MemPtr->StdHeader);
        Value8 <<= 4;
        // WR
        Temp32 = NBPtr->MemNGetMR0WR (NBPtr);
        Value8 |= (UINT8) ((Temp32 >> 8) & 0x000000FF);
        MemTSendExtMBCtlWord3 (TechPtr, 0x00C9 , Value8, 1);
        // ExtRC_xCA
        MemTSendExtMBCtlWord3 (TechPtr, 0x00CA , 0, 1);
        // ExtRC_xCB
        MemTSendExtMBCtlWord3 (TechPtr, 0x00CB , 0, 1);
        // ExtRC_xCC
        // CWL
        Value8 = (UINT8) (NBPtr->MemNGetMR2CWL (NBPtr) & 0x000000FF);
        // SRT|ASR
        Value8 |= 0x40;
        MemTSendExtMBCtlWord3 (TechPtr, 0x00CC , Value8, 1);
        // ExtRC_xCD
        MemTSendExtMBCtlWord3 (TechPtr, 0x00CD , 0, 1);
        // ExtRC_xCE
        MemTSendExtMBCtlWord3 (TechPtr, 0x00CE , 0, 1);
        // ExtRC_xCF
        MemTSendExtMBCtlWord3 (TechPtr, 0x00CF , 0, 1);
      }
    }
  }
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sends LRDIMM Control Words to all LRDIMMS
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out]   OptParam   - Optional parameter
 *
 *     @return    FALSE  - The current channel does not have LRDIMM populated
 *                TRUE   - The current channel has LRDIMM populated
 */
BOOLEAN
STATIC
MemTLrDimmFreqChgCtrlWrd3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  )
{
  UINT8 Dct;
  MEM_NB_BLOCK    *NBPtr;

  NBPtr = TechPtr->NBPtr;

  if (NBPtr->MCTPtr->Status[SbLrdimms]) {
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      MemNSwitchDCTNb (NBPtr, Dct);
      if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
        MemTLrDimmControlRegInit3 (TechPtr, NULL);
      }
    }
    return TRUE;
  }
  return FALSE;
}

/*-----------------------------------------------------------------------------
 *
 *
 *     This function prepares LRDIMMs for WL training.
 *
 *     @param[in,out]  *TechPtr     - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out]  *Wl - Indicates if WL mode should be enabled
 *
 *     @return    TRUE - LRDIMMs present
 * ----------------------------------------------------------------------------
 */
BOOLEAN
STATIC
MemTWLPrepareLrdimm3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *Wl
  )
{
  UINT8 Dimm;
  UINT8 Value8;
  UINT16 MrsAddress;
  MEM_NB_BLOCK  *NBPtr;
  NBPtr = TechPtr->NBPtr;
  MrsAddress = 0;
  if (NBPtr->MCTPtr->Status[SbLrdimms]) {
    for (Dimm = 0; Dimm < MAX_DIMMS_PER_CHANNEL; Dimm++) {
      if (*(BOOLEAN *) Wl == TRUE) {
        // Program WrLvOdt
        NBPtr->SetBitField (NBPtr, BFWrLvOdt, NBPtr->ChannelPtr->PhyWLODT[Dimm]);
      }
      if ((NBPtr->ChannelPtr->LrDimmPresent & ((UINT8) 1 << Dimm)) != 0) {
        if (Dimm == TechPtr->TargetDIMM) {
          if (*(BOOLEAN *) Wl == TRUE) {
            //
            // Select the Target Chipselects
            //
            NBPtr->SetBitField (NBPtr, BFMrsChipSel, (Dimm << 1));
            NBPtr->SetBitField (NBPtr, BFCtrlWordCS, 3 << (Dimm << 1));
            // Program F0RC12 to 1h
            MemTSendMBCtlWord3 (TechPtr, F0, RC12, 0x01);
            if (NBPtr->ChannelPtr->Dimms >= 2) {
              // For two or more LRDIMMs per channel program the buffer RttNom to the
              // corresponding specifed RttWr termination
              Value8 = NBPtr->MemNGetDynDramTerm (NBPtr, Dimm << 2);
            } else {
              // Program RttNom as normal
              Value8 = NBPtr->MemNGetDramTerm (NBPtr, Dimm << 2);
            }
            if ((Value8 & ((UINT8) 1 << 2)) != 0) {
              MrsAddress |= ((UINT16) 1 << 9);
            }
            if ((Value8 & ((UINT8) 1 << 1)) != 0) {
              MrsAddress |= ((UINT16) 1 << 6);
            }
            if ((Value8 & ((UINT8) 1 << 0)) != 0) {
              MrsAddress |= ((UINT16) 1 << 2);
            }
            NBPtr->SetBitField (NBPtr, BFMrsAddress, MrsAddress);
          } else {
            // Program F0RC12 to 0h
            MemTSendMBCtlWord3 (TechPtr, F0, RC12, 0x00);
          }
        }
      }
    }
    return TRUE;
  } else {
    return FALSE;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This send all MR commands to all physical ranks of an LRDIMM
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       *CsPtr     - Target Chip Select
 *
 *     @return    FALSE  - The current channel does not have LRDIMM populated
 *                TRUE   - The current channel has LRDIMM populated
 */
BOOLEAN
STATIC
MemTSendAllMRCmdsLR3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *CsPtr
  )
{
  UINT8        *SpdBufferPtr;
  UINT8         Rank;
  UINT8         PhyRank;
  UINT8         ChipSel;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;
  ChipSel = *((UINT8 *) CsPtr);

  if (NBPtr->MCTPtr->Status[SbLrdimms]) {
    //
    //   For LRDIMMs, MR0, MR2, and MR3 can be broadcast to any physicall ranks behind
    // each logical rank(CS) by setting MRSAddress[13]. MR1[Rtt_Nom] needs to be programmed
    // differently per physical rank, so it must target a physical rank using MrsAddress[17:14].
    // The actual bits used to index the physical rank are determined by the DRAM Capacity.
    //
    //   This function will be called once for each CS where CSPresent is set, so each time
    // it only needs to handle the Physical ranks behind each CS.  If a Dimm is not using some
    // CS due to Rank Mux, those CSPresent bits will have been already cleared.
    //

    //
    // Select target chip select
    //
    NBPtr->SetBitField (NBPtr, BFMrsChipSel, ChipSel);
    //
    // 13.Send EMRS(2)
    //
    MemTEMRS2Lr3 (TechPtr, ChipSel);
    NBPtr->SetBitField (NBPtr, BFMrsAddressHi, 1);    // Set Address bit 13 to broadcast
    NBPtr->SendMrsCmd (NBPtr);
    //
    // 14.Send EMRS(3). Ordinarily at this time, MrsAddress[2:0]=000b
    //
    MemTEMRS33 (TechPtr);
    NBPtr->SetBitField (NBPtr, BFMrsAddressHi, 1);    // Set Address bit 13 to broadcast
    NBPtr->SendMrsCmd (NBPtr);
    //
    // 15.Send EMRS(1). Send to each physical rank.
    //
    MemTGetDimmSpdBuffer3 (TechPtr, &SpdBufferPtr, ChipSel >> 1);
    //
    // Determine first physical rank relative to the LRDIMM for this CS
    //
    PhyRank = ((((ChipSel & NBPtr->ChannelPtr->LrDimmLogicalRanks[ChipSel >> 1]) >> 1) & 2) | (ChipSel & 1));
    for (Rank = 0; Rank < NBPtr->ChannelPtr->LrDimmRankMult[ChipSel >> 1]; Rank++) {
      MemTEMRS1Lr3 (TechPtr, ChipSel, PhyRank);
      //
      // Set Address bit 14, 15, 16, or 17 to select physical rank, relative to the CS, according to the device size
      //
      NBPtr->SetBitField (NBPtr, BFMrsAddressHi, Rank << ((SpdBufferPtr[SPD_DENSITY] & 0xF) - 1 ) );
      NBPtr->SendMrsCmd (NBPtr);
      //
      // Index to the next physical rank
      //
      PhyRank = PhyRank + NBPtr->ChannelPtr->LrDimmLogicalRanks[ChipSel >> 1];
    }
    //
    // 16.Send MRS with MrsAddress[8]=1(reset the DLL)
    //
    MemTMRS3 (TechPtr);
    NBPtr->SetBitField (NBPtr, BFMrsAddressHi, 1);    // Set Address bit 13 to broadcast
    NBPtr->SendMrsCmd (NBPtr);
    //
    // If LRDIMM, return TRUE to skip sending regular MR commands.
    //
    return TRUE;
  }
  //
  // If not LRDIMM, send regular MR commands.
  //
  return FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function calculates the EMRS1 value for an LRDIMM
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]        ChipSel   - Chip select number
 *     @param[in]        PhyRank   - Physical rank number
 */

VOID
STATIC
MemTEMRS1Lr3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 ChipSel,
  IN       UINT8 PhyRank
  )
{
  UINT16       MrsAddress;
  UINT8        Value8;
  UINT8        *SpdBufferPtr;
  UINT8        FreqDiffOffset;
  MEM_NB_BLOCK *NBPtr;

  NBPtr = TechPtr->NBPtr;

  MemTGetDimmSpdBuffer3 (TechPtr, &SpdBufferPtr, ChipSel >> 1);
  FreqDiffOffset = (UINT8) (SPD_FREQ_DIFF_OFFSET * (((NBPtr->DCTPtr->Timings.Speed / 133) - 3) / 2));

  // BA2=0,BA1=0,BA0=1
  NBPtr->SetBitField (NBPtr, BFMrsBank, 1);

  MrsAddress = 0;

  // program MrsAddress[5,1]=output driver impedance control (DIC): 01b
  MrsAddress |= ((UINT16) 1 << 1);

  // program MrsAddress[5,1]=output driver impedance control (DIC):
  // DIC is read from SPD byte 77, 83, or 89 depending on DDR speed
  Value8 = SpdBufferPtr[SPD_MR1_MR2_800_1066 + FreqDiffOffset] & 3;
  if ((Value8 & ((UINT8) 1 << 1)) != 0) {
    MrsAddress |= ((UINT16) 1 << 5);
  }
  if ((Value8 & ((UINT8) 1 << 0)) != 0) {
    MrsAddress |= ((UINT16) 1 << 1);
  }

  // program MrsAddress[9,6,2]=nominal termination resistance of ODT (RTT):
  // RttNom is read from SPD byte 77, 83, or 89 depending on DDR speed
  if (PhyRank <= 1) {
    Value8 = (SpdBufferPtr[SPD_MR1_MR2_800_1066 + FreqDiffOffset] >> 2) & 7;
    if ((Value8 & ((UINT8) 1 << 2)) != 0) {
      MrsAddress |= ((UINT16) 1 << 9);
    }
    if ((Value8 & ((UINT8) 1 << 1)) != 0) {
      MrsAddress |= ((UINT16) 1 << 6);
    }
    if ((Value8 & ((UINT8) 1 << 0)) != 0) {
      MrsAddress |= ((UINT16) 1 << 2);
    }
  }

  NBPtr->SetBitField (NBPtr, BFMrsAddress, MrsAddress);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function calculates the EMRS2 value for an LRDIMM
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]        ChipSel   - Chip select number
 */

VOID
STATIC
MemTEMRS2Lr3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 ChipSel
  )
{
  UINT8        RttWr;
  UINT8        *SpdBufferPtr;
  UINT8        FreqDiffOffset;
  MEM_NB_BLOCK *NBPtr;

  NBPtr = TechPtr->NBPtr;

  // Save default RttWr
  RttWr = NBPtr->PsPtr->RttWr[ChipSel];

  // Override RttWr with the value read from SPD byte 77, 83, or 89 depending on DDR speed
  MemTGetDimmSpdBuffer3 (TechPtr, &SpdBufferPtr, ChipSel >> 1);
  FreqDiffOffset = (UINT8) (SPD_FREQ_DIFF_OFFSET * (((NBPtr->DCTPtr->Timings.Speed / 133) - 3) / 2));
  NBPtr->PsPtr->RttWr[ChipSel] = SpdBufferPtr[SPD_MR1_MR2_800_1066 + FreqDiffOffset] >> 6;

  // Call EMRS2 calculation
  MemTEMRS23 (TechPtr);

  // Restore RttWr
  NBPtr->PsPtr->RttWr[ChipSel] = RttWr;
}

/*-----------------------------------------------------------------------------
 *
 *
 *     This function to determine the Rank Multiplication to use for an LRDIMM
 *
 *     @param[in,out]  *TechPtr     - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out]  *DimmID      - Dimm ID
 *
 *     @return    TRUE - LRDIMM Support is installed and LRDIMMs are present
 * ----------------------------------------------------------------------------
 */
BOOLEAN
STATIC
MemTLrdimmRankMultiplication (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *DimmID
  )
{
  BOOLEAN RetVal;
  UINT8 *SpdBufferPtr;
  UINT8 Dimm;
  UINT8 NumDimmslots;
  UINT8 DramCapacity;
  UINT8 Ranks;
  UINT8 Rows;
  UINT8 RankMult;
  MEM_NB_BLOCK  *NBPtr;
  CH_DEF_STRUCT *ChannelPtr;

  ASSERT (TechPtr != NULL);
  ASSERT (DimmID != NULL);

  Dimm = *(UINT8*)DimmID;
  ASSERT (Dimm < MAX_DIMMS_PER_CHANNEL);

  NBPtr = TechPtr->NBPtr;
  ChannelPtr = NBPtr->ChannelPtr;
  RetVal = FALSE;
  RankMult = 0;

  if (!MemTGetDimmSpdBuffer3 (TechPtr, &SpdBufferPtr, Dimm)) {
    ASSERT (FALSE);
  }

  NumDimmslots = GetMaxDimmsPerChannel (NBPtr->RefPtr->PlatformMemoryConfiguration,
                                        NBPtr->MCTPtr->SocketId,
                                        ChannelPtr->ChannelID);

  if (NBPtr->MCTPtr->Status[SbLrdimms]) {
    RetVal = TRUE;
    //
    // Determine LRDIMM Rank Multiplication
    //
    Ranks = ((SpdBufferPtr[SPD_RANKS] >> 3) & 0x07) + 1;
    if (Ranks == 5) {
      Ranks = 8;
    }
    DramCapacity = (SpdBufferPtr[SPD_DENSITY] & 0x0F);
    Rows = 12 + ((SpdBufferPtr[SPD_ROW_SZ] >> 3) & 0x7);

    if (Ranks < 4) {
      RankMult = 1;
    } else if (Ranks == 4) {
      RankMult = (NumDimmslots < 3) ? 1 : 2;
    } else if (Ranks == 8) {
      RankMult = ((NumDimmslots < 3) && (DramCapacity < 4)) ? 2 : 4;
    }
    //
    // Save Rank Information
    //
    ChannelPtr->LrDimmRankMult[Dimm] = RankMult;
    ChannelPtr->LrDimmLogicalRanks[Dimm] = Ranks / RankMult;
    NBPtr->PsPtr->LrdimmRowAddrBits[Dimm] = Rows + (RankMult >> 1);
    //
    // Program RankDef
    //
    NBPtr->SetBitField (NBPtr, BFRankDef0 + Dimm, (RankMult == 4) ? 3 : RankMult);
    //
    // If LrdimmRowAddressBits > 16, then we must be using some CS signals for rank
    //   multiplication.  If this is the case, then we want to clear the CSPresent bits
    //   that correspond to those  chipselects.
    //   If there are 3 DIMMs per channel, then it will always be CS67, if there are
    //   2DPCH, then DIMM0 will use CS45, and DIMM1 will use CS67.
    //
    if ((ChannelPtr->LrDimmLogicalRanks[Dimm] < 4) && (Dimm >= NumDimmslots)) {
      NBPtr->DCTPtr->Timings.CsPresent &= ~(0x3 << (Dimm << 1));
      ChannelPtr->LrDimmRankMult[Dimm] = 0;
      ChannelPtr->LrDimmLogicalRanks[Dimm] = 0;
      NBPtr->PsPtr->LrdimmRowAddrBits[Dimm] = 0;
    } else {
      IDS_HDT_CONSOLE_DEBUG_CODE (
        if (Dimm < NumDimmslots) {
          IDS_HDT_CONSOLE (MEM_FLOW,"\tDimm %d: Log. Ranks:%d   Phys. Ranks:%d   RowAddrBits:%d   RankMult:%d\n",
            Dimm,
            ChannelPtr->LrDimmLogicalRanks[Dimm],
            ChannelPtr->LrdimmPhysicalRanks[Dimm],
            NBPtr->PsPtr->LrdimmRowAddrBits[Dimm],
            RankMult
            );
        }
      );
    }
  }
  return RetVal;
}

/* -----------------------------------------------------------------------------
 *
 *   This function performs buffer to DRAM training for LRDIMMs
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out]   OptParam   - Optional parameter
 *
 *     @return    TRUE
 */

BOOLEAN
STATIC
MemTLrdimmBuf2DramTrain3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  )
{
  MEM_DATA_STRUCT *MemPtr;
  MEM_NB_BLOCK    *NBPtr;
  UINT8  Dct;
  UINT8  Dimm;
  UINT8  ChipSel;
  UINT16 DimmMask;
  UINT8  i;

  NBPtr = TechPtr->NBPtr;
  MemPtr = NBPtr->MemPtr;

  if (NBPtr->MCTPtr->Status[SbLrdimms]) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\nStart Buffer to DRAM training\n");
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      IDS_HDT_CONSOLE (MEM_STATUS, "\tDct %d\n", Dct);
      NBPtr->SwitchDCT (NBPtr, Dct);
      //
      // ODM needs to be set after Dram Init
      //
      if (NBPtr->StartupSpeed == NBPtr->DCTPtr->Timings.Speed) {
        for (ChipSel = 1; ChipSel < MAX_CS_PER_CHANNEL; ChipSel += 2) {
          if ((NBPtr->DCTPtr->Timings.CsPresent & ((UINT16)1 << ChipSel)) != 0) {
            if ((NBPtr->DCTPtr->Timings.DimmMirrorPresent & (1 << (ChipSel >> 1))) != 0) {
              NBPtr->SetBitField (NBPtr, BFCSBaseAddr0Reg + ChipSel, ((NBPtr->GetBitField (NBPtr, BFCSBaseAddr0Reg + ChipSel)) | ((UINT32)1 << BFOnDimmMirror )));
            }
          }
        }
      }
      //
      // Buffer to DRAM training
      //
      for (Dimm = 0; Dimm < MAX_DIMMS_PER_CHANNEL; Dimm++) {
        DimmMask = (UINT16)1 << Dimm;
        if ((NBPtr->ChannelPtr->LrDimmPresent & DimmMask) != 0) {
          IDS_HDT_CONSOLE (MEM_STATUS, "\t\nDimm %d\n", Dimm);
          //
          // Select the Target Chipselects
          //
          NBPtr->SetBitField (NBPtr, BFMrsChipSel, (Dimm << 1));
          NBPtr->SetBitField (NBPtr, BFCtrlWordCS, 3 << (Dimm << 1));

          NBPtr->SetBitField (NBPtr, BFLrDimmErrOutMonEn, 1);
          MemTSendMBCtlWord3 (TechPtr, F2, RC3, 8);
          // Send F0RC12 with data = 0010b.
          MemTSendMBCtlWord3 (TechPtr, F0, RC12, 2);
          //
          // Wait until D18F2xA0_dct[1:0][RcvParErr]=0 or tCAL * the number of physical ranks expires.
          //
          IDS_HDT_CONSOLE (MEM_FLOW, "\t\tWaiting %d ms...\n", 10 * NBPtr->ChannelPtr->LrdimmPhysicalRanks[Dimm]);
          for (i = 0; i < (NBPtr->ChannelPtr->LrdimmPhysicalRanks[Dimm] * 10); i++) {
            MemUWait10ns (1000000, MemPtr);
            //
            /// Provide option for polling RcvParErr to optimize DRAM bus timing. Currently not required since there are no LRDIMM support
            //
          }
          IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\tRcvParErr = %02x\n", NBPtr->GetBitField (NBPtr, BFRcvParErr));
          NBPtr->SetBitField (NBPtr, BFLrDimmErrOutMonEn, 0);
          MemTSendMBCtlWord3 (TechPtr, F2, RC3, 0);
          // Configure for normal operation: Send F0RC12 with data = 0000b.
          MemTSendMBCtlWord3 (TechPtr, F0, RC12, 0);
        }
      }
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\nEnd Buffer to DRAM training\n");
  }
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function copies trained delays of the first rank of a QR LRDIMM to the third rank
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out]   OptParam   - Optional parameter
 *
 *     @return    TRUE
 */

BOOLEAN
STATIC
MemTLrdimmSyncTrainedDlys (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  )
{
  UINT8 i;
  UINT8 Dimm;
  UINT8 Dct;
  MEM_NB_BLOCK *NBPtr;
  CH_DEF_STRUCT *ChannelPtr;
  UINT16 WrDqsDly;
  UINT16 RcvEnDly;
  UINT16 RdDqsDly;
  UINT16 WrDatDly;
  UINT8  RdDqs2dDly;
  NBPtr = TechPtr->NBPtr;

  if (NBPtr->MCTPtr->Status[SbLrdimms]) {
    IDS_HDT_CONSOLE (MEM_STATUS, "\tSync LRDIMM Delays to remaining ranks.\n");
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      IDS_HDT_CONSOLE (MEM_STATUS, "\tDct %d\n", Dct);
      NBPtr->SwitchDCT (NBPtr, Dct);
      ChannelPtr = NBPtr->ChannelPtr;
      for (Dimm = 0; Dimm < 2; Dimm++) {
        if (ChannelPtr->LrDimmLogicalRanks[Dimm] > 2) {
          // If logical QR LRDIMM, copy trained delays from first rank to third rank
          IDS_HDT_CONSOLE (MEM_FLOW, "\t\tDimm %d -> Dimm %d\n",Dimm, Dimm + 2);
          for (i = 0; i < TechPtr->DlyTableWidth (); i++) {
            WrDqsDly = ChannelPtr->WrDqsDlys[Dimm * TechPtr->DlyTableWidth () + i];
            NBPtr->SetTrainDly (NBPtr, AccessWrDqsDly, DIMM_BYTE_ACCESS (Dimm + 2, i), WrDqsDly);
            ChannelPtr->WrDqsDlys[(Dimm + 2) * TechPtr->DlyTableWidth () + i] = (UINT8)WrDqsDly;

            RcvEnDly = ChannelPtr->RcvEnDlys[Dimm * TechPtr->DlyTableWidth () + i];
            NBPtr->SetTrainDly (NBPtr, AccessRcvEnDly, DIMM_BYTE_ACCESS (Dimm + 2, i), RcvEnDly);
            ChannelPtr->RcvEnDlys[(Dimm + 2) * TechPtr->DlyTableWidth () + i] = RcvEnDly;

            RdDqsDly = ChannelPtr->RdDqsDlys[Dimm * TechPtr->DlyTableWidth () + i];
            NBPtr->SetTrainDly (NBPtr, AccessRdDqsDly, DIMM_BYTE_ACCESS (Dimm + 2, i), RdDqsDly);
            ChannelPtr->RdDqsDlys[(Dimm + 2) * TechPtr->DlyTableWidth () + i] = (UINT8)RdDqsDly;

            WrDatDly = ChannelPtr->WrDatDlys[Dimm * TechPtr->DlyTableWidth () + i];
            NBPtr->SetTrainDly (NBPtr, AccessWrDatDly, DIMM_BYTE_ACCESS (Dimm + 2, i), WrDatDly);
            ChannelPtr->WrDatDlys[(Dimm + 2) * TechPtr->DlyTableWidth () + i] = (UINT8)WrDatDly;
          }
          if ((ChannelPtr->DimmNibbleAccess & (1 << Dimm)) != 0) {
            //
            // If 2D x4 (Not Currently POR for LRDIMMs)
            //
            for (i = 0; i < MAX_NUMBER_LANES; i++) {
              if (ChannelPtr->LrDimmLogicalRanks[Dimm] > 2) {
                // If logical QR LRDIMM, copy trained delays from first rank to third rank
                RdDqs2dDly = ChannelPtr->RdDqs2dDlys[Dimm * MAX_NUMBER_LANES + i];
                NBPtr->SetTrainDly (NBPtr, AccessRdDqs2dDly, DIMM_NBBL_ACCESS (Dimm + 2, i),
                                    ChannelPtr->RdDqs2dDlys[Dimm * MAX_NUMBER_LANES + i]);
                ChannelPtr->RdDqs2dDlys[(Dimm + 2) * MAX_NUMBER_LANES + i] = (UINT8)RdDqs2dDly;
              }
            }
          }
        }
      }
    }
    return TRUE;
  } else {
    return FALSE;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function performs LRDIMM specific tasks during Dimm Presence detection
 *
 *     @param[in,out]  *TechPtr     - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out]  *DimmID      - Dimm ID
 *
 *     @return    TRUE
 *
 */

BOOLEAN
STATIC
MemTLrdimmPresence (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *DimmID
  )
{
  MEM_NB_BLOCK *NBPtr;
  UINT32 BufferID;
  UINT8 Dimm;

  NBPtr = TechPtr->NBPtr;
  Dimm = *(UINT8*) DimmID;

  BufferID = MemTLrDimmGetBufferID (TechPtr, Dimm);
  if ((BufferID == 0x0020B304) || (BufferID == 0x0020B380)) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\tDimm %d: Unsupported LRDIMM Buffer Revision\n", Dimm);
    PutEventLog (AGESA_WARNING, MEM_WARNING_UNSUPPORTED_LRDIMM, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, Dimm, &NBPtr->MemPtr->StdHeader);
    NBPtr->DCTPtr->Timings.CsTestFail |= (UINT16)0x3 << (Dimm << 1);
  }
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function returns LRDIMM Buffer ID Info from the SPD
 *
 *
 *     @param[in,out]   *TechPtr   - Pointer to the Technology Block
 *     @param[in]       Dimm      - Dimm number
 *
 *     @return          Buffer ID Information
 *
 */

UINT32
STATIC
MemTLrDimmGetBufferID (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dimm
  )
{
  UINT8   *SpdBufferPtr;
  UINT32  BufferID;

  BufferID = 0;
  MemTGetDimmSpdBuffer3 (TechPtr, &SpdBufferPtr, Dimm);
  BufferID = (SpdBufferPtr[64] << 16) | (SpdBufferPtr[66] << 8) | (SpdBufferPtr[65]);
  return BufferID;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function implements special case Initialization hooks for LRDIMMs
 *
 *     @param[in]     TechPtr    - Tech Block Pointer
 *     @param[in]     Entrypoint - Entrypoint to indicate when this hook is called
 *     @param[in]     Dimm       - Dimm being configured when hook is called
 *     @param[in]     OptParam   - Not Used
 */

VOID
STATIC
MemTLrdimmInitHook (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       LRDIMM_HOOK_ENTRYPOINT Entrypoint,
  IN       UINT8 Dimm,
  IN OUT   VOID  *OptParam
  )
{
  UINT8         i;
  CONST UINT16   AfterTstabRcwTable[] = {
    0x0270, 0x0000,
    0x0122, 0x0074,
    0x0124, 0x009B,
    0x0126, 0x00C2,
    0x0128, 0x00E8,
    0x01D2, 0x5942,
    0x01D4, 0x836D,
    0x01CE, 0x5942,
    0x01D0, 0x836D,
    0x01D6, 0x017F,
    0x01D8, 0x0000,
    0x01F0, 0x008E,
    0x01F2, 0x00BA,
    0x01F4, 0x00E8,
    0x01F6, 0x0114,
    0x0B40, 0x7054,
    0x0B42, 0xA48A,
    0x0B3C, 0x7054,
    0x0B3E, 0xA48A,
    0x0B38, 0x0100,
    0x0B3A, 0x0000,

    0x0274, 0x55AA,
    0x3012, 0x0080,
    0x3018, 0x6B80
  };
  if (MemTLrDimmGetBufferID (TechPtr, Dimm) != 0x0021B304) {
    return;
  }
  switch (Entrypoint) {
  case AFTER_TSTAB:
    MemTSendMBCtlWord3 (TechPtr, F14, RC0, 0xB);
    for ( i = 0 ; i < (sizeof (AfterTstabRcwTable) / sizeof (UINT16)); i += 2 ) {
      MemTSendExtMBCtlWord3 (TechPtr, AfterTstabRcwTable[i], AfterTstabRcwTable[i + 1], 2);
    }
    break;
  default:
    //
    // If a hook entrypoint is called, it should have a case for it.
    //
    ASSERT (FALSE);
    break;
  }
}
