/** @file
 This initializes all devices that needs to be initialized in PEI.

;******************************************************************************
;* Copyright 2017 Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/

#include <SioDummyPei.h>

STATIC
EFI_PEI_PPI_DESCRIPTOR  mSioInitializedPpi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiSioInitializedPpiGuid,
  NULL
};

/**
  SIO XXX PEIM entry point.

  Create "ChipInformation" structure
  Program I/O decode range at SoC if needed
  Get "SioXXXChip1Setup" or "SioXXXChip2Setup" variable
  Deal with special KBC case when the chip vendor is ITE
  Initialize keyboard, mouse, watchdog timer and GPIO if needed
  Program OEM customized demands

  @param[in]       FileHandle               FileHandle  Handle of the file being invoked
  @param[in]       PeiServices              PeiServices Describes the list of possible PEI Services

  @retval          EFI_SUCCESS              Function complete successfully
  @retval          EFI_NO_MEDIA             Get Sio chip information failed

**/
EFI_STATUS
EFIAPI
SioDummyPeimEntry (
  IN       EFI_PEI_FILE_HANDLE    FileHandle,
  IN CONST EFI_PEI_SERVICES       **PeiServices
  )
{
  EFI_STATUS Status;

  //
  // The PostCode 70h that is Super I/O PEI Module Entry.
  //
  PostCode (PEI_SIO_INIT);
  DEBUG ((EFI_D_INFO, "--SIO PEI Module: %a Start--\n", __FUNCTION__));

#if FixedPcdGetBool (PcdH2OSioDummyChip1Uart1Supported)
  IoDecodeControl (FixedPcdGet16 (PcdH2OSioDummyChip1Uart1Base), 0x08);
#endif
#if FixedPcdGetBool (PcdH2OSioDummyChip1Uart2Supported)
  IoDecodeControl (FixedPcdGet16 (PcdH2OSioDummyChip1Uart2Base), 0x08);
#endif
#if FixedPcdGetBool (PcdH2OSioDummyChip1KbcSupported)
  IoDecodeControl (SIO_KBC_DATA_PORT, 0x01);
  IoDecodeControl (SIO_KBC_COMMAND_PORT, 0x01);
#endif

  Status = (**PeiServices).InstallPpi (PeiServices, &mSioInitializedPpi);

  DEBUG ((EFI_D_INFO, "--SIO PEI Module: %a End--\n", __FUNCTION__));
  return Status;
}
