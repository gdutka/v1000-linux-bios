/** @file
  This module install ACPI Security Devices ACPI Table (SDEV).

;******************************************************************************
;* Copyright (c) 2018 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "InstallSdevDxe.h"

EFI_ACPI_6_2_SECURE_DEVICES_TABLE_HEADER mSdevHeaderTemplate = {
    EFI_ACPI_6_2_SECURE_DEVICES_TABLE_SIGNATURE,
    //
    // Length will be updated at runtime
    //
    0,
    EFI_ACPI_6_2_SECURE_DEVICES_TABLE_REVISION,
    //
    // Checksum will be updated at runtime
    //
    0x00,
    //
    // It is expected that these values will be updated at runtime.
    //
    EFI_ACPI_OEM_ID,
    EFI_ACPI_OEM_TABLE_ID,
    EFI_ACPI_OEM_REVISION,
    EFI_ACPI_CREATOR_ID,
    EFI_ACPI_CREATOR_REVISION
};

/**
 This function calculates and updates an UINT8 checksum.

 @param [in]   Buffer
 @param [in]   Size             Number of bytes to checksum.

 @retval None

**/
VOID
SdevAcpiTableChecksum (
  IN UINT8      *Buffer,
  IN UINTN      Size
  )
{
  UINTN ChecksumOffset;

  ChecksumOffset = OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER, Checksum);

  //
  // Set checksum to 0 first.
  //
  Buffer[ChecksumOffset] = 0;

  //
  // Update checksum value.
  //
  Buffer[ChecksumOffset] = CalculateCheckSum8 (Buffer, Size);
}

/**
  Notification function for SdevNotifyFunction Handler Protocol.

  @param  Event                 Event whose notification function is being invoked.
  @param  Context               The pointer to the notification function's context,
                                which is implementation-dependent.

  @retval VOID

**/
VOID
EFIAPI
SdevNotifyFunction (
  IN EFI_EVENT   Event,
  IN VOID        *Context
  )
{
  EFI_STATUS                            Status;
  UINTN                                 Index;
  UINTN                                 Handle;
  EFI_ACPI_TABLE_VERSION                Version;
  EFI_ACPI_COMMON_HEADER                *Table;
  EFI_ACPI_TABLE_PROTOCOL               *AcpiTableProtocol;
  EFI_ACPI_SUPPORT_PROTOCOL             *AcpiSupport;
  EFI_ACPI_6_2_SDEV_STRUCTURE_HEADER    *SdevStructureHeader;
  EFI_ACPI_6_2_SDEV_STRUCTURE_HEADER    *SdevPtr;
  UINT8                                 *SdevTable;
  UINT8                                 *Ptr;
  UINT32                                SdevStructureSize;
  UINTN                                 Offset;
  UINTN                                 TableSize;
  UINTN                                 TableHandle;

  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  //
  // Find the AcpiTable protocol
  //
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID**)&AcpiTableProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Locate AcpiTableProtocol(%r)\n", Status));
    return;
  }

  AcpiSupport = NULL;
  Status = gBS->LocateProtocol (&gEfiAcpiSupportProtocolGuid, NULL, (VOID **)&AcpiSupport);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "Locate AcpiSupportProtocol(%r)\n", Status));
    return;
  }

  Index = 0;
  do {
    Table = NULL;
    Status = AcpiSupport->GetAcpiTable (AcpiSupport, Index, (VOID **)&Table, &Version, &Handle);
    if (EFI_ERROR(Status)) {
      break;
    }
    if (Table->Signature == EFI_ACPI_6_2_SECURE_DEVICES_TABLE_SIGNATURE) {
      DEBUG ((DEBUG_ERROR, "\nWARNING !\n"));
      DEBUG ((DEBUG_ERROR, "\nWARNING - The ACPI SDEV table is already exist!\n"));
      DEBUG ((DEBUG_ERROR, "\nWARNING - The default ACPI SDEV table should not be inatlled!\n"));
      FreePool (Table);
      return;
    }
    FreePool (Table);
    Index++;
  } while (TRUE);

  //
  // Skip PCD data if the size of PCD PcdH2OAcpiSdevData is smaller than size
  // of EFI_ACPI_6_2_SDEV_STRUCTURE_HEADER
  //
  SdevStructureHeader = NULL;
  SdevStructureSize = 0;
  if (PcdGetSize (PcdH2OAcpiSdevData) >= sizeof (EFI_ACPI_6_2_SDEV_STRUCTURE_HEADER)) {
    SdevStructureHeader = (EFI_ACPI_6_2_SDEV_STRUCTURE_HEADER *) PcdGetPtr (PcdH2OAcpiSdevData);
    SdevStructureSize = (UINT32)PcdGetSize (PcdH2OAcpiSdevData);
  }

  if (PcdGetBool(PcdH2OBdsCpUpdateAcpiSdevSupported)) {
    H2O_BDS_CP_UPDATE_ACPI_SDEV_DATA  BdsCpUpdateAcpiSdevData;
    BdsCpUpdateAcpiSdevData.Size = sizeof (H2O_BDS_CP_UPDATE_ACPI_SDEV_DATA);
    BdsCpUpdateAcpiSdevData.Status = H2O_BDS_TASK_NORMAL;
    if (PcdGetSize (PcdH2OAcpiSdevData) >= sizeof (EFI_ACPI_6_2_SDEV_STRUCTURE_HEADER)) {
      BdsCpUpdateAcpiSdevData.SdevStructureHeader = AllocateCopyPool (PcdGetSize (PcdH2OAcpiSdevData), PcdGetPtr (PcdH2OAcpiSdevData));
      BdsCpUpdateAcpiSdevData.SdevStructureSize = (UINT32)PcdGetSize (PcdH2OAcpiSdevData);
    } else {
      BdsCpUpdateAcpiSdevData.SdevStructureHeader = NULL;
      BdsCpUpdateAcpiSdevData.SdevStructureSize = 0;
    }
    DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpUpdateAcpiSdevGuid));
    H2OCpTrigger (&gH2OBdsCpUpdateAcpiSdevGuid, &BdsCpUpdateAcpiSdevData);
    DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", BdsCpUpdateAcpiSdevData.Status));
    if (BdsCpUpdateAcpiSdevData.Status == H2O_CP_TASK_SKIP) {
      if (BdsCpUpdateAcpiSdevData.SdevStructureHeader != NULL) {
        FreePool(BdsCpUpdateAcpiSdevData.SdevStructureHeader);
      }
      goto UpdateAcpiSdevSkip;
    } else if (BdsCpUpdateAcpiSdevData.Status == H2O_CP_TASK_UPDATE) {
      SdevStructureHeader = BdsCpUpdateAcpiSdevData.SdevStructureHeader;
      SdevStructureSize = BdsCpUpdateAcpiSdevData.SdevStructureSize;
    } else {
      if (BdsCpUpdateAcpiSdevData.SdevStructureHeader != NULL) {
        FreePool(BdsCpUpdateAcpiSdevData.SdevStructureHeader);
      }
    }
  }
  //
  // Check length of each table
  //
  if ((SdevStructureSize == 0) || (SdevStructureHeader == NULL)) {
    DEBUG ((DEBUG_ERROR, "Size of ACPI Secure Device Structures = 0 or ACPI Secure Device Structures = NULL !"));
    goto UpdateAcpiSdevSkip;
  }
  Offset = 0;
  SdevPtr = SdevStructureHeader;
  while (Offset < SdevStructureSize) {
    //
    // Check size of current table
    //
    if (SdevPtr->Length < sizeof (EFI_ACPI_6_2_SDEV_STRUCTURE_HEADER)) {
      DEBUG ((DEBUG_ERROR, "ACPI Secure Device Structures size error!"));
      break;
    }
    Offset += SdevPtr->Length;
    if (Offset == SdevStructureSize) {
      break;
    } else if (Offset > SdevStructureSize) {
      DEBUG ((DEBUG_ERROR, "ACPI Secure Device Structures size error!"));
      break;
    }
    //
    // Set to next table
    //
    Ptr = (UINT8 *) SdevPtr;
    Ptr += SdevPtr->Length;
    SdevPtr = (EFI_ACPI_6_2_SDEV_STRUCTURE_HEADER *) Ptr;
  }
  if (Offset != SdevStructureSize) {
    DEBUG ((DEBUG_ERROR, "ACPI Secure Device Structures size error!"));
    goto UpdateAcpiSdevSkip;
  }

  TableSize = sizeof(EFI_ACPI_6_2_SECURE_DEVICES_TABLE_HEADER) + SdevStructureSize;
  mSdevHeaderTemplate.Header.Length = (UINT32) TableSize;
  SdevTable = AllocateZeroPool (TableSize);
  if (SdevTable == NULL) {
    DEBUG ((DEBUG_ERROR, "Fail to allocate pool!\n"));
    goto UpdateAcpiSdevSkip;
  }
  CopyMem (SdevTable, &mSdevHeaderTemplate, sizeof(EFI_ACPI_6_2_SECURE_DEVICES_TABLE_HEADER));
  CopyMem (SdevTable + sizeof(EFI_ACPI_6_2_SECURE_DEVICES_TABLE_HEADER), SdevStructureHeader, SdevStructureSize);
  SdevAcpiTableChecksum ((UINT8 *) SdevTable, TableSize);
  TableHandle = 0x00;
  Status = AcpiTableProtocol->InstallAcpiTable (
                                   AcpiTableProtocol,
                                   SdevTable,
                                   TableSize,
                                   &TableHandle
                                   );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "InstallAcpiTable(%r)\n", Status));
  }
  FreePool(SdevTable);

UpdateAcpiSdevSkip:
  if ((SdevStructureHeader != NULL) && (SdevStructureHeader != (EFI_ACPI_6_2_SDEV_STRUCTURE_HEADER *) PcdGetPtr (PcdH2OAcpiSdevData))) {
    FreePool(SdevStructureHeader);
  }
  return;
}

/**
 Install ACPI Security Devices Table

 @param [in]   ImageHandle      The firmware allocated handle for the EFI image.
 @param [in]   SystemTable      A pointer to the EFI System Table.

 @return Status code

**/
EFI_STATUS
EFIAPI
InstallSdevDxeEntryPoint (
  IN EFI_HANDLE          ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_STATUS          Status;
  EFI_EVENT           Event;

  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             SdevNotifyFunction,
             NULL,
             &Event
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "CreateEventReadyToBoot(%r)\n", Status));
  }
  return Status;
}
