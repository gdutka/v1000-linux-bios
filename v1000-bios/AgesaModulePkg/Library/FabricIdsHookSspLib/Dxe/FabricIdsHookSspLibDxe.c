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
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdIdsHookLib.h>
#include <IdsHookId.h>
#include <IdsNvIdSsp.h>
#include <IdsNvDefSSP.h>
#include <Filecode.h>

#define FILECODE LIBRARY_FABRICIDSHOOKSSPLIB_DXE_FABRICIDSHOOKSSPLIBDXE_FILECODE

#ifndef IDS_HOOK_INTERNAL_SUPPORT
  #define FABRIC_SSP_IDS_HOOKS_INT_DXE
#else
  #include "Internal/FabricIdsHookSspLibIntDxe.h"
#endif

IDS_HOOK_ELEMENT FabricSspIdsHooksDxe[] = {
  FABRIC_SSP_IDS_HOOKS_INT_DXE
  IDS_HOOKS_END
};

IDS_HOOK_TABLE FabricSspIdsHookTableDxe = {
  IDS_HOOK_TABLE_HEADER_REV1_DATA,
  FabricSspIdsHooksDxe
};


AGESA_STATUS
GetIdsHookTable (
  IDS_HOOK_TABLE **IdsHookTable
  )
{
  *IdsHookTable = &FabricSspIdsHookTableDxe;
  return AGESA_SUCCESS;
}



