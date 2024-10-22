/*****************************************************************************
  *
  * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
  *
  *******************************************************************************
  *
 */

#include "SocLogicalId.h"
#include "Library/AmdMemBaseV5Lib.h"
#include "Ids.h"
#include "Library/IdsLib.h"
#include "Library/AmdErrorLogLib.h"
#include "Library/AmdHeapLib.h"
#include "Filecode.h"

#define FILECODE LIBRARY_HARDCODEDMEMCOMMONSERVICELIB_HARDCODEDMEMCOMMONSERVICELIB_FILECODE

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

UINT32
GetPlatformNumberOfSockets (
  VOID
  )
{
  return 1;
}

BOOLEAN
GetSocketModuleOfNode (
  IN       UINT32    Node,
     OUT   UINT32    *Socket,
     OUT   UINT32    *Module,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  *Socket = 0;
  *Module = 0;

  return TRUE;
}

VOID
PutEventLog (
  IN       AGESA_STATUS EventClass,
  IN       UINT32 EventInfo,
  IN       UINT32 DataParam1,
  IN       UINT32 DataParam2,
  IN       UINT32 DataParam3,
  IN       UINT32 DataParam4,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{

}

BOOLEAN
GetPciAddress (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       UINT32            Socket,
  IN       UINT32            Module,
     OUT   PCI_ADDR          *PciAddress,
     OUT   AGESA_STATUS      *AgesaStatus
  )
{
  BOOLEAN      Result;

  Result = TRUE;
  // Always Succeeds.
  *AgesaStatus = AGESA_SUCCESS;

  PciAddress->AddressValue = MAKE_SBDFO (0, 0, 24, 0, 0);

  return Result;
}


BOOLEAN
IsProcessorPresent (
  IN       UINT32             Socket,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  BOOLEAN Result;

  Result = FALSE;

  if (Socket == 0) {
    Result = TRUE;
  }
  return Result;
}

VOID
GetLogicalIdOfSocket (
  IN       UINT32 Socket,
     OUT   SOC_LOGICAL_ID *LogicalId,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  LogicalId->Family = AMD_FAMILY_CZ;
  LogicalId->Revision = AMD_REV_F15_BR_A1;
}


