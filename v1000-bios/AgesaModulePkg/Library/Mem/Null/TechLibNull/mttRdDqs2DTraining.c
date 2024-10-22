/* $NoKeywords:$ */
/**
 * @file
 *
 * mttRdDqs2DTraining.c
 *
 * RD DQS 2 dimentional training
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech)
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
#include "Library/AmdHeapLib.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "mt.h"
#include "mport.h"
#include "merrhdl.h"
#include "Filecode.h"
#include "OptionMemory.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE LIBRARY_MEM_NULL_TECHLIBNULL_MTTRDDQS2DTRAINING_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define MAX_DELAYS    9   /* 8 data bytes + 1 ECC byte */
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
BOOLEAN
MemT2DRdDQSProcessConvolution (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       MEM_2D_ENTRY *Data
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
  extern MEM_PSC_FLOW_BLOCK* memPlatSpecFlowArray[];

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function executes 2D training for Rd DQS
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE - No Errors occurred
 *     @return  FALSE - Errors occurred
 */

BOOLEAN
MemTAmdRdDqs2DTraining (
  IN OUT   MEM_TECH_BLOCK *TechPtr
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
 *     This function determines the maximum number of lanes to program 2D RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE - Configuration valid
 *              FALSE - Configuration invalid
 */
BOOLEAN
MemTCheck2DTrainingPerConfig (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return FALSE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function determines the maximum number of lanes to program 2D RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE - Max Number of Lanes
 */
UINT8
MemT2DGetMaxLanes (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return 0;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function programs Vref to internal or external control for 2D RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */
VOID
MemT2DProgramIntExtVrefSelect (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{

}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function programs Vref for 2D RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       Vref - Vref value
 *
 */
VOID
MemT2DProgramVref (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Vref
  )
{

}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function programs RdDQS for 2D RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       RdDQSDly -  RdDqs value
 *
 */
VOID
MemT2DPrograRdDQSDly (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 RdDQSDly
  )
{

}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function stores data for 2D RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   *Data   - Pointer to Result data structure
 *     @param[in]   *InPhaseResult[]   - Array of inphase results
 *     @param[in]   *PhaseResult180[]   - Array of Phase 180 results
 *
 *     @return  TRUE - No Errors occurred
 */
VOID
StoreResult (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       MEM_2D_ENTRY *Data,
  IN       UINT32 InPhaseResult[],
  IN       UINT32 PhaseResult180[]
  )
{

}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function determines the height of data for 2D RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   *Data   - Pointer to Result data structure
 *
 *     @return  TRUE - No Errors occurred
 *     @return  FALSE - Errors occurred
 */
BOOLEAN
MemT2DRdDQSHeight (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       MEM_2D_ENTRY *Data
  )
{
  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function gets the width for 2D RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   *Data   - Pointer to Result data structure
 *
 *     @return  width
 */
UINT8
MemGet2dRdDQSWidth (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       MEM_2D_ENTRY *Data
  )
{
  return 0;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function gets the step height for the dimond mask for 2D RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   *Data   - Pointer to Result data structure
 *     @param[in]   Vref   - current Vref value
 *     @param[in]   Lane   - current Lane
 *
 *     @return  TRUE - Step found and value should be updated
 *              FALSE - Step not found and value should not be updated
 */
BOOLEAN
MemCheck2dRdDQSDiamondMaskStep (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       MEM_2D_ENTRY *Data,
  IN       UINT8 Vref,
  IN       UINT8 Lane
  )
{
  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function applies a mask fo 2D RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   *Data   - Pointer to Result data structure
 *
 *     @return  TRUE - No Errors occurred
 *     @return  FALSE - Errors occurred
 */
BOOLEAN
MemT2DRdDQSApplyMask (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       MEM_2D_ENTRY *Data
  )
{
  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function Examines the convolution function and determines the Max RDqs for
 *     2D RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   *Data   - Pointer to Result data structure
 *
 *     @return  TRUE - No Errors occurred
 *     @return  FALSE - Errors occurred
 */

BOOLEAN
MemT2DRdDQSProcessConvolution (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       MEM_2D_ENTRY *Data
  )
{
  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function programs the Max RDqs for 2D RdDQS training from convolution
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   *Data   - Pointer to Result data structure
 *
 *     @return  TRUE - No Errors occurred
 *     @return  FALSE - Errors occurred
 */
BOOLEAN
MemT2DRdDQSPrograMaxRdDQSDly (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       MEM_2D_ENTRY *Data
  )
{
  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function finds the Positive and negative Vref Margin for the current CS
 *     for 2D RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   *Data   - Pointer to Result data structure
 *
 *     @return  TRUE - No Errors occurred
 *     @return  FALSE - Errors occurred
 */
BOOLEAN
MemT2DRdDQSFindCsVrefMargin (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       MEM_2D_ENTRY *Data
  )
{
  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function finds the final Vref Margin for 2D RdDQS training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   *Data   - Pointer to Result data structure
 *
 *     @return  TRUE - No Errors occurred
 *     @return  FALSE - Errors occurred
 */
BOOLEAN
MemT2DRdDQSFinalVrefMargin (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       MEM_2D_ENTRY *Data
  )
{
  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function displays ther results of the 2D search
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   *Data   - Pointer to Result data structure
 *
 *     @return
 */
VOID
MemT2DRdDqsDisplaySearch (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       MEM_2D_ENTRY *Data
  )
{

}

