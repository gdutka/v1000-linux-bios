/** @file
  This driver provides Bios Region Lock in DXE

;*******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/
#include <BiosRegionLockInfo.h>
#include <BiosRegionLockHelpFun.h>
#include <ChipsetSmiTable.h>
#include <Library/IoLib.h>
//[-start-190618-IB06070035-add]//
#include <Library/DebugLib.h>
//[-end-190618-IB06070035-add]//

EFI_STATUS
SetRegionByType (
  IN BIOS_REGION_LOCK_PROTOCOL  *This,
  IN BIOS_REGION_TYPE            Type
  )
/*++

Routine Description:

  This function set the BIOS region by requested type to the BIOS protect region register.

Arguments:

  This - Protocol instance pointer.
  Type - The BIOS Region type which need to be locked.

Returns:

  EFI_ACCESS_DENIED      - The BIOS protect region register has been locked.
  EFI_INVALID_PARAMETER  - The input Type is invalid.
  EFI_OUT_OF_RESOURCES   - All BIOS protect region registers have been set and the requested region
                           can not be merge to existing protected region.
  EFI_SUCCESS            - Set BIOS protect region register successfully

--*/
{
  return EFI_UNSUPPORTED;
}


EFI_STATUS
SetRegionByAddress (
  IN BIOS_REGION_LOCK_PROTOCOL  *This,
  IN UINTN                       BaseAddress,
  IN UINTN                       Length
  )
/*++

Routine Description:

  This function sets the BIOS region by requested base address and legnth to the BIOS protect region register.

Arguments:

  This        - Protocol instance pointer.
  BaseAddress - The start address of the BIOS region which need to be locked.
  Length      - The Length of the BIOS region which need to be locked.

Returns:

  EFI_ACCESS_DENIED      - The BIOS protect region register has been locked.
  EFI_OUT_OF_RESOURCES   - All BIOS protect region registers have been set and the requested region
                           can not be merge to existing protected region.
  EFI_SUCCESS            - Set BIOS protect region register successfully

--*/
{
  BIOS_REGION_LOCK_INSTANCE  *Private;
  EFI_STATUS                  Status;

  if (Length == 0) {
    return EFI_INVALID_PARAMETER;
  }

  Private = BIOS_REGION_LOCK_INSTANCE_FROM_THIS (This);
  Status  = AddNewRegion (Private->BiosLock, BaseAddress, Length);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
ClearRegionByType (
  IN BIOS_REGION_LOCK_PROTOCOL  *This,
  IN BIOS_REGION_TYPE            Type
  )
/*++

Routine Description:

  This function removes the BIOS region by requested type from the BIOS protect region register.

Arguments:

  This - Protocol instance pointer.
  Type - The BIOS Region type which need to be locked.

Returns:

  EFI_ACCESS_DENIED      - The BIOS protect region register has been locked.
  EFI_INVALID_PARAMETER  - The input Type is invalid.
  EFI_OUT_OF_RESOURCES   - The BIOS protect region registers are not enough to set for all discontinuous BIOS region
  EFI_SUCCESS            - Set BIOS protect region register successfully

--*/
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
ClearRegionByAddress (
  IN BIOS_REGION_LOCK_PROTOCOL  *This,
  IN UINTN                       BaseAddress,
  IN UINTN                       Length
  )
/*++

Routine Description:

  This function removes the BIOS region by requested base address and legnth from the BIOS protect region register.

Arguments:

  This        - Protocol instance pointer.
  BaseAddress - The start address of the BIOS region which need to be locked.
  Length      - The Length of the BIOS region which need to be locked.

Returns:

  EFI_ACCESS_DENIED      - The BIOS protect region register has been locked.
  EFI_OUT_OF_RESOURCES   - The BIOS protect region registers are not enough to set for all discontinuous BIOS region
  EFI_SUCCESS            - Set BIOS protect region register successfully

--*/
{

  return EFI_UNSUPPORTED;
}

EFI_STATUS
Lock (
  IN BIOS_REGION_LOCK_PROTOCOL  *This
  )
/*++

Routine Description:

  This function locks BIOS region described by PR0 to PR5 and trigger software SMI to enable BIOS Region SMM Protection.

Arguments:

  This        - Protocol instance pointer.

Returns:

  EFI_SUCCESS

--*/
{
  EFI_STATUS                 Status;
  BIOS_REGION_LOCK_INSTANCE  *Private;

  Private = BIOS_REGION_LOCK_INSTANCE_FROM_THIS (This);

  //
  // Generate the software SMI to program ROM protect registers.
  //
  IoWrite8 (SW_SMI_PORT, SPI_ROM_PROTECT_PROGRAM);

  Status = CreateS3ResumeScriptTable ();
  if (EFI_ERROR (Status)) {
   #ifdef EFI_DEBUG
       DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\nProgram Register failed and Lock SpiProtectLock failed: %r\n", Status));
   #endif
    return Status;
  }

  return EFI_SUCCESS;
}
