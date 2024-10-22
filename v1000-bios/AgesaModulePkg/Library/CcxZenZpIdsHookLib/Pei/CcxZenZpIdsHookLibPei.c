/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX Zen - Zeppelin API, and related functions.
 *
 * Contains code that initializes the core complex
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CCX
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/CcxRolesLib.h>
#include <Library/CcxDownCoreLib.h>
#include <IdsHookId.h>
#include "IdsNvIdZP.h"
#include "IdsNvDefZP.h"
#include "Filecode.h"

#define FILECODE LIBRARY_CCXZENZPIDSHOOKLIB_PEI_CCXZENZPIDSHOOKLIBPEI_FILECODE
/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

#ifndef IDS_HOOK_INTERNAL_SUPPORT
  #define CCX_IDS_HOOKS_INT
#else
  #include <Internal/CcxZenZpIdsHookLibIntPei.h>
#endif

IDS_NV_ID IdsNvIdCoreDisMask[] = {
  IDSNVID_CMN_CPU_GEN_DIE0_DOWNCORE_BIT_MAP,
  IDSNVID_CMN_CPU_GEN_DIE1_DOWNCORE_BIT_MAP,
  IDSNVID_CMN_CPU_GEN_DIE2_DOWNCORE_BIT_MAP,
  IDSNVID_CMN_CPU_GEN_DIE3_DOWNCORE_BIT_MAP
};

IDS_HOOK_STATUS
CcxZenZpCoreDisMask (
  HOOK_ID   HookId,
  VOID      *Handle,
  VOID      *Data
  )
{
  UINT8             SystemDieLoop;
  UINT64            IdsNvValue;
  CCX_CORE_DIS_MASK *DownCore;

  IDS_HDT_CONSOLE (CPU_TRACE, "CcxZenZpCoreDisMask External Options\n");
  DownCore = (CCX_CORE_DIS_MASK *) Data;

  for (SystemDieLoop = 0; SystemDieLoop < DownCore->SystemDieCount; SystemDieLoop++) {
    IDS_NV_READ_SKIP (IdsNvIdCoreDisMask[SystemDieLoop], &IdsNvValue) {
      if (IdsNvValue != 0) {
        DownCore->CoreDisMask[SystemDieLoop] = (UINT32) IdsNvValue;
      }
    }
  }
  return IDS_HOOK_SUCCESS;
}

IDS_HOOK_ELEMENT CcxZenZpIdsHooks[] = {
  {
    IDS_HOOK_CCX_CORE_DIS_MASK,
    &CcxZenZpCoreDisMask
  },
  CCX_IDS_HOOKS_INT
  IDS_HOOKS_END
};

IDS_HOOK_TABLE CcxZenZpIdsHookTable = {
  IDS_HOOK_TABLE_HEADER_REV1_DATA,
  CcxZenZpIdsHooks
};


AGESA_STATUS
GetIdsHookTable (
  IDS_HOOK_TABLE **IdsHookTable
  )
{
  *IdsHookTable = &CcxZenZpIdsHookTable;
  return AGESA_SUCCESS;
}

