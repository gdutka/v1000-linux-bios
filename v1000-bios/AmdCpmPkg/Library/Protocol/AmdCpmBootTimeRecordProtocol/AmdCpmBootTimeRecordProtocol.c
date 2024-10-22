/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <AmdCpmBase.h>

#include CPM_PROTOCOL_DEFINITION (AmdCpmBootTimeRecordProtocol)

EFI_GUID  gBootTimeRecordProtocolGuid = AMD_BOOT_TIME_RECORD_PROTOCOL_GUID;

EFI_GUID_STRING (&gBootTimeRecordProtocolGuid, "AmdCpmBootTimeRecordProtocolGuid", "AMD CPM Boot Time Record Protocol Guid");

