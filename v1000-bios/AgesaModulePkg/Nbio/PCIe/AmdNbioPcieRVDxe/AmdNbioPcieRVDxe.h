/* $NoKeywords:$ */
/**
 * @file
 *
 * Local Function Prototype Header File
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 312288 $   @e \$Date: 2015-02-04 00:39:01 -0600 (Wed, 04 Feb 2015) $
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
#ifndef _AMDNBIOPCIERVDXE_H_
#define _AMDNBIOPCIERVDXE_H_

VOID
PcieSrisInit (
  IN       GNB_HANDLE  *GnbHandle
  );

VOID
PcieClkReqInterface (
  IN       GNB_HANDLE  *GnbHandle
  );

VOID
AmdAPlusAConfig (
  IN       PCIe_PLATFORM_CONFIG       *Pcie
  );

VOID
AmdPsppConfig (
  IN       PCIe_PLATFORM_CONFIG       *Pcie
  );

VOID
AmdPciePhyClkCntrl (
  IN       PCIe_PLATFORM_CONFIG       *Pcie
  );

#endif

