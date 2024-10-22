/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Protocol/I2cMaster.h>
#include <Protocol/Pca9545aProtocol.h>
#include "Pca9545aDxe.h"

extern  EFI_BOOT_SERVICES       *gBS;

EFI_DXE_PCA9545A_PROTOCOL mDxePca9545aProtocol = {
  PCA9545A_DXE_REVISION,
  Pca9545aSetDxe,
  Pca9545aGetDxe
};

//
// Driver Global Data
//

/*********************************************************************************
 * Name: Pca9545aDxeInit
 *
 * Description
 *   Entry point of the AMD PCA9545A DXE driver
 *   Perform the configuration init, resource reservation, early post init
 *   and install all the supported protocol
 *
 * Input
 *   ImageHandle : EFI Image Handle for the DXE driver
 *   SystemTable : pointer to the EFI system table
 *
 * Output
 *   EFI_SUCCESS : Module initialized successfully
 *   EFI_ERROR   : Initialization failed (see error for more details)
 *
 *********************************************************************************/
EFI_STATUS
EFIAPI
Pca9545aDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS          Status = EFI_SUCCESS;
  EFI_HANDLE          Handle;

  Handle = ImageHandle;

  // Publish Error Log service Protocol For IP driver
  Status = gBS->InstallProtocolInterface (
                &Handle,
                &gPca9545aProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &mDxePca9545aProtocol
                );

  return (Status);
}

EFI_STATUS
EFIAPI
Pca9545aSetDxe (
  IN       EFI_DXE_PCA9545A_PROTOCOL  *This,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  IN       UINT8             ControlByte
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_I2C_MASTER_PROTOCOL  *I2cMaster;
  EFI_I2C_REQUEST_PACKET  RequestPacket;
  EFI_I2C_OPERATION       *Operation;

  Status = gBS->LocateProtocol (&gEfiI2cMasterProtocolGuid, NULL, (VOID**)&I2cMaster);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //Write Byte Operation
  Operation   = RequestPacket.Operation;
  RequestPacket.OperationCount = 1;
  Operation[0].Flags           = 0;
  Operation[0].LengthInBytes   = 1;
  Operation[0].Buffer          = &ControlByte;

  Status = I2cMaster->StartRequest(I2cMaster, SlaveAddress, &RequestPacket, NULL, NULL);

  return Status;
}

EFI_STATUS
EFIAPI
Pca9545aGetDxe (
  IN       EFI_DXE_PCA9545A_PROTOCOL  *This,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  OUT      UINT8             *ControlByte
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_I2C_MASTER_PROTOCOL  *I2cMaster;
  EFI_I2C_REQUEST_PACKET  RequestPacket;
  EFI_I2C_OPERATION       *Operation;

  Status = gBS->LocateProtocol (&gEfiI2cMasterProtocolGuid, NULL, (VOID**)&I2cMaster);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Operation   = RequestPacket.Operation;
  RequestPacket.OperationCount = 1;
  Operation[0].Flags            = I2C_FLAG_READ;
  Operation[0].LengthInBytes    = 1;
  Operation[0].Buffer           = ControlByte;

  Status = I2cMaster->StartRequest(I2cMaster, SlaveAddress, &RequestPacket, NULL, NULL);

  return Status;
}
