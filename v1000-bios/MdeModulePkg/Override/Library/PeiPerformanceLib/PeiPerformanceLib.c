/** @file
  Performance library instance used in PEI phase.

  This file implements all APIs in Performance Library class in MdePkg. It creates
  performance logging GUIDed HOB on the first performance logging and then logs the
  performance data to the GUIDed HOB. Due to the limitation of temporary RAM, the maximum
  number of performance logging entry is specified by PcdMaxPeiPerformanceLogEntries or 
  PcdMaxPeiPerformanceLogEntries16.

Copyright (c) 2006 - 2016, Intel Corporation. All rights reserved.<BR>
(C) Copyright 2015-2016 Hewlett Packard Enterprise Development LP<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#include <PiPei.h>

#include <Guid/Performance.h>

#include <Library/PerformanceLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/BaseLib.h>
#include <Library/TimerLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>


/**
  Gets the GUID HOB for PEI performance.

  This internal function searches for the GUID HOB for PEI performance.
  If that GUID HOB is not found, it will build a new one.
  It outputs the data area of that GUID HOB to record performance log.

  @param    PeiPerformanceLog           Pointer to Pointer to PEI performance log header.
  @param    PeiPerformanceIdArray       Pointer to Pointer to PEI performance identifier array.

**/
VOID
InternalGetPerformanceHobLog (
  OUT PEI_PERFORMANCE_LOG_HEADER    **PeiPerformanceLog,
  OUT UINT32                        **PeiPerformanceIdArray
  )
{
  EFI_HOB_GUID_TYPE           *GuidHob;
  UINTN                       PeiPerformanceSize;
  UINT16                      PeiPerformanceLogEntries;

  ASSERT (PeiPerformanceLog != NULL);
  ASSERT (PeiPerformanceIdArray != NULL);

  PeiPerformanceLogEntries = (UINT16) (PcdGet16 (PcdMaxPeiPerformanceLogEntries16) != 0 ?
                                       PcdGet16 (PcdMaxPeiPerformanceLogEntries16) :
                                       PcdGet8 (PcdMaxPeiPerformanceLogEntries));
  GuidHob = GetFirstGuidHob (&gPerformanceProtocolGuid);

  if (GuidHob != NULL) {
    //
    // PEI Performance HOB was found, then return the existing one.
    //
    *PeiPerformanceLog = GET_GUID_HOB_DATA (GuidHob);

    GuidHob = GetFirstGuidHob (&gPerformanceExProtocolGuid);
    ASSERT (GuidHob != NULL);
    *PeiPerformanceIdArray = GET_GUID_HOB_DATA (GuidHob);
  } else {
    //
    // PEI Performance HOB was not found, then build one.
    //
    PeiPerformanceSize     = sizeof (PEI_PERFORMANCE_LOG_HEADER) +
                             sizeof (PEI_PERFORMANCE_LOG_ENTRY) * PeiPerformanceLogEntries;
    *PeiPerformanceLog     = BuildGuidHob (&gPerformanceProtocolGuid, PeiPerformanceSize);
    *PeiPerformanceLog     = ZeroMem (*PeiPerformanceLog, PeiPerformanceSize);

    PeiPerformanceSize     = sizeof (UINT32) * PeiPerformanceLogEntries;
    *PeiPerformanceIdArray = BuildGuidHob (&gPerformanceExProtocolGuid, PeiPerformanceSize);
    *PeiPerformanceIdArray = ZeroMem (*PeiPerformanceIdArray, PeiPerformanceSize);
  }
}

/**
  Searches in the log array with keyword Handle, Token, Module and Identifier.

  This internal function searches for the log entry in the log array.
  If there is an entry that exactly matches the given keywords
  and its end time stamp is zero, then the index of that log entry is returned;
  otherwise, the the number of log entries in the array is returned.

  @param  PeiPerformanceLog       Pointer to the data structure containing PEI 
                                  performance data.
  @param  PeiPerformanceIdArray   Pointer to PEI performance identifier array.
  @param  Handle                  Pointer to environment specific context used
                                  to identify the component being measured.
  @param  Token                   Pointer to a Null-terminated ASCII string
                                  that identifies the component being measured.
  @param  Module                  Pointer to a Null-terminated ASCII string
                                  that identifies the module being measured.
  @param  Identifier              32-bit identifier.

  @retval The index of log entry in the array.

**/
UINT32
InternalSearchForLogEntry (
  IN PEI_PERFORMANCE_LOG_HEADER *PeiPerformanceLog,
  IN UINT32                     *PeiPerformanceIdArray,
  IN CONST VOID                 *Handle,  OPTIONAL
  IN CONST CHAR8                *Token,   OPTIONAL
  IN CONST CHAR8                *Module,   OPTIONAL
  IN UINT32                     Identifier
  )
{
  UINT32                    Index;
  UINT32                    Index2;
  UINT32                    NumberOfEntries;
  PEI_PERFORMANCE_LOG_ENTRY *LogEntryArray;


  if (Token == NULL) {
    Token = "";
  }
  if (Module == NULL) {
    Module = "";
  }
  NumberOfEntries = PeiPerformanceLog->NumberOfEntries;
  LogEntryArray   = (PEI_PERFORMANCE_LOG_ENTRY *) (PeiPerformanceLog + 1);

  Index2 = 0;

  for (Index = 0; Index < NumberOfEntries; Index++) {
    Index2 = NumberOfEntries - 1 - Index;
    if (LogEntryArray[Index2].EndTimeStamp == 0 &&
        (LogEntryArray[Index2].Handle == (EFI_PHYSICAL_ADDRESS) (UINTN) Handle) &&
        AsciiStrnCmp (LogEntryArray[Index2].Token, Token, PEI_PERFORMANCE_STRING_LENGTH) == 0 &&
        AsciiStrnCmp (LogEntryArray[Index2].Module, Module, PEI_PERFORMANCE_STRING_LENGTH) == 0) {
      Index = Index2;
      break;
    }
  }
  return Index;
}

/**
  Creates a record for the beginning of a performance measurement.

  Creates a record that contains the Handle, Token, Module and Identifier.
  If TimeStamp is not zero, then TimeStamp is added to the record as the start time.
  If TimeStamp is zero, then this function reads the current time stamp
  and adds that time stamp value to the record as the start time.

  @param  Handle                  Pointer to environment specific context used
                                  to identify the component being measured.
  @param  Token                   Pointer to a Null-terminated ASCII string
                                  that identifies the component being measured.
  @param  Module                  Pointer to a Null-terminated ASCII string
                                  that identifies the module being measured.
  @param  TimeStamp               64-bit time stamp.
  @param  Identifier              32-bit identifier. If the value is 0, the created record
                                  is same as the one created by StartPerformanceMeasurement.

  @retval RETURN_SUCCESS          The start of the measurement was recorded.
  @retval RETURN_OUT_OF_RESOURCES There are not enough resources to record the measurement.

**/
RETURN_STATUS
EFIAPI
StartPerformanceMeasurementEx (
  IN CONST VOID   *Handle,  OPTIONAL
  IN CONST CHAR8  *Token,   OPTIONAL
  IN CONST CHAR8  *Module,  OPTIONAL
  IN UINT64       TimeStamp,
  IN UINT32       Identifier
  )
{
  PEI_PERFORMANCE_LOG_HEADER  *PeiPerformanceLog;
  UINT32                      *PeiPerformanceIdArray;
  PEI_PERFORMANCE_LOG_ENTRY   *LogEntryArray;
  UINT32                      Index;
  UINT16                      PeiPerformanceLogEntries;

  PeiPerformanceLogEntries = (UINT16) (PcdGet16 (PcdMaxPeiPerformanceLogEntries16) != 0 ?
                                       PcdGet16 (PcdMaxPeiPerformanceLogEntries16) :
                                       PcdGet8 (PcdMaxPeiPerformanceLogEntries));

  InternalGetPerformanceHobLog (&PeiPerformanceLog, &PeiPerformanceIdArray);

  if (PeiPerformanceLog->NumberOfEntries >= PeiPerformanceLogEntries) {
    DEBUG ((DEBUG_ERROR, "PEI performance log array out of resources\n"));
    return RETURN_OUT_OF_RESOURCES;
  }
  Index                       = PeiPerformanceLog->NumberOfEntries++;
  LogEntryArray               = (PEI_PERFORMANCE_LOG_ENTRY *) (PeiPerformanceLog + 1);
  LogEntryArray[Index].Handle = (EFI_PHYSICAL_ADDRESS) (UINTN) Handle;

  if (Token != NULL) {
    AsciiStrnCpyS (LogEntryArray[Index].Token, PEI_PERFORMANCE_STRING_SIZE, Token, PEI_PERFORMANCE_STRING_LENGTH);
  }
  if (Module != NULL) {
    AsciiStrnCpyS (LogEntryArray[Index].Module, PEI_PERFORMANCE_STRING_SIZE, Module, PEI_PERFORMANCE_STRING_LENGTH);
  }

  LogEntryArray[Index].EndTimeStamp = 0;
  PeiPerformanceIdArray[Index] = Identifier;

  if (TimeStamp == 0) {
    TimeStamp = GetPerformanceCounter ();
  }
  LogEntryArray[Index].StartTimeStamp = TimeStamp;

  return RETURN_SUCCESS;
}

/**
  Fills in the end time of a performance measurement.

  Looks up the record that matches Handle, Token and Module.
  If the record can not be found then return RETURN_NOT_FOUND.
  If the record is found and TimeStamp is not zero,
  then TimeStamp is added to the record as the end time.
  If the record is found and TimeStamp is zero, then this function reads
  the current time stamp and adds that time stamp value to the record as the end time.

  @param  Handle                  Pointer to environment specific context used
                                  to identify the component being measured.
  @param  Token                   Pointer to a Null-terminated ASCII string
                                  that identifies the component being measured.
  @param  Module                  Pointer to a Null-terminated ASCII string
                                  that identifies the module being measured.
  @param  TimeStamp               64-bit time stamp.
  @param  Identifier              32-bit identifier. If the value is 0, the found record
                                  is same as the one found by EndPerformanceMeasurement.

  @retval RETURN_SUCCESS          The end of  the measurement was recorded.
  @retval RETURN_NOT_FOUND        The specified measurement record could not be found.

**/
RETURN_STATUS
EFIAPI
EndPerformanceMeasurementEx (
  IN CONST VOID   *Handle,  OPTIONAL
  IN CONST CHAR8  *Token,   OPTIONAL
  IN CONST CHAR8  *Module,  OPTIONAL
  IN UINT64       TimeStamp,
  IN UINT32       Identifier
  )
{
  PEI_PERFORMANCE_LOG_HEADER  *PeiPerformanceLog;
  UINT32                      *PeiPerformanceIdArray;
  PEI_PERFORMANCE_LOG_ENTRY   *LogEntryArray;
  UINT32                      Index;

  if (TimeStamp == 0) {
    TimeStamp = GetPerformanceCounter ();
  }

  InternalGetPerformanceHobLog (&PeiPerformanceLog, &PeiPerformanceIdArray);
  Index             = InternalSearchForLogEntry (PeiPerformanceLog, PeiPerformanceIdArray, Handle, Token, Module, Identifier);
  if (Index >= PeiPerformanceLog->NumberOfEntries) {
    return RETURN_NOT_FOUND;
  }
  LogEntryArray     = (PEI_PERFORMANCE_LOG_ENTRY *) (PeiPerformanceLog + 1);
  LogEntryArray[Index].EndTimeStamp = TimeStamp;

  return RETURN_SUCCESS;
}

/**
  Attempts to retrieve a performance measurement log entry from the performance measurement log.
  It can also retrieve the log created by StartPerformanceMeasurement and EndPerformanceMeasurement,
  and then assign the Identifier with 0.

  Attempts to retrieve the performance log entry specified by LogEntryKey.  If LogEntryKey is
  zero on entry, then an attempt is made to retrieve the first entry from the performance log,
  and the key for the second entry in the log is returned.  If the performance log is empty,
  then no entry is retrieved and zero is returned.  If LogEntryKey is not zero, then the performance
  log entry associated with LogEntryKey is retrieved, and the key for the next entry in the log is
  returned.  If LogEntryKey is the key for the last entry in the log, then the last log entry is
  retrieved and an implementation specific non-zero key value that specifies the end of the performance
  log is returned.  If LogEntryKey is equal this implementation specific non-zero key value, then no entry
  is retrieved and zero is returned.  In the cases where a performance log entry can be returned,
  the log entry is returned in Handle, Token, Module, StartTimeStamp, EndTimeStamp and Identifier.
  If LogEntryKey is not a valid log entry key for the performance measurement log, then ASSERT().
  If Handle is NULL, then ASSERT().
  If Token is NULL, then ASSERT().
  If Module is NULL, then ASSERT().
  If StartTimeStamp is NULL, then ASSERT().
  If EndTimeStamp is NULL, then ASSERT().
  If Identifier is NULL, then ASSERT().

  @param  LogEntryKey             On entry, the key of the performance measurement log entry to retrieve.
                                  0, then the first performance measurement log entry is retrieved.
                                  On exit, the key of the next performance of entry entry.
  @param  Handle                  Pointer to environment specific context used to identify the component
                                  being measured.
  @param  Token                   Pointer to a Null-terminated ASCII string that identifies the component
                                  being measured.
  @param  Module                  Pointer to a Null-terminated ASCII string that identifies the module
                                  being measured.
  @param  StartTimeStamp          Pointer to the 64-bit time stamp that was recorded when the measurement
                                  was started.
  @param  EndTimeStamp            Pointer to the 64-bit time stamp that was recorded when the measurement
                                  was ended.
  @param  Identifier              Pointer to the 32-bit identifier that was recorded.

  @return The key for the next performance log entry (in general case).

**/
UINTN
EFIAPI
GetPerformanceMeasurementEx (
  IN  UINTN       LogEntryKey,
  OUT CONST VOID  **Handle,
  OUT CONST CHAR8 **Token,
  OUT CONST CHAR8 **Module,
  OUT UINT64      *StartTimeStamp,
  OUT UINT64      *EndTimeStamp,
  OUT UINT32      *Identifier
  )
{
  PEI_PERFORMANCE_LOG_HEADER  *PeiPerformanceLog;
  UINT32                      *PeiPerformanceIdArray;
  PEI_PERFORMANCE_LOG_ENTRY   *CurrentLogEntry;
  PEI_PERFORMANCE_LOG_ENTRY   *LogEntryArray;
  UINTN                       NumberOfEntries;

  ASSERT (Handle != NULL);
  ASSERT (Token != NULL);
  ASSERT (Module != NULL);
  ASSERT (StartTimeStamp != NULL);
  ASSERT (EndTimeStamp != NULL);
  ASSERT (Identifier != NULL);

  InternalGetPerformanceHobLog (&PeiPerformanceLog, &PeiPerformanceIdArray);

  NumberOfEntries   = (UINTN) (PeiPerformanceLog->NumberOfEntries);
  LogEntryArray     = (PEI_PERFORMANCE_LOG_ENTRY *) (PeiPerformanceLog + 1);
  //
  // Make sure that LogEntryKey is a valid log entry key.
  //
  ASSERT (LogEntryKey <= NumberOfEntries);

  if (LogEntryKey == NumberOfEntries) {
    return 0;
  }

  CurrentLogEntry = &(LogEntryArray[LogEntryKey]);

  *Handle         = (VOID *) (UINTN) (CurrentLogEntry->Handle);
  *Token          = CurrentLogEntry->Token;
  *Module         = CurrentLogEntry->Module;
  *StartTimeStamp = CurrentLogEntry->StartTimeStamp;
  *EndTimeStamp   = CurrentLogEntry->EndTimeStamp;
  *Identifier     = PeiPerformanceIdArray[LogEntryKey++];

  return LogEntryKey;
}

/**
  Creates a record for the beginning of a performance measurement.

  Creates a record that contains the Handle, Token, and Module.
  If TimeStamp is not zero, then TimeStamp is added to the record as the start time.
  If TimeStamp is zero, then this function reads the current time stamp
  and adds that time stamp value to the record as the start time.

  @param  Handle                  Pointer to environment specific context used
                                  to identify the component being measured.
  @param  Token                   Pointer to a Null-terminated ASCII string
                                  that identifies the component being measured.
  @param  Module                  Pointer to a Null-terminated ASCII string
                                  that identifies the module being measured.
  @param  TimeStamp               64-bit time stamp.

  @retval RETURN_SUCCESS          The start of the measurement was recorded.
  @retval RETURN_OUT_OF_RESOURCES There are not enough resources to record the measurement.

**/
RETURN_STATUS
EFIAPI
StartPerformanceMeasurement (
  IN CONST VOID   *Handle,  OPTIONAL
  IN CONST CHAR8  *Token,   OPTIONAL
  IN CONST CHAR8  *Module,  OPTIONAL
  IN UINT64       TimeStamp
  )
{
  return StartPerformanceMeasurementEx (Handle, Token, Module, TimeStamp, 0);
}

/**
  Fills in the end time of a performance measurement.

  Looks up the record that matches Handle, Token, and Module.
  If the record can not be found then return RETURN_NOT_FOUND.
  If the record is found and TimeStamp is not zero,
  then TimeStamp is added to the record as the end time.
  If the record is found and TimeStamp is zero, then this function reads
  the current time stamp and adds that time stamp value to the record as the end time.

  @param  Handle                  Pointer to environment specific context used
                                  to identify the component being measured.
  @param  Token                   Pointer to a Null-terminated ASCII string
                                  that identifies the component being measured.
  @param  Module                  Pointer to a Null-terminated ASCII string
                                  that identifies the module being measured.
  @param  TimeStamp               64-bit time stamp.

  @retval RETURN_SUCCESS          The end of  the measurement was recorded.
  @retval RETURN_NOT_FOUND        The specified measurement record could not be found.

**/
RETURN_STATUS
EFIAPI
EndPerformanceMeasurement (
  IN CONST VOID   *Handle,  OPTIONAL
  IN CONST CHAR8  *Token,   OPTIONAL
  IN CONST CHAR8  *Module,  OPTIONAL
  IN UINT64       TimeStamp
  )
{
  return EndPerformanceMeasurementEx (Handle, Token, Module, TimeStamp, 0);
}

/**
  Attempts to retrieve a performance measurement log entry from the performance measurement log.
  It can also retrieve the log created by StartPerformanceMeasurementEx and EndPerformanceMeasurementEx,
  and then eliminate the Identifier.

  Attempts to retrieve the performance log entry specified by LogEntryKey.  If LogEntryKey is
  zero on entry, then an attempt is made to retrieve the first entry from the performance log,
  and the key for the second entry in the log is returned.  If the performance log is empty,
  then no entry is retrieved and zero is returned.  If LogEntryKey is not zero, then the performance
  log entry associated with LogEntryKey is retrieved, and the key for the next entry in the log is
  returned.  If LogEntryKey is the key for the last entry in the log, then the last log entry is
  retrieved and an implementation specific non-zero key value that specifies the end of the performance
  log is returned.  If LogEntryKey is equal this implementation specific non-zero key value, then no entry
  is retrieved and zero is returned.  In the cases where a performance log entry can be returned,
  the log entry is returned in Handle, Token, Module, StartTimeStamp, and EndTimeStamp.
  If LogEntryKey is not a valid log entry key for the performance measurement log, then ASSERT().
  If Handle is NULL, then ASSERT().
  If Token is NULL, then ASSERT().
  If Module is NULL, then ASSERT().
  If StartTimeStamp is NULL, then ASSERT().
  If EndTimeStamp is NULL, then ASSERT().

  @param  LogEntryKey             On entry, the key of the performance measurement log entry to retrieve.
                                  0, then the first performance measurement log entry is retrieved.
                                  On exit, the key of the next performance of entry entry.
  @param  Handle                  Pointer to environment specific context used to identify the component
                                  being measured.
  @param  Token                   Pointer to a Null-terminated ASCII string that identifies the component
                                  being measured.
  @param  Module                  Pointer to a Null-terminated ASCII string that identifies the module
                                  being measured.
  @param  StartTimeStamp          Pointer to the 64-bit time stamp that was recorded when the measurement
                                  was started.
  @param  EndTimeStamp            Pointer to the 64-bit time stamp that was recorded when the measurement
                                  was ended.

  @return The key for the next performance log entry (in general case).

**/
UINTN
EFIAPI
GetPerformanceMeasurement (
  IN  UINTN       LogEntryKey,
  OUT CONST VOID  **Handle,
  OUT CONST CHAR8 **Token,
  OUT CONST CHAR8 **Module,
  OUT UINT64      *StartTimeStamp,
  OUT UINT64      *EndTimeStamp
  )
{
  UINT32 Identifier;
  return GetPerformanceMeasurementEx (LogEntryKey, Handle, Token, Module, StartTimeStamp, EndTimeStamp, &Identifier);
}

/**
  Returns TRUE if the performance measurement macros are enabled.

  This function returns TRUE if the PERFORMANCE_LIBRARY_PROPERTY_MEASUREMENT_ENABLED bit of
  PcdPerformanceLibraryPropertyMask is set.  Otherwise FALSE is returned.

  @retval TRUE                    The PERFORMANCE_LIBRARY_PROPERTY_MEASUREMENT_ENABLED bit of
                                  PcdPerformanceLibraryPropertyMask is set.
  @retval FALSE                   The PERFORMANCE_LIBRARY_PROPERTY_MEASUREMENT_ENABLED bit of
                                  PcdPerformanceLibraryPropertyMask is clear.

**/
BOOLEAN
EFIAPI
PerformanceMeasurementEnabled (
  VOID
  )
{
  return (BOOLEAN) ((PcdGet8(PcdPerformanceLibraryPropertyMask) & PERFORMANCE_LIBRARY_PROPERTY_MEASUREMENT_ENABLED) != 0);
}

/**
  Create performance record with event description and a timestamp.

  @param CallerIdentifier  - Image handle or pointer to caller ID GUID
  @param Guid              - Pointer to a GUID
  @param String            - Pointer to a string describing the measurement
  @param Address           - Pointer to a location in memory relevant to the measurement
  @param Identifier        - Performance identifier describing the type of measurement

  @retval RETURN_SUCCESS           - Successfully created performance record
  @retval RETURN_OUT_OF_RESOURCES  - Ran out of space to store the records
  @retval RETURN_INVALID_PARAMETER - Invalid parameter passed to function - NULL
                                     pointer or invalid PerfId

**/
RETURN_STATUS
EFIAPI
LogPerformanceMeasurement (
  IN CONST VOID   *CallerIdentifier,
  IN CONST VOID   *Guid,    OPTIONAL
  IN CONST CHAR8  *String,  OPTIONAL
  IN UINT64       Address, OPTIONAL
  IN UINT32       Identifier
  )
{
  EFI_STATUS Status;

  Status = EFI_SUCCESS;

  if (Identifier == MODULE_START_ID){
    Status =StartPerformanceMeasurementEx (CallerIdentifier, NULL, String, 0, Identifier);
  } else if (Identifier == MODULE_END_ID) {
    Status = EndPerformanceMeasurementEx  (CallerIdentifier, NULL, String, 0, Identifier);
  }
  return Status;
}


/**
  Check whether the specified performance measurement can be logged.

  This function returns TRUE when the PERFORMANCE_LIBRARY_PROPERTY_MEASUREMENT_ENABLED bit of PcdPerformanceLibraryPropertyMask is set
  and the Type disable bit in PcdPerformanceLibraryPropertyMask is not set.

  @param Type        - Type of the performance measurement entry.

  @retval TRUE         The performance measurement can be logged.
  @retval FALSE        The performance measurement can NOT be logged.

**/
BOOLEAN
EFIAPI
LogPerformanceMeasurementEnabled (
  IN  CONST UINTN        Type
  )
{
  //
  // When Performance measurement is enabled and the type is not filtered, the performance can be logged.
  //
  if (PerformanceMeasurementEnabled () && (PcdGet8(PcdPerformanceLibraryPropertyMask) & Type) == 0) {
    return TRUE;
  }
  return FALSE;
}