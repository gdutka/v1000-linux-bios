/** @file

;******************************************************************************
;* Copyright (c) 2017, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include <Uefi.h>
#include <SecureFlash.h>
#include <FchRegs.h>

#include <Library/ProcessPendingCapsuleLib.h>
#include <Library/IoLib.h>
#include <Library/VariableLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>


BOOLEAN     *msetEspFlashFlag = FALSE;

EFI_STATUS
SetEspFlashFlag(
  VOID
  )
{
  
  *msetEspFlashFlag = TRUE;
  DEBUG ((EFI_D_INFO, "set msetEspFlashFlag = %x\n",*msetEspFlashFlag));
  return EFI_SUCCESS;
}


EFI_STATUS
ProcessPendingCapsule (
  VOID
  )
{
  EFI_STATUS                    Status;
  IMAGE_INFO                    ImageInfo;
  UINTN                         Size;
  UINT16                        AcpiPmCntBase;

  Status        = EFI_NOT_FOUND;
  Size          = 0;
  AcpiPmCntBase = 0;

  DEBUG ((EFI_D_INFO, "ProcessPendingCapsule Entry\n"));

  //
  // Check CapsuleUpdate request.
  //
  Size = sizeof (IMAGE_INFO);
  Status = CommonGetVariable (
             SECURE_FLASH_INFORMATION_NAME,
             &gSecureFlashInfoGuid,
             &Size,
             &ImageInfo
             );
  DEBUG ((EFI_D_INFO, "  Get SECURE_FLASH_INFORMATION_NAME. (%r)\n", Status));
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG ((EFI_D_INFO, "  FlashMode = 0x%x\n", (UINT8)ImageInfo.FlashMode));
  if (!ImageInfo.FlashMode) {
    return EFI_UNSUPPORTED;
  }

  DEBUG ((EFI_D_INFO, "  msetEspFlashFlag = %x\n", *msetEspFlashFlag));

  if (*msetEspFlashFlag == TRUE) {
    DEBUG ((EFI_D_INFO, "  ESP Flag is TRUE should return\n"));
    return EFI_UNSUPPORTED;
  }
  //
  // Flash mode found, enable S3.
  //
  AcpiPmCntBase = MmioRead16 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG62);
  DEBUG ((EFI_D_INFO, "  AcpiPmCntBase = 0x%x\n", AcpiPmCntBase));

  //
  // Enable S3.
  //
  IoWrite16 (AcpiPmCntBase, (UINT16)(V_SLP_TYPE_S3 | V_ACPI_SLP_EN));
  IoWrite8 (0xED, 0xFF);

  DEBUG ((EFI_D_INFO, "  SleepTypeValue = 0x%x\n", IoRead16 (AcpiPmCntBase)));

  //
  // Check SlpTypeEvent65 from SMIx88 SmiStatus2.
  //
  DEBUG ((EFI_D_INFO, "  SmiStatus2 = 0x%x\n", MmioRead32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG88)));


  DEBUG ((EFI_D_INFO, "ProcessPendingCapsule Exit\n"));

  
  return EFI_SUCCESS;

}
