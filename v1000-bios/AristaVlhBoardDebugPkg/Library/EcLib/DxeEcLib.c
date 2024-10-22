/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "SharedEcLib.h"
#include <Library/DxeServicesTableLib.h>


/**
  EC initialize.

  @retval     EFI_UNSUPPORTED     Returns unsupported by default.
  @retval     EFI_SUCCESS         The service is customized in the project.
  @retval     EFI_MEDIA_CHANGED   The value of IN OUT parameter is changed.
**/
EFI_STATUS
EcInit (
  )
{
  EFI_STATUS                      Status;
  UINTN                           Address;

  //
  // Add ACPI_MMIO to Runtime memory.
  //
  Address = ACPI_MMIO_BASE + MISC_BASE;
  Status = gDS->SetMemorySpaceAttributes (
                (EFI_PHYSICAL_ADDRESS) (Address & (~EFI_PAGE_MASK)),
                0x1000,
                EFI_MEMORY_RUNTIME
                );

  return Status;
}


EFI_STATUS
EFIAPI
DxeEcLibConstructor (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EcInit ();
  return EFI_SUCCESS;
}

