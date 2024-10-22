/* $NoKeywords:$ */
/**
 * @file
 *
 * CZ specific PCIe configuration data services
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 311183 $   @e \$Date: 2015-01-19 22:51:58 +0800 (Mon, 19 Jan 2015) $
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

#ifndef _PCIELIBCZ_H_
#define _PCIELIBCZ_H_

VOID
PcieSetVoltageCZ (
  IN      PCIE_LINK_SPEED_CAP     LinkCap,
  IN      PCIe_PLATFORM_CONFIG    *Pcie
  );

UINT8
PciePifGetPllPowerUpLatencyCZ (
  IN      PCIe_WRAPPER_CONFIG     *Wrapper,
  IN      PCIe_PLATFORM_CONFIG    *Pcie
  );

PCIE_LINK_SPEED_CAP
PcieGetLinkSpeedCapCZ (
  IN       UINT32                Flags,
  IN       PCIe_ENGINE_CONFIG    *Engine
  );

UINT32
GnbTimeStampCZ (
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

VOID
PcieTopologyLaneControlCZ (
  IN      LANE_CONTROL          Control,
  IN      UINT32                LaneBitMap,
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
    );

VOID
PciePwrPowerDownUnusedLanesCZ (
  IN       PCIe_WRAPPER_CONFIG    *Wrapper,
  IN       PCIe_PLATFORM_CONFIG   *Pcie
  );

AGESA_STATUS
PcieEdpPortPowerCheckCZ (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );
#endif

