/* $NoKeywords:$ */
/**
 * @file
 *
 * Contains definitions for AmdMemCzPei
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 12:28:05 -0600 (Tue, 09 Dec 2014) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _AMD_CALLOUT_LIB_H_
#define _AMD_CALLOUT_LIB_H_
#include <AGESA.h>

/// Parameters structure for the interface call-out AgesaReadSpd & AgesaReadSpdRecovery
typedef struct {
  IN OUT    AMD_CONFIG_PARAMS   StdHeader;      ///< Standard configuration header
  IN        UINT8               SocketId;       ///< Address of SPD - socket ID
  IN        UINT8               MemChannelId;   ///< Address of SPD - memory channel ID
  IN        UINT8               DimmId;         ///< Address of SPD - DIMM ID
  IN OUT    UINT8               *Buffer;        ///< Location where to place the SPD content
  IN OUT    MEM_DATA_STRUCT     *MemData;       ///< Location of the MemData structure, for reference
} AGESA_READ_SPD_PARAMS;

AGESA_STATUS
AgesaHookBeforeDramInit (
  IN       UINTN           SocketIdModuleId,
  IN OUT   MEM_DATA_STRUCT *MemData
  );

AGESA_STATUS
AgesaHookBeforeExitSelfRefresh (
  IN       UINTN           FcnData,
  IN OUT   MEM_DATA_STRUCT *MemData
  );

AGESA_STATUS
AgesaReadSpd  (
  IN       UINTN                   FcnData,
  IN OUT   AGESA_READ_SPD_PARAMS   *ReadSpd
  );

AGESA_STATUS
AgesaSaveNonVolatileS3Info (
  IN       UINTN                        DataSize,
  IN       VOID                         *Data
  );

AGESA_STATUS
AgesaSaveVolatileS3Info (
  IN       UINTN                        DataSize,
  IN       VOID                         *Data
  );
#endif      /* _AMD_CALLOUT_LIB_H_ */

