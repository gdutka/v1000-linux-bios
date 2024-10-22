/*****************************************************************************
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include "CBSDfFunDXE_RV.h"

#ifdef AMD_CBS_DF_RV
DXE_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(DfDxeFunTable,_RV)[] =
{
  {
    0,        // F17_RV_RAW_ID,
    0,        // AMD_RESERVE_ALL,
    NULL,     // CBS_FUNC_DECLARE(DfFunc_Init_DXE,_RV),
    0         // CBS_CALLOUT_ID
  }
};
#endif

