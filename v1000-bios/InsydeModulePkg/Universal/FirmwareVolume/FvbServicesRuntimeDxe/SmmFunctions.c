/** @file
  Provide support functions for FVB services which can be executed in
  SMM mode.

;******************************************************************************
;* Copyright (c) 2023, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/FlashRegionLib.h>
#include "SmmFunctions.h"
#include "CommonFunctions.h"

SMM_FW_VOL_INSTANCE       *mSmmFwVolInstance;
SMM_FD_FUNCTIONS          mSmmFdFunctions = {FlashRead, FlashProgram, FlashErase};
EFI_GUID                  mSmmFdFunctionsProtocolGuid = SMM_FD_FUNCTIONS_PROTOCOL_GUID;
BOOLEAN                   mSmmReadyToLock;
/**
  Internal function to initialzie module mSmmFwVolInstance

  @retval EFI_SUCCESS   Initialzie mSmmFwVolInstance successful.
  @retval other         Allocate memory failed or locate gEfiSmmFwBlockServiceProtocolGuid or
                        gEfiSmmCpuProtocolGuid failed.
**/
STATIC
EFI_STATUS
InitializeSmmFwVolInstance (
  VOID
  )
{
  EFI_STATUS        Status;
  EFI_HANDLE        Handle;

  mSmmFwVolInstance = FvbAllocateZeroBuffer (sizeof (SMM_FW_VOL_INSTANCE), TRUE);
  ASSERT (mSmmFwVolInstance != NULL);

  Status = mSmst->SmmLocateProtocol (
                    &gEfiSmmFwBlockServiceProtocolGuid,
                    NULL,
                    (VOID **)&mSmmFwVolInstance->SmmFwbService
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &mSmmFdFunctionsProtocolGuid,
                  &mSmmFdFunctions,
                  NULL
                  );

  return Status;
}

/**
  Check the input memory buffer is whether overlap the SMRAM ranges.

  @param[in] Buffer       The pointer to the buffer to be checked.
  @param[in] BufferSize   The size in bytes of the input buffer

  @retval  TURE        The buffer overlaps SMRAM ranges.
  @retval  FALSE       The buffer doesn't overlap SMRAM ranges.
**/
STATIC
BOOLEAN
BufferOverlapSmram (
  IN VOID              *Buffer,
  IN UINTN              BufferSize
  )
{
  EFI_STATUS                                Status;
  STATIC H2O_IHISI_PROTOCOL                 *Ihisi = NULL;

  if (Ihisi == NULL) {
    Status = mSmst->SmmLocateProtocol (
                      &gH2OIhisiProtocolGuid,
                      NULL,
                      (VOID **) &Ihisi
                      );
    if (EFI_ERROR (Status)) {
      return FALSE;
    }
  }
  if (Ihisi->BufferOverlapSmram((VOID *) Buffer, BufferSize)) {
    return TRUE;
  }
  return FALSE;
}

/**
  Communication service SMI Handler entry.
  This SMI handler provides services for the firmware volume access through SMI.

  @param[in]     DispatchHandle               The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]     RegisterContext              Points to an optional handler context which was specified when the
                                              handler was registered.
  @param[in, out] CommBuffer                  A pointer to a collection of data in memory that will
                                              be conveyed from a non-SMM environment into an SMM environment.
  @param[in, out] CommBufferSize              The size of the CommBuffer.

  @retval EFI_SUCCESS                         The interrupt was handled and quiesced. No other handlers
                                              should still be called.
  @retval EFI_WARN_INTERRUPT_SOURCE_QUIESCED  The interrupt has been quiesced but other handlers should
                                              still be called.
  @retval EFI_WARN_INTERRUPT_SOURCE_PENDING   The interrupt is still pending and other handlers should still
                                              be called.
  @retval EFI_INTERRUPT_PENDING               The interrupt could not be quiesced.
**/
EFI_STATUS
EFIAPI
SmmFvbAccessHandler (
  IN EFI_HANDLE     DispatchHandle,
  IN CONST VOID     *Context         OPTIONAL,
  IN OUT VOID       *CommBuffer      OPTIONAL,
  IN OUT UINTN      *CommBufferSize  OPTIONAL
  )
{
  SMM_FVB_BUFFER                           *SmmFvbBuffer;
  UINT8                                    *DataBuffer;
  EFI_STATUS                               Status;
  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL        *SmmFwbService;

  if (mSmmReadyToLock || CommBuffer == NULL || CommBufferSize == NULL) {
    return EFI_SUCCESS;
  }
  //
  // Check buffer size, address, not overlap SMMRAM and the signature to make sure the communication buffer is correct.
  //
  if (*CommBufferSize != mSmmCommunicationBufferSize - sizeof (EFI_SMM_COMMUNICATE_HEADER) + sizeof (UINT8) ||
      (UINTN)CommBuffer != (UINTN)mSmmPhyCommunicationBuffer + sizeof (EFI_SMM_COMMUNICATE_HEADER) - sizeof (UINT8) ||
      ((SMM_FVB_BUFFER *)CommBuffer)->Signature != SMM_FVB_ACCESS_SIGNATURE) {
    return EFI_SUCCESS;
  }
  //
  // Copy SMM_FVB_BUFFER and data buffer to SMRAM to prevent from TOCTOU attack.
  //
  SmmFvbBuffer = AllocateCopyPool (mSmmCommunicationBufferSize - sizeof (EFI_SMM_COMMUNICATE_HEADER) + sizeof (UINT8), CommBuffer);
  if (SmmFvbBuffer == NULL) {
    return EFI_SUCCESS;
  }
  DataBuffer = (UINT8 *)(SmmFvbBuffer + 1);
  //
  // Make sure DataSize isn't larger than buffer size.
  //
  if (SmmFvbBuffer->DataSize > *CommBufferSize - sizeof (SMM_FVB_BUFFER) ||
      BufferOverlapSmram ((VOID *)SmmFvbBuffer->AccessAddress, SmmFvbBuffer->DataSize)) {
    FreePool (SmmFvbBuffer); 
    return EFI_SUCCESS;
  }
  Status        = EFI_UNSUPPORTED;
  SmmFwbService = mSmmFwVolInstance->SmmFwbService;

  switch (SmmFvbBuffer->AccessType) {

  case PfatReadFvb:
    Status = SmmFwbService->Read (
                              SmmFwbService,
                              SmmFvbBuffer->AccessAddress,
                              0,
                              &SmmFvbBuffer->DataSize,
                              DataBuffer
                              );
    break;

  case PfatWriteFvb:
    Status = SmmFwbService->Write (
                              SmmFwbService,
                              SmmFvbBuffer->AccessAddress,
                              &SmmFvbBuffer->DataSize,
                              DataBuffer
                              );
    break;

  case PfatEraseFvb:
    Status = SmmFwbService->EraseBlocks (
                              SmmFwbService,
                              SmmFvbBuffer->AccessAddress,
                              &SmmFvbBuffer->DataSize
                              );
    break;

  default:
    ASSERT (FALSE);
    break;
  }
  //
  // Restore SMM_FVB_BUFFER data from SMRAM before free SMRAM.
  //
  SmmFvbBuffer->Status = Status;
  CopyMem (CommBuffer, SmmFvbBuffer, mSmmCommunicationBufferSize - sizeof (EFI_SMM_COMMUNICATE_HEADER) + sizeof (UINT8));
  FreePool (SmmFvbBuffer);
  return EFI_SUCCESS;
}

/**
  Notification function of gEfiSmmReadyToLockProtocolGuid protocol.

  @param[in] Protocol   Points to the protocol's unique identifier
  @param[in] Interface  Points to the interface instance
  @param[in] Handle     The handle on which the interface was installed

  @retval EFI_SUCCESS   OnSmmReadyToLock runs successfully
**/
STATIC
EFI_STATUS
EFIAPI
OnSmmReadyToLock (
  IN CONST EFI_GUID                       *Protocol,
  IN VOID                                 *Interface,
  IN EFI_HANDLE                           Handle
  )
{
  mSmmReadyToLock = TRUE;
  return EFI_SUCCESS;
}

/**
  Register software SMI callback function to provide interface to access FVB through SMI.

  @retval EFI_SUCCESS   Register callback function successful.
  @retval Other         Locate gEfiSmmSwDispatch2ProtocolGuid or register callback function failed.
**/
STATIC
EFI_STATUS
RegisterSmmAccessCallback (
  VOID
  )
{
  EFI_HANDLE        Handle;
  EFI_STATUS        Status;
  VOID              *Registration;

  Handle = NULL;
  Status = mSmst->SmiHandlerRegister (SmmFvbAccessHandler, &gFvbAccessThroughSmiGuid, &Handle);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = mSmst->SmmRegisterProtocolNotify (
                    &gEfiSmmReadyToLockProtocolGuid,
                    OnSmmReadyToLock,
                    &Registration
                    );
  return Status;
}

/**
  Initailize SMM FVB access service.

  1.Initialize moudle SmmFwVolInstance.
  2.Register SMM callback function to provide SMM FVB access interface.

  @retval EFI_SUCCESS   Initialize SMM FVB access service.
  @retval Other         Any error occurred while initializing SMM FVB service.
**/
EFI_STATUS
InitializeSmmFvbAccess (
  VOID
  )
{
  EFI_STATUS        Status;

  Status = InitializeSmmFwVolInstance ();
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = RegisterSmmAccessCallback ();
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Send the data in communicate buffer to SMM.

  @retval      EFI_SUCCESS            Success is returned from the functin in SMM.
  @return      Others                 Failure is returned from the function in SMM.
**/
EFI_STATUS
SendCommunicateBuffer (
  VOID
  )
{
  EFI_STATUS                                Status;
  UINTN                                     CommSize;
  EFI_SMM_COMMUNICATE_HEADER                *SmmCommunicateHeader;
  SMM_FVB_BUFFER                            *SmmFvbBuffer;

  //
  // Note!!! In runtime, the memory address is virtual address in protected mode but the memory address is
  // physical address in SMM mode. Therefore, if we want to access FV through SMI in runtime, we must set data
  // to virtual address in protected mode and then use physical address to call SMI.
  //
  CommSize = mSmmCommunicationBufferSize;
  Status = mSmmCommunication->Communicate (mSmmCommunication, mSmmPhyCommunicationBuffer, &CommSize);
  ASSERT_EFI_ERROR (Status);

  SmmCommunicateHeader    = (EFI_SMM_COMMUNICATE_HEADER *)mSmmCommunicationBuffer;
  SmmFvbBuffer            = (SMM_FVB_BUFFER *)SmmCommunicateHeader->Data;
  return  SmmFvbBuffer->Status;
}

/**
  According to access to send SMI to do FVB access.

  @param[in]     AccessType     Input access command type. system support three tyeps of command -
                                PfatReadFvb, PfatWriteFvb and PfatEraseFvb.
  @param[in]     AccessAddress  target address to access.
  @param[in,out] AccessLength   In:  input size in bytes to access.
                                Out: only update this update length if AccessType is PfatReadFvb and
                                     read data successful.
  @param[in,out] AccessBuffer   In:  only use this buffer as input buffer to write flash if AccessType
                                     is PfatWriteFvb.
                                Out: only use this buffer as output buffer to read flash if AccessType
                                     is PfatReadFvb.

  @retval EFI_SUCCESS       Data successfully read from flash device.
  @retval EFI_UNSUPPORTED   The flash device is not supported.
  @retval EFI_DEVICE_ERROR  Failed to read the blocks.
**/
STATIC
EFI_STATUS
SendSmiByAccessType (
  IN       UINTN      AccessType,
  IN       UINTN      AccessAddress,
  IN OUT   UINTN      *AccessLength,
  IN OUT   VOID       *AccessBuffer
  )
{
  UINT8                         *WorkingBuf;
  EFI_STATUS                    Status;
  EFI_SMM_COMMUNICATE_HEADER    *SmmCommunicateHeader;
  SMM_FVB_BUFFER                *SmmFvbBuffer;

  ZeroMem (mSmmCommunicationBuffer, mSmmCommunicationBufferSize);
  SmmCommunicateHeader = (EFI_SMM_COMMUNICATE_HEADER *)mSmmCommunicationBuffer;
  CopyGuid (&SmmCommunicateHeader->HeaderGuid, &gFvbAccessThroughSmiGuid);
  SmmCommunicateHeader->MessageLength = mSmmCommunicationBufferSize;
  SmmFvbBuffer = (SMM_FVB_BUFFER *)SmmCommunicateHeader->Data;

  SmmFvbBuffer->Status        = EFI_UNSUPPORTED;
  SmmFvbBuffer->Signature     = SMM_FVB_ACCESS_SIGNATURE;
  SmmFvbBuffer->AccessAddress = AccessAddress;
  SmmFvbBuffer->AccessType    = AccessType;
  SmmFvbBuffer->DataSize      = *AccessLength;

  //
  // copy write data to write buffer before sending SMI
  //
  WorkingBuf = (UINT8 *)(SmmFvbBuffer + 1);
  if (AccessType == PfatWriteFvb) {
    CopyMem (WorkingBuf, AccessBuffer, *AccessLength);
  }
  Status = SendCommunicateBuffer ();
  //
  // Update access length after executing SMI function
  //
  *AccessLength = SmmFvbBuffer->DataSize;
  if (!EFI_ERROR (Status) && AccessType == PfatReadFvb) {
    CopyMem (AccessBuffer, WorkingBuf, *AccessLength);
  }

  return Status;
}

/**
  Function to do flash read through SMI.

  @param[in]      ReadAddress  Target address to be read.
  @param[in, out] ReadLength   In:  Input buffer size in bytes.
                               Out: Total read data size in bytes.
  @param[out]     ReadBuffer   Output buffer to contains read data.

  @retval EFI_SUCCESS       Data successfully read from flash device.
  @retval EFI_UNSUPPORTED   The flash device is not supported.
  @retval EFI_DEVICE_ERROR  Failed to read the blocks.
**/
EFI_STATUS
ReadFdThroughSmi (
  IN       UINTN      ReadAddress,
  IN OUT   UINTN      *ReadLength,
  OUT      VOID       *ReadBuffer
  )
{
  UINTN        RemaindingSize;
  UINTN        ReadSize;
  UINTN        TotalReadSize;
  UINT8        *CurrentBuffer;
  EFI_STATUS   Status;

  RemaindingSize = *ReadLength;
  TotalReadSize  = 0;
  CurrentBuffer  = (UINT8 *) ReadBuffer;
  do {
    if (RemaindingSize > (UINTN) FdmGetNAtSize (&gH2OFlashMapRegionFtwBackupGuid , 1)) {
      ReadSize = (UINTN) FdmGetNAtSize (&gH2OFlashMapRegionFtwBackupGuid , 1);
    } else {
      ReadSize = RemaindingSize;
    }
    RemaindingSize -= ReadSize;
    Status = SendSmiByAccessType (PfatReadFvb, ReadAddress + TotalReadSize, &ReadSize, (VOID *) CurrentBuffer);
    CurrentBuffer += ReadSize;
    TotalReadSize += ReadSize;;
  } while (RemaindingSize != 0 && !EFI_ERROR (Status));

  if (!EFI_ERROR (Status)) {
    *ReadLength = TotalReadSize;
  }

  return Status;
}

/**
  Function to do flash write through SMI.

  @param[in]      WriteAddress  Target address to write.
  @param[in, out] WriteLength   In:  Input buffer size in bytes.
                                Out: Total write data size in bytes.
  @param[out]     WriteBuffer   input buffer to write.

  @retval EFI_SUCCESS       Data successfully write to flash device.
  @retval EFI_UNSUPPORTED   The flash device is not supported.
  @retval EFI_DEVICE_ERROR  Failed to write the blocks.
**/
EFI_STATUS
WriteFdThroughSmi (
  IN       UINTN      WriteAddress,
  IN OUT   UINTN      *WriteLength,
  IN       VOID       *WriteBuffer
  )
{
  UINTN        RemaindingSize;
  UINTN        WriteSize;
  UINTN        TotalWriteSize;
  UINT8        *CurrentBuffer;
  EFI_STATUS   Status;

  RemaindingSize = *WriteLength;
  TotalWriteSize  = 0;
  CurrentBuffer  = (UINT8 *) WriteBuffer;
  do {
    if (RemaindingSize > (UINTN) FdmGetNAtSize (&gH2OFlashMapRegionFtwBackupGuid , 1)) {
      WriteSize = (UINTN) FdmGetNAtSize (&gH2OFlashMapRegionFtwBackupGuid , 1);
    } else {
      WriteSize = RemaindingSize;
    }
    RemaindingSize -= WriteSize;
    Status = SendSmiByAccessType (PfatWriteFvb, WriteAddress + TotalWriteSize, &WriteSize, (VOID *) CurrentBuffer);
    CurrentBuffer += WriteSize;
    TotalWriteSize += WriteSize;;
  } while (RemaindingSize != 0 && !EFI_ERROR (Status));

  if (!EFI_ERROR (Status)) {
    *WriteLength = TotalWriteSize;
  }

  return Status;
}

/**
  Function to do flash erase through SMI.

  @param[in] EraseAddress   Target address to erase.
  @param[in] EraseLength    Erase size in bytes.

  @retval EFI_SUCCESS           Erase flash block successful.
  @retval EFI_UNSUPPORTED       The flash device is not supported.
  @retval EFI_DEVICE_ERROR      Failed to erase blocks.
**/
EFI_STATUS
EraseFdThroughSmi (
  IN  UINTN      EraseAddress,
  IN  UINTN      EraseLength
  )
{
  return SendSmiByAccessType (PfatEraseFvb, EraseAddress, &EraseLength, NULL);
}
