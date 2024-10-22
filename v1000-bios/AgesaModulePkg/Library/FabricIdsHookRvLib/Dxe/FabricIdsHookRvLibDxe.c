/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Fabric IDS Hook implementation
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Library
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include "Library/AmdBaseLib.h"
#include "Library/IdsLib.h"
#include "Library/AmdIdsHookLib.h"
#include "FabricRegistersRV.h"
#include "IdsHookId.h"
#include "IdsNvIdRV.h"
#include "IdsNvDefRV.h"
#include "Filecode.h"

#define FILECODE LIBRARY_FABRICIDSHOOKRVLIB_DXE_FABRICIDSHOOKRVLIBDXE_FILECODE

#ifndef IDS_HOOK_INTERNAL_SUPPORT
  #define FABRIC_RV_IDS_HOOKS_INT_DXE
#else
  #include "Internal/FabricIdsHookRvLibIntDxe.h"
#endif


IDS_HOOK_STATUS
FabricRvDxePiePwrMgmtInit (
  HOOK_ID HookId,
  VOID *Handle,
  VOID *Data
  )
{
  UINT64 IdsNvValue;
  CSTATE_CTRL_REGISTER *PCstateCtrl;
  IDS_HDT_CONSOLE (CPU_TRACE, "FabricRvDxePiePwrMgmtInit Option\n");

  //DF C-state control
  PCstateCtrl = (CSTATE_CTRL_REGISTER *) Data;
  IDS_NV_READ_SKIP (IDSNVID_DF_CMN_CSTATE_CTRL, &IdsNvValue) {
    switch (IdsNvValue) {
    case IDSOPT_DF_CMN_CSTATE_CTRL_DISABLED:
      PCstateCtrl->Field.DfCstateDisable = 1;
      break;
    case IDSOPT_DF_CMN_CSTATE_CTRL_ENABLED:
      PCstateCtrl->Field.DfCstateDisable = 0;
      break;
    case IDSOPT_DF_CMN_CSTATE_CTRL_AUTO:
      break;
    default:
      ASSERT (FALSE);
      break;
    }
  }
  return IDS_HOOK_SUCCESS;
}


IDS_HOOK_ELEMENT FabricRvIdsHooksDxe[] = {
  FABRIC_RV_IDS_HOOKS_INT_DXE
  {
    IDS_HOOK_PIE_PWR_MGMT_INIT,
    &FabricRvDxePiePwrMgmtInit,
  },
  IDS_HOOKS_END
};

IDS_HOOK_TABLE FabricRvIdsHookTableDxe = {
  IDS_HOOK_TABLE_HEADER_REV1_DATA,
  FabricRvIdsHooksDxe
};


AGESA_STATUS
GetIdsHookTable (
  IDS_HOOK_TABLE **IdsHookTable
  )
{
  *IdsHookTable = &FabricRvIdsHookTableDxe;
  return AGESA_SUCCESS;
}

