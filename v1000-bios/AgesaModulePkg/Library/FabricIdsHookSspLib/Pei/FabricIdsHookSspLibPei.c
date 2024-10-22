/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Fabric IDS hook implementation
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
#include <FabricRegistersDf3.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <Library/FabricRegisterAccLib.h>
#include <IdsHookId.h>
#include <IdsNvIdSSP.h>
#include <IdsNvDefSSP.h>
#include <Filecode.h>

#define FILECODE LIBRARY_FABRICIDSHOOKSSPLIB_PEI_FABRICIDSHOOKSSPLIBPEI_FILECODE


/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

IDS_HOOK_STATUS
FabricSspPeiDfFreezeModuleQueuesOnError (
  HOOK_ID HookId,
  VOID    *Handle,
  VOID    *Data
  )
{
  UINT64                IdsNvValue;
  DF_GLBL_CTRL_REGISTER *PDfGlblCtrl;

  IDS_HDT_CONSOLE (CPU_TRACE, "FabricSspFreezeModuleQueuesOnError Option\n");

  // Freeze DF module queues on error
  // D18F0x3F8[3]
  PDfGlblCtrl = (DF_GLBL_CTRL_REGISTER *) Data;
  IDS_NV_READ_SKIP (IDSNVID_DF_CMN_FREEZE_QUEUE_ERROR, &IdsNvValue) {
    switch (IdsNvValue) {
    case IDSOPT_DF_CMN_FREEZE_QUEUE_ERROR_DISABLED:
      PDfGlblCtrl->Field.DisImmSyncFloodOnFatalErr = 1;
      break;
    case IDSOPT_DF_CMN_FREEZE_QUEUE_ERROR_ENABLED:
      break;
    case IDSOPT_DF_CMN_FREEZE_QUEUE_ERROR_AUTO:
      break;
    default:
      ASSERT (FALSE);
      break;
    }
  }
  return IDS_HOOK_SUCCESS;
}


// DRAM scrub time --- AGESA
// gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdFabricDramScrubTime

// Redirect scrubber control --- AGESA
// gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdFabricEccScrubRedirection

// Poison scrubber control --- AGESA
// gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdFabricEccScrubPoisoned

// Redirect scrubber limit --- AGESA
// gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdFabricEccScrubRedirectionLimit

// Disable DF sync flood propagation --- ABL
// APCB_TOKEN_UID_DF_SYNC_FLOOD_PROP

// GMI encryption control --- ABL
// APCB_TOKEN_UID_DF_GMI_ENCRYPT

// xGMI encryption control --- ABL
// APCB_TOKEN_UID_DF_XGMI_ENCRYPT

// CC6 memory region encryption --- ABL
// APCB_TOKEN_UID_DF_SAVE_RESTORE_MEM_ENCRYPT

// Location of private memory regions --- ABL
// APCB_TOKEN_UID_DF_SYS_STORAGE_AT_TOP_OF_MEM

// System probe filter --- ABL
// APCB_TOKEN_UID_DF_PROBE_FILTER

// Memory interleaving --- ABL
// APCB_TOKEN_UID_DF_MEM_INTERLEAVING

// Memory interleaving size --- ABL
// APCB_TOKEN_UID_DF_MEM_INTERLEAVING_SIZE

// Channel interleaving hash --- ABL
// APCB_TOKEN_UID_DF_CHAN_INTLV_HASH

// Memory Clear --- ABL
// APCB_TOKEN_UID_DF_MEM_CLEAR

// CAKE CRC perf bounds --- ABL
// APCB_TOKEN_UID_DF_CAKE_CRC_THRESH_PERF_BOUNDS

// GMI2 replay --- ABL
// APCB_TOKEN_UID_DF_GMI2_REPLAY

// ACPI SLIT Distance Control --- AGESA
// gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdFabricSlitDegree

// ACPI SLIT non-self distance --- AGESA
// gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdFabric2ndDegreeSlitDistance

// ACPI SLIT same socket distance --- AGESA
// gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdFabric3rdDegreeSlitLocalDistance

// ACPI SLIT remote socket distance --- AGESA
// gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdFabric3rdDegreeSlitRemoteDistance

// PSP error injection support --- AGESA
// gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdPspEinjSupport

#ifndef IDS_HOOK_INTERNAL_SUPPORT
  #define FABRIC_SSP_IDS_HOOKS_INT_PEI
#else
  #include "Internal/FabricIdsHookSspLibIntPei.h"
#endif

IDS_HOOK_ELEMENT FabricSspIdsHooksPei[] = {
  {
    IDS_HOOK_DF_RAS_INIT,
    &FabricSspPeiDfFreezeModuleQueuesOnError
  },
  FABRIC_SSP_IDS_HOOKS_INT_PEI
  IDS_HOOKS_END
};

IDS_HOOK_TABLE FabricSspIdsHookTablePei = {
  IDS_HOOK_TABLE_HEADER_REV1_DATA,
  FabricSspIdsHooksPei
};


AGESA_STATUS
GetIdsHookTable (
  IDS_HOOK_TABLE **IdsHookTable
  )
{
  *IdsHookTable = &FabricSspIdsHookTablePei;
  return AGESA_SUCCESS;
}

