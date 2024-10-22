/** @file
  Provide helper functions and misc functions for variable driver

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

#include "Variable.h"
#include "AuthService.h"
#include "VarCheck.h"
#include "VariableLock.h"
#include "VariableCache.h"
#include <Protocol/SmmAccess2.h>

UINTN                                 mSmramRangeCount;
EFI_SMRAM_DESCRIPTOR                  *mSmramRanges;
VAR_ERROR_FLAG                        mCurrentBootVarErrFlag = VAR_ERROR_FLAG_NO_ERROR;
BOOLEAN                               mReadyToBootEventSignaled;
/**
  Initialize SMM RAM information.

  @retval EFI_SUCCESS           Initialize SMM RAM information successfully.
  @retval EFI_OUT_OF_RESOURCES  Allocate memory to save SMM RAM informaiton failed.
  @retval Other                 Any error occurred during initialize SMM RAM information.
**/
EFI_STATUS
EFIAPI
SmmRamInfoInitialize (
  VOID
  )
{

  EFI_STATUS                  Status;
  UINTN                       Size;
  EFI_SMM_ACCESS2_PROTOCOL   *SmmAccess;

  Status = gBS->LocateProtocol (&gEfiSmmAccess2ProtocolGuid, NULL, (VOID **)&SmmAccess);
  ASSERT (Status == EFI_SUCCESS);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  Size = 0;
  Status = SmmAccess->GetCapabilities (SmmAccess, &Size, NULL);
  ASSERT (Status == EFI_BUFFER_TOO_SMALL);
  if (Status != EFI_BUFFER_TOO_SMALL) {
    return Status;
  }
  mSmramRanges = VariableAllocateZeroBuffer (Size, TRUE);
  ASSERT (mSmramRanges != NULL);
  if (mSmramRanges == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = SmmAccess->GetCapabilities (SmmAccess, &Size, mSmramRanges);
  ASSERT_EFI_ERROR (Status);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  mSmramRangeCount = Size / sizeof (EFI_SMRAM_DESCRIPTOR);
  return EFI_SUCCESS;
}

/**
  Check the input memory buffer is whether overlap the SMRAM ranges.

  @param[in] Buffer       The pointer to the buffer to be checked.
  @param[in] BufferSize   The size in bytes of the input buffer

  @retval  TRUE        The buffer overlaps SMRAM ranges.
  @retval  FALSE       The buffer doesn't overlap SMRAM ranges.
**/
BOOLEAN
EFIAPI
BufferOverlapSmram (
  IN VOID              *Buffer,
  IN UINTN              BufferSize
  )
{
  UINTN                 Index;
  EFI_PHYSICAL_ADDRESS  BufferEnd;
  EFI_PHYSICAL_ADDRESS  BufferStart;

  if (mSmst == NULL || Buffer == NULL || BufferSize == 0) {
    return FALSE;
  }

  BufferStart = (EFI_PHYSICAL_ADDRESS) (UINTN) Buffer;
  BufferEnd =   (EFI_PHYSICAL_ADDRESS) ((UINTN) Buffer + BufferSize - 1);

  //
  // Integer overflow check
  //
  if (BufferEnd < BufferStart) {
    return TRUE;
  }

  for (Index = 0; Index < mSmramRangeCount; Index ++) {
    //
    // The condition for two ranges doesn't overlap is:
    // Buffer End is smaller than the range start or Buffer start is larger than the range end.
    // so the overlap condition is above condition isn't satisfied.
    //
    if (!(BufferEnd < mSmramRanges[Index].CpuStart ||
        BufferStart >= (mSmramRanges[Index].CpuStart + mSmramRanges[Index].PhysicalSize))) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Record variable error flag.

  @param[in] Flag               Variable error flag to record.
  @param[in] VariableName       Name of variable.
  @param[in] VendorGuid         Guid of variable.
  @param[in] Attributes         Attributes of the variable.
  @param[in] VariableSize       Size of the variable.
**/
VOID
RecordVarErrorFlag (
  IN VAR_ERROR_FLAG         Flag,
  IN CHAR16                 *VariableName,
  IN EFI_GUID               *VendorGuid,
  IN UINT32                 Attributes,
  IN UINTN                  VariableSize
  )
{
  EFI_STATUS                Status;
  VARIABLE_POINTER_TRACK    Variable;
  VAR_ERROR_FLAG            *VarErrFlag;
  VAR_ERROR_FLAG            TempFlag;
  UINTN                     VariableCount;

  if (!AfterEndOfDxe ()) {
    //
    // Before EndOfDxe, just record the current boot variable error flag to local variable,
    // and leave the variable error flag in NV flash as the last boot variable error flag.
    // After EndOfDxe in InitializeVarErrorFlag (), the variable error flag in NV flash
    // will be initialized to this local current boot variable error flag.
    //
    mCurrentBootVarErrFlag &= Flag;
    return;
  }


  //
  // Record error flag (it should have be initialized).
  //
  VariableCount = 0;
  Status = FindVariableByLifetime (
             VAR_ERROR_FLAG_NAME,
             &gEdkiiVarErrorFlagGuid,
             &Variable,
             &VariableCount,
             &mVariableModuleGlobal->VariableBase
             );
  if (!EFI_ERROR (Status)) {
    VarErrFlag = (VAR_ERROR_FLAG *) GetVariableDataPtr (Variable.CurrPtr);
    TempFlag = *VarErrFlag;
    TempFlag &= Flag;
    if (TempFlag == *VarErrFlag) {
      return;
    }
    Status = UpdateVariableStore (
               &mVariableModuleGlobal->VariableBase,
               FALSE,
               FALSE,
               (UINTN) VarErrFlag,
               sizeof (TempFlag),
               &TempFlag
               );
    if (!EFI_ERROR (Status)) {
      //
      // Update the data in NV cache.
      //
      *VarErrFlag = TempFlag;
    }
  }
}


/**
  Get maximum user variable space which can be used.

  @return maximum user variable space by bytes.
**/
UINTN
GetMaxUserVariableSpace (
  VOID
  )
{
  if (mSmst == NULL) {
    return mVariableModuleGlobal->CommonMaxUserVariableSpace;
  }
  return mSmmVariableGlobal->ProtectedModeVariableModuleGlobal->CommonMaxUserVariableSpace;
}

/**
  Get total user variable size by bytes.

  @return the size by bytes of user variable.
**/
UINTN
GetUserVariableTotalSize (
  VOID
  )
{
  if (mSmst == NULL) {
    return mVariableModuleGlobal->CommonUserVariableTotalSize;
  }
  return mSmmVariableGlobal->ProtectedModeVariableModuleGlobal->CommonUserVariableTotalSize;
}

/**
  Is user variable?

  @param[in] VariableName   Name of Variable to be found.
  @param[in] VendorGuid     Variable vendor GUID.

  @retval TRUE          User variable.
  @retval FALSE         System variable.
**/
BOOLEAN
IsUserVariable (
  IN CHAR16                  *VariableName,
  IN EFI_GUID                *VendorGuid
  )
{
  VAR_CHECK_VARIABLE_PROPERTY   Property;

  //
  // Only after End Of Dxe, the variables belong to system variable are fixed.
  // If PcdMaxUserNvStorageVariableSize is 0, it means user variable share the same NV storage with system variable,
  // then no need to check if the variable is user variable or not specially.
  //
  if (AfterEndOfDxe () && (GetMaxUserVariableSpace () != 0)) {
    if (InternalVarCheckVariablePropertyGet (VariableName, VendorGuid, &Property) == EFI_NOT_FOUND) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  Initialize variable error flag.

  Before EndOfDxe, the variable indicates the last boot variable error flag,
  then it means the last boot variable error flag must be got before EndOfDxe.
  After EndOfDxe, the variable indicates the current boot variable error flag,
  then it means the current boot variable error flag must be got after EndOfDxe.
**/
STATIC
VOID
InitializeVarErrorFlag (
  VOID
  )
{
  EFI_STATUS                Status;
  VARIABLE_POINTER_TRACK    Variable;
  VAR_ERROR_FLAG            Flag;
  VAR_ERROR_FLAG            VarErrFlag;
  UINTN                     VariableCount;

  if (!AfterEndOfDxe ()) {
    return;
  }

  Flag = mCurrentBootVarErrFlag;
  VariableCount = 0;
  Status = FindVariableByLifetime (
             VAR_ERROR_FLAG_NAME,
             &gEdkiiVarErrorFlagGuid,
             &Variable,
             &VariableCount,
             &mVariableModuleGlobal->VariableBase
             );
  if (!EFI_ERROR (Status)) {
    VarErrFlag = *((VAR_ERROR_FLAG *) GetVariableDataPtr (Variable.CurrPtr));
    if (VarErrFlag == Flag) {
      return;
    }
  }

  UpdateVariable (
    VAR_ERROR_FLAG_NAME,
    &gEdkiiVarErrorFlagGuid,
    &Flag,
    sizeof (Flag),
    VARIABLE_ATTRIBUTE_NV_BS_RT,
    0,
    0,
    &Variable,
    NULL,
    &mVariableModuleGlobal->VariableBase
    );
}

/**
  Calculate common user variable total size.
**/
STATIC
VOID
CalculateCommonUserVariableTotalSize (
  VOID
  )
{
  VARIABLE_HEADER               *Variable;
  VARIABLE_HEADER               *NextVariable;
  UINTN                         VariableSize;
  VAR_CHECK_VARIABLE_PROPERTY   Property;

  //
  // Only after End Of Dxe, the variables belong to system variable are fixed.
  // If PcdMaxUserNvStorageVariableSize is 0, it means user variable share the same NV storage with system variable,
  // then no need to calculate the common user variable total size specially.
  //
  if (AfterEndOfDxe ()  && (GetMaxUserVariableSpace () != 0)) {
    Variable = GetStartPointer ((VARIABLE_STORE_HEADER *) mVariableModuleGlobal->NonVolatileVariableCache);
    while (IsValidVariableHeaderInVarRegion (Variable, GetNonVolatileEndPointer ((VARIABLE_STORE_HEADER *) mVariableModuleGlobal->NonVolatileVariableCache))) {
      NextVariable = GetNextVariablePtr (Variable);
      VariableSize = (UINTN) NextVariable - (UINTN) Variable;
      if ((Variable->Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) != EFI_VARIABLE_HARDWARE_ERROR_RECORD) {
        if (InternalVarCheckVariablePropertyGet ((CHAR16 *) (Variable + 1), &Variable->VendorGuid, &Property) == EFI_NOT_FOUND) {
          //
          // No property, it is user variable.
          //
          if (mSmst == NULL) {
            mVariableModuleGlobal->CommonUserVariableTotalSize += VariableSize;
          } else {
            mSmmVariableGlobal->ProtectedModeVariableModuleGlobal->CommonUserVariableTotalSize += VariableSize;
          }
        }
      }

      Variable = NextVariable;
    }
  }
}

/**
  Initialize variable quota.
**/
VOID
InitializeVariableQuota (
  VOID
  )
{
  STATIC BOOLEAN    Initialized = FALSE;

  if (!AfterEndOfDxe () || Initialized) {
    return;
  }
  Initialized = TRUE;

  InitializeVarErrorFlag ();
  CalculateCommonUserVariableTotalSize ();
}

/**
  Calculate the sum of all elements in a buffer in unit of UINT8.
  During calculation, the carry bits are dropped.

  @param  Buffer   Pointer to the buffer to carry out the sum operation.
  @param  Length   The size, in bytes, of Buffer.

  @return The sum of Buffer with carry bits dropped during additions.
**/
UINT16
EFIAPI
InternalCalculateSum16 (
  IN      CONST UINT8         *Buffer,
  IN      UINTN               Length
  )
{
  UINT32    Sum;
  UINTN     Count;

  for (Sum = 0, Count = 0; Count < Length; Count++) {
    Sum = (UINT32) (Sum + *(Buffer + Count));
  }

  Sum = 0x10000 - (Sum & 0x0000ffff);
  return (UINT16) Sum;
}

/**
  Initialize authenticated services.

  @param[in] Event    Event whose notification function is being invoked.
  @param[in] Context  Pointer to the notification function's context.
**/
VOID
EFIAPI
CryptoCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                     Status;
  STATIC BOOLEAN                 Initialized = FALSE;

  if (!Initialized) {
    Status = AuthenticatedVariableServiceInitialize (&mVariableModuleGlobal->VariableBase);
    Initialized = TRUE;
    ASSERT_EFI_ERROR (Status);
  }
  if (Event == NULL) {
    return;
  }

  if (mSmst == NULL) {
    gBS->CloseEvent (Event);
  }
}

/**
  The notification of gEfiCryptoServiceProtocolGuid protocol is installed

  @param[in]  Protocol              Points to the protocol's unique identifier.
  @param[in]  Interface             Points to the interface instance.
  @param[in]  Handle                The handle on which the interface was installed.
**/
EFI_STATUS
EFIAPI
CryptoServicesInstalled (
  IN     CONST EFI_GUID                *Protocol,
  IN     VOID                          *Interface,
  IN     EFI_HANDLE                    Handle
  )
{
  CryptoCallback (NULL, NULL);
  return EFI_SUCCESS;
}

/**
  Notification function of EFI_END_OF_DXE_EVENT_GROUP_GUID event group.

  This is a notification function registered on EFI_END_OF_DXE_EVENT_GROUP_GUID event group.

  @param[in]  Event        Event whose notification function is being invoked.
  @param[in]  Context      Pointer to the notification function's context.
**/
VOID
EFIAPI
OnEndOfDxe (
  IN EFI_EVENT                               Event,
  IN VOID                                    *Context
  )
{
  if (!mVariableModuleGlobal->EndOfDxe) {
    CreateVariableLockListThroughSmi ();
    CreateVariableCheckListThroughSmi ();
  }
  mVariableModuleGlobal->EndOfDxe = TRUE;
  if (PcdGetBool (PcdReclaimVariableSpaceAtEndOfDxe)) {
    ReclaimForOS ();
  }
  InitializeVariableQuota ();
}

/**
  Callback function for ready to boot event.

  @param[in] Event    - Event whose notification function is being invoked.
  @param[in] Context  - Pointer to the notification function's context.
**/
VOID
EFIAPI
ReadyToBootCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  if (mReadyToBootEventSignaled) {
    return;
  }

  if (!mVariableModuleGlobal->EndOfDxe) {
    CreateVariableLockListThroughSmi ();
    CreateVariableCheckListThroughSmi ();
  }
  mVariableModuleGlobal->EndOfDxe = TRUE;
  mReadyToBootEventSignaled = TRUE;
  InitializeVariableQuota ();
}

/**
  Callback function for legacy boot event.

  @param[in] Event   Event whose notification function is being invoked.
  @param[in] Context Pointer to the notification function's context.
**/
VOID
EFIAPI
LegacyBootCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  UINT32       Signature;

  DisableVariableCache ();
  if (mVariableModuleGlobal->SmmCodeReady && mSmst == NULL) {
    Signature = SMM_LEGACY_BOOT_SIGNATURE;
    CopyMem (mSmmPhyVarBuf, &Signature, sizeof (Signature));
    SmmSecureBootCall ((UINT8 *) mSmmPhyVarBuf, sizeof (Signature), LEGACY_BOOT_SMI_FUN_NUM, SW_SMI_PORT);
  }
  gBS->CloseEvent (Event);
  return;
}


/**
  This function allows the caller to determine if UEFI ExitBootServices() has been called.

  This function returns TRUE after all the EVT_SIGNAL_EXIT_BOOT_SERVICES functions have
  executed as a result of the OS calling ExitBootServices().  Prior to this time FALSE
  is returned. This function is used by runtime code to decide it is legal to access
  services that go away after ExitBootServices().

  @retval  TRUE  The system has finished executing the EVT_SIGNAL_EXIT_BOOT_SERVICES event.
  @retval  FALSE The system has not finished executing the EVT_SIGNAL_EXIT_BOOT_SERVICES event.
**/
BOOLEAN
EFIAPI
AfterEndOfDxe (
  VOID
  )
{
  return mSmst == NULL ? mVariableModuleGlobal->EndOfDxe : mSmmVariableGlobal->ProtectedModeVariableModuleGlobal->EndOfDxe;
}

/**
  This function allows the caller to determine if UEFI ExitBootServices() has been called.

  This function returns TRUE after all the EVT_SIGNAL_EXIT_BOOT_SERVICES functions have
  executed as a result of the OS calling ExitBootServices().  Prior to this time FALSE
  is returned. This function is used by runtime code to decide it is legal to access
  services that go away after ExitBootServices().

  @retval  TRUE  The system has finished executing the EVT_SIGNAL_EXIT_BOOT_SERVICES event.
  @retval  FALSE The system has not finished executing the EVT_SIGNAL_EXIT_BOOT_SERVICES event.
**/
BOOLEAN
EFIAPI
VariableAtRuntime (
  VOID
  )
{
  return mSmst == NULL ? mVariableModuleGlobal->AtRuntime : mSmmVariableGlobal->ProtectedModeVariableModuleGlobal->AtRuntime;
}


/**
  Creates and returns a notification event and registers that event with all the protocol
  instances specified by ProtocolGuid.

  This function causes the notification function to be executed for every protocol of type
  ProtocolGuid instance that exists in the system when this function is invoked. In addition,
  every time a protocol of type ProtocolGuid instance is installed or reinstalled, the notification
  function is also executed. This function returns the notification event that was created.
  If ProtocolGuid is NULL, then ASSERT().
  If NotifyTpl is not a legal TPL value, then ASSERT().
  If NotifyFunction is NULL, then ASSERT().
  If Registration is NULL, then ASSERT().

  @param[in] ProtocolGuid    Supplies GUID of the protocol upon whose installation the event is fired.
  @param[in] NotifyTpl       Supplies the task priority level of the event notifications.
  @param[in] NotifyFunction  Supplies the function to notify when the event is signaled.
  @param[in] NotifyContext   The context parameter to pass to NotifyFunction.
  @param[in] Registration    A pointer to a memory location to receive the registration value.
                             This value is passed to LocateHandle() to obtain new handles that
                             have been added that support the ProtocolGuid-specified protocol.

  @return The notification event that was created.
**/
EFI_EVENT
EFIAPI
VariableCreateProtocolNotifyEvent (
  IN  EFI_GUID          *ProtocolGuid,
  IN  EFI_TPL           NotifyTpl,
  IN  EFI_EVENT_NOTIFY  NotifyFunction,
  IN  VOID              *NotifyContext,  OPTIONAL
  OUT VOID              **Registration
  )
{
  EFI_STATUS  Status;
  EFI_EVENT   Event;

  ASSERT (ProtocolGuid != NULL);
  ASSERT (NotifyFunction != NULL);
  ASSERT (Registration != NULL);

  //
  // Create the event
  //

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  NotifyTpl,
                  NotifyFunction,
                  NotifyContext,
                  &Event
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Register for protocol notifications on this event
  //

  Status = gBS->RegisterProtocolNotify (
                  ProtocolGuid,
                  Event,
                  Registration
                  );

  ASSERT_EFI_ERROR (Status);

  return Event;
}

/**
  According to system mode to allocate pool.
  Allocate BootServicesData pool in protect mode if Runtime is FALSE.
  Allocate EfiRuntimeServicesData pool in protect mode if Runtime is TRUE.
  Allocate memory from SMM ram if system in SMM mode.

  @param[in] Size    The size of buffer to allocate
  @param[in] Runtime Runtime Data or not.

  @return Pointer to allocated memory buffer or NULL if allocate failed.
**/
VOID *
VariableAllocateZeroBuffer (
  IN UINTN     Size,
  IN BOOLEAN   Runtime
  )
{
  VOID           *Buffer;
  EFI_STATUS     Status;

  if (mSmst == NULL) {
    Status = gBS->AllocatePool (
                    Runtime ? EfiRuntimeServicesData : EfiBootServicesData,
                    Size,
                    &Buffer
                    );
  } else {
    Status = mSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      Size,
                      &Buffer
                      );
  }

  if (!EFI_ERROR (Status)) {
    ZeroMem (Buffer, Size);
  } else {
    Buffer = NULL;
  }
  return Buffer;
}

/**
  This function uses to read saved CPU double word register by CPU index

  @param  RegisterNum   Register number which want to get
  @param  CpuIndex      CPU index number to get register.
  @param  RegisterData  pointer to output register data

  @retval EFI_SUCCESS   Read double word register successful
  @return Other         Any error occurred while disabling all secure boot SMI functions successful.
**/
STATIC
EFI_STATUS
GetDwordRegisterByCpuIndex (
  IN  EFI_SMM_SAVE_STATE_REGISTER       RegisterNum,
  IN  UINTN                             CpuIndex,
  OUT UINT32                            *RegisterData
  )
{
  return mSmmVariableGlobal->SmmCpu->ReadSaveState (
                                       mSmmVariableGlobal->SmmCpu,
                                       sizeof (UINT32),
                                       RegisterNum,
                                       CpuIndex,
                                       RegisterData
                                       );
}

/**
  This function uses to update current executing CPU to SMM_VARIABLE_INSTANCE

  @retval EFI_SUCCESS     Update current executing CPU successful.
  @retval EFI_NOT_FOUND   Cannot find current executing CPU.
**/
EFI_STATUS
UpdateCurrentExecutingCpu (
  VOID
  )
{
  UINTN           Index;
  UINT32          RegisterEax;
  UINT32          RegisterEdx;
  EFI_STATUS      Status;

  Status = EFI_NOT_FOUND;
  for (Index = 0; Index < mSmst->NumberOfCpus; Index++) {
    GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RAX, Index, &RegisterEax);
    GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RDX, Index, &RegisterEdx);
    if ((RegisterEax & 0xff) == SECURE_BOOT_SW_SMI && (RegisterEdx & 0xffff) == SW_SMI_PORT) {
      mSmmVariableGlobal->CurrentlyExecutingCpu = Index;
      Status = EFI_SUCCESS;
      break;
    }
  }
  return Status;
}

/**
  This function uses to read saved CPU double word register

  @param[in] RegisterNum   Register number which want to get
  @param[in] RegisterData  pointer to output register data

  @retval EFI_SUCCESS      Read double word register successful
  @return Other            Any error occurred while disabling all secure boot SMI functions successful.
**/
EFI_STATUS
GetDwordRegister (
  IN EFI_SMM_SAVE_STATE_REGISTER       RegisterNum,
  OUT UINT32                            *RegisterData
  )
{
  return mSmmVariableGlobal->SmmCpu->ReadSaveState (
                                       mSmmVariableGlobal->SmmCpu,
                                       sizeof (UINT32),
                                       RegisterNum,
                                       mSmmVariableGlobal->CurrentlyExecutingCpu,
                                       RegisterData
                                       );

}

/**
  This function uses to write saved CPU double word register

  @param[in] RegisterNum   Register number which want to get
  @param[in] RegisterData  pointer to output register data

  @retval EFI_SUCCESS      Read double word register successful
  @return Other            Any error occurred while disabling all secure boot SMI functions successful.
**/
EFI_STATUS
SetDwordRegister (
  IN EFI_SMM_SAVE_STATE_REGISTER       RegisterNum,
  OUT UINT32                           RegisterData
  )
{
  return mSmmVariableGlobal->SmmCpu->WriteSaveState (
                                       mSmmVariableGlobal->SmmCpu,
                                       sizeof (UINT32),
                                       RegisterNum,
                                       mSmmVariableGlobal->CurrentlyExecutingCpu,
                                       &RegisterData
                                       );

}

/**
  Convert the standard Lib double linked list to a virtual mapping.

  This service uses gRT->ConvertPointer() to walk a double linked list and convert all the link
  pointers to their virtual mappings. This function is only guaranteed to work during the
  EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE event and calling it at other times has undefined results.

  @param[in]  ListHead           Head of linked list to convert.
**/
VOID
ConvertList (
  IN OUT LIST_ENTRY       *ListHead
  )
{
  LIST_ENTRY  *Link;
  LIST_ENTRY  *NextLink;

  if (ListHead == NULL) {
    return;
  }
  //
  // Convert all the ForwardLink & BackLink pointers in the list
  //
  Link = ListHead;
  do {
    NextLink = Link->ForwardLink;
    gRT->ConvertPointer (0, (VOID **) &Link->ForwardLink);
    gRT->ConvertPointer (0, (VOID **) &Link->BackLink);
    Link = NextLink;
  } while (Link != NULL && Link != ListHead);

}

/**
  Check the CRC32 for whole non-volatile variable cache is whether correct.

  @retval TRUE   The CRC32 value is correct
  @retval FALSE  The CRC32 value is incorrect
**/
BOOLEAN
CheckNonVolatileVariableCacheCrc32 (
  VOID
  )
{
  UINT32          Crc32;

  if (mVariableModuleGlobal->NonVolatileVariableCache == NULL) {
    return FALSE;
  }

  if (mSmst == NULL) {
    Crc32 = mVariableModuleGlobal->NonVolatileVariableCacheCrc32;
  } else {
    Crc32 = mSmmVariableGlobal->ProtectedModeVariableModuleGlobal->NonVolatileVariableCacheCrc32;
  }
  return  (BOOLEAN) (Crc32 == CalculateCrc32 (
                                mVariableModuleGlobal->NonVolatileVariableCache,
                                mVariableModuleGlobal->NonVolatileVariableCacheSize
                                ));
}

/**
  Calculate the CRC32 for whole non-volatile variable cache.

  @return The CRC32 for whole non-volatile variable cache.
**/
UINT32
CalculateNonVolatileVariableCacheCrc32 (
  VOID
  )
{
  if (mVariableModuleGlobal->NonVolatileVariableCache == NULL) {
    return 0;
  }

  return  CalculateCrc32 (
            mVariableModuleGlobal->NonVolatileVariableCache,
            mVariableModuleGlobal->NonVolatileVariableCacheSize
            );
}

/**
  Calculate the CRC32 for whole non-volatile variable cache and set to NonVolatileVariableCacheCrc32.
**/
VOID
SetNonVolatileVariableCacheCrc32 (
  VOID
  )
{
  UINT32        Crc32;

  Crc32 = CalculateNonVolatileVariableCacheCrc32 ();
  if (mSmst == NULL) {
    mVariableModuleGlobal->NonVolatileVariableCacheCrc32 = Crc32;
  } else {
    mSmmVariableGlobal->ProtectedModeVariableModuleGlobal->NonVolatileVariableCacheCrc32 = Crc32;
  }
}