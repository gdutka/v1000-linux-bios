/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _CBS_DXE_H_
#define _CBS_DXE_H_

#include <Base.h>
#include <Uefi.h>

#include "Cbs.h"
#include "AmdCbsConfig.h"
#include "AmdCbsVariable.h"

typedef struct _CBS_CONFIG AMD_SYSTEM_CONFIGURATION;

typedef VOID
DXE_CBS_FUNCTION(
    IN EFI_BOOT_SERVICES          *BootServices,
    IN OUT VOID                   *AMD_PARAMS,
    IN AMD_SYSTEM_CONFIGURATION   *SystemConfiguration
);

typedef DXE_CBS_FUNCTION *PDXE_CBS_FUNCTION;

#pragma pack(push,1)
typedef struct {
    IN    UINT64              Family;
    IN    UINT64              Revision;
    IN    PDXE_CBS_FUNCTION   CBSFuncPtr;
    IN    UINTN               IdsHookId;
} DXE_CBS_FUNCTION_STRUCT;

typedef struct _DXE_CBS_COMPONENT_STRUCT {
    IN    UINT64              	  Family;
    IN    UINT64              	  Revision;
    IN    DXE_CBS_FUNCTION_STRUCT *pFuncTable;
} DXE_CBS_COMPONENT_STRUCT;
#pragma pack(pop)

typedef struct _CBS_IDS_HOOK_PARAMS {
  VOID *Handle;
  VOID *Data;
} CBS_IDS_HOOK_PARAMS;

//Todo: 
//extern VOID
//GetLogicalIdOfCurrentCore (
//     OUT   CPU_LOGICAL_ID *LogicalId,
//  IN       AMD_CONFIG_PARAMS *StdHeader
//  );

#endif

