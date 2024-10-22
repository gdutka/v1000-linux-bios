/* $NoKeywords:$ */
/**
 * @file
 *
 * PSP TPM ACPI related functions
 *
 * Contains PSP TPM interface
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision$   @e \$Date$
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdPspCommonLib.h>
#include <Library/AmdPspFtpmLib.h>
#include <Protocol/AmdPspFtpmProtocol.h>

#include <Protocol/AcpiSupport.h>
#include <Protocol/AcpiSystemDescriptionTable.h>

//fTPM ACPI Object (AML code binary)
#include "fTPMAmlData.h"
#include "fTPMAcpi.h"


STATIC TPM2_ACPI_TABLE  Tpm2AcpiTable = {
  {
    SIGNATURE_32 ('T','P','M','2'),
    sizeof (TPM2_ACPI_TABLE),
    0x03,                             // ACPI Revision.
    0x00,
    TPM2_ACPI_OEM_ID,                 // OEM ID (filled in below).
    TPM2_ACPI_TABLE_OEM_ID,           // OEM Table ID (filled in below).
    TPM2_ACPI_OEM_REVISION ,          // ACPI OEM Revision (filled in below).
    1,                                // OEM Creator ID (filled in below).
    1                                 // OEM Creator Revision (filled in below).
   },
   0,                                 // Flags
   0,                                 // ControlArea
   0x02                               // StartMethod
};

VOID
EFIAPI
fTPMAcpiPublish (
  VOID
  )
{
  EFI_STATUS                  Status;
  EFI_ACPI_SUPPORT_PROTOCOL   *AcpiSupport;
  UINTN                       TableHandle;
  TPM2_CONTROL_AREA           *Tpm2ControlArea;
  UINT8                       *Tpm2AcpiDataPtr;
  UINT32                      *Memory32Fixed;
  EFI_ACPI_TABLE_VERSION      Version;

//
// Locate ACPISupport table. Bail if absent
//
  Status = gBS->LocateProtocol (
                  &gEfiAcpiSupportProtocolGuid,
                  NULL,
                  &AcpiSupport);
  if (EFI_ERROR (Status)) {
    return;
  }

  if (GetFtpmControlArea (&Tpm2ControlArea)) {
    return;
  }

  // Update the TPM ACPI Table for ControlArea location
  Tpm2AcpiTable.ControlArea = (EFI_PHYSICAL_ADDRESS) Tpm2ControlArea + 0x10;

  //
  // Install the ACPI Table
  //
  IDS_HDT_CONSOLE_PSP_TRACE ("\tInstall ACPI TPM2 Table\n");
  TableHandle = 0;
  Version = (EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0 |
             EFI_ACPI_TABLE_VERSION_4_0) ;
  Status = AcpiSupport->SetAcpiTable (
                  AcpiSupport,
                  &Tpm2AcpiTable,
                  TRUE,
                  Version,
                  &TableHandle
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("\tUpdate _CRS Object with actual value\n");

  //Update _CRS Object with actual value
  for (Tpm2AcpiDataPtr = ((UINT8 *)fTPMAmlData + sizeof (EFI_ACPI_DESCRIPTION_HEADER));
       Tpm2AcpiDataPtr <= ((UINT8 *)fTPMAmlData + ((EFI_ACPI_DESCRIPTION_HEADER *)fTPMAmlData)->Length);
       Tpm2AcpiDataPtr++
       ) {
    Memory32Fixed = (UINT32 *)Tpm2AcpiDataPtr;
    switch (*Memory32Fixed) {
    //TPM2.0 Command Buffer allocate by BIOS, should be updated during POST
    //Memory32Fixed (ReadWrite, 0xBBBBBBBB, 0x100000)
    case 0xBBBBBBBB:
      *Memory32Fixed = (UINT32) (Tpm2ControlArea->CommandAddress);
      IDS_HDT_CONSOLE_PSP_TRACE ("Tpm2ControlArea->CommandAddress %x\n", Tpm2ControlArea->CommandAddress);
      Tpm2AcpiDataPtr += (sizeof (UINT32) - 1);
      break;
    //TPM2.0 Response Buffer allocate by BIOS, should be updated during POST
    //Memory32Fixed (ReadWrite, 0xCCCCCCCC, 0x100000)
    case 0xCCCCCCCC:
      *Memory32Fixed = (UINT32) (Tpm2ControlArea->ResponseAddress);
      IDS_HDT_CONSOLE_PSP_TRACE ("Tpm2ControlArea->ResponseAddress %x\n", Tpm2ControlArea->ResponseAddress);
      Tpm2AcpiDataPtr += (sizeof (UINT32) - 1);
      break;
    default:
      break;
    }
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("\tInstall Tpm SSDT table\n");

  TableHandle = 0;
  AcpiSupport->SetAcpiTable (
                 AcpiSupport,
                 fTPMAmlData,
                 TRUE,
                 Version,
                 &TableHandle
                 );
  if (EFI_ERROR (Status)) {
    return;
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("PublishTables ACPI table\n");

  Status = AcpiSupport->PublishTables (AcpiSupport, Version);
  if (EFI_ERROR (Status)) {
    return;
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("fTPMAcpiEvent exit\n");

}

EFI_STATUS
fTpmAcpiEntryPoint (
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )

{
  EFI_STATUS                  Status;

  //If PSP feature turn off, exit the driver
  Status = CheckPspDevicePresent ();
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.Drv.fTpmAcpiEntryPoint Enter\n");

  fTPMAcpiPublish ();

  IDS_HDT_CONSOLE_PSP_TRACE ("fTpmAcpiEntryPoint Exit\n");
  return Status;
}

