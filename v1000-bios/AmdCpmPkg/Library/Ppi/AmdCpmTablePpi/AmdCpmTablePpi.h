/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _AMD_CPM_TABLE_PPI_H_
#define _AMD_CPM_TABLE_PPI_H_

#include <AmdCpmBase.h>

//
// GUID definition
//
#define AMD_CPM_TABLE_PPI_GUID \
  { 0xd71cf893, 0xa8b5, 0x49d3, 0xa2, 0x1b, 0x31, 0xe2, 0xf5, 0xc4, 0xa7, 0x47 }

extern EFI_GUID gAmdCpmTablePpiGuid;

/// AMD CPM TABLE PPI Definition

typedef struct _AMD_CPM_TABLE_PPI {
  UINTN                                   Revision;               ///< Revision Number
  AMD_CPM_MAIN_TABLE                      *MainTablePtr;          ///< Main Table of CPM
  AMD_CPM_CHIP_ID                         ChipId;                 ///< Id of SB Chip
  AMD_CPM_COMMON_FUNCTION                 CommonFunction;         ///< Private Common Functions
  AMD_CPM_PEIM_PUBLIC_FUNCTION            PeimPublicFunction;     ///< Public Function of PPI
} AMD_CPM_TABLE_PPI;

// Current PPI revision
#define AMD_CPM_TABLE_PPI_REV  0x00

#endif
