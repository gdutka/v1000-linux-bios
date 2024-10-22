/** @file
  Exit menu relative functions

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

/**
  To discard user changed setup utility setting in this boot.

  @param This                    Pointer to Form Callback protocol instance.

  @retval EFI_SUCCESS            Function has completed successfully.
  @return Other                  Cannot get SetupUtility browser data or language data.

**/
EFI_STATUS
DiscardChange (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This
  )
{
  EFI_STATUS                                Status;
  UINT16                                    *BootOrderList = NULL;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL        *Interface;
  KERNEL_CONFIGURATION                      *MyIfrNVData;
  EFI_GUID                                  GuidId = SYSTEM_CONFIGURATION_GUID;
  UINTN                                     BufferSize;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo     = SuBrowser->SUCInfo;
  Interface   = &SuBrowser->Interface;

  Status = CommonGetVariableDataAndSize (
             L"Setup",
             &GuidId,
             &BufferSize,
             (VOID **) &MyIfrNVData
             );
  if (EFI_ERROR (Status) || BufferSize == 0) {
    return EFI_NOT_FOUND;
  }

  UpdateLangItem (This, (UINT8 *) (&(MyIfrNVData->Language)));

  CopyMem (
    SuBrowser->SCBuffer,
    MyIfrNVData,
    BufferSize
    );
  FreePool(MyIfrNVData);

  //
  //  System Password reset to the status when entering SCU
  //
  Status = ResetSysPasswordInfoForSCU ();

  //
  //  HDD Password reset to the real HDD status currently
  //
  Status = ResetHddPasswordInfoForSCU ();

  if (SUCInfo->AdvBootDeviceNum == 0){
    return EFI_SUCCESS;
  }

  BootOrderList = CommonGetVariableData (
                    L"BootOrder",
                    &gEfiGlobalVariableGuid
                    );
  if (BootOrderList != NULL) {
    CopyMem (
      SUCInfo->BootOrder,
      BootOrderList,
      SUCInfo->AdvBootDeviceNum * sizeof(UINT16)
      );

    FreePool(BootOrderList);
    BootConfigLoadDefault ();
    UpdateStringToken ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer);
  }

  Interface->Changed = TRUE;

  return EFI_SUCCESS;
}

/**
 Load Custom variable in current setup variable buffer.

 @param[in] SuBrowser             Pointer of browser data

 @retval EFI_SUCCESS              Load Custom variable successfully.
 @retval EFI_INVALID_PARAMETER    Input parameter is NULL.
 @retval EFI_NOT_FOUND            Can not find Custom variable.
**/
EFI_STATUS
LoadCustomVar (
  IN SETUP_UTILITY_BROWSER_DATA             *SuBrowser
  )
{
  EFI_STATUS                                Status;
  KERNEL_CONFIGURATION                      *CustomVar;
  KERNEL_CONFIGURATION                      *OrgKernelConfig;
  UINTN                                     BufferSize;

  if (SuBrowser == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = CommonGetVariableDataAndSize (
             L"Custom",
             &gSystemConfigurationGuid,
             &BufferSize,
             (VOID **) &CustomVar
             );
  if (EFI_ERROR (Status) || CustomVar == NULL) {
    return EFI_NOT_FOUND;
  }

  OrgKernelConfig = (KERNEL_CONFIGURATION *) SuBrowser->SCBuffer;

  CustomVar->Language        = OrgKernelConfig->Language;
  //
  //Update the System Password status is whther Exit
  //
  CustomVar->SetUserPass     = OrgKernelConfig->SetUserPass;
  CustomVar->UserAccessLevel = OrgKernelConfig->UserAccessLevel;
  CustomVar->SupervisorFlag  = OrgKernelConfig->SupervisorFlag;
  CustomVar->UserFlag        = OrgKernelConfig->UserFlag;
  //
  //Update the TPM device is whther Exit
  //
  CustomVar->TpmDeviceOk     = OrgKernelConfig->TpmDeviceOk;

  CopyMem (
    SuBrowser->SCBuffer,
    CustomVar,
    BufferSize
    );

  FreePool (CustomVar);

  return EFI_SUCCESS;
}

/**
 Save Custom variable from current setup variable buffer.

 @param[in] SuBrowser             Pointer of browser data

 @retval EFI_SUCCESS              Save Custom variable successfully.
 @retval EFI_INVALID_PARAMETER    Input parameter is NULL.
 @retval EFI_ABORTED              Setup variable size is zero.
 @retval Other                    Set variable failed.
**/
EFI_STATUS
SaveCustomVar (
  IN SETUP_UTILITY_BROWSER_DATA             *SuBrowser
  )
{
  UINTN                                     BufferSize;

  if (SuBrowser == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  BufferSize = GetSetupVariableSize ();
  if (BufferSize == 0) {
    return EFI_ABORTED;
  }

  return CommonSetVariable (
           L"Custom",
           &gSystemConfigurationGuid,
           EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
           BufferSize,
           (VOID *) SuBrowser->SCBuffer
           );
}

/**
 Load CustomPlatformLang variable in current setup variable buffer and system.

 @param[in] SuBrowser             Pointer of browser data

 @retval EFI_SUCCESS              Load CustomPlatformLang variable successfully.
 @retval EFI_INVALID_PARAMETER    Input parameter is NULL.
 @retval EFI_NOT_FOUND            Can not find CustomPlatformLang variable.
**/
EFI_STATUS
LoadCustomPlatformLangVar (
  IN SETUP_UTILITY_BROWSER_DATA             *SuBrowser
  )
{
  CHAR8                                     *LangStr;
  KERNEL_CONFIGURATION                      *KernelConfig;

  if (SuBrowser == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  LangStr = (CHAR8 *) CommonGetVariableData (L"CustomPlatformLang", &gEfiGenericVariableGuid);
  if (LangStr == NULL) {
    return EFI_NOT_FOUND;
  }

  KernelConfig = (KERNEL_CONFIGURATION *) SuBrowser->SCBuffer;
  GetLangIndex (LangStr, &KernelConfig->Language);
  UpdateLangItem (NULL, &KernelConfig->Language);

  FreePool (LangStr);
  return EFI_SUCCESS;
}

/**
 Save CustomPlatformLang variable from current system.

 @retval EFI_SUCCESS              Save CustomPlatformLang variable successfully.
 @retval Other                    Can not find PlatformLang variable or set variable failed.
**/
EFI_STATUS
SaveCustomPlatformLangVar (
  VOID
  )
{
  EFI_STATUS                                Status;
  UINTN                                     VarSize;
  UINT8                                     *VarData;

  Status = CommonGetVariableDataAndSize (
             L"PlatformLang",
             &gEfiGlobalVariableGuid,
             &VarSize,
             (VOID **) &VarData
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = CommonSetVariable (
             L"CustomPlatformLang",
             &gEfiGenericVariableGuid,
             EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
             VarSize,
             VarData
             );

  FreePool (VarData);
  return Status;
}

/**
 Load CustomBootOrder variable in current boot order buffer.

 @param[in] SuBrowser             Pointer of browser data

 @retval EFI_SUCCESS              Load CustomPlatformLang variable successfully.
 @retval EFI_INVALID_PARAMETER    Input parameter is NULL.
 @retval EFI_OUT_OF_RESOURCES     Allocate memory failed.
**/
EFI_STATUS
LoadCustomBootOrderVar (
  IN SETUP_UTILITY_BROWSER_DATA             *SuBrowser
  )
{
  EFI_STATUS                                Status;
  UINTN                                     CustomBootOrderSize;
  UINT8                                     *CustomBootOrder;
  UINT8                                     *CustomBootOrderPtr;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  CHAR16                                    *BootOrder;
  BOOT_ORDER_OPTION_HEAD                    *OptionInfo;
  UINT16                                    CustomOptionDevNameSize;
  CHAR16                                    *CustomOptionDevName;
  UINTN                                     CustomBootOptionNum;
  UINTN                                     BootOptionNum;
  UINTN                                     Index;
  UINTN                                     OptionIndex;
  UINTN                                     FoundOptionNum;
  UINT16                                    *ResultBootOrder;
  CHAR16                                    OptionName[20];
  UINT8                                     *BootOptionVar;
  UINT8                                     *TempPtr;
  CHAR16                                    *DeviceName;
  UINTN                                     DeviceNameSize;
  BOOLEAN                                   *CustomBootDevSwitch;

  if (SuBrowser == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  SUCInfo = SuBrowser->SUCInfo;
  if (SUCInfo->AdvBootDeviceNum == 0) {
    return EFI_SUCCESS;
  }

  Status = CommonGetVariableDataAndSize (
             L"CustomBootOrder",
             &gEfiGenericVariableGuid,
             &CustomBootOrderSize,
             (VOID **) &CustomBootOrder
             );
  if (EFI_ERROR (Status) || CustomBootOrder == NULL) {
    return EFI_SUCCESS;
  }

  BootOrder     = SUCInfo->BootOrder;
  BootOptionNum = SUCInfo->AdvBootDeviceNum;

  OptionInfo = AllocateZeroPool (BootOptionNum * sizeof(BOOT_ORDER_OPTION_HEAD));
  if (OptionInfo == NULL) {
    FreePool (CustomBootOrder);
    return EFI_OUT_OF_RESOURCES;
  }

  ResultBootOrder = AllocateZeroPool (BootOptionNum * sizeof(UINT16));
  if (ResultBootOrder == NULL) {
    FreePool (CustomBootOrder);
    FreePool (OptionInfo);
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < BootOptionNum; Index++) {
    UnicodeSPrint (OptionName, sizeof(OptionName), L"Boot%04x", BootOrder[Index]);
    BootOptionVar = CommonGetVariableData (
                      OptionName,
                      &gEfiGlobalVariableGuid
                      );
    if (BootOptionVar == NULL) {
      continue;
    }

    TempPtr = BootOptionVar;
    TempPtr += sizeof (UINT32) + sizeof (UINT16);
    DeviceName = (CHAR16 *) TempPtr;
    DeviceNameSize = StrSize (DeviceName);

    OptionInfo[Index].BootOption       = BootOrder[Index];
    OptionInfo[Index].FindFlag         = FALSE;
    OptionInfo[Index].DeviceNameLength = (UINT16 ) DeviceNameSize;
    OptionInfo[Index].DeviceName       = AllocateCopyPool (DeviceNameSize, DeviceName);
    if (OptionInfo[Index].DeviceName == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    FreePool(BootOptionVar);
  }

  FoundOptionNum      = 0;
  CustomBootOrderPtr  = CustomBootOrder;
  CustomBootOptionNum = *(UINTN *) CustomBootOrderPtr;
  CustomBootOrderPtr += sizeof(UINTN);

  for (Index = 0; Index < CustomBootOptionNum; Index++) {
    CustomOptionDevNameSize = *((UINT16 *) CustomBootOrderPtr);
    CustomBootOrderPtr     += sizeof(UINT16);
    CustomOptionDevName     = (CHAR16 *) CustomBootOrderPtr;
    CustomBootOrderPtr     += CustomOptionDevNameSize;

    ASSERT (CustomOptionDevNameSize == StrSize (CustomOptionDevName));
    if ((UINTN) (CustomBootOrderPtr - CustomBootOrder) > CustomBootOrderSize) {
      break;
    }

    for (OptionIndex = 0; OptionIndex < BootOptionNum; OptionIndex++) {
      if (!OptionInfo[OptionIndex].FindFlag &&
          (OptionInfo[OptionIndex].DeviceNameLength == CustomOptionDevNameSize) &&
          (CompareMem (OptionInfo[OptionIndex].DeviceName, CustomOptionDevName, CustomOptionDevNameSize) == 0)) {
        ResultBootOrder[FoundOptionNum++] = OptionInfo[OptionIndex].BootOption;
        OptionInfo[OptionIndex].FindFlag = TRUE;
        OptionInfo[OptionIndex].CustomBootOrderIndex = (UINT16) Index;
        BootDevInfoSetActiveValue (OptionInfo[OptionIndex].BootOption, TRUE);
      }
    }
  }

  if (CustomBootOrderSize >= ((UINTN) (CustomBootOrderPtr - CustomBootOrder)) + CustomBootOptionNum * sizeof(BOOLEAN)) {
    CustomBootDevSwitch = CustomBootOrderPtr;
    CustomBootOrderPtr += (CustomBootOptionNum * sizeof(BOOLEAN));

    for (Index = 0; Index < FoundOptionNum; Index++) {
      for (OptionIndex = 0; OptionIndex < BootOptionNum; OptionIndex++) {
        if (OptionInfo[OptionIndex].FindFlag &&
            OptionInfo[OptionIndex].BootOption == ResultBootOrder[Index]) {
          BootDevInfoSetActiveValue (
            OptionInfo[OptionIndex].BootOption,
            CustomBootDevSwitch[OptionInfo[OptionIndex].CustomBootOrderIndex]
            );
        }
      }
    }
  }

  if (FoundOptionNum < BootOptionNum) {
    for (Index = 0; Index < BootOptionNum; Index++) {
      if (!OptionInfo[Index].FindFlag) {
        ResultBootOrder[FoundOptionNum++] = OptionInfo[Index].BootOption;
        OptionInfo[Index].FindFlag = TRUE;
        BootDevInfoSetActiveValue (OptionInfo[Index].BootOption, TRUE);
      }
    }
  }

  if (FoundOptionNum <= BootOptionNum) {
    CopyMem (
      SUCInfo->BootOrder,
      ResultBootOrder,
      BootOptionNum * sizeof(UINT16)
      );
  }

  for (Index = 0; Index < BootOptionNum; Index++) {
    if (OptionInfo[Index].DeviceName != NULL) {
      FreePool (OptionInfo[Index].DeviceName);
    }
  }
  FreePool(OptionInfo);
  FreePool(CustomBootOrder);
  FreePool(ResultBootOrder);

  UpdateStringToken ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer);
  SyncBootOrderToBootConfig ();

  return EFI_SUCCESS;
}

/**
 Save CustomBootOrder variable from current boot order buffer.

 @param[in] SuBrowser             Pointer of browser data

 @retval EFI_SUCCESS              Save CustomPlatformLang variable successfully.
 @retval EFI_INVALID_PARAMETER    Input parameter is NULL.
 @retval EFI_OUT_OF_RESOURCES     Allocate memory failed.
 @retval Other                    Set variable failed.
**/
EFI_STATUS
SaveCustomBootOrderVar (
  IN SETUP_UTILITY_BROWSER_DATA             *SuBrowser
  )
{
  EFI_STATUS                                Status;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  UINT16                                    *BootOrder;
  UINTN                                     BootOptionNum;
  UINTN                                     BufferSize;
  UINTN                                     AllocateSize;
  UINT8                                     *CustomBootOrder;
  UINT8                                     *CustomBootOrderPtr;
  UINTN                                     CustomBootOrderSize;
  UINTN                                     Index;
  CHAR16                                    OptionName[20];
  UINT8                                     *BootOptionVar;
  UINT8                                     *TempPtr;
  UINTN                                     DescriptionSize;
  CHAR16                                    *Description;
  BOOLEAN                                   *CustomBootDevSwitch;
  UINTN                                     CustomBootDevSwitchCount;
  UINTN                                     CustomBootDevSwitchMaxSize;

  if (SuBrowser == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  SUCInfo = SuBrowser->SUCInfo;
  if (SUCInfo->AdvBootDeviceNum == 0) {
    return EFI_SUCCESS;
  }

  BootOrder       = SUCInfo->BootOrder;
  BootOptionNum   = SUCInfo->AdvBootDeviceNum;

  BufferSize      = CUSTOM_BOOT_ORDER_SIZE;
  CustomBootOrder = AllocateZeroPool (BufferSize);
  if (CustomBootOrder == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CustomBootDevSwitchCount   = 0;
  CustomBootDevSwitchMaxSize = sizeof(BOOLEAN) * BootOptionNum;
  CustomBootDevSwitch        = AllocateZeroPool (CustomBootDevSwitchMaxSize);
  if (CustomBootDevSwitch == NULL) {
    FreePool (CustomBootOrder);
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // CustomBootOrder variable is consist of
  //   (UINTN) Boot device number,
  //   description size and string of first boot option,
  //   ... ,
  //   description size and string of last boot option.
  //   active flag list of boot devices
  //
  CustomBootOrderPtr = CustomBootOrder;
  *((UINTN *) CustomBootOrderPtr) = BootOptionNum;
  CustomBootOrderSize = sizeof(UINTN);
  CustomBootOrderPtr += CustomBootOrderSize;

  for (Index = 0; Index < BootOptionNum; Index++) {
    UnicodeSPrint (OptionName, sizeof (OptionName), L"Boot%04x", BootOrder[Index]);
    BootOptionVar = CommonGetVariableData (
                      OptionName,
                      &gEfiGlobalVariableGuid
                      );
    if (BootOptionVar == NULL) {
      continue;
    }

    TempPtr = (UINT8 *) BootOptionVar;
    TempPtr += sizeof (UINT32) + sizeof (UINT16);
    Description = (CHAR16 *) TempPtr;
    DescriptionSize = StrSize (Description);

    CustomBootOrderSize = (UINTN) (CustomBootOrderPtr - CustomBootOrder);
    while (CustomBootOrderSize + sizeof(UINT16) + DescriptionSize + CustomBootDevSwitchMaxSize > BufferSize) {
      AllocateSize    = MAX (BufferSize * 2, BufferSize + sizeof(UINT16) + DescriptionSize + CustomBootDevSwitchMaxSize);
      CustomBootOrder = ReallocatePool (BufferSize, AllocateSize, CustomBootOrder);
      if (CustomBootOrder == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      BufferSize = AllocateSize;
      CustomBootOrderPtr = CustomBootOrder + CustomBootOrderSize;
    }

    *((UINT16 *) CustomBootOrderPtr) = (UINT16) DescriptionSize;
    CustomBootOrderPtr  += sizeof(UINT16);

    CopyMem (
      CustomBootOrderPtr,
      Description,
      DescriptionSize
      );
    CustomBootOrderPtr  += DescriptionSize;

    CustomBootDevSwitch[CustomBootDevSwitchCount++] = BootDevInfoGetActiveValue (BootOrder[Index]);

    FreePool (BootOptionVar);
  }

  if (CustomBootDevSwitchCount > 0) {
    CopyMem (
      CustomBootOrderPtr,
      CustomBootDevSwitch,
      CustomBootDevSwitchCount * sizeof(BOOLEAN)
      );
    CustomBootOrderPtr  += (CustomBootDevSwitchCount * sizeof(BOOLEAN));
  }

  CustomBootOrderSize = (UINTN) (CustomBootOrderPtr - CustomBootOrder);
  Status = CommonSetVariable (
             L"CustomBootOrder",
             &gEfiGenericVariableGuid,
             EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
             CustomBootOrderSize,
             CustomBootOrder
             );
  FreePool (CustomBootOrder);

  return Status;
}

/**
  To restore setup utility setting to user custom setting

  @param This                    Pointer to Form Callback protocol instance.

  @retval EFI_SUCCESS            Function has completed successfully.
  @return Other                  Cannot get SetupUtility browser data.

**/
EFI_STATUS
LoadCustomOption (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This
  )
{
  EFI_STATUS                                Status;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = LoadCustomVar (SuBrowser);
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  LoadCustomPlatformLangVar (SuBrowser);
  LoadCustomBootOrderVar (SuBrowser);

  return EFI_SUCCESS;
}

/**
  To save current setup utility setting to user custom setting

  @param This                    Pointer to Form Callback protocol instance.

  @retval EFI_SUCCESS            Function has completed successfully.
  @return Other                  Cannot save SetupUtility browser data.

**/
EFI_STATUS
SaveCustomOption (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This
  )
{
  EFI_STATUS                                Status;
  EFI_STATUS                                ResultStatus;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ResultStatus = EFI_SUCCESS;

  Status = SaveCustomPlatformLangVar ();
  if (EFI_ERROR (Status)) {
    ResultStatus = Status;
  }

  Status = SaveCustomVar (SuBrowser);
  if (EFI_ERROR (Status)) {
    ResultStatus = Status;
  }

  Status = SaveCustomBootOrderVar (SuBrowser);
  if (EFI_ERROR (Status)) {
    ResultStatus = Status;
  }

  return ResultStatus;
}

/**
  Make form browser to refresh form set. Update label data to update Hii form package.

  @retval EFI_SUCCESS            Success to trigger form browser refresh form set
  @retval EFI_OUT_OF_RESOURCES   Allocate op code data fail
  @return Other                  Update label fail

**/
EFI_STATUS
BrowserRefreshFormSet (
  VOID
  )
{
  EFI_STATUS                            Status;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  EFI_HII_HANDLE                        HiiHandle;
  VOID                                  *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL                    *StartLabel;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  HiiHandle = SuBrowser->SUCInfo->MapTable[ExitHiiHandle].HiiHandle;

  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  if (StartOpCodeHandle == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = TRIGGER_BROWSER_REFRESH_LABEL;

  Status = HiiUpdateForm (
             HiiHandle,
             NULL,
             ROOT_FORM_ID,
             StartOpCodeHandle,
             NULL
             );

  HiiFreeOpCodeHandle (StartOpCodeHandle);

  return Status;
}

