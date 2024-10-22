/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _AMD_ERROR_LOG_LIB_H_
#define _AMD_ERROR_LOG_LIB_H_

#include <AmdErrorLog.h>
#include <Pi/PiMultiPhase.h>

/**
 * An AMD Error Log entry.
 */
typedef struct {
  AMD_STATUS ErrorClass;   ///< The severity of the error, its associated AGESA_STATUS.
  UINT32   ErrorInfo;        ///< Uniquely identifies the error.
  UINT32   DataParam1;       ///< Error specific additional data
  UINT32   DataParam2;       ///< Error specific additional data
  UINT32   DataParam3;       ///< Error specific additional data
  UINT32   DataParam4;       ///< Error specific additional data
} AMD_ERROR_ENTRY;

/**
 * A wrapper for each Error Log entry.
 */
typedef struct {
  UINT16            Count;         ///< Entry number
  AMD_ERROR_ENTRY   AmdError;    ///< The entry itself.
} AMD_ERROR_STRUCT;

/**
 * The Error Log.
 */
typedef struct {
  UINT16        ReadWriteFlag;   ///< Read Write flag.
  UINT16        Count;           ///< The total number of active entries.
  UINT16        ReadRecordPtr;   ///< The next entry to read.
  UINT16        WriteRecordPtr;  ///< The next entry to write.
  AMD_ERROR_STRUCT AmdErrorStruct[TOTAL_ERROR_LOG_BUFFERS];   ///< The entries.
} AMD_ERROR_BUFFER;

typedef struct {
  EFI_HOB_GUID_TYPE    EfiHobGuidType;   ///< GUID Hob type structure
  AMD_ERROR_BUFFER     AmdErrorBuffer;   ///< AMD Error Log buffer structure
} AMD_ERROR_LOG_INFO_HOB;

extern void ErrorLogBufferInit (AMD_ERROR_BUFFER *ErrorLogBuffer);
extern void AddErrorLog (AMD_ERROR_BUFFER *ErrorLogBuffer, AMD_ERROR_ENTRY  *AmdErrorEntry);
extern void AquireErrorLog (AMD_ERROR_BUFFER *ErrorLogBuffer, ERROR_LOG_DATA_STRUCT *ErrorLogDataPtr);


#endif  // _AMD_ERROR_LOG_LIB_H_

