/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/


#include "AmdCbsVariable.h"

VOID
CbsWriteDefalutValue (
  IN UINT8 *IfrData
  )
{
  CBS_CONFIG *pSetup_Config;

  pSetup_Config = (CBS_CONFIG *)IfrData;

  pSetup_Config->reserve = 1;
}

