/* $NoKeywords:$ */
/**
 * @file
 *
 * mmConditionalPso.c
 *
 * Functions to support conditional entries in the Platform Specific Override Table
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
#include "OptionMemory.h"
#include "Library/AmdBaseLib.h"
#include "Library/IdsLib.h"
#include "PlatformMemoryConfiguration.h"
#include "Ids.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE LIBRARY_MEM_NULL_MAINLIBNULL_MMCONDITIONALPSO_FILECODE
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

#define PSO_TYPE    0
#define PSO_LENGTH  1
#define PSO_DATA    2

typedef enum _PSO_STATE {
  PSO_FIND_CONDITION = 100,  // Searching for initial Condition statement
  PSO_FIND_ACTION,           // Searching for initial Action Statement
  PSO_MATCH_ACTION,          // Trying to find an action that matches the caller's request
  PSO_CHECK_CONDITION,       // Checking the condition that preceded the found action
  PSO_DO_ACTION,             // Performing Action
  PSO_COMPLETE               // Completed processing of this request
} PSO_STATE;

typedef struct _D3_CMP_CAL {
  UINT32 D3Cmp0NCal   :3;
  UINT32 Reserved34   :2;
  UINT32 D3Cmp0PCal   :3;
  UINT32 Reserved89   :2;
  UINT32 D3Cmp1NCal   :3;
  UINT32 Reserved1314 :2;
  UINT32 D3Cmp1PCal   :3;
  UINT32 Reserved1819 :2;
  UINT32 D3Cmp2NCal   :3;
  UINT32 Reserved2324 :2;
  UINT32 D3Cmp2PCal   :3;
  UINT32 Reserved2831 :2;
} D3_CMP_CAL;

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
BOOLEAN
 STATIC
 MemPSODoActionODT (
   IN OUT   MEM_NB_BLOCK *NBPtr,
   IN       UINT8 *Buffer
   );

 BOOLEAN
 STATIC
 MemPSODoActionAddrTmg (
   IN OUT   MEM_NB_BLOCK *NBPtr,
   IN       UINT8 *Buffer
   );

 BOOLEAN
 STATIC
 MemPSODoActionODCControl (
   IN OUT   MEM_NB_BLOCK *NBPtr,
   IN       UINT8 *Buffer
   );

 BOOLEAN
 STATIC
 MemPSODoActionSlewRate (
   IN OUT   MEM_NB_BLOCK *NBPtr,
   IN       UINT8 *Buffer
   );

BOOLEAN
STATIC
MemPSODoActionGetFreqLimit (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  );

BOOLEAN
STATIC
MemCheckRankType (
  IN       CH_DEF_STRUCT *CurrentChannel,
  IN       UINT16 RankType
  );
/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */


/* -----------------------------------------------------------------------------*/
/**
 *
 *      Process Conditional Platform Specific Overrides
 *
 *      @param[in] PlatformMemoryConfiguration - Pointer to Platform config table
 *      @param[in] NBPtr - Pointer to Current NBBlock
 *      @param[in] PsoAction - Action type
 *      @param[in] Dimm - Dimm Number
 *
 *      @return          BOOLEAN -            TRUE : Action was performed
 *                                            FALSE: Action was not performed
 *
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemProcessConditionalOverrides (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 PsoAction,
  IN       UINT8 Dimm
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
 *      Perform ODT Platform Override
 *
 *      @param[in] NBPtr - Pointer to Current NBBlock
 *      @param[in] Buffer - Pointer to the Action Command Data (w/o Type and Len)
 *
 *      @return         BOOLEAN -             TRUE : Action was performed
 *                                            FALSE: Action was not performed
 *
 * ----------------------------------------------------------------------------
 */
BOOLEAN
STATIC
MemPSODoActionODT (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  )
{
  return TRUE;
 }

 /* -----------------------------------------------------------------------------*/
/**
 *      Perform Address Timing Platform Override
 *
 *      @param[in] NBPtr - Pointer to Current NBBlock
 *      @param[in] Buffer - Pointer to the Action Command Data (w/o Type and Len)
 *
 *      @return          BOOLEAN -             TRUE : Action was performed
 *                                             FALSE: Action was not performed
 *
 * ----------------------------------------------------------------------------
 */
BOOLEAN
STATIC
MemPSODoActionAddrTmg (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  )
{
  return TRUE;
 }

  /* -----------------------------------------------------------------------------*/
/**
 *      Perform Drive Strength Platform Override
 *
 *      @param[in] NBPtr - Pointer to Current NBBlock
 *      @param[in] Buffer - Pointer to the Action Command Data (w/o Type and Len)
 *
 *      @return          BOOLEAN -             TRUE : Action was performed
 *                                             FALSE: Action was not performed
 *
 * ----------------------------------------------------------------------------
 */
BOOLEAN
STATIC
MemPSODoActionODCControl (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  )
{
  return TRUE;
 }

  /* -----------------------------------------------------------------------------*/
/**
 *      Perform Slew Rate Platform Override
 *
 *      @param[in] NBPtr - Pointer to Current NBBlock
 *      @param[in] Buffer - Pointer to the Action Command Data (w/o Type and Len)
 *
 *     @return          BOOLEAN -             TRUE : Action was performed
 *                                            FALSE: Action was not performed
 *
 * ----------------------------------------------------------------------------
 */
BOOLEAN
STATIC
MemPSODoActionSlewRate (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    This function overrides the  POR supported speed for a specific config
 *
 *      @param[in] NBPtr - Pointer to Current NBBlock
 *      @param[in] Buffer - Pointer to the Action Command Data (w/o Type and Len)
 *
 *      @return          BOOLEAN -            TRUE : Action was performed
 *                                            FALSE: Action was not performed
 *
 */
BOOLEAN
STATIC
MemPSODoActionGetFreqLimit (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  )
{
  return TRUE;
}

 /* -----------------------------------------------------------------------------*/
/**
 *
 *    This function matches a particular Rank Type Mask to the installed
 *    DIMM configuration on the provided channel.
 *
 *     @param[in]       *CurrentChannel       Pointer to CH_DEF_STRUCT
 *     @param[in]       RankType              Mask of rank type to match
 *
 *     @return          BOOLEAN -             TRUE : Rank types match
 *                                            FALSE: Rank types do not match
 *
 */
BOOLEAN
STATIC
MemCheckRankType (
  IN       CH_DEF_STRUCT *CurrentChannel,
  IN       UINT16 RankType
  )
{
  return TRUE;
}
