/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 * ***************************************************************************
 */
#ifndef _AMD_CPM_BOOT_TIME_RECORD_HOB_H_
#define _AMD_CPM_BOOT_TIME_RECORD_HOB_H_

#include <AmdCpmBase.h>

#define AMD_BOOT_TIME_RECORD_HOB_GUID \
  { 0xd0800033, 0xc743, 0x4565, 0xa1, 0xe3, 0x11, 0x1a, 0x37, 0x1f, 0x2a, 0x4b }

extern EFI_GUID gAmdBootTimeRecordHobGuid;

#endif // _AMD_CPM_BOOT_TIME_RECORD_HOB_H_
