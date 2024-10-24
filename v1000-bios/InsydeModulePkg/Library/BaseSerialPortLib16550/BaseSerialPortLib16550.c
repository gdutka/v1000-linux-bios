/** @file
  16550 UART Serial Port library functions

;******************************************************************************
;* Copyright (c) 2019 - 2021, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/
/** @file
  16550 UART Serial Port library functions

  (C) Copyright 2014 Hewlett-Packard Development Company, L.P.<BR>
  Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>
#include <IndustryStandard/Pci.h>
#include <Library/SerialPortLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/PlatformHookLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>

#include <Library/H2OSerialConfigLib.h>

#define H2O_SERIAL_DEVICE_MAX_DEVICE_PATH    60
#define END_DEVICE_PATH_LENGTH               (sizeof (EFI_DEVICE_PATH_PROTOCOL))

#define UART_16550_COMPATIBLE_HW_GUID \
  { \
    0xe76fd4e9, 0x0a30, 0x4ca9, {0x95, 0x40, 0xd7, 0x99, 0x53, 0x4c, 0xc4, 0xff} \
  }

#pragma pack(1)

typedef struct {
  ACPI_HID_DEVICE_PATH      AcpiHid;
  EFI_DEVICE_PATH_PROTOCOL  End;
} IO_SERIAL_DEVICE_PATH;

typedef struct {
  MMIO_UART_DEVICE_PATH     MmioNode;
  EFI_DEVICE_PATH_PROTOCOL  End;
} MMIO_SERIAL_DEVICE_PATH;

#pragma pack()

STATIC
IO_SERIAL_DEVICE_PATH  mIoSerialDevicePath = {
  {
    ACPI_DEVICE_PATH,
    ACPI_DP,
    (UINT8)(sizeof(ACPI_HID_DEVICE_PATH)),
    (UINT8)((sizeof(ACPI_HID_DEVICE_PATH)) >> 8),
    EISA_PNP_ID(0x0501),                //HID
    0                                   //UID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    END_DEVICE_PATH_LENGTH,
    0
  }
};

STATIC
MMIO_SERIAL_DEVICE_PATH mMmioSerialDevicePath = {
  {
    HARDWARE_DEVICE_PATH,
    HW_VENDOR_DP,
    sizeof (MMIO_UART_DEVICE_PATH),
    0,
    UART_16550_COMPATIBLE_HW_GUID,      //GUID
    0,                                  //BaseAddress
    0                                   //UID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    sizeof (EFI_DEVICE_PATH),
    0
  }
};

/**
  This function returns serial device information

  @param  SerialDevice      On entry, this points to the serial device information pointer.
                            On exit, this points to the updated serial device information.

  @retval EFI_ABORTED       The device is disabled or not present.
  @retval EFI_SUCCESS       The serial device information was initialized.

**/
STATIC
RETURN_STATUS
InternalGetSerialDeviceConfig (
  H2O_SERIAL_DEVICE       *SerialDevice
  )
{
  EFI_STATUS                        Status;
  UINT8                             DevicePath[H2O_SERIAL_DEVICE_MAX_DEVICE_PATH];

  SerialDevice->Size                 = sizeof (H2O_SERIAL_DEVICE);
  SerialDevice->RegisterBase         = PcdGet64(PcdSerialRegisterBase);
  SerialDevice->RegisterStride       = (UINT8)PcdGet32 (PcdSerialRegisterStride);
  if (PcdGetBool (PcdSerialUseMmio)) {
    SerialDevice->AccessType         = H2O_SERIAL_DEV_INFO_ACCESS_MMIO;
    CopyMem (DevicePath, &mMmioSerialDevicePath, sizeof (MMIO_SERIAL_DEVICE_PATH));
  } else {
    SerialDevice->AccessType         = H2O_SERIAL_DEV_INFO_ACCESS_IO;
    CopyMem (DevicePath, &mIoSerialDevicePath, sizeof (IO_SERIAL_DEVICE_PATH));
  }
  SerialDevice->ClockRate            = PcdGet32 (PcdSerialClockRate);
  SerialDevice->MaxReceiveFifoDepth  = 0;
  SerialDevice->MaxTransmitFifoDepth = (UINT16) PcdGet32 (PcdSerialExtendedTxFifoSize);
  SerialDevice->DevicePath           = (EFI_DEVICE_PATH_PROTOCOL *)DevicePath;
  SerialDevice->MaxDevicePathSize    = H2O_SERIAL_DEVICE_MAX_DEVICE_PATH;

  Status = H2OGetSerialDeviceConfig (SerialDevice);
  if (Status != EFI_MEDIA_CHANGED && Status != EFI_UNSUPPORTED) {
    return EFI_ABORTED;
  }

  return EFI_SUCCESS;
}

/**
  This function returns serial attribute information

  @param  SerialDevice      On entry, this points to the serial attribute information pointer.
                            On exit, this points to the updated serial attribute information.

  @retval EFI_ABORTED       The device is disabled or not present.
  @retval EFI_SUCCESS       The serial attribute information was initialized.

**/
STATIC
RETURN_STATUS
InternalGetSerialAttribsConfig (
  H2O_SERIAL_ATTRIBS      *SerialAttribs
  )
{
  EFI_STATUS                        Status;
  UINT8                             DevicePath[H2O_SERIAL_DEVICE_MAX_DEVICE_PATH];

  SerialAttribs->Size                = sizeof (H2O_SERIAL_ATTRIBS);
  SerialAttribs->BaudRate            = PcdGet32 (PcdSerialBaudRate);
  SerialAttribs->Parity              = (UINT8) ((PcdGet8 (PcdSerialLineControl) >> 3) & 0x7);
  SerialAttribs->StopBits            = (UINT8) ((PcdGet8 (PcdSerialLineControl) >> 2) & 0x1);
  SerialAttribs->DataBits            = (UINT8) (PcdGet8 (PcdSerialLineControl) & 0x3);
  SerialAttribs->Present             = TRUE;
  SerialAttribs->Timeout             = 0;
  SerialAttribs->ReceiveFifoDepth    = 0;
  SerialAttribs->TransmitFifoDepth   = 0;
  SerialAttribs->DevicePath          = (EFI_DEVICE_PATH_PROTOCOL *)DevicePath;
  SerialAttribs->MaxDevicePathSize   = H2O_SERIAL_DEVICE_MAX_DEVICE_PATH;

  if (PcdGetBool (PcdSerialUseMmio)) {
    CopyMem (DevicePath, &mMmioSerialDevicePath, sizeof (MMIO_SERIAL_DEVICE_PATH));
  } else {
    CopyMem (DevicePath, &mIoSerialDevicePath, sizeof (IO_SERIAL_DEVICE_PATH));
  }

  Status = H2OGetSerialAttribsConfig (SerialAttribs);
  if (Status != EFI_MEDIA_CHANGED && Status != EFI_UNSUPPORTED) {
    return EFI_ABORTED;
  }

  return EFI_SUCCESS;
}

//
// PCI Defintions.
//
#define PCI_BRIDGE_32_BIT_IO_SPACE              0x01

//
// 16550 UART register offsets and bitfields
//
#define R_UART_RXBUF          0
#define R_UART_TXBUF          0
#define R_UART_BAUD_LOW       0
#define R_UART_BAUD_HIGH      1
#define R_UART_FCR            2
#define   B_UART_FCR_FIFOE    BIT0
#define   B_UART_FCR_FIFO64   BIT5
#define R_UART_LCR            3
#define   B_UART_LCR_DLAB     BIT7
#define R_UART_MCR            4
#define   B_UART_MCR_DTRC     BIT0
#define   B_UART_MCR_RTS      BIT1
#define R_UART_LSR            5
#define   B_UART_LSR_RXRDY    BIT0
#define   B_UART_LSR_TXRDY    BIT5
#define   B_UART_LSR_TEMT     BIT6
#define R_UART_MSR            6
#define   B_UART_MSR_CTS      BIT4
#define   B_UART_MSR_DSR      BIT5
#define   B_UART_MSR_RI       BIT6
#define   B_UART_MSR_DCD      BIT7

//
// 4-byte structure for each PCI node in PcdSerialPciDeviceInfo
//
typedef struct {
  UINT8   Device;
  UINT8   Function;
  UINT16  PowerManagementStatusAndControlRegister;
} PCI_UART_DEVICE_INFO;

/**
  Read an 8-bit 16550 register.  If PcdSerialUseMmio is TRUE, then the value is read from
  MMIO space.  If PcdSerialUseMmio is FALSE, then the value is read from I/O space.  The
  parameter Offset is added to the base address of the 16550 registers that is specified
  by PcdSerialRegisterBase.

  @param  Base    The base address register of UART device.
  @param  Offset  The offset of the 16550 register to read.

  @return The value read from the 16550 register.

**/
UINT8
SerialPortReadRegister (
  H2O_SERIAL_DEVICE        *SerialDevice,
  UINTN                    Base,
  UINTN                    Offset
  )
{
  if (SerialDevice->AccessType == H2O_SERIAL_DEV_INFO_ACCESS_MMIO) {
    return MmioRead8 (Base + Offset * SerialDevice->RegisterStride);
  } else {
    return IoRead8 (Base + Offset * SerialDevice->RegisterStride);
  }
}

/**
  Write an 8-bit 16550 register.  If PcdSerialUseMmio is TRUE, then the value is written to
  MMIO space.  If PcdSerialUseMmio is FALSE, then the value is written to I/O space.  The
  parameter Offset is added to the base address of the 16550 registers that is specified
  by PcdSerialRegisterBase.

  @param  Base    The base address register of UART device.
  @param  Offset  The offset of the 16550 register to write.
  @param  Value   The value to write to the 16550 register specified by Offset.

  @return The value written to the 16550 register.

**/
UINT8
SerialPortWriteRegister (
  H2O_SERIAL_DEVICE        *SerialDevice,
  UINTN                    Base,
  UINTN                    Offset,
  UINT8                    Value
  )
{
  if (SerialDevice->AccessType == H2O_SERIAL_DEV_INFO_ACCESS_MMIO) {
    return MmioWrite8 (Base + Offset * SerialDevice->RegisterStride, Value);
  } else {
    return IoWrite8 (Base + Offset * SerialDevice->RegisterStride, Value);
  }
}

/**
  Update the value of an 16-bit PCI configuration register in a PCI device.  If the
  PCI Configuration register specified by PciAddress is already programmed with a
  non-zero value, then return the current value.  Otherwise update the PCI configuration
  register specified by PciAddress with the value specified by Value and return the
  value programmed into the PCI configuration register.  All values must be masked
  using the bitmask specified by Mask.

  @param  PciAddress  PCI Library address of the PCI Configuration register to update.
  @param  Value       The value to program into the PCI Configuration Register.
  @param  Mask        Bitmask of the bits to check and update in the PCI configuration register.

**/
UINT16
SerialPortLibUpdatePciRegister16 (
  UINTN   PciAddress,
  UINT16  Value,
  UINT16  Mask
  )
{
  UINT16  CurrentValue;

  CurrentValue = PciRead16 (PciAddress) & Mask;
  if (CurrentValue != 0) {
    return CurrentValue;
  }
  return PciWrite16 (PciAddress, Value & Mask);
}

/**
  Update the value of an 32-bit PCI configuration register in a PCI device.  If the
  PCI Configuration register specified by PciAddress is already programmed with a
  non-zero value, then return the current value.  Otherwise update the PCI configuration
  register specified by PciAddress with the value specified by Value and return the
  value programmed into the PCI configuration register.  All values must be masked
  using the bitmask specified by Mask.

  @param  PciAddress  PCI Library address of the PCI Configuration register to update.
  @param  Value       The value to program into the PCI Configuration Register.
  @param  Mask        Bitmask of the bits to check and update in the PCI configuration register.

  @return  The Secondary bus number that is actually programed into the PCI to PCI Bridge device.

**/
UINT32
SerialPortLibUpdatePciRegister32 (
  UINTN   PciAddress,
  UINT32  Value,
  UINT32  Mask
  )
{
  UINT32  CurrentValue;

  CurrentValue = PciRead32 (PciAddress) & Mask;
  if (CurrentValue != 0) {
    return CurrentValue;
  }
  return PciWrite32 (PciAddress, Value & Mask);
}

/**
  Retrieve the I/O or MMIO base address register for the PCI UART device.

  This function assumes Root Bus Numer is Zero, and enables I/O and MMIO in PCI UART
  Device if they are not already enabled.

  @return  The base address register of the UART device.

**/
UINTN
GetSerialRegisterBase (
  H2O_SERIAL_DEVICE        *SerialDevice
  )
{
  UINTN                 PciLibAddress;
  UINTN                 BusNumber;
  UINTN                 SubordinateBusNumber;
  UINT32                ParentIoBase;
  UINT32                ParentIoLimit;
  UINT16                ParentMemoryBase;
  UINT16                ParentMemoryLimit;
  UINT32                IoBase;
  UINT32                IoLimit;
  UINT16                MemoryBase;
  UINT16                MemoryLimit;
  UINTN                 SerialRegisterBase;
  UINTN                 BarIndex;
  UINT32                RegisterBaseMask;
  PCI_UART_DEVICE_INFO  *DeviceInfo;
  BOOLEAN               SerialUseMmio;

  //
  // Get PCI Device Info
  //
  DeviceInfo = (PCI_UART_DEVICE_INFO *) PcdGetPtr (PcdSerialPciDeviceInfo);

  //
  // If PCI Device Info is empty, then assume fixed address UART and return PcdSerialRegisterBase
  //
  if (DeviceInfo->Device == 0xff) {
    return (UINTN)SerialDevice->RegisterBase;
  }

  SerialUseMmio = (BOOLEAN) (SerialDevice->AccessType == H2O_SERIAL_DEV_INFO_ACCESS_MMIO);


  //
  // Assume PCI Bus 0 I/O window is 0-64KB and MMIO windows is 0-4GB
  //
  ParentMemoryBase  = 0 >> 16;
  ParentMemoryLimit = 0xfff00000 >> 16;
  ParentIoBase      = 0 >> 12;
  ParentIoLimit     = 0xf000 >> 12;

  //
  // Enable I/O and MMIO in PCI Bridge
  // Assume Root Bus Numer is Zero.
  //
  for (BusNumber = 0; (DeviceInfo + 1)->Device != 0xff; DeviceInfo++) {
    //
    // Compute PCI Lib Address to PCI to PCI Bridge
    //
    PciLibAddress = PCI_LIB_ADDRESS (BusNumber, DeviceInfo->Device, DeviceInfo->Function, 0);

    //
    // Retrieve and verify the bus numbers in the PCI to PCI Bridge
    //
    BusNumber            = PciRead8 (PciLibAddress + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
    SubordinateBusNumber = PciRead8 (PciLibAddress + PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET);
    if (BusNumber == 0 || BusNumber > SubordinateBusNumber) {
      return 0;
    }

    //
    // Retrieve and verify the I/O or MMIO decode window in the PCI to PCI Bridge
    //
    if (SerialUseMmio) {
      MemoryLimit = PciRead16 (PciLibAddress + OFFSET_OF (PCI_TYPE01, Bridge.MemoryLimit)) & 0xfff0;
      MemoryBase  = PciRead16 (PciLibAddress + OFFSET_OF (PCI_TYPE01, Bridge.MemoryBase))  & 0xfff0;

      //
      // If PCI Bridge MMIO window is disabled, then return 0
      //
      if (MemoryLimit < MemoryBase) {
        return 0;
      }

      //
      // If PCI Bridge MMIO window is not in the address range decoded by the parent PCI Bridge, then return 0
      //
      if (MemoryBase < ParentMemoryBase || MemoryBase > ParentMemoryLimit || MemoryLimit > ParentMemoryLimit) {
        return 0;
      }
      ParentMemoryBase  = MemoryBase;
      ParentMemoryLimit = MemoryLimit;
    } else {
      IoLimit = PciRead8 (PciLibAddress + OFFSET_OF (PCI_TYPE01, Bridge.IoLimit));
      if ((IoLimit & PCI_BRIDGE_32_BIT_IO_SPACE ) == 0) {
        IoLimit = IoLimit >> 4;
      } else {
        IoLimit = (PciRead16 (PciLibAddress + OFFSET_OF (PCI_TYPE01, Bridge.IoLimitUpper16)) << 4) | (IoLimit >> 4);
      }
      IoBase = PciRead8 (PciLibAddress + OFFSET_OF (PCI_TYPE01, Bridge.IoBase));
      if ((IoBase & PCI_BRIDGE_32_BIT_IO_SPACE ) == 0) {
        IoBase = IoBase >> 4;
      } else {
        IoBase = (PciRead16 (PciLibAddress + OFFSET_OF (PCI_TYPE01, Bridge.IoBaseUpper16)) << 4) | (IoBase >> 4);
      }

      //
      // If PCI Bridge I/O window is disabled, then return 0
      //
      if (IoLimit < IoBase) {
        return 0;
      }

      //
      // If PCI Bridge I/O window is not in the address range decoded by the parent PCI Bridge, then return 0
      //
      if (IoBase < ParentIoBase || IoBase > ParentIoLimit || IoLimit > ParentIoLimit) {
        return 0;
      }
      ParentIoBase  = IoBase;
      ParentIoLimit = IoLimit;
    }
  }

  //
  // Compute PCI Lib Address to PCI UART
  //
  PciLibAddress = PCI_LIB_ADDRESS (BusNumber, DeviceInfo->Device, DeviceInfo->Function, 0);

  //
  // Find the first IO or MMIO BAR
  //
  RegisterBaseMask = 0xFFFFFFF0;
  for (BarIndex = 0; BarIndex < PCI_MAX_BAR; BarIndex ++) {
    SerialRegisterBase = PciRead32 (PciLibAddress + PCI_BASE_ADDRESSREG_OFFSET + BarIndex * 4);
    if (SerialUseMmio && ((SerialRegisterBase & BIT0) == 0)) {
      //
      // MMIO BAR is found
      //
      RegisterBaseMask = 0xFFFFFFF0;
      break;
    }

    if ((!SerialUseMmio) && ((SerialRegisterBase & BIT0) != 0)) {
      //
      // IO BAR is found
      //
      RegisterBaseMask = 0xFFFFFFF8;
      break;
    }
  }

  //
  // MMIO or IO BAR is not found.
  //
  if (BarIndex == PCI_MAX_BAR) {
    return 0;
  }

  //
  // Program UART BAR
  //
  SerialRegisterBase = SerialPortLibUpdatePciRegister32 (
                         PciLibAddress + PCI_BASE_ADDRESSREG_OFFSET + BarIndex * 4,
                         (UINT32) SerialDevice->RegisterBase,
                         RegisterBaseMask
                         );

  //
  // Verify that the UART BAR is in the address range decoded by the parent PCI Bridge
  //
  if (SerialUseMmio) {
    if (((SerialRegisterBase >> 16) & 0xfff0) < ParentMemoryBase || ((SerialRegisterBase >> 16) & 0xfff0) > ParentMemoryLimit) {
      return 0;
    }
  } else {
    if ((SerialRegisterBase >> 12) < ParentIoBase || (SerialRegisterBase >> 12) > ParentIoLimit) {
      return 0;
    }
  }

  //
  // Enable I/O and MMIO in PCI UART Device if they are not already enabled
  //
  PciOr16 (
    PciLibAddress + PCI_COMMAND_OFFSET,
    SerialUseMmio ? EFI_PCI_COMMAND_MEMORY_SPACE : EFI_PCI_COMMAND_IO_SPACE
    );

  //
  // Force D0 state if a Power Management and Status Register is specified
  //
  if (DeviceInfo->PowerManagementStatusAndControlRegister != 0x00) {
    if ((PciRead16 (PciLibAddress + DeviceInfo->PowerManagementStatusAndControlRegister) & (BIT0 | BIT1)) != 0x00) {
      PciAnd16 (PciLibAddress + DeviceInfo->PowerManagementStatusAndControlRegister, (UINT16)~(BIT0 | BIT1));
      //
      // If PCI UART was not in D0, then make sure FIFOs are enabled, but do not reset FIFOs
      //
      SerialPortWriteRegister (SerialDevice, SerialRegisterBase, R_UART_FCR, (UINT8)(PcdGet8 (PcdSerialFifoControl) & (B_UART_FCR_FIFOE | B_UART_FCR_FIFO64)));
    }
  }

  //
  // Get PCI Device Info
  //
  DeviceInfo = (PCI_UART_DEVICE_INFO *) PcdGetPtr (PcdSerialPciDeviceInfo);

  //
  // Enable I/O or MMIO in PCI Bridge
  // Assume Root Bus Numer is Zero.
  //
  for (BusNumber = 0; (DeviceInfo + 1)->Device != 0xff; DeviceInfo++) {
    //
    // Compute PCI Lib Address to PCI to PCI Bridge
    //
    PciLibAddress = PCI_LIB_ADDRESS (BusNumber, DeviceInfo->Device, DeviceInfo->Function, 0);

    //
    // Enable the I/O or MMIO decode windows in the PCI to PCI Bridge
    //
    PciOr16 (
      PciLibAddress + PCI_COMMAND_OFFSET,
      SerialUseMmio ? EFI_PCI_COMMAND_MEMORY_SPACE : EFI_PCI_COMMAND_IO_SPACE
      );

    //
    // Force D0 state if a Power Management and Status Register is specified
    //
    if (DeviceInfo->PowerManagementStatusAndControlRegister != 0x00) {
      if ((PciRead16 (PciLibAddress + DeviceInfo->PowerManagementStatusAndControlRegister) & (BIT0 | BIT1)) != 0x00) {
        PciAnd16 (PciLibAddress + DeviceInfo->PowerManagementStatusAndControlRegister, (UINT16)~(BIT0 | BIT1));
      }
    }

    BusNumber = PciRead8 (PciLibAddress + PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
  }

  return SerialRegisterBase;
}

/**
  Return whether the hardware flow control signal allows writing.

  @param  SerialRegisterBase The base address register of UART device.

  @retval TRUE  The serial port is writable.
  @retval FALSE The serial port is not writable.
**/
BOOLEAN
SerialPortWritable (
  H2O_SERIAL_DEVICE        *SerialDevice,
  UINTN                    SerialRegisterBase
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
      return (BOOLEAN) ((SerialPortReadRegister (SerialDevice, SerialRegisterBase, R_UART_MSR) & (B_UART_MSR_DSR | B_UART_MSR_CTS)) == (B_UART_MSR_DSR | B_UART_MSR_CTS));
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
      return (BOOLEAN) ((SerialPortReadRegister (SerialDevice, SerialRegisterBase, R_UART_MSR) & (B_UART_MSR_DSR | B_UART_MSR_CTS)) != (B_UART_MSR_DSR));
    }
  }

  return TRUE;
}

/**
  Initialize the serial device hardware.

  If no initialization is required, then return RETURN_SUCCESS.
  If the serial device was successfully initialized, then return RETURN_SUCCESS.
  If the serial device could not be initialized, then return RETURN_DEVICE_ERROR.

  @retval RETURN_SUCCESS        The serial device was initialized.
  @retval RETURN_DEVICE_ERROR   The serial device could not be initialized.

**/
RETURN_STATUS
EFIAPI
SerialPortInitialize (
  VOID
  )
{
  RETURN_STATUS            Status;
  UINTN                    SerialRegisterBase;
  UINT32                   Divisor;
  UINT32                   CurrentDivisor;
  BOOLEAN                  Initialized;
  H2O_SERIAL_DEVICE        SerialDevice;
  H2O_SERIAL_ATTRIBS       SerialAttribs;
  UINT8                    SerialLineControl;
  UINT32                   SerialBaudRate;

  Status = InternalGetSerialDeviceConfig (&SerialDevice);
  if (RETURN_ERROR (Status)) {
    return Status;
  }

  Status = InternalGetSerialAttribsConfig (&SerialAttribs);
  if (RETURN_ERROR (Status)) {
    return Status;
  }

  //
  // Perform platform specific initialization required to enable use of the 16550 device
  // at the location specified by PcdSerialUseMmio and PcdSerialRegisterBase.
  //
  Status = PlatformHookSerialPortInitialize ();
  if (RETURN_ERROR (Status)) {
    return Status;
  }

  //
  // Calculate divisor for baud generator
  //    Ref_Clk_Rate / Baud_Rate / 16
  //

  SerialBaudRate = (UINT32)SerialAttribs.BaudRate;
  Divisor = SerialDevice.ClockRate / (SerialBaudRate * 16);
  if ((SerialDevice.ClockRate % (SerialBaudRate * 16)) >= (SerialBaudRate * 8)) {
    Divisor++;
  }

  //
  // Get the base address of the serial port in either I/O or MMIO space
  //
  SerialRegisterBase = GetSerialRegisterBase (&SerialDevice);
  if (SerialRegisterBase ==0) {
    return RETURN_DEVICE_ERROR;
  }

  //
  // See if the serial port is already initialized
  //
  Initialized = TRUE;
  SerialLineControl = ((SerialAttribs.Parity & 0x7) << 3) | ((SerialAttribs.StopBits & 0x1) << 2) | (SerialAttribs.DataBits & 0x3);
  if ((SerialPortReadRegister (&SerialDevice, SerialRegisterBase, R_UART_LCR) & 0x3F) != SerialLineControl) {
    Initialized = FALSE;
  }
  SerialPortWriteRegister (
    &SerialDevice,
    SerialRegisterBase,
    R_UART_LCR,
    (UINT8)(SerialPortReadRegister (&SerialDevice, SerialRegisterBase, R_UART_LCR) | B_UART_LCR_DLAB)
    );
  CurrentDivisor =  SerialPortReadRegister (&SerialDevice, SerialRegisterBase, R_UART_BAUD_HIGH) << 8;
  CurrentDivisor |= (UINT32) SerialPortReadRegister (&SerialDevice, SerialRegisterBase, R_UART_BAUD_LOW);
  SerialPortWriteRegister (
    &SerialDevice,
    SerialRegisterBase,
    R_UART_LCR,
    (UINT8)(SerialPortReadRegister (&SerialDevice, SerialRegisterBase, R_UART_LCR) & ~B_UART_LCR_DLAB)
    );
  if (CurrentDivisor != Divisor) {
    Initialized = FALSE;
  }
  if (Initialized) {
    return RETURN_SUCCESS;
  }

  //
  // Wait for the serial port to be ready.
  // Verify that both the transmit FIFO and the shift register are empty.
  //
  while ((SerialPortReadRegister (&SerialDevice, SerialRegisterBase, R_UART_LSR) & (B_UART_LSR_TEMT | B_UART_LSR_TXRDY)) != (B_UART_LSR_TEMT | B_UART_LSR_TXRDY));

  //
  // Configure baud rate
  //
  SerialPortWriteRegister (&SerialDevice, SerialRegisterBase, R_UART_LCR, B_UART_LCR_DLAB);
  SerialPortWriteRegister (&SerialDevice, SerialRegisterBase, R_UART_BAUD_HIGH, (UINT8) (Divisor >> 8));
  SerialPortWriteRegister (&SerialDevice, SerialRegisterBase, R_UART_BAUD_LOW, (UINT8) (Divisor & 0xff));

  //
  // Clear DLAB and configure Data Bits, Parity, and Stop Bits.
  // Strip reserved bits from PcdSerialLineControl
  //
  SerialPortWriteRegister (&SerialDevice, SerialRegisterBase, R_UART_LCR, SerialLineControl);

  //
  // Enable and reset FIFOs
  // Strip reserved bits from PcdSerialFifoControl
  //
  SerialPortWriteRegister (&SerialDevice, SerialRegisterBase, R_UART_FCR, 0x00);
  SerialPortWriteRegister (&SerialDevice, SerialRegisterBase, R_UART_FCR, (UINT8)(PcdGet8 (PcdSerialFifoControl) & (B_UART_FCR_FIFOE | B_UART_FCR_FIFO64)));

  //
  // Put Modem Control Register(MCR) into its reset state of 0x00.
  //
  SerialPortWriteRegister (&SerialDevice, SerialRegisterBase, R_UART_MCR, 0x00);

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
                           If this value is less than NumberOfBytes, then the write operation failed.

**/
UINTN
EFIAPI
SerialPortWrite (
  IN UINT8     *Buffer,
  IN UINTN     NumberOfBytes
  )
{
  UINTN                    SerialRegisterBase;
  UINTN                    Result;
  UINTN                    Index;
  UINTN                    FifoSize;
  EFI_STATUS               Status;
  H2O_SERIAL_DEVICE        SerialDevice;

  if (Buffer == NULL) {
    return 0;
  }

  Status = InternalGetSerialDeviceConfig (&SerialDevice);
  if (RETURN_ERROR (Status)) {
    return Status;
  }

  SerialRegisterBase = GetSerialRegisterBase (&SerialDevice);
  if (SerialRegisterBase ==0) {
    return 0;
  }

  if (NumberOfBytes == 0) {
    //
    // Flush the hardware
    //

    //
    // Wait for both the transmit FIFO and shift register empty.
    //
    while ((SerialPortReadRegister (&SerialDevice, SerialRegisterBase, R_UART_LSR) & (B_UART_LSR_TEMT | B_UART_LSR_TXRDY)) != (B_UART_LSR_TEMT | B_UART_LSR_TXRDY));

    //
    // Wait for the hardware flow control signal
    //
    while (!SerialPortWritable (&SerialDevice, SerialRegisterBase));
    return 0;
  }

  //
  // Compute the maximum size of the Tx FIFO
  //
  FifoSize = 1;
  if ((PcdGet8 (PcdSerialFifoControl) & B_UART_FCR_FIFOE) != 0) {
    if ((PcdGet8 (PcdSerialFifoControl) & B_UART_FCR_FIFO64) == 0) {
      FifoSize = 16;
    } else {
      FifoSize = SerialDevice.MaxTransmitFifoDepth;
    }
  }

  Result = NumberOfBytes;
  while (NumberOfBytes != 0) {
    //
    // Wait for the serial port to be ready, to make sure both the transmit FIFO
    // and shift register empty.
    //
    while ((SerialPortReadRegister (&SerialDevice, SerialRegisterBase, R_UART_LSR) & B_UART_LSR_TEMT) == 0);

    //
    // Fill then entire Tx FIFO
    //
    for (Index = 0; Index < FifoSize && NumberOfBytes != 0; Index++, NumberOfBytes--, Buffer++) {
      //
      // Wait for the hardware flow control signal
      //
      while (!SerialPortWritable (&SerialDevice, SerialRegisterBase));

      //
      // Write byte to the transmit buffer.
      //
      SerialPortWriteRegister (&SerialDevice, SerialRegisterBase, R_UART_TXBUF, *Buffer);
    }
  }
  return Result;
}

/**
  Reads data from a serial device into a buffer.

  @param  Buffer           Pointer to the data buffer to store the data read from the serial device.
  @param  NumberOfBytes    Number of bytes to read from the serial device.

  @retval 0                NumberOfBytes is 0.
  @retval >0               The number of bytes read from the serial device.
                           If this value is less than NumberOfBytes, then the read operation failed.

**/
UINTN
EFIAPI
SerialPortRead (
  OUT UINT8     *Buffer,
  IN  UINTN     NumberOfBytes
  )
{
  UINTN                    SerialRegisterBase;
  UINTN                    Result;
  UINT8                    Mcr;
  H2O_SERIAL_DEVICE        SerialDevice;
  EFI_STATUS               Status;

  if (NULL == Buffer) {
    return 0;
  }

  Status = InternalGetSerialDeviceConfig (&SerialDevice);
  if (RETURN_ERROR (Status)) {
    return 0;
  }

  SerialRegisterBase = GetSerialRegisterBase (&SerialDevice);
  if (SerialRegisterBase == 0) {
    return 0;
  }

  Mcr = (UINT8)(SerialPortReadRegister (&SerialDevice, SerialRegisterBase, R_UART_MCR) & ~B_UART_MCR_RTS);

  for (Result = 0; NumberOfBytes-- != 0; Result++, Buffer++) {
    //
    // Wait for the serial port to have some data.
    //
    while ((SerialPortReadRegister (&SerialDevice, SerialRegisterBase, R_UART_LSR) & B_UART_LSR_RXRDY) == 0) {
      if (PcdGetBool (PcdSerialUseHardwareFlowControl)) {
        //
        // Set RTS to let the peer send some data
        //
        SerialPortWriteRegister (&SerialDevice, SerialRegisterBase, R_UART_MCR, (UINT8)(Mcr | B_UART_MCR_RTS));
      }
    }
    if (PcdGetBool (PcdSerialUseHardwareFlowControl)) {
      //
      // Clear RTS to prevent peer from sending data
      //
      SerialPortWriteRegister (&SerialDevice, SerialRegisterBase, R_UART_MCR, Mcr);
    }

    //
    // Read byte from the receive buffer.
    //
    *Buffer = SerialPortReadRegister (&SerialDevice, SerialRegisterBase, R_UART_RXBUF);
  }

  return Result;
}


/**
  Polls a serial device to see if there is any data waiting to be read.

  Polls aserial device to see if there is any data waiting to be read.
  If there is data waiting to be read from the serial device, then TRUE is returned.
  If there is no data waiting to be read from the serial device, then FALSE is returned.

  @retval TRUE             Data is waiting to be read from the serial device.
  @retval FALSE            There is no data waiting to be read from the serial device.

**/
BOOLEAN
EFIAPI
SerialPortPoll (
  VOID
  )
{
  UINTN                    SerialRegisterBase;
  EFI_STATUS               Status;
  H2O_SERIAL_DEVICE        SerialDevice;

  Status = InternalGetSerialDeviceConfig (&SerialDevice);
  if (RETURN_ERROR (Status)) {
    return FALSE;
  }

  SerialRegisterBase = GetSerialRegisterBase (&SerialDevice);
  if (SerialRegisterBase ==0) {
    return FALSE;
  }

  //
  // Read the serial port status
  //
  if ((SerialPortReadRegister (&SerialDevice, SerialRegisterBase, R_UART_LSR) & B_UART_LSR_RXRDY) != 0) {
    if (PcdGetBool (PcdSerialUseHardwareFlowControl)) {
      //
      // Clear RTS to prevent peer from sending data
      //
      SerialPortWriteRegister (&SerialDevice, SerialRegisterBase, R_UART_MCR, (UINT8)(SerialPortReadRegister (&SerialDevice, SerialRegisterBase, R_UART_MCR) & ~B_UART_MCR_RTS));
    }
    return TRUE;
  }

  if (PcdGetBool (PcdSerialUseHardwareFlowControl)) {
    //
    // Set RTS to let the peer send some data
    //
    SerialPortWriteRegister (&SerialDevice, SerialRegisterBase, R_UART_MCR, (UINT8)(SerialPortReadRegister (&SerialDevice, SerialRegisterBase, R_UART_MCR) | B_UART_MCR_RTS));
  }

  return FALSE;
}

/**
  Sets the control bits on a serial device.

  @param Control                Sets the bits of Control that are settable.

  @retval RETURN_SUCCESS        The new control bits were set on the serial device.
  @retval RETURN_UNSUPPORTED    The serial device does not support this operation.
  @retval RETURN_DEVICE_ERROR   The serial device is not functioning correctly.

**/
RETURN_STATUS
EFIAPI
SerialPortSetControl (
  IN UINT32 Control
  )
{
  UINTN                    SerialRegisterBase;
  UINT8                    Mcr;
  EFI_STATUS               Status;
  H2O_SERIAL_DEVICE        SerialDevice;

  //
  // First determine the parameter is invalid.
  //
  if ((Control & (~(EFI_SERIAL_REQUEST_TO_SEND | EFI_SERIAL_DATA_TERMINAL_READY |
                    EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE))) != 0) {
    return RETURN_UNSUPPORTED;
  }

  Status = InternalGetSerialDeviceConfig (&SerialDevice);
  if (RETURN_ERROR (Status)) {
    return RETURN_UNSUPPORTED;
  }

  SerialRegisterBase = GetSerialRegisterBase (&SerialDevice);
  if (SerialRegisterBase ==0) {
    return RETURN_UNSUPPORTED;
  }

  //
  // Read the Modem Control Register.
  //
  Mcr = SerialPortReadRegister (&SerialDevice, SerialRegisterBase, R_UART_MCR);
  Mcr &= (~(B_UART_MCR_DTRC | B_UART_MCR_RTS));

  if ((Control & EFI_SERIAL_DATA_TERMINAL_READY) == EFI_SERIAL_DATA_TERMINAL_READY) {
    Mcr |= B_UART_MCR_DTRC;
  }

  if ((Control & EFI_SERIAL_REQUEST_TO_SEND) == EFI_SERIAL_REQUEST_TO_SEND) {
    Mcr |= B_UART_MCR_RTS;
  }

  //
  // Write the Modem Control Register.
  //
  SerialPortWriteRegister (&SerialDevice, SerialRegisterBase, R_UART_MCR, Mcr);

  return RETURN_SUCCESS;
}

/**
  Retrieve the status of the control bits on a serial device.

  @param Control                A pointer to return the current control signals from the serial device.

  @retval RETURN_SUCCESS        The control bits were read from the serial device.
  @retval RETURN_UNSUPPORTED    The serial device does not support this operation.
  @retval RETURN_DEVICE_ERROR   The serial device is not functioning correctly.

**/
RETURN_STATUS
EFIAPI
SerialPortGetControl (
  OUT UINT32 *Control
  )
{
  UINTN                    SerialRegisterBase;
  UINT8                    Msr;
  UINT8                    Mcr;
  UINT8                    Lsr;
  EFI_STATUS               Status;
  H2O_SERIAL_DEVICE        SerialDevice;

  Status = InternalGetSerialDeviceConfig (&SerialDevice);
  if (RETURN_ERROR (Status)) {
    return RETURN_UNSUPPORTED;
  }

  SerialRegisterBase = GetSerialRegisterBase (&SerialDevice);
  if (SerialRegisterBase ==0) {
    return RETURN_UNSUPPORTED;
  }

  *Control = 0;

  //
  // Read the Modem Status Register.
  //
  Msr = SerialPortReadRegister (&SerialDevice, SerialRegisterBase, R_UART_MSR);

  if ((Msr & B_UART_MSR_CTS) == B_UART_MSR_CTS) {
    *Control |= EFI_SERIAL_CLEAR_TO_SEND;
  }

  if ((Msr & B_UART_MSR_DSR) == B_UART_MSR_DSR) {
    *Control |= EFI_SERIAL_DATA_SET_READY;
  }

  if ((Msr & B_UART_MSR_RI) == B_UART_MSR_RI) {
    *Control |= EFI_SERIAL_RING_INDICATE;
  }

  if ((Msr & B_UART_MSR_DCD) == B_UART_MSR_DCD) {
    *Control |= EFI_SERIAL_CARRIER_DETECT;
  }

  //
  // Read the Modem Control Register.
  //
  Mcr = SerialPortReadRegister (&SerialDevice, SerialRegisterBase, R_UART_MCR);

  if ((Mcr & B_UART_MCR_DTRC) == B_UART_MCR_DTRC) {
    *Control |= EFI_SERIAL_DATA_TERMINAL_READY;
  }

  if ((Mcr & B_UART_MCR_RTS) == B_UART_MCR_RTS) {
    *Control |= EFI_SERIAL_REQUEST_TO_SEND;
  }

  if (PcdGetBool (PcdSerialUseHardwareFlowControl)) {
    *Control |= EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE;
  }

  //
  // Read the Line Status Register.
  //
  Lsr = SerialPortReadRegister (&SerialDevice, SerialRegisterBase, R_UART_LSR);

  if ((Lsr & (B_UART_LSR_TEMT | B_UART_LSR_TXRDY)) == (B_UART_LSR_TEMT | B_UART_LSR_TXRDY)) {
    *Control |= EFI_SERIAL_OUTPUT_BUFFER_EMPTY;
  }

  if ((Lsr & B_UART_LSR_RXRDY) == 0) {
    *Control |= EFI_SERIAL_INPUT_BUFFER_EMPTY;
  }

  return RETURN_SUCCESS;
}

/**
  Sets the baud rate, receive FIFO depth, transmit/receice time out, parity,
  data bits, and stop bits on a serial device.

  @param BaudRate           The requested baud rate. A BaudRate value of 0 will use the
                            device's default interface speed.
                            On output, the value actually set.
  @param ReveiveFifoDepth   The requested depth of the FIFO on the receive side of the
                            serial interface. A ReceiveFifoDepth value of 0 will use
                            the device's default FIFO depth.
                            On output, the value actually set.
  @param Timeout            The requested time out for a single character in microseconds.
                            This timeout applies to both the transmit and receive side of the
                            interface. A Timeout value of 0 will use the device's default time
                            out value.
                            On output, the value actually set.
  @param Parity             The type of parity to use on this serial device. A Parity value of
                            DefaultParity will use the device's default parity value.
                            On output, the value actually set.
  @param DataBits           The number of data bits to use on the serial device. A DataBits
                            vaule of 0 will use the device's default data bit setting.
                            On output, the value actually set.
  @param StopBits           The number of stop bits to use on this serial device. A StopBits
                            value of DefaultStopBits will use the device's default number of
                            stop bits.
                            On output, the value actually set.

  @retval RETURN_SUCCESS            The new attributes were set on the serial device.
  @retval RETURN_UNSUPPORTED        The serial device does not support this operation.
  @retval RETURN_INVALID_PARAMETER  One or more of the attributes has an unsupported value.
  @retval RETURN_DEVICE_ERROR       The serial device is not functioning correctly.

**/
RETURN_STATUS
EFIAPI
SerialPortSetAttributes (
  IN OUT UINT64             *BaudRate,
  IN OUT UINT32             *ReceiveFifoDepth,
  IN OUT UINT32             *Timeout,
  IN OUT EFI_PARITY_TYPE    *Parity,
  IN OUT UINT8              *DataBits,
  IN OUT EFI_STOP_BITS_TYPE *StopBits
  )
{
  UINTN                    SerialRegisterBase;
  UINT32                   SerialBaudRate;
  UINTN                    Divisor;
  UINT8                    Lcr;
  UINT8                    LcrData;
  UINT8                    LcrParity;
  UINT8                    LcrStop;
  H2O_SERIAL_DEVICE        SerialDevice;
  H2O_SERIAL_ATTRIBS       SerialAttribs;
  EFI_STATUS               Status;

  Status = InternalGetSerialDeviceConfig (&SerialDevice);
  if (RETURN_ERROR (Status)) {
    return Status;
  }

  Status = InternalGetSerialAttribsConfig (&SerialAttribs);
  if (RETURN_ERROR (Status)) {
    return Status;
  }

  SerialRegisterBase = GetSerialRegisterBase (&SerialDevice);
  if (SerialRegisterBase ==0) {
    return RETURN_UNSUPPORTED;
  }

  //
  // Check for default settings and fill in actual values.
  //
  if (*BaudRate == 0) {
    *BaudRate = (UINT32)SerialAttribs.BaudRate;
  }
  SerialBaudRate = (UINT32) *BaudRate;


  if (*DataBits == 0) {
    LcrData = (UINT8) (SerialAttribs.DataBits & 0x3);
    *DataBits = LcrData + 5;
  } else {
    if ((*DataBits < 5) || (*DataBits > 8)) {
      return RETURN_INVALID_PARAMETER;
    }
    //
    // Map 5..8 to 0..3
    //
    LcrData = (UINT8) (*DataBits - (UINT8) 5);
  }

  if (*Parity == DefaultParity) {
    LcrParity = (UINT8) (SerialAttribs.Parity & 0x7);
    switch (LcrParity) {
      case 0:
        *Parity = NoParity;
        break;

      case 3:
        *Parity = EvenParity;
        break;

      case 1:
        *Parity = OddParity;
        break;

      case 7:
        *Parity = SpaceParity;
        break;

      case 5:
        *Parity = MarkParity;
        break;

      default:
        break;
    }
  } else {
    switch (*Parity) {
      case NoParity:
        LcrParity = 0;
        break;

      case EvenParity:
        LcrParity = 3;
        break;

      case OddParity:
        LcrParity = 1;
        break;

      case SpaceParity:
        LcrParity = 7;
        break;

      case MarkParity:
        LcrParity = 5;
        break;

      default:
        return RETURN_INVALID_PARAMETER;
    }
  }

  if (*StopBits == DefaultStopBits) {
    LcrStop = (UINT8) (SerialAttribs.StopBits & 0x1);
    switch (LcrStop) {
      case 0:
        *StopBits = OneStopBit;
        break;

      case 1:
        if (*DataBits == 5) {
          *StopBits = OneFiveStopBits;
        } else {
          *StopBits = TwoStopBits;
        }
        break;

      default:
        break;
    }
  } else {
    switch (*StopBits) {
      case OneStopBit:
        LcrStop = 0;
        break;

      case OneFiveStopBits:
      case TwoStopBits:
        LcrStop = 1;
        break;

      default:
        return RETURN_INVALID_PARAMETER;
    }
  }

  //
  // Calculate divisor for baud generator
  //    Ref_Clk_Rate / Baud_Rate / 16
  //
  Divisor = SerialDevice.ClockRate / (SerialBaudRate * 16);
  if ((SerialDevice.ClockRate % (SerialBaudRate * 16)) >= SerialBaudRate * 8) {
    Divisor++;
  }

  //
  // Configure baud rate
  //
  SerialPortWriteRegister (&SerialDevice, SerialRegisterBase, R_UART_LCR, B_UART_LCR_DLAB);
  SerialPortWriteRegister (&SerialDevice, SerialRegisterBase, R_UART_BAUD_HIGH, (UINT8) (Divisor >> 8));
  SerialPortWriteRegister (&SerialDevice, SerialRegisterBase, R_UART_BAUD_LOW, (UINT8) (Divisor & 0xff));

  //
  // Clear DLAB and configure Data Bits, Parity, and Stop Bits.
  // Strip reserved bits from line control value
  //
  Lcr = (UINT8) ((LcrParity << 3) | (LcrStop << 2) | LcrData);
  SerialPortWriteRegister (&SerialDevice, SerialRegisterBase, R_UART_LCR, (UINT8) (Lcr & 0x3F));

  return RETURN_SUCCESS;
}

