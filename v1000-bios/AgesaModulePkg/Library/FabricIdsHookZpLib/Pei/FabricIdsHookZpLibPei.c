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
#include "FabricRegistersZP.h"
#include "Library/AmdBaseLib.h"
#include "Library/IdsLib.h"
#include "Library/AmdIdsHookLib.h"
#include "Library/BaseFabricTopologyLib.h"
#include "Library/FabricRegisterAccLib.h"
#include "IdsHookId.h"
#include "IdsNvIdZP.h"
#include "IdsNvDefZP.h"
#include "Filecode.h"

#define FILECODE LIBRARY_FABRICIDSHOOKZPLIB_PEI_FABRICIDSHOOKZPLIBPEI_FILECODE


/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

IDS_HOOK_STATUS
FabricZpPeiDfFreezeModuleQueuesOnError (
  HOOK_ID HookId,
  VOID    *Handle,
  VOID    *Data
  )
{
  UINT64                IdsNvValue;
  DF_GLBL_CTRL_REGISTER *PDfGlblCtrl;

  IDS_HDT_CONSOLE (CPU_TRACE, "FabricZpFreezeModuleQueuesOnError Option\n");

  //Disable DF sync flood propagation
  // D18F03F8[3]
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


IDS_HOOK_STATUS
FabricZpPeiDfRedirectScrubCtrl (
  HOOK_ID HookId,
  VOID    *Handle,
  VOID    *Data
  )
{
  UINT64                IdsNvValue;
  BOOLEAN               *PDfRedirScrubCtrl;

  IDS_HDT_CONSOLE (CPU_TRACE, "FabricZpPeiRedirectScrubCtrl Option\n");

  //Redirect scrubber control
  // D18F6x58[0]
  PDfRedirScrubCtrl = (BOOLEAN *) Data;
  IDS_NV_READ_SKIP (IDSNVID_DF_CMN_REDIR_SCRUB_CTRL, &IdsNvValue) {
    switch (IdsNvValue) {
    case IDSOPT_DF_CMN_REDIR_SCRUB_CTRL_DISABLED:
      *PDfRedirScrubCtrl = 0;
      break;
    case IDSOPT_DF_CMN_REDIR_SCRUB_CTRL_ENABLED:
      *PDfRedirScrubCtrl = 1;
      break;
    case IDSOPT_DF_CMN_REDIR_SCRUB_CTRL_AUTO:
      break;
    default:
      ASSERT (FALSE);
      break;
    }
  }
  return IDS_HOOK_SUCCESS;
}


IDS_HOOK_STATUS
FabricZpPeiDfDramScrubTime (
  HOOK_ID HookId,
  VOID    *Handle,
  VOID    *Data
  )
{
  UINT64                IdsNvValue;
  UINT8                 *PDfDramScrubTime;

  IDS_HDT_CONSOLE (CPU_TRACE, "FabricZpPeiDramScrubTime Option\n");

  //DRAM scrub time
  PDfDramScrubTime = (UINT8 *) Data;
  IDS_NV_READ_SKIP (IDSNVID_DF_CMN_DRAM_SCRUB_TIME, &IdsNvValue) {
    switch (IdsNvValue) {
    case IDSOPT_DF_CMN_DRAM_SCRUB_TIME_DISABLED:
      *PDfDramScrubTime = 0;
      break;
    case IDSOPT_DF_CMN_DRAM_SCRUB_TIME_1HOUR:
      *PDfDramScrubTime = 1;
      break;
    case IDSOPT_DF_CMN_DRAM_SCRUB_TIME_4HOURS:
      *PDfDramScrubTime = 4;
      break;
    case IDSOPT_DF_CMN_DRAM_SCRUB_TIME_8HOURS:
      *PDfDramScrubTime = 8;
      break;
    case IDSOPT_DF_CMN_DRAM_SCRUB_TIME_16HOURS:
      *PDfDramScrubTime = 16;
      break;
    case IDSOPT_DF_CMN_DRAM_SCRUB_TIME_24HOURS:
      *PDfDramScrubTime = 24;
      break;
    case IDSOPT_DF_CMN_DRAM_SCRUB_TIME_48HOURS:
      *PDfDramScrubTime = 48;
      break;
    case IDSOPT_DF_CMN_DRAM_SCRUB_TIME_AUTO:
      break;
    default:
      ASSERT (FALSE);
      break;
    }
  }
  return IDS_HOOK_SUCCESS;
}


#ifndef IDS_HOOK_INTERNAL_SUPPORT
  #define FABRIC_ZP_IDS_HOOKS_INT_PEI
#else
  #include "Internal/FabricIdsHookZpLibIntPei.h"
#endif

IDS_HOOK_ELEMENT FabricZpIdsHooksPei[] = {
  {
    IDS_HOOK_DF_RAS_INIT,
    &FabricZpPeiDfFreezeModuleQueuesOnError
  },
  {
    IDS_HOOK_PIE_REDIRECT_SCRUB_CTRL_INIT,
    &FabricZpPeiDfRedirectScrubCtrl
  },
  {
    IDS_HOOK_PIE_DRAM_SCRUB_TIME_INIT,
    &FabricZpPeiDfDramScrubTime
  },
  FABRIC_ZP_IDS_HOOKS_INT_PEI
  IDS_HOOKS_END
};

IDS_HOOK_TABLE FabricZpIdsHookTablePei = {
  IDS_HOOK_TABLE_HEADER_REV1_DATA,
  FabricZpIdsHooksPei
};


AGESA_STATUS
GetIdsHookTable (
  IDS_HOOK_TABLE **IdsHookTable
  )
{
  *IdsHookTable = &FabricZpIdsHookTablePei;
  return AGESA_SUCCESS;
}
