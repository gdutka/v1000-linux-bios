/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef XGBE_AMD_I2C_MASTER_PEI_H_
#define XGBE_AMD_I2C_MASTER_PEI_H_

typedef struct _I2C_CONFIGURATION {
  UINT32 RxFifoDepth;
  UINT32 TxFifoDepth;
} I2C_CONFIGURATION;


typedef struct _XGBE_I2C_MASTER_PRIVATE
{
    EFI_PEI_SERVICES          **PeiServices;      ///< Pointer to PeiServices
    UINT32                    I2cSdpAddress;
    UINT8                     *WriteData;
    UINT32                    WriteCount;
    UINT8                     *ReadData;
    UINT32                    ReadCount;
    UINT32                    TransferCount;
    I2C_CONFIGURATION         I2cConfiguration;
} XGBE_I2C_MASTER_PRIVATE;

#define XGBE_BLOCK_I2C_CONTROLLER_COUNT 2

/// Private I2C Master Data Block Structure
typedef struct _AMD_XGBE_I2CMASTER_PRIVATE {
  XGBE_I2C_MASTER_PRIVATE   I2cMasterPpi[XGBE_BLOCK_I2C_CONTROLLER_COUNT];      ///< I2c0~1 Data Block
} AMD_XGBE_I2C_MASTER_PRIVATE;

EFI_STATUS
EFIAPI
XgbeSetBusFrequency (
  IN XGBE_I2C_MASTER_PRIVATE   *Private,
  IN UINTN                    *BusClockHertz
  );

EFI_STATUS
EFIAPI
XgbeStartRequest (
   IN XGBE_I2C_MASTER_PRIVATE     *This,
   IN UINTN                            SlaveAddress,
   IN EFI_I2C_REQUEST_PACKET           *RequestPacket
  );

EFI_STATUS
I2cWriteRead(
  IN OUT XGBE_I2C_MASTER_PRIVATE    *Private
  );

EFI_STATUS
I2cAccess(
   IN XGBE_I2C_MASTER_PRIVATE         *Private,
   IN UINTN                         SlaveAddress,
   IN OUT EFI_I2C_REQUEST_PACKET    *RequestPacket
  );

EFI_STATUS
I2cDwWaitI2cEnable(
   IN  UINT32   Base,
   IN  UINT32   I2cEnBit
  );

EFI_STATUS
I2cInit(
   IN OUT XGBE_I2C_MASTER_PRIVATE    *Private
   );

#endif // XGBE_AMD_I2C_MASTER_PEI_H_

