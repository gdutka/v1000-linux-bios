/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/PciLib.h>
#include <Library/IoLib.h>
#include <Library/PlatformHookLib.h>
#include <Library/DebugLib.h>

#define SIO_UNLOCK_PORT       0x2E

#define SIO_INDEX_PORT        0x2E
#define SIO_DATA_PORT         0x2F

#define R_UART_LSR            5

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
  // Assume R_UART_LSR will never return 0xFF when serial port is enabled.  Use
  // that to determine if we already enabled the serial port.
  
  if (IoRead8 ((UINTN)(PcdGet64 (PcdSerialRegisterBase)) + R_UART_LSR) == 0xFF) {

    //
    // Enable LPC DMA cycle
    //
    PciOr32 (PCI_LIB_ADDRESS (0, 20, 3, 0x40), BIT2);
    //
    // Enable I/O Port 0x2E/0x2F & 0x4E/0x4F decoding to LPC
    //
    PciOr32 (PCI_LIB_ADDRESS (0, 20, 3, 0x48), BIT0+BIT1);
    //
    // Enable I/O Port 0x3F8~0x3FF & 0x2F8~0x2FF & 0x2E8~0x2EF & 0x3E8~0x3EF decoding to LPC
    //
    PciOr32 (PCI_LIB_ADDRESS (0, 20, 3, 0x44), BIT6+BIT7+BIT11+BIT13);

    //
    // enter MB PNP Mode so we can program
    //
    IoWrite8 (SIO_UNLOCK_PORT, 0x87);
    IoWrite8 (SIO_UNLOCK_PORT, 0x01);
    IoWrite8 (SIO_UNLOCK_PORT, 0x55);
    IoWrite8 (SIO_UNLOCK_PORT, 0x55);

    // Select Serial Port 1 LDN 1
    IoWrite8 (SIO_INDEX_PORT, 0x07);
    IoWrite8 (SIO_DATA_PORT, 0x01);

    // Set Serial Port 1 base address
    IoWrite8 (SIO_INDEX_PORT, 0x60);
    IoWrite8 (SIO_DATA_PORT, (UINT8)(((UINTN)PcdGet64 (PcdSerialRegisterBase) >> 8) & 0xFF));
    IoWrite8 (SIO_INDEX_PORT, 0x61);
    IoWrite8 (SIO_DATA_PORT, (UINT8)(((UINTN)PcdGet64 (PcdSerialRegisterBase)) & 0xFF));

    // Activate Serial Port 1
    IoWrite8 (SIO_INDEX_PORT, 0x30);
    IoWrite8 (SIO_DATA_PORT, 0x01);

    // exit MB PNP mode
//    IoWrite8 (SIO_INDEX_PORT, 0x02);
//    IoOr8 (SIO_DATA_PORT, 1);

  }
  return EFI_SUCCESS;
}

