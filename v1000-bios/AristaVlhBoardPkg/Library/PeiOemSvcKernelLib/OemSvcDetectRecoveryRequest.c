/** @file
  This OemService provides OEM to decide the method of recovery request.
  When DXE loader found that the DXE-core of firmware volume is corrupt,
  it will force system to restart.
  This service will be called to set the recovery requests before system
  restart.
  To design the recovery requests according to OEM specification.

;*******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#include <Ppi/CpuIo.h>
#include <Ppi/Stall.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/RomCorrupt.h>

#include <Library/PeiServicesLib.h>
#include <Library/IoLib.h>
#include <Library/PeiOemSvcKernelLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>

#include <ChipsetSetupConfig.h>
#include <Library/ChipsetConfigLib.h>
//
// Data type definitions
//
#define KBC_DATA_PORT             0x60
#define KBC_CMD_STS_PORT          0x64

#define KBC_NOT_EXIST             0xFF
#define KBC_ENABLED               0x10
#define KBC_OBF                   0x01
#define KBC_IBF                   0x02

#define KBC_CMD_SELF_TEST         0xAA
#define KBC_STS_SELF_TEST_PASS    0x55
#define KBC_CMD_ENABLE_INTERFACE  0xAE
#define KBC_CMD_DISABLE_INTERFACE 0xAD

#define COUNT_TIMEOUT_WIBE        100
#define COUNT_TIMEOUT_WOBF        10000
#define KEY_DATA_BUFFER_COUNT     10

#define SCAN_CODE_MASK            0x7F

#define RECOVERY_REQUEST_KEY      0x13 //Scan code of Ctrl-R

#define BIOS_RAM_INDEX            0xCD4
#define BIOS_RAM_DATA             0xCD5
#define RECOVERY_REQUEST_START    0x80
#define RECOVERY_REQUEST_END      0x88
#define RECOVERY_REQUEST_VALUE    0x44

// #define RECOVERY_CTRL_R_SUPPORT
#define RECOVERY_REQUEST_SUPPORT
#define RECOVERY_EC_DETECT_SUPPORT

#ifndef LEGACY_FREE_SUPPORT

/**
  To wait KBC input buffer empty.

  @param [in]  **PeiServices         PEI Services pointer.
  @param [in]  *CpuIo                CpuIo service pointer.

  @retval      EFI_SUCCESS           Input Buffer empty.
  @retval      EFI_TIMEOUT           Timeout.
**/
EFI_STATUS
WaitInputBufferEmpty (
  VOID
  )
{
  UINT32    Count;
  UINT8     Value8;

  for (Count = 0; Count < COUNT_TIMEOUT_WIBE; Count++) {
    Value8 = IoRead8 (KBC_CMD_STS_PORT);
    if ((Value8 & KBC_IBF) != KBC_IBF) {
      break;
    }
  }

  if (Count >= COUNT_TIMEOUT_WIBE) {
    return EFI_TIMEOUT;
  }
  return EFI_SUCCESS;
}

/**
  To wait KBC output buffer full.

  @param [in]  **PeiServices         PEI Services pointer.
  @param [in]  *CpuIo                CpuIo service pointer.

  @retval      EFI_SUCCESS           Output Buffer Full.
  @retval      EFI_TIMEOUT           Timeout.

**/
EFI_STATUS
WaitOutputBufferFull (
  VOID
  )
{
  UINT32    Count;
  UINT8     Value8;

  for (Count = 0; Count < COUNT_TIMEOUT_WOBF; Count++) {
    Value8 = IoRead8 (KBC_CMD_STS_PORT);
    if ((Value8 & KBC_OBF) == KBC_OBF) {
      break;
    }
  }

  if (Count >= COUNT_TIMEOUT_WOBF) {
    return EFI_TIMEOUT;
  }
  return EFI_SUCCESS;
}
#endif

/**
  To check if there is any Recovery request happened.

  @param [in, out]  *IsRecovery Recovery request flag, if it is TRUE,
                                recovery request is detected, or the
                                system is on normal boot.
**/
VOID
CheckHotKeyRecoveryRequest (
  IN OUT BOOLEAN                           *IsRecovery
  )
{
  EFI_STATUS                                    Status;
  UINT8                                         Value8;
  UINT8                                         KeyDataBuffer [KEY_DATA_BUFFER_COUNT];
  UINT32                                        Count;

  Status = EFI_SUCCESS;
  //
  // 0. Initial Local variable
  //
  Value8 = 0;
  Count = 0;
  for (Count = 0; Count < KEY_DATA_BUFFER_COUNT; Count++) {
    KeyDataBuffer [Count] = 0;
  }
#ifndef LEGACY_FREE_SUPPORT

    Status = WaitInputBufferEmpty ();
    if (EFI_ERROR (Status)) {
      return;
    }
    IoWrite8 (KBC_CMD_STS_PORT, KBC_CMD_SELF_TEST);
    Status = WaitOutputBufferFull ();
    if (EFI_ERROR (Status)) {
      return;
    }
#endif


  Value8 = IoRead8 (KBC_DATA_PORT);
  if (Value8 != KBC_STS_SELF_TEST_PASS) {
    return;
  }
#if !defined (LEGACY_FREE_SUPPORT) && defined (RECOVERY_CTRL_R_SUPPORT)

    //
    // 1. Check KBC exist or not
    //
    Value8 = IoRead8 (KBC_CMD_STS_PORT);
    if (Value8 == KBC_NOT_EXIST) {
      return;
    }
    if ((Value8 & KBC_ENABLED) != KBC_ENABLED) {
      return;
    }
    //
    // 2. KBC exist, Send KBC self-test command, make sure KBC works properly
    //
    Status = WaitInputBufferEmpty ();
    if (EFI_ERROR (Status)) {
      return;
    }
    IoWrite8 (KBC_CMD_STS_PORT, KBC_CMD_SELF_TEST);
    Status = WaitOutputBufferFull ();
    if (EFI_ERROR (Status)) {
      return;
    }
    Value8 = IoRead8 (KBC_DATA_PORT);
    if (Value8 != KBC_STS_SELF_TEST_PASS) {
      return;
    }
    //
    // 3. KBC Works properly, Enable Keyboard interface.
    //
    Status = WaitInputBufferEmpty ();
    if (EFI_ERROR (Status)) {
      return;
    }
    IoWrite8 (KBC_CMD_STS_PORT, KBC_CMD_ENABLE_INTERFACE);
    //
    // 4. Keyboard interface enabled, Try to get Press key data from KBC
    //
    for (Count = 0; Count < KEY_DATA_BUFFER_COUNT; Count++) {
      Status = WaitOutputBufferFull ();
      if (Status == EFI_TIMEOUT) break;
      KeyDataBuffer [Count] = IoRead8 (KBC_DATA_PORT);
    }
    //
    // 5. Already get the key, Disable Keyboard interface
    //
    Status = WaitInputBufferEmpty ();
    if (EFI_ERROR (Status)) {
      return;
    }
    IoWrite8 (KBC_CMD_STS_PORT, KBC_CMD_DISABLE_INTERFACE);
    //
    // 6. Check key value
    //
    for (Count = 0; Count < KEY_DATA_BUFFER_COUNT; Count++) {
      if (KeyDataBuffer [Count] == 0) break;
      if ((KeyDataBuffer [Count] & SCAN_CODE_MASK) == RECOVERY_REQUEST_KEY) {
        *IsRecovery = TRUE;
        break;
      }
    }
#endif


#if !defined (LEGACY_FREE_SUPPORT) && defined (RECOVERY_EC_DETECT_SUPPORT)
    if (!(*IsRecovery)) { //Press Left CTRL + Left ALT + Z at G3 and then release these key at S5.
      Status = WaitInputBufferEmpty ();
      if (EFI_ERROR (Status)) {
        return;
      }
      IoWrite8 (KBC_CMD_STS_PORT, 0xA3);
      Status = WaitOutputBufferFull ();
      if (EFI_ERROR (Status)) {
        return;
      }

      Value8 = IoRead8 (KBC_DATA_PORT);
      if (Value8 == 0xFA) {
        *IsRecovery = TRUE;
      } else {
        *IsRecovery = FALSE;
      }
    }
#endif

  return ;
}
VOID
CheckRomPartCrashOrNot (

  IN OUT BOOLEAN                           *IsRecovery
)
/*++
Routine Description:
  Check is there any Rom Data Crash or not?
Arguments:
  PeiServices - Pei Services pointer
  IsRecovery  - Recovery request flag
Returns:
  IsRecovery is TRUE, detect recovery request
  IsRecovery is FALSE, normal boot
--*/
{

#if defined (RECOVERY_REQUEST_SUPPORT)
    VOID                   *GuidHob;

    if (!(*IsRecovery)) {
      UINT8                 Index;
      //
      // If ROM data incorrect has been detected at last boot,
      // BIOS_RAM RECOVERY_REQUEST_START~RECOVERY_REQUEST_END will all been fill RECOVERY_REQUEST_VALUE
      // This is for ATI SB only
      //
      for (Index = RECOVERY_REQUEST_START; Index <= RECOVERY_REQUEST_END; Index++) {
        IoWrite8 (BIOS_RAM_INDEX, Index);
        if (IoRead8 (BIOS_RAM_DATA) != RECOVERY_REQUEST_VALUE) {
          break ;
        }
      }
      if (Index > RECOVERY_REQUEST_END) {
        *IsRecovery = TRUE;
        //
        // Boot from ROM corrupt has been detected,
        // Fill 0x0 to BIOS_RAM RECOVERY_REQUEST_START~RECOVERY_REQUEST_END
        // This is for ATI SB only
        //
        for (Index = RECOVERY_REQUEST_START; Index <= RECOVERY_REQUEST_END; Index++) {
          IoWrite8 (BIOS_RAM_INDEX, Index);
          IoWrite8 (BIOS_RAM_DATA, 0);
        }
        //
        // Build guid hob as tag after first call.
        //
        BuildGuidHob (
          &gPeiRomCorruptPpiGuid,
          0
          );
      }
    }

    if (!(*IsRecovery)) {
      GuidHob = GetFirstGuidHob (&gPeiRomCorruptPpiGuid);
      if (GuidHob != NULL) {
        *IsRecovery = TRUE;
      }
    }
#endif
}

/*
  This function checks the EXT CMOS for the recovery mode

*/
VOID
CheckCMOSRecoveryFlag (
  IN OUT BOOLEAN                           *IsRecovery
)
{
  UINT8                                RecoveryFlag0, RecoveryFlag1;

  IoWrite8 (0x72, 0xA4);
  RecoveryFlag0 = IoRead8 (0x73);
  IoWrite8 (0x72, 0xA5);
  RecoveryFlag1 = IoRead8 (0x73);
  if (RecoveryFlag0 == 0x55 && RecoveryFlag1 == 0xAA) {
    *IsRecovery = TRUE;
//[-start-191001-IB06070053-remove]//
//    IoWrite8 (0x72, 0xA4);
//    IoWrite8 (0x73, 0x0);
//    IoWrite8 (0x72, 0xA5);
//    IoWrite8 (0x73, 0x0);
//[-end-191001-IB06070053-remove]//
    BuildGuidHob (&gPeiRomCorruptPpiGuid, 0);
  }
}

/**
  This OemService provides OEM to detect the recovery mode.
  OEM designs the rule to detect that boot mode is recovery mode or not,
  The rule bases on the recovery request which defined in OemService "OemSetRecoveryRequest".

  @param [in, out]  *IsRecovery        If service detects that the current system is recovery mode.
                                       This parameter will return TRUE, else return FALSE.

  @retval           EFI_MEDIA_CHANGED  The Recovery is detected. (*IsRecovery is set)
**/
EFI_STATUS
OemSvcDetectRecoveryRequest (
  IN OUT BOOLEAN                        *IsRecovery
  )
{
  EFI_STATUS                            GetSetupVariableStatus;
  CHIPSET_CONFIGURATION                 ChipsetConfiguration;

  GetSetupVariableStatus = GetChipsetConfiguration (&ChipsetConfiguration);

  if (!EFI_ERROR (GetSetupVariableStatus)) {
    *IsRecovery = ChipsetConfiguration.FastRecoverySwitch ? TRUE : FALSE;
  }

  if (!(*IsRecovery)) {
    if (ChipsetConfiguration.PeiGop == 1 && PcdGetBool(PcdUseFastCrisisRecovery)) {
      CheckCMOSRecoveryFlag(IsRecovery);
    }
    CheckRomPartCrashOrNot (IsRecovery);
  }

  return EFI_MEDIA_CHANGED;
}
