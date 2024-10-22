/** @file

;******************************************************************************
;* Copyright (c) 2013 - 2015, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "SetupUtility.h"
#include <Protocol/FormBrowserEx2.h>
#include <Protocol/H2OFormBrowser.h>
#include <Library/DxeOemSvcChipsetLib.h>

UINT16                          gSCUSystemHealth;
EFI_EVENT                       gSCUTimerEvent;
BOOLEAN                         mPopLoadDefaultDialog = TRUE;
UINT32                          mScuRecord = 0;

EFI_STATUS
EFIAPI
SendSubmitExitNotify (
  VOID
)
{
  EFI_STATUS                         Status;
  H2O_DISPLAY_ENGINE_EVT_SUBMIT_EXIT SubmitExitNotify;
  H2O_FORM_BROWSER_PROTOCOL          *FBProtocol;

  Status = gBS->LocateProtocol (
                   &gH2OFormBrowserProtocolGuid,
                   NULL,
                   (VOID **)&FBProtocol
                   );
  if (!EFI_ERROR (Status)){    
    ZeroMem (&SubmitExitNotify, sizeof (SubmitExitNotify));
    
    SubmitExitNotify.Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_SUBMIT_EXIT);
    SubmitExitNotify.Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_SUBMIT_EXIT;
    SubmitExitNotify.Hdr.Target = H2O_DISPLAY_ENGINE_EVT_TARGET_FORM_BROWSER;
    
    Status = FBProtocol->Notify (FBProtocol, &SubmitExitNotify.Hdr);
  }

  return Status;
}

EFI_STATUS
EFIAPI
JumpToFirstQuestion (
  VOID
  )
{
  EFI_STATUS                                  Status;
  H2O_FORM_BROWSER_PROTOCOL                   *FBProtocol;
  H2O_DISPLAY_ENGINE_EVT_SELECT_Q             SelectQNotify;
  H2O_FORM_BROWSER_S                          *Statement;
  UINTN                                       Index;
  H2O_FORM_BROWSER_P                          *CurrentP;
  EFI_QUESTION_ID                             QuestionId;

  Status = gBS->LocateProtocol (&gH2OFormBrowserProtocolGuid, NULL, (VOID **) &FBProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  QuestionId = 0;
  CurrentP   = FBProtocol->CurrentP;
  for (Index = 0; Index < CurrentP->NumberOfStatementIds; Index++) {
    Status = FBProtocol->GetSInfo (FBProtocol, CurrentP->PageId, CurrentP->StatementIds[Index], &Statement);
    if (EFI_ERROR (Status)) {
      continue;
    }

    if (!Statement->Selectable) {
      continue;
    }

    QuestionId = Statement->QuestionId;
    FreePool (Statement);

    if (QuestionId != 0) {
      break;
    }
  }

  ZeroMem (&SelectQNotify, sizeof (H2O_DISPLAY_ENGINE_EVT_SELECT_Q));
  SelectQNotify.Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_SELECT_Q);
  SelectQNotify.Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_SELECT_Q;
  SelectQNotify.Hdr.Target = H2O_DISPLAY_ENGINE_EVT_TARGET_FORM_BROWSER;
  SelectQNotify.PageId     = FBProtocol->CurrentP->PageId;
  SelectQNotify.QuestionId = QuestionId;
  SelectQNotify.IfrOpCode  = 0;

  return FBProtocol->Notify (FBProtocol, &SelectQNotify.Hdr);
}


/**
  Execute form browser action by EDK2 form browser extension 2 protocol

  @param[in] Action     Execute the request action.
  @param[in] DefaultId  The default Id info when need to load default value.

  @retval EFI_SUCCESS   Successfully execute form browser action
  @retval Other         Locate protocol fail
**/
EFI_STATUS
ExecuteFormBrowserAction (
  IN UINT32                                    Action,
  IN UINT16                                    DefaultId
  )
{
  EFI_STATUS                                   Status;
  EDKII_FORM_BROWSER_EXTENSION2_PROTOCOL       *FormBrowserEx2;

  Status = gBS->LocateProtocol (
                  &gEdkiiFormBrowserEx2ProtocolGuid,
                  NULL,
                  (VOID **)&FormBrowserEx2
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return FormBrowserEx2->ExecuteAction (Action, DefaultId);
}

EFI_STATUS
HotKeyCallBackByAction (
  IN CONST  EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN  EFI_BROWSER_ACTION                    Action,
  IN  EFI_QUESTION_ID                       QuestionId,
  IN  UINT8                                 Type,
  IN  EFI_IFR_TYPE_VALUE                    *Value,
  OUT EFI_BROWSER_ACTION_REQUEST            *ActionRequest
  )
{
  EFI_STATUS                     Status;

  Status = EFI_UNSUPPORTED;

  switch (Action) {

  case EFI_BROWSER_ACTION_CHANGING:
    return EFI_SUCCESS;

  case EFI_BROWSER_ACTION_DEFAULT_MANUFACTURING:
    if (QuestionId == KEY_SCAN_F9) {
      mPopLoadDefaultDialog = FALSE;
      Status = HotKeyCallBack (
                 This,
                 EFI_BROWSER_ACTION_CHANGED,
                 QuestionId,
                 Type,
                 Value,
                 ActionRequest
                 );
      mPopLoadDefaultDialog = TRUE;
      BrowserRefreshFormSet ();
    }
    //
    // avoid GetQuestionDefault execute ExtractConfig, so always return success.
    //
    return EFI_SUCCESS;

  default:
    break;
  }

  return Status;
}
/**
 Proccess HotKey function

 @param [in]   This
 @param [in]   HiiHandle
 @param [in]   Action
 @param [in]   QuestionId
 @param [in]   Type
 @param [in]   Value
 @param [out]  ActionRequest

**/
EFI_STATUS
HotKeyCallBack (
  IN CONST  EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN  EFI_BROWSER_ACTION                    Action,
  IN  EFI_QUESTION_ID                       QuestionId,
  IN  UINT8                                 Type,
  IN  EFI_IFR_TYPE_VALUE                    *Value,
  OUT EFI_BROWSER_ACTION_REQUEST            *ActionRequest

)
{
  CHIPSET_CONFIGURATION                     *MyIfrNVData;
  CHAR16                                    *StringPtr;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  EFI_INPUT_KEY                             Key;
  STATIC UINT8                              BackupLang;
  EFI_STATUS                                Status;
  UINT16                                    *BootOrderList;
  UINTN                                     BufferSize = 0;
  UINTN                                     VariableSize;
  EFI_HII_HANDLE                            HiiHandle;

  if (Action != EFI_BROWSER_ACTION_CHANGED) {
    return HotKeyCallBackByAction (This, Action, QuestionId, Type, Value, ActionRequest);
  }
  VariableSize        = 0;
  BootOrderList = NULL;
  SUCInfo     = gSUBrowser->SUCInfo;
  MyIfrNVData = (CHIPSET_CONFIGURATION *) gSUBrowser->SCBuffer;

  Status              = EFI_SUCCESS;
  HiiHandle           = mDriverHiiHandle;
  *ActionRequest      = EFI_BROWSER_ACTION_REQUEST_NONE;

  switch (QuestionId) {

  case KEY_SCAN_ESC :
    //
    // Discard setup and exit
    //
    StringPtr = HiiGetString (
      HiiHandle,
      STRING_TOKEN (STR_EXIT_DISCARDING_CHANGES_STRING),
      NULL
      );
    gSUBrowser->H2ODialog->ConfirmDialog (
                             0,
                             FALSE,
                             0,
                             NULL,
                             &Key,
                             StringPtr
                             );
    if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      CheckLanguage ();
      *ActionRequest = EFI_BROWSER_ACTION_REQUEST_EXIT;
    }

    gBS->FreePool (StringPtr);
    break;

  case KEY_SCAN_F9 :
    //
    // Load Optimal
    //
    // If user Accesslevel = USER_PASSWORD_VIEW_ONLY(0x02) or USER_PASSWORD_LIMITED(0x03)
    // and use user password into SCU,the user don't use load optimal function.
    if ((MyIfrNVData->SetUserPass == TRUE) &&
        ((MyIfrNVData->UserAccessLevel == 2) ||
        (MyIfrNVData->UserAccessLevel == 3))) {
      return EFI_ABORTED;
    }

    EventTimerControl(0);

    //
    // Load optimal setup.
    //
    Key.UnicodeChar = CHAR_CARRIAGE_RETURN;
    if (mPopLoadDefaultDialog) {
      StringPtr = HiiGetString (
        HiiHandle,
        STRING_TOKEN (STR_LOAD_OPTIMAL_DEFAULTS_STRING),
        NULL
        );
      gSUBrowser->H2ODialog->ConfirmDialog (
                               0,
                               FALSE,
                               0,
                               NULL,
                               &Key,
                               StringPtr
                               );
      gBS->FreePool (StringPtr);
      if (Key.UnicodeChar != CHAR_CARRIAGE_RETURN) {
        EventTimerControl(TIMEOUT_OF_EVENT);
        return EFI_UNSUPPORTED;
      }
      return ExecuteFormBrowserAction (BROWSER_ACTION_DEFAULT, EFI_HII_DEFAULT_CLASS_STANDARD);
    }
    if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      mScuRecord |= SCU_ACTION_LOAD_DEFAULT;
      Status = gRT->SetVariable (
                      L"PlatformLang",
                      &gEfiGlobalVariableGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                      AsciiStrSize ((CHAR8 *) PcdGetPtr (PcdUefiVariableDefaultPlatformLang)),
                      (VOID *) PcdGetPtr (PcdUefiVariableDefaultPlatformLang)
                      );
      if (!EFI_ERROR (Status)) {
        GetLangIndex ((CHAR8 *) PcdGetPtr (PcdUefiVariableDefaultPlatformLang), &MyIfrNVData->Language);
      }

      BrowserRefreshFormSet ();

      UpdatePasswordState (gSUBrowser->SUCInfo->MapTable[SecurityHiiHandle].HiiHandle);
      BootOrderList = GetVariableAndSize (
                        L"BootOrder",
                        &gEfiGlobalVariableGuid,
                        &BufferSize
                        );

      if (BufferSize != 0) {
         gBS->FreePool (SUCInfo->BootOrder);
         SUCInfo->BootOrder = BootOrderList;
         SUCInfo->AdvBootDeviceNum  = (UINT16) (BufferSize / sizeof(UINT16));
       }

      MyIfrNVData->BootMenuType         = NORMAL_MENU;
      MyIfrNVData->BootNormalPriority   = EFI_FIRST;
      MyIfrNVData->LegacyNormalMenuType = NORMAL_MENU;
      //
      //  reset the flag of showing from.
      //
      DefaultSetup (MyIfrNVData);

      UpdateHDCConfigure (
        gSUBrowser->SUCInfo->MapTable[AdvanceHiiHandle].HiiHandle,
        MyIfrNVData
        );
      UpdateStringToken ((KERNEL_CONFIGURATION *) MyIfrNVData);
      SetupUtilityLibLoadDefault ();
//       if (MyIfrNVData->MpSupport != ((CHIPSET_CONFIGURATION *) SUCInfo->SCBuffer)->MpSupport) {
//         WriteExtCmos8 (EXT_CMOS_INDEX, EXT_CMOS_DATA, CPU_DUAL_CORE_SWITCH,0x01);
//       }
      JumpToFirstQuestion ();
    }
    EventTimerControl(TIMEOUT_OF_EVENT);
    break;

  case KEY_SCAN_F10 :
    //
    // If user Accesslevel = USER_PASSWORD_VIEW_ONLY(0x02) or USER_PASSWORD_LIMITED(0x03)
    // and use user password into SCU,the user don't use load optimal function.
    //
    if ((MyIfrNVData->SetUserPass == TRUE) &&
        (MyIfrNVData->UserAccessLevel == 2)) {
      return EFI_ABORTED;
    }
    EventTimerControl (0);

    SendSubmitExitNotify ();
    break;

  case KEY_SAVE_WITHOUT_EXIT:
    StringPtr = HiiGetString (
      HiiHandle,
      STRING_TOKEN (STR_SAVE_CHANGE_WITHOUT_EXIT_STRING),
      NULL
      );

    gSUBrowser->H2ODialog->ConfirmDialog (
                             0,
                             FALSE,
                             0,
                             NULL,
                             &Key,
                             StringPtr
                             );
    gBS->FreePool (StringPtr);

    if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      ExecuteFormBrowserAction (BROWSER_ACTION_SUBMIT, 0);
    } else {
      Status = EFI_UNSUPPORTED;
    }
    break;

  case KEY_DISCARD_CHANGE:
    //
    // discard setup change.
    //
    StringPtr = HiiGetString (
      HiiHandle,
      STRING_TOKEN (STR_DISCARD_CHANGES_STRING),
      NULL
      );
    gSUBrowser->H2ODialog->ConfirmDialog (
                             0,
                             FALSE,
                             0,
                             NULL,
                             &Key,
                             StringPtr
                             );
    if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      mScuRecord &= (~SCU_ACTION_LOAD_DEFAULT);
      BrowserRefreshFormSet ();
      ExecuteFormBrowserAction (BROWSER_ACTION_DISCARD, 0);

      Status = DiscardChange (This);
      UpdateHDCConfigure (
        gSUBrowser->SUCInfo->MapTable[AdvanceHiiHandle].HiiHandle,
        MyIfrNVData
        );
      SUCInfo->DoRefresh = TRUE;
      JumpToFirstQuestion ();
    }

    gBS->FreePool (StringPtr);

    break;

  case KEY_BOOT_MODE_TYPE:
    //
    // For Plug-in display selection:
    //
    ClearFormDataFromLabel (HiiHandle, VIDEO_CONFIG_FORM_ID, PLUG_IN_DISPLAY_SELECTION_START_LABEL);
    break;

  default :
    break;
  }


  return Status;
}

/**
 This function allows a caller to extract the current configuration for one
 or more named elements from the target driver.

 @param [in]   This             Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
 @param [in]   Request          A null-terminated Unicode string in <ConfigRequest> format.
 @param [out]  Progress         On return, points to a character in the Request string.
                                Points to the string's null terminator if request was successful.
                                Points to the most recent '&' before the first failing name/value
                                pair (or the beginning of the string if the failure is in the
                                first name/value pair) if the request was not successful.
 @param [out]  Results          A null-terminated Unicode string in <ConfigAltResp> format which
                                has all values filled in for the names in the Request string.
                                String to be allocated by the called function.

 @retval EFI_SUCCESS            The Results is filled with the requested values.
 @retval EFI_OUT_OF_RESOURCES   Not enough memory to store the results.
 @retval EFI_INVALID_PARAMETER  Request is NULL, illegal syntax, or unknown name.
 @retval EFI_NOT_FOUND          Routing data doesn't match any storage in this driver.

**/
EFI_STATUS
EFIAPI
GenericExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  EFI_STATUS                    Status;
  UINTN                         BufferSize;
  EFI_STRING                    ConfigRequestHdr;
  EFI_STRING                    ConfigRequest;
  BOOLEAN                       AllocatedRequest;
  UINTN                         Size;
  CHAR16                        *StrPointer;
  EFI_CALLBACK_INFO             *CallbackInfo;
  EFI_HANDLE                    DriverHandle;
  EFI_GUID                      VarStoreGuid = SYSTEM_CONFIGURATION_GUID;
  
  
  if (This == NULL || Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }
   *Progress = Request;
  CallbackInfo     = EFI_CALLBACK_INFO_FROM_THIS (This);
  BufferSize       = GetVarStoreSize (CallbackInfo->HiiHandle, &CallbackInfo->FormsetGuid, &VarStoreGuid, "SystemConfig");
  ConfigRequestHdr = NULL;
  ConfigRequest    = NULL;
  AllocatedRequest = FALSE;
  Size             = 0; 

  if (Request == NULL) {
    //
    // Request is set to NULL, construct full request string.
    //
    //
    // Allocate and fill a buffer large enough to hold the <ConfigHdr> template
    // followed by "&OFFSET=0&WIDTH=WWWWWWWWWWWWWWWW" followed by a Null-terminator
    //
    Status = gSUBrowser->HiiDatabase->GetPackageListHandle (gSUBrowser->HiiDatabase, CallbackInfo->HiiHandle, &DriverHandle);
    if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
    }
    ConfigRequestHdr = HiiConstructConfigHdr (&mFormSetGuid, mVariableName, DriverHandle);
    if (ConfigRequestHdr == NULL) {
      return EFI_NOT_FOUND;
    }
    Size = (StrLen (ConfigRequestHdr) + 32 + 1) * sizeof (CHAR16);
    ConfigRequest = AllocateZeroPool (Size);
    ASSERT (ConfigRequest != NULL);
    AllocatedRequest = TRUE;
    UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, (UINT64) BufferSize);
    FreePool (ConfigRequestHdr);
    ConfigRequestHdr = NULL;
  } else {
    //
    // Check routing data in <ConfigHdr>.
    // Note: if only one Storage is used, then this checking could be skipped.
    //
    if (!HiiIsConfigHdrMatch (Request, &mFormSetGuid, mVariableName)) {
      return EFI_NOT_FOUND;
    }
    //
    // Set Request to the unified request string.
    //
    ConfigRequest = Request;
    //
    // Check whether Request includes Request Element.
    //
    if (StrStr (Request, L"OFFSET") == NULL) {
      //
      // Check Request Element does exist in Reques String
      //
      StrPointer = StrStr (Request, L"PATH");
      if (StrPointer == NULL) {
        return EFI_INVALID_PARAMETER;
      }
      if (StrStr (StrPointer, L"&") == NULL) {
        Size = (StrLen (Request) + 32 + 1) * sizeof (CHAR16);
        ConfigRequest = AllocateZeroPool (Size);
        ASSERT (ConfigRequest != NULL);
        AllocatedRequest = TRUE;
        UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", Request, (UINT64) BufferSize);
      }
    }
  }

  if (StrStr (ConfigRequest, L"OFFSET") == NULL) {
    //
    // If requesting Name/Value storage, return not found.
    //
    return EFI_NOT_FOUND;
  }

  //
  // Convert buffer data to <ConfigResp> by helper function BlockToConfig()
  //
  Status = gSUBrowser->HiiConfigRouting->BlockToConfig (
                                           gSUBrowser->HiiConfigRouting,
                                           ConfigRequest,
                                           (UINT8 *) gSUBrowser->SCBuffer,
                                           BufferSize,
                                           Results,
                                           Progress
                                           );
  //
  // Free the allocated config request string.
  //
  if (AllocatedRequest) {
    gBS->FreePool (ConfigRequest);
    ConfigRequest = NULL;
  }
  //
  // Set Progress string to the original request string.
  //
  if (Request == NULL) {
    *Progress = NULL;
  } else if (StrStr (Request, L"OFFSET") == NULL) {
    *Progress = Request + StrLen (Request);
  }

  return Status;
}


/**
 This function processes the results of changes in configuration.

 @param [in]   This             Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
 @param [in]   Configuration    A null-terminated Unicode string in <ConfigResp> format.
 @param [out]  Progress         A pointer to a string filled in with the offset of the most
                                recent '&' before the first failing name/value pair (or the
                                beginning of the string if the failure is in the first
                                name/value pair) or the terminating NULL if all was successful.

 @retval EFI_SUCCESS            The Results is processed successfully.
 @retval EFI_INVALID_PARAMETER  Configuration is NULL.
 @retval EFI_NOT_FOUND          Routing data doesn't match any storage in this driver.

**/
EFI_STATUS
EFIAPI
GenericRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
  EFI_STATUS                                Status;
  CHIPSET_CONFIGURATION                     SetupNvData;
  UINTN                                     BufferSize;
  EFI_GUID                                  VarStoreGuid = SYSTEM_CONFIGURATION_GUID;
  UINT8                                     *Lang;


  if (Configuration != NULL && !HiiIsConfigHdrMatch (Configuration, &mFormSetGuid, mVariableName)) {
    return EFI_SUCCESS;
  }

  BufferSize = PcdGet32 (PcdSetupConfigSize);
  SetupVariableConfig (
    &VarStoreGuid,
    L"SystemConfig",
    BufferSize,
    (UINT8 *) gSUBrowser->SCBuffer,
    TRUE
    );

//
// Oem hook when F10 or Exit Saving Changes or Save Change Without Exit submit.
// And system will set current setuputility setting to browser.
//
  OemSvcHookRouteConfig ((VOID *)gSUBrowser->SCBuffer, (UINT32)BufferSize, mScuRecord);

  SetupVariableConfig (
    &VarStoreGuid,
    L"SystemConfig",
    BufferSize,
    (UINT8 *) gSUBrowser->SCBuffer,
    FALSE
    );

  CopyMem (&SetupNvData, gSUBrowser->SCBuffer, PcdGet32 (PcdSetupConfigSize));

  Lang = GetVariableAndSize (L"PlatformLang", &gEfiGlobalVariableGuid, &BufferSize);
  Status = gRT->SetVariable (
                  L"BackupPlatformLang",
                  &gEfiGenericVariableGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  (Lang == NULL) ? 0 : BufferSize,
                  Lang
                  );
  if (Lang != NULL) {
    FreePool (Lang);
  }

  SetupUtilityLibRouteConfig ();

  SetSecurityStatus ();

  BufferSize = PcdGet32 (PcdSetupConfigSize);
  Status = SaveSetupConfig (
             L"Setup",
             &mFormSetGuid,
             EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
             BufferSize,
             &SetupNvData
             );
  PlugInVgaDisplaySelectionSave ();

  return  Status;
}

