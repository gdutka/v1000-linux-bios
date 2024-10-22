/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _CBS_HOOK_DXE_LIB_H_
#define _CBS_HOOK_DXE_LIB_H_

#include <PiPei.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/DebugLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/AmdBaseLib.h>

#include "AmdCbsHookPpi.h"
#include "AmdCbsConfig.h"
#include "AmdCbsVariable.h"
#include "CbsPei.h"

extern PEI_CBS_COMPONENT_STRUCT CbsCommonFuncTable[];

extern VOID
CbsWriteDefalutValue (
  IN UINT8 *IfrData
  );

extern VOID
CbsSetAgesaPcds (
  IN       VOID *CbsVariable
  );

EFI_STATUS
CbsHookFunc (
  IN       EFI_PEI_SERVICES     **PeiServices,
  IN OUT   VOID                 *AMD_PARAMS,
  IN       UINTN                IdsHookId
  );
#endif

