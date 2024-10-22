/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include "CbsIDSCommonPei.h"
#include <IdsHookId.h>

PEI_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(IdsPeiFunTable,_Common)[] =
{
    {
        0xFFFFFFFF,  // Common function in combo case
        AMD_RESERVE_ALL,
        (PPEI_CBS_FUNCTION) CBS_FUNC_DECLARE(CbsInitIdsValue,_Common),
        IDS_HOOK_GET_NV_TABLE
    },
    {
        0,
        0,
        NULL,
        0
    }

};

void
CBS_FUNC_DECLARE(CbsInitIdsValue,_Common) (
  IN EFI_PEI_SERVICES                 **PeiServices,
  IN OUT    VOID                      *AMD_PARAMS,
  IN AMD_SYSTEM_CONFIGURATION         *SystemConfiguration
  )
{
  CBS_IDS_HOOK_DATA_GETIDSNV *IdsHookDataGetNv;

  IdsHookDataGetNv = ((CBS_IDS_HOOK_PARAMS *)AMD_PARAMS)->Data;
  PrepareIdsNvTable (SystemConfiguration, IdsHookDataGetNv->IdsNvTable, IdsHookDataGetNv->IdsNvTableSize);
}

