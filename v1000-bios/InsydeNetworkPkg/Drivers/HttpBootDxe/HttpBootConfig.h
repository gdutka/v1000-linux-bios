/** @file
  HTTP boot configuration related functions.

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
/** @file
  The header file of functions for configuring or getting the parameters
  relating to HTTP Boot.

Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _HTTP_BOOT_CONFIG_H_
#define _HTTP_BOOT_CONFIG_H_


#include "HttpBootConfigNVDataStruc.h"
#include <Protocol/FormBrowserEx2.h>
#include <Protocol/H2OFormBrowser.h>

typedef struct _HTTP_BOOT_FORM_CALLBACK_INFO   HTTP_BOOT_FORM_CALLBACK_INFO;

extern   UINT8                            HttpBootDxeStrings[];
/// This VFR is for LinkForm, install in SCU\Boot\Link
extern   UINT8                            HttpBootConfigLinkFormVfrBin[];
/// This VFR is for original HTTP URL manage form, install in FrontPage\DeviceManager.
extern   UINT8                            HttpBootConfigVfrBin[];

#pragma pack()

#define HTTP_BOOT_FORM_SUPPORTED_DEVICES_MAX    100
#define HTTP_BOOT_DEVICE_MAX                    100
#define HTTP_BOOT_FORM_CALLBACK_INFO_SIGNATURE  SIGNATURE_32 ('H', 'B', 'f', 'c')

#define HTTP_BOOT_FORM_CALLBACK_INFO_FROM_CONFIG_ACCESS(Callback) \
  CR ( \
  Callback, \
  HTTP_BOOT_FORM_CALLBACK_INFO, \
  ConfigAccess, \
  HTTP_BOOT_FORM_CALLBACK_INFO_SIGNATURE \
  )

typedef struct {
  UINT16               StringId;
  UINT16               HiiDevPathStringId;
} HTTP_DEVICE_ENTRY;

struct _HTTP_BOOT_FORM_CALLBACK_INFO {
  UINT32                           Signature;
  BOOLEAN                          IsRootForm;
  BOOLEAN                          Initialized;
  EFI_HANDLE                       ChildHandle;
  EFI_GUID                         *FormGuid;
  EFI_DEVICE_PATH_PROTOCOL         *HiiVendorDevicePath;
  EFI_HII_HANDLE                   RegisteredHandle;
  EFI_HII_CONFIG_ACCESS_PROTOCOL   ConfigAccess;
  HTTP_BOOT_CONFIG_IFR_NVDATA      HttpBootNvData;
  EDKII_FORM_BROWSER_EXTENSION2_PROTOCOL  *FormBrowserEx2;

  HTTP_DEVICE_ENTRY                *HttpDeviceList;
  UINTN                            StringIdCnt;
  EFI_HANDLE                       UndiHandle;
};



/**
  Initialize the configuration form.

  @param[in]  Private             Pointer to the driver private data.

  @retval EFI_SUCCESS             The configuration form is initialized.
  @retval EFI_OUT_OF_RESOURCES    Failed to allocate memory.

**/
EFI_STATUS
HttpBootConfigFormInit (
  IN HTTP_BOOT_PRIVATE_DATA     *Private
  );

/**
  Unload the configuration form, this includes: delete all the configuration
  entries, uninstall the form callback protocol, and free the resources used.
  The form will only be unload completely when both IP4 and IP6 stack are stopped.

  @param[in]  Private             Pointer to the driver private data.

  @retval EFI_SUCCESS             The configuration form is unloaded.
  @retval Others                  Failed to unload the form.

**/
EFI_STATUS
HttpBootConfigFormUnload (
  IN HTTP_BOOT_PRIVATE_DATA     *Private
  );

/**
  Initialize the link configuration form for Link.
  This form will be install to SCU\Boot\Link

  @param[out] Private             Initilailze the driver private data for HII.

  @retval EFI_SUCCESS             The configuration form is initialized.
  @retval EFI_OUT_OF_RESOURCES    Failed to allocate memory.

**/
EFI_STATUS
HttpBootConfigLinkFormInit (
  OUT HTTP_BOOT_FORM_CALLBACK_INFO    *CallbackInfo
  );

/**
  Unload the link configuration form.

  @param[in]  Private             Pointer to the driver private data.

  @retval EFI_SUCCESS             The configuration form is unloaded.
  @retval Others                  Failed to unload the form.

**/
EFI_STATUS
HttpBootConfigLinkFormUnload (
  IN   HTTP_BOOT_FORM_CALLBACK_INFO    *CallbackInfo
  );

#endif
