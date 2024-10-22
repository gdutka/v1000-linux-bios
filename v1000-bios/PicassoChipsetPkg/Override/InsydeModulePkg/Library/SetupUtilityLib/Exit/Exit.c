/** @file
  Initial and callback functions for Exit page of SetupUtilityLib

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

#include "Exit.h"

STATIC EFI_CALLBACK_INFO                            *mExitCallBackInfo;


/**
  For all form set including "link" page, save or load custom default variable.

  @param Save                    TRUE is save custom default, FALSE is load custom default.
  @param DefaultId               It is load custom default

  @retval EFI_SUCCESS            The callback successfully handled the action.
  @retval EFI_OUT_OF_RESOURCES   Not enough storage is available to hold the variable and its data.

**/
EFI_STATUS
SaveLoadCustomDefault (
  IN BOOLEAN                                   Save,
  IN UINT16                                    DefaultId
  )
{
  EFI_STATUS                                Status;

  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  H2O_FORM_BROWSER_PROTOCOL                 *FBProtocol;

  UINTN                                     PageIdx;
  UINTN                                     CheckPageIdx;
  UINT32                                    PageIdCount;
  H2O_PAGE_ID                               *PageIdList;
  H2O_PAGE_ID                               PageId;
  BOOLEAN                                   FoundSamePage;

  EFI_VARSTORE_ID                           VarStoreId;
  UINTN                                     VarStoreIdIdx;
  UINTN                                     VarStoreIdCount;
  EFI_VARSTORE_ID                           *VarStoreIdList;

  H2O_FORM_BROWSER_VS                       *VarStoreData;
  CHAR16                                    *CustomDefaultVarName;

  UINT8                                     *VarBuffer;
  UINTN                                     VarBufferSize;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (
                   &gH2OFormBrowserProtocolGuid,
                   NULL,
                   (VOID **)&FBProtocol
                   );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = FBProtocol->GetPAll (FBProtocol, &PageIdCount, &PageIdList);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (PageIdx = 0; PageIdx < PageIdCount; PageIdx++) {
    PageId = PageIdList[PageIdx];

    //
    // skip the same formset
    //
    FoundSamePage = FALSE;
    for (CheckPageIdx = 1; CheckPageIdx < PageIdx; CheckPageIdx++) {
      if ((PageIdList[CheckPageIdx] >> 16) == (PageId >> 16)) {
        FoundSamePage = TRUE;
        break;
      }
    }
    if (FoundSamePage) {
      continue;
    }

    Status = FBProtocol->GetVSAll (FBProtocol, PageId, &VarStoreIdCount, &VarStoreIdList);
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      continue;
    }

    for (VarStoreIdIdx = 0; VarStoreIdIdx < VarStoreIdCount; VarStoreIdIdx++) {
      VarStoreData = NULL;
      VarStoreId = VarStoreIdList[VarStoreIdIdx];
      Status = FBProtocol->GetVSInfo (FBProtocol, PageId, VarStoreId, &VarStoreData);
      ASSERT_EFI_ERROR (Status);
      if (EFI_ERROR (Status)) {
        continue;
      }
      //
      // skip name value
      //
      if (VarStoreData->Name == NULL) {
        FreePool (VarStoreData);
        continue;
      }

      CustomDefaultVarName = CatSPrint (NULL, L"%s%04x", VarStoreData->Name, DefaultId);
      if (CustomDefaultVarName == NULL) {
        FreePool (VarStoreData);
        FreePool (VarStoreIdList);
        FreePool (PageIdList);
        return EFI_OUT_OF_RESOURCES;
      }

      if (Save) {
        VarBufferSize = VarStoreData->StorageSize;
        VarBuffer     = VarStoreData->EditBuffer;

        //
        // Avoid Setup variable isn't same in multiple page, use SCBuffer directly.
        //
        if (StrCmp (VarStoreData->Name, L"SystemConfig") == 0) {
          VarBufferSize = GetSetupVariableSize ();
          VarBuffer     = SuBrowser->SCBuffer;
        }

        Status = gRT->SetVariable (
                        CustomDefaultVarName,
                        &VarStoreData->Guid,
                        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                        VarBufferSize,
                        VarBuffer
                        );
        ASSERT_EFI_ERROR (Status);
      } else {
        VarBuffer     = NULL;
        VarBufferSize = 0;
        Status = CommonGetVariableDataAndSize (
                   CustomDefaultVarName,
                   &VarStoreData->Guid,
                   &VarBufferSize,
                   (VOID **)&VarBuffer
                   );
        if (!EFI_ERROR (Status)) {
          if (VarStoreData->StorageSize < VarBufferSize) {
            VarBufferSize = VarStoreData->StorageSize;
          }
          CopyMem (VarStoreData->EditBuffer, VarBuffer, VarBufferSize);
          FreePool (VarBuffer);
        }
      }
      DEBUG ((EFI_D_INFO, "%g %s\n", &VarStoreData->Guid, VarStoreData->Name));
      FreePool (CustomDefaultVarName);
      FreePool (VarStoreData);
    }
    FreePool (VarStoreIdList);
  }
  FreePool (PageIdList);

  return EFI_SUCCESS;
}



/**
  This function is called by the forms browser in response to a user action on a question which has the
  EFI_IFR_FLAG_CALLBACK bit set in the EFI_IFR_QUESTION_HEADER. The user action is specified by Action.
  Depending on the action, the browser may also pass the question value using Type and Value. Upon return,
  the callback function may specify the desired browser action. Callback functions should return
  EFI_UNSUPPORTEDfor all values of Action that they do not support.

  @param This                    Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Action                  Specifies the type of action taken by the browser.
  @param QuestionId              A unique value which is sent to the original exporting driver so that it can identify the
                                 type of data to expect. The format of the data tends to vary based on the opcode that
                                 generated the callback.
  @param Type                    The type of value for the question.
  @param Value                   A pointer to the data being sent to the original exporting driver. The type is specified
                                 by Type. Type EFI_IFR_TYPE_VALUE is defined in EFI_IFR_ONE_OF_OPTION.
  @param ActionRequest           On return, points to the action requested by the callback function. Type
                                 EFI_BROWSER_ACTION_REQUEST is specified in SendForm() in the Form Browser Protocol.

  @retval EFI_SUCCESS            The callback successfully handled the action.
  @retval EFI_OUT_OF_RESOURCES   Not enough storage is available to hold the variable and its data.
  @retval EFI_DEVICE_ERROR       The variable could not be saved.
  @return EFI_UNSUPPORTED        The specified Action is not supported by the callback.

**/
EFI_STATUS
EFIAPI
ExitCallbackRoutine (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS                                Status;
  CHAR16                                    *StringPtr;
  KERNEL_CONFIGURATION                      *MyIfrNVData;
  EFI_HII_HANDLE                            HiiHandle;
  EFI_INPUT_KEY                             Key;
  EFI_CALLBACK_INFO                         *CallbackInfo;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL        *Interface;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  UINTN                                     BufferSize;
  EFI_GUID                                  VarStoreGuid = SYSTEM_CONFIGURATION_GUID;

  if (Action != EFI_BROWSER_ACTION_CHANGED) {
    return ExitCallbackRoutineByAction (This, Action, QuestionId, Type, Value, ActionRequest);
  }

  *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  CallbackInfo   = EFI_CALLBACK_INFO_FROM_THIS (This);

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  BufferSize  = GetVarStoreSize (CallbackInfo->HiiHandle, &CallbackInfo->FormsetGuid, &VarStoreGuid, "SystemConfig");

  SUCInfo                 = SuBrowser->SUCInfo;
  Status                  = EFI_SUCCESS;
  StringPtr               = NULL;
  HiiHandle               = CallbackInfo->HiiHandle;

  Status = SetupVariableConfig (
             &VarStoreGuid,
             L"SystemConfig",
             BufferSize,
             (UINT8 *) SuBrowser->SCBuffer,
             TRUE
             );

  Interface               = &SuBrowser->Interface;
  Interface->MyIfrNVData  = (UINT8 *) SuBrowser->SCBuffer;
  MyIfrNVData = (KERNEL_CONFIGURATION *) SuBrowser->SCBuffer;

  switch (QuestionId) {

  case KEY_SAVE_EXIT:
    Status = SuBrowser->HotKeyCallback (
                          This,
                          Action,
                          KEY_SCAN_F10,
                          Type,
                          Value,
                          ActionRequest
                          );
    break;

  case KEY_EXIT_DISCARD:
    //
    // Discard setup and exit
    //
    Status = SuBrowser->HotKeyCallback (
                          This,
                          Action,
                          KEY_SCAN_ESC,
                          Type,
                          Value,
                          ActionRequest
                          );
    break;

  case KEY_LOAD_OPTIMAL:
    //
    //Select "Load Optimal Defaults" and Press "Load Optimal hotkey", the functionality is the same,
    //so remove the original code Use "Load Optimal hotkey" to call HotKeyCallBack () directly
    //
    Status = SuBrowser->HotKeyCallback (
                          This,
                          Action,
                          KEY_SCAN_F9,
                          Type,
                          Value,
                          ActionRequest
                          );
    break;

  case KEY_LOAD_CUSTOM:
    //
    // Load custom setup.
    //
    if ((MyIfrNVData->SetUserPass == TRUE) &&
        ((MyIfrNVData->UserAccessLevel == 2) ||
         (MyIfrNVData->UserAccessLevel == 3))) {
      return EFI_UNSUPPORTED;
    }

    StringPtr = HiiGetString (
                  HiiHandle,
                  STRING_TOKEN (STR_LOAD_CUSTOM_DEFAULTS_STRING),
                  NULL
                  );
    SuBrowser->H2ODialog->ConfirmDialog (
                              0,
                              FALSE,
                              0,
                              NULL,
                              &Key,
                              StringPtr
                              );
    if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      Status = LoadCustomOption (This);
      if (FeaturePcdGet (PcdH2OCustomDefaultSupported)) {
        Status = SaveLoadCustomDefault (FALSE, 0xC001);
      }
      if (!EFI_ERROR (Status)) {
        SUCInfo->DoRefresh = TRUE;
        BrowserRefreshFormSet ();
        Status = SuBrowser->HotKeyCallback (
                              This,
                              Action,
                              KEY_LOAD_CUSTOM,
                              Type,
                              Value,
                              ActionRequest
                              );
      }
    } else {
      Status = EFI_UNSUPPORTED;
    }

    FreePool (StringPtr);
    break;

  case KEY_SAVE_CUSTOM:
    //
    // Save custom setup.
    //
    StringPtr = HiiGetString (
                  HiiHandle,
                  STRING_TOKEN (STR_SAVE_CUSTOM_DEFAULTS_STRING),
                  NULL
                  );
    SuBrowser->H2ODialog->ConfirmDialog (
                              0,
                              FALSE,
                              0,
                              NULL,
                              &Key,
                              StringPtr
                              );
    if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      if (FeaturePcdGet (PcdH2OCustomDefaultSupported)) {
        Status = SaveLoadCustomDefault (TRUE, 0xC001);
      }
      Status = SaveCustomOption (This);
      if (!EFI_ERROR (Status)) {
        Status = SuBrowser->HotKeyCallback (
                              This,
                              Action,
                              KEY_SAVE_CUSTOM,
                              Type,
                              Value,
                              ActionRequest
                              );
      }
    } else {
      Status = EFI_UNSUPPORTED;
    }

    FreePool (StringPtr);
    break;

  default:
    Status = SuBrowser->HotKeyCallback (
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
    (UINT8 *) SuBrowser->SCBuffer,
    FALSE
    );

  return Status;
}

EFI_STATUS
ExitCallbackRoutineByAction (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS                                Status;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  EFI_CALLBACK_INFO                         *CallbackInfo;
  UINTN                                     BufferSize;
  EFI_GUID                                  VarStoreGuid = SYSTEM_CONFIGURATION_GUID;

  if ((This == NULL) ||
      ((Value == NULL) &&
       (Action != EFI_BROWSER_ACTION_FORM_OPEN) &&
       (Action != EFI_BROWSER_ACTION_FORM_CLOSE))||
      (ActionRequest == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
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
                 (UINT8 *) SuBrowser->SCBuffer,
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
                 (UINT8 *) SuBrowser->SCBuffer,
                 TRUE
                 );
    }
    break;

  case EFI_BROWSER_ACTION_CHANGING:
    Status = EFI_SUCCESS;
    break;

  case EFI_BROWSER_ACTION_DEFAULT_MANUFACTURING:
    if (QuestionId == KEY_SCAN_F9) {
      Status = SuBrowser->HotKeyCallback (
                            This,
                            Action,
                            QuestionId,
                            Type,
                            Value,
                            ActionRequest
                            );
      Status = SetupVariableConfig (
                 &VarStoreGuid,
                 L"SystemConfig",
                 BufferSize,
                 (UINT8 *) SuBrowser->SCBuffer,
                 TRUE
                 );
    }
    //
    // avoid GetQuestionDefault execute ExtractConfig
    //
    return EFI_SUCCESS;

  case H2O_BROWSER_ACTION_HOT_KEY_CALLBACK:
    Status = SuBrowser->HotKeyCallback (
                          This,
                          EFI_BROWSER_ACTION_CHANGED,
                          QuestionId,
                          Type,
                          Value,
                          ActionRequest
                          );
    break;

  default:
    break;
  }

  return Status;
}

/**
  Install Exit Callback routine.

  @param DriverHandle            Specific driver handle for the call back routine
  @param HiiHandle               Hii hanlde for the call back routine

  @retval EFI_SUCCESS            Function has completed successfully.
  @return Other                  Error occurred during execution.

**/
EFI_STATUS
EFIAPI
InstallExitCallbackRoutine (
  IN EFI_HANDLE                            DriverHandle,
  IN EFI_HII_HANDLE                        HiiHandle
  )
{
  EFI_STATUS                                Status;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  EFI_GUID                                  FormsetGuid = FORMSET_ID_GUID_EXIT;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mExitCallBackInfo = AllocatePool (sizeof (EFI_CALLBACK_INFO));
  if (mExitCallBackInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  mExitCallBackInfo->Signature                    = EFI_CALLBACK_INFO_SIGNATURE;
  mExitCallBackInfo->DriverCallback.ExtractConfig = SuBrowser->ExtractConfig;
  mExitCallBackInfo->DriverCallback.RouteConfig   = SuBrowser->RouteConfig;
  mExitCallBackInfo->DriverCallback.Callback      = ExitCallbackRoutine;
  mExitCallBackInfo->HiiHandle                    = HiiHandle;
  CopyGuid (&mExitCallBackInfo->FormsetGuid, &FormsetGuid);

  //
  // Install protocol interface
  //
  Status = gBS->InstallProtocolInterface (
                  &DriverHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mExitCallBackInfo->DriverCallback
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Uninstall Exit Callback routine.

  @param DriverHandle            Specific driver handle for the call back routine

  @retval EFI_SUCCESS            Function has completed successfully.
  @return Other                  Error occurred during execution.

**/
EFI_STATUS
EFIAPI
UninstallExitCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle
  )
{
  EFI_STATUS                                Status;
  if (mExitCallBackInfo == NULL) {
    return EFI_SUCCESS;
  }
  Status = gBS->UninstallProtocolInterface (
                  DriverHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &mExitCallBackInfo->DriverCallback
                  );
  ASSERT_EFI_ERROR (Status);
  FreePool (mExitCallBackInfo);
  mExitCallBackInfo = NULL;
  return Status;
}

/**
  Initialize exit menu for setuputility use

  @param HiiHandle               Hii hanlde for the call back routine

  @retval EFI_SUCCESS            Function has completed successfully.
  @return Other                  Error occurred during execution.

**/
EFI_STATUS
InitExitMenu (
  IN EFI_HII_HANDLE                         HiiHandle
  )
{
  return EFI_SUCCESS;
}

