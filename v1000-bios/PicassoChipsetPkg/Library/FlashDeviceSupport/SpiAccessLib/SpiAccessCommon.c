/** @file
  Common implementation for SpiAccessLib

;******************************************************************************
;* Copyright (c) 2015, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Fch.h>
#include <SpiAccess.h>

#define FCH_SPI_MMIO_BASE  (0xFEC10000)
#define MAX_SPI_FIFO_PTR   (MAX_SPI_FIFO_SIZE - 1)

#define SpiMmioRead8(Register)                            MmioRead8 (FCH_SPI_MMIO_BASE + Register)
#define SpiMmioWrite8(Register, Value)                    MmioWrite8 (FCH_SPI_MMIO_BASE + Register, Value)
#define SpiMmioBitFieldRead8(Register, StartBit, EndBit)  MmioBitFieldRead8 (FCH_SPI_MMIO_BASE + Register, StartBit, EndBit)
#define SpiMmioRead32(Register)                           MmioRead32 (FCH_SPI_MMIO_BASE + Register)
#define SpiMmioOr32(Register, OrData)                     MmioOr32 (FCH_SPI_MMIO_BASE + Register, OrData)

/**
  Resets FIFO Ptr

  @retval VOID
  
**/
STATIC
VOID
ResetFifoPtr (
  )
{
  UINT32                        CurrFifoPtr;

  do {
    SpiMmioOr32 (FCH_SPI_MMIO_REG00, FCH_SPI_FIFO_PTR_CRL);
    CurrFifoPtr = SpiMmioRead8 (FCH_SPI_MMIO_REG0C+1) & 0x07;
  } while (CurrFifoPtr != 0);
}

/**
  Copy data from FIFO region to system memory by using TX/RX FIFO port IO access.

  Copy data from FIFO region specified by starting pointer Start
  to system memory specified by Buffer by using TX/RX FIFO port IO access.
  The total number of byte to be copied is specified by Length. 
  Buffer is returned.

  If Length is greater than (MAX_SPI_FIFO_PTR - Start + 1), then ASSERT(). 

  @param  Start           Starting Ptr for the FIFO region to be copied from.
  @param  Length          The size, in bytes, of Buffer.
  @param  Buffer          Pointer to a system memory buffer receiving the data read.

  @return Buffer

**/
STATIC
UINT8 *
RingBufferIoRead (
  IN  UINT8                     Start,
  IN  UINT8                     Length,
  OUT UINT8                     *Buffer
  )
{
  UINT8                         *ReturnBuffer;

  ASSERT ((Length - 1) <= (MAX_SPI_FIFO_PTR - Start));

  ReturnBuffer = Buffer;

  //
  // Reset and Calibrate FifoPtr if need be.
  //
  ResetFifoPtr ();
  while (Start !=  SpiMmioBitFieldRead8 (FCH_SPI_MMIO_REG4C_SPISTATUS + 2, 0, 7)) {
    SpiMmioRead8 (FCH_SPI_MMIO_REG0C);
  }

  //
  // Get data from TX/RX FIFO port.
  //
  while (Length-- > 0) {
    *(Buffer++) = SpiMmioRead8 (FCH_SPI_MMIO_REG0C);
  }
  
  return ReturnBuffer;
}

/**
  Copy data from system memory to FIFO region by using TX/RX FIFO port IO access.

  Copy data from system memory specified by Buffer to FIFO region specified 
  by starting pointer Start by using TX/RX FIFO port IO access.
  The total number of byte to be copied is specified by Length. 
  Buffer is returned.
  
  If Length is greater than (MAX_SPI_FIFO_PTR - Start + 1), then ASSERT(). 

  @param  StartAddress    Starting Ptr for the FIFO region to be copied to.
  @param  Length          The size, in bytes, of Buffer.
  @param  Buffer          Pointer to a system memory buffer containing the data to write.

  @return Buffer

**/
STATIC
UINT8 *
RingBufferIoWrite (
  IN  UINT8                     Start,
  IN  UINT8                     Length,
  IN  CONST UINT8               *Buffer
  )
{
  UINT8                         *ReturnBuffer;

  ASSERT ((Length - 1) <= (MAX_SPI_FIFO_PTR - Start));

  ReturnBuffer = (UINT8 *) Buffer;

  //
  // Reset and Calibrate FifoPtr if need be.
  //
  ResetFifoPtr ();
//  while (Start !=  MmioBitFieldRead8 (FCH_SPI_MMIO_REG0C + 1, 0, 2)) {
  while (Start !=  (SpiMmioRead8 (FCH_SPI_MMIO_REG0C + 1) & 0x07)) {
    SpiMmioWrite8 (FCH_SPI_MMIO_REG0C, SpiMmioRead8 (FCH_SPI_MMIO_REG0C));
  }

  //
  // Load data into TX/RX FIFO port.
  //
  while (Length-- > 0) {
    SpiMmioWrite8 (FCH_SPI_MMIO_REG0C, *(Buffer++));
  }
  
  return ReturnBuffer;
}

/**
  Copy data from FIFO region to system memory by using MMIO access.

  Copy data from FIFO region specified by starting pointer Start
  to system memory specified by Buffer by using MMIO access.
  The total number of byte to be copied is specified by Length. 
  Buffer is returned.

  If Length is greater than (MAX_SPI_FIFO_PTR - Start + 1), then ASSERT(). 

  @param  Start           Starting Ptr for the FIFO region to be copied from.
  @param  Length          The size, in bytes, of Buffer.
  @param  Buffer          Pointer to a system memory buffer receiving the data read.

  @return Buffer

**/
STATIC
UINT8 *
RingBufferMmioRead (
  IN  UINT8                     Start,
  IN  UINT8                     Length,
  OUT UINT8                     *Buffer
  )
{
  UINT8                         *ReturnBuffer;

  ASSERT ((Length - 1) <= (MAX_SPI_FIFO_PTR - Start));

  ReturnBuffer = Buffer;

  //
  // This function is implemented specifically for those platforms
  // at which the FIFO Ring Buffer is memory mapped for read. So this
  // function just do a MMIO copy for Ring Buffer Read.
  //
  while (Length-- > 0) {
    *(Buffer++) = SpiMmioRead8 (FCH_SPI_MMIO_REG80_FIFO + Start++);
  }

  return ReturnBuffer;
}

/**
  Copy data from system memory to FIFO region by using MMIO access.

  Copy data from system memory specified by Buffer to FIFO region specified 
  by starting pointer Start by using MMIO access.
  The total number of byte to be copied is specified by Length. 
  Buffer is returned.
  
  If Length is greater than (MAX_SPI_FIFO_PTR - Start + 1), then ASSERT(). 

  @param  StartAddress    Starting Ptr for the FIFO region to be copied to.
  @param  Length          The size, in bytes, of Buffer.
  @param  Buffer          Pointer to a system memory buffer containing the data to write.

  @return Buffer

**/
STATIC
UINT8 *
RingBufferMmioWrite (
  IN  UINT8                     Start,
  IN  UINT8                     Length,
  IN  CONST UINT8               *Buffer
  )
{
  VOID*                         ReturnBuffer;

  ASSERT ((Length - 1) <= (MAX_SPI_FIFO_PTR - Start));

  ReturnBuffer = (UINT8 *) Buffer;

  //
  // This function is implemented specifically for those platforms
  // at which the FIFO Ring Buffer is memory mapped for write. So this
  // function just do a MMIO copy for Ring Buffer write.
  //
  while (Length-- > 0) {
     SpiMmioWrite8 (FCH_SPI_MMIO_REG80_FIFO + Start++, *(Buffer++));
  }

  return ReturnBuffer;
}

/**
  This function execute the transaction of SPI command to the slave device
 
  @param[in]      OpCode        SPI command
  @param[in]      TxByteCount   The number of bytes to be sent to SPI ROM
  @param[in]      RxByteCount   The number of bytes to be received from the SPI ROM

  @retval EFI_SUCCESS           Function successfully returned
  
**/
STATIC
EFI_STATUS
ExecuteOpCmd (
  IN  UINT8                     OpCmd,
  IN  UINT8                     TxByteCount,
  IN  UINT8                     RxByteCount
  )
{
  UINT32                        SpiReg00;

  //
  // Wait for Spi bus getting ready to take this transaction.
  //
  do {
    SpiReg00 = SpiMmioRead32 (FCH_SPI_MMIO_REG00);
    IoRead8 ((UINT16) (0x80));
  } while ((SpiReg00 & (FCH_SPI_BUSY + FCH_SPI_EXEC_OPCODE)));

  //
  // Specifies TxByteCount, RxByteCount.
  //
//   SpiMmioWrite8 (FCH_SPI_MMIO_REG48_TXBYTECOUNT, TxByteCount);
//   SpiMmioWrite8 (FCH_SPI_MMIO_REG4B_RXBYTECOUNT, RxByteCount);
  SpiMmioWrite8 (FCH_SPI_MMIO_REG1E, 0x05);
  SpiMmioWrite8 (FCH_SPI_MMIO_REG1F, TxByteCount);
  SpiMmioRead8 (FCH_SPI_MMIO_REG1F);
  SpiMmioWrite8 (FCH_SPI_MMIO_REG1E, 0x06);
  SpiMmioWrite8 (FCH_SPI_MMIO_REG1F, RxByteCount);
  SpiMmioRead8 (FCH_SPI_MMIO_REG1F);
  IoRead8 ((UINT16) (0x80));

  //
  // Specifies the SPI opcode.
  //
  SpiMmioWrite8 (FCH_SPI_MMIO_REG00, OpCmd);
  IoRead8 ((UINT16) (0x80));
  SpiMmioOr32 (FCH_SPI_MMIO_REG00, FCH_SPI_EXEC_OPCODE);

  //
  // Exist when transaction is complete.
  //
  do {
    SpiReg00 = SpiMmioRead32 (FCH_SPI_MMIO_REG00);
    IoRead8 ((UINT16) (0x80));
  } while ((SpiReg00 & (FCH_SPI_BUSY + FCH_SPI_EXEC_OPCODE)));

  return EFI_SUCCESS;
}

/**
  This function sends the programmed SPI command to the slave device,
  and retrieve response data.
 
  @param[in]      OpCode        SPI command
  @param[in]      BufferIn      Buffer for command data
  @param[in]      SizeIn        Size of command data
  @param[in, out] BufferOut     Buffer for response data
  @param[in, out] SizeOut       Size of retrieved response data

  @retval EFI_SUCCESS           Function successfully returned
  @retval EFI_BAD_BUFFER_SIZE   SizeIn + SizeOut is out of FIFO buffer
  @retval EFI_DEVICE_ERROR      Failed to Recoginize the SPI device
  
**/
EFI_STATUS
SendSpiCmd (
  IN     UINT8                  OpCode,
  IN     UINT8                  *BufferIn,
  IN     UINT8                  SizeIn,
  IN OUT UINT8                  *BufferOut,
  IN OUT UINT8                  *SizeOut
  )
{
  UINT8                         TxByteCount;
  UINT8                         RxByteCount;
  UINT8                         RemainingBytes;

  if (SizeIn > MAX_SPI_FIFO_SIZE) {
    return EFI_BAD_BUFFER_SIZE;
  }

  TxByteCount = SizeIn;
  RemainingBytes = (SizeOut == NULL) ? 0 : *SizeOut;

  //
  // Based on TxByteCount and FIFO size, we need to determine how many Rx
  // can be taken in the this SPI transaction.
  //
  RxByteCount = (RemainingBytes > MAX_SPI_FIFO_SIZE - TxByteCount) ? MAX_SPI_FIFO_SIZE - TxByteCount : RemainingBytes;
  RemainingBytes -= RxByteCount;

  //
  // If shifts data out, load data into the the FIFO Ring Buffer
  //
  if (TxByteCount != 0 && BufferIn != NULL) {
    RingBufferIoWrite (0, TxByteCount, BufferIn);
  }

  ExecuteOpCmd (OpCode, TxByteCount, RxByteCount);

  //
  // If shifts data in, get data from the FIFO Ring Buffer
  //
  if (RxByteCount != 0 && BufferOut != NULL) {
    RingBufferIoRead (TxByteCount, RxByteCount, BufferOut);
    //
    // Actual number of bytes retrieved in the this SPI transaction.
    //
    *SizeOut -= RemainingBytes;
  }
  
  return EFI_SUCCESS;
}

VOID
WaitForSpiBusNotBusy (
  )
{
  UINT32   SpiStatus;

  do {
    SpiStatus = SpiMmioRead32 (FCH_SPI_MMIO_REG00);
    IoRead8 ((UINT16) (0x80));
  } while ((SpiStatus & (FCH_SPI_BUSY + FCH_SPI_EXEC_OPCODE)));
}

VOID
WaitForSpiDeviceComplete (
  )
{
  UINT8    OpCmd;
  UINT8    ReadStatus = 0;

  //
  // Retrive RDSR Cmd from SPI_CmdValue1[31:24]
  //
  OpCmd = SpiMmioRead8 (0x17);

  //
  // Execute Read Status Command and
  // Stall until the device is not in progress of 
  // program/erase/write status register cycle
  //
  do {
    ExecuteOpCmd (OpCmd, 0, 1);
    RingBufferIoRead (0, 1, &ReadStatus);
  } while ((ReadStatus & BIT0) != 0);
}

VOID
WaitForSpiDeviceWriteEnabled (
  )
{
  UINT8    OpCmd;
  UINT8    ReadStatus = 0;

  //
  // Retrive RDSR Cmd from SPI_CmdValue1[31:24]
  //
  OpCmd = SpiMmioRead8 (0x17);

  //
  // Execute Read Status Command and
  // Stall until the device can accept
  // program/erase/write status register instruction
  //
  do {
    ExecuteOpCmd (OpCmd, 0, 1);
    RingBufferIoRead (0, 1, &ReadStatus);
  } while ((ReadStatus & BIT1) == 0);
}