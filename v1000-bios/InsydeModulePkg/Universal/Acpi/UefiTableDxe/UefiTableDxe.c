/** @file
  Install UEFI ACPI table which contains BVDT(BIOS Version Date and Time) data.

;******************************************************************************
;* Copyright (c) 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Protocol/AcpiTable.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/FlashRegionLib.h>
#include <AcpiHeaderDefaultValue.h>
#include "UefiTableDxe.h"

EFI_ACPI_UEFI_TABLE mUefiTable = {
  {
    {
      UEFI_ACPI_TABLE_SIGNATURE,
      sizeof (EFI_ACPI_UEFI_TABLE),
      1,
      //
      // Checksum will be updated at runtime
      //
      0x00,
      EFI_ACPI_OEM_ID,
      EFI_ACPI_OEM_TABLE_ID,
      EFI_ACPI_OEM_REVISION,
      EFI_ACPI_CREATOR_ID,
      EFI_ACPI_CREATOR_REVISION
    },
    UEFI_ACPI_TABLE_INSYDE_BIOS_GUID,
    0
  },
    
  //
  // BVDT Data fields
  //
  {
    0
  }
};



/**
 This function calculates and updates an UINT8 checksum.

 @param [in]   Buffer
 @param [in]   Size             Number of bytes to checksum.

 @retval None

**/
VOID
UefiAcpiTableChecksum (
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
 Install UEFI ACPI table which contains BVDT data.

 @param [in]   Event
 @param [in]   Context

 @retval None

**/
VOID
EFIAPI
InstallUefiTable (
  IN EFI_EVENT        Event,
  IN VOID             *Context
)
{
  EFI_STATUS                        Status;
  UINTN                             TableHandle;
  EFI_ACPI_TABLE_PROTOCOL           *AcpiTable;
 
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&AcpiTable);
  if (EFI_ERROR (Status)) {
    return;
  }

  mUefiTable.UefiAcpiDataTable.DataOffset = (UINT16) ((UINTN) mUefiTable.Data - (UINTN) &mUefiTable);
  CopyMem (
    mUefiTable.Data,
    (VOID *) (UINTN) FdmGetNAtAddr (&gH2OFlashMapRegionBvdtGuid, 1),
    (UINTN) BVDT_SIZE
    );
  
  UefiAcpiTableChecksum ((UINT8 *) &mUefiTable, sizeof (mUefiTable));

  TableHandle = 0x00;
  Status = AcpiTable->InstallAcpiTable (
                        AcpiTable,
                        &mUefiTable,
                        sizeof (mUefiTable),
                        &TableHandle
                        );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "InstallAcpiTable(%r)\n", Status));
  }
}

/**
  Entry point for the driver.

 @param [in]   ImageHandle      The firmware allocated handle for the EFI image.
 @param [in]   SystemTable      A pointer to the EFI System Table.

 @retval       EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
UefiTableDxeEntryPoint (
  IN EFI_HANDLE          ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
  )
{
  VOID         *Registration;
 
  EfiCreateProtocolNotifyEvent (
    &gEfiAcpiTableProtocolGuid,
    TPL_CALLBACK,
    InstallUefiTable,
    NULL,
    &Registration
    );

  return EFI_SUCCESS;
}
