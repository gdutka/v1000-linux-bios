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

#pragma optimize( "", off )
#include "SetupUtility.h"

//#define SECURITY_CALLBACK_HOOK_SUPPORT
#define EFI_SECURITY_MENU_SIGNATURE SIGNATURE_32('S','e','c','M')
#define EFI_SECURITY_CALLBACK_INFO_FROM_THIS(a) CR (a, EFI_CALLBACK_INFO, DriverCallback, EFI_SECURITY_MENU_SIGNATURE)

#define EXIT_CALLBACK_HOOK_SUPPORT
#define EFI_EXIT_MENU_SIGNATURE SIGNATURE_32('E','x','i','M')
#define EFI_EXIT_CALLBACK_INFO_FROM_THIS(a) CR (a, EFI_CALLBACK_INFO, DriverCallback, EFI_EXIT_MENU_SIGNATURE)

BOOLEAN                     PSWDMOD = FALSE;

#ifdef  SECURITY_CALLBACK_HOOK_SUPPORT
BOOLEAN                      HasHookSecurityCallback = FALSE;
EFI_HII_ACCESS_FORM_CALLBACK OriginalSecurityCallbackRoutine;
#endif

#ifdef  EXIT_CALLBACK_HOOK_SUPPORT
BOOLEAN                      HasHookExitCallback = FALSE;
EFI_HII_ACCESS_FORM_CALLBACK OriginalExitCallbackRoutine;
#endif

VOID  *mFormCallbackProtocolEventlRegistration;

/**
  Set HddPswdModified flag to 1 so a PCI full reset will be triggered.

  @param   None.

  @retval  None.
**/
VOID
SetHddPswdModifiedFlag ()
{
  EFI_STATUS    Status;
  UINTN			    HDPswModSize;
  UINTN			    HDPswModData;

  if (PSWDMOD == FALSE) {
    HDPswModSize = 1;
    HDPswModData = 0xff;
    Status = gRT->SetVariable (
            L"HDPassWordModified",
            &gHddPasswordModifiedGuid,
            EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS ,
            HDPswModSize,
            &HDPswModData
            );
    PSWDMOD = TRUE;
  }

}

#ifdef  SECURITY_CALLBACK_HOOK_SUPPORT
/**
  This is the Security page callback function that is called to provide results data to the driver.
  This dataconsists of a unique key which is used to identify what data is either being passed
  back or being asked for.

  @param[in]     This            Pointer to Form Callback protocol instance.
  @param[in]     KeyValue        A unique value which is sent to the original exporting driver so that it
                                 can identify the type of data to expect.  The format of the data tends to
                                 vary based on the op-code that geerated the callback.
  @param[in]     Data            A pointer to the data being sent to the original exporting driver.
  @param[in]     Packet          The result data returned.

  @retval        EFI_SUCCESS  -  Must return success upon completion.
  @retval        Others       -  Error occurred during execution.
**/
EFI_STATUS
SecurityCallbackHook (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS                            Status, OriginalStatus;
  CHAR16                                *StringPtr;
  CHIPSET_CONFIGURATION                 *MyIfrNVData;
  EFI_HII_HANDLE                        HiiHandle;
  EFI_CALLBACK_INFO                     *CallbackInfo;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL    *SetupUtilityBrowserProtocol;
  UINTN                                 BufferSize;

  OriginalStatus = OriginalSecurityCallbackRoutine (This, Action, QuestionId, Type, Value, ActionRequest);

  Status = gBS->LocateProtocol (
               &gEfiSetupUtilityBrowserProtocolGuid,
               NULL,
               &SetupUtilityBrowserProtocol
               );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  SuBrowser = EFI_SETUP_UTILITY_BROWSER_FROM_THIS (SetupUtilityBrowserProtocol);

  CallbackInfo = EFI_SECURITY_CALLBACK_INFO_FROM_THIS (This);
  if (QuestionId == GET_SETUP_CONFIG || QuestionId == SET_SETUP_CONFIG) {
    BufferSize = PcdGet32 (PcdSetupConfigSize);
    Status = SetupVariableConfig (
               NULL,
               NULL,
               BufferSize,
               (UINT8 *) gSUBrowser->SCBuffer,
               (BOOLEAN)(QuestionId == GET_SETUP_CONFIG)
               );
    return Status;
  }
  BufferSize = PcdGet32 (PcdSetupConfigSize);
  Status = SetupVariableConfig (
             NULL,
             NULL,
             BufferSize,
             (UINT8 *) gSUBrowser->SCBuffer,
             TRUE
             );
  MyIfrNVData = (CHIPSET_CONFIGURATION *) gSUBrowser->SCBuffer;

  Status = EFI_SUCCESS;
  StringPtr = NULL;
  HiiHandle = CallbackInfo->HiiHandle;
  SUCInfo   = SuBrowser->SUCInfo;

  switch (QuestionId) {

  default:
    break;

  }

  BufferSize = PcdGet32 (PcdSetupConfigSize);
  SetupVariableConfig (
    NULL,
    NULL,
    BufferSize,
    (UINT8 *) gSUBrowser->SCBuffer,
    FALSE
    );

  return OriginalStatus;
}
#endif

#ifdef  EXIT_CALLBACK_HOOK_SUPPORT
/**
  This is the callback function for the Exit Menu.

Arguments:

  @param[in]     This            Pointer to Form Callback protocol instance.
  @param[in]     KeyValue        A unique value which is sent to the original exporting driver so that it
                                 can identify the type of data to expect.  The format of the data tends to
                                 vary based on the op-code that geerated the callback.
  @param[in]     Data            A pointer to the data being sent to the original exporting driver.
  @param[in]     Packet          The result data returned.

  @retval        EFI_SUCCESS     Must return success upon completion.
  @retval        Others          Error occurred during execution.
**/
EFI_STATUS
ExitCallbackHook (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS                                Status, OriginalStatus;
  CHAR16                                    *StringPtr;
  CHIPSET_CONFIGURATION                     *MyIfrNVData;
  EFI_HII_HANDLE                            HiiHandle;
  EFI_CALLBACK_INFO                         *CallbackInfo;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL        *Interface;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL        *SetupUtilityBrowserProtocol;
  UINTN                                     BufferSize;

  OriginalStatus = OriginalExitCallbackRoutine (This, Action, QuestionId, Type, Value, ActionRequest);

  Status = gBS->LocateProtocol (
               &gEfiSetupUtilityBrowserProtocolGuid,
               NULL,
               &SetupUtilityBrowserProtocol
               );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  SuBrowser = EFI_SETUP_UTILITY_BROWSER_FROM_THIS (SetupUtilityBrowserProtocol);

  CallbackInfo            = EFI_EXIT_CALLBACK_INFO_FROM_THIS (This);
  if (QuestionId == GET_SETUP_CONFIG || QuestionId == SET_SETUP_CONFIG) {
    BufferSize = PcdGet32 (PcdSetupConfigSize);
    Status = SetupVariableConfig (
               NULL,
               NULL,
               BufferSize,
               (UINT8 *) gSUBrowser->SCBuffer,
               (BOOLEAN)(QuestionId == GET_SETUP_CONFIG)
               );
    return Status;
  }
  BufferSize = PcdGet32 (PcdSetupConfigSize);
  Status = SetupVariableConfig (
             NULL,
             NULL,
             BufferSize,
             (UINT8 *) gSUBrowser->SCBuffer,
             TRUE
             );
  MyIfrNVData = (CHIPSET_CONFIGURATION *) gSUBrowser->SCBuffer;
  SUCInfo     = SuBrowser->SUCInfo;
  Status      = EFI_SUCCESS;
  StringPtr   = NULL;
  HiiHandle   = CallbackInfo->HiiHandle;
  Interface   = &SuBrowser->Interface;

//   switch (QuestionId)
// 
//   }
 
  BufferSize = PcdGet32 (PcdSetupConfigSize);
  SetupVariableConfig (
    NULL,
    NULL,
    BufferSize,
    (UINT8 *) gSUBrowser->SCBuffer,
    FALSE
    );
  return OriginalStatus;
}
#endif

/**
  Notificaltion function for FormCallbackProtocol 
  
  @param[in]      Event        EFI_EVENT structure
  @param[in]      Context      Event context
   
  @retval  None.
**/
EFI_STATUS
FormCallbackProtocolNotify (
   IN EFI_EVENT        Event,
   IN VOID             *Context
  )
{ 
  EFI_STATUS                      Status;
  EFI_HII_CONFIG_ACCESS_PROTOCOL  *EfiFormCallbackProtocol;
  EFI_CALLBACK_INFO               *CallbackInfo;
  UINTN                           HandleCount;
  EFI_HANDLE                      *HandleBuffer;
  UINTN                           HandleIndex;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiHiiConfigAccessProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;;
  }
  for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
    Status = gBS->OpenProtocol (
                    HandleBuffer[HandleIndex],
                    &gEfiHiiConfigAccessProtocolGuid,
                    &EfiFormCallbackProtocol,
                    Context,
                    NULL,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }
#ifdef  EXIT_CALLBACK_HOOK_SUPPORT
    if (!HasHookExitCallback) {
      CallbackInfo = BASE_CR (EfiFormCallbackProtocol, EFI_CALLBACK_INFO, DriverCallback);
//    CallbackInfo = EFI_EXIT_CALLBACK_INFO_FROM_THIS (EfiFormCallbackProtocol);
      if (CallbackInfo->Signature == EFI_EXIT_MENU_SIGNATURE) {
        OriginalExitCallbackRoutine = EfiFormCallbackProtocol->Callback;
        EfiFormCallbackProtocol->Callback = ExitCallbackHook;
        HasHookExitCallback = TRUE;
      }
    }
#endif
#ifdef  SECURITY_CALLBACK_HOOK_SUPPORT
    if (!HasHookSecurityCallback) {
      CallbackInfo = BASE_CR (EfiFormCallbackProtocol, EFI_CALLBACK_INFO, DriverCallback);
//    CallbackInfo = EFI_SECURITY_CALLBACK_INFO_FROM_THIS (EfiFormCallbackProtocol);
      if (CallbackInfo->Signature == EFI_SECURITY_MENU_SIGNATURE) {
        OriginalSecurityCallbackRoutine = EfiFormCallbackProtocol->Callback;
        EfiFormCallbackProtocol->Callback = SecurityCallbackHook;
        HasHookSecurityCallback = TRUE;
      }
    }
#endif
  }
  return Status;
}

/**
  Register a protocol notification for FormCallbackProtocol then we can hook 
  kernel owned callback function for platform specific purpose.

  @param[in]      ImageHandle

  @retval  None.
**/
VOID
NotifyFormCallbackProtocol (
  IN  EFI_HANDLE                            ImageHandle
)
{
  EFI_STATUS        Status;
  EFI_EVENT         FormCallbackProtocolEvent;

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  FormCallbackProtocolNotify,
                  ImageHandle,
                  &FormCallbackProtocolEvent
                  );
  if (!EFI_ERROR (Status)) {
    Status = gBS->RegisterProtocolNotify (
                    &gEfiHiiConfigAccessProtocolGuid,
                    FormCallbackProtocolEvent,
                    &mFormCallbackProtocolEventlRegistration
                    );  
  }

  return;
}

#pragma optimize( "", on )

