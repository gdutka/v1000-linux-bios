/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD PSP IDS functions
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
#include "Uefi.h"
#include "Library/AmdBaseLib.h"
#include "Library/IdsLib.h"
#include "Library/AmdIdsHookLib.h"
#include "IdsHookId.h"
#include "Filecode.h"

#define FILECODE LIBRARY_PSPIDSHOOKLIB_DXE_PSPIDSHOOKLIBDXE_FILECODE


#ifndef IDS_HOOK_INTERNAL_SUPPORT
  #define PSP_IDS_HOOKS_INT
#else
  #include "Internal/PspIdsHookLibDxeInt.h"
#endif

IDS_HOOK_ELEMENT PspIdsHooks[] = {
  PSP_IDS_HOOKS_INT
  IDS_HOOKS_END
};

IDS_HOOK_TABLE PspIdsHookTable = {
  IDS_HOOK_TABLE_HEADER_REV1_DATA,
  PspIdsHooks
};

AGESA_STATUS
GetIdsHookTable (
  IDS_HOOK_TABLE **IdsHookTable
  )
{
  *IdsHookTable = &PspIdsHookTable;
  return AGESA_SUCCESS;
}
