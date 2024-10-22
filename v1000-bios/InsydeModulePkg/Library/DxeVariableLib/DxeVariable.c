/** @file
   Implement variable library related functions.

;******************************************************************************
;* Copyright (c) 2013 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/VariableLib.h>
#include <Library/IrsiRegistrationLib.h>
#include <Library/VariableSupportLib.h>
#include <Library/PcdLib.h>
#include <Library/FlashRegionLib.h>

#include <Guid/EventGroup.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmVariable.h>
#include <Protocol/VariableLock.h>
#include <Protocol/NonVolatileVariable.h>
#include <SmiTable.h>
#include <Pi/PiFirmwareVolume.h>

UINT8
EFIAPI
SmmSensitiveVariableCall (
  IN     UINT8            *InPutBuff,       // rcx
  IN     UINTN            DataSize,         // rdx
  IN     UINT8            SubFunNum,        // r8
  IN     UINT16           SmiPort           // r9
  );


STATIC EFI_SMM_VARIABLE_PROTOCOL             *mSmmVariable;
STATIC EFI_SMM_SYSTEM_TABLE2                 *mSmst;
STATIC BOOLEAN                               mAtRuntime;
STATIC EFI_EVENT                             mExitBootServicesEvent;
STATIC EFI_RUNTIME_SERVICES                  *mRT;
STATIC UINTN                                 mVariableDefaultBase;
STATIC UINTN                                 mVariableDefaultSize;
STATIC VARIABLE_STORE_HEADER                 *mVariableStoreBase;
STATIC VOID                                  *mSmmVariableProtocolRegistration;
/**
  Notification function of gEfiEventExitBootServicesGuid.

  This is a notification function registered on gEfiEventExitBootServicesGuid event.

  @param  Event        Event whose notification function is being invoked.
  @param  Context      Pointer to the notification function's context.

**/
STATIC
VOID
EFIAPI
ExitBootServicesEvent (
  IN EFI_EVENT                            Event,
  IN VOID                                 *Context
  )
{
  mAtRuntime = TRUE;
  gBS->CloseEvent (Event);
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
STATIC
BOOLEAN
AtRuntime (
  VOID
  )
{
  return mAtRuntime;
}

/**
  According to system mode to allocate pool.
  Allocate BootServicesData pool in protect mode if AtRuntime () returns FALSE.
  Allocate memory from SMM ram if system in SMM mode.
  Return NULL pointer if system is in protected mode and AtRuntime () returns TRUE.

  @param[in] Size    The size of buffer to allocate

  @retval NULL       Buffer unsuccessfully allocated.
  @retval Other      Buffer successfully allocated.
**/
STATIC
VOID *
InternalAllocateZeroBuffer (
  IN UINTN     Size
  )
{
  VOID           *Buffer;
  EFI_STATUS     Status;

  if (mSmst == NULL) {
    if (AtRuntime ()) {
      return NULL;
    }
    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    Size,
                    (VOID **)&Buffer
                    );
  } else {
    Status = mSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      Size,
                      (VOID **)&Buffer
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
  Internal function to return the value of a variable in different mode (DXE/SMM).

  @param[in]     VariableName   Name of Variable to be found.
  @param[in]     VendorGuid     Variable vendor GUID.
  @param[out]    Attributes     Attribute value of the variable found.
  @param[in,out] DataSize       Size of Data found. If size is less than the data, this value contains
                                the required size.
  @param[out]    Data           Data pointer.


  @retval EFI_SUCCESS           Find the specified variable.
  @retval EFI_INVALID_PARAMETER Invalid parameter.
  @retval EFI_NOT_FOUND         Not found.
  @retval EFI_BUFFER_TO_SMALL   DataSize is too small for the result.

**/
STATIC
EFI_STATUS
InternalGetVariable (
  IN      CHAR16            *VariableName,
  IN      EFI_GUID          *VendorGuid,
  OUT     UINT32            *Attributes OPTIONAL,
  IN OUT  UINTN             *DataSize,
  OUT     VOID              *Data
  )
{
  if (mSmst != NULL && mSmmVariable == NULL) {
    return EFI_UNSUPPORTED;
  }

  if (mSmmVariable == NULL) {
    return mRT->GetVariable (
                  VariableName,
                  VendorGuid,
                  Attributes,
                  DataSize,
                  Data
                  );
  } else {
    return mSmmVariable->SmmGetVariable (
                           VariableName,
                           VendorGuid,
                           Attributes,
                           DataSize,
                           Data
                           );
  }
}

/**
  Internal function to set variable in different mode (DXE/SMM).

  @param[in] VariableName        Name of Variable.
  @param[in] VendorGuid          Variable vendor GUID.
  @param[in] Attributes          Attribute value of the variable.
  @param[in] DataSize            Size of variable data by bytes.
  @param[in] Data                Data pointer.

  @return EFI_SUCCESS            Set variable successfully.
  @retval EFI_INVALID_PARAMETER  Invalid parameter.
  @retval EFI_OUT_OF_RESOURCES   Resource not enough to set variable.
  @retval EFI_WRITE_PROTECTED    Variable is read-only.
  @retval EFI_SECURITY_VIOLATION The variable could not be written due to EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS
                                 or EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS being set, but the AuthInfo
                                 does NOT pass the validation check carried out by the firmware.
  @retval EFI_NOT_FOUND          The variable trying to be updated or deleted was not found.
**/
STATIC
EFI_STATUS
InternalSetVariable (
  IN CHAR16      *VariableName,
  IN EFI_GUID    *VendorGuid,
  IN UINT32      Attributes,
  IN UINTN       DataSize,
  IN VOID        *Data
  )
{
  if (mSmst != NULL && mSmmVariable == NULL) {
    return EFI_UNSUPPORTED;
  }

  if (mSmmVariable == NULL) {
    return mRT->SetVariable (
                  VariableName,
                  VendorGuid,
                  Attributes,
                  DataSize,
                  Data
                  );
  } else {
    return mSmmVariable->SmmSetVariable (
                           VariableName,
                           VendorGuid,
                           Attributes,
                           DataSize,
                           Data
                           );
  }
}

/**
  Internal function to enumerates the current variable name in different mode (DXE/SMM).

  @param[in, out] VariableNameSize Size of the variable name.
  @param[in, out] VariableName     Pointer to variable name.
  @param[in, out] VendorGuid       Variable Vendor Guid.

  @retval EFI_SUCCESS              Invalid parameter.
  @retval EFI_BUFFER_TOO_SMALL     Find the specified variable.
  @retval EFI_NOT_FOUND            Not found.
**/
STATIC
EFI_STATUS
InternalGetNextVariableName (
  IN OUT  UINTN       *VariableNameSize,
  IN OUT  CHAR16      *VariableName,
  IN OUT  EFI_GUID    *VendorGuid
  )
{
  if (mSmst != NULL && mSmmVariable == NULL) {
    return EFI_UNSUPPORTED;
  }

  if (mSmmVariable == NULL) {
    return mRT->GetNextVariableName (
                  VariableNameSize,
                  VariableName,
                  VendorGuid
                  );
  } else {
    return mSmmVariable->SmmGetNextVariableName (
                           VariableNameSize,
                           VariableName,
                           VendorGuid
                           );
  }
}

/**
  Detect whether the system is in SMM mode.

  @retval TRUE                  System is at SMM mode.
  @retval FALSE                 System is not at SMM mode.
**/
STATIC
BOOLEAN
IsInSmm (
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_SMM_BASE2_PROTOCOL         *SmmBase;
  BOOLEAN                        InSmm;

  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **) &SmmBase
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  InSmm = FALSE;
  SmmBase->InSmm (SmmBase, &InSmm);
  return InSmm;
}

/**
  The notification of gEfiSmmVariableProtocolGuid protocol is installed

  @param  Protocol              Points to the protocol's unique identifier.
  @param  Interface             Points to the interface instance.
  @param  Handle                The handle on which the interface was installed.

**/
EFI_STATUS
EFIAPI
SmmVariableProtocolInstalled (
  IN     CONST EFI_GUID                *Protocol,
  IN     VOID                          *Interface,
  IN     EFI_HANDLE                    Handle
  )
{
  return mSmst->SmmLocateProtocol (
                  &gEfiSmmVariableProtocolGuid,
                  NULL,
                  (VOID **) &mSmmVariable
                  );
}

STATIC
EFI_STATUS
InitializeSmstAndSmmVariable (
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_SMM_BASE2_PROTOCOL         *SmmBase;

  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **) &SmmBase
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = SmmBase->GetSmstLocation(
                      SmmBase,
                      &mSmst
                      );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = mSmst->SmmLocateProtocol (
                    &gEfiSmmVariableProtocolGuid,
                    NULL,
                    (VOID **)&mSmmVariable
                    );
  if (EFI_ERROR (Status)) {
    Status = mSmst->SmmRegisterProtocolNotify (
                      &gEfiSmmVariableProtocolGuid,
                      SmmVariableProtocolInstalled,
                      &mSmmVariableProtocolRegistration
                      );
  }
  return Status;
}


/**
  The constructor function to check current mode is protected mode or SMM mode.
  If system is in SMM mode, constructor will locate gEfiSmmVariableProtocolGuid
  instance for future usage.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The constructor always returns EFI_SUCCESS.
**/
EFI_STATUS
EFIAPI
DxeVariableLibConstructor (
  IN      EFI_HANDLE                ImageHandle,
  IN      EFI_SYSTEM_TABLE          *SystemTable
  )
{

  EFI_GUID                    VariableDefaultId  = FDM_VARIABLE_DEFAULT_ID_VARIABLE_DEFAULT;
  EFI_FIRMWARE_VOLUME_HEADER  *FvHeader;

  mVariableDefaultBase = (UINTN) FdmGetAddressById (&gH2OFlashMapRegionVarDefaultGuid, &VariableDefaultId, 1);
  mVariableDefaultSize = (UINTN) FdmGetSizeById (&gH2OFlashMapRegionVarDefaultGuid, &VariableDefaultId, 1);
  FvHeader   = (EFI_FIRMWARE_VOLUME_HEADER *) (UINTN) FdmGetNAtAddr (&gH2OFlashMapRegionVarGuid, 1);
  mVariableStoreBase = (VARIABLE_STORE_HEADER *) ((UINT8 *) FvHeader + FvHeader->HeaderLength);
  if (IsInSmm ()) {
    return InitializeSmstAndSmmVariable ();
  } else {
    mRT = gRT;
    IrsiAddVirtualPointer ((VOID **) &mRT);
    IrsiAddVirtualPointer ((VOID **) &mVariableDefaultBase);
    IrsiAddVirtualPointer ((VOID **) &mVariableStoreBase);
    return gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    ExitBootServicesEvent,
                    NULL,
                    &gEfiEventExitBootServicesGuid,
                    &mExitBootServicesEvent
                    );
  }


}

/**
  If a runtime driver exits with an error, it must call this routine
  to free the allocated resource before the exiting.
  It will ASSERT() if gBS is NULL.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval  EFI_SUCCESS      The Runtime Driver Lib shutdown successfully.
**/
EFI_STATUS
EFIAPI
DxeVariableLibDestructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  ASSERT (gBS != NULL);
  if (mExitBootServicesEvent != NULL) {
    Status = gBS->CloseEvent (mExitBootServicesEvent);
    ASSERT_EFI_ERROR (Status);
  }
  if (mRT != NULL) {
    IrsiRemoveVirtualPointer ((VOID **) &mRT);
    IrsiRemoveVirtualPointer ((VOID **) &mVariableDefaultBase);
  }
  if (mSmst != NULL && mSmmVariableProtocolRegistration != NULL) {
    mSmst->SmmRegisterProtocolNotify (
             &gEfiSmmVariableProtocolGuid,
             NULL,
             &mSmmVariableProtocolRegistration
             );
  }

  return EFI_SUCCESS;
}

/**
  Read the EFI variable (VariableName/VendorGuid) according to input variable data size.

  @param[in]      VariableName       String part of EFI variable name.
  @param[in]      VendorGuid         GUID part of EFI variable name.
  @param[in, out] VariableDataSize   Returns the size of the EFI variable that was read.
  @param[out]     VariableData       Dynamically allocated memory that contains a copy of the EFI variable.
                                     Caller is responsible freeing the buffer.

  @retval EFI_SUCCESS                Get EFI variable Successful.
  @retval EFI_INVALID_PARAMETER      VariableName is NULL or VendorGuid is NULL or VariableDataSize is NULL or VariableData
                                     is NULL.
  @retval EFI_NOT_FOUND              Cannot find specific EFI variable.
  @retval EFI_BUFFER_TOO_SMALL       Input variable data size is too small.  VariableDataSize has been updated with the size
                                     needed to complete the request.
  @return Other                      Other errors cause get variable failed.
**/
EFI_STATUS
EFIAPI
CommonGetVariable (
  IN     CHAR16        *VariableName,
  IN     EFI_GUID      *VendorGuid,
  IN OUT UINTN         *VariableDataSize,
  OUT    VOID          *VariableData
  )
{
  return InternalGetVariable (
           VariableName,
           VendorGuid,
           NULL,
           VariableDataSize,
           VariableData
           );
}

/**
  Read the EFI variable (VariableName/VendorGuid) and return a dynamically allocated buffer.

  @param[in]  VariableName  String part of EFI variable name.
  @param[in]  VendorGuid    GUID part of EFI variable name.

  @retval NULL              The variable could not be retrieved.
  @retval NULL              There are not enough resources available for the variable contents.
  @retval Other             A pointer to allocated buffer containing the variable contents.
**/
VOID *
EFIAPI
CommonGetVariableData (
  IN CHAR16    *VariableName,
  IN EFI_GUID  *VendorGuid
  )
{
  UINTN       DataSize;
  VOID        *Data;

  Data = NULL;
  CommonGetVariableDataAndSize (VariableName, VendorGuid, &DataSize, &Data);
  return Data;
}

/**
  Read the EFI variable (VariableName/VendorGuid) and return a dynamically allocated
  buffer, and the size of the buffer.

  @param[in]  VariableName       String part of EFI variable name.
  @param[in]  VendorGuid         GUID part of EFI variable name.
  @param[out] VariableDataSize   Returns the size of the EFI variable that was read.
  @param[out] VariableData       Dynamically allocated memory that contains a copy of the EFI variable.
                                 Caller is responsible freeing the buffer.

  @retval EFI_SUCCESS            Get EFI variable Successful.
  @retval EFI_INVALID_PARAMETER  VariableName is NULL or VendorGuid is NULL or VariableDataSize is NULL or VariableData
                                 is NULL.
  @retval EFI_NOT_FOUND          Cannot find specific EFI variable.
  @retval EFI_OUT_OF_RESOURCES   Allocate memory for read variable data failed.
  @return Other                  Other errors cause get variable failed.
**/
EFI_STATUS
EFIAPI
CommonGetVariableDataAndSize (
  IN  CHAR16              *VariableName,
  IN  EFI_GUID            *VendorGuid,
  OUT UINTN               *VariableDataSize,
  OUT VOID                **VariableData
  )
{
  EFI_STATUS    Status;
  UINTN         DataSize;
  VOID          *DataPtr;

  if (VariableDataSize == NULL || VariableData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (mSmst == NULL && AtRuntime ()) {
    return EFI_OUT_OF_RESOURCES;
  }

  DataSize = 0;
  Status = InternalGetVariable (
             VariableName,
             VendorGuid,
             NULL,
             &DataSize,
             NULL
             );
  if (Status != EFI_BUFFER_TOO_SMALL) {
    return Status;
  }

  DataPtr = InternalAllocateZeroBuffer (DataSize);
  if (DataPtr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = InternalGetVariable (
            VariableName,
            VendorGuid,
            NULL,
            &DataSize,
            DataPtr
            );
  if (EFI_ERROR (Status)) {
    FreePool (DataPtr);
    return Status;
  }

  *VariableDataSize = DataSize;
  *VariableData     = DataPtr;
  return EFI_SUCCESS;
}

/**
  Read the EFI variable (VariableName/VendorGuid) and return a dynamically allocated
  buffer, and the size of the buffer.

  @param[in]  VariableName             String part of EFI variable name.
  @param[in]  VendorGuid               GUID part of EFI variable name.
  @param[out] VariableDataSize         Returns the size of the EFI variable that was read.
  @param[out] VariableData             Dynamically allocated memory that contains a copy of the EFI variable.
                                       Caller is responsible freeing the buffer.
  @param[in]  DefaultVariableDataSize  Default variable data size.
  @param[in]  DefaultVariableData      Pointer to default variable data.

  @retval TRUE                         The variable exists and the VariableDataSize and VariableData is filled by variable
                                       data size and variable data respectively.
  @retval FALSE                        The variable doesn't exit and the VariableDataSize and VariableData is filled by
                                       default variable data size and default variable data respectively.
**/
BOOLEAN
EFIAPI
CommonGetVariableWithDefault (
  IN  CHAR16              *VariableName,
  IN  EFI_GUID            *VendorGuid,
  OUT UINTN               *VariableDataSize,
  OUT VOID                **VariableData,
  IN  UINTN               DefaultVariableDataSize,
  IN  VOID                *DefaultVariableData
  )
{
  EFI_STATUS          Status;

  Status = CommonGetVariableDataAndSize (VariableName, VendorGuid, VariableDataSize, VariableData);
  if (!EFI_ERROR (Status)) {
    return TRUE;
  }

  ASSERT (DefaultVariableDataSize != 0 && DefaultVariableData != NULL);
  *VariableData = InternalAllocateZeroBuffer (DefaultVariableDataSize);
  if (*VariableData == NULL) {
    return FALSE;
  }

  CopyMem (*VariableData, DefaultVariableData, DefaultVariableDataSize);
  *VariableDataSize = DefaultVariableDataSize;
  return FALSE;
}
/**
  Enumerates the current variable names.

  @param[in,out]  VariableNameSize    The size of the VariableName buffer.
  @param[in,out]  VariableName        On input, supplies the last VariableName that was returned by GetNextVariableName().
                                      On output, returns the Null-terminated string of the current variable.
  @param[in,out]  VendorGuid          On input, supplies the last VendorGuid that was returned by GetNextVariableName().
                                      On output, returns the VendorGuid of the current variable.

  @retval EFI_SUCCESS                 The function completed successfully.
  @retval EFI_NOT_FOUND               The next variable was not found.
  @retval EFI_BUFFER_TOO_SMALL        The VariableNameSize is too small for the result. VariableNameSize has been updated
                                      with the size needed to complete the request.
  @retval EFI_INVALID_PARAMETER       VariableNameSize is NULL or VariableName is NULL or VendorGuid is NULL.
  @retval EFI_DEVICE_ERROR            The variable name could not be retrieved due to a hardware error.
**/
EFI_STATUS
EFIAPI
CommonGetNextVariableName (
  IN OUT UINTN       *VariableNameSize,
  IN OUT CHAR16      *VariableName,
  IN OUT EFI_GUID    *VendorGuid
  )
{
  return InternalGetNextVariableName (
           VariableNameSize,
           VariableName,
           VendorGuid
           );
}

/**
  Sets the value of a variable.

  Since variable PPI only provide read-only services, this function always returns
  EFI_UNSUPPORTED in PEI phase.

  @param[in] VariableName         String part of EFI variable name.
  @param[in] VendorGuid           GUID part of EFI variable name.
  @param[in] Attributes           Attributes bitmask to set for the variable
  @param[in] DataSize             The size in bytes of the Data buffer.
  @param[in] Data                 The contents for the variable.

  @retval EFI_SUCCESS                 The firmware has successfully stored the variable and its data as defined by
                                      the Attributes.
  @retval EFI_INVALID_PARAMETER       An invalid combination of attribute bits, name, and GUID  was supplied, or the
                                      DataSize exceeds the maximum allowed or name is a empty string.
  @retval EFI_OUT_OF_RESOURCES        Not enough storage is available to hold the variable and its data.
  @retval EFI_DEVICE_ERROR            The variable name could not be retrieved due to a hardware error.
  @retval EFI_WRITE_PROTECTED         The variable in question is read-only or the variable cannot be deleted.
  @retval EFI_SECURITY_VIOLATION      The variable could not be written due to EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS or
                                      EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS being set, but the AuthInfo does
                                      NOT pass the validation check carried out by the firmware.
**/
EFI_STATUS
EFIAPI
CommonSetVariable (
  IN CHAR16      *VariableName,
  IN EFI_GUID    *VendorGuid,
  IN UINT32      Attributes,
  IN UINTN       DataSize,
  IN VOID        *Data
  )
{
  return InternalSetVariable (
           VariableName,
           VendorGuid,
           Attributes,
           DataSize,
           Data
           );
}

/**
  Set SENSITIVE_SETUP_VARIABLE_NAME variable to variable store through SMI.

  @param[out] DataSize           The data size by byte of found variable
  @param[out] Data               Pointer to the start address of found data.

  @retval EFI_SUCCESS            Update SENSITIVE_SETUP_VARIABLE_NAME variable successfully.
  @retval EFI_INVALID_PARAMETER  Any input parameter is incorrect.
  @retval Other                  Any other error occurred while updating SENSITIVE_SETUP_VARIABLE_NAME variable.
**/
EFI_STATUS
UpdateSetupSensitiveVariableThroughSmi (
  IN   UINTN              DataSize,
  IN   VOID               *Data
  )
{
  SENSITIVE_VARIABLE_AUTHENTICATION  *WriteData;
  UINTN                              WriteDataSize;
  EFI_STATUS                         Status;
  EFI_TPL                            Tpl;
  NON_VOLATILE_VARIABLE_PROTOCOL     *NonVolatileVariableProtocol;
  VOID                               *VariableBuffer;
  UINTN                              VariableBufferSize;

  Status = gBS->LocateProtocol (
               &gEfiNonVolatileVariableProtocolGuid,
               NULL,
               (VOID **) &NonVolatileVariableProtocol
               );
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = NonVolatileVariableProtocol->GetRuntimeVariableBuffer (&VariableBuffer, &VariableBufferSize);
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }
  WriteDataSize = sizeof (SENSITIVE_VARIABLE_AUTHENTICATION) + DataSize;
  if (WriteDataSize > VariableBufferSize) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Because the VariableBuffer will be used in many places, raise TPL to prevent from the data in this
  // buffer is destroyed in other events.
  //
  Tpl = gBS->RaiseTPL (TPL_NOTIFY);

  WriteData = (SENSITIVE_VARIABLE_AUTHENTICATION *) VariableBuffer;
  WriteData->Signature = SET_SENSITIVE_VARIABLE_SIGNATURE;
  WriteData->DataSize  = (UINT32) DataSize;
  WriteData->Status    = EFI_UNSUPPORTED;
  CopyMem (WriteData + 1, Data, DataSize);

  Status = SmmSensitiveVariableCall ((UINT8 *) WriteData, (UINTN) WriteData->DataSize, SET_SENSITIVE_VARIABLE_FUN_NUM, SW_SMI_PORT);
  Status = WriteData->Status;

  gBS->RestoreTPL (Tpl);
  return Status;
}

/**
  Set changed sensitive setup data to sensitive setup variable.

  @param[in]  VariableName       A pointer to a null-terminated string that is the variable's name.
  @param[in]  VendorGuid         A pointer to an EFI_GUID that is the variable's GUID. The combination of
                                 VendorGuid and VariableName must be unique.
  @param[in]  Attributes         Attributes bitmask to set for the variable.
  @param[out] DataSize           The data size by byte of found variable
  @param[out] Data               Pointer to the start address of found data.

  @retval EFI_SUCCESS            Find variable from sensitive variable successfully.
  @retval EFI_INVALID_PARAMETER  Any input parameter is incorrect.
  @retval EFI_UNSUPPORTED        The data format to set sensitive variable is incorrect.
  @retval EFI_WRITE_PROTECTED    The service to set variable through SMI is disabled.
**/
EFI_STATUS
EFIAPI
SetVariableToSensitiveVariable (
  IN   CONST   CHAR16                           *VariableName,
  IN   CONST   EFI_GUID                         *VendorGuid,
  IN           UINT32                           Attributes,
  IN           UINTN                            DataSize,
  IN           VOID                             *Data
  )
{

  SENSITIVE_VARIABLE_STORE_HEADER *CurrentStoreHeader;
  SENSITIVE_VARIABLE_HEADER       *CurrentVariable;
  EFI_STATUS                      Status;
  UINTN                           BufferSize;
  EFI_SMM_VARIABLE_PROTOCOL       *SmmVariable;
  EDKII_VARIABLE_LOCK_PROTOCOL    *VariableLockProtocol;

  if (AtRuntime ()) {
    return EFI_UNSUPPORTED;
  }

  SmmVariable = NULL;
  if (mSmst == NULL) {
    Status = gBS->LocateProtocol (
                 &gEfiSmmVariableProtocolGuid,
                 NULL,
                 (VOID **) &SmmVariable
                 );
    //
    // Set variable directly if system doesn't provide variable lock protocol to lock variable.
    //
    Status = gBS->LocateProtocol (
                    &gEdkiiVariableLockProtocolGuid,
                    NULL,
                    (VOID **) &VariableLockProtocol
                    );
    if (EFI_ERROR (Status)) {
      return CommonSetVariable (
               (CHAR16 *) VariableName,
               (EFI_GUID *) VendorGuid,
               Attributes,
               DataSize,
               Data
               );
    }
  }
  //
  // If mSmst isn't NULL, set variable through SmmVariable instance directly.
  // If mSmst is NULL and SmmVariable is NULL, set variable through gRT variable services directly.
  //
  if (mSmst != NULL || SmmVariable == NULL) {
    return CommonSetVariable (
             (CHAR16 *) VariableName,
             (EFI_GUID *) VendorGuid,
             Attributes,
             DataSize,
             Data
             );
  }

  if (VariableName == NULL || VendorGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Attributes & EFI_VARIABLE_APPEND_WRITE) != 0) {
    return EFI_INVALID_PARAMETER;
  }
  if (DataSize != 0 && Data == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  BufferSize = sizeof (SENSITIVE_VARIABLE_STORE_HEADER) + sizeof (SENSITIVE_VARIABLE_HEADER) + StrSize (VariableName) + DataSize;

  CurrentStoreHeader = AllocatePool (BufferSize);
  if (CurrentStoreHeader == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CurrentStoreHeader->VariableCount = 1;
  CurrentStoreHeader->HeaderSize = sizeof (SENSITIVE_VARIABLE_STORE_HEADER);
  CurrentVariable = (SENSITIVE_VARIABLE_HEADER *) (CurrentStoreHeader + 1);

  CurrentVariable->Attributes = Attributes;
  CurrentVariable->DataSize   = (UINT32) DataSize;
  CopyGuid (&CurrentVariable->VendorGuid, VendorGuid);
  CopyMem (CurrentVariable + 1, VariableName, StrSize (VariableName));
  CopyMem (GetSensitiveVariableDataPtr (CurrentVariable), Data, DataSize);
  Status = UpdateSetupSensitiveVariableThroughSmi (BufferSize, (UINT8 *) CurrentStoreHeader);

  FreePool (CurrentStoreHeader);

  return Status;
}

/**
  Internal function to get the start address of variable default header according to
  input SKU ID.

  @param[in]  SkuId   Input SKU ID.

  @return  The address of variable store header or NULL if not found.
**/
STATIC
VOID *
GetVariableDefaultStoreHeaderBySkuId (
  IN  UINT8       SkuId
  )
{
  VARIABLE_STORE_HEADER           *VariableStoreHeader;
  ECP_VARIABLE_STORE_HEADER       *EcpVariableStoreHeader;

  if (PcdGetBool (PcdUseEcpVariableStoreHeader)) {
    for (EcpVariableStoreHeader = (ECP_VARIABLE_STORE_HEADER *) mVariableDefaultBase;
         EcpVariableStoreHeader != NULL &&
         EcpVariableStoreHeader->Signature == ECP_VARIABLE_STORE_SIGNATURE &&
         EcpVariableStoreHeader->Format == VARIABLE_STORE_FORMATTED &&
         EcpVariableStoreHeader->State == VARIABLE_STORE_HEALTHY &&
         (EcpVariableStoreHeader->Flags & VARIABLE_STORE_ACTIVE_MASK) == VARIABLE_STORE_ACTIVE &&
         EcpVariableStoreHeader->Size >= sizeof (ECP_VARIABLE_STORE_HEADER) &&
         (UINTN) EcpVariableStoreHeader < mVariableDefaultBase + mVariableDefaultSize;
         EcpVariableStoreHeader = (ECP_VARIABLE_STORE_HEADER *) ((UINT8 *) EcpVariableStoreHeader + EcpVariableStoreHeader->Size)) {
      if (EcpVariableStoreHeader->DefaultId == 0 && SkuId == EcpVariableStoreHeader->BoardId) {
        return (VOID *) EcpVariableStoreHeader;
      }
    }
  } else {
    for (VariableStoreHeader = (VARIABLE_STORE_HEADER *) mVariableDefaultBase;
         VariableStoreHeader != NULL &&
         (CompareGuid (&VariableStoreHeader->Signature, &gEfiAuthenticatedVariableGuid) ||
          CompareGuid (&VariableStoreHeader->Signature, &gEfiVariableGuid)) &&
         VariableStoreHeader->Format == VARIABLE_STORE_FORMATTED &&
         VariableStoreHeader->State == VARIABLE_STORE_HEALTHY &&
         (VariableStoreHeader->Flags & VARIABLE_STORE_ACTIVE_MASK) == VARIABLE_STORE_ACTIVE &&
         VariableStoreHeader->Size >= sizeof (VARIABLE_STORE_HEADER) &&
         (UINTN) VariableStoreHeader < mVariableDefaultBase + mVariableDefaultSize;
         VariableStoreHeader = (VARIABLE_STORE_HEADER *) ((UINT8 *) VariableStoreHeader + VariableStoreHeader->Size)) {
      if (VariableStoreHeader->DefaultId == 0 && SkuId == VariableStoreHeader->BoardId) {
        return (VOID *) VariableStoreHeader;
      }
    }
  }
  return NULL;
}

/**
  Read the EFI variable (VariableName/VendorGuid) according to input variable data size.

  @param[in]      VariableName       String part of EFI variable name.
  @param[in]      VendorGuid         GUID part of EFI variable name.
  @param[in]      SkuId              Input SKU ID.
  @param[in]      Attributes         Attribute value of the variable found.
  @param[in, out] VariableDataSize   Returns the size of the EFI variable that was read.
  @param[out]     VariableData       Dynamically allocated memory that contains a copy of the EFI variable.
                                     Caller is responsible freeing the buffer.

  @retval EFI_SUCCESS                Get EFI variable Successful.
  @retval EFI_INVALID_PARAMETER      VariableName is NULL or VendorGuid is NULL or VariableDataSize is NULL or VariableData
                                     is NULL.
  @retval EFI_NOT_FOUND              Cannot find specific EFI variable.
  @retval EFI_BUFFER_TOO_SMALL       Input variable data size is too small.  VariableDataSize has been updated with the size
                                     needed to complete the request.
**/
EFI_STATUS
EFIAPI
CommonGetDefaultVariable (
  IN     CHAR16        *VariableName,
  IN     EFI_GUID      *VendorGuid,
  IN     UINT8         SkuId,
  IN     UINT32        *Attributes, OPTIONAL
  IN OUT UINTN         *VariableDataSize,
  OUT    VOID          *VariableData
  )
{
  VOID                            *VariableStoreHeader;
  VARIABLE_HEADER                 *Variable;
  VARIABLE_HEADER                 *EndVariable;

  if (VariableName == NULL || VendorGuid == NULL || VariableDataSize == NULL || VariableName[0] == 0) {
    return EFI_INVALID_PARAMETER;
  }

  VariableStoreHeader = GetVariableDefaultStoreHeaderBySkuId (SkuId);
  if (VariableStoreHeader == NULL) {
    return EFI_NOT_FOUND;
  }

  Variable    = (VARIABLE_HEADER *) ((UINT8 *) VariableStoreHeader + GetVariableStoreHeaderSize ());
  EndVariable = (VARIABLE_HEADER *) ((UINT8 *) VariableStoreHeader + GetVariableStoreSize (VariableStoreHeader));
  while ((GetNextVariablePtr (Variable) <= EndVariable) && IsValidVariableHeader (Variable)) {
    if ((CompareGuid (VendorGuid, &Variable->VendorGuid) &&
        !StrCmp (VariableName, GET_VARIABLE_NAME_PTR (Variable)))) {
      if (Variable->State == VAR_ADDED) {
        if (*VariableDataSize >= Variable->DataSize) {
          if (VariableData == NULL) {
            return EFI_INVALID_PARAMETER;
          }
          CopyMem (VariableData, GetVariableDataPtr (Variable), Variable->DataSize);
          if (Attributes != NULL) {
            *Attributes = (Variable->Attributes & ~(EFI_VARIABLE_DEFAULT_READY_ONLY | EFI_VARIABLE_INSYDE_AUTHENTICATED_WRITE_ACCESS));
          }
          *VariableDataSize = Variable->DataSize;
          return EFI_SUCCESS;
        } else {
          *VariableDataSize = Variable->DataSize;
          return EFI_BUFFER_TOO_SMALL;
        }
      }
    }
    Variable = GetNextVariablePtr (Variable);
  }

  return EFI_NOT_FOUND;
}


/**
  Read the EFI variable (VariableName/VendorGuid) and return a dynamically allocated
  buffer, and the size of the buffer.

  @param[in]  VariableName       String part of EFI variable name.
  @param[in]  VendorGuid         GUID part of EFI variable name.
  @param[in]  SkuId              Input SKU ID.
  @param[in]  Attributes         Attribute value of the variable found.
  @param[out] VariableDataSize   Returns the size of the EFI variable that was read.
  @param[out] VariableData       Dynamically allocated memory that contains a copy of the EFI variable.
                                 Caller is responsible freeing the buffer.

  @retval EFI_SUCCESS            Get EFI variable Successful.
  @retval EFI_INVALID_PARAMETER  VariableName is NULL or VendorGuid is NULL or VariableDataSize is NULL or VariableData
                                 is NULL.
  @retval EFI_NOT_FOUND          Cannot find specific EFI variable.
  @retval EFI_OUT_OF_RESOURCES   Allocate memory for read variable data failed.
  @return Other                  Other errors cause get variable failed.
**/
EFI_STATUS
EFIAPI
CommonGetDefaultVariableDataAndSize (
  IN  CHAR16              *VariableName,
  IN  EFI_GUID            *VendorGuid,
  IN  UINT8               SkuId,
  IN  UINT32              *Attribute, OPTIONAL
  OUT UINTN               *VariableDataSize,
  OUT VOID                **VariableData
  )
{
  EFI_STATUS                          Status;
  UINTN                               DataSize;
  VOID                                *DataPtr;

  if (VariableDataSize == NULL || VariableData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (mSmst == NULL && AtRuntime ()) {
    return EFI_OUT_OF_RESOURCES;
  }

  DataSize = 0;
  Status = CommonGetDefaultVariable (
             VariableName,
             VendorGuid,
             SkuId,
             Attribute,
             &DataSize,
             NULL
             );
  if (Status != EFI_BUFFER_TOO_SMALL) {
    return Status;
  }

  DataPtr = InternalAllocateZeroBuffer (DataSize);
  if (DataPtr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = CommonGetDefaultVariable (
             VariableName,
             VendorGuid,
             SkuId,
             Attribute,
             &DataSize,
             DataPtr
             );
  if (EFI_ERROR (Status)) {
    FreePool (DataPtr);
    return Status;
  }

  *VariableDataSize = DataSize;
  *VariableData     = DataPtr;
  return EFI_SUCCESS;
}

/**
  Check the input variable is stored in variable store region.

  @param[in]  VariableName       String part of EFI variable name.
  @param[in]  VendorGuid         GUID part of EFI variable name.

  @retval TRUE                   Variable is stored in variable store region.
  @retval FALSE                  Variable isn't stored in variable store region or doesn't exist.
**/
BOOLEAN
EFIAPI
IsVariableInVariableStoreRegion (
  IN  CHAR16              *VariableName,
  IN  EFI_GUID            *VendorGuid
  )
{
  VARIABLE_HEADER             *VariableHeader;
  VARIABLE_HEADER             *VariableEnd;

  VariableHeader = (VARIABLE_HEADER *) ((UINT8 *) mVariableStoreBase + GetVariableStoreHeaderSize ());
  VariableEnd    =  GetEndPointer (mVariableStoreBase);
  while (IsValidVariableHeader (VariableHeader) && VariableHeader < VariableEnd) {
    if ((VariableHeader->State == VAR_ADDED || VariableHeader->State == (VAR_ADDED & VAR_IN_DELETED_TRANSITION)) &&
        StrCmp (VariableName, GET_VARIABLE_NAME_PTR (VariableHeader)) == 0 && CompareGuid (VendorGuid, &VariableHeader->VendorGuid)) {
      return TRUE;
    }
    VariableHeader = GetNextVariablePtr (VariableHeader);
  }
  return FALSE;

}