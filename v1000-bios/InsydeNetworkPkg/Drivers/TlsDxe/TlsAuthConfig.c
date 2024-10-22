/** @file
  TLS certificate management related functions.

;******************************************************************************
;* Copyright (c) 2019 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "TlsAuthConfig.h"
#include "TlsAuthConfigNvData.h"
#include <Library/UefiBootManagerLib.h>
#include <Library/DevicePathLib.h>
#include <Guid/MdeModuleHii.h>

STATIC CHAR16                       mTlcAuthManageStorageName[] = L"TLS_AUTH_CERT_MANAGE_IFR_NVDATA";
STATIC CRYPTO_SERVICES_PROTOCOL     *mCryptoServices;
STATIC BOOLEAN                      mgH2OFormBrowserProtocoSupported = FALSE;

STATIC ENROLL_MENU_OPTION           mFileListFsOptionMenu = {
  EM_MENU_ENTRY_SIGNATURE,
  NULL,
  0
};
STATIC ENROLL_MENU_OPTION           mFileListDirectoryMenu =  {
  EM_MENU_ENTRY_SIGNATURE,
  NULL,
  0
};
STATIC ENROLL_MENU_OPTION           mFileListCertFileMenu = {
  EM_MENU_ENTRY_SIGNATURE,
  NULL,
  0
};
STATIC EFI_TEXT_STRING              mOriginalOutputString;
STATIC STRING_DEPOSITORY            *mSecureBootFileOptionStrDepository;

STATIC
EFI_STATUS
ProcessFile (
  IN TLS_CERT_MANAGER_CALLBACK_DATA   *CallbackData,
  IN ENROLL_MENU_ENTRY                 *MenuEntry,
  IN EM_FILE_CONTEXT                   *FileContext
  );

STATIC
EFI_STATUS
ProcessCertificatesFromCertStore (
  IN TLS_CERT_MANAGER_CALLBACK_DATA    *CallbackData,
  IN UINT16                            SelectIndex,
  IN CHAR16                            *OutputFileName
  );

STATIC
BOOLEAN
IsGuidExist (
  IN   EFI_GUID                        *TargetCertGuid
  );

/**
  Reclaim string depository by moving the current node pointer to list head.

  @param[in] StringDepository   Pointer to the string repository
**/
VOID
FileListReclaimStrDepository (
  IN STRING_DEPOSITORY                 *StrDepository
  )
{
  if (StrDepository != NULL) {
    StrDepository->CurrentNode = StrDepository->ListHead;
  }
}

/**
  Translate PEM to DER.

  @param[in]  Data                     Pointer to X509 data, it's possible PEM or DER content.
  @param[in]  DataSize                 size of X509 data.
  @param[out] DerData                  Pointer to data pointer to output translated DER data.
  @param[out] DerDataSize              Pointer the size pointer to output DER data size.

  @return String token or zero if input parameter is NULL or fail to allocate memory.
**/
STATIC
EFI_STATUS
TranslatePemToDer (
  IN   UINT8                           *Data,
  IN   UINTN                           DataSize,
  OUT  UINT8                           **DerData,
  OUT  UINTN                           *DerSize
  ) 
{
  BOOLEAN                              BooleanStatus;
  UINT8                                *TmpDerData;
  UINTN                                TmpDerSize;

  if (mCryptoServices == NULL) {
    return EFI_UNSUPPORTED;
  }

  TmpDerData = NULL;
  TmpDerSize = 0;
  BooleanStatus = mCryptoServices->X509GetCaCertificateFromPemToDer (Data, DataSize, &TmpDerData, &TmpDerSize);
  if (!BooleanStatus) {
    return EFI_NOT_FOUND;
  }
  *DerData = TmpDerData;
  *DerSize = TmpDerSize;
  return EFI_SUCCESS;
}

/**
  Fetch a usable string node from the string depository and return the string token.

  @param[in] CallbackData             Pointer to SECURE_BOOT_MANAGER_CALLBACK_DATA instance
  @param[in] StringDepository         Pointer of the string depository

  @return String token or zero if input parameter is NULL or fail to allocate memory.
**/
STATIC
EFI_STRING_ID
FileListGetStrTokenFromDepository (
  IN TLS_CERT_MANAGER_CALLBACK_DATA   *CallbackData,
  IN STRING_DEPOSITORY                 *StringDepository
  )
{
  STRING_LIST_NODE                     *CurrentListNode;
  STRING_LIST_NODE                     *NextListNode;

  if (CallbackData == NULL || StringDepository == NULL) {
    return 0;
  }

  CurrentListNode = StringDepository->CurrentNode;

  if ((NULL != CurrentListNode) && (NULL != CurrentListNode->Next)) {
    //
    // Fetch one reclaimed node from the list.
    //
    NextListNode = StringDepository->CurrentNode->Next;
  } else {
    //
    // If there is no usable node in the list, update the list.
    //
    NextListNode = AllocateZeroPool (sizeof (STRING_LIST_NODE));
    if (NextListNode == NULL) {
      return 0;
    }
    NextListNode->StringToken = HiiSetString (
                                  CallbackData->RegisteredHandle,
                                  NextListNode->StringToken,
                                  L" ",
                                  NULL
                                  );
    ASSERT (NextListNode->StringToken != 0);

    StringDepository->TotalNodeNumber++;

    if (NULL == CurrentListNode) {
      StringDepository->ListHead = NextListNode;
    } else {
      CurrentListNode->Next = NextListNode;
    }
  }

  StringDepository->CurrentNode = NextListNode;

  return StringDepository->CurrentNode->StringToken;
}

/**
  Wrap original FreePool gBS call in order to decrease code length.
**/
STATIC
VOID
SafeFreePool (
  IN VOID                              **Buffer
  )
{
  if (Buffer != NULL && *Buffer != NULL) {
    gBS->FreePool (*Buffer);
    *Buffer = NULL;
  }
}

/**
  Destroy the menu entry passed in

  @param  MenuEntry         The menu entry need to be destroyed

**/
STATIC
VOID
DestroyMenuEntry (
  ENROLL_MENU_ENTRY         *MenuEntry
  )
{
  EM_FILE_CONTEXT           *FileContext;

  FileContext = (EM_FILE_CONTEXT *) MenuEntry->VariableContext;

  if (!FileContext->IsRoot) {
    SafeFreePool ((VOID **) &FileContext->DevicePath);
  } else {
    if (FileContext->FHandle != NULL) {
      FileContext->FHandle->Close (FileContext->FHandle);
    }
  }

  SafeFreePool ((VOID **) &FileContext->FileName);
  SafeFreePool ((VOID **) &FileContext->Info);
  SafeFreePool ((VOID **) &FileContext);
  SafeFreePool ((VOID **) &MenuEntry->DisplayString);
  SafeFreePool ((VOID **) &MenuEntry->HelpString);
  SafeFreePool ((VOID **) &MenuEntry);
}

/**
  Dummy function uses to hook original OutputString ().

  @param[in] This       The protocol instance pointer.
  @param[in] String     The NULL-terminated string to be displayed on the output
                        device(s). All output devices must also support the Unicode
                        drawing character codes defined in this file.

  @retval EFI_SUCCESS   Always return EFI_SUCCESS.
**/
EFI_STATUS
EFIAPI
DummyOutputString (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL        *This,
  IN CHAR16                                 *String
  )
{
  return EFI_SUCCESS;
}

/**
  Fuction to hook original Outputstring function to disable all of
  output strings.

  @retval EFI_SUCCESS   Always return EFI_SUCCESS.
**/
STATIC
EFI_STATUS
HookOutputString (
  VOID
  )
{
  mOriginalOutputString = gST->ConOut->OutputString;
  gST->ConOut->OutputString = DummyOutputString;

  return EFI_SUCCESS;
}

/**
  Fuction to restore original Outputstring function to ConOut in
  system table.

  @retval EFI_SUCCESS   Always return EFI_SUCCESS.
**/
STATIC
EFI_STATUS
RestoreOutputString (
  VOID
  )
{
  gST->ConOut->OutputString = mOriginalOutputString;

  return EFI_SUCCESS;
}


/**
  Generate GUID, Calculate hash of Subject and Issuer.
  Use this hash as GUID value.

  @param[in]   Subject         Input Subject binary.
  @param[in]   SubjectSize     Input size of Subject binary.
  @param[in]   Issuer          Input Issuer binary.
  @param[in]   IssuerSize      Input size of Issuer binary.
  @param[out]  Guid            Generated GUID value.

**/
STATIC
EFI_STATUS
GenerateGuidFromX509 (
  IN UINT16                            *Subject,
  IN UINTN                             SubjectSize,
  IN UINT16                            *Issuer,
  IN UINTN                             IssuerSize,
  OUT EFI_GUID                         *Guid
  )
{
  UINT8                                *Context;
  UINTN                                ContextSize;
  BOOLEAN                              HashStatus;

  UINT8                                HashOutput[20];

  if (mCryptoServices == NULL) {
    return EFI_NOT_READY;
  }

  if ((Subject == NULL) || (Issuer == NULL) || (Guid == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  ContextSize = mCryptoServices->Sha1GetContextSize ();
  Context = AllocateZeroPool (ContextSize);
  if (Context == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  HashStatus = mCryptoServices->Sha1Init (Context);
  if (!HashStatus) {
    goto ON_EXIT;
  }

  HashStatus = mCryptoServices->Sha1Update (Context, Subject, SubjectSize);
  if (!HashStatus) {
    goto ON_EXIT;
  }

  HashStatus = mCryptoServices->Sha1Update (Context, Issuer, IssuerSize);
  if (!HashStatus) {
    goto ON_EXIT;
  }

  HashStatus = mCryptoServices->Sha1Final (Context, HashOutput);
  if (!HashStatus) {
    goto ON_EXIT;
  }
  CopyMem (Guid, HashOutput, sizeof (EFI_GUID));

ON_EXIT:
  if (Context != NULL) {
    FreePool (Context);
  }
  return (!HashStatus) ? EFI_NOT_READY : EFI_SUCCESS;
}

/**
  CA parse and dump data function, modified from TlsChkCaCertificate function.

  @param[in]  Data                 Pointer to the data buffer of a DER-encoded binary
                                   X.509 certificate or PEM-encoded X.509 certificate.
  @param[in]  DataSize             The size of data buffer in bytes.

  @retval  EFI_SUCCESS             The operation succeeded.
  @retval  EFI_INVALID_PARAMETER   The parameter is invalid.
  @retval  EFI_OUT_OF_RESOURCES    Required resources could not be allocated.
  @retval  EFI_ABORTED             Invalid X.509 certificate.
  @retval  others                  Error status from external functions.

**/
STATIC
EFI_STATUS
DumpCaCertificate (
  IN     VOID                     *Data,
  IN     UINTN                    DataSize,
  OUT    CHAR16                   **SubjectName,
  OUT    CHAR16                   **IssuerName,
  OUT    EFI_GUID                 *CertGuid
  )
{
  EFI_STATUS      Status;
  BOOLEAN         BooleanStatus;
  UINTN           SubjectSize;
  UINTN           IssuerSize;
  UINT8           *CertData;
  UINTN           CertSize;
  UINT8           *DerData;
  UINTN           DerSize;
  CHAR16          *SubjectNameStr;
  CHAR16          *IssuerNameStr;

  if (mCryptoServices == NULL) {
    return EFI_NOT_READY;
  }

  if ((Data == NULL) || (DataSize == 0) 
    || (SubjectName == NULL) || (IssuerName == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  DerSize        = 0;
  DerData        = NULL;
  CertData       = Data;
  CertSize       = DataSize;
  Status = TranslatePemToDer (Data, DataSize, &DerData, &DerSize);
  if (!EFI_ERROR (Status)) {
    CertData = DerData;
    CertSize = DerSize;
  }
  
  SubjectSize    = 0;
  IssuerSize     = 0;
  SubjectNameStr = NULL;
  IssuerNameStr  = NULL;
  BooleanStatus = mCryptoServices->X509GetSubjectNameEx (CertData, CertSize, &SubjectNameStr, &SubjectSize);
  if (!BooleanStatus) {
    /// SubjectName get failed.
    Status = EFI_ABORTED;
    goto ON_EXIT;
  }

  BooleanStatus = mCryptoServices->X509GetIssuerNameEx (CertData, CertSize, &IssuerNameStr, &IssuerSize);
  if (!BooleanStatus) {
    /// IssuerName get failed.
    Status = EFI_ABORTED;
    goto ON_EXIT;
  }

  //
  // Use SubjectName and IssuerName to generate GUID.
  //
  Status = GenerateGuidFromX509 (SubjectNameStr, SubjectSize, IssuerNameStr, IssuerSize, CertGuid);
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

  *SubjectName = SubjectNameStr;
  *IssuerName  = IssuerNameStr;
  if (DerData != NULL) {
    FreePool (DerData);
  }
  return EFI_SUCCESS;
  
ON_EXIT:
  if (SubjectNameStr != NULL) {
    FreePool (SubjectNameStr);
  }
  if (IssuerNameStr != NULL) {
    FreePool (IssuerNameStr);
  }
  if (DerData != NULL) {
    FreePool (DerData);
  }
  return Status;
}

/**
  Decode X509 certificate to a string content SubjectName and IssuerName.

  @param[in]  X509Data             Pointer to the data buffer of a DER-encoded binary
                                   X.509 certificate or PEM-encoded X.509 certificate.
  @param[in]  X509DataSize         Size of X509 binary data.
  @param[out] X509DataInfo         String content SubjectName and IssuerName.
  @param[out] Guid                 GUID generated by SubjectName and IssuerName.          

  @retval  EFI_SUCCESS             The operation succeeded.
  @retval  EFI_INVALID_PARAMETER   The parameter is invalid.
  @retval  EFI_OUT_OF_RESOURCES    Required resources could not be allocated.
  @retval  EFI_NOT_READY           CryptoServices protocol is not ready.
  @retval  others                  Error status from external functions.

**/
STATIC
EFI_STATUS
DecodeX509ToSubjectAndIssuerName (
  IN VOID                              *X509Data,
  IN UINTN                             X509DataSize,
  OUT CHAR16                           **X509DataInfo,
  OUT EFI_GUID                         *Guid
  )
{
  EFI_STATUS                           Status;
  CHAR16                               *SubjectName;
  CHAR16                               *IssuerName;
  CHAR16                               *OutputStringBuf;
  UINTN                                OutputStringMaxSize;

  if ((X509Data == NULL) || (X509DataInfo == NULL) || (Guid == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (mCryptoServices == NULL) {
    return EFI_NOT_READY;
  }

  SubjectName        = NULL;
  IssuerName         = NULL;
  Status = DumpCaCertificate (X509Data, X509DataSize, &SubjectName, &IssuerName, Guid);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  OutputStringMaxSize = sizeof (CHAR16) * TLS_X509_INFO_STRING_MAX_SIZE;
  OutputStringBuf = AllocateZeroPool (OutputStringMaxSize);
  if (OutputStringBuf == NULL) {
    goto ON_ERROR;
  }
  UnicodeSPrint (OutputStringBuf, OutputStringMaxSize, L"SubjectName:%s\nIssuerName:%s", SubjectName, IssuerName);

  *X509DataInfo = OutputStringBuf;
  OutputStringBuf = NULL;
  Status = EFI_SUCCESS;

ON_ERROR:
  if (SubjectName != NULL) {
    FreePool (SubjectName);
  }
  if (IssuerName != NULL) {
    FreePool (IssuerName);
  }
  if (OutputStringBuf != NULL) {
    FreePool (OutputStringBuf);
  }
  return Status;
}

/**
  Read file based on the input file context.

  @param[in]  FileContext       Pointer to the file context
  @param[out] FileSize          Returns the size of the file that was read
  @param[out] File              Dynamically allocated memory that contains a copy of the file

  @retval EFI_SUCCESS           Read file successfully.
  @retval EFI_UNSUPPORTED       File context is invalid or load file function return failed.
  @retval EFI_OUT_OF_RESOURCES  Fail to allocate memory.
  @retval EFI_NOT_FOUND         Fail to get file info.
  @retval Other                 Fail to get load file protocol instance or open/read file.
**/
STATIC
EFI_STATUS
ReadFile (
  IN  EM_FILE_CONTEXT                  *FileContext,
  OUT UINTN                            *FileSize,
  OUT UINT8                            **File
  )
{
  EFI_FILE_HANDLE                      Dir;
  EFI_FILE_HANDLE                      NewDir;
  EFI_FILE_INFO                        *DirInfo;
  EFI_STATUS                           Status;
  UINTN                                BufferSize;
  UINT8                                *Buffer;
  EFI_LOAD_FILE_PROTOCOL               *LoadFile;

  if (FileContext->IsLoadFile && !FileContext->IsBootLegacy) {
    if (FileContext->Handle == NULL) {
      return EFI_UNSUPPORTED;
    }
    Status = gBS->HandleProtocol (FileContext->Handle, &gEfiLoadFileProtocolGuid, (VOID**)&LoadFile);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    HookOutputString ();
    //
    // Call LoadFile with the correct buffer size
    //
    BufferSize = 0;
    Buffer     = NULL;
    Status = LoadFile->LoadFile (
                         LoadFile,
                         FileContext->DevicePath,
                         TRUE,
                         &BufferSize,
                         Buffer
                         );
    if (Status == EFI_BUFFER_TOO_SMALL) {
      Buffer = AllocatePool (BufferSize);
      if (Buffer == NULL) {
        RestoreOutputString ();
        return EFI_OUT_OF_RESOURCES;
      }
      Status = LoadFile->LoadFile (
                           LoadFile,
                           FileContext->DevicePath,
                           TRUE,
                           &BufferSize,
                           Buffer
                           );
    }
    RestoreOutputString ();
    if (EFI_ERROR (Status)) {
      SafeFreePool ((VOID **) &Buffer);
      return EFI_UNSUPPORTED;
    }
  } else {
    Dir = FileContext->FHandle;
    Status = Dir->Open (
                    Dir,
                    &NewDir,
                    FileContext->FileName,
                    EFI_FILE_READ_ONLY,
                    0
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    DirInfo = UtilityLibFileInfo (NewDir);
    if (!DirInfo) {
      NewDir->Close (NewDir);
      return EFI_NOT_FOUND;
    }

    BufferSize = (UINTN) DirInfo->FileSize;
    Buffer     = AllocateZeroPool (BufferSize);
    ASSERT (Buffer != NULL);
    if (Buffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    SafeFreePool ((VOID **) &DirInfo);

    Status = NewDir->Read (NewDir, &BufferSize, Buffer);
    NewDir->Close (NewDir);
    if (EFI_ERROR (Status)) {
      FreePool (Buffer);
      return Status;
    }
  }

  *File     = Buffer;
  *FileSize = BufferSize;
  return EFI_SUCCESS;
}

/**
  Enroll a new X509 certificate into Variable.

  @param[in]  CallbackData           Instance to control HII page.
  @param[in] PrivateData             The module's private data.
  @param[in] VariableName            Variable name of CA database.

  @retval   EFI_SUCCESS              New X509 is enrolled successfully.
  @retval   EFI_OUT_OF_RESOURCES     Could not allocate needed resources.

**/
STATIC
EFI_STATUS
EnrollX509toVariable (
  IN TLS_CERT_MANAGER_CALLBACK_DATA    *CallbackData,
  IN VOID                              *X509Data,
  IN UINTN                             X509DataSize,
  IN CHAR16                            *VariableName,
  IN EFI_GUID                          *CertGuid
  )
{
  EFI_STATUS                           Status;
  EFI_SIGNATURE_LIST                   *CACert;
  EFI_SIGNATURE_DATA                   *CACertData;
  VOID                                 *Data;
  UINTN                                DataSize;
  UINTN                                SigDataSize;
  UINT32                               Attr;
  EFI_INPUT_KEY                        Key;

  SigDataSize   = 0;
  DataSize      = 0;
  CACert        = NULL;
  CACertData    = NULL;
  Data          = NULL;

  DEBUG ((DEBUG_INFO, "CertGuid : %g\n", CertGuid));

  if (IsGuidExist (CertGuid)) {
    DEBUG ((DEBUG_INFO, "The same CertGuid is existed.\n"));
    CallbackData->H2ODialog->ConfirmDialog (
                               DlgOk,
                               FALSE,
                               0,
                               NULL,
                               &Key,
                               L"The same CertGuid is existed.\n"
                               );
    return EFI_SUCCESS;
  }

  SigDataSize = sizeof(EFI_SIGNATURE_LIST) + sizeof(EFI_SIGNATURE_DATA) - 1 + X509DataSize;
  Data = AllocateZeroPool (SigDataSize);
  if (Data == NULL) {
    DEBUG ((DEBUG_INFO, "EFI_OUT_OF_RESOURCES.\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  //
  // Fill Certificate Database parameters.
  //
  CACert = (EFI_SIGNATURE_LIST*) Data;
  CACert->SignatureListSize   = (UINT32) SigDataSize;
  CACert->SignatureHeaderSize = 0;
  CACert->SignatureSize = (UINT32) (sizeof(EFI_SIGNATURE_DATA) - 1 + X509DataSize);
  CopyGuid (&CACert->SignatureType, &gEfiCertX509Guid);

  CACertData = (EFI_SIGNATURE_DATA*) ((UINT8* ) CACert + sizeof (EFI_SIGNATURE_LIST));
  CopyGuid (&CACertData->SignatureOwner, CertGuid);
  CopyMem ((UINT8* ) (CACertData->SignatureData), X509Data, X509DataSize);

  //
  // Check if signature database entry has been already existed.
  // If true, use EFI_VARIABLE_APPEND_WRITE attribute to append the
  // new signature data to original variable
  //
  Attr = TLS_CA_CERTIFICATE_VARIABLE_BASE_ATTR;

  Status = CommonGetVariable (
                  VariableName,
                  &gEfiTlsCaCertificateGuid,
                  &DataSize,
                  NULL
                  );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Attr |= EFI_VARIABLE_APPEND_WRITE;
  } else if (Status != EFI_NOT_FOUND) {
    DEBUG ((DEBUG_INFO, "ERROR Status : %r.\n", Status));
    goto ON_EXIT;
  }

  Status = CommonSetVariable(
                  VariableName,
                  &gEfiTlsCaCertificateGuid,
                  Attr,
                  SigDataSize,
                  Data
                  );
ON_EXIT:
  if (Data != NULL) {
    FreePool (Data);
  }
  DEBUG ((DEBUG_INFO, "EnrollX509toVariable Status : %r\n", Status));
  return Status;
}

/**
  Process PEM file to CERT 

  @param[in]  CallbackData           Instance to control HII page.
  @param[in]  BufferSize             Buffer size of buffer.
  @param[in]  Buffer                 Buffer pointer to PEM file content.

  @retval  EFI_SUCCESS             The operation succeeded.
  @retval  EFI_INVALID_PARAMETER   The parameter is invalid.
  @retval  EFI_OUT_OF_RESOURCES    Required resources could not be allocated.
  @retval  EFI_NOT_READY           CryptoServices protocol is not ready.
  @retval  EFI_ABORTED             User cancel from dialog.
  @retval  others                  Error status from external functions.

**/
STATIC
EFI_STATUS
ProcessPemFile (
  IN TLS_CERT_MANAGER_CALLBACK_DATA    *CallbackData,
  IN UINTN                             BufferSize,
  IN UINT8                             *Buffer
  )
{
  EFI_STATUS                           Status;
  EFI_INPUT_KEY                        Key;
  CHAR16                               *X509Info;

  EFI_GUID                             CertGuid;

  if (mCryptoServices == NULL) {
    return EFI_NOT_READY;
  }

  //
  // Generate information to user
  //
  X509Info = NULL;
  DEBUG ((DEBUG_INFO, "DecodeX509ToSubjectAndIssuerName %08x %d\n", Buffer, BufferSize));
  Status = DecodeX509ToSubjectAndIssuerName (
             Buffer,
             BufferSize,
             &X509Info,
             &CertGuid
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "ConfirmDialog OK %08x\n", CallbackData->H2ODialog));
    CallbackData->H2ODialog->ConfirmDialog (
                               DlgOk,
                               FALSE,
                               0,
                               NULL,
                               &Key,
                               L"Certificate file is incorrect."
                               );
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((DEBUG_INFO, "ConfirmDialog Yes or no\n"));
  Status = CallbackData->H2ODialog->ConfirmDialog (
                                      DlgYesNo,
                                      FALSE,
                                      0,
                                      NULL,
                                      &Key,
                                      X509Info
                                      );
  FreePool (X509Info);
  if (Key.UnicodeChar != CHAR_CARRIAGE_RETURN) {
    return EFI_ABORTED;
  }

  //
  // Insert certificate to variable
  //
  DEBUG ((DEBUG_INFO, "Import to variable %g Size: %d\n", &CertGuid, BufferSize));
  Status = EnrollX509toVariable (CallbackData, Buffer, BufferSize, TLS_CA_CERTIFICATE_VARIABLE, &CertGuid);
  return Status;
}

/**
  Append file name to existing file name.

  @param  Str1         existing file name
  @param  Str2         file name to be appended

  @return Allocate a new string to hold the appended result.
          Caller is responsible to free the returned string.
**/
STATIC
CHAR16 *
AppendFileName (
  IN  CHAR16  *Str1,
  IN  CHAR16  *Str2
  )
{
  UINTN   Size1;
  UINTN   Size2;
  CHAR16  *Str;
  CHAR16  *TmpStr;
  CHAR16  *Ptr;
  CHAR16  *LastSlash;

  Size1 = StrSize (Str1);
  Size2 = StrSize (Str2);
  //
  // Integer Overflow check
  //
  if (Size1 > Size1 + Size2 + sizeof (CHAR16) || Size2 > Size1 + Size2 + sizeof (CHAR16)) {
    ASSERT (FALSE);
    return NULL;
  }
  Str   = AllocateZeroPool (Size1 + Size2 + sizeof (CHAR16));
  ASSERT (Str != NULL);
  if (Str == NULL) {
    return NULL;
  }
  TmpStr = AllocateZeroPool (Size1 + Size2 + sizeof (CHAR16));
  ASSERT (TmpStr != NULL);
  if (TmpStr == NULL) {
    return NULL;
  }

  StrCatS (Str, (Size1 + Size2 + sizeof (CHAR16)) / sizeof(CHAR16), Str1);
  if (!((*Str == '\\') && (*(Str + 1) == 0))) {
    StrCatS (Str, (Size1 + Size2 + sizeof (CHAR16)) / sizeof(CHAR16), L"\\");
  }

  StrCatS (Str, (Size1 + Size2 + sizeof (CHAR16)) / sizeof(CHAR16), Str2);

  Ptr       = Str;
  LastSlash = Str;
  while (*Ptr != 0) {
    if (*Ptr == '\\' && *(Ptr + 1) == '.' && *(Ptr + 2) == '.' && *(Ptr + 3) == L'\\') {
      //
      // Convert "\Name\..\" to "\"
      // DO NOT convert the .. if it is at the end of the string. This will
      // break the .. behavior in changing directories.
      //

      //
      // Use TmpStr as a backup, as StrCpy in BaseLib does not handle copy of two strings
      // that overlap.
      //
      StrCpyS (TmpStr, (Size1 + Size2 + sizeof (CHAR16)) / sizeof(CHAR16), Ptr + 3);
      StrCpyS (LastSlash,  (Size1 + Size2 + sizeof (CHAR16) - ((UINT8 *) LastSlash - (UINT8 *) Str)) / sizeof(CHAR16), TmpStr);
      Ptr = LastSlash;
    } else if (*Ptr == '\\' && *(Ptr + 1) == '.' && *(Ptr + 2) == '\\') {
      //
      // Convert a "\.\" to a "\"
      //

      //
      // Use TmpStr as a backup, as StrCpy in BaseLib does not handle copy of two strings
      // that overlap.
      //
      StrCpyS (TmpStr,  (Size1 + Size2 + sizeof (CHAR16))/sizeof(CHAR16), Ptr + 2);
      StrCpyS (Ptr,  (Size1 + Size2 + sizeof (CHAR16)) / sizeof(CHAR16), TmpStr);
      Ptr = LastSlash;
    } else if (*Ptr == '\\') {
      LastSlash = Ptr;
    }

    Ptr++;
  }

  FreePool (TmpStr);

  return Str;
}

/**
  Check if input file name matches with the file extension string or not.

  @param[in] FileName          Pointer to the file name
  @param[in] FileExtensionStr  Pointer to the file extension string

  @retval TRUE   Input file name matches with the file extension string.
  @retval FALSE  Input file name does not match with the file extension string.
**/
STATIC
BOOLEAN
IsFileExtensionMatched (
  IN CHAR16             *FileName,
  IN CHAR16             *FileExtensionStr
  )
{
  STATIC CHAR16         *UpperCaseStr = NULL;
  STATIC CHAR16         *LowerCaseStr = NULL;
  STATIC UINTN          BufferLen     = 0;
  UINTN                 FileExtensionStrLen;
  UINTN                 Index;
  CHAR16                Char;

  if (FileName == NULL || FileExtensionStr == NULL || *FileName == CHAR_NULL || *FileExtensionStr == CHAR_NULL) {
    return FALSE;
  }

  //
  // Allocate memory to store file extension string in upper case and lower case.
  //
  FileExtensionStrLen = StrLen (FileExtensionStr);
  if (BufferLen < FileExtensionStrLen) {
    SafeFreePool ((VOID **) &UpperCaseStr);
    SafeFreePool ((VOID **) &LowerCaseStr);
    UpperCaseStr = AllocatePool (FileExtensionStrLen * sizeof (CHAR16));
    LowerCaseStr = AllocatePool (FileExtensionStrLen * sizeof (CHAR16));
    BufferLen    = FileExtensionStrLen;
  }
  if (UpperCaseStr == NULL || LowerCaseStr == NULL) {
    BufferLen = 0;
    return FALSE;
  }

  for (Index = 0; Index < FileExtensionStrLen; Index++) {
    Char = FileExtensionStr[Index];

    UpperCaseStr[Index] = (Char >= L'a' && Char <= L'z') ? ((Char - L'a') + L'A') : Char;
    LowerCaseStr[Index] = (Char >= L'A' && Char <= L'Z') ? ((Char - L'A') + L'a') : Char;
  }

  //
  // Check if file name contains the file extension name.
  //
  while (*FileName) {
    for (Index = 0; Index < FileExtensionStrLen; Index++) {
      if (FileName[Index] == CHAR_NULL) {
        return FALSE;
      }
      if (FileName[Index] != LowerCaseStr[Index] &&
          FileName[Index] != UpperCaseStr[Index]) {
        break;
      }
      DEBUG ((DEBUG_INFO, "%c\n", FileName[Index]));
    }
    if (Index == FileExtensionStrLen) {
      return TRUE;
    }

    FileName += 1;
  }

  return FALSE;
}

/**
  Based on target file format, check if the input file is qualified or not.

  @param[in] FileFormat        Target file format
  @param[in] FileInfo          Pointer to the file info

  @retval TRUE   The input file is qualified.
  @retval FALSE  The input file is not qualified or input parameter is invalid.
**/
STATIC
BOOLEAN
IsFileQualified (
  IN UINTN                             FileFormat,
  IN EFI_FILE_INFO                     *FileInfo
  )
{
  DEBUG ((DEBUG_INFO, "%d IsFileQualified FileName %s %08x\n", FileFormat, FileInfo->FileName, FileInfo));

  if (FileInfo == NULL) {
    return FALSE;
  }

  if (FileFormat == EM_FILE_FORMAT_PEM) {
    if (IsFileExtensionMatched (FileInfo->FileName, L".pem") 
      || IsFileExtensionMatched (FileInfo->FileName, L".cer")
      || IsFileExtensionMatched (FileInfo->FileName, L".crt")) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Create Menu Entry for future use, make all types together
  in order to reduce code size

  @param  MenuType      Use this parameter to identify current
                        Menu type

  @return A valid pointer pointing to the allocated memory pool for current menu entry
          or NULL if it cannot allocate memory for current menu entry

**/
STATIC
ENROLL_MENU_ENTRY *
CreateFileMenuEntry (
  VOID
  )
{
  ENROLL_MENU_ENTRY *MenuEntry;
  UINTN             ContextSize;

  DEBUG ((DEBUG_INFO, "CreateFileMenuEntry\n"));

  ContextSize = sizeof (BM_FILE_CONTEXT);
  MenuEntry = AllocateZeroPool (sizeof (ENROLL_MENU_ENTRY));
  if (NULL == MenuEntry) {
    return MenuEntry;
  }

  MenuEntry->VariableContext = AllocateZeroPool (ContextSize);
  if (NULL == MenuEntry->VariableContext) {
    SafeFreePool ((VOID**) &MenuEntry);
    MenuEntry = NULL;
    return MenuEntry;
  }

  MenuEntry->Signature        = EM_MENU_ENTRY_SIGNATURE;
  MenuEntry->ContextSelection = EM_FILE_CONTEXT_SELECT;
  return MenuEntry;
}

/**
  Create Menu Entry for future use, make all types together
  in order to reduce code size

  @param  MenuType      Use this parameter to identify current
                        Menu type

  @return A valid pointer pointing to the allocated memory pool for current menu entry
          or NULL if it cannot allocate memory for current menu entry

**/
STATIC
ENROLL_MENU_ENTRY *
CreateVariableMenuEntry (
  VOID
  )
{
  ENROLL_MENU_ENTRY *MenuEntry;
  UINTN             ContextSize;

  DEBUG ((DEBUG_INFO, "CreateVariableMenuEntry\n"));

  ContextSize = sizeof (EM_CERTIFICATE_CONTEXT);

  MenuEntry = AllocateZeroPool (sizeof (ENROLL_MENU_ENTRY));
  if (NULL == MenuEntry) {
    return MenuEntry;
  }

  MenuEntry->VariableContext = AllocateZeroPool (ContextSize);
  if (NULL == MenuEntry->VariableContext) {
    SafeFreePool ((VOID**) &MenuEntry);
    MenuEntry = NULL;
    return MenuEntry;
  }

  MenuEntry->Signature        = EM_MENU_ENTRY_SIGNATURE;
//  MenuEntry->ContextSelection = EM_FILE_CONTEXT_SELECT;
  return MenuEntry;
}

/**
  Find files under current directory
  All files and sub-directories in current directory
  will be stored in DirectoryMenu for future use.

  @param  FileOption    Pointer for Dir to explore

  @retval TRUE          Get files from current dir successfully
  @retval FALSE         Can't get files from current dir

**/
STATIC
EFI_STATUS
FindFiles (
  IN TLS_CERT_MANAGER_CALLBACK_DATA        *CallbackData,
  IN ENROLL_MENU_ENTRY                      *MenuEntry
  )
{
  EFI_FILE_HANDLE     NewDir;
  EFI_FILE_HANDLE     Dir;
  EFI_FILE_INFO       *DirInfo;
  UINTN               BufferSize;
  UINTN               DirBufferSize;
  ENROLL_MENU_ENTRY   *NewMenuEntry;
  EM_FILE_CONTEXT     *FileContext;
  EM_FILE_CONTEXT     *NewFileContext;
  UINTN               Pass;
  EFI_STATUS          Status;
  UINTN               OptionNumber;

  DEBUG ((DEBUG_INFO, "FindFiles File format %d\n", CallbackData->EmFileFormat));

  FileContext   = (EM_FILE_CONTEXT *) MenuEntry->VariableContext;
  Dir           = FileContext->FHandle;
  OptionNumber  = 0;
  //
  // Open current directory to get files from it
  //
  Status = Dir->Open (
                  Dir,
                  &NewDir,
                  FileContext->FileName,
                  EFI_FILE_READ_ONLY,
                  0
                  );
  if (!FileContext->IsRoot) {
    Dir->Close (Dir);
  }

  if (EFI_ERROR (Status)) {
    return Status;
  }

  DirInfo = UtilityLibFileInfo (NewDir);
  if (!DirInfo) {
    return EFI_NOT_FOUND;
  }

  if (!(DirInfo->Attribute & EFI_FILE_DIRECTORY)) {
    return EFI_INVALID_PARAMETER;
  }

  FileContext->DevicePath = FileDevicePath (
                              FileContext->Handle,
                              FileContext->FileName
                              );

  DirBufferSize = sizeof (EFI_FILE_INFO) + 1024;
  DirInfo       = AllocateZeroPool (DirBufferSize);
  if (!DirInfo) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Get all files in current directory
  // Pass 1 to get Directories
  // Pass 2 to get files that are EFI images
  //
  for (Pass = 1; Pass <= 2; Pass++) {
    NewDir->SetPosition (NewDir, 0);
    DEBUG ((DEBUG_INFO, "Pass %d\n", Pass));
    for (;;) {
      BufferSize  = DirBufferSize;
      Status      = NewDir->Read (NewDir, &BufferSize, DirInfo);
      if (EFI_ERROR (Status) || BufferSize == 0) {
        break;
      }

      if ((DirInfo->Attribute & EFI_FILE_DIRECTORY && Pass == 2) ||
          (!(DirInfo->Attribute & EFI_FILE_DIRECTORY) && Pass == 1)
          ) {
        //
        // Pass 1 is for Directories
        // Pass 2 is for file names
        //
        continue;
      }

      DEBUG ((DEBUG_INFO, "!!! %d FileName %s %d\n", Pass, DirInfo->FileName, DirInfo->Attribute));
      if (!(DirInfo->Attribute & EFI_FILE_DIRECTORY || IsFileQualified (CallbackData->EmFileFormat, DirInfo))) {
        //
        // Skip file unless it is a directory entry or file is qualified in current format.
        //
        DEBUG ((DEBUG_INFO, "    Skiped\n", Pass, DirInfo->FileName, DirInfo->Attribute));
        continue;
      } else if ((DirInfo->Attribute & EFI_FILE_DIRECTORY) && (StrLen (DirInfo->FileName) == 1) && (DirInfo->FileName[0] == '.')) {
        //
        // Skip . folder
        //
        DEBUG ((DEBUG_INFO, "    Skiped\n", Pass, DirInfo->FileName, DirInfo->Attribute));
        continue;
      }

      NewMenuEntry = CreateFileMenuEntry ();
      if (NULL == NewMenuEntry) {
        return EFI_OUT_OF_RESOURCES;
      }

      NewFileContext          = (EM_FILE_CONTEXT *) NewMenuEntry->VariableContext;
      NewFileContext->Handle  = FileContext->Handle;
      NewFileContext->FileName = AppendFileName (
                                  FileContext->FileName,
                                  DirInfo->FileName
                                  );
      if (NewFileContext->FileName == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      NewFileContext->FHandle = NewDir;
      NewFileContext->DevicePath = FileDevicePath (
                                    NewFileContext->Handle,
                                    NewFileContext->FileName
                                    );
      NewMenuEntry->HelpString = NULL;

      MenuEntry->DisplayStringToken = FileListGetStrTokenFromDepository (
                                        CallbackData,
                                        mSecureBootFileOptionStrDepository
                                        );

      NewFileContext->IsDir = (BOOLEAN) ((DirInfo->Attribute & EFI_FILE_DIRECTORY) == EFI_FILE_DIRECTORY);

      if (NewFileContext->IsDir) {
        BufferSize                  = StrLen (DirInfo->FileName) * 2 + 6;
        NewMenuEntry->DisplayString = AllocateZeroPool (BufferSize);

        UnicodeSPrint (
          NewMenuEntry->DisplayString,
          BufferSize,
          L"<%s>",
          DirInfo->FileName
          );

      } else {
        NewMenuEntry->DisplayString = AllocateCopyPool (StrSize (DirInfo->FileName), DirInfo->FileName);
      }

      NewFileContext->IsRoot            = FALSE;
      NewFileContext->IsLoadFile        = FALSE;
      NewFileContext->IsRemovableMedia  = FALSE;

      NewMenuEntry->OptionNumber        = OptionNumber;
      OptionNumber++;
      InsertTailList (&mFileListDirectoryMenu.Head, &NewMenuEntry->Link);
    }
  }

  mFileListDirectoryMenu.MenuNumber = OptionNumber;
  SafeFreePool ((VOID**) &DirInfo);
  return EFI_SUCCESS;
}


/**
  Use this routine to get one particular menu entry in specified menu

  @param  MenuOption         The menu that we will search
  @param  MenuNumber         The menunubmer that we want

  @return The desired menu entry

**/
STATIC
ENROLL_MENU_ENTRY *
GetMenuEntry (
  ENROLL_MENU_OPTION     *MenuOption,
  UINTN                  MenuNumber
  )
{
  ENROLL_MENU_ENTRY   *NewMenuEntry;
  UINTN               Index;
  LIST_ENTRY          *List;

  if (MenuNumber >= MenuOption->MenuNumber) {
    return NULL;
  }

  List = MenuOption->Head.ForwardLink;
  for (Index = 0; Index < MenuNumber; Index++) {
    List = List->ForwardLink;
  }

  NewMenuEntry = CR (List, ENROLL_MENU_ENTRY, Link, EM_MENU_ENTRY_SIGNATURE);

  return NewMenuEntry;
}

/**
  Get the form ID of enroll menu based on secure boot manager state.

  @param[in] SecureBootMgrState        Secure boot manager state

  @return The form ID of enroll menu or zero if not found.
**/
STATIC
EFI_FORM_ID
FileListGetMenuFormId (
  IN FILE_LIST_STATE                   FileListState
  )
{
  switch (FileListState) {

  case TlsForm:
    return FORM_ID_IMPORT_CERTIFICATE_FILE;

  case VariableForm:
    return FORM_ID_MANAGE_CERTIFICATE_FILE;

  default:
    return FileListStateMax;
  }
}

/**
  Update the Enroll Menu page.

  @param  CallbackData         Pointer to TLS_CERT_MANAGER_CALLBACK_DATA instance
  @param  MenuOption           Pointer to menu options to display.

**/
STATIC
VOID
UpdateEnrollMenuPage (
  IN TLS_CERT_MANAGER_CALLBACK_DATA       *CallbackData,
  IN ENROLL_MENU_OPTION                    *MenuOption
  )
{
  UINTN                       Index;
  ENROLL_MENU_ENTRY           *NewMenuEntry;
  EM_FILE_CONTEXT             *NewFileContext;
  EFI_FORM_ID                 FormId;
  VOID                        *StartOpCodeHandle;
  VOID                        *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL          *StartLabel;
  EFI_IFR_GUID_LABEL          *EndLabel;

  UINT16                      NewQuestionId;

  NewMenuEntry    = NULL;
  NewFileContext  = NULL;
  FormId          = FileListGetMenuFormId (CallbackData->FileListState);
  ASSERT (FormId != 0);
  if (FormId == 0) {
    return;
  }

  DEBUG ((DEBUG_INFO, "UpdateEnrollMenuPage %04x %d %d\n", FormId, CallbackData->FileListState, CallbackData->EmFileFormat));

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
  StartLabel->Number       = LABEL_ID_FILE_LIST_START;

  //
  // Create Hii Extend Label OpCode as the end opcode
  //
  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = LABEL_ID_FILE_LIST_END;

  DEBUG ((DEBUG_INFO, "Menu number %d\n", MenuOption->MenuNumber));
  for (Index = 0; Index < MenuOption->MenuNumber; Index++) {
    NewMenuEntry    = GetMenuEntry (MenuOption, Index);
    NewFileContext  = (EM_FILE_CONTEXT *) NewMenuEntry->VariableContext;

    DEBUG ((DEBUG_INFO, "Index : %d\n", Index));
    if (CallbackData->FileListState == VariableForm) {
      if (CallbackData->EmFileFormat == EM_FILE_FORMAT_DEL_VAR) {
        NewQuestionId = (UINT16)(ENROLL_VAR_DELETE_OPTION_OFFSET + Index);
        DEBUG ((DEBUG_INFO, "Create string token to delete certificate : %04x\n", NewQuestionId));

      } else if (CallbackData->EmFileFormat == EM_FILE_FORMAT_EXP_VAR) {
        NewQuestionId = (UINT16)(ENROLL_VAR_EXPORT_OPTION_OFFSET + Index);
        DEBUG ((DEBUG_INFO, "Create string token to export certificate : %04x\n", NewQuestionId));

      } else {
        DEBUG ((DEBUG_INFO, "Unknow status\n"));
        NewQuestionId = 0xFFFF;
      }
      DEBUG ((DEBUG_INFO, "HiiCreateActionOpCode VariableForm %04x\n", NewQuestionId));
      HiiCreateActionOpCode (
        StartOpCodeHandle,
        (UINT16) NewQuestionId,
        NewMenuEntry->DisplayStringToken,
        NewMenuEntry->HelpStringToken,
        EFI_IFR_FLAG_CALLBACK,
        0
        );
    }else if (CallbackData->FileListState == TlsForm || NewFileContext->IsDir || NewFileContext->IsLoadFile) {
      DEBUG ((DEBUG_INFO, "ENROLL_FILE_OPTION_OFFSET\n"));
      HiiCreateActionOpCode (
        StartOpCodeHandle,
        (UINT16) (ENROLL_FILE_OPTION_OFFSET + Index),
        NewMenuEntry->DisplayStringToken,
        STRING_TOKEN (STR_BLANK_STRING),
        EFI_IFR_FLAG_CALLBACK,
        0
        );
    } 
  }
  DEBUG ((DEBUG_INFO, "Update form ID : %04x\n", FormId));
  HiiUpdateForm (
    CallbackData->RegisteredHandle,
    NULL,
    FormId,
    StartOpCodeHandle,
    EndOpCodeHandle
    );
  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (EndOpCodeHandle);

}


/**
  Create string tokens for a menu from its help strings and display strings

  @param  HiiHandle         Hii Handle of the package to be updated.
  @param  MenuOption        The Menu whose string tokens need to be created

  @retval EFI_SUCCESS   string tokens created successfully
  @retval Other         contain some errors

**/
STATIC
EFI_STATUS
CreateMenuStringToken (
  IN EFI_HII_HANDLE                   HiiHandle,
  IN ENROLL_MENU_OPTION               *MenuOption
  )
{
  ENROLL_MENU_ENTRY *NewMenuEntry;
  UINTN             Index;

  for (Index = 0; Index < MenuOption->MenuNumber; Index++) {
    NewMenuEntry = GetMenuEntry (MenuOption, Index);
    DEBUG ((DEBUG_INFO, "SetString : %s\n", NewMenuEntry->DisplayString));
    NewMenuEntry->DisplayStringToken = HiiSetString (
                                         HiiHandle,
                                         NewMenuEntry->DisplayStringToken,
                                         NewMenuEntry->DisplayString,
                                         NULL
                                         );
    if (NULL == NewMenuEntry->HelpString) {
      NewMenuEntry->HelpStringToken = NewMenuEntry->DisplayStringToken;
    } else {
      NewMenuEntry->HelpStringToken = HiiSetString (
                                        HiiHandle,
                                        NewMenuEntry->HelpStringToken,
                                        NewMenuEntry->HelpString,
                                        NULL
                                        );
    }
  }

  return EFI_SUCCESS;
}

/**
  Free resources allocated in Allocate Rountine

  @param  FreeMenu      Menu to be freed

**/
STATIC
VOID
FreeMenu (
  ENROLL_MENU_OPTION       *FreeMenu
  )
{
  ENROLL_MENU_ENTRY    *MenuEntry;

  while (!IsListEmpty (&FreeMenu->Head)) {
    MenuEntry = CR (
                  FreeMenu->Head.ForwardLink,
                  ENROLL_MENU_ENTRY,
                  Link,
                  EM_MENU_ENTRY_SIGNATURE
                  );
    RemoveEntryList (&MenuEntry->Link);
    DestroyMenuEntry (MenuEntry);
  }
}

/**
  Find file systems for current Extensible Firmware
  Including Handles that support Simple File System
  protocol, Load File protocol.

  Building up the FileSystem Menu for user selection
  All file system will be stored in FsOptionMenu
  for future use.

  @param  CallbackData  BMM context data

  @retval EFI_SUCCESS               Success find the file system
  @retval EFI_OUT_OF_RESOURCES      Can not create menu entry

**/
STATIC
EFI_STATUS
FindFileSystem (
  VOID
  )
{
  UINTN                     NoBlkIoHandles;
  UINTN                     NoSimpleFsHandles;
  EFI_HANDLE                *BlkIoHandle;
  EFI_HANDLE                *SimpleFsHandle;
  UINT16                    *VolumeLabel;
  EFI_BLOCK_IO_PROTOCOL     *BlkIo;
  UINTN                     Index;
  EFI_STATUS                Status;
  ENROLL_MENU_ENTRY          *MenuEntry;
  EM_FILE_CONTEXT           *FileContext;
  UINT16                    *TempStr;
  UINTN                     OptionNumber;
  VOID                      *Buffer;
  BOOLEAN                   RemovableMedia;

  NoSimpleFsHandles = 0;
  OptionNumber      = 0;
  InitializeListHead (&mFileListFsOptionMenu.Head);

  //
  // Locate Handles that support BlockIo protocol
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiBlockIoProtocolGuid,
                  NULL,
                  &NoBlkIoHandles,
                  &BlkIoHandle
                  );
  if (!EFI_ERROR (Status)) {

    for (Index = 0; Index < NoBlkIoHandles; Index++) {
      Status = gBS->HandleProtocol (
                      BlkIoHandle[Index],
                      &gEfiBlockIoProtocolGuid,
                      (VOID **) &BlkIo
                      );

      if (EFI_ERROR (Status)) {
        continue;
      }

      //
      // Issue a dummy read to trigger reinstall of BlockIo protocol for removable media
      //
      if (BlkIo->Media->RemovableMedia) {
        Buffer = AllocateZeroPool (BlkIo->Media->BlockSize);
        if (NULL == Buffer) {
          SafeFreePool ((VOID**) &BlkIoHandle);
          return EFI_OUT_OF_RESOURCES;
        }

        BlkIo->ReadBlocks (
                 BlkIo,
                 BlkIo->Media->MediaId,
                 0,
                 BlkIo->Media->BlockSize,
                 Buffer
                 );
        SafeFreePool ((VOID**) &Buffer);
      }
    }
    SafeFreePool ((VOID**) &BlkIoHandle);
  }

  //
  // Locate Handles that support Simple File System protocol
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &NoSimpleFsHandles,
                  &SimpleFsHandle
                  );
  if (!EFI_ERROR (Status)) {
    //
    // Find all the instances of the File System prototocol
    //
    for (Index = 0; Index < NoSimpleFsHandles; Index++) {
      Status = gBS->HandleProtocol (
                      SimpleFsHandle[Index],
                      &gEfiBlockIoProtocolGuid,
                      (VOID **) &BlkIo
                      );
      if (EFI_ERROR (Status)) {
        //
        // If no block IO exists assume it's NOT a removable media
        //
        RemovableMedia = FALSE;
      } else {
        //
        // If block IO exists check to see if it's remobable media
        //
        RemovableMedia = BlkIo->Media->RemovableMedia;
      }

      //
      // Allocate pool for this load option
      //
      DEBUG ((DEBUG_INFO, "FindFileSystem CreateFileMenuEntry\n"));
      MenuEntry = CreateFileMenuEntry ();
      if (NULL == MenuEntry) {
        SafeFreePool ((VOID**) &SimpleFsHandle);
        return EFI_OUT_OF_RESOURCES;
      }

      FileContext = (EM_FILE_CONTEXT *) MenuEntry->VariableContext;
      FileContext->Handle     = SimpleFsHandle[Index];
      MenuEntry->OptionNumber = Index;
      FileContext->FHandle    = UtilityLibOpenRoot (FileContext->Handle);
      if (!FileContext->FHandle) {
        DestroyMenuEntry (MenuEntry);
        continue;
      }

      MenuEntry->HelpString = ConvertDevicePathToText (DevicePathFromHandle (FileContext->Handle), FALSE, TRUE);
      FileContext->Info = UtilityLibFileSystemVolumeLabelInfo (FileContext->FHandle);
      FileContext->FileName = UtilityLibStrDuplicate (L"\\");
      if (FileContext->FileName == NULL) {
        DestroyMenuEntry (MenuEntry);
        SafeFreePool ((VOID**) &SimpleFsHandle);
        return EFI_OUT_OF_RESOURCES;
      }
      FileContext->DevicePath = FileDevicePath (
                                  FileContext->Handle,
                                  FileContext->FileName
                                  );
      FileContext->IsDir            = TRUE;
      FileContext->IsRoot           = TRUE;
      FileContext->IsRemovableMedia = FALSE;
      FileContext->IsLoadFile       = FALSE;

      //
      // Get current file system's Volume Label
      //
      if (FileContext->Info == NULL) {
        VolumeLabel = L"NO FILE SYSTEM INFO";
      } else {
        VolumeLabel = FileContext->Info->VolumeLabel;
        if (*VolumeLabel == 0x0000) {
          VolumeLabel = L"NO VOLUME LABEL";
        }
      }

      TempStr                   = MenuEntry->HelpString;
      MenuEntry->DisplayString  = AllocateZeroPool (MAX_CHAR);
      ASSERT (MenuEntry->DisplayString != NULL);
      UnicodeSPrint (
        MenuEntry->DisplayString,
        MAX_CHAR,
        L"%s, [%s]",
        VolumeLabel,
        TempStr
        );
      OptionNumber++;
      DEBUG ((DEBUG_INFO, "FindFileSystem InsertTailList %s\n", MenuEntry->DisplayString));
      InsertTailList (&mFileListFsOptionMenu.Head, &MenuEntry->Link);
    }
  }

  if (NoSimpleFsHandles != 0) {
    SafeFreePool ((VOID**) &SimpleFsHandle);
  }

  //
  // Remember how many file system options are here
  //
  mFileListFsOptionMenu.MenuNumber = OptionNumber;
  return EFI_SUCCESS;
}

/**
  Update the file explower page with the certificate list from Certificate store variable.

  @param[in]  CallbackData      BSECURE_BOOT_MANAGER_CALLBACK_DATA instance
  @param[in]  KeyValue          Key value to identify the type of data to expect.

  @retval TRUE              Inform the caller to create a callback packet to exit file explorer.
  @retval FALSE             Indicate that there is no need to exit file explorer.

**/
STATIC
BOOLEAN
UpdateEmCertFileExplorer (
  IN TLS_CERT_MANAGER_CALLBACK_DATA        *CallbackData,
  IN UINT16                                 KeyValue
  )
{
  EFI_STATUS                 Status;
  UINT16                     FileOptionMask;
  ENROLL_MENU_ENTRY          *NewMenuEntry;
  UINT16                     CertIndex;

  FileOptionMask    = (UINT16) (ENROLL_VAR_OPTION_CMD_MASK & KeyValue);
  NewMenuEntry      = NULL;
  CertIndex         = 0xFFFF;

  if (FileOptionMask == ENROLL_VAR_DELETE_OPTION_OFFSET) {
    DEBUG ((DEBUG_INFO, "DELETE certificate from variable %04x\n", KeyValue));
    CertIndex = KeyValue - ENROLL_VAR_DELETE_OPTION_OFFSET;
    
  } else if (FileOptionMask == ENROLL_VAR_EXPORT_OPTION_OFFSET) {
    DEBUG ((DEBUG_INFO, "EXPORT certificate from variable %04x\n", KeyValue));
    CertIndex = KeyValue - ENROLL_VAR_EXPORT_OPTION_OFFSET;

  } else {
    DEBUG ((DEBUG_INFO, "UpdateEmCertFileExplorer %d %04x\n", CallbackData->EmDisplayContext, KeyValue));
  }

  DEBUG ((DEBUG_INFO, "ProcessCertificatesFromCertStore \n" ));
  Status = ProcessCertificatesFromCertStore (CallbackData, CertIndex, NULL);

  DEBUG ((DEBUG_INFO, "CreateMenuStringToken \n" ));
  CreateMenuStringToken (CallbackData->RegisteredHandle, &mFileListCertFileMenu);
  UpdateEnrollMenuPage (CallbackData, &mFileListCertFileMenu);

  return FALSE;
}

/**
  Update the file explower page with the refershed file system.

  @param  CallbackData      TLS_CERT_MANAGER_CALLBACK_DATA instance
  @param  KeyValue          Key value to identify the type of data to expect.

  @retval TRUE              Inform the caller to create a callback packet to exit file explorer.
  @retval FALSE             Indicate that there is no need to exit file explorer.

**/
STATIC
BOOLEAN
UpdateEmFileExplorer (
  IN TLS_CERT_MANAGER_CALLBACK_DATA         *CallbackData,
  IN UINT16                                 KeyValue
  )
{
  EFI_STATUS                                Status;
  UINT16                                    FileOptionMask;
  ENROLL_MENU_ENTRY                         *NewMenuEntry;
  EM_FILE_CONTEXT                           *NewFileContext;
  
  FileOptionMask    = (UINT16) (ENROLL_FILE_OPTION_MASK & KeyValue);
  NewMenuEntry      = NULL;

  DEBUG ((DEBUG_INFO, "UpdateEmFileExplorer %d %04x\n", CallbackData->EmDisplayContext, KeyValue));
  if (EM_UNKNOWN_CONTEXT == CallbackData->EmDisplayContext) {
    FindFileSystem ();
    CreateMenuStringToken (CallbackData->RegisteredHandle, &mFileListFsOptionMenu);
    UpdateEnrollMenuPage (CallbackData, &mFileListFsOptionMenu);
    CallbackData->EmDisplayContext = EM_FILE_SYSTEM;
  } else {
    if (EM_FILE_SYSTEM == CallbackData->EmDisplayContext) {
      NewMenuEntry = GetMenuEntry (&mFileListFsOptionMenu, FileOptionMask);
    } else if (EM_DIRECTORY == CallbackData->EmDisplayContext) {
      NewMenuEntry = GetMenuEntry (&mFileListDirectoryMenu, FileOptionMask);
    }
    if (NewMenuEntry == NULL) {
      return FALSE;
    }
    NewFileContext = (EM_FILE_CONTEXT *) NewMenuEntry->VariableContext;

    if (NewFileContext->IsDir ) {
      CallbackData->EmDisplayContext = EM_DIRECTORY;
      RemoveEntryList (&NewMenuEntry->Link);
      FreeMenu (&mFileListDirectoryMenu);
      FileListReclaimStrDepository (mSecureBootFileOptionStrDepository);
      Status = FindFiles (CallbackData, NewMenuEntry);
      DestroyMenuEntry (NewMenuEntry);
      if (EFI_ERROR (Status)) {
        return TRUE;
      }

      CreateMenuStringToken (CallbackData->RegisteredHandle, &mFileListDirectoryMenu);
      UpdateEnrollMenuPage (CallbackData, &mFileListDirectoryMenu);
    } else {
      //
      // Process file to insert to Variable.
      //
      ProcessFile (CallbackData, NewMenuEntry, NewFileContext);
    }
  }

  return FALSE;
}

/**
  Save data to variable(gEfiTlsCaCertificateGuid, TLS_CA_CERTIFICATE_VARIABLE)

  @param[in]  Data          Data
  @param[in]  DataSize      size of data, in byte.

  @retval Status            Status from SetVariable.

**/
STATIC
EFI_STATUS
UpdateCaCertificateVariable (
  IN  UINT8                            *Data,
  IN  UINTN                            DataSize
  )
{
  DEBUG ((DEBUG_INFO, "UpdateCaCertificateVariable %08x %d\n", Data, DataSize));
  if (DataSize == 0) {
    Data = NULL;
  }
  return CommonSetVariable(
                TLS_CA_CERTIFICATE_VARIABLE,
                &gEfiTlsCaCertificateGuid,
                TLS_CA_CERTIFICATE_VARIABLE_BASE_ATTR,
                DataSize,
                Data
                );
}

/**
  Get data to variable(gEfiTlsCaCertificateGuid, TLS_CA_CERTIFICATE_VARIABLE)

  @param[out]  Data                    Data pointer to a buffer pointer.
  @param[out]  DataSize                Size pointer parameter, function will return how many bytes get from variable.

  @retval EFI_SUCCESS                  Process successful.
  @retval EFI_OUT_OF_RESOURCES         Allocate memory failed.
  @retval Status                       Status from GetVariable.

**/
STATIC
EFI_STATUS
GetCaCertificateVariable (
  OUT UINT8                            **Data,
  OUT UINTN                            *DataSize
  )
{
  EFI_STATUS                           Status;
  CHAR16                               *VariableName;
  UINTN                                TmpDataSize;
  UINT8                                *TmpDataPtr;

  VariableName = TLS_CA_CERTIFICATE_VARIABLE;
  *Data        = NULL;
  TmpDataSize  = 0;

  Status = CommonGetVariable(
                  VariableName,
                  &gEfiTlsCaCertificateGuid,
                  &TmpDataSize,
                  NULL
                  );
  if (EFI_ERROR (Status) && Status != EFI_BUFFER_TOO_SMALL) {
    DEBUG ((DEBUG_INFO, "GetCaCertificateVariable %r\n", Status ));
    return Status;
  }
  DEBUG ((DEBUG_INFO, "GetCaCertificateVariable size: %d\n", TmpDataSize ));
  TmpDataPtr = AllocateZeroPool (TmpDataSize);
  if (TmpDataPtr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = CommonGetVariable (
                  VariableName,
                  &gEfiTlsCaCertificateGuid,
                  &TmpDataSize,
                  (VOID *) TmpDataPtr
                  );
  if (EFI_ERROR (Status)) {
    FreePool (TmpDataPtr);
    return Status;
  }

  *Data           = TmpDataPtr;
  *DataSize       = TmpDataSize;
  return EFI_SUCCESS;
}

/**
  Dump information from certificate to HII element.

  @param[in]   CertList     Certificate list
  @param[out]  NewEmEntry   HII element.

  @retval EFI_SUCCESS       The operation succeeded.
  @retval Status            Status from other functions.

**/
STATIC
EFI_STATUS
DumpCertificate (
  IN EFI_SIGNATURE_LIST                *CertList,
  OUT ENROLL_MENU_ENTRY                *NewEmEntry
  )
{
  EFI_STATUS                           Status;
  EFI_SIGNATURE_DATA                   *CertRecord;
  UINTN                                CertCount;
  UINT8                                *TmpCert;
  UINTN                                TmpCertSize;

  CHAR16                               *SubjectNameStr;
  CHAR16                               *IssuerNameStr;
  EFI_GUID                             CertGuid;

  if (mCryptoServices == NULL) {
    return EFI_NOT_READY;
  }

  CertCount   = (CertList->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - CertList->SignatureHeaderSize) / CertList->SignatureSize;
  CertRecord  = (EFI_SIGNATURE_DATA *) ((UINT8 *) CertList + sizeof (EFI_SIGNATURE_LIST) + CertList->SignatureHeaderSize);
  TmpCert     = CertRecord->SignatureData;
  TmpCertSize = CertList->SignatureSize - sizeof(EFI_SIGNATURE_DATA) + 1;
    
  //
  // Display GUID and help
  //
  ZeroMem (&CertGuid, sizeof (EFI_GUID));
  Status = DumpCaCertificate (TmpCert, TmpCertSize, &SubjectNameStr, &IssuerNameStr, &CertGuid);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "  X509 parse failed.\n", &CertGuid, SubjectNameStr, IssuerNameStr));
    return Status;
  }
  DEBUG ((DEBUG_INFO, "  %g\n  SubjectName:%s\n  IssuerName:%s\n", &CertGuid, SubjectNameStr, IssuerNameStr));
  NewEmEntry->DisplayString = AllocateZeroPool (MAX_CHAR);
  if (NewEmEntry->DisplayString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  NewEmEntry->HelpString    = AllocateZeroPool (MAX_CHAR);
  if (NewEmEntry->HelpString == NULL) {
    FreePool (NewEmEntry->DisplayString);
    NewEmEntry->DisplayString = NULL;
    return EFI_OUT_OF_RESOURCES;
  }
  UnicodeSPrint (NewEmEntry->DisplayString, MAX_CHAR, L"%s", SubjectNameStr);
  UnicodeSPrint (NewEmEntry->HelpString, MAX_CHAR, L"SubjectName:%s\nIssuerName:%s", SubjectNameStr, IssuerNameStr);
  return EFI_SUCCESS;
}

/**
  Find the certificate from CERT store variable.

  @param[in]   Data                    Certificate store variable content.
  @param[in]   DataSize                Certificate store variable content size.
  @param[out]  SignatureType           Certificate type, refer GUIDs in MdePkg/Include/Guid/ImageAuthentication.h
  @param[out]  CertGuid                The first certificate GUID of this X509 certificate list, if this list is not type of X509, it will be NULL.
  @param[out]  CertLength              Length of X509 certificate list.

  @retval EFI_SUCCESS                  The operation succeeded, please check SignatureType to avoid parse type is not gEfiCertX509Guid of data.
  @retval EFI_NOT_FOUND                Not found certificate.
  @retval EFI_INVALID_PARAMETER        Parameters is not valid.

**/
STATIC
EFI_STATUS
NextCertListInCertStore (
  IN UINT8                             *Data,
  IN UINTN                             DataSize,
  OUT EFI_GUID                         **SignatureType,
  OUT EFI_GUID                         **CertGuid,
  OUT UINTN                            *CertLength
  )
{
  EFI_SIGNATURE_LIST                   *CertList;
  UINTN                                ItemDataSize;
  UINTN                                CertSize;
  EFI_SIGNATURE_DATA                   *CertRecord;

  //
  // Check parameters
  //
  if ((Data == NULL) || (DataSize == 0) || (SignatureType == NULL) || (CertGuid == NULL) || (CertLength == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check validation of the certificate list.
  //
  ItemDataSize = DataSize;
  CertList     = (EFI_SIGNATURE_LIST *) Data;
  if (ItemDataSize < CertList->SignatureListSize) {
    return EFI_NOT_FOUND;
  }

  CertSize       = 0;
  *SignatureType = &CertList->SignatureType;
  *CertGuid      = NULL;
  *CertLength    = CertList->SignatureListSize;
  if (CompareGuid (&CertList->SignatureType, &gEfiCertX509Guid)) {
    CertRecord = (EFI_SIGNATURE_DATA *)(CertList + 1);
    *CertGuid = &CertRecord->SignatureOwner;
  }
  return EFI_SUCCESS;
}

/**
  Check cert GUID is exist in CERT variable to avoid the same certificate import many times.

  @param[in]   CertGuid         Target of certificate GUID.

  @retval TRUE                  Exist.           
  @retval FALSE                 Not exist.
**/
STATIC
BOOLEAN
IsGuidExist (
  IN   EFI_GUID                        *TargetCertGuid
  )
{
  EFI_STATUS                           Status;
  UINT8                                *Data;
  UINTN                                DataSize;
  UINTN                                ItemDataSize;
  UINT8                                *TmpDataPtr;
  EFI_GUID                             *SignatureType;
  EFI_GUID                             *CertGuid;
  UINTN                                CertListSize;
  BOOLEAN                              IsExist;

  DataSize = 0;
  Data     = NULL;
  Status   = GetCaCertificateVariable (&Data, &DataSize);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  ItemDataSize = DataSize;
  TmpDataPtr   = Data;
  IsExist      = FALSE;
  while (ItemDataSize > 0) {
    Status = NextCertListInCertStore (TmpDataPtr, ItemDataSize, &SignatureType, &CertGuid, &CertListSize);
    DEBUG ((DEBUG_INFO, "IsGuidExist %r %g\n", Status, CertGuid ));
    if (EFI_ERROR (Status)) {
      break;
    }

    //
    // Push ptr to next CertList.
    //
    TmpDataPtr     = TmpDataPtr + CertListSize;
    ItemDataSize   = ItemDataSize - CertListSize;
    if (!CompareGuid (SignatureType, &gEfiCertX509Guid)) {
      DEBUG ((DEBUG_INFO, "Not X509 certificate list\n"));
      continue;
    }

    if (!CompareGuid (TargetCertGuid, CertGuid)) {
      continue;
    }
    IsExist = TRUE;
  }
  if (Data != NULL) {
    FreePool (Data);
  }
  return IsExist;
}

/**
  Delete select index of certificate from CaCertificate variable.

  @param[in]   CallbackData     Instance of HII.
  @param[in]   SelectIndex      Index of cert in the CERT store.

  @retval EFI_SUCCESS           The operation succeeded.
  @retval EFI_NOT_FOUND         Selected index is not found in current MenuList.
  @retval EFI_ABORTED           User aborted the process from dialog.
  @retval other                 Status from other functions.

**/
STATIC
EFI_STATUS
DeleteCertificateFromCertStore (
  IN TLS_CERT_MANAGER_CALLBACK_DATA    *CallbackData,
  IN UINT16                            SelectIndex
  )
{
  EFI_STATUS                           Status;
  UINT8                                *Data;
  UINTN                                DataSize;
  EM_CERTIFICATE_CONTEXT               *CertContext;
  ENROLL_MENU_ENTRY                    *SelectMenuEntry;
  EFI_INPUT_KEY                        Key;

  DataSize = 0;
  Data     = NULL;
  DEBUG ((DEBUG_INFO, "DeleteCertificateFromCertStore \n" ));
  Status   = GetCaCertificateVariable (&Data, &DataSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  /// Find certificate offset and length in variable.
  SelectMenuEntry = GetMenuEntry (&mFileListCertFileMenu, SelectIndex);
  if (SelectMenuEntry == NULL) {
    DEBUG ((DEBUG_INFO, "Selected index not found \n" ));
    return EFI_NOT_FOUND;
  }

  DEBUG ((DEBUG_INFO, "Delete certificate ConfirmDialog Yes or no\n"));
  Status = CallbackData->H2ODialog->ConfirmDialog (
                                      DlgYesNo,
                                      FALSE,
                                      0,
                                      NULL,
                                      &Key,
                                      L"Confirm to delete certificate?"
                                      );
  if (Key.UnicodeChar != CHAR_CARRIAGE_RETURN) {
    DEBUG ((DEBUG_INFO, "User canceled.\n" ));
    return EFI_ABORTED;
  }

  CertContext = SelectMenuEntry->VariableContext;
  DEBUG ((DEBUG_INFO, "Variable Offset : %08x Length : %d %d\n", CertContext->VariableOffset, CertContext->CertLength, CertContext->VariableLengthToEnd));
  if ((CertContext->VariableOffset == 0) && (CertContext->VariableLengthToEnd == 0)) {
    /// Do nothing.
  } else {
    CopyMem (
      (VOID *) (Data + CertContext->VariableOffset),
      (VOID *) (Data + CertContext->VariableOffset + CertContext->CertLength),
      CertContext->VariableLengthToEnd
      );
  }
  //
  // Update certificate variable.
  //
  UpdateCaCertificateVariable (Data, DataSize - CertContext->CertLength);
  FreePool (Data);
  DataSize = 0;
  Data     = NULL;
  return EFI_SUCCESS;
}

/**
  Process certificate files from CERT store variable.
  If the OutputFileName is NULL.
  If caller select the index of SelectIndex, this process will delete certificate from variable.

  @param[in]   CallbackData     Instance of HII.
  @param[in]   SelectIndex      Index of cert in the CERT store.
  @param[in]   OutputFileName   Output file name, not support output yet.
                                If this is NULL, the certificate that selected by will be delete from variable store.

  @retval EFI_SUCCESS           The operation succeeded.
  @retval EFI_UNSUPPORTED       Operation not supported.
  @retval Status                Status from other functions.

**/
STATIC
EFI_STATUS
ProcessCertificatesFromCertStore (
  IN TLS_CERT_MANAGER_CALLBACK_DATA    *CallbackData,
  IN UINT16                            SelectIndex,
  IN CHAR16                            *OutputFileName
  )
{
  EFI_STATUS                           Status;
  UINT8                                *Data;
  UINTN                                DataSize;
  UINTN                                ItemDataSize;
  UINTN                                Index;
  UINT8                                *TmpDataPtr;
  UINT8                                *CertPtr;
  EFI_GUID                             *SignatureType;
  EFI_GUID                             *CertGuid;
  UINTN                                CertListSize;
  UINTN                                VariableOffset;
  ENROLL_MENU_ENTRY                    *NewMenuEntry;
  EM_CERTIFICATE_CONTEXT               *CertContext;

  if (SelectIndex != 0xFFFF) {
    if (OutputFileName == NULL) {
      /// Delete certificate and update variable.
      DeleteCertificateFromCertStore (CallbackData, SelectIndex);
    } else {
      /// Export certificate
      /// Not supported yet.
      return EFI_UNSUPPORTED;
    }
  }

  DataSize = 0;
  Data     = NULL;
  Status   = GetCaCertificateVariable (&Data, &DataSize);
  if (EFI_ERROR (Status)) {
    /// If Certificate variable get failed, process need to update to empty.
    /// So this section will do nothing.
  }
  InitializeListHead (&mFileListCertFileMenu.Head);
  DEBUG ((DEBUG_INFO, "CA db total size : %d bytes\n", DataSize));
  Index        = 0;
  ItemDataSize = DataSize;
  TmpDataPtr   = Data;
  while (ItemDataSize > 0) {
    Status = NextCertListInCertStore (TmpDataPtr, ItemDataSize, &SignatureType, &CertGuid, &CertListSize);
    if (EFI_ERROR (Status)) {
      break;
    }

    //
    // Push ptr to next CertList.
    //
    CertPtr        = TmpDataPtr;
    VariableOffset = DataSize - ItemDataSize;
    TmpDataPtr     = TmpDataPtr + CertListSize;
    ItemDataSize   = ItemDataSize - CertListSize;
    if (!CompareGuid (SignatureType, &gEfiCertX509Guid)) {
      //
      // The signature type is not supported in current implementation.
      //
      DEBUG ((DEBUG_INFO,  "CERT is not supported\n"));
      continue;
    }
    DEBUG ((DEBUG_INFO, "\nIndex %d: %g\n", Index, CertGuid));
    DEBUG ((DEBUG_INFO, "GUID for CERT Offset:%08x Size:%d bytes\n", VariableOffset, CertListSize));

    //
    // Generate HII item
    //
    NewMenuEntry = CreateVariableMenuEntry ();
    if (NewMenuEntry == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ON_EXIT;
    }
    //
    // Check the validation of this certificate.
    //
    Status = DumpCertificate ((EFI_SIGNATURE_LIST *)CertPtr, NewMenuEntry);
    if (EFI_ERROR (Status)) {
      continue;
    }
    CertContext = (EM_CERTIFICATE_CONTEXT *) NewMenuEntry->VariableContext;
    CertContext->VariableOffset      = VariableOffset;
    CertContext->CertLength          = CertListSize;
    CertContext->VariableLengthToEnd = DataSize - (VariableOffset + CertListSize);
    InsertTailList (&mFileListCertFileMenu.Head, &NewMenuEntry->Link); 
    NewMenuEntry->OptionNumber = Index;
    Index++;
  }
  mFileListCertFileMenu.MenuNumber = Index;

ON_EXIT:
  if (Data != NULL) {
    FreePool (Data);
  }
  return Status;
}

/**
  Get the file format of enroll menu based on secure boot manager state.

  @param[in]  CallbackData       Pointer to the secure boot manager private data
  @param[out] FileFormat         Returns the file format

  @retval EFI_SUCCESS            Success find the file system
  @retval EFI_INVALID_PARAMETER  Input parameter is NULL.
  @retval EFI_OUT_OF_RESOURCES   Fail to allocate memory.
  @retval EFI_ABORTED            User aborts the question dialog.
  @retval EFI_NOT_FOUND          File format is not found.
**/
STATIC
EFI_STATUS
GetEnrollMenuFileFormat (
  IN  TLS_CERT_MANAGER_CALLBACK_DATA    *CallbackData,
  OUT UINTN                              *FileFormat
  )
{
  if (CallbackData == NULL || FileFormat == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  switch (CallbackData->FileListState) {

  case TlsForm:
    *FileFormat = EM_FILE_FORMAT_PEM;
    break;

  case VariableForm:
    *FileFormat = EM_FILE_FORMAT_DEL_VAR;
    break;

  default:
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/**
  Enroll file based on the input file context.

  @param[in] CallbackData       Pointer to the secure boot manager private data
  @param[in] MenuEntry          Pointer to the enroll menu data
  @param[in] FileContext        Pointer to the file context

  @retval EFI_SUCCESS           Enroll file successfully.
  @retval EFI_UNSUPPORTED       Fail to read file.
  @retval EFI_OUT_OF_RESOURCES  Fail to allocate memory.
  @retval EFI_ABORTED           User aborts the enroll process.
  @retval Other                 Fail to enroll file.
**/
STATIC
EFI_STATUS
ProcessFile (
  IN TLS_CERT_MANAGER_CALLBACK_DATA   *CallbackData,
  IN ENROLL_MENU_ENTRY                 *MenuEntry,
  IN EM_FILE_CONTEXT                   *FileContext
  )
{
  EFI_STATUS                           Status;
  UINTN                                BufferSize;
  UINT8                                *Buffer;
  UINTN                                DerSize;
  UINT8                                *DerData;

  Status = ReadFile (FileContext, &BufferSize, &Buffer);
  if (EFI_ERROR (Status) || Buffer == NULL) {
    return EFI_UNSUPPORTED;
  }

  DEBUG ((DEBUG_INFO, "File size %d\n", BufferSize));
  DerSize = 0;
  DerData = NULL;
  Status = TranslatePemToDer (Buffer, BufferSize, &DerData, &DerSize);
  if (!EFI_ERROR (Status)) {
    //
    // Replace PEM data to DER, it can filter out unnecessary data in PEM data.
    //
    DEBUG ((DEBUG_INFO, "Translated size %d\n", DerSize));
    FreePool (Buffer);
    Buffer     = DerData;
    BufferSize = DerSize;
  }

  if (CallbackData->EmFileFormat == EM_FILE_FORMAT_PEM) {
    Status = ProcessPemFile (CallbackData, BufferSize, Buffer); 
  }

  FreePool (Buffer);
  return Status;
}

/**
  Initialize file system

  @param[in] CallbackData    Pointer to SECURE_BOOT_MANAGER_CALLBACK_DATA instance

  @retval EFI_SUCCESS        Initialize enroll menu successful
  @retval Other              Fail to get the file format of enroll menu.
**/
EFI_STATUS
InitializeFileListMenu (
  IN  TLS_CERT_MANAGER_CALLBACK_DATA        *CallbackData
  )
{
  EFI_STATUS                                Status;

  Status = gBS->LocateProtocol (
                  &gCryptoServicesProtocolGuid,
                  NULL,
                  (VOID **) &mCryptoServices
                  );
  if (EFI_ERROR (Status)) {
    mCryptoServices = NULL;
  }

  if (CallbackData->H2ODialog == NULL) {
    Status = gBS->LocateProtocol (
                    &gH2ODialogProtocolGuid,
                    NULL,
                    (VOID **) &CallbackData->H2ODialog
                    );
    if (EFI_ERROR(Status)) {
      return Status;
    }
  }

  DEBUG ((DEBUG_INFO, "InitializeFileListMenu\n"));

  InitializeListHead (&mFileListFsOptionMenu.Head);
  InitializeListHead (&mFileListDirectoryMenu.Head);

  if ((CallbackData->EmFileFormat == EM_FILE_FORMAT_DEL_VAR) || (CallbackData->EmFileFormat == EM_FILE_FORMAT_EXP_VAR)) {
    DEBUG ((DEBUG_INFO, "InitializeCertificateListMenu\n"));
    CallbackData->FileListState = VariableForm;
    UpdateEmCertFileExplorer (CallbackData, 0);

  } else if (CallbackData->EmFileFormat == EM_FILE_FORMAT_PEM) {
    DEBUG ((DEBUG_INFO, "InitializeFileListMenu\n"));
    CallbackData->EmDisplayContext = EM_UNKNOWN_CONTEXT;
    CallbackData->FileListState = TlsForm;
    UpdateEmFileExplorer (CallbackData, 0);

  }

  return EFI_SUCCESS;
}

/**
   
  This function allows the caller to request the current
  configuration for one or more named elements. The resulting
  string is in <ConfigAltResp> format. Also, any and all alternative
  configuration strings shall be appended to the end of the
  current configuration string. If they are, they must appear
  after the current configuration. They must contain the same
  routing (GUID, NAME, PATH) as the current configuration string.
  They must have an additional description indicating the type of
  alternative configuration the string represents,
  "ALTCFG=<StringToken>". That <StringToken> (when
  converted from Hex UNICODE to binary) is a reference to a
  string in the associated string pack.

  @param[in]  This       Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.

  @param[in]  Request    A null-terminated Unicode string in
                         <ConfigRequest> format. Note that this
                         includes the routing information as well as
                         the configurable name / value pairs. It is
                         invalid for this string to be in
                         <MultiConfigRequest> format.

  @param[out] Progress   On return, points to a character in the
                         Request string. Points to the string's null
                         terminator if request was successful. Points
                         to the most recent "&" before the first
                         failing name / value pair (or the beginning
                         of the string if the failure is in the first
                         name / value pair) if the request was not successful.

  @param[out] Results    A null-terminated Unicode string in
                         <ConfigAltResp> format which has all values
                         filled in for the names in the Request string.
                         String to be allocated by the called function.

  @retval EFI_SUCCESS             The Results string is filled with the
                                  values corresponding to all requested
                                  names.

  @retval EFI_INVALID_PARAMETER   For example, passing in a NULL
                                  for the Request parameter
                                  would result in this type of
                                  error. In this case, the
                                  Progress parameter would be
                                  set to NULL.

  @retval EFI_NOT_FOUND           Routing data doesn't match any
                                  known driver. Progress set to the
                                  first character in the routing header.
                                  Note: There is no requirement that the
                                  driver validate the routing data. It
                                  must skip the <ConfigHdr> in order to
                                  process the names.

  @retval EFI_INVALID_PARAMETER   Illegal syntax. Progress set
                                  to most recent "&" before the
                                  error or the beginning of the
                                  string.

  @retval EFI_INVALID_PARAMETER   Unknown name. Progress points
                                  to the & before the name in
                                  question.

**/
EFI_STATUS
EFIAPI
TlsCertificateManagementExtractConfig (
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

  This function applies changes in a driver's configuration.
  Input is a Configuration, which has the routing data for this
  driver followed by name / value configuration pairs. The driver
  must apply those pairs to its configurable storage. If the
  driver's configuration is stored in a linear block of data
  and the driver's name / value pairs are in <BlockConfig>
  format, it may use the ConfigToBlock helper function (above) to
  simplify the job.

  @param[in]  This           Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.

  @param[in]  Configuration  A null-terminated Unicode string in
                             <ConfigString> format.

  @param[out] Progress       A pointer to a string filled in with the
                             offset of the most recent '&' before the
                             first failing name / value pair (or the
                             beginning of the string if the failure
                             is in the first name / value pair) or
                             the terminating NULL if all was
                             successful.

  @retval EFI_SUCCESS             The results have been distributed or are
                                  awaiting distribution.

  @retval EFI_INVALID_PARAMETER   Passing in a NULL for the
                                  Results parameter would result
                                  in this type of error.

  @retval EFI_NOT_FOUND           Target for the specified routing data
                                  was not found.

**/
EFI_STATUS
EFIAPI
TlsCertificateManagementRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
//  EFI_STATUS                                 Status;
  TLS_CERT_MANAGER_FORM_CALLBACK_INFO        *CallbackInfo;

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *Progress = Configuration;

  if (Configuration == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  CallbackInfo = TLS_AUTH_CONFIG_PRIVATE_FROM_THIS (This);

  //
  // Check routing data in <ConfigHdr>.
  // Note: there is no name for Name/Value storage, only GUID will be checked
  //
  if (!HiiIsConfigHdrMatch (Configuration, CallbackInfo->FormGuid, mTlcAuthManageStorageName)) {
    return EFI_NOT_FOUND;
  }

  // //
  // // Get NV configuration data.
  // //
  // BufferSize = sizeof (HTTP_BOOT_CONFIG_IFR_NVDATA);
  // ZeroMem (&CallbackInfo->HttpBootNvData, BufferSize);
  // Status = gHiiConfigRouting->ConfigToBlock (
  //                           gHiiConfigRouting,
  //                           Configuration,
  //                           (UINT8 *) &CallbackInfo->HttpBootNvData,
  //                           &BufferSize,
  //                           Progress
  //                           );
  // if (EFI_ERROR (Status)) {
  //   return Status;
  // }

  return EFI_SUCCESS;
}

/**
  This function is called to provide results data to the driver.
  This data consists of a unique key that is used to identify
  which data is either being passed back or being asked for.

  @param[in]       This          Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param[in]       Action        Specifies the type of action taken by the browser.
  @param[in]       QuestionId    A unique value which is sent to the original
                                 exporting driver so that it can identify the type
                                 of data to expect. The format of the data tends to
                                 vary based on the opcode that generated the callback.
  @param[in]       Type          The type of value for the question.
  @param[in, out]  Value         A pointer to the data being sent to the original
                                 exporting driver.
  @param[out]      ActionRequest On return, points to the action requested by the
                                 callback function.

  @retval EFI_SUCCESS            The callback successfully handled the action.
  @retval EFI_OUT_OF_RESOURCES   Not enough storage is available to hold the
                                 variable and its data.
  @retval EFI_DEVICE_ERROR       The variable could not be saved.
  @retval EFI_UNSUPPORTED        The specified Action is not supported by the
                                 callback.
**/
EFI_STATUS
EFIAPI
TlsCertificateManagementCallback (
  IN CONST  EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN        EFI_BROWSER_ACTION               Action,
  IN        EFI_QUESTION_ID                  QuestionId,
  IN        UINT8                            Type,
  IN OUT    EFI_IFR_TYPE_VALUE               *Value,
  OUT       EFI_BROWSER_ACTION_REQUEST       *ActionRequest
  )
{
  EFI_STATUS                                 Status;
  TLS_CERT_MANAGER_FORM_CALLBACK_INFO        *CallbackInfo;
  TLS_CERT_MANAGER_CALLBACK_DATA             *CallbackData;

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status   = EFI_SUCCESS;
  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  CallbackInfo = TLS_AUTH_CONFIG_PRIVATE_FROM_THIS (This);
  CallbackData = &CallbackInfo->CallbackData;

  //
  // This page only have goto operation, so it will not get the callback of CHANGED.
  //
  // if (!mgH2OFormBrowserProtocoSupported && Action == EFI_BROWSER_ACTION_CHANGING) {
  //   //
  //   // For compatible to old form browser which only use EFI_BROWSER_ACTION_CHANGING action,
  //   // change action to EFI_BROWSER_ACTION_CHANGED to make it workable.
  //   //
  //   Action = EFI_BROWSER_ACTION_CHANGED;
  // }
 
  if (Action != EFI_BROWSER_ACTION_CHANGING) {
    return EFI_UNSUPPORTED;
  }

  DEBUG ((DEBUG_INFO, "Callback Action: %d QuestionId: %08x\n", Action, QuestionId));

  if (Value == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  switch (QuestionId) {
  case KEY_IMPORT_CERT_FILE:
  case KEY_DELETE_CERT_FILE:
  case KEY_EXPORT_CERT_FILE:
    CallbackData->EmFileFormat = 0;
    if (QuestionId == KEY_IMPORT_CERT_FILE) {
      CallbackData->EmFileFormat = EM_FILE_FORMAT_PEM;
    } else if (QuestionId == KEY_DELETE_CERT_FILE) { 
      CallbackData->EmFileFormat = EM_FILE_FORMAT_DEL_VAR;
    } else if (QuestionId == KEY_EXPORT_CERT_FILE) {
      CallbackData->EmFileFormat = EM_FILE_FORMAT_EXP_VAR;
    }
    CallbackData->RegisteredHandle = CallbackInfo->RegisteredHandle;
    CallbackData->EmDisplayContext = 0;
    CallbackData->Type             = Type;
    CallbackData->Value            = Value;
    Status = InitializeFileListMenu (CallbackData);
    break;

  default:
    DEBUG ((DEBUG_INFO, "Callback Action: %d QuestionId: %08x DEFAULT\n", Action, QuestionId));
    if (QuestionId >= ENROLL_FILE_OPTION_OFFSET) {
      if (CallbackData->FileListState == TlsForm) {
        if (UpdateEmFileExplorer (CallbackData, QuestionId)) {
          CallbackData->EmDisplayContext  = EM_UNKNOWN_CONTEXT;
          UpdateEmFileExplorer (CallbackData, 0);
        }
      } else if (CallbackData->FileListState == VariableForm) {
        UpdateEmCertFileExplorer (CallbackData, QuestionId);
      }
      break;
    }
  };
  return Status;
}

/**
  Initialize the configuration form to SCU Security\Link.

  @param[in]  Private             Pointer to the driver private data.

  @retval EFI_SUCCESS             The configuration form is initialized.
  @retval EFI_INVALID_PARAMETER   HiiHandle is NULL.
  @retval EFI_OUT_OF_RESOURCES    Failed to allocate memory.

**/
EFI_STATUS
TlsCertificateManagementFormUnload (
  IN   VOID                            *CallbackInfo
  )
{
  TLS_CERT_MANAGER_FORM_CALLBACK_INFO  *TmpCallbackInfo;

  TmpCallbackInfo = CallbackInfo;
  //
  // Uninstall Device Path Protocol and Config Access protocol to driver handle.
  //
  gBS->UninstallMultipleProtocolInterfaces (
         TmpCallbackInfo->ChildHandle,
         &gEfiDevicePathProtocolGuid,
         TmpCallbackInfo->HiiVendorDevicePath,
         &gEfiHiiConfigAccessProtocolGuid,
         &TmpCallbackInfo->ConfigAccess,
         NULL
         );

  HiiRemovePackages (TmpCallbackInfo->ChildHandle);

  if (TmpCallbackInfo->HiiVendorDevicePath != NULL) {
    FreePool (TmpCallbackInfo->HiiVendorDevicePath);
  }
  if (TmpCallbackInfo != NULL) {
    FreePool (TmpCallbackInfo);
  }
  if (mSecureBootFileOptionStrDepository != NULL) {
      FreePool (mSecureBootFileOptionStrDepository);
  }
  return EFI_SUCCESS;
}

/**
  Initialize the configuration form to SCU Security\Link.

  @param[in]  Private             Pointer to the driver private data.

  @retval EFI_SUCCESS             The configuration form is initialized.
  @retval EFI_INVALID_PARAMETER   HiiHandle is NULL.
  @retval EFI_OUT_OF_RESOURCES    Failed to allocate memory.

**/
EFI_STATUS
TlsCertificateManagementFormInit (
  IN   VOID                            **CallbackInfo
  )
{
  EFI_STATUS                           Status;
  VENDOR_DEVICE_PATH                   *VendorDeviceNode;
  UINTN                                SizeOfBuffer;
  TLS_CERT_MANAGER_FORM_CALLBACK_INFO  *TmpCallbackInfo;

  mSecureBootFileOptionStrDepository = AllocateZeroPool (sizeof (STRING_DEPOSITORY));
  TmpCallbackInfo = AllocateZeroPool (sizeof (TLS_CERT_MANAGER_FORM_CALLBACK_INFO));
  VendorDeviceNode = AllocateZeroPool (sizeof (VENDOR_DEVICE_PATH) + sizeof (EFI_DEVICE_PATH_PROTOCOL));
  if ((TmpCallbackInfo == NULL) || (VendorDeviceNode == NULL) || (mSecureBootFileOptionStrDepository == NULL)) {
    if (TmpCallbackInfo != NULL) {
      FreePool (TmpCallbackInfo);
    }
    if (VendorDeviceNode != NULL) {
      FreePool (VendorDeviceNode);
    }
    if (mSecureBootFileOptionStrDepository != NULL) {
      FreePool (mSecureBootFileOptionStrDepository);
    }
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Set VENDOR device path content.
  //
  VendorDeviceNode->Header.Type    = HARDWARE_DEVICE_PATH;
  VendorDeviceNode->Header.SubType = HW_VENDOR_DP;
  CopyGuid (&VendorDeviceNode->Guid, &gEfiCallerIdGuid);
  SetDevicePathNodeLength (VendorDeviceNode, sizeof (VENDOR_DEVICE_PATH));

  //
  // Set DevicePath end node.
  //
  SetDevicePathEndNode (VendorDeviceNode + 1);

  TmpCallbackInfo->Signature                  = TLS_AUTH_CONFIG_PRIVATE_DATA_SIGNATURE;
  TmpCallbackInfo->HiiVendorDevicePath        = (EFI_DEVICE_PATH_PROTOCOL *) VendorDeviceNode;
  TmpCallbackInfo->ConfigAccess.ExtractConfig = TlsCertificateManagementExtractConfig;
  TmpCallbackInfo->ConfigAccess.RouteConfig   = TlsCertificateManagementRouteConfig;
  TmpCallbackInfo->ConfigAccess.Callback      = TlsCertificateManagementCallback;
  TmpCallbackInfo->FormGuid                   = &gTlsAuthConfigGuid;

  //
  // Install Device Path Protocol and Config Access protocol to driver handle.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &TmpCallbackInfo->ChildHandle,
                  &gEfiDevicePathProtocolGuid,
                  TmpCallbackInfo->HiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &TmpCallbackInfo->ConfigAccess,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    goto Error;
  }

  //
  // Publish our HII data.
  //
  TmpCallbackInfo->RegisteredHandle = HiiAddPackages (
                                        &gTlsAuthConfigGuid,
                                        TmpCallbackInfo->ChildHandle,
                                        TlsDxeStrings,
                                        TlsAuthConfigVfrBin,
                                        NULL
                                        );
  if (TmpCallbackInfo->RegisteredHandle == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Error;
  }

  SizeOfBuffer = sizeof (EFI_GUID);
  Status = LibPcdSetExPtrS (&gLinkSecurityTokenSpaceGuid, PcdToken (PcdH2OTlsCertManageFormsetGuid), &SizeOfBuffer, &gTlsAuthConfigGuid);
  ASSERT_EFI_ERROR (Status);
  *CallbackInfo = TmpCallbackInfo;
  return EFI_SUCCESS;

Error:
  TlsCertificateManagementFormUnload (TmpCallbackInfo);
  return Status;
}