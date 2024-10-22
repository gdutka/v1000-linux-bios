/** @file

  Initial and callback functions for H2OUve Config utility DXE

;******************************************************************************
;* Copyright (c) 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <H2OUveConfigUtil.h>

EFI_HANDLE                                mDriverHandle = NULL;
H2OUVE_CONFIG_UTIL_PRIVATE_DATA           *mPrivateData = NULL;
CHAR16                                    mVariableName[] = H2OUVE_VARSTORE_NAME;
HII_VENDOR_DEVICE_PATH                    mHiiVendorDevicePath0 = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    H2OUVE_FORMSET_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};

/**
  This function allows a caller to extract the current configuration for one
  or more named elements from the target driver.

  @param  This                   Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param  Request                A null-terminated Unicode string in
                                 <ConfigRequest> format.
  @param  Progress               On return, points to a character in the Request
                                 string. Points to the string's null terminator if
                                 request was successful. Points to the most recent
                                 '&' before the first failing name/value pair (or
                                 the beginning of the string if the failure is in
                                 the first name/value pair) if the request was not
                                 successful.
  @param  Results                A null-terminated Unicode string in
                                 <ConfigAltResp> format which has all values filled
                                 in for the names in the Request string. String to
                                 be allocated by the called function.

  @retval EFI_SUCCESS            The Results is filled with the requested values.
  @retval EFI_OUT_OF_RESOURCES   Not enough memory to store the results.
  @retval EFI_INVALID_PARAMETER  Request is illegal syntax, or unknown name.
  @retval EFI_NOT_FOUND          Routing data doesn't match any storage in this
                                 driver.

**/
EFI_STATUS
EFIAPI
ExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  EFI_STATUS                       Status;
  UINTN                            BufferSize;
  EFI_HII_CONFIG_ROUTING_PROTOCOL  *HiiConfigRouting;
  EFI_STRING                       ConfigRequest;
  EFI_STRING                       ConfigRequestHdr;
  UINTN                            Size;
  BOOLEAN                          AllocatedRequest;
  CHAR16                           *StrPointer;

  DEBUG ((DEBUG_INFO, "%a:%d ExtractConfig\n", __FUNCTION__, __LINE__));

  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Initialize the local variables.
  //
  ConfigRequestHdr  = NULL;
  ConfigRequest     = NULL;
  Size              = 0;
  *Progress         = Request;
  AllocatedRequest  = FALSE;

  HiiConfigRouting = mPrivateData->HiiConfigRouting;

  //
  // Get Buffer Storage data from EFI variable.
  // Try to get the current setting from variable.
  //
  BufferSize = sizeof (VARIABLE_EDIT_FUNCTION_DISABLE_SETTING);
  Status = gRT->GetVariable (
                  mVariableName,
                  &gH2OUveVarstoreGuid,
                  NULL,
                  &BufferSize,
                  &mPrivateData->Configuration
                  );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  if (Request == NULL) {
    //
    // Request is set to NULL, construct full request string.
    //

    //
    // Allocate and fill a buffer large enough to hold the <ConfigHdr> template
    // followed by "&OFFSET=0&WIDTH=WWWWWWWWWWWWWWWW" followed by a Null-terminator
    //
    ConfigRequestHdr = HiiConstructConfigHdr (&gH2OUveVarstoreGuid, mVariableName, mPrivateData->DriverHandle);
    if (ConfigRequestHdr == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto EXIT;
    }
    Size = (StrLen (ConfigRequestHdr) + 32 + 1) * sizeof (CHAR16);
    ConfigRequest = AllocateZeroPool (Size);
    if (ConfigRequest == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto EXIT;
    }
    AllocatedRequest = TRUE;
    UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, (UINT64)BufferSize);
    FreePool (ConfigRequestHdr);
    ConfigRequestHdr = NULL;
  } else {
    //
    // Check routing data in <ConfigHdr>.
    // Note: if only one Storage is used, then this checking could be skipped.
    //
    if (!HiiIsConfigHdrMatch (Request, &gH2OUveVarstoreGuid, NULL)) {
      return EFI_NOT_FOUND;
    }
    //
    // Check whether request for EFI Varstore. EFI varstore get data
    // through hii database, not support in this path.
    //
    if (!HiiIsConfigHdrMatch (Request, &gH2OUveVarstoreGuid, mVariableName)) {
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
        ConfigRequest    = AllocateZeroPool (Size);
        if (ConfigRequest == NULL) {
          Status = EFI_OUT_OF_RESOURCES;
          goto EXIT;
        }
        AllocatedRequest = TRUE;
        UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", Request, (UINT64)BufferSize);
      }
    }
  }

  Status = HiiConfigRouting->BlockToConfig (
                               HiiConfigRouting,
                               ConfigRequest,
                               (UINT8 *)&mPrivateData->Configuration,
                               sizeof(VARIABLE_EDIT_FUNCTION_DISABLE_SETTING),
                               Results,
                               Progress
                               );
  if (!EFI_ERROR (Status)) {
    ConfigRequestHdr = HiiConstructConfigHdr (&gH2OUveVarstoreGuid, mVariableName, mPrivateData->DriverHandle);
  }

EXIT:

  //
  // Free the allocated config request string.
  //
  if (AllocatedRequest) {
    FreePool (ConfigRequest);
  }

  if (ConfigRequestHdr != NULL) {
    FreePool (ConfigRequestHdr);
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

  @param  This                   Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param  Configuration          A null-terminated Unicode string in <ConfigResp>
                                 format.
  @param  Progress               A pointer to a string filled in with the offset of
                                 the most recent '&' before the first failing
                                 name/value pair (or the beginning of the string if
                                 the failure is in the first name/value pair) or
                                 the terminating NULL if all was successful.

  @retval EFI_SUCCESS            The Results is processed successfully.
  @retval EFI_INVALID_PARAMETER  Configuration is NULL.
  @retval EFI_NOT_FOUND          Routing data doesn't match any storage in this
                                 driver.
**/
EFI_STATUS
EFIAPI
RouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
  VARIABLE_EDIT_FUNCTION_DISABLE_SETTING        VarEditFunDis;
  EFI_STATUS                                    Status;
  EFI_HII_CONFIG_ROUTING_PROTOCOL               *HiiConfigRouting;
  UINTN                                         BufferSize;

  DEBUG ((DEBUG_INFO, "%a:%d RouteConfig\n", __FUNCTION__, __LINE__));

  ZeroMem (&VarEditFunDis, sizeof (VARIABLE_EDIT_FUNCTION_DISABLE_SETTING));
  
  if (Configuration == NULL || Progress == NULL || This == NULL) {
   return EFI_INVALID_PARAMETER;
  }

  *Progress = Configuration;

  if (!HiiIsConfigHdrMatch(Configuration, &gH2OUveVarstoreGuid, mVariableName)) {
    return EFI_NOT_FOUND;
  }

  HiiConfigRouting = mPrivateData->HiiConfigRouting;
  BufferSize = sizeof (VARIABLE_EDIT_FUNCTION_DISABLE_SETTING);
//[-start-210115-IB12340256-modify]//
  Status = CommonGetVariable (
             mVariableName,
             &gH2OUveVarstoreGuid,
             &BufferSize,
             &VarEditFunDis
             );

  if (!EFI_ERROR(Status)) {
    Status = HiiConfigRouting->ConfigToBlock (
                                 HiiConfigRouting,
                                 Configuration,
                                 (UINT8 *) &VarEditFunDis,
                                 &BufferSize,
                                 Progress
                                 );
  }
//[-end-210115-IB12340256-modify]//
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Store broswer data Buffer Storage to EFI variable
  //
  Status = CommonSetVariable (
             mVariableName,
             &gH2OUveVarstoreGuid,
             EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
             sizeof (VARIABLE_EDIT_FUNCTION_DISABLE_SETTING),
             &VarEditFunDis
             );
  return Status;
  
}

/**
  This function processes the results of changes in configuration.

  @param  This                   Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param  Action                 Specifies the type of action taken by the browser.
  @param  QuestionId             A unique value which is sent to the original
                                 exporting driver so that it can identify the type
                                 of data to expect.
  @param  Type                   The type of value for the question.
  @param  Value                  A pointer to the data being sent to the original
                                 exporting driver.
  @param  ActionRequest          On return, points to the action requested by the
                                 callback function.

  @retval EFI_SUCCESS            The callback successfully handled the action.
  @retval EFI_OUT_OF_RESOURCES   Not enough storage is available to hold the
                                 variable and its data.
  @retval EFI_DEVICE_ERROR       The variable could not be saved.
  @retval EFI_UNSUPPORTED        The specified Action is not supported by the
                                 callback.

**/
EFI_STATUS
EFIAPI
DriverCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  DEBUG ((DEBUG_INFO, "%a:%d DriverCallback\n", __FUNCTION__, __LINE__));
  //
  // Do nothing, so return EFI_UNSUPPORTED.
  //
  return EFI_UNSUPPORTED;
}

/**
  Unloads the application and its installed protocol.

  @param[in]  ImageHandle       Handle that identifies the image to be unloaded.

  @retval EFI_SUCCESS           The image has been unloaded.
**/
EFI_STATUS
EFIAPI
SetupUnload (
  IN EFI_HANDLE  ImageHandle
  )
{
  if (mPrivateData != NULL) {
    if (mDriverHandle != NULL) {
      gBS->UninstallMultipleProtocolInterfaces (
             mDriverHandle,
             &gEfiDevicePathProtocolGuid,
             &mHiiVendorDevicePath0,
             &gEfiHiiConfigAccessProtocolGuid,
             &mPrivateData->ConfigAccess,
             NULL
             );
      mDriverHandle = NULL;
    }

    if (mPrivateData->HiiHandle != NULL) {
      HiiRemovePackages (mPrivateData->HiiHandle);
    }

    FreePool (mPrivateData);
    mPrivateData = NULL;
  }

  return EFI_SUCCESS;
}

/**
  Initialization for the Setup related functions.

  @param ImageHandle     Image handle this driver.
  @param SystemTable     Pointer to SystemTable.

  @retval EFI_SUCESS     This function always complete successfully.

**/
EFI_STATUS
EFIAPI
H2OUveConfigUtilDxeInitialize (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS                              Status;
  EFI_HII_HANDLE                          HiiHandle;
  EFI_HII_DATABASE_PROTOCOL               *HiiDatabase;
  EFI_HII_STRING_PROTOCOL                 *HiiString;
  EFI_HII_CONFIG_ROUTING_PROTOCOL         *HiiConfigRouting;
  CHAR16                                  *NewString;
  UINTN                                   BufferSize;
  VARIABLE_EDIT_FUNCTION_DISABLE_SETTING  *Configuration;
  BOOLEAN                                 ActionFlag;
  EFI_STRING                              ConfigRequestHdr;

  DEBUG ((DEBUG_INFO, "->H2OUveConfigUtilDxeInitialize\n"));

  //
  // Initialize the local variables.
  //
  ConfigRequestHdr = NULL;
  NewString        = NULL;

  //
  // Initialize driver private data
  //
  mPrivateData = AllocateZeroPool (sizeof (H2OUVE_CONFIG_UTIL_PRIVATE_DATA));
  if (mPrivateData == NULL) {
    DEBUG ((DEBUG_ERROR, "%a:%d EFI_OUT_OF_RESOURCES\n", __FUNCTION__, __LINE__));
    return EFI_OUT_OF_RESOURCES;
  }

  mPrivateData->Signature = H2OUVE_CONFIG_UTIL_PRIVATE_SIGNATURE;

  mPrivateData->ConfigAccess.ExtractConfig = ExtractConfig;
  mPrivateData->ConfigAccess.RouteConfig = RouteConfig;
  mPrivateData->ConfigAccess.Callback = DriverCallback;

  //
  // Locate Hii Database protocol
  //
  Status = gBS->LocateProtocol (&gEfiHiiDatabaseProtocolGuid, NULL, (VOID **) &HiiDatabase);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a:%d Status:%r\n", __FUNCTION__, __LINE__, Status));
    return Status;
  }
  mPrivateData->HiiDatabase = HiiDatabase;

  //
  // Locate HiiString protocol
  //
  Status = gBS->LocateProtocol (&gEfiHiiStringProtocolGuid, NULL, (VOID **) &HiiString);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a:%d Status:%r\n", __FUNCTION__, __LINE__, Status));
    return Status;
  }
  mPrivateData->HiiString = HiiString;

  //
  // Locate ConfigRouting protocol
  //
  Status = gBS->LocateProtocol (&gEfiHiiConfigRoutingProtocolGuid, NULL, (VOID **) &HiiConfigRouting);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a:%d Status:%r\n", __FUNCTION__, __LINE__, Status));
    return Status;
  }
  mPrivateData->HiiConfigRouting = HiiConfigRouting;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mDriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mHiiVendorDevicePath0,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &mPrivateData->ConfigAccess,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a:%d, Fail to install protocol interface, Status:%r\n", __FUNCTION__, __LINE__, Status));
  }

  mPrivateData->DriverHandle = mDriverHandle;

  //
  // Publish our HII data
  //
  HiiHandle = HiiAddPackages (
                &gH2OUveFormsetGuid,
                mDriverHandle,
                H2OUveConfigUtilDxeStrings,
                H2OUveConfigUtilConfigPageVfrBin,
                NULL
                );
  if (HiiHandle == NULL) {
    DEBUG ((DEBUG_ERROR, "%a:%d EFI_OUT_OF_RESOURCES\n", __FUNCTION__, __LINE__));
    return EFI_OUT_OF_RESOURCES;
  }

  mPrivateData->HiiHandle = HiiHandle;
  
  //
  // Initialize configuration data
  //
  Configuration = &mPrivateData->Configuration;
  ZeroMem (Configuration, sizeof (VARIABLE_EDIT_FUNCTION_DISABLE_SETTING));

  //
  // Try to read NV config EFI variable first
  //
  ConfigRequestHdr = HiiConstructConfigHdr (&gH2OUveVarstoreGuid, mVariableName, mDriverHandle);
  if (ConfigRequestHdr == NULL) {
    DEBUG ((DEBUG_ERROR, "%a:%d EFI_OUT_OF_RESOURCES\n", __FUNCTION__, __LINE__));
    return EFI_OUT_OF_RESOURCES;
  }

  BufferSize = sizeof (VARIABLE_EDIT_FUNCTION_DISABLE_SETTING);
  Status = gRT->GetVariable (mVariableName, &gH2OUveVarstoreGuid, NULL, &BufferSize, Configuration);

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a:%d, Fail to get variable, Status:%r\n"));
  } else {
    //
    // EFI variable does exist and Validate Current Setting
    //

    ActionFlag = HiiValidateSettings (ConfigRequestHdr);
    if (!ActionFlag) {
      DEBUG ((DEBUG_ERROR, "%a:%d Status:%r\n", __FUNCTION__, __LINE__, EFI_INVALID_PARAMETER));
      SetupUnload (ImageHandle);
      return EFI_INVALID_PARAMETER;
    }
  }
  FreePool (ConfigRequestHdr);

  DEBUG ((DEBUG_INFO, "<-H2OUveConfigUtilDxeInitialize\n"));

  return Status;
}
