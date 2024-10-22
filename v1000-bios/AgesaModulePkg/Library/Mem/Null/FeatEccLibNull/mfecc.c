/* $NoKeywords:$ */
/**
 * @file
 *
 * mfecc.c
 *
 * Feature ECC initialization functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/ECC)
 * @e \$Revision: 313790 $ @e \$Date: 2015-02-27 13:31:28 +0800 (Fri, 27 Feb 2015) $
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
#include "Library/AmdBaseLib.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mfecc.h"
#include "Filecode.h"
#include "mfmemclr.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEM_NULL_FEATECCLIBNULL_MFECC_FILECODE

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

VOID
STATIC
InitECCOverriedeStruct (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   ECC_OVERRIDE_STRUCT *pecc_override_struct
  );

BOOLEAN
MemFCheckECC (
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
 *      This function checks to see if ECC can be enabled on all nodes
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  This feature is enabled.
 *     @return          FALSE - This feature is not enabled.
 */

BOOLEAN
MemFCheckECC (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return FALSE;
}

  /* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function initializes the ECC on all nodes
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  This feature is enabled.
 *     @return          FALSE - This feature is not enabled.
 */

BOOLEAN
MemFInitECC (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return TRUE;
}

VOID
STATIC
InitECCOverriedeStruct (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   ECC_OVERRIDE_STRUCT *pecc_override_struct
  )
{

}

