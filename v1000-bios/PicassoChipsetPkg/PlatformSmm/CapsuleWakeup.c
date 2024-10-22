/** @file

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <SmmPlatform.h>
#include <Library/BaseLib.h>
#include <SecureFlash.h>
#include <Library/VariableLib.h>
#include <Library/CmosLib.h>

#include <Protocol/AmdCapsuleSmmHookProtocol.h>

VOID
EnableRtcWakeup (
  IN UINT16 AcpiBaseAddr,
  IN UINT8  WakeAfter
  )
{
  UINT8   Reminder;
  UINT8   RtcSecond;
  UINT8   RtcMinute;
  UINT8   RtcHour;
  UINT8   RtcSts;
  UINT8   RtcEn;
  UINT8   AIE;
  BOOLEAN BcdMode;
  UINT8   Buffer;

  Reminder  = 0;
  RtcSecond = 0;
  RtcMinute = 0;
  RtcHour   = 0;
  RtcSts    = 0;
  RtcEn     = 0;
  AIE       = 0;
  BcdMode   = TRUE;
  Buffer    = 0;

  //
  // Wake time should locate between 1s ~ 15s
  //
  WakeAfter &= 0x0F;
  if (WakeAfter < 1) {
    WakeAfter = 1;
  }

  //
  // Check RTC mode is BCD or Binary
  //
  Buffer  = ReadCmos8 (RTC_ADDRESS_REGISTER_B);
  BcdMode = ~(Buffer >> 2);
  BcdMode &= BIT0;

  //
  // Caculate RTC wake Second/Minute/Hour
  //
  RtcSecond = ReadCmos8 (RTC_ADDRESS_SECONDS);    
  RtcMinute = ReadCmos8 (RTC_ADDRESS_MINUTES);
  RtcHour   = ReadCmos8 (RTC_ADDRESS_HOURS);


  if (BcdMode) {
    RtcSecond = BcdToDecimal8 (RtcSecond);
    RtcMinute = BcdToDecimal8 (RtcMinute);
    RtcHour   = BcdToDecimal8 (RtcHour);
  }
  
  Reminder  = WakeAfter + RtcSecond;
  RtcSecond = Reminder % 60;
  Reminder  = Reminder / 60;
  Reminder  = Reminder + RtcMinute;
  RtcMinute = Reminder % 60;
  Reminder  = Reminder / 60;
  Reminder  = Reminder + RtcHour;
  RtcHour   = Reminder % 24;


  if (BcdMode) {
    RtcSecond = DecimalToBcd8 (RtcSecond);
    RtcMinute = DecimalToBcd8 (RtcMinute);
    RtcHour   = DecimalToBcd8 (RtcHour);
  }

  //
  // Set RTC alarm
  //
  WriteCmos8 (RTC_ADDRESS_SECONDS_ALARM, RtcSecond);
  WriteCmos8 (RTC_ADDRESS_MINUTES_ALARM, RtcMinute);
  WriteCmos8 (RTC_ADDRESS_HOURS_ALARM,   RtcHour);

  //
  // Clear RTC flag register
  //
  ReadCmos8 (RTC_ADDRESS_REGISTER_C);

  //
  // clear RegisterD Bits 0-6 :0 for EveryDay
  //
  IoWrite8 (
    PCAT_RTC_ADDRESS_REGISTER,
    RTC_ADDRESS_REGISTER_D
    );
  
  Buffer = IoRead8 (PCAT_RTC_DATA_REGISTER);    
  Buffer = Buffer & 0x80;
  IoWrite8 (
    PCAT_RTC_DATA_REGISTER,
    Buffer
    );

  //
  // Enable Alarm Interrupt Enable
  //
  AIE = ReadCmos8 (RTC_ADDRESS_REGISTER_B);
  AIE |= ALARM_INTERRUPT_ENABLE;
  WriteCmos8 (RTC_ADDRESS_REGISTER_B, AIE);

  //
  // Clear RTC_STS (PMBase + 0h bit10) and write RTC_EN (PMBase + 2h bit10) to generates a wake event
  //
  RtcSts = IoRead8 (AcpiBaseAddr);
  RtcSts |= 0x400;
  IoWrite8 (AcpiBaseAddr, RtcSts);

  RtcEn = IoRead8 (AcpiBaseAddr + PM1_EN_HIGH_BYTE);
  RtcEn |= RTC_EVENT_ENABLE;
  IoWrite8 (AcpiBaseAddr + PM1_EN_HIGH_BYTE, RtcEn);

  return;

}

/**

 @param [in]   System wakeup time

 @retval None.

**/
VOID
WakeToProcessPendingCapsule (
  IN UINT16           AcpiBaseAddr,
  IN UINT8            WakeAfter
  )
{
  EFI_STATUS                    Status;
  UINTN                         Size;
  IMAGE_INFO                    ImageInfo;
  AMD_CAPSULE_SMM_HOOK_PROTOCOL *AmdCapsuleSmmHookProtocol;

  Status                    = EFI_NOT_FOUND;
  AmdCapsuleSmmHookProtocol = NULL;

  DEBUG ((EFI_D_INFO, "WakeToProcessPendingCapsule Entry\n"));

  //
  // Check any CapsuleUpdate through S3.
  //
  Size = sizeof (IMAGE_INFO);
  Status = CommonGetVariable (
             SECURE_FLASH_INFORMATION_NAME,
             &gSecureFlashInfoGuid,
             &Size,
             &ImageInfo
             );
  DEBUG ((EFI_D_INFO, "  Get SECURE_FLASH_INFORMATION_NAME. (%r)\n", Status));
  if (EFI_ERROR (Status)) {
    return;
  }

  DEBUG ((EFI_D_INFO, "  FlashMode = 0x%x\n", (UINT8)ImageInfo.FlashMode));
  if (!ImageInfo.FlashMode) {
    return;
  }

//[-start-240426-IB18160212-modify-]//
  //
  // Should be enable S3 secure flash flag before flash
  //
  Status = gSmst->SmmLocateProtocol (
                    &gAmdCapsuleSmmHookProtocolGuid,
                    NULL,
                    (VOID**)&AmdCapsuleSmmHookProtocol
                    );
  DEBUG ((EFI_D_INFO, "  AmdCapsuleSmmHookProtocol Status = %r\n", Status));
  if (!EFI_ERROR (Status)) {
    AmdCapsuleSmmHookProtocol->Hook (0);
  }
//[-end-240426-IB18160212-modify-]//

  //
  // Enable RTC wakeup
  //
  EnableRtcWakeup (AcpiBaseAddr, WakeAfter);

  DEBUG ((EFI_D_INFO, "WakeToProcessPendingCapsule Exit\n"));

  return;

}
