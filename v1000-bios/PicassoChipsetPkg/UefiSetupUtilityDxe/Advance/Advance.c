/** @file

;******************************************************************************
;* Copyright (c) 2015 - 2018, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "Advance.h"
//#include <Library/CrVfrConfigLib.h>

IDE_CONFIG                               *mIdeConfig;
EFI_CALLBACK_INFO                         *mAdvCallBackInfo;
ADVANCE_CONFIGURATION                     mAdvConfig;

VOID
SetHddPswdModifiedFlag ();

/**
 This is the callback function for the Advance Menu.

 @param [in]   This
 @param [in]   Action
 @param [in]   QuestionId
 @param [in]   Type
 @param [in]   Value
 @param [out]  ActionRequest


**/
EFI_STATUS
AdvanceCallbackRoutine (
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
  EFI_INPUT_KEY                         Key;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  UINT8                                 DeviceKind;
  UINTN                                 BufferSize;
 /// UINT16                                KeyHtfFeatrueAndValue;
  EFI_GUID                              VarStoreGuid = SYSTEM_CONFIGURATION_GUID;

  if (Action != EFI_BROWSER_ACTION_CHANGED) {
    return AdvanceCallbackRoutineByAction (This, Action, QuestionId, Type, Value, ActionRequest);
  }
  
  *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  CallbackInfo = EFI_CALLBACK_INFO_FROM_THIS (This);
  BufferSize = GetVarStoreSize (CallbackInfo->HiiHandle, &CallbackInfo->FormsetGuid, &VarStoreGuid, "SystemConfig");

  BufferSize = PcdGet32 (PcdSetupConfigSize);
  Status = SetupVariableConfig (
             &VarStoreGuid,
             L"SystemConfig",
             BufferSize,
             (UINT8 *) gSUBrowser->SCBuffer,
             TRUE
             );
  Status = SetupVariableConfig (
             &VarStoreGuid,
             L"AdvanceConfig",
             sizeof (mAdvConfig),
             (UINT8 *) &mAdvConfig,
             TRUE
             );
  MyIfrNVData = (CHIPSET_CONFIGURATION *) gSUBrowser->SCBuffer;
  Status    = EFI_SUCCESS;
  StringPtr = NULL;
  SUCInfo   = gSUBrowser->SUCInfo;

  HiiHandle = CallbackInfo->HiiHandle;


  switch (QuestionId) {

  case KEY_SERIAL_PORT_EN:
    //
    //  Check IRQ conflict between serial port and pci solt.
    //
    if (CheckSioAndPciSoltConflict (MyIfrNVData, (UINT8 *)&(MyIfrNVData->PciSlot4), &DeviceKind)) {
      StringPtr = HiiGetString (HiiHandle, STRING_TOKEN(ISA_AND_PCISOLT_CONFLICT_STRING), NULL);
      gSUBrowser->H2ODialog->ConfirmDialog (2, FALSE, 0, NULL, &Key, StringPtr);
    	gBS->FreePool (StringPtr);
    	switch (DeviceKind) {
    	  case 0:
    	    MyIfrNVData->ComPortA = 0;
    	    break;
    	  case 1:
    	    MyIfrNVData->ComPortB = 0;
    	    break;
      }
    }

    if (CheckSioConflict (MyIfrNVData)) {
      StringPtr = HiiGetString (HiiHandle, STRING_TOKEN(CONFLICT_STRING), NULL);
      gSUBrowser->H2ODialog->ConfirmDialog (
                                 2,
                                 FALSE,
                                 0,
                                 NULL,
                                 &Key,
                                 StringPtr
                                 );
      gBS->FreePool (StringPtr);
    }
    break;

  case KEY_CHIPSET_EXTENDED_CONFIG:
    if (MyIfrNVData->ExtendedConfig == 0) {
      MyIfrNVData->SdRamFrequency    = 0;
      MyIfrNVData->SdRamTimeCtrl     = 0;
      MyIfrNVData->SdRamRasActToPre  = 6;
      MyIfrNVData->SdRamCasLatency   = 25;
      MyIfrNVData->SdRamRasCasDelay  = 3;
      MyIfrNVData->SdRamRasPrecharge = 3;
    }
    break;

  case KEY_CHIPSET_SDRAM_TIME_CTRL:
    switch (MyIfrNVData->SdRamTimeCtrl) {

    case 0:         // AUTO
      MyIfrNVData->SdRamRasActToPre = 6;
      MyIfrNVData->SdRamCasLatency = 25;
      MyIfrNVData->SdRamRasCasDelay = 3;
      MyIfrNVData->SdRamRasPrecharge = 3;
      break;

    case 1:         // MANUAL_AGRESSIVE
      MyIfrNVData->SdRamRasActToPre  = 5;
      MyIfrNVData->SdRamCasLatency   = 20;
      MyIfrNVData->SdRamRasCasDelay  = 2;
      MyIfrNVData->SdRamRasPrecharge = 2;
      break;
    }
    SUCInfo->DoRefresh = TRUE;
    break;

  case KEY_PARALLEL_PORT_EN:
    if (MyIfrNVData->Lpt1 > 1) {
      //
      // when LPT enable,check IRQ conflict
      //
      switch (CheckLptAndPciSoltConflict(MyIfrNVData, (UINT8 *)&(MyIfrNVData->PciSlot4))) {
      //
      // case 3: IRQ5 and IRQ7 have conflict.
      // case 2: IRQ7 have conflict.
      // case 1: IRQ5 have conflict.
      // case 0: NO conflict
      case 3:
        MyIfrNVData->Lpt1 = 0;
        StringPtr = HiiGetString (HiiHandle, STRING_TOKEN(LPT_BOTH_IRQ_CONFLICT_STRING), NULL);
        gSUBrowser->H2ODialog->ConfirmDialog (2, FALSE, 0, NULL, &Key, StringPtr);
        gBS->FreePool (StringPtr);
        break;
		
      case 1:
        StringPtr = HiiGetString (HiiHandle, STRING_TOKEN(LPT_IRQ5_CONFLICT_STRING), NULL);
        gSUBrowser->H2ODialog->ConfirmDialog (2, FALSE, 0, NULL, &Key, StringPtr);
        gBS->FreePool (StringPtr);
        break;
		
      case 2:
        StringPtr = HiiGetString (HiiHandle, STRING_TOKEN(LPT_IRQ7_CONFLICT_STRING), NULL);
        gSUBrowser->H2ODialog->ConfirmDialog (2, FALSE, 0, NULL, &Key, StringPtr);
        gBS->FreePool (StringPtr);
        break;
		
      default:
        break;
      }
    } else if (MyIfrNVData->Lpt1 == 1 ) {
      //
      // when LPT auto,check IRQ Conflict.
      //
      switch (CheckLptAndPciSoltConflict(MyIfrNVData, (UINT8 *)&(MyIfrNVData->PciSlot4))) {
        
		case 3:
          MyIfrNVData->Lpt1 = 0;
          StringPtr = HiiGetString (HiiHandle, STRING_TOKEN(LPT_BOTH_IRQ_CONFLICT_STRING), NULL);
          gSUBrowser->H2ODialog->ConfirmDialog (2, FALSE, 0, NULL, &Key, StringPtr);
          gBS->FreePool (StringPtr);
          break;
		  
        default:
          break;
      }
    }
    break;
	
  case KEY_PCI_SLOT_4_IRQ_SET:
  case KEY_PCI_SLOT_5_IRQ_SET:
  case KEY_PCI_SLOT_6_IRQ_SET:
    if (CheckPciSioConflict (MyIfrNVData, (UINT8 *)&(MyIfrNVData->PciSlot4), &DeviceKind)) {
   	  switch (DeviceKind) {

      case 1:
        StringPtr = HiiGetString (HiiHandle, STRING_TOKEN(COMPORTA_CONFLICT_STRING), NULL);
        break;
		
      case 2:
        StringPtr = HiiGetString (HiiHandle, STRING_TOKEN(COMPORTB_CONFLICT_STRING), NULL);
        break;
		
      case 3:
        StringPtr = HiiGetString (HiiHandle, STRING_TOKEN(PARALLEL_PORT_CONFLICT_STRING), NULL);
        break;
		
      }
      gSUBrowser->H2ODialog->ConfirmDialog (2, FALSE, 0, NULL, &Key, StringPtr);
      gBS->FreePool (StringPtr);
    }
    break;

  case KEY_SATA_CNFIGURE_MODE :
    UpdateHDCConfigure (
      HiiHandle,
      MyIfrNVData
      );
    break;
 /// case KEY_LASSO_SUPPORT:
 ///   if (MyIfrNVData->amdLassoSupport)
 ///   {
 ///  		MyIfrNVData->XgpSupport = 1;
 ///  		MyIfrNVData->PrimaryVideoAdaptor = 1;
 ///   }
 ///   else
 ///   {
 ///       MyIfrNVData->XgpSupport = 0;
///	    MyIfrNVData->PrimaryVideoAdaptor = 0;
///   }
///    break;

///  case KEY_SPECIAL_VGA_FEATURE:
///    SUCInfo->DoRefresh = TRUE;
///  break;

  case KEY_UMA_MODE:
    SUCInfo->DoRefresh = TRUE;
  break;

///  case KEY_XGP_FEATURE:
///    if (MyIfrNVData->SpecialVgaFeature == 0) {
///          MyIfrNVData->PrimaryVideoAdaptor = 0;   //AUTO
///    }
///    SUCInfo->DoRefresh = TRUE;
///  break;
// HT Frequence change feedback
///  case KEY_HTF_FEATURE:
///    KeyHtfFeatrueAndValue = ((UINT16) BIT9);
///    PciAnd16 (PCI_LIB_ADDRESS (0, 0x18, 0, 0x6c), ~KeyHtfFeatrueAndValue);
///    SUCInfo->DoRefresh = TRUE;
///  break;

//  case CONSOLE_REDIRECTION_FORM_ID_0 :
//  case CONSOLE_REDIRECTION_FORM_ID_1 :
//  case CONSOLE_REDIRECTION_FORM_ID_2 :
//  case CONSOLE_REDIRECTION_FORM_ID_3 :
//  case CONSOLE_REDIRECTION_FORM_ID_4 :
//  case CONSOLE_REDIRECTION_FORM_ID_5 :
//  case CONSOLE_REDIRECTION_FORM_ID_6 :
//  case CONSOLE_REDIRECTION_FORM_ID_7 :
//  case KEY_CONSOLE_REDIRECTION_1 : 
//  case KEY_CONSOLE_REDIRECTION_2 :
//  case KEY_CONSOLE_REDIRECTION_3 :    
//  case KEY_CONSOLE_REDIRECTION_4 :
//  case KEY_CONSOLE_REDIRECTION_5 :
//  case KEY_CONSOLE_REDIRECTION_6 :
//  case KEY_CONSOLE_REDIRECTION_7 :
//  case KEY_CONSOLE_REDIRECTION_8 :
//  case KEY_CONSOLE_REDIRECTION_9 :
//  case KEY_CONSOLE_REDIRECTION_10 :  
//  case KEY_CONSOLE_REDIRECTION_11 :
//  case KEY_CONSOLE_REDIRECTION_12 : 
//  case KEY_CONSOLE_REDIRECTION_13 : 
//  case KEY_CONSOLE_REDIRECTION_14 :
//  case KEY_CONSOLE_REDIRECTION_15 :
//  case KEY_CONSOLE_REDIRECTION_16 :
//
//    CRScuAdvanceCallback(QuestionId);
//    
//    break;

  case KEY_PLUG_IN_DISPLAY_SELECTION1:
  case KEY_PLUG_IN_DISPLAY_SELECTION2:
    PlugInVideoDisplaySelectionOption (QuestionId, Value);
    break;

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

  BufferSize = PcdGet32 (PcdSetupConfigSize);
  SetupVariableConfig (
    &VarStoreGuid,
    L"SystemConfig",
    BufferSize,
    (UINT8 *) gSUBrowser->SCBuffer,
    FALSE
    );
  SetupVariableConfig (
    &VarStoreGuid,
    L"AdvanceConfig",
    sizeof (mAdvConfig),
    (UINT8 *) &mAdvConfig,
    FALSE
    );

  return Status;
}

EFI_STATUS
AdvanceCallbackRoutineByAction (
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
      Status = SetupVariableConfig (
                 &VarStoreGuid,
                 L"AdvanceConfig",
                 sizeof (mAdvConfig),
                 (UINT8 *) &mAdvConfig,
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
      Status = SetupVariableConfig (
                 &VarStoreGuid,
                 L"AdvanceConfig",
                 sizeof (mAdvConfig),
                 (UINT8 *) &mAdvConfig,
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
      Status = SetupVariableConfig (
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
InstallAdvanceCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle,
  IN EFI_HII_HANDLE                         HiiHandle
  )
{
  EFI_STATUS                                Status;
  EFI_GUID                                  FormsetGuid = FORMSET_ID_GUID_ADVANCE;

  mAdvCallBackInfo = AllocatePool (sizeof(EFI_CALLBACK_INFO));
  if (mAdvCallBackInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  mAdvCallBackInfo->Signature                    = EFI_CALLBACK_INFO_SIGNATURE;
  mAdvCallBackInfo->DriverCallback.ExtractConfig = gSUBrowser->ExtractConfig;
  mAdvCallBackInfo->DriverCallback.RouteConfig   = gSUBrowser->RouteConfig;
  mAdvCallBackInfo->DriverCallback.Callback      = AdvanceCallbackRoutine;
  mAdvCallBackInfo->HiiHandle                    = HiiHandle;
  CopyGuid (&mAdvCallBackInfo->FormsetGuid, &FormsetGuid);

  //
  // Install protocol interface
  //
  Status = gBS->InstallProtocolInterface (
                  &DriverHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mAdvCallBackInfo->DriverCallback
                  );
  ASSERT_EFI_ERROR (Status);

  Status = InitAdvanceMenu (HiiHandle);

  return Status;
}

EFI_STATUS
UninstallAdvanceCallbackRoutine (
  IN EFI_HANDLE                             DriverHandle
  )
{
  EFI_STATUS     Status;

  if (mAdvCallBackInfo == NULL) {
    return EFI_SUCCESS;
  }
  Status = gBS->UninstallProtocolInterface (
                  DriverHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &mAdvCallBackInfo->DriverCallback
                  );
  ASSERT_EFI_ERROR (Status);
  gBS->FreePool (mAdvCallBackInfo);
  mAdvCallBackInfo = NULL;
  return Status;
}

EFI_STATUS
InitAdvanceMenu (
  IN EFI_HII_HANDLE                         HiiHandle
  )
{
  EFI_STATUS                                Status;

  //
  // Check and update IDE configuration.
  //
  gSUBrowser->SUCInfo->PrevSataCnfigure = ((CHIPSET_CONFIGURATION *)gSUBrowser->SCBuffer)->SataCnfigure;

  mIdeConfig = AllocateZeroPool (sizeof(IDE_CONFIG)*8);
  InitIdeConfig (mIdeConfig);
  gSUBrowser->IdeConfig = mIdeConfig;
  Status = CheckIde (
             HiiHandle,
             (CHIPSET_CONFIGURATION *)gSUBrowser->SCBuffer,
             FALSE
             );
  ASSERT_EFI_ERROR (Status);

  //
  // Audio detect Azalia or AC97
  //
//   Status = SelectAudioOption (HiiHandle);

//  CrSerialDevOptionInit (HiiHandle);

  //
  // Auto detect available output device.
  //
  PlugInVideoDisplaySelectionLabel (HiiHandle);

  return Status;
}
