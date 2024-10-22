/*****************************************************************************
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include "CBSNbioFunPEI_RV.h"

#ifdef AMD_CBS_NBIO_RV
PEI_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(NbioPeiFunTable,_RV)[] =
{
  {
    F17_RV_RAW_ID,
    AMD_RESERVE_ALL,
    CBS_FUNC_DECLARE(NbioInitEarlyParams,_RV),
    IDS_HOOK_NBIO_INIT_EARLY
  },
  {
    F17_RV2_RAW_ID,
    AMD_RESERVE_ALL,
    CBS_FUNC_DECLARE(NbioInitEarlyParams,_RV),
    IDS_HOOK_NBIO_INIT_EARLY
  },
  {
    F17_PIC_RAW_ID,
    AMD_RESERVE_ALL,
    CBS_FUNC_DECLARE(NbioInitEarlyParams,_RV),
    IDS_HOOK_NBIO_INIT_EARLY
  },
  {
    0,        // F17_RV_RAW_ID,
    0,        // AMD_RESERVE_ALL,
    NULL,     // CBS_FUNC_DECLARE(NbioFunc_Init_PEI,_RV),
    0         // CBS_CALLOUT_ID
  }

};

VOID
CBS_FUNC_DECLARE(NbioInitEarlyParams,_RV) (
    IN      EFI_PEI_SERVICES    **PeiServices,
    IN OUT  VOID                *AmdGnbCbsInterface,
    IN AMD_SYSTEM_CONFIGURATION *SystemConfiguration
  )
{
}

#endif

