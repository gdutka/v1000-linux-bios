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
#include "PiSmm.h"
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include "Library/UefiBootServicesTableLib.h"
#include "Library/SmmServicesTableLib.h"
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/CcxRolesLib.h>
#include "Protocol/FchSmmSwDispatch2.h"
#include "CcxZenRvIdsSyncMsrSmm.h"
#include <IdsHookId.h>
#include "IdsNvIdRV.h"
#include "IdsNvDefRV.h"
#include "Filecode.h"

#define FILECODE LIBRARY_CCXZENRVIDSHOOKLIB_SMM_CCXZENRVIDSHOOKLIBSMM_FILECODE
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
  #include <Internal/CcxZenRvIdsHookLibIntSmm.h>
#endif

IDS_HOOK_ELEMENT CcxZenRvIdsHooks[] = {
  CCX_IDS_HOOKS_INT
  IDS_HOOKS_END
};

IDS_HOOK_TABLE CcxZenRvIdsHookTable = {
  IDS_HOOK_TABLE_HEADER_REV1_DATA,
  CcxZenRvIdsHooks
};


AGESA_STATUS
GetIdsHookTable (
  IDS_HOOK_TABLE **IdsHookTable
  )
{
  *IdsHookTable = &CcxZenRvIdsHookTable;
  return AGESA_SUCCESS;
}
