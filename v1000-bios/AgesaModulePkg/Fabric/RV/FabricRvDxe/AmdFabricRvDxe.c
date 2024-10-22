/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX Zen - Zeppelin API, and related functions.
 *
 * Contains code that initializes the core complex
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CCX
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include <PiDxe.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Guid/HobList.h>
#include <Guid/EventGroup.h>
#include <Filecode.h>
#include <Library/DxeFabricTopologyServices2Lib.h>
#include <Library/DxeFabricResourceManagerServicesLib.h>
#include <Library/FabricResourceReportToGcdLib.h>
#include <Protocol/FabricNumaServicesProtocol.h>
#include "FabricPiePwrMgmt.h"
#include "FabricAcpiTable.h"
#include "FabricReadyToBoot.h"
#include <Library/UefiBootServicesTableLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdHeapLib.h>

#define FILECODE FABRIC_RV_FABRICRVDXE_AMDFABRICRVDXE_FILECODE
/*++
Module Name:

  AmdFabricRvDxe.c
  Init Data Fabric interface

Abstract:
--*/

VOID
EFIAPI
FabricNbioSmuServicesCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );

/*++

Routine Description:

  Zen - Zeppelin Driver Entry.  Initialize the core complex.

Arguments:

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
AmdFabricRvDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  UINT32                         NumaDomainCount;
  EFI_STATUS                     Status;
  EFI_STATUS                     CalledStatus;
  EFI_EVENT                      ReadyToBootEvent;
  FABRIC_NUMA_SERVICES_PROTOCOL  *FabricNuma;

  AGESA_TESTPOINT (TpDfDxeEntry, NULL);

  Status = EFI_SUCCESS;

  IDS_HDT_CONSOLE (CPU_TRACE, "  AmdFabricRvDxeInit Entry\n");

  // Publish DXE topology services
  CalledStatus = FabricTopologyService2ProtocolInstall (ImageHandle, SystemTable);
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  // Publish DF Resource Manager services
  CalledStatus = FabricResourceManagerServiceProtocolInstall (ImageHandle, SystemTable);
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  // Report MMIO Region to GCD
  // NOTE: this must be after FabricResourceManagerServiceProtocolInstall
  FabricResourceReportToGcd ();

  // Publish NUMA services protocol
  CalledStatus = FabricRvNumaServicesProtocolInstall (ImageHandle, SystemTable);
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  if (gBS->LocateProtocol (&gAmdFabricNumaServicesProtocolGuid, NULL, (VOID **) &FabricNuma) == EFI_SUCCESS) {
    if (FabricNuma->GetDomainInfo (FabricNuma, &NumaDomainCount, NULL) == EFI_SUCCESS) {
      if (NumaDomainCount > 1) {
        // Publish ACPI Fabric SRAT services protocol
        CalledStatus = FabricRvAcpiSratProtocolInstall (ImageHandle, SystemTable);
        Status = (CalledStatus > Status) ? CalledStatus : Status;

        // Publish ACPI SLIT services protocol
        CalledStatus = FabricRvAcpiSlitProtocolInstall (ImageHandle, SystemTable);
        Status = (CalledStatus > Status) ? CalledStatus : Status;
      }
    }

    // Publish ACPI Fabric CRAT services protocol
    CalledStatus = FabricRvAcpiCratProtocolInstall (ImageHandle, SystemTable);
    Status = (CalledStatus > Status) ? CalledStatus : Status;

    // Publish ACPI CDIT services
    CalledStatus = FabricRvAcpiCditProtocolInstall (ImageHandle, SystemTable);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
  }

  IDS_HDT_CONSOLE (CPU_TRACE, "    PIE power management\n");
  FabricPiePwrMgmtInit ();

  CalledStatus = gBS->CreateEventEx (
                      EVT_NOTIFY_SIGNAL,
                      TPL_NOTIFY,
                      FabricReadyToBoot,
                      NULL,
                      &gEfiEventReadyToBootGuid,
                      &ReadyToBootEvent
                      );
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  IDS_HDT_CONSOLE (CPU_TRACE, "  AmdFabricRvDxeInit End\n");

  AGESA_TESTPOINT (TpDfDxeExit, NULL);

  return (Status);
}


