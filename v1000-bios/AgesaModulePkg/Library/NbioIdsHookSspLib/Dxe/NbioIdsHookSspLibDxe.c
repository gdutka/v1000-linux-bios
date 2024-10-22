/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include <Filecode.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/PcdLib.h>
#include <IdsHookId.h>
#include <IdsNvIdSSP.h>
#include <IdsNvDefSSP.h>

#include "smu11_bios_if.h"

#define FILECODE LIBRARY_NBIOIDSHOOKSSPLIB_DXE_NBIOIDSHOOKSSPLIBDXE_FILECODE

/**
 *
 * IDS HOOK for NBIO After CCX
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
CmnHookFuncNbioAfterCcx (
  HOOK_ID HookId,
  VOID *Handle,
  VOID *Data
  )
{
  return IDS_HOOK_SUCCESS;
}
/**
 *
 * IDS HOOK for NBIO PCIe AER initialization
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
CmnHookFuncNbioPcieAerInit (
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
  UINT64     IdsNvValue;
  UINT32     Value;
  PPTable_t  *PPTable;

  PPTable = (PPTable_t*)Data;

  ASSERT (Handle != NULL);

  // Common/External SMU Feature Flags
  Value = PcdGet32 (PcdSmuFeatureControlDefines);
  //DF Cstates
  IDS_NV_READ_SKIP (IDSNVID_CMN_GNB_SMU_DF_CSTATES, &IdsNvValue) {
    if (IdsNvValue != 0xf) {
      Value &= (UINT32) (~BIT19);
      if (IdsNvValue == 1) {
        Value |= BIT19;
      }
    }
  }
  PcdSet32 (PcdSmuFeatureControlDefines, Value);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcdSmuFeatureControlDefines(0x%08x)\n", Value);

  //Determinism Slider: If Auto set to enable (agesa PCD is 0 by default)
  IDS_NV_READ_SKIP (IDSNVID_CMN_DETERMINISM_SLIDER, &IdsNvValue) {
    switch (IdsNvValue) {
    case IDSOPT_CMN_DETERMINISM_SLIDER_POWER:
      PPTable->DeterminismPercentage = 0; // Power
      break;
    case IDSOPT_CMN_DETERMINISM_SLIDER_AUTO:
    case IDSOPT_CMN_DETERMINISM_SLIDER_PERFORMANCE:
      PPTable->DeterminismPercentage = 100; // Performance
      break;
    }
  }

  return IDS_HOOK_SUCCESS;
}

#ifndef IDS_HOOK_INTERNAL_SUPPORT
  #define NBIO_SSP_IDS_HOOKS_DXE_INT
#else
  #include <Internal/NbioIdsHookSspLibDxeInt.h>
#endif

IDS_HOOK_ELEMENT NbioSspIdsHookFuncs[] = {
  {
    IDS_HOOK_NBIO_AFTER_CCX,
    &CmnHookFuncNbioAfterCcx
  },
  {
    IDS_HOOK_NBIO_PCIE_AER_INIT,
    &CmnHookFuncNbioPcieAerInit
  },
  {
    IDS_HOOK_NBIO_SMU_INIT,
    &CmnHookFuncNbioSMUInit
  },
  NBIO_SSP_IDS_HOOKS_DXE_INT
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

