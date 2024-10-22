/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Library
 *
 * Contains interface to the AMD AGESA library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision: 312770 $   @e \$Date: 2015-02-11 13:24:42 +0800 (Wed, 11 Feb 2015) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _MEM_SMBIOS_V2_LIB_H_
#define _MEM_SMBIOS_V2_LIB_H_
#include "AMD.h"
#include <Guid/AmdMemoryInfoHob.h>

AGESA_STATUS
GetMemSmbiosV2 (
  IN       UINT8                       DramType,
  IN       UINT8                       NumSockets,
  IN       VOID                        *ApobSmbiosInfo,
  IN       HOST_TO_APCB_CHANNEL_XLAT   *MemChanXLatTab,
  IN       AMD_CONFIG_PARAMS           *StdHeader,
  IN       AMD_MEMORY_INFO_HOB         *AmdMemoryInfoHob
  );

#endif // _MEM_SMBIOS_V2_LIB_H_

