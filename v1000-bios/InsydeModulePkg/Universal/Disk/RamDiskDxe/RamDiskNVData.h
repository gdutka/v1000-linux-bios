/** @file

;******************************************************************************
;* Copyright (c) 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/** @file
  Header file for NV data structure definition.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  (C) Copyright 2016 Hewlett Packard Enterprise Development LP<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _RAM_DISK_NVDATA_H_
#define _RAM_DISK_NVDATA_H_

#include <Guid/HiiPlatformSetupFormset.h>


#define MAIN_FORM_ID                        0x1000
#define MAIN_GOTO_FILE_EXPLORER_ID          0x1001
#define MAIN_REMOVE_RD_QUESTION_ID          0x1002
#define MAIN_LABEL_LIST_START               0x1003
#define MAIN_LABEL_LIST_END                 0x1004
#define MAIN_CHECKBOX_QUESTION_ID_START     0x1100

#define FORM_FILE_LIST_ID                   0x1F00
#define FORM_FILE_LIST_END_ID               0x1F01

#define CREATE_RAW_RAM_DISK_FORM_ID         0x2000
#define CREATE_RAW_SIZE_QUESTION_ID         0x2001
#define CREATE_RAW_SUBMIT_QUESTION_ID       0x2002
#define CREATE_RAW_DISCARD_QUESTION_ID      0x2003
#define CREATE_RAW_MEMORY_TYPE_QUESTION_ID  0x2004
#define CREATE_RAW_FROM_FILE_QUESTION_ID    0x2F00

#define RAM_DISK_BOOT_SERVICE_DATA_MEMORY   0x00
#define RAM_DISK_RESERVED_MEMORY            0x01
#define RAM_DISK_MEMORY_TYPE_MAX            0x02


#define KEY_FILE_EXPLORER                   0x3000
#define FILE_EXPLORER_RETRIEVE_ID           0x8000


#define H2O_RAM_DISK_FORMSET_GUID \
{ \
0xe817decb, 0x5bd5, 0x45a1, 0x95, 0x20, 0x83, 0xd3, 0x19, 0x1f, 0x96, 0xfc\
}


typedef struct {
  //
  // The size of the RAM disk to be created.
  //
  UINT64                          Size;
  //
  // Selected RAM Disk Memory Type
  //
  UINT8                           MemType;
} RAM_DISK_CONFIGURATION;

#endif
