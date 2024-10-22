/* $NoKeywords:$ */
/**
 * @file
 *
 * ma.c
 *
 * Initializes ARDK Block
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ardk)
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
#include "ma.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEM_ARDKLIB_MA_FILECODE

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
 *    This is the default return function of the ARDK block. The function always
 *    returns   AGESA_UNSUPPORTED
 *
 *     @param[in,out]   *MemData           Pointer to MEM_DATA_STRUCTURE
 *     @param[in]       SocketID        Socket number
 *     @param[in]       *CurrentChannel       Pointer to CH_DEF_STRUCT
 *
 *     @return          AGESA_UNSUPPORTED  AGESA status indicating that default is unsupported
 *
 */

AGESA_STATUS
MemAGetPsCfgDef (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  )
{
  return AGESA_UNSUPPORTED;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    This function returns the rank type map of a channel.
 *
 *     @param[in]       *CurrentChannel       Pointer to CH_DEF_STRUCT
 *
 *     @return          UINT16 - The map of rank type.
 *
 */
UINT16
MemAGetPsRankType (
  IN       CH_DEF_STRUCT *CurrentChannel
  )
{
  UINT8 i;
  UINT16 DIMMRankType;

  DIMMRankType = 0;
  for (i = 0; i < MAX_DIMMS_PER_CHANNEL; i++) {
    if (CurrentChannel->MCTPtr->Status[SbLrdimms]) {
      if ((CurrentChannel->LrDimmPresent & (UINT8) 1 << i) != 0) {
        DIMMRankType |= (UINT16) 1 << (i << 2);
      }
    } else {
      if ((CurrentChannel->DimmQrPresent & (UINT8) 1 << i) != 0) {
        if (i < 2) {
          DIMMRankType |= (UINT16) 4 << (i << 2);
        }
      } else if ((CurrentChannel->DimmDrPresent & (UINT8) 1 << i) != 0) {
        DIMMRankType |= (UINT16) 2 << (i << 2);
      } else if ((CurrentChannel->DimmSRPresent & (UINT8) 1 << i) != 0) {
        DIMMRankType |= (UINT16) 1 << (i << 2);
      }
    }
  }
  return DIMMRankType;
}

