/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */
#include <AmdCpmBase.h>

#include CPM_PROTOCOL_DEFINITION (AmdCpmTableProtocol)

EFI_GUID  gAmdCpmTableProtocolGuid      = AMD_CPM_TABLE_PROTOCOL_GUID;
EFI_GUID  gAmdCpmTableSmmProtocolGuid   = AMD_CPM_TABLE_SMM_PROTOCOL_GUID;

EFI_GUID_STRING (gAmdCpmTableProtocolGuid, "AmdCpmTableProtocol", "AMD CPM TABLE Protocol");
EFI_GUID_STRING (gAmdCpmTableSmmProtocolGuid, "AmdCpmTableSmmProtocol", "AMD CPM TABLE SMM Protocol");
