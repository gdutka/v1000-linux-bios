/* $NoKeywords:$ */
/**
 * @file
 *
 * mfDMID34.c
 *
 * Memory DMI table support for Combo DDR3 and DDR4.
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
#include "Ids.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "mtspd4.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEM_FEATDMICOLLECTLIB_AMDMEMDMID34LIB_FILECODE

extern OPTION_MEM_FEATURE_MAIN MemFDMISupport3;
extern OPTION_MEM_FEATURE_MAIN MemFDMISupport4;

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
 *
 *  This function gets Calls the appropriate Technology Specific DMI support
 *  function
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 */
BOOLEAN
MemFDMISupport34 (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  if (MemMainPtr->NBPtr->ChannelPtr->TechType == DDR4_TECHNOLOGY) {
    return MemFDMISupport4 (MemMainPtr);
  } else {
    return MemFDMISupport3 (MemMainPtr);
  }
}



