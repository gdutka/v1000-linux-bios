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
#include "Version.h"

EFI_CALLBACK_INFO                         *mVersionCallBackInfo;

/**
  This is the callback function for the Version Menu.

  @param [in]   *This
  @param [in]   Action            A pointer to the data being sent to the original exporting driver.
  @param [in]   QuestionId
  @param [in]   Type
  @param [in]   *Value
  @param [in]   *ActionRequest

  @retval
**/
EFI_STATUS
VersionCallbackRoutine (
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
  SYSTEM_CONFIGURATION                  *MyIfrNVData;
  EFI_HII_HANDLE                        HiiHandle;
  EFI_CALLBACK_INFO                     *CallbackInfo;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL    *Interface;
  UINTN                                 BufferSize;

  *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  CallbackInfo = EFI_VERSION_CALLBACK_INFO_FROM_THIS (This);
  if (QuestionId == GET_SETUP_CONFIG || QuestionId == SET_SETUP_CONFIG) {
    BufferSize = sizeof (SYSTEM_CONFIGURATION);
    Status = SetupVariableConfig (
               NULL,
               NULL,
               BufferSize,
               (UINT8 *) gSUBrowser->SCBuffer,
               (BOOLEAN)(QuestionId == GET_SETUP_CONFIG)
               );
    return Status;
  }

  BufferSize = sizeof (SYSTEM_CONFIGURATION);
  Status = SetupVariableConfig (
             NULL,
             NULL,
             BufferSize,
             (UINT8 *) gSUBrowser->SCBuffer,
             TRUE
             );
  MyIfrNVData = (SYSTEM_CONFIGURATION *) gSUBrowser->SCBuffer;

  
  Status = gBS->LocateProtocol (
                 &gEfiSetupUtilityBrowserProtocolGuid,
                 NULL,
                 &Interface
                 );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status    = EFI_SUCCESS;
  StringPtr = NULL;
  HiiHandle = CallbackInfo->HiiHandle;

  switch (QuestionId) {

  default:
    Status = HotKeyCallBack (
               This,
               gSUBrowser->SUCInfo->MapTable[VersionHiiHandle].HiiHandle,
               Action,
               QuestionId,
               Type,
               Value,
               ActionRequest
               );

    break;
  }
  
  BufferSize = sizeof (SYSTEM_CONFIGURATION);
  SetupVariableConfig (
    NULL,
    NULL,
    BufferSize,
    (UINT8 *) gSUBrowser->SCBuffer,
    FALSE
    );

  return Status;
}

EFI_STATUS
InstallVersionCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle,
  IN EFI_HII_HANDLE                         HiiHandle
  )
{
  EFI_STATUS                                Status;

  mVersionCallBackInfo = AllocatePool (sizeof(EFI_CALLBACK_INFO));
  if (mVersionCallBackInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  mVersionCallBackInfo->Signature                    = EFI_VERSION_MENU_SIGNATURE;
  mVersionCallBackInfo->DriverCallback.ExtractConfig = gSUBrowser->ExtractConfig;
  mVersionCallBackInfo->DriverCallback.RouteConfig   = gSUBrowser->RouteConfig;
  mVersionCallBackInfo->DriverCallback.Callback      = VersionCallbackRoutine;
  mVersionCallBackInfo->HiiHandle = HiiHandle;

  //
  // Install protocol interface
  //
  Status = gBS->InstallProtocolInterface (
                  &DriverHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mVersionCallBackInfo->DriverCallback
                  );
  ASSERT_EFI_ERROR (Status);

  Status = InitVersionMenu (HiiHandle);

  return Status;
}

EFI_STATUS
UninstallVersionCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle
  )
{
  EFI_STATUS       Status;

  if (mVersionCallBackInfo == NULL) {
    return EFI_SUCCESS;
  }

  Status = gBS->UninstallProtocolInterface (
                  DriverHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &mVersionCallBackInfo->DriverCallback
                  );

  ASSERT_EFI_ERROR (Status);
  gBS->FreePool (mVersionCallBackInfo);
  mVersionCallBackInfo = NULL;
  return Status;
}

/**
  Convert CPU Current Operating Frequency Data to a string

  @param [in]    ProcessorFrequency        The CPU Current Operating frequency data to process
  @param [out]   String                    The string that is created

**/
EFI_STATUS
ConvertCoreFrequencyToString(
  IN  EFI_PROCESSOR_FSB_FREQUENCY_DATA  *ProcessorCoreFrequency,
  OUT CHAR16                            **String
  )
{
  CHAR16        *StringBuffer;
  UINTN         Index;
  UINT32        FreqMhz;

  if (ProcessorCoreFrequency->Exponent >= 6) {
    FreqMhz = ProcessorCoreFrequency->Value;
    for (Index = 0; Index < (UINTN) (ProcessorCoreFrequency->Exponent - 6); Index++) {
      FreqMhz *= 10;
    }
  } else {
    FreqMhz = 0;
  }

  StringBuffer = AllocateZeroPool (0x20);
  Index = UnicodeValueToString (StringBuffer, PREFIX_ZERO, FreqMhz, 4);
  StrCat (StringBuffer, L" MHz");

  *String = (CHAR16 *)StringBuffer;

  return EFI_SUCCESS;
}

EFI_STATUS
InitVersionMenu (
  IN EFI_HII_HANDLE                         HiiHandle
)
{
  EFI_STATUS                            Status;
  EFI_STRING_ID                         TokenToUpdate;
  CHAR16                                *NewString;
  UINT64                                MonotonicCount;
  EFI_DATA_HUB_PROTOCOL                 *DataHub;
  EFI_DATA_RECORD_HEADER                *Record;
  EFI_SUBCLASS_TYPE1_HEADER             *DataHeader;
  EFI_PROCESSOR_CORE_FREQUENCY_DATA     *ProcessorCoreFrequency;

  //
  // Update Front Page strings
  //
  Status = gBS->LocateProtocol (
                  &gEfiDataHubProtocolGuid,
                  NULL,
                  &DataHub
                  );

  MonotonicCount  = 0;
  Record = NULL;

  do {
    Status = DataHub->GetNextRecord (DataHub, &MonotonicCount, NULL, &Record);
    if (Record->DataRecordClass == EFI_DATA_RECORD_CLASS_DATA) {
      DataHeader  = (EFI_SUBCLASS_TYPE1_HEADER *) (Record + 1);

      //
      // CPU Current Operating Frequency
      //
      if (CompareGuid (&Record->DataRecordGuid, &gEfiProcessorSubClassGuid) &&
                 (DataHeader->RecordType == ProcessorCoreFrequencyRecordType)) {
        ProcessorCoreFrequency = (EFI_PROCESSOR_CORE_FREQUENCY_DATA *)(DataHeader + 1);
        ConvertCoreFrequencyToString (ProcessorCoreFrequency, &NewString);
        TokenToUpdate = (EFI_STRING_ID)STR_SYSTEM_CORE_FREQ_STRING2;
        HiiSetString (HiiHandle, TokenToUpdate, NewString, NULL);
        gBS->FreePool(NewString);
      }

    }
  } while (!EFI_ERROR(Status) && (MonotonicCount != 0));
  
  return EFI_SUCCESS;
}
