/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _CBS_HOOK_DXE_LIB_H_
#define _CBS_HOOK_DXE_LIB_H_

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>

#include <Protocol/AmdCbsHookProtocol.h>
#include <Guid/AmdCbsConfig.h>

EFI_STATUS
CbsHookFunc (
  IN       EFI_BOOT_SERVICES    *BootServices,
  IN OUT   VOID                 *AMD_PARAMS,
  IN       UINTN                IdsHookId
  );
#endif

