/** @file
  Definitions for helper functions and misc functions in variable driver

;******************************************************************************
;* Copyright (c) 2015 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#ifndef _VARIABLE_MISC_H_
#define _VARIABLE_MISC_H_


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
  );

/**
  Check the input memory buffer is whether overlap the SMRAM ranges.

  @param[in] Buffer       The pointer to the buffer to be checked.
  @param[in] BufferSize   The size in bytes of the input buffer

  @retval  TURE        The buffer overlaps SMRAM ranges.
  @retval  FALSE       The buffer doesn't overlap SMRAM ranges.
**/
BOOLEAN
EFIAPI
BufferOverlapSmram (
  IN VOID              *Buffer,
  IN UINTN              BufferSize
  );

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
  );


/**
  Get maximum user variable space which can be used.

  @return maximum user variable space by bytes.
**/
UINTN
GetMaxUserVariableSpace (
  VOID
  );

/**
  Get total user variable size by bytes.

  @return the size by bytes of user variable.
**/
UINTN
GetUserVariableTotalSize (
  VOID
  );

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
  );

/**
  Initialize variable quota.
**/
VOID
InitializeVariableQuota (
  VOID
  );

/**
  Calculate the sum of all elements in a buffer in unit of UINT8.
  During calculation, the carry bits are dropped.

  @param  Buffer   Pointer to the buffer to carry out the sum operation.
  @param  Length   The size, in bytes, of Buffer.

  @return Sum      The sum of Buffer with carry bits dropped during additions.
**/
UINT16
EFIAPI
InternalCalculateSum16 (
  IN      CONST UINT8         *Buffer,
  IN      UINTN               Length
  );

/**
  Initialize authenticated services.

  @param Event    Event whose notification function is being invoked.
  @param Context  Pointer to the notification function's context.
**/
VOID
EFIAPI
CryptoCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );

/**
  The notification of gEfiCryptoServiceProtocolGuid protocol is installed

  @param  Protocol              Points to the protocol's unique identifier.
  @param  Interface             Points to the interface instance.
  @param  Handle                The handle on which the interface was installed.
**/
EFI_STATUS
EFIAPI
CryptoServicesInstalled (
  IN     CONST EFI_GUID                *Protocol,
  IN     VOID                          *Interface,
  IN     EFI_HANDLE                    Handle
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );;

/**
  According to system mode to allocate pool.
  Allocate BootServicesData pool in protect mode if Runtime is FALSE.
  Allocate EfiRuntimeServicesData pool in protect mode if Runtime is TRUE.
  Allocate memory from SMM ram if system in SMM mode.

  @param[in] Size    The size of buffer to allocate
  @param[in] Runtime Runtime Data or not.

  @return Pointer to allocated memory buffer or NULL if allcoate failed.
**/
VOID *
VariableAllocateZeroBuffer (
  IN UINTN     Size,
  IN BOOLEAN   Runtime
  );

/**
  This fucntion uses to update current executing CPU to SMM_VARIABLE_INSTANCE

  @retval EFI_SUCCESS     Update current executing CPU successful.
  @retval EFI_NOT_FOUND   Cannot find current executing CPU.
**/
EFI_STATUS
UpdateCurrentExecutingCpu (
  VOID
  );

/**
  This fucnitons uses to read saved CPU double word register

  @param[in] RegisterNum   Register number which want to get
  @param[in] RegisterData  pointer to output register data

  @retval EFI_SUCCESS      Read double word register successful
  @return Other            Any error occured while disabling all secure boot SMI functions successful.
**/
EFI_STATUS
GetDwordRegister (
  IN EFI_SMM_SAVE_STATE_REGISTER       RegisterNum,
  OUT UINT32                            *RegisterData
  );

/**
  This fucnitons uses to write saved CPU double word register

  @param[in] RegisterNum   Register number which want to get
  @param[in] RegisterData  pointer to output register data

  @retval EFI_SUCCESS      Read double word register successful
  @return Other            Any error occured while disabling all secure boot SMI functions successful.
**/
EFI_STATUS
SetDwordRegister (
  IN EFI_SMM_SAVE_STATE_REGISTER       RegisterNum,
  IN UINT32                            RegisterData
  );

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
  );

/**
  Check the CRC32 for whole non-volatile variable cache is whether correct.

  @retval TRUE   The CRC32 value is correct
  @retval FALSE  The CRC32 value is incorrect
**/
BOOLEAN
CheckNonVolatileVariableCacheCrc32 (
  VOID
  );


/**
  Calcualte Calcualte the CRC32 for whole non-volatile variable cache.

  @return The CRC32 for whole non-volatile variable cache.
**/
UINT32
CalculateNonVolatileVariableCacheCrc32 (
  VOID
  );

/**
  Calcualte the CRC32 for whole non-volatile variable cache and set to NonVolatileVariableCacheCrc32.
**/
VOID
SetNonVolatileVariableCacheCrc32 (
  VOID
  );

#define EFI_FREE_POOL(Addr) \
{  mSmst ? mSmst->SmmFreePool((VOID*) (Addr)) : gBS->FreePool ((VOID *) (Addr)); \
   Addr = NULL;}

extern BOOLEAN               mReadyToBootEventSignaled;
#endif
