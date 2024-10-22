/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _DXE_SOC_LOGICAL_ID_SERVICES_LIB_H_
#define _DXE_SOC_LOGICAL_ID_SERVICES_LIB_H_

EFI_STATUS
EFIAPI
SocLogicalIdServiceProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  );

#endif

