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
#include <IdsNvIdZP.h>
#include <IdsNvDefZP.h>

#define FILECODE LIBRARY_NBIOIDSHOOKCZLIB_NBIOIDSHOOKCZLIB_FILECODE



IDS_HOOK_STATUS
NbioInitEarlyCZFeatureEnable (
  HOOK_ID   HookId,
  VOID      *Handle,
  VOID      *Data
  )
{
  UINT64            IdsNvValue;
  EFI_STATUS        Status;

  Status = EFI_SUCCESS;
  //System Configuration
  IDS_NV_READ_SKIP (IDSNVID_CMN_GNB_SMU_SYSTEM_CONFIG, &IdsNvValue) {
    switch (IdsNvValue) {
    case IDSOPT_CMN_GNB_SMU_SYSTEM_CONFIG_65WPORCONFIGURATION:
    case IDSOPT_CMN_GNB_SMU_SYSTEM_CONFIG_45WPORCONFIGURATION:
    case IDSOPT_CMN_GNB_SMU_SYSTEM_CONFIG_35WPORCONFIGURATION:
      PcdSet8 (PcdCfgSystemConfiguration, (UINT8) IdsNvValue);
      break;
    case IDSOPT_CMN_GNB_SMU_SYSTEM_CONFIG_AUTO:
      break;
    default:
      ASSERT (FALSE);
      break;
    }
  }
  return Status;
}

IDS_HOOK_ELEMENT NbioCZIdsHooks[] = {
  {
    IDS_HOOK_NBIO_INIT_EARLY,
    &NbioInitEarlyCZFeatureEnable
  },
  IDS_HOOKS_END
};

IDS_HOOK_TABLE NbioCZIdsHookTable = {
  IDS_HOOK_TABLE_HEADER_REV1_DATA,
  NbioCZIdsHooks
};


AGESA_STATUS
GetIdsHookTable (
  IDS_HOOK_TABLE **IdsHookTable
  )
{
  *IdsHookTable = &NbioCZIdsHookTable;
  return AGESA_SUCCESS;
}
