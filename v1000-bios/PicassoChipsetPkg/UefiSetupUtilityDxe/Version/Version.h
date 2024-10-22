/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _VERSION_CALLBACK_H
#define _VERSION_CALLBACK_H

#include "SetupUtility.h"

#define EFI_VERSION_MENU_SIGNATURE SIGNATURE_32('V','E','R','S')
#define EFI_VERSION_CALLBACK_INFO_FROM_THIS(a) CR (a, EFI_CALLBACK_INFO, DriverCallback, EFI_VERSION_MENU_SIGNATURE)

EFI_STATUS
VersionCallbackRoutine (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  );
  
EFI_STATUS
InstallVersionCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle,
  IN EFI_HII_HANDLE                         HiiHandle
  );

EFI_STATUS
UninstallVersionCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle
  );

EFI_STATUS
InitVersionMenu (
  IN EFI_HII_HANDLE                         HiiHandle
  );
#endif
