/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <AmdCpmBase.h>

#include CPM_PROTOCOL_DEFINITION (AmdCpmNvDataProtocol)

EFI_GUID  gAmdCpmNvDataProtocolGuid = AMD_CPM_NV_DATA_PROTOCOL_GUID;

EFI_GUID_STRING (gAmdCpmNvDataProtocolGuid, "AmdCpmNvDataProtocol", "AMD PMODULE NV Data Protocol");
