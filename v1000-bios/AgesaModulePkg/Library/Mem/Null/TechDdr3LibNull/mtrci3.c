/* $NoKeywords:$ */
/**
 * @file
 *
 * mtrci3.c
 *
 * Technology Control word initialization for DDR3
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
#include "Library/IdsLib.h"
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "mt.h"
#include "mt3.h"
#include "mtrci3.h"
#include "merrhdl.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE LIBRARY_MEM_NULL_TECHDDR3LIBNULL_MTRCI3_FILECODE
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
 *   This function sends control words
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemTDramControlRegInit3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function calculates the ControlRC value
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]     CtrlWordNum  -  control Word number.
 *     @param[in]     RawCard  -  Raw Card
 *     @param[in]     ChipSel  - Target Chip Select
 *     @return  Control Word value
 */

UINT8
MemTGetCtlWord3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 CtrlWordNum,
  IN       UINT8 RawCard,
  IN       UINT8 ChipSel
  )
{
  return 0;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sends control word command
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]     CmdNum  -  control number.
 *     @param[in]     Value  -  value to send
 *
 */

VOID
MemTSendCtlWord3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 CmdNum,
  IN       UINT8 Value
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sends specific control words commands before frequency change for certain DRAM buffers.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
FreqChgCtrlWrd3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{

}


