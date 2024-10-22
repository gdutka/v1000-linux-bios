/* $NoKeywords:$ */
/**
 * @file
 *
 * mnpspcz.c
 *
 * Northbridge CZ PSP supporting functions
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
#include "mport.h"
#include "mm.h"
#include "mn.h"
#include "OptionMemory.h"
#include "mncz.h"
#include "Filecode.h"
#include "mftds.h"
#include "mu.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)


#define FILECODE LIBRARY_MEM_NULL_NBCZLIBNULL_MNPSPCZ_FILECODE

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
 *     This function detects PSP presence
 *
 *     @param[in,out]   *NBPtr             - Pointer to the MEM_NB_BLOCK
 *
 *     @return    TRUE
 */
BOOLEAN
MemPspDetectionMechanismCZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function detects PSP fuse of PLATFORM_SECURE_BOOT
 *
 *     @param[in,out]   *NBPtr             - Pointer to the MEM_NB_BLOCK
 *
 *     @return    TRUE
 */
BOOLEAN
MemPspPlatformSecureBootEnCZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function detects if code is currently running on PSP
 *
 *     @param[in,out]   *NBPtr             - Pointer to the MEM_NB_BLOCK
 *
 *     @return    TRUE
 */
BOOLEAN
MemRunningOnPspCZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return FALSE;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

