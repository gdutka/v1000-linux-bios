/* $NoKeywords:$ */
/**
 * @file
 *
 * Base Fabric MMIO map manager Lib implementation for UNB
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
#ifndef _FABRIC_RESOURCE_MANAGER_UNB_LIB_H_
#define _FABRIC_RESOURCE_MANAGER_UNB_LIB_H_

#include "Porting.h"
#include <Protocol/FabricResourceManagerServicesProtocol.h>

#pragma pack (push, 1)

/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */
#define MMIO_REG_PAIR_NUM     12

#define CONF_MAP_RANGE_0      0xE0
#define CONF_MAP_RANGE_1      0xE4
#define CONF_MAP_RANGE_2      0xE8
#define CONF_MAP_RANGE_3      0xEC
#define CONF_MAP_NUM          4

#define MMIO_ALIGN            0x10000l

/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */

/// MMIO destination
typedef struct _FABRIC_MMIO_DST {
  UINT32   DstNode:3;           ///< Destination node ID bits
  UINT32   DstLink:2;           ///< Destination link ID
  UINT32   DstSubLink:1;        ///< Destination sublink
} MMIO_DST;

/// MMIO range
typedef struct _FABRIC_MMIO_RANGE {
  UINT64  Base;                 ///< Base
  UINT64  Limit;                ///< Limit
  FABRIC_MMIO_ATTRIBUTE Attribute; ///< Attribute
  MMIO_DST Destination;     ///< Destination
  UINT8   RangeNum;             ///< Range No.
  BOOLEAN Modified;             ///< if this MMIO base/limit registers need to be updated
} MMIO_RANGE;

/// MMIO base low
typedef struct {
  UINT32 RE:1;          ///< Read enable
  UINT32 WE:1;          ///< Write enable
  UINT32 CpuDis:1;      ///< CPU Disable
  UINT32 Lock:1;        ///< Lock
  UINT32 :4;            ///< Reserved
  UINT32 MmioBase:24;   ///< MMIO base address register bits[39:16]
} MMIO_BASE_LOW;

/// MMIO limit low
typedef struct {
  UINT32 DstNode:3;     ///< Destination node ID bits
  UINT32 :1;            ///< Reserved
  UINT32 DstLink:2;     ///< Destination link ID
  UINT32 DstSubLink:1;  ///< Destination sublink
  UINT32 NP:1;          ///< Non-posted
  UINT32 MmioLimit:24;  ///< MMIO limit address register bits[39:16]
} MMIO_LIMIT_LOW;

/// MMIO base/limit high
typedef struct {
  UINT32 MmioBase:8;    ///< MMIO base address register bits[47:40]
  UINT32 :8;            ///< Reserved
  UINT32 MmioLimit:8;   ///< MMIO limit address register bits[47:40]
  UINT32 :8;            ///< Reserved
} MMIO_BASE_LIMIT_HI;

/// Configuration map
typedef struct {
  UINT32 RE:1;          ///< Read enable
  UINT32 WE:1;          ///< Write enable
  UINT32 DevCmpEn:1;    ///< Device number compare mode enable
  UINT32 :1;            ///< Reserved
  UINT32 DstNode:3;     ///< Destination node ID bits
  UINT32 :1;            ///< Reserved
  UINT32 DstLink:2;     ///< Destination link ID
  UINT32 DstSubLink:1;  ///< Destination sublink
  UINT32 :5;            ///< Reserved
  UINT32 BusNumBase:8;  ///< Bus number base bits
  UINT32 BusNumLimit:8; ///< Bus number limit bits
} CONFIGURATION_MAP;

#pragma pack (pop)
#endif // _FABRIC_RESOURCE_MANAGER_UNB_LIB_H_
