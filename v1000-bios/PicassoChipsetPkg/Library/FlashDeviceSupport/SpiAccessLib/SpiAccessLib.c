/** @file
  SpiAccessLib implementation for SPI Flash Type devices

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/FdSupportLib.h>
#include <Library/SpiAccessLib.h>
#include <SpiAccess.h>
//
// SFDP Table
//
#define SFDP_SIGNATURE                  0x50444653
#define SPI_READ_SFDP_OPCODE            0x5A
#define SPI_READ_SFDP_DUMMY_CYCLE_NUM   1

#pragma pack(1)
typedef struct
{
  UINT32  SfdpSignature;
  UINT8   SfdpMinorRevNum;
  UINT8   SfdpMajorRevNum;
  UINT8   ParameterHeaderNum;       // n: n+1 parameter header
//[-start-221128-IB14860873-modify]//
//  UINT8   Reserve0;
  UINT8   SfdpAccessProtocol;
//[-end-221128-IB14860873-modify]//
} SPI_SFDP_HEADER;

typedef struct
{
//[-start-221128-IB14860873-modify]//
//  UINT8  ID;
  UINT8  ParameterIdLsb;
//[-end-221128-IB14860873-modify]//
  UINT8  ParameterTableMinorRev;
  UINT8  ParameterTableMajorRev;
  UINT8  ParameterTableLen;         //in double word
  UINT8  ParameterTablePointer[3];
//[-start-221128-IB14860873-modify]//
//  UINT8  Reserve0;
  UINT8  ParameterIdMsb;
//[-end-221128-IB14860873-modify]//
} SPI_SFDP_PARAMETER_HEADER;

typedef struct
{
  //
  // 1th dword
  //
  UINT8  BlockEraseSizes : 2;
  UINT8  WriteGranularity : 1;
  UINT8  VolatitleStatusBit : 1;
  UINT8  WriteEnableOpcodeSelect : 1;
  UINT8  Reserve0 : 3;
  UINT8  EraseOpcode;
  UINT8  FastRead112Support : 1;   //112(1-1-2) (x-y-z): x: x pins used for the opcode
  UINT8  AddressBytes : 2;         //                    y: y pins used for the address
  UINT8  DTRSupport : 1;           //                    z: z pins used for the data
  UINT8  FastRead122Support : 1;
  UINT8  FastRead144Support : 1;
  UINT8  FastRead114Support : 1;
  UINT8  Reserve1 : 1;
  UINT8  Reserve2;
  //
  // 2nd dword
  //
  UINT32 FlashMemoryDensity;
  //
  // 3rd dword
  //
  UINT8  FastRead144DummyCycle : 5;
  UINT8  FastRead144ModeBit : 3;
  UINT8  FastRead144OpCode;
  UINT8  FastRead114DummyCycle : 5;
  UINT8  FastRead114ModeBit : 3;
  UINT8  FastRead114OpCode;
  //
  // 4th dword
  //
  UINT8  FastRead112DummyCycle : 5;
  UINT8  FastRead112ModeBit : 3;
  UINT8  FastRead112OpCode;
  UINT8  FastRead122DummyCycle : 5;
  UINT8  FastRead122ModeBit : 3;
  UINT8  FastRead122OpCode;
  //
  // 5th dword
  //
  UINT8  FastRead222Support : 1;
  UINT8  Reserve3 : 3;
  UINT8  FastRead444Support : 1;
  UINT8  Reserve4 : 3;
  UINT8  Reserve5[3];
  //
  // 6th dword
  //
  UINT8  Reserve6[2];
  UINT8  FastRead222DummyCycle : 5;
  UINT8  FastRead222ModeBit : 3;
  UINT8  FastRead222OpCode;
  //
  // 7th dword
  //
  UINT8  Reserve7[2];
  UINT8  FastRead444DummyCycle : 5;
  UINT8  FastRead444ModeBit : 3;
  UINT8  FastRead444OpCode;
  //
  // 8th dword
  //
  UINT8  SectorType1Size;
  UINT8  SectorType1EraseOpcode;
  UINT8  SectorType2Size;
  UINT8  SectorType2EraseOpcode;
  //
  // 9th dword
  //
  UINT8  SectorType3Size;
  UINT8  SectorType3EraseOpcode;
  UINT8  SectorType4Size;
  UINT8  SectorType4EraseOpcode;
//[-start-221128-IB14860873-add]//
  //
  // 10th dword
  //
  UINT32 MultiplierFromTypicalEraseTimeToMaximumEraseTime : 4;
  UINT32 Type1TypicalEraseTime : 7;
  UINT32 Type2TypicalEraseTime : 7;
  UINT32 Type3TypicalEraseTime : 7;
  UINT32 Type4TypicalEraseTime : 7;
  //
  // 11th dword
  //
  UINT32 MultiplierFromTypicalTimeToMaxTimeForPageOrByteProgram : 4;
  UINT32 PageSize : 4;
  UINT32 PageProgramTypicalTime : 6;
  UINT32 FirstByteProgramTypicalTime : 5;
  UINT32 AdditionalByteProgramTypicalTime : 5;
  UINT32 ChipEraseTypicalTime : 7;
  UINT32 : 1;
  //
  // 12th dword
  //
  UINT32 ProhibitedOperationsDuringProgramSuspend : 4;
  UINT32 ProhibitedOperationsDuringEraseSuspend : 4;
  UINT32 : 1;
  UINT32 ProgramResumeToSuspendInterval : 4;
  UINT32 SuspendInProgressProgramMaxLatency : 7;
  UINT32 EraseResumeToSuspendInterval : 4;
  UINT32 SuspendInProgressEraseMaxLatency : 7;
  UINT32 SuspendResumeSupported : 1;
  //
  // 13th dword
  //
  UINT32 ProgramResumeInstruction : 8;
  UINT32 ProgramSuspendInstruction : 8;
  UINT32 ResumeInstruction : 8;
  UINT32 SuspendInstruction : 8;
  //
  // 14th dword
  //
  UINT32 : 2;
  UINT32 StatusRegisterPollingDeviceBusy : 6;
  UINT32 ExitDeepPowerdownToNextOperationDelay : 7;
  UINT32 ExitDeepPowerdownInstruction : 8;
  UINT32 EnterDeepPowerdownInstruction : 8;
  UINT32 DeepPowerdownSupported : 1;
  //
  // 15th dword
  //
  UINT32 DisableSequences4S4S4SMode : 4;
  UINT32 EnableSequences4S4S4SMode : 5;
  UINT32 Supported044Mode : 1;
  UINT32 ExitMethod044Mode : 6;
  UINT32 EntryMethod044Mode : 4;
  UINT32 QuadEnableRequirements : 3;
  UINT32 HoldOrResetDisable : 1;
  UINT32 : 8;
  //
  // 16th dword
  //
  UINT32 VolatileOrNonVolatileRegisterAndWriteEnableInstructionForStatusRegister1 : 7;
  UINT32 : 1;
  UINT32 SoftResetAndRescueSequenceSupport : 6;
  UINT32 Exit4ByteAddressing : 10;
  UINT32 Enter4ByteAddressing : 8;
  //
  // 17th dword
  //
  UINT32 FastRead1S8S8SDummyCycle : 5;
  UINT32 FastRead1S8S8SModeBit : 3;
  UINT32 FastRead1S8S8SOpCode : 8;
  UINT32 FastRead1S1S8SDummyCycle : 5;
  UINT32 FastRead1S1S8SModeBit : 3;
  UINT32 FastRead1S1S8SOpCode : 8;
  //
  // 18th dword
  //
  UINT32 : 18;
  UINT32 VariableOutputDriverStrength : 5;
  UINT32 JedecSpiProtocolReset : 1;
  UINT32 DataStrobeWaveformsInStrMode : 2;
  UINT32 DataStrobeSupportForQpiStrMode4S4S4S : 1;
  UINT32 DataStrobeSupportForQpiStrMode4D4D4D : 1;
  UINT32 : 1;
  UINT32 OctalDtr8D8D8DCommandAndCommandExtension : 2;
  UINT32 ByteOrderIn8D8D8DMode : 1;
  //
  // 19th dword
  //
  UINT32 DisableSequences8S8S8SMode : 4;
  UINT32 EnableSequences8S8S8SMode : 5;
  UINT32 Supported088Mode : 1;
  UINT32 ExitMethod088Mode : 6;
  UINT32 EntryMethod088Mode : 4;
  UINT32 OctalEnableRequirements : 3;
  UINT32 : 9;
  //
  // 20th dword
  //
  UINT32 MaxSpeed4S4S4SModeNotUtilizingDataStrobe : 4;
  UINT32 MaxSpeed4S4S4SModeUtilizingDataStrobe : 4;
  UINT32 MaxSpeed4S4D4DModeNotUtilizingDataStrobe : 4;
  UINT32 MaxSpeed4S4D4DModeUtilizingDataStrobe : 4;
  UINT32 MaxSpeed8S8S8SModeNotUtilizingDataStrobe : 4;
  UINT32 MaxSpeed8S8S8SModeUtilizingDataStrobe : 4;
  UINT32 MaxSpeed8D8D8DModeNotUtilizingDataStrobe : 4;
  UINT32 MaxSpeed8D8D8DModeUtilizingDataStrobe : 4;
  //
  // 21st dword
  //
  UINT32 FastRead1S1D1DSupport : 1;
  UINT32 FastRead1S2D2DSupport : 1;
  UINT32 FastRead1S4D4DSupport : 1;
  UINT32 FastRead4S4D4DSupport : 1;
  UINT32 : 28;
  //
  // 22nd dword
  //
  UINT32 FastRead1S1D1DDummyCycle : 5;
  UINT32 FastRead1S1D1DModeBit : 3;
  UINT32 FastRead1S1D1DOpCode : 8;
  UINT32 FastRead1S2D2DDummyCycle : 5;
  UINT32 FastRead1S2D2DModeBit : 3;
  UINT32 FastRead1S2D2DOpCode : 8;
  //
  // 23rd dword
  //
  UINT32 FastRead1S4D4DDummyCycle : 5;
  UINT32 FastRead1S4D4DModeBit : 3;
  UINT32 FastRead1S4D4DOpCode : 8;
  UINT32 FastRead4S4D4DDummyCycle : 5;
  UINT32 FastRead4S4D4DModeBit : 3;
  UINT32 FastRead4S4D4DOpCode : 8;
//[-end-221128-IB14860873-add]//
} SPI_SFDP_JEDEC_PARAMETER;

typedef struct
{
  SPI_SFDP_HEADER                  Header;
  SPI_SFDP_PARAMETER_HEADER        JedecParameterHeader;
  SPI_SFDP_JEDEC_PARAMETER         JedecParameter;
} SPI_SFDP_TABLE;

VOID
EFIAPI
MemFence (
  VOID
  );

#pragma pack()

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
  );

/**
  Check flash support SFDP or not.
  If support, update relative device information.

  @param FlashDevice            pointer to FLASH_DEVICE structure

  @retval EFI_SUCCESS           Support SFDP and update relative information
  @retval EFI_UNSUPPORTED       Not support SFDP
  @retval EFI_DEVICE_ERROR      Failed to read SFDP

**/
EFI_STATUS
UpdateDevInfoFromSfdp(
  IN OUT FLASH_DEVICE           *FlashDevice
  )
{
  EFI_STATUS                            Status;
  UINT8                                 *DstAddress;
  UINT8                                 RemainingBytes;
  SPI_CONFIG_BLOCK                      *SpiConfigBlock;
  UINT8                                 RDSR;
  UINT8                                 RDSFDP;
  UINT8                                 SizeOut;
  UINT8                                 SizeIn;
  UINT32                                DataFiFo;
  //[-start-240426-IB18160212-modify-]//
  SPI_SFDP_TABLE                        SfdpTable = {{0}, {0}, {0}};
  //[-end-240426-IB18160212-modify-]//
  UINT32                                SfdpParameterAddr;
  UINT32                                FlashDeviceSize;

  if (FlashDevice == NULL) {
    return EFI_UNSUPPORTED;
  }

  SpiConfigBlock = (SPI_CONFIG_BLOCK *)FlashDevice->TypeSpecificInfo;

  //
  // Extrat Spi Command Sets for recognition
  //
  RDSR   = SpiConfigBlock->OpCodeMenu[5];
  RDSFDP = SPI_READ_SFDP_OPCODE;

  //
  // Read SFDP header and JEDEC parameter header
  //
  Status            = EFI_UNSUPPORTED;
  DstAddress        = (UINT8*)(&SfdpTable.Header);
  SfdpParameterAddr = 0;
  RemainingBytes    = sizeof (SPI_SFDP_HEADER) + sizeof (SPI_SFDP_PARAMETER_HEADER);
  while (RemainingBytes > 0) {
     SizeOut  = RemainingBytes;
     SizeIn   = 4;
     DataFiFo = SfdpParameterAddr << 8;
     DataFiFo = SwapBytes32 (DataFiFo);
     Status = SendSpiCmd (RDSFDP, (UINT8 *)&DataFiFo, SizeIn, DstAddress, &SizeOut);
     if (EFI_ERROR (Status)) {
       return EFI_DEVICE_ERROR;
     }

     RemainingBytes    -= SizeOut;
     SfdpParameterAddr += SizeOut;
     DstAddress        += SizeOut;
  }

  //
  // Check SFDP signature
  //
  if (SfdpTable.Header.SfdpSignature == (UINT32)SFDP_SIGNATURE) {
    //
    // Read JEDEC parameter tables.
    //
    Status            = EFI_UNSUPPORTED;
    DstAddress        = (UINT8*)(&SfdpTable.JedecParameter);
    SfdpParameterAddr = SfdpTable.JedecParameterHeader.ParameterTablePointer[0];
//[-start-221128-IB14860873-modify]//
//    RemainingBytes    = SfdpTable.JedecParameterHeader.ParameterTableLen * 4;
    RemainingBytes    = MIN (SfdpTable.JedecParameterHeader.ParameterTableLen * sizeof(UINT32), sizeof(SPI_SFDP_JEDEC_PARAMETER));
//[-end-221128-IB14860873-modify]//
    while (RemainingBytes > 0) {
       SizeOut  = RemainingBytes;
       SizeIn   = 4;
       DataFiFo = SfdpParameterAddr << 8;
       DataFiFo = SwapBytes32 (DataFiFo);
       Status = SendSpiCmd (RDSFDP, (UINT8 *)&DataFiFo, SizeIn, DstAddress, &SizeOut);
       if (EFI_ERROR (Status)) {
         return EFI_DEVICE_ERROR;
       }
  
       RemainingBytes    -= SizeOut;
       SfdpParameterAddr += SizeOut;
       DstAddress        += SizeOut;
    }

    //
    // Update Flash information.
    //
    if (SfdpTable.JedecParameter.BlockEraseSizes == 1) {
      SpiConfigBlock->BlockEraseSize = 0x1000;
    }
    
    if (SfdpTable.JedecParameter.WriteGranularity != 1) {
      SpiConfigBlock->ProgramGranularity = 0;
    } else {
      SpiConfigBlock->ProgramGranularity = 1;
    }
    
    if (SfdpTable.JedecParameter.VolatitleStatusBit != 1) {
      SpiConfigBlock->NVStatusBit = 1;
    } else {
      SpiConfigBlock->NVStatusBit = 0;
    }

    SpiConfigBlock->OpCodeMenu[1] = SfdpTable.JedecParameter.EraseOpcode;

    if ((SfdpTable.JedecParameter.FlashMemoryDensity & BIT31) == BIT31) {
      FlashDeviceSize = 1 << (SfdpTable.JedecParameter.FlashMemoryDensity & 0x7FFFFFFF) / 8;
    } else {
      FlashDeviceSize = (SfdpTable.JedecParameter.FlashMemoryDensity + 1) / 8;
    }
    SpiConfigBlock->DeviceSize = FlashDeviceSize;
  } else {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

/**
  Detect and Initialize SPI flash part OpCode and other parameter through FCH

  @param FlashDevice            pointer to FLASH_DEVICE structure

  @retval EFI_SUCCESS           The SPI device was successfully recognized
  @retval EFI_UNSUPPORTED       The flash device is not supported by this function
  @retval EFI_DEVICE_ERROR      Failed to Recoginize the SPI device

**/
EFI_STATUS
EFIAPI
SpiRecognize (
  IN FLASH_DEVICE                       *FlashDevice
  )
{
  EFI_STATUS                            Status;
  SPI_CONFIG_BLOCK                      *SpiConfigBlock;
  UINT32                                FlashDeviceID;
  UINT8                                 RDID;
  UINT8                                 SizeOut;

  if (FlashDevice == NULL) {
    return EFI_UNSUPPORTED;
  }

  SpiConfigBlock = (SPI_CONFIG_BLOCK *)FlashDevice->TypeSpecificInfo;

  //
  // Always support if this is a common type of spi device
  //
  if (FlashDevice->DeviceInfo.Id == ID_COMMON_SPI_DEVICE) {
    //
    // ToDo: Update DeviceInfo by SFDP
    //
    UpdateDevInfoFromSfdp (FlashDevice);
    return EFI_SUCCESS;
  }

  //
  // Extrat Spi Command Sets for recognition
  //
  RDID = SpiConfigBlock->OpCodeMenu[0];

  //
  // Read Device ID for recognition
  //
  Status        = EFI_UNSUPPORTED;
  FlashDeviceID = 0;
  SizeOut       = (UINT8) SpiConfigBlock->FlashIDSize & 0x03;
  Status = SendSpiCmd (RDID, NULL, 0, (UINT8 *) &FlashDeviceID, &SizeOut);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  if (FlashDeviceID != FlashDevice->DeviceInfo.Id) {
    return EFI_UNSUPPORTED;
  }

  //
  // Found a matching SPI device
  //
  DEBUG ((EFI_D_INFO, "Found SPI Flash Type in SPI Flash Device Library\n"));
  DEBUG ((EFI_D_INFO, "  Device Vendor ID = 0x%08x!\n", FlashDevice->DeviceInfo.Id));

  return EFI_SUCCESS;
}

/**
  Erase the SPI flash device from LbaWriteAddress through FCH

  @param FlashDevice            pointer to FLASH_DEVICE structure
  @param FlashAddress           Target address to be erased
  @param Size                   The size in bytes to be erased

  @retval EFI_SUCCESS           The SPI device was successfully recognized
  @retval EFI_UNSUPPORTED       The flash device is not supported by this function
  @retval EFI_DEVICE_ERROR      Failed to erase the target address

**/
EFI_STATUS
EFIAPI
SpiErase(
  IN  FLASH_DEVICE              *FlashDevice,
  IN  UINTN                     FlashAddress,
  IN  UINTN                     Size
  )
{
  EFI_STATUS                    Status;
  UINT8                         SpiStatus;
  EFI_PHYSICAL_ADDRESS          DstAddress;
  UINT32                        SwappedAddress;
  UINTN                         RemainingBytes;
  SPI_CONFIG_BLOCK              *SpiConfigBlock;
  UINT8                         SE;
  UINT8                         RDSR;
  UINT8                         WREN;
  UINT8                         SizeOut;

  if (FlashDevice == NULL) {
    return EFI_UNSUPPORTED;
  }

  SpiConfigBlock = (SPI_CONFIG_BLOCK *)FlashDevice->TypeSpecificInfo;

  //
  // Make sure the flash address alignment on sector/block
  //
  FlashAddress &= ~(SpiConfigBlock->BlockEraseSize - 1);

  //
  // Convert to physical address if need be.
  // Calculate device offset
  //
  DstAddress = (UINTN)(FlashAddress - (0x100000000 - (UINT64)SpiConfigBlock->DeviceSize));

  //
  // Extrat Spi Command Sets for recognition
  //
  SE   = SpiConfigBlock->OpCodeMenu[1];
  RDSR = SpiConfigBlock->OpCodeMenu[5];
  WREN = SpiConfigBlock->PrefixMenu[0];

  //
  // Execute Erase Command
  //
  Status         = EFI_DEVICE_ERROR;
  RemainingBytes = Size;
  while (RemainingBytes > 0) {

    SizeOut = 1;
    do {
      //
      // To issue read status to make sure the spi rom is ready to take command
      // Stalls until any pending SPI transactions are complete
      //
      Status = SendSpiCmd (RDSR, NULL, 0, &SpiStatus, &SizeOut);
      if (EFI_ERROR (Status)) {
        goto Exit;
      }
      if ((SpiStatus & 1) != 0) {
        continue;
      }
      //
      // To set the Write Enable Latch (WEL) bit before sending the Page Program
      //
      Status = SendSpiCmd (WREN, NULL, 0, NULL, NULL);
      if (EFI_ERROR (Status)) {
          goto Exit;
      }
      Status = SendSpiCmd (RDSR, NULL, 0, &SpiStatus, &SizeOut);
      if (EFI_ERROR (Status)) {
        goto Exit;
      }
    } while ((SpiStatus & 2) == 0);

    //
    // Ready to Sector Erase
    //
    SwappedAddress = SwapBytes32 ((UINT32)DstAddress);
    SwappedAddress >>= 8;
    Status = SendSpiCmd (SE, (UINT8 *)&SwappedAddress, 3, NULL, NULL);
    if (EFI_ERROR (Status)) {
      goto Exit;
    }

    //
    // Stalls until any pending SPI transactions are complete
    // to avoid race condition
    //
    SizeOut = 1;
    do {
      Status = SendSpiCmd (RDSR, NULL, 0, &SpiStatus, &SizeOut);
      if (EFI_ERROR (Status)) {
        goto Exit;
      }
    } while ((SpiStatus & 1) != 0);

    MemFence();
    
    DstAddress     += SpiConfigBlock->BlockEraseSize;
    RemainingBytes -= SpiConfigBlock->BlockEraseSize;
  }

  DEBUG ((EFI_D_INFO, "SpiErase: FlashAddress: 0x%lx Size: 0x%x\n", FlashAddress, Size));

Exit:
  //
  // Actual number of bytes erased
  //
  Size -= RemainingBytes;

  return Status;
}

/**
  Write the SPI flash device with given address and size through FCH

  @param FlashDevice            pointer to FLASH_DEVICE structure
  @param FlashAddress           Destination Offset
  @param SrcAddress             Source Offset
  @param SPIBufferSize          The size for programming
  @param LbaWriteAddress        Write Address

  @retval EFI_SUCCESS           The SPI device was successfully recognized
  @retval EFI_UNSUPPORTED       The flash device is not supported by this function
  @retval EFI_DEVICE_ERROR      Failed to erase the target address

**/
EFI_STATUS
SpiProgram (
  IN  FLASH_DEVICE              *FlashDevice,
  IN  UINT8                     *FlashAddress,
  IN  UINT8                     *SrcAddress,
  IN  UINTN                     *SPIBufferSize,
  IN  UINTN                     LbaWriteAddress
  )
{
  EFI_STATUS                    Status;
  UINTN                         Length;
  UINTN                         DstAddress;
  UINT32                        SwappedAddress;
  UINTN                         RemainingBytes;
  SPI_CONFIG_BLOCK              *SpiConfigBlock;
  UINT8                         RDSR;
  UINT8                         WREN;
  UINT8                         PP;
  UINT8                         SpiStatus;
  UINT8                         SizeOut;
  UINT8                         SizeIn;
  UINT8                         DataFiFo[MAX_SPI_FIFO_SIZE];

  if (FlashDevice == NULL) {
    return EFI_UNSUPPORTED;
  }

  //
  // Extrat Spi Command Sets
  //
  SpiConfigBlock = (SPI_CONFIG_BLOCK *)FlashDevice->TypeSpecificInfo;
  RDSR = SpiConfigBlock->OpCodeMenu[5];
  WREN = SpiConfigBlock->PrefixMenu[0];
  PP   = SpiConfigBlock->OpCodeMenu[2];

  //
  // The device programs only the last 256 data bytes sent to the device.
  // If the eight least significant address bits (A7-A0) are not all 0,
  // all transmitted data going beyond the end of the current page are programmed 
  // from the start address of the same page (from the address A7-A0 are all 0).
  // If more than 256 bytes are sent to the device, the data of the
  // last 256-byte is programmed at the requested page and previous data will be disregarded.
  //
  Status         = EFI_SUCCESS;
  Length         = 0;
  DstAddress     = (UINTN) FlashAddress;
  RemainingBytes = *SPIBufferSize;
  while (RemainingBytes > 0) {
    //
    // Calibrate 256-byte boundary
    //
    if ((DstAddress & 0xFF) == 0) {
      Length = (RemainingBytes > 0x100) ? 0x100 : RemainingBytes;
    } else {
      Length = (RemainingBytes > ((~DstAddress + 1) & 0xFF)) ? ((~DstAddress + 1) & 0xFF) : RemainingBytes;
    }
    
    SizeOut = 1;
    do {
      //
      // To issue read status to make sure the spi rom is ready to take command
      // Stalls until any pending SPI transactions are complete
      //
      Status = SendSpiCmd (RDSR, NULL, 0, &SpiStatus, &SizeOut);
      if (EFI_ERROR (Status)) {
        goto Exit;
      }
      if ((SpiStatus & 1) != 0) {
        continue;
      }
      //
      // To set the Write Enable Latch (WEL) bit before sending the Page Program
      //
      Status = SendSpiCmd (WREN, NULL, 0, NULL, NULL);
      if (EFI_ERROR (Status)) {
          goto Exit;
      }
      Status = SendSpiCmd (RDSR, NULL, 0, &SpiStatus, &SizeOut);
      if (EFI_ERROR (Status)) {
        goto Exit;
      }
    } while ((SpiStatus & 2) == 0);

    if (!PcdGetBool (PcdSpiMmioAccessEnable)) {
      //
      // To issue SPI PP instruction for write.
      // Adject transmitted data length to match FIFO buffer size.
      //
      Length = (Length > MAX_SPI_WIRTE_BYTES_PER_TIME) ? MAX_SPI_WIRTE_BYTES_PER_TIME : Length;
      SwappedAddress = (UINT32)(DstAddress - (0x100000000 - (UINT64)SpiConfigBlock->DeviceSize));
      SwappedAddress = SwapBytes32 (SwappedAddress);
      SwappedAddress >>= 8;
      CopyMem ((VOID *)DataFiFo, (VOID*)&SwappedAddress, 3);
      CopyMem ((VOID *)(DataFiFo+3), SrcAddress, Length);
      SizeIn = (UINT8)Length + 3;
      Status = SendSpiCmd (PP, DataFiFo, SizeIn, NULL, 0);
      if (EFI_ERROR (Status)) {
        goto Exit;
      }
    } else {
      //
      // This function is implemented specifically for those platforms
      // at which the SPI device is memory mapped for write. So this
      // function just do a memory copy for Spi Flash write.
      //
      CopyMem ((VOID *)DstAddress, SrcAddress, Length);
      AsmWbinvd ();
    }

    //
    // Stalls until any pending SPI transactions are complete
    // to avoid race condition
    //
    SizeOut = 1;
    do {
      Status = SendSpiCmd (RDSR, NULL, 0, &SpiStatus, &SizeOut);
      if (EFI_ERROR (Status)) {
        goto Exit;
      }
    } while ((SpiStatus & 1) != 0);

    MemFence();

    DEBUG_CODE_BEGIN ();
      //
      //  Read it back to ensure programming successfully
      //
      AsmWbinvd ();
      ASSERT (!CompareMem ((VOID *)DstAddress, SrcAddress, Length));
    DEBUG_CODE_END ();
    
    RemainingBytes -= Length;
    DstAddress     += Length;
    SrcAddress     += Length;
  }

//   DEBUG ((EFI_D_INFO, "SpiProgram: DstAddress: 0x%lx Size: 0x%x, SrcAddress: 0x%x\n", DstAddress, *SPIBufferSize, SrcAddress));

Exit:
  //
  // Flush cache after accomplishing to avoid mismatch when CopyMem
  //
  AsmWbinvd ();

  //
  // Actual number of bytes written
  //
  *SPIBufferSize -= RemainingBytes;

  return Status;
}

/**
  Read the SPI flash device with given address and size through FCH

  @param FlashDevice            pointer to FLASH_DEVICE structure
  @param DstAddress             Destination buffer address
  @param FlashAddress           The flash device address to be read
  @param BufferSize             The size to be read

  @retval EFI_SUCCESS           The SPI device was successfully recognized
  @retval EFI_UNSUPPORTED       The flash device is not supported by this function
  @retval EFI_DEVICE_ERROR      Failed to erase the target address

**/
EFI_STATUS
SpiRead (
  IN  FLASH_DEVICE              *FlashDevice,
  IN  UINT8                     *DstAddress,
  IN  UINT8                     *FlashAddress,
  IN  UINTN                     BufferSize
  )
{
  ASSERT ((DstAddress != NULL) && (BufferSize != 0));
//[-start-220714-IB148600822-add]//
  if (((UINTN)FlashAddress > BASE_4GB) || (((UINTN)FlashAddress + BufferSize) > BASE_4GB) ||
     ((UINTN)FlashAddress < PcdGet32 (PcdFlashAreaBaseAddress))) {
    return EFI_INVALID_PARAMETER;
  }
//[-end-220714-IB148600822-add]//  
  //
  // This function is implemented specifically for those platforms
  // at which the SPI device is memory mapped for read. So this
  // function just do a memory copy for Spi Flash Read.
  //
  AsmWbinvd ();
  CopyMem (DstAddress, (VOID *) FlashAddress, BufferSize);

  return EFI_SUCCESS;
}


/**
  Get flash number from SPI Descriptor.

  @param[out] FlashNumber       Number of SPI flash devices returned.

  @retval EFI_SUCCESS           Function successfully returned.
  @retval EFI_UNSUPPORTED       The SPI flash is not in Descriptor mode.
**/
EFI_STATUS
GetSpiFlashNumber (
  OUT   UINT8    *FlashNumber
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Offset the BIOS address from top of 4G memory address to correct BIOS region
  described in SPI Descriptor.

  @param[in]  MemoryAddress     BIOS memory mapped address.
  @param[out] BiosRegionAddress Flash ROM start address + BIOS address in flash ROM.

  @retval EFI_SUCCESS           Function successfully returned.
  @retval EFI_UNSUPPORTED       The SPI flash is not in Descriptor mode.
**/
EFI_STATUS
MemoryToBiosRegionAddress (
  IN UINTN       MemoryAddress,
  OUT UINTN      *BiosRegionAddress
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Get flash table from platform.

  @param[in, out] DataBuffer    IN: the input buffer address.
                                OUT:the flash region table from rom file.

  @retval EFI_SUCCESS           Function successfully returned.
**/
EFI_STATUS
GetSpiPlatformFlashTable (
  IN OUT   UINT8    *DataBuffer
  )
{
  return EFI_UNSUPPORTED;
}
