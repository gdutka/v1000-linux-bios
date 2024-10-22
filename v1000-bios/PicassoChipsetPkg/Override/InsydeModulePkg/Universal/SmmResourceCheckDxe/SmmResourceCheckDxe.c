/** @file
  Driver checks SMM resource in ready to boot callback function.

  This is a COMBINED_SMM_DXE type driver and DXE part uses SMM communication

;******************************************************************************
;* Copyright (c) 2022, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include "SmmResourceCheckDxe.h"

//[-start-240426-IB18160212-modify-]//
EFI_GUID                     mSmmResourceCommunicationGuid = {0x4b52e4da, 0x60eb, 0x4ec2, {0xa8, 0xc, 0xf9, 0xfd, 0xa, 0xb8, 0x5e, 0x97}};
//[-end-240426-IB18160212-modify-]//
EFI_SMM_COMMUNICATE_HEADER   *mSmmMemoryInfo;
UINTN                        mSmramRangeCount;
EFI_SMRAM_DESCRIPTOR         *mSmramRanges;
VOID                         *mSmmAddressBuffer[MAX_ALLOCATE_COUNT];

/**
  Get SMM memory information. It includes total SMM memory size and remaining SMM
  memory size.

  @param[out]     MemorySize       Total SMM memory size in bytes.
  @param[out]     RemainingSize    Remaiing SMM memory size in bytes.

  @retval EFI_SUCCESS              Get SMM memory information successfully.
  @retval EFI_INVALID_PARAMETER    MemorySize or RemainingSize is NULL.
  @return Other                    Other error in this function.
*/
EFI_STATUS
EFIAPI
GetSmmMemoryInfo (
  OUT    UINTN                             *MemorySize,
  OUT    UINTN                             *RemainingSize
  )
{
  EFI_STATUS                                Status;
  EFI_SMM_COMMUNICATE_HEADER                *SmmCommunicateBufHeader;
  SMM_MEMORY_INFO                           *SmmMemoryInfo;
  EFI_SMM_COMMUNICATION_PROTOCOL            *SmmCommunication;
  UINTN                                     CommSize;

  Status = gBS->LocateProtocol (&gEfiSmmCommunicationProtocolGuid, NULL, (VOID **) &SmmCommunication);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Initial Communication buffer
  //
  SmmCommunicateBufHeader = mSmmMemoryInfo;
  CopyGuid (&SmmCommunicateBufHeader->HeaderGuid, &mSmmResourceCommunicationGuid);
  SmmCommunicateBufHeader->MessageLength = (SMM_RESOURCE_COMM_BUFF_SIZE - OFFSET_OF (EFI_SMM_COMMUNICATE_HEADER, Data));
  SmmMemoryInfo = (SMM_MEMORY_INFO *) SmmCommunicateBufHeader->Data;
  SmmMemoryInfo->ResourceHeader.Function = GET_SMM_MEMORY_INFO;
  SmmMemoryInfo->ResourceHeader.ReturnStatus = EFI_UNSUPPORTED;

  CommSize = SMM_RESOURCE_COMM_BUFF_SIZE;
  Status = SmmCommunication->Communicate (SmmCommunication, mSmmMemoryInfo, &CommSize);
  ASSERT_EFI_ERROR (Status);

  if (EFI_ERROR (SmmMemoryInfo->ResourceHeader.ReturnStatus)) {
    return SmmMemoryInfo->ResourceHeader.ReturnStatus;
  }
  *MemorySize    = SmmMemoryInfo->MemoryInfo.TotalSmmMemorySize;
  *RemainingSize = SmmMemoryInfo->MemoryInfo.RemainingSmmMemorySize;
  return EFI_SUCCESS;
}

/**
  Function to get remaining SMM memory size in bytes.

  @return The remaining SMM memory size in bytes.
**/
STATIC
UINTN
GetRemainingSmmMemorySize (
  VOID
  )
{
  UINTN    RemainingSmmMemorySize;
  UINTN    AllocateSize;
  UINTN    AllocateCount;
  UINTN    Index;

  //
  // Try to find the largest free SMM memory by AllocatePool function first.
  //
  RemainingSmmMemorySize = 0;
  AllocateSize           = EFI_PAGE_SIZE;
  while (TRUE) {
    mSmmAddressBuffer[0] = AllocatePool (AllocateSize);
    if (mSmmAddressBuffer[0] != NULL) {
      FreePool (mSmmAddressBuffer[0]);
      AllocateSize *= 2;
    } else {
      break;
    }
  }
  if (AllocateSize == EFI_PAGE_SIZE) {
    return RemainingSmmMemorySize;
  }
  AllocateSize /= 2;
  mSmmAddressBuffer[0] = AllocatePool (AllocateSize);
  if (mSmmAddressBuffer[0] == NULL) {
    return RemainingSmmMemorySize;
  }
  //
  // Using AllocatePool function to calculate free SMM memory size.
  //
  RemainingSmmMemorySize += AllocateSize;
  AllocateSize /= 2;
  for (Index = 1; Index < MAX_ALLOCATE_COUNT; Index++) {
    mSmmAddressBuffer[Index] = AllocatePool (AllocateSize);
    while (mSmmAddressBuffer[Index] == NULL) {
      if (AllocateSize <= EFI_PAGE_SIZE) {
        break;
      }
      AllocateSize /= 2;
      mSmmAddressBuffer[Index] = AllocatePool (AllocateSize);
    }
    if (mSmmAddressBuffer[Index] != NULL) {
      RemainingSmmMemorySize += AllocateSize;
    } else {
      break;
    }
  }
  AllocateCount = Index;

  //
  // Free all of the allocated SMM memory in this function
  //
  for (Index = 0; Index < AllocateCount; Index++) {
    FreePool (mSmmAddressBuffer[Index]);
  }

  return RemainingSmmMemorySize;
}

/**
  Function to get total SMM memory size in bytes.

  @return The total SMM memory size in bytes.
**/
STATIC
UINTN
GetTotalSmmMemorySize (
  VOID
  )
{
  UINTN    TotalSmmMemorySize;
  UINTN    Index;

  TotalSmmMemorySize = 0;
  for (Index = 0; Index < mSmramRangeCount; Index ++) {
     TotalSmmMemorySize += (UINTN) mSmramRanges[Index].PhysicalSize;
  }
  return TotalSmmMemorySize;
}

/**
  Function to update SMM memory information. it includes total SMM memory size and remaining SMM
  memory size

  @param[out]  SmmMemoryInfo  Pointer to SMM_MEMORY_INFO instance.

**/
STATIC
VOID
UpdateSmmMemoryInfo (
  OUT SMM_MEMORY_INFO      *SmmMemoryInfo
  )
{
  SmmMemoryInfo->MemoryInfo.TotalSmmMemorySize     = GetTotalSmmMemorySize ();
  SmmMemoryInfo->MemoryInfo.RemainingSmmMemorySize = GetRemainingSmmMemorySize ();
  SmmMemoryInfo->ResourceHeader.ReturnStatus       = EFI_SUCCESS;
}

/**
  This function is SMM Communication call back for get SMM resource information.

  @param[in]     DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]     Context         Points to an optional handler context which was specified when the
                                 handler was registered.
  @param[in,out] CommBuffer      A pointer to a collection of data in memory that will
                                 be conveyed from a non-SMM environment into an SMM environment.
  @param[in,out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS            The interrupt was handled and quiesced. No other handlers
                                 should still be called.
**/
EFI_STATUS
EFIAPI
SmmGetSmmResourceInfoHandler (
  IN  EFI_HANDLE                        DispatchHandle,
  IN  CONST VOID                        *RegisterContext,
  IN  OUT VOID                          *CommBuffer,
  IN  OUT UINTN                         *CommBufferSize
  )
{

  SMM_MEMORY_INFO            *SmmMemoryInfo;

  if (CommBuffer == NULL || CommBufferSize == NULL) {
    return EFI_SUCCESS;
  }
  //
  // Check buffer size, address, must be the memory which is allocated in DXE part entry point to make sure
  // the memory will not overlap SMM RAM.
  //
  if (*CommBufferSize != SMM_RESOURCE_COMM_BUFF_SIZE - SMM_COMMUNICATE_HEADER_SIZE ||
      (UINTN)CommBuffer != (UINTN)mSmmMemoryInfo + SMM_COMMUNICATE_HEADER_SIZE) {
    return EFI_SUCCESS;
  }

  //
  // Copy SMM_MEMORY_INFO to SMRAM to prevent from TOCTOU attack.
  //
  SmmMemoryInfo = AllocateCopyPool (sizeof (SMM_MEMORY_INFO), CommBuffer);
  if (SmmMemoryInfo == NULL) {
    return EFI_SUCCESS;
  }
  switch (SmmMemoryInfo->ResourceHeader.Function) {

  case GET_SMM_MEMORY_INFO:
    UpdateSmmMemoryInfo (SmmMemoryInfo);
    break;

  default:
    break;
  }
  //
  // Restore data from SMRAM to parameter communication buffer.
  //
  CopyMem (CommBuffer, SmmMemoryInfo, sizeof (SMM_MEMORY_INFO));
  FreePool (SmmMemoryInfo);
  return EFI_SUCCESS;
}

/**
  Check SMM resource is wether under threshold.

  @param[in] Event      Event whose notification function is being invoked.
  @param[in] Context    Pointer to the notification function's context.

**/
VOID
EFIAPI
SmmResourceOnReadyToBoot (
  IN      EFI_EVENT               Event,
  IN      VOID                    *Context
  )
{
  UINTN                  MemorySize;
  UINTN                  RemainingSize;
  EFI_STATUS             Status;
  UINTN                  MemoryThreshold;
  H2O_DIALOG_PROTOCOL    *H2ODialog;
  CHAR16                 *String;
  EFI_INPUT_KEY          Key;

  gBS->CloseEvent (Event);
  if (!PcdGetBool (PcdH2OSmmMemorySizeCheckSupported)) {
    return;
  }
  RemainingSize = 0;
  MemorySize    = 0;
  Status = GetSmmMemoryInfo (&MemorySize, &RemainingSize);
  if (EFI_ERROR (Status)) {
    return;
  }
  MemoryThreshold = PcdGet32 (PcdH2OSmmMemorySizeThreshold);
  if (RemainingSize >= MemoryThreshold) {
    return;
  }
  //
  // Display warning message to indicate remaing SMM memory size is too small.
  //
  Status = gBS->LocateProtocol (
                  &gH2ODialogProtocolGuid,
                  NULL,
                  (VOID **) &H2ODialog
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return;
  }
  String = CatSPrint (NULL, L"Remaining SMM memory size is too small!!!\n Remaining Size: 0x%08x\n Threshold Size: 0x%08x\n Total Size: 0x%08x\n", RemainingSize, MemoryThreshold, MemorySize);
  H2ODialog->ConfirmDialog (
               DlgOk,
               FALSE,
               0,
               NULL,
               &Key,
               String
               );
  FreePool (String);
}

/**
  The entry of SmmResourceCheckDxe driver.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       FVB service successfully initialized.
**/
EFI_STATUS
EFIAPI
SmmResourceCheckEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                   Status;
  EFI_HANDLE                   DispatchHandle;
  UINTN                        Size;
  EFI_SMM_ACCESS2_PROTOCOL     *SmmAccess;
  EFI_EVENT                    Event;
  EFI_HANDLE                   Handle;

  if (InSmm ()) {
    //
    // Locate SMM Access2 Protocol
    //
    Status = gBS->LocateProtocol (
                    &gEfiSmmAccess2ProtocolGuid,
                    NULL,
                    (VOID **)&SmmAccess
                    );
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    //
    // Get SMRAM range information
    //
    Size = 0;
    Status = SmmAccess->GetCapabilities (SmmAccess, &Size, NULL);
    ASSERT (Status == EFI_BUFFER_TOO_SMALL);
    mSmramRanges = AllocatePool (Size);
    ASSERT (mSmramRanges != NULL);
    if (mSmramRanges == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    Status = SmmAccess->GetCapabilities (SmmAccess, &Size, mSmramRanges);
    ASSERT_EFI_ERROR (Status);
    mSmramRangeCount = Size / sizeof (EFI_SMRAM_DESCRIPTOR);
    Status = gBS->LocateProtocol (
                    &gEfiCallerIdGuid,
                    NULL,
                    (VOID **) &mSmmMemoryInfo
                    );
    Status = gSmst->SmiHandlerRegister (
                      SmmGetSmmResourceInfoHandler,
                      &mSmmResourceCommunicationGuid,
                      &DispatchHandle
                    );
  } else {
    Status = EFI_SUCCESS;
    mSmmMemoryInfo = AllocateRuntimePool (SMM_RESOURCE_COMM_BUFF_SIZE);
    if (mSmmMemoryInfo == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    Handle = NULL;
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &Handle,
                    &gEfiCallerIdGuid,
                    mSmmMemoryInfo,
                    NULL
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    EfiCreateEventReadyToBootEx (
      TPL_CALLBACK,
      SmmResourceOnReadyToBoot,
      NULL,
      &Event
      );
  }

  return Status;
}
