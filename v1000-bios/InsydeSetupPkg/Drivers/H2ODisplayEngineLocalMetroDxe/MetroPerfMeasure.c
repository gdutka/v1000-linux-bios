/** @file
  Performance measure related functions

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

#include "MetroPerfMeasure.h"

typedef struct _METRO_PERFORMANCE_PRIVATE_DATA {
  UINT64                               StartTick[PERF_INFO_NUM];
  UINT64                               TotalDiffTick[PERF_INFO_NUM];
  UINT32                               Count[PERF_INFO_NUM];
  CHAR16                               *UnitStr;
  UINT32                               UnitDivisor;
  METRO_PERFORMANCE_MEASURE            PerfMeasure;
} METRO_PERFORMANCE_PRIVATE_DATA;
METRO_PERFORMANCE_PRIVATE_DATA         *mPerfPrivateData = NULL;

typedef struct _METRO_PERFORMANCE_MEASURE_UNIT_INFO {
  PERFORMANCE_MEASURE_UINT             Unit;
  UINT32                               Divisor;
  CHAR16                               *String;
} METRO_PERFORMANCE_MEASURE_UNIT_INFO;
METRO_PERFORMANCE_MEASURE_UNIT_INFO    mUnitInfo[] = {{MeasureInMilliSec, 1000000000, L"MilliSec"},
                                                      {MeasureInMicroSec, 1000000   , L"MicroSec"},
                                                      {MeasureInNanoSec , 1000      , L"NanoSec" }
                                                      };

/**
 Converts elapsed ticks of performance counter to time in picoseconds.
 This function converts the elapsed ticks of running performance counter to time value in unit of picoseconds.

 @param[in] Ticks     The number of elapsed ticks of running performance counter.

 @return The elapsed time in picoseconds.
**/
STATIC
UINT64
InternalGetTimeInPicoSecond (
  IN UINT64                        Ticks
  )
{
  STATIC UINT64                    Frequency = 0;
  UINT64                           PicoSeconds;
  UINT64                           Remainder;
  INTN                             Shift;

  if (Frequency == 0) {
    Frequency = GetPerformanceCounterProperties (NULL, NULL);
  }

  //
  //          Ticks
  // Time = --------- x 1,000,000,000
  //        Frequency
  //
  PicoSeconds = MultU64x32 (DivU64x64Remainder (Ticks, Frequency, &Remainder), 1000000000u);

  //
  // Ensure (Remainder * 1,000,000,000) will not overflow 64-bit.
  // Since 2^29 < 1,000,000,000 = 0x3B9ACA00 < 2^30, Remainder should < 2^(64-30) = 2^34,
  // i.e. highest bit set in Remainder should <= 33.
  //
  Shift = MAX (0, HighBitSet64 (Remainder) - 33);
  Remainder = RShiftU64 (Remainder, (UINTN) Shift);
  Frequency = RShiftU64 (Frequency, (UINTN) Shift);
  PicoSeconds += DivU64x64Remainder (MultU64x32 (Remainder, 1000000000u), Frequency, NULL);

  return PicoSeconds;
}

/**
 Start performance measure for record index.

 @param[in] RecordIndex   Record index
 @param[in] ValidFlag     Flag to determine if start performance measure
**/
VOID
EFIAPI
PerfMeasureStart (
  IN UINTN                         RecordIndex,
  IN BOOLEAN                       ValidFlag
  )
{
  ASSERT (RecordIndex < PERF_INFO_NUM);
  if (RecordIndex >= PERF_INFO_NUM || !ValidFlag) {
    return;
  }

  mPerfPrivateData->StartTick[RecordIndex] = GetPerformanceCounter();
}

/**
 End performance measure for record index.

 @param[in] RecordIndex   Record index
 @param[in] ValidFlag     Flag to determine if end performance measure
 @param[in] StartString   Pointer to start string for debug
 @param[in] EndString     Pointer to end string for debug

 @return The differenc time or ticks based on parformance measure private data.
**/
UINT64
EFIAPI
PerfMeasureEnd (
  IN UINTN                         RecordIndex,
  IN BOOLEAN                       ValidFlag,
  IN CHAR16                        *StartString,
  IN CHAR16                        *EndString
  )
{
  UINT64                           EndTick;
  UINT64                           DiffTick;
  UINT64                           DiffTime;
  METRO_PERFORMANCE_MEASURE        *PerfMeasure;

  ASSERT (RecordIndex < PERF_INFO_NUM);
  if (RecordIndex >= PERF_INFO_NUM || !ValidFlag) {
    return 0;
  }

  EndTick  = GetPerformanceCounter();
  DiffTick = EndTick - mPerfPrivateData->StartTick[RecordIndex];
  mPerfPrivateData->TotalDiffTick[RecordIndex] += DiffTick;
  mPerfPrivateData->Count[RecordIndex]++;

  PerfMeasure = &mPerfPrivateData->PerfMeasure;
  if (PerfMeasure->PrintTickDifference) {
    if (PerfMeasure->IsPrintEnable) {
      DEBUG ((EFI_D_ERROR, "%s, Ticks = %08ld %s", StartString, DiffTick, EndString));
    }
    return DiffTick;
  } else {
    DiffTime = DivU64x32 (InternalGetTimeInPicoSecond (DiffTick), mPerfPrivateData->UnitDivisor);
    if (PerfMeasure->IsPrintEnable) {
      DEBUG ((EFI_D_ERROR, "%s, %s = %08ld %s", StartString, mPerfPrivateData->UnitStr, DiffTime, EndString));
    }
    return DiffTime;
  }
}

/**
 Get total difference for record index.

 @param[in] RecordIndex   Record index
 @param[in] ValidFlag     Flag to determine if end performance measure
 @param[in] ClearRecord   Flag to determine if clear record
 @param[in] StartString   Pointer to start string for debug
 @param[in] EndString     Pointer to end string for debug

 @return The total differenc time or ticks based on parformance measure private data.
**/
UINT64
EFIAPI
PerfMeasureTotal (
  IN UINTN                         RecordIndex,
  IN BOOLEAN                       ValidFlag,
  IN BOOLEAN                       ClearRecord,
  IN CHAR16                        *StartString,
  IN CHAR16                        *EndString
  )
{
  UINT64                           Difference;
  METRO_PERFORMANCE_MEASURE        *PerfMeasure;

  ASSERT (RecordIndex < PERF_INFO_NUM);
  if (RecordIndex >= PERF_INFO_NUM || !ValidFlag) {
    return 0;
  }

  PerfMeasure = &mPerfPrivateData->PerfMeasure;
  if (PerfMeasure->PrintTickDifference) {
    Difference = mPerfPrivateData->TotalDiffTick[RecordIndex];
    if (PerfMeasure->IsPrintEnable) {
      DEBUG ((EFI_D_ERROR, "%s, Ticks = %08ld %s", StartString, Difference, EndString));
    }
  } else {
    Difference = DivU64x32 (InternalGetTimeInPicoSecond (mPerfPrivateData->TotalDiffTick[RecordIndex]), mPerfPrivateData->UnitDivisor);
    if (PerfMeasure->IsPrintEnable) {
      if (mPerfPrivateData->Count[RecordIndex] == 0) {
        DEBUG ((EFI_D_ERROR, "%s, %s = %08ld %s", StartString, mPerfPrivateData->UnitStr, Difference, EndString));
      } else {
        DEBUG ((EFI_D_ERROR, "%s, %s = %08ld (Count = %d, Avg = %d)\n", StartString, mPerfPrivateData->UnitStr, Difference, mPerfPrivateData->Count[RecordIndex], DivU64x32 (Difference, mPerfPrivateData->Count[RecordIndex])));
      }
    }
  }

  if (ClearRecord) {
    mPerfPrivateData->TotalDiffTick[RecordIndex] = 0;
    mPerfPrivateData->Count[RecordIndex] = 0;
  }

  return Difference;
}

/**
 Change measure unit.

 @param[in] MeasureUnit    Measure unit

 @retval EFI_SUCCESS       Change measure unit successfully
 @retval EFI_UNSUPPORTED   Input unit is not found
**/
EFI_STATUS
EFIAPI
PerfMeasureChangeUnit (
  IN PERFORMANCE_MEASURE_UINT      MeasureUnit
  )
{
  UINTN                            Index;

  for (Index = 0; Index < sizeof (mUnitInfo) / sizeof (METRO_PERFORMANCE_MEASURE_UNIT_INFO); Index++) {
    if (mUnitInfo[Index].Unit == MeasureUnit) {
      mPerfPrivateData->UnitStr     = mUnitInfo[Index].String;
      mPerfPrivateData->UnitDivisor = mUnitInfo[Index].Divisor;
      return EFI_SUCCESS;
    }
  }

  return EFI_UNSUPPORTED;
}

/**
 Get performance measure instance

 @return The pointer of performance measure instance or NULL if allocate memory failed.
**/
METRO_PERFORMANCE_MEASURE *
EFIAPI
GetPerfMeasure (
  VOID
  )
{
  METRO_PERFORMANCE_MEASURE        *PerfMeasure;

  if (mPerfPrivateData != NULL) {
    return &mPerfPrivateData->PerfMeasure;
  }

  mPerfPrivateData = AllocateZeroPool (sizeof (METRO_PERFORMANCE_PRIVATE_DATA));
  if (mPerfPrivateData == NULL) {
    return NULL;
  }
  PerfMeasure                      = &mPerfPrivateData->PerfMeasure;
  PerfMeasure->IsPrintEnable       = FALSE;
  PerfMeasure->PrintTickDifference = FALSE;
  PerfMeasure->Start               = PerfMeasureStart;
  PerfMeasure->End                 = PerfMeasureEnd;
  PerfMeasure->Total               = PerfMeasureTotal;
  PerfMeasure->ChangeUnit          = PerfMeasureChangeUnit;

  PerfMeasureChangeUnit (MeasureInMicroSec);

  return PerfMeasure;
}

