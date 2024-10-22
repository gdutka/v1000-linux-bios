/* $NoKeywords:$ */
/**
 * @file
 *
 * mtot3.c
 *
 * Technology Non-SPD Timings for DDR3
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
#include "Library/IdsLib.h"
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mtot3.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE LIBRARY_MEM_TECHDDR3LIB_MTOT3_FILECODE
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
 *   This function adjusts the Twrwr value for DDR3.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemTAdjustTwrwr3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  DCT_STRUCT *DCTPtr;

  DCTPtr = TechPtr->NBPtr->DCTPtr;

  // For DDR3, value 0000b-0001b and >= 1011b of Twrwr is reserved.
  if (DCTPtr->Timings.Twrwr < 2) {
    DCTPtr->Timings.Twrwr = 2;
  } else if (DCTPtr->Timings.Twrwr > 10) {
    DCTPtr->Timings.Twrwr = 10;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function adjusts the Twrrd value for DDR3.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemTAdjustTwrrd3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  DCT_STRUCT *DCTPtr;

  DCTPtr = TechPtr->NBPtr->DCTPtr;

  // For DDR3, value 0000b, 0001b, and > 1010b of Twrrd is reserved.
  if (DCTPtr->Timings.Twrrd < 2) {
    DCTPtr->Timings.Twrrd = 2;
  } else if (DCTPtr->Timings.Twrrd > 10) {
    DCTPtr->Timings.Twrrd = 10;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function gets the LD value for DDR3.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  Value of LD
 */

INT8
MemTGetLD3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  INT8 LD;
  MEM_NB_BLOCK *NBPtr;
  NBPtr = TechPtr->NBPtr;
  //
  // For DDR3, BIOS calculates the latency difference (Ld) as equal to read CAS latency minus write CAS
  // latency, in MEMCLKs (see F2x[1, 0]88[Tcl] and F2x[1, 0]84[Tcwl]) which can be a negative or positive
  // value.
  //
  LD = ((INT8) NBPtr->GetBitField (NBPtr, BFTcl) + 4) - ((INT8) NBPtr->GetBitField (NBPtr, BFTcwl) + 5);

  return LD;
}

