/** @file
  Initial and callback functions for Boot page of SetupUtilityLib

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

#include "Boot.h"

STATIC EFI_CALLBACK_INFO                  *mBootCallBackInfo;
BOOT_CONFIGURATION                        mBootConfig;

/**
  Initialize boot menu for setuputility use

  @param HiiHandle               Hii hanlde for the call back routine

  @retval EFI_SUCCESS            Function has completed successfully.
  @return Other                  Error occurred during execution.

**/
STATIC
EFI_STATUS
InitBootMenu (
  IN EFI_HII_HANDLE       HiiHandle
  )
{
  EFI_STATUS                                Status;
  UINT16                                    *BootOrder;
  UINT16                                    BootDeviceNum;
  UINTN                                     BootOrderSize;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  UINT16                                    Timeout;
  UINTN                                     Size;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Sync Timeout variable to timeout value of Setup variable.
  //
  Timeout = 0;
  Size = sizeof(UINT16);
  Status = CommonGetVariable (
                  L"Timeout",
                  &gEfiGlobalVariableGuid,
                  &Size,
                  &Timeout
                  );
  if (!EFI_ERROR(Status) && Timeout != ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->Timeout) {
    ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->Timeout = Timeout;
  }

  GetLegacyBootDevType (
    &(SuBrowser->SUCInfo->LegacyBootDevTypeCount),
    &(SuBrowser->SUCInfo->LegacyBootDevType)
    );

  BootOrder     = NULL;
  BootOrderSize = 0;
  CommonGetVariableDataAndSize (
    L"BootOrder",
    &gEfiGlobalVariableGuid,
    &BootOrderSize,
    (VOID **) &BootOrder
    );
  if (BootOrderSize != 0) {
    BootDeviceNum = (UINT16) (BootOrderSize / sizeof(UINT16));
  } else {
    BootDeviceNum = 0;
    BootOrder     = NULL;
  }

  SuBrowser->SUCInfo->BootOrder        = BootOrder;
  SuBrowser->SUCInfo->AdvBootDeviceNum = BootDeviceNum;
  BootDevInfoInit ();
  Status = UpdateBootOrderLabels (
             HiiHandle,
             (KERNEL_CONFIGURATION *) SuBrowser->SCBuffer
             );

  BootConfigInit (SuBrowser);

  return EFI_SUCCESS;
}

/**
 Load the default value of boot configuration varstore.

 @retval EFI_SUCCESS             Load the default value successfully.
 @retval Other                   Fail to load the default value of boot configuration varstore.
**/
EFI_STATUS
BootLoadDefault (
  VOID
  )
{
  return BootConfigLoadDefault ();
}

/**
 Save the current settings of all varstores belong to Boot VFR.

 @retval EFI_SUCCESS             Save the current settings successfully
 @retval Other                   Fail to save the current settings of boot configuration or fail to get browser data.
**/
EFI_STATUS
BootSaveSetting (
  VOID
  )
{
  EFI_STATUS                                Status;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  KERNEL_CONFIGURATION                      *KernelConfig;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo      = SuBrowser->SUCInfo;
  KernelConfig = (KERNEL_CONFIGURATION *) SuBrowser->SCBuffer;

  gRT->SetVariable (
         L"BootOrder",
         &gEfiGlobalVariableGuid,
         EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
         SUCInfo->AdvBootDeviceNum * sizeof (UINT16),
         (UINT16 *) SUCInfo->BootOrder
         );

  gRT->SetVariable (
         L"Timeout",
         &gEfiGlobalVariableGuid,
         EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
         sizeof (UINT16),
         (UINT16 *) &KernelConfig->Timeout
         );

  return BootConfigSaveSetting ();
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
BootCallbackRoutine (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS                                Status;
  KERNEL_CONFIGURATION                      *MyIfrNVData;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  EFI_CALLBACK_INFO                         *CallbackInfo;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  UINTN                                     BufferSize;
  EFI_GUID                                  VarStoreGuid = SYSTEM_CONFIGURATION_GUID;
  BOOT_DEV_INFO                             *BootDevInfo;
  BOOLEAN                                   RetrieveData;

  if (Action != EFI_BROWSER_ACTION_CHANGED) {
    return BootCallbackRoutineByAction (This, Action, QuestionId, Type, Value, ActionRequest);
  }

  *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  CallbackInfo   = EFI_CALLBACK_INFO_FROM_THIS (This);

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }


  BufferSize = GetVarStoreSize (CallbackInfo->HiiHandle, &CallbackInfo->FormsetGuid, &VarStoreGuid, "SystemConfig");

  SUCInfo      = SuBrowser->SUCInfo;
  RetrieveData = TRUE;
  SetupVariableConfig (&VarStoreGuid, L"SystemConfig", BufferSize                 , (UINT8 *) SuBrowser->SCBuffer, RetrieveData);
  SetupVariableConfig (&VarStoreGuid, L"BootConfig"  , sizeof (BOOT_CONFIGURATION), (UINT8 *) &mBootConfig       , RetrieveData);
  MyIfrNVData = (KERNEL_CONFIGURATION *) SuBrowser->SCBuffer;

  switch (QuestionId) {

  case KEY_UP_SHIFT:
  case KEY_DOWN_SHIFT:
    if (Type != EFI_IFR_TYPE_NUM_SIZE_32) {
      break;
    }

    Status = Shiftitem (
               This,
               (EFI_QUESTION_ID) Value->u16,
               (BOOLEAN) (QuestionId == KEY_UP_SHIFT),
               MyIfrNVData
               );
    break;

  case KEY_BOOT_MENU_TYPE:
  case KEY_NORMAL_BOOT_PRIORITY:
  case KEY_LEGACY_NORMAL_BOOT_MENU:
    if (QuestionId == KEY_NORMAL_BOOT_PRIORITY) {
      AdjustBootOrder (
        !((BOOLEAN) MyIfrNVData->BootNormalPriority),
        SUCInfo->AdvBootDeviceNum,
        SUCInfo->BootOrder
        );
    }
    SetupUtilityLibUpdateDeviceString (CallbackInfo->HiiHandle, MyIfrNVData);
    SyncBootOrderToBootConfig ();
    break;

  case KEY_NEW_POSITION_POLICY:
    //
    // If change New Added postion policy to auto, we need adjust boot oreder to suit the
    // auto position policy
    //
    if (MyIfrNVData->NewPositionPolicy == IN_AUTO) {
      ChangeToAutoBootOrder (
        SUCInfo->AdvBootDeviceNum,
        SUCInfo->BootOrder
        );
      SetupUtilityLibUpdateDeviceString (CallbackInfo->HiiHandle, MyIfrNVData);
      SyncBootOrderToBootConfig ();
    }
    break;

  case KEY_BOOT_MODE_TYPE:
    switch (MyIfrNVData->BootType) {

    case EFI_BOOT_TYPE:
      if (MyIfrNVData->NetworkProtocol == UEFI_NETWORK_BOOT_OPTION_LEGACY) {
        MyIfrNVData->NetworkProtocol = UEFI_NETWORK_BOOT_OPTION_DISABLE;
      }
      break;

    case LEGACY_BOOT_TYPE:
      if (MyIfrNVData->NetworkProtocol != UEFI_NETWORK_BOOT_OPTION_LEGACY && MyIfrNVData->NetworkProtocol != UEFI_NETWORK_BOOT_OPTION_DISABLE) {
        MyIfrNVData->NetworkProtocol = UEFI_NETWORK_BOOT_OPTION_DISABLE;
      }
      break;
    }
    Status = SuBrowser->HotKeyCallback (
                          This,
                          Action,
                          QuestionId,
                          Type,
                          Value,
                          ActionRequest
                          );
    break;

  case KEY_PXE_BOOT_TO_LAN:
    //
    // Modify NetworkProtocol to DISABLE if PxeBootToLan switch to DISABLED.
    //
    MyIfrNVData->NetworkProtocol = UEFI_NETWORK_BOOT_OPTION_DISABLE;
    break;

  default:
    BootDevInfo = GetBootDevInfo (QuestionId);
    if (BootDevInfo != NULL) {
      if (Type == EFI_IFR_TYPE_BOOLEAN && Value != NULL) {
        BootDevInfoSetActiveValue (BootDevInfo->BootOptionNum, Value->b);
      }
      break;
    }

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

  RetrieveData = FALSE;
  SetupVariableConfig (&VarStoreGuid, L"SystemConfig", BufferSize                 , (UINT8 *) SuBrowser->SCBuffer, RetrieveData);
  SetupVariableConfig (&VarStoreGuid, L"BootConfig"  , sizeof (BOOT_CONFIGURATION), (UINT8 *) &mBootConfig       , RetrieveData);

  return Status;
}

EFI_STATUS
BootCallbackRoutineByAction (
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
  EFI_IFR_TYPE_VALUE                        HotKeyValue;
  EFI_QUESTION_ID                           HotKeyQuestionId;
  BOOT_DEV_INFO                             *BootDevInfo;
  BOOLEAN                                   RetrieveData;

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
  Status         = EFI_UNSUPPORTED;

  switch (Action) {

  case EFI_BROWSER_ACTION_FORM_OPEN:
  case EFI_BROWSER_ACTION_FORM_CLOSE:
    if (QuestionId == 0) {
      RetrieveData = (BOOLEAN) (Action == EFI_BROWSER_ACTION_FORM_CLOSE);
      BufferSize   = GetVarStoreSize (CallbackInfo->HiiHandle, &CallbackInfo->FormsetGuid, &VarStoreGuid, "SystemConfig");
      SetupVariableConfig (&VarStoreGuid, L"SystemConfig", BufferSize                 , (UINT8 *) SuBrowser->SCBuffer, RetrieveData);
      SetupVariableConfig (&VarStoreGuid, L"BootConfig"  , sizeof (BOOT_CONFIGURATION), (UINT8 *) &mBootConfig       , RetrieveData);
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
      RetrieveData = FALSE;
      BufferSize   = GetVarStoreSize (CallbackInfo->HiiHandle, &CallbackInfo->FormsetGuid, &VarStoreGuid, "SystemConfig");
      SetupVariableConfig (&VarStoreGuid, L"SystemConfig", BufferSize                 , (UINT8 *) SuBrowser->SCBuffer, RetrieveData);
      SetupVariableConfig (&VarStoreGuid, L"BootConfig"  , sizeof (BOOT_CONFIGURATION), (UINT8 *) &mBootConfig       , RetrieveData);
    }
    //
    // avoid GetQuestionDefault execute ExtractConfig
    //
    return EFI_SUCCESS;

  case H2O_BROWSER_ACTION_HOT_KEY_CALLBACK:
    if (QuestionId == KEY_LEGACY_NORMAL_BOOT_MENU) {
      //
      // Because modify value hot key is changed to change boot order hot key in EFI\Legacy page,
      // modify question value for this question to achieve the same behavior.
      //
      HotKeyValue.u8 = (((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->LegacyNormalMenuType == 0) ? 1 : 0;
      ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->LegacyNormalMenuType = HotKeyValue.u8;

      RetrieveData = FALSE;
      BufferSize   = GetVarStoreSize (CallbackInfo->HiiHandle, &CallbackInfo->FormsetGuid, &VarStoreGuid, "SystemConfig");
      SetupVariableConfig (
        &VarStoreGuid,
        L"SystemConfig",
        BufferSize,
        (UINT8 *) SuBrowser->SCBuffer,
        RetrieveData
        );

      BootCallbackRoutine (
         This,
         EFI_BROWSER_ACTION_CHANGED,
         KEY_LEGACY_NORMAL_BOOT_MENU,
         EFI_IFR_TYPE_NUM_SIZE_8,
         &HotKeyValue,
         ActionRequest
         );
      SendGotoHotKeyNotify (0, QuestionId);
      break;
    }

    if (((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->NewPositionPolicy == IN_AUTO) {
      BootDevInfo = GetBootDevInfo (QuestionId);
      if (BootDevInfo != NULL && BootDevInfo->IsEfiBootDev) {
        //
        // Not allow to change EFI boot device order when "Add Boot Options" is AUTO.
        //
        break;
      }
    }

    HotKeyValue.u32  = QuestionId;
    HotKeyQuestionId = Value->b ? KEY_UP_SHIFT : KEY_DOWN_SHIFT;

    Status = BootCallbackRoutine (
               This,
               EFI_BROWSER_ACTION_CHANGED,
               HotKeyQuestionId,
               EFI_IFR_TYPE_NUM_SIZE_32,
               &HotKeyValue,
               ActionRequest
               );
    if (!EFI_ERROR (Status)) {
      SendGotoHotKeyNotify (0, (HotKeyQuestionId == KEY_UP_SHIFT) ? QuestionId - 1 : QuestionId + 1);
    }
    break;

  default:
    break;
  }

  return Status;
}

/**
 This function allows a caller to extract BootConfig configuration.

 @param[in]  This               Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
 @param[in]  Request            A null-terminated Unicode string in <ConfigRequest> format.
 @param[out] Progress           On return, points to a character in the Request string.
                                Points to the string's null terminator if request was successful.
                                Points to the most recent '&' before the first failing name/value
                                pair (or the beginning of the string if the failure is in the
                                first name/value pair) if the request was not successful.
 @param[out] Results            A null-terminated Unicode string in <ConfigAltResp> format which
                                has all values filled in for the names in the Request string.
                                String to be allocated by the called function.

 @retval EFI_SUCCESS            The Results is filled with the requested values.
 @retval EFI_OUT_OF_RESOURCES   Not enough memory to store the results.
 @retval EFI_INVALID_PARAMETER  Request is NULL, illegal syntax, or unknown name.
 @retval EFI_NOT_FOUND          Routing data doesn't match any storage in this driver.
**/
EFI_STATUS
InternalBootExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  EFI_STATUS                                 Status;
  UINTN                                      BufferSize;
  EFI_STRING                                 ConfigRequestHdr;
  EFI_STRING                                 ConfigRequest;
  BOOLEAN                                    AllocatedRequest;
  UINTN                                      Size;
  CHAR16                                     *StrPointer;
  EFI_CALLBACK_INFO                          *CallbackInfo;
  EFI_HANDLE                                 DriverHandle;
  SETUP_UTILITY_BROWSER_DATA                 *SuBrowser;

  if (This == NULL || Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!HiiIsConfigHdrMatch (Request, &gSystemConfigurationGuid, L"BootConfig")) {
    return EFI_UNSUPPORTED;
  }

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  *Progress        = Request;
  CallbackInfo     = EFI_CALLBACK_INFO_FROM_THIS (This);
  BufferSize       = sizeof (BOOT_CONFIGURATION);
  ConfigRequestHdr = NULL;
  ConfigRequest    = NULL;
  AllocatedRequest = FALSE;
  Size             = 0;

  if (Request == NULL) {
    //
    // Request is set to NULL, construct full request string.
    //
    // Allocate and fill a buffer large enough to hold the <ConfigHdr> template
    // followed by "&OFFSET=0&WIDTH=WWWWWWWWWWWWWWWW" followed by a Null-terminator
    //
    Status = SuBrowser->HiiDatabase->GetPackageListHandle (SuBrowser->HiiDatabase, CallbackInfo->HiiHandle, &DriverHandle);
    if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
    }
    ConfigRequestHdr = HiiConstructConfigHdr (&gSystemConfigurationGuid, L"BootConfig", DriverHandle);
    if (ConfigRequestHdr == NULL) {
      return EFI_UNSUPPORTED;
    }
    Size = (StrLen (ConfigRequestHdr) + 32 + 1) * sizeof (CHAR16);
    ConfigRequest = AllocateZeroPool (Size);
    ASSERT (ConfigRequest != NULL);
    if (ConfigRequest == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    AllocatedRequest = TRUE;
    UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, (UINT64) BufferSize);
    FreePool (ConfigRequestHdr);
    ConfigRequestHdr = NULL;
  } else {
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
        if (ConfigRequest == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
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
  Status = SuBrowser->HiiConfigRouting->BlockToConfig (
                                           SuBrowser->HiiConfigRouting,
                                           ConfigRequest,
                                           (UINT8 *) &mBootConfig,
                                           BufferSize,
                                           Results,
                                           Progress
                                           );
  if (AllocatedRequest) {
    FreePool (ConfigRequest);
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
 This function allows a caller to extract the current configuration for one
 or more named elements from the target driver.

 @param[in]  This               Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
 @param[in]  Request            A null-terminated Unicode string in <ConfigRequest> format.
 @param[out] Progress           On return, points to a character in the Request string.
                                Points to the string's null terminator if request was successful.
                                Points to the most recent '&' before the first failing name/value
                                pair (or the beginning of the string if the failure is in the
                                first name/value pair) if the request was not successful.
 @param[out] Results            A null-terminated Unicode string in <ConfigAltResp> format which
                                has all values filled in for the names in the Request string.
                                String to be allocated by the called function.

 @retval EFI_SUCCESS            The Results is filled with the requested values.
 @retval EFI_OUT_OF_RESOURCES   Not enough memory to store the results.
 @retval EFI_INVALID_PARAMETER  Request is NULL, illegal syntax, or unknown name.
 @retval EFI_NOT_FOUND          Routing data doesn't match any storage in this driver.
**/
EFI_STATUS
EFIAPI
BootExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  EFI_STATUS                                 Status;
  SETUP_UTILITY_BROWSER_DATA                 *SuBrowser;

  Status = InternalBootExtractConfig (This, Request, Progress, Results);
  if (!EFI_ERROR (Status)) {
    return Status;
  }

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  return SuBrowser->ExtractConfig (This, Request, Progress, Results);
}

/**
  Install Boot Callback routine.

  @param DriverHandle            Specific driver handle for the call back routine
  @param HiiHandle               Hii hanlde for the call back routine

  @retval EFI_SUCCESS            Function has completed successfully.
  @return Other                  Error occurred during execution.

**/
EFI_STATUS
EFIAPI
InstallBootCallbackRoutine (
  IN EFI_HANDLE                     DriverHandle,
  IN EFI_HII_HANDLE                 HiiHandle
  )
{
  EFI_STATUS                                Status;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  EFI_GUID                                  FormsetGuid = FORMSET_ID_GUID_BOOT;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mBootCallBackInfo = AllocatePool (sizeof(EFI_CALLBACK_INFO));
  if (mBootCallBackInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  mBootCallBackInfo->Signature                    = EFI_CALLBACK_INFO_SIGNATURE;
  mBootCallBackInfo->DriverCallback.ExtractConfig = BootExtractConfig;
  mBootCallBackInfo->DriverCallback.RouteConfig   = SuBrowser->RouteConfig;
  mBootCallBackInfo->DriverCallback.Callback      = BootCallbackRoutine;
  mBootCallBackInfo->HiiHandle                    = HiiHandle;
  CopyGuid (&mBootCallBackInfo->FormsetGuid, &FormsetGuid);

  //
  // Install protocol interface
  //
  Status = gBS->InstallProtocolInterface (
                  &DriverHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mBootCallBackInfo->DriverCallback
                  );
  ASSERT_EFI_ERROR (Status);

  Status = InitBootMenu (HiiHandle);

  return Status;
}

/**
  Uninstall Boot Callback routine.

  @param DriverHandle            Specific driver handle for the call back routine

  @retval EFI_SUCCESS            Function has completed successfully.
  @return Other                  Error occurred during execution.

**/
EFI_STATUS
EFIAPI
UninstallBootCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle
  )
{
  EFI_STATUS                                Status;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  SETUP_UTILITY_CONFIGURATION                *SUCInfo;

  if (mBootCallBackInfo == NULL) {
    return EFI_SUCCESS;
  }
  Status = gBS->UninstallProtocolInterface (
                  DriverHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &mBootCallBackInfo->DriverCallback
                  );
  ASSERT_EFI_ERROR (Status);
  FreePool (mBootCallBackInfo);
  mBootCallBackInfo = NULL;

  BootDevInfoShutdown ();
  ShutdownBbsTypeTable ();

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (!EFI_ERROR (Status)) {
    SUCInfo = SuBrowser->SUCInfo;

    if (SUCInfo->LegacyBootDevType != NULL) {
      FreePool (SUCInfo->LegacyBootDevType);
      SUCInfo->LegacyBootDevType = NULL;
    }
    if (SUCInfo->BootOrder != NULL) {
      FreePool (SUCInfo->BootOrder);
      SUCInfo->BootOrder = NULL;
    }
    if (SUCInfo->BootPriority != NULL) {
      FreePool (SUCInfo->BootPriority);
      SUCInfo->BootPriority = NULL;
    }
  }

  return Status;
}
