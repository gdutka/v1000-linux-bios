/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <AmdCpmBase.h>

#include CPM_PROTOCOL_DEFINITION (AmdCpmOemTableProtocol)

EFI_GUID  gAmdCpmOemTableProtocolGuid = AMD_CPM_OEM_TABLE_PROTOCOL_GUID;

EFI_GUID_STRING (gAmdCpmOemTableProtocolGuid, "AmdCpmOemTableProtocol", "AMD CPM Oem TABLE Protocol");
