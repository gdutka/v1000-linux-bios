/* $NoKeywords:$ */
/**
 * @file
 *
 * NB services
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
#ifndef _GNBNBINITLIBV4_H_
#define _GNBNBINITLIBV4_H_

BOOLEAN
GnbCheckPhantomFuncSupport (
  IN       PCI_ADDR                 Device,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

AGESA_STATUS
GnbGetTopologyInfo (
  IN       PCI_ADDR                  StartPciAddress,
  IN       PCI_ADDR                  EndPciAddress,
     OUT   GNB_TOPOLOGY_INFO         *TopologyInfo,
  IN       AMD_CONFIG_PARAMS         *StdHeader
  );

PCI_ADDR
GnbGetIommuPciAddress (
  IN       GNB_HANDLE               *GnbHandle,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

AGESA_STATUS
GnbEnableIommuMmio (
  IN       GNB_HANDLE           *GnbHandle,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

AGESA_STATUS
GnbSetTom (
  IN      PCI_ADDR            NbPciAddress,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

UINT8
GnbLocateHighestVidIndex (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

UINT32
GnbTranslateVidCodeToMillivolt (
  IN       UINT8               Vid,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
GnbOrbDynamicWake (
  IN      PCI_ADDR            NbPciAddress,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
GnbLpcDmaDeadlockPrevention (
  IN       GNB_HANDLE               *GnbHandle,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

#endif

