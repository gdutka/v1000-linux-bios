/** @file
  String processing for HDD Password Protocol

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

#include "HddPassword.h"

#define INIFINITE_LOOP while(1)

HDD_PASSWORD_POST_DIALOG_PRIVATE *mHddPasswordPostDialogPrivate;

HDD_PASSWORD_PRIVATE      *mHddPasswordPrivate;
DRIVER_INSTALL_INFO       *mDriverInstallInfo;

//
// HII specific Vendor Device Path definition.
//
typedef struct {
  VENDOR_DEVICE_PATH        VendorDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  End;
} HII_VENDOR_DEVICE_PATH;

HII_VENDOR_DEVICE_PATH  mHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    {0xad2e3474, 0x93e6, 0x488b, {0x93, 0x19, 0x64, 0x88, 0xfc, 0x68, 0x1f, 0x16}}
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

extern UINT8                            HddPasswordStrings[];

EFI_GUID gHddPasswordStringPackGuid = { 0xac6f7313, 0xcea, 0x461d, {0x9a, 0xbc, 0x64, 0xf0, 0x77, 0x7a, 0x70, 0x4b} };

/**
  The HII driver handle passed in for HiiDatabase.NewPackageList() requires
  that there should be DevicePath Protocol installed on it.
  This routine create a virtual Driver Handle by installing a vendor device
  path on it, so as to use it to invoke HiiDatabase.NewPackageList().

  @param[out]       DriverHandle         Handle to be returned

  @retval           EFI_SUCCESS          Handle destroy success.
  @retval           EFI_OUT_OF_RESOURCES Not enough memory.

**/
EFI_STATUS
CreateHiiDriverHandle (
  OUT EFI_HANDLE                         *DriverHandle
  )
{
  EFI_STATUS                             Status;

  *DriverHandle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mHiiVendorDevicePath,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Get string by string id from HII Interface

  @param[in]        Id                  String ID

  @retval                               String from ID

**/
CHAR16 *
GetStringById (
  IN EFI_STRING_ID                      Id
  )
{
  CHAR16                                *String;

  String = HiiGetString (
             gStringPackHandle,
             Id,
             NULL
             );
  return String;
}

/**
  Initialize HII global accessor for string support

  @param

  @retval

**/
EFI_STATUS
InitializeStringSupport (
  VOID
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            DriverHandle;

  Status = CreateHiiDriverHandle (&DriverHandle);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  gStringPackHandle = HiiAddPackages (
                        &gHddPasswordStringPackGuid,
                        DriverHandle,
                        HddPasswordStrings,
                        NULL
                        );

  return Status;
}

/**
  Init. StringTokenArray for HddPassword

  @param

  @retval
**/
EFI_STATUS
InitDialogStringTokenArray (
  VOID
  )
{
  EFI_STATUS                            Status;
  CHAR16                                **StrTokenArray;

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  (STR_TOKEN_NUMBERS * sizeof (CHAR16 *)),
                  (VOID *)&StrTokenArray);

  if (Status != EFI_SUCCESS) {
    return EFI_OUT_OF_RESOURCES;
  }

  StrTokenArray [STR_HDD_ESC_SKIP_MSG_INDEX]        = GetStringById (STRING_TOKEN (STR_HDD_ESC_SKIP_MSG));
  StrTokenArray [STR_HDD_TITLE_MSG_INDEX]           = GetStringById (STRING_TOKEN (STR_HDD_TITLE_MSG));
  StrTokenArray [STR_HDD_DIALOG_THREE_ERRORS_INDEX] = GetStringById (STRING_TOKEN (STR_HDD_DIALOG_THREE_ERRORS));
  StrTokenArray [STR_HDD_DIALOG_CONTINUE_MSG_INDEX] = GetStringById (STRING_TOKEN (STR_HDD_DIALOG_CONTINUE_MSG));
  StrTokenArray [STR_HDD_DIALOG_ENTER_MSG_INDEX]    = GetStringById (STRING_TOKEN (STR_HDD_DIALOG_ENTER_MSG));
  StrTokenArray [STR_HDD_DIALOG_ERROR_STATUS_INDEX] = GetStringById (STRING_TOKEN (STR_HDD_DIALOG_ERROR_STATUS));
  StrTokenArray [STR_HDD_ENTER_MANY_ERRORS_INDEX]   = GetStringById (STRING_TOKEN (STR_HDD_ENTER_MANY_ERRORS));
  StrTokenArray [STR_SECURITY_COUNT_EXPIRED_INDEX]  = GetStringById (STRING_TOKEN (STR_SECURITY_COUNT_EXPIRED));
  StrTokenArray [STR_HDD_DIALOG_COLDBOOT_MSG_INDEX] = GetStringById (STRING_TOKEN (STR_HDD_DIALOG_COLDBOOT_MSG));
  StrTokenArray [STR_HDD_DIALOG_HELP_TITLE_INDEX]   = GetStringById (STRING_TOKEN (STR_HDD_DIALOG_HELP_TITLE));

  mDriverInstallInfo->StrTokenInfo.StrTokenArray    = StrTokenArray;

  return EFI_SUCCESS;
}

/**
  Get StringTokenArray for HddPassword

  @param[out]       StrTokenArray       StringToken array pointer

  @retval           EFI_SUCCESS
**/
EFI_STATUS
GetStringTokenArray (
  IN  EFI_HDD_PASSWORD_DIALOG_PROTOCOL  *This,
  OUT CHAR16                            **StrTokenArray
  )
{
  CHAR16                                **StrTokenArrayPtr;

  if (This == NULL || StrTokenArray == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  StrTokenArrayPtr = mDriverInstallInfo->StrTokenInfo.StrTokenArray;

  *StrTokenArray = (VOID *)StrTokenArrayPtr;

  return EFI_SUCCESS;
}

/**
  Free SteingTokenArray memory buffer

  @param

  @retval           EFI_SUCCESS

**/
EFI_STATUS
ReleaseStringTokenArray (
  VOID
  )
{
  UINTN                                 Index;
  CHAR16                                **StrTokenArray;

  Index            = 0;
  StrTokenArray    = mDriverInstallInfo->StrTokenInfo.StrTokenArray;

  for (Index = 0; Index < STR_TOKEN_NUMBERS; Index++) {
    if (StrTokenArray[Index] != NULL) {
      gBS->FreePool (StrTokenArray[Index]);
    }
  }

  gBS->FreePool ((VOID *)StrTokenArray);

  mDriverInstallInfo->StrTokenInfo.StrTokenArray = NULL;

  return EFI_SUCCESS;
}

/**
  To check is SecureBoot reset

  @param

  @retval           EFI_SUCCESS
**/
BOOLEAN
IsAdmiSecureBootReset (
  IN  EFI_HDD_PASSWORD_DIALOG_PROTOCOL  *This
  )
{
  UINT8                                 AdmiSecureBoot;
  EFI_STATUS                            Status;
  BOOLEAN                               AdmiSecureBootReset;
  UINTN                                 BufferSize;

  AdmiSecureBootReset = FALSE;

  BufferSize = sizeof (UINT8);

  Status = gRT->GetVariable (
                   EFI_ADMINISTER_SECURE_BOOT_NAME,
                   &gEfiGenericVariableGuid,
                   NULL,
                   &BufferSize,
                   &AdmiSecureBoot
                   );

  if (!EFI_ERROR (Status) && AdmiSecureBoot == 1) {
    AdmiSecureBootReset = TRUE;
  }

  return AdmiSecureBootReset;

}


/**
  Copy SourceBufferPtr string to DescBufferPtr.

  @param[in]        DescBufferPtr       Destination buffer address.
  @param[in]        SourceBufferPtr     Source buffer addess.

  @retval           EFI_SUCCESS
**/
EFI_STATUS
GetHddModelNumber (
  IN  EFI_HDD_PASSWORD_DIALOG_PROTOCOL  *This,
  IN VOID                               *DescBufferPtr,
  IN VOID                               *SourceBufferPtr
  )
{
  if (This == NULL || DescBufferPtr == NULL || SourceBufferPtr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  return GetModelNumber (DescBufferPtr, SourceBufferPtr) ;
}

VOID
HexToString (
  CHAR16  *String,
  UINTN   Value,
  UINTN   Digits
  )
{
  CHAR16                          HexDigit[17] = L"0123456789ABCDEF";
  for (; Digits > 0; Digits--, String++) {
    *String = HexDigit[((Value >> (4*(Digits-1))) & 0x0f)];
  }
}


/**
  Fill in HDD information string to buffer.

  @param[in]        HddInfoArray            The array of HDD information used in HDD Password.
  @param[in]        NumOfHdd                Number of HDD.
  @param[in, out]   HddDialogItemInfoString String buffer array for HDD password unlock dialog.

  @retval EFI_SUCCESS
**/
EFI_STATUS
PrepareHddDialogItems (
  IN EFI_HDD_PASSWORD_DIALOG_PROTOCOL   *This,
  IN HDD_PASSWORD_HDD_INFO              *HddInfoArray,
  IN UINTN                              NumOfHdd,
  OUT ITEM_INFO_IN_DIALOG               *HddDialogItemInfoString
  )
{
  UINT32                                    Index;
  CHAR16                                    *PortStatusString;
  CHAR16                                    PortNumberString[7];
  ITEM_INFO_IN_DIALOG                       DialogItemInfoTempBuffer;
  UINT16                                    HddPort;
  UINTN                                     StringLength;
  UINTN                                     ModelLength;
  UINTN                                     StartIndex;
  UINTN                                     EndIndex;
  EFI_STATUS                                Status;  
  EFI_DEVICE_PATH_PROTOCOL                  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL                  *DevicePathNode;
  NVME_NAMESPACE_DEVICE_PATH                *NVMeDevicePath;

  if (This == NULL || HddInfoArray == NULL || HddDialogItemInfoString == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (NumOfHdd == 0) {
    return EFI_UNSUPPORTED;
  }

  StringLength = 0;

  for (Index = 0; Index < NumOfHdd; Index++) {
    SetMem16 ((VOID *)&(DialogItemInfoTempBuffer.ItemInfoString), sizeof (ITEM_INFO_IN_DIALOG), CHAR_SPACE);
    SetMem16 ((VOID *)&(PortNumberString), sizeof(CHAR16) * 6, CHAR_SPACE);
    PortNumberString[6] = '\0';

    Status = gBS->HandleProtocol (
                    HddInfoArray[Index].DeviceHandleInDxe,
                    &gEfiDevicePathProtocolGuid,
                    (VOID *) &DevicePath
                    );

    NVMeDevicePath = NULL;

    if (Status == EFI_SUCCESS) {
      DevicePathNode = DevicePath;
      while (!IsDevicePathEnd (DevicePathNode)) {
        if ((DevicePathType (DevicePathNode) == MESSAGING_DEVICE_PATH) &&
            (DevicePathSubType (DevicePathNode) == MSG_NVME_NAMESPACE_DP)) {
              NVMeDevicePath = (NVME_NAMESPACE_DEVICE_PATH *) DevicePathNode;
              break;
        }
        DevicePathNode = NextDevicePathNode (DevicePathNode);
      }
    }

    if (HddInfoArray[Index].ControllerMode == ATA_IDE_MODE) {
      HddPort = HddInfoArray[Index].MappedPort;
    } else {
      HddPort = HddInfoArray[Index].PortNumber;
    }

    if (HddInfoArray[Index].ControllerMode < NVMe_MODE && NVMeDevicePath == NULL) {
      PortStatusString = GetStringById (STRING_TOKEN (STR_SATA));
    } else {
       PortStatusString = (NVMeDevicePath != NULL) ? GetStringById (STRING_TOKEN (STR_NVME)) : GetStringById (STRING_TOKEN (STR_EMMC));
    }


    //
    //  Copy which port to string
    //
    StrCpyS (
      &(DialogItemInfoTempBuffer.ItemInfoString[0]),
      sizeof(DialogItemInfoTempBuffer.ItemInfoString) / sizeof(CHAR16),
      PortStatusString
      );
    StringLength = StrLen (PortStatusString);
    if (DialogItemInfoTempBuffer.ItemInfoString[StringLength] == CHAR_NULL) {
      DialogItemInfoTempBuffer.ItemInfoString[StringLength] = CHAR_SPACE;
    }

    //
    //  Allocate by GetStringById
    //
    gBS->FreePool (PortStatusString);

    if (HddInfoArray[Index].ControllerMode < NVMe_MODE) {
      HexToString(PortNumberString, HddPort, 2);
      StrCpyS (
        &(DialogItemInfoTempBuffer.ItemInfoString[StringLength]),
        sizeof(DialogItemInfoTempBuffer.ItemInfoString) / sizeof(CHAR16),
        PortNumberString
        );
      StringLength += StrLen (PortNumberString);
      if (DialogItemInfoTempBuffer.ItemInfoString[StringLength] == CHAR_NULL) {
        DialogItemInfoTempBuffer.ItemInfoString[StringLength] = CHAR_SPACE;
      }
    }

    ModelLength = StrLen (HddInfoArray[Index].HddModelString);
    CopyMem (
      &(DialogItemInfoTempBuffer.ItemInfoString[StringLength]),
      (CHAR16*)HddInfoArray[Index].HddModelString,
      MODEL_NUMBER_LENGTH * sizeof (CHAR16)
      );
    while (DialogItemInfoTempBuffer.ItemInfoString[StringLength + ModelLength - 1] == CHAR_SPACE) {
      ModelLength--;
    }
    
    //
    //  Copy HDD status to string
    //
    if ((HddInfoArray[Index].HddSecurityStatus & HDD_LOCKED_BIT) != HDD_LOCKED_BIT) {
      PortStatusString = GetStringById (STRING_TOKEN (STR_HDD_PSW_UNLOCK));
    } else {
      PortStatusString = GetStringById (STRING_TOKEN (STR_HDD_PSW_LOCK));
    }

    //
    //  change PortStatusString form in GUI mode
    //
    StartIndex = 0;
    EndIndex = StrLen (PortStatusString) - 1;
    while (StartIndex <= EndIndex) {
      if (PortStatusString[StartIndex] == CHAR_SPACE) {
        StartIndex++;
      } else if (PortStatusString[EndIndex] == CHAR_SPACE) {
        PortStatusString[EndIndex] = 0;
        EndIndex--;
      } else {
        break;
      }
    }

    DialogItemInfoTempBuffer.ItemInfoString[StringLength + ModelLength - 1] = '(';
    StrCpyS (
      &(DialogItemInfoTempBuffer.ItemInfoString[StringLength + ModelLength]),
      (sizeof(DialogItemInfoTempBuffer.ItemInfoString) / sizeof(CHAR16)) - (StringLength + ModelLength),
      &PortStatusString[StartIndex]
      );
    DialogItemInfoTempBuffer.ItemInfoString[StrLen (DialogItemInfoTempBuffer.ItemInfoString)] = ')';
    //
    //  Allocate by GetStringById
    //
    gBS->FreePool (PortStatusString);

    DialogItemInfoTempBuffer.ItemInfoString[OPTION_ICON_MAX_STR_SIZE - 1] = CHAR_NULL;
    CopyMem (
      HddDialogItemInfoString[Index].ItemInfoString,
      DialogItemInfoTempBuffer.ItemInfoString,
      (OPTION_ICON_MAX_STR_SIZE * sizeof(CHAR16))
      );
  }

  return EFI_SUCCESS;
}

/**
  Checking all HDD security status is LOCK or not.

  @param[in]        HddInfoArray        The array of HDD information used in HDD Password.
  @param[in]        NumOfHdd            Number of HDD.

  @retval           TRUE                Found harddisk is locked.
  @retval           FALSE               Could not found any harddisk is locked.
**/
BOOLEAN
CheckHddLock (
  IN  EFI_HDD_PASSWORD_DIALOG_PROTOCOL  *This,
  IN HDD_PASSWORD_HDD_INFO              *HddInfoArray,
  IN UINTN                              NumOfHdd
  )
{
  UINTN                                 Index;

  if (This == NULL || HddInfoArray == NULL) {
    return FALSE;
  }

  for (Index = 0; Index < NumOfHdd; Index++) {
    if ((HddInfoArray[Index].HddSecurityStatus & HDD_LOCKED_BIT) == HDD_LOCKED_BIT) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  Checking all HDD security status is Enabled or not.

  @param[in]        HddInfoArray        The array of HDD information used in HDD Password.
  @param[in]        NumOfHdd            Number of HDD.

  @retval           TRUE                Found one of HDD is security Enabled.
  @retval           FALSE               Could not found any HDD is  security Enabled.
**/
BOOLEAN
CheckHddSecurityEnable (
  IN  EFI_HDD_PASSWORD_DIALOG_PROTOCOL  *This,
  IN HDD_PASSWORD_HDD_INFO              *HddInfoArray,
  IN UINTN                              NumOfHdd
  )
{
  UINTN                                 Index;

  if (This == NULL || HddInfoArray == NULL) {
    return FALSE;
  }

  for (Index = 0; Index < NumOfHdd; Index++) {
    if ((HddInfoArray[Index].HddSecurityStatus & HDD_ENABLE_BIT) == HDD_ENABLE_BIT) {
       return TRUE;
    }
  }
  return FALSE;
}

EFI_STATUS
ResetAllSecurityStatus (
  IN EFI_HDD_PASSWORD_DIALOG_PROTOCOL   *This,
  IN HDD_PASSWORD_HDD_INFO              *HddInfoArray,
  IN UINTN                              NumOfHdd
  )
{
  EFI_STATUS                            Status;
  UINTN                                 Index;
  UINT8                                 SkipCounter;
  EFI_HDD_PASSWORD_SERVICE_PROTOCOL     *HddPasswordService;
  HDD_PASSWORD_POST_DIALOG_PRIVATE      *HddPasswordPostDialogPrivate;

  if (This == NULL || HddInfoArray == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  HddPasswordPostDialogPrivate = GET_PRIVATE_FROM_HDD_PASSWORD_POST_DIALOG (This);

  HddPasswordService = HddPasswordPostDialogPrivate->HddPasswordService;

  SkipCounter = 0;


  for (Index = 0; Index < NumOfHdd; Index++) {
    if (((HddInfoArray[Index].HddSecurityStatus & HDD_LOCKED_BIT) != HDD_LOCKED_BIT) &&
        ((HddInfoArray[Index].HddSecurityStatus & HDD_ENABLE_BIT) == HDD_ENABLE_BIT)) {

        Status = HddPasswordService->ResetSecurityStatus (
                                       HddPasswordService,
                                       &(HddInfoArray[Index])
                                       );

    }

    if (((HddInfoArray[Index].HddSecurityStatus & HDD_LOCKED_BIT) != HDD_LOCKED_BIT) ||
        ((HddInfoArray[Index].HddSecurityStatus & HDD_FROZEN_BIT) == HDD_FROZEN_BIT)) {
      SkipCounter++;
    }
    if (SkipCounter == NumOfHdd) {
      //
      //  No any HDD having to be unlock
      //
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_READY;
}

EFI_STATUS
SelectedHddPrepareTitleString (
  IN EFI_HDD_PASSWORD_DIALOG_PROTOCOL   *This,
  IN HDD_PASSWORD_HDD_INFO              *HddInfo,
  OUT CHAR16                            **HddTitleString
  )
{
  EFI_STATUS                            Status;
  CHAR16                                **StrTokenArray;
  CHAR16                                *HddTitleStringPtr;

  if (This == NULL || HddInfo == NULL || HddTitleString == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status            = EFI_SUCCESS;
  StrTokenArray     = NULL;
  HddTitleStringPtr = (CHAR16 *)HddTitleString;

  Status = This->GetStringTokenArray (
                   This,
                   (VOID *)&StrTokenArray
                   );
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  SetMem16 (HddTitleStringPtr, HDD_TITLE_STRING_LENGTH * sizeof (CHAR16), CHAR_SPACE);
  StrCpyS (HddTitleStringPtr, HDD_TITLE_STRING_LENGTH, StrTokenArray[STR_HDD_DIALOG_HELP_TITLE_INDEX]);
  CopyMem (&(HddTitleStringPtr[StrLen(StrTokenArray[STR_HDD_DIALOG_HELP_TITLE_INDEX])]), HddInfo->HddModelString, MODEL_NUMBER_LENGTH * sizeof (CHAR16));
  HddTitleStringPtr[StrLen (StrTokenArray[STR_HDD_DIALOG_HELP_TITLE_INDEX]) + MODEL_NUMBER_LENGTH] = CHAR_NULL;

  return Status;
}

EFI_STATUS
ShowErrorCountExpiredMessageAndSystemStop (
  IN EFI_HDD_PASSWORD_DIALOG_PROTOCOL   *HddPasswordDialog
  )
{
  CHAR16                                **StrTokenArray;
  HDD_PASSWORD_POST_DIALOG_PRIVATE      *HddPasswordPostDialogPrivate;
  H2O_DIALOG_PROTOCOL                   *H2oDialogProtocol;


  if (HddPasswordDialog == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  HddPasswordPostDialogPrivate = GET_PRIVATE_FROM_HDD_PASSWORD_POST_DIALOG (HddPasswordDialog);

  H2oDialogProtocol = HddPasswordPostDialogPrivate->H2oDialogProtocol;

  HddPasswordDialog->GetStringTokenArray (
                       HddPasswordDialog,
                       (VOID *)&StrTokenArray
                       );

  H2oDialogProtocol->CreateMsgPopUp (
                       HDD_TITLE_STRING_LENGTH,
                       4,
                       StrTokenArray[STR_HDD_DIALOG_ERROR_STATUS_INDEX],
                       StrTokenArray[STR_HDD_ENTER_MANY_ERRORS_INDEX],
                       StrTokenArray[STR_SECURITY_COUNT_EXPIRED_INDEX],
                       StrTokenArray[STR_HDD_DIALOG_COLDBOOT_MSG_INDEX]
                       );

  INIFINITE_LOOP;

  return EFI_SUCCESS;
}

/**
  To show the selected HDD unlock dialog and to unlock HDD by inputing HDD password.

  @param[in]        HddPasswordService  EFI_HDD_PASSWORD_SERVICE_PROTOCOL instance.
  @param[in]        HddInfo             The HDD information used in HDD Password.
  @param[in]        OemFormBrowserPtr   EFI_OEM_FORM_BROWSER_PROTOCOL instance.
  @param[in]        HddInfoIndex        HDD information index.

  @retval EFI_SUCCESS

**/
EFI_STATUS
UnlockSelectedHdd (
  IN EFI_HDD_PASSWORD_DIALOG_PROTOCOL   *This,
  IN HDD_PASSWORD_HDD_INFO              *HddInfo,
  OUT PASSWORD_INFORMATION              *PasswordInfo
  )
{
  EFI_STATUS                            Status;
  EFI_STATUS                            UnlockHddStatus;
  CHAR16                                HddTitleString[HDD_TITLE_STRING_LENGTH];
  CHAR16                                UnicodePasswordString[HDD_PASSWORD_MAX_NUMBER + 1];
  UINTN                                 UnicodePasswordLength;
  EFI_INPUT_KEY                         Key;
  CHAR16                                **StrTokenArray;
  UINT8                                 PasswordToHdd[HDD_PASSWORD_MAX_NUMBER + 1];
  UINTN                                 PasswordToHddLength;
  HDD_PASSWORD_POST_DIALOG_PRIVATE      *HddPasswordPostDialogPrivate;
  H2O_DIALOG_PROTOCOL                   *H2oDialogProtocol;
  EFI_HDD_PASSWORD_SERVICE_PROTOCOL     *HddPasswordService;

  if (This == NULL || HddInfo == NULL || PasswordInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((HddInfo->HddSecurityStatus & HDD_LOCKED_BIT) != HDD_LOCKED_BIT) {
    return EFI_SUCCESS;
  }

  HddPasswordPostDialogPrivate = GET_PRIVATE_FROM_HDD_PASSWORD_POST_DIALOG (This);

  HddPasswordService = HddPasswordPostDialogPrivate->HddPasswordService;
  H2oDialogProtocol = HddPasswordPostDialogPrivate->H2oDialogProtocol;

  StrTokenArray = NULL;

  Key.ScanCode    = SCAN_NULL;
  Key.UnicodeChar = CHAR_NULL;

  UnlockHddStatus = EFI_SUCCESS;
  UnicodePasswordLength = 0;

  This->GetStringTokenArray (
          This,
          (VOID *)&StrTokenArray
          );

  Status = SelectedHddPrepareTitleString (
             This,
             HddInfo,
             (CHAR16 **)&HddTitleString
             );

  ZeroMem (UnicodePasswordString, (HDD_PASSWORD_MAX_NUMBER + 1) * sizeof(CHAR16));

  Status = H2oDialogProtocol->PasswordDialog (
                                0,
                                FALSE,
                                (MIN (PcdGet16 (PcdH2OHddPasswordMaxLength), HddInfo->MaxPasswordLengthSupport) + 1),
                                UnicodePasswordString,
                                &Key,
                                HddTitleString
                                );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  UnicodePasswordLength = StrLen (UnicodePasswordString);

  ZeroMem (PasswordToHdd, (HDD_PASSWORD_MAX_NUMBER + 1));
  Status = HddPasswordService->PasswordStringProcess (
                                 HddPasswordService,
                                 HddInfo->StorageTcgSecuritySupported ? MASTER_PSW : USER_PSW,
                                 UnicodePasswordString,
                                 UnicodePasswordLength,
                                 (VOID **)&PasswordToHdd,
                                 &PasswordToHddLength
                                 );

  if (PasswordToHddLength == 0) {
    return Status;
  }

  UnlockHddStatus = HddPasswordService->UnlockHddPassword (
                                          HddPasswordService,
                                          HddInfo,
                                          HddInfo->StorageTcgSecuritySupported ? MASTER_PSW : USER_PSW,
                                          PasswordToHdd,
                                          PasswordToHddLength
                                          );
  if (!EFI_ERROR (UnlockHddStatus) && !((HddInfo->HddSecurityStatus & HDD_LOCKED_BIT) == HDD_LOCKED_BIT)) {
    CopyMem (
      PasswordInfo->PasswordStr,
      UnicodePasswordString,
      (UnicodePasswordLength * sizeof (CHAR16))
      );
    PasswordInfo->PasswordType = HddInfo->StorageTcgSecuritySupported ? MASTER_PSW : USER_PSW;
  } else {
    //
    //To check the Identify word 128 Security count expired bit
    //It is set to one if enter incorrect password too many times.
    //
    if ((HddInfo->HddSecurityStatus & HDD_EXPIRED_BIT) == HDD_EXPIRED_BIT) {
      ShowErrorCountExpiredMessageAndSystemStop (This);
    }

    UnlockHddStatus = HddPasswordService->UnlockHddPassword (
                                            HddPasswordService,
                                            HddInfo,
                                            HddInfo->StorageTcgSecuritySupported ? USER_PSW : MASTER_PSW,
                                            PasswordToHdd,
                                            PasswordToHddLength
                                            );
    if (!EFI_ERROR (UnlockHddStatus) && !((HddInfo->HddSecurityStatus & HDD_LOCKED_BIT) == HDD_LOCKED_BIT)) {
      CopyMem (
        PasswordInfo->PasswordStr,
        UnicodePasswordString,
        (UnicodePasswordLength * sizeof (CHAR16))
        );
      PasswordInfo->PasswordType = HddInfo->StorageTcgSecuritySupported ? USER_PSW : MASTER_PSW;
    }

    if (HddInfo->StorageTcgSecuritySupported) {
      if (PasswordToHddLength > 0) {
        ZeroMem (PasswordToHdd, PasswordToHddLength);
      }
    }
    return UnlockHddStatus;
  }

  //
  // zeroing the PWD content in memory after it's being used
  //
  if (HddInfo->StorageTcgSecuritySupported) {
    if (UnicodePasswordLength > 0) {
      ZeroMem (UnicodePasswordString, (UnicodePasswordLength * sizeof (CHAR16)));
    }

    if (PasswordToHddLength > 0) {
      ZeroMem (PasswordToHdd, PasswordToHddLength);
    }
  }



  return UnlockHddStatus;
}

/**
  Check the valid skip Hot-Key

  @param[in]        SkipDialogKey       Check the key is skip dialog Hot-Key or not

  @retval           EFI_NOT_FOUND       SkipDialogKey is not Hot-Key
  @retval           EFI_SUCCESS         Checking is successful and SkipDialogKey is Hot-Key
**/
EFI_STATUS
CheckSkipDialogKey (
  IN  EFI_HDD_PASSWORD_DIALOG_PROTOCOL  *This,
  IN  VOID                              *SkipDialogKey
  )
{
  EFI_INPUT_KEY                         HotKey;

  HotKey.ScanCode    = SCAN_ESC;
  HotKey.UnicodeChar = CHAR_NULL;


  if (SkipDialogKey == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (((EFI_INPUT_KEY *)SkipDialogKey)->ScanCode == SCAN_NULL &&
      ((EFI_INPUT_KEY *)SkipDialogKey)->UnicodeChar == CHAR_NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (((EFI_INPUT_KEY *)SkipDialogKey)->ScanCode == HotKey.ScanCode &&
      ((EFI_INPUT_KEY *)SkipDialogKey)->UnicodeChar == HotKey.UnicodeChar) {
    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}

/**
  Zero out HddPasswordTable by policy.

  @param[in]        HddPasswordTable         Point to HddPasswordTable.
  @param[in]        HddPasswordTableSize     Size of HddPasswordTable.

**/
VOID
ZeroMemByPolicy (
  IN  HDD_PASSWORD_TABLE                    *HddPasswordTable,
  IN  UINTN                                 HddPasswordTableSize
  )
{
  HDD_PASSWORD_TABLE                    *HddPasswordTablePtr;
  TCG_OPAL_EXT_DATA                     *TcgExtDataPtr;

  HddPasswordTablePtr = (HDD_PASSWORD_TABLE *)HddPasswordTable;
  while (((UINTN)HddPasswordTable + HddPasswordTableSize) > (UINTN)HddPasswordTablePtr) {
    TcgExtDataPtr = (TCG_OPAL_EXT_DATA *)(HddPasswordTablePtr + 1);
    if (TcgExtDataPtr->Signature == TCG_OPAL_EXT_DATA_SIGNATURE) {
      //
      // Keep the ExtData but zero out password.
      //
      ZeroMem ((VOID *)HddPasswordTablePtr->PasswordStr, sizeof (HddPasswordTablePtr->PasswordStr));
      ZeroMem ((VOID *)&HddPasswordTablePtr->PasswordType, sizeof (HddPasswordTablePtr->PasswordType));
    } else {
      ZeroMem ((VOID *)HddPasswordTablePtr, sizeof (HDD_PASSWORD_TABLE) + HddPasswordTablePtr->ExtDataSize);
    }
    HddPasswordTablePtr = GetNextTableEntry (HddPasswordTablePtr);
  }
}

/**
  Update extend data area.

  @param[in]        HddPasswordTable     Pointer to HddPasswordTable
  @param[in]        PasswordType         Password type

**/
VOID
UpdateExtData (
  IN  HDD_PASSWORD_TABLE                *HddPasswordTable,
  IN  UINT8                              PasswordType
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
  Return ExtDataSize.

  @retval           The Size of ExtData is going to be used
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
  To show the hdd check dialog.

  @param[in]        HddPasswordProtocol EFI_HDD_PASSWORD_SERVICE_PROTOCOL instance.

  @retval           EFI_SUCCESS

**/
EFI_STATUS
HddUnlockDialog (
  IN EFI_HDD_PASSWORD_DIALOG_PROTOCOL   *HddPasswordDialog
  )
{
  UINTN                                 Index;
  UINT8                                 *ErrorCount;
  UINTN                                 NumOfHdd;
  UINT8                                 SkipCounter;
  EFI_STATUS                            Status;
  H2O_DIALOG_PROTOCOL                   *H2oDialogProtocol;
  ITEM_INFO_IN_DIALOG                   HddDialogItemInfoString[SUPPORT_SATA_PORTS_NUM];
  CHAR16                                **HddDialogItemInfoStringPtr;
  CHAR16                                **StrTokenArray;
  EFI_INPUT_KEY                         Key;
  EFI_INPUT_KEY                         KeyList[] = {{SCAN_UP,   CHAR_NULL},
                                                     {SCAN_DOWN, CHAR_NULL},
                                                     {SCAN_ESC,  CHAR_NULL},
                                                     {SCAN_NULL, CHAR_CARRIAGE_RETURN},
                                                     {SCAN_NULL, CHAR_NULL}
                                                     };
  HDD_PASSWORD_TABLE                    *HddPasswordTable;
  UINTN                                 HddPasswordTableSize;
  HDD_PASSWORD_HDD_INFO                 *HddInfoArray;
  HDD_PASSWORD_POST_DIALOG_PRIVATE      *HddPasswordPostDialogPrivate;
  EFI_HDD_PASSWORD_SERVICE_PROTOCOL     *HddPasswordService;
  PASSWORD_INFORMATION                  PasswordInfo;
  HDD_PASSWORD_TABLE                    *HddPasswordTablePtr;
  UINTN                                 Count;
  UINTN                                 OrgMode;

  StrTokenArray = NULL;
  SkipCounter  = 0;

  HddInfoArray = NULL;
  NumOfHdd = 0;
  HddPasswordTable = NULL;

  ErrorCount = NULL;
  HddPasswordTableSize = 0;

  HddPasswordPostDialogPrivate = GET_PRIVATE_FROM_HDD_PASSWORD_POST_DIALOG (HddPasswordDialog);

  HddPasswordService = HddPasswordPostDialogPrivate->HddPasswordService;
  H2oDialogProtocol = HddPasswordPostDialogPrivate->H2oDialogProtocol;

  Status = HddPasswordService->GetHddInfo (
                                 HddPasswordService,
                                 &HddInfoArray,
                                 &NumOfHdd
                                 );

  if (NumOfHdd == 0) {
    Status = EFI_SUCCESS;
    goto EndOfDialog;
  }

  Status = HddPasswordDialog->ResetAllSecurityStatus (
                                HddPasswordDialog,
                                HddInfoArray,
                                NumOfHdd
                                );

  if (Status == EFI_SUCCESS) {
    //
    //  No HDD having to be unlock
    //

    goto EndOfDialog;
  } else if (Status != EFI_NOT_READY) {
    goto EndOfDialog;
  }

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

    if ((HddPasswordTableSize / (sizeof (HDD_PASSWORD_TABLE) + GetExtDataSize ())) != (NumOfHdd + 1)) {
      Status = EFI_NOT_FOUND;
      gBS->FreePool (HddPasswordTable);
    } else {
      ZeroMemByPolicy (HddPasswordTable, HddPasswordTableSize);
    }
  }

  if (Status != EFI_SUCCESS) {
    //
    // Create new table
    //
    HddPasswordTable = CreateHddPasswordTable (NumOfHdd, &HddPasswordTableSize);
    if (HddPasswordTable == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  }

  HddDialogItemInfoStringPtr = AllocatePool ( (NumOfHdd + 1) * sizeof(CHAR16 *));
  if (HddDialogItemInfoStringPtr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  for (Index = 0; Index < NumOfHdd; Index++) {
    HddDialogItemInfoStringPtr[Index] = (CHAR16 *)&(HddDialogItemInfoString[Index].ItemInfoString);
  }

  HddDialogItemInfoStringPtr[Index] = NULL;

  Key.ScanCode    = SCAN_NULL;
  Key.UnicodeChar = CHAR_NULL;

  ErrorCount = (UINT8 *)AllocateZeroPool (NumOfHdd * sizeof(UINT8));

  if (ErrorCount == NULL) {
    goto EndOfDialog;
  }

  Index = 0;

  HddPasswordDialog->GetStringTokenArray (
                       HddPasswordDialog,
                       (VOID *)&StrTokenArray
                       );

  OrgMode = (UINTN) gST->ConOut->Mode->Mode;
  DisableQuietBoot ();

  gST->ConOut->ClearScreen (gST->ConOut);

  while (HddPasswordDialog->CheckHddLock (HddPasswordDialog, HddInfoArray, NumOfHdd)) {
    gST->ConOut->SetCursorPosition (
                   gST->ConOut,
                   0,
                   0
                   );
    Print (L"%s", StrTokenArray[STR_HDD_ESC_SKIP_MSG_INDEX]);

    HddPasswordDialog->PrepareHddDialogItems (
                         HddPasswordDialog,
                         HddInfoArray,
                         NumOfHdd,
                         HddDialogItemInfoString
                         );

    H2oDialogProtocol->OneOfOptionDialog (
                         (UINT32)NumOfHdd,
                         TRUE,
                         KeyList,
                         &Key,
                         OPTION_ICON_MAX_STR_SIZE,
                         StrTokenArray[STR_HDD_TITLE_MSG_INDEX],
                         (UINT32 *)&Index,
                         HddDialogItemInfoStringPtr,
                         0
                         );

    Status = HddPasswordDialog->CheckSkipDialogKey (
                                  HddPasswordDialog,
                                  (VOID *)&Key
                                  );

    if (Status != EFI_SUCCESS && (Key.UnicodeChar != CHAR_CARRIAGE_RETURN)) {
      switch (Key.ScanCode) {
      case SCAN_UP:
           if (Index == 0) {
             Index = NumOfHdd - 1;
           } else {
             Index--;
           }
           break;
      case SCAN_DOWN:
           if (Index == (UINTN) (NumOfHdd - 1)) {
             Index = 0;
           } else {
             Index++;
           }
           break;
      default:
           break;
      }
      continue;
    }
    if (Status == EFI_SUCCESS)  {
      //
      //  Skip POST unlock dialog Hot Key is pressed and skip unlock behavior
      //
      break;
    }
    ZeroMem (&PasswordInfo, sizeof(PASSWORD_INFORMATION));
    Status = UnlockSelectedHdd (
               HddPasswordDialog,
               &HddInfoArray[Index],
               &PasswordInfo
               );

    if ((Status == EFI_SUCCESS) && (StrLen(PasswordInfo.PasswordStr) > 0)) {
      //
      // Find corresponding HddPasswordTable according to Index
      //
      HddPasswordTablePtr = (HDD_PASSWORD_TABLE *)HddPasswordTable;
      for (Count = 0; Count < Index; Count++) {
        HddPasswordTablePtr = GetNextTableEntry (HddPasswordTablePtr);
      }

      CopyMem (
        HddPasswordTablePtr->PasswordStr,
        PasswordInfo.PasswordStr,
        StrSize (PasswordInfo.PasswordStr)
        );
      HddPasswordTablePtr->PasswordType     = PasswordInfo.PasswordType;
      HddPasswordTablePtr->ControllerNumber = HddInfoArray[Index].ControllerNumber;
      HddPasswordTablePtr->PortNumber       = HddInfoArray[Index].PortNumber;
      HddPasswordTablePtr->PortMulNumber    = HddInfoArray[Index].PortMulNumber;
      UpdateExtData (HddPasswordTablePtr, HddPasswordTablePtr->PasswordType);
      //
      // zeroing the PWD content in memory after it's being used
      //
      if (HddInfoArray->StorageTcgSecuritySupported) {
        ZeroMem (&PasswordInfo, sizeof(PASSWORD_INFORMATION));
      }

    } else if (Status == EFI_DEVICE_ERROR) {
      ErrorCount[Index]++;
      if (ErrorCount[Index] == PcdGet8(PcdH2OHddPasswordMaxCheckPasswordCount)) {
        ShowErrorCountExpiredMessageAndSystemStop (HddPasswordDialog);
      }
    }

  }

  if (OrgMode != (UINTN) gST->ConOut->Mode->Mode) {
    gST->ConOut->SetMode (gST->ConOut, OrgMode);
  }
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
             (VOID *) HddPasswordTable
             );

  gBS->FreePool (HddDialogItemInfoStringPtr);

  //
  // zeroing the PWD content in memory after it's being used
  //
  if (HddInfoArray->StorageTcgSecuritySupported) {
    ZeroMem (HddPasswordTable,HddPasswordTableSize);
  }


EndOfDialog:

  if (HddPasswordTable != NULL) {
    gBS->FreePool (HddPasswordTable);
  }

  if (HddInfoArray != NULL) {
    gBS->FreePool (HddInfoArray);
  }

  if (ErrorCount != NULL) {
    gBS->FreePool (ErrorCount);
  }

  if (Status != EFI_SUCCESS) {
    return Status;
  }

  return EFI_SUCCESS;

}

EFI_STATUS
HddPasswordUnlocked (
  IN EFI_HDD_PASSWORD_DIALOG_PROTOCOL   *This
  )
{
  HDD_PASSWORD_POST_DIALOG_PRIVATE *HddPasswordPostDialogPrivate;

  HddPasswordPostDialogPrivate = GET_PRIVATE_FROM_HDD_PASSWORD_POST_DIALOG (This);

  HddPasswordPostDialogPrivate->PostUnlocked = TRUE;

  return EFI_SUCCESS;
}

EFI_STATUS
HddUnlockDialogInit (
  IN EFI_HDD_PASSWORD_SERVICE_PROTOCOL  *HddPasswordService
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            Handle;
  VOID                                  *HobList;
  BOOLEAN                               UnlockHddByDialogFlag;
  HDD_PASSWORD_PRIVATE                  *HddPasswordPrivate;
  H2O_DIALOG_PROTOCOL                   *H2oDialogProtocol;

  Handle = NULL;
  //
  // Get Hob list to check boot mode
  //
  HobList           = NULL;
  UnlockHddByDialogFlag = TRUE;



  mHddPasswordPostDialogPrivate = (HDD_PASSWORD_POST_DIALOG_PRIVATE *)AllocateZeroPool (sizeof (HDD_PASSWORD_POST_DIALOG_PRIVATE));
  if (mHddPasswordPostDialogPrivate == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  mHddPasswordPostDialogPrivate->Signature = HDD_PASSWORD_POST_DIALOG_SIGNATURE;



  Status = gBS->LocateProtocol (
                  &gH2ODialogProtocolGuid,
                  NULL,
                  (VOID **)&H2oDialogProtocol
                  );
  if (Status != EFI_SUCCESS) {
    return Status;
  }



  mHddPasswordPostDialogPrivate->H2oDialogProtocol = H2oDialogProtocol;
  mHddPasswordPostDialogPrivate->HddPasswordService = HddPasswordService;

  HddPasswordPrivate = GET_PRIVATE_FROM_HDD_PASSWORD (HddPasswordService);


  mHddPasswordPostDialogPrivate->HddPasswordDialogProtocol.GetStringTokenArray           = GetStringTokenArray;
  mHddPasswordPostDialogPrivate->HddPasswordDialogProtocol.CheckSkipDialogKey            = CheckSkipDialogKey;
  mHddPasswordPostDialogPrivate->HddPasswordDialogProtocol.ResetAllSecurityStatus        = ResetAllSecurityStatus;
  mHddPasswordPostDialogPrivate->HddPasswordDialogProtocol.CheckHddLock                  = CheckHddLock;
  mHddPasswordPostDialogPrivate->HddPasswordDialogProtocol.CheckHddSecurityEnable        = CheckHddSecurityEnable;
  mHddPasswordPostDialogPrivate->HddPasswordDialogProtocol.PrepareHddDialogItems         = PrepareHddDialogItems;
  mHddPasswordPostDialogPrivate->HddPasswordDialogProtocol.GetModelNumber                = GetHddModelNumber;
  mHddPasswordPostDialogPrivate->HddPasswordDialogProtocol.UnlockSelectedHdd             = UnlockSelectedHdd;
  mHddPasswordPostDialogPrivate->HddPasswordDialogProtocol.SelectedHddPrepareTitleString = SelectedHddPrepareTitleString;
  mHddPasswordPostDialogPrivate->HddPasswordDialogProtocol.IsAdmiSecureBootReset         = IsAdmiSecureBootReset;
  mHddPasswordPostDialogPrivate->HddPasswordDialogProtocol.HddPasswordUnlocked           = HddPasswordUnlocked;

  //
  //  Init String for HDD Password Dialog
  //
  InitDialogStringTokenArray ();

  //
  // Install HddPasswordDialogProtocol
  //
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiHddPasswordDialogProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &(mHddPasswordPostDialogPrivate->HddPasswordDialogProtocol)
                  );
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  if (mHddPasswordPostDialogPrivate->PostUnlocked != TRUE) {

    Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
    if (Status == EFI_SUCCESS) {

      if (GetBootModeHob () == BOOT_ON_S4_RESUME || IsAdmiSecureBootReset (&(mHddPasswordPostDialogPrivate->HddPasswordDialogProtocol))) {

        HddPasswordService->UnlockAllHdd (
          HddPasswordService
          );

        UnlockHddByDialogFlag = FALSE;
      }
    }

    if (UnlockHddByDialogFlag) {
      HddUnlockDialog (
        &(mHddPasswordPostDialogPrivate->HddPasswordDialogProtocol)
        );
    }
  }


  //
  //  Release String for HDD Password Dialog
  //
  ReleaseStringTokenArray ();

  return EFI_SUCCESS;

}

