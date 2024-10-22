/** @file
  The boot manager implementation

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

/**
  The platform boot manager reference implementation

Copyright (c) 2004 - 2012, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "BootManager.h"

STATIC EFI_FORM_BROWSER2_PROTOCOL         *gBootManagerFormBrowser2;
BOOLEAN                                   gConnectAllHappened;

STATIC BOOLEAN                             mBootMgrSendFormBySelf = FALSE;
STATIC EFI_EVENT                           mUpdateBootOptionListEvt = NULL;
STATIC LIST_ENTRY                          mBackupBootOptionsList = INITIALIZE_LIST_HEAD_VARIABLE (mBackupBootOptionsList);
STATIC LIST_ENTRY                          *mBootOptionsList;
STATIC H2O_BDS_LOAD_OPTION                 *gOption;
STATIC EFI_STRING_ID                       mDeviceTypeStrId[] = {
                                             STRING_TOKEN (STR_DEVICE_TYPE_LEGACY_BEV),
                                             STRING_TOKEN (STR_DEVICE_TYPE_LEGACY_FLOPPY),
                                             STRING_TOKEN (STR_DEVICE_TYPE_LEGACY_HARD_DRIVE),
                                             STRING_TOKEN (STR_DEVICE_TYPE_LEGACY_CDROM),
                                             STRING_TOKEN (STR_DEVICE_TYPE_LEGACY_PCMCIA),
                                             STRING_TOKEN (STR_DEVICE_TYPE_LEGACY_USB),
                                             STRING_TOKEN (STR_DEVICE_TYPE_LEGACY_NETWORK),
                                             STRING_TOKEN (STR_DEVICE_TYPE_LEGACY_UNKNOWN_DEVICE),
                                             };

EFI_LOAD_FILE_PROTOCOL                     mBootManagerLoadFile = {BootManagerLoadFile};
STATIC HII_VENDOR_DEVICE_PATH              mBootManagerHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    BOOT_MANAGER_FORMSET_GUID
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

STATIC BOOT_MANAGER_CALLBACK_DATA  gBootManagerPrivate = {
  BOOT_MANAGER_CALLBACK_DATA_SIGNATURE,
  NULL,
  NULL,
  {
    BootManagerFakeExtractConfig,
    BootManagerFakeRouteConfig,
    BootManagerCallback
  }
};

/**
  Compares two boot option.

  @param[in] BootOptions1       A pointer to one boot option.
  @param[in] BootOptions2       A pointer to another boot option.

  @retval TRUE        BootOptions1 and BootOptions2 are identical.
  @retval FALSE       BootOptions1 and BootOptions2 are not identical.
**/
STATIC
BOOLEAN
IsIdenticalBootOption (
  IN H2O_BDS_LOAD_OPTION      *Option1,
  IN H2O_BDS_LOAD_OPTION      *Option2
  )
{
  if (Option1 == NULL || Option2 == NULL) {
    return FALSE;
  }

  if (Option1->DevicePath == Option2->DevicePath ||
      (GetDevicePathSize (Option1->DevicePath) == GetDevicePathSize (Option2->DevicePath) &&
       CompareMem (Option1->DevicePath, Option2->DevicePath, GetDevicePathSize (Option1->DevicePath)) == 0)) {
    return TRUE;
  }

  return FALSE;
}

/**
  Free each boot option from boot option list.

  @param[in, out] BootOptionsList       A pointer to the boot option list.
**/
STATIC
VOID
FreeBootOptionList (
  IN OUT LIST_ENTRY           *BootOptionsList
  )
{
  LIST_ENTRY                  *Link;
  H2O_BDS_LOAD_OPTION         *Option;

  if (BootOptionsList == NULL) {
    return;
  }

  Link = GetFirstNode (BootOptionsList);
  while (!IsNull (BootOptionsList, Link)) {
    Option = BDS_OPTION_FROM_LINK (Link);
    Link   = GetNextNode (BootOptionsList, Link);

    RemoveEntryList (&Option->Link);
    FreePool (Option);
  }
}

/**
  Copy the source boot option list to the destination boot option list.

  @param[in, out] DstBootOptionsList       A pointer to the destination boot option list.
  @param[in]      SrcBootOptionsList       A pointer to the source boot option list.
**/
STATIC
VOID
CopyBootOptionList (
  IN OUT LIST_ENTRY           *DstBootOptionsList,
  IN     LIST_ENTRY           *SrcBootOptionsList
  )
{
  LIST_ENTRY                  *SrcLink;
  H2O_BDS_LOAD_OPTION         *SrcOption;
  H2O_BDS_LOAD_OPTION         *DstOption;

  if (DstBootOptionsList == NULL || SrcBootOptionsList == NULL) {
    return;
  }

  SrcLink = GetFirstNode (SrcBootOptionsList);
  while (!IsNull (SrcBootOptionsList, SrcLink)) {
    SrcOption = BDS_OPTION_FROM_LINK (SrcLink);
    SrcLink   = GetNextNode (SrcBootOptionsList, SrcLink);

    DstOption = AllocateCopyPool (sizeof (H2O_BDS_LOAD_OPTION), SrcOption);
    if (DstOption != NULL) {
      InsertTailList (DstBootOptionsList, &DstOption->Link);
    }
  }
}

/**
  Compares two boot option lists.

  @param[in] BootOptionsList1       A pointer to one boot option list.
  @param[in] BootOptionsList2       A pointer to another boot option list.

  @retval TRUE        BootOptionsList1 and BootOptionsList2 are identical.
  @retval FALSE       BootOptionsList1 and BootOptionsList2 are not identical.
**/
STATIC
BOOLEAN
CompareBootOptionList (
  IN LIST_ENTRY               *BootOptionsList1,
  IN LIST_ENTRY               *BootOptionsList2
  )
{
  LIST_ENTRY                  *Link1;
  LIST_ENTRY                  *Link2;
  H2O_BDS_LOAD_OPTION         *Option1;
  H2O_BDS_LOAD_OPTION         *Option2;

  if (BootOptionsList1 == NULL || BootOptionsList2 == NULL) {
    return FALSE;
  }

  Link1 = GetFirstNode (BootOptionsList1);
  Link2 = GetFirstNode (BootOptionsList2);
  while (!IsNull (BootOptionsList1, Link1) &&
         !IsNull (BootOptionsList2, Link2)) {
    Option1 = BDS_OPTION_FROM_LINK (Link1);
    Option2 = BDS_OPTION_FROM_LINK (Link2);
    if (!IsIdenticalBootOption (Option1, Option2)) {
      return FALSE;
    }

    Link1 = GetNextNode (BootOptionsList1, Link1);
    Link2 = GetNextNode (BootOptionsList2, Link2);
  }
  if (IsNull (BootOptionsList1, Link1) &&
      IsNull (BootOptionsList2, Link2)) {
    return TRUE;
  }

  return FALSE;
}

/**
  Check if the boot option is a legacy boot option.

  @param[in] Option                    A pointer to the boot option.

  @retval TRUE                         The boot option is a legacy boot option.
  @retval FALSE                        The boot option is not a legacy boot option.
**/
STATIC
BOOLEAN
IsLegacyBootOption (
  IN H2O_BDS_LOAD_OPTION               *Option
  )
{
  if (Option == NULL) {
    return FALSE;
  }

  return (BOOLEAN) ((DevicePathType (Option->DevicePath) == BBS_DEVICE_PATH) &&
                    (DevicePathSubType (Option->DevicePath) == BBS_BBS_DP));
}

/**
  Send hot key notify event.

  @param[in] HotKeyAction              Hot key action
  @param[in] OptionsList               Target question ID

  @retval EFI_SUCCESS                  Send hot key notify successfully
  @retval Other                        H2O form browser is not found or notify function return failed.
**/
STATIC
EFI_STATUS
SendHotKeyNotify (
  IN HOT_KEY_ACTION                    HotKeyAction,
  IN UINT16                            QuestionId
  )
{
  EFI_STATUS                           Status;
  H2O_FORM_BROWSER_PROTOCOL            *FBProtocol;
  H2O_DISPLAY_ENGINE_EVT_HOT_KEY       HotKeyNotify;

  Status = gBS->LocateProtocol (&gH2OFormBrowserProtocolGuid, NULL, (VOID **) &FBProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (&HotKeyNotify, sizeof (HotKeyNotify));
  HotKeyNotify.Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_HOT_KEY);
  HotKeyNotify.Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_HOT_KEY;
  HotKeyNotify.Hdr.Target = H2O_DISPLAY_ENGINE_EVT_TARGET_FORM_BROWSER;

  HotKeyNotify.HotKeyAction           = HotKeyAction;
  HotKeyNotify.HotKeyTargetQuestionId = QuestionId;

  return FBProtocol->Notify (FBProtocol, &HotKeyNotify.Hdr);
}

/**
  Shift boot option in list and update to BootOrder variable based on list.

  @param[in] QuestionId                The question ID of current boot option.
  @param[in] ToHigherPriority          Shift current boot option to higher (TRUE) or lower (FALSE) priority.

  @retval EFI_SUCCESS                  Shift boot option successfully
  @retval EFI_NOT_FOUND                Current boot option is not found
  @retval EFI_ABORTED                  Exchanged two boot options are not the same boot type.
  @retval EFI_OUT_OF_RESOURCES         Failed to allocate memory
**/
STATIC
EFI_STATUS
ShiftBootOption (
  IN EFI_QUESTION_ID                   QuestionId,
  IN BOOLEAN                           ToHigherPriority
  )
{
  EFI_STATUS                           Status;
  LIST_ENTRY                           *OptionsList;
  H2O_BDS_LOAD_OPTION                  *CurrentOption;
  H2O_BDS_LOAD_OPTION                  *Option;
  H2O_BDS_LOAD_OPTION                  *PreviousOption;
  EFI_QUESTION_ID                      PreviousQuestionId;
  H2O_BDS_LOAD_OPTION                  *NextOption;
  EFI_QUESTION_ID                      NextQuestionId;
  LIST_ENTRY                           *Link;
  UINT16                               OptionCount;
  UINT16                               *BootOrderVar;

  //
  // Enumerate option list to get the info of previous, current and next options.
  //
  CurrentOption      = NULL;
  PreviousOption     = NULL;
  PreviousQuestionId = 0;
  NextOption         = NULL;
  NextQuestionId     = 0;
  OptionCount        = 0;
  OptionsList        = &mBackupBootOptionsList;
  for (Link = GetFirstNode (OptionsList); !IsNull (OptionsList, Link); Link = GetNextNode (OptionsList, Link)) {
    Option = BDS_OPTION_FROM_LINK (Link);
    OptionCount++;

    if (((Option->Attributes & LOAD_OPTION_HIDDEN) != 0) || ((Option->Attributes & LOAD_OPTION_ACTIVE) == 0)) {
      continue;
    }

    if (CurrentOption == NULL) {
      if (OptionCount == QuestionId) {
        CurrentOption = Option;
        continue;
      }
      PreviousOption     = Option;
      PreviousQuestionId = OptionCount;
    } else {
      NextOption     = Option;
      NextQuestionId = OptionCount;
      break;
    }
  }

  if (CurrentOption == NULL ||
      (ToHigherPriority  && PreviousOption == NULL) ||
      (!ToHigherPriority && NextOption     == NULL)) {
    return EFI_NOT_FOUND;
  }

  //
  // Forbid to exchange EFI and legacy boot options because of Setup "EFI device first" policy.
  //
  if ((ToHigherPriority && (IsLegacyBootOption (CurrentOption) != IsLegacyBootOption (PreviousOption))) ||
      (!ToHigherPriority && (IsLegacyBootOption (CurrentOption) != IsLegacyBootOption (NextOption)))) {
    return EFI_ABORTED;
  }

  BootOrderVar = CommonGetVariableData (L"BootOrder", &gEfiGlobalVariableGuid);
  if (BootOrderVar == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Shift the position of current option in the list and update BootOrder variable based on the order in the list.
  //
  RemoveEntryList (&CurrentOption->Link);
  if (ToHigherPriority) {
    InsertTailList (&PreviousOption->Link, &CurrentOption->Link);
  } else {
    InsertHeadList (&NextOption->Link, &CurrentOption->Link);
  }

  OptionCount = 0;
  for (Link = GetFirstNode (OptionsList); !IsNull (OptionsList, Link); Link = GetNextNode (OptionsList, Link)) {
    Option = BDS_OPTION_FROM_LINK (Link);
    BootOrderVar[OptionCount++] = Option->LoadOrder;
  }

  if (FeaturePcdGet (PcdAutoCreateDummyBootOption)) {
    SyncBootOrder ();
    BdsLibRestoreBootOrderFromPhysicalBootOrder ();
  }

  Status = CommonSetVariable (
             L"BootOrder",
             &gEfiGlobalVariableGuid,
             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
             OptionCount * sizeof (UINT16),
             BootOrderVar
             );
  FreePool (BootOrderVar);

  //
  // Update option list and HII form.
  //
  gBdsServices->GetBootList (gBdsServices, &mBootOptionsList);
  FreeBootOptionList (&mBackupBootOptionsList);
  CopyBootOptionList (&mBackupBootOptionsList, mBootOptionsList);
  UpdateBootOptionListIntoHiiForm ();

  SendHotKeyNotify (HotKeyGoTo, (UINT16) (ToHigherPriority ? PreviousQuestionId : NextQuestionId));
  return EFI_SUCCESS;
}

/**
  This function allows a caller to extract the current configuration for one
  or more named elements from the target driver.


  @param This            Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Request         A null-terminated Unicode string in <ConfigRequest> format.
  @param Progress        On return, points to a character in the Request string.
                         Points to the string's null terminator if request was successful.
                         Points to the most recent '&' before the first failing name/value
                         pair (or the beginning of the string if the failure is in the
                         first name/value pair) if the request was not successful.
  @param Results         A null-terminated Unicode string in <ConfigAltResp> format which
                         has all values filled in for the names in the Request string.
                         String to be allocated by the called function.

  @retval  EFI_SUCCESS            The Results is filled with the requested values.
  @retval  EFI_OUT_OF_RESOURCES   Not enough memory to store the results.
  @retval  EFI_INVALID_PARAMETER  Request is illegal syntax, or unknown name.
  @retval  EFI_NOT_FOUND          Routing data doesn't match any storage in this driver.

**/
EFI_STATUS
EFIAPI
BootManagerFakeExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *Progress = Request;
  return EFI_NOT_FOUND;
}

/**
  This function processes the results of changes in configuration.


  @param This            Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Configuration   A null-terminated Unicode string in <ConfigResp> format.
  @param Progress        A pointer to a string filled in with the offset of the most
                         recent '&' before the first failing name/value pair (or the
                         beginning of the string if the failure is in the first
                         name/value pair) or the terminating NULL if all was successful.

  @retval  EFI_SUCCESS            The Results is processed successfully.
  @retval  EFI_INVALID_PARAMETER  Configuration is NULL.
  @retval  EFI_NOT_FOUND          Routing data doesn't match any storage in this driver.

**/
EFI_STATUS
EFIAPI
BootManagerFakeRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
  if (Configuration == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Configuration;
  return EFI_NOT_FOUND;
}

/**
  This call back function is registered with Boot Manager formset.
  When user selects a boot option, this call back function will
  be triggered. The boot option is saved for later processing.


  @param This            Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Action          Specifies the type of action taken by the browser.
  @param QuestionId      A unique value which is sent to the original exporting driver
                         so that it can identify the type of data to expect.
  @param Type            The type of value for the question.
  @param Value           A pointer to the data being sent to the original exporting driver.
  @param ActionRequest   On return, points to the action requested by the callback function.

  @retval  EFI_SUCCESS           The callback successfully handled the action.
  @retval  EFI_INVALID_PARAMETER The setup browser call this function with invalid parameters.

**/
EFI_STATUS
EFIAPI
BootManagerCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  H2O_BDS_LOAD_OPTION     *Option;
  LIST_ENTRY              *Link;
  UINT16                  KeyCount;
  EFI_STATUS              Status;
  CHAR16                  *ExitData;
  UINTN                   ExitDataSize;

  if (!FeaturePcdGet (PcdH2OFormBrowserSupported) && Action == EFI_BROWSER_ACTION_CHANGING) {
    //
    // For compatible to old form browser which only use EFI_BROWSER_ACTION_CHANGING action,
    // change action to EFI_BROWSER_ACTION_CHANGED to make it workable.
    //
    Action = EFI_BROWSER_ACTION_CHANGED;
  }

  if (Action == EFI_BROWSER_ACTION_CHANGED) {
    if ((Value == NULL) || (ActionRequest == NULL)) {
      return EFI_INVALID_PARAMETER;
    }

    //
    // Initialize the key count
    //
    KeyCount = 0;
    gOption = NULL;
    for (Link = GetFirstNode (mBootOptionsList); !IsNull (mBootOptionsList, Link); Link = GetNextNode (mBootOptionsList, Link)) {
      Option = BDS_OPTION_FROM_LINK (Link);

      KeyCount++;

      //
      // Is this device the one chosen?
      //
      if (KeyCount == QuestionId) {
         gOption = Option;
        //
        // Request to exit SendForm(), so that we could boot the selected option
        //
        *ActionRequest = EFI_BROWSER_ACTION_REQUEST_EXIT;
        if (mUpdateBootOptionListEvt != NULL) {
          gBS->CloseEvent (mUpdateBootOptionListEvt);
          mUpdateBootOptionListEvt = NULL;
        }
        break;
      }
    }

    return EFI_SUCCESS;
  } else if (Action == H2O_BROWSER_ACTION_HOT_KEY_CALLBACK) {
    if (QuestionId == BOOT_MANAGER_UPDATE_BOOT_LIST_QUESTION_ID) {
      return UpdateBootOptionList ();
    }

    if (FeaturePcdGet (PcdH2OBootMgrChangeBootOrderSupported)) {
      return ShiftBootOption (QuestionId, Value->b);
    }
  } else if (Action == EFI_BROWSER_ACTION_FORM_OPEN) {
    if (!mBootMgrSendFormBySelf && QuestionId == 0x1212) {
      //
      // Boot Manager is imported by other VFR driver. Here to initialize boot manager.
      //
      InternalCallBootManagerBefore ();
    }
  } else if (Action == EFI_BROWSER_ACTION_FORM_CLOSE) {
    if (!mBootMgrSendFormBySelf && QuestionId == 0x1212) {
      InternalCallBootManagerAfter ();

      if (gOption != NULL) {
        Status = gBdsServices->LaunchLoadOption (gBdsServices, gOption, &ExitDataSize, &ExitData);
        if (!EFI_ERROR (Status)) {
          BdsLibBootSuccess (gOption);
        } else {
          BdsLibBootFailed (gOption, Status, ExitData, ExitDataSize);
        }
      }
    }
  }

  //
  // All other action return unsupported.
  //
  return EFI_UNSUPPORTED;
}

/**

  Registers HII packages for the Boot Manger to HII Database.
  It also registers the browser call back function.

  @retval  EFI_SUCCESS           HII packages for the Boot Manager were registered successfully.
  @retval  EFI_OUT_OF_RESOURCES  HII packages for the Boot Manager failed to be registered.

**/
EFI_STATUS
InitializeBootManager (
  VOID
  )
{
  EFI_STATUS                  Status;

  Status = gBS->LocateProtocol (&gEfiFormBrowser2ProtocolGuid, NULL, (VOID **) &gBootManagerFormBrowser2);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Install Device Path Protocol and Config Access protocol to driver handle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &gBootManagerPrivate.DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mBootManagerHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &gBootManagerPrivate.ConfigAccess,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Publish our HII data
  //
  gBootManagerPrivate.HiiHandle = HiiAddPackages (
                                    &gBootManagerFormSetGuid,
                                    gBootManagerPrivate.DriverHandle,
                                    BootManagerVfrBin,
                                    BootManagerDxeStrings,
                                    BootManagerDxeImages,
                                    NULL
                                    );
  if (gBootManagerPrivate.HiiHandle == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
  } else {
    Status = EFI_SUCCESS;
  }
  return Status;
}

/**
 Check the specific BBS Table entry is USB device

 @param[in] CurrentBbsTable     Pointer to current BBS table start address

 @retval TRUE                   It is USB device
 @retval FALSE                  It isn't USB device
**/
STATIC
BOOLEAN
IsUsbDevice (
  IN BBS_TABLE              *CurrentBbsTable
  )
{
  if ((CurrentBbsTable->Class == PCI_CLASS_SERIAL) &&
      (CurrentBbsTable->SubClass == PCI_CLASS_SERIAL_USB)) {
    return TRUE;
  }

  return FALSE;
}

/**
 Get prompt string which is consist of the description of EFI boot option and hardware device name

 @param[in] Option              Pointer to boot option data
 @param[in] HwBootDeviceInfo    Pointer to the array of hardware device info
 @param[in] HwBootDeviceCount   The number of hardware device info

 @return pointer to the prompt string or NULL if input parameter is invalid
**/
CHAR16 *
GetPromptString (
  IN H2O_BDS_LOAD_OPTION                *Option,
  IN HARDWARE_BOOT_DEVICE_INFO          *HwBootDeviceInfo,
  IN UINTN                              HwBootDeviceCount
  )
{
  CHAR16                                NoDeviceStr[] = L"No Device";
  UINTN                                 Index;
  UINT8                                 *Ptr;
  UINT8                                 *VarData;
  UINTN                                 DeviceCount;
  UINTN                                 Size;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  CHAR16                                *HwDeviceName;
  CHAR16                                *PromptString;


  if (Option == NULL) {
    return NULL;
  }

  if (!((DevicePathType (Option->DevicePath) == MEDIA_DEVICE_PATH &&
         DevicePathSubType (Option->DevicePath) == MEDIA_HARDDRIVE_DP) ||
        (DevicePathType (Option->DevicePath) == MESSAGING_DEVICE_PATH &&
         DevicePathSubType (Option->DevicePath) == MSG_USB_CLASS_DP))) {
    return AllocateCopyPool (StrSize (Option->Description), Option->Description);
  }

  VarData      = NULL;
  HwDeviceName = NULL;
  DeviceCount  = 1;
  DevicePath   = Option->DevicePath;

  if (DevicePathType (Option->DevicePath) == MESSAGING_DEVICE_PATH &&
      DevicePathSubType (Option->DevicePath) == MSG_USB_CLASS_DP) {
    //
    // For Windows To Go, assign no device string as device name if device number is 0.
    //
    VarData = BdsLibGetVariableAndSize (
                L"WindowsToGo",
                &gEfiGenericVariableGuid,
                &Size
                );
    if (VarData != NULL) {
      Ptr         = VarData;
      DevicePath  = (EFI_DEVICE_PATH_PROTOCOL *) Ptr;
      Ptr        += GetDevicePathSize (DevicePath);
      DeviceCount = *(UINT16 *) Ptr;
    } else {
      DeviceCount = 0;
    }

    if (DeviceCount == 0) {
      HwDeviceName = NoDeviceStr;
    }
  }

  //
  // Append device name to boot option description if BlkIo device path is match.
  //
  if (HwDeviceName == NULL) {
    for (Index = 0; Index < HwBootDeviceCount; Index++) {
      if (BdsLibCompareBlockIoDevicePath (HwBootDeviceInfo[Index].BlockIoDevicePath, DevicePath)) {
        HwDeviceName = HwBootDeviceInfo[Index].HwDeviceName;
        break;
      }
    }
  }

  if (HwDeviceName != NULL) {
    if (DeviceCount > 1) {
      Size = StrSize (Option->Description) + StrSize (L" (") + StrSize (HwDeviceName) + StrSize (L",...") + StrSize (L")");
    } else {
      Size = StrSize (Option->Description) + StrSize (L" (") + StrSize (HwDeviceName) + StrSize (L")");
    }
    PromptString = AllocateZeroPool (Size);
    if (PromptString != NULL) {
      StrCatS (PromptString, Size / sizeof(CHAR16), Option->Description);
      StrCatS (PromptString, Size / sizeof(CHAR16), L" (");
      StrCatS (PromptString, Size / sizeof(CHAR16), HwDeviceName);
      if (DeviceCount > 1) {
        StrCatS (PromptString, Size / sizeof(CHAR16), L",...");
      }
      StrCatS (PromptString, Size / sizeof(CHAR16), L")");
    }
  } else {
    PromptString = AllocateCopyPool (StrSize (Option->Description), Option->Description);
  }

  if (VarData != NULL) {
    FreePool (VarData);
  }

  return PromptString;
}

/**
  Update boot option list into HII form.
**/
VOID
UpdateBootOptionListIntoHiiForm (
  VOID
  )
{
  H2O_BDS_LOAD_OPTION         *Option;
  LIST_ENTRY                  *Link;
  EFI_STRING_ID               Token;
  EFI_HII_HANDLE              HiiHandle;
  VOID                        *StartOpCodeHandle;
  VOID                        *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL          *StartLabel;
  EFI_IFR_GUID_LABEL          *EndLabel;
  UINT16                      DeviceType;
  BOOLEAN                     IsLegacyOption;
  BOOLEAN                     NeedEndOp;
  BOOLEAN                     PreviousOptionIsLegacy;
  UINTN                       Index;
  HARDWARE_BOOT_DEVICE_INFO   *HwBootDeviceInfo;
  UINTN                       HwBootDeviceCount;
  CHAR16                      *PromptString;
  UINT16                      OptionDeviceType;
  UINT16                      KeyInput;
  UINTN                       StrIdIndex;
  STATIC EFI_STRING_ID        *StrIdRecordList = NULL;
  STATIC UINTN                StrIdRecordCount = 0;

  DeviceType = (UINT16) -1;
  OptionDeviceType = (UINT16) -1;
  HiiHandle = gBootManagerPrivate.HiiHandle;

  //
  // Create list to record allocated string IDs and reuse these to avoid exhausting string ID.
  //
  for (Link = GetFirstNode (mBootOptionsList), StrIdIndex = 0; !IsNull (mBootOptionsList, Link); Link = GetNextNode (mBootOptionsList, Link), StrIdIndex++);
  if (StrIdIndex > StrIdRecordCount) {
    StrIdRecordList = ReallocatePool (StrIdRecordCount * sizeof(EFI_STRING_ID), StrIdIndex * sizeof(EFI_STRING_ID), StrIdRecordList);
    if (StrIdRecordList == NULL) {
      StrIdRecordCount = 0;
      return;
    }
    StrIdRecordCount = StrIdIndex;
  }

  BdsLibGetAllHwBootDeviceInfo (&HwBootDeviceCount, &HwBootDeviceInfo);

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
  StartLabel->Number       = LABEL_BOOT_OPTION;

  //
  // Create Hii Extend Label OpCode as the end opcode
  //
  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = LABEL_BOOT_OPTION_END;

  KeyInput = 0;
  NeedEndOp = FALSE;
  PreviousOptionIsLegacy = TRUE;
  for (Link = GetFirstNode (mBootOptionsList), StrIdIndex = 0; !IsNull (mBootOptionsList, Link); Link = GetNextNode (mBootOptionsList, Link), StrIdIndex++) {
    Option = BDS_OPTION_FROM_LINK (Link);
    //
    // At this stage we are creating a menu entry, thus the Keys are reproduceable
    //
    KeyInput++;

    //
    // Don't display the hidden/inactive boot option
    //
    if (((Option->Attributes & LOAD_OPTION_HIDDEN) != 0) || ((Option->Attributes & LOAD_OPTION_ACTIVE) == 0)) {
      continue;
    }

    //
    // Group the legacy boot option in the sub title created dynamically
    //
    IsLegacyOption = IsLegacyBootOption (Option);
    if (IsLegacyOption) {
      if (Option->LoadOptionalDataSize != 0 &&
          IsUsbDevice ((BBS_TABLE *) Option->LoadOptionalData)) {
        OptionDeviceType = BBS_USB;
      } else {
        OptionDeviceType = ((BBS_BBS_DEVICE_PATH *) Option->DevicePath)->DeviceType;
      }
      if (NeedEndOp && (IsLegacyOption != PreviousOptionIsLegacy || DeviceType != OptionDeviceType)) {
        HiiCreateEndOpCode (StartOpCodeHandle);
      }
      if  (DeviceType != OptionDeviceType) {
        DeviceType = OptionDeviceType;
        Index = DeviceType & 0xF;
        Token = mDeviceTypeStrId[MIN (Index, sizeof (mDeviceTypeStrId) / sizeof (mDeviceTypeStrId[0]) - 1)];
        HiiCreateSubTitleOpCode (StartOpCodeHandle, Token, 0, 0, 1);
        NeedEndOp = TRUE;
      }
    } else if (IsLegacyOption != PreviousOptionIsLegacy) {
      if (NeedEndOp) {
        HiiCreateEndOpCode (StartOpCodeHandle);
      }

      HiiCreateSubTitleOpCode (StartOpCodeHandle, STRING_TOKEN (STR_DEVICE_TYPE_EFI), 0, 0, 1);
      NeedEndOp = TRUE;
    }

    ASSERT (Option->Description != NULL);

    PromptString = GetPromptString (
                     Option,
                     HwBootDeviceInfo,
                     HwBootDeviceCount
                     );
    if (PromptString != NULL) {
      StrIdRecordList[StrIdIndex] = HiiSetString (HiiHandle, StrIdRecordList[StrIdIndex], PromptString, NULL);
      FreePool (PromptString);
    } else {
      StrIdRecordList[StrIdIndex] = HiiSetString (HiiHandle, StrIdRecordList[StrIdIndex], Option->Description, NULL);
    }

    HiiCreateActionOpCode (
      StartOpCodeHandle,
      KeyInput,
      StrIdRecordList[StrIdIndex],
      0,
      EFI_IFR_FLAG_CALLBACK,
      0
      );

    PreviousOptionIsLegacy = IsLegacyOption;
  }

  if (NeedEndOp) {
    HiiCreateEndOpCode (StartOpCodeHandle);
  }

  HiiUpdateForm (
    HiiHandle,
    &gBootManagerFormSetGuid,
    BOOT_MANAGER_FORM_ID,
    StartOpCodeHandle,
    EndOpCodeHandle
    );

  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (EndOpCodeHandle);

  if (HwBootDeviceCount != 0 && HwBootDeviceInfo != NULL) {
    for (Index = 0; Index < HwBootDeviceCount; Index++) {
      FreePool (HwBootDeviceInfo[Index].HwDeviceName);
    }
    FreePool (HwBootDeviceInfo);
  }
}

/**
 Update boot option list. If boot option list is changed, update HII form.

 @retval EFI_SUCCESS        Boot option list is changed and updated.
 @retval EFI_ABORTED        Boot option list is not changed.
**/
EFI_STATUS
UpdateBootOptionList (
  VOID
  )
{
  BdsLibEnumerateAllBootOption (FALSE, NULL);
  gBdsServices->GetBootList (gBdsServices, &mBootOptionsList);

  if (CompareBootOptionList (&mBackupBootOptionsList, mBootOptionsList)) {
    return EFI_ABORTED;
  }

  FreeBootOptionList (&mBackupBootOptionsList);
  CopyBootOptionList (&mBackupBootOptionsList, mBootOptionsList);

  UpdateBootOptionListIntoHiiForm ();
  return EFI_SUCCESS;
}

/**
  Event function to send hot key callback notify to update boot option list.

  @param[in] Event                  The Event this notify function registered to.
  @param[in] Context                Pointer to the context data registerd to the Event.
**/
STATIC
VOID
EFIAPI
UpdateBootOptionListEvtFunc (
  IN EFI_EVENT                Event,
  IN VOID                     *Context
  )
{
  //
  // Prevent from updating mBootOptionsList causes gOption is invalid, move update list process in callback function.
  //
  SendHotKeyNotify (HotKeyCallback, BOOT_MANAGER_UPDATE_BOOT_LIST_QUESTION_ID);
}

/**
  Perform necessary process before call boot manager.
**/
VOID
InternalCallBootManagerBefore (
  VOID
  )
{
  EFI_STATUS                  Status;

  gOption    = NULL;

  //
  // Connect all prior to entering the platform setup menu.
  //
  if (!gConnectAllHappened) {
    BdsLibConnectAllDriversToAllControllers ();
    gConnectAllHappened = TRUE;
  }

  BdsLibEnumerateAllBootOption (FALSE, NULL);
  Status = gBdsServices->GetBootList (gBdsServices, &mBootOptionsList);
  ASSERT_EFI_ERROR (Status);


  CopyBootOptionList (&mBackupBootOptionsList, mBootOptionsList);
  UpdateBootOptionListIntoHiiForm ();

  if (FeaturePcdGet (PcdH2OBootMgrAutoDetectRemovableBootDevice) && FeaturePcdGet (PcdH2OFormBrowserSupported)) {
    Status = gBS->CreateEvent (
                    EVT_TIMER | EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    UpdateBootOptionListEvtFunc,
                    NULL,
                    &mUpdateBootOptionListEvt
                    );
    if (!EFI_ERROR(Status)) {
      Status = gBS->SetTimer (mUpdateBootOptionListEvt, TimerPeriodic, EFI_TIMER_PERIOD_SECONDS(1));
    } else {
      mUpdateBootOptionListEvt = NULL;
    }
  }
}

/**
  Perform necessary process after called boot manager.
**/
VOID
InternalCallBootManagerAfter (
  VOID
  )
{
  if (mUpdateBootOptionListEvt != NULL) {
    gBS->CloseEvent (mUpdateBootOptionListEvt);
    mUpdateBootOptionListEvt = NULL;
  }
  FreeBootOptionList (&mBackupBootOptionsList);
}

/**
  This function invokes Boot Manager. If all devices have not a chance to be connected,
  the connect all will be triggered. It then enumerate all boot options. If
  a boot option from the Boot Manager page is selected, Boot Manager will boot
  from this boot option.

**/
VOID
InternalCallBootManager (
  VOID
  )
{
  EFI_STATUS                  Status;
  CHAR16                      *ExitData;
  UINTN                       ExitDataSize;
  EFI_HII_HANDLE              HiiHandle;
  EFI_BROWSER_ACTION_REQUEST  ActionRequest;

  InternalCallBootManagerBefore ();

  mBootMgrSendFormBySelf = TRUE;
  HiiHandle = gBootManagerPrivate.HiiHandle;
  ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  Status = gBootManagerFormBrowser2->SendForm (
                                       gBootManagerFormBrowser2,
                                       &HiiHandle,
                                       1,
                                       &gBootManagerFormSetGuid,
                                       0,
                                       NULL,
                                       &ActionRequest
                                       );
  if (ActionRequest == EFI_BROWSER_ACTION_REQUEST_RESET) {
    EnableResetRequired ();
  }
  mBootMgrSendFormBySelf = FALSE;

  InternalCallBootManagerAfter ();
  if (gOption == NULL) {
    return ;
  }

  //
  // Will leave browser, check any reset required change is applied? if yes, reset system
  //
  SetupResetReminder ();

  //
  // Restore to original mode before launching boot option.
  //

  //
  // parse the selected option
  //
  Status = gBdsServices->LaunchLoadOption (gBdsServices, gOption, &ExitDataSize, &ExitData);

  if (!EFI_ERROR (Status)) {
    BdsLibBootSuccess (gOption);
  } else {
    BdsLibBootFailed (gOption, Status, ExitData, ExitDataSize);
  }
}

/**
  Enter boot manager.

  @param[in]      This          Protocol instance pointer.
  @param[in]      FilePath      The device specific path of the file to load.
  @param[in]      BootPolicy    If TRUE, indicates that the request originates from the boot manager is attempting to
                                load FilePath as a boot selection. If FALSE, then FilePath must match as exact file to be loaded.
  @param[in, out] BufferSize    On input the size of Buffer in bytes. On output with a return code of EFI_SUCCESS,
                                the amount of data transferred to Buffer. On output with a return code of EFI_BUFFER_TOO_SMALL,
                                the size of Buffer required to retrieve the requested file.
  @param[in]  Buffer            The memory buffer to transfer the file to. IF Buffer is NULL, then no the size of the
                                requested file is returned in BufferSize.

  @retval EFI_NOT_FOUND         Always return not found.
**/
EFI_STATUS
EFIAPI
BootManagerLoadFile (
  IN EFI_LOAD_FILE_PROTOCOL           *This,
  IN EFI_DEVICE_PATH_PROTOCOL         *FilePath,
  IN BOOLEAN                          BootPolicy,
  IN OUT UINTN                        *BufferSize,
  IN VOID                             *Buffer OPTIONAL
  )
{
  if (gBootManagerPrivate.HiiHandle != NULL) {
    while (TRUE) {
      InternalCallBootManager ();
      if (gOption == NULL) {
        //
        // User presses ESC key to exit boot manager.
        //
        break;
      }
      if (PcdGetBool (PcdH2OBootMgrExitAfterLaunchOption)) {
        break;
      }
    }
  }

  return EFI_NOT_FOUND;
}

/**
 Initialize HII data.

 @param[in] Event          A pointer to the Event that triggered the callback.
 @param[in] Handle         Checkpoint handle.
**/
VOID
EFIAPI
BootManagerCpInitNotifyFunc (
  IN EFI_EVENT                        Event,
  IN H2O_CP_HANDLE                    Handle
  )
{
  H2OCpUnregisterHandler (Handle);
  InitializeBootManager ();
}

/**
 Register H2OBdsCpInit notification to initialize HII data.
 Install the instance of the EFI_LOAD_FILE_PROTOCOL and EFI_DEVICE_PATH_PROTOCOL on new handle.

 @param[in] ImageHandle    The firmware allocated handle for the EFI image.
 @param[in] SystemTable    A pointer to the EFI System Table.

 @retval EFI_SUCCESS       Install protocol successfully.
 @retval other             Fail to install protocol instance.
**/
EFI_STATUS
EFIAPI
BootManagerEntry (
  IN EFI_HANDLE                       ImageHandle,
  IN EFI_SYSTEM_TABLE                 *SystemTable
  )
{
  H2O_CP_HANDLE                       CpHandle;
  EFI_STATUS                          Status;
  EFI_HANDLE                          DriverHandle;

  if (FeaturePcdGet (PcdH2OBdsCpInitSupported)) {
    Status = H2OCpRegisterHandler (
               &gH2OBdsCpInitGuid,
               BootManagerCpInitNotifyFunc,
               H2O_CP_MEDIUM,
               &CpHandle
               );
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Checkpoint Register Fail: %g (%r)\n", &gH2OBdsCpInitGuid, Status));
        return Status;
      }
      DEBUG ((DEBUG_INFO, "Checkpoint Registered: %g (%r)\n", &gH2OBdsCpInitGuid, Status));
  }

  DriverHandle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &gH2OBootManagerDevicePath,
                  &gEfiLoadFileProtocolGuid,
                  &mBootManagerLoadFile,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
