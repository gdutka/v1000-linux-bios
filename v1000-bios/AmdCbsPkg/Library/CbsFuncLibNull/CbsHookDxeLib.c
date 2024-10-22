/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include "CbsHookDxeLib.h"

extern EFI_RUNTIME_SERVICES *gRT;

static CBS_DXE_PROTOCOL mCbsProtocolInstance=
{
  CbsHookFunc,
  0
};

EFI_STATUS
CbsHookFunc (
  IN       EFI_BOOT_SERVICES    *BootServices,
  IN OUT   VOID                 *AMD_PARAMS,
  IN       UINTN                IdsHookId
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS CbsInitEntry(
  IN  EFI_BOOT_SERVICES *BootServices
  )
{
  EFI_STATUS       Status;
  EFI_HANDLE       Handle;
  CBS_DXE_PROTOCOL *pCbsProtocol;

  Status = BootServices->LocateProtocol (&gCbsProtocolGuid, NULL, &pCbsProtocol);
  if (EFI_ERROR(Status)) {
    Handle = NULL;
    Status = BootServices->InstallProtocolInterface (&Handle,
                                        &gCbsProtocolGuid,
                                        EFI_NATIVE_INTERFACE,
                                        &mCbsProtocolInstance);
  }
  return Status;
}

EFI_STATUS
CbsInterfaceFunc (
  IN  EFI_BOOT_SERVICES      *BootServices,
  IN  OUT VOID               *AMD_PARAMS,
  IN      UINTN              IdsHookId
  )
{
  return EFI_SUCCESS;
}

BOOLEAN
CbsFamilyIdentify (
  )
{
  return TRUE;
}

