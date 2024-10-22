/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _AMD_CPM_TABLE_HOB_PPI_H_
#define _AMD_CPM_TABLE_HOB_PPI_H_

#include <AmdCpmBase.h>

//
// GUID definition
//
#define AMD_CPM_TABLE_HOB_PPI_GUID \
  { 0xc02c596b, 0xcd04, 0x486e, 0x86, 0x66, 0x30, 0x3e, 0x55, 0x67, 0xc0, 0x48 }

extern EFI_GUID gAmdCpmTableHobPpiGuid;

/// AMD CPM TABLE PPI Definition

typedef struct _AMD_CPM_TABLE_HOB_PPI {
  UINTN                                   Revision;               ///< Revision Number
} AMD_CPM_TABLE_HOB_PPI;

// Current PPI revision
#define AMD_CPM_TABLE_HOB_PPI_REV  0x00

#endif
