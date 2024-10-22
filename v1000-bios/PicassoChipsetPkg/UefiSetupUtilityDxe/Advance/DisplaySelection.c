/** @file

;*******************************************************************************
;* Copyright (c) 2015-2018, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#include <DisplaySelection.h>

UINT8                                  *mPtrToVbtTable = NULL;
PLUG_IN_ACTIVE_VIDEO_CONTROLLER_INFO   *mPlugInVgaInfo = NULL;

EFI_DEVICE_PATH_PROTOCOL *
PartMatchInstance (
  IN     EFI_DEVICE_PATH_PROTOCOL  *Multi,
  IN     EFI_DEVICE_PATH_PROTOCOL  *Single
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *Instance;
  UINTN                     InstanceSize;
  UINTN                     SingleDpSize;
  UINTN                     Size;
  EFI_DEVICE_PATH_PROTOCOL  *NewDevicePath;

  NewDevicePath = NULL;

  if (Multi == NULL || Single == NULL) {
    return FALSE;
  }

  Instance        =  GetNextDevicePathInstance (&Multi, &InstanceSize);
  SingleDpSize    =  GetDevicePathSize (Single) - END_DEVICE_PATH_LENGTH;
  InstanceSize    -= END_DEVICE_PATH_LENGTH;

  while (Instance != NULL) {

    Size = (SingleDpSize < InstanceSize) ? SingleDpSize : InstanceSize;

    if ((CompareMem (Instance, Single, Size) == 0)) {
      NewDevicePath = AppendDevicePathInstance (NewDevicePath, Instance);
    }
    gBS->FreePool (Instance);
    Instance = GetNextDevicePathInstance (&Multi, &InstanceSize);
    InstanceSize  -= END_DEVICE_PATH_LENGTH;
  }

  return NewDevicePath;
}

ACPI_ADR_DEVICE_PATH *
GetAcpiADRFromDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  ACPI_ADR_DEVICE_PATH         *AcpiADR;

  while (!IsDevicePathEnd (DevicePath)) {
    if ((DevicePathType (DevicePath) == ACPI_DEVICE_PATH) &&
        (DevicePathSubType (DevicePath) == ACPI_ADR_DP)) {
      AcpiADR = (ACPI_ADR_DEVICE_PATH*)DuplicateDevicePath (DevicePath);
      return AcpiADR;
    }
    DevicePath = NextDevicePathNode (DevicePath);
  }

  return NULL;
}

EFI_STATUS
UpdateDisplayPortInfo (
  IN PLUG_IN_ACTIVE_VIDEO_CONTROLLER_INFO    *VgaControllerInfo
  )
{
  UINTN                            DriverBindingHandleCount;
  EFI_HANDLE                       *DriverBindingHandleBuffer;
  EFI_STATUS                       Status;
  UINTN                            DrvierBindingIndex;
  UINTN                            ChildHandleIndex;
  CHAR16                           *TempControllerName;
  CHAR16                           *TempVideoOutputName;
  CHAR16                           *ControllerName;
  CHAR16                           *VideoOutputName;
  EFI_COMPONENT_NAME2_PROTOCOL     *ComponentNameInterface;
  UINTN                            ChildhandleCount;
  EFI_HANDLE                       *ChildhandleBuffer;
  EFI_DEVICE_PATH_PROTOCOL         *ChildHandleDevicePath;
  EFI_DEVICE_PATH_PROTOCOL         *ControllerDevicePath;
  VIDEO_OUTPUT_PORT_INFO           *TempDisplayPortInfo;
  UINTN                            Index;

  TempVideoOutputName                   = NULL;
  TempControllerName                    = NULL;
  VgaControllerInfo->NumOfConnectedPort = 0;
  ComponentNameInterface                = NULL;
  DriverBindingHandleCount              = 0;
  ChildhandleCount                      = 0;

  Status = gBS->HandleProtocol (
                  VgaControllerInfo->ControllerHandle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **)&ControllerDevicePath
                  );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }
  //
  // Get Controller Name by DriverBinding Protocol.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiDriverBindingProtocolGuid,
                  NULL,
                  &DriverBindingHandleCount,
                  &DriverBindingHandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (DrvierBindingIndex = 0; DrvierBindingIndex < DriverBindingHandleCount; DrvierBindingIndex++) {
    Status = gBS->HandleProtocol (
                    DriverBindingHandleBuffer[DrvierBindingIndex],
                    &gEfiComponentName2ProtocolGuid,
                    (VOID **)&ComponentNameInterface
                    );
    if (!EFI_ERROR (Status)) {
      TempControllerName = NULL;
      Status = ComponentNameInterface->GetControllerName (
                                         ComponentNameInterface,
                                         VgaControllerInfo->ControllerHandle,
                                         NULL,
                                         ComponentNameInterface->SupportedLanguages,
                                         &TempControllerName
                                         );

      if (!EFI_ERROR (Status)) {
        //
        // Update Controller Name.
        //
        ControllerName = AllocateZeroPool (StrSize (TempControllerName));
        ASSERT (ControllerName != NULL);
        if (ControllerName == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }

        StrCpy (ControllerName, TempControllerName);
        VgaControllerInfo->ControllerName = ControllerName;
        break;
      }
    }
  }

  if (DriverBindingHandleCount != 0) {
    gBS->FreePool (DriverBindingHandleBuffer);
  }

  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Get connected display port name.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiEdidDiscoveredProtocolGuid,
                  NULL,
                  &ChildhandleCount,
                  &ChildhandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  TempDisplayPortInfo = AllocateZeroPool (sizeof (VIDEO_OUTPUT_PORT_INFO) * ChildhandleCount);
  ASSERT (TempDisplayPortInfo != NULL);
  if (TempDisplayPortInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (ChildHandleIndex = 0; ChildHandleIndex < ChildhandleCount; ChildHandleIndex++) {
    Status = gBS->HandleProtocol (
                    ChildhandleBuffer[ChildHandleIndex],
                    &gEfiDevicePathProtocolGuid,
                    (VOID*)&ChildHandleDevicePath
                    );
    if (!EFI_ERROR (Status)) {
      if (CompareMem (
              ControllerDevicePath,
              ChildHandleDevicePath,
              GetDevicePathSize (ControllerDevicePath) - END_DEVICE_PATH_LENGTH
              ) == 0) {
        Status = ComponentNameInterface->GetControllerName (
                                           ComponentNameInterface,
                                           VgaControllerInfo->ControllerHandle,
                                           ChildhandleBuffer[ChildHandleIndex],
                                           ComponentNameInterface->SupportedLanguages,
                                           &TempVideoOutputName
                                           );
        if (!EFI_ERROR (Status)) {
          VideoOutputName = AllocateZeroPool (StrSize (TempVideoOutputName));
          ASSERT (VideoOutputName != NULL);
          if (VideoOutputName == NULL) {
            return EFI_OUT_OF_RESOURCES;
          }

          StrCpy (VideoOutputName, TempVideoOutputName);
          TempDisplayPortInfo[ChildHandleIndex].PortName = VideoOutputName;
        } else {
          TempDisplayPortInfo[ChildHandleIndex].PortName = L"\0";
        }
        TempDisplayPortInfo[ChildHandleIndex].PortAcpiADR = GetAcpiADRFromDevicePath (ChildHandleDevicePath);
        VgaControllerInfo->NumOfConnectedPort++;
      }
    }
  }

  Index = 0;
  VgaControllerInfo->ConnectedPortInfo = AllocateZeroPool (sizeof (PLUG_IN_ACTIVE_VIDEO_CONTROLLER_INFO) * VgaControllerInfo->NumOfConnectedPort);
  ASSERT (VgaControllerInfo->ConnectedPortInfo != NULL);
  if (VgaControllerInfo->ConnectedPortInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (ChildHandleIndex = 0; ChildHandleIndex < ChildhandleCount; ChildHandleIndex++) {
    if (TempDisplayPortInfo[ChildHandleIndex].PortAcpiADR != NULL) {
      VgaControllerInfo->ConnectedPortInfo[Index].PortName = TempDisplayPortInfo[ChildHandleIndex].PortName;
      VgaControllerInfo->ConnectedPortInfo[Index].PortAcpiADR = TempDisplayPortInfo[ChildHandleIndex].PortAcpiADR;
      Index++;
    }
  }
  gBS->FreePool (TempDisplayPortInfo);
  if (ChildhandleCount != 0) {
    gBS->FreePool (ChildhandleBuffer);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
GetAcpiADRFromGopHandle (
  IN    EFI_HANDLE             VgaControllerHandle,
  OUT   ACPI_ADR_DEVICE_PATH   **AcpiADR
  )
{
  EFI_STATUS                 Status;
  EFI_HANDLE                 *GopHandleBuffer;
  UINTN                      GopHandleCount;
  UINTN                      Index;
  EFI_DEVICE_PATH_PROTOCOL   *ControllerDevicePath;
  EFI_DEVICE_PATH_PROTOCOL   *GopDevicePath;

  Status = gBS->HandleProtocol (
                  VgaControllerHandle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **)&ControllerDevicePath
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  GopHandleCount = 0;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiGraphicsOutputProtocolGuid,
                  NULL,
                  &GopHandleCount,
                  &GopHandleBuffer
                  );
  if (!EFI_ERROR (Status)) {
    //
    // Add all the child handles as possible Console Device
    //
    for (Index = 0; Index < GopHandleCount; Index++) {
      Status = gBS->HandleProtocol (
                      GopHandleBuffer[Index],
                      &gEfiDevicePathProtocolGuid,
                      (VOID*)&GopDevicePath
                      );
      if (EFI_ERROR (Status)) {
        continue;
      }

      if (CompareMem (ControllerDevicePath, GopDevicePath, GetDevicePathSize (ControllerDevicePath) - END_DEVICE_PATH_LENGTH) == 0) {
        *AcpiADR = GetAcpiADRFromDevicePath (GopDevicePath);
        gBS->FreePool (GopHandleBuffer);
        return EFI_SUCCESS;
      }
    }

    if (GopHandleCount != 0) {
      gBS->FreePool (GopHandleBuffer);
    }
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
PlugInVgaUpdateInfo (
  VOID
  )
{
  EFI_HANDLE                               *HandleList;
  EFI_HANDLE                               *PtrToHandle;
  UINTN                                    NumOfHandles;
  UINTN                                    Index;
  PLUG_IN_ACTIVE_VIDEO_CONTROLLER_INFO     *PtrToList;
  UINTN                                    VarSize;

  if (mPlugInVgaInfo != NULL) {
    return EFI_SUCCESS;
  }

  HandleList = NULL;
  HandleList = GetVariableAndSize (
                 PLUG_IN_VIDEO_DISPLAY_INFO_VAR_NAME,
                 &gEfiGenericVariableGuid,
                 &VarSize
                 );
  if (HandleList == NULL) {
    return EFI_NOT_FOUND;
  }

  PtrToHandle = HandleList;
  NumOfHandles = 0;
  while (*PtrToHandle != NULL_HANDLE) {
    NumOfHandles++;
    PtrToHandle++;
  }
  if (NumOfHandles == 0) {
    return EFI_NOT_FOUND;
  }
  //
  // Add the NULL PLUG_IN_ACTIVE_VIDEO_CONTROLLER_INFO as the end of list.
  //
  mPlugInVgaInfo = AllocateZeroPool ((NumOfHandles + 1) * sizeof (PLUG_IN_ACTIVE_VIDEO_CONTROLLER_INFO));
  ASSERT (mPlugInVgaInfo != NULL);
  if (mPlugInVgaInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  PtrToList = mPlugInVgaInfo;

  for (Index = 0; Index < NumOfHandles; Index++) {

    PtrToList->ControllerHandle = HandleList[Index];
    UpdateDisplayPortInfo (PtrToList);
    GetAcpiADRFromGopHandle (PtrToList->ControllerHandle, &PtrToList->DefaultAcpiADR);

    PtrToList++;
  }

  if (HandleList) {
    gBS->FreePool (HandleList);
  }

  return EFI_SUCCESS;
}

VOID
FindDefaultOption (
  IN OUT PLUG_IN_ACTIVE_VIDEO_CONTROLLER_INFO  *VgaControllerInfo,
  IN  EFI_DEVICE_PATH_PROTOCOL                 *ConOutDevicePath,
  IN  UINTN                                    NumOfOptions,
  OUT EFI_IFR_ONE_OF_OPTION                    *OptionList,
  IN  VOID                                     *OptionsOpCodeHandle
  )
{
  ACPI_ADR_DEVICE_PATH            *AcpiADR;
  UINTN                           Index;

  AcpiADR = NULL;

  while (!IsDevicePathEnd (ConOutDevicePath)) {
    if ((DevicePathType (ConOutDevicePath) == ACPI_DEVICE_PATH) &&
        (DevicePathSubType (ConOutDevicePath) == ACPI_ADR_DP)) {
      if (DevicePathNodeLength (ConOutDevicePath) != sizeof (ACPI_ADR_DEVICE_PATH)) {
        VgaControllerInfo->SelectedAcpiADR = VgaControllerInfo->DefaultAcpiADR;
        OptionList[NumOfOptions - 1].Flags = EFI_IFR_OPTION_DEFAULT;
        return;
      }
      AcpiADR = (ACPI_ADR_DEVICE_PATH*)ConOutDevicePath;
    }
    ConOutDevicePath = NextDevicePathNode (ConOutDevicePath);
  }

  if (AcpiADR == NULL) {
    return;
  }

  for (Index = 0; Index < NumOfOptions; Index++) {
    if (AcpiADR->ADR == OptionList[Index].Value.u32) {
      VgaControllerInfo->SelectedAcpiADR = VgaControllerInfo->ConnectedPortInfo[Index].PortAcpiADR;
      OptionList[Index].Flags = EFI_IFR_OPTION_DEFAULT;
      return;
    }
  }

  OptionList[NumOfOptions - 1].Flags = EFI_IFR_OPTION_DEFAULT;
  return;
}


EFI_DEVICE_PATH_PROTOCOL*
UpdateDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL  *Source,
  IN  EFI_DEVICE_PATH_PROTOCOL  *ControllerDevicePath,
  IN  EFI_DEVICE_PATH_PROTOCOL  *CustomizedConDevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *NewDevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *Instance;
  EFI_DEVICE_PATH_PROTOCOL      *PtrToSrc;
  UINTN                         Size;

  PtrToSrc = Source;
  NewDevicePath = NULL;
  Instance      = NULL;

  while (PtrToSrc != NULL) {

    Instance = GetNextDevicePathInstance (&PtrToSrc, &Size);

    if (CompareMem (Instance, ControllerDevicePath, GetDevicePathSize (ControllerDevicePath) - END_DEVICE_PATH_LENGTH) == 0) {
      Instance = CustomizedConDevicePath;
    }

    NewDevicePath = AppendDevicePathInstance (NewDevicePath, Instance);
  }

  return NewDevicePath;
}

EFI_STATUS
PlugInVgaDisplaySelectionSave (
  VOID
  )
{
  EFI_STATUS                               Status;
  EFI_DEVICE_PATH_PROTOCOL                 *ControllerDevicePath;
  EFI_DEVICE_PATH_PROTOCOL                 *NewDevicePath;
  EFI_DEVICE_PATH_PROTOCOL                 *NewConOutVar;
  EFI_DEVICE_PATH_PROTOCOL                 *PtrNewConOutVar;
  UINTN                                    ConOutSize;
  PLUG_IN_ACTIVE_VIDEO_CONTROLLER_INFO     *PtrToList;
  UINT32                                   OrigDisplayDeviceAdr;

  if (mPlugInVgaInfo == NULL) {
    return EFI_UNSUPPORTED;
  }

  PtrToList = mPlugInVgaInfo;

  NewDevicePath = NULL;
  OrigDisplayDeviceAdr = 0;

  NewConOutVar = GetVariableAndSize (
                   L"ConOut",
                   &gEfiGlobalVariableGuid,
                   &ConOutSize
                   );

  PtrNewConOutVar = NewConOutVar;
  if (PtrNewConOutVar != NULL) {
    while (!IsDevicePathEnd (PtrNewConOutVar)) {
      if ((DevicePathType (PtrNewConOutVar) == ACPI_DEVICE_PATH) && (DevicePathSubType (PtrNewConOutVar) == ACPI_ADR_DP)) {
        if (DevicePathNodeLength (PtrNewConOutVar) == sizeof (ACPI_ADR_DEVICE_PATH)) {
          OrigDisplayDeviceAdr = ((ACPI_ADR_DEVICE_PATH *)(PtrNewConOutVar))->ADR;
        }
      }
      PtrNewConOutVar = NextDevicePathNode (PtrNewConOutVar);
    }
  }

  while (PtrToList->ControllerHandle != NULL) {

    Status = gBS->HandleProtocol (
                    PtrToList->ControllerHandle,
                    &gEfiDevicePathProtocolGuid,
                    &ControllerDevicePath
                    );

    if (!EFI_ERROR (Status)) {
      //
      // Append Acpi ADR to new Devicepath, If AcpiADR poineter has been assigneded address from
      // DevicePath of Child Handle of GOP.
      //
      if (PtrToList->SelectedAcpiADR->ADR != OrigDisplayDeviceAdr) {
        NewDevicePath = AppendDevicePathNode (ControllerDevicePath, (EFI_DEVICE_PATH_PROTOCOL*)PtrToList->SelectedAcpiADR);
      } else {
        return EFI_UNSUPPORTED;
      }
    }

    NewConOutVar = UpdateDevicePath (NewConOutVar, ControllerDevicePath, NewDevicePath);

    PtrToList++;
  }

  ConOutSize = GetDevicePathSize (NewConOutVar);

  Status = gRT->SetVariable (
                  L"UserVgaSelection",
                  &gEfiGenericVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  ConOutSize,
                  NewConOutVar
                  );

  return Status;
}

EFI_STATUS
PlugInVideoDisplaySelectionOption (
  IN  EFI_QUESTION_ID                      QuestionId,
  IN  EFI_IFR_TYPE_VALUE                   *Value
  )
{
  PLUG_IN_ACTIVE_VIDEO_CONTROLLER_INFO     *PtrToList;
  UINTN                                    Index;

  if (mPlugInVgaInfo == NULL) {
    return EFI_UNSUPPORTED;
  }

  PtrToList = mPlugInVgaInfo + (QuestionId - KEY_PLUG_IN_DISPLAY_SELECTION1);

  if (Value->u32 == 0) {
    PtrToList->SelectedAcpiADR = PtrToList->DefaultAcpiADR;
  } else {
    for (Index = 0; Index < PtrToList->NumOfConnectedPort; Index++) {
      if (PtrToList->ConnectedPortInfo[Index].PortAcpiADR->ADR == Value->u32)
       PtrToList->SelectedAcpiADR = PtrToList->ConnectedPortInfo[Index].PortAcpiADR;
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
PlugInVideoDisplaySelectionLabel (
  IN  EFI_HII_HANDLE                        HiiHandle
  )
{
  EFI_STATUS                                Status;
  EFI_IFR_ONE_OF_OPTION                     *OptionList;
  UINTN                                     OptionsConut;
  PLUG_IN_ACTIVE_VIDEO_CONTROLLER_INFO      *PtrToList;
  UINTN                                     ControllerIndex;
  UINTN                                     ChildHandleIndex;
  EFI_STRING_ID                             ControllerStringId;
  EFI_STRING_ID                             ChildStringId;
  EFI_QUESTION_ID                           QuestionId;
  EFI_DEVICE_PATH_PROTOCOL                  *ConOutVar;
  UINTN                                     ConOutSize;
  EFI_DEVICE_PATH_PROTOCOL                  *MatchConOutDevice;
  EFI_DEVICE_PATH_PROTOCOL                  *ControllerDevicePath;
  EFI_STRING_ID                             HelpStringId;
  CHAR16                                    *PluginHelpString = \
                                             L"Display Selection for Plug-in Vga Controller, Only supported Uefi Boot mode";
  VOID                                      *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL                        *StartLabel;
  VOID                                      *OptionsOpCodeHandle;

  if (mPlugInVgaInfo == NULL) {
    return EFI_UNSUPPORTED;
  }

  ConOutVar = NULL;
  ConOutVar = GetVariableAndSize (
                L"ConOut",
                &gEfiGlobalVariableGuid,
                &ConOutSize
                );
  if (ConOutVar == NULL) {
    return EFI_NOT_FOUND;
  }

  PtrToList = mPlugInVgaInfo;
  ControllerIndex = 0;
  while (PtrToList->ControllerHandle != NULL) {
    StartOpCodeHandle = HiiAllocateOpCodeHandle ();
    ASSERT (StartOpCodeHandle != NULL);

    OptionsOpCodeHandle= HiiAllocateOpCodeHandle ();
    ASSERT (OptionsOpCodeHandle != NULL);

    StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
    StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
    StartLabel->Number       = PLUG_IN_DISPLAY_SELECTION_START_LABEL;

    Status = gBS->HandleProtocol (
                    PtrToList->ControllerHandle,
                    &gEfiDevicePathProtocolGuid,
                    (VOID **)&ControllerDevicePath
                    );

    MatchConOutDevice = PartMatchInstance (ConOutVar, ControllerDevicePath);
    if (MatchConOutDevice == NULL) {
      PtrToList++;
      ControllerIndex++;
      continue;
    }
    //
    // Add the Controller Name.
    //
    ControllerStringId = HiiSetString (HiiHandle, 0, PtrToList->ControllerName, NULL);
    HelpStringId = HiiSetString (HiiHandle, 0, PluginHelpString, NULL);
    //
    // Add the Video Output Ports.
    //
    OptionsConut = (PtrToList->NumOfConnectedPort + 1);
    OptionList = AllocateZeroPool (sizeof (EFI_IFR_ONE_OF_OPTION) * OptionsConut);

    for (ChildHandleIndex = 0; ChildHandleIndex < PtrToList->NumOfConnectedPort; ChildHandleIndex++) {
      ChildStringId = HiiSetString (HiiHandle, 0, PtrToList->ConnectedPortInfo[ChildHandleIndex].PortName, NULL);
      OptionList[ChildHandleIndex].Option       = ChildStringId;
      OptionList[ChildHandleIndex].Value.u32    = PtrToList->ConnectedPortInfo[ChildHandleIndex].PortAcpiADR->ADR;
    }

    FindDefaultOption (PtrToList, MatchConOutDevice, OptionsConut, OptionList, OptionsOpCodeHandle);

    for (ChildHandleIndex = 0; ChildHandleIndex < PtrToList->NumOfConnectedPort; ChildHandleIndex++) {
      HiiCreateOneOfOptionOpCode (
        OptionsOpCodeHandle,
        OptionList[ChildHandleIndex].Option,
        OptionList[ChildHandleIndex].Flags,
        EFI_IFR_NUMERIC_SIZE_4,
        OptionList[ChildHandleIndex].Value.u32
        );
    }

    QuestionId = (EFI_QUESTION_ID)(KEY_PLUG_IN_DISPLAY_SELECTION1 + ControllerIndex);

    HiiCreateOneOfOpCode (
      StartOpCodeHandle,
      QuestionId,
      INVALID_VARSTORE_ID,
      0,
      ControllerStringId,
      HelpStringId,
      0x04,
      EFI_IFR_NUMERIC_SIZE_4,
      OptionsOpCodeHandle,
      NULL
      );

    HiiUpdateForm (
      HiiHandle,
      NULL,
      (EFI_FORM_ID)VIDEO_CONFIG_FORM_ID,
      StartOpCodeHandle,
      NULL
      );
    HiiFreeOpCodeHandle (StartOpCodeHandle);
    HiiFreeOpCodeHandle (OptionsOpCodeHandle);
    gBS->FreePool (OptionList);
    gBS->FreePool (MatchConOutDevice);

    PtrToList++;
    ControllerIndex++;
  }

  if (ConOutVar) {
    gBS->FreePool (ConOutVar);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
ClearFormDataFromLabel (
  IN     EFI_HII_HANDLE               HiiHandle,
  IN     EFI_FORM_ID                  FormID,
  IN     UINT16                       Label
  )
{
  VOID                                *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL                  *StartLabel;
  VOID                                *OptionsOpCodeHandle;
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  OptionsOpCodeHandle= HiiAllocateOpCodeHandle ();
  ASSERT (OptionsOpCodeHandle != NULL);

  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number = Label;

  HiiUpdateForm (
    HiiHandle,
    NULL,
    (EFI_FORM_ID)FormID,
    StartOpCodeHandle,
    NULL
    );
  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (OptionsOpCodeHandle);

  return EFI_SUCCESS;
}
