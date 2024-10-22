/** @file
  Header file for BDS hot key.

;******************************************************************************
;* Copyright (c) 2016 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _H2O_BDS_HOT_KEY_H_
#define _H2O_BDS_HOT_KEY_H_

#include "Bds.h"
#include "String.h"
#include <Guid/BdsHotKey.h>
#include <Guid/HotKeyEvent.h>
#include <Protocol/DevicePathFromText.h>
#include <Protocol/H2OBdsHotKey.h>
#include <Protocol/H2OKeyDesc.h>
#include <Library/H2OCpLib.h>
#include <Library/SortLib.h>
#include <Library/RectLib.h>

#define TRIGGER_POINT_BEFORE_OS_INDICATIONS          BIT0
#define TRIGGER_POINT_BEFORE_BOOT_NEXT               BIT1
#define TRIGGER_POINT_BEFORE_CHECK_EACH_BOOT_OPTION  BIT2

#define H2O_HOT_KEY_CHORD_SEPARATOR_ASCII_STRING     ";"
#define H2O_HOT_KEY_CHORD_SEPARATOR_STRING           L";"
#define H2O_HOT_KEY_KEY_SEPARATOR_STRING             L","

#define TIMER_EVENT_ONE_MILLISEC                     10000

#define SET_BOOT_OPTION_SUPPORT_KEY_COUNT(a, c) {  \
          (a) = ((a) & ~EFI_BOOT_OPTION_SUPPORT_COUNT) | (((c) << LowBitSet32 (EFI_BOOT_OPTION_SUPPORT_COUNT)) & EFI_BOOT_OPTION_SUPPORT_COUNT); \
          }

typedef struct _H2O_HOT_KEY_CHORD {
  EFI_KEY_DATA                              *KeyList;
  UINT32                                    KeyCount;
  UINT32                                    CurrentIndex;
  CHAR8                                     *ChordAsciiStr;
} H2O_HOT_KEY_CHORD;

typedef enum {
  UEFI_HOT_KEY,
  PCD_HOT_KEY,
  REGISTERED_CALLBACK_HOT_KEY,
  REGISTERED_LOAD_OPTION_HOT_KEY,
  H2O_HOT_KEY_TYPE_MAX
} H2O_HOT_KEY_TYPE;

typedef union {
  EFI_GUID                                  HotKeyProtocolGuid;
  UINT16                                    BootOptionNum;
  H2O_BDS_HOT_KEY_LAUNCH                    HotKeyFn;
  H2O_BDS_LOAD_OPTION                       *LoadOption;
} H2O_HOT_KEY_TYPE_VALUE;

typedef struct _H2O_HOT_KEY_DESCRIPTION {
  //
  // Text
  //
  EFI_STRING_ID                             BeforeDescStrId;
  EFI_STRING_ID                             AfterDescStrId;

  //
  // Color
  //
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL             ForegroundColor;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL             BackgroundColor;

  //
  // Font
  //
  CHAR8                                     *FontName;
  UINT16                                    FontSize;
  EFI_HII_FONT_STYLE                        FontStyle;

  //
  // Position
  //
  INT32                                     X;
  UINT32                                    XUnit;
  UINT32                                    XOrigin;
  INT32                                     Y;
  UINT32                                    YUnit;
  UINT32                                    YOrigin;

  //
  // Internal usage
  //
  RECT                                      DisplayRegion;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL             *OrgBltBuffer;
} H2O_HOT_KEY_DESCRIPTION;

#define H2O_BDS_HOT_KEY_SIGNATURE SIGNATURE_32 ('H', 'B', 'H', 'K')

typedef struct _H2O_HOT_KEY_OPTION {
  UINTN                                     Signature;
  LIST_ENTRY                                Link;

  UINT8                                     Type;
  H2O_HOT_KEY_TYPE_VALUE                    TypeValue;

  UINTN                                     PcdTokenNum;
  UINT32                                    Id;
  UINT32                                    HotKeyChordCount;
  H2O_HOT_KEY_CHORD                         *HotKeyChordList;
  CHAR8                                     *Options;
  EFI_DEVICE_PATH_PROTOCOL                  *DevicePath;

  H2O_HOT_KEY_DESCRIPTION                   Description;
  H2O_HOT_KEY_DESCRIPTION                   QuietDescription;
} H2O_HOT_KEY_OPTION;

#define H2O_HOT_KEY_OPTION_FROM_LINK(a) CR (a, H2O_HOT_KEY_OPTION, Link, H2O_BDS_HOT_KEY_SIGNATURE)

EFI_STATUS
BdsHotKeyInit (
  VOID
  );

EFI_STATUS
BdsHotKeyBoot (
  IN UINT32                                 TriggerPoint
  );

BOOLEAN
EnableBdsHotKey (
  IN BOOLEAN                           Enable
  );

EFI_STATUS
EFIAPI
BdsServicesEnableHotKeys (
  IN     H2O_BDS_SERVICES_PROTOCOL          *This,
  IN OUT BOOLEAN                            *Enable
  );

EFI_STATUS
EFIAPI
BdsServicesRegisterCallbackHotKey (
  IN  H2O_BDS_SERVICES_PROTOCOL             *This,
  IN  CONST CHAR8                           *HotKey,
  IN  H2O_BDS_HOT_KEY_LAUNCH                HotKeyFn,
  IN  UINT32                                Id,
  IN  CONST CHAR8                           *Options OPTIONAL,
  OUT EFI_HANDLE                            *HotKeyHandle
  );

EFI_STATUS
EFIAPI
BdsServicesRegisterLoadOptionHotKey (
  IN  H2O_BDS_SERVICES_PROTOCOL             *This,
  IN  CONST CHAR8                           *HotKey,
  IN  CONST H2O_BDS_LOAD_OPTION             *HotKeyLoadOption,
  OUT EFI_HANDLE                            *HotKeyHandle
  );

EFI_STATUS
EFIAPI
BdsServicesUnregisterHotKey (
  IN H2O_BDS_SERVICES_PROTOCOL              *This,
  IN EFI_HANDLE                             HotKeyHandle
  );

EFI_STATUS
GetTokenNumList (
  IN  CONST GUID                            *TokenSpaceGuid,
  OUT UINTN                                 **TokenNumList,
  OUT UINTN                                 *TokenNumCount
  );

H2O_HOT_KEY_OPTION *
GetHotKeyOption (
  IN EFI_HANDLE                             HotKeyHandle
  );

EFI_DEVICE_PATH_PROTOCOL *
GetDevicePathByAsciiStr (
  IN CHAR8                                  *DevicePathAsciiStr
  );

EFI_DEVICE_PATH_PROTOCOL *
GetDevicePathByStr (
  IN CHAR16                                 *DevicePathStr
  );

extern LIST_ENTRY                           mBdsHotKeyList;
extern H2O_HOT_KEY_OPTION                   *mSelectedHotKeyOption;
#endif
