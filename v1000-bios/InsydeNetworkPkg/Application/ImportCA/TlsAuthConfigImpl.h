/** @file
  Header file of Miscellaneous Routines for TlsAuthConfigDxe driver.

Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>

This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __TLS_AUTH_CONFIG_IMPL_H__
#define __TLS_AUTH_CONFIG_IMPL_H__

#include <Uefi.h>

#include <Protocol/HiiConfigAccess.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/CryptoServices.h>

//
// Libraries
//
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/HiiLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/FileExplorerLib.h>
#include <Library/PrintLib.h>

#include <Guid/MdeModuleHii.h>
#include <Guid/ImageAuthentication.h>
#include <Guid/TlsAuthentication.h>

#include <Library/ShellLib.h>

//
// Include files with function prototypes
//
#define TLS_AUTH_CONFIG_PRIVATE_DATA_SIGNATURE    SIGNATURE_32 ('T', 'A', 'C', 'D')
#define TLS_AUTH_CONFIG_PRIVATE_FROM_THIS(a)      CR (a, TLS_AUTH_CONFIG_PRIVATE_DATA, ConfigAccess, TLS_AUTH_CONFIG_PRIVATE_DATA_SIGNATURE)

#define TLS_AUTH_CONFIG_VAR_BASE_ATTR  (EFI_VARIABLE_BOOTSERVICE_ACCESS)

typedef struct _TLS_AUTH_CONFIG_FILE_CONTEXT      TLS_AUTH_CONFIG_FILE_CONTEXT;

///
/// HII specific Vendor Device Path definition.
///
typedef struct {
  VENDOR_DEVICE_PATH                VendorDevicePath;
  EFI_DEVICE_PATH_PROTOCOL          End;
} HII_VENDOR_DEVICE_PATH;

struct _TLS_AUTH_CONFIG_FILE_CONTEXT {
  EFI_FILE_HANDLE                   FHandle;
  UINT16                            *FileName;
};

#define EFI_TLS_CA_CERTIFICATE_VARIABLE       L"TlsCaCertificate"

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
  );

//
// Implement in TlsLib, TlsConfig.c
//
EFI_STATUS
EFIAPI
TlsChkCaCertificate (
  IN     VOID                     *Data,
  IN     UINTN                    DataSize
  );

//
// Implement in TlsLib, TlsConfig.c
//
EFI_STATUS
EFIAPI
TlsDumpCaCertificate (
  IN     VOID                     *Data,
  IN     UINTN                    DataSize
  );

#endif

