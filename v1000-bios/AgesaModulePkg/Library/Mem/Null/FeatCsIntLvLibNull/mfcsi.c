/* $NoKeywords:$ */
/**
 * @file
 *
 * mfcsi.c
 *
 * Feature bank interleaving support (AKA Chip Select Interleaving )
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/Csintlv)
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


/* This file contains functions for Chip Select interleaving */



#include "AGESA.h"
#include "Library/AmdBaseLib.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "Library/AmdMemFeatCsIntLvLib.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEM_NULL_FEATCSINTLVLIBNULL_MFCSI_FILECODE

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

BOOLEAN
STATIC
MemFDctInterleaveBanks (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
STATIC
CsIntSwap (
  IN OUT   UINT32 *BaseMaskRegPtr,
  IN       UINT8 EnChipSels,
  IN       UINT8 LoBit,
  IN       UINT8 HiBit
  );

BOOLEAN
MemFUndoInterleaveBanks (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

/*
 *-----------------------------------------------------------------------------
 *                                EXPORTED FUNCTIONS
 *
 *-----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function Applies DIMM bank (chip-select) interleaving if enabled
 *      and if all criteria are met.  Interleaves chip-selects on page boundaries.
 *      This function calls subfunctions that sets up CS interleaving on multiple Sockets
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  This feature is enabled.
 *     @return          FALSE - This feature is not enabled.
 */

BOOLEAN
MemFInterleaveBanks (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *     This function checks if bank interleaving has been enabled or not. If yes, it will
 *     undo bank interleaving. Otherwise, it does nothing.
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  Bank interleaving has been enabled.
 *     @return          FALSE - Bank interleaving has not been enabled.
 */

BOOLEAN
MemFUndoInterleaveBanks (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
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
 *
 *      This function Applies DIMM bank (chip-select) interleaving if enabled
 *      and if all criteria are met.  Interleaves chip-selects on page boundaries.
 *      This function is run once per Socket
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  Register bits have been swapped.
 *     @return          FALSE - Register bits have not been swapped.
 *
 */

BOOLEAN
STATIC
MemFDctInterleaveBanks (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This supporting function swaps Chip selects
 *
 *     @param[in,out]  *BaseMaskRegPtr   - Pointer to the Mask Register
 *     @param[in]      *EnChipSels    - Chip Selects to Enable
 *     @param[in]      *LoBit         - Lowest Bit
 *     @param[in]      *HiBit         - Highest Bit
 *
 *
 */

VOID
STATIC
CsIntSwap (
  IN OUT   UINT32 *BaseMaskRegPtr,
  IN       UINT8 EnChipSels,
  IN       UINT8 LoBit,
  IN       UINT8 HiBit
  )
{
}

