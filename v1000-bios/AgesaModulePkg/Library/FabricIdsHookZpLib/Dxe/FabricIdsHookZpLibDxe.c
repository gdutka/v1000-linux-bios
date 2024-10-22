/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Fabric IDS Hook implementation
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
#include "Library/AmdBaseLib.h"
#include "Library/IdsLib.h"
#include "Library/AmdIdsHookLib.h"
#include "IdsHookId.h"
#include "IdsNvIdZP.h"
#include "IdsNvDefZP.h"
#include "Filecode.h"

#define FILECODE LIBRARY_FABRICIDSHOOKZPLIB_DXE_FABRICIDSHOOKZPLIBDXE_FILECODE

#ifndef IDS_HOOK_INTERNAL_SUPPORT
  #define FABRIC_ZP_IDS_HOOKS_INT_DXE
#else
  #include "Internal/FabricIdsHookZpLibIntDxe.h"
#endif

IDS_HOOK_ELEMENT FabricZpIdsHooksDxe[] = {
  FABRIC_ZP_IDS_HOOKS_INT_DXE
  IDS_HOOKS_END
};

IDS_HOOK_TABLE FabricZpIdsHookTableDxe = {
  IDS_HOOK_TABLE_HEADER_REV1_DATA,
  FabricZpIdsHooksDxe
};


AGESA_STATUS
GetIdsHookTable (
  IDS_HOOK_TABLE **IdsHookTable
  )
{
  *IdsHookTable = &FabricZpIdsHookTableDxe;
  return AGESA_SUCCESS;
}
