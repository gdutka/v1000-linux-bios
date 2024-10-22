/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <Library/DebugLib.h>
#include <Ppi/I2cMaster.h>
#include <Ppi/Pca9545aPpi.h>
#include <Pca9545aPei.h>

STATIC EFI_PEI_PCA9545A_PPI mPca9545aPpi = {
  PCA9545A_PPI_REVISION,
  Pca9545aSet,
  Pca9545aGet
};

STATIC EFI_PEI_PPI_DESCRIPTOR mPca9545aPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPca9545aPpiGuid,
  &mPca9545aPpi
};


/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD CPM PCA9545A PEI driver
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
Pca9545aPeiEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  // Publish Pca9545a control Ppi
  Status = (*PeiServices)->InstallPpi (
                           PeiServices,
                           &mPca9545aPpiList
                           );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

EFI_STATUS
EFIAPI
Pca9545aSet (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  IN       UINT8             ControlByte
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_PEI_I2C_MASTER_PPI  *I2cMaster;
  EFI_I2C_REQUEST_PACKET  RequestPacket;
  EFI_I2C_OPERATION       *Operation;

  Status = (*PeiServices)->LocatePpi (
     PeiServices,
     &gEfiPeiI2cMasterPpiGuid,
     BusSelect,
     NULL,
     (VOID**)&I2cMaster
     );

  //Write Byte Operation
  Operation   = RequestPacket.Operation;
  RequestPacket.OperationCount = 1;
  Operation[0].Flags           = 0;
  Operation[0].LengthInBytes   = 1;
  Operation[0].Buffer          = &ControlByte;

  Status = I2cMaster->StartRequest(I2cMaster, SlaveAddress, &RequestPacket);

  return Status;
}

EFI_STATUS
EFIAPI
Pca9545aGet (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  OUT      UINT8             *ControlByte
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_PEI_I2C_MASTER_PPI  *I2cMaster;
  EFI_I2C_REQUEST_PACKET  RequestPacket;
  EFI_I2C_OPERATION       *Operation;

  Status = (*PeiServices)->LocatePpi (
     PeiServices,
     &gEfiPeiI2cMasterPpiGuid,
     BusSelect,
     NULL,
     (VOID**)&I2cMaster
     );

  Operation   = RequestPacket.Operation;
  RequestPacket.OperationCount = 1;
  Operation[0].Flags            = I2C_FLAG_READ;
  Operation[0].LengthInBytes    = 1;
  Operation[0].Buffer           = ControlByte;

  Status = I2cMaster->StartRequest(I2cMaster, SlaveAddress, &RequestPacket);

  return Status;
}
