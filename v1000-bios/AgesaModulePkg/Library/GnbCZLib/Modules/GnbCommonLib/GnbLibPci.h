/* $NoKeywords:$ */
/**
 * @file
 *
 * Various PCI service routines.
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

#ifndef _GNBLIBPCI_H_
#define _GNBLIBPCI_H_

#define PCIE_CAP_ID                 0x10
#define IOMMU_CAP_ID                0x0F
#define ARI_CAP_ID                  0x0E

/// PCIe device type
typedef enum {
  PcieDeviceEndPoint,                       ///< Endpoint
  PcieDeviceLegacyEndPoint,                 ///< Legacy endpoint
  PcieDeviceRootComplex = 4,                ///< Root complex
  PcieDeviceUpstreamPort,                   ///< Upstream port
  PcieDeviceDownstreamPort,                 ///< Downstream Port
  PcieDevicePcieToPcix,                     ///< PCIe to PCI/PCIx bridge
  PcieDevicePcixToPcie,                     ///< PCI/PCIx to PCIe bridge
  PcieNotPcieDevice = 0xff                  ///< unknown device
} PCIE_DEVICE_TYPE;

typedef  UINT32 SCAN_STATUS;

#define SCAN_SKIP_FUNCTIONS         0x1
#define SCAN_SKIP_DEVICES           0x2
#define SCAN_SKIP_BUSES             0x4
#define SCAN_SUCCESS                0x0

//  Forward declaration needed for multi-structure mutual references
AGESA_FORWARD_DECLARATION (GNB_PCI_SCAN_DATA);

typedef SCAN_STATUS (*GNB_SCAN_CALLBACK) (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  );

///Scan supporting data
struct _GNB_PCI_SCAN_DATA {
  GNB_SCAN_CALLBACK     GnbScanCallback;  ///< Callback for each found device
  AMD_CONFIG_PARAMS     *StdHeader;       ///< Standard configuration header
};

#define   PCIE_CAP_ID               0x10
#define   PCIE_LINK_CAP_REGISTER    0x0C
#define   PCIE_LINK_CTRL_REGISTER   0x10
#define   PCIE_DEVICE_CAP_REGISTER  0x04
#define   PCIE_DEVICE_CTRL_REGISTER 0x08
#define   PCIE_ASPM_L1_SUPPORT_CAP  BIT11

#define MAX_PAYLOAD_128             0x0     ///< Max allowed payload size 128 bytes
#define MAX_PAYLOAD_256             0x1     ///< Max allowed payload size 256 bytes
#define MAX_PAYLOAD_512             0x2     ///< Max allowed payload size 512 bytes
#define MAX_PAYLOAD_1024            0x3     ///< Max allowed payload size 1024 bytes
#define MAX_PAYLOAD_2048            0x4     ///< Max allowed payload size 2048 bytes
#define MAX_PAYLOAD_4096            0x5     ///< Max allowed payload size 4096 bytes
#define MAX_PAYLOAD                 0x5     ///< Max allowed payload size according to spec is 101b (4096 bytes)

BOOLEAN
GnbLibPciIsDevicePresent (
  IN      UINT32              Address,
  IN      AMD_CONFIG_PARAMS   *StdHeader
 );

BOOLEAN
GnbLibPciIsBridgeDevice (
  IN      UINT32              Address,
  IN      AMD_CONFIG_PARAMS   *StdHeader
 );

BOOLEAN
GnbLibPciIsMultiFunctionDevice (
  IN      UINT32              Address,
  IN      AMD_CONFIG_PARAMS   *StdHeader
 );

BOOLEAN
GnbLibPciIsPcieDevice (
  IN      UINT32              Address,
  IN      AMD_CONFIG_PARAMS   *StdHeader
 );

UINT8
GnbLibFindPciCapability (
  IN      UINT32              Address,
  IN      UINT8               CapabilityId,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

UINT16
GnbLibFindPcieExtendedCapability (
  IN      UINT32              Address,
  IN      UINT16              ExtendedCapabilityId,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
GnbLibPciScan (
  IN      PCI_ADDR            Start,
  IN      PCI_ADDR            End,
  IN      GNB_PCI_SCAN_DATA   *ScanData
 );

VOID
GnbLibPciScanSecondaryBus (
  IN       PCI_ADDR             Bridge,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  );

PCIE_DEVICE_TYPE
GnbLibGetPcieDeviceType (
  IN      PCI_ADDR            Device,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
GnbLibS3SaveConfigSpace (
  IN      UINT32              Address,
  IN      UINT16              StartRegisterAddress,
  IN      UINT16              EndRegisterAddress,
  IN      ACCESS_WIDTH        Width,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

#endif

