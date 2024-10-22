/** @file
  Report Status Code Router SMM Driver.

;******************************************************************************
;* Copyright (c) 2012 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/** @file
  Report Status Code Router Driver which produces SMM Report Stataus Code Handler Protocol
  and SMM Status Code Protocol.

  Copyright (c) 2009 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "ReportStatusCodeRouterSmm.h"
#include <Guid/StatusCodeDataTypeDebug.h>
#include <Guid/StatusCodeDataTypeH2ODebug.h>

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>

LIST_ENTRY   mCallbackListHead          = INITIALIZE_LIST_HEAD_VARIABLE (mCallbackListHead);

//
// Report operation nest status.
// If it is set, then the report operation has nested.
//
UINT32       mStatusCodeNestStatus = 0;

EFI_SMM_STATUS_CODE_PROTOCOL  mSmmStatusCodeProtocol  = {
  ReportDispatcher
};

EFI_SMM_RSC_HANDLER_PROTOCOL  mSmmRscHandlerProtocol = {
  Register,
  Unregister
  };

/**
  Register the callback function for ReportStatusCode() notification.

  When this function is called the function pointer is added to an internal list and any future calls to
  ReportStatusCode() will be forwarded to the Callback function.

  @param[in] Callback           A pointer to a function of type EFI_PEI_RSC_HANDLER_CALLBACK that is called
                                when a call to ReportStatusCode() occurs.

  @retval EFI_SUCCESS           Function was successfully registered.
  @retval EFI_INVALID_PARAMETER The callback function was NULL.
  @retval EFI_OUT_OF_RESOURCES  The internal buffer ran out of space. No more functions can be
                                registered.
  @retval EFI_ALREADY_STARTED   The function was already registered. It can't be registered again.

**/
EFI_STATUS
EFIAPI
Register (
  IN EFI_SMM_RSC_HANDLER_CALLBACK   Callback
  )
{
  LIST_ENTRY                      *Link;
  SMM_RSC_HANDLER_CALLBACK_ENTRY  *CallbackEntry;

  if (Callback == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  for (Link = GetFirstNode (&mCallbackListHead); !IsNull (&mCallbackListHead, Link); Link = GetNextNode (&mCallbackListHead, Link)) {
    CallbackEntry = CR (Link, SMM_RSC_HANDLER_CALLBACK_ENTRY, Node, SMM_RSC_HANDLER_CALLBACK_ENTRY_SIGNATURE);
    if (CallbackEntry->RscHandlerCallback == Callback) {
      //
      // If the function was already registered. It can't be registered again.
      //
      return EFI_ALREADY_STARTED;
    }
  }

  CallbackEntry = (SMM_RSC_HANDLER_CALLBACK_ENTRY *)AllocatePool (sizeof (SMM_RSC_HANDLER_CALLBACK_ENTRY));
  if (CallbackEntry == NULL) {
    ASSERT (CallbackEntry != NULL);
    return EFI_OUT_OF_RESOURCES;
  }

  CallbackEntry->Signature          = SMM_RSC_HANDLER_CALLBACK_ENTRY_SIGNATURE;
  CallbackEntry->RscHandlerCallback = Callback;

  InsertTailList (&mCallbackListHead, &CallbackEntry->Node);

  return EFI_SUCCESS;
}

/**
  Remove a previously registered callback function from the notification list.

  ReportStatusCode() messages will no longer be forwarded to the Callback function.

  @param[in] Callback           A pointer to a function of type EFI_PEI_RSC_HANDLER_CALLBACK that is to be
                                unregistered.

  @retval EFI_SUCCESS           The function was successfully unregistered.
  @retval EFI_INVALID_PARAMETER The callback function was NULL.
  @retval EFI_NOT_FOUND         The callback function was not found to be unregistered.

**/
EFI_STATUS
EFIAPI
Unregister (
  IN EFI_SMM_RSC_HANDLER_CALLBACK Callback
  )
{
  LIST_ENTRY                        *Link;
  SMM_RSC_HANDLER_CALLBACK_ENTRY    *CallbackEntry;

  if (Callback == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  for (Link = GetFirstNode (&mCallbackListHead); !IsNull (&mCallbackListHead, Link); Link = GetNextNode (&mCallbackListHead, Link)) {
    CallbackEntry = CR (Link, SMM_RSC_HANDLER_CALLBACK_ENTRY, Node, SMM_RSC_HANDLER_CALLBACK_ENTRY_SIGNATURE);
    if (CallbackEntry->RscHandlerCallback == Callback) {
      //
      // If the function is found in list, delete it and return.
      //
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
// size. (to prevent from C6240 warning)
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

  @param  This             EFI_SMM_STATUS_CODE_PROTOCOL instance.
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
  IN CONST EFI_SMM_STATUS_CODE_PROTOCOL  *This,
  IN EFI_STATUS_CODE_TYPE                Type,
  IN EFI_STATUS_CODE_VALUE               Value,
  IN UINT32                              Instance,
  IN CONST EFI_GUID                      *CallerId  OPTIONAL,
  IN EFI_STATUS_CODE_DATA                *Data      OPTIONAL
  )
{
  LIST_ENTRY                        *Link;
  SMM_RSC_HANDLER_CALLBACK_ENTRY    *CallbackEntry;
  EFI_STATUS                    Status;
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
    CallbackEntry = CR (Link, SMM_RSC_HANDLER_CALLBACK_ENTRY, Node, SMM_RSC_HANDLER_CALLBACK_ENTRY_SIGNATURE);
    //
    // The handler may remove itself, so get the next handler in advance.
    //
    Link = GetNextNode (&mCallbackListHead, Link);
    CallbackEntry->RscHandlerCallback (
                     Type,
                     Value,
                     Instance,
                     (EFI_GUID*)CallerId,
                     Data
                     );

  }

  //
  // Restore the nest status of report
  //
  InterlockedCompareExchange32 (&mStatusCodeNestStatus, 1, 0);

  return EFI_SUCCESS;
}

/**
  Entry point of Generic Status Code Driver.

  This function is the entry point of SMM Status Code Router .
  It produces SMM Report Stataus Code Handler and Status Code protocol.

  @param  ImageHandle       The firmware allocated handle for the EFI image.
  @param  SystemTable       A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.

**/
EFI_STATUS
EFIAPI
GenericStatusCodeSmmEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS     Status;
  EFI_HANDLE     Handle;
  EFI_HANDLE     Handle2;

  Handle     = NULL;
  Handle2    = NULL;

  //
  // Install SmmRscHandler Protocol
  //
  Status = gSmst->SmmInstallProtocolInterface (
                    &Handle,
                    &gEfiSmmRscHandlerProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mSmmRscHandlerProtocol
                    );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->InstallProtocolInterface (
                  &Handle2,
                  &gEfiSmmStatusCodeProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mSmmStatusCodeProtocol
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Install SmmStatusCode Protocol
  //
  Status = gSmst->SmmInstallProtocolInterface (
                    &Handle,
                    &gEfiSmmStatusCodeProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mSmmStatusCodeProtocol
                    );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
