/** @file
  The Miscellaneous Routines for TlsAuthConfigDxe driver.

Copyright (c) 2016 - 2017, Intel Corporation. All rights reserved.<BR>

This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "TlsAuthConfigImpl.h"

VOID                    *mStartOpCodeHandle = NULL;
VOID                    *mEndOpCodeHandle   = NULL;
EFI_IFR_GUID_LABEL      *mStartLabel        = NULL;
EFI_IFR_GUID_LABEL      *mEndLabel          = NULL;


CHAR16                  mTlsAuthConfigStorageName[] = L"TLS_AUTH_CONFIG_IFR_NVDATA";

/**
  Worker function that prints an EFI_GUID into specified Buffer.

  @param[in]     Guid          Pointer to GUID to print.
  @param[in]     Buffer        Buffer to print Guid into.
  @param[in]     BufferSize    Size of Buffer.

  @retval    Number of characters printed.

**/
UINTN
GuidToString (
  IN  EFI_GUID  *Guid,
  IN  CHAR16    *Buffer,
  IN  UINTN     BufferSize
  )
{
  return UnicodeSPrint (
           Buffer,
           BufferSize,
           L"%g",
           Guid
           );
}

/**
  Enroll a new X509 certificate into Variable.

  @param[in] PrivateData     The module's private data.
  @param[in] VariableName    Variable name of CA database.

  @retval   EFI_SUCCESS            New X509 is enrolled successfully.
  @retval   EFI_OUT_OF_RESOURCES   Could not allocate needed resources.

**/
EFI_STATUS
EnrollX509toVariable (
  IN VOID                           *X509Data,
  IN UINTN                          X509DataSize,
  IN CHAR16                         *VariableName,
  IN EFI_GUID                       *CertGuid
  )
{
  EFI_STATUS                        Status;
  EFI_SIGNATURE_LIST                *CACert;
  EFI_SIGNATURE_DATA                *CACertData;
  VOID                              *Data;
  UINTN                             DataSize;
  UINTN                             SigDataSize;
  UINT32                            Attr;

  SigDataSize   = 0;
  DataSize      = 0;
  CACert        = NULL;
  CACertData    = NULL;
  Data          = NULL;

  SigDataSize = sizeof(EFI_SIGNATURE_LIST) + sizeof(EFI_SIGNATURE_DATA) - 1 + X509DataSize;
  Data = AllocateZeroPool (SigDataSize);
  if (Data == NULL) {
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
  Attr = TLS_AUTH_CONFIG_VAR_BASE_ATTR;

  Status = gRT->GetVariable(
                  VariableName,
                  &gEfiTlsCaCertificateGuid,
                  NULL,
                  &DataSize,
                  NULL
                  );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Attr |= EFI_VARIABLE_APPEND_WRITE;
  } else if (Status != EFI_NOT_FOUND) {
    goto ON_EXIT;
  }

  //
  // Check
  //
  Status = TlsChkCaCertificate  (X509Data, X509DataSize);
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

  TlsDumpCaCertificate (X509Data, X509DataSize);

  Status = gRT->SetVariable(
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
  return Status;
}

/**
  Enroll Cert into TlsCaCertificate. The GUID will be Private->CertGuid.

  @param[in] PrivateData     The module's private data.
  @param[in] VariableName    Variable name of signature database.

  @retval   EFI_SUCCESS            New Cert enrolled successfully.
  @retval   EFI_INVALID_PARAMETER  The parameter is invalid.
  @retval   EFI_UNSUPPORTED        The Cert file is unsupported type.
  @retval   others                 Fail to enroll Cert data.

**/
EFI_STATUS
EnrollCertDatabase (
  IN CHAR16                        *FilePath,
  IN CHAR16                        *VariableName,
  IN EFI_GUID                      *CertGuid
  )
{
  EFI_STATUS                       Status;
  EFI_FILE_INFO                    *FileInfo;
  SHELL_FILE_HANDLE                FileHandle;
  UINT8                            *Content;
  UINTN                            ContentLength;

  //
  // Parse the file's postfix.
  //
  FileHandle    = NULL;
  Content       = NULL;
  ContentLength = 0;
  Status = ShellOpenFileByName (FilePath, &FileHandle, EFI_FILE_MODE_READ, 0);
  if (EFI_ERROR (Status)) {
    AsciiPrint ("File open failed %08x", Status);
    goto Exit;
  }
  FileInfo = ShellGetFileInfo (FileHandle);
  if (FileInfo == NULL) {
    Status = EFI_UNSUPPORTED;
    goto Exit;
  }

  ContentLength = (UINTN) FileInfo->FileSize;
  Content = AllocateZeroPool (ContentLength);
  if (Content == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  Status = ShellReadFile (FileHandle, &ContentLength, Content);
  if (EFI_ERROR (Status)) {
      AsciiPrint ("File read failed %08x", Status);
      goto Exit;
  }

  Status = EnrollX509toVariable (Content, ContentLength, VariableName, CertGuid);

Exit:
  if (FileHandle != NULL) {
    ShellCloseFile (&FileHandle);
  }
  if (Content != NULL) {
    FreePool (Content);
  }
  return Status;
}
