/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate SMBIOS type 4 7 16 17 19 20
 *
 * Contains code that generate SMBIOS type 4 7 16 17 19 20
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Universal
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include "AGESA.h"
#include "Filecode.h"
#include "AmdAcpiDxe.h"
#include "AmdAcpiCpuSsdt.h"
#include "AmdAcpiSrat.h"
#include "AmdAcpiSlit.h"
#include "AmdAcpiCrat.h"
#include "AmdAcpiCdit.h"
#include "AmdAcpiMsct.h"
#include "Library/AmdBaseLib.h"
#include "Library/AmdHeapLib.h"
#include "Library/UefiBootServicesTableLib.h"
#include "Protocol/AcpiTable.h"
#include "Protocol/AmdAcpiCompleteProtocol.h"
#include "PiDxe.h"

#define FILECODE UNIVERSAL_ACPI_AMDACPIDXE_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Calculate an ACPI style checksum
 *
 * Computes the checksum and stores the value to the checksum
 * field of the passed in ACPI table's header.
 *
 * @param[in]  Table             ACPI table to checksum
 * @param[in]  StdHeader         Config handle for library and services
 *
 */
VOID
ChecksumAcpiTable (
  IN OUT   ACPI_TABLE_HEADER *Table,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8  *BuffTempPtr;
  UINT8  Checksum;
  UINT32 BufferOffset;

  Table->Checksum = 0;
  Checksum = 0;
  BuffTempPtr = (UINT8 *) Table;
  for (BufferOffset = 0; BufferOffset < Table->TableLength; BufferOffset++) {
    Checksum = Checksum - *(BuffTempPtr + BufferOffset);
  }

  Table->Checksum = Checksum;
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/**
 *---------------------------------------------------------------------------------------
 *
 *  Entry for AmdAcpiDxe
 *  Generate ACPI tables
 *
 *  Description:
 *    This function will populate ACPI tables with
 *      SLIT:
 *      SRAT:
 *
 *    @retval          EFI_STATUS
 *
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
AmdGenerateAcpiTables (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                 Status;
  EFI_STATUS                 CalledStatus;
  EFI_HANDLE                 Handle;
  AMD_CONFIG_PARAMS          StdHeader;
  EFI_ACPI_TABLE_PROTOCOL   *AcpiTableProtocol;
  DXE_AMD_ACPI_INIT_COMPLETE_PROTOCOL AmdAcpiInitCompleteProtocol;

  AGESA_TESTPOINT (TpUniversalAcpiEntry, NULL);

  IDS_HDT_CONSOLE (MAIN_FLOW, "  AmdGenerateAcpiTables Entry\n");

  // Avoid re-entry by locating gAmdAcpiDxeInitCompleteProtocolGuid.
  CalledStatus = gBS->LocateProtocol (&gAmdAcpiDxeInitCompleteProtocolGuid, NULL, (VOID **)&AmdAcpiInitCompleteProtocol);
  if (CalledStatus == EFI_SUCCESS) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "    re-entry, just return EFI_ALREADY_STARTED\n");
    AGESA_TESTPOINT (TpUniversalAcpiAbort, NULL);
    return EFI_ALREADY_STARTED;
  }

  Status = EFI_SUCCESS;

  // Locate ACPISupport table.
  CalledStatus = gBS->LocateProtocol (
                       &gEfiAcpiTableProtocolGuid,
                       NULL,
                       (VOID **)&AcpiTableProtocol
                       );
  if (EFI_ERROR (CalledStatus)) {
    AGESA_TESTPOINT (TpUniversalAcpiAbort, NULL);
    return CalledStatus;
  }

  // Processor SSDT
  if (PcdGetBool (PcdAmdAcpiCpuSsdt)) {
    CalledStatus = AmdAcpiCpuSSdt (AcpiTableProtocol, &StdHeader);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
  }

  // System Resource Affinity Table (SRAT)
  if (PcdGetBool (PcdAmdAcpiSrat)) {
    CalledStatus = AmdAcpiSrat (AcpiTableProtocol, &StdHeader);
    Status = (CalledStatus > Status) ? CalledStatus : Status;

    // Maximum System Characteristics Table (MSCT), provided only when SRAT exists
    if ((PcdGetBool (PcdAmdAcpiMsct)) && (CalledStatus == AGESA_SUCCESS)) {
      CalledStatus = AmdAcpiMsct (AcpiTableProtocol, &StdHeader);
      Status = (CalledStatus > Status) ? CalledStatus : Status;
    }
  }

  // System Locality Distance Information Table (SLIT)
  if (PcdGetBool (PcdAmdAcpiSlit)) {
    CalledStatus = AmdAcpiSlit (AcpiTableProtocol, &StdHeader);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
  }

  // Component Resource Affinity Table (CRAT)
  if (PcdGetBool (PcdAmdAcpiCrat)) {
    CalledStatus = AmdAcpiCrat (AcpiTableProtocol, &StdHeader);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
  }

  // Component Locality Distance Information Table (CDIT)
  if (PcdGetBool (PcdAmdAcpiCdit)) {
    CalledStatus = AmdAcpiCdit (AcpiTableProtocol, &StdHeader);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
  }

  // End
  IDS_HDT_CONSOLE (MAIN_FLOW, "  Install Protocol gAmdAcpiDxeInitCompleteProtocolGuid to avoid re-entry\n");
  AmdAcpiInitCompleteProtocol.Revision = 0;
  Handle = NULL;
  gBS->InstallProtocolInterface (
                          &Handle,
                          &gAmdAcpiDxeInitCompleteProtocolGuid,
                          EFI_NATIVE_INTERFACE,
                          &AmdAcpiInitCompleteProtocol
                          );

  IDS_HDT_CONSOLE (MAIN_FLOW, "  AmdGenerateAcpiTables Exit\n");

  AGESA_TESTPOINT (TpUniversalAcpiExit, NULL);

  return Status;

}


