/* $NoKeywords:$ */
/**
 * @file
 *
 * CCX SMBIOS library prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Library
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _CCX_SMBIOS_LIB_H_
#define _CCX_SMBIOS_LIB_H_
#include "AMD.h"
#include "Protocol/AmdSmbiosServicesProtocol.h"
#pragma pack (push, 1)

/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */
/// Information for each CACHE level
typedef struct _GET_EACH_LEVEL_CACHE_INFO {
  UINT32   CacheSize;                              ///< Cache size
  UINT8    Associativity;                          ///< Associativity
} GET_EACH_LEVEL_CACHE_INFO;

/// CACHE information structure
typedef struct _GET_CACHE_INFO {
  GET_EACH_LEVEL_CACHE_INFO  CacheEachLevelInfo[CpuLmaxCache]; ///< Cache info
} GET_CACHE_INFO;

/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */
UINT16
CcxGetProcessorFamilyForSmbios (
  IN       UINTN               Socket,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

UINT8
CcxGetProcessorUpgradeForSmbios (
  IN       UINTN               Socket,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

VOID
CcxGetCacheInfo (
  IN       UINTN               Socket,
  IN       GET_CACHE_INFO     *CacheInfo,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

#pragma pack (pop)
#endif // _CCX_SMBIOS_LIB_H_
