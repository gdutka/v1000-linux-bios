/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _PCIE_TOPOLOGY_TABLE_PPI_H_
#define _PCIE_TOPOLOGY_TABLE_PPI_H_

#include <AmdCpmBase.h>

//
// GUID definition
//
#define PCIE_TOPOLOGY_TABLE_PPI_GUID \
  { 0x433f54b5, 0x50ed, 0x48a8, 0xa0, 0xe7, 0xa7, 0x85, 0x0c, 0x4b, 0x32, 0xfe }

extern EFI_GUID gPcieTopologyTablePpiGuid;

/// Pcie Topology Table PPI Definition

typedef struct _PCIE_TOPOLOGY_TABLE_PPI {
  UINTN                                   Revision;               ///< Revision Number
} PCIE_TOPOLOGY_TABLE_PPI;

// Current PPI Revision
#define PCIE_TOPOLOGY_TABLE_PPI_REV   0x00

#endif
