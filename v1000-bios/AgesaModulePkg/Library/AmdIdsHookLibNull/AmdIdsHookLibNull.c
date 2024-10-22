/**
 * @file
 *
 * AMD Integrated Debug Hook Routines
 *
 * Contains all functions related to IDS Hook
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 */
/*****************************************************************************
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

 /*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Uefi.h>
#include <Pi/PiMultiPhase.h>
#include <Library/AmdBaseLib.h>
#include <Library/AmdIdsHookLib.h>
#include <Filecode.h>

#define FILECODE LIBRARY_AMDIDSHOOKLIBNULL_AMDIDSHOOKLIBNULL_FILECODE

/// Control IDS HOOK supported or not. TRUE: IDS HOOK supported. FALSE: IDS HOOK unsupported
BOOLEAN IsIdsHookEnabled (
  )
{
  return FALSE;
}


/// Entry function of IDS HOOK
IDS_HOOK_STATUS
IdsHookEntry (
  HOOK_ID HookId,
  VOID *Handle,
  VOID *Data
  )
{
  return AGESA_UNSUPPORTED;
}

///Return IDS HOOK function table
AGESA_STATUS
GetIdsHookTable (
  IDS_HOOK_FUNCTION_TABLE *IdsHookFunctionTable
  )
{
  return AGESA_UNSUPPORTED;
}

/// IDS HOOK dispatcher, it will loop and process the table report by GetIdsHookTable
IDS_HOOK_STATUS
IdsHookDispatcher (
  HOOK_ID HookId,
  VOID *Handle,
  VOID *Data
  )
{
  return AGESA_UNSUPPORTED;
}
