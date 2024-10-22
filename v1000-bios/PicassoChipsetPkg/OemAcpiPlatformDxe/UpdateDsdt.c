/** @file

   This module depends on OEM project request to update DSDT table content.

;*******************************************************************************
;* Copyright (c) 2015, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#include "OemAcpiPlatform.h"

#include <IndustryStandard/Acpi.h>
#include <OemGlobalNvsArea.h>

#include <Protocol/GlobalNvsArea.h>

#include <Library/AcpiPlatformLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/KernelConfigLib.h>
#include <Library/ChipsetConfigLib.h>


/**
  Depends on OEM project request to update DSDT table content.

  @retval           EFI_SUCCESS         Successfully update table.
                    EFI_NOT_FOUND       Failed to udpate the table.

*/
EFI_STATUS
CrbUpdateOemDsdt (
  VOID
  )
{
  EFI_STATUS                            Status;
  BOOLEAN                               DsdtUpdated;
  BOOLEAN                               DsdtFound;
  BOOLEAN                               OGNSFound;
  EFI_ACPI_DESCRIPTION_HEADER           *Table;
  EFI_GLOBAL_NVS_AREA_PROTOCOL          *GlobalNvsAreaProtocol;
  EFI_OEM_GLOBAL_NVS_AREA               *mOemGlobalNvsArea;
  INTN                                  Index;
  UINTN                                 Handle;
  EFI_ACPI_TABLE_VERSION                Version;
  UINT8                                 *DsdtPointer;
  UINT32                                Signature;
  KERNEL_CONFIGURATION                  KernelConfiguration;
  CHIPSET_CONFIGURATION                 ChipSetConfiguration;
  UINTN                                    Size;
  UINT8                                    BootType;

  //
  // Locate ACPI support protocol
  //
  Status = gBS->LocateProtocol (&gEfiAcpiSupportProtocolGuid, NULL, (VOID **)&mAcpiSupport);
  ASSERT_EFI_ERROR (Status);

  Size = sizeof (UINT8);
  Status = gRT->GetVariable (
                  L"BootType",
                  &gSystemConfigurationGuid,
                  NULL,
                  &Size,
                  &BootType
                  );

  DsdtUpdated = FALSE;
  DsdtFound = FALSE;
  OGNSFound = FALSE;

  //
  // Locate EFI_GLOBAL_NVS_AREA_PROTOCOL protocol.
  //
  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID**)&GlobalNvsAreaProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  mOemGlobalNvsArea = GlobalNvsAreaProtocol->OemArea;

  Status = GetKernelConfiguration (&KernelConfiguration);
  ASSERT_EFI_ERROR (Status);

  Status = GetChipsetConfiguration (&ChipSetConfiguration);
  ASSERT_EFI_ERROR (Status);

  //
  // While loop is to find ACPI tables.
  // Inner for-loop is to go through an ACPI table.
  //
  Index = 0;
  while (!DsdtFound) {
    Status = mAcpiSupport->GetAcpiTable (mAcpiSupport, Index, (VOID**)&Table, &Version, &Handle);
    if (Status == EFI_NOT_FOUND) {
      break;
    }
    
    ASSERT_EFI_ERROR (Status);
    ++Index;
    
    if (Table->Signature != EFI_ACPI_3_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) {
      FreePool (Table);
      continue;
    }

    for (DsdtPointer = (UINT8*)((UINTN) Table + sizeof (EFI_ACPI_DESCRIPTION_HEADER));
          (!OGNSFound) && (DsdtPointer <= (UINT8*)((UINTN) Table + (UINTN) (Table->Length))); ++DsdtPointer) {

      Signature = *((UINT32*) DsdtPointer);
      switch (Signature) {

      case (SIGNATURE_32 ('O', 'G', 'N', 'S')):
        if (IsAmlOpRegionObject (DsdtPointer)) {
          SetOpRegion (DsdtPointer, mOemGlobalNvsArea, sizeof (EFI_OEM_GLOBAL_NVS_AREA));
          OGNSFound = TRUE;
        }
        break;

      case (SIGNATURE_32 ('X', 'B', 'I', 'X')):
        if (BootType == 2) {				// EFI boot type for Win 8
          *((UINT32*)DsdtPointer) = SIGNATURE_32 ('_', 'B', 'I', 'X');
        }
        break;

      case (SIGNATURE_32 ('X', 'B', 'T', 'P')):
        if (BootType == 2) {				// EFI boot type for Win 8
          *((UINT32*)DsdtPointer) = SIGNATURE_32 ('_', 'B', 'T', 'P');
        }
        break;

      case (SIGNATURE_32 ('_', 'B', 'I', 'F')):
        if (BootType == 2) {				// EFI boot type for Win 8
          *((UINT32*)DsdtPointer) = SIGNATURE_32 ('X', 'B', 'I', 'F');
        }
        break;

      default:
        break;
      }
    }

    if (OGNSFound == TRUE) {
      Status = mAcpiSupport->SetAcpiTable (mAcpiSupport, Table, TRUE, Version, &Handle);
      DsdtUpdated = TRUE;
    }
    FreePool (Table);
    
    DsdtFound = TRUE;
  }

  if (DsdtUpdated) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}

