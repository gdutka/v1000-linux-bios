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
#include "PiDxe.h"
#include "Library/AmdBaseLib.h"
#include "Library/IdsLib.h"
#include "Library/AmdIdsHookLib.h"
#include "IdsHookId.h"

#ifndef IDS_HOOK_INTERNAL_SUPPORT
  #define CCX_IDS_HOOKS_INT_DXE
#else
  #include "Internal/CcxXvIdsHookLibIntDxe.h"
#endif

IDS_HOOK_ELEMENT CcxXvIdsHooksDxe[] = {
//  {
//    IDS_HOOK_CCX_BEFORE_AP_LAUNCH,
//    &CcxXvPeiBeforeApLauch
//  },
  CCX_IDS_HOOKS_INT_DXE
  IDS_HOOKS_END
};

IDS_HOOK_TABLE CcxXvIdsHookTableDxe = {
  IDS_HOOK_TABLE_HEADER_REV1_DATA,
  CcxXvIdsHooksDxe
};


AGESA_STATUS
GetIdsHookTable (
  IDS_HOOK_TABLE **IdsHookTable
  )
{
  *IdsHookTable = &CcxXvIdsHookTableDxe;
  return AGESA_SUCCESS;
}
