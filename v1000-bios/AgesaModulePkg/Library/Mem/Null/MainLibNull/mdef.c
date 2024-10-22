/* $NoKeywords:$ */
/**
 * @file
 *
 * mdef.c
 *
 * Memory Controller header file
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
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
#include "Library/AmdBaseLib.h"
#include "Library/IdsLib.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "mport.h"
#include "mm.h"
#include "mu.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE LIBRARY_MEM_NULL_MAINLIBNULL_MDEF_FILECODE
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
AGESA_STATUS
MemMFlowDef (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/
/**
 *
 *         This is the default return function
 */

VOID
memDefRet ( VOID )
{
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function is the default return function that returns TRUE
 *
 */
BOOLEAN
memDefTrue ( VOID )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function is used in place of an un-supported function that returns FALSE.
 *
 */
BOOLEAN
memDefFalse ( VOID )
{
  return FALSE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *         This is the default return function for flow control
 */
AGESA_STATUS
MemMFlowDef (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  return AGESA_SUCCESS;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function is used in place of an un-supported function that returns AGESA_SUCCESS.
 *
 */
AGESA_STATUS
memDefRetSuccess ( VOID )
{
  return AGESA_SUCCESS;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function is used to initialize functions which must be overriden
 *        With platform - specific code. It will always assert.
 *
 */
VOID
memDefaultUndefined ( VOID )
{
  ASSERT (FALSE)
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *  Waits specified number of 10ns cycles
 *      @param[in,out] MemPtr - pointer to MEM_DATA_STRUCTURE
 *      @param[in] Count - Number of 10ns cycles to wait
 *
 * ----------------------------------------------------------------------------
 */

VOID
MemFS3Wait10ns (
  IN       UINT32 Count,
  IN OUT   MEM_DATA_STRUCT *MemPtr
  )
{
}

