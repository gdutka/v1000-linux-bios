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
 * @e \$Revision: 313790 $ @e \$Date: 2015-02-26 23:31:28 -0600 (Thu, 26 Feb 2015) $
 *
 **/
/*****************************************************************************
  *
  * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
  *
  *******************************************************************************
  *
 */

#include "Library/AmdBaseLib.h"
#include "Library/AmdMemBaseLib/AmdMemBaseLib.h"
#include "Library/AmdMemFeatUmaLib.h"
#include "Filecode.h"

#define FILECODE LIBRARY_MEM_FEATUMALIB_AMDMEMFEATUMALIB_FILECODE

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


  /* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function returns the status of UMA
 *
 *     @param[in,out]   *MemDataPtr   - Pointer to the MEM_DATA_STRUCT
 *
 *     @return          TRUE -  This feature is enabled.
 *     @return          FALSE - This feature is not enabled.
 */

BOOLEAN
MemFGetUmaInfo (
  IN OUT   MEM_DATA_STRUCT *MemDataPtr
  )
{
  if (MemDataPtr->UmaEnabled == TRUE) {
    return TRUE;
  } else {
    return FALSE;
  }
}

