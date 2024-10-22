/** @file
  SPI Access Init routines

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/DxeServicesTableLib.h>

/**
  Initialization routine for SpiAccessLib

  @param  BaseAddress           SPI Base Address.

  @retval EFI_SUCCESS           SpiAccessLib successfully initialized
  @return Others                SpiAccessLib initialization failed

**/
EFI_STATUS
EFIAPI
SpiAccessInit (
  IN  EFI_PHYSICAL_ADDRESS      BaseAddress
  )
{
  UINTN                Length;
  EFI_STATUS           Status;

  //
  // Reserve SPI BAR space in GCD, which will be reported to OS by E820
  // It will assert if SPI BAR Memory Space is not allocated
  // The caller is responsible for the existence and allocation of the SPI BAR space Memory Spaces.
  //
  Length = 0x1000;
  Status = gDS->SetMemorySpaceAttributes (
                    BaseAddress,
                    Length,
                    EFI_MEMORY_RUNTIME
                    );
  return Status;
}
