/** @file
  Library classes for enabling/disabling flash write access

;******************************************************************************
;* Copyright (c) 2012 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/FdSupportLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/BaseOemSvcKernelLib.h>
#include <Library/AmdPspBaseLibV2.h>
#include <Fch.h>

UINT32    mSmi98;
STATIC UINT32 SmiRegxA8;

STATIC
BOOLEAN
IsAcpiOn (
  VOID
  )
{
  UINT16  PmCntBlk;
  UINT32  PmControl;

  PmCntBlk = MmioRead16 (0xFED80362); // AcpiPm1CntBlk
  PmControl = IoRead32 (PmCntBlk);

  if (PmControl & BIT0) {             // SciEn
    return TRUE;
  }

  return FALSE;
}

STATIC
VOID
SmiEnDis (
  BOOLEAN SmiEn
  )
{

  if (SmiEn) {
      MmioWrite32 (0xFED80298, mSmi98);               // Actually, we don't clear SmieEn bit to enable Smi, just set it to original value
  } else {
      mSmi98 = MmioRead32 (0xFED80298);
      MmioWrite32 (0xFED80298, mSmi98 | 0x80000000);
  }
}

EFI_STATUS
EnableFvbWrites (
  BOOLEAN EnableWrites
  )
{
  if (IsAcpiOn()) {
    if (EnableWrites) {
      OemSvcEcWait (TRUE);
      SmiEnDis (FALSE);                 // set SmiEn bit to block fake smi trigger from psp (even block all of smi trigger)
    } else {
      OemSvcEcWait (FALSE);
      SmiEnDis (TRUE);                  // restore previous SmiEn bit setting
    }
  }
  else {
    if (EnableWrites) {
      SmiEnDis (FALSE);                 // set SmiEn bit to block fake smi trigger from psp (even block all of smi trigger)
    } else {
      SmiEnDis (TRUE);                  // restore previous SmiEn bit setting
    }
  }
  return EFI_SUCCESS;
}

/**
  Enable flash device write access

  @param  FlashDevice           Pointer to the FLASH_DEVICE structure

  @retval EFI_SUCCESS           Flash device write access was successfully enabled
  @return Others                Failed to enable flash device write access

**/
EFI_STATUS
EFIAPI
FlashWriteEnable (
  IN FLASH_DEVICE               *FlashDevice
  )
{
  EnableFvbWrites(TRUE);

  AcquirePspSmiRegMutexV2 ();
  
  SmiRegxA8 = ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGA8);         // Save original setting
  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGA8) &= ~(BIT3 + BIT2);   // Disable Fake SMI
  
  ReleasePspSmiRegMutexV2 ();

  return EFI_SUCCESS;
}

/**
  Disable flash device write access

  @param  FlashDevice           Pointer to the FLASH_DEVICE structure

  @retval EFI_SUCCESS           Flash device write access was successfully disabled
  @return Others                Failed to disable flash device write access

**/
EFI_STATUS
EFIAPI
FlashWriteDisable (
  IN FLASH_DEVICE               *FlashDevice
  )
{
  EnableFvbWrites (FALSE);

  AcquirePspSmiRegMutexV2 ();

  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGA8) = SmiRegxA8;         // Restore original setting
  
  ReleasePspSmiRegMutexV2 ();
  
  return EFI_SUCCESS;
}

