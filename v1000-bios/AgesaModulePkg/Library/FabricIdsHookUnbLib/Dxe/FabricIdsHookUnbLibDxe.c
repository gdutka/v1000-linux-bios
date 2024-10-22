/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD UNB XV - Excavator API, and related functions.
 *
 * Contains code that initializes the UNB
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
#include "PiDxe.h"
#include "Library/AmdBaseLib.h"
#include "Library/IdsLib.h"
#include "Library/AmdIdsHookLib.h"
#include "IdsHookId.h"

#ifndef IDS_HOOK_INTERNAL_SUPPORT
  #define FABRIC_UNB_IDS_HOOKS_INT_DXE
#else
  #include "Internal/FabricIdsHookUnbLibIntDxe.h"
#endif

IDS_HOOK_ELEMENT FabricUnbIdsHooksDxe[] = {
//  {
//    IDS_HOOK_CCX_BEFORE_AP_LAUNCH,
//    &CcxXvPeiBeforeApLauch
//  },
  FABRIC_UNB_IDS_HOOKS_INT_DXE
  IDS_HOOKS_END
};

IDS_HOOK_TABLE FabricUnbIdsHookTableDxe = {
  IDS_HOOK_TABLE_HEADER_REV1_DATA,
  FabricUnbIdsHooksDxe
};


AGESA_STATUS
GetIdsHookTable (
  IN       IDS_HOOK_TABLE **IdsHookTable
  )
{
  *IdsHookTable = &FabricUnbIdsHookTableDxe;
  return AGESA_SUCCESS;
}
