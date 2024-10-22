/** @file

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiDxe.h>

#include <Protocol/Cpu.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

//
// Worker Functions
//

/**
  Unexpected exception #F Handler.

  @param InterruptType    The type of interrupt that occured
  @param SystemContext    A pointer to the system context when the interrupt occured
**/
VOID
EFIAPI
DummyInterruptHandler (
  IN EFI_EXCEPTION_TYPE   InterruptType,
  IN EFI_SYSTEM_CONTEXT   SystemContext
  )
{
}

/**
  Initialize Dummy Interrupt Handler for Unexpected exception #F

  @param ImageHandle     ImageHandle of the loaded driver
  @param SystemTable     Pointer to the System Table

  @retval EFI_SUCCESS            Successfully Initialized.
  @retval EFI_OUT_OF_RESOURCES   Not enough resources available to initialize driver.
  @retval EFI_DEVICE_ERROR       A device error occured attempting to initialize the driver.

**/
EFI_STATUS
EFIAPI
UnexpectedIrqWA (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS               Status;
  EFI_CPU_ARCH_PROTOCOL    *Cpu;

  //
  // Find the CPU architectural protocol.
  //
  Status = gBS->LocateProtocol (&gEfiCpuArchProtocolGuid, NULL, (VOID **) &Cpu);
  ASSERT_EFI_ERROR (Status);

  //
  // Install interrupt handler for exception #F (WA unexpected IRQ7)
  //
  Status = Cpu->RegisterInterruptHandler (Cpu, 0x0F, DummyInterruptHandler);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

