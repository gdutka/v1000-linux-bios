/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _AMD_CPM_ALL_PCI_IO_PROTOCOLS_INSTALLED_H_
#define _AMD_CPM_ALL_PCI_IO_PROTOCOLS_INSTALLED_H_

//
// GUID definition
//
#define AMD_CPM_ALL_PCI_IO_PROTOCOLS_INSTALLED_PROTOCOL_GUID \
  { 0x676D7012, 0x139B, 0x485A, 0x96, 0xF1, 0x98, 0x6F, 0xC4, 0x8A, 0x86, 0x4B }

extern EFI_GUID gAmdCpmAllPciIoProtocolsInstalledProtocolGuid;

/// DXE Protocol Structure
typedef struct _AMD_CPM_ALL_PCI_IO_PROTOCOLS_INSTALLED_PROTOCOL {

  VOID *Buffer;                 ///< Pointer to a buffer

} AMD_CPM_ALL_PCI_IO_PROTOCOLS_INSTALLED_PROTOCOL;


#endif
