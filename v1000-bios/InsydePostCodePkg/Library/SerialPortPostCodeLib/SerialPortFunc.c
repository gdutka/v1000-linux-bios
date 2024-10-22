/** @file
  The instance of SerialPortPostCodeLib Library

;******************************************************************************
;* Copyright (c) 2016 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/** @file
  16550 UART Serial Port library functions

  Copyright (c) 2006 - 2012, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>
#include <Library/SerialPortPostCodeLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/PlatformHookLib.h>
#include <SerialPortFunc.h>

/**
  Read an 8-bit 16550 register.  If PcdSerialUseMmio is TRUE, then the value is read from
  MMIO space.  If PcdSerialUseMmio is FALSE, then the value is read from I/O space.  The
  parameter Offset is added to the base address of the 16550 registers that is specified
  by PcdSerialRegisterBase.

  @param  Offset  The offset of the 16550 register to read.

  @return The value read from the 16550 register.

**/
UINT8
SerialPortReadRegister (
  UINTN  Offset
  )
{
  if (PcdGetBool (PcdSerialUseMmio)) {
    return MmioRead8 ((UINTN)PcdGet64 (PcdH2OPostCodeSerialPortRegBase) + Offset * 4);
  } else {
    return IoRead8 ((UINT16)PcdGet64 (PcdH2OPostCodeSerialPortRegBase) + Offset);
  }
}

/**
  Write an 8-bit 16550 register.  If PcdSerialUseMmio is TRUE, then the value is written to
  MMIO space.  If PcdSerialUseMmio is FALSE, then the value is written to I/O space.  The
  parameter Offset is added to the base address of the 16550 registers that is specified
  by PcdSerialRegisterBase.

  @param  Offset  The offset of the 16550 register to write.
  @param  Value   The value to write to the 16550 register specified by Offset.

  @return The value written to the 16550 register.

**/
UINT8
SerialPortWriteRegister (
  UINTN  Offset,
  UINT8  Value
  )
{
  if (PcdGetBool (PcdSerialUseMmio)) {
    return MmioWrite8 ((UINTN)PcdGet64 (PcdH2OPostCodeSerialPortRegBase) + Offset * 4, Value);
  } else {
    return IoWrite8 ((UINT16)PcdGet64 (PcdH2OPostCodeSerialPortRegBase) + Offset, Value);
  }
}

/**
  Return whether the hardware flow control signal allows writing.

  @retval TRUE  The serial port is writable.
  @retval FALSE The serial port is not writable.
**/
BOOLEAN
SerialPortWritable (
  VOID
  )
{
  if (PcdGetBool (PcdSerialUseHardwareFlowControl)) {
    if (PcdGetBool (PcdSerialDetectCable)) {
      //
      // Wait for both DSR and CTS to be set
      //   DSR is set if a cable is connected.
      //   CTS is set if it is ok to transmit data
      //
      //   DSR  CTS  Description                               Action
      //   ===  ===  ========================================  ========
      //    0    0   No cable connected.                       Wait
      //    0    1   No cable connected.                       Wait
      //    1    0   Cable connected, but not clear to send.   Wait
      //    1    1   Cable connected, and clear to send.       Transmit
      //
      return (BOOLEAN) ((SerialPortReadRegister (R_UART_MSR) & (B_UART_MSR_DSR | B_UART_MSR_CTS)) == (B_UART_MSR_DSR | B_UART_MSR_CTS));
    } else {
      //
      // Wait for both DSR and CTS to be set OR for DSR to be clear.
      //   DSR is set if a cable is connected.
      //   CTS is set if it is ok to transmit data
      //
      //   DSR  CTS  Description                               Action
      //   ===  ===  ========================================  ========
      //    0    0   No cable connected.                       Transmit
      //    0    1   No cable connected.                       Transmit
      //    1    0   Cable connected, but not clear to send.   Wait
      //    1    1   Cable connected, and clar to send.        Transmit
      //
      return (BOOLEAN) ((SerialPortReadRegister (R_UART_MSR) & (B_UART_MSR_DSR | B_UART_MSR_CTS)) != (B_UART_MSR_DSR));
    }
  }

  return TRUE;
}

/**
  Initialize the serial device hardware.

  @retval RETURN_SUCCESS        The serial device was successfully initialized.
  @retval RETURN_DEVICE_ERROR   The serial device could not be initialized.

**/
RETURN_STATUS
EFIAPI
SerialPortInitialize (
  VOID
  )
{
  RETURN_STATUS  Status;
  UINTN          Divisor;
  UINT8          ResetDeviceString[] = {0x1B, 0x63};


  //
  // Perform platform specific initialization required to enable use of the 16550 device
  // at the location specified by PcdSerialUseMmio and PcdSerialRegisterBase.
  //
  Status = PlatformHookSerialPortInitialize ();
  if (RETURN_ERROR (Status)) {
    return Status;
  }

  //
  // Configure baud rate
  //
 // Divisor = 0x1A; //115200 baud
  Divisor = 0x139;  //9600 baud

  SerialPortWriteRegister (R_UART_LCR, B_UART_LCR_DLAB);
  SerialPortWriteRegister (R_UART_BAUD_HIGH, (UINT8) (Divisor >> 8));
  SerialPortWriteRegister (R_UART_BAUD_LOW, (UINT8) (Divisor & 0xff));

  //
  // Clear DLAB and configure Data Bits, Parity, and Stop Bits.
  // Strip reserved bits from PcdSerialLineControl
  //
  SerialPortWriteRegister (R_UART_LCR, (UINT8)(PcdGet8 (PcdSerialLineControl) & 0x3F));

  //
  // Enable and reset FIFOs
  // Strip reserved bits from PcdSerialFifoControl
  //
  SerialPortWriteRegister (R_UART_FCR, (UINT8)(PcdGet8 (PcdSerialFifoControl) & 0x27));

  //
  // Put Modem Control Register(MCR) into its reset state of 0x00.
  //
  SerialPortWriteRegister (R_UART_MCR, 0x00);

  //
  // Reset terminal to load default character set
  //
  SerialPortWrite (ResetDeviceString, sizeof (ResetDeviceString));

  return RETURN_SUCCESS;
}

/**
  Write data from buffer to serial device.

  Writes NumberOfBytes data bytes from Buffer to the serial device.
  The number of bytes actually written to the serial device is returned.
  If the return value is less than NumberOfBytes, then the write operation failed.

  If Buffer is NULL, then ASSERT().

  If NumberOfBytes is zero, then return 0.

  @param  Buffer           Pointer to the data buffer to be written.
  @param  NumberOfBytes    Number of bytes to written to the serial device.

  @retval 0                NumberOfBytes is 0.
  @retval >0               The number of bytes written to the serial device.
                           If this value is less than NumberOfBytes, then the read operation failed.

**/
UINTN
EFIAPI
SerialPortWrite (
  IN UINT8     *Buffer,
  IN UINTN     NumberOfBytes
  )
{
  UINTN    Result;
  UINTN    Index;
  UINTN    FifoSize;

  if (Buffer == NULL) {
    return 0;
  }

  //
  // Compute the maximum size of the Tx FIFO
  //
  FifoSize = 1;
  if ((SerialPortReadRegister (R_UART_IRR) & (B_UART_IRR_FIFOEH | B_UART_IRR_FIFOEL))
       == (B_UART_IRR_FIFOEH | B_UART_IRR_FIFOEL)) {
    FifoSize = 16;
  }

  Result = NumberOfBytes;
  while (NumberOfBytes != 0) {
    //
    // Wait for the serial port to be ready, to make sure both the transmit FIFO
    // and shift register empty.
    //
    while ((SerialPortReadRegister (R_UART_LSR) & B_UART_LSR_TEMT) == 0);

    //
    // Fill then entire Tx FIFO
    //
    for (Index = 0; Index < FifoSize && NumberOfBytes != 0; Index++, NumberOfBytes--, Buffer++) {
      //
      // Wait for the hardware flow control signal
      //
      while (!SerialPortWritable ());

      //
      // Write byte to the transmit buffer.
      //
      SerialPortWriteRegister (R_UART_TXBUF, *Buffer);
    }
  }
  return Result;
}

