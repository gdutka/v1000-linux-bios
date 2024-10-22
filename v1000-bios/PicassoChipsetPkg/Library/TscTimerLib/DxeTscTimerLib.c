/** @file
  A Dxe Timer Library implementation which uses the Time Stamp Counter in the processor.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

#include <PiDxe.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Guid/TscFrequency.h>
#include "TscTimerLibInternal.h"

UINT64 mTscFrequency;

/** The constructor function determines the actual TSC frequency.

  First, Get TSC frequency from system configuration table with TSC frequency GUID,
  if the table is not found, install it.
  This function will always return EFI_SUCCESS.

  @param  ImageHandle       The firmware allocated handle for the EFI image.
  @param  SystemTable       A pointer to the EFI System Table.

  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
DxeTscTimerLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  UINT64      *TscFrequency;

  //
  // Get TSC frequency from system configuration table with TSC frequency GUID.
  //
  Status = EfiGetSystemConfigurationTable (&gEfiTscFrequencyGuid, (VOID **) &TscFrequency);
  if (Status == EFI_SUCCESS) {
    mTscFrequency = *TscFrequency;
    return EFI_SUCCESS;
  }

  //
  // TSC frequency GUID system configuration table is not found, install it.
  //

  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (UINT64), (VOID **) &TscFrequency);
  ASSERT_EFI_ERROR (Status);

  *TscFrequency = InternalCalculateTscFrequency ();
  //
  // TscFrequency now points to the number of TSC counts per second, install system configuration table for it.
  //
  gBS->InstallConfigurationTable (&gEfiTscFrequencyGuid, TscFrequency);

  mTscFrequency = *TscFrequency;
  return EFI_SUCCESS;
}

/**  Get TSC frequency.

  @return The number of TSC counts per second.

**/
UINT64
InternalGetTscFrequency (
  VOID
  )
{
  return mTscFrequency;
}

