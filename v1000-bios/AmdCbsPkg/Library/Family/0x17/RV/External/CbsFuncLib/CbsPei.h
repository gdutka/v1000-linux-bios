/*****************************************************************************
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _CBS_PEI_H_
#define _CBS_PEI_H_


#include <PiPei.h>
#include <Uefi/UefiSpec.h>

#include <Ppi/ReadOnlyVariable2.h>

#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/PeiServicesTablePointerLib.h>

#include "Cbs.h"
#include "AmdCbsConfig.h"
#include "AmdCbsVariable.h"

typedef struct _CBS_CONFIG AMD_SYSTEM_CONFIGURATION;

typedef VOID
PEI_CBS_FUNCTION(
    IN EFI_PEI_SERVICES           **PeiServices,
    IN OUT VOID                   *AMD_PARAMS,
    IN AMD_SYSTEM_CONFIGURATION   *Setup_Config
);

typedef PEI_CBS_FUNCTION *PPEI_CBS_FUNCTION;

#pragma pack(push,1)
typedef struct {
    IN    UINT64              Family;
    IN    UINT64              Revision;
    IN    PPEI_CBS_FUNCTION   CBSFuncPtr;
    IN    UINTN               IdsHookId;
} PEI_CBS_FUNCTION_STRUCT;

typedef struct _PEI_CBS_COMPONENT_STRUCT {
    IN    UINT64                  Family;
    IN    UINT64                  Revision;
    IN    PEI_CBS_FUNCTION_STRUCT *pFuncTable;
} PEI_CBS_COMPONENT_STRUCT;
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

