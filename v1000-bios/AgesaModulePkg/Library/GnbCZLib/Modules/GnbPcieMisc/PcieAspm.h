/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe link ASPM
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

#ifndef _PCIEASPM_H_
#define _PCIEASPM_H_

VOID
PcieAspmEnableOnFunction (
  IN       PCI_ADDR                Function,
  IN       PCIE_ASPM_TYPE          Aspm,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  );

AGESA_STATUS
PcieAspmInterface (
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

#endif

