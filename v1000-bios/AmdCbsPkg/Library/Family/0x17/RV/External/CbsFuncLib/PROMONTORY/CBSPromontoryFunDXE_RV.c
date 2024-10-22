/*****************************************************************************
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include "CBSPromontoryFunDXE_RV.h"

#ifdef AMD_CBS_PROMONTORY_RV
DXE_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(PromontoryDxeFunTable,_RV)[] =
{
  {
    0,        // F17_RV_RAW_ID,
    0,        // AMD_RESERVE_ALL,
    NULL,     // CBS_FUNC_DECLARE(PromontoryFunc_Init_DXE,_RV),
    0         // CBS_CALLOUT_ID
  }
};
#endif

