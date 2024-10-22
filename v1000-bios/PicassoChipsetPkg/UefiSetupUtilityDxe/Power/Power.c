/** @file

;******************************************************************************
;* Copyright (c) 2015 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "Power.h"

EFI_CALLBACK_INFO                         *mPowerCallBackInfo;

/**
  This is the callback function for the Boot Menu. Dynamically creates the boot order list,
  depending if the network boot option is populated or not.

  @param [in]   *This
  @param [in]   Action            A pointer to the data being sent to the original exporting driver.
  @param [in]   QuestionId
  @param [in]   Type
  @param [in]   *Value
  @param [in]   *ActionRequest

  @retval
**/
EFI_STATUS
PowerCallbackRoutine (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS                            Status;
  CHAR16                                *StringPtr;
  CHIPSET_CONFIGURATION                 *MyIfrNVData;
  EFI_HII_HANDLE                        HiiHandle;
  EFI_CALLBACK_INFO                     *CallbackInfo;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  UINTN                                 BufferSize;
  EFI_GUID                              VarStoreGuid = SYSTEM_CONFIGURATION_GUID;

  if (Action != EFI_BROWSER_ACTION_CHANGED) {
    return PowerCallbackRoutineByAction (This, Action, QuestionId, Type, Value, ActionRequest);
  }

  *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  CallbackInfo = EFI_CALLBACK_INFO_FROM_THIS (This);

  BufferSize = GetVarStoreSize (CallbackInfo->HiiHandle, &CallbackInfo->FormsetGuid, &VarStoreGuid, "SystemConfig");

  Status = SetupVariableConfig (
             &VarStoreGuid,
             L"SystemConfig",
             BufferSize,
             (UINT8 *) gSUBrowser->SCBuffer,
             TRUE
             );
  MyIfrNVData = (CHIPSET_CONFIGURATION *) gSUBrowser->SCBuffer;

  Status = EFI_SUCCESS;
  StringPtr = NULL;
  HiiHandle = CallbackInfo->HiiHandle;
  SUCInfo   = gSUBrowser->SUCInfo;

  switch (QuestionId) {
//     case KEY_MP_SUPPORT:
//       if (MyIfrNVData->MpSupport != ((CHIPSET_CONFIGURATION *) SUCInfo->SCBuffer)->MpSupport) {
//         WriteExtCmos8 (EXT_CMOS_INDEX, EXT_CMOS_DATA, CPU_DUAL_CORE_SWITCH, 0x01);
//       }
// 
//       break;

    default:
      Status = HotKeyCallBack (
                This,
                Action,
                QuestionId,
                Type,
                Value,
                ActionRequest
                );
      break;
  }

  SetupVariableConfig (
    &VarStoreGuid,
    L"SystemConfig",
    BufferSize,
    (UINT8 *) gSUBrowser->SCBuffer,
    FALSE
    );

  return Status;
}

EFI_STATUS
PowerCallbackRoutineByAction (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS                            Status;
  EFI_CALLBACK_INFO                     *CallbackInfo;
  UINTN                                 BufferSize;
  EFI_GUID                              VarStoreGuid = SYSTEM_CONFIGURATION_GUID;

  if ((This == NULL) ||
      ((Value == NULL) &&
       (Action != EFI_BROWSER_ACTION_FORM_OPEN) &&
       (Action != EFI_BROWSER_ACTION_FORM_CLOSE))||
      (ActionRequest == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  CallbackInfo   = EFI_CALLBACK_INFO_FROM_THIS (This);
  BufferSize     = GetVarStoreSize (CallbackInfo->HiiHandle, &CallbackInfo->FormsetGuid, &VarStoreGuid, "SystemConfig");
  Status         = EFI_UNSUPPORTED;

  switch (Action) {

  case EFI_BROWSER_ACTION_FORM_OPEN:
    if (QuestionId == 0) {
      Status = SetupVariableConfig (
                 &VarStoreGuid,
                 L"SystemConfig",
                 BufferSize,
                 (UINT8 *) gSUBrowser->SCBuffer,
                 FALSE
                 );
    }
    break;

  case EFI_BROWSER_ACTION_FORM_CLOSE:
    if (QuestionId == 0) {
      Status = SetupVariableConfig (
                 &VarStoreGuid,
                 L"SystemConfig",
                 BufferSize,
                 (UINT8 *) gSUBrowser->SCBuffer,
                 TRUE
                 );
    }
    break;

  case EFI_BROWSER_ACTION_CHANGING:
    Status = EFI_SUCCESS;
    break;

  case EFI_BROWSER_ACTION_DEFAULT_MANUFACTURING:
    if (QuestionId == KEY_SCAN_F9) {
      Status = HotKeyCallBack (
                 This,
                 Action,
                 QuestionId,
                 Type,
                 Value,
                 ActionRequest
                 );
      SetupVariableConfig (
        &VarStoreGuid,
        L"SystemConfig",
        BufferSize,
        (UINT8 *) gSUBrowser->SCBuffer,
        FALSE
        );
    }
    //
    // avoid GetQuestionDefault execute ExtractConfig
    //
    return EFI_SUCCESS;

  default:
    break;
  }

  return Status;
}

EFI_STATUS
InstallPowerCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle,
  IN EFI_HII_HANDLE                         HiiHandle
  )
{
  EFI_STATUS                                Status;
  EFI_GUID                                  FormsetGuid = FORMSET_ID_GUID_POWER;

  mPowerCallBackInfo = AllocatePool (sizeof(EFI_CALLBACK_INFO));
  if (mPowerCallBackInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  mPowerCallBackInfo->Signature                    = EFI_CALLBACK_INFO_SIGNATURE;
  mPowerCallBackInfo->DriverCallback.ExtractConfig = gSUBrowser->ExtractConfig;
  mPowerCallBackInfo->DriverCallback.RouteConfig   = gSUBrowser->RouteConfig;
  mPowerCallBackInfo->DriverCallback.Callback      = PowerCallbackRoutine;
  mPowerCallBackInfo->HiiHandle                    = HiiHandle;
  CopyGuid (&mPowerCallBackInfo->FormsetGuid, &FormsetGuid);

  //
  // Install protocol interface
  //

  Status = gBS->InstallProtocolInterface (
                  &DriverHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mPowerCallBackInfo->DriverCallback
                  );
  ASSERT_EFI_ERROR (Status);

  Status = InitPowerMenu (HiiHandle);

  return Status;
}

EFI_STATUS
UninstallPowerCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle
  )
{
  EFI_STATUS       Status;
  if (mPowerCallBackInfo == NULL) {
    return EFI_SUCCESS;
  }

  Status = gBS->UninstallProtocolInterface (
                  DriverHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &mPowerCallBackInfo->DriverCallback
                  );

  ASSERT_EFI_ERROR (Status);
  gBS->FreePool (mPowerCallBackInfo);
  mPowerCallBackInfo = NULL;
  return Status;
}

EFI_STATUS
InitPowerMenu (
  IN EFI_HII_HANDLE                         HiiHandle
)
{
  EFI_STATUS                                Status = EFI_SUCCESS;

  return Status;
}
