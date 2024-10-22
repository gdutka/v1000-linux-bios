/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _AMD_CPM_OVERRIDE_TABLE_PPI_H_
#define _AMD_CPM_OVERRIDE_TABLE_PPI_H_

#include <AmdCpmBase.h>

//
// GUID definition
//
#define AMD_CPM_OVERRIDE_TABLE_PPI_GUID \
  { 0x25d79807, 0x7b43, 0x4767, 0xb9, 0xfa, 0x72, 0x62, 0xa7, 0x4f, 0x85, 0xc3 }

extern EFI_GUID gAmdCpmOverrideTablePpiGuid;

/// AMD CPM Override Table PPI Definition

typedef struct _AMD_CPM_OVERRIDE_TABLE_PPI {
  UINTN                                   Revision;               ///< Revision Number
} AMD_CPM_OVERRIDE_TABLE_PPI;

// Current PPI Revision
#define AMD_CPM_OVERRIDE_TABLE_PPI_REV   0x00

#endif
