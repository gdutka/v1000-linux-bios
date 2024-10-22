/** @file
  DNS application sample code

;******************************************************************************
;* Copyright (c) 2017 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "ImportCA.h"
#include "TlsAuthConfigImpl.h"
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>

BOOLEAN             mDebugMode = 0;

CHAR16              mCertGuidStr[100];

SHELL_PARAM_ITEM    mNetworkCheckList[] = {
  {
    L"--file",      // Indicate Certificate file, INPUT or OUTPUT
    TypeValue
  },
  {
    L"--guid",      // Indicate certificate GUID
    TypeValue       // If not indicate, it will input as this application's file GUID.
  },
  {
    L"--export",    // Export certificate to filel, the attach value is INDEX of certificates.
    TypeValue
  },
  {
    L"--import",    // Import certificate
    TypeFlag
  },
  {
    L"--list",      // List certificates
    TypeFlag
  },
  {
    L"--debug",
    TypeFlag
  },
  {
    NULL,
    TypeMax
  },
};

/**
  CA check function, modified from TlsSetCaCertificate function.

  @param[in]  Data        Pointer to the data buffer of a DER-encoded binary
                          X.509 certificate or PEM-encoded X.509 certificate.
  @param[in]  DataSize    The size of data buffer in bytes.

  @retval  EFI_SUCCESS             The operation succeeded.
  @retval  EFI_INVALID_PARAMETER   The parameter is invalid.
  @retval  EFI_OUT_OF_RESOURCES    Required resources could not be allocated.
  @retval  EFI_ABORTED             Invalid X.509 certificate.

**/
EFI_STATUS
EFIAPI
TlsChkCaCertificate (
  IN     VOID                     *Data,
  IN     UINTN                    DataSize
  )
{
  BIO             *BioCert;
  X509            *Cert;
  EFI_STATUS      Status;

  BioCert   = NULL;
  Cert      = NULL;
  Status    = EFI_SUCCESS;

  if (Data == NULL || DataSize == 0) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // DER-encoded binary X.509 certificate or PEM-encoded X.509 certificate.
  // Determine whether certificate is from DER encoding, if so, translate it to X509 structure.
  //
  Cert = d2i_X509 (NULL, (const unsigned char ** )&Data, (long) DataSize);
  if (Cert == NULL) {
    //
    // Certificate is from PEM encoding.
    //
    BioCert = BIO_new (BIO_s_mem ());
    if (BioCert == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ON_EXIT;
    }

    if (BIO_write (BioCert, Data, (UINT32) DataSize) <= 0) {
      Status = EFI_ABORTED;
      goto ON_EXIT;
    }

    Cert = PEM_read_bio_X509 (BioCert, NULL, NULL, NULL);
    if (Cert == NULL) {
      Status = EFI_ABORTED;
    }
  }

ON_EXIT:
  if (BioCert != NULL) {
    BIO_free (BioCert);
  }

  if (Cert != NULL) {
    X509_free (Cert);
  }

  return Status;
}

/**
  CA parse and dump data function, modified from TlsChkCaCertificate function.

  @param[in]  Data        Pointer to the data buffer of a DER-encoded binary
                          X.509 certificate or PEM-encoded X.509 certificate.
  @param[in]  DataSize    The size of data buffer in bytes.

  @retval  EFI_SUCCESS             The operation succeeded.
  @retval  EFI_INVALID_PARAMETER   The parameter is invalid.
  @retval  EFI_OUT_OF_RESOURCES    Required resources could not be allocated.
  @retval  EFI_ABORTED             Invalid X.509 certificate.

**/
EFI_STATUS
EFIAPI
TlsDumpCaCertificate (
  IN     VOID                     *Data,
  IN     UINTN                    DataSize
  )
{
  BIO             *BioCert;
  X509            *Cert;
  EFI_STATUS      Status;

  X509_NAME       *SubjectName;
  X509_NAME       *IssuerName;

  UINT8           *StringBuffer;

  BioCert   = NULL;
  Cert      = NULL;
  Status    = EFI_SUCCESS;

  if (Data == NULL || DataSize == 0) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // DER-encoded binary X.509 certificate or PEM-encoded X.509 certificate.
  // Determine whether certificate is from DER encoding, if so, translate it to X509 structure.
  //
  Cert = d2i_X509 (NULL, (const unsigned char ** )&Data, (long) DataSize);
  if (Cert == NULL) {
    //
    // Certificate is from PEM encoding.
    //
    BioCert = BIO_new (BIO_s_mem ());
    if (BioCert == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ON_EXIT;
    }

    if (BIO_write (BioCert, Data, (UINT32) DataSize) <= 0) {
      Status = EFI_ABORTED;
      goto ON_EXIT;
    }

    Cert = PEM_read_bio_X509 (BioCert, NULL, NULL, NULL);
    if (Cert == NULL) {
      Status = EFI_ABORTED;
    }
  }

  SubjectName = X509_get_subject_name (Cert);
  if (SubjectName != NULL) {
    StringBuffer = X509_NAME_oneline (SubjectName, 0, 0);
    AsciiPrint ("SubjectName: %a\n", StringBuffer);
    free (StringBuffer);
//    free (SubjectName);
  }
  IssuerName  = X509_get_issuer_name (Cert);
  if (IssuerName != NULL) {
    StringBuffer = X509_NAME_oneline (IssuerName, 0, 0);
    AsciiPrint ("IssuerName: %a\n", StringBuffer);
    free (StringBuffer);
//    free (IssuerName);
  }

ON_EXIT:
  if (BioCert != NULL) {
    BIO_free (BioCert);
  }

  if (Cert != NULL) {
    X509_free (Cert);
  }

  return Status;
}
//
// Don't free output vairables.
//
EFI_STATUS
DumpCertificate (
  IN EFI_SIGNATURE_LIST                *CertList,
  OUT VOID                             **CertContent,
  OUT UINTN                            *CertSize
  )
{
  EFI_SIGNATURE_DATA                   *CertRecord;
  UINTN                                CertCount;
  
  UINTN                                Index;

  UINT8                                *TmpCert;
  UINTN                                TmpCertSize;

  CertCount  = (CertList->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - CertList->SignatureHeaderSize) / CertList->SignatureSize;
  for (Index = 0; Index < CertCount; Index++) {
    CertRecord = (EFI_SIGNATURE_DATA *) ((UINT8 *) CertList
                                            + sizeof (EFI_SIGNATURE_LIST)
                                            + CertList->SignatureHeaderSize
                                            + Index * CertList->SignatureSize);

    TmpCert     = CertRecord->SignatureData;
    TmpCertSize = CertList->SignatureSize - sizeof(EFI_SIGNATURE_DATA);
    GuidToString (&CertRecord->SignatureOwner, mCertGuidStr, 100);
    
    if (CertContent == NULL) {
      //
      // Display GUID and help
      //
      AsciiPrint ("  %d %s\n", Index, mCertGuidStr);
      TlsDumpCaCertificate (TmpCert, TmpCertSize);
      continue;
    }
    //
    // Output Cert CertSize and GuidString.
    //
    if ((CertContent == NULL) || (CertSize == NULL)) {
      return EFI_INVALID_PARAMETER;
    }

    *CertContent      = TmpCert;
    *CertSize         = TmpCertSize;
    break;
  }

  return EFI_SUCCESS;
}

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

  VariableName = EFI_TLS_CA_CERTIFICATE_VARIABLE;
  *Data        = NULL;
  TmpDataSize  = 0;

  Status = gRT->GetVariable(
                  VariableName,
                  &gEfiTlsCaCertificateGuid,
                  NULL,
                  &TmpDataSize,
                  NULL
                  );
  if (EFI_ERROR (Status) && Status != EFI_BUFFER_TOO_SMALL) {
    return Status;
  }
  TmpDataPtr = AllocateZeroPool (TmpDataSize);
  if (TmpDataPtr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = gRT->GetVariable(
                  VariableName,
                  &gEfiTlsCaCertificateGuid,
                  NULL,
                  &TmpDataSize,
                  (VOID *) TmpDataPtr
                  );
  if (EFI_ERROR (Status)) {
    FreePool (TmpDataPtr);
    return Status;
  }

  *Data           = TmpDataPtr;
  *DataSize = TmpDataSize;
  return EFI_SUCCESS;
}

EFI_STATUS
OutputCertificateToFile (
  IN CHAR16                            *FileName,
  IN UINT8                             *Certificate,
  IN UINTN                             CertificateSize
  )
{
  EFI_STATUS                           Status;
  SHELL_FILE_HANDLE                    FileHandle;
  UINTN                                WriteSize;
    
  Status = ShellOpenFileByName (FileName, &FileHandle, EFI_FILE_MODE_WRITE|EFI_FILE_MODE_READ|EFI_FILE_MODE_CREATE, 0);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  WriteSize = CertificateSize;
  Status = ShellWriteFile (FileHandle, &WriteSize, (VOID *) Certificate);
  ShellFlushFile (FileHandle);
  ShellCloseFile (&FileHandle);

  return Status;
}

EFI_STATUS
ListCertificatesFromCertStore (
  IN UINTN                             SelectIndex,
  IN CHAR16                            *OutputFileName
  )
{
  EFI_STATUS                           Status;
  UINT8                                *Data;
  UINTN                                DataSize;
  EFI_SIGNATURE_LIST                   *CertList;
  UINTN                                ItemDataSize;
  UINTN                                Index;

  BOOLEAN                              DisplayFlag;
  UINTN                                CertSize;
  UINT8                                *Cert;

  DataSize = 0;
  Data     = NULL;
  Status   = GetCaCertificateVariable (&Data, &DataSize);
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

  DisplayFlag = TRUE;
  if (OutputFileName != NULL) {
    DisplayFlag = FALSE;
  }

  if (DisplayFlag) {
    AsciiPrint ("CA db total size : %d bytes\n", DataSize);
  }
  Index        = 0;
  ItemDataSize = DataSize;
  Cert         = NULL;
  CertSize     = 0;
  CertList     = (EFI_SIGNATURE_LIST *) Data;
  while ((ItemDataSize > 0) && (ItemDataSize >= CertList->SignatureListSize)) {

    if (!CompareGuid (&CertList->SignatureType, &gEfiCertX509Guid)) {
      //
      // The signature type is not supported in current implementation.
      //
      if (DisplayFlag) {
        AsciiPrint ("CERT is not supported\n");
      }
      ItemDataSize -= CertList->SignatureListSize;
      CertList = (EFI_SIGNATURE_LIST *) ((UINT8 *) CertList + CertList->SignatureListSize);
      continue;
    }

    if (DisplayFlag) {
      AsciiPrint ("\nIndex %d:\n", Index);
      AsciiPrint ("GUID for CERT\n");
    }
    if ((!DisplayFlag) && (SelectIndex == Index)) {
      Status = DumpCertificate (CertList, (VOID **) &Cert, &CertSize);
      if (EFI_ERROR (Status)) {
        goto ON_EXIT;
      }
      Status = OutputCertificateToFile (OutputFileName, Cert, CertSize);
      goto ON_EXIT;
    }
    Status = DumpCertificate (CertList, NULL, NULL);
    if (EFI_ERROR (Status)) {
      goto ON_EXIT;
    }

    ItemDataSize -= CertList->SignatureListSize;
    CertList = (EFI_SIGNATURE_LIST *) ((UINT8 *) CertList + CertList->SignatureListSize);
    Index++;
  }

ON_EXIT:
  if (Data != NULL) {
    FreePool (Data);
  }
  return Status;
}

/**
  The application entry point

  @param[in]  ImageHandle              Common header of ImageHandle
  @param[in]  SystemTable              Common header of SystemTable

  @retval EFI_SUCCESS                  Application process success
  @retval other                        Wrong status.

**/
EFI_STATUS
EFIAPI
ImportCAEntry (
  IN EFI_HANDLE                        ImageHandle,
  IN EFI_SYSTEM_TABLE                  *SystemTable
  )
{
  EFI_STATUS                           Status;
  LIST_ENTRY                           *ParamPackage;
  EFI_GUID                             CertGuid;
  CHAR16                               *CertGuidStr;
  CHAR16                               *FileName;
  UINTN                                CAIndex;
  CHAR16                               *CAIndexString;

  ZeroMem (&CertGuid, sizeof (EFI_GUID));
  FileName = NULL;
  ShellInitialize ();
  ParamPackage = NULL;
  Status = ShellCommandLineParseEx (mNetworkCheckList, &ParamPackage, NULL, TRUE, FALSE);
  if (EFI_ERROR (Status)) {
    Status = EFI_INVALID_PARAMETER;
    goto ON_EXIT;
  }
  mDebugMode = ShellCommandLineGetFlag (ParamPackage, L"--debug");

  if (ShellCommandLineGetFlag (ParamPackage, L"--list")) {
    Status = ListCertificatesFromCertStore (0, NULL);
    goto ON_EXIT;
  }

  FileName = NULL;
  if (ShellCommandLineGetFlag (ParamPackage, L"--file")) {
    FileName = (CHAR16 *) ShellCommandLineGetValue (ParamPackage, L"--file");
    if (FileName == NULL) {
      Status = EFI_INVALID_PARAMETER;
      goto ON_EXIT;
    }
  }

  if (ShellCommandLineGetFlag (ParamPackage, L"--export")) {

    //
    // File parameter check
    //
    if (FileName == NULL) {
      AsciiPrint ("Please input filename to export CA to file\n");
      Status = EFI_INVALID_PARAMETER;
      goto ON_EXIT;
    }

    CAIndexString = (CHAR16 *) ShellCommandLineGetValue (ParamPackage, L"--export");
    if (CAIndexString == NULL) {
      Status = EFI_INVALID_PARAMETER;
      goto ON_EXIT;
    }
    CAIndex = StrDecimalToUintn (CAIndexString);

    Status = ListCertificatesFromCertStore (CAIndex, FileName);
    goto ON_EXIT;
  }

  if (ShellCommandLineGetFlag (ParamPackage, L"--import")) {

    //
    // File parameter check
    //
    if (FileName == NULL) {
      AsciiPrint ("Please input filename to import CA to file\n");
      Status = EFI_INVALID_PARAMETER;
      goto ON_EXIT;
    }

    //
    // CA GUID check.
    //
    CertGuidStr = NULL;
    if (ShellCommandLineGetFlag (ParamPackage, L"--guid")) {
      CertGuidStr = (CHAR16 *) ShellCommandLineGetValue (ParamPackage, L"--guid");
      if (CertGuidStr == NULL) {
        Status = EFI_INVALID_PARAMETER;
        goto ON_EXIT;
      }
      Status = StrToGuid (CertGuidStr, &CertGuid);
      if (EFI_ERROR (Status)) {
        AsciiPrint ("GUID translate failed.\n");
        goto ON_EXIT;
      }
    } else {
      AsciiPrint ("GUID use default.\n");
      CopyMem (&CertGuid, &gEfiCallerIdGuid, sizeof (EFI_GUID));
    } 

    Status = EnrollCertDatabase (FileName, EFI_TLS_CA_CERTIFICATE_VARIABLE, &CertGuid);

  }

ON_EXIT:
  if (ParamPackage != NULL) {
    gBS->FreePool (ParamPackage);
  }
  return Status;
}
