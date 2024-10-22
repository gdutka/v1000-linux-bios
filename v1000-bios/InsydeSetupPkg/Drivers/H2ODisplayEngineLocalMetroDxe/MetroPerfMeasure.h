/** @file
  Header file for the performance measurement of metro display engine

;******************************************************************************
;* Copyright (c) 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _METRO_PERFORMANCE_MEASURE_H_
#define _METRO_PERFORMANCE_MEASURE_H_

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/TimerLib.h>
#include <Library/UefiBootServicesTableLib.h>

typedef struct _METRO_PERFORMANCE_MEASURE METRO_PERFORMANCE_MEASURE;

METRO_PERFORMANCE_MEASURE *
EFIAPI
GetPerfMeasure (
  VOID
  );

#define PERF_INFO_NUM              100

typedef enum {
  MeasureInMilliSec,
  MeasureInMicroSec,
  MeasureInNanoSec,
  MeasureMax
} PERFORMANCE_MEASURE_UINT;

typedef
VOID
(EFIAPI *METRO_PERFORMANCE_MEASURE_START) (
  IN UINTN                         RecordIndex,
  IN BOOLEAN                       ValidFlag
  );

typedef
UINT64
(EFIAPI *METRO_PERFORMANCE_MEASURE_END) (
  IN UINTN                         RecordIndex,
  IN BOOLEAN                       ValidFlag,
  IN CHAR16                        *StartString,
  IN CHAR16                        *EndString
  );


typedef
UINT64
(EFIAPI *METRO_PERFORMANCE_MEASURE_TOTAL) (
  IN UINTN                         RecordIndex,
  IN BOOLEAN                       ValidFlag,
  IN BOOLEAN                       ClearRecord,
  IN CHAR16                        *StartString,
  IN CHAR16                        *EndString
  );

typedef
EFI_STATUS
(EFIAPI *METRO_PERFORMANCE_MEASURE_CHANGE_UNIT) (
  IN PERFORMANCE_MEASURE_UINT      MeasureUnit
  );

struct _METRO_PERFORMANCE_MEASURE {
  BOOLEAN                                 IsPrintEnable;
  BOOLEAN                                 PrintTickDifference;
  METRO_PERFORMANCE_MEASURE_START         Start;
  METRO_PERFORMANCE_MEASURE_END           End;
  METRO_PERFORMANCE_MEASURE_TOTAL         Total;
  METRO_PERFORMANCE_MEASURE_CHANGE_UNIT   ChangeUnit;
};

#endif
