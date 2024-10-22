/** @file
  SmmPlatform.h.

;*******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#ifndef _SMM_PLATFORM_H_
#define _SMM_PLATFORM_H_

#include <PiSmm.h>

#include <Protocol/SetupUtility.h>
#include <Protocol/SmmPowerButtonDispatch2.h>
#include <Protocol/SmmSxDispatch2.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/AcpiEnableCallbackDone.h>
#include <Protocol/AcpiDisableCallbackDone.h>
#include <Protocol/AcpiRestoreCallbackDone.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/SmmOemSvcKernelLib.h>
#include <Library/SmmChipsetSvcLib.h>
#include <Library/PciExpressLib.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseOemSvcKernelLib.h>
#include <Library/PcdLib.h>
#include <Library/VariableLib.h>
#include <Library/ProcessPendingCapsuleLib.h>
#include <Library/AmdCbsVariable.h>

#include <SetupConfig.h>
#include <SmiTable.h>
#include <PostCode.h>
#include <FchRegs.h>

#include <Guid/AmdCbsConfig.h>

#define RTC_ADDRESS_SECONDS         0x00
#define RTC_ADDRESS_SECONDS_ALARM   0x01
#define RTC_ADDRESS_MINUTES         0x02
#define RTC_ADDRESS_MINUTES_ALARM   0x03
#define RTC_ADDRESS_HOURS           0x04
#define RTC_ADDRESS_HOURS_ALARM     0x05
#define RTC_ADDRESS_REGISTER_B      0x0B
#define RTC_ADDRESS_REGISTER_C      0x0C
#define RTC_ADDRESS_REGISTER_D      0x0D
#define ALARM_INTERRUPT_ENABLE      0x20
#define PM1_EN_HIGH_BYTE            0x03
#define RTC_EVENT_ENABLE            0x04
#define PCAT_RTC_ADDRESS_REGISTER   0x70
#define PCAT_RTC_DATA_REGISTER      0x71

/**
  Setting for S5 wake up.

  @param [in]   AcpiBaseAddr        ACPI Base Address.
  @param [in]   SetupVariable       SETUP Variable pointer.

  @retval       void.

**/
VOID
S5WakeUpSetting (
  IN UINT16                         AcpiBaseAddr,
  IN SYSTEM_CONFIGURATION          *SetupNVRam
  );

/**
  SaveRestoreCpu.

  @param [in]   SaveRestoreFlag  True: write data to Cpu registers.
                                 False: read data from Cpu registers to global registers.

  @retval EFI_SUCCESS            This routine was handled successfully.

**/
EFI_STATUS
SaveRestoreCpu (
  IN  BOOLEAN                       SaveRestoreFlag
  );
  
/**
  The setting which is for S5WakeOnLan.

  @param                          None.

  @retval void.

**/  
VOID
S5WakeOnLanSetting (
  VOID
  );  

VOID
WakeToProcessPendingCapsule (
  IN UINT16                         AcpiBaseAddr,
  IN UINT8                          WakeAfter
  );

#endif
