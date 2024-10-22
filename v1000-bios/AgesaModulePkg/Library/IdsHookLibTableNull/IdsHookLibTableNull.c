/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include "Uefi.h"
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdIdsHookLib.h>
#include <IdsHookId.h>
#include <Filecode.h>

#define FILECODE LIBRARY_IDSHOOKLIBTABLENULL_IDSHOOKLIBTABLENULL_FILECODE

AGESA_STATUS
GetIdsHookTable (
  IDS_HOOK_TABLE **IdsHookTable
  )
{
  return AGESA_UNSUPPORTED;
}
