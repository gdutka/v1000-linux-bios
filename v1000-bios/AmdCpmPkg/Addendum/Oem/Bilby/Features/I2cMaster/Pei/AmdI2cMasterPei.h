/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _AMD_I2C_MASTER_PEI_H_
#define _AMD_I2C_MASTER_PEI_H_

EFI_GUID AmdI2cMasterID =\
  { 0x33df1f4a, 0xf8fe, 0x47e3, { 0x87, 0x53, 0x7a, 0x61, 0x75, 0x88, 0xa4, 0x2c }};

UINT32 I2cSdpAddressList[] = {
   0xFEDC2000,
   0xFEDC3000,
   0xFEDC4000,  //I2C2 maps to SMN space as D4000 - D4FFF and SDP address as FEDC4000 - FEDC4FFF
   0xFEDC5000,  //I2C3 maps to SMN space as D5000 - D5FFF and SDP address as FEDC5000 - FEDC5FFF.
   0xFEDC6000,  //I2C4 maps to SMN space as D6000 - D6FFF and SDP address as FEDC6000 - FEDC6FFF.
   0xFEDCB000
};

typedef struct _I2C_CONFIGURATION {
  UINT32 RxFifoDepth;
  UINT32 TxFifoDepth;
} I2C_CONFIGURATION;

#define I2C_CONTROLLER_COUNT  (sizeof (I2cSdpAddressList) / sizeof (UINT32))

typedef struct _I2CMASTER_PPI_PRIVATE
{
    EFI_PEI_I2C_MASTER_PPI    I2cPpi;
    EFI_PEI_SERVICES          **PeiServices;      ///< Pointer to PeiServices
    UINT32                    I2cSdpAddress;
    UINT8                     *WriteData;
    UINT32                    WriteCount;
    UINT8                     *ReadData;
    UINT32                    ReadCount;
    UINT32                    TransferCount;
    I2C_CONFIGURATION         I2cConfiguration;
} I2CMASTER_PPI_PRIVATE;

/// Private I2C Master Data Block Structure
typedef struct _AMD_PEI_I2C_MASTER_PRIVATE {
  I2CMASTER_PPI_PRIVATE   I2cMasterPpi[I2C_CONTROLLER_COUNT];      ///< I2c0~5 Data Block
} AMD_PEI_I2C_MASTER_PRIVATE;

EFI_STATUS
EFIAPI
SetBusFrequency (
  IN EFI_PEI_I2C_MASTER_PPI   *This,
  IN UINTN                    *BusClockHertz
  );

EFI_STATUS
EFIAPI
Reset (
   IN CONST EFI_PEI_I2C_MASTER_PPI  *This
  );

EFI_STATUS
EFIAPI
StartRequest (
   IN CONST EFI_PEI_I2C_MASTER_PPI     *This,
   IN UINTN                            SlaveAddress,
   IN EFI_I2C_REQUEST_PACKET           *RequestPacket
  );


//*******************************************************
//      AmdI2cMasterPei.c use only functions prototypes
//*******************************************************

EFI_STATUS
I2cInit(
   IN OUT I2CMASTER_PPI_PRIVATE    *Private
   );

EFI_STATUS
I2cWriteRead(
  IN OUT I2CMASTER_PPI_PRIVATE    *Private
  );

EFI_STATUS
I2cAccess(
   IN I2CMASTER_PPI_PRIVATE         *Private,
   IN UINTN                         SlaveAddress,
   IN OUT EFI_I2C_REQUEST_PACKET    *RequestPacket
  );

EFI_STATUS
I2cDwWaitI2cEnable(
   IN  UINT32   Base,
   IN  UINT32   I2cEnBit
  );

#endif // _AMD_I2C_MASTER_PEI_H_

