/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH task launcher
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 309083 $   @e \$Date: 2014-12-09 09:28:24 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*/
#include "FchPlatform.h"
#define FILECODE PROC_FCH_TAISHAN_TSINTERFACE_TSFCHTASKLAUNCHER_FILECODE

AGESA_STATUS
FchTaskLauncher (
  IN       FCH_TASK_ENTRY     **TaskPtr,
  IN       VOID               *FchCfg,
  IN       AGESA_POST_CODE           TestPoint
  )
{
  AGESA_TESTPOINT (TestPoint, *(AMD_CONFIG_PARAMS **) FchCfg);
  while (*TaskPtr != NULL) {
    (*TaskPtr) (FchCfg);
    TaskPtr++;
  }
  return AGESA_SUCCESS;
}


