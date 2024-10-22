/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include <Uefi.h>
#include <Pi/PiMultiPhase.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/AmdIdsHookExtLib.h>
#include "AmdCbsHookProtocol.h"

typedef struct _CBS_IDS_HOOK_PARAMS {
  VOID *Handle;
  VOID *Data;
} CBS_IDS_HOOK_PARAMS;

typedef struct _CBS_IDS_HOOK_DATA_GETIDSNV {
  VOID *IdsNvTable;
  UINT32 *IdsNvTableSize;
} CBS_IDS_HOOK_DATA_GETIDSNV;

IDS_HOOK_STATUS
IdsHookExtEntry (
  HOOK_ID HookId,
  VOID *Handle,
  VOID *Data
  )
{
  CBS_IDS_HOOK_PARAMS       CbsIdsHookParams;
  EFI_STATUS                Status;
  CBS_DXE_PROTOCOL          *pCbsProtocol;

  CbsIdsHookParams.Handle = Handle;
  CbsIdsHookParams.Data = Data;
  Status = gBS->LocateProtocol (&gCbsProtocolGuid, NULL, (VOID**)&pCbsProtocol);
  if (!EFI_ERROR(Status)) {
    Status = pCbsProtocol->CbsHookInterface(gBS, &CbsIdsHookParams, HookId);
  }	

  return (EFI_ERROR (Status) ? IDS_HOOK_ERROR : IDS_HOOK_SUCCESS);
}

IDS_HOOK_STATUS
GetIdsNvTable (
  IN OUT   VOID *IdsNvTable,
  IN OUT   UINT32 *IdsNvTableSize
  )
{
  CBS_IDS_HOOK_DATA_GETIDSNV IdsHookData;
  //Use the special reserved HOOK to get the IDS NV table
  IdsHookData.IdsNvTable = IdsNvTable;
  IdsHookData.IdsNvTableSize = IdsNvTableSize;

  return IdsHookExtEntry (IDS_HOOK_GET_NV_TABLE, NULL, &IdsHookData);
}

