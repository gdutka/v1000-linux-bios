/** @file
  BdsDxe

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

/**

  This module produce main entry for BDS phase - BdsEntry.
  When this module was dispatched by DxeCore, gEfiBdsArchProtocolGuid will be installed which contains interface of BdsEntry.
  After DxeCore finish DXE phase, gEfiBdsArchProtocolGuid->BdsEntry will be invoked to enter BDS phase.

Copyright (c) 2004 - 2012, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Bds.h"
#include "String.h"
#include <Protocol/SetupUtilityApplication.h>
#include <Protocol/DevicePathToText.h>

typedef struct _LOAD_OPTION_ATTRIBUTE_STRING_INFO {
  UINT32                    Mask;
  UINT32                    Value;
  CHAR16                    *String;
} LOAD_OPTION_ATTRIBUTE_STRING_INFO;
LOAD_OPTION_ATTRIBUTE_STRING_INFO  mLoadOptionAttrStrInfo[] = {
                                     {LOAD_OPTION_CATEGORY       , LOAD_OPTION_CATEGORY_BOOT  , L"CATEGORY_BOOT"},
                                     {LOAD_OPTION_CATEGORY       , LOAD_OPTION_CATEGORY_APP   , L"CATEGORY_APP"},
                                     {LOAD_OPTION_ACTIVE         , LOAD_OPTION_ACTIVE         , L"ACTIVE"},
                                     {LOAD_OPTION_FORCE_RECONNECT, LOAD_OPTION_FORCE_RECONNECT, L"FORCE_RECONNECT"},
                                     {LOAD_OPTION_HIDDEN         , LOAD_OPTION_HIDDEN         , L"HIDDEN"}
                                     };

#ifndef EFI_OS_INDICATIONS_START_OS_RECOVERY
#define EFI_OS_INDICATIONS_START_OS_RECOVERY 0x0000000000000020
#endif
#ifndef EFI_OS_INDICATIONS_START_PLATFORM_RECOVERY
#define EFI_OS_INDICATIONS_START_PLATFORM_RECOVERY 0x0000000000000040
#endif

#define NO_BOOT_DEVICE           0
#define NO_ACTIVE_BOOT_OPTION    1


//
// The data structure for performance data in ACPI memory.
//
#define PERFORMANCE_SIGNATURE   SIGNATURE_32 ('P', 'e', 'r', 'f')
#define PERF_TOKEN_SIZE         28
#define PERF_TOKEN_LENGTH       (PERF_TOKEN_SIZE - 1)
#define PERF_PEI_ENTRY_MAX_NUM  50
#define PERF_DATA_MAX_LENGTH    0x4000

typedef struct {
  CHAR8   Token[PERF_TOKEN_SIZE];
  UINT32  Duration;
} PERF_DATA;

typedef struct {
  UINT64        BootToOs;
  UINT64        S3Resume;
  UINT32        S3EntryNum;
  PERF_DATA     S3Entry[PERF_PEI_ENTRY_MAX_NUM];
  UINT64        CpuFreq;
  UINT64        BDSRaw;
  UINT32        Count;
  UINT32        Signiture;
} PERF_HEADER;

///
/// BDS arch protocol instance initial value.
///
/// Note: Current BDS not directly get the BootMode, DefaultBoot,
/// TimeoutDefault, MemoryTestLevel value from the BDS arch protocol.
/// Please refer to the library useage of BdsLibGetBootMode, BdsLibGetTimeout
/// and PlatformBdsDiagnostics in BdsPlatform.c
///

EFI_HANDLE             gBdsHandle = NULL;
UINT16                 *mBootNext = NULL;

EFI_BDS_ARCH_PROTOCOL  gBds = {
  BdsEntry
};

H2O_BDS_CP_EXIT_BOOT_SERVICES_BEFORE_DATA          mBdsExitBootServicesBeforeData;
EFI_EXIT_BOOT_SERVICES                             mOrginalExitBootServices;
STATIC BOOLEAN                                     mExitBootServicesHasbeenCalled;

STATIC
UINTN
GetMapKey (
  VOID
  )
{
  EFI_MEMORY_DESCRIPTOR     *EfiMemoryMap;
  UINTN                     EfiMemoryMapSize;
  UINTN                     EfiMapKey;
  UINTN                     EfiDescriptorSize;
  UINT32                    EfiDescriptorVersion;

  EfiMemoryMapSize = 0;
  EfiMemoryMap     = NULL;
  gBS->GetMemoryMap (
         &EfiMemoryMapSize,
         EfiMemoryMap,
         &EfiMapKey,
         &EfiDescriptorSize,
         &EfiDescriptorVersion
         );
  return EfiMapKey;


}

EFI_STATUS
EFIAPI
ExitBootServicesHookFunction (
  IN EFI_HANDLE   ImageHandle,
  IN UINTN        MapKey
  )
{
  UINTN      CurrentMapKey;

  if (mExitBootServicesHasbeenCalled) {
     return mOrginalExitBootServices (ImageHandle, MapKey);
  }
  CurrentMapKey = GetMapKey ();
  mExitBootServicesHasbeenCalled = TRUE;
  mBdsExitBootServicesBeforeData.Size   = sizeof (H2O_BDS_CP_EXIT_BOOT_SERVICES_BEFORE_DATA);
  mBdsExitBootServicesBeforeData.Status = H2O_CP_TASK_NORMAL;
  DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpExitBootServicesBeforeGuid));
  H2OCpTrigger (&gH2OBdsCpExitBootServicesBeforeGuid, &mBdsExitBootServicesBeforeData);
  DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", mBdsExitBootServicesBeforeData.Status));
  return mOrginalExitBootServices (ImageHandle, CurrentMapKey == MapKey ? GetMapKey () : MapKey);
}

STATIC
UINT8
CalculateAddParity (
  UINT8         Data
  )
{
  UINTN         Index;
  UINT8         TempData;
  BOOLEAN       ClearParityBit;

  TempData = Data;

  ClearParityBit = FALSE;
  for (Index = 0; Index < 7; Index++) {
    ClearParityBit = (BOOLEAN) ((TempData & 1) ^ ClearParityBit);
    TempData >>= 1;
  }
  if (ClearParityBit) {
    Data &= 0x7f;
  } else {
    Data |= 0x80;
  }

  return Data;
}


EFI_STATUS
SetSimpBootFlag (
  VOID
  )
{
  UINT8     Data8;
  UINT8     TempData8;

  Data8 = ReadCmos8 (SimpleBootFlag);
  //
  // check BOOT bit. If BOOT bit enable, enable DIAG bit. otherwise, disable DIAG bit.
  // then enable BOOT bit      BOOT bit = Bit1  DIAG bit = bit2 SUPPERSSBOOTDISPLAY bit = bit3
  //
  TempData8 = Data8 & 0x02;
  if (TempData8 == 0) {
    Data8 &= 0xFB;
  } else {
    Data8 |= 0x04;
  }
  Data8 |= 0x02;

  //
  // disable SUPPERSSBOOTDISPLAY Flag
  //
  Data8 &= 0xF7;
  Data8 = CalculateAddParity (Data8);

  WriteCmos8 (SimpleBootFlag, Data8);

  return EFI_SUCCESS;
}


VOID
EFIAPI
LoadDefaultCheck (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINTN                           VariableSize;
  BOOLEAN                         LoadDefault;

  LoadDefault     = PcdGetBool (PcdLoadDefaultSetupMenu);

  //
  // OemServices
  //
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcLoadDefaultSetupMenu \n"));
  Status = OemSvcLoadDefaultSetupMenu (&LoadDefault);
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcLoadDefaultSetupMenu Status: %r\n", Status));

  if (!EFI_ERROR (Status) || (!LoadDefault)) {
    return;
  }

  if (LoadDefault) {
    DxeCsSvcLoadDefaultSetupMenu ();
    //
    // Is "Setup" variable existence?
    //
    VariableSize = 0;
    Status = gRT->GetVariable (
                    SETUP_VARIABLE_NAME,
                    &gSystemConfigurationGuid,
                    NULL,
                    &VariableSize,
                    NULL
                    );
    if (Status != EFI_BUFFER_TOO_SMALL) {
      //
      // No Setup variable found
      // Setup menu is default setting.
      //
      return;
    }

    //
    // Delete "Setup" variable. It will load default in GetSystemConfigurationVar() of SetupUtility.c
    //
    VariableSize = 0;
    Status = gRT->SetVariable (
                    SETUP_VARIABLE_NAME,
                    &gSystemConfigurationGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    VariableSize,
                    NULL
                    );
    if (EFI_ERROR (Status)) {
      return;
    }
    //
    //Reset start system.
    //
    gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
  }

  return;
}

/**
 Display TimeOut invalid message dialog.

 @param[in] Timeout               Current timeout variable value.
**/
STATIC
VOID
DisplayTimeOutInvalidDialog (
  IN UINT16                       Timeout
  )
{
  EFI_STATUS                      Status;
  H2O_DIALOG_PROTOCOL             *H2ODialog;
  CHAR16                          *ChangeStr;
  CHAR16                          *String;
  UINTN                           StringSize;
  EFI_INPUT_KEY                   Key;

  Status = gBS->LocateProtocol (
                  &gH2ODialogProtocolGuid,
                  NULL,
                  (VOID **) &H2ODialog
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return;
  }

  ChangeStr  = GetStringById (STRING_TOKEN (STR_TIMEOUT_CHANGE));
  if (ChangeStr == NULL) {
    return;
  }
  StringSize = 0x100 + StrSize (ChangeStr);
  String     = AllocateZeroPool (StringSize);
  if (String == NULL) {
    FreePool (ChangeStr);
    return;
  }
  UnicodeSPrint (String, StringSize, ChangeStr, Timeout, PcdGet16 (PcdPlatformBootTimeOutMax), PcdGet16 (PcdPlatformBootTimeOut));
  DisableQuietBoot ();
  H2ODialog->ConfirmDialog (
               DlgOk,
               FALSE,
               0,
               NULL,
               &Key,
               String
               );
  FreePool (String);
  FreePool (ChangeStr);
}

/**
  Check timeout is whether larger than PcdPlatformBootTimeOutMax.

**/
STATIC
VOID
ProcessTimeoutChange (
  VOID
  )
{
  UINT16                          Timeout;
  UINTN                           Size;
  EFI_STATUS                      Status;

  Size = sizeof (UINT16);
  Status = gRT->GetVariable (
                  L"Timeout",
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &Size,
                  &Timeout
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return;
  }
  if (Timeout <= PcdGet16 (PcdPlatformBootTimeOutMax)) {
    return;
  }

  if (PcdGetBool(PcdPlatformBootTimeOutInvalidDialogSupported)) {
    DisplayTimeOutInvalidDialog (Timeout);
  }

  Timeout = PcdGet16 (PcdPlatformBootTimeOut);
  Status = gRT->SetVariable (
                  L"Timeout",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  sizeof (UINT16),
                  &Timeout
                  );
}

/**
  Callback function to check Boot type is whether changed by other tools

  @param Event    Event whose notification function is being invoked.
  @param Context  Pointer to the notification function's context.

**/
VOID
EFIAPI
ProcessChangeCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                  Status;
  VOID                        *BdsDiagnostics;

  Status = gBS->LocateProtocol (
                  &gEfiStartOfBdsDiagnosticsProtocolGuid,
                  NULL,
                  &BdsDiagnostics
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  gBS->CloseEvent (Event);
  ProcessTimeoutChange ();
}

/**
  Callback function to restore physical boot order and enumerate all boot options before enter SCU.

  @param Event    Event whose notification function is being invoked.
  @param Context  Pointer to the notification function's context.
**/
VOID
EFIAPI
SetupUtilityAppNotifyFn (
  IN EFI_EVENT                             Event,
  IN VOID                                  *Context
  )
{
  EFI_STATUS                               Status;
  EFI_SETUP_UTILITY_APPLICATION_PROTOCOL   *SetupUtilityApp;

  Status = gBS->LocateProtocol (
                  &gEfiSetupUtilityApplicationProtocolGuid,
                  NULL,
                  (VOID **) &SetupUtilityApp
                  );
  if (!EFI_ERROR(Status) && SetupUtilityApp->VfrDriverState == InitializeSetupUtility) {
    if (FeaturePcdGet (PcdAutoCreateDummyBootOption)) {
      SyncBootOrder ();
      BdsLibRestoreBootOrderFromPhysicalBootOrder ();
    }
    BdsLibEnumerateAllBootOption (FALSE, NULL);
  }
}

/**

  Install Boot Device Selection Protocol

  @param ImageHandle     The image handle.
  @param SystemTable     The system table.

  @retval  EFI_SUCEESS  BDS has finished initializing.
                        Return the dispatcher and recall BDS.Entry
  @retval  Other        Return status from AllocatePool() or gBS->InstallProtocolInterface

**/
EFI_STATUS
EFIAPI
BdsInitialize (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
{
  EFI_STATUS  Status;
  VOID        *Registration;

  LoadDefaultCheck ();
  BdsLibDeleteInvalidBootOptions ();
  EfiCreateProtocolNotifyEvent (
    &gEfiStartOfBdsDiagnosticsProtocolGuid,
    TPL_CALLBACK,
    ProcessChangeCallback,
    NULL,
    &Registration
    );
  EfiCreateProtocolNotifyEvent (
    &gEfiSetupUtilityApplicationProtocolGuid,
    TPL_CALLBACK,
    SetupUtilityAppNotifyFn,
    NULL,
    &Registration
    );
  //
  // Install protocol interface
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &gBdsHandle,
                  &gEfiBdsArchProtocolGuid,
                  &gBds,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  mBdsOrgGetMemoryMap = gBS->GetMemoryMap;
  return Status;
}

/**

  Popup a message for user to indicate no mapping boot option to boot.

  @param OptionNum          Input boot option number.

**/
VOID
CreateNoMappingBootOptionPopUp (
  IN  UINT16      OptionNum
  )
{
  H2O_DIALOG_PROTOCOL                   *H2ODialog;
  EFI_INPUT_KEY                         Key;
  EFI_STATUS                            Status;
  CHAR16                                *String;

  if (!BdsLibIsDummyBootOption (OptionNum)) {
    return;
  }

  Status = gBS->LocateProtocol (
                  &gH2ODialogProtocolGuid,
                  NULL,
                  (VOID **) &H2ODialog
                  );
  ASSERT_EFI_ERROR (Status);

  DisableQuietBoot ();
  String = NULL;
  if (OptionNum == DummyUsbBootOptionNum) {
    String = GetStringById (STRING_TOKEN (STR_USB_OPTION_INEXISTENCE));
  } else if (OptionNum == DummyCDBootOptionNum){
    String = GetStringById (STRING_TOKEN (STR_CD_OPTION_INEXISTENCE));
  } else if (OptionNum == DummyNetwokrBootOptionNum){
    String = GetStringById (STRING_TOKEN (STR_NETWORK_OPTION_INEXISTENCE));
  }
  ASSERT (String != NULL);
  if (String == NULL) {
    return;
  }
  H2ODialog->ConfirmDialog (
                    DlgOk,
                    FALSE,
                    0,
                    NULL,
                    &Key,
                    String
                    );
  FreePool (String);

  return;
}


/**

  Popup a option list for user to select which mapping boot option want to boot.

  @param OptionNum            Input boot option number.
  @param OptionCount          The total number of boot option in option order.
  @param OptionOrder          Pointer to option order.
  @param SelectedOpitonNum    The option number which user selected.

  @retval  EFI_SUCEESS            User selects a specific USB boot option successful.
  @retval  EFI_INVALID_PARAMETER  OptionCount is 0, OptionOrder is NULL, SelectedOpitonNum is NULL or OptinNum isn't
                                  a dummy boot option.
  @retval  EFI_NOT_FOUND          Cannot find corresponding title string for specific boot option type.
  @retval  EFI_ABORTED            User presses ESC key to skip boot option selection menu.

**/
EFI_STATUS
SelectMappingBootOption (
  IN       UINT16      OptionNum,
  IN       UINTN       OptionCount,
  IN       UINT16      *OptionOrder,
  OUT      UINT16      *SelectedOpitonNum
  )
{
  H2O_DIALOG_PROTOCOL                   *H2ODialog;
  EFI_STATUS                            Status;
  CHAR16                                **DescriptionOrder;
  UINT32                                Index;
  CHAR16                                *TitleString;
  BOOLEAN                               BootOptionSelected;
  EFI_INPUT_KEY                         Key;
  UINTN                                 MaxStrLen;

  if (OptionCount == 0 || OptionOrder == NULL || SelectedOpitonNum == NULL || !BdsLibIsDummyBootOption (OptionNum)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (
                  &gH2ODialogProtocolGuid,
                  NULL,
                  (VOID **) &H2ODialog
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  TitleString = NULL;
  if (OptionNum == DummyUsbBootOptionNum) {
    TitleString = GetStringById (STRING_TOKEN (STR_SELECT_USB_OPTION));
  } else if (OptionNum == DummyCDBootOptionNum) {
    TitleString = GetStringById (STRING_TOKEN (STR_SELECT_CD_OPTION));
  } else if (OptionNum == DummyNetwokrBootOptionNum) {
    TitleString = GetStringById (STRING_TOKEN (STR_SELECT_NETWORK_OPTION));
  }
  if (TitleString == NULL) {
    return EFI_NOT_FOUND;
  }
  DescriptionOrder = AllocateZeroPool (OptionCount * sizeof (CHAR16 *));
  if (DescriptionOrder == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  MaxStrLen = 0;
  for (Index = 0; Index < OptionCount; Index++) {
    DescriptionOrder[Index] = BdsLibGetDescriptionFromBootOption (OptionOrder[Index]);
    MaxStrLen = StrLen (DescriptionOrder[Index]) > MaxStrLen ? StrLen (DescriptionOrder[Index]) : MaxStrLen;
  }

  DisableQuietBoot ();
  BootOptionSelected = FALSE;

  H2ODialog->OneOfOptionDialog (
                    (UINT32) OptionCount,
                    FALSE,
                    NULL,
                    &Key,
                    (UINT32) MaxStrLen,
                    TitleString,
                    &Index,
                    DescriptionOrder,
                    0
                    );
  if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
    BootOptionSelected = TRUE;
    *SelectedOpitonNum = OptionOrder[Index];
  }

  FreePool (TitleString);
  for (Index = 0; Index < OptionCount; Index++) {
    FreePool (DescriptionOrder[Index]);
  }

  Status = BootOptionSelected ? EFI_SUCCESS : EFI_ABORTED;

  return Status;
}

/**
 Check if the BDS hot key is support or not.

 @retval TRUE   The BDS hot key is support.
 @retval FALSE  The BDS hot key is not support.
**/
BOOLEAN
IsBdsHotKeySupport (
  VOID
  )
{
  EFI_STATUS                 Status;
  UINTN                      Size;
  IMAGE_INFO                 ImageInfo;

  if (FeaturePcdGet(PcdSecureFlashSupported)) {
    Size = sizeof (IMAGE_INFO);
    Status = gRT->GetVariable (
                    L"SecureFlashInfo",
                    &gSecureFlashInfoGuid,
                    NULL,
                    &Size,
                    &ImageInfo
                    );
    if ((Status == EFI_SUCCESS) && (ImageInfo.FlashMode)) {
      return FALSE;
    }
  }

  return TRUE;
}

/**
 According to boot option number to check this boot option is whether a Legacy boot option.

 @param[in] OptionNum    The boot option number.

 @retval TRUE            This is a Legacy boot option.
 @retval FALSE           This isn't a Legacy boot option.
**/
BOOLEAN
IsLegacyBootOption (
  IN  UINT16     OptionNum
  )
{
  EFI_DEVICE_PATH_PROTOCOL   *DevicePath;
  BOOLEAN                    IsLegacyBoot;
  UINT32                     DevicePathType;

  if (OptionNum == 0xFF) {
    return FALSE;
  }

  DevicePath = BdsLibGetDevicePathFromBootOption (OptionNum);
  if (DevicePath == NULL) {
    return FALSE;
  }

  IsLegacyBoot = FALSE;
  DevicePathType = BdsLibGetBootTypeFromDevicePath (DevicePath);
  if (DevicePathType == BDS_LEGACY_BBS_BOOT) {
    IsLegacyBoot = TRUE;
  }

  FreePool (DevicePath);

  return IsLegacyBoot;
}

/**
  Internal function to initialize H2O_BDS_CP_NO_BOOT_DEVICE_DATA data and trigger
  gH2OBdsCpNoBootDeviceGuid checkpoint.

  @return The Status in H2O_BDS_CP_NO_BOOT_DEVICE_DATA data.
**/
STATIC
UINT32
TriggerCpNoBootDevice (
  VOID
  )
{
  H2O_BDS_CP_NO_BOOT_DEVICE_DATA        BdsNoBootDeviceData;
  EFI_STATUS                            Status;


  BdsNoBootDeviceData.Size   = sizeof (H2O_BDS_CP_NO_BOOT_DEVICE_DATA);
  BdsNoBootDeviceData.Status = H2O_BDS_TASK_NORMAL;
  DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpNoBootDeviceGuid));
  Status = H2OCpTrigger (&gH2OBdsCpNoBootDeviceGuid, &BdsNoBootDeviceData);
  DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", BdsNoBootDeviceData.Status));
  return BdsNoBootDeviceData.Status;
}

/**
  Launch boot failed application which is defined by PcdH2OBdsBootFailApp.

  @retval EFI_SUCCESS                  Launch boot failed application success.
  @retval EFI_NOT_FOUND                String pointer is NULL.
**/
STATIC
EFI_STATUS
LaunchBootFailApp (
  VOID
  )
{
  EFI_DEVICE_PATH_PROTOCOL             *DevicePath;

  DevicePath = GetDevicePathByAsciiStr ((CHAR8 *) PcdGetPtr (PcdH2OBdsBootFailApp));
  if (DevicePath == NULL) {
    return EFI_NOT_FOUND;
  }

  LaunchBootOptionByDevicePath (DevicePath);
  FreePool (DevicePath);
  return EFI_SUCCESS;
}

/**
  Show boot fail message. If auto failover is disable, it will go into firmware UI.

  @param  AutoFailover           Auto failover polciy
  @param  Message                Boot fail message

  @retval EFI_SUCCESS            Perform auto failover policy success.
  @retval EFI_INVALID_PARAMETER  String pointer is NULL.
  @retval Other                  Locate protocol fail or pop message fail.

**/
EFI_STATUS
AutoFailoverPolicyBehavior (
  IN BOOLEAN    AutoFailover,
  IN CHAR16     *Message
  )
{
  EFI_STATUS                        Status;
  EFI_INPUT_KEY                     Key;
  H2O_DIALOG_PROTOCOL               *H2ODialog;

  if (Message == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (
                  &gH2ODialogProtocolGuid,
                  NULL,
                  (VOID **)&H2ODialog
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  DisableQuietBoot ();

  Status = H2ODialog->ConfirmDialog (2, FALSE, 0, NULL, &Key, Message);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  gST->ConOut->ClearScreen (gST->ConOut);

  //
  // If auto failover is disable, go into firmware UI.
  //
  if (!AutoFailover) {
    BdsLibConnectAll ();
    BdsLibEnumerateAllBootOption (TRUE, NULL);
    LaunchBootFailApp ();
  }

  return EFI_SUCCESS;
}

/**
  Perform no boot device operation (show message and wait for user to restart system)

  @param[in]  NoBootDeviceType     What kind of no boot device
**/
VOID
BdsNoBootDevice (
  IN  UINT8      NoBootDeviceType
  )
{
  EFI_STATUS                         Status;
  EFI_INPUT_KEY                      Key;
  CHAR16                             *BootErrorString;
  volatile UINTN                     Index;
  //
  // PostCode = 0xF9, No Boot Device
  //
  POST_CODE (POST_BDS_NO_BOOT_DEVICE);
  if (FeaturePcdGet (PcdH2OBdsCpNoBootDeviceSupported)) {
    if (TriggerCpNoBootDevice () == H2O_BDS_TASK_SKIP) {
      return;
    }
  }
  if (NoBootDeviceType == NO_BOOT_DEVICE) {
    BootErrorString = GetStringById (STRING_TOKEN (STR_AUTO_FAILOVER_NO_BOOT_DEVICE));
  } else {
    BootErrorString = GetStringById (STRING_TOKEN (STR_AUTO_FAILOVER_NO_ACTIVE_BOOT_OPTION));
  }
  if (BootErrorString != NULL) {
    AutoFailoverPolicyBehavior (FALSE, BootErrorString);
    FreePool(BootErrorString);
  }

  DisableQuietBoot ();

  BootErrorString = GetStringById (STRING_TOKEN (STR_BOOT_DEVICE_ERROR_MESSAGE));
  if (BootErrorString == NULL || StrLen (BootErrorString) == 0) {
    BootErrorString = L"No bootable device -- Please restart system\n";
  }

  gST->ConOut->EnableCursor (gST->ConOut, TRUE);
  gST->ConOut->ClearScreen(gST->ConOut);
  gST->ConOut->SetAttribute (gST->ConOut, EFI_WHITE);
  Print (BootErrorString);

  for (Index = 0; Index == 0;) {
    Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
    if (!EFI_ERROR (Status)) {
      Print (BootErrorString);
    }
  }
}

/**
  Creates a new copy of an option list.

  @param[in] SrcOptionList   Pointer to source option list.

  @return A pointer to the duplicated option list or NULL if SrcOptionList is NULL or allocate memory failed.
**/
STATIC
LIST_ENTRY *
DuplicateOptionList (
  IN CONST LIST_ENTRY              *SrcOptionList
  )
{
  LIST_ENTRY                       *NewOptionList;
  LIST_ENTRY                       *Link;
  H2O_BDS_LOAD_OPTION              *LoadOption;
  H2O_BDS_LOAD_OPTION              *NewLoadOption;
  EFI_STATUS                       Status;

  if (SrcOptionList == NULL) {
    return NULL;
  }

  NewOptionList = AllocatePool (sizeof (LIST_ENTRY));
  if (NewOptionList == NULL) {
    return NULL;
  }
  InitializeListHead (NewOptionList);

  Link = GetFirstNode (SrcOptionList);
  while (!IsNull (SrcOptionList, Link)) {
    LoadOption = BDS_OPTION_FROM_LINK (Link);
    Link       = GetNextNode (SrcOptionList, Link);

    Status = gBdsServices->CreateLoadOption2 (
                             gBdsServices,
                             LoadOption->OptionType,
                             BDS_OPTION_HAVE_LOAD_ORDER_VAR_NAME(LoadOption) ? LoadOption->LoadOrderVarName  : NULL,
                             BDS_OPTION_HAVE_LOAD_ORDER_VAR_NAME(LoadOption) ? &LoadOption->LoadOrderVarGuid : NULL,
                             LoadOption->Attributes,
                             LoadOption->DevicePath,
                             LoadOption->Description,
                             LoadOption->LoadOptionalData,
                             LoadOption->LoadOptionalDataSize,
                             &NewLoadOption
                             );
    if (!EFI_ERROR (Status)) {
      InsertTailList (NewOptionList, &NewLoadOption->Link);
    }
  }

  return NewOptionList;
}

/**
  Free each BDS option from option list.

  @param[in, out] OptionsList       A pointer to the BDS option list.
**/
STATIC
VOID
FreeBdsOptionList (
  IN OUT LIST_ENTRY           *OptionsList
  )
{
  LIST_ENTRY                  *Link;
  H2O_BDS_LOAD_OPTION         *Option;

  if (OptionsList == NULL) {
    return;
  }

  Link = GetFirstNode (OptionsList);
  while (!IsNull (OptionsList, Link)) {
    Option = BDS_OPTION_FROM_LINK (Link);
    Link   = GetNextNode (OptionsList, Link);

    RemoveEntryList (&Option->Link);
    gBdsServices->FreeLoadOption (gBdsServices, Option);
  }
}

/**
  Internal function to initialize boot device select checkpoint data and trigger checkpoint.

  @retval EFI_SUCCESS             Trigger boot device select checkpoint successfully.
  @retval EFI_OUT_OF_RESOURCES    Allocate memory to initialize checkpoint data failed.
**/
STATIC
EFI_STATUS
TriggerCpBootDeviceSelect (
  IN LIST_ENTRY                          **BootLists
  )
{
  H2O_BDS_CP_BOOT_DEVICE_SELECT_DATA     BdsBootDeviceSelectData;
  EFI_STATUS                             Status;

  BdsBootDeviceSelectData.Size     = sizeof (H2O_BDS_CP_BOOT_DEVICE_SELECT_DATA);
  BdsBootDeviceSelectData.Status   = H2O_BDS_TASK_NORMAL;
  BdsBootDeviceSelectData.BootList = BootLists;

  DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpBootDeviceSelectGuid));
  Status = H2OCpTrigger (&gH2OBdsCpBootDeviceSelectGuid, &BdsBootDeviceSelectData);
  DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", BdsBootDeviceSelectData.Status));
  return Status;
}

/**
  Update new option list to BDS services protocol.

  @param[in] OptionType            BDS load option type
  @param[in] NewOptionList         Pointer to new option list

  @retval EFI_SUCCESS              Update new option list successfully.
  @retval EFI_INVALID_PARAMETER    NewOptionList is NULL.
  @retval EFI_UNSUPPORTED          Option type is not supported.
  @return Other                    Get original option list failed.
**/
EFI_STATUS
UpdateOptionList (
  IN H2O_BDS_LOAD_OPTION_TYPE      OptionType,
  IN CONST LIST_ENTRY              *NewOptionList
  )
{
  EFI_STATUS                       Status;
  LIST_ENTRY                       *OldOptionList;
  LIST_ENTRY                       *Link;
  H2O_BDS_LOAD_OPTION              *LoadOption;
  H2O_BDS_LOAD_OPTION              *NewLoadOption;
  H2O_BDS_LOAD_OPTION              *PreviousLoadOption;
  CHAR16                           *VariableName;
  EFI_GUID                         VariableGuid;

  if (NewOptionList == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  switch (OptionType) {

  case H2O_BDS_LOAD_OPTION_TYPE_SYSPREP:
    Status = gBdsServices->GetSysPrepList (gBdsServices, &OldOptionList);
    break;

  case H2O_BDS_LOAD_OPTION_TYPE_OS_RECOVERY:
    Status = gBdsServices->GetOsRecoveryList (gBdsServices, &OldOptionList);
    break;

  case H2O_BDS_LOAD_OPTION_TYPE_PLATFORM_RECOVERY:
    Status = gBdsServices->GetPlatformRecoveryList (gBdsServices, &OldOptionList);
    break;

  default:
    return EFI_UNSUPPORTED;
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Remove all load options from current option list.
  //
  Link = GetFirstNode (OldOptionList);
  while (!IsNull (OldOptionList, Link)) {
    LoadOption = BDS_OPTION_FROM_LINK (Link);
    Link       = GetNextNode (OldOptionList, Link);

    CopyGuid (&VariableGuid, &LoadOption->LoadOrderVarGuid);
    gBdsServices->RemoveLoadOption2 (
                    gBdsServices,
                    LoadOption->OptionType,
                    LoadOption->LoadOrder,
                    LoadOption->OptionType == H2O_BDS_LOAD_OPTION_TYPE_OS_RECOVERY ? &VariableGuid : NULL
                    );
  }

  //
  // Insert all load options from new list in sequence.
  //
  PreviousLoadOption = NULL;
  Link = GetFirstNode (NewOptionList);
  while (!IsNull (NewOptionList, Link)) {
    LoadOption = BDS_OPTION_FROM_LINK (Link);
    Link       = GetNextNode (NewOptionList, Link);

    Status = gBdsServices->CreateLoadOption2 (
                             gBdsServices,
                             LoadOption->OptionType,
                             BDS_OPTION_HAVE_LOAD_ORDER_VAR_NAME(LoadOption) ? LoadOption->LoadOrderVarName  : NULL,
                             BDS_OPTION_HAVE_LOAD_ORDER_VAR_NAME(LoadOption) ? &LoadOption->LoadOrderVarGuid : NULL,
                             LoadOption->Attributes,
                             LoadOption->DevicePath,
                             LoadOption->Description,
                             LoadOption->LoadOptionalData,
                             LoadOption->LoadOptionalDataSize,
                             &NewLoadOption
                             );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = gBdsServices->ConvertLoadOptionToVar (
                             gBdsServices,
                             NewLoadOption,
                             &VariableName,
                             &VariableGuid
                             );
    if (EFI_ERROR (Status)) {
      gBdsServices->FreeLoadOption (gBdsServices, NewLoadOption);
      continue;
    }

    FreePool (VariableName);
    gBdsServices->InsertLoadOption2 (
                    gBdsServices,
                    NewLoadOption,
                    PreviousLoadOption
                    );
    PreviousLoadOption = NewLoadOption;
  }

  return EFI_SUCCESS;
}

/**
  Perform the processing of the OS Recovery load option list.

  @retval EFI_SUCCESS    Perform OS recovery successfully.
  @retval EFI_ABORTED    Skip processing OS Recovery by checkpoint requirement.
  @return Other          Get OS Recovery load option list failed or there is no option which is launched successfully.
**/
EFI_STATUS
PerformOsRecovery (
  VOID
  )
{
  EFI_STATUS                       Status;
  LIST_ENTRY                       *OsRecoveryList;
  LIST_ENTRY                       *Link;
  H2O_BDS_LOAD_OPTION              *LoadOption;
  UINTN                            ExitDataSize;
  CHAR16                           *ExitData;
  EFI_STATUS                       LaunchStatus;
  H2O_BDS_CP_OS_RECOVERY_DATA      BdsOsRecoveryData;

  if (FeaturePcdGet (PcdH2OBdsCpOsRecoverySupported)) {
    Status = gBdsServices->GetOsRecoveryList (gBdsServices, &OsRecoveryList);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    BdsOsRecoveryData.Size           = sizeof (H2O_BDS_CP_OS_RECOVERY_DATA);
    BdsOsRecoveryData.Status         = H2O_BDS_TASK_NORMAL;
    BdsOsRecoveryData.OsRecoveryList = DuplicateOptionList (OsRecoveryList);
    DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpOsRecoveryGuid));
    H2OCpTrigger (&gH2OBdsCpOsRecoveryGuid, &BdsOsRecoveryData);
    DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", BdsOsRecoveryData.Status));

    switch (BdsOsRecoveryData.Status) {

    case H2O_BDS_TASK_SKIP:
      FreeBdsOptionList (BdsOsRecoveryData.OsRecoveryList);
      return EFI_ABORTED;

    case H2O_BDS_TASK_UPDATE:
      UpdateOptionList (H2O_BDS_LOAD_OPTION_TYPE_OS_RECOVERY, BdsOsRecoveryData.OsRecoveryList);
      break;
    }
    FreeBdsOptionList (BdsOsRecoveryData.OsRecoveryList);
  }

  POST_CODE (BDS_OS_RECOVERY);

  Status = gBdsServices->GetOsRecoveryList (gBdsServices, &OsRecoveryList);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = EFI_NOT_FOUND;
  for (Link = GetFirstNode (OsRecoveryList); !IsNull (OsRecoveryList, Link); Link = GetNextNode (OsRecoveryList, Link)) {
    LoadOption = BDS_OPTION_FROM_LINK (Link);
    if ((LoadOption->Attributes & LOAD_OPTION_ACTIVE) == 0) {
      continue;
    }

    LaunchStatus = gBdsServices->LaunchLoadOption (gBdsServices, LoadOption, &ExitDataSize, &ExitData);
    if (!EFI_ERROR (LaunchStatus)) {
      Status = EFI_SUCCESS;
    }
  }

  return Status;
}

/**
  Perform the processing of the Platform Recovery load option list.

  @retval EFI_SUCCESS    Perform Platform recovery successfully.
  @retval EFI_ABORTED    Skip processing Platform Recovery by checkpoint requirement.
  @return EFI_NOT_FOUND  There is no option which is launched successfully.
  @return Other          Get Platform Recovery load option list failed.
**/
EFI_STATUS
PerformPlatformRecovery (
  VOID
  )
{
  EFI_STATUS                             Status;
  LIST_ENTRY                             *PlatformRecoveryList;
  LIST_ENTRY                             *Link;
  H2O_BDS_LOAD_OPTION                    *LoadOption;
  UINTN                                  ExitDataSize;
  CHAR16                                 *ExitData;
  H2O_BDS_CP_PLATFORM_RECOVERY_DATA      BdsPlatformRecoveryData;

  if (FeaturePcdGet (PcdH2OBdsCpPlatformRecoverySupported)) {
    Status = gBdsServices->GetPlatformRecoveryList (gBdsServices, &PlatformRecoveryList);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    BdsPlatformRecoveryData.Size                 = sizeof (H2O_BDS_CP_PLATFORM_RECOVERY_DATA);
    BdsPlatformRecoveryData.Status               = H2O_BDS_TASK_NORMAL;
    BdsPlatformRecoveryData.PlatformRecoveryList = DuplicateOptionList (PlatformRecoveryList);
    DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpPlatformRecoveryGuid));
    H2OCpTrigger (&gH2OBdsCpPlatformRecoveryGuid, &BdsPlatformRecoveryData);
    DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", BdsPlatformRecoveryData.Status));

    switch (BdsPlatformRecoveryData.Status) {

    case H2O_BDS_TASK_SKIP:
      FreeBdsOptionList (BdsPlatformRecoveryData.PlatformRecoveryList);
      return EFI_ABORTED;

    case H2O_BDS_TASK_UPDATE:
      UpdateOptionList (H2O_BDS_LOAD_OPTION_TYPE_PLATFORM_RECOVERY, BdsPlatformRecoveryData.PlatformRecoveryList);
      break;
    }
    FreeBdsOptionList (BdsPlatformRecoveryData.PlatformRecoveryList);
  }

  POST_CODE (BDS_PLATFORM_RECOVERY);

  Status = gBdsServices->GetPlatformRecoveryList (gBdsServices, &PlatformRecoveryList);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Link = GetFirstNode (PlatformRecoveryList); !IsNull (PlatformRecoveryList, Link); Link = GetNextNode (PlatformRecoveryList, Link)) {
    LoadOption = BDS_OPTION_FROM_LINK (Link);
    if ((LoadOption->Attributes & LOAD_OPTION_ACTIVE) == 0) {
      continue;
    }

    Status = gBdsServices->LaunchLoadOption (gBdsServices, LoadOption, &ExitDataSize, &ExitData);
    if (!EFI_ERROR (Status)) {
      return Status;
    }
  }

  return EFI_NOT_FOUND;
}

/**
  Perform the processing of the System Preparation load option list.

  @retval EFI_SUCCESS    Perform System Preparation successfully.
  @retval EFI_ABORTED    Skip processing System Preparation by checkpoint requirement.
  @return Other          Get System Preparation load option list failed or there is no option which is launched successfully.
**/
EFI_STATUS
PerformSysPrep (
  VOID
  )
{
  EFI_STATUS                             Status;
  LIST_ENTRY                             *SysPrepList;
  LIST_ENTRY                             *Link;
  H2O_BDS_LOAD_OPTION                    *LoadOption;
  UINTN                                  ExitDataSize;
  CHAR16                                 *ExitData;
  EFI_STATUS                             LaunchStatus;
  H2O_BDS_CP_SYSPREP_DATA                BdsSysPrepData;

  if (FeaturePcdGet (PcdH2OBdsCpSysPrepSupported)) {
    Status = gBdsServices->GetSysPrepList (gBdsServices, &SysPrepList);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    BdsSysPrepData.Size        = sizeof (H2O_BDS_CP_SYSPREP_DATA);
    BdsSysPrepData.Status      = H2O_BDS_TASK_NORMAL;
    BdsSysPrepData.SysPrepList = DuplicateOptionList (SysPrepList);
    DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpSysPrepGuid));
    H2OCpTrigger (&gH2OBdsCpSysPrepGuid, &BdsSysPrepData);
    DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", BdsSysPrepData.Status));

    switch (BdsSysPrepData.Status) {

    case H2O_BDS_TASK_SKIP:
      FreeBdsOptionList (BdsSysPrepData.SysPrepList);
      return EFI_ABORTED;

    case H2O_BDS_TASK_UPDATE:
      UpdateOptionList (H2O_BDS_LOAD_OPTION_TYPE_SYSPREP, BdsSysPrepData.SysPrepList);
      break;
    }
    FreeBdsOptionList (BdsSysPrepData.SysPrepList);
  }

  POST_CODE (BDS_SYSPREP);

  Status = gBdsServices->GetSysPrepList (gBdsServices, &SysPrepList);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = EFI_NOT_FOUND;
  for (Link = GetFirstNode (SysPrepList); !IsNull (SysPrepList, Link); Link = GetNextNode (SysPrepList, Link)) {
    LoadOption = BDS_OPTION_FROM_LINK (Link);
    if ((LoadOption->Attributes & LOAD_OPTION_ACTIVE) == 0) {
      continue;
    }

    LaunchStatus = gBdsServices->LaunchLoadOption (gBdsServices, LoadOption, &ExitDataSize, &ExitData);
    if (!EFI_ERROR (LaunchStatus)) {
      Status = EFI_SUCCESS;
    }
  }

  return Status;
}

/**
 Report the capability of system preparation by setting BootOptionSupport variable.

 @retval EFI_SUCCESS        Report the capability of system preparation successfully.
 @retval Other              Report the capability of system preparation failed.
**/
STATIC
EFI_STATUS
ReportSysPrepCapability (
  VOID
  )
{
  EFI_STATUS                       Status;
  UINT32                           BootOptionSupport;
  UINTN                            Size;

  Size   = sizeof (BootOptionSupport);
  Status = gRT->GetVariable (
                  EFI_BOOT_OPTION_SUPPORT_VARIABLE_NAME,
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &Size,
                  &BootOptionSupport
                  );
  if (EFI_ERROR (Status)) {
    BootOptionSupport = 0;
  }

  if (FeaturePcdGet (PcdH2OBdsSysPrepSupported)) {
    BootOptionSupport |= (UINT32) EFI_BOOT_OPTION_SUPPORT_SYSPREP;
  } else {
    BootOptionSupport &= (~(UINT32) EFI_BOOT_OPTION_SUPPORT_SYSPREP);
  }

  return gRT->SetVariable (
                EFI_BOOT_OPTION_SUPPORT_VARIABLE_NAME,
                &gEfiGlobalVariableGuid,
                EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                sizeof (BootOptionSupport),
                &BootOptionSupport
                );
}

/**
  Perform boot option enumeration to refresh boot list.

  @param[out] BootLists            Double pointer to boot list

  @retval EFI_SUCCESS              Refresh boot list successfully.
  @return Other                    Get boot option list failed.
**/
STATIC
EFI_STATUS
RefreshBootList (
  OUT LIST_ENTRY                   **BootLists
  )
{
  BdsLibEnumerateAllBootOption (TRUE, NULL);
  return gBdsServices->GetBootList (gBdsServices, BootLists);
}

/**
  Internal function to check is whether no any active boot option.

  @param[out] BootLists     Pointer to boot list

  @retval TRUE              Doesn't have any boot option with active boot attribute.
  @return FALSE             Has at least one boot opton with active boot attribute.
**/
STATIC
BOOLEAN
IsNoActiveBootOption (
  LIST_ENTRY                    *BootLists
  )
{
  LIST_ENTRY                *Link;
  H2O_BDS_LOAD_OPTION       *BootOption;

   if (BootLists == NULL) {
     return TRUE;
   }
   for (Link = GetFirstNode (BootLists); Link != BootLists; Link = GetNextNode (BootLists, Link)) {
      BootOption = BDS_OPTION_FROM_LINK (Link);
      if (IS_LOAD_OPTION_TYPE (BootOption->Attributes, LOAD_OPTION_ACTIVE)) {
        return FALSE;
      }
   }
   return TRUE;
}

/**
 Dump boot option list.
**/
STATIC
VOID
DumpBootOptionList (
  IN LIST_ENTRY                    *BootLists,
  IN H2O_BDS_LOAD_OPTION           *CurrentBootOption
  )
{
  EFI_STATUS                       Status;
  EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;
  LIST_ENTRY                       *Link;
  H2O_BDS_LOAD_OPTION              *BootOption;
  H2O_BDS_LOAD_OPTION              *ExpandedBootOption;
  EFI_DEVICE_PATH_PROTOCOL         *DevicePath;
  CHAR16                           *DevicePathStr;
  UINTN                            Index;
  BOOLEAN                          AttrStrFound;

  if (BootLists == NULL || CurrentBootOption == NULL) {
    return;
  }

  Status = gBS->LocateProtocol (&gEfiDevicePathToTextProtocolGuid, NULL, (VOID **) &DevPathToText);
  if (EFI_ERROR (Status)) {
    return;
  }

  DEBUG ((EFI_D_INFO, "-------------------------- Boot Option List --------------------------\n"));
  for (Link = GetFirstNode (BootLists); !IsNull (BootLists, Link); Link = GetNextNode (BootLists, Link)) {
    BootOption = BDS_OPTION_FROM_LINK (Link);

    if (BootOption->Expanded && !IsListEmpty (&BootOption->ExpandedLoadOptions)) {
      ExpandedBootOption = BDS_OPTION_FROM_LINK (BootOption->ExpandedLoadOptions.ForwardLink);
      DevicePath = ExpandedBootOption->DevicePath;
    } else {
      DevicePath = BootOption->DevicePath;
    }
    DevicePathStr = DevPathToText->ConvertDevicePathToText (DevicePath, FALSE, FALSE);
    if (DevicePathStr == NULL) {
      break;
    }

    DEBUG (\
      (EFI_D_INFO, "%s[Boot%04x] %s \"%s\" Attributes:0x%08x",\
      (BootOption->LoadOrder == CurrentBootOption->LoadOrder) ? L"*" : L" ",\
      BootOption->LoadOrder,\
      DevicePathStr,\
      BootOption->Description,\
      BootOption->Attributes
      ));
    FreePool (DevicePathStr);

    AttrStrFound = FALSE;
    for (Index = 0; Index < sizeof (mLoadOptionAttrStrInfo) / sizeof (LOAD_OPTION_ATTRIBUTE_STRING_INFO); Index++) {
      if ((BootOption->Attributes & mLoadOptionAttrStrInfo[Index].Mask) == mLoadOptionAttrStrInfo[Index].Value) {
        if (!AttrStrFound) {
          AttrStrFound = TRUE;
          DEBUG ((EFI_D_INFO, "("));
        } else {
          DEBUG ((EFI_D_INFO, "|"));
        }
        DEBUG ((EFI_D_INFO, "%s",mLoadOptionAttrStrInfo[Index].String));
      }
    }
    if (AttrStrFound) {
      DEBUG ((EFI_D_INFO, ")"));
    }
    DEBUG ((EFI_D_INFO, "\n"));
  }
  DEBUG ((EFI_D_INFO, "----------------------------------------------------------------------\n"));
}

/**
  This function attempts to boot for the boot order specified
  by platform policy.
**/
VOID
BdsBootDeviceSelect (
  VOID
  )
{
  EFI_STATUS                    Status;
  LIST_ENTRY                    *Link;
  H2O_BDS_LOAD_OPTION           *BootOption;
  H2O_BDS_LOAD_OPTION           *BootNextOption;
  UINTN                         ExitDataSize;
  CHAR16                        *ExitData;
  LIST_ENTRY                    *BootLists;
  LIST_ENTRY                    DefautlBootLists;
  CHAR16                        Buffer[20];
  KERNEL_CONFIGURATION          SystemConfiguration;
  EFI_STATUS                    SystemConfigStatus;
  EFI_BOOT_MODE                 BootMode;
  UINT8                         CmosData = 0;
  CHAR16                        *String;
  BOOLEAN                       AlreadyConnectAll;
  BOOLEAN                       LinkUpdated;
  BOOLEAN                       BootFromBootNext;
  UINTN                         MappingOpitonCount;
  UINT16                        *MappingOpitonOrder;
  UINT16                        SelectedOptionNum;
  UINTN                         VariableSize;
  VOID                          *VariablePtr;
  H2O_BDS_LOAD_OPTION           *TempBootOption;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  BOOLEAN                        BootDefaultOption;
  H2O_BDS_LOAD_OPTION           *LoadOption;
  UINT64                        OsIndications;
  UINT64                        OsIndicationsSupported;
  BOOLEAN                       RequireOsRecovery;
  BOOLEAN                       RequirePlatformRecovery;
  UINT8                         RecoveryRetryTime;
  BOOLEAN                       OrgHotKeySetting;

  //
  // PostCode = 0x2B, Try to boot system to OS
  //
  POST_CODE (BDS_BOOT_DEVICE_SELECT);

  OrgHotKeySetting = EnableBdsHotKey (FALSE);
  BdsHotKeyBoot (TRIGGER_POINT_BEFORE_OS_INDICATIONS);

  if (DoesOsIndicateBootToFwUI ()) {
    POST_CODE (BDS_ENTER_FIRMWARE_UI);
    ClearOsIndicationsBits ((UINT64) EFI_OS_INDICATIONS_BOOT_TO_FW_UI);
    DevicePath = GetDevicePathByStr ((CHAR16 *) PcdGetPtr (PcdH2OBdsOsIndicationsFwUiApp));
    if (DevicePath != NULL) {
      LaunchBootOptionByDevicePath (DevicePath);
      FreePool (DevicePath);
    }
  }

  Status = gBdsServices->GetOsIndications (gBdsServices, &OsIndications, &OsIndicationsSupported);
  if (Status != EFI_SUCCESS) {
    OsIndications          = 0;
    OsIndicationsSupported = 0;
  }
  RequirePlatformRecovery = (BOOLEAN) ((OsIndications & OsIndicationsSupported & EFI_OS_INDICATIONS_START_PLATFORM_RECOVERY) != 0);
  RequireOsRecovery       = (BOOLEAN) ((OsIndications & OsIndicationsSupported & EFI_OS_INDICATIONS_START_OS_RECOVERY      ) != 0);

  if ((PcdGet64 (PcdOsIndicationsSupported) & EFI_OS_INDICATIONS_START_PLATFORM_RECOVERY) != 0 && RequirePlatformRecovery) {
    ClearOsIndicationsBits ((UINT64) EFI_OS_INDICATIONS_START_PLATFORM_RECOVERY);
    PerformPlatformRecovery ();
  } else if ((PcdGet64 (PcdOsIndicationsSupported) & EFI_OS_INDICATIONS_START_OS_RECOVERY) != 0 && RequireOsRecovery) {
    ClearOsIndicationsBits ((UINT64) EFI_OS_INDICATIONS_START_OS_RECOVERY);
    PerformOsRecovery ();
  }
  EnableBdsHotKey (OrgHotKeySetting);

  Status = BdsLibGetBootMode (&BootMode);
  if (EFI_ERROR (Status)) {
    return;
  }

  //
  // Got the latest boot option
  //
  BootNextOption    = NULL;
  AlreadyConnectAll = FALSE;

  //
  // First check the boot next option
  //
  ZeroMem (Buffer, sizeof (Buffer));

  if (mBootNext == NULL) {
    //
    // Check if we have the boot next option created during BDS
    //
    mBootNext = BdsLibGetVariableAndSize (
                  L"BootNext",
                  &gEfiGlobalVariableGuid,
                  &VariableSize
                  );
  }

  if (gBdsServices->BootNextLoadOption == NULL) {
    //
    // Check if we have the boot next option created during BDS
    //
    Status = gBdsServices->ConvertVarToLoadOption (gBdsServices, EFI_BOOT_NEXT_VARIABLE_NAME, &gEfiGlobalVariableGuid, &LoadOption);
    if (Status == EFI_SUCCESS) {
      gBdsServices->BootNextLoadOption = LoadOption;
    }
  }

  OrgHotKeySetting = EnableBdsHotKey (FALSE);
  BdsHotKeyBoot (TRIGGER_POINT_BEFORE_BOOT_NEXT);

  if (FeaturePcdGet(PcdH2OBdsSysPrepSupported) && !RequireOsRecovery && !RequirePlatformRecovery) {
    PerformSysPrep ();
  }

  gRT->SetVariable (
         EFI_BOOT_NEXT_VARIABLE_NAME,
         &gEfiGlobalVariableGuid,
         0,
         0,
         NULL
         );
  if (gBdsServices->BootNextLoadOption && mBootNext != NULL) {
    //
    // Indicate we have the boot next variable, so this time
    // boot will always have this boot option
    //
    if (BdsLibIsDummyBootOption (*mBootNext)) {
      Status = BdsLibGetMappingBootOptions (*mBootNext, &MappingOpitonCount, &MappingOpitonOrder);
      if (Status == EFI_SUCCESS) {
        if (MappingOpitonCount == 0) {
          //
          // Show no USB boot option message for user and then go to Boot Manager menu for user to select boot option.
          //
          CreateNoMappingBootOptionPopUp (*mBootNext);
          VariableSize = 0;
          VariablePtr = NULL;
          Status  = gRT->GetVariable (L"BootOrder", &gEfiGlobalVariableGuid, NULL, &VariableSize, VariablePtr);
          if (Status == EFI_BUFFER_TOO_SMALL) {
            LaunchBootFailApp ();
          }
        } else if (MappingOpitonCount == 1) {
          //
          // System has one mapping boot option, boot this boot option direclty.
          //
          UnicodeSPrint (Buffer, sizeof (Buffer), L"Boot%04x", MappingOpitonOrder[0]);
          gBdsServices->ConvertVarToLoadOption (gBdsServices, Buffer, &gEfiGlobalVariableGuid, &BootNextOption);
        } else {
          //
          // If system has more than 1 mapping boot option, let user to select specific mapping boot option.
          //
          Status = SelectMappingBootOption (*mBootNext, MappingOpitonCount, MappingOpitonOrder, &SelectedOptionNum);
          if (!EFI_ERROR (Status)) {
            UnicodeSPrint (Buffer, sizeof (Buffer), L"Boot%04x", SelectedOptionNum);
            gBdsServices->ConvertVarToLoadOption (gBdsServices, Buffer, &gEfiGlobalVariableGuid, &BootNextOption);
          }
        }
        if (MappingOpitonOrder != NULL) {
          FreePool (MappingOpitonOrder);
        }
      }
    } else {
      //
      // Add the boot next boot option
      //
      UnicodeSPrint (Buffer, sizeof (Buffer), L"Boot%04x", *mBootNext);
      gBdsServices->ConvertVarToLoadOption (gBdsServices, Buffer, &gEfiGlobalVariableGuid, &BootNextOption);
    }
  } else if (BootMode == BOOT_ON_S4_RESUME) {
    CmosData = ReadCmos8 (LastBootDevice);
    if (IsLegacyBootOption ((UINT16)CmosData)) {
      //
      // Add the boot next boot option
      //
      UnicodeSPrint (Buffer, sizeof (Buffer), L"Boot%04x", ReadCmos8 (LastBootDevice));
      gBdsServices->ConvertVarToLoadOption (gBdsServices, Buffer, &gEfiGlobalVariableGuid, &BootNextOption);
    }
  }

  BootDefaultOption = FALSE;
  gBdsServices->GetBootList (gBdsServices, &BootLists);

  if (IsListEmpty (BootLists)) {
    InitializeListHead (&DefautlBootLists);
    AddDefaultBootOptionsToBootList (&DefautlBootLists);
    BootLists = &DefautlBootLists;
    BootDefaultOption = TRUE;
  }

  RecoveryRetryTime  = 0;
  if (FeaturePcdGet (PcdH2OBdsCpBootDeviceSelectSupported)) {
    TriggerCpBootDeviceSelect (&BootLists);
  }

  Link = GetFirstNode (BootLists);
  SystemConfigStatus = GetKernelConfiguration (&SystemConfiguration);

  // Here we make the boot in a loop, every boot success will
  // return to the front page
  //
  for (;;) {
    if (IsListEmpty (BootLists)) {
      BdsNoBootDevice (NO_BOOT_DEVICE);
    } else if (IsNoActiveBootOption (BootLists)) {
      BdsNoBootDevice (NO_ACTIVE_BOOT_OPTION);
    }

    //
    // Check the boot option list first
    //
    if (Link == BootLists && BootNextOption == NULL) {
      //
      // There are two ways to enter here:
      // 1. There is no active boot option, give user chance to
      //    add new boot option
      // 2. All the active boot option processed, and there is no
      //    one is success to boot, then we back here to allow user
      //    add new active boot option
      //
      if (RecoveryRetryTime == 0) {
        RecoveryRetryTime++;
        Status = PerformOsRecovery ();
        if (!EFI_ERROR (Status)) {
          RefreshBootList (&BootLists);
          Link = GetFirstNode (BootLists);
          continue;
        }
      }
      if (RecoveryRetryTime == 1) {
        RecoveryRetryTime++;
        Status = PerformPlatformRecovery ();
        if (!EFI_ERROR (Status)) {
          RefreshBootList (&BootLists);
          Link = GetFirstNode (BootLists);
          continue;
        }
      }

      String = GetStringById (STRING_TOKEN (STR_AUTO_FAILOVER_NO_BOOT_DEVICE));
      if (String != NULL) {
        AutoFailoverPolicyBehavior (TRUE, String);
        FreePool(String);
      }

      LaunchBootFailApp ();

      gBdsServices->GetBootList (gBdsServices, &BootLists);
      Link = GetFirstNode (BootLists);
      continue;
    }
    EnableBdsHotKey (OrgHotKeySetting);

    if (BootNextOption != NULL) {
      //
      // Need expand load option to prevent from the boot option is windows to GO, short form device path, USB
      // WWID or USB class device path.
      //
      gBdsServices->ExpandLoadOption (gBdsServices, BootNextOption);

      BootOption = BootNextOption;
      BootNextOption = NULL;
      BootFromBootNext = TRUE;
    } else {
      //
      // Get the boot option from the link list
      //
      BootOption = BDS_OPTION_FROM_LINK (Link);
      BootFromBootNext = FALSE;
    }

    OrgHotKeySetting = EnableBdsHotKey (FALSE);
    BdsHotKeyBoot (TRIGGER_POINT_BEFORE_CHECK_EACH_BOOT_OPTION);
    //
    // According to EFI Specification, if a load option is not marked
    // as LOAD_OPTION_ACTIVE, the boot manager will not automatically
    // load the option.
    //
    if (!IS_LOAD_OPTION_TYPE (BootOption->Attributes, LOAD_OPTION_ACTIVE)) {
      //
      // skip the header of the link list, because it has no boot option
      //
      if (!BootFromBootNext) {
        Link = Link->ForwardLink;
      }
      continue;
    }
    //
    // Make sure the boot option device path connected,
    // but ignore the BBS device path
    //
    if (DevicePathType (BootOption->DevicePath) != BBS_DEVICE_PATH) {
      //
      // Notes: the internal shell can not been connected with device path
      // so we do not check the status here
      //
      BdsLibConnectDevicePath (BootOption->DevicePath);
    }

    DEBUG_CODE (DumpBootOptionList(BootLists, BootOption););
    //
    // All the driver options should have been processed since
    // now boot will be performed.
    //

    Status = gBdsServices->LaunchLoadOption (gBdsServices, BootOption, &ExitDataSize, &ExitData);
    LinkUpdated = FALSE;
    if (BdsLibIsWin8FastBootActive () && !AlreadyConnectAll && !BootDefaultOption) {
      //
      // create new boot option to prevent original boot option is freed by calling gBdsServices->GetBootList ()
      // to synchronize BootList to BootOrder variable.
      //
      gBdsServices->ConvertVarToLoadOption (gBdsServices, BootOption->LoadOrderVarName, &BootOption->LoadOrderVarGuid, &BootOption);
      //
      // For Win 8 Fast Boot, do one time ConnectAll to connect all boot devices and point Link to next boot device.
      //
      AlreadyConnectAll = TRUE;
      BdsLibConnectUsbHID ();
      BdsLibConnectAll ();
      RefreshBootList (&BootLists);
      //
      // If last boot is fail, update Link for next boot.
      //
      if (EFI_ERROR (Status)) {
        Link = GetFirstNode (BootLists);
        if (!IsListEmpty (BootLists)) {
          //
          // If next boot device path is the same as previous one, skip to next.
          //
          DevicePath = BootOption->DevicePath;
          TempBootOption = BDS_OPTION_FROM_LINK (Link);
          if (CompareMem (DevicePath, TempBootOption->DevicePath, GetDevicePathSize (DevicePath)) == 0) {
            Link = Link->ForwardLink;
          }
        }
        LinkUpdated = TRUE;
      }

      if (EFI_ERROR (Status) && !FeaturePcdGet(PcdWin8FastBootErrorMessageSupported)) {
        continue;
      }
    }

    if (Status != EFI_SUCCESS) {
      //
      // Call platform action to indicate the boot fail
      //
      // If boot default device (not including Windows To Go) fail and Auto Failover is disable, go into firmware UI.
      //
      if (!EFI_ERROR (SystemConfigStatus) &&
          SystemConfiguration.AutoFailover == 0 &&
          !((DevicePathType (BootOption->DevicePath) == MESSAGING_DEVICE_PATH) &&
            (DevicePathSubType (BootOption->DevicePath) == MSG_USB_CLASS_DP))) {
        String = GetStringById (STRING_TOKEN (STR_AUTO_FAILOVER_DISABLE_BOOT_FAIL));
        if (String != NULL) {
          AutoFailoverPolicyBehavior (FALSE, String);
          FreePool(String);
        }
      } else {
        if (!(((DevicePathType (BootOption->DevicePath) == MESSAGING_DEVICE_PATH) &&
               (DevicePathSubType (BootOption->DevicePath) == MSG_USB_CLASS_DP)) &&
              Status != EFI_SECURITY_VIOLATION)) {
          BdsLibBootFailed (BootOption, Status, ExitData, ExitDataSize);
        }
      }

      if (!LinkUpdated && !BootFromBootNext) {
        Link = Link->ForwardLink;
      }
    } else {
      //
      // Call platform action to indicate the boot success
      //
      BdsLibBootSuccess (BootOption);

      //
      // Boot success, then stop process the boot order, and go to boot success application.
      //
      RefreshBootList (&BootLists);

      DevicePath = GetDevicePathByAsciiStr ((CHAR8 *) PcdGetPtr (PcdH2OBdsBootSuccessApp));
      if (DevicePath != NULL) {
        LaunchBootOptionByDevicePath (DevicePath);
        FreePool (DevicePath);
      }

      gBdsServices->GetBootList (gBdsServices, &BootLists);
      Link = GetFirstNode (BootLists);
    }
  }
}

/**

  Allocate a block of memory that will contain performance data to OS.

**/
VOID
BdsAllocateMemoryForPerformanceData (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_PHYSICAL_ADDRESS          AcpiLowMemoryBase;
  EDKII_VARIABLE_LOCK_PROTOCOL  *VariableLock;

  AcpiLowMemoryBase = 0x0FFFFFFFFULL;

  //
  // Allocate a block of memory that will contain performance data to OS.
  //
  Status = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiReservedMemoryType,
                  EFI_SIZE_TO_PAGES (PERF_DATA_MAX_LENGTH),
                  &AcpiLowMemoryBase
                  );
  if (!EFI_ERROR (Status)) {
    //
    // Save the pointer to variable for use in S3 resume.
    //
    Status = gRT->SetVariable (
               L"PerfDataMemAddr",
               &gEfiGenericVariableGuid,
               EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
               sizeof (UINT32),
               &AcpiLowMemoryBase
               );
    ASSERT_EFI_ERROR (Status);
    //
    // Mark L"PerfDataMemAddr" variable to read-only if the Variable Lock protocol exists
    //
    Status = gBS->LocateProtocol (&gEdkiiVariableLockProtocolGuid, NULL, (VOID **) &VariableLock);
    if (!EFI_ERROR (Status)) {
      Status = VariableLock->RequestToLock (VariableLock, L"PerfDataMemAddr", &gEfiGenericVariableGuid);
      ASSERT_EFI_ERROR (Status);
    }
  }
}

EFI_STATUS
LockSensitiveVariables (
  VOID
  )
{
  EFI_STATUS                    Status;
  EDKII_VARIABLE_LOCK_PROTOCOL  *VariableLock;


  //
  // Lock setup sensitive variables
  //
  Status = gBS->LocateProtocol (&gEdkiiVariableLockProtocolGuid, NULL, (VOID **) &VariableLock);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = VariableLock->RequestToLock (VariableLock, L"Setup", &gSystemConfigurationGuid);
  Status = VariableLock->RequestToLock (VariableLock, SECURE_FLASH_INFORMATION_NAME, &gSecureFlashInfoGuid);
  Status = VariableLock->RequestToLock (VariableLock, ACPI_GLOBAL_VARIABLE, &gAcpiVariableSetGuid);
  Status = VariableLock->RequestToLock (VariableLock, EFI_ADMINISTER_SECURE_BOOT_NAME, &gEfiGenericVariableGuid);
  Status = VariableLock->RequestToLock (VariableLock, EFI_MEMORY_TYPE_INFORMATION_VARIABLE_NAME, &gEfiMemoryTypeInformationGuid);
  Status = VariableLock->RequestToLock (VariableLock, L"TargetHddDevPath", &gEfiGenericVariableGuid);
  return Status;
}
/**

  Hook Boot Services ExitBootServices ().

**/
STATIC
VOID
HookExitBootServices (
  VOID
  )
{
  UINT32                          Crc;

  mOrginalExitBootServices = gBS->ExitBootServices;
  gBS->ExitBootServices = ExitBootServicesHookFunction;
  gBS->Hdr.CRC32 = 0;
  Crc = 0;
  gBS->CalculateCrc32 ((VOID *) gBS, gBS->Hdr.HeaderSize, &Crc);
  gBS->Hdr.CRC32 = Crc;
}

/**

  Initialize firmware vendor and firmware version in system table.

**/
EFI_STATUS
InitFirmwareInfo (
  VOID
  )
{
  CHAR16                          *FirmwareVendor;
  UINT32                          Crc;
  //
  // Fill in FirmwareVendor and FirmwareRevision from PCDs
  //
  FirmwareVendor = (CHAR16 *) PcdGetPtr (PcdFirmwareVendor);
  gST->FirmwareVendor = AllocateRuntimeCopyPool (StrSize (FirmwareVendor), FirmwareVendor);
  ASSERT (gST->FirmwareVendor != NULL);
  if (gST->FirmwareVendor == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  gST->FirmwareRevision = PcdGet32 (PcdFirmwareRevision);
  DEBUG ((EFI_D_ERROR, "EFI Vendor : %s\nEFI Revision : %d.%d\n", gST->FirmwareVendor, gST->FirmwareRevision >> 16, gST->FirmwareRevision & 0xFFFF));
  //
  // Fixup Tasble CRC after we updated Firmware Vendor and Revision
  //
  gST->Hdr.CRC32 = 0;
  gBS->CalculateCrc32 ((VOID *)gST, gST->Hdr.HeaderSize, &Crc);
  gST->Hdr.CRC32 = Crc;

  if (FeaturePcdGet (PcdH2OBdsCpExitBootServicesBeforeSupported)) {
    //
    // Hook ExitBootServices () to trigger H2O_BDS_CP_EXIT_BOOT_SERVICES_BEFORE_DATA
    // check point before entering ExitBootServices ().
    //
    HookExitBootServices ();
  }
  return EFI_SUCCESS;
}

/**
  Initialize the L"OsIndicationsSupported" variable.

  @retval EFI_SUCCESS   Initialize L"OsIndicationsSupported" variable successfully.
  @retval Other         Fail status returned from set variable function.
**/
STATIC
EFI_STATUS
InitOsIndicationsSupportedVariable (
  VOID
  )
{
  UINT64                          OsIndicationsSupported;

  OsIndicationsSupported = PcdGet64 (PcdOsIndicationsSupported);
  return gRT->SetVariable (
                EFI_OS_INDICATIONS_SUPPORT_VARIABLE_NAME,
                &gEfiGlobalVariableGuid,
                EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                sizeof (OsIndicationsSupported),
                &OsIndicationsSupported
                );
}

/**
  An empty function to pass error checking of CreateEventEx ().

  This empty function ensures that EVT_NOTIFY_SIGNAL_ALL is error
  checked correctly since it is now mapped into CreateEventEx() in UEFI 2.0.

  @param  Event                 Event whose notification function is being invoked.
  @param  Context               The pointer to the notification function's context,
                                which is implementation-dependent.

**/
VOID
EFIAPI
EmptyFunction (
  IN EFI_EVENT                Event,
  IN VOID                     *Context
  )
{
  return;
}

/**
  Internal function to initialize  H2O_BDS_CP_INIT_DATA data and trigger gH2OBdsCpInitGuid
  checkpoint.

  @retval EFI_SUCCESS             Trigger gH2OBdsCpInitGuid checkpoint successfully.
  @retval EFI_OUT_OF_RESOURCES    Allocate memory to initialize checkpoint data failed.
  @return Other                   Other error occurred while triggering gH2OBdsCpInitGuid
                                  checkpoint.
**/
STATIC
EFI_STATUS
TriggerCpInit (
  VOID
  )
{

  H2O_BDS_CP_INIT_DATA       BdsInitData;
  EFI_STATUS                 Status;


  BdsInitData.Size   = sizeof (H2O_BDS_CP_INIT_DATA);
  BdsInitData.Status = H2O_CP_TASK_NORMAL;
  DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpInitGuid));
  Status = H2OCpTrigger (&gH2OBdsCpInitGuid, &BdsInitData);
  DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", BdsInitData.Status));
  return Status;
}

/**
 Internal function to initialize H2O_BDS_CP_PLATFORM_INIT_DATA data and trigger gH2OBdsCpPlatformInitGuid checkpoint.

 @retval EFI_SUCCESS             Trigger gH2OBdsCpPlatformInitGuid checkpoint successfully.
 @return Other                   Other error occurred while triggering gH2OBdsCpPlatformInitGuid checkpoint.
**/
STATIC
EFI_STATUS
TriggerCpPlatformInit (
  VOID
  )
{
  H2O_BDS_CP_PLATFORM_INIT_DATA           BdsPlatformInitData;
  EFI_STATUS                              Status;

  BdsPlatformInitData.Size   = sizeof (H2O_BDS_CP_PLATFORM_INIT_DATA);
  BdsPlatformInitData.Status = H2O_CP_TASK_NORMAL;
  DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpPlatformInitGuid));
  Status = H2OCpTrigger (&gH2OBdsCpPlatformInitGuid, &BdsPlatformInitData);
  DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", BdsPlatformInitData.Status));
  return Status;
}

/**
  Internal function to initialize  _H2O_BDS_CP_END_OF_DXE_BEFORE_PROTOCOL data and trigger gH2OBdsCpEndOfDxeBeforeGuid
  checkpoint.

  @retval EFI_SUCCESS             Trigger gH2OBdsCpEndOfDxeBeforeGuid checkpoint successfully.
  @retval EFI_OUT_OF_RESOURCES    Allocate memory to initialize checkpoint data failed.
  @return Other                   Other error occurred while triggering gH2OBdsCpEndOfDxeBeforeGuid
                                  checkpoint.
**/
STATIC
EFI_STATUS
TriggerCpEndOfDxeBefore (
  VOID
  )
{
  H2O_BDS_CP_END_OF_DXE_BEFORE_DATA       BdsEndOfDxeBeforeData;
  EFI_STATUS                              Status;


  BdsEndOfDxeBeforeData.Size   = sizeof (H2O_BDS_CP_END_OF_DXE_BEFORE_DATA);
  BdsEndOfDxeBeforeData.Status = H2O_CP_TASK_NORMAL;
  DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpEndOfDxeBeforeGuid));
  Status = H2OCpTrigger (&gH2OBdsCpEndOfDxeBeforeGuid, &BdsEndOfDxeBeforeData);
  DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", BdsEndOfDxeBeforeData.Status));
  return Status;
}

/**
  Internal function to initialize H2O_BDS_CP_END_OF_DXE_AFTER_DATA data and trigger
  gH2OBdsCpEndOfDxeAfterGuid checkpoint.

  @retval EFI_SUCCESS             Trigger gH2OBdsCpEndOfDxeAfterGuid checkpoint successfully.
  @retval EFI_OUT_OF_RESOURCES    Allocate memory to initialize checkpoint data failed.
  @return Other                   Other error occurred while triggering gH2OBdsCpEndOfDxeAfterGuid
                                  checkpoint.
**/
STATIC
EFI_STATUS
TriggerCpEndOfDxeAfter (
  VOID
  )
{

  H2O_BDS_CP_END_OF_DXE_AFTER_DATA   BdsEndOfDxeAfterData;
  EFI_STATUS                         Status;


  BdsEndOfDxeAfterData.Size   = sizeof (H2O_BDS_CP_END_OF_DXE_AFTER_DATA);
  BdsEndOfDxeAfterData.Status = H2O_CP_TASK_NORMAL;
  DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpEndOfDxeAfterGuid));
  Status = H2OCpTrigger (&gH2OBdsCpEndOfDxeAfterGuid, &BdsEndOfDxeAfterData);
  DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", BdsEndOfDxeAfterData.Status));
  return Status;
}

/**
  Internal function to initialize H2O_BDS_CP_DRIVER_CONNECT_AFTER_DATA data and trigger
  gH2OBdsCpDriverConnectAfterGuid checkpoint.

  @retval EFI_SUCCESS             Trigger gH2OBdsCpDriverConnectAfterGuid checkpoint successfully.
  @retval EFI_OUT_OF_RESOURCES    Allocate memory to initialize checkpoint data failed.
  @return Other                   Other error occurred while triggering gH2OBdsCpDriverConnectAfterGuid
                                  checkpoint.
**/
EFI_STATUS
TriggerCpDriverConnectAfter (
  VOID
  )
{
  H2O_BDS_CP_DRIVER_CONNECT_AFTER_DATA   BdsDriverConnectAfterData;
  EFI_STATUS                             Status;

  BdsDriverConnectAfterData.Size   = sizeof (H2O_BDS_CP_DRIVER_CONNECT_AFTER_DATA);
  BdsDriverConnectAfterData.Status = H2O_CP_TASK_NORMAL;
  DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpDriverConnectAfterGuid));
  Status = H2OCpTrigger (&gH2OBdsCpDriverConnectAfterGuid, &BdsDriverConnectAfterData);
  DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", BdsDriverConnectAfterData.Status));
  return Status;

}

/**
  Internal function to initialize H2O_BDS_CP_DRIVER_CONNECT_BEFORE_DATA data and trigger
  gH2OBdsCpDriverConnectBeforeGuid checkpoint.

  @retval EFI_SUCCESS             Trigger gH2OBdsCpDriverConnectBeforeGuid checkpoint successfully.
  @retval EFI_OUT_OF_RESOURCES    Allocate memory to initialize checkpoint data failed.
  @return Other                   Other error occurred while triggering gH2OBdsCpDriverConnectBeforeGuid
                                  checkpoint.
**/
EFI_STATUS
TriggerCpDriverConnectBefore (
  VOID
  )
{
  H2O_BDS_CP_DRIVER_CONNECT_BEFORE_DATA   BdsDriverConnectBeforeData;
  EFI_STATUS                              Status;

  BdsDriverConnectBeforeData.Size   = sizeof (H2O_BDS_CP_DRIVER_CONNECT_BEFORE_DATA);
  BdsDriverConnectBeforeData.Status = H2O_CP_TASK_NORMAL;
  DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpDriverConnectBeforeGuid));
  Status = H2OCpTrigger (&gH2OBdsCpDriverConnectBeforeGuid, &BdsDriverConnectBeforeData);
  DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", BdsDriverConnectBeforeData.Status));
  return Status;
}


/**

  Service routine for BdsInstance->Entry(). Devices are connected, the
  consoles are initialized, and the boot options are tried.

  @param This             Protocol Instance structure.

**/
VOID
EFIAPI
BdsEntry (
  IN EFI_BDS_ARCH_PROTOCOL  *This
  )
{
  UINTN                                    Size;
  EFI_STATUS                               Status;
  UINT16                                   Timeout;
  EFI_TIMER_ARCH_PROTOCOL                  *Timer;
  EFI_EVENT                                EndOfDxeEvent;
  //
  // PostCode = 0x10, Enter BDS entry
  //
  POST_CODE (BDS_ENTER_BDS);
  //
  // Insert the performance probe
  //
  PERF_END (0, DXE_TOK, NULL, 0);
  PERF_START (0, BDS_TOK, NULL, 0);

  InitializeStringSupport ();
  if (FeaturePcdGet (PcdH2OBdsCpInitSupported)) {
    TriggerCpInit ();
  }
  PERF_CODE (
    BdsAllocateMemoryForPerformanceData ();
  );
  LockSensitiveVariables ();

  //
  // Set the timer tick to 100hz to make whole timer interrupt handler operate in desire frequency
  //
  Status = gBS->LocateProtocol (&gEfiTimerArchProtocolGuid, NULL, (VOID **) &Timer);
  if (!EFI_ERROR (Status)) {
    Timer->SetTimerPeriod (Timer, 100000);
  }

  if (PcdGetBool (PcdEndOfDxeEventSupported)) {
    //
    // According to PI 1.2.1, signal EndOfDxe at end of DXE
    //
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    EmptyFunction,
                    NULL,
                    &gEfiEndOfDxeEventGroupGuid,
                    &EndOfDxeEvent
                    );
    if (!EFI_ERROR (Status)) {
      if (FeaturePcdGet (PcdH2OBdsCpEndOfDxeBeforeSupported)) {
        TriggerCpEndOfDxeBefore ();
      }
      gBS->SignalEvent (EndOfDxeEvent);
      if (FeaturePcdGet (PcdH2OBdsCpEndOfDxeAfterSupported)) {
        TriggerCpEndOfDxeAfter ();
      }
      gBS->CloseEvent (EndOfDxeEvent);
      DEBUG((EFI_D_INFO,"All EndOfDxe callbacks have returned successfully\n"));
    }
  }
  //
  // Do the platform init, can be customized by OEM/IBV
  //
  PERF_START (NULL, "PlatformBds", "BDS", 0);
  InitFirmwareInfo ();
  PlatformBdsInit ();
  if (IsBdsHotKeySupport ()) {
    BdsHotKeyInit ();
  }
  Status = InstallH2OBdsServicesProtocol ();
  if (!EFI_ERROR (Status)) {
    if (FeaturePcdGet (PcdH2OBdsCpPlatformInitSupported)) {
      TriggerCpPlatformInit ();
    }
  }

  //
  // BdsLibGetTimeout() will initialize Timeout variable if variable is not exist.
  //
  Size = sizeof (Timeout);
  Status = gRT->GetVariable (
                  L"Timeout",
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &Size,
                  &Timeout
                  );
  if (EFI_ERROR (Status)) {
    Timeout = (UINT16) PcdGet16 (PcdPlatformBootTimeOut);
    Status = gRT->SetVariable (
                    L"Timeout",
                    &gEfiGlobalVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    sizeof (Timeout),
                    &Timeout
                    );
  }
  InitOsIndicationsSupportedVariable ();
  ReportSysPrepCapability ();

  UpdateBootMessage ();

  //
  // Set up the device list based on EFI 1.1 variables
  // process Driver#### and Load the driver's in the
  // driver option list
  //
  if (FeaturePcdGet (PcdH2OBdsCpDriverConnectBeforeSupported)) {
    TriggerCpDriverConnectBefore ();
  }
  BdsLibLaunchDrivers ();
  if (FeaturePcdGet (PcdH2OBdsCpDriverConnectAfterSupported)) {
    TriggerCpDriverConnectAfter ();
  }
  //
  // Check if we have the boot next option
  //
  mBootNext = BdsLibGetVariableAndSize (
                L"BootNext",
                &gEfiGlobalVariableGuid,
                &Size
                );

  SetSimpBootFlag ();
  if (BdsLibGetBootType () == LEGACY_BOOT_TYPE) {
    BdsLibSkipEbcDispatch ();
  }

  //
  // Setup some platform policy here
  //
  PlatformBdsPolicyBehavior (NULL, NULL, BdsProcessCapsules, BdsMemoryTest);
  PERF_END (NULL, "PlatformBds", "BDS", 0);

  //
  // BDS select the boot device to load OS
  //
  BdsBootDeviceSelect ();

  //
  // Only assert here since this is the right behavior, we should never
  // return back to DxeCore.
  //
  ASSERT (FALSE);

  return ;
}

