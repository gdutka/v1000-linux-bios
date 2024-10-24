/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */
#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/I2cMaster.h>
#include <Library/IoLib.h>
#include <AmdCpmPei.h>
#include <Ppi/M24Lc128Ppi.h>
#include <Ppi/Smbus2.h>
#include <Addendum/Oem/Bilby/Pei/AmdCpmOemInitPei/AmdCpmOemInitPeim.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/AmdCbsVariable.h>
#include <Guid/AmdCbsConfig.h>
#include <Dxe/AmdPbsSetupDxe/AmdPbsConfig.h>
#include <Library/AmdPbsConfigLib/AmdPbsConfigLib.h>
#include <Fch.h>

#define AGPIO90 90
#define AGPIO115 115
#define AGPIO116 116
#define EGPIO120 120
#define EGPIO121 121
#define EGPIO131 131

#define REV_A_BOARD 1

EFI_STATUS
EFIAPI
UARTGpioInit (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  );

STATIC EFI_PEI_NOTIFY_DESCRIPTOR mCpmOemTableOverrideNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdCpmGpioInitFinishedPpiGuid,
  UARTGpioInit
};

EFI_STATUS
EFIAPI
ExternalFanSpeedInit (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  );

STATIC EFI_PEI_NOTIFY_DESCRIPTOR mPeiSmbus2PpiGuidNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiSmbus2PpiGuid,
  ExternalFanSpeedInit
};

EFI_STATUS
EFIAPI
PI3DPX1203BWrite (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN UINTN                   BusSelect,
  IN UINTN                   SlaveAddress,
  IN UINT32                  Length,
  IN UINT8                   *Data
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_PEI_I2C_MASTER_PPI        *I2cMaster;
  EFI_I2C_REQUEST_PACKET  RequestPacket;
  EFI_I2C_OPERATION       *Operation;

  Status = (*PeiServices)->LocatePpi (
     PeiServices,
     &gEfiPeiI2cMasterPpiGuid,
     BusSelect,
     NULL,
     (VOID**)&I2cMaster
     );
  
  if (EFI_ERROR (Status)) {
    return Status;
  }

   Operation   = RequestPacket.Operation;
   RequestPacket.OperationCount  = 1;
   Operation[0].Flags            = 0;
   Operation[0].LengthInBytes    = Length;
   Operation[0].Buffer           = Data;

   Status = I2cMaster->StartRequest(I2cMaster, SlaveAddress, &RequestPacket);

  return Status;
}

EFI_STATUS
EFIAPI
PI3DPX1203BRead (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN UINTN                   BusSelect,
  IN UINTN                   SlaveAddress,
  IN UINT32                  Length,
  OUT UINT8                  *Data
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_PEI_I2C_MASTER_PPI        *I2cMaster;
  EFI_I2C_REQUEST_PACKET        *RequestPacket;
  EFI_I2C_OPERATION             *Operation;
  UINT8                         PacketBuffer[sizeof(EFI_I2C_REQUEST_PACKET) + sizeof(EFI_I2C_OPERATION)];

  Status = (*PeiServices)->LocatePpi (
     PeiServices,
     &gEfiPeiI2cMasterPpiGuid,
     BusSelect,
     NULL,
     (VOID**)&I2cMaster
     );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  RequestPacket = (EFI_I2C_REQUEST_PACKET*)PacketBuffer;
  Operation   = RequestPacket->Operation;
  RequestPacket->OperationCount = 1;
  Operation[0].Flags            = I2C_FLAG_READ;
  Operation[0].LengthInBytes    = Length;
  Operation[0].Buffer           = Data;
 

  Status = I2cMaster->StartRequest(I2cMaster, SlaveAddress, RequestPacket);
  return Status;
}


/**
 *Pericom's PI3DPX1203B is the DisplayPort 1.4 compliant, up to 4 channel, 8.1
 *Gbps HBR3 Linear ReDriver with Link Training transparency support. 
 *This function Programs DP port re-driver through i2c interface
 *
 * @param[in]     PeiServices    Pointer to Pei Services
 * @param[in]     SlaveAdd       Slave address of Linear PI3DPX1203B Redriver
 * 
 *
 */

EFI_STATUS
EFIAPI
DpReDriverI2cInit (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  UINT8                         SlaveAdd 
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT8 Data[8]={0,0,0,0xA,0xA,0xA,0xA,0};

  /*  
  Below sequence of bytes are written to DP PI3DPX1203B re-driver  through i2c
  Byte 0 = XX reserved
  Byte 1 = reserved
  Byte 2 = 00H
  Byte 3 = 0AH
  Byte 4 = 0AH
  Byte 5 = 0AH
  Byte 6 = 0AH
  Byte 7 = reserved
  */
 
  Status = PI3DPX1203BWrite (PeiServices, 2, SlaveAdd, 8 , Data);
  DEBUG ((EFI_D_INFO, " Write Status:%r \n",Status));

  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Common function to read the register of Smbus Device.
 *
 * @param[in]     This           Point to Ppi
 * @param[in]     Select         The Smbus number.
 * @param[in]     Address        The address of Smbus device
 * @param[in]     Offset         The Offset of Smbus register
 * @param[in]     Length         The register size in BYTE
 * @param[out]    Value          Data Pointer to save register value
 *
 * @retval        EFI_SUCCESS    SSDT table initialized successfully
 * @retval        EFI_ERROR      Initialization failed
 */

EFI_STATUS
FchSmbusGetByte (
  IN    CONST   EFI_PEI_SERVICES        **PeiServices,
  IN       UINT8                   Select,
  IN       UINT8                   Address,

  IN       UINT8                   Offset,
  IN       UINTN                   Length,
     OUT   UINT8                   *Value
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_SMBUS2_PPI              *SmbusPpi;
  EFI_SMBUS_DEVICE_ADDRESS        SmbusSlaveAddress;
  UINTN                           SmbusLength;
  UINTN                           SmbusOffset;

  Status = (**PeiServices).LocatePpi (
                                      PeiServices,
                                      &gEfiPeiSmbus2PpiGuid,
                                      Select,
                                      NULL,
                                      (VOID**)&SmbusPpi
                                      );
  if (!EFI_ERROR (Status)) {
    SmbusLength = Length;
    SmbusOffset = Offset;
    SmbusSlaveAddress.SmbusDeviceAddress = Address;

    Status = SmbusPpi->Execute (
                                    SmbusPpi,
                                    SmbusSlaveAddress,
                                    SmbusOffset,
                                    EfiSmbusReadByte,
                                    FALSE,
                                    &SmbusLength,
                                    Value
                                    );
  }
  return Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Common function to set the register of Smbus device.
 *
 * @param[in]     This           Point to Ppi
 * @param[in]     Select         The Smbus number.
 * @param[in]     Address        The address of Smbus device
 * @param[in]     Offset         The Offset of Smbus register
 * @param[in]     Length         The register size in BYTE
 * @param[in]     Value          Data Pointer to register value
 *
 * @retval        EFI_SUCCESS    SSDT table initialized successfully
 * @retval        EFI_ERROR      Initialization failed
 */
EFI_STATUS
FchSmbusSetByte (
  IN    CONST   EFI_PEI_SERVICES        **PeiServices,
  IN       UINT8                   Select,
  IN       UINT8                   Address,
  IN       UINT8                   Offset,
  IN       UINTN                   Length,
  IN       UINT8                   *Value
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_SMBUS2_PPI              *SmbusPpi;
  EFI_SMBUS_DEVICE_ADDRESS        SmbusSlaveAddress;
  UINTN                           SmbusLength;
  UINTN                           SmbusOffset;


  Status = (**PeiServices).LocatePpi (
                                      PeiServices,
                                      &gEfiPeiSmbus2PpiGuid,
                                      Select,
                                      NULL,
                                      (VOID**)&SmbusPpi
                                      );
  if (!EFI_ERROR (Status)) {
    SmbusLength = Length;
    SmbusOffset = Offset;
    SmbusSlaveAddress.SmbusDeviceAddress = Address;

    Status = SmbusPpi->Execute (
                                    SmbusPpi,
                                    SmbusSlaveAddress,
                                    SmbusOffset,
                                    EfiSmbusWriteByte,
                                    FALSE,
                                    &SmbusLength,
                                    Value
                                    );
  }
  return Status;
}

/**
 *
 * This function programs the 'EMC2103 Fancontroller' chip registers through 
 * smbus0  to monitor the system fan or external fan based on the board temperature
 *
 * Temp > 10c 10%PWM
 * Temp > 45c 80%PWM
 *
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    PHY Reset complete
 * @retval        EFI_ERROR      PHY Reset failed.
 */

EFI_STATUS
ExternalFanSpeedInit(  
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
 )
{

 //UINT8  Value;
 EFI_STATUS Status = EFI_SUCCESS;
 
 DEBUG ((EFI_D_ERROR, "Programming EMC2103 Fancontroller \n"));

/*
  // Enable Smbus CLK and Data
  MmioWrite8(FCH_IOMUXx71_SCL0_I2C2_SCL_EGPIO113,0);   //Programming IOMUX to enable SCL0
  MmioWrite8(FCH_IOMUXx72_SDA0_I2C2_SDA_EGPIO114,0);   //Programming IOMUX to enable SDA0

 // Look up table configuration
 //Setting 1 => Temperature > 10 C then PWM = 10%
 //Write to 'LUT Drive Setting 1' register(offset 0x51) for PWM = 10% (25/255=10%)
 Value = 25;
 //Fancontroller Slave Address is 0x2E
 Status = FchSmbusSetByte ((CONST EFI_PEI_SERVICES**)PeiServices, 0 , 0x2E, 0x51,1, &Value);
 DEBUG ((EFI_D_ERROR, "Write Status:%r At 0x51 \n",Status));

 //Write 10 C temperature to 'LUT ExtDiode 1 Setting 1'register(offset 0x52)
 Value = 10;
 Status = FchSmbusSetByte ((CONST EFI_PEI_SERVICES**)PeiServices, 0 , 0x2E, 0x52,1, &Value);
 DEBUG ((EFI_D_ERROR, "Write Status:%r At 0x52 \n",Status));

 //Setting 2 => Temperature  > 45 C then PWM = 80% ((204/255=80%))
 //Write to 'LUT Drive Setting 2' for 80% PWM
 Value = 204;
 Status = FchSmbusSetByte ((CONST EFI_PEI_SERVICES**)PeiServices, 0 , 0x2E, 0x56,1, &Value);

 //Write  45 C temperature to 'LUT ExtDiode 1 Setting 2'(0x57) register
 Value = 45;
 Status = FchSmbusSetByte ((CONST EFI_PEI_SERVICES**)PeiServices, 0 , 0x2E, 0x57,1, &Value);
 DEBUG ((EFI_D_ERROR, "Write Status:%r At 0x57 \n",Status));

 //Writting 4 to 'LUT Temp Hysteresis register'(0x79)
 Value = 4;  
 Status = FchSmbusSetByte ((CONST EFI_PEI_SERVICES**)PeiServices, 0 , 0x2E, 0x79,1, &Value);
 DEBUG ((EFI_D_ERROR, "Smbus Write Status:%r At 0x79 Value:%x \n",Status,Value));

 //Write to LUT configuration register(0x50)
 //Setting Bit4 to enable PWM mode, Setting Bit5-Look Up Table entries are locked
 Value = 0x30;  
 Status = FchSmbusSetByte ((CONST EFI_PEI_SERVICES**)PeiServices, 0 , 0x2E, 0x50,1, &Value);
 DEBUG ((EFI_D_ERROR, "Smbus 0 -Write Status:%r At 0x50 Value:%x \n",Status,Value));
*/

 return Status;

}


/**
 *
 * This function programs GPIO90 and GPIO121  as high to enable 4 wired UART1 and URAT0
 * Default Bios UART0, UART1 is 4 wired. When UART2 is enabled UART0 becomes as 2 wired 
 * UART. When UART3 is enabled UART1 will become as 2 wired UART
 *
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    
 * @retval        EFI_ERROR      
 */
EFI_STATUS
EFIAPI
UARTGpioInit (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  )
{
  UINT16 Value = 0x0;
  EFI_STATUS Status = EFI_SUCCESS;
  AMD_CPM_TABLE_PPI* AmdCpmTablePpi = NULL;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *Variable;
  UINTN                           VariableSize;
  CBS_CONFIG                      CbsVariable;


  DEBUG((DEBUG_INFO, "In UARTGpioInit\n"));

  (**PeiServices).SetMem (&CbsVariable, sizeof (CBS_CONFIG), 0);

  Status = (**PeiServices).LocatePpi ((CONST EFI_PEI_SERVICES**)PeiServices,&gEfiPeiReadOnlyVariable2PpiGuid,0,NULL,(VOID**)&Variable);
  if (!EFI_ERROR(Status)) {
    VariableSize = 0;
    Status = Variable->GetVariable (Variable, L"AmdSetup", &gCbsSystemConfigurationGuid, NULL, &VariableSize, &CbsVariable);
    if ((Status == EFI_BUFFER_TOO_SMALL) && (VariableSize == sizeof (CBS_CONFIG))) {
      Status = Variable->GetVariable (Variable, L"AmdSetup", &gCbsSystemConfigurationGuid, NULL, &VariableSize, &CbsVariable);
      if (EFI_ERROR(Status)) {
        DEBUG((DEBUG_INFO, "gCbsSystemConfigurationGuid GetVariable Status = %r\n", Status));
        return Status;
      }
    } else {
      DEBUG((DEBUG_INFO, "gCbsSystemConfigurationGuid GetVariable Status = %r\n", Status));
      return Status;
    }
  }

  Status = (*PeiServices)->LocatePpi (
                           (CONST EFI_PEI_SERVICES**)PeiServices,
                           &gAmdCpmTablePpiGuid,
                           0,
                           NULL,
                           (VOID**)&AmdCpmTablePpi
                           );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOemInitPpiTable Status = %r\n", Status));
    return Status;
  }



  // when UART2 is enabled UART0 works as 2 wired UART
  if(CbsVariable.CbsCmnFchUart2Config == 1) {
	  if(PcdGet8 (PcdBoardRevision)== REV_A_BOARD) {
	  //
      //  To enable UART0 as 2-wire UART make GPIO115 as Low
      //
      Value = GPIO_CONTROL_REG(GPIO_FUNCTION_1, GPIO_OUTPUT_LOW, GPIO_PU_EN, GPIO_STICKY_DIS);
      DEBUG((DEBUG_INFO, "GPIO115 as Low Value = %x \n", Value));
      AmdCpmTablePpi->CommonFunction.SetGpio(AmdCpmTablePpi, AGPIO115, Value);
	  } else {
      //
      //  To enable UART0 as 2-wire UART make EGPIO121 as Low
      //
      Value = GPIO_CONTROL_REG(GPIO_FUNCTION_1, GPIO_OUTPUT_LOW, GPIO_PD_EN, GPIO_STICKY_DIS);
      DEBUG((DEBUG_INFO, "GPIO121 as Low Value = %x \n", Value));
      AmdCpmTablePpi->CommonFunction.SetGpio(AmdCpmTablePpi, EGPIO121, Value);
    }

  } else {
      if(PcdGet8 (PcdBoardRevision)== REV_A_BOARD) {
        //
        //  To enable UART0 as 4-wire UART with transceiver make GPIO115 as high
        //
        Value = GPIO_CONTROL_REG(GPIO_FUNCTION_1, GPIO_OUTPUT_HIGH, GPIO_PU_EN, GPIO_STICKY_DIS);
        DEBUG((DEBUG_INFO, "GPIO115 is high Value = %x\n", Value));
        AmdCpmTablePpi->CommonFunction.SetGpio(AmdCpmTablePpi, AGPIO115, Value);
      } else {
        //
        //  To enable UART0 as 4-wire UART with transceiver make EGPIO121 as high
        //
        Value = GPIO_CONTROL_REG(GPIO_FUNCTION_1, GPIO_OUTPUT_HIGH, GPIO_PD_EN, GPIO_STICKY_DIS);
        DEBUG((DEBUG_INFO, "GPIO121 is high Value = %x\n", Value));
        AmdCpmTablePpi->CommonFunction.SetGpio(AmdCpmTablePpi, EGPIO121, Value);
      }
  }
  // when UART3 is enabled UART1 works as 2 wired UART
  //if(CbsVariable.CbsCmnFchUart3Config == 1) {
    //
    //  To enable UART1 as 2-wire UART make GPIO90 is Low
    //
  //  Value = GPIO_CONTROL_REG(GPIO_FUNCTION_2, GPIO_OUTPUT_LOW, GPIO_PU_EN, GPIO_STICKY_DIS);
  //  DEBUG((DEBUG_INFO, "GPIO90 is Low Value = %x\n", Value));
  //  AmdCpmTablePpi->CommonFunction.SetGpio(AmdCpmTablePpi, AGPIO90, Value);
  //} else {
    //
    //  To enable UART1 as 4-wire UART with transceiver make GPIO90 is high
    //
  //  Value = GPIO_CONTROL_REG(GPIO_FUNCTION_2, GPIO_OUTPUT_HIGH, GPIO_PU_EN, GPIO_STICKY_DIS);
  //  DEBUG((DEBUG_INFO, "GPIO90 is high Value = %x\n", Value));
  //  AmdCpmTablePpi->CommonFunction.SetGpio(AmdCpmTablePpi, AGPIO90, Value);
  //}


  
  return Status;
}

EFI_STATUS
EFIAPI
DPReDriverProgramming (
    IN CONST EFI_PEI_SERVICES     **PeiServices 
    )
{

  UINT8 SlaveAdd;
  EFI_STATUS        Status;

  DEBUG((DEBUG_INFO, "In DPReDriverProgramming\n"));


  // Enable I2C2 CLK and Data
  MmioWrite8(FCH_IOMUXx71_SCL0_I2C2_SCL_EGPIO113,1);   //Programming IOMUX to enable I2C2_SCL
  MmioWrite8(FCH_IOMUXx72_SDA0_I2C2_SDA_EGPIO114,1);   //Programming IOMUX to enable I2C2_SDA

  SlaveAdd = 0x71;
  Status = DpReDriverI2cInit(PeiServices,SlaveAdd);  //For DP port 0
  SlaveAdd = 0x73;
  Status = DpReDriverI2cInit(PeiServices,SlaveAdd);  //For DP port 1
  SlaveAdd = 0x75;
  Status = DpReDriverI2cInit(PeiServices,SlaveAdd);  //For DP port 2
  SlaveAdd = 0x77;
  Status = DpReDriverI2cInit(PeiServices,SlaveAdd);  //For DP port 3

  return Status;
}

EFI_STATUS
EFIAPI
GetBoardRevision (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  UINT8                         *BoardRev 
  )
{
  EFI_PEI_M24LC128_PPI            *M24Lc128Ppi;
  EFI_STATUS                       Status;
  UINT8                            BRev[2] = { 0 , 0};
  UINT8    IOMUXx13Value,IOMUXx14Value;
  UINT32						   BoardID = 0;
  UINT32						   Data32 = 0;
  // store the IOMUX values; later these values will be restored
  IOMUXx13Value = MmioRead8(FCH_IOMUXx13_SCL1_I2C3_SCL_EGPIO19); 
  IOMUXx14Value = MmioRead8(FCH_IOMUXx14_SDA1_I2C3_SDA_EGPIO20); 

  // Enable I2C3 CLK and Data
  MmioWrite8(FCH_IOMUXx13_SCL1_I2C3_SCL_EGPIO19,1);   //Programming IOMUX to enable I2C3_SCL
  MmioWrite8(FCH_IOMUXx14_SDA1_I2C3_SDA_EGPIO20,1);   //Programming IOMUX to enable I2C3_SDA

  //Read the EEPROM(0x50 slave address) offset 0x12 to know the Board Revision from i2c3
  Status = (*PeiServices)->LocatePpi (PeiServices, &gM24Lc128PpiGuid, 0, NULL, (VOID**)&M24Lc128Ppi);
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "Could not locate gM24Lc128PpiGuid\n"));
    goto Exit;
  }
/* <Arista: Do not read board ID from eeprom, it does not exist
  Status = M24Lc128Ppi->Read (PeiServices, 3, 0x50, 0x12, 1, BRev);
  
  if (EFI_ERROR (Status)) {
      Status = M24Lc128Ppi->Read (PeiServices, 3, 0x50, 0x12, 1, BRev);
      if (EFI_ERROR (Status)) {
		  DEBUG((EFI_D_ERROR,"I2C Read Failed Status:%r",Status));
          goto Exit;
      }
  }
*/

// Data32 = MmioRead32 (ACPI_MMIO_BASE + GPIO_BANK0_BASE + 0x14); //Read GPIO5- Board_ID0 status
//    DEBUG((EFI_D_ERROR,"Data32=%x\n", Data32));
 Data32 = (MmioRead32 (ACPI_MMIO_BASE + GPIO_BANK0_BASE + 0x14) & 0x00010000); //Read GPIO5- Board_ID0 status
 BoardID =+ (Data32 >> 16); // move to bit 0
 Data32 = (MmioRead32 (ACPI_MMIO_BASE + GPIO_BANK0_BASE + 0x18) & 0x00010000); //Read GPIO6- Board_ID1 status
 BoardID = BoardID + (Data32 >> 15); // move to bit 1
 Data32 = (MmioRead32 (ACPI_MMIO_BASE + GPIO_BANK0_BASE + 0x1C) & 0x00010000); //Read GPIO7- Board_ID2 status
 BoardID = BoardID + (Data32 >> 14); // move to bit 2 
 Data32 = (MmioRead32 (ACPI_MMIO_BASE + GPIO_BANK0_BASE + 0xA0) & 0x00010000); //Read GPIO40- Board_ID3 status
 BoardID = BoardID + (Data32 >> 13); // move to bit 3 
    DEBUG((EFI_D_ERROR,"BoardID from GPIO5-7, GPIO40 = %x\n", BoardID));


  BRev[0] = (UINT8)(BoardID & 0x000000FF);
   *BoardRev = BRev[0];
  DEBUG((EFI_D_ERROR,"GetBoardRevision Status:%r BoardId:%x \n",Status,BRev[0]));

Exit:
  // Restore the IOMUX values
  MmioWrite8(FCH_IOMUXx13_SCL1_I2C3_SCL_EGPIO19,IOMUXx13Value);   
  MmioWrite8(FCH_IOMUXx14_SDA1_I2C3_SDA_EGPIO20,IOMUXx14Value); 
  
  return Status;
}


/**
 This is the declaration of platform customize PEIM entry point.

 @param[in]         FileHandle          Handle of the file being invoked.
 @param[in]         PeiServices         Describes the list of possible PEI Services.

 @retval            EFI_SUCCESS         The operation completed successfully.
 @retval            Others              An unexpected error occurred.
*/
EFI_STATUS
EFIAPI
PlatformCustomizePeiEntryPoint (
  IN EFI_PEI_FILE_HANDLE             FileHandle,
  IN CONST EFI_PEI_SERVICES        **PeiServices
  )
{
  UINT8    BoardRev = 0;
  UINT8    IOMUXx71Value,IOMUXx72Value;
  EFI_STATUS                      Status;

  DEBUG((DEBUG_INFO, "In PlatformCustomizePeiEntryPoint\n"));

  // store the IOMUX values; later these values will be restored
  IOMUXx71Value = MmioRead8(FCH_IOMUXx71_SCL0_I2C2_SCL_EGPIO113); 
  IOMUXx72Value = MmioRead8(FCH_IOMUXx72_SDA0_I2C2_SDA_EGPIO114); 
  
  PcdSetBool (PcdSbTsiAlertComparatorModeEn, TRUE);
  DPReDriverProgramming(PeiServices);

  GetBoardRevision (PeiServices , &BoardRev);
  DEBUG((DEBUG_INFO, "BoardRev = %x \n", BoardRev));
  PcdSet8 (PcdBoardRevision, BoardRev);
  

  Status = (**PeiServices).NotifyPpi (PeiServices, &mCpmOemTableOverrideNotify);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "PlatformCustomizePeiEntryPoint, Notification of mCpmOemTableOverrideNotify failed, Status = %r\n", Status));
    return Status;
  }
  Status = (**PeiServices).NotifyPpi (PeiServices, &mPeiSmbus2PpiGuidNotify);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "PlatformCustomizePeiEntryPoint, Notification of mPeiSmbus2PpiGuidNotify failed, Status = %r\n", Status));
    return Status;
  }

  // Restore the IOMUX values
  MmioWrite8(FCH_IOMUXx71_SCL0_I2C2_SCL_EGPIO113,IOMUXx71Value);   
  MmioWrite8(FCH_IOMUXx72_SDA0_I2C2_SDA_EGPIO114,IOMUXx72Value);   


  return EFI_SUCCESS;
}
