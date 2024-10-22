/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Protocol/I2cMaster.h>
#include <Protocol/Pca9535aProtocol.h>
#include "Pca9535aDxe.h"

extern  EFI_BOOT_SERVICES       *gBS;

EFI_DXE_PCA9535A_PROTOCOL mDxePca9535aProtocol = {
  PCA9535A_DXE_REVISION,
  Pca9535aSetDxe,
  Pca9535aGetDxe
};

//
// Driver Global Data
//

/*********************************************************************************
 * Name: Pca9535aDxeInit
 *
 * Description
 *   Entry point of the AMD PCA9535A DXE driver
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
Pca9535aDxeInit (
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
                &gPca9535aProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &mDxePca9535aProtocol
                );

  return (Status);
}

EFI_STATUS
EFIAPI
Pca9535aSetDxe (
  IN       EFI_DXE_PCA9535A_PROTOCOL  *This,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  IN       UINT8             Offset,
  IN       UINT16            Data
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_I2C_MASTER_PROTOCOL  *I2cMaster;
  EFI_I2C_REQUEST_PACKET  RequestPacket;
  EFI_I2C_OPERATION       *Operation;
  UINT8                   Buffer[3];

  Status = gBS->LocateProtocol (&gEfiI2cMasterProtocolGuid, NULL, (VOID**)&I2cMaster);
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

  Status = I2cMaster->StartRequest(I2cMaster, SlaveAddress, &RequestPacket, NULL, NULL);

  return Status;
}

EFI_STATUS
EFIAPI
Pca9535aGetDxe (
  IN       EFI_DXE_PCA9535A_PROTOCOL  *This,
  IN       UINTN             BusSelect,
  IN       UINTN             SlaveAddress,
  IN       UINT8             Offset,
  OUT      UINT16            *Data
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_I2C_MASTER_PROTOCOL  *I2cMaster;
  UINT8                   PacketBuffer[sizeof(EFI_I2C_REQUEST_PACKET) + sizeof(EFI_I2C_OPERATION)];
  EFI_I2C_REQUEST_PACKET  *RequestPacket;
  EFI_I2C_OPERATION       *Operation;

  Status = gBS->LocateProtocol (&gEfiI2cMasterProtocolGuid, NULL, (VOID**)&I2cMaster);
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

  Status = I2cMaster->StartRequest(I2cMaster, SlaveAddress, RequestPacket, NULL, NULL);

  return Status;
}
