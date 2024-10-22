/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Fabric - Zeppelin API, and related functions.
 *
 * Contains code that initializes the data fabric
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
/*++
Module Name:

  AmdFabricRvPei.c
  Init DF interface

Abstract:
--*/

#include <PiPei.h>
#include "AGESA.h"
#include <Filecode.h>
#include <AmdSoc.h>
#include "FabricPstateServicesPei.h"
#include <Library/PeiFabricTopologyServices2Lib.h>
#include <Library/PeiFabricResourceManagerServicesLib.h>
#include "FabricDfClkGatingInit.h"
#include "FabricCsInit.h"
#include "FabricCcmInit.h"
#include "FabricGcmNcmInit.h"
#include "FabricIomsInit.h"
#include "FabricPieRasInit.h"
#include <Library/AmdIdsHookLib.h>
#include "Library/IdsLib.h"
#include <Ppi/SocLogicalIdPpi.h>
#include "FabricScrubInit.h"

#define FILECODE FABRIC_RV_FABRICRVPEI_AMDFABRICRVPEI_FILECODE


/*++

Routine Description:

  Fabric - Zeppelin Driver Entry.  Initialize the core complex.

Arguments:

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
AmdFabricRvPeiInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                      Status;
  EFI_STATUS                      CalledStatus;
  AMD_PEI_SOC_LOGICAL_ID_PPI      *SocLogicalIdPpi;

  AGESA_TESTPOINT (TpDfPeiEntry, NULL);

  Status = EFI_SUCCESS;

  IDS_HDT_CONSOLE (CPU_TRACE, "  AmdFabricRvPeiInit Entry\n");

  CalledStatus = (*PeiServices)->LocatePpi (
                                 PeiServices,
                                 &gAmdSocLogicalIdPpiGuid,
                                 0,
                                 NULL,
                                 (VOID**)&SocLogicalIdPpi
                                 );

  ASSERT (CalledStatus == EFI_SUCCESS);

  // Publish PEI topology services
  CalledStatus = FabricTopologyService2PpiInstall (PeiServices);
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  // Publish PEI P-state services
  CalledStatus = FabricPstateServicePpiInstall (PeiServices);
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  // Publish PEI resource manager services
  CalledStatus = FabricResourceManagerServicePpiInstall (PeiServices);
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  IDS_HDT_CONSOLE (CPU_TRACE, "    Clock Gating Init\n");
  FabricClockGatingInit (PeiServices, SocLogicalIdPpi);

  IDS_HDT_CONSOLE (CPU_TRACE, "    CS Init\n");
  FabricCsInit (PeiServices, SocLogicalIdPpi);

  IDS_HDT_CONSOLE (CPU_TRACE, "    CCM Init\n");
  FabricCcmInit (PeiServices, SocLogicalIdPpi);

  IDS_HDT_CONSOLE (CPU_TRACE, "    GCM/NCM Init\n");
  FabricGcmNcmInit (PeiServices, SocLogicalIdPpi);

  IDS_HDT_CONSOLE (CPU_TRACE, "    IOMS Init\n");
  FabricIomsInit (PeiServices, SocLogicalIdPpi);

  IDS_HDT_CONSOLE (CPU_TRACE, "    PIE RAS Init\n");
  FabricPieRasInit (PeiServices, SocLogicalIdPpi);

  IDS_HDT_CONSOLE (CPU_TRACE, "    Scrub Init\n");
  FabricScrubInit (PeiServices, SocLogicalIdPpi);

  IDS_HOOK (IDS_HOOK_FABRIC_PEI_INIT_END, NULL, NULL);

  IDS_HDT_CONSOLE (CPU_TRACE, "  AmdFabricRvPeiInit End\n");

  AGESA_TESTPOINT (TpDfPeiExit, NULL);

  return (Status);
}


