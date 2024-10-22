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
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/PeiServicesLib.h>

#include <Ppi/AmdCbsHookPpi.h>
#include <Guid/AmdCbsConfig.h>

EFI_STATUS
CbsHookFunc (
  IN       EFI_PEI_SERVICES     **PeiServices,
  IN OUT   VOID                 *AMD_PARAMS,
  IN       UINTN                IdsHookId
  );
#endif

