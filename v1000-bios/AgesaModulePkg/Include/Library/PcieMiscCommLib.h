/* $NoKeywords:$ */
/**
 * @file
 *
 * Configure Pcie Misc Common feature
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
#ifndef _PCIEMISCCOMMLIB_H_
#define _PCIEMISCCOMMLIB_H_

AGESA_STATUS
PcieAspmBlackListFeature (
  IN       PCIe_LINK_ASPM         *LinkAspm,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

VOID
PcieAspmGetMaxExitLatency (
  IN       PCI_ADDR                DownstreamPort,
     OUT   PCIe_ASPM_LATENCY_INFO  *AspmLatencyInfo,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  );

VOID
PcieLinkAspmEnable (
  IN       PCI_ADDR                DownstreamPort,
  IN       PCIE_ASPM_TYPE          Aspm,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  );

VOID
PcieAspmEnableOnFunction (
  IN       PCI_ADDR                Function,
  IN       PCIE_ASPM_TYPE          Aspm,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  );

VOID
PcieAspmInterface (
  IN       GNB_HANDLE  *GnbHandle
  );

VOID
PcieClkPmInterface (
  IN       GNB_HANDLE  *GnbHandle
  );

VOID
PcieCommClkCfgInterface (
  IN       GNB_HANDLE  *GnbHandle
  );

VOID
PcieSetMaxPayload (
  IN       PCI_ADDR                DownstreamPort,
  IN       UINT8                   EngineMaxPayload,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  );

VOID
PcieMaxPayloadInterface (
  IN       GNB_HANDLE  *GnbHandle
  );

VOID
PcieMaxCplPayloadInterface (
  IN       GNB_HANDLE  *GnbHandle
  );

AGESA_STATUS
PciePayloadBlackListFeature (
  IN       PCI_ADDR               Device,
  IN       UINT8                  *MaxPayload,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

VOID
PcieAriInit (
  IN       GNB_HANDLE  *GnbHandle
  );

VOID
PcieAspmL1SSInterface (
  IN       GNB_HANDLE  *GnbHandle
  );

VOID
PcieSlotPowerLimitInterface (
  IN       GNB_HANDLE  *GnbHandle
  );

VOID
PcieMaxReadRequestInterface (
  IN       GNB_HANDLE  *GnbHandle
  );

VOID
PcieLTRPortInitCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer
  );

VOID
PcieLTRInterface (
  IN       GNB_HANDLE  *GnbHandle
  );
BOOLEAN
PcieLinkL1ExitLatencyWA (
  IN       PCIe_ENGINE_CONFIG  *Engine,
  IN OUT   UINT16              *L1ExitLatency
  );

BOOLEAN
PcieDisableL1SSCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine
  );

UINT16
PcieDisableASPMCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine
  );

#endif


