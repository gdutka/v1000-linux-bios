/** @file
  Header file for boot manager driver

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

/**
  The platform boot manager reference implement

Copyright (c) 2004 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EFI_BOOT_MANAGER_H_
#define _EFI_BOOT_MANAGER_H_

#include <IndustryStandard/Pci22.h>

#include <Protocol/HiiConfigAccess.h>
#include <Protocol/FormBrowser2.h>
#include <Protocol/HiiConfigRouting.h>
#include <Protocol/H2OBdsServices.h>
#include <Protocol/LoadFile.h>
#include <Protocol/H2OFormBrowser.h>

#include <Guid/MdeModuleHii.h>
#include <Guid/BdsHii.h>
#include <Guid/DebugMask.h>
#include <Guid/InsydeModuleHii.h>
#include <Guid/H2OBdsCheckPoint.h>
#include <Guid/GlobalVariable.h>

#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/HiiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/GenericBdsLib.h>
#include <Library/GenericUtilityLib.h>
#include <Library/H2OCpLib.h>
#include <Library/VariableLib.h>

#define BOOT_MANAGER_FORM_ID     0x1000

#define LABEL_BOOT_OPTION        0x00
#define LABEL_BOOT_OPTION_END    0x01

#define BOOT_MANAGER_UPDATE_BOOT_LIST_QUESTION_ID     0xF000

//
// These are the VFR compiler generated data representing our VFR data.
//
extern UINT8 BootManagerVfrBin[];
extern UINT8 BootManagerStrings[];

#define BOOT_MANAGER_CALLBACK_DATA_SIGNATURE  SIGNATURE_32 ('B', 'M', 'C', 'B')

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
} BOOT_MANAGER_CALLBACK_DATA;

/**
  This call back function is registered with Boot Manager formset.
  When user selects a boot option, this call back function will
  be triggered. The boot option is saved for later processing.


  @param This            Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Action          Specifies the type of action taken by the browser.
  @param QuestionId      A unique value which is sent to the original exporting driver
                         so that it can identify the type of data to expect.
  @param Type            The type of value for the question.
  @param Value           A pointer to the data being sent to the original exporting driver.
  @param ActionRequest   On return, points to the action requested by the callback function.

  @retval  EFI_SUCCESS           The callback successfully handled the action.
  @retval  EFI_INVALID_PARAMETER The setup browser call this function with invalid parameters.

**/
EFI_STATUS
EFIAPI
BootManagerCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  );

/**

  Registers HII packages for the Boot Manger to HII Database.
  It also registers the browser call back function.

  @retval  EFI_SUCCESS           HII packages for the Boot Manager were registered successfully.
  @retval  EFI_OUT_OF_RESOURCES  HII packages for the Boot Manager failed to be registered.

**/
EFI_STATUS
InitializeBootManager (
  VOID
  );

EFI_STATUS
EFIAPI
BootManagerFakeExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  );

EFI_STATUS
EFIAPI
BootManagerFakeRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  );

EFI_STATUS
EFIAPI
BootManagerLoadFile (
  IN EFI_LOAD_FILE_PROTOCOL                  *This,
  IN EFI_DEVICE_PATH_PROTOCOL                *FilePath,
  IN BOOLEAN                                 BootPolicy,
  IN OUT UINTN                               *BufferSize,
  IN VOID                                    *Buffer OPTIONAL
  );

VOID
UpdateBootOptionListIntoHiiForm (
  VOID
  );

EFI_STATUS
UpdateBootOptionList (
  VOID
  );

VOID
InternalCallBootManagerBefore (
  VOID
  );

VOID
InternalCallBootManagerAfter (
  VOID
  );

#endif
