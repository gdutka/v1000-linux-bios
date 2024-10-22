/** @file
  S5Wakeup.c.

;*******************************************************************************
;* Copyright (c) 2015, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#include <SmmPlatform.h>

/**
  Converts setup time to RTC time.

  @param [in]   DecimalValue      The deciaml value.

  @retval       UINT8             RTC time.

**/
UINT8
ConvertSetupTimeToRtcTime (
  IN  UINT8 DecimalValue
  )
{
  UINTN   High, Low;

  High    = DecimalValue / 10;
  Low     = DecimalValue % 10;

  return ((UINT8)(Low + (High * 16)));
}

/**
  Setting for S5 wake up.

  @param [in]   AcpiBaseAddr        ACPI Base Address.
  @param [in]   SetupVariable       SETUP Variable pointer.

  @retval       void.

**/
VOID
S5WakeUpSetting (
  IN UINT16                         AcpiBaseAddr,
  IN SYSTEM_CONFIGURATION          *SetupVariable
  )
{
  UINT8   RtcSecond;
  UINT8   RtcMinute;
  UINT8   RtcHour;
  UINT8   Buffer;
  BOOLEAN BcdMode;

  RtcSecond = 0;
  RtcMinute = 0;
  RtcHour   = 0;
  Buffer    = 0;
  BcdMode   = TRUE;

  if (SetupVariable->WakeOnS5 == 0x00) {
    return;
  }

  //
  // Get the Data Mode. (BCD or Binary)
  //
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_REGISTER_B);
  Buffer = IoRead8 (PCAT_RTC_DATA_REGISTER);
  BcdMode = ~(Buffer >> 2);
  BcdMode &= BIT0;

  if (BcdMode) {
    RtcSecond = ConvertSetupTimeToRtcTime(SetupVariable->WakeOnS5Time.Second);
    RtcMinute = ConvertSetupTimeToRtcTime(SetupVariable->WakeOnS5Time.Minute);
    RtcHour   = ConvertSetupTimeToRtcTime(SetupVariable->WakeOnS5Time.Hour);
  } else {
    RtcSecond = SetupVariable->WakeOnS5Time.Second;
    RtcMinute = SetupVariable->WakeOnS5Time.Minute;
    RtcHour   = SetupVariable->WakeOnS5Time.Hour;
  }
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_SECONDS_ALARM);
  IoWrite8 (PCAT_RTC_DATA_REGISTER, RtcSecond);

  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_MINUTES_ALARM);
  IoWrite8 (PCAT_RTC_DATA_REGISTER, RtcMinute);

  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_HOURS_ALARM);
  IoWrite8 (PCAT_RTC_DATA_REGISTER, RtcHour);

  //  
  // Clear RTC flag register
  //
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_REGISTER_C);
  IoRead8 (PCAT_RTC_DATA_REGISTER);
  
  //
  // Clear these bits for S5 Wake up everyday. If it's S5 Wake up on day of month,
  // these bits will be updated at the end of the function.
  //
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_REGISTER_D);
  Buffer = 0;
  IoWrite8 (PCAT_RTC_DATA_REGISTER, Buffer);

  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_REGISTER_B);
  Buffer = IoRead8 (PCAT_RTC_DATA_REGISTER);
  Buffer |= ALARM_INTERRUPT_ENABLE;
  IoWrite8 (PCAT_RTC_DATA_REGISTER, Buffer);

  Buffer = IoRead8 (AcpiBaseAddr + PM1_EN_HIGH_BYTE);
  Buffer |= RTC_EVENT_ENABLE;
  IoWrite8 (AcpiBaseAddr + PM1_EN_HIGH_BYTE, Buffer);

  if (SetupVariable->WakeOnS5 == 2) {
    IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_REGISTER_D);
    if (BcdMode){
      Buffer = ConvertSetupTimeToRtcTime(SetupVariable->WakeOnS5DayOfMonth);
    }
    IoWrite8 (PCAT_RTC_DATA_REGISTER, Buffer);
  }

  return;
}