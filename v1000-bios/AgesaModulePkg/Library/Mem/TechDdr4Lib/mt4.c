/* $NoKeywords:$ */
/**
 * @file
 *
 * mt4.c
 *
 * Common Technology  functions for DDR4
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech/DDR4)
 * @e \$Revision: 311646 $ @e \$Date: 2015-01-26 17:32:59 +0800 (Mon, 26 Jan 2015) $
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
#include "mu.h"
#include "mt.h"
#include "mt4.h"
#include "mtspd4.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

/* features */
#define FILECODE LIBRARY_MEM_TECHDDR4LIB_MT4_FILECODE

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
 *   This function Constructs the technology block
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

BOOLEAN
MemConstructTechBlock4 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  TECHNOLOGY_TYPE *TechTypePtr;
  UINT8 Dct;
  UINT8 Channel;
  UINT8 i;
  DIE_STRUCT *MCTPtr;
  DCT_STRUCT *DCTPtr;
  CH_DEF_STRUCT *ChannelPtr;
  UINT8 DimmSlots;


  TechTypePtr = (TECHNOLOGY_TYPE *) FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_MEM_TECH, NBPtr->MCTPtr->SocketId, 0, 0, NULL, NULL);
  if (TechTypePtr != NULL) {
    // Ensure the platform override value is valid
    ASSERT ((*TechTypePtr == DDR4_TECHNOLOGY) || (*TechTypePtr == DDR3_TECHNOLOGY) || (*TechTypePtr == DDR2_TECHNOLOGY));
    if (*TechTypePtr != DDR4_TECHNOLOGY) {
      return FALSE;
    }
  }

  TechPtr->NBPtr = NBPtr;
  TechPtr->RefPtr = NBPtr->RefPtr;
  MCTPtr = NBPtr->MCTPtr;

  TechPtr->DimmPresence = MemTDIMMPresence4;
  TechPtr->SpdCalcWidth = MemTSPDCalcWidth4;
  TechPtr->SpdGetTargetSpeed = MemTSPDGetTargetSpeed4;
  TechPtr->AutoCycTiming = MemTAutoCycTiming4;
  TechPtr->SpdSetBanks = MemTSPDSetBanks4;
  TechPtr->GetCSIntLvAddr = MemTGetCSIntLvAddr4;
  TechPtr->GetDimmSpdBuffer = MemTGetDimmSpdBuffer4;
  TechPtr->MaxFilterDly = 0;
  //
  // Map the Logical Dimms on this channel to the SPD that should be used for that logical DIMM.
  // The pointers to the DIMM SPD information is as follows (2 Dimm/Ch and 3 Dimm/Ch examples).
  //
  //    DIMM Spd Buffer                          Current Channel DimmSpdPtr[MAX_DIMMS_PER_CHANNEL] array
  //    (Number of dimms varies by platform)     (Array size is determined in AGESA.H) Dimm operations loop
  //                                              on this array only)
  //    2 DIMMS PER CHANNEL
  //
  //    Socket N  Channel N Dimm 0 SR/DR DIMM <--------------DimmSpdPtr[0]
  //                        Dimm 1 SR/DR DIMM <--------------DimmSpdPtr[1]
  //                                                         DimmSpdPtr[2]------->NULL
  //                                                         DimmSpdPtr[3]------->NULL
  //
  //    Socket N  Channel N Dimm 0 SR/DR DIMM <--------------DimmSpdPtr[0]
  //                        Dimm 1    QR DIMM <---------+----DimmSpdPtr[1]
  //                                                    |    DimmSpdPtr[2]------->NULL
  //                                                    +----DimmSpdPtr[3]
  //
  //    Socket N  Channel N Dimm 0    QR DIMM <-----+--------DimmSpdPtr[0]
  //                        Dimm 1    QR DIMM <-----|---+----DimmSpdPtr[1]
  //                                                +-- | ---DimmSpdPtr[2]
  //                                                    +----DimmSpdPtr[3]
  //
  //    3 DIMMS PER CHANNEL
  //
  //    Socket N  Channel N Dimm 0 SR/DR DIMM <--------------DimmSpdPtr[0]
  //                        Dimm 1 SR/DR DIMM <--------------DimmSpdPtr[1]
  //                        Dimm 3 SR/DR DIMM <--------------DimmSpdPtr[2]
  //                                                         DimmSpdPtr[3]------->NULL
  //
  //    Socket N  Channel N Dimm 0 SR/DR DIMM <--------------DimmSpdPtr[0]
  //                        Dimm 1    QR DIMM <---------+----DimmSpdPtr[1]
  //                        Dimm 3 SR/DR DIMM <-------- | ---DimmSpdPtr[2]
  //                                                    +----DimmSpdPtr[3]
  //
  //
  //    FOR LRDIMMS
  //
  //    This code will assign SPD pointers on the basis of Physical ranks, even though
  //    an LRDIMM may only use one or two logical ranks, that determination will have to
  //    be made from downstream code.  An LRDIMM with greater than 2 Physical ranks will have
  //    its DimmSpdPtr[] mapped as if it were a QR in the above diagrams.

  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    DCTPtr = NBPtr->DCTPtr;
    for (Channel = 0; Channel < NBPtr->ChannelCount; Channel++) {
      NBPtr->SwitchChannel (NBPtr, Channel);
      ChannelPtr = NBPtr->ChannelPtr;
      ChannelPtr->TechType = DDR4_TECHNOLOGY;
      ChannelPtr->MCTPtr = MCTPtr;
      ChannelPtr->DCTPtr = DCTPtr;

      DimmSlots = GetMaxDimmsPerChannel (NBPtr->RefPtr->PlatformMemoryConfiguration,
                                         MCTPtr->SocketId,
                                         NBPtr->GetSocketRelativeChannel (NBPtr, Dct, Channel)
                                        );
      //
      // Initialize the SPD pointers for each Dimm
      //
      for (i = 0 ; i < (sizeof (ChannelPtr->DimmSpdPtr) / sizeof (ChannelPtr->DimmSpdPtr[0])) ; i++) {
        ChannelPtr->DimmSpdPtr[i] = NULL;
      }
      for (i = 0 ; i < DimmSlots; i++) {
        ChannelPtr->DimmSpdPtr[i] = &(ChannelPtr->SpdPtr[i]);
        if ( (i + 2) < (sizeof (ChannelPtr->DimmSpdPtr) / sizeof (ChannelPtr->DimmSpdPtr[0]))) {
          if (ChannelPtr->DimmSpdPtr[i]->DimmPresent) {
            if ((((ChannelPtr->DimmSpdPtr[i]->Data[SPD_RANKS] >> RANKS_SHIFT) & RANKS_MASK) + 1) > 2) {
              ChannelPtr->DimmSpdPtr[i + 2] = &(ChannelPtr->SpdPtr[i]);
            }
          }
        }
      }
    }
  }
  // Initialize Common technology functions
  MemTCommonTechInit (TechPtr);

  return TRUE;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

