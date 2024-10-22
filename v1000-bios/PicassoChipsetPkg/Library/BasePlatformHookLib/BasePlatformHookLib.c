/** @file
  Platform Hook Library instance.

  Copyright (c) 2010, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>
#include <Library/PlatformHookLib.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>

#define SMSC_1036_LPC_INDEX     0x164E
#define SMSC_1036_LPC_DATA      0x164F

#define SMSC_1036_LPC_ID_INDEX  0x0D
#define SMSC_1036_LPC_ID_DATA   0x82
#define SMSC_1036_LPC_CR02      0x02
#define SMSC_1036_LPC_CR0C      0x0C
#define SMSC_1036_LPC_CR25      0x25
#define SMSC_1036_LPC_CR28      0x28
#define SMSC_1036_LPC_ADDR      0x3F8
#define SMSC_1036_LPC_IRQ       0x04

BOOLEAN
EFIAPI
IsSio1036 (
  VOID
  )
{
  UINT8 	  Data8 = 0;

  //
  // Enter Conf-mode
  //
  IoWrite8 (SMSC_1036_LPC_INDEX, 0x55);

  //
  // Check ID
  //
  IoWrite8 (SMSC_1036_LPC_INDEX, SMSC_1036_LPC_ID_INDEX);
  Data8 = IoRead8 (SMSC_1036_LPC_DATA);

  //
  // Exit Conf-mode
  //
  IoWrite8 (SMSC_1036_LPC_INDEX, 0xAA);

  return ((Data8 == SMSC_1036_LPC_ID_DATA) ? TRUE:FALSE);
}

VOID
EnSio1036Uart (
  )
{
  UINT8 	  Data8 = 0;

  //
  // Enter Conf-mode
  //
  IoWrite8 (SMSC_1036_LPC_INDEX, 0x55);

  //
  // Turn on UART power
  //
  IoWrite8 (SMSC_1036_LPC_INDEX, SMSC_1036_LPC_CR02);
  Data8 = IoRead8 (SMSC_1036_LPC_DATA);
  Data8 |= 0x80;
  IoWrite8 (SMSC_1036_LPC_DATA, Data8);

  //
  // UART mode
  //
  IoWrite8 (SMSC_1036_LPC_INDEX, SMSC_1036_LPC_CR0C);
  Data8 = IoRead8 (SMSC_1036_LPC_DATA);
  Data8 &= ~(0x38);       // Standard Com
  Data8 |= 0x80;          // High Speed
  IoWrite8 (SMSC_1036_LPC_DATA, Data8);

  //
  // UART Address
  //
  IoWrite8 (SMSC_1036_LPC_INDEX, SMSC_1036_LPC_CR25);
  Data8 = IoRead8 (SMSC_1036_LPC_DATA);
  Data8 &= ~(0x01);
  Data8 |= (UINT8)(RShiftU64 (PcdGet64 (PcdSerialRegisterBase), 2));
  IoWrite8 (SMSC_1036_LPC_DATA, Data8);

  //
  // UART IRQ
  //
  IoWrite8 (SMSC_1036_LPC_INDEX, SMSC_1036_LPC_CR28);
  Data8 = IoRead8 (SMSC_1036_LPC_DATA);
  Data8 &= ~(0x0F);
  Data8 |= (SMSC_1036_LPC_IRQ & 0x0F);
  IoWrite8 (SMSC_1036_LPC_DATA, Data8);

  //
  // Exit Conf-mode
  //
  IoWrite8 (SMSC_1036_LPC_INDEX, 0xAA);
}

/**
  Performs platform specific initialization required for the CPU to access
  the hardware associated with a SerialPortLib instance.  This function does
  not intiailzie the serial port hardware itself.  Instead, it initializes
  hardware devices that are required for the CPU to access the serial port
  hardware.  This function may be called more than once.

  @retval RETURN_SUCCESS       The platform specific initialization succeeded.
  @retval RETURN_DEVICE_ERROR  The platform specific initialization could not be completed.

**/
RETURN_STATUS
EFIAPI
PlatformHookSerialPortInitialize (
  VOID
  )
{
  UINT32 	  Data32;

  //
  // SerialPortEnable0
  //
  Data32 = 0;
  IoWrite32 (0xCF8, 0x8000A348);
  Data32 = IoRead32 (0xCFC);
  Data32 |= 0x04;
  IoWrite32 (0xCFC, Data32);

  //
  // Enable decode on Port4E/4F
  //
  IoWrite32 (0xCF8, 0x8000A364);
  Data32 = IoRead32 (0xCFC);
  Data32 &= 0xFFFF0000;
  Data32 |= 0x1640;
  IoWrite32 (0xCFC, Data32);

  //
  // Config LPC SMSC 1036 SIO
  //
  if (IsSio1036 ()) {
    EnSio1036Uart ();
  }

  return  EFI_SUCCESS;
}

