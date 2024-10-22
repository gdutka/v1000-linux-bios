/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <AmdCpmBase.h>

#include CPM_PROTOCOL_DEFINITION (AmdCpmAllPciIoProtocolsInstalled)

EFI_GUID  gAmdCpmAllPciIoProtocolsInstalledProtocolGuid = AMD_CPM_ALL_PCI_IO_PROTOCOLS_INSTALLED_PROTOCOL_GUID;

EFI_GUID_STRING (&gAmdCpmAllPciIoProtocolsInstalledProtocolGuid, "AmdCpmAllPciIoProtocolsInstalled", "AMD CPM All Pci Io Protocols Installed Protocol");
