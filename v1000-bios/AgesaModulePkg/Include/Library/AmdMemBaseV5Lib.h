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

#ifndef _AMD_MEM_BASE_V5_LIB_H_
#define _AMD_MEM_BASE_V5_LIB_H_
#include "Uefi.h"
#include "AMD.h"
#include "Library/IdsLib.h"
#include "SocLogicalId.h"

UINT32
GetPlatformNumberOfSockets (
  VOID
  );

BOOLEAN
GetSocketModuleOfNode (
  IN       UINT32    Node,
     OUT   UINT32    *Socket,
     OUT   UINT32    *Module,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
PutEventLog (
  IN       AGESA_STATUS EventClass,
  IN       UINT32 EventInfo,
  IN       UINT32 DataParam1,
  IN       UINT32 DataParam2,
  IN       UINT32 DataParam3,
  IN       UINT32 DataParam4,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

BOOLEAN
GetPciAddress (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       UINT32            Socket,
  IN       UINT32            Module,
     OUT   PCI_ADDR          *PciAddress,
     OUT   AGESA_STATUS      *AgesaStatus
);

BOOLEAN
IsProcessorPresent (
  IN       UINT32             Socket,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
GetLogicalIdOfSocket (
  IN       UINT32 Socket,
     OUT   SOC_LOGICAL_ID *LogicalId,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

#endif // _AMD_BASE_LIB_H_

