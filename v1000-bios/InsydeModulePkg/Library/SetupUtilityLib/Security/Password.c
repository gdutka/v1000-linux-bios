/** @file
  Password relaitve code, including HDD password and System password

;******************************************************************************
;* Copyright (c) 2012 - 2022, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "Password.h"
#include "Security.h"
#include <Protocol/DiskInfo.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <Library/OemGraphicsLib.h>
#include <Library/DxeChipsetSvcLib.h>
#include <Library/H2OCpLib.h>
#include <Library/SavedPasswordStringProcessLib.h>
#include <Guid/H2OCp.h>

TCG_PSID                           *mStoredPsid = NULL;
PASSWORD_CONFIGURATION             mPasswordConfig;
BOOLEAN                            mTakeOwnershipByUserPassword = FALSE;
BOOLEAN                            mSingleUserPassword = FALSE;
PASSWORD_INFORMATION               *mOrgPassword = NULL;

BOOLEAN                            mFreeze = TRUE;

STATIC
EFI_STATUS
CheckSetupPassword (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL *SysPasswordService,
  IN  SETUP_UTILITY_CONFIGURATION       *ScuInfo,
  IN  KERNEL_CONFIGURATION              *KernelConfig
  );


BOOLEAN
HaveSysPasswordCheck (
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL     *SysPasswordService  OPTIONAL
  );

EFI_STATUS
CleanSysPasswordCheck (
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL     *SysPasswordService  OPTIONAL
  );

CURRENT_USER_TYPE
GetSysPasswordCurrentUser (
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL     *SysPasswordService  OPTIONAL
  );

EFI_STATUS
SetSysPasswordCurrentUser (
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL     *SysPasswordService, OPTIONAL
  CURRENT_USER_TYPE                     CurrentUserType
  );

EFI_STATUS
UpdateAllHddPasswordFlag (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  HDD_PASSWORD_SCU_DATA             *HddPasswordScuData,
  IN  UINTN                             NumOfHdd
  )
{
  UINTN                                 Index;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  EFI_STATUS                            Status;


  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (HddPasswordScuData == NULL) {
    mPasswordConfig.SetAllHddPasswordFlag = 0;
    return EFI_SUCCESS;
  }

  if (NumOfHdd == 0) {
    mPasswordConfig.SetAllHddPasswordFlag = 0;
    return EFI_SUCCESS;
  }

  //
  // Set All HDD Password is not supported for any locked drive or TCG supported drive presented.
  //
  if (CheckAnyHddLocked (HddPasswordScuData, NumOfHdd) || CheckAnyTcgDrive (HddPasswordScuData, NumOfHdd)) {
    mPasswordConfig.SetAllHddPasswordFlag = 0;
  } else {
    mPasswordConfig.SetAllHddPasswordFlag = 1;
    mPasswordConfig.SetAllMasterHddPasswordFlag = 1;
    for (Index = 0; Index < NumOfHdd; Index++) {
      if (HddPasswordScuData[Index].Flag == CHANGE_PASSWORD ||
          HddPasswordScuData[Index].Flag == ENABLE_PASSWORD) {
        mPasswordConfig.SetAllHddPasswordFlag = 0;
        mPasswordConfig.SetAllMasterHddPasswordFlag = 0;
        break;
      }
    }
  }

  return EFI_SUCCESS;
}


EFI_STATUS
ValidateAllHddPassword (
  IN  BOOLEAN                           UserOrMaster,
  IN  HDD_PASSWORD_SCU_DATA             *HddPasswordScuData,
  IN  UINTN                             NumOfHdd
  )
{

  //
  //  Always return EFI_SUCCESS to display two password items for new password and confirm new password.
  //
  return EFI_SUCCESS;
}

EFI_STATUS
SetAllHddPassword (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  HDD_PASSWORD_SCU_DATA             *HddPasswordScuData,
  IN  BOOLEAN                           UserOrMaster,
  IN  UINTN                             NumOfHdd,
  IN  CHAR16                            *Password
  )
{
  UINTN                              Index;
  UINTN                              SetSecurityFlag;
  SETUP_UTILITY_BROWSER_DATA         *SuBrowser;
  EFI_STATUS                         Status;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    //
    //  Return EFI_NOT_AVAILABLE_YET or EFI_UNSUPPORTED to let browser terminate password flow.
    //
    return EFI_NOT_AVAILABLE_YET;
  }

  SetSecurityFlag = DISABLE_PASSWORD;
  mPasswordConfig.SetAllMasterHddPasswordFlag = FALSE;
  if ((Password[0] == 0)) {
    SetSecurityFlag = DISABLE_PASSWORD;
    mPasswordConfig.SetAllMasterHddPasswordFlag = FALSE;
  } else {
    SetSecurityFlag = CHANGE_PASSWORD;
  }

  if (UserOrMaster == USER_PSW) {
    for (Index = 0; Index < NumOfHdd; Index++) {
      HddPasswordScuData[Index].Flag = SetSecurityFlag;

      ZeroMem (
        HddPasswordScuData[Index].InputString,
        (HddPasswordScuData[Index].StringLength * sizeof (CHAR16))
        );

      if ((Password[0] == 0)) {
        HddPasswordScuData[Index].MasterFlag = SetSecurityFlag;

        HddPasswordScuData[Index].StringLength = 0;
      } else {
        mPasswordConfig.SetAllMasterHddPasswordFlag = FALSE;
        HddPasswordScuData[Index].MasterFlag = MASTER_PASSWORD_GRAYOUT;

        HddPasswordScuData[Index].StringLength = StrLen (Password);

        StrCpyS (
          HddPasswordScuData[Index].InputString,
          sizeof(HddPasswordScuData[Index].InputString) / sizeof(CHAR16),
          Password
          );
      }


    }
  } else {
    for (Index = 0; Index < NumOfHdd; Index++) {
      HddPasswordScuData[Index].MasterFlag = SetSecurityFlag;

      ZeroMem (
        HddPasswordScuData[Index].MasterInputString,
        (HddPasswordScuData[Index].StringLength * sizeof (CHAR16))
        );
      if ((Password[0] == 0)) {
        HddPasswordScuData[Index].Flag = SetSecurityFlag;

        HddPasswordScuData[Index].StringLength = 0;
      } else {
        HddPasswordScuData[Index].StringLength = StrLen (Password);

        StrCpyS (
          HddPasswordScuData[Index].MasterInputString,
          sizeof(HddPasswordScuData[Index].MasterInputString) / sizeof(CHAR16),
          Password
          );
      }

    }
  }

  UpdateAllHddPasswordFlag (
    HiiHandle,
    HddPasswordScuData,
    NumOfHdd
    );

  //
  //  Return EFI_SUCCESS if password was accepted and saved.
  //
  return EFI_SUCCESS;
}

/**
  Set password to all harddisk

  @param  FormCallback           The form call back protocol.
  @param  MasterOrUser           HDD password or master HDD password.
  @param  HiiHandle              Return string token of device status.
  @param  Type                   Return string token of device status.
  @param  Value
  @param  ActionRequest
  @param  PState                 Password access is success or not, if access success then return TRUE.
  @param  HddPasswordDataInfo    Pointer to the HDD password buffer
  @param  NumOfHddPswd           Number of Harddisk

  @retval EFI_SUCCESS            Password callback success.
  @retval EFI_ALREADY_STARTED    Already password in SetupUtility.
  @retval EFI_NOT_READY          Password confirm error.

**/
EFI_STATUS
AllHddPasswordCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL    *FormCallback,
  IN  BOOLEAN                                 UserOrMaster,
  IN  EFI_HII_HANDLE                          HiiHandle,
  IN  UINT8                                   Type,
  IN  EFI_IFR_TYPE_VALUE                      *Value,
  OUT EFI_BROWSER_ACTION_REQUEST              *ActionRequest,
  OUT BOOLEAN                                 *PState,
  IN  HDD_PASSWORD_SCU_DATA                   *HddPasswordScuData,
  IN  UINTN                                   NumOfHdd
  )
{

  STATIC UINTN             PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
  EFI_STATUS               Status;
  CHAR16                   *Password;

  *PState = FALSE;
  if (Type != EFI_IFR_TYPE_STRING) {
    return  EFI_INVALID_PARAMETER;
  }

  if (Value->string == 0) {
    //
    // If Value->String == 0, only need reset the state machine to BROWSER_STATE_VALIDATE_PASSWORD
    //
    PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
    return EFI_SUCCESS;
  }


  Password = HiiGetString (HiiHandle, Value->string, NULL);
  //
  // When try to set a new password, user will be chanlleged with old password.
  // The Callback is responsible for validating old password input by user,
  // If Callback return EFI_SUCCESS, it indicates validation pass.
  //
  switch (PasswordState) {

  case BROWSER_STATE_VALIDATE_PASSWORD:
    Status = ValidateAllHddPassword (
               UserOrMaster,
               HddPasswordScuData,
               NumOfHdd
               );
    if (Status == EFI_SUCCESS) {
      PasswordState = BROWSER_STATE_SET_PASSWORD;
    }
    break;

  case BROWSER_STATE_SET_PASSWORD:
    Status = SetAllHddPassword (
               HiiHandle,
               HddPasswordScuData,
               UserOrMaster,
               NumOfHdd,
               Password
               );
    *PState = TRUE;
    PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
    break;

  default:
    Status = EFI_NOT_AVAILABLE_YET;
    break;
  }

  if (Password != NULL) {
    FreePool (Password);
  }

  return Status;
}

/**
 Initialize and check system password.

 @param[in] ScuInfo                Point to SETUP_UTILITY_CONFIGURATION instance.
 @param[in] KernelConfig           The kernel configuration data.
 @param[in] HiiHandle              HII handle of Security page.

 @retval EFI_SUCCESS               Initialize and check system password success.
 @retval EFI_UNSUPPORTED           There is no SysPasswordService instance on system.
 @retval Other                     Allocate memory failed.
**/
EFI_STATUS
SetupUtilityLibPasswordCheck (
  IN SETUP_UTILITY_CONFIGURATION          *ScuInfo,
  IN KERNEL_CONFIGURATION                 *KernelConfig,
  IN EFI_HII_HANDLE                       HiiHandle
  )
{
  EFI_STATUS                              Status;
  CHAR16                                  *NewString;
  UINTN                                   NumOfSupervisorPswdInfo;
  UINTN                                   NumOfUserPswdInfo;
  SYS_PASSWORD_INFO_DATA                  *SysPasswordInfoData;
  SETUP_UTILITY_BROWSER_DATA              *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  NumOfSupervisorPswdInfo = 0;
  NumOfUserPswdInfo       = 0;

  if (ScuInfo->SysPasswordService == NULL) {
    return EFI_UNSUPPORTED;
  }

  SysPasswordInfoData = NULL;
  //
  // Get Supervisor password
  //
  SysPasswordInfoData = AllocatePool (sizeof (SYS_PASSWORD_INFO_DATA));
  if (SysPasswordInfoData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = ScuInfo->SysPasswordService->GetStatus (
                                          ScuInfo->SysPasswordService,
                                          SystemSupervisor
                                          );
  if (!EFI_ERROR (Status)) {
    NumOfSupervisorPswdInfo = 1;
  }

  SysPasswordInfoData->Flag           = NO_ACCESS_PASSWORD;
  SysPasswordInfoData->StringLength   = 0x00;
  SysPasswordInfoData->NumOfEntry     = NumOfSupervisorPswdInfo;
  SysPasswordInfoData->SystemPasswordStatus = Status;
  SysPasswordInfoData->InputString = AllocatePool ((SysPasswordMaxNumber (ScuInfo->SysPasswordService) + 1) * sizeof (CHAR16));
  if (SysPasswordInfoData->InputString == NULL) {
    FreePool (SysPasswordInfoData);
    return EFI_OUT_OF_RESOURCES;
  }

   ZeroMem (
     SysPasswordInfoData->InputString,
     ((SysPasswordMaxNumber (ScuInfo->SysPasswordService) + 1) * sizeof (CHAR16))
     );

  ScuInfo->SupervisorPassword = SysPasswordInfoData;

  //
  // Get User password
  //
  if (PcdGetBool (PcdSysPasswordSupportUserPswd)) {
    SysPasswordInfoData = AllocatePool (sizeof (SYS_PASSWORD_INFO_DATA));
    if (SysPasswordInfoData == NULL) {
      FreePool (ScuInfo->SupervisorPassword);
      FreePool (ScuInfo->SupervisorPassword->InputString);
      return EFI_OUT_OF_RESOURCES;
    }

    Status = ScuInfo->SysPasswordService->GetStatus (
                                            ScuInfo->SysPasswordService,
                                            SystemUser
                                            );
    if (!EFI_ERROR (Status)) {
      NumOfUserPswdInfo = 1;
    }

    SysPasswordInfoData->Flag           = NO_ACCESS_PASSWORD;
    SysPasswordInfoData->StringLength   = 0x00;
    SysPasswordInfoData->NumOfEntry     = NumOfUserPswdInfo;
    SysPasswordInfoData->SystemPasswordStatus = Status;
    SysPasswordInfoData->InputString = AllocatePool ((SysPasswordMaxNumber (ScuInfo->SysPasswordService) + 1) * sizeof (CHAR16));
    if (SysPasswordInfoData->InputString == NULL) {
      FreePool (ScuInfo->SupervisorPassword->InputString);
      FreePool (ScuInfo->SupervisorPassword);
      FreePool (SysPasswordInfoData);
      return EFI_OUT_OF_RESOURCES;
    }

     ZeroMem (
       SysPasswordInfoData->InputString,
       ((SysPasswordMaxNumber (ScuInfo->SysPasswordService) + 1) * sizeof (CHAR16))
       );

    ScuInfo->UserPassword = SysPasswordInfoData;

    if ((NumOfUserPswdInfo != 0) || (NumOfSupervisorPswdInfo != 0)) {
      //
      // OemServices
      //
      DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcPasswordEntryCallBack \n"));
      Status = OemSvcPasswordEntryCallBack ();
      DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcPasswordEntryCallBack Status: %r\n", Status));
    }
  } else {
    if (NumOfSupervisorPswdInfo != 0) {
      //
      // OemServices
      //
      DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcPasswordEntryCallBack \n"));
      Status = OemSvcPasswordEntryCallBack ();
      DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcPasswordEntryCallBack Status: %r\n", Status));
    }
  }

  if (ScuInfo->SupervisorPassword->NumOfEntry == 0) {
    //
    // No set Supervisor password
    //
    NewString = HiiGetString (HiiHandle, STRING_TOKEN (STR_NOT_INSTALLED_TEXT), NULL);
    UpdateSupervisorTags (
      HiiHandle,
      FALSE,
      NewString,
      KernelConfig
      );
    ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->SupervisorFlag = 0;
    ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->UserFlag = 0;
    SuBrowser->SUCInfo->SupervisorPwdFlag = FALSE;
    SuBrowser->SUCInfo->UserPwdFlag       = FALSE;
    FreePool (NewString);
    Status = ScuInfo->SysPasswordService->UnlockPassword (ScuInfo->SysPasswordService, NULL, 0);
    return EFI_SUCCESS;
  }

  if (HaveSysPasswordCheck (ScuInfo->SysPasswordService)) {
    DisableQuietBoot ();
    gST->ConOut->EnableCursor (gST->ConOut, TRUE);
    Status = CheckSetupPassword (
               HiiHandle,
               ScuInfo->SysPasswordService,
               ScuInfo,
               KernelConfig
               );
    CleanSysPasswordCheck (ScuInfo->SysPasswordService);
  } else {
    if (GetSysPasswordCurrentUser (ScuInfo->SysPasswordService) == SystemSupervisorType) {
      KernelConfig->SetUserPass = FALSE;
    } else {
      KernelConfig->SetUserPass = TRUE;
    }
  }

  NewString = HiiGetString (HiiHandle, STRING_TOKEN (STR_INSTALLED_TEXT), NULL);
  UpdateSupervisorTags (
    HiiHandle,
    TRUE,
    NewString,
    KernelConfig
    );
  ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->SupervisorFlag = 1;
  SuBrowser->SUCInfo->SupervisorPwdFlag = TRUE;

  if (PcdGetBool (PcdSysPasswordSupportUserPswd)) {
    if (ScuInfo->UserPassword->NumOfEntry == 0) {
      ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->UserFlag = 0;
      SuBrowser->SUCInfo->UserPwdFlag = FALSE;
    } else {
      ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->UserFlag = 1;
      SuBrowser->SUCInfo->UserPwdFlag = TRUE;
    }

    if (ScuInfo->UserPassword->NumOfEntry != 0) {
      UpdateUserTags (
        HiiHandle,
        TRUE,
        NewString,
        KernelConfig
        );
    }
  }
  FreePool (NewString);

  return EFI_SUCCESS;
}

/**
  Check system pasword status.

  @param  ScuInfo                Point to SETUP_UTILITY_CONFIGURATION instance.
  @param  KernelConfig           The kernel configuration data.

  @retval EFI_SUCCESS            Check system password success.
  @retval EFI_NOT_FOUND          Not found system password.

**/
EFI_STATUS
PasswordCheck (
  IN  SETUP_UTILITY_CONFIGURATION       *ScuInfo,
  IN  KERNEL_CONFIGURATION              *KernelConfig
  )
{
  if (mSecurityCallBackInfo == NULL && GetBootModeHob () == BOOT_ON_S4_RESUME) {
    //
    // When not in SCU, skip password check if boot mode is S4.
    //
    return EFI_SUCCESS;
  }

  return SetupUtilityLibPasswordCheck (ScuInfo, KernelConfig, ScuInfo->MapTable[SecurityHiiHandle].HiiHandle);
}

/**
  Update supervisor tag.

  @param  HiiHandle              Return string token of device status.
  @param  Installed              TRUE installed supervisor password.
  @param  NewString              String buffer.
  @param  KernelConfig           The kernel configuration data.

  @retval EFI_SUCCESS            Always return.

**/
EFI_STATUS
UpdateSupervisorTags (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  BOOLEAN                           Installed,
  IN  CHAR16                            *NewString,
  IN  KERNEL_CONFIGURATION              *KernelConfig
  )
{
  EFI_STRING_ID                         TokenToUpdate;

  if (Installed) {
    KernelConfig->SupervisorFlag = 1;
  } else {
    KernelConfig->SupervisorFlag = 0;
    if (PcdGetBool (PcdSysPasswordSupportUserPswd)) {
      UpdateUserTags (HiiHandle, FALSE, NewString, KernelConfig);
    }
  }

  TokenToUpdate = STRING_TOKEN (STR_SUPERVISOR_PASSWORD_STRING2);
  HiiSetString (HiiHandle, TokenToUpdate, NewString, NULL);

  return EFI_SUCCESS;
}

/**
  Update user password tag.

  @param  HiiHandle              Return string token of device status.
  @param  Installed              TRUE installed supervisor password.
  @param  NewString              String buffer.
  @param  KernelConfig           The kernel configuration data.

  @retval EFI_SUCCESS            Always return.

**/
EFI_STATUS
UpdateUserTags (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  BOOLEAN                           Installed,
  IN  CHAR16                            *NewString,
  IN  KERNEL_CONFIGURATION              *KernelConfig
  )
{
  EFI_STRING_ID                         TokenToUpdate;

  if (Installed) {
    KernelConfig->UserFlag = TRUE;
  } else {
    KernelConfig->UserFlag = FALSE;
  }

  TokenToUpdate = STRING_TOKEN (STR_USER_PASSWORD_STRING2);
  HiiSetString (HiiHandle, TokenToUpdate, NewString, NULL);

  return EFI_SUCCESS;
}

/**
  Internal function to initialize enter incorrect password checkpoint data and trigger checkpoint.

  @retval EFI_SUCCESS             Trigger enter incorrect password checkpoint successfully
  @retval EFI_OUT_OF_RESOURCES    Allocate memory to initialize checkpoint data failed.
**/
STATIC
EFI_STATUS
TriggerEnterIncorrectSetupPassword (
  IN OUT  UINTN              *PasswordRetryCount,
  IN      EFI_STATUS         CheckPasswordStatus    
  )
{
  H2O_BDS_CP_ENTER_INCORRECT_SETUP_PASSWORD_DATA         IncorrectSetupPassword;
  EFI_STATUS                                             Status;

  IncorrectSetupPassword.Size                  = sizeof (H2O_BDS_CP_ENTER_INCORRECT_SETUP_PASSWORD_DATA);
  IncorrectSetupPassword.Status                = H2O_CP_TASK_NORMAL;
  IncorrectSetupPassword.PasswordRetryCount    = *PasswordRetryCount;
  IncorrectSetupPassword.CheckPasswordStatus   = CheckPasswordStatus;
  DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpEnterIncorrectSetupPasswordGuid));
  Status = H2OCpTrigger (&gH2OBdsCpEnterIncorrectSetupPasswordGuid, &IncorrectSetupPassword);
  DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", IncorrectSetupPassword.Status));
  if (IncorrectSetupPassword.Status == H2O_CP_TASK_UPDATE) {
    *PasswordRetryCount = IncorrectSetupPassword.PasswordRetryCount;
  }
  return Status;
}

/**
  Callback function for dialog freeze event.

  @param[in]  Event     Event whose notification function is being invoked.
  @param[in]  Context   The pointer to the notification function's context,
                        which is implementation-dependent.
**/
VOID
DialogFreezeNotify (
  IN  EFI_EVENT   Event,
  IN  VOID        *Context
  )
{
  DEBUG ((EFI_D_INFO, "%a() - Start\n", __FUNCTION__));
  mFreeze = FALSE;
  SendShutDNotifyForInternal ();
  DEBUG ((EFI_D_INFO, "%a() - End\n", __FUNCTION__));
}

/**
  Internal function to initialize enter incorrect password checkpoint data and trigger checkpoint.

  @retval EFI_SUCCESS             Trigger enter incorrect password checkpoint successfully
  @retval EFI_OUT_OF_RESOURCES    Allocate memory to initialize checkpoint data failed.
  @retval EFI_INVALID_PARAMETER   FailPolicy incorrect.
**/
STATIC
EFI_STATUS
TriggerCheckSetupPasswordFail (
  IN OUT  UINTN                                 *PasswordRetryCount,
  IN      SETUP_UTILITY_BROWSER_DATA            *SuBrowser,
  IN      EFI_HII_HANDLE                        HiiHandle
  )
{
  H2O_BDS_CP_CHECK_SETUP_PASSWORD_FAIL_DATA         PasswordFailData;
  EFI_INPUT_KEY                                     InputKey;
  EFI_STATUS                                        Status;
  CHAR16                                            *StringPtr = NULL;
  EFI_EVENT                                         TimerEvent = NULL;  

  DEBUG ((EFI_D_INFO, "%a() - Start\n", __FUNCTION__));
  PasswordFailData.Size                  = sizeof (H2O_BDS_CP_ENTER_INCORRECT_SETUP_PASSWORD_DATA);
  PasswordFailData.Status                = H2O_CP_TASK_NORMAL;
  PasswordFailData.PasswordRetryCount    = 3;
  PasswordFailData.FailPolicy            = PolicyColdReset;
  PasswordFailData.FreezeTime            = 180;

  DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpCheckSetupPasswordFailGuid));
  Status = H2OCpTrigger (&gH2OBdsCpCheckSetupPasswordFailGuid, &PasswordFailData);
  DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", PasswordFailData.Status));
  switch (PasswordFailData.Status) {

  case H2O_CP_TASK_UPDATE:
    mFreeze = TRUE;
    //
    // Update retry count and set a minimum time for user.
    //
    StringPtr = HiiGetString (HiiHandle, STRING_TOKEN (STR_SYS_FREEZE_DIALOG), NULL);
    Status = gBS->CreateEvent (
                    EVT_TIMER | EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    DialogFreezeNotify, 
                    NULL,
                    &TimerEvent
                    );  
    if (EFI_ERROR (Status)) {
      goto Exit;
    }

    Status = gBS->SetTimer (
                    TimerEvent,
                    TimerRelative,
                    EFI_TIMER_PERIOD_SECONDS (PasswordFailData.FreezeTime)
                    );
    if (EFI_ERROR (Status)) {
      gBS->CloseEvent (TimerEvent);
      goto Exit;
    }

    do {
      SuBrowser->H2ODialog->ConfirmDialog (
                                2,
                                FALSE,
                                40,
                                NULL,
                                &InputKey,
                                StringPtr
                                );
    } while (mFreeze);
    gBS->CloseEvent (TimerEvent);
    *PasswordRetryCount  = PasswordFailData.PasswordRetryCount; 
  break;
  
  case H2O_CP_TASK_SKIP:
    if (PasswordFailData.FailPolicy>= PolicyNoAction) {
      DEBUG ((DEBUG_INFO, "Checkpoint: %g, PasswordFailData.FailPolicy error\n", &gH2OBdsCpCheckSetupPasswordFailGuid));
      Status = EFI_INVALID_PARAMETER;
      break;
    }

    if ((PasswordFailData.FailPolicy == PolicyColdReset) ||
        (PasswordFailData.FailPolicy == PolicyWarmReset)) {
      StringPtr = HiiGetString (HiiHandle, STRING_TOKEN (STR_SYS_FAILE_TOO_MANY_REBOOT), NULL);
    } else if (PasswordFailData.FailPolicy == PolicyShutdown) {
      StringPtr = HiiGetString (HiiHandle, STRING_TOKEN (STR_SYS_FAILE_TOO_MANY_SHUTDOWN), NULL);
    }
    //
    // Do the reset or shutdown
    //
    SuBrowser->H2ODialog->ConfirmDialog (
                              2,
                              FALSE,
                              40,
                              NULL,
                              &InputKey,
                              StringPtr
                              );
    if (StringPtr != NULL) {
      FreePool (StringPtr);
    }                                  
    gRT->ResetSystem (PasswordFailData.FailPolicy, EFI_SECURITY_VIOLATION, 0, NULL);
  break;

  }
Exit:
  if (StringPtr != NULL) {
    FreePool (StringPtr);
  }
  DEBUG ((EFI_D_INFO, "%a() - End, Status = %r\n", __FUNCTION__));
  return Status;
}

/**
  Check system pasword status.

  @param  HiiHandle              Return string token of device status.
  @param  SysPasswordService     System Password protocol service.
  @param  ScuInfo
  @param  SetupVariable          The Setup NV data.

  @retval EFI_SUCCESS            Check system password success.
  @retval EFI_NOT_FOUND          Not found system password.

**/
STATIC
EFI_STATUS
CheckSetupPassword (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL *SysPasswordService,
  IN  SETUP_UTILITY_CONFIGURATION       *ScuInfo,
  IN  KERNEL_CONFIGURATION              *KernelConfig
  )
{
  EFI_STATUS                            Status;
  EFI_INPUT_KEY                         Key;
  CHAR16                                *PasswordString;
  CHAR16                                *StringPtr;
  CHAR16                                *ErrorStatusMsg;
  CHAR16                                *ThreeIncorrectMsg;
  CHAR16                                *RebootMsg;
  UINTN                                 TryPasswordTimes;
  BOOLEAN                               DrawFlag;
  BOOLEAN                               ReadChar;
  UINT16                                Password;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  CHAR16                                *SysInvalidPassword;
  CURRENT_USER_TYPE                     CurrentUserType;

  if (SysPasswordService == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (ScuInfo->SupervisorPassword->NumOfEntry == 0) {
    return EFI_SUCCESS;
  }

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }


  Password = 0;
  DrawFlag = FALSE;
  ReadChar = TRUE;
  TryPasswordTimes = (UINTN)PcdGet8 (PcdMaxCheckPasswordCount);
  StringPtr = AllocatePool ((SysPasswordMaxNumber (SysPasswordService) + 1) * sizeof (CHAR16));
  if (StringPtr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (FeaturePcdGet (PcdH2OBdsCpEnterIncorrectSetupPasswordSupported)) {
    DEBUG ((DEBUG_INFO, "Enter TriggerEnterIncorrectSetupPassword\n"));
    TriggerEnterIncorrectSetupPassword (&TryPasswordTimes, Status);
    DEBUG ((DEBUG_INFO, "Exit TriggerEnterIncorrectSetupPassword\n"));
  }

  //
  // OemServices
  //
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcGetMaxCheckPasswordCount \n"));
  Status = OemSvcGetMaxCheckPasswordCount (
             &TryPasswordTimes
             );
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcGetMaxCheckPasswordCount Status: %r\n", Status));

  PasswordString = HiiGetString (HiiHandle, STRING_TOKEN (STR_CHECK_PASSWORD_STRING), NULL);

  SysInvalidPassword = HiiGetString (HiiHandle, STRING_TOKEN (STR_SYS_DIALOG_INVALID_PASSWORD), NULL);
  //
  // Clear screen before print Harddisk passwork prompt message
  //
  gST->ConOut->ClearScreen (gST->ConOut);
  do {
    ZeroMem (StringPtr, (SysPasswordMaxNumber (SysPasswordService) + 1) * sizeof (CHAR16));
    SuBrowser->H2ODialog->PasswordDialog (
                              0,
                              FALSE,
                              (UINT32) (SysPasswordMaxNumber (SysPasswordService) + 1),
                              StringPtr,
                              &Key,
                              PasswordString
                              );

    Status = SysPasswordService->GetStatus (
                                   SysPasswordService,
                                   SystemSupervisor
                                   );
    if (Status == EFI_SUCCESS) {
      //
      // Check Supervisor password
      //
      Status = SysPasswordService->CheckPassword (
                                     SysPasswordService,
                                     StringPtr,
                                     (SysPasswordMaxNumber (SysPasswordService) + 1) * sizeof(CHAR16),
                                     SystemSupervisor
                                     );

      if (Status == EFI_SUCCESS) {
        KernelConfig->SetUserPass = FALSE;
        CurrentUserType = SystemSupervisorType;
        FreePool (PasswordString);
        FreePool (SysInvalidPassword);
        Status = SysPasswordService->UnlockPassword (
                                       SysPasswordService,
                                       StringPtr,
                                       (SysPasswordMaxNumber (SysPasswordService) + 1) * sizeof(CHAR16)
                                       );

        SetSysPasswordCurrentUser (
          SysPasswordService,
          CurrentUserType
          );
        FreePool (StringPtr);
        return EFI_SUCCESS;
      }
    }
    if (PcdGetBool (PcdSysPasswordSupportUserPswd)) {
      if (ScuInfo->UserPassword->NumOfEntry != 0) {
        Status = SysPasswordService->GetStatus(
                                       SysPasswordService,
                                       SystemUser
                                       );

        if (Status == EFI_SUCCESS) {
          //
          // Check User password
          //
          Status = SysPasswordService->CheckPassword (
                                         SysPasswordService,
                                         StringPtr,
                                         (SysPasswordMaxNumber (SysPasswordService) + 1) * sizeof(CHAR16),
                                         SystemUser
                                         );

          if (Status == EFI_SUCCESS) {
            KernelConfig->SetUserPass = TRUE;
            CurrentUserType = SystemUserType;
            FreePool (PasswordString);
            FreePool (SysInvalidPassword);
            SetSysPasswordCurrentUser (
              SysPasswordService,
              CurrentUserType
              );
            FreePool (StringPtr);
            return EFI_SUCCESS;
          }
        }
      }
    }

    TryPasswordTimes--;
    ReadChar = TRUE;
    if (TryPasswordTimes > 0) {
      SuBrowser->H2ODialog->ConfirmDialog (
                                2,
                                FALSE,
                                40,
                                NULL,
                                &Key,
                                SysInvalidPassword
                                );
    }
    if (TryPasswordTimes == 0 && Status != EFI_SUCCESS) {
      if (FeaturePcdGet (PcdH2OBdsCpCheckSetupPasswordFailSupported)) {
        DEBUG ((DEBUG_INFO, "Enter TriggerCheckSetupPasswordFail\n"));
        TriggerCheckSetupPasswordFail (&TryPasswordTimes, SuBrowser, HiiHandle);
        DEBUG ((DEBUG_INFO, "Exit TriggerCheckSetupPasswordFail\n"));
      }  
    }
  } while (TryPasswordTimes > 0);

  //
  // OemServices
  //
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcCheckPasswordFailCallBack \n"));
  Status = OemSvcCheckPasswordFailCallBack ();
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcCheckPasswordFailCallBack Result: %r\n", Status));

  if (Status == EFI_UNSUPPORTED) {
    ErrorStatusMsg    = HiiGetString (HiiHandle, STRING_TOKEN (STR_SYS_DIALOG_ERROR_STATUS), NULL);
    ThreeIncorrectMsg = HiiGetString (HiiHandle, STRING_TOKEN (STR_SYS_DIALOG_THREE_ERRORS), NULL);
    RebootMsg         = HiiGetString (HiiHandle, STRING_TOKEN (STR_SYS_DIALOG_RESTART_MSG), NULL);
    SuBrowser->H2ODialog->CreateMsgPopUp (
                            40,
                            3,
                            ErrorStatusMsg,
                            ThreeIncorrectMsg,
                            RebootMsg
                            );

    CpuDeadLoop ();
  }

  FreePool (PasswordString);
  FreePool (StringPtr);

  return EFI_SECURITY_VIOLATION;
}

/**
  Check any TCG supported drive.

  @param[in]  HddPasswordScuData     The Harddisk information array.
  @param[in]  NumOfHdd               Number of simple password protocol of harddisk.

  @retval TRUE                   TCG supported drive is found.
  @retval FALSE                  No TCG supported drive found.

**/
BOOLEAN
CheckAnyTcgDrive (
  IN  HDD_PASSWORD_SCU_DATA      *HddPasswordScuData,
  IN  UINTN                      NumOfHdd
  )
{
  UINT32  Index;

  if ((HddPasswordScuData == NULL) || (NumOfHdd == 0)) {
    return FALSE;
  }

  for (Index = 0; Index < NumOfHdd; Index++) {
    if (HddPasswordScuData[Index].HddInfo->StorageTcgSecuritySupported) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Check Harddisk security ststus.

  @param  HddPasswordScuData     The Harddisk information array.
  @param  NumOfHdd               Number of simple password protocol of harddisk.

  @retval TRUE                   Harddisk security status is locked
  @retval EFI_NOT_FOUND          It could not found any harddisk security status is locked.

**/
BOOLEAN
CheckAnyHddLocked (
  IN  HDD_PASSWORD_SCU_DATA      *HddPasswordScuData,
  IN  UINTN                      NumOfHdd
  )
{
  UINT32  Index;
  UINT32  StatusFlag;

  if ((HddPasswordScuData == NULL) || (NumOfHdd == 0)) {
    return FALSE;
  }

  StatusFlag = 0;
  for (Index = 0; Index < NumOfHdd; Index++) {
    StatusFlag |= HddPasswordScuData[Index].Flag;
  }

  if ((StatusFlag & SECURITY_LOCK) != 0) {
    return TRUE;
  }

  return FALSE;
}

/**
  Check hard disk password status.

  @param  HiiHandle              Specific HII handle for Security menu.
  @param  BootOrSCU              TRUE: In to SCU
                                 FALSE: want to boot
  @param  KernelConfig           Point to KERNEL_CONFIGURATION instance.
  @param  SavePasswordPoint      Point to security harddisk password instance

  @retval EFI_SUCCESS            Check hard disk password success.
  @retval EFI_NOT_FOUND          Not found system password.

**/
EFI_STATUS
FrozenHddResetStatus (
  VOID
  )
{
  EFI_STATUS                            Status;
  UINTN                                 Index;
  EFI_HDD_PASSWORD_SERVICE_PROTOCOL     *HddPasswordService;
  HDD_PASSWORD_HDD_INFO                 *HddInfoArray;
  UINTN                                 NumOfHdd;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;

  HddPasswordService = NULL;
  HddInfoArray       = NULL;
  NumOfHdd           = 0;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }


  SUCInfo = SuBrowser->SUCInfo;

  HddPasswordService = SUCInfo->HddPasswordService;

  Status = HddPasswordService->GetHddInfo (
                                 HddPasswordService,
                                 &HddInfoArray,
                                 &NumOfHdd
                                 );
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  if (NumOfHdd == 0) {
    return EFI_SUCCESS;
  }

  for (Index = 0; Index < NumOfHdd; Index++) {
    if ((HddInfoArray[Index].HddSecurityStatus & HDD_SECURITY_FROZEN) == HDD_SECURITY_FROZEN) {

        HddPasswordService->ResetSecurityStatus (
                              HddPasswordService,
                              &HddInfoArray[Index]
                              );
    }
  }

  if (HddInfoArray != NULL) {
    FreePool (HddInfoArray);
  }

  return EFI_SUCCESS;
}

/**
  Return ExtDataSize.

  @retval           The Size of ExtData is going to be used.

**/
UINT16
GetExtDataSize (
  VOID
  )
{
  if (PcdGetBool (PcdH2OHddPasswordTcgOpalSupported)) {
    return sizeof (TCG_OPAL_EXT_DATA);
  }
  return 0;
}

/**
  Create new HDD password table.

  @param[in]        NumOfHdd     The number of HDDs.
  @param[in]        TableSize    The size of table allocated.

  @retval           HddPasswordTable pointer

**/
VOID *
CreateHddPasswordTable (
  IN     UINTN                          NumOfHdd,
     OUT UINTN                          *TableSize
  )
{
  HDD_PASSWORD_TABLE                    *HddPasswordTable;
  UINTN                                 HddPasswordTableSize;
  HDD_PASSWORD_TABLE                    *HddPasswordTablePtr;
  TCG_OPAL_EXT_DATA                     *TcgExtDataPtr;

  HddPasswordTableSize = (sizeof (HDD_PASSWORD_TABLE) + GetExtDataSize ()) * (NumOfHdd + 1);
  HddPasswordTable = AllocateZeroPool (HddPasswordTableSize);
  if (HddPasswordTable == NULL) {
    *TableSize = 0;
    return NULL;
  }

  *TableSize = HddPasswordTableSize;

  if (PcdGetBool (PcdH2OHddPasswordTcgOpalSupported)) {
    //
    // Fill out the TCG Opal extend data size and sig
    //
    HddPasswordTablePtr = (HDD_PASSWORD_TABLE *)HddPasswordTable;
    while (((UINTN)HddPasswordTable + HddPasswordTableSize) > (UINTN)HddPasswordTablePtr) {
      HddPasswordTablePtr->ExtDataSize = GetExtDataSize ();

      TcgExtDataPtr = (TCG_OPAL_EXT_DATA *)(HddPasswordTablePtr + 1);
      TcgExtDataPtr->Signature = TCG_OPAL_EXT_DATA_SIGNATURE;

      HddPasswordTablePtr = GetNextTableEntry (HddPasswordTablePtr);
    }
  }

  return (VOID *)HddPasswordTable;
}

/**
  Update extend data area.

  @param[in]        HddPasswordTable     Pointer to HddPasswordTable
  @param[in]        PasswordType         Password type

**/
VOID
UpdateExtData (
  IN OUT HDD_PASSWORD_TABLE             *HddPasswordTable,
  IN     UINT8                          PasswordType
  )
{
  TCG_OPAL_EXT_DATA                     *TcgExtDataPtr;

  TcgExtDataPtr = (TCG_OPAL_EXT_DATA *)(HddPasswordTable + 1);
  if (TcgExtDataPtr->Signature == TCG_OPAL_EXT_DATA_SIGNATURE) {
    if (PasswordType == MASTER_PSW) {
      //
      // Record as SID password
      //
      CopyMem (
        TcgExtDataPtr->SIDPasswordStr,
        HddPasswordTable->PasswordStr,
        StrSize (HddPasswordTable->PasswordStr)
        );
      //
      // Record as Admin1 password
      //
      CopyMem (
        TcgExtDataPtr->Admin1PasswordStr,
        HddPasswordTable->PasswordStr,
        StrSize (HddPasswordTable->PasswordStr)
        );
    } else { // USER_PSW
      CopyMem (
        TcgExtDataPtr->User1PasswordStr,
        HddPasswordTable->PasswordStr,
        StrSize (HddPasswordTable->PasswordStr)
        );
    }
  }
}

/**
  Restore PSID value to PsidString.

  @param[in] HddIndex               The number of drive selected.

**/
VOID
UpdatePsidString (
  IN  UINT16                              HddIndex
  )
{
  UINT8               Psid[PSID_CHARACTER_STRING_END_LENGTH];

  if (mStoredPsid == NULL) {
    return;
  }

  ZeroMem (Psid, sizeof (Psid));
  CopyMem (Psid, mStoredPsid[HddIndex].Psid, PSID_CHARACTER_LENGTH);

  AsciiStrToUnicodeStrS ((CHAR8 *)Psid, mPasswordConfig.PsidString, PSID_CHARACTER_STRING_END_LENGTH);
}

/**
  Check if PSID is enabled.

  @param[in]        HddPasswordService     Pointer to HddPasswordService protocol
  @param[in]        HddInfo                Pointer to HddInfo

  @retval PSID is enabled or not.

**/
BOOLEAN
IsTcgPsidEnabled (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL     *HddPasswordService,
  IN  HDD_PASSWORD_HDD_INFO                 *HddInfo
  )
{
  EFI_STATUS                            Status;
  ATA_IDENTIFY_DATA                     AtaIdentifyData;

  Status = HddPasswordService->HddIdentify (
                                 HddPasswordService,
                                 HddInfo,
                                 (UINT16 *)&AtaIdentifyData
                                 );
  if ((Status == EFI_SUCCESS) &&
      (AtaIdentifyData.retired_20_21[0] != 0)) {
    return TRUE;
  }
  return FALSE;
}

/**
  Check if USER1 is enabled.

  @param[in]        HddPasswordService     Pointer to HddPasswordService protocol
  @param[in]        HddInfo                Pointer to HddInfo

  @retval USER1 is enabled or not.

**/
BOOLEAN
IsTcgUser1Enabled (
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL     *HddPasswordService,
  IN  HDD_PASSWORD_HDD_INFO                 *HddInfo
  )
{
  EFI_STATUS                            Status;
  ATA_IDENTIFY_DATA                     AtaIdentifyData;

  Status = HddPasswordService->HddIdentify (
                                 HddPasswordService,
                                 HddInfo,
                                 (UINT16 *)&AtaIdentifyData
                                 );
  if ((Status == EFI_SUCCESS) &&
      (AtaIdentifyData.command_set_feature_enb_85 != 0)) {
    return TRUE;
  }
  return FALSE;
}

/**
  TCG Opal policy handler.

  @param[in]  HddPasswordScuData     The Harddisk information array.
  @param[in]  NumOfHdd               Number of simple password protocol of harddisk.
  @param[in]  HddPasswordService     Pointer to HddPasswordService protocol.

**/
VOID
TcgOpalPolicyHandler (
  IN  HDD_PASSWORD_SCU_DATA                 *HddPasswordScuData,
  IN  UINTN                                 NumOfHdd,
  IN  EFI_HDD_PASSWORD_SERVICE_PROTOCOL     *HddPasswordService
  )
{
  UINT32                         Index;

  if ((HddPasswordScuData == NULL) || (NumOfHdd == 0)) {
    return;
  }

  if ((PcdGet8 (PcdH2OHddPasswordTcgOpalPolicy) & BIT0) == BIT0) {
    //
    // Allow enabling LockingSP\USER1 when the drive is unowned
    //
    mTakeOwnershipByUserPassword = TRUE;
  }
  if ((PcdGet8 (PcdH2OHddPasswordTcgOpalPolicy) & BIT1) == BIT1) {
    //
    // Single password scenario. Display only user password option under SCU.
    //
    //
    // Used for caching original password for MASTER password change.
    //
    mOrgPassword = (PASSWORD_INFORMATION *)ReallocatePool (
                                             NumOfHdd * sizeof (PASSWORD_INFORMATION),
                                             NumOfHdd * sizeof (PASSWORD_INFORMATION),
                                             (VOID *)mOrgPassword);
    if (mOrgPassword != NULL) {
      ZeroMem ((VOID *)mOrgPassword, NumOfHdd * sizeof (PASSWORD_INFORMATION));
      mSingleUserPassword = TRUE;
    }
  }

  //
  // TCG PSID
  //
  mStoredPsid = (TCG_PSID *)ReallocatePool (
                              NumOfHdd * sizeof (TCG_PSID),
                              NumOfHdd * sizeof (TCG_PSID),
                              (VOID *)mStoredPsid
                              );
  if (mStoredPsid != NULL) {
    ZeroMem ((VOID *)mStoredPsid, NumOfHdd * sizeof (TCG_PSID));
    //
    // PSID support check
    //
    for (Index = 0; Index < NumOfHdd; Index++) {
      if (HddPasswordScuData[Index].HddInfo->StorageTcgSecuritySupported) {
        mStoredPsid[Index].Supported = IsTcgPsidEnabled (HddPasswordService, HddPasswordScuData[Index].HddInfo);
      }
    }
  }
}

/**
  Set user set password status. This founction could set password status
  when press 'F10' and set 'yes'.

  @retval EFI_SUCCESS            Set system password and harddisk password success.

**/
EFI_STATUS
SetSecurityStatus (
  VOID
  )
{
  UINTN                               Index;
  SETUP_UTILITY_CONFIGURATION         *SUCInfo;
  EFI_STATUS                          Status;
  SETUP_UTILITY_BROWSER_DATA          *SuBrowser;
  UINT8                               PasswordToHdd[HDD_PASSWORD_MAX_NUMBER + 1];
  UINTN                               PasswordToHddLength;
  UINTN                               DisabledNumOfHdd;
  HDD_PASSWORD_TABLE                  *HddPasswordTable;
  UINTN                               HddPasswordTableSize;
  HDD_PASSWORD_TABLE                  *HddPasswordTablePtr;

  PasswordToHddLength = 0;
  HddPasswordTableSize = 0;
  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo = SuBrowser->SUCInfo;

  if (SUCInfo->SysPasswordService == NULL) {
    return EFI_NOT_READY;
  }

  if (PcdGetBool (PcdSysPasswordSupportUserPswd)) {
    //
    // Set User Password
    //
    if (SUCInfo->UserPassword->Flag != NO_ACCESS_PASSWORD) {

      SUCInfo->SysPasswordService->DisablePassword (
                                     SUCInfo->SysPasswordService,
                                     SystemUser
                                     );

      if (SUCInfo->UserPassword->Flag == CHANGE_PASSWORD) {
        SUCInfo->SysPasswordService->SetPassword (
                                       SUCInfo->SysPasswordService,
                                       SUCInfo->UserPassword->InputString,
                                       SUCInfo->UserPassword->StringLength,
                                       SystemUser
                                       );
      }
    }
  }

  //
  // Set Supervisor Password
  //
  if (SUCInfo->SupervisorPassword->Flag != NO_ACCESS_PASSWORD) {
    SUCInfo->SysPasswordService->DisablePassword (
                                   SUCInfo->SysPasswordService,
                                   SystemSupervisor
                                   );

    if (SUCInfo->SupervisorPassword->Flag == CHANGE_PASSWORD) {
      SUCInfo->SysPasswordService->SetPassword (
                                     SUCInfo->SysPasswordService,
                                     SUCInfo->SupervisorPassword->InputString,
                                     SUCInfo->SupervisorPassword->StringLength,
                                     SystemSupervisor
                                     );
    }
  }

  //
  // HDD Password
  //
  DisabledNumOfHdd = 0;
  Status = CommonGetVariableDataAndSize (
             SAVE_HDD_PASSWORD_VARIABLE_NAME,
             &gSaveHddPasswordGuid,
             &HddPasswordTableSize,
             (VOID **) &HddPasswordTable
             );

  if (Status == EFI_SUCCESS) {
    SavedPasswordStringProcess (
          DecryptProcess,
          HddPasswordTable,
          HddPasswordTableSize
          );
    //
    // May get old table
    //

    if ((HddPasswordTableSize / (sizeof (HDD_PASSWORD_TABLE) + GetExtDataSize ())) != (SUCInfo->NumOfHdd + 1)) {
      Status = EFI_NOT_FOUND;
      FreePool (HddPasswordTable);
    }
  }

  if (Status != EFI_SUCCESS) {
    //
    // Create new table
    //
    HddPasswordTable = CreateHddPasswordTable (SUCInfo->NumOfHdd, &HddPasswordTableSize);
    if (HddPasswordTable == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  }

  HddPasswordTablePtr = (HDD_PASSWORD_TABLE *)HddPasswordTable;
  for (Index = 0; Index < SUCInfo->NumOfHdd; Index++, HddPasswordTablePtr = GetNextTableEntry (HddPasswordTablePtr)) {

    if (mStoredPsid[Index].Psid[0] != 0) {
      //
      // Issue PSID Revert to revert device to factory default
      //
      Status = SUCInfo->HddPasswordService->SendHddSecurityCmd (
                                              SUCInfo->HddPasswordService,
                                              H2O_HDD_PASSWORD_TCG_PSID_REVERT,
                                              SUCInfo->HddPasswordScuData[Index].HddInfo,
                                              USER_PSW,
                                              mStoredPsid[Index].Psid,
                                              sizeof (mStoredPsid[Index].Psid)
                                              );
      continue;
    }

    if ((SUCInfo->HddPasswordScuData[Index].Flag == NO_ACCESS_PASSWORD) &&
        (SUCInfo->HddPasswordScuData[Index].MasterFlag == NO_ACCESS_PASSWORD)) {
      DisabledNumOfHdd++;
      continue;
    }

    if ((SUCInfo->HddPasswordScuData[Index].Flag != CHANGE_PASSWORD) &&
        (SUCInfo->HddPasswordScuData[Index].Flag != DISABLE_PASSWORD) &&
        (SUCInfo->HddPasswordScuData[Index].MasterFlag != CHANGE_PASSWORD) &&
        (SUCInfo->HddPasswordScuData[Index].MasterFlag != DISABLE_PASSWORD)) {
      continue;
    }

    if (SUCInfo->HddPasswordScuData[Index].Flag == SECURITY_LOCK) {
      continue;
    }

    //
    // Change HDD user password
    //
    if (SUCInfo->HddPasswordScuData[Index].Flag == CHANGE_PASSWORD) {
      ZeroMem (PasswordToHdd, (HDD_PASSWORD_MAX_NUMBER + 1));
      PasswordToHddLength = 0;

      Status = SUCInfo->HddPasswordService->PasswordStringProcess (
                                     SUCInfo->HddPasswordService,
                                     USER_PSW,
                                     SUCInfo->HddPasswordScuData[Index].InputString,
                                     StrLen (SUCInfo->HddPasswordScuData[Index].InputString),
                                     (VOID **)&PasswordToHdd,
                                     &PasswordToHddLength
                                     );

      if (EFI_ERROR(Status)) {
        continue;
      }

      Status = SUCInfo->HddPasswordService->SetHddPassword (
                                     SUCInfo->HddPasswordService,
                                     SUCInfo->HddPasswordScuData[Index].HddInfo,
                                     USER_PSW,
                                     PasswordToHdd,
                                     PasswordToHddLength
                                     );
      if (Status == EFI_SUCCESS) {
        CopyMem (
          HddPasswordTablePtr->PasswordStr,
          SUCInfo->HddPasswordScuData[Index].InputString,
          StrSize (SUCInfo->HddPasswordScuData[Index].InputString)
          );
        HddPasswordTablePtr->PasswordType     = USER_PSW;
        HddPasswordTablePtr->ControllerNumber = SUCInfo->HddPasswordScuData[Index].HddInfo->ControllerNumber;
        HddPasswordTablePtr->PortNumber       = SUCInfo->HddPasswordScuData[Index].HddInfo->PortNumber;
        HddPasswordTablePtr->PortMulNumber    = SUCInfo->HddPasswordScuData[Index].HddInfo->PortMulNumber;
        if (SUCInfo->HddPasswordScuData[Index].HddInfo->StorageTcgSecuritySupported) {
          if (SUCInfo->HddPasswordScuData[Index].DisableAllType == NO_ACCESS_PSW) {
            //
            // USER password has been set the same as MASTER password. Record the password as SID/Admin1
            //
            UpdateExtData (HddPasswordTablePtr, MASTER_PSW);
          } else if (mSingleUserPassword) {
            //
            // Change MASTER password if single user password policy is set.
            // Should do UnlockHddPassword prior to SetHddPassword.
            //
            Status = SUCInfo->HddPasswordService->PasswordStringProcess (
                                                    SUCInfo->HddPasswordService,
                                                    MASTER_PSW,
                                                    mOrgPassword[Index].PasswordStr,
                                                    StrLen (mOrgPassword[Index].PasswordStr),
                                                    (VOID **)&PasswordToHdd,
                                                    &PasswordToHddLength
                                                    );
            if (EFI_ERROR(Status)) {
              continue;
            }

            Status = SUCInfo->HddPasswordService->UnlockHddPassword (
                                                    SUCInfo->HddPasswordService,
                                                    SUCInfo->HddPasswordScuData[Index].HddInfo,
                                                    MASTER_PSW,
                                                    PasswordToHdd,
                                                    PasswordToHddLength
                                                    );
            if (EFI_ERROR(Status)) {
              continue;
            }

            StrCpyS (
              SUCInfo->HddPasswordScuData[Index].MasterInputString,
              sizeof (SUCInfo->HddPasswordScuData[Index].MasterInputString) / sizeof (CHAR16),
              SUCInfo->HddPasswordScuData[Index].InputString
              );

            SUCInfo->HddPasswordScuData[Index].MasterFlag = CHANGE_PASSWORD;

            //
            // Clear the cached password.
            //
            ZeroMem ((VOID *)&mOrgPassword[Index], sizeof (mOrgPassword[Index]));
          }
        }
      }
    }

    //
    // Change HDD Master password
    //
    if (SUCInfo->HddPasswordScuData[Index].MasterFlag == CHANGE_PASSWORD ||
        (SUCInfo->HddPasswordScuData[Index].Flag == CHANGE_PASSWORD &&
         SUCInfo->HddPasswordScuData[Index].MasterFlag == MASTER_PASSWORD_GRAYOUT &&
         SUCInfo->HddPasswordScuData[Index].MasterInputString[0] != 0) ) {
      //
      // Change Password
      //   Condition 1: Master HDD password is changed
      //   Condition 2: MasterFlag is not CHANGE_PASSWORD but Master HDD Password should be set.
      //                When supporting "Set Master HDD Password" related item to grayout
      //                by HDD security status changing from disabled to enabled,
      //                there are more check rules to determine to set master HDD password.
      //

      ZeroMem (PasswordToHdd, (HDD_PASSWORD_MAX_NUMBER + 1));
      PasswordToHddLength = 0;

      Status = SUCInfo->HddPasswordService->PasswordStringProcess (
                                              SUCInfo->HddPasswordService,
                                              MASTER_PSW,
                                              SUCInfo->HddPasswordScuData[Index].MasterInputString,
                                              StrLen (SUCInfo->HddPasswordScuData[Index].MasterInputString),
                                              (VOID **)&PasswordToHdd,
                                              &PasswordToHddLength
                                              );

      if (EFI_ERROR(Status)) {
        continue;
      }

      Status = SUCInfo->HddPasswordService->SetHddPassword (
                                              SUCInfo->HddPasswordService,
                                              SUCInfo->HddPasswordScuData[Index].HddInfo,
                                              MASTER_PSW,
                                              PasswordToHdd,
                                              PasswordToHddLength
                                              );
      if (Status == EFI_SUCCESS &&
          SUCInfo->HddPasswordScuData[Index].HddInfo->StorageTcgSecuritySupported) {
        CopyMem (
          HddPasswordTablePtr->PasswordStr,
          SUCInfo->HddPasswordScuData[Index].MasterInputString,
          StrSize (SUCInfo->HddPasswordScuData[Index].MasterInputString)
          );
        HddPasswordTablePtr->PasswordType     = MASTER_PSW;
        HddPasswordTablePtr->ControllerNumber = SUCInfo->HddPasswordScuData[Index].HddInfo->ControllerNumber;
        HddPasswordTablePtr->PortNumber       = SUCInfo->HddPasswordScuData[Index].HddInfo->PortNumber;
        HddPasswordTablePtr->PortMulNumber    = SUCInfo->HddPasswordScuData[Index].HddInfo->PortMulNumber;
        //
        // Record the password as SID/Admin1
        //
        UpdateExtData (HddPasswordTablePtr, HddPasswordTablePtr->PasswordType);
      }
    }

    if (SUCInfo->HddPasswordScuData[Index].HddInfo->StorageTcgSecuritySupported) {
      //
      // Disable User Password
      //
      if (SUCInfo->HddPasswordScuData[Index].Flag == DISABLE_PASSWORD &&
          (SUCInfo->HddPasswordScuData[Index].MasterFlag != DISABLE_PASSWORD)) {
        ZeroMem (PasswordToHdd, (HDD_PASSWORD_MAX_NUMBER + 1));
        PasswordToHddLength = 0;

        Status = SUCInfo->HddPasswordService->PasswordStringProcess (
                                                SUCInfo->HddPasswordService,
                                                USER_PSW,
                                                SUCInfo->HddPasswordScuData[Index].DisableInputString,
                                                SUCInfo->HddPasswordScuData[Index].DisableStringLength,
                                                (VOID **)&PasswordToHdd,
                                                &PasswordToHddLength
                                                );

        if (EFI_ERROR(Status)) {
          continue;
        }


        Status = SUCInfo->HddPasswordService->DisableHddPassword (
                                                SUCInfo->HddPasswordService,
                                                SUCInfo->HddPasswordScuData[Index].HddInfo,
                                                USER_PSW,
                                                PasswordToHdd,
                                                PasswordToHddLength
                                                );
        if (Status == EFI_SUCCESS) {
          //
          // Keep the ExtData but zero out password.
          //
          ZeroMem ((VOID *)HddPasswordTablePtr->PasswordStr, sizeof (HddPasswordTablePtr->PasswordStr));
          ZeroMem ((VOID *)&HddPasswordTablePtr->PasswordType, sizeof (HddPasswordTablePtr->PasswordType));
          continue;
        }
      }
      //
      // Disable Master Password
      //
      if (SUCInfo->HddPasswordScuData[Index].MasterFlag == DISABLE_PASSWORD) {
        ZeroMem (PasswordToHdd, (HDD_PASSWORD_MAX_NUMBER + 1));
        PasswordToHddLength = 0;

        Status = SUCInfo->HddPasswordService->PasswordStringProcess (
                                                SUCInfo->HddPasswordService,
                                                MASTER_PSW,
                                                SUCInfo->HddPasswordScuData[Index].DisableInputString,
                                                SUCInfo->HddPasswordScuData[Index].DisableStringLength,
                                                (VOID **)&PasswordToHdd,
                                                &PasswordToHddLength
                                                );

        if (EFI_ERROR(Status)) {
          continue;
        }


        Status = SUCInfo->HddPasswordService->DisableHddPassword (
                                                SUCInfo->HddPasswordService,
                                                SUCInfo->HddPasswordScuData[Index].HddInfo,
                                                MASTER_PSW,
                                                PasswordToHdd,
                                                PasswordToHddLength
                                                );
        if (Status == EFI_SUCCESS) {
          DisabledNumOfHdd++;
          continue;
        }
      }
    } else { // StorageTcgSecuritySupported = FALSE
      //
      // Disable User Password
      //
      if (SUCInfo->HddPasswordScuData[Index].Flag == DISABLE_PASSWORD) {
        ZeroMem (PasswordToHdd, (HDD_PASSWORD_MAX_NUMBER + 1));
        PasswordToHddLength = 0;

        Status = SUCInfo->HddPasswordService->PasswordStringProcess (
                                                SUCInfo->HddPasswordService,
                                                USER_PSW,
                                                SUCInfo->HddPasswordScuData[Index].DisableInputString,
                                                SUCInfo->HddPasswordScuData[Index].DisableStringLength,
                                                (VOID **)&PasswordToHdd,
                                                &PasswordToHddLength
                                                );

        if (EFI_ERROR(Status)) {
          continue;
        }

        Status = SUCInfo->HddPasswordService->DisableHddPassword (
                                                SUCInfo->HddPasswordService,
                                                SUCInfo->HddPasswordScuData[Index].HddInfo,
                                                USER_PSW,
                                                PasswordToHdd,
                                                PasswordToHddLength
                                                );
        if (Status == EFI_SUCCESS) {
          DisabledNumOfHdd++;
          continue;
        }
      }
      //
      // Disable Master Password
      //
      if (SUCInfo->HddPasswordScuData[Index].MasterFlag == DISABLE_PASSWORD) {
        ZeroMem (PasswordToHdd, (HDD_PASSWORD_MAX_NUMBER + 1));
        PasswordToHddLength = 0;

        Status = SUCInfo->HddPasswordService->PasswordStringProcess (
                                                SUCInfo->HddPasswordService,
                                                MASTER_PSW,
                                                SUCInfo->HddPasswordScuData[Index].DisableInputString,
                                                SUCInfo->HddPasswordScuData[Index].DisableStringLength,
                                                (VOID **)&PasswordToHdd,
                                                &PasswordToHddLength
                                                );

        if (EFI_ERROR(Status)) {
          continue;
        }


        Status = SUCInfo->HddPasswordService->DisableHddPassword (
                                                SUCInfo->HddPasswordService,
                                                SUCInfo->HddPasswordScuData[Index].HddInfo,
                                                MASTER_PSW,
                                                PasswordToHdd,
                                                PasswordToHddLength
                                                );
        if (Status == EFI_SUCCESS) {
          DisabledNumOfHdd++;
          continue;
        }
        if (Status != EFI_SUCCESS) {
          ZeroMem (PasswordToHdd, (HDD_PASSWORD_MAX_NUMBER + 1));
          PasswordToHddLength = 0;

          Status = SUCInfo->HddPasswordService->PasswordStringProcess (
                                                  SUCInfo->HddPasswordService,
                                                  SUCInfo->HddPasswordScuData[Index].DisableAllType,
                                                  SUCInfo->HddPasswordScuData[Index].DisableAllInputString,
                                                  SUCInfo->HddPasswordScuData[Index].DisableStringLength,
                                                  (VOID **)&PasswordToHdd,
                                                  &PasswordToHddLength
                                                  );

          if (EFI_ERROR(Status)) {
            continue;
          }

          Status = SUCInfo->HddPasswordService->DisableHddPassword (
                                                  SUCInfo->HddPasswordService,
                                                  SUCInfo->HddPasswordScuData[Index].HddInfo,
                                                  SUCInfo->HddPasswordScuData[Index].DisableAllType,
                                                  PasswordToHdd,
                                                  PasswordToHddLength
                                                  );
          if (Status == EFI_SUCCESS) {
            DisabledNumOfHdd++;
          }
        }
      }
    }
  }

  if (DisabledNumOfHdd == SUCInfo->NumOfHdd) {
      Status = CommonSetVariable (
         SAVE_HDD_PASSWORD_VARIABLE_NAME,
         &gSaveHddPasswordGuid,
         EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
         0,
         NULL
         );
  } else {
    SavedPasswordStringProcess (
          EncryptProcess,
          HddPasswordTable,
          HddPasswordTableSize
          );
    Status = CommonSetVariable (
       SAVE_HDD_PASSWORD_VARIABLE_NAME,
       &gSaveHddPasswordGuid,
       EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
       HddPasswordTableSize,
       HddPasswordTable
       );

    //
    //  Reset all storage device security status to locked if security enabled
    //
    for (Index = 0; Index < SUCInfo->NumOfHdd; Index++) {
      if ((SUCInfo->HddPasswordScuData[Index].HddInfo->HddSecurityStatus & HDD_ENABLE_BIT) == HDD_ENABLE_BIT) {
        Status = SUCInfo->HddPasswordService->ResetSecurityStatus (
                                                SUCInfo->HddPasswordService,
                                                SUCInfo->HddPasswordScuData[Index].HddInfo
                                                );
      }
    }
  }


  //
  // Zeroing the PWD content in memory after it's being used
  //
  if (PasswordToHddLength > 0) {
    ZeroMem (PasswordToHdd, PasswordToHddLength);
  }

  if (HddPasswordTable != NULL) {
    ZeroMem(HddPasswordTable, HddPasswordTableSize);
    FreePool(HddPasswordTable);
  }

  return EFI_SUCCESS;
}



/**
  Check the system password checking flag to decide to ask password or not.

  @param  SysPasswordService     system password service protocol

  @retval TRUE                   the system password checking flag is enable.
  @retval FALSE                  the system password checking flag is disable.

**/
BOOLEAN
HaveSysPasswordCheck (
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL    *SysPasswordService  OPTIONAL
  )
{
  EFI_STATUS                              Status;
  SYS_PASSWORD_SETUP_INFO_DATA            SetupInfoBuffer;


  if (SysPasswordService == NULL) {
    Status = gBS->LocateProtocol (
                    &gEfiSysPasswordServiceProtocolGuid,
                    NULL,
                    (VOID **) &SysPasswordService
                    );
    if (EFI_ERROR (Status)) {
      return FALSE;
    }
  }


  Status = SysPasswordService->GetSysPswdSetupInfoData(
                                 SysPasswordService,
                                 &SetupInfoBuffer
                                 );

  return SetupInfoBuffer.HaveSysPasswordCheck;
}

/**
  Set system password checking to disable.

  @param  SysPasswordService     system password service protocol

  @retval EFI_SUCCESS            set successful

**/
EFI_STATUS
CleanSysPasswordCheck (
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL       *SysPasswordService OPTIONAL
  )
{
  EFI_STATUS                              Status;
  SYS_PASSWORD_SETUP_INFO_DATA            SetupInfoBuffer;
  UINT32                                  SetupBits;

  SetupBits = 0;

  if (SysPasswordService == NULL) {
    Status = gBS->LocateProtocol (
                    &gEfiSysPasswordServiceProtocolGuid,
                    NULL,
                    (VOID **) &SysPasswordService
                    );
    if (EFI_ERROR (Status)) {
      return FALSE;
    }
  }

  Status = SysPasswordService->GetSysPswdSetupInfoData(
                                 SysPasswordService,
                                 &SetupInfoBuffer
                                 );

  SetupInfoBuffer.HaveSysPasswordCheck = FALSE;

  SetupBits = SYS_PASSWORD_CHECK_BIT;

  Status = SysPasswordService->SetSysPswdSetupInfoData(
                                 SysPasswordService,
                                 &SetupInfoBuffer,
                                 SetupBits
                                 );

  return Status;
}

/**
  Get previous checking password is supervior password or user password.

  @param  SysPasswordService     system password service protocol

  @retval SystemSupervisorType   current user is supervisor.
  @retval SystemUserType         current user is user.

**/
CURRENT_USER_TYPE
GetSysPasswordCurrentUser (
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL       *SysPasswordService OPTIONAL
  )
{
  EFI_STATUS                              Status;
  SYS_PASSWORD_SETUP_INFO_DATA            SetupInfoBuffer;


  if (SysPasswordService == NULL) {
    Status = gBS->LocateProtocol (
                    &gEfiSysPasswordServiceProtocolGuid,
                    NULL,
                    (VOID **) &SysPasswordService
                    );
    if (EFI_ERROR (Status)) {
      return FALSE;
    }
  }

  Status = SysPasswordService->GetSysPswdSetupInfoData(
                                 SysPasswordService,
                                 &SetupInfoBuffer
                                 );

  return SetupInfoBuffer.CurrentUser;
}

/**
  Set current password input is supervior password or user password.

  @param  SysPasswordService     system password service protocol
  @param  CurrentUserType        systemSupervisorType or systemUserType

  @retval EFI_SUCCESS            set current user successful.

**/
EFI_STATUS
SetSysPasswordCurrentUser (
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL       *SysPasswordService, OPTIONAL
  CURRENT_USER_TYPE       CurrentUserType
  )
{
  EFI_STATUS                              Status;
  SYS_PASSWORD_SETUP_INFO_DATA            SetupInfoBuffer;
  UINT32                                  SetupBits;

  SetupBits = 0;

  if (SysPasswordService == NULL) {
    Status = gBS->LocateProtocol (
                    &gEfiSysPasswordServiceProtocolGuid,
                    NULL,
                    (VOID **) &SysPasswordService
                    );
    if (EFI_ERROR (Status)) {
      return FALSE;
    }
  }

  Status = SysPasswordService->GetSysPswdSetupInfoData(
                                 SysPasswordService,
                                 &SetupInfoBuffer
                                 );

  SetupInfoBuffer.CurrentUser = CurrentUserType;

  SetupBits = CURRENT_USER_BIT;

  Status = SysPasswordService->SetSysPswdSetupInfoData(
                                 SysPasswordService,
                                 &SetupInfoBuffer,
                                 SetupBits
                                 );

  return Status;
}

/**
  Get the length of system password support.

  @param  SysPasswordService     system password service protocol

  @retval                        return the length of system password support.

**/
UINTN
SysPasswordMaxNumber (
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL    *SysPasswordService  OPTIONAL
  )
{
  EFI_STATUS                              Status;
  SYS_PASSWORD_SETUP_INFO_DATA            SetupInfoBuffer;


  if (SysPasswordService == NULL) {
    Status = gBS->LocateProtocol (
                    &gEfiSysPasswordServiceProtocolGuid,
                    NULL,
                    (VOID **) &SysPasswordService
                    );
    if (EFI_ERROR (Status)) {
      return FALSE;
    }
  }


  Status = SysPasswordService->GetSysPswdSetupInfoData(
                                 SysPasswordService,
                                 &SetupInfoBuffer
                                 );

  if (EFI_ERROR(Status)) {
    return 0;
  }

  return SetupInfoBuffer.MaxSysPasswordLength;
}

/**
 Initialize HDD Password information

 @retval EFI_SUCCESS            Initialize setup utility private data success
 @retval EFI_OUT_OF_RESOURCES   Allocate pool fail.
 @return other                  Get setup utility private data fail or locate HDD Password service protocol fail or
                                get HDD security info fail
**/
EFI_STATUS
InitHddPasswordScuData (
  VOID
  )
{
  EFI_HDD_PASSWORD_SERVICE_PROTOCOL     *HddPasswordService;
  HDD_PASSWORD_SCU_DATA                 *HddPasswordScuData;
  UINTN                                 Index;
  EFI_STATUS                            Status;
  UINTN                                 NumOfHdd;
  HDD_PASSWORD_HDD_INFO                 *HddInfoArray;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  HDD_PASSWORD_TABLE                    *HddPasswordTable;
  UINTN                                 DataSize;
  HDD_PASSWORD_TABLE                    *HddPasswordTablePtr;

  SuBrowser = NULL;
  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (Status != EFI_SUCCESS || SuBrowser == NULL) {
    return Status;
  }


  HddPasswordScuData          = NULL;
  HddInfoArray                = NULL;
  SUCInfo                     = SuBrowser->SUCInfo;
  HddPasswordService          = SUCInfo->HddPasswordService;
  SUCInfo->HddPasswordScuData = NULL;
  SUCInfo->NumOfHdd           = 0;
  SuBrowser->IdeConfig        = NULL;
  NumOfHdd                    = 0;
  HddPasswordTable            = NULL;
  DataSize                    = 0;

  ZeroMem (&mPasswordConfig, sizeof (PASSWORD_CONFIGURATION));

  if (HddPasswordService == NULL) {
    return EFI_UNSUPPORTED;
  }

  Status = HddPasswordService->GetHddInfo (
                                 HddPasswordService,
                                 &HddInfoArray,
                                 &NumOfHdd
                                 );
  if (Status != EFI_SUCCESS || NumOfHdd == 0) {
    return Status;
  }

  HddPasswordScuData = (HDD_PASSWORD_SCU_DATA *)AllocateZeroPool (((NumOfHdd) * sizeof(HDD_PASSWORD_SCU_DATA)));

  if (HddPasswordScuData == NULL) {
    return Status;
  }

  SUCInfo->NumOfHdd = NumOfHdd;
  SUCInfo->HddPasswordScuData  = HddPasswordScuData;

  if (NumOfHdd != 0) {
    mPasswordConfig.HddPasswordSupport = 1;

    Status = CommonGetVariableDataAndSize (
               SAVE_HDD_PASSWORD_VARIABLE_NAME,
               &gSaveHddPasswordGuid,
               &DataSize,
               (VOID **) &HddPasswordTable
               );
    SavedPasswordStringProcess (
      DecryptProcess,
      HddPasswordTable,
      DataSize
      );

    HddPasswordTablePtr = (HDD_PASSWORD_TABLE *)HddPasswordTable;
    for (Index = 0; Index < NumOfHdd; Index++, HddPasswordTablePtr = GetNextTableEntry (HddPasswordTablePtr)) {

      if ((HddInfoArray[Index].HddSecurityStatus & HDD_SECURITY_LOCK) == HDD_SECURITY_LOCK) {
        HddPasswordScuData[Index].Flag = SECURITY_LOCK;
      } else if ((HddInfoArray[Index].HddSecurityStatus & HDD_SECURITY_ENABLE) == HDD_SECURITY_ENABLE) {
        HddPasswordScuData[Index].Flag = ENABLE_PASSWORD;

        if (HddPasswordTablePtr != NULL) {
          if (HddPasswordTablePtr->PasswordType != MASTER_PSW) {
            HddPasswordScuData[Index].MasterFlag = MASTER_PASSWORD_GRAYOUT;
          }

          ZeroMem (
            HddPasswordScuData[Index].DisableInputString,
            (HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (CHAR16)
            );

          StrCpyS (
            HddPasswordScuData[Index].DisableInputString,
            sizeof(HddPasswordScuData[Index].DisableInputString) / sizeof(CHAR16),
            HddPasswordTablePtr->PasswordStr
            );

          HddPasswordScuData[Index].DisableAllType = HddPasswordTablePtr->PasswordType;
          HddPasswordScuData[Index].DisableStringLength = StrLen (HddPasswordScuData[Index].DisableInputString);
        }

      } else {
        HddPasswordScuData[Index].Flag           = NO_ACCESS_PASSWORD;
        HddPasswordScuData[Index].MasterFlag     = NO_ACCESS_PASSWORD;
        HddPasswordScuData[Index].DisableAllType = NO_ACCESS_PSW;
      }
      HddPasswordScuData[Index].HddInfo = &HddInfoArray[Index];
      HddPasswordScuData[Index].NumOfEntry  = 0x01;
    }

    if (HddPasswordTable != NULL) {
      FreePool (HddPasswordTable);
    }
  }

  TcgOpalPolicyHandler (HddPasswordScuData, NumOfHdd, HddPasswordService);

  return EFI_SUCCESS;
}

/**
 Reset System Password information for SCU

 @retval EFI_SUCCESS            Initialize setup utility private data success
 @retval EFI_OUT_OF_RESOURCES   Allocate pool fail
 @return other                  Get setup utility private data fail or locate System Password service protocol fail or
                                get System password info fail
**/
EFI_STATUS
ResetSysPasswordInfoForSCU (
  VOID
  )
{
  EFI_STATUS                                Status;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo = SuBrowser->SUCInfo;

  if (SUCInfo->SysPasswordService != NULL) {
    if (SUCInfo->SupervisorPassword  != NULL) {
      if (SUCInfo->SupervisorPassword->InputString != NULL) {
        FreePool (SUCInfo->SupervisorPassword->InputString);
      }
      FreePool (SUCInfo->SupervisorPassword);
      SUCInfo->SupervisorPassword = NULL;
    }
    if (SUCInfo->UserPassword  != NULL) {
      if (SUCInfo->UserPassword->InputString != NULL) {
        FreePool (SUCInfo->UserPassword->InputString);
      }
      FreePool (SUCInfo->UserPassword);
      SUCInfo->UserPassword = NULL;
    }

    Status = PasswordCheck (
               SUCInfo,
               (KERNEL_CONFIGURATION *) SuBrowser->SCBuffer
               );
  }

  return Status;
}

/**
 Reset HDD Password information for SCU

 @retval EFI_SUCCESS            Initialize setup utility private data success
 @retval EFI_OUT_OF_RESOURCES   Allocate pool fail
 @return other                  Get setup utility private data fail or locate HDD Password service protocol fail or
                                get HDD security info fail
**/
EFI_STATUS
ResetHddPasswordInfoForSCU (
  VOID
  )
{
  EFI_STATUS                                Status;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo = SuBrowser->SUCInfo;

  if (SUCInfo->HddPasswordService!= NULL) {
    ZeroMem (&mPasswordConfig, sizeof (PASSWORD_CONFIGURATION));

    if (SUCInfo->HddPasswordScuData != NULL) {
      if (SUCInfo->HddPasswordScuData[0].HddInfo != NULL) {
        FreePool (SUCInfo->HddPasswordScuData[0].HddInfo);
      }
      FreePool (SUCInfo->HddPasswordScuData);
      SUCInfo->HddPasswordScuData = NULL;
      SUCInfo->NumOfHdd = 0;
    }

    Status = InitHddPasswordScuData ();

    UpdateAllHddPasswordFlag (
      SuBrowser->SUCInfo->MapTable[SecurityHiiHandle].HiiHandle,
      SUCInfo->HddPasswordScuData,
      SUCInfo->NumOfHdd
      );
  }

  return Status;
}

EFI_STATUS
EFIAPI
InitStoragePasswordForSCU (
  IN  EFI_HII_HANDLE                    HiiHandle
  )
{
  EFI_STATUS                            Status;
  UINTN                                 Index;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  VOID                                  *StartOpCodeHandle;
  VOID                                  *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL                    *StartLabel;
  EFI_IFR_GUID_LABEL                    *EndLabel;
  EFI_HDD_PASSWORD_SERVICE_PROTOCOL     *HddPasswordService;
  HDD_PASSWORD_SCU_DATA                 *HddPasswordScuData;
  UINTN                                 NumOfHdd;
  HDD_PASSWORD_HDD_INFO                 *HddInfo;
  CHAR16                                HddModelString[DEVICE_MODEL_NAME_STRING_LENGTH + 1];
  EFI_STRING_ID                         DeviceName;
  UINTN                                 StringIndex;

  SuBrowser = NULL;
  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (Status != EFI_SUCCESS || SuBrowser == NULL) {
    return Status;
  }

  SUCInfo            = SuBrowser->SUCInfo;
  HddPasswordService = SUCInfo->HddPasswordService;
  HddPasswordScuData = SUCInfo->HddPasswordScuData;
  NumOfHdd           = SUCInfo->NumOfHdd;


  if (HddPasswordService == NULL || HddPasswordScuData == NULL || NumOfHdd == 0) {
    return EFI_UNSUPPORTED;
  }

  //
  // Allocate space for creation of UpdateData Buffer
  //
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  EndOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (EndOpCodeHandle != NULL);

  //
  // Create Hii Extend Label OpCode as the start opcode
  //
  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = LABEL_STORAGE_PASSWORD_OPTION;

  //
  // Create Hii Extend Label OpCode as the end opcode
  //
  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = LABEL_STORAGE_PASSWORD_OPTION_END;

  for (Index = 0; Index < NumOfHdd; Index++) {

    HddInfo = HddPasswordScuData[Index].HddInfo;

    //
    // Copy Model String
    //
    ZeroMem (HddModelString, (DEVICE_MODEL_NAME_STRING_SIZE + sizeof(CHAR16)));
    CopyMem (HddModelString, HddInfo->HddModelString, DEVICE_MODEL_NAME_STRING_SIZE);

    //
    // Clean space from tail
    //
    StringIndex = DEVICE_MODEL_NAME_STRING_LENGTH - 1;
    while (HddModelString[StringIndex] == 0x0020) {
      SetMem (&HddModelString[StringIndex], sizeof(CHAR16), 0x0000);
      StringIndex--;
    }

    //
    // Add user name selection.
    //
    DeviceName = HiiSetString (HiiHandle, 0, (EFI_STRING)HddModelString, NULL);

    HiiCreateGotoOpCode (
      StartOpCodeHandle,                                // Container for dynamic created opcodes
      STORAGE_PASSWORD_DEVICE_FORM_ID,                  // Target Form ID
      DeviceName,                                       // Prompt text
      STRING_TOKEN (STR_STORAGE_PASSWORD_DEVICE_HELP),  // Help text
      EFI_IFR_FLAG_CALLBACK,                            // Question flag
      (UINT16)(LABEL_STORAGE_PASSWORD_OPTION + Index)   // Question ID
      );

  }

  HiiCreateEndOpCode (EndOpCodeHandle);

  HiiUpdateForm (
    HiiHandle,
    NULL,
    STORAGE_PASSWORD_FORM_ID,
    StartOpCodeHandle,
    EndOpCodeHandle
    );

  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (EndOpCodeHandle);

  mPasswordConfig.StoragePasswordCallbackState = BROWSER_STATE_VALIDATE_PASSWORD;

  return EFI_SUCCESS;
}

/**
  Create op code for TCG PSID label and update to storage password device form.

  @param[in] HiiHandle              Specific HII handle for Security menu.
  @param[in] HddIndex               The number of drive selected.

**/
VOID
UpdateTcgPsidLabel (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  UINT16                            HddIndex
  )
{
  EFI_IFR_GUID_LABEL                    *StartTcgPsidLabel;
  EFI_IFR_GUID_LABEL                    *EndTcgPsidLabel;
  VOID                                  *StartTcgPsidOpCodeHandle;
  VOID                                  *EndTcgPsidOpCodeHandle;

  StartTcgPsidOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartTcgPsidOpCodeHandle != NULL);

  EndTcgPsidOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (EndTcgPsidOpCodeHandle != NULL);

  //
  // Create Hii Extend Label OpCode as the start PSID option opcode
  //
  StartTcgPsidLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartTcgPsidOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartTcgPsidLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartTcgPsidLabel->Number       = LABEL_STORAGE_PASSWORD_DEVICE_TCG_PSID_OPTION;
  //
  // Create Hii Extend Label OpCode as the end PSID option opcode
  //
  EndTcgPsidLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (EndTcgPsidOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndTcgPsidLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndTcgPsidLabel->Number       = LABEL_STORAGE_PASSWORD_DEVICE_TCG_PSID_OPTION_END;

  if (mStoredPsid[HddIndex].Supported) {
    HiiCreateStringOpCode (
      StartTcgPsidOpCodeHandle,
      (EFI_QUESTION_ID) KEY_PSID_REVERT,
      PASSWORD_CONFIGURATION_VARSTORE_ID,
      OFFSET_OF (PASSWORD_CONFIGURATION, PsidString),
      STRING_TOKEN (STR_TCG_PSID_PROMPT),
      STRING_TOKEN (STR_TCG_PSID_HELP),
      EFI_IFR_FLAG_CALLBACK,
      0,
      PSID_CHARACTER_LENGTH,
      PSID_CHARACTER_LENGTH,
      NULL
      );
  }

  HiiUpdateForm (
    HiiHandle,
    NULL,
    STORAGE_PASSWORD_DEVICE_FORM_ID,
    StartTcgPsidOpCodeHandle,
    EndTcgPsidOpCodeHandle
    );

  HiiFreeOpCodeHandle (StartTcgPsidOpCodeHandle);
  HiiFreeOpCodeHandle (EndTcgPsidOpCodeHandle);
}

EFI_STATUS
StoragePasswordUpdateForm (
  IN  EFI_HII_HANDLE                      HiiHandle,
  IN  UINT16                              HddIndex
  )
{
  EFI_STATUS                            Status;
  EFI_IFR_GUID_LABEL                    *StartUserPasswordLabel;
  EFI_IFR_GUID_LABEL                    *EndUserPasswordLabel;
  EFI_IFR_GUID_LABEL                    *StartMasterPasswordLabel;
  EFI_IFR_GUID_LABEL                    *EndMasterPasswordLabel;
  HDD_PASSWORD_HDD_INFO                 *HddInfo;
  CHAR16                                HddModelString[DEVICE_MODEL_NAME_STRING_LENGTH + 1];
  CHAR16                                *NewString;
  CHAR16                                ModelNameToken[DEVICE_MODEL_NAME_STRING_LENGTH + 3];
  EFI_STRING_ID                         StorageStatusToken;
  VOID                                  *StartUserPasswordOpCodeHandle;
  VOID                                  *EndUserPasswordOpCodeHandle;
  VOID                                  *StartMasterPasswordOpCodeHandle;
  VOID                                  *EndMasterPasswordOpCodeHandle;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  EFI_HDD_PASSWORD_SERVICE_PROTOCOL     *HddPasswordService;
  HDD_PASSWORD_SCU_DATA                 *HddPasswordScuData;
  UINTN                                 NumOfHdd;
  UINTN                                 StringIndex;
  BOOLEAN                               GrayoutUserPassword;

  SuBrowser = NULL;
  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (Status != EFI_SUCCESS || SuBrowser == NULL) {
    return Status;
  }

  SUCInfo            = SuBrowser->SUCInfo;
  HddPasswordService = SUCInfo->HddPasswordService;
  HddPasswordScuData = SUCInfo->HddPasswordScuData;
  NumOfHdd           = SUCInfo->NumOfHdd;
  HddInfo            = HddPasswordScuData[HddIndex].HddInfo;

  if (HddPasswordService == NULL || HddPasswordScuData == NULL || NumOfHdd == 0) {
    return EFI_UNSUPPORTED;
  }

  UpdateTcgPsidLabel (HiiHandle, HddIndex);

  if (mStoredPsid[HddIndex].Supported) {
    //
    // Update PasswordConfig.PsidString
    //
    UpdatePsidString (HddIndex);
  }

  StartUserPasswordOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartUserPasswordOpCodeHandle != NULL);

  EndUserPasswordOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (EndUserPasswordOpCodeHandle != NULL);

  StartMasterPasswordOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartMasterPasswordOpCodeHandle != NULL);

  EndMasterPasswordOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (EndMasterPasswordOpCodeHandle != NULL);

  //
  // Create Hii Extend Label OpCode as the start storage user password opcode
  //
  StartUserPasswordLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartUserPasswordOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartUserPasswordLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartUserPasswordLabel->Number       = LABEL_STORAGE_PASSWORD_DEVICE_USER_PASSWORD_OPTION;

  //
  // Create Hii Extend Label OpCode as the end storage user password opcode
  //
  EndUserPasswordLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (EndUserPasswordOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndUserPasswordLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndUserPasswordLabel->Number       = LABEL_STORAGE_PASSWORD_DEVICE_USER_PASSWORD_OPTION_END;

  //
  // Create Hii Extend Label OpCode as the start storage master password opcode
  //
  StartMasterPasswordLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartMasterPasswordOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartMasterPasswordLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartMasterPasswordLabel->Number       = LABEL_STORAGE_PASSWORD_DEVICE_MASTER_PASSWORD_OPTION;

  //
  // Create Hii Extend Label OpCode as the end storage master password opcode
  //
  EndMasterPasswordLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (EndMasterPasswordOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndMasterPasswordLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndMasterPasswordLabel->Number       = LABEL_STORAGE_PASSWORD_DEVICE_MASTER_PASSWORD_OPTION_END;

  //
  // Copy Model String
  //
  ZeroMem (HddModelString, (DEVICE_MODEL_NAME_STRING_SIZE + sizeof(CHAR16)));
  CopyMem (HddModelString, HddInfo->HddModelString, DEVICE_MODEL_NAME_STRING_SIZE);

  //
  // Clean space from tail
  //
  StringIndex = DEVICE_MODEL_NAME_STRING_LENGTH - 1;
  while (HddModelString[StringIndex] == 0x0020) {
    SetMem (&HddModelString[StringIndex], sizeof(CHAR16), 0x0000);
    StringIndex--;
  }

  ZeroMem (ModelNameToken, (DEVICE_MODEL_NAME_STRING_SIZE + sizeof(CHAR16) * 3));


  StrCatS (ModelNameToken, sizeof(ModelNameToken) / sizeof(CHAR16), L"[");
  StrCatS (ModelNameToken, sizeof(ModelNameToken) / sizeof(CHAR16), HddModelString);
  StrCatS (ModelNameToken, sizeof(ModelNameToken) / sizeof(CHAR16), L"]");

  HiiSetString (
    HiiHandle,
    STRING_TOKEN (STR_STORAGE_DEVICE_NAME_STRING),
    ModelNameToken,
    NULL
    );


  //
  // Update Information
  //
  if (HddPasswordScuData[HddIndex].Flag == SECURITY_LOCK) {
    //
    // Harddisk Security Enable [Unlock]
    //
    StorageStatusToken = STRING_TOKEN (STR_HDD_PSW_LOCK);
  } else if (HddPasswordScuData[HddIndex].Flag == CHANGE_PASSWORD ||
             ((HddPasswordScuData[HddIndex].Flag != DISABLE_PASSWORD &&
               HddPasswordScuData[HddIndex].Flag != NO_ACCESS_PASSWORD) &&
              HddPasswordScuData[HddIndex].MasterFlag == CHANGE_PASSWORD)) {
    //
    // Password Changed
    // 1. User Password changed.
    // 2. Security enabled, only master password changed.
    //
    StorageStatusToken = STRING_TOKEN (STR_HDD_PSW_CHANGE);
  } else if (HddPasswordScuData[HddIndex].Flag == DISABLE_PASSWORD ||
             HddPasswordScuData[HddIndex].MasterFlag == DISABLE_PASSWORD ) {
    //
    // Harddisk Security Disable
    //
    StorageStatusToken = STRING_TOKEN (STR_HDD_PSW_DISABLE);
  } else if (HddPasswordScuData[HddIndex].Flag == ENABLE_PASSWORD) {
    //
    // Harddisk Security Unlock
    //
    StorageStatusToken = STRING_TOKEN (STR_HDD_PSW_UNLOCK);
  } else {
    //
    //Harddisk don't access password
    //
    StorageStatusToken = STRING_TOKEN (STR_HDD_PSW_NONACCESS);
  }

  NewString = HiiGetString (
                HiiHandle,
                StorageStatusToken,
                NULL
                );
  HiiSetString (
    HiiHandle,
    STRING_TOKEN (STR_STORAGE_SECURITY_MODE_STATUS),
    NewString,
    NULL
    );

  //
  // Update Password Configuration Structure
  //
  mPasswordConfig.SelectedScuDataIndex = (UINT8)HddIndex;
  if (HddPasswordScuData[HddIndex].Flag == SECURITY_LOCK) {
    mPasswordConfig.SetHddPasswordFlag = 0;
    mPasswordConfig.UnlockHddPasswordFlag = 1;
  } else {
    mPasswordConfig.SetHddPasswordFlag = 1;
    mPasswordConfig.UnlockHddPasswordFlag = 0;
  }
  //
  // According to ATA8-ACS-4 spec, section 4.17.4
  // When security is enabled, master password is indicated by master password capability
  // High - user and master password are interchangeable
  // Maximum - master password cannot be used for security unlock, security disable
  // Only security erase unit command is available. Thus when the capability bit
  // is set maximum, we will hide the master password option in SCU
  //
  if ((HddInfo->HddSecurityStatus & HDD_MASTER_CAPABILITY)) {
    mPasswordConfig.UseMasterPassword = 0;
  } else {
    mPasswordConfig.UseMasterPassword = 1;
    if (HddPasswordScuData[HddIndex].MasterFlag == MASTER_PASSWORD_GRAYOUT) {
      mPasswordConfig.MasterHddPasswordFlag = 0;
    } else {
      mPasswordConfig.MasterHddPasswordFlag = 1;
    }
  }
  GrayoutUserPassword = FALSE;

  //
  // Handle TCG Opal case
  //
  if (HddInfo->StorageTcgSecuritySupported) {
    //
    // Always use master
    //
    mPasswordConfig.UseMasterPassword = 1;
    mPasswordConfig.MasterHddPasswordFlag = 1;
    //
    // Grayout USER password if password is not set yet by design, in order to let MASTER password to be set first
    //
    GrayoutUserPassword = (HddPasswordScuData[HddIndex].Flag == NO_ACCESS_PASSWORD);
    //
    // Handle TCG Policy
    //
    if (mTakeOwnershipByUserPassword) {
      //
      // Don't grayout USER password in this case
      //
      GrayoutUserPassword = FALSE;
    }
    if (mSingleUserPassword) {
      //
      // Disable MASTER password support if single user password policy set
      //
      mPasswordConfig.UseMasterPassword = 0;
      mPasswordConfig.MasterHddPasswordFlag = 0;
      //
      // Don't grayout USER password in this case
      //
      GrayoutUserPassword = FALSE;
    }
  }

  if (mPasswordConfig.UnlockHddPasswordFlag) {
    HiiCreatePasswordOpCodeEx (
      StartUserPasswordOpCodeHandle,
      (EFI_QUESTION_ID)(KEY_CHECK_STORAGE_PASSWORD),
      PASSWORD_NAME_VALUE_VARSTORE_ID,
      STRING_TOKEN (STR_HDD_PASSWORD_VAR_NAME),
      STRING_TOKEN (STR_STORAGE_CHECK_PASSWORD_STRING),
      STRING_TOKEN (STR_LOCK_ONE_STORAGE_SECURITY_TITLE_HELP),
      0x0,
      EFI_IFR_FLAG_CALLBACK,
      PcdGet16 (PcdH2OHddPasswordMinLength),
      MIN (PcdGet16 (PcdH2OHddPasswordMaxLength), HddInfo->MaxPasswordLengthSupport)
      );
  } else {
    if (GrayoutUserPassword) {
      //
      // Grayout User password
      //
      HiiCreateGrayOutIfOpCodeEx (StartUserPasswordOpCodeHandle, 1);
      HiiCreateTrueOpCode (StartUserPasswordOpCodeHandle);
    }
    HiiCreatePasswordOpCodeEx (
      StartUserPasswordOpCodeHandle,
      (EFI_QUESTION_ID)(KEY_SET_STORAGE_PASSWORD),
      PASSWORD_NAME_VALUE_VARSTORE_ID,
      STRING_TOKEN (STR_HDD_PASSWORD_VAR_NAME),
      STRING_TOKEN (STR_STORAGE_PASSWORD_PROMPT),
      STRING_TOKEN (STR_ONE_STORAGE_SECURITY_TITLE_HELP),
      0x0,
      EFI_IFR_FLAG_CALLBACK,
      PcdGet16 (PcdH2OHddPasswordMinLength),
      MIN (PcdGet16 (PcdH2OHddPasswordMaxLength), HddInfo->MaxPasswordLengthSupport)
      );
    if (GrayoutUserPassword) {
      //
      // Grayout User password
      //
      HiiCreateEndOpCode (StartUserPasswordOpCodeHandle);
    }
  }

  if (mPasswordConfig.UseMasterPassword == 1) {
    if (mPasswordConfig.MasterHddPasswordFlag == 0) {
      HiiCreateGrayOutIfOpCodeEx (StartMasterPasswordOpCodeHandle, 1);
      HiiCreateTrueOpCode (StartMasterPasswordOpCodeHandle);
    }

    HiiCreatePasswordOpCodeEx (
      StartMasterPasswordOpCodeHandle,
      (EFI_QUESTION_ID)((HddPasswordScuData[HddIndex].Flag == SECURITY_LOCK) ? KEY_CHECK_MASTER_STORAGE_PASSWORD : KEY_SET_MASTER_STORAGE_PASSWORD),
      PASSWORD_NAME_VALUE_VARSTORE_ID,
      STRING_TOKEN (STR_MASTER_HDD_PASSWORD_VAR_NAME),
      STRING_TOKEN (STR_MASTER_STORAGE_PASSWORD_PROMPT),
      STRING_TOKEN (STR_ONE_STORAGE_MASTER_SECURITY_TITLE_HELP),
      0x0,
      EFI_IFR_FLAG_CALLBACK,
      PcdGet16 (PcdH2OHddPasswordMinLength),
      MIN (PcdGet16 (PcdH2OHddPasswordMaxLength), HddInfo->MaxPasswordLengthSupport)
      );

    if (mPasswordConfig.MasterHddPasswordFlag == 0) {
      HiiCreateEndOpCode (EndMasterPasswordOpCodeHandle);
    }
  }

  HiiUpdateForm (
    HiiHandle,
    NULL,
    STORAGE_PASSWORD_DEVICE_FORM_ID,
    StartUserPasswordOpCodeHandle,
    EndUserPasswordOpCodeHandle
    );

  HiiUpdateForm (
    HiiHandle,
    NULL,
    STORAGE_PASSWORD_DEVICE_FORM_ID,
    StartMasterPasswordOpCodeHandle,
    EndMasterPasswordOpCodeHandle
    );

  HiiFreeOpCodeHandle (StartUserPasswordOpCodeHandle);
  HiiFreeOpCodeHandle (EndUserPasswordOpCodeHandle);

  HiiFreeOpCodeHandle (StartMasterPasswordOpCodeHandle);
  HiiFreeOpCodeHandle (EndMasterPasswordOpCodeHandle);

  return EFI_SUCCESS;
}

EFI_STATUS
ValidateStoragePassword (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  UINT16                            HddIndex,
  IN  BOOLEAN                           UserOrMaster,
  IN  CHAR16                            *Password
  )
{
  EFI_STATUS                            Status;
  KERNEL_CONFIGURATION                  *MyIfrNVData;
  UINT8                                 PasswordToHdd[HDD_PASSWORD_MAX_NUMBER + 1];
  UINTN                                 PasswordToHddLength;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  EFI_HDD_PASSWORD_SERVICE_PROTOCOL     *HddPasswordService;
  HDD_PASSWORD_SCU_DATA                 *HddPasswordScuData;
  UINTN                                 NumOfHdd;

  SuBrowser = NULL;
  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (Status != EFI_SUCCESS || SuBrowser == NULL) {
    //
    //  Return EFI_NOT_AVAILABLE_YET or EFI_UNSUPPORTED to let browser terminate password flow.
    //  If callback function return other status, the password flow won't terminate.
    //
    return EFI_NOT_AVAILABLE_YET;
  }

  SUCInfo            = SuBrowser->SUCInfo;
  HddPasswordService = SUCInfo->HddPasswordService;
  HddPasswordScuData = SUCInfo->HddPasswordScuData;
  NumOfHdd           = SUCInfo->NumOfHdd;

  if (HddPasswordService == NULL || HddPasswordScuData == NULL || NumOfHdd == 0) {
    //
    //  Return EFI_NOT_AVAILABLE_YET or EFI_UNSUPPORTED to let browser terminate password flow.
    //  If callback function return other status, the password flow won't terminate.
    //
    return EFI_NOT_AVAILABLE_YET;
  }

  MyIfrNVData = (KERNEL_CONFIGURATION *) SuBrowser->SCBuffer;

  if (HddPasswordScuData[HddIndex].Flag == ENABLE_PASSWORD) {
  }
  if (HddPasswordScuData[HddIndex].Flag == DISABLE_PASSWORD) {
    //
    //  return EFI_SUCCESS to display two password items for new password and confirm new password
    //
    return EFI_SUCCESS;
  }
  if (HddPasswordScuData[HddIndex].Flag == NO_ACCESS_PASSWORD) {
    //
    //  return EFI_SUCCESS to display two password items for new password and confirm new password
    //
    return EFI_SUCCESS;
  }

  if (Password[0] == 0x00 && HddPasswordScuData[HddIndex].Flag == SECURITY_LOCK) {
    //
    // If First time enter here, HDD is LOCK and not yet ask user input HDD password
    //

    //
    //  return EFI_CRC_ERROR to display one password item for unlock HDD first by correct password
    //
    return EFI_CRC_ERROR;
  }

  if (HddPasswordScuData[HddIndex].HddInfo->StorageTcgSecuritySupported) {
    if (Password[0] == 0) {
      if (UserOrMaster == USER_PSW &&
        !IsTcgUser1Enabled (HddPasswordService, HddPasswordScuData[HddIndex].HddInfo)) {
        //
        //  return EFI_SUCCESS to display two password items for new password and confirm new password
        //
        return EFI_SUCCESS;
      }
      //
      //  Check for an existing password by sending a NULL string value.
      //  Returns an error, indicating that there is a preexisting password.
      //
      return EFI_NOT_READY;
    }

    if (HddPasswordScuData[HddIndex].Flag != SECURITY_LOCK) {
      //
      //  Directly checking  password which is stored in HDD by using Unlock command.
      //
      ZeroMem (PasswordToHdd, (HDD_PASSWORD_MAX_NUMBER + 1));
      PasswordToHddLength = 0;

      Status = HddPasswordService->PasswordStringProcess (
                                     HddPasswordService,
                                     UserOrMaster,
                                     Password,
                                     StrLen (Password),
                                     (VOID **)&PasswordToHdd,
                                     &PasswordToHddLength
                                     );

      if (Status != EFI_SUCCESS) {
        //
        //  Return EFI_NOT_AVAILABLE_YET or EFI_UNSUPPORTED to let browser terminate password flow.
        //
        return EFI_NOT_AVAILABLE_YET;
      }

      Status = HddPasswordService->UnlockHddPassword (
                                     HddPasswordService,
                                     HddPasswordScuData[HddIndex].HddInfo,
                                     UserOrMaster,
                                     PasswordToHdd,
                                     PasswordToHddLength
                                     );
      if (EFI_ERROR (Status)) {
        //
        //  user input password is  NOT  match the password in HDD
        //  return EFI_NOT_READY to clean dialog and let browser terminate password flow.
        //
        return EFI_NOT_READY;
      }

      if (mSingleUserPassword) {
        //
        // Cache the old password for single user password scenario
        //
        StrCpyS (
          mOrgPassword[HddIndex].PasswordStr,
          sizeof (mOrgPassword[HddIndex].PasswordStr) / sizeof (CHAR16),
          Password
          );
      }

      if (UserOrMaster == MASTER_PSW) {
        //
        // Update the disable password
        //
        StrCpyS (
          HddPasswordScuData[HddIndex].DisableInputString,
          sizeof(HddPasswordScuData[HddIndex].DisableInputString) / sizeof(CHAR16),
          Password
          );

        HddPasswordScuData[HddIndex].DisableAllType = MASTER_PSW;
        HddPasswordScuData[HddIndex].DisableStringLength = StrLen (HddPasswordScuData[HddIndex].DisableInputString);
      }

      Status = StoragePasswordUpdateForm (
                 HiiHandle,
                 HddIndex
                 );

      //
      //  input password is match current set HDD password
      //  return EFI_SUCCESS to display another two password items for new password and confirm new password
      //
      return EFI_SUCCESS;
    }
  } else {
    if (Password[0] == 0) {
      //
      //  Check for an existing password by sending a NULL string value.
      //  Returns an error, indicating that there is a preexisting password.
      //
      return EFI_NOT_READY;
    }
    if (HddPasswordScuData[HddIndex].Flag != SECURITY_LOCK) {
      //
      //  To check previous password for change password or disable password
      //

      if (HddPasswordScuData[HddIndex].Flag == CHANGE_PASSWORD ||
          HddPasswordScuData[HddIndex].MasterFlag == CHANGE_PASSWORD) {
        //
        //  User has changed new password, but the new password is not set to HDD.
        //  The new password is stored in buffer until "Save and Exit" to set to HDD by SetSecurityStatus()
        //
        if (UserOrMaster == USER_PSW && HddPasswordScuData[HddIndex].InputString[0] != 0) {
          if (StrCmp (HddPasswordScuData[HddIndex].InputString, Password) == 0) {
            //
            //  input password is match current set User HDD password
            //  return EFI_SUCCESS to display another two password items for new password and confirm new password
            //
            return EFI_SUCCESS;
          } else {
            //
            //  input password is  NOT  match current set User HDD password
            //  return EFI_NOT_READY to clean dialog and let browser terminate password flow
            //
            return EFI_NOT_READY;
          }
        } else if (UserOrMaster == MASTER_PSW && HddPasswordScuData[HddIndex].MasterInputString[0] != 0) {
          if (StrCmp (HddPasswordScuData[HddIndex].MasterInputString, Password) == 0) {
            //
            //  input password is match current set Master password
            //  return EFI_SUCCESS to display another two password items for new password and confirm new password
            //
            return EFI_SUCCESS;
          } else {
            //
            //  input password is  NOT  match current set Master HDD password
            //  return EFI_NOT_READY to clean dialog and let browser terminate password flow.
            //
            return EFI_NOT_READY;
          }
        } else if (HddPasswordScuData[HddIndex].InputString[0] != 0 &&
                   HddPasswordScuData[HddIndex].MasterInputString[0] != 0) {
          //
          //  input password is  NOT  match current set password
          //  return EFI_NOT_READY to clean dialog and let browser terminate password flow.
          //
          return EFI_NOT_READY;
        }
      }

      if (HddPasswordScuData[HddIndex].DisableAllType == UserOrMaster &&
          StrCmp (HddPasswordScuData[HddIndex].DisableInputString, Password) == 0) {
        //
        //  Check previos HDD password stored in variable.
        //

        Status = StoragePasswordUpdateForm (
                   HiiHandle,
                   HddIndex
                   );

        //
        //  input password is match current set HDD password
        //  return EFI_SUCCESS to display another two password items for new password and confirm new password
        //
        return EFI_SUCCESS;
      } else {
        //
        //  Directly checking  password which is stored in HDD by using Unlock command.
        //

        ZeroMem (PasswordToHdd, (HDD_PASSWORD_MAX_NUMBER + 1));
        PasswordToHddLength = 0;

        Status = HddPasswordService->PasswordStringProcess (
                                       HddPasswordService,
                                       UserOrMaster,
                                       Password,
                                       StrLen (Password),
                                       (VOID **)&PasswordToHdd,
                                       &PasswordToHddLength
                                       );

        if (Status != EFI_SUCCESS) {
          //
          //  Return EFI_NOT_AVAILABLE_YET or EFI_UNSUPPORTED to let browser terminate password flow.
          //
          return EFI_NOT_AVAILABLE_YET;
        }

        Status = HddPasswordService->UnlockHddPassword (
                                       HddPasswordService,
                                       HddPasswordScuData[HddIndex].HddInfo,
                                       UserOrMaster,
                                       PasswordToHdd,
                                       PasswordToHddLength
                                       );
        if (EFI_ERROR (Status)) {
          //
          //  user input password is  NOT  match the password in HDD
          //  return EFI_NOT_READY to clean dialog and let browser terminate password flow.
          //
          return EFI_NOT_READY;
        }

        Status = StoragePasswordUpdateForm (
                   HiiHandle,
                   HddIndex
                   );

        //
        //  input password is match current set HDD password
        //  return EFI_SUCCESS to display another two password items for new password and confirm new password
        //
        return EFI_SUCCESS;
      }
    }
  }
  //
  //  Unlock HDD in SCU
  //
  ZeroMem (PasswordToHdd, (HDD_PASSWORD_MAX_NUMBER + 1));
  PasswordToHddLength = 0;

  Status = HddPasswordService->PasswordStringProcess (
                                 HddPasswordService,
                                 UserOrMaster,
                                 Password,
                                 StrLen (Password),
                                 (VOID **)&PasswordToHdd,
                                 &PasswordToHddLength
                                 );

  if (EFI_ERROR(Status)) {
    //
    //  Return EFI_NOT_AVAILABLE_YET or EFI_UNSUPPORTED to let browser terminate password flow.
    //
    return EFI_NOT_AVAILABLE_YET;
  }

  Status = HddPasswordService->UnlockHddPassword (
                                 HddPasswordService,
                                 HddPasswordScuData[HddIndex].HddInfo,
                                 UserOrMaster,
                                 PasswordToHdd,
                                 PasswordToHddLength
                                 );
  if (EFI_ERROR (Status)) {
    //
    //  Unlock HDD in SCU fail
    //  return EFI_NOT_READY to clean dialog
    //
    return EFI_NOT_READY;
  }

  ZeroMem (
    HddPasswordScuData[HddIndex].DisableInputString,
    ((HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (CHAR16))
    );
  StrCpyS (
    HddPasswordScuData[HddIndex].DisableInputString,
    sizeof(HddPasswordScuData[HddIndex].DisableInputString) / sizeof(CHAR16),
    Password
    );
  HddPasswordScuData[HddIndex].DisableStringLength = StrLen (Password);
  HddPasswordScuData[HddIndex].DisableAllType = UserOrMaster;

  if (UserOrMaster != MASTER_PSW) {
    //
    //  If input HDD User Password,
    //  let Set HDD Master Password grayout
    //

    HddPasswordScuData[HddIndex].MasterFlag = MASTER_PASSWORD_GRAYOUT;
  }

  if ((UserOrMaster == MASTER_PSW) && (HddPasswordScuData[HddIndex].Flag != SECURITY_LOCK)) {
    HddPasswordScuData[HddIndex].MasterFlag = ENABLE_PASSWORD;
    Status = StoragePasswordUpdateForm (
               HiiHandle,
               HddIndex
               );

    //
    //  input password is match current set HDD password
    //  return EFI_SUCCESS to display another two password items for new password and confirm new password
    //
    return EFI_SUCCESS;
  }
  HddPasswordScuData[HddIndex].Flag = ENABLE_PASSWORD;


  Status = StoragePasswordUpdateForm (
             HiiHandle,
             HddIndex
             );

  //
  //  If unlock successfully, return EFI_UNSUPPORTED to not change StoragePasswordCallbackState
  //
  return EFI_UNSUPPORTED;

}

EFI_STATUS
SetStoragePassword (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  UINT16                            HddIndex,
  IN  BOOLEAN                           UserOrMaster,
  IN  CHAR16                            *Password
  )
{
  EFI_STATUS                            Status;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  EFI_HDD_PASSWORD_SERVICE_PROTOCOL     *HddPasswordService;
  HDD_PASSWORD_SCU_DATA                 *HddPasswordScuData;
  UINTN                                 NumOfHdd;
  CHAR16                                *StringPtr;
  EFI_INPUT_KEY                         Key;

  SuBrowser = NULL;
  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (Status != EFI_SUCCESS || SuBrowser == NULL) {

    //
    //  Return EFI_NOT_AVAILABLE_YET or EFI_UNSUPPORTED to let browser terminate password flow.
    //
    return EFI_NOT_AVAILABLE_YET;
  }

  SUCInfo            = SuBrowser->SUCInfo;
  HddPasswordService = SUCInfo->HddPasswordService;
  HddPasswordScuData = SUCInfo->HddPasswordScuData;
  NumOfHdd           = SUCInfo->NumOfHdd;

  if (HddPasswordService == NULL || HddPasswordScuData == NULL || NumOfHdd == 0) {
    //
    //  Return EFI_NOT_AVAILABLE_YET or EFI_UNSUPPORTED to let browser terminate password flow.
    //
    return EFI_NOT_AVAILABLE_YET;
  }

  if (Password[0] != 0x00) {
    //
    // Password String is not NULL => Change New Password
    //

    if (UserOrMaster == USER_PSW) {
      if (HddPasswordScuData[HddIndex].HddInfo->StorageTcgSecuritySupported &&
          HddPasswordScuData[HddIndex].Flag == NO_ACCESS_PASSWORD) {
        //
        // Single password scenario prompt
        //
        StringPtr = HiiGetString (HiiHandle, STRING_TOKEN(STR_WARNING_TCG_SINGLE_PASSWORD_PROMPT), NULL);
        SuBrowser->H2ODialog->ConfirmDialog (DlgOk, FALSE, 0, NULL, &Key, StringPtr);
        FreePool (StringPtr);
      }

      //
      // Set flag relative to User password
      //

      HddPasswordScuData[HddIndex].Flag = CHANGE_PASSWORD;
      if (HddPasswordScuData[HddIndex].DisableAllType != MASTER_PSW) {
        HddPasswordScuData[HddIndex].MasterFlag = MASTER_PASSWORD_GRAYOUT;
      }

      //
      // Store password string to User password buffer
      //

      ZeroMem (
        HddPasswordScuData[HddIndex].InputString,
        ((HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (CHAR16))
        );

      StrCpyS (
        HddPasswordScuData[HddIndex].InputString,
        sizeof(HddPasswordScuData[HddIndex].InputString) / sizeof(CHAR16),
        Password
        );

      HddPasswordScuData[HddIndex].StringLength = StrLen (Password);
    } else {
      //
      // Set flag relative to Master password
      //

      HddPasswordScuData[HddIndex].MasterFlag = CHANGE_PASSWORD;

      //
      // Store password string to Master password buffer
      //

      ZeroMem (
        HddPasswordScuData[HddIndex].MasterInputString,
        ((HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (CHAR16))
        );

      StrCpyS (
        HddPasswordScuData[HddIndex].MasterInputString,
        sizeof(HddPasswordScuData[HddIndex].MasterInputString) / sizeof(CHAR16),
        Password
        );
    }
  } else {
    //
    // Password String is NULL => Disable password
    //

    HddPasswordScuData[HddIndex].Flag = DISABLE_PASSWORD;
    if (!HddPasswordScuData[HddIndex].HddInfo->StorageTcgSecuritySupported
        || (UserOrMaster == MASTER_PSW)
        || (mSingleUserPassword)) {
      HddPasswordScuData[HddIndex].MasterFlag = DISABLE_PASSWORD;
    }
    HddPasswordScuData[HddIndex].DisableAllType = NO_ACCESS_PSW;

    ZeroMem (
      HddPasswordScuData[HddIndex].InputString,
      ((HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (CHAR16))
      );

    ZeroMem (
      HddPasswordScuData[HddIndex].MasterInputString,
      ((HDD_PASSWORD_MAX_NUMBER + 1) * sizeof (CHAR16))
      );

  }

  UpdateAllHddPasswordFlag (
    HiiHandle,
    HddPasswordScuData,
    NumOfHdd
    );

  Status = StoragePasswordUpdateForm (
             HiiHandle,
             HddIndex
             );

  //
  //  Return EFI_SUCCESS if password was accepted and saved.
  //
  return EFI_SUCCESS;
}

/**
  PSID revert callback. Record and normalize PSID value.

  @param[in] HiiHandle              Specific HII handle for Security menu.
  @param[in] Type                   The type of value for the question.
  @param[in] Value                  A pointer to the data being sent to the original exporting driver.
  @param[in] HddIndex               The number of drive selected.

  @retval EFI_SUCCESS               Operation completed successfully.
  @retval EFI_INVALID_PARAMETER     The data is not a string type.
                                    The data string is not found.

**/
EFI_STATUS
PsidRevertCallback (
  IN  EFI_HII_HANDLE                      HiiHandle,
  IN  UINT8                               Type,
  IN  EFI_IFR_TYPE_VALUE                  *Value,
  IN  UINT16                              HddIndex
  )
{
  EFI_STATUS                            Status;
  UINTN                                 Index;
  UINTN                                 RawPsidLength;
  CHAR16                                *RawPsid;
  CHAR16                                *StringPtr;
  EFI_INPUT_KEY                         Key;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;

  SuBrowser = NULL;
  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (Status != EFI_SUCCESS || SuBrowser == NULL) {
    //
    //  Return EFI_NOT_AVAILABLE_YET or EFI_UNSUPPORTED to let browser terminate password flow.
    //
    return EFI_NOT_AVAILABLE_YET;
  }

  if (Type != EFI_IFR_TYPE_STRING) {
    return  EFI_INVALID_PARAMETER;
  }

  if (Value->string == 0) {
    return EFI_SUCCESS;
  }

  RawPsid = HiiGetString (HiiHandle, Value->string, NULL);
  if (RawPsid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  RawPsidLength = StrLen (RawPsid);
  for (Index = 0; Index < RawPsidLength; Index++) {
    //
    // Change small to capital letter
    //
    if (RawPsid[Index] >= 'a' && RawPsid[Index] <= 'z') {
      RawPsid[Index] = RawPsid[Index] - 'a' + 'A';
    }
    //
    // Unicode to ASCII
    //
    mStoredPsid[HddIndex].Psid[Index] = (UINT8)RawPsid[Index];
  }

  //
  // PSID validity check
  //
  Status = SuBrowser->SUCInfo->HddPasswordService->SendHddSecurityCmd (
                                                     SuBrowser->SUCInfo->HddPasswordService,
                                                     H2O_HDD_PASSWORD_TCG_PSID_REVERT,
                                                     SuBrowser->SUCInfo->HddPasswordScuData[HddIndex].HddInfo,
                                                     MASTER_PSW,
                                                     mStoredPsid[HddIndex].Psid,
                                                     sizeof (mStoredPsid[HddIndex].Psid)
                                                     );
  if (EFI_ERROR (Status)) {
    //
    // Prompt for PSID validation failure
    //
    StringPtr = HiiGetString (HiiHandle, STRING_TOKEN(STR_WARNING_TCG_PSID_INVALID_PROMPT), NULL);
    SuBrowser->H2ODialog->ConfirmDialog (DlgOk, FALSE, 0, NULL, &Key, StringPtr);
    FreePool (StringPtr);
    //
    // Clear the stored PSID
    //
    ZeroMem (mStoredPsid[HddIndex].Psid, sizeof (mStoredPsid[HddIndex].Psid));
  }

  UpdatePsidString (HddIndex);

  FreePool (RawPsid);

  return EFI_SUCCESS;
}

EFI_STATUS
StoragePasswordCallback (
  IN  EFI_HII_HANDLE                      HiiHandle,
  IN  UINT8                               Type,
  IN  EFI_IFR_TYPE_VALUE                  *Value,
  IN  UINT16                              HddIndex,
  IN  BOOLEAN                             UserOrMaster
  )
{
  EFI_STATUS               Status;
  CHAR16                   *Password;

  if (Type != EFI_IFR_TYPE_STRING) {
    return  EFI_INVALID_PARAMETER;
  }

  if (Value->string == 0) {
    //
    // If Value->String == 0, only need reset the state machine to BROWSER_STATE_VALIDATE_PASSWORD
    //
    mPasswordConfig.StoragePasswordCallbackState = BROWSER_STATE_VALIDATE_PASSWORD;
    return EFI_SUCCESS;
  }

  Password = NULL;
  Password = HiiGetString (HiiHandle, Value->string, NULL);
  //
  // When try to set a new password, user will be chanlleged with old password.
  // The Callback is responsible for validating old password input by user,
  // If Callback return EFI_SUCCESS, it indicates validation pass.
  //
  switch (mPasswordConfig.StoragePasswordCallbackState) {

  case BROWSER_STATE_VALIDATE_PASSWORD:
    Status = ValidateStoragePassword (
               HiiHandle,
               HddIndex,
               UserOrMaster,
               Password
               );
    if (Status == EFI_SUCCESS) {
      mPasswordConfig.StoragePasswordCallbackState = BROWSER_STATE_SET_PASSWORD;
    }
    break;

  case BROWSER_STATE_SET_PASSWORD:
    Status = SetStoragePassword (
               HiiHandle,
               HddIndex,
               UserOrMaster,
               Password
               );
    mPasswordConfig.StoragePasswordCallbackState = BROWSER_STATE_VALIDATE_PASSWORD;
    break;

  default:
    Status = EFI_NOT_AVAILABLE_YET;
    break;
  }

  if (Password != NULL) {
    FreePool (Password);
  }

  return Status;
}

