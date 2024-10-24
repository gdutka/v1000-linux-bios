/** @file
  HTTP boot configuration related functions.

;******************************************************************************
;* Copyright (c) 2019 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
/** @file
  Define NVData structures used by the HTTP Boot configuration component.

Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _HTTP_BOOT_NVDATA_STRUC_H_
#define _HTTP_BOOT_NVDATA_STRUC_H_

#include <Guid/HttpBootConfigHii.h>

#define HTTP_BOOT_IP_VERSION_4       0
#define HTTP_BOOT_IP_VERSION_6       1

//
// Macros used for an IPv4 or an IPv6 address.
//
#define URI_STR_MIN_SIZE             0
#define URI_STR_MAX_SIZE             255

#define DESCRIPTION_STR_MIN_SIZE     6
#define DESCRIPTION_STR_MAX_SIZE     75

#define CONFIGURATION_VARSTORE_ID    0x1234

#define FORMID_MAIN_FORM             1

#define KEY_INITIATOR_URI            0x101

#define HTTP_BOOT_DEFAULT_DESCRIPTION_STR   L"UEFI HTTP"


#define BOOT_HTTP_URI_DEVICES_FORM_ID         0x53E
#define BOOT_HTTP_URI_MANAGE_FORM_ID          0x53F

#define HTTP_BOOT_ENTRY_MANAGER               0x99FE
#define HTTP_BOOT_KEY_INITIATOR_URI           0x99FF
#define HTTP_BOOT_DEVICE_LIST_START_ID        0x9900
#define HTTP_BOOT_DEVICE_LIST_END_ID          0x997F
#define HTTP_BOOT_URL_LIST_START_ID           0x9980
#define HTTP_BOOT_URL_LIST_END_ID             0x99FD

#pragma pack(1)
typedef struct _HTTP_BOOT_CONFIG_IFR_NVDATA {
  UINT8     IpVersion;
  UINT8     Padding;
  CHAR16    Description[DESCRIPTION_STR_MAX_SIZE];
  CHAR16    Uri[URI_STR_MAX_SIZE];
} HTTP_BOOT_CONFIG_IFR_NVDATA;
#pragma pack()


#endif
