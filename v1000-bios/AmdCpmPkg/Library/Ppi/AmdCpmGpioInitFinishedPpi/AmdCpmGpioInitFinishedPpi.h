/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _AMD_CPM_GPIO_INIT_FINISHED_PPI_H_
#define _AMD_CPM_GPIO_INIT_FINISHED_PPI_H_

#include <AmdCpmBase.h>

//
// GUID definition
//
#define AMD_CPM_GPIO_INIT_FINISHED_PPI_GUID \
  { 0xfd4e6776, 0x2482, 0x48ba, 0xbf, 0x3e, 0x27, 0x65, 0xf2, 0xd6, 0x7d, 0xcb }

extern EFI_GUID gAmdCpmGpioInitFinishedPpiGuid;

/// AMD CPM GPIO Init Finished PPI Definition

typedef struct _AMD_CPM_GPIO_INIT_FINISHED_PPI {
  UINTN                                   Revision;               ///< Revision Number
} AMD_CPM_GPIO_INIT_FINISHED_PPI;

// Current PPI Revision
#define AMD_CPM_GPIO_INIT_FINISHED_PPI_REV  0x00

#endif
