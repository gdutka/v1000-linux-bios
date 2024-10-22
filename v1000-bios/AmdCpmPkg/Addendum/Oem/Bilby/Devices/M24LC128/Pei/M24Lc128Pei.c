/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <Library/DebugLib.h>
#include <Ppi/I2cMaster.h>
#include <Ppi/M24Lc128Ppi.h>
#include <M24Lc128Pei.h>

STATIC EFI_PEI_M24LC128_PPI mM24Lc128Ppi = {
  M24LC128_PPI_REVISION,
  M24Lc128Write,
  M24Lc128Read
};

STATIC EFI_PEI_PPI_DESCRIPTOR mM24Lc128PpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gM24Lc128PpiGuid,
  &mM24Lc128Ppi
};

/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD CPM M24LC128 PEI driver
 *
 * This function registers the function to publish M24LC128 EEPROM access ppi.
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
M24Lc128PeiEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  // Publish M24LC128 access Ppi
  Status = (*PeiServices)->InstallPpi (
                           PeiServices,
                           &mM24Lc128PpiList
                           );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

EFI_STATUS
EFIAPI
M24Lc128Write (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN UINTN                   BusSelect,
  IN UINTN                   SlaveAddress,
  IN UINT16                  Offset,
  IN UINT32                  Length,
  IN UINT8                   *Data
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_PEI_I2C_MASTER_PPI        *I2cMaster;

  Status = (*PeiServices)->LocatePpi (
     PeiServices,
     &gEfiPeiI2cMasterPpiGuid,
     BusSelect,
     NULL,
     (VOID**)&I2cMaster
     );

  Status = PageWrite(PeiServices, I2cMaster, SlaveAddress, Offset, Length, Data);

  return Status;
}

EFI_STATUS
PageWrite (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  EFI_PEI_I2C_MASTER_PPI     *I2cMaster,
  IN UINTN                   SlaveAddress,
  IN UINT16                  Offset,
  IN UINT32                  Length,
  IN UINT8                   *Data
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_I2C_REQUEST_PACKET  RequestPacket;
  EFI_I2C_OPERATION       *Operation;
  WRITE_PAGE              WritePage;
  UINT32                  partialCount;
  UINT32                  pageSize;
  UINT32                  writeCount;
  UINT16                  address;

  Operation   = RequestPacket.Operation;
  RequestPacket.OperationCount = 1;
  pageSize = M24LC128_PAGESIZE;
  address = Offset;

 // Adjust first write so that it starts within the page and ends at the page boundary
 partialCount = (Offset % pageSize);
 if (partialCount)
 {
   partialCount = pageSize - partialCount;
   partialCount = (partialCount < Length) ? partialCount : Length;
 }

 while (0 < Length)
 {
   writeCount = pageSize;
   if (partialCount)
   {
     writeCount = partialCount;
     partialCount = 0;
   }
   // If there are fewer bytes to write than a single page then adjust down
   else if (pageSize > Length)
   {
     writeCount = Length;
   }

   //Big Enddian Address
   WritePage.prependAddress[0] = address >> 8;
   WritePage.prependAddress[1] = address & 0x00FF;

   (*PeiServices)->CopyMem((UINT8*)&WritePage.TxData[0], Data, writeCount);

   RequestPacket.OperationCount  = 1;
   Operation[0].Flags            = 0;
   Operation[0].LengthInBytes    = sizeof(WritePage.prependAddress) + writeCount;
   Operation[0].Buffer           = (UINT8*)&WritePage.prependAddress[0];

   Status = I2cMaster->StartRequest(I2cMaster, SlaveAddress, &RequestPacket);

   if (Status) {
     break;
   }

   Data = &Data[writeCount];
   Length -= writeCount;
   address += (UINT16)writeCount;
 }
 return Status;
}

EFI_STATUS
EFIAPI
M24Lc128Read (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN UINTN                   BusSelect,
  IN UINTN                   SlaveAddress,
  IN UINT16                  Offset,
  IN UINT32                  Length,
  OUT UINT8                  *Data
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_PEI_I2C_MASTER_PPI        *I2cMaster;
  EFI_I2C_REQUEST_PACKET        *RequestPacket;
  EFI_I2C_OPERATION             *Operation;
  UINT8                         PacketBuffer[sizeof(EFI_I2C_REQUEST_PACKET) + sizeof(EFI_I2C_OPERATION)];
  UINT8 prependAddress[2];

  //Big Enddian Address
  prependAddress[0] = Offset >> 8;
  prependAddress[1] = Offset & 0x00FF;

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
  Operation[0].LengthInBytes    = sizeof(UINT16);
  Operation[0].Buffer           = (UINT8*)&prependAddress;
  Operation[1].Flags            = I2C_FLAG_READ;
  Operation[1].LengthInBytes    = Length;
  Operation[1].Buffer           = Data;
  Status = I2cMaster->StartRequest(I2cMaster, SlaveAddress, RequestPacket);
  return Status;
}
