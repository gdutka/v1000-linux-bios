/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include "CbsSetupDxe.h"
#include <Library/ApobCommonServiceLib.h>
#include <Protocol/AmdApcbProtocol.h>
#include <Porting.h>
#include <Addendum/Apcb/Inc/RV/APCB.h>
#include <Library/PcdLib.h>

CHAR16 VariableName[] = CBS_SYSTEM_CONFIGURATION_NAME;

//
// Global Variables.
//

EFI_GUID mFormSetGuid = CBS_SYSTEM_CONFIGURATION_GUID;

HII_VENDOR_DEVICE_PATH  mCbsHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    CBS_SYSTEM_CONFIGURATION_GUID
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

STATIC CBS_SETUP_PROTOCOL mCbsSetupProtocolInstance=
{
  CbsSetupLoadDefault
};

EFI_STATUS
EFIAPI
CbsSetupLoadDefault (
  IN  CBS_SETUP_PROTOCOL   *This
  )
{
  return CbsSetupLoadDefaultFunc();
}

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
  CBS_DXE_PRIVATE_DATA             *Private;
  EFI_HII_CONFIG_ROUTING_PROTOCOL  *HiiConfigRouting;
  EFI_STRING                       ConfigRequestHdr;
  EFI_STRING                       ConfigRequest;
  BOOLEAN                          AllocatedRequest;
  UINTN                            Size;
  CHAR16                           *StrPointer;

  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Initialize the local variables.
  //
  ConfigRequestHdr = NULL;
  ConfigRequest    = NULL;
  Size             = 0;
  *Progress = Request;
  AllocatedRequest = FALSE;

  Private = CBS_DXE_PRIVATE_FROM_THIS (This);
  HiiConfigRouting = Private->HiiConfigRouting;

  //
  // Get Buffer Storage data from EFI variable.
  // Try to get the current setting from variable.
  //
  BufferSize = 0;
  Status = gRT->GetVariable (
            VariableName,
            &gCbsSystemConfigurationGuid,
            NULL,
            &BufferSize,
            Private->IfrData
            );
  if ((Status == EFI_BUFFER_TOO_SMALL) && (BufferSize == CbsVariableSize)) {
    Status = gRT->GetVariable (
              VariableName,
              &gCbsSystemConfigurationGuid,
              NULL,
              &BufferSize,
              Private->IfrData
              );
    if (EFI_ERROR (Status)) {
      // set all default value to &Private->IfrData
      CbsWriteDefalutValue(Private->IfrData);
    }
  } else {
    // set all default value to &Private->IfrData
    CbsWriteDefalutValue(Private->IfrData);
  }
  CbsComboIdentify(Private->IfrData);

  if (Request == NULL) {
    //
    // Request is set to NULL, construct full request string.
    // Allocate and fill a buffer large enough to hold the <ConfigHdr> template
    // followed by "&OFFSET=0&WIDTH=WWWWWWWWWWWWWWWW" followed by a Null-terminator
    //
    ConfigRequestHdr = HiiConstructConfigHdr (&gCbsSystemConfigurationGuid, VariableName, Private->DriverHandle);
    if (ConfigRequestHdr != NULL) {
      Size = (StrLen (ConfigRequestHdr) + 32 + 1) * sizeof (CHAR16);
      ConfigRequest = AllocateZeroPool (Size);
      if (ConfigRequest == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      AllocatedRequest = TRUE;
      UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, (UINT64)BufferSize);
      FreePool (ConfigRequestHdr);
      ConfigRequestHdr = NULL;
    }
  } else {
    //
    // Check routing data in <ConfigHdr>.
    // Note: if only one Storage is used, then this checking could be skipped.
    //
    if (!HiiIsConfigHdrMatch (Request, &mFormSetGuid, VariableName)) {
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

  Status = HiiConfigRouting->BlockToConfig (
                                HiiConfigRouting,
                                ConfigRequest,
                                Private->IfrData,
                                BufferSize,
                                Results,
                                Progress
                                );

  //
  // Free the allocated config request string.
  //
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
  EFI_STATUS                       Status;
  UINTN                            BufferSize;
  CBS_DXE_PRIVATE_DATA             *Private;
  EFI_HII_CONFIG_ROUTING_PROTOCOL  *HiiConfigRouting;

  if (Configuration == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }


  Private = CBS_DXE_PRIVATE_FROM_THIS (This);
  HiiConfigRouting = Private->HiiConfigRouting;
  *Progress = Configuration;

  //
  // Check routing data in <ConfigHdr>.
  // Note: if only one Storage is used, then this checking could be skipped.
  //
  if (!HiiIsConfigHdrMatch (Configuration, &gCbsSystemConfigurationGuid, VariableName)) {
    return EFI_NOT_FOUND;
  }

  //
  // Convert <ConfigResp> to buffer data by helper function ConfigToBlock()
  //
  BufferSize = CbsVariableSize;
  Status = HiiConfigRouting->ConfigToBlock (
                               HiiConfigRouting,
                               Configuration,
                               Private->IfrData,
                               &BufferSize,
                               Progress
                               );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Store Buffer Storage back to EFI variable
  //
  BufferSize = CbsVariableSize;
  Status = gRT->SetVariable(
                  VariableName,
                  &gCbsSystemConfigurationGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  BufferSize,
                  Private->IfrData
                  );
  if (!EFI_ERROR (Status)) {
    AmdSaveCbsConfigData(Private->IfrData);
  }

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
FormCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS                       Status;
  CBS_DXE_PRIVATE_DATA             *Private;
  UINTN                            BufferSize;

  if ((Action == EFI_BROWSER_ACTION_FORM_OPEN) || (Action == EFI_BROWSER_ACTION_FORM_CLOSE)) {
    return EFI_SUCCESS;
  }

  if ((Value == NULL) || (ActionRequest == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Type == EFI_IFR_TYPE_STRING) && (Value->string == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_SUCCESS;
  Private = CBS_DXE_PRIVATE_FROM_THIS (This);

  //
  // Retrive data from Browser
  //
  BufferSize = CbsVariableSize;
  if (!HiiGetBrowserData(&gCbsSystemConfigurationGuid,
                         VariableName,
                         BufferSize,
                         (UINT8*)Private->IfrData)){
    return EFI_NOT_FOUND;
  }

  Status = AmdCbsSetupCallbackRoutine(This, Action, QuestionId, Type, Value, ActionRequest, (UINT8*)Private->IfrData, Private->HiiHandle);

  if (!EFI_ERROR (Status)) {
    //
    // Pass changed uncommitted data back to Form Browser
    //
    HiiSetBrowserData(&gCbsSystemConfigurationGuid,
                      VariableName,
                      BufferSize,
                      (UINT8 *)Private->IfrData,
                      NULL);
  }

  return Status;
}
/**
  This function is used to get the persistent memory reserved size.

  @param  Private        CBS private data.
  @retval EFI_SUCESS     This function always complete successfully.

**/
EFI_STATUS
EFIAPI
CbsPersistentMemCheck (
  CBS_DXE_PRIVATE_DATA            *Private
  )
{
  EFI_STATUS                       Status;
  AMD_APCB_SERVICE_PROTOCOL        *pApcbDxeServiceProtocol;
  UINT32                           ReservedMemL=0;
  UINT32                           ReservedMemH=0;
  UINT8                            ApcbPurpose;

  pApcbDxeServiceProtocol = NULL;
  ApcbPurpose = 0;

  Status = gBS->LocateProtocol (&gAmdApcbDxeServiceProtocolGuid, NULL, &pApcbDxeServiceProtocol);
  if (!EFI_ERROR (Status)) {
    pApcbDxeServiceProtocol->ApcbGetToken32 (pApcbDxeServiceProtocol, &ApcbPurpose, APCB_TOKEN_CBS_CMN_PERSISTENT_SIZE_LOW, &ReservedMemL);
    pApcbDxeServiceProtocol->ApcbGetToken32 (pApcbDxeServiceProtocol, &ApcbPurpose, APCB_TOKEN_CBS_CMN_PERSISTENT_SIZE_HIGH, &ReservedMemH);
    DEBUG ((EFI_D_INFO, "CbsPersistentMemCheck: Reserved Persistent memory=0x%X\n", (((UINT64)ReservedMemH<<32)|ReservedMemL)));
    } else {
    DEBUG ((EFI_D_ERROR, "CbsPersistentMemCheck: Unable to locate APCB Protocol\n"));
  }

  return Status;
}
/**
  This function processes Cbs Setup Recovery.

  @param  Private        CBS private data.
  @retval EFI_SUCESS     This function always complete successfully.

**/
EFI_STATUS
EFIAPI
CbsSetupRecovery (
  CBS_DXE_PRIVATE_DATA            *Private
  )
{
  EFI_STATUS                       Status;
  BOOLEAN                          ApcbRecoveryFlag;
  UINTN                            BufferSize;

  Status = ApobGetApcbRecoveryFlag (&ApcbRecoveryFlag);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  if (ApcbRecoveryFlag) {
    DEBUG ((EFI_D_ERROR, "APCB Recovery enabled\n"));
    CbsWriteDefalutValue(Private->IfrData);
    CbsComboIdentify(Private->IfrData);

    //
    // Store Buffer Storage back to EFI variable
    //
    BufferSize = CbsVariableSize;
    Status = gRT->SetVariable(
                    VariableName,
                    &gCbsSystemConfigurationGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    BufferSize,
                    Private->IfrData
                    );
  }

  return Status;
}

/**
  This function processes loading CBS vaiable default value.

  @param  IfrData        data stream.
  @retval VOID

**/
VOID
InitializeCbsVarDefault(
  UINT8 *IfrData
  )
{
  EFI_STATUS   Status;
//[-start-221013-IB14860855-remove]//
//  UINT32       RecordOffset;
//[-end-221013-IB14860855-remove]//
  UINT8        *tempBuffer;
//[-start-221013-IB14860855-remove]//
//  UINT32       i;
//[-end-221013-IB14860855-remove]//
  UINTN        BufferSize;

  tempBuffer = NULL;
  BufferSize = 0;
  Status = gRT->GetVariable (
              VariableName,
              &gCbsSystemConfigurationGuid,
              NULL,
              &BufferSize,
              IfrData
              );
  if ((Status == EFI_BUFFER_TOO_SMALL) && (BufferSize == CbsVariableSize)) {
    Status = gRT->GetVariable (
                  VariableName,
                  &gCbsSystemConfigurationGuid,
                  NULL,
                  &BufferSize,
                  IfrData
                  );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "No CBS Variable, Force loading CBS variable default value.(Status=0x%x)\n", Status));
      CbsWriteDefalutValue(IfrData);
      CbsComboIdentify(IfrData);

      //
      // Store Buffer Storage back to EFI variable
      //
      //Arista Add
      DEBUG ((EFI_D_ERROR, "<AVM> Writing CBS default values..\n"));
      BufferSize = CbsVariableSize;
      Status = gRT->SetVariable(
                      VariableName,
                      &gCbsSystemConfigurationGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                      BufferSize,
                      IfrData
                      );
      //Arista Add
       if (EFI_ERROR (Status)) {
          DEBUG ((EFI_D_ERROR, "<AVM> Write variables failed!!\n")); 
       }
       else {
          DEBUG ((EFI_D_ERROR, "<AVM> Write variables succeeded!!\n"));
       }
    } else {
      tempBuffer = AllocateZeroPool (CbsVariableSize);
      if (tempBuffer != NULL) {
        CbsWriteDefalutValue(tempBuffer);
        DEBUG ((EFI_D_ERROR, "Found CBS Variable: oID=0x%08x, nID=0x%08x, Offset = 0x%08x\n",
                (UINT32) *(UINT32 *)IfrData,
                (UINT32) *(UINT32 *)tempBuffer,
                (UINT32) *(UINT32*)(IfrData + sizeof(UINT32))
                ));
        if ((UINT32) *(UINT32 *)IfrData != (UINT32) *(UINT32 *)tempBuffer) {
//[-start-221013-IB14860855-remove]//
//          RecordOffset = (UINT32) *(UINT32*)(IfrData + sizeof(UINT32));
//          for(i = 32; i < RecordOffset; i++) {
//            tempBuffer[i] = IfrData[i];
//          }
//          for(i = 0; i < CbsVariableSize; i++) {
//            IfrData[i] = tempBuffer[i];
//          }
//[-end-221013-IB14860855-remove]//
//[-start-221013-IB14860855-add]//
          *(UINT32*)IfrData = *(UINT32*)tempBuffer;
          CbsComboIdentify(IfrData);
//[-end-221013-IB14860855-add]//
          //
          // Store Buffer Storage back to EFI variable
          //
          Status = gRT->SetVariable(
                          VariableName,
                          &gCbsSystemConfigurationGuid,
                          EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                          CbsVariableSize,
                          IfrData
                          );
        }
        FreePool(tempBuffer);
      }
    }
  } else {
    DEBUG ((EFI_D_ERROR, "No CBS Variable, Force loading CBS variable default value.(OrgSize=0x%x, NewSize=0x%x)\n", BufferSize, CbsVariableSize));
    CbsWriteDefalutValue(IfrData);
    CbsComboIdentify(IfrData);

    //
    // Store Buffer Storage back to EFI variable
    //
    BufferSize = CbsVariableSize;
    Status = gRT->SetVariable(
                    VariableName,
                    &gCbsSystemConfigurationGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    BufferSize,
                    IfrData
                    );
  }
}

/**
  Main entry for this driver.

  @param ImageHandle     Image handle this driver.
  @param SystemTable     Pointer to SystemTable.

  @retval EFI_SUCESS     This function always complete successfully.

**/
EFI_STATUS
EFIAPI
CbsSetupDxeInit (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS                      Status;
  EFI_HII_DATABASE_PROTOCOL       *HiiDatabase;
  EFI_HII_STRING_PROTOCOL         *HiiString;
  EFI_HII_CONFIG_ROUTING_PROTOCOL *HiiConfigRouting;
  CBS_DXE_PRIVATE_DATA            *Private;

  if (CbsFamilyIdentify() == FALSE) {
    return EFI_SUCCESS;
  }

  Private = AllocateZeroPool (sizeof (CBS_DXE_PRIVATE_DATA));
  if (Private == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Private->Signature   = CBS_DXE_PRIVATE_SIGNATURE;

  Private->ConfigAccess.ExtractConfig = ExtractConfig;
  Private->ConfigAccess.RouteConfig   = RouteConfig;
  Private->ConfigAccess.Callback      = FormCallback;

  Private->IfrData = AllocateZeroPool (CbsVariableSize);
  ASSERT (Private->IfrData != NULL);

  InitializeCbsVarDefault (Private->IfrData);
  if (PcdGetBool(PcdAmdPmemFeatureEnable) == TRUE) {
    CbsPersistentMemCheck (Private);
  }
  //
  // Locate Hii Database protocol
  //
  Status = gBS->LocateProtocol (&gEfiHiiDatabaseProtocolGuid, NULL, (VOID **) &HiiDatabase);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Private->HiiDatabase = HiiDatabase;

  //
  // Locate HiiString protocol
  //
  Status = gBS->LocateProtocol (&gEfiHiiStringProtocolGuid, NULL, (VOID **) &HiiString);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Private->HiiString = HiiString;

  //
  // Locate ConfigRouting protocol
  //
  Status = gBS->LocateProtocol (&gEfiHiiConfigRoutingProtocolGuid, NULL, (VOID **) &HiiConfigRouting);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Private->HiiConfigRouting = HiiConfigRouting;

  //
  // Install Device Path Protocol and Config Access protocol to driver handle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Private->DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mCbsHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &Private->ConfigAccess,
                  &gCbsSetupProtocolGuid,
                  &mCbsSetupProtocolInstance,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  CbsSetupRecovery (Private);

  //
  // Publish our HII data
  //
  Private->HiiHandle = HiiAddPackages (
                         &gCbsSystemConfigurationGuid,
                         Private->DriverHandle,
                         CbsSetupLibInstanceStrings,
                         AmdCbsFormBin,
                         NULL
                         );

  if (Private->HiiHandle == NULL) {
    FreePool(Private);
    return EFI_OUT_OF_RESOURCES;
  }

  return EFI_SUCCESS;
}

