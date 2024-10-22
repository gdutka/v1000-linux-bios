/* $NoKeywords:$ */
/**
 * @file
 *
 * mnfeat.c
 *
 * Common Northbridge features
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB)
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
#include "Ids.h"
#include "Ids.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "PlatformMemoryConfiguration.h"
#include "merrhdl.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE LIBRARY_MEM_NULL_NBLIBNULL_MNFEAT_FILECODE
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
 *   This function initializes member functions of HW Rx En Training.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

BOOLEAN
MemNInitDqsTrainRcvrEnHwNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function disables member functions of Hw Rx En Training.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

BOOLEAN
MemNDisableDqsTrainRcvrEnHwNb (
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
 *      This function calculates RcvEn seed value for each rank
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
MemNPrepareRcvrEnDlySeedNb (
  IN OUT   MEM_NB_BLOCK  *NBPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  Waits specified number of MEMCLKs
 *      @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *      @param[in] MemClkCount - Number of MEMCLKs
 *
 * ----------------------------------------------------------------------------
 */
VOID
MemNWaitXMemClksNb (
  IN OUT   MEM_NB_BLOCK  *NBPtr,
  IN       UINT32 MemClkCount
  )
{
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function uses the PRBS generator in the DCT to send a DDR Activate command
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]        ChipSelect - Chip select 0-7
 *     @param[in]        Bank - Bank Address 0-7
 *     @param[in]        RowAddress - Row Address [17:0]
 *
 */

VOID
MemNRrwActivateCmd (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 ChipSelect,
  IN       UINT8 Bank,
  IN       UINT32 RowAddress
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function uses the PRBS generator in the DCT to send a DDR Precharge
 *     or Precharge All command
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]        ChipSelect - Chip select 0-7
 *     @param[in]        Bank - Bank Address 0-7, PRECHARGE_ALL_BANKS = Precharge All
 *
 *
 */

VOID
MemNRrwPrechargeCmd (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 ChipSelect,
  IN       UINT8 Bank
  )
{
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function generates a continuous burst of reads for HW RcvEn
 *        training using the Unified Northbridge Reliable Read/Write Engine.
 *
 *     @param[in,out] NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]     Address - Unused by this function
 *
 */
VOID
MemNGenHwRcvEnReadsUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function generates a continuous stream of reads from DRAM using the
 *       Unified Northbridge Reliable Read/Write Engine.
 *
 *     @param[in,out] NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out] Buffer  - Unused by this function
 *     @param[in]     Address - Unused by this function
 *     @param[in]     ClCount - Number of cache lines to read
 *
 *     Assumptions:
 *
 *
 *
 */

VOID
MemNContReadPatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT32 Address,
  IN       UINT16 ClCount
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function generates a continuous stream of writes to DRAM using the
 *       Unified Northbridge Reliable Read/Write Engine.
 *
 *     @param[in,out] NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out] Address - Unused by this function
 *     @param[in]     Pattern - Unused by this function
 *     @param[in]     ClCount - Number of cache lines to write
 *
 */

VOID
MemNContWritePatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address,
  IN       UINT8 Pattern[],
  IN       UINT16 ClCount
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function checks the Error status bits for comparison results
 *
 *     @param[in,out]   *NBPtr    - Pointer to the MEM_NB_BLOCK
 *     @param[in]       Buffer[]  -  Not used in this implementation
 *     @param[in]       Pattern[] - Not used in this implementation
 *     @param[in]       ByteCount - Not used in this implementation
 *
 *     @return  PASS - Bitmap of results of comparison
 */

UINT16
MemNCompareTestPatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT16 ByteCount
  )
{
  return 0;
}

/*-----------------------------------------------------------------------------*/
/**
 *
 *       This function checks the Error status bits for offset comparison results
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       Buffer[]  -  Buffer data from DRAM (Measured data from DRAM) to compare
 *     @param[in]       Pattern[]  - Pattern (Expected data in ROM/CACHE) to compare against
 *     @param[in]       ByteCount - Byte count
 *
 *     @retval  Bitmap of results of comparison
 */
UINT16
MemNInsDlyCompareTestPatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
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
 *      This function assigns read/write function pointers to CPG read/write modules.
 *
 *     @param[in,out] NBPtr  - Pointer to the MEM_NB_BLOCK
 *
 */
BOOLEAN
MemNInitCPGUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function generates a continuous stream of writes infinite writes to DRAM using the
 *       Unified Northbridge Reliable Read/Write Engine.
 *
 *     @param[in,out] NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNEnableInfiniteWritePatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function disables the infinite stream of writes to DRAM using the
 *       Unified Northbridge Reliable Read/Write Engine.
 *
 *     @param[in,out] NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNDisableInfiniteWritePatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/*-----------------------------------------------------------------------------*/
/**
 *
 *       This function checks the 180 Error status bits for RD DQS 2D training
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       Buffer[]  -  Buffer data from DRAM (Measured data from DRAM) to compare
 *     @param[in]       Pattern[]  - Pattern (Expected data in ROM/CACHE) to compare against
 *     @param[in]       ByteCount - Byte count,
 *
 *     @retval  Bitmap of results of comparison
 */
UINT32
MemN180CompareRdDqs2DPatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
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
 *       This function checks the In Phase Error status bits for comparison results for RDDQS 2D training
 *
 *     @param[in,out]   *NBPtr    - Pointer to the MEM_NB_BLOCK
 *     @param[in]       Buffer[]  -  Not used in this implementation
 *     @param[in]       Pattern[] - Not used in this implementation
 *     @param[in]       ByteCount - Not used in this implementation
 *
 *     @return  PASS - Bitmap of results of comparison
 */

UINT32
MemNInPhaseCompareRdDqs2DPatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
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
 *       This function starts the Victim for 2D RdDqs Training Continuous Writes
 *
 *     @param[in,out]   *NBPtr    - Pointer to the MEM_NB_BLOCK
 *     @param[in]       SeedCount  -  Seed count
 */
VOID
MemNStartRdDqs2dVictimContinuousWritesUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 SeedCount
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function Initializes the Victim chipSelects for 2D RdDqs Training Continuous Writes
 *
 *     @param[in,out]   *NBPtr    - Pointer to the MEM_NB_BLOCK
 */
VOID
MemNInitializeRdDqs2dVictimChipSelContinuousWritesUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  NBPtr->SetBitField (NBPtr, BFTgtChipSelectA, NBPtr->TechPtr->ChipSel);
  NBPtr->SetBitField (NBPtr, BFTgtChipSelectB, NBPtr->TechPtr->ChipSel);
  NBPtr->SetBitField (NBPtr, BFResetAllErr, 1);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function finalizes the Victim for 2D RdDqs Training
 *
 *     @param[in,out]   *NBPtr    - Pointer to the MEM_NB_BLOCK
 */
VOID
MemNFinalizeRdDqs2dVictimContinuousWritesUnb  (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function Initializes the Victim for 2D RdDqs Training
 *
 *     @param[in,out]   *NBPtr    - Pointer to the MEM_NB_BLOCK
 */
VOID
MemNInitializeRdDqs2dVictimContinuousWritesUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function enables continuous writes on unused channels
 *     @param[in,out]   *NBPtr    - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *SeedCount -  seed index
 *
 *     @return    Prbs Seed
 *
 */

UINT32
MemNGetPrbs2dRdDqsSeedUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 SeedCount
  )
{
  return 0;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function enables/disables continuous writes on unused agressor channels
 *     @param[in,out]   *NBPtr    - Pointer to the MEM_NB_BLOCK
 *     @param[in]       SeedCount  -  Seed count
 *     @param[in]       TurnOnInfinite - TRUE - Enable Infinite Mode
 *                      TurnOnInfinite - FALSE - Disable Infinite Mode
 *
 */
VOID
MemNAgressorContinuousWritesUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 SeedCount,
  IN       BOOLEAN TurnOnInfinite
  )
{

}


