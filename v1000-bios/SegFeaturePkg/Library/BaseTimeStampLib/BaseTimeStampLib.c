/** @file
  Implementation of BaseTimeStampLib

;******************************************************************************
;* Copyright (c) 2021, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "InternalBaseTimeStampLib.h"

#include <Uefi.h>

#include <Pi/PiBootMode.h>
#include <Pi/PiHob.h>

#include <Library/BaseLib.h>
#include <Library/BaseTimeStampLib.h>
#include <Library/CmosLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Library/TimerLib.h>

STATIC UINTN mDayOfMonth[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/**
  Checks an 8-bit BCD value, and converts to an 8-bit value if valid.

  This function checks the 8-bit BCD value specified by Value.
  If valid, the function converts it to an 8-bit value and returns it.
  Otherwise, return 0xff.

  @param[in] Value       The 8-bit BCD value to check and convert

  @return The 8-bit value converted. Or 0xff if Value is invalid.

**/
STATIC
UINT8
CheckAndConvertBcd8ToDecimal8 (
  IN  UINT8  Value
  )
{
  if ((Value < 0xa0) && ((Value & 0xf) < 0xa)) {
    return BcdToDecimal8 (Value);
  }

  return 0xff;
}

/**
   Converts time read from RTC to EFI_TIME format defined by UEFI spec.

  This function converts raw time data read from RTC to the EFI_TIME format defined by UEFI spec.
  If data mode of RTC is BCD, then converts it to decimal,
  If RTC is in 12-hour format, then converts it to 24-hour format.

  @param[in, out] Time       On input, the time data read from RTC to convert
                              On output, the time converted to UEFI format
  @param[in]      Century    Value of century read from RTC.
  @param[in]      RegisterB  Value of Register B of RTC, indicating data mode and hour format.

**/
STATIC
VOID
ConvertRtcTimeToEfiTime (
  IN OUT  EFI_TIME        *Time,
  IN      UINT8           Century,
  IN      RTC_REGISTER_B  RegisterB
  )
{
  BOOLEAN IsPM;

  if ((Time->Hour & 0x80) != 0) {
    IsPM = TRUE;
  } else {
    IsPM = FALSE;
  }

  Time->Hour = (UINT8) (Time->Hour & 0x7f);

  if (RegisterB.Bits.Dm == 0) {
    Time->Year    = CheckAndConvertBcd8ToDecimal8 ((UINT8) Time->Year);
    Time->Month   = CheckAndConvertBcd8ToDecimal8 (Time->Month);
    Time->Day     = CheckAndConvertBcd8ToDecimal8 (Time->Day);
    Time->Hour    = CheckAndConvertBcd8ToDecimal8 (Time->Hour);
    Time->Minute  = CheckAndConvertBcd8ToDecimal8 (Time->Minute);
    Time->Second  = CheckAndConvertBcd8ToDecimal8 (Time->Second);
  }
  Century = CheckAndConvertBcd8ToDecimal8 (Century);

  Time->Year = (UINT16) (Century * 100 + Time->Year);

  //
  // If time is in 12 hour format, convert it to 24 hour format
  //
  if (RegisterB.Bits.Mil == 0) {
    if (IsPM && Time->Hour < 12) {
      Time->Hour = (UINT8) (Time->Hour + 12);
    }

    if (!IsPM && Time->Hour == 12) {
      Time->Hour = 0;
    }
  }

  Time->Nanosecond  = 0;
}

/**
  Read RTC content through its registers.

  @param[in] Address  Address offset of RTC. It is recommended to use macros such as RTC_ADDRESS_SECONDS.

  @return The data of UINT8 type read from RTC.

**/
STATIC
UINT8
RtcRead (
  IN  UINT8 Address
  )
{
  return ReadCmos8 (Address);
}

/**
  Get RTC time data. Before reading every RTC data, it will check RTC update status
  to make sure every RTC data is correct.

  @param[out] Time               Pointer to output time data

  @retval EFI_SUCCESS            Get RTC time data successfully.
  @retval EFI_NOT_READY          RTC data is in update progress. Fail to get time data.
  @retval EFI_INVALID_PARAMETER  Pointer of output time data is NULL.

**/
STATIC
EFI_STATUS
GetTimeWithRtcUpdateCheck (
  OUT   EFI_TIME  *Time
  )
{
  RTC_REGISTER_A  RegisterA;
  RTC_REGISTER_B  RegisterB;
  UINT8           Century;
  UINTN           Index;
  UINTN           RtcComponentNum;
  RTC_COMPONENT   RtcComponent[] = {{RTC_ADDRESS_YEAR            , 0},
                                    {RTC_ADDRESS_MONTH           , 0},
                                    {RTC_ADDRESS_DAY_OF_THE_MONTH, 0},
                                    {RTC_ADDRESS_HOURS           , 0},
                                    {RTC_ADDRESS_MINUTES         , 0},
                                    {RTC_ADDRESS_SECONDS         , 0}
                                    };

  if (Time == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  RtcComponentNum = sizeof (RtcComponent) / sizeof (RTC_COMPONENT);
  RegisterB.Data = RtcRead (RTC_ADDRESS_REGISTER_B);

  //
  // In order to preventing RTC data reading error upon RTC updating due to interrupt/SMI caused delay,
  // making RTC update checking before every RTC data reading to make sure every RTC data corrected
  //
  for (Index = 0; Index < RtcComponentNum; Index++) {
    RegisterA.Data = RtcRead (RTC_ADDRESS_REGISTER_A);
    if (RegisterA.Bits.Uip == 1) {
      MicroSecondDelay (10);
      return EFI_NOT_READY;
    }

    RtcComponent[Index].Data = RtcRead (RtcComponent[Index].Address);
  }

  Time->Year   = (UINT16) RtcComponent[0].Data;
  Time->Month  = RtcComponent[1].Data;
  Time->Day    = RtcComponent[2].Data;
  Time->Hour   = RtcComponent[3].Data;
  Time->Minute = RtcComponent[4].Data;
  Time->Second = RtcComponent[5].Data;

  Time->TimeZone = (INT16) (ReadCmos16 (CmosTimeZone));
  Time->Daylight = ReadCmos8 (CmosDaylight);

  Century = RtcRead (RTC_ADDRESS_CENTURY);

  ConvertRtcTimeToEfiTime (Time, Century, RegisterB);

  return EFI_SUCCESS;
}

/**
  Check if it is a leap year.

  @param    Time   The time to be checked.

  @retval   TRUE   It is a leap year.
  @retval   FALSE  It is NOT a leap year.

**/
STATIC
BOOLEAN
IsLeapYear (
  IN EFI_TIME   *Time
  )
{
  if (Time->Year % 4 == 0) {
    if (Time->Year % 100 == 0) {
      if (Time->Year % 400 == 0) {
        return TRUE;
      } else {
        return FALSE;
      }
    } else {
      return TRUE;
    }
  } else {
    return FALSE;
  }
}

/**
  See if field Day of an EFI_TIME is correct.

  @param    Time   Its Day field is to be checked.

  @retval   TRUE   Day field of Time is correct.
  @retval   FALSE  Day field of Time is NOT correct.

**/
STATIC
BOOLEAN
DayValid (
  IN  EFI_TIME  *Time
  )
{
  //
  // The validity of Time->Month field should be checked before
  //
  ASSERT (Time->Month >=1);
  ASSERT (Time->Month <=12);
  if (Time->Day < 1 ||
      Time->Day > mDayOfMonth[Time->Month - 1] ||
      (Time->Month == 2 && (!IsLeapYear (Time) && Time->Day > 28))
      ) {
    return FALSE;
  }

  return TRUE;
}

/**
   See if all fields of a variable of EFI_TIME type is correct.

  @param[in] Time                The time to be checked.

  @retval EFI_SUCCESS            Time is a valid EFI_TIME variable.
  @retval EFI_INVALID_PARAMETER  Some fields of Time are not correct.

**/
STATIC
EFI_STATUS
RtcTimeFieldsValid (
  IN  EFI_TIME  *Time
  )
{
  if (Time->Year < 1900 || // Follow the definition of year range (1900~9999) from UEFI spec
      Time->Year > 9999 ||
      Time->Month < 1 ||
      Time->Month > 12 ||
      (!DayValid (Time)) ||
      Time->Hour > 23 ||
      Time->Minute > 59 ||
      Time->Second > 59 ||
      Time->Nanosecond > 999999999 ||
      (!(Time->TimeZone == EFI_UNSPECIFIED_TIMEZONE || (Time->TimeZone >= -1440 && Time->TimeZone <= 1440))) ||
      ((Time->Daylight & (~(EFI_TIME_ADJUST_DAYLIGHT | EFI_TIME_IN_DAYLIGHT))) != 0)) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

/**
  Get timestamp string (ASCII).

  The timestamp string has format of "yyyy/mm/dd hh:mm:ss [aaa.bbbs]".
  As example, "2021/02/17 08:30:15 [015.394s]".
  The time in square brackets is elapsed time since system boot-up.

  @param[in]  BufferSize  The size of the string buffer. According to the format,
                          the size must be greater than 44 bytes.
  @param[out] Buffer      Pointer to the buffer for getting the timestamp string.

  @retval   The timestamp string.
  @retval   NULL. If an inappropriate buffer size was given.

**/
UINTN
EFIAPI
GetTimeStampString (
  IN  UINTN   BufferSize,
  OUT CHAR8   *Buffer
  )
{
  EFI_TIME          Time;
  EFI_STATUS        Status;
  UINTN             Index;
  EFI_HOB_GUID_TYPE *GuidHob;
  UINT64            StartTick;
  UINT64            EndTick;
  UINT64            InitialTimeTick;
  UINT64            CurrentTimeTick;
  INT64             DeltaTimeTick;
  UINT64            ElapsedTime;
  UINT64            Remainder;
  UINT16            Seconds;
  UINT16            Fraction;

  Status = EFI_SUCCESS;

  for (Index = 0; Index < MAX_RETRY; Index++) {
    Status = GetTimeWithRtcUpdateCheck (&Time);
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = RtcTimeFieldsValid (&Time);
    if (EFI_ERROR (Status)) {
      continue;
    }

    break;
  }

  if (EFI_ERROR (Status)) {
    Time.Second = PcdGet8 (PcdRealTimeClockInitSecond);
    Time.Minute = PcdGet8 (PcdRealTimeClockInitMinute);
    Time.Hour   = PcdGet8 (PcdRealTimeClockInitHour);
    Time.Day    = PcdGet8 (PcdRealTimeClockInitDay);
    Time.Month  = PcdGet8 (PcdRealTimeClockInitMonth);
    Time.Year   = PcdGet16 (PcdRealTimeClockInitYear);
  }

  GuidHob         = GetFirstGuidHob (&gPerformanceCounterReadyHobGuid);
  InitialTimeTick = *((UINT64 *) GET_GUID_HOB_DATA (GuidHob));
  CurrentTimeTick = GetPerformanceCounter ();
  GetPerformanceCounterProperties (&StartTick, &EndTick);
  DeltaTimeTick   = (EndTick < StartTick)?
                      InitialTimeTick - CurrentTimeTick :
                      CurrentTimeTick - InitialTimeTick;
  ElapsedTime     = GetTimeInNanoSecond ((UINT64) DeltaTimeTick);
  Seconds         = (UINT16) DivU64x64Remainder (ElapsedTime, 1000000000, &Remainder);
  Fraction        = (UINT16) DivU64x64Remainder (Remainder, 1000000, NULL);

  return AsciiSPrint (
           Buffer,
           BufferSize,
           "%04d/%02d/%02d %02d:%02d:%02d [%03d.%03ds]",
           Time.Year,
           Time.Month,
           Time.Day,
           Time.Hour,
           Time.Minute,
           Time.Second,
           Seconds,
           Fraction
           );
}

/**
  The constructor function of BaseTimeStampLib.

  The constructor will try to get time tick from system and record the
  initial tick as a GUID HOB data for timestamp service.

  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
BaseTimeStampLibConstructor (
  VOID
  )
{
  EFI_HOB_GUID_TYPE   *GuidHob;
  UINT8               RetryCount;
  UINT64              InitialTimeTick;

  GuidHob = GetFirstGuidHob (&gPerformanceCounterReadyHobGuid);
  if (GuidHob != NULL) {
    return EFI_SUCCESS;
  }

  RetryCount = 0;
  do {
    InitialTimeTick = GetPerformanceCounter ();
    RetryCount++;
  } while (InitialTimeTick == 0 && RetryCount < 10);

  GuidHob = BuildGuidDataHob (
              &gPerformanceCounterReadyHobGuid,
              &InitialTimeTick,
              sizeof (InitialTimeTick)
              );
  if (GuidHob == NULL) {
    DEBUG ((EFI_D_INFO, "Fail to BuildGuidDataHob for %g!", gPerformanceCounterReadyHobGuid));
  }

  return EFI_SUCCESS;
}