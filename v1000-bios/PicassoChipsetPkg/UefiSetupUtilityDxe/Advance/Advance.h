/** @file

;******************************************************************************
;* Copyright (c) 2013 - 2015, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _ADVANCE_CALLBACK_H
#define _ADVANCE_CALLBACK_H

#include "SetupUtility.h"
#include "DisplaySelection.h"


extern  IDE_CONFIG                      *mIdeConfig;
extern ADVANCE_CONFIGURATION            mAdvConfig;

EFI_STATUS
AdvanceCallbackRoutine (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  );
  
EFI_STATUS
AdvanceCallbackRoutineByAction (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  );

EFI_STATUS
InstallAdvanceCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle,
  IN EFI_HII_HANDLE                         HiiHandle
  );

EFI_STATUS
UninstallAdvanceCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle
  );

EFI_STATUS
InitAdvanceMenu (
  IN EFI_HII_HANDLE                         HiiHandle
  );
EFI_STATUS
UpdateHDCConfigure (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  CHIPSET_CONFIGURATION             *Buffer
  );
  
EFI_STATUS
GetIdeDevNameString (
  IN OUT CHAR16 *                        *DevNameString
  );
EFI_STATUS  
InitIdeConfig (
  IDE_CONFIG                             *IdeConfig
  );

#endif
