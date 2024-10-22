/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _AMD_CPM_NV_DATA_PROTOCOL_H_
#define _AMD_CPM_NV_DATA_PROTOCOL_H_

#include <AmdCpmBase.h>

//
// GUID definition
//
#define AMD_CPM_NV_DATA_PROTOCOL_GUID \
  { 0xdb4a79ac, 0x87ba, 0x4625, 0x6a, 0x9e, 0xfe, 0xbf, 0x9d, 0x6d, 0x95, 0xeb }

extern EFI_GUID gAmdCpmNvDataProtocolGuid;


/// DXE Protocol Structure
typedef struct _AMD_CPM_NV_DATA_PROTOCOL {          // See the Forward Declaration above
  UINTN                   Revision;                 ///< Protocol Revision
  AMD_CPM_NV_DATA_STRUCT  *NvDataPtr;               ///< Pointer to NV Data Buffer
} AMD_CPM_NV_DATA_PROTOCOL;

// Current Protocol Revision
#define AMD_CPM_NV_DATA_REV  0x00

#endif
