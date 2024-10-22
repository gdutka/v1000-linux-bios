/* $NoKeywords:$ */
/**
 * @file
 *
 * mfodthermal.c
 *
 * On Dimm thermal management.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat)
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
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "Library/AmdMemFeatOnDimmThermalLib.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEM_NULL_FEATONDIMMTHERMALLIBNULL_MFODTHERMAL_FILECODE

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
 *   This function does On-Dimm thermal management.
 *
 *   @param[in, out]   *NBPtr - Pointer to the MEM_NB_BLOCK.
 *
 *     @return          TRUE -  This feature is enabled.
 *     @return          FALSE - This feature is not enabled.
 */

BOOLEAN
MemFOnDimmThermal (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;
  CH_DEF_STRUCT *ChannelPtr;
  MEM_DATA_STRUCT *MemPtr;
  BOOLEAN ODTSEn;

  ODTSEn = TRUE;

  ASSERT (NBPtr != NULL);
  MemPtr = NBPtr->MemPtr;
  AGESA_TESTPOINT (TpProcMemOnDimmThermal, &MemPtr->StdHeader);
  //
  // Enable ODTSEn only if all installed DIMMs on this node have an On-Dimm Temperature Sensor.
  //
  if (NBPtr->MCTPtr->NodeMemSize != 0) {
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      NBPtr->SwitchDCT (NBPtr, Dct);
      if (NBPtr->GetBitField (NBPtr, BFDisDramInterface) == 0) {
        ChannelPtr = NBPtr->ChannelPtr;
        //
        // If all DIMMs on this channel do not support the Thermal Sensor, Set ODTSEn to false and quit
        //
        if ((ChannelPtr->DimmThermSensorPresent & ChannelPtr->ChDimmValid) != ChannelPtr->ChDimmValid) {
          ODTSEn = FALSE;
          break;
        }
      }
    }
  }
  //
  // If all dram devices on support ODTS
  //
  NBPtr->SetBitField (NBPtr, BFODTSEn, (ODTSEn == TRUE) ? 1 : 0);
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tODTSEn = %d\n", ODTSEn);
  return ODTSEn;
}


/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

