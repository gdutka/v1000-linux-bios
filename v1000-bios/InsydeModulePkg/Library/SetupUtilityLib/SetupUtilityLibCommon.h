/** @file
  Header file for Setup Utility Library common use

;******************************************************************************
;* Copyright (c) 2012 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SETUP_UTILITY_LIB_COMMON_H_
#define _SETUP_UTILITY_LIB_COMMON_H_

#include <Library/SetupUtilityLib.h>
#include <OemServices/Kernel.h>
#include <Library/BvdtLib.h>
#include <Library/VariableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/IdeControllerInit.h>
#include <Protocol/SetupUtility.h>
#include <Protocol/FormBrowser2.h>
#include <Protocol/H2OFormBrowser.h>
#include <Guid/DebugMask.h>

#define PRINTABLE_LANGUAGE_NAME_STRING_ID     0x0001


typedef struct _BOOT_DEV_INFO {
  BOOLEAN                                   IsEfiBootDev;
  UINT16                                    BootOptionNum;
  CHAR16                                    *pString;
  BBS_TABLE                                 *BbsEntry;
  EFI_DEVICE_PATH_PROTOCOL                  *DevicePath;
  UINT16                                    DevType;

  //
  // Settings in BOOT_CONFIGURATION
  //
  BOOLEAN                                   IsActive;
} BOOT_DEV_INFO;

typedef struct {
  UINT8                                     DeviceType;
  EFI_STRING_ID                             StrToken;
  UINTN                                     DevInfoCount;
  UINT16                                    CurrentIndex;
  UINT16                                    KeyBootDeviceBase;
  UINT16                                    BootDevicesLabel;
  EFI_FORM_ID                               FormId;
  BOOT_DEV_INFO                             *DevInfoList;
} BBS_TYPE_TABLE;

//
// The following functioons are used in library internal
//
EFI_STATUS
GetSetupUtilityBrowserData (
  OUT SETUP_UTILITY_BROWSER_DATA   **SuBrowser
  );

EFI_STATUS
UpdatePasswordState (
  EFI_HII_HANDLE                        HiiHandle
  );

CHAR16 *
SetupUtilityLibGetTokenStringByLanguage (
  IN EFI_HII_HANDLE                             HiiHandle,
  IN EFI_STRING_ID                              Token,
  IN CHAR8                                      *LanguageString
  );

EFI_STATUS
SetupUtilityLibUpdateDeviceString (
  IN     EFI_HII_HANDLE                         HiiHandle,
  IN OUT KERNEL_CONFIGURATION                   *KernelConfig
  );

typedef struct {
  UINTN    LangNum;
  UINT8    LangString[1];
  } LANGUAGE_DATA_BASE;

EFI_STATUS
SetupUtilityLibAsciiToUnicode (
  IN    CHAR8     *AsciiString,
  IN    CHAR16    *UnicodeString,
  IN    UINTN     Length
  );

EFI_STATUS
SetupUtilityLibUpdateAtaString(
  IN      EFI_IDENTIFY_DATA                 *IdentifyDriveInfo,
  IN      UINTN                             BufSize,
  IN OUT  CHAR16                            **BootString
  );

/**
 Send goto hot key event.

 @param[in] FormId               Target form ID
 @param[in] QuestionId           Target question ID

 @retval EFI_SUCCESS             Send goto hot key event successfully.
 @retval Other                   Locate form browser protocol failed or notify event failed.
**/
EFI_STATUS
SendGotoHotKeyNotify (
  IN UINT16                               FormId,
  IN UINT16                               QuestionId
  );

EFI_STATUS
SendShutDNotifyForInternal (
  VOID
  );

EFI_STATUS
ChangeToAutoBootOrder (
  IN     UINTN                              DeviceCount,
  IN OUT UINT16                             *BootOrder
  );

EFI_STATUS
BootConfigLoadDefault (
  VOID
  );

BOOLEAN
BootDevInfoGetActiveValue (
  IN UINT16                                 BootOptionNum
  );

EFI_STATUS
BootDevInfoSetActiveValue (
  IN UINT16                                 BootOptionNum,
  IN BOOLEAN                                IsActive
  );

EFI_STATUS
BootLoadDefault (
  VOID
  );

EFI_STATUS
BootSaveSetting (
  VOID
  );

EFI_STATUS
SecuritySaveSetting (
  VOID
  );

VOID
SyncBootOrderToBootConfig (
  VOID
  );

/**
  Load the default value of security page.

  @retval EFI_SUCCESS     Load the default value successfully.
  @retval Others          Fail to load the default value.

**/
EFI_STATUS
SecurityLoadDefault (
  VOID
  );

#endif
