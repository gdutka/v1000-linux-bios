/*****************************************************************************
  *
  * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
  *
  *******************************************************************************
  *
 */

#include <Filecode.h>
#include <PiPei.h>
#include <Library/AmdBaseLib.h>
#include <Library/AmdErrorLogLib.h>

#define FILECODE LIBRARY_AMDERRORLOGLIB_AMDERRORLOGLIB_FILECODE

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

VOID
AquireErrorLog (
  IN       AMD_ERROR_BUFFER *ErrorLogBuffer,
  IN OUT   ERROR_LOG_DATA_STRUCT *ErrorLogDataPtr
  )
{
  UINT32        ErrorLogCounter;
  UINT16        Index;

  ErrorLogCounter = 0;
  LibAmdMemFill (ErrorLogDataPtr, 0, sizeof (ERROR_LOG_DATA_STRUCT), NULL);
  while (!((ErrorLogBuffer->ReadRecordPtr == ErrorLogBuffer->WriteRecordPtr) && (ErrorLogBuffer->ReadWriteFlag == 1))) {
    Index = ErrorLogBuffer->ReadRecordPtr;
    // ErrorLogCounter is start from 0
    ErrorLogDataPtr->ErrorLog_Param[ErrorLogCounter].ErrorClass = ErrorLogBuffer->AmdErrorStruct[Index].AmdError.ErrorClass;
    ErrorLogDataPtr->ErrorLog_Param[ErrorLogCounter].ErrorInfo  = ErrorLogBuffer->AmdErrorStruct[Index].AmdError.ErrorInfo;
    ErrorLogDataPtr->ErrorLog_Param[ErrorLogCounter].DataParam1 = ErrorLogBuffer->AmdErrorStruct[Index].AmdError.DataParam1;
    ErrorLogDataPtr->ErrorLog_Param[ErrorLogCounter].DataParam2 = ErrorLogBuffer->AmdErrorStruct[Index].AmdError.DataParam2;
    ErrorLogDataPtr->ErrorLog_Param[ErrorLogCounter].DataParam3 = ErrorLogBuffer->AmdErrorStruct[Index].AmdError.DataParam3;
    ErrorLogDataPtr->ErrorLog_Param[ErrorLogCounter].DataParam4 = ErrorLogBuffer->AmdErrorStruct[Index].AmdError.DataParam4;
    if (ErrorLogBuffer->ReadRecordPtr == (TOTAL_ERROR_LOG_BUFFERS - 1)) {
      ErrorLogBuffer->ReadRecordPtr = 0;
    } else {
      ErrorLogBuffer->ReadRecordPtr = ErrorLogBuffer->ReadRecordPtr + 1;
    }
    if (ErrorLogBuffer->ReadRecordPtr == ErrorLogBuffer->WriteRecordPtr) {
      ErrorLogBuffer->ReadWriteFlag = 1;
    }
    // Actual Error Log count
    ErrorLogCounter += 1;
  }
  ErrorLogDataPtr->Count = ErrorLogCounter;
}

VOID
AddErrorLog (
  IN OUT   AMD_ERROR_BUFFER *ErrorLogBuffer,
  IN       AMD_ERROR_ENTRY  *AmdErrorEntry
  )
{
  UINT16 Index;

  Index = ErrorLogBuffer->WriteRecordPtr;

  // Add the new Error log data into a circular buffer
  ErrorLogBuffer->AmdErrorStruct[Index].AmdError.ErrorClass = AmdErrorEntry->ErrorClass;
  ErrorLogBuffer->AmdErrorStruct[Index].AmdError.ErrorInfo  = AmdErrorEntry->ErrorInfo;
  ErrorLogBuffer->AmdErrorStruct[Index].AmdError.DataParam1 = AmdErrorEntry->DataParam1;
  ErrorLogBuffer->AmdErrorStruct[Index].AmdError.DataParam2 = AmdErrorEntry->DataParam2;
  ErrorLogBuffer->AmdErrorStruct[Index].AmdError.DataParam3 = AmdErrorEntry->DataParam3;
  ErrorLogBuffer->AmdErrorStruct[Index].AmdError.DataParam4 = AmdErrorEntry->DataParam4;

  if ((ErrorLogBuffer->WriteRecordPtr == ErrorLogBuffer->ReadRecordPtr) &&
     (ErrorLogBuffer->ReadWriteFlag == 0)) {
    ErrorLogBuffer->WriteRecordPtr += 1;
    ErrorLogBuffer->ReadRecordPtr += 1;
    if (ErrorLogBuffer->WriteRecordPtr == TOTAL_ERROR_LOG_BUFFERS) {
      ErrorLogBuffer->WriteRecordPtr = 0;
      ErrorLogBuffer->ReadRecordPtr  = 0;
    }
  } else {
    ErrorLogBuffer->WriteRecordPtr += 1;
    if (ErrorLogBuffer->WriteRecordPtr == TOTAL_ERROR_LOG_BUFFERS) {
      ErrorLogBuffer->WriteRecordPtr = 0;
    }
    ErrorLogBuffer->ReadWriteFlag = 0;
  }
  ErrorLogBuffer->Count = ErrorLogBuffer->Count + 1;

  if (ErrorLogBuffer->Count <= TOTAL_ERROR_LOG_BUFFERS) {
    ErrorLogBuffer->AmdErrorStruct[Index].Count = Index;
  }
}

VOID
ErrorLogBufferInit (
  IN OUT   AMD_ERROR_BUFFER *ErrorLogBuffer
  )
{
  ErrorLogBuffer->Count = 0;
  ErrorLogBuffer->ReadRecordPtr = 0;
  ErrorLogBuffer->WriteRecordPtr = 0;
  ErrorLogBuffer->ReadWriteFlag = 1;
}

