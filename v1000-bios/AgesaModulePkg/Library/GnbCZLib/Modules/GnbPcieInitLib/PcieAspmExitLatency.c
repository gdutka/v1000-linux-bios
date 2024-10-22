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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieInitLib.h"

#define FILECODE LIBRARY_GNBCZLIB_MODULES_GNBPCIEINITLIB_PCIEASPMEXITLATENCY_FILECODE
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

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

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
PcieAspmGetMinExitLatencyCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  PCIE_EXIT_LATENCY_DATA  *PcieExitLatencyData;
  PCIE_DEVICE_TYPE        DeviceType;
  UINT32                  Value;
  UINT8                   PcieCapPtr;
  UINT8                   ActualL1Latency;
  UINT8                   TotalL1Latency;

  PcieExitLatencyData = (PCIE_EXIT_LATENCY_DATA*) ScanData;
  DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);
  IDS_HDT_CONSOLE (GNB_TRACE, "  PcieAspmGetMinExitLatencyCallback for Device = %d:%d:%d\n",
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
      ActualL1Latency = (UINT8) (1 << ((Value >> 9) & 0x7));
      TotalL1Latency =  ActualL1Latency;
      if (PcieExitLatencyData->LinkCount > 1) {
        TotalL1Latency = ActualL1Latency - (PcieExitLatencyData->LinkCount - 1);
      }
      if (TotalL1Latency < PcieExitLatencyData->AspmLatencyInfo->MaxL1ExitLatency) {
        PcieExitLatencyData->AspmLatencyInfo->MaxL1ExitLatency = TotalL1Latency;
      }
      IDS_HDT_CONSOLE (PCIE_MISC, "  Device max exit latency  L1 - %d us\n",
        ActualL1Latency
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
PcieAspmGetMinExitLatency (
  IN       PCI_ADDR                DownstreamPort,
     OUT   PCIe_ASPM_LATENCY_INFO  *AspmLatencyInfo,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  PCIE_EXIT_LATENCY_DATA  PcieExitLatencyData;
  PcieExitLatencyData.AspmLatencyInfo = AspmLatencyInfo;
  PcieExitLatencyData.ScanData.StdHeader = StdHeader;
  PcieExitLatencyData.LinkCount = 0;
  PcieExitLatencyData.ScanData.GnbScanCallback = PcieAspmGetMinExitLatencyCallback;
  GnbLibPciScan (DownstreamPort, DownstreamPort, &PcieExitLatencyData.ScanData);
}

