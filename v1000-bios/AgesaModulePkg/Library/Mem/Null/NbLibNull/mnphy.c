/* $NoKeywords:$ */
/**
 * @file
 *
 * mnphy.c
 *
 * Common Northbridge Phy support
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
#include "mport.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mu.h"
#include "PlatformMemoryConfiguration.h"
#include "Library/AmdHeapLib.h"
#include "merrhdl.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)
#define FILECODE LIBRARY_MEM_NULL_NBLIBNULL_MNPHY_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define UNUSED_CLK 4

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */
/// Type of an entry for processing phy init compensation for client NB
typedef struct {
  BIT_FIELD_NAME IndexBitField;          ///< Bit field on which the value is decided
  BIT_FIELD_NAME StartTargetBitField;    ///< First bit field to be modified
  BIT_FIELD_NAME EndTargetBitField;      ///< Last bit field to be modified
  UINT16 ExtraValue;                     ///< Extra value needed to be written to bit field
  CONST UINT16 (*TxPrePN)[3][5];         ///< Pointer to slew rate table
} PHY_COMP_INIT_CLIENTNB;

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
 *
 *   This function gets a delay value a PCI register during training
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   TrnDly - type of delay to be set
 *     @param[in]   DrbnVar - encoding of Dimm-Rank-Byte-Nibble to be accessed
 *                  (use either DIMM_BYTE_ACCESS(dimm,byte) or CS_NBBL_ACCESS(cs,nibble) to use this encoding
 *
 *     @return      Value read
 */

UINT32
MemNGetTrainDlyNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       TRN_DLY_TYPE TrnDly,
  IN       DRBN DrbnVar
  )
{
  return 0;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function sets a delay value a PCI register during training
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   TrnDly - type of delay to be set
 *     @param[in]   DrbnVar - encoding of Dimm-Rank-Byte-Nibble to be accessed
 *                  (use either DIMM_BYTE_ACCESS(dimm,byte) or CS_NBBL_ACCESS(cs,nibble) to use this encoding
 *     @param[in]   Field - Value to be programmed
 *
 */

VOID
MemNSetTrainDlyNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       TRN_DLY_TYPE TrnDly,
  IN       DRBN DrbnVar,
  IN       UINT16 Field
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function executes prototypical Phy fence training function.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNPhyFenceTrainingUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function executes Phy fence training
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNTrainPhyFenceNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/*-----------------------------------------------------------------------------*/
/**
 *
 *     Returns the parameters for a requested delay value to be used in training
 *     The correct Min, Max and Mask are determined based on the type of Delay,
 *     and the frequency
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in] TrnDly - Type of delay
 *     @param[in,out] *Parms - Pointer to the TRN_DLY-PARMS struct
 *
 */

VOID
MemNGetTrainDlyParmsUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       TRN_DLY_TYPE TrnDly,
  IN OUT   TRN_DLY_PARMS *Parms
  )
{

}
/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/
/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets or set DQS timing during training.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   TrnDly - type of delay to be set
 *     @param[in]   DrbnVar - encoding of Dimm-Rank-Byte-Nibble to be accessed
 *                  (use either DIMM_BYTE_ACCESS(dimm,byte) or CS_NBBL_ACCESS(cs,nibble) to use this encoding
 *     @param[in]   Field - Value to be programmed
 *     @param[in]   IsSet - Indicates if the function will set or get
 *
 *     @return      value read, if the function is used as a "get"
 */

UINT32
MemNcmnGetSetTrainDlyUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 IsSet,
  IN       TRN_DLY_TYPE TrnDly,
  IN       DRBN DrbnVar,
  IN       UINT16 Field
  )
{
  return 0;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function initializes the training pattern.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return      AGESA_STATUS - Result
 *                  AGESA_SUCCESS - Training pattern is ready to use
 *                  AGESA_ERROR   - Unable to initialize the pattern.
 */

AGESA_STATUS
MemNTrainingPatternInitNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return AGESA_SUCCESS;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function determined the settings for the Reliable Read/Write engine
 *       for each specific type of training
 *
 *     @param[in,out]   *NBPtr      - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *OptParam   - Pointer to an Enum of TRAINING_TYPE
 *
 *     @return      TRUE
 */

BOOLEAN
MemNSetupHwTrainingEngineUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       VOID *OptParam
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function finalizes the training pattern.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       Index    - Index of Write Data Delay Value
 *     @param[in,out]   *Value   - Write Data Delay Value
 *     @return      BOOLEAN - TRUE - Use the value returned.
 *                            FALSE - No more values in table.
 */

BOOLEAN
MemNGetApproximateWriteDatDelayNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Index,
  IN OUT   UINT8 *Value
  )
{
  return FALSE;
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function finalizes the training pattern.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return      AGESA_STATUS - Result
 *                  AGESA_SUCCESS - Training pattern has been finalized.
 *                  AGESA_ERROR   - Unable to initialize the pattern.
 */

AGESA_STATUS
MemNTrainingPatternFinalizeNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return AGESA_SUCCESS;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function returns the number of Chipselects controlled by each set
 *     of Delay registers under current conditions.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return
 */

UINT8
MemNCSPerDelayNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return 1;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function returns the minimum data eye width in 32nds of a UI for
 *     the type of data eye(Rd/Wr) that is being trained.  This value will
 *     be the minimum number of consecutive delays that yield valid data.
 *     Uses TechPtr->Direction to determine read or write.
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return
 */

UINT8
MemNMinDataEyeWidthNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function programs the phy registers according to the desired phy VDDIO voltage level
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNPhyVoltageLevelNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/*-----------------------------------------------------------------------------
 *
 *
 *     This function calculates the value of WrDqDqsEarly and programs it into
 *       the DCT and adds it to the WrDqsGrossDelay of each byte lane on each
 *       DIMM of the channel.
 *
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  OptParam   - Optional parameter
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNCalcWrDqDqsEarlyUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{

  return TRUE;
}

/*-----------------------------------------------------------------------------
 *
 *
 *     This function forces phy to M0 state
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *
 *     @return  none
 * ----------------------------------------------------------------------------
 */
VOID
MemNForcePhyToM0Unb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/*-----------------------------------------------------------------------------
 *
 *
 *     This function sets SkewMemClk before enabling MemClk
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  *OptParam   - Optional parameter
 *
 *     @return  TRUE - always
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNSetSkewMemClkUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function masks the RdDqsDly Bit 0 before writing to register for UNB.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *Offset -  Bit offset of the field to be programmed
 *
 *     @return    TRUE
 */
BOOLEAN
MemNAdjustRdDqsDlyOffsetUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Offset
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *     This function initializes RxEn Delays for RxEn seedless training
 *
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  OptParam   - Optional parameter
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNInitializeRxEnSeedlessTrainingUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  return TRUE;
}
/*-----------------------------------------------------------------------------
 *
 *
 *     This function checks each bytelane for no window error.
 *
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   OptParam   - Optional parameter
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNTrackRxEnSeedlessRdWrNoWindBLErrorUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  return TRUE;
}
/*-----------------------------------------------------------------------------
 *
 *
 *      This function checks each bytelane for small window error.
 *
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   OptParam   - Optional parameter
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNTrackRxEnSeedlessRdWrSmallWindBLErrorUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  return TRUE;
}
/*-----------------------------------------------------------------------------
 *
 *
 *      This function initializes a ByteLaneError error.
 *
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   OptParam   - Optional parameter
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNInitialzeRxEnSeedlessByteLaneErrorUnb (
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
 *     This function sets phy power saving related settings in different MPstate context.
 *
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *
 *     @return    none
 * ----------------------------------------------------------------------------
 */
VOID
MemNPhyPowerSavingMPstateUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *      This function adjusts the Phase Mask based on ECC.
 *
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   OptParam   - Optional parameter
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNAdjust2DPhaseMaskBasedOnEccUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function adjusts WrDqsBias before seed scaling
 *
 *     @param[in,out]   *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *WrDqsBias - Pointer to WrDqsBias
 *
 *     @return    FALSE - Supported
 *     @return    TRUE - Not supported
 */

BOOLEAN
MemNAdjustWrDqsBeforeSeedScalingUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *WrDqsBias
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function adjust RdDqsDly used for MaxRdLatency calculation
 *
 *     @param[in,out]   *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *OptParam  - Optional parameter
 *
 *     @return    FALSE - Supported
 *     @return    TRUE - Not supported
 */

BOOLEAN
MemNAdjustRdDqsDlyForMaxRdLatUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *     This function collects data for Eye Rim Search
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return  TRUE - No Errors occurred
 *     @return  FALSE - Errors occurred
 */
BOOLEAN
MemN2DRdDQSEyeRimSearchUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return TRUE;
}

/*-----------------------------------------------------------------------------
 *
 *
 *     Hook for some families which need an intermediate Mem Clk Frequency Valid
 *       before changing to the initial startup training frequency.
 *
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  *OptParam   - Optional parameter
 *
 *     @return  TRUE - always
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNIntermediateMemclkFreqValUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  return TRUE;
}
