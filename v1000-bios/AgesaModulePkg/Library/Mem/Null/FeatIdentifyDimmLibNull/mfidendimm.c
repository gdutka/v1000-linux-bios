/* $NoKeywords:$ */
/**
 * @file
 *
 * mfidendimm.c
 *
 * Translate physical system address to dimm identification.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat)
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
#include "cpuRegisters.h"
#include "OptionMemory.h"
#include "Library/AmdHeapLib.h"
#include "Library/AmdMemFeatIdentifyDimmLib.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEM_NULL_FEATIDENTIFYDIMMLIBNULL_MFIDENDIMM_FILECODE

extern MEM_NB_SUPPORT memNBInstalled[];

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define MAX_DCTS_PER_DIE        2   ///< Max DCTs per die
#define MAX_CHLS_PER_DCT        1   ///< Max Channels per DCT

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
*   This function identifies the dimm on which the given memory address locates.
*
*   @param[in, out]   *AmdDimmIdentify - Pointer to the parameter structure AMD_IDENTIFY_DIMM
*
*   @retval           AGESA_SUCCESS - Successfully translate physical system address
*                                     to dimm identification.
*                     AGESA_BOUNDS_CHK - Targeted address is out of bound.
*
*/

AGESA_STATUS
AmdIdentifyDimm (
  IN OUT   AMD_IDENTIFY_DIMM *AmdDimmIdentify
  )
{
  return AGESA_SUCCESS;
}


/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------*/
/**
*
*   This function is the interface to call the PCI register access function
*   defined in NB block.
*
*   @param[in]   *NBPtr - Pointer to the parameter structure MEM_NB_BLOCK
*   @param[in]   NodeID - Node ID number of the target Northbridge
*   @param[in]   DctNum - DCT number if applicable, otherwise, put 0
*   @param[in]   BitFieldName - targeted bitfield
*
*   @retval      UINT32 - 32 bits PCI register value
*
*/
UINT32
MemFGetPCI (
  IN   MEM_NB_BLOCK *NBPtr,
  IN   UINT8 NodeID,
  IN   UINT8 DctNum,
  IN   BIT_FIELD_NAME BitFieldName
  )
{
  return 0;
}

/*-----------------------------------------------------------------------------*/
/**
*
*   This function returns an even parity bit (making the total # of 1's even)
*   {0, 1} = number of set bits in argument is {even, odd}.
*
*   @param[in]   address - the address on which the parity bit will be calculated
*
*   @retval      UINT8 - parity bit
*
*/

UINT8
MemFUnaryXOR (
  IN   UINT32 address
  )
{
  return 0;
}

