/** @file
  Header file for secure boot manager driver

;******************************************************************************
;* Copyright (c) 2012 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SECURE_BOOT_MGR_H_
#define _SECURE_BOOT_MGR_H_

#include <Protocol/FormBrowserEx.h>
#include <Protocol/DevicePath.h>
#include <Protocol/Hash.h>
#include <Protocol/BlockIo.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/FormBrowser2.h>
#include <Protocol/H2ODialog.h>
#include <Protocol/HiiConfigRouting.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/LoadFile.h>
#include <Protocol/NonVolatileVariable.h>

#include <Guid/MdeModuleHii.h>
#include <Guid/AdmiSecureBoot.h>
#include <Guid/DebugMask.h>
#include <Guid/ImageAuthentication.h>
#include <Guid/InsydeModuleHii.h>
#include <Guid/H2OBdsCheckPoint.h>
#include <Guid/ZeroGuid.h>
#include <Guid/AuthenticatedVariableFormat.h>

#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/HiiLib.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Library/GenericBdsLib.h>
#include <Library/GenericUtilityLib.h>
#include <Library/H2OCpLib.h>
#include "SecureBootFormGuid.h"

#define SECURE_BOOT_CALLBACK_DATA_SIGNATURE  SIGNATURE_32 ('S', 'B', 'C', 'B')

#define SECURE_BOOT_CALLBACK_DATA_FROM_THIS(a) \
  CR (a, \
      SECURE_BOOT_MANAGER_CALLBACK_DATA, \
      ConfigAccess, \
      SECURE_BOOT_CALLBACK_DATA_SIGNATURE \
      )

#define SECURE_BOOT_DATA_NAME    L"SecureBootData"

typedef enum {
  PkForm = 0,
  KekForm,
  DbForm,
  DbxForm,
  HashImageForm,
  SecureBootMgrStateMax
} SECURE_BOOT_MANAGER_STATE;

typedef struct {
  UINTN                           Signature;

  //
  // HII relative handles
  //
  EFI_HII_HANDLE                  HiiHandle;
  EFI_HANDLE                      DriverHandle;

  //
  // Produced protocols
  //
  EFI_HII_CONFIG_ACCESS_PROTOCOL   ConfigAccess;

  H2O_DIALOG_PROTOCOL              *H2ODialog;
  SECURE_BOOT_NV_DATA              SecureBootData;
  SECURE_BOOT_NV_DATA              BackupSecureBootData;
  UINTN                            EmDisplayContext;
  UINTN                            EmFileFormat;
  EFI_GUID                         EmEnrollSigOwnerGuid;

  SECURE_BOOT_MANAGER_STATE        SecureBootMgrState;
} SECURE_BOOT_MANAGER_CALLBACK_DATA;


EFI_STATUS
EFIAPI
SecureBootCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  );

EFI_STATUS
EFIAPI
SecureBootExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  );

EFI_STATUS
EFIAPI
SecureBootRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  );

UINT8
EFIAPI
SmmSecureBootCall (
  IN     UINT8            *InPutBuff,       // rcx
  IN     UINTN            DataSize,         // rdx
  IN     UINT8            SubFunNum,        // r8
  IN     UINT16           SmiPort           // r9
  );

EFI_STATUS
SmmSecureBootCallWithRuntimeMemory (
  IN     UINT8            *InputBuff,
  IN     UINTN            DataSize,
  IN     UINT8            SubFunNum
  );

EFI_STATUS
EFIAPI
SecureBootMgrLoadFile (
  IN EFI_LOAD_FILE_PROTOCOL           *This,
  IN EFI_DEVICE_PATH_PROTOCOL         *FilePath,
  IN BOOLEAN                          BootPolicy,
  IN OUT UINTN                        *BufferSize,
  IN VOID                             *Buffer OPTIONAL
  );

EFI_STATUS
UpdatePkVariable (
  IN UINT8                             *VarData,
  IN UINTN                             VarDataSize
  );

EFI_STATUS
UpdateKekVariable (
  IN UINT8                             *VarData,
  IN UINTN                             VarDataSize,
  IN UINTN                             UpdateType
  );

EFI_STATUS
UpdateDbVariable (
  IN UINT8                             *VarData,
  IN UINTN                             VarDataSize,
  IN UINTN                             UpdateType
  );

EFI_STATUS
UpdateDbxVariable (
  IN UINT8                             *VarData,
  IN UINTN                             VarDataSize,
  IN UINTN                             UpdateType
  );

extern UINT8                              SecureBootMgrVfrBin[];
extern EFI_GUID                           mSecureBootGuid;
extern SECURE_BOOT_MANAGER_CALLBACK_DATA  mSecureBootPrivate;
extern LIST_ENTRY                         mSecureBootHashLinkList;
#endif
