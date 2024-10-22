/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD common SOC IDS functions
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

#define FILECODE LIBRARY_SOCCMNIDSHOOKZPLIB_PEI_SOCCMNIDSHOOKZPLIBPEI_FILECODE


#ifndef IDS_HOOK_INTERNAL_SUPPORT
  #define SOC_IDS_HOOKS_INT
#else
  #include "Internal/SocCmnIdsHookZpLibIntPei.h"
#endif

IDS_HOOK_ELEMENT SocIdsCmnHooks[] = {
  SOC_IDS_HOOKS_INT
  IDS_HOOKS_END
};

IDS_HOOK_TABLE SocIdsCmnHookTable = {
  IDS_HOOK_TABLE_HEADER_REV1_DATA,
  SocIdsCmnHooks
};


AGESA_STATUS
GetIdsHookTable (
  IDS_HOOK_TABLE **IdsHookTable
  )
{
  *IdsHookTable = &SocIdsCmnHookTable;
  return AGESA_SUCCESS;
}
