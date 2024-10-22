/* $NoKeywords:$ */
/**
 * @file
 *
 * mfmchi.c
 *
 * Feature Multi-channel interleaving support (more than 2 channels)
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/Chintlv)
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
#include "Library/AmdMemFeatChIntLvLib.h"
#include "mu.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEM_NULL_FEATCHINTLVLIBNULL_MFMCHI_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

#define _4GB_ (0x10000ul >> 10)
#define _16MB_RJ16  0x0100

#define DCT_ADDR_VAL      0
#define LGCY_MMIO_HOLE_EN 1
#define DCT_OFFSET_EN     3
#define DCT_SEL           4

#define FIX_DCT(Dct)  ((UINT8) (((NBPtr->IsSupported[MasterDct1Relocate]) && ((Dct) == 1)) ? 3 : (Dct)))

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
 *  Applies DIMM channel interleaving if enabled. Called once per Node.
 *  This function is dedicated for the memory controller that has DRAM Controller Base/Limit registers
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  This feature is enabled.
 *     @return          FALSE - This feature is not enabled.
 */

BOOLEAN
MemFInterleaveMultiChannels (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return TRUE;
}

