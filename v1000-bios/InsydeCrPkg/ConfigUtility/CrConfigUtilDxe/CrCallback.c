/** @file

  Callback Functions for Console Redirection Configuration

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

#include "CrConfigUtil.h"
#include "CrDeviceMenu.h"
#include "CrConfig.h"

extern CR_CONFIG_MANAGER_PRIVATE_DATA         *mPrivateData;
extern CHAR16                                 *mVariableName;

CR_DEV_NODE                                   *mCurrentDevNode = NULL;

STATIC
EFI_STATUS
ForceSubmit (
  VOID
  )
{
  EFI_STATUS                                  Status;
  EFI_FORM_BROWSER2_PROTOCOL                  *FormBrowser2;
  CHAR16                                      *ConfigRequest;
  UINTN                                       ConfigRequestSize;
  EFI_GUID                                    *NameValueGuid;
  CHAR16                                      *NameValueName;
  STATIC UINT8                                NameValueValue = 0;

  //
  // Change name value data to make form brower will call RouteConfig () when SubmitForm ()
  //
  Status = gBS->LocateProtocol (&gEfiFormBrowser2ProtocolGuid, NULL, (VOID **) &FormBrowser2);
  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return Status;
  }

  NameValueGuid = &gH2OCrConfigurationGuid;
  NameValueName = HiiGetString (mPrivateData->HiiHandle, STRING_TOKEN(STR_NAME_VALUE_VAR_NAME0), NULL);
  if (NameValueValue < 0xFF) {
    NameValueValue ++;
  } else {
    NameValueValue = 0;
  }

  ConfigRequestSize = 0x100;
  ConfigRequest     = AllocateZeroPool (ConfigRequestSize);
  UnicodeSPrint (ConfigRequest, ConfigRequestSize, L"%s=%02x", NameValueName, NameValueValue);
  Status = FormBrowser2->BrowserCallback (
                           FormBrowser2,
                           &ConfigRequestSize,
                           ConfigRequest,
                           FALSE,
                           NameValueGuid,
                           NameValueName
                           );
  FreePool (NameValueName);
  FreePool (ConfigRequest);
  if (EFI_ERROR (Status)) {
//    ASSERT (FALSE);
    DEBUG((EFI_D_WARN, "[%a %d] Status = %r \n", __FUNCTION__, __LINE__, Status));
    DEBUG((EFI_D_WARN, "ConfigRequest = %s \n", ConfigRequest));
    DEBUG((EFI_D_WARN, "\n"));
    return Status;
  }

  return EFI_SUCCESS;
}

STATIC
BOOLEAN
IsGlobalSetting (
  IN EFI_QUESTION_ID                          QuestionId
  )
{
  switch (QuestionId) {

  case KEY_GLOBAL_TERMINAL_TYPE:
  case KEY_GLOBAL_BAUD_RATE:
  case KEY_GLOBAL_DATA_BITS:
  case KEY_GLOBAL_PARITY:
  case KEY_GLOBAL_STOP_BITS:
  case KEY_GLOBAL_FLOW_CONTROL:
    return TRUE;

  default:
    break;
  }

  return FALSE;
}

STATIC
BOOLEAN
IsLocalSetting (
  IN EFI_QUESTION_ID                          QuestionId
  )
{
  switch (QuestionId) {

//[-start-190613-IB07401110-remove]//
//  case KEY_PORT_ENABLE:
//  case KEY_USE_GLOBAL_SETTING:
//  case KEY_TERMINAL_TYPE:
//  case KEY_BAUD_RATE:
//  case KEY_DATA_BITS:
//  case KEY_PARITY:
//  case KEY_STOP_BITS:
//  case KEY_FLOW_CONTROL:
//[-end-190613-IB07401110-remove]//
//[-start-190612-IB07401110-add]//
  case KEY_PORT_ENABLE0:
  case KEY_PORT_ENABLE1:
  case KEY_PORT_ENABLE2:
  case KEY_PORT_ENABLE3:
  case KEY_PORT_ENABLE4:
  case KEY_PORT_ENABLE5:
  case KEY_PORT_ENABLE6:
  case KEY_PORT_ENABLE7:
  case KEY_PORT_ENABLE8:
  case KEY_PORT_ENABLE9:
  case KEY_PORT_ENABLE10:
  case KEY_USE_GLOBAL_SETTING0:
  case KEY_USE_GLOBAL_SETTING1:
  case KEY_USE_GLOBAL_SETTING2:
  case KEY_USE_GLOBAL_SETTING3:
  case KEY_USE_GLOBAL_SETTING4:
  case KEY_USE_GLOBAL_SETTING5:
  case KEY_USE_GLOBAL_SETTING6:
  case KEY_USE_GLOBAL_SETTING7:
  case KEY_USE_GLOBAL_SETTING8:
  case KEY_USE_GLOBAL_SETTING9:
  case KEY_USE_GLOBAL_SETTING10:
  case KEY_TERMINAL_TYPE0:
  case KEY_TERMINAL_TYPE1:
  case KEY_TERMINAL_TYPE2:
  case KEY_TERMINAL_TYPE3:
  case KEY_TERMINAL_TYPE4:
  case KEY_TERMINAL_TYPE5:
  case KEY_TERMINAL_TYPE6:
  case KEY_TERMINAL_TYPE7:
  case KEY_TERMINAL_TYPE8:
  case KEY_TERMINAL_TYPE9:
  case KEY_TERMINAL_TYPE10:
  case KEY_BAUD_RATE0:
  case KEY_BAUD_RATE1:
  case KEY_BAUD_RATE2:
  case KEY_BAUD_RATE3:
  case KEY_BAUD_RATE4:
  case KEY_BAUD_RATE5:
  case KEY_BAUD_RATE6:
  case KEY_BAUD_RATE7:
  case KEY_BAUD_RATE8:
  case KEY_BAUD_RATE9:
  case KEY_BAUD_RATE10:
  case KEY_DATA_BITS0:
  case KEY_DATA_BITS1:
  case KEY_DATA_BITS2:
  case KEY_DATA_BITS3:
  case KEY_DATA_BITS4:
  case KEY_DATA_BITS5:
  case KEY_DATA_BITS6:
  case KEY_DATA_BITS7:
  case KEY_DATA_BITS8:
  case KEY_DATA_BITS9:
  case KEY_DATA_BITS10:
  case KEY_PARITY0:
  case KEY_PARITY1:
  case KEY_PARITY2:
  case KEY_PARITY3:
  case KEY_PARITY4:
  case KEY_PARITY5:
  case KEY_PARITY6:
  case KEY_PARITY7:
  case KEY_PARITY8:
  case KEY_PARITY9:
  case KEY_PARITY10:
  case KEY_STOP_BITS0:
  case KEY_STOP_BITS1:
  case KEY_STOP_BITS2:
  case KEY_STOP_BITS3:
  case KEY_STOP_BITS4:
  case KEY_STOP_BITS5:
  case KEY_STOP_BITS6:
  case KEY_STOP_BITS7:
  case KEY_STOP_BITS8:
  case KEY_STOP_BITS9:
  case KEY_STOP_BITS10:
  case KEY_FLOW_CONTROL0:
  case KEY_FLOW_CONTROL1:
  case KEY_FLOW_CONTROL2:
  case KEY_FLOW_CONTROL3:
  case KEY_FLOW_CONTROL4:
  case KEY_FLOW_CONTROL5:
  case KEY_FLOW_CONTROL6:
  case KEY_FLOW_CONTROL7:
  case KEY_FLOW_CONTROL8:
  case KEY_FLOW_CONTROL9:
  case KEY_FLOW_CONTROL10:
//[-end-190612-IB07401110-add]//
    return TRUE;

  default:
    break;
  }

  return FALSE;
}

STATIC
EFI_STATUS
UpdateStatusStr (
  IN CR_DEV_NODE                              *DevNode
  )
{
  CHAR16                                      *Str;

  if (DevNode == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Str = GetStatusStr (&DevNode->Dev);
  HiiSetString (mPrivateData->HiiHandle, DevNode->StatusStrId, Str, NULL);
  FreePool (Str);

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
UpdateGlobalToLocal (
  IN CR_CONFIGURATION                         *CrConfig
  )
{
  if (CrConfig == NULL) {
    return EFI_INVALID_PARAMETER;
  }

//[-start-190612-IB07401110-modify]//
  CrConfig->CrPortConfig[mCurrentDevNode->Index].TerminalType = CrConfig->GlobalTerminalType;
  CrConfig->CrPortConfig[mCurrentDevNode->Index].BaudRate     = CrConfig->GlobalBaudRate;
  CrConfig->CrPortConfig[mCurrentDevNode->Index].Parity       = CrConfig->GlobalParity;
  CrConfig->CrPortConfig[mCurrentDevNode->Index].DataBits     = CrConfig->GlobalDataBits;
  CrConfig->CrPortConfig[mCurrentDevNode->Index].StopBits     = CrConfig->GlobalStopBits;
  CrConfig->CrPortConfig[mCurrentDevNode->Index].FlowControl  = CrConfig->GlobalFlowControl;
//[-end-190612-IB07401110-modify]//

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
UpdateDeviceToCrConfig (
  IN CR_DEVICE_SETTING                        *Device,
  IN CR_CONFIGURATION                         *CrConfig
  )
{
  if (Device == NULL || CrConfig == NULL) {
    return EFI_INVALID_PARAMETER;
  }

//[-start-190612-IB07401110-modify]//
  CrConfig->CrPortConfig[mCurrentDevNode->Index].PortEnable       = Device->Attribute.PortEnable;
  CrConfig->CrPortConfig[mCurrentDevNode->Index].UseGlobalSetting = Device->Attribute.UseGlobalSetting;

  CrConfig->CrPortConfig[mCurrentDevNode->Index].TerminalType     = Device->Attribute.TerminalType;
  CrConfig->CrPortConfig[mCurrentDevNode->Index].BaudRate         = Device->Attribute.BaudRate;
  CrConfig->CrPortConfig[mCurrentDevNode->Index].Parity           = Device->Attribute.Parity;
  CrConfig->CrPortConfig[mCurrentDevNode->Index].DataBits         = Device->Attribute.DataBits;
  CrConfig->CrPortConfig[mCurrentDevNode->Index].StopBits         = Device->Attribute.StopBits;
  CrConfig->CrPortConfig[mCurrentDevNode->Index].FlowControl      = Device->Attribute.FlowControl;
//[-end-190612-IB07401110-modify]//
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
UpdateDeviceFromCrConfig (
  IN CR_DEVICE_SETTING                        *Device,
  IN CR_CONFIGURATION                         *CrConfig
  )
{
  if (Device == NULL || CrConfig == NULL) {
    return EFI_INVALID_PARAMETER;
  }

//[-start-190612-IB07401110-modify]//
  Device->Attribute.PortEnable       = CrConfig->CrPortConfig[mCurrentDevNode->Index].PortEnable;
  Device->Attribute.UseGlobalSetting = CrConfig->CrPortConfig[mCurrentDevNode->Index].UseGlobalSetting;

  Device->Attribute.TerminalType     = CrConfig->CrPortConfig[mCurrentDevNode->Index].TerminalType;
  Device->Attribute.BaudRate         = CrConfig->CrPortConfig[mCurrentDevNode->Index].BaudRate;
  Device->Attribute.Parity           = CrConfig->CrPortConfig[mCurrentDevNode->Index].Parity;
  Device->Attribute.DataBits         = CrConfig->CrPortConfig[mCurrentDevNode->Index].DataBits;
  Device->Attribute.StopBits         = CrConfig->CrPortConfig[mCurrentDevNode->Index].StopBits;
  Device->Attribute.FlowControl      = CrConfig->CrPortConfig[mCurrentDevNode->Index].FlowControl;
//[-end-190612-IB07401110-modify]//

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
UpdateAllDevFromConfig (
  IN CR_CONFIGURATION                         *CrConfig
  )
{
  LIST_ENTRY                                  *Link;
  CR_DEV_NODE                                 *CrDevNode;

  if (IsListEmpty (&mPrivateData->CrDevListHead)) {
    return EFI_NOT_FOUND;
  }

  Link = GetFirstNode (&mPrivateData->CrDevListHead);
  while (!IsNull (&mPrivateData->CrDevListHead, Link)) {

    CrDevNode = CR_DEV_NODE_FROM_LINK (Link);
    Link = GetNextNode (&mPrivateData->CrDevListHead, Link);

    if (CrDevNode->Dev.Attribute.UseGlobalSetting) {
      CrDevNode->Dev.Attribute.TerminalType = CrConfig->GlobalTerminalType;
      CrDevNode->Dev.Attribute.BaudRate     = CrConfig->GlobalBaudRate;
      CrDevNode->Dev.Attribute.Parity       = CrConfig->GlobalParity;
      CrDevNode->Dev.Attribute.DataBits     = CrConfig->GlobalDataBits;
      CrDevNode->Dev.Attribute.StopBits     = CrConfig->GlobalStopBits;
      CrDevNode->Dev.Attribute.FlowControl  = CrConfig->GlobalFlowControl;
    }
    UpdateStatusStr (CrDevNode);
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
UpdateDevPage (
  VOID
  )
{
  EFI_STATUS                                  Status;
  CR_DEVICE_SETTING                           *Device;
  CR_CONFIGURATION                            CrConfig;
  BOOLEAN                                     IsSuccess;

  if (mCurrentDevNode == NULL) {
    return EFI_UNSUPPORTED;
  }

  Device = &mCurrentDevNode->Dev;

  Status = HiiSetString (mPrivateData->HiiHandle, STRING_TOKEN (STR_STR_CR_DEVICE_TITLE), Device->DevName, NULL);
  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return Status;
  }

  IsSuccess = HiiGetBrowserData (&gH2OCrConfigurationGuid, mVariableName, sizeof (CR_CONFIGURATION), (UINT8 *)&CrConfig);
  if (!IsSuccess) {
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }

  UpdateDeviceToCrConfig (Device, &CrConfig);

  IsSuccess = HiiSetBrowserData (&gH2OCrConfigurationGuid, mVariableName, sizeof (CR_CONFIGURATION), (UINT8 *)&CrConfig, NULL);
  if (!IsSuccess) {
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }

  return Status;
}

STATIC
EFI_STATUS
GotoDevCallback (
  IN EFI_QUESTION_ID                          QuestionId
  )
{
  EFI_STATUS                                  Status;
  CR_DEV_NODE                                 *DevNode;

  //
  // Current question is the "created goto question"
  //
  Status = GetDevNodeByQuestionId (QuestionId, &DevNode);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  mCurrentDevNode = DevNode;

  UpdateDevPage ();

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
GlobalSettingCallback (
  VOID
  )
{
  CR_CONFIGURATION                            CrConfig;
  BOOLEAN                                     IsSuccess;

  IsSuccess = HiiGetBrowserData (&gH2OCrConfigurationGuid, mVariableName, sizeof (CR_CONFIGURATION), (UINT8 *)&CrConfig);
  if (!IsSuccess) {
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }

  UpdateAllDevFromConfig (&CrConfig);

  IsSuccess = HiiSetBrowserData (&gH2OCrConfigurationGuid, mVariableName, sizeof (CR_CONFIGURATION), (UINT8 *)&CrConfig, NULL);
  if (!IsSuccess) {
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
LocalSettingCallback (
  VOID
  )
{
  CR_DEVICE_SETTING                           *Device;
  CR_CONFIGURATION                            CrConfig;
  BOOLEAN                                     IsSuccess;

  if (mCurrentDevNode == NULL) {
    return EFI_UNSUPPORTED;
  }

  IsSuccess = HiiGetBrowserData (&gH2OCrConfigurationGuid, mVariableName, sizeof (CR_CONFIGURATION), (UINT8 *)&CrConfig);
  if (!IsSuccess) {
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }

  Device = &mCurrentDevNode->Dev;

//[-start-190612-IB07401110-modify]//
  if (CrConfig.CrPortConfig[mCurrentDevNode->Index].UseGlobalSetting) {
//[-end-190612-IB07401110-modify]//
    UpdateGlobalToLocal (&CrConfig);
  }
  UpdateDeviceFromCrConfig (Device, &CrConfig);

  UpdateStatusStr (mCurrentDevNode);

  IsSuccess = HiiSetBrowserData (&gH2OCrConfigurationGuid, mVariableName, sizeof (CR_CONFIGURATION), (UINT8 *)&CrConfig, NULL);
  if (!IsSuccess) {
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
AfterLoadDefault (
  VOID
  )
{
  //
  // Update CR devices
  //
  UpdateDevices (TRUE);

  //
  // Check use global setting
  //
  GlobalSettingCallback ();

  //
  // Update device page
  //
  UpdateDevPage ();

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
CrSubmit (
  VOID
  )
{
  return SetDevVariables ();
}

EFI_STATUS
EFIAPI
CrDriverCallback (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL     *This,
  IN EFI_BROWSER_ACTION                       Action,
  IN EFI_QUESTION_ID                          QuestionId,
  IN UINT8                                    Type,
  IN EFI_IFR_TYPE_VALUE                       *Value,
  OUT EFI_BROWSER_ACTION_REQUEST              *ActionRequest
  )
{
  EFI_STATUS                                  Status;
//[-start-180823-IB10920058-add]//
  CR_CONFIGURATION                            Configuration = {0};
  UINTN                                       ConfigSize;
//[-end-180823-IB10920058-add]//


  //
  // Ensure call RoutConfig () when submit even if no changing anything
  //
  if (IsListEmpty (&mPrivateData->CrDevListHead)) {
    return EFI_UNSUPPORTED;
  } else {
    ForceSubmit ();
  }

  if (((Value == NULL) && (Action != EFI_BROWSER_ACTION_FORM_OPEN) && (Action != EFI_BROWSER_ACTION_FORM_CLOSE))||
    (ActionRequest == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_UNSUPPORTED;

  switch (Action) {

  case EFI_BROWSER_ACTION_CHANGING:
    DEBUG ((EFI_D_INFO, "CRCallBack->CHANGING               Qid:0x%x  Typ:0x%x\n", QuestionId, Type));
    Status = GotoDevCallback (QuestionId);
    break;

  case EFI_BROWSER_ACTION_CHANGED:
    DEBUG ((EFI_D_INFO, "CRCallBack->CHANGED                Qid:0x%x  Typ:0x%x\n", QuestionId, Type));
    if (IsGlobalSetting (QuestionId)) {
      Status = GlobalSettingCallback ();
    } else if (IsLocalSetting (QuestionId)) {
      Status = LocalSettingCallback ();
    }
    break;

  case EFI_BROWSER_ACTION_DEFAULT_STANDARD:
    DEBUG ((EFI_D_INFO, "CRCallBack->DEFAULT_STANDARD       Qid:0x%x  Typ:0x%x\n", QuestionId, Type));
//[-start-180823-IB10920058-add]//
    if (QuestionId == KEY_SCAN_F9) {
      //
      // Last question in this formset
      //
      ConfigSize = sizeof (CR_CONFIGURATION);
      Status = LoadVarDefault (
                 mVariableName, 
                 &gH2OCrConfigurationGuid, 
                 &ConfigSize, 
                 &Configuration
                 );
    }
//[-end-180823-IB10920058-add]//      
    if (QuestionId == KEY_SCAN_F9) {
        Status = AfterLoadDefault ();
    } else {
      Status = EFI_UNSUPPORTED;
    }
    break;

  case EFI_BROWSER_ACTION_RETRIEVE:
    DEBUG ((EFI_D_INFO, "CRCallBack->RETRIEVE               Qid:0x%x  Typ:0x%x\n", QuestionId, Type));
    if (QuestionId == KEY_SCAN_F9) {
      Status = EFI_SUCCESS;
    } else {
      Status = EFI_UNSUPPORTED;
    }
    break;

  default:
    DEBUG ((EFI_D_INFO, "CRCallBack->default:Action=0x%x    Qid:0x%x  Typ:0x%x\n", Action , QuestionId, Type));
    Status = EFI_UNSUPPORTED;
    break;
  }

  return Status;
}

