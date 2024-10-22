/** @file

;******************************************************************************
;* Copyright (c) 2017 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Protocol/AcpiTable.h>
#include <Protocol/AcpiSupport.h>
#include <Guid/H2OCp.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/H2OCpLib.h>
#include <Library/MemoryAllocationLib.h>

typedef struct {
  EFI_ACPI_SDT_HEADER                       *Table;
  EFI_ACPI_TABLE_VERSION                    Version;
  UINTN                                     Handle;
  BOOLEAN                                   Skip;
} ACPI_SDT_INFO;

/**

  Notification function for Update ACPI Description Header

  @param  Event                 Event whose notification function is being invoked.
  @param  Context               The pointer to the notification function's context,
                                which is implementation-dependent.
**/
VOID
EFIAPI
UpdateAcpiDescriptionHeader (
  IN EFI_EVENT   Event,
  IN VOID        *Context
  )
{
  EFI_STATUS                                         Status;
  UINTN                                              Index;
  UINTN                                              Handle;
  EFI_ACPI_TABLE_VERSION                             Version;
  EFI_ACPI_SUPPORT_PROTOCOL                          *AcpiSupport;
  EFI_ACPI_SDT_HEADER                                *Table;
  UINTN                                              Count;
  ACPI_SDT_INFO                                      *AcpiSdtInfo;
  H2O_BDS_CP_UPDATE_ACPI_DESC_HDR_DATA               BdsCpUpdateAcpiDescHdrData;

  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  Status = gBS->LocateProtocol (&gEfiAcpiSupportProtocolGuid, NULL, (VOID **)&AcpiSupport);
  if (EFI_ERROR(Status)) {
    DEBUG ((EFI_D_ERROR, "Locate AcpiSupportProtocol(%r)\n", Status));
    return;
  }

  Count = 0;
  AcpiSdtInfo = NULL;
  while (TRUE) {
    Status = AcpiSupport->GetAcpiTable (AcpiSupport, Count, (VOID **)&Table, &Version, &Handle);
    if (EFI_ERROR (Status)) {
      break;
    }
    FreePool (Table);
    Count++;
  }
  if (Count == 0) {
    DEBUG ((EFI_D_ERROR, "Not found ACPI table.\n"));
    return;
  }

  AcpiSdtInfo = AllocatePool (sizeof (ACPI_SDT_INFO) * Count);
  if (AcpiSdtInfo == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    ASSERT_EFI_ERROR (Status);
    return;
  }

  for (Index = 0; Index < Count; Index++) {
    Status = AcpiSupport->GetAcpiTable (AcpiSupport, Index, (VOID **)&Table, &Version, &Handle);
    if (EFI_ERROR (Status)) {
      AcpiSdtInfo[Index].Table = NULL;
      continue;
    }

    AcpiSdtInfo[Index].Table   = Table;
    AcpiSdtInfo[Index].Version = Version;
    AcpiSdtInfo[Index].Handle  = Handle;
    AcpiSdtInfo[Index].Skip    = TRUE;
    if (Table->Signature == EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE) {
      continue;
    }

    BdsCpUpdateAcpiDescHdrData.Size = sizeof (H2O_BDS_CP_UPDATE_ACPI_DESC_HDR_DATA);
    BdsCpUpdateAcpiDescHdrData.Status = H2O_BDS_TASK_NORMAL;
    BdsCpUpdateAcpiDescHdrData.AcpiTableHeader = Table;
    DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpUpdateAcpiDescHdrGuid));
    H2OCpTrigger (&gH2OBdsCpUpdateAcpiDescHdrGuid, &BdsCpUpdateAcpiDescHdrData);
    DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", BdsCpUpdateAcpiDescHdrData.Status));
    if (BdsCpUpdateAcpiDescHdrData.Status == H2O_CP_TASK_UPDATE) {
      AcpiSdtInfo[Index].Skip = FALSE;
    }
  }

  for (Index = 0; Index < Count; Index++) {
  	if (AcpiSdtInfo[Index].Table != NULL) {
      Table = AcpiSdtInfo[Index].Table;
      if (!AcpiSdtInfo[Index].Skip) {
        Version = AcpiSdtInfo[Index].Version;
        Handle  = AcpiSdtInfo[Index].Handle;
        AcpiSupport->SetAcpiTable (AcpiSupport, Table, TRUE, Version, &Handle);
      }
      FreePool (Table);
    }
  }

  if (AcpiSdtInfo != NULL) {
    FreePool (AcpiSdtInfo);
  }
  return;
}
