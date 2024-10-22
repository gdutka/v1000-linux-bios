/*****************************************************************************
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _CBS_NBIO_ENTRY_PEI_RV_H_
#define _CBS_NBIO_ENTRY_PEI_RV_H_

#include "CbsPei.h"
#include  <Library/PcdLib.h>

VOID
CBS_FUNC_DECLARE(NbioInitEarlyParams,_RV) (
    IN      EFI_PEI_SERVICES    **PeiServices,
    IN OUT  VOID                *AmdGnbCbsInterface,
    IN AMD_SYSTEM_CONFIGURATION *SystemConfiguration
  );

#endif

