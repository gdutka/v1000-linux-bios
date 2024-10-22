/* $NoKeywords:$ */
/**
 * @file
 *
 * minit.c
 *
 * Initializer support function
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
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
#include "mport.h"
#include "mu.h"
#include "OptionMemory.h"
#include "Ids.h"
#include "merrhdl.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE LIBRARY_MEM_MAINLIB_MINIT_FILECODE
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

extern MEM_PLATFORM_CFG* memPlatformTypeInstalled[];

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *  This function initializes the default parameter, function pointers, build options
 *   and SPD data for memory configuration
 *
 *     @param[in,out]   *MemPtr   - Pointer to the MEM_DATA_STRUCT
 *     @param[in,out]   *PlatFormConfig -  Platform profile/build option config structure
 *
 */
VOID
AmdMemInitDataStructDef (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   PLATFORM_CONFIGURATION   *PlatFormConfig
  )
{
  UINT8 p;
  UINT8 i;
  // We need a way of specifying default values for each particular northbridge
  // family.  We also need to make sure that the IBV knows which parameter struct
  // is for which northbridge.
  //----------------------------------------------------------------------------
  AGESA_TESTPOINT (TpProcMemBeforeMemDataInit, &MemPtr->StdHeader);
  IDS_PERF_TIMESTAMP (TP_BEGINMEMBEFOREMEMDATAINIT, &MemPtr->StdHeader);

  MemPtr->PlatFormConfig = PlatFormConfig;

  //----------------------------------------------------------------------------
  // INITIALIZE PLATFORM SPECIFIC CONFIGURATION STRUCT
  //----------------------------------------------------------------------------
  AGESA_TESTPOINT (TpProcMemPlatformSpecificConfig, &MemPtr->StdHeader);
  i = 0;
  for (p = 0; p < MAX_PLATFORM_TYPES; p++) {
    if (((MEM_OPTION_INSTALL_STRUCT *)MemPtr->OptionMemoryInstallPtr)->memPlatformTypeInstalled[i] != NULL) {
      MemPtr->GetPlatformCfg[p] = ((MEM_OPTION_INSTALL_STRUCT *)MemPtr->OptionMemoryInstallPtr)->memPlatformTypeInstalled[i];
      i++;
    } else {
      MemPtr->GetPlatformCfg[p] = MemAGetPsCfgDef;
    }
  }

  AGESA_TESTPOINT (TpProcMemAfterMemDataInit, &MemPtr->StdHeader);
  IDS_PERF_TIMESTAMP (TP_ENDMEMBEFOREMEMDATAINIT, &MemPtr->StdHeader);

  MemPtr->ErrorHandling = MemErrHandle;
}

