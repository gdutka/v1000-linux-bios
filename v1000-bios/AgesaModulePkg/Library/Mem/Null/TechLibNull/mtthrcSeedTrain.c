/* $NoKeywords:$ */
/**
 * @file
 *
 * mtthrcSt.c
 *
 * Phy assisted DQS receiver enable seedless training
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
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mttEdgeDetect.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE LIBRARY_MEM_NULL_TECHLIBNULL_MTTHRCSEEDTRAIN_FILECODE
/*----------------------------------------------------------------------------
3 *                          DEFINITIONS AND MACROS
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

VOID
STATIC
MemTRdPosRxEnSeedSetDly3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   UINT16 RcvEnDly,
  IN OUT   UINT8 ByteLane
  );

VOID
STATIC
MemTRdPosRxEnSeedCheckRxEndly3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/*-----------------------------------------------------------------------------
 *
 *
 *     This function checks each bytelane for no window error.
 *
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out]   OptParam   - Optional parameter
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemTTrackRxEnSeedlessRdWrNoWindBLError (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
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
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out]   OptParam   - Optional parameter(Unused)
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemTTrackRxEnSeedlessRdWrSmallWindBLError (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  )
{
  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function sets the RxEn delay
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out]   *RcvEnDly   - Receiver Enable Delay
 *     @param[in,out]   *ByteLane   - Bytelane
 *
*/
VOID
STATIC
MemTRdPosRxEnSeedSetDly3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   UINT16 RcvEnDly,
  IN OUT   UINT8 ByteLane
  )
{

}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function determines if the currert RxEn delay settings have failed
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
*/
VOID
STATIC
MemTRdPosRxEnSeedCheckRxEndly3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{

}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function executes RdDQS training and if fails adjusts the RxEn Gross results for
 *     each bytelane
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE - All bytelanes pass
 *     @return  FALSE - Some bytelanes fail
*/
BOOLEAN
MemTRdPosWithRxEnDlySeeds3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return TRUE;
}

