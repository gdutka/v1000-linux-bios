/* $NoKeywords:$ */
/**
 * @file
 *
 * GNB function to create/locate PCIe configuration data area
 *
 * Contain code that create/locate and rebase configuration data area.
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

#ifndef _GNBHANDLELIB_H_
#define _GNBHANDLELIB_H_


GNB_HANDLE *
GnbGetHandle (
  IN      AMD_CONFIG_PARAMS                 *StdHeader
  );

UINT8
GnbGetSocketId (
  IN      GNB_HANDLE                        *GnbHandle
  );

PCI_ADDR
GnbGetHostPciAddress (
  IN      GNB_HANDLE          *Handle
 );

UINT64
GnbGetGmmBaseAddress (
  IN      GNB_HANDLE          *Handle
  );

#define GnbGetNextHandle(Descriptor) (GNB_HANDLE *) PcieConfigGetNextTopologyDescriptor (Descriptor, DESCRIPTOR_TERMINATE_TOPOLOGY)

#define GnbGetSiliconId(Handle) (Handle != NULL ? (Handle)->SiliconId : NULL)
#define GnbGetNodeId(Handle) (Handle != NULL ? (Handle)->NodeId : NULL)

#define GnbIsGnbConnectedToSb(Handle) (Handle != NULL ? ((Handle)->Address.AddressValue == 0x0) : FALSE)

#endif


