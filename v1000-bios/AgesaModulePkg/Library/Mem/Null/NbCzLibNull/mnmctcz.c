/* $NoKeywords:$ */
/**
 * @file
 *
 * mnmctcz.c
 *
 * Northbridge CZ MCT supporting functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/CZ)
 * @e \$Revision: 312808 $ @e \$Date: 2015-02-11 19:22:11 +0800 (Wed, 11 Feb 2015) $
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
#include "Library/AmdHeapLib.h"
#include "mport.h"
#include "Gnb.h"
#include "mm.h"
#include "mn.h"
#include "mnreg.h"
#include "OptionMemory.h"
#include "mncz.h"
#include "merrhdl.h"
//#include "cpuFeatures.h"
#include "Filecode.h"
#include "mftds.h"
#include "mu.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)


#define FILECODE LIBRARY_MEM_NULL_NBCZLIBNULL_MNMCTCZ_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define DCT_ADDR_VAL      0
#define LGCY_MMIO_HOLE_EN 1
#define DCT_SEL           4
#define ACP_ENGINE_SIZE_4MB_RJ16  0x0040

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
 *     This function switches memory pstate for CZ
 *
 *     @param[in,out]   *NBPtr    - Pointer to the MEM_NB_BLOCK
 *     @param[in]       MemPstate - Mem Pstate
 *
 * ----------------------------------------------------------------------------
 */
VOID
MemNSwitchMemPstateCZ (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 MemPstate
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function create the HT memory map
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return  TRUE  - indicates that a FATAL error has not occurred
 *     @return  FALSE - indicates that a FATAL error has occurred
 *
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNHtMemMapCZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function does synchronous channel initialization
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNSyncChannelInitCZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function obtains the memory frequency in the current context
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */
UINT16
MemNGetMemClkFreqInCurrentContextCZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return 0;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function calculates and programs NB P-state dependent registers for CZ
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNProgramNbPstateDependentRegistersCZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function releases the NB P-state force for CZ
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   OptParam - Optional parameter
 *
 *     @return    TRUE
 */
BOOLEAN
MemNReleaseNbPstateCZ (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  return TRUE;
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *     Report the Uma size that is going to be allocated.
 *     Total system memory   UMASize
 *     >= 6G                   1G
 *     >= 4G                 512M
 *     >= 2G                 256M
 *     < 2G                  256M
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          Uma size [31:0] = Addr [47:16]
 */
UINT32
MemNGetUmaSizeCZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return 0;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function allocates memory storage for C6 and ACP Engine when
 *     it is requested to be enabled
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
MemNAllocateC6AndAcpEngineStorageCZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function fixes up UMA info
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   OptParam - Optional parameter
 *
 *     @return    TRUE
 */
BOOLEAN
MemNFixupUmaInfoCZ (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function sets final values for specific registers.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemNFinalizeMctCZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function force the ECC symbol size
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *Size    - ECC symbol size
 *                               FALSE = x4
 *                               TRUE  = x8
 *
 *     @return    TRUE
 */

BOOLEAN
MemNForceEccSymbolSizeCZ (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Size
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   Set DRAM Controller select channel interleave address bit.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNInitChannelIntlvAddressBitCZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function disables scrubber temporarily for write access to
 *     DCT additional address space
 *
 *     @param[in,out]   *NBPtr            - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNDisableScrubberCZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function restores scrubber settings
 *
 *     @param[in,out]   *NBPtr            - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNRestoreScrubberCZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function disable memory hole mappings
 *
 *     @param[in,out]   *NBPtr             - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *OptParam          - Optional parameter
 *
 *     @return    TRUE
 */

BOOLEAN
MemNDisableMemHoleMappingCZ (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function restore memory hole mappings
 *
 *     @param[in,out]   *NBPtr             - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *OptParam          - Optional parameter
 *
 *     @return    TRUE
 */

BOOLEAN
MemNRestoreMemHoleMappingCZ (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  return TRUE;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

