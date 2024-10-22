/*****************************************************************************
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _CBS_NBIO_ENTRY_DXE_RV_H_
#define _CBS_NBIO_ENTRY_DXE_RV_H_

#include "CbsDxe.h"

VOID
CBS_FUNC_DECLARE(NbioInitEnvParams,_RV) (
  IN EFI_BOOT_SERVICES        *BootServices,
  IN OUT  VOID                *DataParams,
  IN AMD_SYSTEM_CONFIGURATION *SystemConfiguration
  );

#endif

