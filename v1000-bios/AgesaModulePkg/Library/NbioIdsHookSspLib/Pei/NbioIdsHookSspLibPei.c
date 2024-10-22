/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include "PiPei.h"
#include <Filecode.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/PcdLib.h>
#include <IdsHookId.h>
#include <IdsNvIdSSP.h>
#include <IdsNvDefSSP.h>

#define FILECODE LIBRARY_NBIOIDSHOOKSSPLIB_PEI_NBIOIDSHOOKSSPLIBPEI_FILECODE

/**
 *
 * IDS HOOK for NBIO Base initialization
 *
 *
 *
 * @param[in] HookId          IDS HOOK ID
 * @param[in] Handle          Handle
 * @param[in] Data            Data
 * @retval    AGESA_STATUS
 *
 **/
IDS_HOOK_STATUS
CmnHookFuncNbioBaseInit (
  HOOK_ID HookId,
  VOID *Handle,
  VOID *Data
  )
{
  return IDS_HOOK_SUCCESS;
}

/**
 *
 * IDS HOOK for NBIO PCIe Topology initialization
 *
 *
 *
 * @param[in] HookId          IDS HOOK ID
 * @param[in] Handle          Handle
 * @param[in] Data            Data
 * @retval    AGESA_STATUS
 *
 **/
IDS_HOOK_STATUS
CmnHookFuncNbioPcieTopology (
  HOOK_ID HookId,
  VOID *Handle,
  VOID *Data
  )
{
  return IDS_HOOK_SUCCESS;
}
/**
 *
 * IDS HOOK for NBIO PCIe User Config initialization
 *
 *
 *
 * @param[in] HookId          IDS HOOK ID
 * @param[in] Handle          Handle
 * @param[in] Data            Data
 * @retval    AGESA_STATUS
 *
 **/
IDS_HOOK_STATUS
CmnHookFuncNbioPcieUserConfig (
  HOOK_ID HookId,
  VOID *Handle,
  VOID *Data
  )
{
  return IDS_HOOK_SUCCESS;
}

/**
 *
 * IDS HOOK for SMU initialization
 *
 *
 *
 * @param[in] HookId          IDS HOOK ID
 * @param[in] Handle          Handle
 * @param[in] Data            Data
 * @retval    AGESA_STATUS
 *
 **/
IDS_HOOK_STATUS
CmnHookFuncNbioSMUInit (
  HOOK_ID   HookId,
  VOID      *Handle,
  VOID      *Data
  )
{
  return IDS_HOOK_SUCCESS;
}

#ifndef IDS_HOOK_INTERNAL_SUPPORT
  #define NBIO_SSP_IDS_HOOKS_PEI_INT
#else
  #include <Internal/NbioIdsHookSspLibPeiInt.h>
#endif

IDS_HOOK_ELEMENT NbioSspIdsHookFuncs[] = {
  {
    IDS_HOOK_NBIO_BASE_INIT,
    &CmnHookFuncNbioBaseInit
  },
  {
    IDS_HOOK_NBIO_PCIE_TOPOLOGY,
    &CmnHookFuncNbioPcieTopology
  },
  {
    IDS_HOOK_NBIO_PCIE_USER_CONFIG,
    &CmnHookFuncNbioPcieUserConfig
  },
  {
    IDS_HOOK_NBIO_SMU_INIT,
    &CmnHookFuncNbioSMUInit
  },
  NBIO_SSP_IDS_HOOKS_PEI_INT
  IDS_HOOKS_END
};

IDS_HOOK_TABLE NbioSspIdsHookTable = {
  IDS_HOOK_TABLE_HEADER_REV1_DATA,
  NbioSspIdsHookFuncs
};


AGESA_STATUS
GetIdsHookTable (
  IDS_HOOK_TABLE **IdsHookTable
  )
{
  *IdsHookTable = &NbioSspIdsHookTable;
  return AGESA_SUCCESS;
}

