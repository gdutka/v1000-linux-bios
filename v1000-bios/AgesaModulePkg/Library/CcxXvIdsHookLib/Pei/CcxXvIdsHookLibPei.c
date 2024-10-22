/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX XV - Excavator API, and related functions.
 *
 * Contains code that initializes the core complex
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
#include "PiPei.h"
#include "Library/AmdBaseLib.h"
#include "Library/IdsLib.h"
#include "Library/AmdIdsHookLib.h"
#include "IdsHookId.h"

#ifndef IDS_HOOK_INTERNAL_SUPPORT
  #define CCX_IDS_HOOKS_INT_PEI
#else
  #include "Internal/CcxXvIdsHookLibIntPei.h"
#endif

IDS_HOOK_ELEMENT CcxXvIdsHooksPei[] = {
//  {
//    IDS_HOOK_CCX_BEFORE_AP_LAUNCH,
//    &CcxXvPeiBeforeApLauch
//  },
  CCX_IDS_HOOKS_INT_PEI
  IDS_HOOKS_END
};

IDS_HOOK_TABLE CcxXvIdsHookTablePei = {
  IDS_HOOK_TABLE_HEADER_REV1_DATA,
  CcxXvIdsHooksPei
};


AGESA_STATUS
GetIdsHookTable (
  IDS_HOOK_TABLE **IdsHookTable
  )
{
  *IdsHookTable = &CcxXvIdsHookTablePei;
  return AGESA_SUCCESS;
}
