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
#ifndef _AMD_IDS_HOOK_EXTLIB_H_
#define _AMD_IDS_HOOK_EXTLIB_H_

#include <IdsHookId.h>

///
/// The return status for all IDS HOOK services.
///
typedef enum {
  IDS_HOOK_SUCCESS = 0,       ///< The service completed normally
  IDS_HOOK_UNSUPPORTED,       ///< Unsupported IDS HOOK
  IDS_HOOK_BUFFER_TOO_SMALL,  ///< Too small buffer
  IDS_HOOK_NOT_FOUND,         ///< Haven't found accordingly service entry for specific IDS HOOK ID
  IDS_HOOK_ERROR,             ///< Error happens during service IDS HOOK
  IDS_HOOK_SKIP,              ///< Use to notify the IDS HOOK caller to skip a block of codes, used for IDS_HOOK_SKIP
  IDS_HOOK_NO_SKIP,           ///< Use to notify the IDS HOOK caller not skip a block of codes, used for IDS_HOOK_SKIP
  IDS_HOOK_MAX                ///< Not a status, for limit checking.
} IDS_HOOK_STATUS;
/**
 * Entry function of IDS Ext HOOK
 *
 *
 * @param[in] HookId            ID used to identify the IDS HOOK
 * @param[in,out] Handle        Optional, Used to transfer variable information except the Data.
 *                              e.g. It can set with the service table pointer. Or
 *                              It can set with the global data structure
 * @param[in,out] Data          Point to the data used in the Hook
 *
 * @retval    IDS_HOOK_SUCCESS     Hook execute success
 * @retval    other value          refer to IDS_HOOK_STATUS
 *
 */
IDS_HOOK_STATUS
IdsHookExtEntry (
  HOOK_ID HookId,
  VOID *Handle,
  VOID *Data
  );

/**
 * Get IDS NV Table
 *
 * IDS NV table is a data filled multiple IDS_NV_RECORD structure
 *
 * @param[in,out] IdsNvTable        Data pointer used to filled with multiple IDS_NV_RECORD structure
 *                              The memory space need be pre-allocated before call this routine
 *
 * @param[in,out] IdsNvTableSize   Size of IdsNvTable, if size is less than IdsNvTable occupied, it will
 *                                 filled with the required data size on ouput.
 *                                 On input, the size in bytes allocated for the return IdsNvTable.
 *                                 On output, the size of data returned in IdsNvTable.
 *
 * @retval    IDS_HOOK_SUCCESS     Get IDS NV Table successfully
 * @retval    IDS_HOOK_BUFFER_TOO_SMALL       Buffer too small to hold IdsNvTable
 * @retval    Other                Other error happens during get IDS NV Table
 *
 */
IDS_HOOK_STATUS
GetIdsNvTable (
  IN OUT   VOID *IdsNvTable,
  IN OUT   UINT32 *IdsNvTableSize
  );

#endif //#define _AMD_IDS_HOOK_EXTLIB_H_

