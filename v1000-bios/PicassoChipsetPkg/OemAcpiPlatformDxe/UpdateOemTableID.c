/** @file

;*******************************************************************************
;* Copyright (c) 2013 - 2017, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#include "OemAcpiPlatform.h"
#include <Protocol/AcpiTable.h>
#include <IndustryStandard/Acpi.h>
#include <Library/H2OCpLib.h>
#include <Guid/H2OCp.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>

/**
 Update OEM Table ID information

 @param[in] Event                      The Event this notify function registered to.
 @param[in] Handle                     The handle associated with a previously registered checkpoint handler.

**/
VOID
EFIAPI
UpdateAcpiDescHdrCallback (
  IN EFI_EVENT                        Event,
  IN H2O_CP_HANDLE                    Handle
  )
{
  EFI_STATUS                                        Status;
  UINTN                                             Length;

  CHAR8                                             *AcpiDefaultOemId;
  H2O_BDS_CP_UPDATE_ACPI_DESC_HDR_DATA              *BdsCpUpdateAcpiDescHdrData;
  EFI_GUID                                          BdsCpUpdateAcpiDescHdrGuid;
  EFI_ACPI_SDT_HEADER                               *Table;
  UINT64                                            AcpiDefaultOemTableId;

  //
  // Get checkpoint data by H2OCpLookup() if need use checkpoint data in callback function
  //
  Status = H2OCpLookup (Handle, (VOID **) &BdsCpUpdateAcpiDescHdrData, &BdsCpUpdateAcpiDescHdrGuid);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Checkpoint Data Not Found: %x (%r)\n", Handle, Status));
    DEBUG ((EFI_D_ERROR, "   %a\n", __FUNCTION__));
    return;
  }

  BdsCpUpdateAcpiDescHdrData->Status = H2O_CP_TASK_UPDATE;
  Table = BdsCpUpdateAcpiDescHdrData->AcpiTableHeader;

  if (Table->Signature == EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE) {
    return;
  }

  //
  // Update OemId
  //
  AcpiDefaultOemId = (CHAR8 *) PcdGetPtr (PcdAcpiDefaultOemId);
  if (AcpiDefaultOemId == NULL) {
    DEBUG ((EFI_D_INFO, "PcdAcpiDefaultOemId is NULL.\n"));
  }
  Length = (AcpiDefaultOemId == NULL) ? 0 : AsciiStrLen (AcpiDefaultOemId);
  if (AcpiDefaultOemId != NULL) {
    if ((Length > sizeof (Table->OemId))) {
      Length = sizeof (Table->OemId);
      DEBUG ((EFI_D_INFO, "OemId length is invalid.\n"));
    }
    ZeroMem (Table->OemId, Length);
    CopyMem (Table->OemId, AcpiDefaultOemId, Length);
  }

  //
  // Update OemTableId
  //
  AcpiDefaultOemTableId = PcdGet64 (PcdAcpiDefaultOemTableId);
  CopyMem (Table->OemTableId, &AcpiDefaultOemTableId, sizeof (UINT64));

  //
  // Creator ID & Revision are used for ASL compiler
  //
  Table->CreatorId              = SIGNATURE_32 ('A', 'C', 'P', 'I');
  Table->CreatorRevision        = 0x00040000;

  DEBUG ((EFI_D_INFO, "Checkpoint Data Updated: %g\n", &BdsCpUpdateAcpiDescHdrGuid));
  DEBUG ((EFI_D_INFO, "   %a\n", __FUNCTION__));
}

/**
 Update OEM Table ID information for all ACPI TAbles

 @param None

 @retval EFI_SUCCESS            Update Table ID Success

**/
EFI_STATUS
UpdateOemTableID (
  VOID
  )
{
  EFI_STATUS  Status;
  
  Status = EFI_SUCCESS;
  if (PcdGetBool (PcdH2OBdsCpUpdateAcpiDescHdrSupported)) {
    H2O_CP_HANDLE   CpHandle;

    Status = H2OCpRegisterHandler (
               &gH2OBdsCpUpdateAcpiDescHdrGuid,
               UpdateAcpiDescHdrCallback,
               H2O_CP_MEDIUM_HIGH,
               &CpHandle
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Checkpoint Register Fail: %g (%r)\n", &gH2OBdsCpUpdateAcpiDescHdrGuid, Status));
      return Status;
    }
    DEBUG ((EFI_D_INFO, "Checkpoint Registered: %g (%r)\n", &gH2OBdsCpUpdateAcpiDescHdrGuid, Status));
  }
  return EFI_SUCCESS;
}

