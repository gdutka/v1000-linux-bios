/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _AMD_RAS_SSP_SMM_DRIVER_H_
#define _AMD_RAS_SSP_SMM_DRIVER_H_

#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>

#include <Protocol/FchSmmSwDispatch2.h>

#include <Library/RasSspSmmLib.h>
#include "AmdRasRegistersSsp.h"
#include "AmdRas.h"

extern EFI_GUID gAmdRasInitDataProtocolGuid;

#endif  //_AMD_RAS_SSP_SMM_DRIVER_H_

