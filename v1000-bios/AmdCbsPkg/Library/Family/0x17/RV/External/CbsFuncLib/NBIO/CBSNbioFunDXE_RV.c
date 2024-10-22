/*****************************************************************************
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include "CBSNbioFunDXE_RV.h"

#ifdef AMD_CBS_NBIO_RV
DXE_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(NbioDxeFunTable,_RV)[] =
{
  {
    F17_RV_RAW_ID,
    AMD_RESERVE_ALL,
    CBS_FUNC_DECLARE(NbioInitEnvParams,_RV),
    IDS_HOOK_NBIO_INIT_ENV
  },
  {
    F17_RV2_RAW_ID,
    AMD_RESERVE_ALL,
    CBS_FUNC_DECLARE(NbioInitEnvParams,_RV),
    IDS_HOOK_NBIO_INIT_ENV
  },
  {
    F17_PIC_RAW_ID,
    AMD_RESERVE_ALL,
    CBS_FUNC_DECLARE(NbioInitEnvParams,_RV),
    IDS_HOOK_NBIO_INIT_ENV
  },
  {
    0,        // F17_RV_RAW_ID,
    0,        // AMD_RESERVE_ALL,
    NULL,     // CBS_FUNC_DECLARE(NbioFunc_Init_DXE,_RV),
    0         // CBS_CALLOUT_ID
  }
};

VOID
CBS_FUNC_DECLARE(NbioInitEnvParams,_RV) (
  IN EFI_BOOT_SERVICES        *BootServices,
  IN OUT  VOID                *DataParams,
  IN AMD_SYSTEM_CONFIGURATION *SystemConfiguration
  )
{

}

#endif

