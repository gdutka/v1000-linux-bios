/* $NoKeywords:$ */
/**
 * @file
 *
 * Service procedure to calculate PCIe topology segment maximum exit latency
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  <GnbDxio.h>
#include  <Filecode.h>
#include  <Library/IdsLib.h>
#include  <Library/GnbCommonLib.h>
#include  <Library/PcieMiscCommLib.h>

#define FILECODE LIBRARY_PCIEMISCCOMMLIB_PCIEASPMEXITLATENCYLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

typedef struct {
  GNB_PCI_SCAN_DATA       ScanData;
  PCIe_ASPM_LATENCY_INFO  *AspmLatencyInfo;
  PCI_ADDR                DownstreamPort;
  UINT8                   LinkCount;
} PCIE_EXIT_LATENCY_DATA;

typedef struct {
  GNB_PCI_SCAN_DATA ScanData;
  PCI_ADDR          DownstreamPort;
  UINT16            L1ExitLatency;
  BOOLEAN           BlackList;
} PCIE_L1_EXIT_LATENCY_DATA;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
UINT16  DeviceTableL1ExitLatency[] = {
 //VID   DID     5 - L1 exit latency 16us~32us
 0x1022, 0x43C6, 5,
 0x1022, 0x43C7, 5,
 0x1022, 0x43C8, 5,
 0x1022, 0x43D0, 5,
 0x1022, 0x43D1, 5,
 0x1022, 0x43D2, 5,
 0x1022, 0x43D3, 5,
 0x1022, 0x43D4, 5,
 0x1022, 0x43D5, 5,
 0x1022, 0x43D6, 5,
 0x1022, 0x43D7, 5,
 0x1022, 0x43D8, 5,
 0x1022, 0x43D9, 5,
 0x1022, 0x43DA, 5,
 0x1022, 0x43DB, 5,
 0x1022, 0x43DC, 5,
 0x1022, 0x43DD, 5,
 0x1022, 0x43DE, 5,
 0x1022, 0x43DF, 5,
 0x1022, 0x43EC, 5,
 0x1022, 0x43EE, 5,
 0x1022, 0x43EF, 5,
};

/*----------------------------------------------------------------------------------------*/
/**
 * Pcie L1ExitLatency Black List
 *
 *
 *
 * @param[in] LinkL1ExitLatency   L1ExitLatency black list
 */

VOID
PcieL1ExitLatencyBlackListFeature (
  IN       PCIE_L1_EXIT_LATENCY_DATA *LinkL1ExitLatency
  )
{
  UINT32  i;
  UINT32  DeviceId;
  UINT32  VendorId;
  UINT32  DownstreamDeviceId;

  GnbLibPciRead (LinkL1ExitLatency->DownstreamPort.AddressValue, AccessWidth32, &DownstreamDeviceId, NULL);
  for (i = 0; i < (sizeof (DeviceTableL1ExitLatency) / sizeof (UINT16)); i = i + 3) {
    VendorId = DeviceTableL1ExitLatency[i];
    DeviceId = DeviceTableL1ExitLatency[i + 1];
    if ((VendorId == (UINT16)DownstreamDeviceId) && (DeviceId == (DownstreamDeviceId >> 16))) {
      LinkL1ExitLatency->BlackList = TRUE;
      LinkL1ExitLatency->L1ExitLatency = DeviceTableL1ExitLatency[i + 2];
      IDS_HDT_CONSOLE (GNB_TRACE, "  - Vid(0x%x) Did(0x%x) L1ExitLatency = %d\n",
        (UINT16)DownstreamDeviceId,
        (UINT16)(DownstreamDeviceId >> 16),
        LinkL1ExitLatency->L1ExitLatency
        );
      break;
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get maximum exit latency for device
 *
 *
 *
 * @param[in]     Device          PCI Address
 * @param[in,out] ScanData        Scan configuration data
 * @retval                        Scan Status of 0
 */

SCAN_STATUS
PcieAspmGetMaxExitLatencyCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  PCIE_EXIT_LATENCY_DATA  *PcieExitLatencyData;
  PCIE_DEVICE_TYPE        DeviceType;
  UINT32                  Value;
  UINT8                   PcieCapPtr;
  UINT8                   L1AcceptableLatency;

  PcieExitLatencyData = (PCIE_EXIT_LATENCY_DATA*) ScanData;
  DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);
  IDS_HDT_CONSOLE (GNB_TRACE, "  PcieAspmGetMaxExitLatencyCallback for Device = %d:%d:%d\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function
    );
  switch (DeviceType) {
  case  PcieDeviceRootComplex:
  case  PcieDeviceDownstreamPort:
    PcieExitLatencyData->DownstreamPort = Device;
    PcieExitLatencyData->LinkCount++;
    GnbLibPciScanSecondaryBus (Device, &PcieExitLatencyData->ScanData);
    PcieExitLatencyData->LinkCount--;
    break;
  case  PcieDeviceUpstreamPort:
    GnbLibPciScanSecondaryBus (Device, &PcieExitLatencyData->ScanData);
    break;
  case  PcieDeviceEndPoint:
  case  PcieDeviceLegacyEndPoint:
    PcieCapPtr = GnbLibFindPciCapability (Device.AddressValue, PCIE_CAP_ID, ScanData->StdHeader);
    ASSERT (PcieCapPtr != 0);
    GnbLibPciRead (
      Device.AddressValue | (PcieCapPtr + PCIE_LINK_CAP_REGISTER),
      AccessWidth32,
      &Value,
      ScanData->StdHeader
      );
    if ((Value & PCIE_ASPM_L1_SUPPORT_CAP) != 0) {
      GnbLibPciRead (
        Device.AddressValue | (PcieCapPtr + PCIE_DEVICE_CAP_REGISTER),
        AccessWidth32,
        &Value,
        ScanData->StdHeader
        );
      L1AcceptableLatency = (UINT8) (1 << ((Value >> 9) & 0x7));
      if (PcieExitLatencyData->LinkCount > 1) {
        L1AcceptableLatency = L1AcceptableLatency + PcieExitLatencyData->LinkCount;
      }
      if (PcieExitLatencyData->AspmLatencyInfo->MaxL1ExitLatency <  L1AcceptableLatency) {
        PcieExitLatencyData->AspmLatencyInfo->MaxL1ExitLatency = L1AcceptableLatency;
      }
      IDS_HDT_CONSOLE (PCIE_MISC, "  Device max exit latency  L1 - %d us\n",
        L1AcceptableLatency
        );
    }
    break;
  default:
    break;
  }
  return SCAN_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Determine ASPM L-state maximum exit latency for PCIe segment
 *
 *  Scan through all link in segment to determine maxim exit latency requirement by EPs.
 *
 * @param[in]   DownstreamPort    PCI address of PCIe port
 * @param[out]  AspmLatencyInfo   Latency info
 * @param[in]   StdHeader         Standard configuration header
 *
 */

VOID
PcieAspmGetMaxExitLatency (
  IN       PCI_ADDR                DownstreamPort,
     OUT   PCIe_ASPM_LATENCY_INFO  *AspmLatencyInfo,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  PCIE_EXIT_LATENCY_DATA  PcieExitLatencyData;
  PcieExitLatencyData.AspmLatencyInfo = AspmLatencyInfo;
  PcieExitLatencyData.ScanData.StdHeader = StdHeader;
  PcieExitLatencyData.LinkCount = 0;
  PcieExitLatencyData.ScanData.GnbScanCallback = PcieAspmGetMaxExitLatencyCallback;
  GnbLibPciScan (DownstreamPort, DownstreamPort, &PcieExitLatencyData.ScanData);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Evaluate device
 *
 *
 *
 * @param[in]     Device          PCI Address
 * @param[in,out] ScanData        Scan configuration data
 * @retval                        Scan Status of 0
 */

SCAN_STATUS
PcieL1ExitLatencyWACallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  SCAN_STATUS                ScanStatus;
  PCIE_L1_EXIT_LATENCY_DATA  *PcielinkData;
  PCIE_DEVICE_TYPE           DeviceType;

  ScanStatus = SCAN_SUCCESS;
  PcielinkData = (PCIE_L1_EXIT_LATENCY_DATA *) ScanData;
  PcielinkData->DownstreamPort = Device;
  PcieL1ExitLatencyBlackListFeature(PcielinkData);
  if (PcielinkData->BlackList) {
    return ScanStatus;
  }

  DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);
  switch (DeviceType) {
  case  PcieDeviceRootComplex:
  case  PcieDeviceDownstreamPort:
    GnbLibPciScanSecondaryBus (Device, &PcielinkData->ScanData);
    break;
  case  PcieDeviceUpstreamPort:
    GnbLibPciScanSecondaryBus (Device, &PcielinkData->ScanData);
    break;
  case  PcieDeviceEndPoint:
  case  PcieDeviceLegacyEndPoint:
    break;
  default:
    break;
  }
  return ScanStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * L1 Exit Latency
 *
 *
 *
 * @param[in]       Engine          Pointer to engine config descriptor
 * @param[in, out]  L1ExitLatency   L1 Exit Latency Value
 * @retval                          FALSE or TRUE
 */

BOOLEAN
PcieLinkL1ExitLatencyWA (
  IN       PCIe_ENGINE_CONFIG      *Engine,
  IN OUT   UINT16                  *L1ExitLatency
  )
{
  PCIE_L1_EXIT_LATENCY_DATA PcieL1ExitLatencyData;

  PcieL1ExitLatencyData.ScanData.StdHeader = NULL;
  PcieL1ExitLatencyData.ScanData.GnbScanCallback = PcieL1ExitLatencyWACallback;
  PcieL1ExitLatencyData.L1ExitLatency = 0xFFFF;
  PcieL1ExitLatencyData.DownstreamPort = Engine->Type.Port.Address;
  PcieL1ExitLatencyData.BlackList = FALSE;
  GnbLibPciScan (Engine->Type.Port.Address, Engine->Type.Port.Address, &PcieL1ExitLatencyData.ScanData);
  *L1ExitLatency = PcieL1ExitLatencyData.L1ExitLatency;

  return PcieL1ExitLatencyData.BlackList;
}

