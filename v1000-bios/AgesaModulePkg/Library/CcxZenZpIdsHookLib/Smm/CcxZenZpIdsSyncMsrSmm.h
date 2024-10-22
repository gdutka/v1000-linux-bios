/* $NoKeywords:$ */
/**
 * @file
 *
 * Implement Ccx IDS sync up MSRs
 *
 * Contains code that sync up MSRs
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
#ifndef _CCX_ZEN_IDS_SYNC_MSR_SMM_H_
#define _CCX_ZEN_IDS_SYNC_MSR_SMM_H_
#include "AMD.h"

#pragma pack (push, 1)

/// BSC to AP MSR sync up
typedef struct {
  UINT32 RegisterAddress;   ///< MSR Address
  UINT64 RegisterValue;     ///< BSC's MSR Value
  UINT64 RegisterMask;      ///< MSR mask
  BOOLEAN ForceSetting;     ///< if TRUE, then force to set MSR to value of RegisterValue
} IDS_BSC_AP_MSR_SYNC;

/*---------------------------------------------------------------------------------------*/
/**
 * CcxIdsZenDxeSyncMsr
 *
 * Sync variable MTRR
 *
 * @param[in]  ApMsrSync         MSRs table
 * @param[in]  StdHeader         Config handle for library and services
 *
 */
VOID
CcxIdsZenSmmSyncMsr (
  IN OUT   IDS_BSC_AP_MSR_SYNC *ApMsrSync,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

#pragma pack (pop)
#endif // _CCX_ZEN_IDS_SYNC_MSR_SMM_H_
