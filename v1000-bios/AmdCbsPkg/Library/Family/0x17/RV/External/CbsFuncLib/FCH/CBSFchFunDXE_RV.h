/*****************************************************************************
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _CBS_FCH_ENTRY_DXE_RV_H_
#define _CBS_FCH_ENTRY_DXE_RV_H_

#include "CbsDxe.h"
#include  <Library/PcdLib.h>
#include "FchPlatform.h"

VOID
CBS_FUNC_DECLARE(FchInitEnvParams,_RV) (
  IN EFI_BOOT_SERVICES        *BootServices,
  IN OUT  VOID                *DataParams,
  IN AMD_SYSTEM_CONFIGURATION *SystemConfiguration
  );
#endif

