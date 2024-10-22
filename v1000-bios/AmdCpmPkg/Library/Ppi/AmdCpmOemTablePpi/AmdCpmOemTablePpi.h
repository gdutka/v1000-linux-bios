/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _AMD_CPM_OEM_TABLE_PPI_H_
#define _AMD_CPM_OEM_TABLE_PPI_H_

#include <AmdCpmBase.h>

//
// GUID definition
//
#define AMD_CPM_OEM_TABLE_PPI_GUID \
  { 0xfd1fe103, 0x40f1, 0x459c, 0x98, 0x3e, 0x11, 0x0b, 0x69, 0x5e, 0xd1, 0x1a }

extern EFI_GUID gAmdCpmOemTablePpiGuid;

/// AMD CPM OEM TABLE PPI Definition

typedef struct _AMD_CPM_OEM_TABLE_PPI {
  UINTN               Revision;               ///< Revision Number
  UINT16              PlatformId;             ///< Current Platform Id
  VOID                *TableList;             ///< The Point of CPM Definition Table List
} AMD_CPM_OEM_TABLE_PPI;

// Current PPI revision
#define AMD_CPM_OEM_TABLE_PPI_REV   0x00

#endif
