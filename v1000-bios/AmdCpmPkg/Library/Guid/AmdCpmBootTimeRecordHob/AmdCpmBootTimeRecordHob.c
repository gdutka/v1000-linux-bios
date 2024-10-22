/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 * ***************************************************************************
 */
#include <AmdCpmBase.h>

#include CPM_GUID_DEFINITION (AmdCpmBootTimeRecordHob)

EFI_GUID gAmdBootTimeRecordHobGuid = AMD_BOOT_TIME_RECORD_HOB_GUID;

EFI_GUID_STRING (&gAmdBootTimeRecordHobGuid, "AmdCpmBootTimeRecordHob", "AMD CPM Boot Time Record HOB");

