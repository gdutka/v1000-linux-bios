/* $NoKeywords:$ */
/**
 * @file
 *
 * mttRdDqs2DEyeRimmSearch.c
 *
 * RD DQS 2 Dimentional Training using Eye Rim Sampling
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

#define FILECODE LIBRARY_MEM_NULL_TECHLIBNULL_MTTRDDQS2DEYERIMSEARCH_FILECODE

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
BOOLEAN
MemTInitializeEyeRimSearch (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

VOID
MemTEyeFill (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
CheckSaveResAtEdge (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN   UINT8 lane,
  IN   INT8 y,
  IN   INT8 x,
  IN   INT8 xdir
  );

UINT8
DetermineSavedState (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN   UINT8 lane,
  IN   INT8 y,
  IN   INT8 x
  );

UINT8
GetPassFailValue (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN   UINT8 lane,
  IN   INT8 y,
  IN   INT8 x
  );

VOID
SetPassFailValue (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN   UINT8 lane,
  IN   INT8 y,
  IN   INT8 x,
  IN   UINT8 result
  );

VOID
SetSavedState (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN   UINT8 lane,
  IN   INT8 y,
  IN   INT8 x,
  IN   UINT8 result
  );

INT8 MemTGet1DTrainedEyeCenter (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN UINT8 lane
  );

BOOLEAN
CheckForFail (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN   UINT8 lane,
  IN   INT8 y,
  IN   INT8 x
  );

BOOLEAN
AllocateSaveLaneStorage (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

INT8
xlateY (
  IN INT8 y
  );

BOOLEAN
ClearSampledPassResults (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

/* -----------------------------------------------------------------------------*/
/**
 *
 *    Initialize Eye Rim Search
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE
 */
BOOLEAN
MemTInitializeEyeRimSearch (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *     This function collects data for Eye Rim Search
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE - No Errors occurred
 *     @return  FALSE - Errors occurred
 */
BOOLEAN
MemT2DRdDQSEyeRimSearch (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return TRUE;
}


/* -----------------------------------------------------------------------------*/
/**
 *     Fill the data eye
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return
 */
VOID
MemTEyeFill (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{

}
/* -----------------------------------------------------------------------------*/
/**
 *
 *    Get the 1D trained center
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]  lane - current lane
 *
 *     @return
 */
INT8
MemTGet1DTrainedEyeCenter (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 lane
  )
{
  return 0;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    Determine if the saved value is at or close to the edge
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   *lane   - current lane
 *     @param[in]   y   -  vref value
 *     @param[in]   x   -  RdDqs value
 *     @param[in]   xdir   -  x-direction
 *
 *     @return  TRUE - close to edge, FALSE - not close to edge
 */
BOOLEAN
CheckSaveResAtEdge (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 lane,
  IN       INT8 y,
  IN       INT8 x,
  IN       INT8 xdir
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    determine if a BL has been saved
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   lane   - current lane
 *     @param[in]   y   -  vref value
 *     @param[in]   x   -  RdDqs value
 *
 *     @return  1 - value saved, 0 - value not saved
 */
UINT8
DetermineSavedState (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 lane,
  IN       INT8 y,
  IN       INT8 x
  )
{
  return 0;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *    Determine if a failure has occured
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   lane   - current lane
 *     @param[in]   y   -  vref value
 *     @param[in]   x   -  RdDqs value
 *
 *     @return  2 - Fail, 3 - Pass
 */
BOOLEAN
CheckForFail (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 lane,
  IN       INT8 y,
  IN       INT8 x
  )
{
  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *    Get pass fail state of lane
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   lane   - current lane
 *     @param[in]   y   -  vref value
 *     @param[in]   x   -  RdDqs value
 *
 *     @return  0 - Value not saved, 2 - Fail, 3 - Pass
 */
UINT8
GetPassFailValue (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 lane,
  IN       INT8 y,
  IN       INT8 x
  )
{
  return 0;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *    Set the Pass/Fail state of lane
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   *lane   - current lane
 *     @param[in]   y   -  vref value
 *     @param[in]   x   -  RdDqs value
 *     @param[in]   result   -  result value
 *
 *     @return  Saved Value
 */
VOID
SetPassFailValue (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 lane,
  IN       INT8 y,
  IN       INT8 x,
  IN       UINT8 result
  )
{

}
/* -----------------------------------------------------------------------------*/
/**
 *
 *    Set the save state of lane
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   *lane   - current lane
 *     @param[in]   y   -  vref value
 *     @param[in]   x   -  RdDqs value
 *     @param[in]   result   -  result value
 *
 *     @return  Saved Value
 */
VOID
SetSavedState (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 lane,
  IN       INT8 y,
  IN       INT8 x,
  IN       UINT8 result
  )
{

}
/* -----------------------------------------------------------------------------*/
/**
 *
 *    Allocate data storage
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  FALSE - No Errors occurred
 *     @return  TRUE - Errors occurred
 */
BOOLEAN
AllocateSaveLaneStorage (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    Translate Vref into a positive, linear value that can be used as an
 *      array index.
 *
 *     @param[in]   y   -  vref value
 *
 *     @return  Saved Value
 */
INT8
xlateY (
  IN INT8 y
  )
{
  return 0;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     Re-walking the eye rim for each aggressor combination, which invalidates previous Passes
 *     in the sample array.  Previous Fails in the sample array remain valid. Knowledge of previous Fails
 *     and speeds sampling for the subsequent walks, esp. when used in conjunction w/ ParallelSampling
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE
 */
BOOLEAN
ClearSampledPassResults (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return TRUE;
}

