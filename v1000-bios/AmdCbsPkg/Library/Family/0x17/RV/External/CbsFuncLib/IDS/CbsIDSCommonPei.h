/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _CBS_IDS_INIT_H_
#define _CBS_IDS_INIT_H_

#include "Cbs.h"
#include "AmdCbsVariable.h"
#include "CbsPei.h"

#define SIZE_STRUCT(type,field) sizeof(((type*)0)->field)
#define OFFSET_STRUCT(type,field) ((UINT32)(&((type*)0)->field))

typedef struct _CBS_IDS_HOOK_DATA_GETIDSNV {
  VOID *IdsNvTable;
  UINT32 *IdsNvTableSize;
} CBS_IDS_HOOK_DATA_GETIDSNV;

VOID
CBS_FUNC_DECLARE(CbsInitIdsValue,_Common) (
  IN EFI_PEI_SERVICES                 **PeiServices,
  IN OUT VOID                         *AMD_PARAMS,
  IN AMD_SYSTEM_CONFIGURATION         *SystemConfiguration
  );

extern BOOLEAN
PrepareIdsNvTable (
  IN       VOID *CbsVariable,
  IN OUT   VOID *IdsNvTable,
  IN OUT   UINT32 *IdsNvTableSize
  );

#endif //_CBS_IDS_INIT_H_

