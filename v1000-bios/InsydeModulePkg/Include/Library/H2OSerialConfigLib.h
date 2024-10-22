/** @file
  Definitions for H2O Serial Config Library

;******************************************************************************
;* Copyright (c) 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef __H2O_SERIAL_CONFIG_LIB_
#define __H2O_SERIAL_CONFIG_LIB_

#include <Uefi.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SerialIo.h>

typedef enum {
  H2O_SERIAL_DEV_INFO_ACCESS_IO,
  H2O_SERIAL_DEV_INFO_ACCESS_MMIO
} H2O_SERIAL_DEVICE_ACCESS_TYPE;

typedef enum {
  H2O_SERIAL_USAGE_TERMINAL,
  H2O_SERIAL_USAGE_LOG,
  H2O_SERIAL_USAGE_DEBUG
} H2O_SERIAL_USAGE;

#pragma pack(1)

typedef struct {
  VENDOR_DEVICE_PATH Hdr;
  UINT32             BaseAddress;
  UINT32             UID;
} MMIO_UART_DEVICE_PATH;


typedef struct _H2O_SERIAL_DEVICE {
  UINT32                    Size;

  UINT64                    RegisterBase;
  UINT8                     RegisterStride;
  UINT8                     AccessType;
  UINT8                     Reserved[2];
  UINT32                    ClockRate;
  UINT16                    MaxReceiveFifoDepth;
  UINT16                    MaxTransmitFifoDepth;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  UINT32                    MaxDevicePathSize;
} H2O_SERIAL_DEVICE;


typedef struct _H2O_SERIAL_ATTRIBS {
  UINT32                    Size;

  UINT64                    BaudRate;
  EFI_PARITY_TYPE           Parity;
  EFI_STOP_BITS_TYPE        StopBits;
  UINT8                     DataBits;
  BOOLEAN                   Present;
  UINT8                     Reserved[2];
  UINT32                    Timeout;
  UINT32                    ReceiveFifoDepth;
  UINT32                    TransmitFifoDepth;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  UINT32                    MaxDevicePathSize;
} H2O_SERIAL_ATTRIBS;

#pragma pack()

RETURN_STATUS
EFIAPI
H2OGetSerialDeviceConfig (
  IN OUT H2O_SERIAL_DEVICE *SerialDevice
  );


RETURN_STATUS
EFIAPI
H2OGetSerialAttribsConfig (
  IN OUT H2O_SERIAL_ATTRIBS *SerialAttribs
  );

#endif
