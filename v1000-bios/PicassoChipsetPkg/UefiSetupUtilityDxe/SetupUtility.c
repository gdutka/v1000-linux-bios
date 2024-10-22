/** @file

;******************************************************************************
;* Copyright (c) 2015 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "SetupUtility.h"

#include <Protocol/SetupMouse.h>
#include <Protocol/GlobalNvsArea.h>

#include <Guid/NvVariableFail.h>

#include <IndustryStandard/SmBios.h>
#include <Protocol/Smbios.h>
#include <Include/SetupConfig.h>
#include <Dxe/AmdPbsSetupDxe/AmdPbsConfig.h>
#include <Library/AmdPbsConfigLib/AmdPbsConfigLib.h>
#include <Library/AmdCbsVariable.h>
#include <Guid/AmdCbsConfig.h>




EFI_STATUS
EFIAPI
LoadAmdPbsDefault (
  OUT AMD_PBS_SETUP_OPTION      *AmdPbsConfiguration
  );

VOID
CbsWriteDefalutValue (
  IN UINT8                      *IfrData
  );

EFI_STATUS
GetOptionalStringByIndex (
  IN  CHAR8                        *OptionalStrStart,
  IN  UINT8                        Index,
  OUT CHAR16                       **String
  );


STATIC
EFI_STATUS
UpdateMainVfrLabel (
  IN  EFI_HII_HANDLE       HiiHandle
);

extern UINT32 mScuRecord;

UINT8 mFullResetFlag = 0;
EFI_HII_STRING_PROTOCOL         *gIfrLibHiiString;
#pragma pack(1)
typedef struct {
  VENDOR_DEVICE_PATH             VendorDevicePath;
  UINT32                         Reserved;
  UINT64                         UniqueId;
} HII_VENDOR_DEVICE_PATH_NODE;
#pragma pack()

typedef struct {
  HII_VENDOR_DEVICE_PATH_NODE    Node;
  EFI_DEVICE_PATH_PROTOCOL       End;
} HII_TEMP_DEVICE_PATH;

//
// Hii vendor device path template
//
HII_TEMP_DEVICE_PATH  mHiiVendorDevicePathTemplate = {
  {
    {
      {
        HARDWARE_DEVICE_PATH,
        HW_VENDOR_DP,
        (UINT8) (sizeof (HII_VENDOR_DEVICE_PATH_NODE)),
        (UINT8) ((sizeof (HII_VENDOR_DEVICE_PATH_NODE)) >> 8)
      },
      EFI_IFR_TIANO_GUID,
    },
    0,
    0
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    END_DEVICE_PATH_LENGTH,
    0
  }
};

/**
 Check the formset in the input HII form packge is supported formset or not.

 @param[in] HiiFormPackage            Pointer to the HII form package

 @retval TRUE                         The formset of input HII form packge is suppoted formset
 @retval FALSE                        The formset of input HII form packge is suppoted formset
**/
STATIC
BOOLEAN
IsSupportedFormSet (
  IN EFI_HII_PACKAGE_LIST_HEADER      *HiiFormPackage
  )
{
  EFI_IFR_FORM_SET                    *FormSetPtr;
  EFI_IFR_GUID_CLASS                  *ClassOpCode;
  EFI_IFR_GUID_SUBCLASS               *SubClassOpCode;
  EFI_GUID                            *ClassGuid;
  UINT8                               ClassGuidNum;
  UINT8                               Index;
  EFI_GUID                            ScuClassGuid = SETUP_UTILITY_FORMSET_CLASS_GUID;

  FormSetPtr = (EFI_IFR_FORM_SET *) ((EFI_HII_FORM_PACKAGE_HDR *) (HiiFormPackage + 1) + 1);

  //
  // If the formset (which class GUID contains HII platform setup) belongs in the device manager,
  // need to check class and subclass to determine it is supported formset or not.
  //
  ClassGuid    = (EFI_GUID *) ((UINT8 *) FormSetPtr + sizeof (EFI_IFR_FORM_SET));
  ClassGuidNum = (UINT8) (FormSetPtr->Flags & 0x3);
  for (Index = 0; Index < ClassGuidNum; Index++, ClassGuid++) {
    if (!CompareGuid (ClassGuid, &ScuClassGuid)) {
      continue;
    }

    ClassOpCode    = (EFI_IFR_GUID_CLASS    *) ((UINT8 *) FormSetPtr  + FormSetPtr->Header.Length);
    SubClassOpCode = (EFI_IFR_GUID_SUBCLASS *) ((UINT8 *) ClassOpCode + ClassOpCode->Header.Length);

    if (ClassOpCode->Header.OpCode    == EFI_IFR_GUID_OP && CompareGuid ((EFI_GUID *)((UINT8 *) ClassOpCode    + sizeof (EFI_IFR_OP_HEADER)), &gEfiIfrTianoGuid) &&
        SubClassOpCode->Header.OpCode == EFI_IFR_GUID_OP && CompareGuid ((EFI_GUID *)((UINT8 *) SubClassOpCode + sizeof (EFI_IFR_OP_HEADER)), &gEfiIfrTianoGuid)) {
      if (ClassOpCode->Class == EFI_NON_DEVICE_CLASS) {
        return FALSE;
      }

      if (SubClassOpCode->SubClass == EFI_USER_ACCESS_THREE &&
          ((CHIPSET_CONFIGURATION *)gSUBrowser->SCBuffer)->SetUserPass == 1 &&
          ((CHIPSET_CONFIGURATION *)gSUBrowser->SCBuffer)->UserAccessLevel == 3) {
        return FALSE;
      }

      if (SubClassOpCode->SubClass == EFI_USER_ACCESS_TWO &&
          ((CHIPSET_CONFIGURATION *)gSUBrowser->SCBuffer)->SetUserPass == 1 &&
          ((CHIPSET_CONFIGURATION *)gSUBrowser->SCBuffer)->UserAccessLevel == 2) {
        return FALSE;
      }
    }
  }

  return TRUE;
}

/**
 The HII driver handle passed in for HiiDatabase.NewPackageList() requires
 that there should be DevicePath Protocol installed on it.
 This routine create a virtual Driver Handle by installing a vendor device
 path on it, so as to use it to invoke HiiDatabase.NewPackageList().

 @param [in]     DriverHandle         Handle to be returned

 @retval EFI_SUCCESS            Handle destroy success.
 @retval EFI_OUT_OF_RESOURCES   Not enough memory.

**/
EFI_STATUS
CreateHiiDriverHandle (
  OUT EFI_HANDLE               *DriverHandle
  )
{
  EFI_STATUS                   Status;
  HII_VENDOR_DEVICE_PATH_NODE  *VendorDevicePath;

  VendorDevicePath = AllocateCopyPool (sizeof (HII_TEMP_DEVICE_PATH), &mHiiVendorDevicePathTemplate);
  if (VendorDevicePath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Use memory address as unique ID to distinguish from different device paths
  //
  VendorDevicePath->UniqueId = (UINT64) ((UINTN) VendorDevicePath);

  *DriverHandle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  VendorDevicePath,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
 Destroy the Driver Handle created by CreateHiiDriverHandle().

 @param [in]     DriverHandle   Handle returned by CreateHiiDriverHandle()

 @retval EFI_SUCCESS            Handle destroy success.
 @return other                  Handle destroy fail.

**/
EFI_STATUS
DestroyHiiDriverHandle (
  IN EFI_HANDLE                 DriverHandle
  )
{
  EFI_STATUS                   Status;
  EFI_DEVICE_PATH_PROTOCOL     *DevicePath;

  Status = gBS->HandleProtocol (
                  DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **)&DevicePath
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->UninstallProtocolInterface (
                  DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  DevicePath
                  );
  gBS->FreePool (DevicePath);
  return Status;
}

USER_UNINSTALL_CALLBACK_ROUTINE mUninstallCallbackRoutine[] ={
  UninstallExitCallbackRoutine,     UninstallBootCallbackRoutine,
  UninstallPowerCallbackRoutine,    UninstallSecurityCallbackRoutine,
  UninstallAdvanceCallbackRoutine,  UninstallMainCallbackRoutine
//   UninstallVersionCallbackRoutine,
  };

EFI_GUID  mFormSetGuid = SYSTEM_CONFIGURATION_GUID;
EFI_GUID  mFormSetClassGuid = SETUP_UTILITY_FORMSET_CLASS_GUID;
CHAR16    mVariableName[] = L"SystemConfig";
EFI_HII_HANDLE                   mDriverHiiHandle;

EFI_STATUS
InitSetupUtilityBrowser(
  IN  EFI_SETUP_UTILITY_PROTOCOL            *This
);

VOID
NotifyFormCallbackProtocol (
  IN  EFI_HANDLE                            ImageHandle
);

static
EFI_STATUS
UpdateSystemBusSpeed (
  IN  EFI_HII_HANDLE       HiiHandle
);

SETUP_UTILITY_BROWSER_DATA                   *gSUBrowser;
UINT16                                       gCallbackKey;


EFI_STATUS
EFIAPI
SetupUtilityInit (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS                            Status;
  SETUP_UTILITY_DATA                    *SetupData;
//  UINT32                                DummyVersion;
  EFI_EVENT                             Event;
  VOID                                  *Registration;


  //
  // Initialize the library.
  //
  CheckLanguage ();

  SetupData = AllocatePool (sizeof(SETUP_UTILITY_DATA));
  if (SetupData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  SetupData->Signature                    = EFI_SETUP_UTILITY_SIGNATURE;
  SetupData->SetupUtility.StartEntry      = NULL;
  SetupData->SetupUtility.PowerOnSecurity = PowerOnSecurity;
  mFullResetFlag = 0;

  //
  // The FirstIn flag is for that if there is the first time entering SCU, we should install
  // Vfr of Menu to Hii database.
  // After that, we shouldn't install the Vfr to Hii database again.
  //
  SetupData->SetupUtility.FirstIn         = TRUE;
  Status = GetSystemConfigurationVar (SetupData);

  //
  // Install Setup Utility
  //
  SetupData->Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                 &SetupData->Handle,
                 &gEfiSetupUtilityProtocolGuid,
                 EFI_NATIVE_INTERFACE,
                 &SetupData->SetupUtility
                 );

  if (EFI_ERROR (Status)) {
    gBS->FreePool (SetupData);
    return Status;
  }

  NotifyFormCallbackProtocol (ImageHandle);

  //
  // When execute Setup Utility application, install HII data
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK - 1,
                  SetupUtilityNotifyFn,
                  NULL,
                  &Event
                  );
  if (!EFI_ERROR (Status)) {
    Status = gBS->RegisterProtocolNotify (
                    &gEfiSetupUtilityApplicationProtocolGuid,
                    Event,
                    &Registration
                    );
  }

  return Status;
}

EFI_STATUS
CreateScuData (
  BOOLEAN  DoClearScreen
  )
{
  EFI_STATUS                                Status;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL        *Interface;
  UINTN                                     BufferSize;
  UINT8                                     *Lang = NULL;
  EFI_SETUP_UTILITY_PROTOCOL                *This;

  Status = gBS->LocateProtocol (
                  &gEfiSetupUtilityBrowserProtocolGuid,
                  NULL,
                  (VOID **)&Interface
                  );
  //
  // If there was no error, assume there is an installation and fail to load
  //
  if (EFI_ERROR(Status) ) {
    Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **) &This);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = InitSetupUtilityBrowser(This);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = InstallSetupUtilityBrowserProtocol (This);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    ZeroMem (gSUBrowser->SUCInfo->MapTable, sizeof (HII_HANDLE_VARIABLE_MAP_TABLE) * MAX_HII_HANDLES);
    PlugInVgaUpdateInfo ();

    Status = InstallHiiData ();
    This->FirstIn = FALSE;
    if (EFI_ERROR(Status)) {
      return Status;
    }

    //
    // Load the variable data records
    //
    gSUBrowser->Interface.SCBuffer = (UINT8 *) gSUBrowser->SCBuffer;
//     Status = SetBufferToAmdSystemConfig0 ();
  }

  Lang = GetVariableAndSize (
           L"PlatformLang",
           &gEfiGlobalVariableGuid,
           &BufferSize
           );
  Status = gRT->SetVariable (
                  L"BackupPlatformLang",
                  &gEfiGenericVariableGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  BufferSize,
                  Lang
                  );
  gBS->FreePool (Lang);

  return Status;
}


EFI_STATUS
DestroyScuData (
  BOOLEAN  DoClearScreen
  )
{
  EFI_STATUS                                Status;
  UINTN                                     BufferSize;
  UINT8                                     *Lang;
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL         *SysPasswordService;
  EFI_SETUP_UTILITY_PROTOCOL                *This;

  Status = gBS->LocateProtocol (
                  &gEfiSysPasswordServiceProtocolGuid,
                  NULL,
                  (VOID **)&SysPasswordService
                  );
  ASSERT_EFI_ERROR (Status);
  SysPasswordService->LockPassword (SysPasswordService);

  Status = RemoveHiiData (
             SetupUtilityStrings,
             (HII_HANDLE_VARIABLE_MAP_TABLE *) &(gSUBrowser->SUCInfo->MapTable[ExitHiiHandle]),
//              (HII_HANDLE_VARIABLE_MAP_TABLE *) &(gSUBrowser->SUCInfo->MapTable[AmdCbsHiiHandle]),
             (HII_HANDLE_VARIABLE_MAP_TABLE *) &(gSUBrowser->SUCInfo->MapTable[BootHiiHandle]),
             (HII_HANDLE_VARIABLE_MAP_TABLE *) &(gSUBrowser->SUCInfo->MapTable[PowerHiiHandle]),
             (HII_HANDLE_VARIABLE_MAP_TABLE *) &(gSUBrowser->SUCInfo->MapTable[SecurityHiiHandle]),
             (HII_HANDLE_VARIABLE_MAP_TABLE *) &(gSUBrowser->SUCInfo->MapTable[AdvanceHiiHandle]),
             (HII_HANDLE_VARIABLE_MAP_TABLE *) &(gSUBrowser->SUCInfo->MapTable[MainHiiHandle]),
//              (HII_HANDLE_VARIABLE_MAP_TABLE *) &(gSUBrowser->SUCInfo->MapTable[VersionHiiHandle]),
             NULL
             );

  Status = UninstallSetupUtilityBrowserProtocol ();
  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **) &This);
  if (!EFI_ERROR(Status)) {
    This->FirstIn = TRUE;
  }
  gSUBrowser = NULL;
  if (DoClearScreen) {
    if (gST->ConOut != NULL) {
      gST->ConOut->SetAttribute (gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
      gST->ConOut->ClearScreen (gST->ConOut);
    }
  }
  Lang = GetVariableAndSize (
           L"BackupPlatformLang",
           &gEfiGenericVariableGuid,
           &BufferSize
           );
  if (Lang != NULL) {
    Status = gRT->SetVariable (
                    L"PlatformLang",
                    &gEfiGlobalVariableGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    BufferSize,
                    Lang
                    );
    gBS->FreePool (Lang);

    gRT->SetVariable (
           L"BackupPlatformLang",
           &gEfiGenericVariableGuid,
           EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
           0,
           NULL
           );
  }

  return Status;
}

BOOLEAN
AsfResetSetupData (
  IN  VOID
  )
{
  EFI_STATUS                          Status;
  EFI_ALERT_STANDARD_FORMAT_PROTOCOL  *Asf;
  EFI_ASF_BOOT_OPTIONS                *mAsfBootOptions;

  //
  // Get Protocol for ASF
  //
  Status = gBS->LocateProtocol (
                      &gEfiAlertStandardFormatProtocolGuid,
                      NULL,
                      (VOID **)&Asf
                      );

  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Error gettings ASF protocol -> %r\n", Status));
    return FALSE;
  }

  Status = Asf->GetBootOptions(Asf, &mAsfBootOptions);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  if (mAsfBootOptions->SubCommand != ASF_BOOT_OPTIONS_PRESENT) {
    return FALSE;
  }

  if ((mAsfBootOptions->BootOptions & CONFIG_DATA_RESET) == CONFIG_DATA_RESET) {
    return TRUE;
  }

  return FALSE;
}

EFI_STATUS
EFIAPI
PowerOnSecurity (
  IN  EFI_SETUP_UTILITY_PROTOCOL        *SetupUtility
  )
{
  EFI_STATUS                            Status;
  EFI_HII_HANDLE                        HiiHandle;
  EFI_GUID                              StringPackGuid = STRING_PACK_GUID;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo = NULL;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL    *Interface;
  EFI_HANDLE                            DriverHandle;
  VOID                                  *Table;
  UINT8                                 *HobSetupData;
  VOID                                  *HobList = NULL;
  EFI_BOOT_MODE                         BootMode;

  HiiHandle = 0;

  //
  // There will be only one DeviceManagerSetup in the system.
  // If there is another out there, someone is trying to install us
  // again.  We fail in that scenario.
  //
  Status = gBS->LocateProtocol (
                  &gEfiSetupUtilityBrowserProtocolGuid,
                  NULL,
                  (VOID **)&Interface
                  );

  //
  // If there was no error, assume there is an installation and fail to load
  //
  if (!EFI_ERROR(Status) ) {
    Status = UninstallSetupUtilityBrowserProtocol ();
  }

  Status = InitSetupUtilityBrowser(SetupUtility);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = InstallSetupUtilityBrowserProtocol (SetupUtility);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  SUCInfo = gSUBrowser->SUCInfo;
  Status  = CreateHiiDriverHandle (&DriverHandle);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  HiiHandle = HiiAddPackages (&StringPackGuid, DriverHandle, SecurityVfrBin ,SetupUtilityLibStrings, NULL);
  ASSERT(HiiHandle != NULL);

  gSUBrowser->SUCInfo->MapTable[SecurityHiiHandle].HiiHandle = HiiHandle;
  gSUBrowser->Interface.SCBuffer = (UINT8 *) gSUBrowser->SCBuffer;

  Status = gBS->LocateProtocol (
                  &gEfiSysPasswordServiceProtocolGuid,
                  NULL,
                  (VOID **)&SUCInfo->SysPasswordService
                  );

  SUCInfo->SupervisorPassword = NULL;
  SUCInfo->UserPassword       = NULL;

  if (!EFI_ERROR(Status)) {
    //
    // Check password
    //
    BootMode = GetBootModeHob();
    if (BootMode == BOOT_IN_RECOVERY_MODE) {
      HobList = GetHobList ();
      Table= GetNextGuidHob (&gEfiPowerOnPwSCUHobGuid, HobList);
      if (Table != NULL) {
        HobSetupData = ((UINT8 *) Table) + sizeof (EFI_HOB_GUID_TYPE);
        ((CHIPSET_CONFIGURATION *)gSUBrowser->SCBuffer)->PowerOnPassword = ((CHIPSET_CONFIGURATION *) HobSetupData)->PowerOnPassword;
      }
    }
    if ((((CHIPSET_CONFIGURATION *) gSUBrowser->SCBuffer)->PowerOnPassword) == POWER_ON_PASSWORD) {
      Status = PasswordCheck (
                 SUCInfo,
                 (KERNEL_CONFIGURATION *) gSUBrowser->SCBuffer
                 );
      ASSERT_EFI_ERROR (Status);
    }
  }
  
  if (SUCInfo->SupervisorPassword  != NULL) {
    if (SUCInfo->SupervisorPassword->NumOfEntry != 0) {
//[-start-121120-IB08520065-add]//
      gBS->FreePool (SUCInfo->SupervisorPassword->InputString);
//[-end-121120-IB08520065-add]//
      gBS->FreePool (SUCInfo->SupervisorPassword);
      SUCInfo->SupervisorPassword = NULL;
     }
  }

  if (SUCInfo->UserPassword  != NULL) {
    if (SUCInfo->UserPassword->NumOfEntry != 0) {
//[-start-121120-IB08520065-add]//
      gBS->FreePool (SUCInfo->UserPassword->InputString);
//[-end-121120-IB08520065-add]//
      gBS->FreePool (SUCInfo->UserPassword);
      SUCInfo->UserPassword = NULL;
     }
  }

  gSUBrowser->HiiDatabase->RemovePackageList (gSUBrowser->HiiDatabase, HiiHandle);
  DestroyHiiDriverHandle (DriverHandle);
  Status = UninstallSetupUtilityBrowserProtocol ();

  return Status;
}

/**
  Installs the SetupUtilityBrowser protocol including allocating
  storage for variable record data.

  @param [in]   This

  @retval EFI_SUCEESS            Protocol was successfully installed
  @retval EFI_OUT_OF_RESOURCES   Not enough resource to allocate data structures
  @return Other                  Some other error occured
**/
EFI_STATUS
InstallSetupUtilityBrowserProtocol (
  IN  EFI_SETUP_UTILITY_PROTOCOL            *This
  )
{
  EFI_STATUS                                 Status;

  gSUBrowser->Interface.AtRoot        = TRUE;
  gSUBrowser->Interface.Finished      = FALSE;
  gSUBrowser->Interface.Guid          = &mFormSetGuid;
  gSUBrowser->Interface.UseMenus      = FALSE;
  gSUBrowser->Interface.Direction     = NoChange;
  gSUBrowser->Interface.CurRoot       = FALSE;
  gSUBrowser->Interface.MenuItemCount = FALSE;
  gSUBrowser->Interface.Size          = PcdGet32 (PcdSetupConfigSize);
  gSUBrowser->Interface.Firstin       = TRUE;
  gSUBrowser->Interface.Changed       = FALSE;
  gSUBrowser->Interface.Reload         = FALSE;
//   gSUBrowser->Interface.ChangeLanguage = FALSE;
  gSUBrowser->Interface.JumpToFirstOption = TRUE;

  gSUBrowser->Interface.SCBuffer      = NULL;
  gSUBrowser->Interface.MyIfrNVData   = NULL;
  gSUBrowser->Interface.PreviousMenuEntry = 0;
  //
  // Install the Protocol
  //
  gSUBrowser->Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &gSUBrowser->Handle,
                  &gEfiSetupUtilityBrowserProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &gSUBrowser->Interface
                  );

  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

/**
  Uninstalls the DeviceManagerSetup protocol and frees memory
  used for storing variable record data.

  @param None

  @retval EFI_SUCEESS            Protocol was successfully installed
  @retval EFI_ALREADY_STARTED    Protocol was already installed
  @retval EFI_OUT_OF_RESOURCES   Not enough resource to allocate data structures
  @return Other                  Some other error occured
**/
EFI_STATUS
UninstallSetupUtilityBrowserProtocol (
  VOID
  )
{
  EFI_STATUS                                Status;

  Status = gBS->UninstallProtocolInterface (
                  gSUBrowser->Handle,
                  &gEfiSetupUtilityBrowserProtocolGuid,
                  &gSUBrowser->Interface
                  );

  if ( EFI_ERROR(Status) ) {
    return Status;
  }

  if (gSUBrowser->SCBuffer != NULL) {
    gBS->FreePool (gSUBrowser->SCBuffer);
  }

  if (gSUBrowser->SUCInfo->HddPasswordScuData  != NULL) {
    if (gSUBrowser->SUCInfo->HddPasswordScuData->NumOfEntry != 0) {
      if (gSUBrowser->SUCInfo->HddPasswordScuData[0].HddInfo != NULL) {
        gBS->FreePool (gSUBrowser->SUCInfo->HddPasswordScuData[0].HddInfo);
      }
      gBS->FreePool (gSUBrowser->SUCInfo->HddPasswordScuData);
    }
  }
  if (gSUBrowser->SUCInfo->SupervisorPassword  != NULL) {
    if (gSUBrowser->SUCInfo->SupervisorPassword->NumOfEntry != 0) {
//[-start-121120-IB08520065-add]//
      gBS->FreePool (gSUBrowser->SUCInfo->SupervisorPassword->InputString);
//[-end-121120-IB08520065-add]//
      gBS->FreePool (gSUBrowser->SUCInfo->SupervisorPassword);
    }
  }

  if (gSUBrowser->SUCInfo->UserPassword  != NULL) {
    if (gSUBrowser->SUCInfo->UserPassword->NumOfEntry != 0) {
//[-start-121120-IB08520065-add]//
      gBS->FreePool (gSUBrowser->SUCInfo->UserPassword->InputString);
//[-end-121120-IB08520065-add]//
      gBS->FreePool (gSUBrowser->SUCInfo->UserPassword);
    }
  }
  gBS->FreePool (gSUBrowser->SUCInfo);
  gBS->FreePool(gSUBrowser);

  return EFI_SUCCESS;
}

/**
  Call the browser and display the SetupUtility

  @param  None.
**/
EFI_STATUS
CallSetupUtilityBrowser (
  VOID
  )
{
  EFI_STATUS                                Status;
  UINTN                                     BufferSize;
  EFI_HANDLE                                DispPage;
  BOOLEAN                                   SetupUtilityBrowserEmpty;
  BOOLEAN                                   Continue;
  EFI_BROWSER_ACTION_REQUEST                ResetRequired;
  EFI_STRING_ID                             TempToken;
  UINTN                                     Index;
  EFI_HII_PACKAGE_LIST_HEADER               *Buffer;
  EFI_HII_DATABASE_PROTOCOL                 *HiiDatabase;
  EFI_FORM_BROWSER2_PROTOCOL                *Browser2;
  EFI_SETUP_MOUSE_PROTOCOL                  *SetupMouse;
  EFI_IFR_FORM_SET                          *FormSetPtr;
  UINT8                                     *TempPtr;


  Status = EFI_SUCCESS;
  SetupUtilityBrowserEmpty = TRUE;
  Buffer = NULL;
  gCallbackKey = 0;
  Continue = TRUE;
  HiiDatabase = gSUBrowser->HiiDatabase;
  Browser2    = gSUBrowser->Browser2;
  SetupMouse = NULL;

  for (Index = 0; Index < MAX_HII_HANDLES && gSUBrowser->SUCInfo->MapTable[Index].HiiHandle != NULL; Index++) {
    //
    // Am not initializing Buffer since the first thing checked is the size
    // this way I can get the real buffersize in the smallest code size
    //
    BufferSize = 0;
    Status = HiiDatabase->ExportPackageLists (
                            HiiDatabase,
                            gSUBrowser->SUCInfo->MapTable[Index].HiiHandle,
                            &BufferSize,
                            Buffer
                            );

    if (Status == EFI_NOT_FOUND) {
      break;
    }

    //
    // BufferSize should have the real size of the forms now
    //
    Buffer = AllocateZeroPool (BufferSize);
    ASSERT (Buffer != NULL);

    //
    // Am not initializing Buffer since the first thing checked is the size
    // this way I can get the real buffersize in the smallest code size
    //
    Status = HiiDatabase->ExportPackageLists (
                            HiiDatabase,
                            gSUBrowser->SUCInfo->MapTable[Index].HiiHandle,
                            &BufferSize,
                            Buffer
                            );
    if (!IsSupportedFormSet (Buffer)) {
      gBS->FreePool (Buffer);
      continue;
    }

    //
    // Skips the header, now points to data
    //

    TempPtr = (UINT8 *) (Buffer + 1);
    TempPtr = (UINT8 *) ((EFI_HII_FORM_PACKAGE_HDR *) TempPtr + 1);
    FormSetPtr = (EFI_IFR_FORM_SET *) TempPtr;
    SetupUtilityBrowserEmpty = FALSE;


    if (gSUBrowser->Interface.MenuItemCount < MAX_ITEMS) {
      TempToken = FormSetPtr->FormSetTitle;
      gSUBrowser->Interface.MenuList[gSUBrowser->Interface.MenuItemCount].MenuTitle = TempToken;
      //
      // Set the display page.  Last page found is the first to be displayed.
      //
      gSUBrowser->Interface.MenuList[gSUBrowser->Interface.MenuItemCount].Page = gSUBrowser->SUCInfo->MapTable[Index].HiiHandle;
      //
      // NULL out the string pointer
      //
      gSUBrowser->Interface.MenuList[gSUBrowser->Interface.MenuItemCount].String = NULL;
      //
      // Entry is filled out so update count
      //
      gSUBrowser->Interface.MenuItemCount++;
    }
    gBS->FreePool(Buffer);
  }

  //
  // Drop the TPL level from EFI_TPL_DRIVER to EFI_TPL_APPLICATION
  //
  gBS->RestoreTPL (TPL_APPLICATION);

  //
  // If we didn't add anything - don't bother going to device manager
  //
  if (!SetupUtilityBrowserEmpty) {
    Status = gBS->LocateProtocol (
                    &gSetupMouseProtocolGuid,
                    NULL,
                    (VOID **)&(gSUBrowser->Interface.SetupMouse)
                    );
    if (!EFI_ERROR (Status)) {
      gSUBrowser->Interface.SetupMouseFlag = TRUE;
      SetupMouse = (EFI_SETUP_MOUSE_PROTOCOL *) gSUBrowser->Interface.SetupMouse;
      Status = SetupMouse->Start (SetupMouse);
      if (EFI_ERROR (Status)) {
        SetupMouse = NULL;
        gSUBrowser->Interface.SetupMouseFlag = FALSE;
      }
    }
    //
    // Init before root page loop
    //
    gSUBrowser->Interface.AtRoot    = TRUE;
    gSUBrowser->Interface.CurRoot   = gSUBrowser->Interface.MenuItemCount - 1;
    gSUBrowser->Interface.UseMenus  = TRUE;
    gSUBrowser->Interface.Direction = NoChange;
    DispPage                        = gSUBrowser->Interface.MenuList[gSUBrowser->Interface.MenuItemCount - 1].Page;

    //
    // Loop until exit condition is found.  Use direction indicators and
    // the menu list to determine what root page needs to be displayed.
    //
    while (Continue) {
      Status = Browser2->SendForm (
                           Browser2,
                           (EFI_HII_HANDLE *) &DispPage,
                           1,
                           &mFormSetClassGuid,
                           1,
                           NULL,
                           &ResetRequired
                           );

      if (ResetRequired == EFI_BROWSER_ACTION_REQUEST_RESET) {
        gBS->RaiseTPL (TPL_NOTIFY);
        gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
      } else if (ResetRequired == EFI_BROWSER_ACTION_REQUEST_EXIT) {
        gSUBrowser->Interface.UseMenus = FALSE;
        break;
      }

      //
      // Force return to Device Manager or Exit if finished
      //
      gSUBrowser->Interface.AtRoot  = TRUE;
      gSUBrowser->Interface.Firstin = FALSE;
      if (gSUBrowser->Interface.Finished) {
        // Need to set an exit at this point
        gSUBrowser->Interface.UseMenus = FALSE;
        Continue = FALSE;
        break;
      }

      //
      // Check for next page or exit states
      //
      switch (gSUBrowser->Interface.Direction) {

     case Right:
        //
        // Update Current Root Index
        //
        if (gSUBrowser->Interface.CurRoot == 0) {
          gSUBrowser->Interface.CurRoot = gSUBrowser->Interface.MenuItemCount - 1;
        } else {
          gSUBrowser->Interface.CurRoot--;
        }
        //
        // Set page to display
        //
        DispPage = gSUBrowser->Interface.MenuList[gSUBrowser->Interface.CurRoot].Page;
        //
        // Update Direction Flag
        //
        gSUBrowser->Interface.Direction = NoChange;
        break;

      case Left:
        //
        // Update Current Root Index
        //
        if (gSUBrowser->Interface.CurRoot == gSUBrowser->Interface.MenuItemCount - 1) {
          gSUBrowser->Interface.CurRoot = 0;
        } else {
          gSUBrowser->Interface.CurRoot++;
        }
        //
        // Set page to display
        //
        DispPage = gSUBrowser->Interface.MenuList[gSUBrowser->Interface.CurRoot].Page;
        //
        // Update Direction Flag
        //
        gSUBrowser->Interface.Direction = NoChange;
        break;

      case Jump:
        //
        // Update Current Root Index
        //
        if ((gSUBrowser->Interface.CurRoot >= 0) &&
            (gSUBrowser->Interface.CurRoot <= gSUBrowser->Interface.MenuItemCount - 1)) {
          //
          // Set page to display
          //
          DispPage = gSUBrowser->Interface.MenuList[gSUBrowser->Interface.CurRoot].Page;
        }
        gSUBrowser->Interface.Direction = NoChange;
        break;

      default:
        break;
      }

    }
  }
  if ((SetupMouse != NULL) && (gSUBrowser->Interface.SetupMouseFlag == TRUE)) {
    Status = SetupMouse->Close (SetupMouse);
  }
  //
  // We are exiting so clear the screen
  //
  gST->ConOut->SetAttribute (gST->ConOut, EFI_BLACK | EFI_BACKGROUND_BLACK);
  gST->ConOut->ClearScreen (gST->ConOut);

  gBS->RaiseTPL (TPL_APPLICATION + 2);   // TPL_APPLICATION+2 = EFI_TPL_DRIVER

  return Status;
}

EFI_STATUS
GetSystemConfigurationVar (
  IN SETUP_UTILITY_DATA                     *SetupData
  )
{
  EFI_STATUS                                Status;
  UINTN                                     BufferSize;
  CHIPSET_CONFIGURATION                     *SetupNvData;
  BOOLEAN                                   LoadVariableDefault;
  UINT16                                    Timeout;
  AMD_PBS_SETUP_OPTION                      AmdPbsConfiguration;
  UINT8                                     *AmdCbsConfig;

  LoadVariableDefault = FALSE;

  if ( FeaturePcdGet (PcdNvVariableFailDetect)) {
    VOID                      *HobList;

    Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
    ASSERT_EFI_ERROR (Status);

    HobList = GetNextGuidHob (&gNvVariableFailGuid, HobList);
    if (HobList != NULL ) {
      LoadVariableDefault = TRUE;
    }
  }

  //
  // Check the setup variable was create or not, if not then create default setup variable.
  //
  BufferSize = PcdGet32 (PcdSetupConfigSize);

  Status = gBS->AllocatePool(
                  EfiACPIMemoryNVS,
                  BufferSize,
                  (VOID **)&SetupNvData
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }
  ZeroMem (SetupNvData, BufferSize);

  //
  // Check "Setup" variable is exist or not...
  //
  BufferSize = PcdGet32 (PcdSetupConfigSize);
  if (AsfResetSetupData() == FALSE) {
    Status = gRT->GetVariable (
                    L"Setup",
                    &mFormSetGuid,
                    NULL,
                    &BufferSize,
                    (VOID *) SetupNvData
                    );
    if (Status == EFI_SUCCESS && !IsVariableInVariableStoreRegion (L"Setup", &mFormSetGuid)) {
      Status = EFI_NOT_FOUND;
    }    
  } else {
    Status = EFI_NOT_FOUND;
  }

  if (EFI_ERROR (Status) || LoadVariableDefault) {
    //
    //"Setup" Variable doesn't exist,so get a buffer with default variable
    //

    DefaultSetup (SetupNvData);
    gRT->SetVariable (
           L"CustomPlatformLang",
           &gEfiGenericVariableGuid,
           EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
           AsciiStrSize ((CHAR8 *) PcdGetPtr (PcdUefiVariableDefaultPlatformLang)),
           (VOID *) PcdGetPtr (PcdUefiVariableDefaultPlatformLang)
           );

    Timeout = (UINT16) PcdGet16 (PcdPlatformBootTimeOut);
    gRT->SetVariable (
           L"Timeout",
           &gEfiGlobalVariableGuid,
           EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
           sizeof (UINT16),
           (VOID *) &Timeout
           );
    //
    // Load default required
    //
    SetupRuntimeDetermination (SetupNvData);
  }

  //
  // Update variable in every boot (e.g. MemorySize change and modify SCU settings)
  //
  SetupRuntimeUpdateEveryBoot ( SetupNvData );
  SetupNvData->DefaultBootType = DEFAULT_BOOT_FLAG;

  //
  // Save Setup variable.
  //
  BufferSize = PcdGet32 (PcdSetupConfigSize);
  Status = SaveSetupConfig (
              L"Setup",
             &mFormSetGuid,
             EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
             BufferSize,
             (VOID *) SetupNvData
             );


  //
  // Set Quick boot setting to variable, common code can use this setting to
  // do quick boot or not.
  //
  gRT->SetVariable (
              L"QuickBoot",
              &gEfiGenericVariableGuid,
              EFI_VARIABLE_BOOTSERVICE_ACCESS,
              sizeof (SetupNvData->QuickBoot),
              (VOID *) &SetupNvData->QuickBoot
              );

  if (SetupNvData->ReclaimLongrunTest) {
    UINTN Index;
    for (Index = 0; Index < FAST_RECLAIM_COUNT; Index += 1) {
      SetupNvData->NumLock = ~SetupNvData->NumLock;
      gRT->SetVariable (
                  L"Setup",
                  &mFormSetGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  BufferSize,
                  (VOID *) SetupNvData
                  );
    }
  }

  gBS->FreePool (SetupNvData);

  //
  // Check "Custom" variable is exist or not...
  //
  SetupNvData = AllocateZeroPool (BufferSize);
  ASSERT (SetupNvData != NULL);
  if (SetupNvData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Status = gRT->GetVariable (
                  L"Custom",
                  &mFormSetGuid,
                  NULL,
                  &BufferSize,
                  (VOID *)SetupNvData
                  );

  if (EFI_ERROR (Status)) {
    //
    //"Custom" Variable doesn't exist,so get a buffer with default variable
    //

    DefaultSetup (SetupNvData);
    gRT->SetVariable (
           L"CustomPlatformLang",
           &gEfiGenericVariableGuid,
           EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
           AsciiStrSize ((CHAR8 *) PcdGetPtr (PcdUefiVariableDefaultPlatformLang)),
           (VOID *) PcdGetPtr (PcdUefiVariableDefaultPlatformLang)
           );

    //
    // Load default required
    //
    SetupRuntimeDetermination (SetupNvData);
  }

  //
  // Update variable in every boot (e.g. MemorySize change and modify SCU settings)
  //
  SetupRuntimeUpdateEveryBoot (SetupNvData);

  //
  //Save Custom variable.
  //
  BufferSize = PcdGet32 (PcdSetupConfigSize);
  Status = gRT->SetVariable (
                  L"Custom",
                  &mFormSetGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  BufferSize,
                  (VOID *)SetupNvData
                  );

  gBS->FreePool (SetupNvData);

//   Status = AmdSystemConfig0Init ();
//   if (EFI_ERROR(Status)) {
//     return Status;
//   }
// 
//   Status = GetAmdSystemConfig0ToBuffer ();
//   if (EFI_ERROR (Status) || LoadVariableDefault) {
//     LoadAmdSystemConfig0Default ();
//     LoadVariableDefault = TRUE;
//     Status = SetBufferToAmdSystemConfig0 ();
//   }

    if (EFI_ERROR (Status) || LoadVariableDefault) {


        // Load PBS default value after clear CMOS
        LoadAmdPbsDefault (&AmdPbsConfiguration);
        Status = SetAmdPbsConfiguration (&AmdPbsConfiguration);
        if (EFI_ERROR (Status)) {
            ASSERT(FALSE);
        }

        // Load CBS default value after clear CMOS
        BufferSize = sizeof (CBS_CONFIG);
        AmdCbsConfig = AllocateZeroPool (BufferSize);
        CbsWriteDefalutValue ((UINT8 *) AmdCbsConfig);
        Status = gRT->SetVariable (
                      CBS_SYSTEM_CONFIGURATION_NAME,
                      &gCbsSystemConfigurationGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                      BufferSize,
                      (VOID *)AmdCbsConfig
                      );
        gBS->FreePool (AmdCbsConfig);
    }
  if ( FeaturePcdGet (PcdUseFastCrisisRecovery)) {
    if (LoadVariableDefault) {
      VOID                *HobList;
      EFI_BOOT_MODE       BootMode;
      EFI_PEI_HOB_POINTERS  Hob;

      if ( FeaturePcdGet (PcdNvVariableFailDetect)) {
        UINT16  TempChecksum, CheckSum;
        TempChecksum = SumaryCmos ();
        CheckSum = (TempChecksum >> 8) + ((TempChecksum & 0xff) << 8);
        WriteCmos16 (CmosCheckSum2E, CheckSum);
      }

      EfiGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);

      Hob.Raw = HobList;
      BootMode = Hob.HandoffInformationTable->BootMode;

      if (BootMode != BOOT_IN_RECOVERY_MODE) {
        if (gRT->ResetSystem != NULL) {
          gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
        }
      }
    }
  }

  return EFI_SUCCESS;
}

/**
 Installs a string and ifr pack set

 @param None

 @retval VOID

**/
EFI_STATUS
InstallHiiData (
  VOID
)
{
  EFI_HII_HANDLE                 HiiHandle;
  EFI_HANDLE                     DriverHandle;
  UINTN                          Index;
  EFI_STATUS                     Status;
  UINTN                          HandleCnt;
  NEW_PACKAGE_INFO               NewPackageInfo [] =
                                 {{InstallExitCallbackRoutine,     ExitVfrBin,     SetupUtilityLibStrings, SetupUtilityLibImages},
                                  {InstallBootCallbackRoutine,     BootVfrBin,     SetupUtilityLibStrings, SetupUtilityLibImages},
                                  {InstallPowerCallbackRoutine,    PowerVfrBin,    SetupUtilityStrings,    SetupUtilityImages},
                                  {InstallSecurityCallbackRoutine, SecurityVfrBin, SetupUtilityLibStrings, SetupUtilityLibImages},
                                  {InstallAdvanceCallbackRoutine,  AdvanceVfrBin,  SetupUtilityStrings,    SetupUtilityImages},
                                  {InstallMainCallbackRoutine,     MainVfrBin,     SetupUtilityLibStrings, SetupUtilityLibImages}
                                 };

  HandleCnt = sizeof (NewPackageInfo) / sizeof (NEW_PACKAGE_INFO);
  if (HandleCnt > MAX_HII_HANDLES) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = EFI_SUCCESS;
  for (Index = 0; Index < HandleCnt; Index++) {
    Status = CreateHiiDriverHandle (&DriverHandle);
    if (EFI_ERROR (Status)) {
      break;
    }

    HiiHandle = HiiAddPackages (
                  &mFormSetGuid,
                  DriverHandle,
                  NewPackageInfo[Index].IfrPack,
                  NewPackageInfo[Index].StringPack,
                  NewPackageInfo[Index].ImagePack,
                  NULL
                  );
    ASSERT(HiiHandle != NULL);

    gSUBrowser->SUCInfo->MapTable[Index].HiiHandle = HiiHandle;
    gSUBrowser->SUCInfo->MapTable[Index].DriverHandle = DriverHandle;
    NewPackageInfo[Index].CallbackRoutine (DriverHandle, HiiHandle);
  }

  UpdateMainVfrLabel (gSUBrowser->SUCInfo->MapTable[MainHiiHandle].HiiHandle);
//   UpdateSystemBusSpeed (gSUBrowser->SUCInfo->MapTable[MainHiiHandle].HiiHandle);
  mDriverHiiHandle = gSUBrowser->SUCInfo->MapTable[AdvanceHiiHandle].HiiHandle;

  UpdateMemoryInfo (gSUBrowser);

  return Status;
}

EFI_STATUS
InitSetupUtilityBrowser(
  IN  EFI_SETUP_UTILITY_PROTOCOL            *This
)
{
  EFI_STATUS                                Status;
  UINT8                                     *Setup = NULL;

  gSUBrowser = AllocateZeroPool (sizeof(SETUP_UTILITY_BROWSER_DATA));
  if (gSUBrowser == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  gSUBrowser->SUCInfo = AllocateZeroPool (sizeof(SETUP_UTILITY_CONFIGURATION));
  if (gSUBrowser->SUCInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Setup = CommonGetVariableData (SETUP_VARIABLE_NAME, &gSystemConfigurationGuid);
  if (Setup == NULL) {
    return EFI_NOT_FOUND;
  }

  gSUBrowser->SCBuffer          = Setup;
  gSUBrowser->SUCInfo->SCBuffer = gSUBrowser->SCBuffer;
  gSUBrowser->Signature         = EFI_SETUP_UTILITY_BROWSER_SIGNATURE;
  gSUBrowser->ExtractConfig     = GenericExtractConfig;
  gSUBrowser->RouteConfig       = GenericRouteConfig;
  gSUBrowser->HotKeyCallback    = HotKeyCallBack;

  //
  // There should only be one HII protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiHiiDatabaseProtocolGuid,
                  NULL,
                  (VOID **)&gSUBrowser->HiiDatabase
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // There should only be one HII protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiHiiStringProtocolGuid,
                  NULL,
                  (VOID **)&gSUBrowser->HiiString
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // There should only be one HII protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiHiiConfigRoutingProtocolGuid,
                  NULL,
                  (VOID **)&gSUBrowser->HiiConfigRouting
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // There will be only one FormConfig in the system
  // If there is another out there, someone is trying to install us
  // again.  Fail that scenario.
  //
  Status = gBS->LocateProtocol (
                  &gEfiFormBrowser2ProtocolGuid,
                  NULL,
                  (VOID **)&gSUBrowser->Browser2
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = gBS->LocateProtocol (
                 &gH2ODialogProtocolGuid,
                 NULL,
                 (VOID **)&gSUBrowser->H2ODialog
                 );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
 Installs a string and ifr pack set

 @param [in]   StringPack       string pack to store in database and associate with IfrPack
                                IrfPack - ifr pack to store in database (it will use StringPack data)
 @param        ...

 @retval VOID

**/
EFI_STATUS
RemoveHiiData (
  IN VOID     *StringPack,
  ...
)
{
  VA_LIST                                   args;
  HII_HANDLE_VARIABLE_MAP_TABLE             *MapTable;
  EFI_HII_DATABASE_PROTOCOL                 *HiiDatabase;
  UINTN                                     Index;

  HiiDatabase = gSUBrowser->HiiDatabase;
  VA_START (args, StringPack );

  Index = 0;
  while (TRUE) {
    MapTable = VA_ARG( args, HII_HANDLE_VARIABLE_MAP_TABLE *);
    if (MapTable == NULL) {
      break;
    }
    HiiDatabase->RemovePackageList (HiiDatabase, MapTable->HiiHandle);
    mUninstallCallbackRoutine[Index++] (MapTable->DriverHandle);
    DestroyHiiDriverHandle (MapTable->DriverHandle);
  }

  if (gSUBrowser->IdeConfig != NULL) {
    gBS->FreePool (gSUBrowser->IdeConfig);
    gSUBrowser->IdeConfig = NULL;
  }

  return EFI_SUCCESS;
}

/**
  Convert Processor Frequency Data to a string

  @param [in]   ProcessorFrequency    The frequency data to process
  @param [out]  String                The string that is created


  @retval        EFI_SUCCESS          convert value to string successful.
**/
STATIC
EFI_STATUS
ConvertFsbFrequencyToString (
  IN  EFI_PROCESSOR_FSB_FREQUENCY_DATA  *ProcessorFsbFrequency,
  OUT CHAR16                            **String
  )
{
  CHAR16        *StringBuffer;
  UINTN         Index;
  UINT32        FreqMhz;

  if (ProcessorFsbFrequency->Exponent >= 6) {
    FreqMhz = ProcessorFsbFrequency->Value;
    for (Index = 0; Index < (UINTN) (ProcessorFsbFrequency->Exponent - 6); Index++) {
      FreqMhz *= 10;
    }
  } else {
    FreqMhz = 0;
  }

  StringBuffer = AllocateZeroPool (0x20);
  Index = UnicodeValueToString (StringBuffer, 0, FreqMhz, 0);
  StrCat (StringBuffer, L" MHz");

  *String = (CHAR16 *) StringBuffer;

  return EFI_SUCCESS;
}

/**
  Override orginal system bus speed information

  @param [in]   HiiHandle      Hii hanlde for the call back routine

  @retval       EFI_SUCCESS    Update system bus speed success

**/
STATIC
EFI_STATUS
UpdateSystemBusSpeed (
  IN  EFI_HII_HANDLE       HiiHandle
)
{
  EFI_STATUS                            Status;
  EFI_STRING_ID                         BusSpeedToken;
  EFI_STRING_ID                         BusSpeedInfoToken;
  CHAR16                                *NewString;
  UINT64                                MonotonicCount;
  EFI_DATA_HUB_PROTOCOL                 *DataHub;
  EFI_DATA_RECORD_HEADER                *Record;
  EFI_SUBCLASS_TYPE1_HEADER             *DataHeader;
  EFI_PROCESSOR_FSB_FREQUENCY_DATA      *ProcessorFsbFrequency;
  EFI_GUID                              ProcessorSubClass = EFI_PROCESSOR_SUBCLASS_GUID;
  VOID                                  *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL                    *StartLabel;

  Status = gBS->LocateProtocol (
                  &gEfiDataHubProtocolGuid,
                  NULL,
                  &DataHub
                  );
  MonotonicCount = 0;
  Record = NULL;
  do {
    Status = DataHub->GetNextRecord (DataHub, &MonotonicCount, NULL, &Record);
    if (Record->DataRecordClass == EFI_DATA_RECORD_CLASS_DATA) {
      DataHeader  = (EFI_SUBCLASS_TYPE1_HEADER *) (Record + 1);
      //
      // System Bus Speed
      //
      if (CompareGuid (&Record->DataRecordGuid, &ProcessorSubClass) &&
          (DataHeader->RecordType == ProcessorFsbFrequencyRecordType)) {


        StartOpCodeHandle = HiiAllocateOpCodeHandle ();
        ASSERT (StartOpCodeHandle != NULL);

        StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
        StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
        StartLabel->Number       = UPDATE_SYSTEM_BUS_SPEED_LABEL;

        AddNewString (
          gSUBrowser->SUCInfo->MapTable[AdvanceHiiHandle].HiiHandle,
          HiiHandle,
          STRING_TOKEN (STR_SYSTEM_BUS_SPEED_STRING),
          &BusSpeedToken
          );
        ProcessorFsbFrequency = (EFI_PROCESSOR_FSB_FREQUENCY_DATA *) (DataHeader + 1);
        ConvertFsbFrequencyToString (ProcessorFsbFrequency, &NewString);
        BusSpeedInfoToken = 0;
        BusSpeedInfoToken = HiiSetString ( HiiHandle, 0, NewString, NULL );
        gBS->FreePool (NewString);
        //
        // Update System Bus Speed information in UPDATE_SYSTEM_BUS_SPEED_LABEL
        //
        HiiCreateTextOpCode (
          StartOpCodeHandle,
          BusSpeedToken,
          0,
          BusSpeedInfoToken
          );

        Status = HiiUpdateForm (
             HiiHandle,
             NULL,
             ROOT_FORM_ID,
             StartOpCodeHandle,
             NULL
             );
        HiiFreeOpCodeHandle (&StartOpCodeHandle);
      }
    }
  } while (!EFI_ERROR(Status) && (MonotonicCount != 0));
  return EFI_SUCCESS;
}


/**
  Update chipset information in Mainvfr

  @param [in]   HiiHandle      Hii hanlde for the call back routine

  @retval       EFI_SUCCESS    Update success

**/
STATIC
EFI_STATUS
UpdateMainVfrLabel (
  IN  EFI_HII_HANDLE       HiiHandle
)
{

  VOID                                  *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL                    *StartLabel;
  EFI_STATUS                            Status;
  EFI_SMBIOS_TABLE_HEADER               *Record;
  EFI_SMBIOS_HANDLE                     SmbiosHandle;
  EFI_SMBIOS_PROTOCOL                   *Smbios;
  SMBIOS_TABLE_TYPE4                    *Type4Record;
  EFI_STRING_ID                         CpuFreqToken;
  EFI_STRING_ID                         CpuFreqInfoToken;
  EFI_STRING_ID                         BlankStringToken;
  EFI_STRING_ID                         ChipsetKernelVersionToken;
  EFI_STRING_ID                         CcbVersionToken;
  EFI_STRING_ID                         ChipsetBiosDateToken;
  EFI_STRING_ID                         ChipsetBiosDateInfoToken;
  EFI_STRING_ID                         AgesaVersionToken;
  EFI_STRING_ID                         AgesaVersionInfoToken;
  CHAR16                                StrBuffer[40];

  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = MAIN_PAGE_PLATFORM_INFO_LABEL;

  AddNewString (
    gSUBrowser->SUCInfo->MapTable[AdvanceHiiHandle].HiiHandle,
    HiiHandle,
    STRING_TOKEN (STR_BLANK_STRING),
    &BlankStringToken
    );
   
  HiiCreateTextOpCode (
    StartOpCodeHandle,
    BlankStringToken,
    0,
    BlankStringToken
    );

  //
  // Update chipset kernel version
  //
  AddNewString (
    gSUBrowser->SUCInfo->MapTable[AdvanceHiiHandle].HiiHandle,
    HiiHandle,
    STRING_TOKEN (STR_BIOS_VERSION_2_STRING),
    &ChipsetKernelVersionToken
    );

  AddNewString (
    gSUBrowser->SUCInfo->MapTable[AdvanceHiiHandle].HiiHandle,
    HiiHandle,
    STRING_TOKEN (STR_CCB_VERSION),
    &CcbVersionToken
    );

  HiiCreateTextOpCode (
    StartOpCodeHandle,
    ChipsetKernelVersionToken,
    0,
    CcbVersionToken
    );
	
  //
  // Update BIOS release date
  //
  AddNewString (
    gSUBrowser->SUCInfo->MapTable[AdvanceHiiHandle].HiiHandle,
    HiiHandle,
    STRING_TOKEN (STR_BIOS_RELEASE_DATE_STRING),
    &ChipsetBiosDateToken
    );

  AddNewString (
    gSUBrowser->SUCInfo->MapTable[AdvanceHiiHandle].HiiHandle,
    HiiHandle,
    STRING_TOKEN (STR_MISC_BIOS_RELEASE_DATE),
    &ChipsetBiosDateInfoToken
    );
  
  HiiCreateTextOpCode (
    StartOpCodeHandle,
    ChipsetBiosDateToken,
    0,
    ChipsetBiosDateInfoToken
    );

  //
  // Update Agesa version
  //
  AddNewString (
    gSUBrowser->SUCInfo->MapTable[AdvanceHiiHandle].HiiHandle,
    HiiHandle,
    STRING_TOKEN (STR_AGESA_VERSION_STRING),
    &AgesaVersionToken
    );

  AddNewString (
    gSUBrowser->SUCInfo->MapTable[AdvanceHiiHandle].HiiHandle,
    HiiHandle,
    STRING_TOKEN (STR_AGESA_VERSION_VALUE),
    &AgesaVersionInfoToken
    );

  HiiCreateTextOpCode (
    StartOpCodeHandle,
    AgesaVersionToken,
    0,
    AgesaVersionInfoToken
    );

  //
  // Update CPU Frequency
  //
  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **) &Smbios
                  );
  ASSERT_EFI_ERROR (Status);

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  do {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, NULL, &Record, NULL);
    if (EFI_ERROR(Status)) {
      break;
    }

    if (Record->Type == EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION) {

      Type4Record = (SMBIOS_TABLE_TYPE4 *) Record;
      ZeroMem (StrBuffer, sizeof (StrBuffer));
      UnicodeValueToString (StrBuffer, 0, (INT64) Type4Record->CurrentSpeed, 0);
      StrCat (StrBuffer, L" MHz");

      AddNewString (
        gSUBrowser->SUCInfo->MapTable[AdvanceHiiHandle].HiiHandle,
        HiiHandle,
        STRING_TOKEN (STR_SYSTEM_CORE_FREQ_STRING),
        &CpuFreqToken
        );
  
      AddNewString (
        gSUBrowser->SUCInfo->MapTable[AdvanceHiiHandle].HiiHandle,
        HiiHandle,
        STRING_TOKEN (STR_SYSTEM_CORE_FREQ_STRING2),
        &CpuFreqInfoToken
        );
 
      HiiSetString (HiiHandle, CpuFreqInfoToken, StrBuffer, NULL);
  
      HiiCreateTextOpCode (
        StartOpCodeHandle,
        CpuFreqToken,
        0,
        CpuFreqInfoToken
        );

    } else {
      Status = EFI_SUCCESS;
    }
  } while(Status == EFI_SUCCESS);

  HiiCreateTextOpCode (
    StartOpCodeHandle,
    BlankStringToken,
    0,
    BlankStringToken
    );

  Status = HiiUpdateForm (
             HiiHandle,
             NULL,
             ROOT_FORM_ID,
             StartOpCodeHandle,
             NULL
             );

  HiiFreeOpCodeHandle (StartOpCodeHandle);

  return EFI_SUCCESS;

}

VOID
EFIAPI
SetupUtilityNotifyFn (
  IN EFI_EVENT                             Event,
  IN VOID                                  *Context
  )
{
  EFI_STATUS                               Status;
  EFI_SETUP_UTILITY_APPLICATION_PROTOCOL   *SetupUtilityApp;
  BOOLEAN                                  DoClearScreen;

  Status = gBS->LocateProtocol (
                  &gEfiSetupUtilityApplicationProtocolGuid,
                  NULL,
                  (VOID **) &SetupUtilityApp
                  );
  if (EFI_ERROR(Status)) {
    return;
  }

  if (SetupUtilityApp->VfrDriverState == InitializeSetupUtility) {
    POST_CODE (BDS_ENTER_SETUP); //PostCode = 0x29, Enter Setup Menu
  }

  mScuRecord = 0;

  DoClearScreen = TRUE;

  switch (SetupUtilityApp->VfrDriverState) {

  case InstallSetupUtilityHiiData:
    DoClearScreen = FALSE;
    CreateScuData (DoClearScreen);
    break;

  case UnInstallSetupUtilityHiiData:
    DoClearScreen = FALSE;
    DestroyScuData (DoClearScreen);
    break;

  case InitializeSetupUtility:
    CreateScuData (DoClearScreen);
    break;

  case ShutdownSetupUtility:
    DestroyScuData (DoClearScreen);
    break;

  default:
    break;
  }
}

/**
 This function use to verify Setup Configuration structure size as a build-time.
 It is not be call in BIOS during POST.

**/
VOID
SetupConfigurationSizeVerify
(
  VOID
)
{
  {C_ASSERT((sizeof(SYSTEM_CONFIGURATION)) == FixedPcdGet32(PcdSetupConfigSize));}
}
