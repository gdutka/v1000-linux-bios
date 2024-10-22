/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */
#ifndef _AMD_CPM_OEM_TABLE_PROTOCOL_H_
#define _AMD_CPM_OEM_TABLE_PROTOCOL_H_

#include <AmdCpmBase.h>

//
// GUID definition
//
#define AMD_CPM_OEM_TABLE_PROTOCOL_GUID \
  { 0x38d91956, 0xd5f1, 0x40fc, 0x8e, 0x61, 0xcb, 0x5e, 0x1a, 0xa9, 0x5c, 0xb7 }

extern EFI_GUID gAmdCpmOemTableProtocolGuid;


/// DXE Protocol Structure
typedef struct _AMD_CPM_OEM_TABLE_PROTOCOL {                      //See the Forward Declaration above
  UINTN                                   Revision;               ///< Protocol Revision
  VOID                                    *TableList;             ///< Pointer to CPM Definition Table list which
                                                                  ///<   is defined in AmdCpmOemInitDxe Driver.
} AMD_CPM_OEM_TABLE_PROTOCOL;

// Current Protocol Revision
#define AMD_CPM_OEM_TABLE_PROTOCOL_REV      0x00

#endif
