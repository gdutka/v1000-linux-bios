/* $NoKeywords:$ */
/**
 * @file
 *
 * PSP Flash Access DXE Library
 * It will route the Flash access to SMM libary through SMM communication protocol
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AMD PSP UEFI Driver
 * @e sub-project   PspStorage Library
 * @e \$Revision$   @e \$Date$
 *
 *//*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include <PiDxe.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/AmdPspFlashAccLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/SmmCommunication.h>

#include "AGESA.h"
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <AmdPspSmmCommunication.h>
#include <Filecode.h>

#define FILECODE LIBRARY_AMDPSPFLASHACCLIBDXE_AMDPSPFLASHACCLIBDXE_FILECODE

EFI_SMM_COMMUNICATION_PROTOCOL *mSmmCommunication = NULL;
BOOLEAN mPspFlashAccSmmCommReady = FALSE;

extern EFI_GUID gPspFlashAccSmmCommReadyProtocolGuid;

EFI_STATUS
InitSmmComProtocol (
  VOID
  )
{
  EFI_STATUS  Status;
  VOID        *Interface;

  //Check if gPspFlashAccSmmCommReadyProtocolGuid is already installed
  if (mPspFlashAccSmmCommReady == FALSE) {
    Status = gBS->LocateProtocol (&gPspFlashAccSmmCommReadyProtocolGuid, NULL, &Interface);
    if (EFI_ERROR (Status)) {
      //if gPspFlashAccSmmCommReadyProtocolGuid is NOT installed, return directly
      return Status;
    }
    mPspFlashAccSmmCommReady = TRUE;
  }

  if (mSmmCommunication == NULL) {
    Status = gBS->LocateProtocol (&gEfiSmmCommunicationProtocolGuid, NULL, (VOID **) &mSmmCommunication);
    ASSERT (Status == EFI_SUCCESS);
    if (EFI_ERROR (Status)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("AmdPspFlashAccLibDxe locate SmmCommunicationProtocol fail \n");
      return Status;
    }
  }
  return EFI_SUCCESS;
}
/**
 * Retrieve block size of Flash Device
 *
 * @param[in]       Lba             Start LBA of PSP NVRAM region
 * @param[in]       BlockSize       Size of each block
 * @param[in, out]  NumberOfBlocks  Number of block
 *
 * @retval EFI_SUCCESS      Initial success
 * @retval Others           Error happens during initialize
 */
EFI_STATUS
EFIAPI
PspGetFlashBlockSize (
     OUT   UINTN                                       *BlockSize
  )
{
  EFI_STATUS  Status;
  UINT8       *Buffer;
  EFI_SMM_COMMUNICATE_HEADER *SmmCommBuff;
  PSP_SMM_COMM_GETBLKSIZE  *PspSmmGetBlkBuff;
  UINTN       SmmCommBufferSize;

  Status = InitSmmComProtocol ();
  if (EFI_ERROR (Status)) {
    return (EFI_NOT_FOUND);
  }
  //Init SMM communication buffer header
  Buffer = (UINT8 *) (UINTN) PcdGet64 (PcdAmdSmmCommunicationAddress);
  ASSERT (Buffer != NULL);
  if (Buffer != NULL) {
    ZeroMem (Buffer, PSP_SMM_COMM_GETBLKSIZE_BUFFER_SIZE);
    SmmCommBuff = (EFI_SMM_COMMUNICATE_HEADER *) Buffer;
    CopyGuid (&SmmCommBuff->HeaderGuid, &gPspSmmCommHandleGuid);
    SmmCommBuff->MessageLength = sizeof (PSP_SMM_COMM_GETBLKSIZE);
    //Init PSP SMM communicate private data
    PspSmmGetBlkBuff = (PSP_SMM_COMM_GETBLKSIZE *) &SmmCommBuff->Data;
    PspSmmGetBlkBuff->id = PSP_SMM_COMM_ID_GET_BLOCK_SIZE;


    //Communicate AMD SMM communication handler to save boot script inside SMM
    SmmCommBufferSize = PSP_SMM_COMM_GETBLKSIZE_BUFFER_SIZE;
    Status = mSmmCommunication->Communicate (mSmmCommunication, &Buffer[0], &SmmCommBufferSize);

    *BlockSize = PspSmmGetBlkBuff->BlockSize;
    IDS_HDT_CONSOLE_PSP_TRACE ("PspGetFlashBlockSizeDxe [%x] %x \n", Status, PspSmmGetBlkBuff->BlockSize);
  }
  return Status;
}

/**
 * Read data from Flash device
 *
 * @param[in]       FlashAddress Physical flash address
 * @param[in, out]  NumBytes     Number in Byte; return Bytes been read on output
 * @param[in]       Buffer       Buffer contain the read data (Allocated by caller)
 *
 * @retval EFI_SUCCESS      Initial success
 * @retval Others           Error happens during initialize
 */
EFI_STATUS
EFIAPI
PspReadFlash (
  IN       UINTN                                        FlashAddress,
  IN OUT   UINTN                                        *NumBytes,
     OUT   UINT8                                        *Buffer
  )
{
  EFI_STATUS  Status;
  UINT8       *RwBuffer;
  EFI_SMM_COMMUNICATE_HEADER *SmmCommBuff;
  PSP_SMM_COMM_RW_FLASH  *PspSmmRwFlashBuff;
  UINTN       SmmCommBufferSize;
  UINTN ByteCounter;
  UINT8 *CurrentBuffer;
  UINTN Length;

  Status = InitSmmComProtocol ();
  if (EFI_ERROR (Status)) {
    return (EFI_NOT_FOUND);
  }
  //Check input Parameters
  if ((*NumBytes == 0) || (Buffer == NULL)) {
    return (EFI_INVALID_PARAMETER);
  }
  //Init SMM communication buffer header
  RwBuffer = (UINT8 *) (UINTN) PcdGet64 (PcdAmdSmmCommunicationAddress);

  ASSERT (RwBuffer != NULL);
  if (RwBuffer != NULL) {
    ZeroMem (RwBuffer, PSP_SMM_COMM_RWFLASH_BUFFER_SIZE);
    SmmCommBuff = (EFI_SMM_COMMUNICATE_HEADER *) RwBuffer;
    CopyGuid (&SmmCommBuff->HeaderGuid, &gPspSmmCommHandleGuid);
    SmmCommBuff->MessageLength = sizeof (PSP_SMM_COMM_RW_FLASH);
    //Init PSP SMM communicate private data
    PspSmmRwFlashBuff = (PSP_SMM_COMM_RW_FLASH *) &((EFI_SMM_COMMUNICATE_HEADER *) RwBuffer)->Data;
    PspSmmRwFlashBuff->id = PSP_SMM_COMM_ID_READ_FLASH;
    SmmCommBufferSize = PSP_SMM_COMM_RWFLASH_BUFFER_SIZE;

    for (ByteCounter = 0; ByteCounter < *NumBytes;) {
      PspSmmRwFlashBuff->FlashAddress = FlashAddress + ByteCounter;
      CurrentBuffer = Buffer + ByteCounter;
      Length = *NumBytes - ByteCounter;
      if (Length > SIZE_4KB) {
        Length = SIZE_4KB;
      }
      PspSmmRwFlashBuff->NumBytes = Length;

      //Communicate AMD SMM communication handler to save boot script inside SMM
      Status = mSmmCommunication->Communicate (mSmmCommunication, RwBuffer, &SmmCommBufferSize);

      ASSERT (Length == PspSmmRwFlashBuff->NumBytes);
      CopyMem (CurrentBuffer, PspSmmRwFlashBuff->Buffer, Length);
      ByteCounter += Length;
      IDS_HDT_CONSOLE_PSP_TRACE ("PspReadFlashDxe [%x] %x %x %x \n", Status, PspSmmRwFlashBuff->FlashAddress, PspSmmRwFlashBuff->NumBytes, PspSmmRwFlashBuff->Buffer);
    }

    *NumBytes = ByteCounter;
  }

  return Status;
}

/**
 * Write data to Flash device
 *
 * @param[in]       FlashAddress Physical flash address
 * @param[in, out]  NumBytes     Number in Byte; return Bytes been written on output
 * @param[in]       Buffer       Buffer contain the written data (Allocated by caller)
 *
 * @retval EFI_SUCCESS      Initial success
 * @retval Others           Error happens during initialize
 */
EFI_STATUS
EFIAPI
PspWriteFlash (
  IN       UINTN                                    FlashAddress,
  IN OUT   UINTN                                    *NumBytes,
  IN       UINT8                                    *Buffer
  )
{
  EFI_STATUS  Status;
  UINT8       *RwBuffer;
  EFI_SMM_COMMUNICATE_HEADER *SmmCommBuff;
  PSP_SMM_COMM_RW_FLASH  *PspSmmRwFlashBuff;
  UINTN       SmmCommBufferSize;
  UINTN ByteCounter;
  UINT8 *CurrentBuffer;
  UINTN Length;

  Status = InitSmmComProtocol ();
  if (EFI_ERROR (Status)) {
    return (EFI_NOT_FOUND);
  }
  //Check input Parameters
  if ((*NumBytes == 0) || (Buffer == NULL)) {
    return (EFI_INVALID_PARAMETER);
  }
  //Init SMM communication buffer header
  RwBuffer = (UINT8 *) (UINTN) PcdGet64 (PcdAmdSmmCommunicationAddress);

  ASSERT (RwBuffer != NULL);
  if (RwBuffer != NULL) {
    ZeroMem (RwBuffer, PSP_SMM_COMM_RWFLASH_BUFFER_SIZE);
    SmmCommBuff = (EFI_SMM_COMMUNICATE_HEADER *) RwBuffer;
    CopyGuid (&SmmCommBuff->HeaderGuid, &gPspSmmCommHandleGuid);
    SmmCommBuff->MessageLength = sizeof (PSP_SMM_COMM_RW_FLASH);
    //Init PSP SMM communicate private data
    PspSmmRwFlashBuff = (PSP_SMM_COMM_RW_FLASH *) &((EFI_SMM_COMMUNICATE_HEADER *) RwBuffer)->Data;

    PspSmmRwFlashBuff->id = PSP_SMM_COMM_ID_WRITE_FALSH;
    SmmCommBufferSize = PSP_SMM_COMM_RWFLASH_BUFFER_SIZE;

    for (ByteCounter = 0; ByteCounter < *NumBytes;) {
      PspSmmRwFlashBuff->FlashAddress = FlashAddress + ByteCounter;
      CurrentBuffer = Buffer + ByteCounter;
      Length = *NumBytes - ByteCounter;
      if (Length > SIZE_4KB) {
        Length = SIZE_4KB;
      }
      PspSmmRwFlashBuff->NumBytes = Length;
      CopyMem (PspSmmRwFlashBuff->Buffer, CurrentBuffer, Length);

      //Communicate AMD SMM communication handler to save boot script inside SMM
      Status = mSmmCommunication->Communicate (mSmmCommunication, RwBuffer, &SmmCommBufferSize);

      ASSERT (Length == PspSmmRwFlashBuff->NumBytes);
      ByteCounter += Length;
      IDS_HDT_CONSOLE_PSP_TRACE ("PspWriteFlashDxe [%x] %x %x %x \n", Status, PspSmmRwFlashBuff->FlashAddress, PspSmmRwFlashBuff->NumBytes, PspSmmRwFlashBuff->Buffer);
    }

    *NumBytes = ByteCounter;
  }

  return Status;
}

/**
 * Erase Flash region according to input in unit of block size
 *
 *
 * @param[in]       FlashAddress Physical flash address
 * @param[in, out]  NumBytes     Number in Byte; return Bytes been erased on output
 *
 * @retval EFI_SUCCESS      Initial success
 * @retval Others           Error happens during initialize
 */
EFI_STATUS
EFIAPI
PspEraseFlash (
  IN       UINTN                                    FlashAddress,
  IN OUT   UINTN                                    *NumBytes
  )
{
  EFI_STATUS  Status;
  UINT8       *Buffer;
  EFI_SMM_COMMUNICATE_HEADER *SmmCommBuff;
  PSP_SMM_COMM_ERASEFLASH  *PspSmmEraseFlashBuff;
  UINTN       SmmCommBufferSize;

  Status = InitSmmComProtocol ();
  if (EFI_ERROR (Status)) {
    return (EFI_NOT_FOUND);
  }
  //Check input Parameters
  if (*NumBytes == 0) {
    return (EFI_INVALID_PARAMETER);
  }
  //Init SMM communication buffer header
  Buffer = (UINT8 *) (UINTN) PcdGet64 (PcdAmdSmmCommunicationAddress);
  ASSERT (Buffer != NULL);
  if (Buffer != NULL) {
    ZeroMem (Buffer, PSP_SMM_COMM_ERASEFLASH_BUFFER_SIZE);
    SmmCommBuff = (EFI_SMM_COMMUNICATE_HEADER *) Buffer;
    CopyGuid (&SmmCommBuff->HeaderGuid, &gPspSmmCommHandleGuid);
    SmmCommBuff->MessageLength = sizeof (PSP_SMM_COMM_ERASEFLASH);
    //Init PSP SMM communicate private data
    PspSmmEraseFlashBuff = (PSP_SMM_COMM_ERASEFLASH *) &(((EFI_SMM_COMMUNICATE_HEADER *) Buffer)->Data);
    PspSmmEraseFlashBuff->id = PSP_SMM_COMM_ID_ERASE_FALSH;
    PspSmmEraseFlashBuff->FlashAddress = FlashAddress;
    PspSmmEraseFlashBuff->NumBytes = *NumBytes;

    //Communicate AMD SMM communication handler to save boot script inside SMM
    SmmCommBufferSize = PSP_SMM_COMM_ERASEFLASH_BUFFER_SIZE;
    Status = mSmmCommunication->Communicate (mSmmCommunication, Buffer, &SmmCommBufferSize);

    *NumBytes = PspSmmEraseFlashBuff->NumBytes;
    IDS_HDT_CONSOLE_PSP_TRACE ("PspEraseFlash [%x] %x %x \n", Status, PspSmmEraseFlashBuff->FlashAddress, PspSmmEraseFlashBuff->NumBytes);
  }

  return Status;
}


/**
 * Increment the Monotonic counter by 1 inside the Serial Flash Device
 *
 *
 * @param[in]       CounterAddr   CounterAddr for IncMc command
 * @param[in]       CounterData   CounterData for IncMc command
 * @param[in]       Signature     Point to the 32 bytes Signature for IncMc command
 *
 * @retval EFI_SUCCESS      Command success
 * @retval Others           Error happens during handling the command
 */
EFI_STATUS
EFIAPI
PspRpmcIncMc (
  IN UINT8   CounterAddr,
  IN UINT32  CounterData,
  IN UINT8*  Signature
  )
{
  EFI_STATUS  Status;
  UINT8       *Buffer;
  EFI_SMM_COMMUNICATE_HEADER *SmmCommBuff;
  PSP_SMM_COMM_RPMC_INCMC  *PspSmmRpmcIncMcBuff;
  UINTN       SmmCommBufferSize;

  Status = InitSmmComProtocol ();
  if (EFI_ERROR (Status)) {
    return (EFI_NOT_FOUND);
  }
  //Init SMM communication buffer header
  Buffer = (UINT8 *) (UINTN) PcdGet64 (PcdAmdSmmCommunicationAddress);
  ASSERT (Buffer != NULL);
  if (Buffer != NULL) {
    ZeroMem (Buffer, sizeof (PSP_SMM_COMM_RPMC_INCMC));
    SmmCommBuff = (EFI_SMM_COMMUNICATE_HEADER *) Buffer;
    CopyGuid (&SmmCommBuff->HeaderGuid, &gPspSmmCommHandleGuid);
    SmmCommBuff->MessageLength = sizeof (PSP_SMM_COMM_RPMC_INCMC);
    //Init PSP SMM communicate private data
    PspSmmRpmcIncMcBuff = (PSP_SMM_COMM_RPMC_INCMC *) &(((EFI_SMM_COMMUNICATE_HEADER *) Buffer)->Data);
    PspSmmRpmcIncMcBuff->id = PSP_SMM_COMM_ID_RPMC_INCMC;
    PspSmmRpmcIncMcBuff->CounterAddr = CounterAddr;
    PspSmmRpmcIncMcBuff->CounterData = CounterData;
    PspSmmRpmcIncMcBuff->Signature = Signature;

    //Communicate AMD SMM communication handler to save boot script inside SMM
    SmmCommBufferSize = PSP_SMM_COMM_RPMC_INCMC_BUFFER_SIZE;
    Status = mSmmCommunication->Communicate (mSmmCommunication, Buffer, &SmmCommBufferSize);

    IDS_HDT_CONSOLE_PSP_TRACE ("PspRpmcIncMc [%x] %x \n", Status, PspSmmRpmcIncMcBuff->CounterAddr, PspSmmRpmcIncMcBuff->CounterData);
  }

  return Status;
}

/**
 * Request the Monotonic counter value inside the Serial Flash Device
 *
 *
 * @param[in]       CounterAddr   CounterAddr for ReqMc command
 * @param[in]       Tag           Tag for ReqMc command
 * @param[in]       Signature     Point to the 32 bytes Signature for ReqMc command
 * @param[in, out]  CounterData   Point to the output CounterData for ReqMc command
  *
 * @retval EFI_SUCCESS      Command success, CounterData is valid
 * @retval Others           Error happens during handling the command
 */
EFI_STATUS
EFIAPI
PspRpmcReqMc (
  IN   UINT8   CounterAddr,
  IN   UINT8   *Tag,
  IN   UINT8   *Signature,
  IN  OUT UINT32  *CounterData,
  IN  OUT UINT8   *OutputSignature
  )
{
  EFI_STATUS  Status;
  UINT8       *Buffer;
  EFI_SMM_COMMUNICATE_HEADER *SmmCommBuff;
  PSP_SMM_COMM_RPMC_REQMC  *PspSmmRpmcReqMcBuff;
  UINTN       SmmCommBufferSize;

  Status = InitSmmComProtocol ();
  if (EFI_ERROR (Status)) {
    return (EFI_NOT_FOUND);
  }
  //Init SMM communication buffer header
  Buffer = (UINT8 *) (UINTN) PcdGet64 (PcdAmdSmmCommunicationAddress);
  ASSERT (Buffer != NULL);
  if (Buffer != NULL) {
    ZeroMem (Buffer, sizeof (PSP_SMM_COMM_RPMC_REQMC));
    SmmCommBuff = (EFI_SMM_COMMUNICATE_HEADER *) Buffer;
    CopyGuid (&SmmCommBuff->HeaderGuid, &gPspSmmCommHandleGuid);
    SmmCommBuff->MessageLength = sizeof (PSP_SMM_COMM_RPMC_REQMC);
    //Init PSP SMM communicate private data
    PspSmmRpmcReqMcBuff = (PSP_SMM_COMM_RPMC_REQMC *) &(((EFI_SMM_COMMUNICATE_HEADER *) Buffer)->Data);
    PspSmmRpmcReqMcBuff->id = PSP_SMM_COMM_ID_RPMC_REQMC;
    PspSmmRpmcReqMcBuff->CounterAddr = CounterAddr;
    PspSmmRpmcReqMcBuff->Tag = Tag;
    PspSmmRpmcReqMcBuff->Signature = Signature;
    PspSmmRpmcReqMcBuff->CounterData = CounterData;
    PspSmmRpmcReqMcBuff->OutputSignature = OutputSignature;

    //Communicate AMD SMM communication handler to save boot script inside SMM
    SmmCommBufferSize = PSP_SMM_COMM_RPMC_REQMC_BUFFER_SIZE;
    mSmmCommunication->Communicate (mSmmCommunication, Buffer, &SmmCommBufferSize);

    IDS_HDT_CONSOLE_PSP_TRACE ("PspRpmcReqMc [%x] %x %x \n", Status, PspSmmRpmcReqMcBuff->CounterAddr, *PspSmmRpmcReqMcBuff->CounterData);
  }

  return Status;
}
