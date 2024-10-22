/** @file
   Seamless Recovery support library for Windows UEFI Firmware Update Platform

;******************************************************************************
;* Copyright (c) 2013 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/FdSupportLib.h>
#include <Library/FlashRegionLib.h>
#include <Protocol/SmmFwBlockService.h>
#include <Library/UefiBootServicesTableLib.h>

STATIC  UINT64       mFtwBackupAddress;

/**
  Check whether it is in firmware failure recovery mode

  @param  None

  @retval TRUE      The system is in firmware failure recovery mode
          FALSE     The system is not in firmware failure recovery mode

**/
BOOLEAN
IsFirmwareFailureRecovery (
  VOID
  )
{
  EFI_GUID *RecoverySig;

  RecoverySig = (EFI_GUID *)(UINTN)mFtwBackupAddress;
  if (RecoverySig != NULL && CompareGuid (PcdGetPtr (PcdSeamlessRecoverySignature), RecoverySig)) {
    return TRUE;
  }

  return FALSE;
}

/**
  Set firmware updating in process signature

  @param  None

  @returns None

**/
VOID
SetFirmwareUpdatingFlag (
  BOOLEAN FirmwareUpdatingFlag
  )
{
  EFI_STATUS  Status;
  UINTN       Size;
  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL     *SmmFwbService;
  UINTN                                 BlocksSize;

  if ((!FirmwareUpdatingFlag) && (!IsFirmwareFailureRecovery())) {
    return;
  }

  BlocksSize = GetFlashBlockSize ();

  Status = gBS->LocateProtocol (
                  &gEfiSmmFwBlockServiceProtocolGuid,
                  NULL,
                  (VOID **)&SmmFwbService
                  );
  if (EFI_ERROR (Status)) {
    Status = FlashErase ((UINTN)mFtwBackupAddress, BlocksSize);
    if (EFI_ERROR(Status)) {
      ASSERT_EFI_ERROR(Status);
      return;
    }
    if (FirmwareUpdatingFlag) {
      Size = sizeof(EFI_GUID);
      FlashProgram (
        (UINT8 *)(UINTN)mFtwBackupAddress,
        PcdGetPtr(PcdSeamlessRecoverySignature),
        &Size,
        (UINTN)mFtwBackupAddress
        );
    }
  } else {

    Status = SmmFwbService->EraseBlocks(
                              SmmFwbService,
                              (UINTN)mFtwBackupAddress,
                              &BlocksSize
                              );
    if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
      return;
    }
    if (FirmwareUpdatingFlag) {
      Size = sizeof (EFI_GUID);
      Status = SmmFwbService->Write (
                                SmmFwbService,
                                (UINTN)mFtwBackupAddress,
                                (UINTN *)&Size,
                                (UINT8 *)PcdGetPtr (PcdSeamlessRecoverySignature)
                                );
    }
  }
}

/**
  The constructor function stores the FDM information to prevent from the
  FDM location is changed after BIOS is updated.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The constructor always returns EFI_SUCCESS.
**/
EFI_STATUS
EFIAPI
SeamlessRecoveryLibConstructor (
  IN      EFI_HANDLE                ImageHandle,
  IN      EFI_SYSTEM_TABLE          *SystemTable
  )
{

  mFtwBackupAddress = FdmGetNAtAddr (&gH2OFlashMapRegionFtwBackupGuid ,1);
  return EFI_SUCCESS;
}