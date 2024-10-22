/* $NoKeywords:$ */
/**
 * @file
 *
 * lvddr3.c
 *
 * Voltage change for DDR3 DIMMs.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/LVDDR3)
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
#include "Library/IdsLib.h"
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEM_FEATLVDDR3LIB_MFLVDDR3_FILECODE

/* features */
#include "Library/AmdMemFeatLvDdr3Lib.h"


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

/*-----------------------------------------------------------------------------*/
/**
 *
 *   This function calculate the common lowest voltage supported by all DDR3
 *   DIMMs in the system. This function only needs to be called on BSP.
 *
 *   @param[in, out]    *NBPtr - Pointer to NB block
 *
 *     @return          TRUE -  This feature is enabled.
 *     @return          FALSE - This feature is not enabled.
 */

BOOLEAN
MemFLvDdr3 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  CH_DEF_STRUCT *ChannelPtr;
  MEM_TECH_BLOCK  *TechPtr;
  MEM_SHARED_DATA *mmSharedPtr;
  UINT8 Dct;
  UINT8 Channel;
  UINT8 Dimm;
  UINT8 *SpdBufferPtr;
  UINT8 VDDByte;
  UINT8 VoltageMap;

  mmSharedPtr = NBPtr->SharedPtr;
  TechPtr = NBPtr->TechPtr;
  VoltageMap = 0xFF;

  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    for (Channel = 0; Channel < NBPtr->ChannelCount; Channel++) {
      NBPtr->SwitchChannel (NBPtr, Channel);
      ChannelPtr = NBPtr->ChannelPtr;
      for (Dimm = 0; Dimm < MAX_DIMMS_PER_CHANNEL; Dimm++) {
        if (TechPtr->GetDimmSpdBuffer (TechPtr, &SpdBufferPtr, Dimm)) {
          // SPD byte 6: Module Nominal Voltage, VDD
          // 1.5v - bit 0
          // 1.35v - bit 1
          // 1.2v - bit 2
          VDDByte = SpdBufferPtr[MNVVDD];
          IDS_HDT_CONSOLE (MEM_FLOW, "Node%d DCT%d Channel%d Dimm%d VDD Byte: 0x%02x\n", NBPtr->Node, Dct, Channel, Dimm, VDDByte);

          // Reverse the 1.5V operable bit. So its encoding can be consistent
          // with that of 1.35V and 1.25V operable bit.
          VDDByte ^= 1;
          ASSERT (VDDByte != 0);

          if (mmSharedPtr->VoltageMap != 0) {
            // Get the common supported voltage map
            VoltageMap &= VDDByte;
          } else {
            // This is the second execution of all the loop as no common voltage is found
            if (VDDByte == (1 << VOLT1_5_ENCODED_VAL)) {
              // Always exclude 1.5V dimm if no common voltage is found
              ChannelPtr->DimmExclude |= (UINT16) 1 << Dimm;
            }
          }
        }
      }
      if (mmSharedPtr->VoltageMap == 0) {
        NBPtr->DCTPtr->Timings.DimmExclude |= ChannelPtr->DimmExclude;
      }
    }
  }

  if (mmSharedPtr->VoltageMap != 0) {
    mmSharedPtr->VoltageMap &= VoltageMap;
  }

  return TRUE;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

