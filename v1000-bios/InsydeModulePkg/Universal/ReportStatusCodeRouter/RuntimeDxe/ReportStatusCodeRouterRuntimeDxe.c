/** @file
  Report Status Code Router Runtime Driver.

;******************************************************************************
;* Copyright (c) 2017 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
/** @file
  Report Status Code Router Driver which produces Report Stataus Code Handler Protocol
  and Status Code Runtime Protocol.

  Copyright (c) 2009 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "ReportStatusCodeRouterRuntimeDxe.h"
#include <Guid/StatusCodeDataTypeDebug.h>
#include <Guid/StatusCodeDataTypeH2ODebug.h>

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugPrintErrorLevelLib.h>

EFI_HANDLE   mHandle                    = NULL;
LIST_ENTRY   mCallbackListHead          = INITIALIZE_LIST_HEAD_VARIABLE (mCallbackListHead);
EFI_EVENT    mVirtualAddressChangeEvent = NULL;

//
// Report operation nest status.
// If it is set, then the report operation has nested.
//
UINT32       mStatusCodeNestStatus = 0;

EFI_STATUS_CODE_PROTOCOL  mStatusCodeProtocol  = {
  ReportDispatcher
};

EFI_RSC_HANDLER_PROTOCOL  mRscHandlerProtocol = {
  Register,
  Unregister
  };

/**
  Event callback function to invoke status code handler in list.

  @param  Event         Event whose notification function is being invoked.
  @param  Context       Pointer to the notification function's context, which is
                        always zero in current implementation.

**/
VOID
EFIAPI
RscHandlerNotification (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  RSC_HANDLER_CALLBACK_ENTRY  *CallbackEntry;
  EFI_PHYSICAL_ADDRESS        Address;
  RSC_DATA_ENTRY              *RscData;

  CallbackEntry = (RSC_HANDLER_CALLBACK_ENTRY *) Context;

  //
  // Traverse the status code data buffer to parse all
  // data to report.
  //
  Address = CallbackEntry->StatusCodeDataBuffer;
  while (Address < CallbackEntry->EndPointer) {
    RscData = (RSC_DATA_ENTRY *) (UINTN) Address;
    CallbackEntry->RscHandlerCallback (
                     RscData->Type,
                     RscData->Value,
                     RscData->Instance,
                     &RscData->CallerId,
                     &RscData->Data
                     );

    Address += (sizeof (RSC_DATA_ENTRY) + RscData->Data.Size);
    Address  = ALIGN_VARIABLE (Address);
  }

  CallbackEntry->EndPointer = CallbackEntry->StatusCodeDataBuffer;
}

/**
  Register the callback function for ReportStatusCode() notification.

  When this function is called the function pointer is added to an internal list and any future calls to
  ReportStatusCode() will be forwarded to the Callback function. During the bootservices,
  this is the callback for which this service can be invoked. The report status code router
  will create an event such that the callback function is only invoked at the TPL for which it was
  registered. The entity that registers for the callback should also register for an event upon
  generation of exit boot services and invoke the unregister service.
  If the handler does not have a TPL dependency, it should register for a callback at TPL high. The
  router infrastructure will support making callbacks at runtime, but the caller for runtime invocation
  must meet the following criteria:
  1. must be a runtime driver type so that its memory is not reclaimed
  2. not unregister at exit boot services so that the router will still have its callback address
  3. the caller must be self-contained (eg. Not call out into any boot-service interfaces) and be
  runtime safe, in general.

  @param[in] Callback   A pointer to a function of type EFI_RSC_HANDLER_CALLBACK that is called when
                        a call to ReportStatusCode() occurs.
  @param[in] Tpl        TPL at which callback can be safely invoked.

  @retval  EFI_SUCCESS              Function was successfully registered.
  @retval  EFI_INVALID_PARAMETER    The callback function was NULL.
  @retval  EFI_OUT_OF_RESOURCES     The internal buffer ran out of space. No more functions can be
                                    registered.
  @retval  EFI_ALREADY_STARTED      The function was already registered. It can't be registered again.

**/
EFI_STATUS
EFIAPI
Register (
  IN EFI_RSC_HANDLER_CALLBACK   Callback,
  IN EFI_TPL                    Tpl
  )
{
  EFI_STATUS                    Status;
  LIST_ENTRY                    *Link;
  RSC_HANDLER_CALLBACK_ENTRY    *CallbackEntry;

  if (Callback == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  for (Link = GetFirstNode (&mCallbackListHead); !IsNull (&mCallbackListHead, Link); Link = GetNextNode (&mCallbackListHead, Link)) {
    CallbackEntry = CR (Link, RSC_HANDLER_CALLBACK_ENTRY, Node, RSC_HANDLER_CALLBACK_ENTRY_SIGNATURE);
    if (CallbackEntry->RscHandlerCallback == Callback) {
      //
      // If the function was already registered. It can't be registered again.
      //
      return EFI_ALREADY_STARTED;
    }
  }

  CallbackEntry = AllocateRuntimeZeroPool (sizeof (RSC_HANDLER_CALLBACK_ENTRY));
  ASSERT (CallbackEntry != NULL);
  if (CallbackEntry == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CallbackEntry->Signature          = RSC_HANDLER_CALLBACK_ENTRY_SIGNATURE;
  CallbackEntry->RscHandlerCallback = Callback;
  CallbackEntry->Tpl                = Tpl;

  //
  // If TPL of registered callback funtion is not TPL_HIGH_LEVEL, then event should be created
  // for it, and related buffer for status code data should be prepared.
  // Here the data buffer must be prepared in advance, because Report Status Code Protocol might
  // be invoked under TPL_HIGH_LEVEL and no memory allocation is allowed then.
  // If TPL is TPL_HIGH_LEVEL, then all status code will be reported immediately, without data
  // buffer and event trigger.
  //
  if (Tpl != TPL_HIGH_LEVEL) {
    CallbackEntry->StatusCodeDataBuffer = (EFI_PHYSICAL_ADDRESS) (UINTN) AllocatePool (EFI_PAGE_SIZE);
    CallbackEntry->BufferSize           = EFI_PAGE_SIZE;
    CallbackEntry->EndPointer           = CallbackEntry->StatusCodeDataBuffer;
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    Tpl,
                    RscHandlerNotification,
                    CallbackEntry,
                    &CallbackEntry->Event
                    );
    ASSERT_EFI_ERROR (Status);
  }

  InsertTailList (&mCallbackListHead, &CallbackEntry->Node);

  return EFI_SUCCESS;
}

/**
  Remove a previously registered callback function from the notification list.

  A callback function must be unregistered before it is deallocated. It is important that any registered
  callbacks that are not runtime complaint be unregistered when ExitBootServices() is called.

  @param[in]  Callback  A pointer to a function of type EFI_RSC_HANDLER_CALLBACK that is to be
                        unregistered.

  @retval EFI_SUCCESS           The function was successfully unregistered.
  @retval EFI_INVALID_PARAMETER The callback function was NULL.
  @retval EFI_NOT_FOUND         The callback function was not found to be unregistered.

**/
EFI_STATUS
EFIAPI
Unregister (
  IN EFI_RSC_HANDLER_CALLBACK Callback
  )
{
  LIST_ENTRY                    *Link;
  RSC_HANDLER_CALLBACK_ENTRY    *CallbackEntry;

  if (Callback == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  for (Link = GetFirstNode (&mCallbackListHead); !IsNull (&mCallbackListHead, Link); Link = GetNextNode (&mCallbackListHead, Link)) {
    CallbackEntry = CR (Link, RSC_HANDLER_CALLBACK_ENTRY, Node, RSC_HANDLER_CALLBACK_ENTRY_SIGNATURE);
    if (CallbackEntry->RscHandlerCallback == Callback) {
      //
      // If the function is found in list, delete it and return.
      //
      if (CallbackEntry->Tpl != TPL_HIGH_LEVEL) {
        FreePool ((VOID *) (UINTN) CallbackEntry->StatusCodeDataBuffer);
        gBS->CloseEvent (CallbackEntry->Event);
      }
      RemoveEntryList (&CallbackEntry->Node);
      FreePool (CallbackEntry);
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

//
// In "(*Format == 'p') && (sizeof (VOID *) > 4)", we intend to use sizeof (VOID *) to
// determine current CPU architecture so introduce GetPointerSize function to get pointer
// size (to prevent from C6240 warning)
//
/**
  Get pointer size in bytes.

  @return   The size of pointer in bytes.
**/
STATIC
UINTN
GetPointerSize (
  VOID
  )
{
  return sizeof (VOID *);
}

/**
  Convert H2ODebugInfo to EfiDebugInfo.

  @param  Data                   Status code data contains a H2ODebugInfo.
  @param  Buffer                 Buffer to store EfiDebugInfo.
  @param  BufferSize             Buffer size.
  @param  NewStatusCodeData      Converted status code data contains a EfiDebugInfo

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_BUFFER_TOO_SMALL   Buffer is too small to contain format string.
  @retval EFI_INVALID_PARAMETER  Converted BASE_LIST is larger than the 12.

**/
EFI_STATUS
ConvertH2ODebugInfoToEfiDebugInfo (
  IN  CONST EFI_STATUS_CODE_DATA    *Data,
  IN  UINT64                        *Buffer,
  IN  UINTN                         BufferSize,
  OUT EFI_STATUS_CODE_DATA          **NewStatusCodeData
  )
{
  EFI_DEBUG_INFO         *DebugInfo;
  H2O_DEBUG_INFO         *H2ODebugInfo;
  UINTN                  TotalSize;
  VA_LIST                VaListMarker;
  BASE_LIST              BaseListMarker;
  BASE_LIST              BaseListMarkerPointer;
  CHAR8                  *FormatString;
  BOOLEAN                Long;
  CHAR8                  *Format;
  UINTN                  ErrorLevel;
  EFI_STATUS_CODE_DATA   *StatusCodeData;


  H2ODebugInfo = (H2O_DEBUG_INFO *)(Data + 1);

  ErrorLevel            = H2ODebugInfo->ErrorLevel;
  Format                = H2ODebugInfo->Format;
  VaListMarker          = H2ODebugInfo->VaListMarker;

  if (Data->Size < sizeof (H2O_DEBUG_INFO)) {
    BaseListMarker        = NULL;
  } else {
    BaseListMarker        = H2ODebugInfo->BaseListMarker;
  }

  //
  // Compute the total size of the record.
  // Note that the passing-in format string and variable parameters will be constructed to
  // the following layout:
  //
  //                Buffer->|------------------------|
  //                        |         Padding        | 4 bytes
  //             DebugInfo->|------------------------|
  //                        |      EFI_DEBUG_INFO    | sizeof(EFI_DEBUG_INFO)
  // BaseListMarkerPointer->|------------------------|
  //                        |           ...          |
  //                        |   variable arguments   | 12 * sizeof (UINT64)
  //                        |           ...          |
  //                        |------------------------|
  //                        |       Format String    |
  //                        |------------------------|<- (UINT8 *)Buffer + sizeof(Buffer)
  //
  TotalSize = sizeof (EFI_STATUS_CODE_DATA) + 4 + sizeof (EFI_DEBUG_INFO) + 12 * sizeof (UINT64) + AsciiStrSize (Format);

  //
  // If the TotalSize is larger than the maximum record size, then return
  //
  if (TotalSize > BufferSize) {
    TotalSize = BufferSize;
  }

  StatusCodeData = (EFI_STATUS_CODE_DATA  *) Buffer;
  StatusCodeData->HeaderSize = (UINT16) sizeof (EFI_STATUS_CODE_DATA);
  StatusCodeData->Size = (UINT16) TotalSize;
  CopyGuid (&StatusCodeData->Type, &gEfiStatusCodeDataTypeDebugGuid);

  //
  // Fill in EFI_DEBUG_INFO
  //
  // Here we skip the first 4 bytes of Buffer, because we must ensure BaseListMarkerPointer is
  // 64-bit aligned, otherwise retrieving 64-bit parameter from BaseListMarkerPointer will cause
  // exception on IPF. Buffer starts at 64-bit aligned address, so skipping 4 types (sizeof(EFI_DEBUG_INFO))
  // just makes address of BaseListMarkerPointer, which follows DebugInfo, 64-bit aligned.
  //
  DebugInfo             = (EFI_DEBUG_INFO *)(StatusCodeData + 1);
  DebugInfo->ErrorLevel = (UINT32)ErrorLevel;
  BaseListMarkerPointer = (BASE_LIST)(DebugInfo + 1);
  FormatString          = (CHAR8 *)((UINT64 *)(DebugInfo + 1) + 12);

  //
  // Copy the Format string into the record. It will be truncated if it's too long.
  //
  AsciiStrnCpyS (
    FormatString, BufferSize - (sizeof (EFI_STATUS_CODE_DATA) + 4 + sizeof(EFI_DEBUG_INFO) + 12 * sizeof(UINT64)),
    Format,       BufferSize - (sizeof (EFI_STATUS_CODE_DATA) + 4 + sizeof(EFI_DEBUG_INFO) + 12 * sizeof(UINT64)) - 1
    );

  //
  // The first 12 * sizeof (UINT64) bytes following EFI_DEBUG_INFO are for variable arguments
  // of format in DEBUG string, which is followed by the DEBUG format string.
  // Here we will process the variable arguments and pack them in this area.
  //

  //
  // Use the actual format string.
  //
  Format = FormatString;
  for (; *Format != '\0'; Format++) {
    //
    // Only format with prefix % is processed.
    //
    if (*Format != '%') {
      continue;
    }
    Long = FALSE;
    //
    // Parse Flags and Width
    //
    for (Format++; TRUE; Format++) {
      if (*Format == '.' || *Format == '-' || *Format == '+' || *Format == ' ') {
        //
        // These characters in format field are omitted.
        //
        continue;
      }
      if (*Format >= '0' && *Format <= '9') {
        //
        // These characters in format field are omitted.
        //
        continue;
      }
      if (*Format == 'L' || *Format == 'l') {
        //
        // 'L" or "l" in format field means the number being printed is a UINT64
        //
        Long = TRUE;
        continue;
      }
      if (*Format == '*') {
        //
        // '*' in format field means the precision of the field is specified by
        // a UINTN argument in the argument list.
        //
        if (BaseListMarker == NULL) {
          BASE_ARG (BaseListMarkerPointer, UINTN) = VA_ARG (VaListMarker, UINTN);
        } else {
          BASE_ARG (BaseListMarkerPointer, UINTN) = BASE_ARG (BaseListMarker, UINTN);
        }
        continue;
      }
      if (*Format == '\0') {
        //
        // Make no output if Format string terminates unexpectedly when
        // looking up for flag, width, precision and type.
        //
        Format--;
      }
      //
      // When valid argument type detected or format string terminates unexpectedly,
      // the inner loop is done.
      //
      break;
    }

    //
    // Pack variable arguments into the storage area following EFI_DEBUG_INFO.
    //
    if ((*Format == 'p') && GetPointerSize () > 4) {
      Long = TRUE;
    }
    if (*Format == 'p' || *Format == 'X' || *Format == 'x' || *Format == 'd' || *Format == 'u') {
      if (Long) {
        if (BaseListMarker == NULL) {
          BASE_ARG (BaseListMarkerPointer, INT64) = VA_ARG (VaListMarker, INT64);
        } else {
          BASE_ARG (BaseListMarkerPointer, INT64) = BASE_ARG (BaseListMarker, INT64);
        }
      } else {
        if (BaseListMarker == NULL) {
          BASE_ARG (BaseListMarkerPointer, int) = VA_ARG (VaListMarker, int);
        } else {
          BASE_ARG (BaseListMarkerPointer, int) = BASE_ARG (BaseListMarker, int);
        }
      }
    } else if (*Format == 's' || *Format == 'S' || *Format == 'a' || *Format == 'g' || *Format == 't') {
      if (BaseListMarker == NULL) {
        BASE_ARG (BaseListMarkerPointer, VOID *) = VA_ARG (VaListMarker, VOID *);
      } else {
        BASE_ARG (BaseListMarkerPointer, VOID *) = BASE_ARG (BaseListMarker, VOID *);
      }
    } else if (*Format == 'c') {
      if (BaseListMarker == NULL) {
        BASE_ARG (BaseListMarkerPointer, UINTN) = VA_ARG (VaListMarker, UINTN);
      } else {
        BASE_ARG (BaseListMarkerPointer, UINTN) = BASE_ARG (BaseListMarker, UINTN);
      }
    } else if (*Format == 'r') {
      if (BaseListMarker == NULL) {
        BASE_ARG (BaseListMarkerPointer, RETURN_STATUS) = VA_ARG (VaListMarker, RETURN_STATUS);
      } else {
        BASE_ARG (BaseListMarkerPointer, RETURN_STATUS) = BASE_ARG (BaseListMarker, RETURN_STATUS);
      }
    }

    //
    // If the converted BASE_LIST is larger than the 12 * sizeof (UINT64) allocated bytes, then ASSERT()
    // This indicates that the DEBUG() macro is passing in more argument than can be handled by
    // the EFI_DEBUG_INFO record
    //
    ASSERT ((CHAR8 *)BaseListMarkerPointer <= FormatString);

    //
    // If the converted BASE_LIST is larger than the 12 * sizeof (UINT64) allocated bytes, then return
    //
    if ((CHAR8 *)BaseListMarkerPointer > FormatString) {
      return EFI_INVALID_PARAMETER;
    }
  }

  *NewStatusCodeData = StatusCodeData;

  return EFI_SUCCESS;
}

/**
  Provides an interface that a software module can call to report a status code.

  @param  Type             Indicates the type of status code being reported.
  @param  Value            Describes the current status of a hardware or software entity.
                           This included information about the class and subclass that is used to
                           classify the entity as well as an operation.
  @param  Instance         The enumeration of a hardware or software entity within
                           the system. Valid instance numbers start with 1.
  @param  CallerId         This optional parameter may be used to identify the caller.
                           This parameter allows the status code driver to apply different rules to
                           different callers.
  @param  Data             This optional parameter may be used to pass additional data.

  @retval EFI_SUCCESS           The function completed successfully
  @retval EFI_DEVICE_ERROR      The function should not be completed due to a device error.

**/
EFI_STATUS
EFIAPI
ReportDispatcher (
  IN EFI_STATUS_CODE_TYPE     Type,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 *CallerId  OPTIONAL,
  IN EFI_STATUS_CODE_DATA     *Data      OPTIONAL
  )
{
  LIST_ENTRY                    *Link;
  RSC_HANDLER_CALLBACK_ENTRY    *CallbackEntry;
  RSC_DATA_ENTRY                *RscData;
  EFI_STATUS                    Status;
  VOID                          *NewBuffer;
  UINT64                        Buffer[((EFI_STATUS_CODE_DATA_MAX_SIZE + sizeof (EFI_STATUS_CODE_DATA) + 64) / sizeof (UINT64))];
  EFI_STATUS_CODE_DATA          *NewStatusCodeData;

  //
  // Use atom operation to avoid the reentant of report.
  // If current status is not zero, then the function is reentrancy.
  //
  if (InterlockedCompareExchange32 (&mStatusCodeNestStatus, 0, 1) == 1) {
    return EFI_DEVICE_ERROR;
  }

  if (Data != NULL && CompareGuid (&Data->Type, &gEfiStatusCodeDataTypeH2ODebugGuid)) {
    Status = ConvertH2ODebugInfoToEfiDebugInfo (Data, Buffer, sizeof (Buffer), &NewStatusCodeData);
    if (EFI_ERROR (Status)) {
      InterlockedCompareExchange32 (&mStatusCodeNestStatus, 1, 0);
      return Status;
    }
    Data = NewStatusCodeData;
  }

  for (Link = GetFirstNode (&mCallbackListHead); !IsNull (&mCallbackListHead, Link);) {
    CallbackEntry = CR (Link, RSC_HANDLER_CALLBACK_ENTRY, Node, RSC_HANDLER_CALLBACK_ENTRY_SIGNATURE);
    //
    // The handler may remove itself, so get the next handler in advance.
    //
    Link = GetNextNode (&mCallbackListHead, Link);
    if ((CallbackEntry->Tpl == TPL_HIGH_LEVEL) || EfiAtRuntime ()) {
      CallbackEntry->RscHandlerCallback (
                       Type,
                       Value,
                       Instance,
                       CallerId,
                       Data
                       );
      continue;
    }

    //
    // If callback is registered with TPL lower than TPL_HIGH_LEVEL, event must be signaled at boot time to possibly wait for
    // allowed TPL to report status code. Related data should also be stored in data buffer.
    //
    CallbackEntry->EndPointer  = ALIGN_VARIABLE (CallbackEntry->EndPointer);
    RscData = (RSC_DATA_ENTRY *) (UINTN) CallbackEntry->EndPointer;
    CallbackEntry->EndPointer += sizeof (RSC_DATA_ENTRY);
    if (Data != NULL) {
      CallbackEntry->EndPointer += Data->Size;
    }

    //
    // If data buffer is about to be used up (7/8 here), try to reallocate a buffer with double size, if not at TPL_HIGH_LEVEL.
    //
    if (CallbackEntry->EndPointer > (CallbackEntry->StatusCodeDataBuffer + (CallbackEntry->BufferSize / 8) * 7)) {
      if (EfiGetCurrentTpl () < TPL_HIGH_LEVEL) {
        NewBuffer = ReallocatePool (
                      CallbackEntry->BufferSize,
                      CallbackEntry->BufferSize * 2,
                      (VOID *) (UINTN) CallbackEntry->StatusCodeDataBuffer
                      );
        if (NewBuffer != NULL) {
          CallbackEntry->EndPointer = (EFI_PHYSICAL_ADDRESS) (UINTN) NewBuffer + (CallbackEntry->EndPointer - CallbackEntry->StatusCodeDataBuffer);
          CallbackEntry->StatusCodeDataBuffer = (EFI_PHYSICAL_ADDRESS) (UINTN) NewBuffer;
          CallbackEntry->BufferSize *= 2;
        }
      }
    }

    //
    // If data buffer is used up, do not report for this time.
    //
    if (CallbackEntry->EndPointer > (CallbackEntry->StatusCodeDataBuffer + CallbackEntry->BufferSize)) {
      continue;
    }

    RscData->Type      = Type;
    RscData->Value     = Value;
    RscData->Instance  = Instance;
    if (CallerId != NULL) {
      CopyGuid (&RscData->CallerId, CallerId);
    }
    if (Data != NULL) {
      CopyMem (&RscData->Data, Data, Data->HeaderSize + Data->Size);
    }

    Status = gBS->SignalEvent (CallbackEntry->Event);
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Restore the nest status of report
  //
  InterlockedCompareExchange32 (&mStatusCodeNestStatus, 1, 0);

  return EFI_SUCCESS;
}

/**
  Virtual address change notification call back. It converts global pointer
  to virtual address.

  @param  Event         Event whose notification function is being invoked.
  @param  Context       Pointer to the notification function's context, which is
                        always zero in current implementation.

**/
VOID
EFIAPI
VirtualAddressChangeCallBack (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS          Status;
  LIST_ENTRY                    *Link;
  RSC_HANDLER_CALLBACK_ENTRY    *CallbackEntry;

  for (Link = GetFirstNode (&mCallbackListHead); !IsNull (&mCallbackListHead, Link); Link = GetNextNode (&mCallbackListHead, Link)) {
    CallbackEntry = CR (Link, RSC_HANDLER_CALLBACK_ENTRY, Node, RSC_HANDLER_CALLBACK_ENTRY_SIGNATURE);
    Status = EfiConvertFunctionPointer (0, (VOID **) &CallbackEntry->RscHandlerCallback);
    ASSERT_EFI_ERROR (Status);
  }

  Status = EfiConvertList (
             0,
             &mCallbackListHead
             );
  ASSERT_EFI_ERROR (Status);
}

/**
  Entry point of Generic Status Code Driver.

  This function is the entry point of this Generic Status Code Driver.
  It installs eport Stataus Code Handler Protocol  and Status Code Runtime Protocol,
  and registers event for EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE.

  @param  ImageHandle       The firmware allocated handle for the EFI image.
  @param  SystemTable       A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.

**/
EFI_STATUS
EFIAPI
GenericStatusCodeRuntimeDxeEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS     Status;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mHandle,
                  &gEfiRscHandlerProtocolGuid,
                  &mRscHandlerProtocol,
                  &gEfiStatusCodeRuntimeProtocolGuid,
                  &mStatusCodeProtocol,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  VirtualAddressChangeCallBack,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &mVirtualAddressChangeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
