/** @file
  TLS certificate management related functions.

;******************************************************************************
;* Copyright (c) 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _HTTP_BOOT_CONFIG_H_
#define _HTTP_BOOT_CONFIG_H_

//#include "TlsDriver.h"
#include "TlsImpl.h"
#include <Library/HiiLib.h>
#include <Library/PrintLib.h>
#include <Library/GenericUtilityLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/VariableLib.h>

#include <Protocol/DevicePathToText.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/CryptoServices.h>
#include <Protocol/H2ODialog.h>
#include <Protocol/BlockIo.h>
#include <Protocol/LoadFile.h>
#include <Protocol/H2OFormBrowser.h>

#include <Guid/FileInfo.h>
#include <Guid/FileSystemVolumeLabelInfo.h>
#include <Guid/TlsAuthConfigHii.h>
#include <Guid/ImageAuthentication.h>
#include <Guid/MdeModuleHii.h>


//#include <Protocol/FormBrowserEx2.h>

extern   UINT8                            TlsDxeStrings[];
extern   UINT8                            TlsAuthConfigVfrBin[];

typedef struct _TLS_CERT_MANAGER_FORM_CALLBACK_INFO   TLS_CERT_MANAGER_FORM_CALLBACK_INFO;

#define TLS_X509_INFO_STRING_MAX_SIZE                 800
#define TLS_AUTH_CONFIG_PRIVATE_DATA_SIGNATURE        SIGNATURE_32 ('T', 'A', 'C', 'D')
#define TLS_AUTH_CONFIG_PRIVATE_FROM_THIS(a)          CR (a, TLS_CERT_MANAGER_FORM_CALLBACK_INFO, ConfigAccess, TLS_AUTH_CONFIG_PRIVATE_DATA_SIGNATURE)
#define TLS_CA_CERTIFICATE_VARIABLE                   L"TlsCaCertificate"
#define TLS_CA_CERTIFICATE_VARIABLE_BASE_ATTR         (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE)

#define EM_MENU_ENTRY_SIGNATURE                       SIGNATURE_32 ('E', 'M', 'E', 'T')

typedef enum {
  TlsForm = 0,
  VariableForm,
  FileListStateMax
} FILE_LIST_STATE;

typedef struct {
  UINTN           Signature;
  LIST_ENTRY      Head;
  UINTN           MenuNumber;
} ENROLL_MENU_OPTION;

typedef struct {
  EFI_HANDLE                        Handle;
  EFI_DEVICE_PATH_PROTOCOL          *DevicePath;
  EFI_FILE_HANDLE                   FHandle;
  UINT16                            *FileName;
  EFI_FILE_SYSTEM_VOLUME_LABEL      *Info;
  BOOLEAN                           IsRoot;
  BOOLEAN                           IsDir;
  BOOLEAN                           IsRemovableMedia;
  BOOLEAN                           IsLoadFile;
  BOOLEAN                           IsBootLegacy;
} EM_FILE_CONTEXT;

typedef struct {
  UINTN                            Signature;
  LIST_ENTRY                       Link;
  UINTN                            OptionNumber;
  UINT16                           *DisplayString;
  UINT16                           *HelpString;
  EFI_STRING_ID                    DisplayStringToken;
  EFI_STRING_ID                    HelpStringToken;
  UINTN                            ContextSelection;
  VOID                             *VariableContext;
} ENROLL_MENU_ENTRY;

typedef struct _STRING_LIST_NODE STRING_LIST_NODE;

struct _STRING_LIST_NODE {
  EFI_STRING_ID           StringToken;
  STRING_LIST_NODE        *Next;
};

typedef struct _STRING_DEPOSITORY {
  UINTN                   TotalNodeNumber;
  STRING_LIST_NODE        *CurrentNode;
  STRING_LIST_NODE        *ListHead;
} STRING_DEPOSITORY;

typedef struct {
  UINTN                             VariableOffset;
  UINTN                             CertLength;
  /// For delete processing
  UINTN                             VariableLengthToEnd;
} EM_CERTIFICATE_CONTEXT;

typedef struct {
  EFI_HANDLE                       Handle;
  EFI_DEVICE_PATH_PROTOCOL         *DevicePath;
  EFI_FILE_HANDLE                  FHandle;
  UINT16                           *FileName;
  EFI_FILE_SYSTEM_VOLUME_LABEL     *Info;

  BOOLEAN                          IsRoot;
  BOOLEAN                          IsDir;
  BOOLEAN                          IsRemovableMedia;
  BOOLEAN                          IsLoadFile;
  BOOLEAN                          IsBootLegacy;
} BM_FILE_CONTEXT;

typedef struct _TLS_CERT_MANAGER_CALLBACK_DATA {
  H2O_DIALOG_PROTOCOL              *H2ODialog;
  UINTN                            EmDisplayContext;
  UINTN                            EmFileFormat;
  UINT8                            Type;
  EFI_IFR_TYPE_VALUE               *Value;
  FILE_LIST_STATE                  FileListState;
  EFI_HII_HANDLE                   RegisteredHandle;
} TLS_CERT_MANAGER_CALLBACK_DATA;

struct _TLS_CERT_MANAGER_FORM_CALLBACK_INFO {
  UINT32                           Signature;
  EFI_HANDLE                       ChildHandle;
  EFI_GUID                         *FormGuid;
  EFI_DEVICE_PATH_PROTOCOL         *HiiVendorDevicePath;
  EFI_HII_HANDLE                   RegisteredHandle;
  EFI_HII_CONFIG_ACCESS_PROTOCOL   ConfigAccess;

  TLS_CERT_MANAGER_CALLBACK_DATA   CallbackData;
};

#define MAX_CHAR                                          480

#define EM_FILE_CONTEXT_SELECT                            0x1

#define EM_FILE_SYSTEM                                    0x0001
#define EM_DIRECTORY                                      0x0002
#define EM_UNKNOWN_CONTEXT                                0x0003

#define EM_FILE_FORMAT_PEM                                0x0001
#define EM_FILE_FORMAT_DEL_VAR                            0x0002
#define EM_FILE_FORMAT_EXP_VAR                            0x0003

#define ENROLL_VAR_DELETE_OPTION_OFFSET                   0x9000
#define ENROLL_VAR_EXPORT_OPTION_OFFSET                   0xA000
#define ENROLL_VAR_OPTION_CMD_MASK                        0xF000
#define ENROLL_VAR_OPTION_INDEX_MASK                      0x0FFF

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
  );

/**
  Initialize the configuration form to SCU Security\Link.

  @param[in]  Private             Pointer to the driver private data.

  @retval EFI_SUCCESS             The configuration form is initialized.
  @retval EFI_INVALID_PARAMETER   HiiHandle is NULL.
  @retval EFI_OUT_OF_RESOURCES    Failed to allocate memory.

**/
EFI_STATUS
TlsCertificateManagementFormInit (
  OUT  VOID                            **CallbackInfo
  );

#endif
