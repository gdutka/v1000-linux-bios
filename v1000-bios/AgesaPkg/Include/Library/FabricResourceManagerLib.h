/* $NoKeywords:$ */
/**
 * @file
 *
 * Low-level Fabric MMIO map manager Services library
 *
 * Contains interface to the family specific fabric MMIO map manager library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _FABRIC_RESOURCE_MANAGER_LIB_H_
#define _FABRIC_RESOURCE_MANAGER_LIB_H_

#include "Porting.h"
#include "AMD.h"
#include <Uefi/UefiBaseType.h>
#include <FabricResourceManagerCmn.h>
#pragma pack (push, 1)

/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */

/// MMIO Region
typedef struct _FABRIC_MMIO_REGION {
  UINT64  BaseNonPci;            ///< Base address of non-discoverable devices
  UINT64  SizeNonPci;            ///< Total size of non-discoverable devices
  UINT64  UsedSizeNonPci;        ///< Already used size of non-discoverable devices
  UINT64  BaseNonPrefetch;       ///< Base address of non prefetchable
  UINT64  SizeNonPrefetch;       ///< Total size of non prefetchable
  UINT64  UsedSizeNonPrefetch;   ///< Already used size of non prefetchable
  UINT64  BasePrefetch;          ///< Base address of prefetchable
  UINT64  SizePrefetch;          ///< Total size of prefetchable
  UINT64  UsedSizePrefetch;      ///< Already used size of prefetchable
  UINT64  AlignNonPrefetch;      ///< Alignment bit map. For example, 0xFFFFF means 1MB alignment
  UINT64  AlignPrefetch;         ///< Alignment bit. For example, 0xFFFFF means 1MB alignment
} FABRIC_MMIO_REGION;

/// MMIO Manager
typedef struct _FABRIC_MMIO_MANAGER {
  FABRIC_MMIO_REGION MmioRegionAbove4G[MAX_SOCKETS_SUPPORTED][MAX_RBS_PER_SOCKET];     ///< MMIO which is above 4G for Socket[0~1], RootBridge[0~3]
  FABRIC_MMIO_REGION MmioRegionBelow4G[MAX_SOCKETS_SUPPORTED][MAX_RBS_PER_SOCKET];     ///< MMIO which is below 4G for Socket[0~1], RootBridge[0~3]
  BOOLEAN            AllocateMmioAbove4GOnThisRb[MAX_SOCKETS_SUPPORTED][MAX_RBS_PER_SOCKET]; ///< Allocate MMIO which is above 4G on this RootBridge?
  BOOLEAN            AllocateMmioBelow4GOnThisRb[MAX_SOCKETS_SUPPORTED][MAX_RBS_PER_SOCKET]; ///< Allocate MMIO which is below 4G on this RootBridge?
  BOOLEAN            PrimaryRbHas2ndMmioBelow4G;                                             ///< Indicate if RootBridge 0 has a 2nd MMIO which is below 4G
  BOOLEAN            ReportToGcd;                                                            ///< TRUE - Need to report GCD if user call FabricAllocateMmio
  UINT8              PrimaryRb2ndMmioPairBelow4G;                                            ///< Since AllocateMmioOnThisRootBridge[i][j] is FALSE, use this one for RootBridge0's 2nd MMIO, RootBridge02ndMmioPair = (i << 4) | j
  UINT64             ImageHandle;                                                            ///< AGESA would report Non-Pci MMIO region to GCD, so save ImageHandle here
} FABRIC_MMIO_MANAGER;

/// IO Region
typedef struct _FABRIC_IO_REGION {
  UINT32  IoBase;                ///< IO base address
  UINT32  IoSize;                ///< IO size
  UINT32  IoUsed;                ///< IO used
} FABRIC_IO_REGION;

/// IO Manager
typedef struct _FABRIC_IO_MANAGER {
  BOOLEAN          GlobalCtrl;                                                      ///< FALSE - forbid FabricAllocateIo
  FABRIC_IO_REGION IoRegion[MAX_SOCKETS_SUPPORTED][MAX_RBS_PER_SOCKET];             ///< IO ragion for Socket[0~1], RootBridge[0~3]
} FABRIC_IO_MANAGER;

/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */

EFI_STATUS
FabricAllocateMmio (
  IN OUT   UINT64 *BaseAddress,
  IN OUT   UINT64 *Length,
  IN       UINT64                 Alignment,
  IN       FABRIC_TARGET          Target,
  IN OUT   FABRIC_MMIO_ATTRIBUTE *Attributes
  );

EFI_STATUS
FabricAllocateIo (
  IN OUT   UINT32 *BaseAddress,
  IN OUT   UINT32 *Length,
  IN       FABRIC_TARGET          Target
  );

EFI_STATUS
FabricGetAvailableResource (
  IN       FABRIC_RESOURCE_FOR_EACH_RB    *ResourceSizeForEachRb  ///< Get available DF resource size for each RootBridge
  );

EFI_STATUS
FabricReallocateResourceForEachRb (
  IN       FABRIC_RESOURCE_FOR_EACH_RB    *ResourceSizeForEachRb, ///< User request
  IN       FABRIC_ADDR_SPACE_SIZE         *SpaceStatus            ///< Report current status
  );

EFI_STATUS
FabricResourceRestoreDefault (
  );

EFI_STATUS
FabricEnableVgaMmio (
  IN       FABRIC_TARGET          Target
  );

#pragma pack (pop)
#endif // _FABRIC_RESOURCE_MANAGER_LIB_H_

