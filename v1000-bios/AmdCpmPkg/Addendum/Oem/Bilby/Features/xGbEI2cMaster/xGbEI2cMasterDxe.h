/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _XGBE_I2C_MASTER_DXE_H_
#define _XGBE_I2C_MASTER_DXE_H_

typedef struct _I2C_CONFIGURATION {
  UINT32 RxFifoDepth;
  UINT32 TxFifoDepth;
} I2C_CONFIGURATION;

/// Private xGbE I2C Master Data Block Structure
typedef struct _XGBE_I2CMASTER_PRIVATE
{
    XGBE_I2C_MASTER_PROTOCOL  I2cController;
    EFI_HANDLE                Handle;                         ///< Handle
    UINT8                     *WriteData;
    UINT32                    WriteCount;
    UINT8                     *ReadData;
    UINT32                    ReadCount;
    UINT32                    TransferCount;
    I2C_CONFIGURATION         I2cConfiguration;
} XGBE_I2CMASTER_PRIVATE;

EFI_STATUS
EFIAPI
SetBusFrequency (
  IN CONST XGBE_I2C_MASTER_PROTOCOL   *This,
  IN UINTN                    *BusClockHertz
  );

EFI_STATUS
EFIAPI
I2cInit(
   IN CONST XGBE_I2C_MASTER_PROTOCOL *This
   );

EFI_STATUS
EFIAPI
StartRequest (
   IN CONST XGBE_I2C_MASTER_PROTOCOL   *This,
   IN UINTN                            SlaveAddress,
   IN EFI_I2C_REQUEST_PACKET           *RequestPacket,
   IN EFI_EVENT                        Event      OPTIONAL,
   OUT EFI_STATUS                      *I2cStatus OPTIONAL
  );


//*******************************************************
//      AmdI2cMasterPei.c use only functions prototypes
//*******************************************************


EFI_STATUS
I2cWriteRead(
  IN OUT XGBE_I2CMASTER_PRIVATE    *Private
  );

EFI_STATUS
I2cAccess(
   IN XGBE_I2CMASTER_PRIVATE        *Private,
   IN UINTN                         SlaveAddress,
   IN OUT EFI_I2C_REQUEST_PACKET    *RequestPacket
  );

EFI_STATUS
I2cDwWaitI2cEnable(
   IN  UINT32   Base,
   IN  UINT32   I2cEnBit
  );

//
// Functions Prototypes
//
EFI_STATUS
EFIAPI
xGbEI2cMasterDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  );

#endif // _XGBE_I2C_MASTER_DXE_H_
