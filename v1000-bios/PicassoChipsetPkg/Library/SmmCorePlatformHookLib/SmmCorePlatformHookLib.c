/** @file
  Library is used in DDT for stopping WDT.

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

/**
  Copyright (c) 2011, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/SmmCorePlatformHookLib.h>
#include <FchRegistersCommon.h>

/**
  This function will stop watchdog in WDT register.

**/
VOID
ControlWatchdogFlag (
  BOOLEAN                                   CpuWdtEn
  )
{
  UINT32                                    Value32;
  //
  // Make sure WDT has already executed.
  // 
  if (MmioRead32 (ACPI_MMIO_BASE + WATCHDOG_BASE + FCH_PMIOA_REG04) > 0) {
    if (CpuWdtEn) {
      //
      // Enable Fch TMR WDT Bit0 WatchdogRunStopB 
      //
      Value32 = MmioRead32 (ACPI_MMIO_BASE + WATCHDOG_BASE + FCH_PMIOA_REG00);
      Value32 |= BIT0;
      MmioWrite32(ACPI_MMIO_BASE + WATCHDOG_BASE + FCH_PMIOA_REG00, Value32);
      //
      // Enable Fch TMR WDT Bit7 WatchdogTrigger 
      //
      Value32 = MmioRead32 (ACPI_MMIO_BASE + WATCHDOG_BASE + FCH_PMIOA_REG00);
      Value32 |= BIT7;
      MmioWrite32(ACPI_MMIO_BASE + WATCHDOG_BASE + FCH_PMIOA_REG00, Value32);
    } else {
      //
      // Disable Fch TMR WDT Bit0 WatchdogRunStopB 
      //
      Value32 = MmioRead32 (ACPI_MMIO_BASE + WATCHDOG_BASE + FCH_PMIOA_REG00);
      Value32 &= ~(BIT0);
      MmioWrite32(ACPI_MMIO_BASE + WATCHDOG_BASE + FCH_PMIOA_REG00, Value32);
    } 
  }   
}

/**
  Performs platform specific tasks before invoking registered SMI handlers.
  
  This function performs platform specific tasks before invoking registered SMI handlers.
  
  @retval EFI_SUCCESS       The platform hook completes successfully.
  @retval Other values      The paltform hook cannot complete due to some error.

**/
EFI_STATUS
EFIAPI
PlatformHookBeforeSmmDispatch (
  VOID
  )
{
  ControlWatchdogFlag (FALSE);
  return EFI_SUCCESS;
}

/**
  Performs platform specific tasks after invoking registered SMI handlers.
  
  This function performs platform specific tasks after invoking registered SMI handlers.
  
  @retval EFI_SUCCESS       The platform hook completes successfully.
  @retval Other values      The paltform hook cannot complete due to some error.

**/
EFI_STATUS
EFIAPI
PlatformHookAfterSmmDispatch (
  VOID
  )
{
  ControlWatchdogFlag (TRUE);
  return EFI_SUCCESS;
}

