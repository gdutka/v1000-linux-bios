/** @file
  Setup utility library related functions

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

#include "SetupUtilityLibCommon.h"


STATIC LANGUAGE_DATA_BASE  *mSupportedLangTable;
STATIC CONST CHAR16 mConfigHdrTemplate[] = L"GUID=00000000000000000000000000000000&NAME=0000&PATH=00";
/**
  Get next language from language code list (with separator ';').

  If LangCode is NULL, then ASSERT.
  If Lang is NULL, then ASSERT.

  @param  LangCode               On input: point to first language in the list.
                                 On output: point to next language in the list, or
                                 NULL if no more language in the list.
  @param  Lang                   The first language in the list.

**/
VOID
EFIAPI
SetupUtilityLibGetNextLanguage (
  IN OUT CHAR8      **LangCode,
  OUT CHAR8         *Lang
  )
{
  UINTN  Index;
  CHAR8  *StringPtr;

  ASSERT (LangCode != NULL);
  ASSERT (*LangCode != NULL);
  ASSERT (Lang != NULL);

  Index = 0;
  StringPtr = *LangCode;
  while (StringPtr[Index] != 0 && StringPtr[Index] != ';') {
    Index++;
  }

  CopyMem (Lang, StringPtr, Index);
  Lang[Index] = 0;

  if (StringPtr[Index] == ';') {
    Index++;
  }
  *LangCode = StringPtr + Index;
}

/**
  Given a token, return the string.

  @param  HiiHandle              The handle the token is located
  @param  Token                  The string reference
  @param  LanguageString         indicate what language string we want to get. if this is a
                                 NULL pointer, using the current language setting to get string

  @retval  Pointer to the string corresponding to the token or NULL if
           it cannot get string from Hii database

**/
CHAR16 *
SetupUtilityLibGetTokenStringByLanguage (
  IN EFI_HII_HANDLE                             HiiHandle,
  IN EFI_STRING_ID                              Token,
  IN CHAR8                                      *LanguageString
  )
{
  CHAR16                                      *Buffer;
  UINTN                                       BufferLength;
  EFI_STATUS                                  Status;
  EFI_HII_STRING_PROTOCOL                     *HiiString;
  SETUP_UTILITY_BROWSER_DATA                  *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  HiiString = SuBrowser->HiiString;
  //
  // Set default string size assumption at no more than 256 bytes
  //
  BufferLength = 0x100;

  Buffer = AllocatePool (BufferLength);
  if (Buffer == NULL) {
    return NULL;
  }
  ZeroMem (Buffer, BufferLength);
  Status = HiiString->GetString (
                        HiiString,
                        LanguageString,
                        HiiHandle,
                        Token,
                        Buffer,
                        &BufferLength,
                        NULL
                        );
  if (EFI_ERROR (Status)) {
    if (Status == EFI_BUFFER_TOO_SMALL) {
      //
      // Free the old pool
      //
      FreePool (Buffer);

      //
      // Allocate new pool with correct value
      //
      Buffer = AllocatePool (BufferLength);
      if (Buffer == NULL) {
        return NULL;
      }
      Status = HiiString->GetString (
                            HiiString,
                            LanguageString,
                            HiiHandle,
                            Token,
                            Buffer,
                            &BufferLength,
                            NULL
                            );
      if (!EFI_ERROR (Status)) {
        //
        // return searched string
        //
        return Buffer;
      }
    }
    //
    // Cannot find string, free buffer and return NULL pointer
    //
    FreePool (Buffer);
    Buffer = NULL;
    return Buffer;
  }
  //
  // return searched string
  //
  return Buffer;
}


/**
  According the priority of langdef in UNI file to add the supported language code
  to supported language database.

  @retval EFI_SUCCESS    Initialize supported language database successful
  @retval Other          Get setup utility browser data fail

**/
STATIC
EFI_STATUS
InitializeSupportLanguage (
  VOID
  )
{
  CHAR8                         *Language;
  CHAR8                         Lang[RFC_3066_ENTRY_SIZE];
  CHAR8                         *SuportedLanguage;
  UINTN                         SupportedLangCnt;

  SuportedLanguage = CommonGetVariableData (L"PlatformLangCodes", &gEfiGlobalVariableGuid);
  if (SuportedLanguage == NULL) {
    return EFI_NOT_FOUND;
  }

  SupportedLangCnt = 0;
  Language  = SuportedLanguage;
  while (*Language != 0) {
    SetupUtilityLibGetNextLanguage (&Language, Lang);
    SupportedLangCnt++;
  }

  mSupportedLangTable = AllocateZeroPool (SupportedLangCnt * RFC_3066_ENTRY_SIZE + sizeof (UINTN));
  if (mSupportedLangTable == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  FreePool (SuportedLanguage);
  SuportedLanguage = CommonGetVariableData (L"PlatformLangCodes", &gEfiGlobalVariableGuid);
  if (SuportedLanguage == NULL) {
    FreePool (mSupportedLangTable);
    mSupportedLangTable = NULL;
    return EFI_NOT_FOUND;
  }

  SupportedLangCnt = 0;
  Language  = SuportedLanguage;
  while (*Language != 0) {
    SetupUtilityLibGetNextLanguage (&Language, Lang);
    AsciiStrCpyS ((CHAR8 *) &mSupportedLangTable->LangString[SupportedLangCnt * RFC_3066_ENTRY_SIZE], RFC_3066_ENTRY_SIZE, Lang);
    SupportedLangCnt++;
  }

  mSupportedLangTable->LangNum = SupportedLangCnt;
  FreePool (SuportedLanguage);

  return EFI_SUCCESS;

}

/**
  Get supported language database. This funciton will return supported language number
  and language string

  @param  LangNumber             Pointer to supported language number
  @param  LanguageString         A double pointer to save the start of supported language string

  @retval EFI_SUCCESS            Initialize supported language database successful
  @retval EFI_INVALID_PARAMETER  Input parameter is invalid.
  @retval EFI_OUT_OF_RESOURCES   Unable allocate memory for language string.

**/
EFI_STATUS
GetLangDatabase (
  OUT UINTN            *LangNumber,
  OUT UINT8            **LanguageString
  )
{
  UINTN       TotalSize;
  EFI_STATUS  Status;

  if (LangNumber == NULL || LanguageString == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (mSupportedLangTable == NULL) {
    Status = InitializeSupportLanguage ();
    if (EFI_ERROR(Status) || mSupportedLangTable == NULL) {
      return Status;
    }
  }

  TotalSize = mSupportedLangTable->LangNum * RFC_3066_ENTRY_SIZE;
  *LanguageString = AllocateZeroPool (TotalSize);
  if (*LanguageString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CopyMem (*LanguageString, mSupportedLangTable->LangString, TotalSize);
  *LangNumber = mSupportedLangTable->LangNum;
  return EFI_SUCCESS;
}

/**
  Get SETUP_UTILITY_BROWSER_DATA instance from gEfiSetupUtilityBrowserProtocolGuid

  @param  SuBrowser              Double pointer point to SETUP_UTILITY_BROWSER_DATA instance

  @retval EFI_SUCCESS            Get SETUP_UTILITY_BROWSER_DATA instance successful
  @retval Other                  Cannot locate gEfiSetupUtilityBrowserProtocolGuid

**/
EFI_STATUS
GetSetupUtilityBrowserData (
  OUT SETUP_UTILITY_BROWSER_DATA    **SuBrowser
  )
{
  EFI_STATUS                                Status;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL        *Interface;


  Status = gBS->LocateProtocol (
               &gEfiSetupUtilityBrowserProtocolGuid,
               NULL,
               (VOID **) &Interface
               );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  *SuBrowser = EFI_SETUP_UTILITY_BROWSER_FROM_THIS (Interface);

  return Status;;
}

/**
  According platform setting to configure setup variable

  @param  VariableGuid           An optional field to indicate the target variable GUID name to use.
  @param  VariableName           An optional field to indicate the target human-readable variable name.
  @param  BufferSize             On input: Length in bytes of buffer to hold retrived data.
                                 On output:
                                   If return EFI_BUFFER_TOO_SMALL, containg length of buffer desired.
  @param  Buffer                 Buffer to hold retrived data.
  @param  RetrieveData           TRUE : Get setup variable from broswer
                                 FALSE: Set current setuputility setting to browser

  @retval EFI_SUCCESS            Setup variable configuration successful
  @retval EFI_ABORTED            Setup variable configuration failed

**/
EFI_STATUS
SetupVariableConfig (
  IN EFI_GUID        *VariableGuid, OPTIONAL
  IN CHAR16          *VariableName, OPTIONAL
  IN UINTN           BufferSize,
  IN UINT8           *Buffer,
  IN BOOLEAN         RetrieveData
  )
{
  BOOLEAN            Success;

  if (RetrieveData) {
    Success = HiiGetBrowserData (VariableGuid, VariableName, BufferSize, Buffer);
  } else {
    Success = HiiSetBrowserData (VariableGuid, VariableName, BufferSize, Buffer, NULL);
  }
  if (!Success) {
    DEBUG ((EFI_D_ERROR, "SetupVariableConfig %s variable (%g, %s) fail\n", RetrieveData ? L"Get" : L"Set", VariableGuid, VariableName));
  }

  return Success ? EFI_SUCCESS : EFI_ABORTED;
}

/**
  Convert ASCII string to Unicode string in fixed length.

  @param  AsciiString            Input ASCII string
  @param  UnicodeString          Output Unicode string
  @param  Length                 The string length of ASCII

  @retval EFI_SUCCESS            Convert ASCII to Unicode string successfully

**/
EFI_STATUS
SetupUtilityLibAsciiToUnicode (
  IN    CHAR8     *AsciiString,
  IN    CHAR16    *UnicodeString,
  IN    UINTN     Length
  )
{
  UINT8           Index;

  for (Index = 0; Index < Length; Index++) {
    UnicodeString[Index] = (CHAR16) AsciiString[Index];
  }

  return EFI_SUCCESS;
}

/**
  Function to update the ATA strings into Model Name -- Size

  @param  IdentifyDriveInfo      Data of ATA or ATAPI device
  @param  BufSize                Output buffer size
  @param  BootString             Buffer to contain output string

  @retval EFI_SUCCESS            Update ATA string successfully

**/
EFI_STATUS
SetupUtilityLibUpdateAtaString(
  IN      EFI_IDENTIFY_DATA                 *IdentifyDriveInfo,
  IN      UINTN                             BufSize,
  IN OUT  CHAR16                            **BootString
  )
{
  CHAR8                                     *TempString;
  UINT16                                    Index;
  CHAR8                                     Temp8;

  TempString = AllocateZeroPool (0x100);
  if (TempString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CopyMem (
    TempString,
    IdentifyDriveInfo->AtapiData.ModelName,
    sizeof(IdentifyDriveInfo->AtapiData.ModelName)
    );

  //
  // Swap the IDE string since Identify Drive format is inverted
  //
  Index = 0;
  while (TempString[Index] != 0 && TempString[Index+1] != 0) {
    Temp8 = TempString[Index];
    TempString[Index] = TempString[Index+1];
    TempString[Index+1] = Temp8;
    Index +=2;
  }
  SetupUtilityLibAsciiToUnicode (TempString, *BootString, (UINTN) Index);
  FreePool(TempString);

  return EFI_SUCCESS;
}


/**
 Check if language code is support in system or not.

 @param[in] LangCode            Pointer to the language code

 @retval TRUE                   Language code is support in system.
 @retval FALSE                  Language code is not support in system.
**/
BOOLEAN
SetupUtilityLibIsLangCodeSupport (
  IN CHAR8                        *LangCode
  )
{
  CHAR8                         *SuuportedLanguage;
  CHAR8                         *LangStrings;
  BOOLEAN                       Support;
  CHAR8                         Lang[RFC_3066_ENTRY_SIZE];

  if (LangCode == NULL) {
    return FALSE;
  }

  SuuportedLanguage = CommonGetVariableData (L"PlatformLangCodes", &gEfiGlobalVariableGuid);
  if (SuuportedLanguage == NULL) {
    return FALSE;
  }

  Support = FALSE;
  LangStrings = SuuportedLanguage;
  while (*LangStrings != 0) {
    SetupUtilityLibGetNextLanguage (&LangStrings, Lang);

    if (AsciiStrCmp (Lang, LangCode) == 0) {
      Support = TRUE;
      break;
    }
  }

  FreePool (SuuportedLanguage);

  return Support;
}

/**
 Get variable store size in bytes from HII package

 @param[in]  Package             HII package data
 @param[in]  FormsetGuid         Formset GUID
 @param[in]  VarStoreGuid        Variable store GUID
 @param[in]  VarStoreName        Variable store name
 @param[out] VarStoreSize        Variable store size

 @retval EFI_SUCCESS             Get size successfully
 @retval EFI_INVALID_PARAMETER   Package or FormsetGuid is NULL or it is not form package
 @retval EFI_NOT_FOUND           Form package or variable store is not found
**/
STATIC
EFI_STATUS
GetVarStoreSizeFromPackage (
  IN  UINT8                     *Package,
  IN  EFI_GUID                  *FormsetGuid,
  IN  EFI_GUID                  *VarStoreGuid,
  IN  CHAR8                     *VarStoreName,
  OUT UINT16                    *VarStoreSize
  )
{
  EFI_HII_PACKAGE_HEADER       *PackageHeader;
  UINT32                       Offset;
  EFI_IFR_OP_HEADER            *OpCodeHdr;
  BOOLEAN                      IsCurrentFormset;
  EFI_IFR_VARSTORE             *VarStore;

  if (Package == NULL || FormsetGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  PackageHeader = (EFI_HII_PACKAGE_HEADER *) Package;
  if (PackageHeader->Type != EFI_HII_PACKAGE_FORMS) {
    return EFI_INVALID_PARAMETER;
  }

  IsCurrentFormset = FALSE;
  Offset = sizeof (EFI_HII_PACKAGE_HEADER);
  while (Offset < PackageHeader->Length) {
    OpCodeHdr = (EFI_IFR_OP_HEADER *) (Package + Offset);

    if (OpCodeHdr->OpCode == EFI_IFR_FORM_SET_OP) {
      if (CompareGuid ((EFI_GUID *) (VOID *) (&((EFI_IFR_FORM_SET *) OpCodeHdr)->Guid), FormsetGuid)) {
        IsCurrentFormset = TRUE;
      } else {
        IsCurrentFormset = FALSE;
      }
    }

    if (IsCurrentFormset && OpCodeHdr->OpCode == EFI_IFR_VARSTORE_OP) {
      VarStore = (EFI_IFR_VARSTORE *) OpCodeHdr;

      if (CompareGuid ((EFI_GUID *) (VOID *) (&VarStore->Guid), VarStoreGuid) &&
          AsciiStrCmp ((CHAR8 *) VarStore->Name, VarStoreName) == 0) {
        *VarStoreSize = VarStore->Size;
        return EFI_SUCCESS;
      }
    }

    Offset += OpCodeHdr->Length;
  }

  return EFI_NOT_FOUND;
}

/**
 Get variable store size in bytes

 @param[in] HiiHandle           An EFI_HII_HANDLE that corresponds to the desired package list in the HII database
 @param[in] FormsetGuid         Formset GUID
 @param[in] VarStoreGuid        Variable store GUID
 @param[in] VarStoreName        Variable store name

 @return The size of variable store in bytes
**/
UINT16
GetVarStoreSize (
  IN EFI_HII_HANDLE            HiiHandle,
  IN EFI_GUID                  *FormsetGuid,
  IN EFI_GUID                  *VarStoreGuid,
  IN CHAR8                     *VarStoreName
  )
{
  EFI_STATUS                   Status;
  EFI_HII_DATABASE_PROTOCOL    *HiiDatabase;
  UINTN                        HiiPackageListSize;
  EFI_HII_PACKAGE_LIST_HEADER  *HiiPackageList;
  UINT8                        *Package;
  EFI_HII_PACKAGE_HEADER       *PackageHeader;
  UINT32                       Offset;
  UINT32                       PackageListLength;
  UINT16                       VarStoreSize;

  VarStoreSize = 0;

  if (HiiHandle == NULL || FormsetGuid == NULL || VarStoreGuid == NULL || VarStoreName == NULL) {
    return VarStoreSize;
  }

  Status = gBS->LocateProtocol (&gEfiHiiDatabaseProtocolGuid, NULL, (VOID **) &HiiDatabase);
  if (EFI_ERROR (Status)) {
    return VarStoreSize;
  }

  //
  // Get HII package list
  //
  HiiPackageList     = NULL;
  HiiPackageListSize = 0;
  Status = HiiDatabase->ExportPackageLists (HiiDatabase, HiiHandle, &HiiPackageListSize, HiiPackageList);
  if (Status != EFI_BUFFER_TOO_SMALL) {
    return VarStoreSize;
  }
  HiiPackageList = AllocateZeroPool (HiiPackageListSize);
  if (HiiPackageList == NULL) {
    return VarStoreSize;
  }
  Status = HiiDatabase->ExportPackageLists (HiiDatabase, HiiHandle, &HiiPackageListSize, HiiPackageList);
  if (EFI_ERROR (Status)) {
    FreePool (HiiPackageList);
    return VarStoreSize;
  }

  //
  // In HII package list, find the variable store size which match the formset GUID, variable store GUID and name.
  //
  Offset            = sizeof (EFI_HII_PACKAGE_LIST_HEADER);
  PackageListLength = ReadUnaligned32 (&HiiPackageList->PackageLength);
  while (Offset < PackageListLength) {
    Package       = (UINT8 *) HiiPackageList + Offset;
    PackageHeader = (EFI_HII_PACKAGE_HEADER *) Package;

    if (PackageHeader->Type == EFI_HII_PACKAGE_FORMS) {
      Status = GetVarStoreSizeFromPackage (Package, FormsetGuid, VarStoreGuid, VarStoreName, &VarStoreSize);
      if (!EFI_ERROR (Status)) {
        break;
      }
    }

    Offset += PackageHeader->Length;
  }

  FreePool (HiiPackageList);

  return VarStoreSize;
}

/**
 Send goto hot key event.

 @param[in] FormId               Target form ID
 @param[in] QuestionId           Target question ID

 @retval EFI_SUCCESS             Send goto hot key event successfully.
 @retval Other                   Locate form browser protocol failed or notify event failed.
**/
EFI_STATUS
SendGotoHotKeyNotify (
  IN UINT16                               FormId,
  IN UINT16                               QuestionId
  )
{
  EFI_STATUS                              Status;
  H2O_FORM_BROWSER_PROTOCOL               *FBProtocol;
  H2O_DISPLAY_ENGINE_EVT_HOT_KEY          HotKeyNotify;

  Status = gBS->LocateProtocol (&gH2OFormBrowserProtocolGuid, NULL, (VOID **) &FBProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (&HotKeyNotify, sizeof (HotKeyNotify));
  HotKeyNotify.Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_HOT_KEY);
  HotKeyNotify.Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_HOT_KEY;
  HotKeyNotify.Hdr.Target = H2O_DISPLAY_ENGINE_EVT_TARGET_FORM_BROWSER;

  HotKeyNotify.HotKeyAction           = HotKeyGoTo;
  HotKeyNotify.HotKeyTargetFormId     = FormId;
  HotKeyNotify.HotKeyTargetQuestionId = QuestionId;

  return FBProtocol->Notify (FBProtocol, &HotKeyNotify.Hdr);
}
/**
  Send shut down notify to close the freeze confirm dialog

  @retval EFI_SUCCESS             shut down the freeze confirm dialog successfully

**/

EFI_STATUS
SendShutDNotifyForInternal (
  VOID
  )
{
  EFI_STATUS                                  Status;
  H2O_FORM_BROWSER_PROTOCOL                   *FBProtocol;
  H2O_DISPLAY_ENGINE_EVT_SHUT_D               ShutDNotify;

  Status = gBS->LocateProtocol (&gH2OFormBrowserProtocolGuid, NULL, (VOID **) &FBProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (&ShutDNotify, sizeof (ShutDNotify));
  ShutDNotify.Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_SHUT_D);
  ShutDNotify.Hdr.Target = H2O_DISPLAY_ENGINE_EVT_TARGET_BROADCAST;
  ShutDNotify.Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_SHUT_D;

  return FBProtocol->Notify (FBProtocol, &ShutDNotify.Hdr);
}

/**
  Load the default value of all varstores belong to setup utility library.

  @retval EFI_SUCCESS             Load the default value successfully

**/
EFI_STATUS
SetupUtilityLibLoadDefault (
  VOID
  )
{
  BootLoadDefault ();
  SecurityLoadDefault ();

  return EFI_SUCCESS;
}

/**
 Save the current settings of all varstores belong to setup utility library.

 @retval EFI_SUCCESS             Save the current settings successfully
 @retval Other                   Save the current settings failed.
**/
EFI_STATUS
SetupUtilityLibRouteConfig (
  VOID
  )
{
  EFI_STATUS                              Status;

  //
  // If Setup variable is changed, Win 8 fast boot should not be active at next boot.
  //
  SetVariableToSensitiveVariable (
    L"TargetHddDevPath",
    &gEfiGenericVariableGuid,
    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
    0,
    NULL
    );

  Status = SecuritySaveSetting ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "SecuritySaveSetting failed!\n"));
  }

  Status = BootSaveSetting ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "BootSaveSetting failed!\n"));
  }

  return Status;
}

