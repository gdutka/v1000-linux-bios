/* $NoKeywords:$ */
/**
 * @file
 *
 * NBIO Common Library. This library is a home for common functions and routines
 * meant to reduce code size and encourage code sharing.
 *
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/
#ifndef _NBIOUTILLIB_H_
#define _NBIOUTILLIB_H_

#include <GnbDxio.h>

#define NONPCI_BARSIZE_512MB                0x20000000ul            ///< Size of 512MB BAR
#define NONPCI_BARSIZE_256MB                0x10000000ul            ///< Size of 256MB BAR                                                             ///
#define NONPCI_BARSIZE_1MB                  0x100000ul              ///< Size of 1MB BAR
#define NONPCI_BARSIZE_512KB                0x7D000ul               ///< Size of 512KB BAR
#define NONPCI_BARSIZE_4KB                  0x1000                  ///< Size of 4KB BAR

VOID
NonPCIBarInit (
  IN       GNB_HANDLE   *GnbHandle,
  IN       UINT32       MmioBarLow,
  IN       UINT32       MmioBarHigh,
  IN       UINT32       MemorySize,
  IN       BOOLEAN      Enable,
  IN       BOOLEAN      LockSettings,
  IN       BOOLEAN      Above4G
  );

#endif /* _NBIOUTILLIB_H_ */

