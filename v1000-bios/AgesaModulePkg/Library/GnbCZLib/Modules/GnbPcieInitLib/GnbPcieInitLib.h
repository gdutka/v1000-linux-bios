/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe Init Library
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
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
#ifndef _PCIEINITLIBV1_H_
#define _PCIEINITLIBV1_H_

#include "PciePortRegAcc.h"
#include "PcieUtilityLib.h"
#include "PcieWrapperRegAcc.h"
#include "PciePortServices.h"
#include "PcieAspm.h"

VOID
PciePortsVisibilityControl (
  IN      PCIE_PORT_VISIBILITY    Control,
  IN      PCIe_PLATFORM_CONFIG    *Pcie
  );

VOID
PcieAspmGetMinExitLatency (
  IN       PCI_ADDR                DownstreamPort,
     OUT   PCIe_ASPM_LATENCY_INFO  *AspmLatencyInfo,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  );
#endif

