/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <Library/DebugLib.h>
#include <Ppi/I2cMaster.h>
#include <Ppi/Pca9535aPpi.h>
#include <Pca9535aPei.h>

STATIC EFI_PEI_PCA9535A_PPI mPca9535aPpi = {
  PCA9535A_PPI_REVISION,
  Pca9535aSet,
  Pca9535aGet
};

STATIC EFI_PEI_PPI_DESCRIPTOR mPca9535aPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPca9535aPpiGuid,
  &mPca9535aPpi
};


/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD CPM PCA9535A PEI driver
 *
 * This function registers the function to publish PCA9545A EEPROM access ppi.
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
Pca9535aPeiEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  // Publish Pca9535a control Ppi
  Status = (*PeiServices)->InstallPpi (
                           PeiServices,
                           &mPca9535aPpiList
                           );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

EFI_STATUS
EFIAPI
Pca9535aSet (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  IN       UINT8             Offset,
  IN       UINT16            Data
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_PEI_I2C_MASTER_PPI  *I2cMaster;
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

  return Status;
}

EFI_STATUS
EFIAPI
Pca9535aGet (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  IN       UINT8             Offset,
  OUT      UINT16            *Data
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_PEI_I2C_MASTER_PPI  *I2cMaster;
  UINT8                   PacketBuffer[sizeof(EFI_I2C_REQUEST_PACKET) + sizeof(EFI_I2C_OPERATION)];
  EFI_I2C_REQUEST_PACKET  *RequestPacket;
  EFI_I2C_OPERATION       *Operation;

  Status = (*PeiServices)->LocatePpi (
     PeiServices,
     &gEfiPeiI2cMasterPpiGuid,
     BusSelect,
     NULL,
     (VOID**)&I2cMaster
     );

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

  return Status;
}
