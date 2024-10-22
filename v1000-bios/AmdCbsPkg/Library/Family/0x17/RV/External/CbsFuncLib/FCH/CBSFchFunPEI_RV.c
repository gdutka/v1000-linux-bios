/*****************************************************************************
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include "CBSFchFunPEI_RV.h"

#ifdef AMD_CBS_FCH_RV
PEI_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(FchPeiFunTable,_RV)[] =
{
  {
    0,        // F17_RV_RAW_ID,
    0,        // AMD_RESERVE_ALL,
    NULL,     // CBS_FUNC_DECLARE(FchFunc_Init_PEI,_RV),
    0         // CBS_CALLOUT_ID
  }

};
#endif

