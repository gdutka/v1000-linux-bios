/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe utility. Various supporting functions.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
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

#ifndef _PCIEUTILLIB_H_
#define _PCIEUTILLIB_H_

/// Core lanes
typedef enum {
  AllCoreLanes,           ///< All core lanes
  AllocatedCoreLanes,     ///< Allocated core lanes
  ActiveCoreLanes,        ///< Active core lanes
  HotplugCoreLanes,       ///< Hot plug core lanes
  SbCoreLanes,            ///< South bridge core lanes
} CORE_LANES;

/// DDI lanes
typedef enum {
  DdiAllLanes,            ///< All DDI Lanes
  DdiActiveLanes          ///< Active DDI Lanes
} DDI_LANES;

BOOLEAN
PcieUtilSearchArray (
  IN      UINT8                         *Buf1,
  IN      UINTN                         Buf1Length,
  IN      UINT8                         *Buf2,
  IN      UINTN                         Buf2Length
  );

VOID
PcieUtilGetLinkHwStateHistory (
  IN       PCIe_ENGINE_CONFIG            *Engine,
     OUT   UINT8                         *History,
  IN       UINT8                         Length,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  );


BOOLEAN
PcieUtilIsLinkReversed (
  IN      BOOLEAN                        HwLinkState,
  IN      PCIe_ENGINE_CONFIG             *Engine,
  IN      PCIe_PLATFORM_CONFIG           *Pcie
  );


UINT8
PcieUtilGetLinkWidth (
  IN      PCIe_ENGINE_CONFIG            *Engine,
  IN      PCIe_PLATFORM_CONFIG          *Pcie
  );


UINT32
PcieUtilGetEngineLaneBitMap (
  IN      UINT32                         IncludeLaneType,
  IN      UINT32                         ExcludeLaneType,
  IN      PCIe_ENGINE_CONFIG             *Engine
  );

UINT32
PcieUtilGetWrapperLaneBitMap (
  IN      UINT32                         IncludeLaneType,
  IN      UINT32                         ExcludeLaneType,
  IN      PCIe_WRAPPER_CONFIG            *Wrapper
  );

VOID
PciePortProgramRegisterTable (
  IN      PCIE_PORT_REGISTER_ENTRY       *Table,
  IN      UINTN                          Length,
  IN      PCIe_ENGINE_CONFIG             *Engine,
  IN      BOOLEAN                        S3Save,
  IN      PCIe_PLATFORM_CONFIG           *Pcie
  );

VOID
PcieLockRegisters (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  );

PCIE_LINK_SPEED_CAP
PcieUtilGlobalGenCapability (
  IN      UINT32                         Flags,
  IN      PCIe_PLATFORM_CONFIG           *Pcie
  );

#endif

