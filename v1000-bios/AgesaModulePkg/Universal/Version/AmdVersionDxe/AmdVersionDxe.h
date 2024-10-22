/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _AMD_VERSION_DXE_H_
#define _AMD_VERSION_DXE_H_

//
// Functions Prototypes
//
EFI_STATUS
EFIAPI
AmdVersionDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  );

extern EFI_GUID gAmdVersionStringProtocolGuid;
#endif // _AMD_VERSION_DXE_H_

