/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _AMD_ERROR_LOG_H_
#define _AMD_ERROR_LOG_H_

#define TOTAL_ERROR_LOG_BUFFERS  128

/// The return status for all AGESA public services.
///
/// Services return the most severe status of any logged error.  Status other than SUCCESS, UNSUPPORTED, and BOUNDS_CHK
/// will have log entries with more detail.
///
typedef enum {
  AMD_SUCCESS = 0,             ///< The service completed normally. Info may be logged.
  AMD_UNSUPPORTED,             ///< The dispatcher or create struct had an unimplemented function requested.
                                 ///<  Not logged.
  AMD_BOUNDS_CHK,              ///< A dynamic parameter was out of range and the service was not provided.
                                 ///< Example, memory address not installed, heap buffer handle not found.
                                 ///< Not Logged.
  // AGESA_STATUS of greater severity (the ones below this line), always have a log entry available.
  AMD_ALERT,                   ///< An observed condition, but no loss of function.  See Log.
  AMD_WARNING,                 ///< Possible or minor loss of function.  See Log.
  AMD_ERROR,                   ///< Significant loss of function, boot may be possible.  See Log.
  AMD_CRITICAL,                ///< Continue boot only to notify user.  See Log.
  AMD_FATAL,                   ///< Halt booting.  See Log, however Fatal errors pertaining to heap problems
                                 ///< may not be able to reliably produce log errors.
  AmdStatusMax                 ///< Not a status, for limit checking.
} AMD_STATUS;

// ========================================
// CPU general events            (SubFunction = 01h)
#define CPU_EVENT_NON_CCX_BIST_FAILURE      0x08000100ul
#define CPU_EVENT_CCX_BIST_FAILURE          0x08000101ul
#define CPU_EVENT_DOWN_CORE_FAILURE         0x08000102ul
  #define CCD_INVALID_SELECTION_FAILURE     0x1
  #define CCD_PREV_DOWN_CORE_FAILURE        0x2
  #define CCD_INVALID_CONFIG_FAILURE        0x3
  #define CCD_DOWN_CORE_CONFIG_MET          0x4
  #define CORE_INVALID_SELECTION_FAILURE    0x5
  #define CORE_PREV_DOWN_CORE_FAILURE       0x6
  #define CORE_INVALID_CONFIG_FAILURE       0x7
  #define CORE_DOWN_CORE_CONFIG_MET         0x8


/// An Error Log Entry.
typedef struct {
     OUT   UINT32            ErrorClass;        ///< The severity of this error, matches AGESA_STATUS.
     OUT   UINT32            ErrorInfo;         ///< The unique error identifier, zero means "no error".
     OUT   UINT32            DataParam1;        ///< Data specific to the error.
     OUT   UINT32            DataParam2;        ///< Data specific to the error.
     OUT   UINT32            DataParam3;        ///< Data specific to the error.
     OUT   UINT32            DataParam4;        ///< Data specific to the error.
} ERROR_LOG_PARAMS;

/// An Error Log Entry.
typedef struct {
     OUT   UINT32            Count;              ///< Actual Error Log entries count.
     OUT   ERROR_LOG_PARAMS  ErrorLog_Param[TOTAL_ERROR_LOG_BUFFERS];   ///< Data specific to the Error.
} ERROR_LOG_DATA_STRUCT;

#endif  // _AMD_ERROR_LOG_H_

