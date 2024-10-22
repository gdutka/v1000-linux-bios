/*****************************************************************************
 *
 * Copyright (C) 2014-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <AmdCpmBase.h>
#include <AmdMemoryInfoHob.h>

EFI_GUID gAmdMemoryInfoHobGuid = AMD_MEMORY_INFO_HOB_GUID;

EFI_GUID_STRING (&gAmdMemoryInfoHobGuid, "AmdMemoryInfoHob", "AMD Memory Info HOB");
