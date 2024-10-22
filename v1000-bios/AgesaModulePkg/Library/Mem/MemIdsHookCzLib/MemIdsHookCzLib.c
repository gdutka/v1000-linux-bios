/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include "PiPei.h"
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdIdsHookLib.h>
#include <IdsHookId.h>
#include "AGESA.h"
#include "Filecode.h"

#define FILECODE LIBRARY_MEM_MEMIDSHOOKCZLIB_MEMIDSHOOKCZLIB_FILECODE

#ifndef IDS_HOOK_INTERNAL_SUPPORT
  #define MEM_IDS_HOOKS_INT
#else
  #include "Internal/MemIdsHookCzLibInt.h"
#endif

IDS_HOOK_ELEMENT MemIdsHookFunc[] = {
  MEM_IDS_HOOKS_INT
  IDS_HOOKS_END
};

IDS_HOOK_TABLE MemIdsHookTable  = {
  {
    IDS_HOOK_TABLE_SIGNATURE,
    IDS_HOOK_TABLE_REV_1,
    0
  },
  MemIdsHookFunc
};

AGESA_STATUS
GetIdsHookTable (
  IDS_HOOK_TABLE **IdsHookTable
  )
{
  *IdsHookTable = &MemIdsHookTable;

  return AGESA_SUCCESS;
}


