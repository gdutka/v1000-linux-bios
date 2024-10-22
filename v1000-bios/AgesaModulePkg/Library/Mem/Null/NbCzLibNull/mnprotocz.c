/* $NoKeywords:$ */
/**
 * @file
 *
 * mnprotocz.c
 *
 * Northbridge support functions for Errata and early samples
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/CZ)
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



#include "AGESA.h"
#include "mm.h"
#include "mn.h"
#include "Filecode.h"
#include "cpuRegisters.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE LIBRARY_MEM_NULL_NBCZLIBNULL_MNPROTOCZ_FILECODE

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*
 *-----------------------------------------------------------------------------
 *                                EXPORTED FUNCTIONS
 *
 *-----------------------------------------------------------------------------
 */
VOID
MemNInitEarlySampleSupportCZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );
/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function initializes early sample support for CZ
 *
 *     @param[in,out] NBPtr  - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
MemNInitEarlySampleSupportCZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  ///@todo  - Need to review for Carrizo whether it is valid
  //if ((NBPtr->MCTPtr->LogicalCpuid.Revision & AMD_F15_CZ_A0) != 0) {
  //}
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

