/* $NoKeywords:$ */
/**
 * @file
 *
 * Base Fabric MMIO map manager Lib implementation for DF3
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _FABRIC_MMIO_MAP_MANAGER_3_LIB_H_
#define _FABRIC_MMIO_MAP_MANAGER_3_LIB_H_

#include "Porting.h"
#include <Library/FabricResourceManagerLib.h>

#pragma pack (push, 1)

/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */
#define VH_MMIO_MIN_SIZE                    0x10000ul
#define VH_NON_PCI_MMIO_ALIGN_MASK          0xFFFFull  // MMIO for non-PCI devices should be 16 bit aligned
#define MMIO_BASE_ADDRESS_REG_0             0x200      // MMIO base address register
#define MMIO_LIMIT_ADDRESS_REG_0            0x204      // MMIO limit address register
#define MMIO_ADDRESS_CONTROL_REG_0          0x208      // MMIO control register
#define MMIO_MIN_NON_PCI_SIZE               0x500000   // 5MB is the minimum size of NonPci MMIO pool
#define MMIO_MIN_NON_PCI_SIZE_ABOVE4G       0x20200000 // 514MB is the minimum size of NonPci MMIO pool
#define VH_NON_PCI_MMIO_ALIGN_MASK_ABOVE4G  0xFFFFFFFull // MMIO for non-PCI devices should be 256MB aligned
#define CFG_ADDR_MAP_REG0                   0xA0       // Configuration Address Map register
#define X86IO_BASE_ADDRESS_REG0             0xC0       // IO base address register
#define X86IO_LIMIT_ADDRESS_REG0            0xC4       // IO limit address register
#define DF_IO_LIMIT                         0x2000000  // IO Limit
#define X86IO_LIMIT                         0x10000    // X86 IO Limit
#define X86_LEGACY_IO_SIZE                  0x1000     // IO size which is reserved for legacy devices
#define VH_IO_SIZE_MASK                     0xFFFFF000ull

#define VH_MAX_SOCKET               2
#define VH_MAX_RB_PER_SOCKET        4
#define VH_MAX_SYSTEM_RB_COUNT      (VH_MAX_SOCKET * VH_MAX_RB_PER_SOCKET)

#define SIZE_16M_ALIGN              0xFFFFFFFFFF000000ull

#define BOTTOM_OF_COMPAT            0xFEC00000ul // From BOTTOM_OF_COMPAT to 4G would be leaved as undescribed (COMPAT)

#define ADDITIONAL_POSTED_REGION_UNDER_PRIMARY_RB_START 0xFED00000ul
#define ADDITIONAL_POSTED_REGION_UNDER_PRIMARY_RB_END   0xFED0FFFFul

#define FABRIC_ID_SOCKET_DIE_MASK   0x20

/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */

/// MMIO address control
typedef union {
  struct {                ///< MMIO address control bitfields
    UINT32 RE:1;          ///< Read enable
    UINT32 WE:1;          ///< Write enable
    UINT32 CpuDis:1;      ///< CPU Disable
    UINT32 :1;            ///< Reserved
    UINT32 DstFabricID:10;///< Destination FabricID of the IOS
    UINT32 :2;            ///< Reserved
    UINT32 NP:1;          ///< Non-Posted
    UINT32 :15;           ///< Reserved
  } Field;
  UINT32  Value;
} MMIO_ADDR_CTRL;

/// X86 IO base address
typedef union {
  struct {                ///< IO Space Base Address bitfields
    UINT32 RE:1;          ///< Read enable
    UINT32 WE:1;          ///< Write enable
    UINT32 :3;            ///< Reserved
    UINT32 IE:1;          ///< IE
    UINT32 :6;            ///< Reserved
    UINT32 IOBase:13;     ///< IO Base
    UINT32 :7;            ///< Reserved
  } Field;
  UINT32  Value;
} X86IO_BASE_ADDR;

/// X86 IO limit address
typedef union {
  struct {                ///< IO Space Limit bitfields
    UINT32 DstFabricID:10; ///< Destination FabricID of the IOS
    UINT32 :2;            ///< Reserved
    UINT32 IOLimit:13;    ///< IO Limit
    UINT32 :7;            ///< Reserved
  } Field;
  UINT32  Value;
} X86IO_LIMIT_ADDR;

/// Configuration Address Maps
typedef union {
  struct {                ///< Configuration Address Map bitfields
    UINT32 RE:1;          ///< Read enable
    UINT32 WE:1;          ///< Write enable
    UINT32 :2;            ///< Reserved
    UINT32 DstFabricID:10;///< Destination FabricID of the IOS
    UINT32 :2;            ///< Reserved
    UINT32 BusNumBase:8;  ///< Bus Number Base Bits
    UINT32 BusNumLimit:8; ///< Bus Number Limit Bits
  } Field;
  UINT32  Value;
} CFG_ADDR_MAP;

/// Logical to physical map
typedef struct _FABRIC_RB_LOG_TO_PHY_MAP {
  UINTN PhySktNum;
  UINTN PhyRbNum;
  UINTN PciBusBase;
  UINTN PciBusLimit;
} FABRIC_RB_LOG_TO_PHY_MAP;

/// MMIO ABOVE 4G
typedef struct _FABRIC_MMIO_ABOVE_4G_QUEUE {
  UINT8  MmioType;
  UINT64 AlignBit;
} FABRIC_MMIO_ABOVE_4G_QUEUE;

/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
FabricInitMmioBasedOnNvVariable3 (
  IN       FABRIC_MMIO_MANAGER *FabricMmioManager,
  IN       FABRIC_RESOURCE_FOR_EACH_RB *MmioSizeForEachRb,
  IN       FABRIC_ADDR_SPACE_SIZE       *SpaceStatus,
  IN       UINT8                SocketNumber,
  IN       UINT8                RbNumberPerSocket,
  IN       BOOLEAN              SetDfRegisters
  );

EFI_STATUS
FabricInitMmioEqually3 (
  IN       FABRIC_MMIO_MANAGER *FabricMmioManager,
  IN       UINT8                SocketNumber,
  IN       UINT8                RbNumberPerSocket
);

EFI_STATUS
FabricInitIoBasedOnNvVariable3 (
  IN       FABRIC_IO_MANAGER   *FabricIoManager,
  IN       FABRIC_RESOURCE_FOR_EACH_RB *IoSizeForEachRb,
  IN       FABRIC_ADDR_SPACE_SIZE       *SpaceStatus,
  IN       UINT8                SocketNumber,
  IN       UINT8                RbNumberPerSocket,
  IN       BOOLEAN              SetDfRegisters
  );

EFI_STATUS
FabricInitIoEqually3 (
  IN       FABRIC_IO_MANAGER   *FabricIoManager,
  IN       UINT8                SocketNumber,
  IN       UINT8                RbNumberPerSocket
  );

EFI_STATUS
FabricInitPciBusBasedOnNvVariable3 (
  IN       FABRIC_RESOURCE_FOR_EACH_RB  *PciBusRangeForEachRb,
  IN       UINT8                SocketNumber,
  IN       UINT8                RbsPerSocket
  );

VOID
FabricAdditionalMmioSetting3 (
  IN       FABRIC_MMIO_MANAGER *FabricMmioManager,
  IN       UINT8                SocketNumber,
  IN       UINT8                RbNumberPerSocket,
  IN       UINT64               BottomOfCompat,
  IN       BOOLEAN              ReservedRegionAlreadySet
  );

VOID
FabricSetMmioReg3 (
  IN       UINT8  TotalSocket,
  IN       UINT8  MmioPairIndex,
  IN       UINTN  SktNum,
  IN       UINTN  RbNum,
  IN       UINT64 BaseAddress,
  IN       UINT64 Length
  );

VOID
FabricSetIoReg3 (
  IN       UINT8  TotalSocket,
  IN       UINT8  RegIndex,
  IN       UINTN  SktNum,
  IN       UINTN  RbNum,
  IN       UINT32 IoBase,
  IN       UINT32 IoSize
  );

VOID
FabricSetCfgAddrMapReg3 (
  IN       UINT8  TotalSocket,
  IN       UINT8  RbPerDie,
  IN       UINT8  RegIndex,
  IN       UINTN  SktNum,
  IN       UINTN  RbNum,
  IN       UINT16 Base,
  IN       UINT16 Limit
  );

VOID
FabricGetPrimayRb (
  IN       UINTN *SocketNum,
  IN       UINTN *RootBridgeNum
  );

VOID
FabricGetPhySktRbNum (
  IN       UINTN LogSktNum,
  IN       UINTN LogRbNum,
  IN       UINTN *PhySktNum,
  IN       UINTN *PhyRbNum
  );

#pragma pack (pop)
#endif // _FABRIC_MMIO_MAP_MANAGER_3_LIB_H_

