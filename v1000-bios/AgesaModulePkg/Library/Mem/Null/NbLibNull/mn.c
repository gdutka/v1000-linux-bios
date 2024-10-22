/* $NoKeywords:$ */
/**
 * @file
 *
 * mn.c
 *
 * Common Northbridge functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/)
 * @e \$Revision: 323045 $ @e \$Date: 2015-07-21 17:18:32 +0800 (Tue, 21 Jul 2015) $
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
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "S3.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE LIBRARY_MEM_NULL_NBLIBNULL_MN_FILECODE


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
extern OPTION_MEM_FEATURE_NB* memNTrainFlowControl[];

extern BUILD_OPT_CFG UserOptions;
extern PSO_ENTRY DefaultPlatformMemoryConfiguration[];
extern MEM_PLATFORM_CFG* memPlatformTypeInstalled[];

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function initializes the default values in the MEM_DATA_STRUCT
 *
 *     @param[in,out]   *MemPtr  - Pointer to the MEM_DATA_STRUCT
 *
 */
VOID
MemNCmnInitDefaultsNb (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function initializes member functions and variables of NB block.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNInitNBDataNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}
/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   Get System address of Chipselect RJ 16 bits (Addr[47:16])
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       Receiver - Chipselect to be targeted [0-7]
 *     @param[out]      AddrPtr -  Pointer to System Address [47:16]
 *
 *     @return      TRUE  - Address is valid
 *     @return      FALSE  - Address is not valid
 */

BOOLEAN
MemNGetMCTSysAddrNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Receiver,
     OUT   UINT32 *AddrPtr
  )
{

  return FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function determines if a Rank is enabled.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]    Receiver - Receiver to check
 *     @return - FALSE
 *
 */

BOOLEAN
MemNRankEnabledNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Receiver
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *     This function sets the EccSymbolSize bit depending upon configurations
 *     and system override.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNSetEccSymbolSizeNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/*-----------------------------------------------------------------------------*/
/**
 *
 *      This function flushes the  training pattern
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in] Address - System Address [47:16]
 *     @param[in] ClCount - Number of cache lines
 *
 */

VOID
MemNFlushPatternNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address,
  IN       UINT16 ClCount
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function compares test pattern with data in buffer and
 *     return a pass/fail bitmap for 8 bytelanes (upper 8 bits are reserved)
 *
 *     @param[in,out]   *NBPtr    - Pointer to the MEM_NB_BLOCK
 *     @param[in]       Buffer[]  - Buffer data from DRAM (Measured data from DRAM) to compare
 *     @param[in]       Pattern[] - Pattern (Expected data in ROM/CACHE) to compare against
 *     @param[in]       ByteCount - Byte count
 *
 *     @return  PASS - Bitmap of results of comparison
 */

UINT16
MemNCompareTestPatternNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT16 ByteCount
  )
{
  return 0;
}

/*-----------------------------------------------------------------------------
 *
 *
 *     This function compares test pattern with data in buffer and
 *     return a pass/fail bitmap for 8 bytelanes (upper 8 bits are reserved)
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in]       Buffer[]  - Buffer data from DRAM (Measured data from DRAM) to compare
 *     @param[in]       Pattern[] - Pattern (Expected data in ROM/CACHE) to compare against
 *     @param[in]       ByteCount - Byte count
 *
 *     @retval  PASS - Bitmap of results of comparison
 * ----------------------------------------------------------------------------
 */
UINT16
MemNInsDlyCompareTestPatternNb (
  IN       MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT16 ByteCount
  )
{
  return 0;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function sets the training control flow for UNB
 *      The DDR3 mode bit must be set prior to calling this function
 *
 *      @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 */
BOOLEAN
MemNTrainingFlowUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function sets the ECC exclusion range for UNB
 *
 *      @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 */
VOID
MemNSetEccExclusionRangeUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}
/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */


