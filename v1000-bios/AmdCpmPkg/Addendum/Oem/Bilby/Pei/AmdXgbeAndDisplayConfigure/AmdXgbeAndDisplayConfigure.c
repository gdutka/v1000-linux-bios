/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/I2cMaster.h>
#include "AmdXgbeAndDisplayConfigure.h"
#include <Library/AmdBaseLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Dxe/AmdPbsSetupDxe/AmdPbsConfig.h>
#include <Library/AmdPbsConfigLib/AmdPbsConfigLib.h>
#include <Ppi/ReadOnlyVariable2.h>

#define TEST_PCA9535_CONFIG     1
#define REV_A_BOARD 1

/*********************************************************************************
 * Name: AmdPca9535Read
 *
 * Description
 *   Call to Read AmdPca9535 I2Cdevices
 *
 * Input
 *   SlaveAdd:      Slave Address of I2C
 *   Offset:        Offset to write to
 *   DataLength:    Size of Data to be Read from device.
 *
 * Output
 *   Data read from device
 *
 *********************************************************************************/
EFI_STATUS
EFIAPI
AmdPca9535Read(
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  IN       UINT8             Offset,
  OUT      UINT16            *Data
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
  RequestPacket->OperationCount = 2;
  Operation[0].Flags            = 0;
  Operation[0].LengthInBytes    = sizeof(UINT8);
  Operation[0].Buffer           = &Offset;
  Operation[1].Flags            = I2C_FLAG_READ;
  Operation[1].LengthInBytes    = sizeof(UINT16);
  Operation[1].Buffer           = (UINT8*)Data;

  Status = I2cMaster->StartRequest(I2cMaster, SlaveAddress, RequestPacket);

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "\n: Failed to StartRequest.\n"));
  }
  
  return Status;
}

/*********************************************************************************
 * Name: AmdPca9535Write
 *
 * Description
 *   Call to write to Pca9535 I2C devices 

 *
 * Input
 *   SlaveAdd:     Slave Address of I2C
 *   Offset:       Offset to write to
 *   Data:         Data to be written on device.
 *
 * Output
 *
 *********************************************************************************/

EFI_STATUS
EFIAPI
AmdPca9535Write(
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  IN       UINT8             Offset,
  IN       UINT16            Data
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_PEI_I2C_MASTER_PPI        *I2cMaster;
  EFI_I2C_REQUEST_PACKET  RequestPacket;
  EFI_I2C_OPERATION       *Operation;
  UINT8                   Buffer[3];

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

  //Prepare write data buffer
  Buffer[0] = Offset;
  Buffer[1] = (UINT8)(Data & 0x00FF); //Low byte
  Buffer[2] = (UINT8)(Data >> 8);     //High byte

  //Write Byte Operation
  Operation   = RequestPacket.Operation;
  RequestPacket.OperationCount = 1;
  Operation[0].Flags           = 0;
  Operation[0].LengthInBytes   = 3;
  Operation[0].Buffer          = &Buffer[0];

  Status = I2cMaster->StartRequest(I2cMaster, SlaveAddress, &RequestPacket);
  
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "AmdPca9535Write: Failed to StartRequest.\n"));
  }
  
  return Status;
}



/*********************************************************************************
 * Name: AmdPca9535OutputPotRegConfig
 *
 * Description
 *   Enable PCA9535 OutputPot register for DP , HDMI , SFP+ and BroadCom Phy Clk
 *
 * Input  SlaveAddress
 *   
 *
 * Output
 *   EFI_SUCCESS : Enable PCA9535 OutputPot register
 *   EFI_ERROR   : Enable PCA9535 OutputPot register failed (see error for more details)
 *
 *********************************************************************************/

EFI_STATUS
AmdPca9535OutputPotRegConfig (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN       UINT8         SlaveAdd 
  )
{
  UINT16 Data;
  UINT8 Offset;
  AMD_PBS_SETUP_OPTION      AmdPbsConfiguration;
  EFI_STATUS Status;
  DEBUG ((DEBUG_INFO, "In AmdPca9535OutputPotRegConfig Start\n"));

  Status = EFI_SUCCESS;
  
  Offset = 2;       // PCA9535 output port register. 
  Data = 0;   
  AmdPca9535Read(PeiServices,2, SlaveAdd, Offset, &Data);
  DEBUG ((EFI_D_ERROR, " PCA9535 OutPut Port register read data = %x \n", Data));
  
  Status = GetAmdPbsConfiguration (&AmdPbsConfiguration);
  
  // DISPLAY_SELECT2 (I/O 0_7) =>  1 -> DP1 , 0 -> HDMI1 
  // DISPLAY_SELECT1 (I/O 0_6) =>  1 -> DP0 , 0 -> HDMI0 
  if(AmdPbsConfiguration.DisplaySelect1 == DP_DISPLAY)
	Data |= (1 << 6);
  else
	Data &= ~(1 << 6);

  if(AmdPbsConfiguration.DisplaySelect2 == DP_DISPLAY)
 	Data |= (1 << 7);
  else
	Data &= ~(1 << 7);

  if(PcdGetBool (PcdXgbeSfp)) {
  	// SFP+_XGB1_DISABLE (I/O 1_3) =>  High:Disable the transmitter  Low : Enable the Transmiter
  	// SFP+_XGB0_DISABLE (I/O 1_7) =>  High:Disable the transmitter  Low : Enable the Transmiter
	// X156M_Disable_L (I/O 0_0 ) =>   Low for SFP+ , High for BroadCom PHY
  	Data &= 0x77FE; // We should not define and modify DISPLAY_SELECTION values here. While & operation keep it as 1.
  	Data |= 0x773E; // We should not define and modify DISPLAY_SELECTION values here. While | operation keep it as 0.
  } else {
  	// X156M_Disable_L (I/O 0_0) =>   Enable 156MHz clock for Broadcom PHY  
  	// High: enable 156MHz clock; LOW: disable the clock.
  	Data |= (1 << 0);  
  }
  
  AmdPca9535Write(PeiServices,2, SlaveAdd, Offset, Data);

#ifdef TEST_PCA9535_CONFIG 
  Data = 0;
  AmdPca9535Read(PeiServices,2, SlaveAdd, Offset, &Data);
  DEBUG ((EFI_D_ERROR, " In AmdPca9535OutputPotRegConfig read data = %x \n", Data));
#endif

  DEBUG ((DEBUG_INFO, "In AmdPca9535OutputPotRegConfig End\n"));
  return (Status);  
 
}

/*********************************************************************************
 * Name: AmdPca9535ConfigPortRegConfig
 *
 * Description
 *   Enable PCA9535 Configure register for inputs and outout ports
 *
 * Input  SlaveAddress
 *   
 *
 * Output
 *   EFI_SUCCESS : Enable PCA9535 Configure register
 *   EFI_ERROR   : Enable PCA9535 Configure register failed (see error for more details)
 *
 *********************************************************************************/

EFI_STATUS
AmdPca9535ConfigPortRegConfig(
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN       UINT8         SlaveAdd 
  )
{
  UINT16 Data;
  UINT8 Offset;

  EFI_STATUS Status;
  DEBUG ((DEBUG_INFO, "In AmdPca9535ConfigPortRegConfig Start\n"));

  Status = EFI_SUCCESS;

  Offset = 6;       // PCA9535 Config register offset
  Data = 0;  
  AmdPca9535Read(PeiServices,2, SlaveAdd, Offset, &Data);
  DEBUG ((EFI_D_ERROR, " PCA9535 Config register read data = %x \n", Data));
  // I/O 0_7 to I/O 0_0 (  0 => output and 1 => input)
  // DISPLAY_SELECT2,DISPLAY_SELECT1,0,0,0,0,0,X156M_DISABLE_L ( 0011 1110 = 0x3E)
  // I/O 1_7 to I/O 1_0 (0111 0111)
  // (0111 0111 0011 1110)
  Data &= 0x773E; // This statement make sure output set as 0 properly
  Data |= 0x773E; // This statement make sure input set as 1 properly
  DEBUG ((EFI_D_ERROR, " PCA9535 Config register before write data = %x \n", Data));
  AmdPca9535Write(PeiServices,2, SlaveAdd, Offset, Data);  
  DEBUG ((DEBUG_INFO, "In AmdPca9535ConfigPortRegConfig End\n")); 
  return (Status);   
}

/*********************************************************************************
 * Name: AmdXgbeAndDisplayConfigurePeiInit
 *
 * Description
 *   Entry point of the AMD XGBE WORK AROUND Pei driver
 *   Perform the required work around needed for XGBE.
 *
 *  @param[in]         FileHandle          Handle of the file being invoked.
 *  @param[in]         PeiServices         Describes the list of possible PEI Services.
 *
 *  @retval            EFI_SUCCESS         Module initialized successfully.
 *  @retval            EFI_ERROR           Initialization failed (see error for more details).
 *
 *********************************************************************************/

EFI_STATUS
EFIAPI
AmdXgbeAndDisplayConfigurePeiInit (
  IN EFI_PEI_FILE_HANDLE             FileHandle,
  IN CONST EFI_PEI_SERVICES        **PeiServices
  )
{
  EFI_STATUS   Status;

  DEBUG ((EFI_D_ERROR, "In AmdXgbeAndDisplayConfigurePeiInit\n"));

  Status = 0;
  // This code needs to be enabled only for REV A Boards.
  if(PcdGet8 (PcdBoardRevision)== REV_A_BOARD) {
    DEBUG ((EFI_D_INFO, "Bilby Rev A board:AmdXgbeAndDisplayConfigurePeiInit\n"));
  	Status = AmdPca9535ConfigPortRegConfig(PeiServices,(UINT8)(PCA9535_ADDRESS));  
  	Status = AmdPca9535OutputPotRegConfig(PeiServices,(UINT8)(PCA9535_ADDRESS));
  }
  
  return (Status);
}
