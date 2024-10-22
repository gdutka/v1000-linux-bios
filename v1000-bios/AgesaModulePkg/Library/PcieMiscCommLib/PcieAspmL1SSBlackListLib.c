/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe link ASPM L1 SS
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
#include  <Library/GnbPcieConfigLib.h>
#include  <Library/GnbCommonLib.h>
#include  <Library/PcieMiscCommLib.h>

#define FILECODE LIBRARY_PCIEMISCCOMMLIB_PCIEASPML1SSLIB_FILECODE
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
  BOOLEAN                 EnableL11;
  BOOLEAN                 EnableL12;
  PCI_ADDR                UpstreamPort;
  PCI_ADDR                DownstreamPort;
} PCIE_L1SS_DATA;

UINT16  L1SSBlacklistDeviceTable[] = {
//VID     DID     L1.1 L1.2
  0x1344, 0x5410, 0,   1,
  0x8086, 0xF1A6, 0,   1,
  0xFFFF, 0xFFFF, 0,   0      //End of Table
};

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * L1SS Black List
 *
 *
 *
 * @param[in] Device              PCI_ADDR of PCIe Device to evaluate
 * @param[in] PcieAspmL1SSData    Pointer to PCIE_L1SS_DATA
 * @param[in] StdHeader           Standard configuration header
 * @retval    AGESA_STATUS
 */

AGESA_STATUS
PcieL1SSBlackListFeature (
  IN       PCIE_L1SS_DATA         *PcieL1SSData,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32  UpstreamDeviceId;
  UINT32  DownstreamDeviceId;
  UINTN   i;
  UINT32  DeviceId;
  UINT32  VendorId;

  GnbLibPciRead (PcieL1SSData->UpstreamPort.AddressValue, AccessWidth32, &UpstreamDeviceId, StdHeader);
  GnbLibPciRead (PcieL1SSData->DownstreamPort.AddressValue, AccessWidth32, &DownstreamDeviceId, StdHeader);
  for (i = 0; i < (sizeof (L1SSBlacklistDeviceTable) / sizeof (UINT16)); i = i + 4) {
    VendorId = L1SSBlacklistDeviceTable[i];
    DeviceId = L1SSBlacklistDeviceTable[i + 1];
    if (VendorId == 0xFFFF) {
      break;
    }
    if ((VendorId == (UINT16)UpstreamDeviceId) || (VendorId == (UINT16)DownstreamDeviceId)) {
      if (DeviceId == 0xFFFF || DeviceId == (UpstreamDeviceId >> 16) || DeviceId == (DownstreamDeviceId >> 16)) {
        if (L1SSBlacklistDeviceTable[i + 2] == 1) {
          PcieL1SSData->EnableL11 = FALSE;
        }
        if (L1SSBlacklistDeviceTable[i + 3] == 1) {
          PcieL1SSData->EnableL12 = FALSE;
        }
        IDS_HDT_CONSOLE (GNB_TRACE, "  L1SS Black List: US_DID=0x%x DS_DID=0x%x L1.1=%d L1.2=%d \n", UpstreamDeviceId, DownstreamDeviceId, PcieL1SSData->EnableL11, PcieL1SSData->EnableL12);
      }
    }
  }
  return AGESA_SUCCESS;
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
PcieL1SSBlackListCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  SCAN_STATUS           ScanStatus;
  PCIE_L1SS_DATA        *PcieAspmL1SSData;
  PCIE_DEVICE_TYPE      DeviceType;

  ScanStatus = SCAN_SUCCESS;
  PcieAspmL1SSData = (PCIE_L1SS_DATA*) ScanData;

  DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);
  switch (DeviceType) {
  case  PcieDeviceRootComplex:
  case  PcieDeviceDownstreamPort:
    PcieAspmL1SSData->DownstreamPort = Device;
    GnbLibPciScanSecondaryBus (Device, &PcieAspmL1SSData->ScanData);
    break;
  case  PcieDeviceUpstreamPort:
    PcieAspmL1SSData->UpstreamPort = Device;
    PcieL1SSBlackListFeature (PcieAspmL1SSData, NULL);
    GnbLibPciScanSecondaryBus (Device, &PcieAspmL1SSData->ScanData);
    break;
  case  PcieDeviceEndPoint:
  case  PcieDeviceLegacyEndPoint:
    PcieAspmL1SSData->UpstreamPort = Device;
    PcieL1SSBlackListFeature (PcieAspmL1SSData, NULL);
    ScanStatus = SCAN_SKIP_FUNCTIONS;
    break;
  default:
    break;
  }
  return ScanStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback to Disable L1SS
 *
 *
 *
 * @param[in]       Engine          Pointer to engine config descriptor
 */

BOOLEAN
PcieDisableL1SSCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine
  )
{
  PCIE_L1SS_DATA PcieAspmL1SSData;
  BOOLEAN        ret;

  ret = FALSE;
  PcieAspmL1SSData.EnableL11 = TRUE;
  PcieAspmL1SSData.EnableL12 = TRUE;
  PcieAspmL1SSData.ScanData.StdHeader = NULL;

  PcieAspmL1SSData.ScanData.GnbScanCallback = PcieL1SSBlackListCallback;
  GnbLibPciScan (Engine->Type.Port.Address, Engine->Type.Port.Address, &PcieAspmL1SSData.ScanData);

  if (!PcieAspmL1SSData.EnableL11) {
    Engine->Type.Port.LinkAspmL1_1 = 0;
    ret = TRUE;
  }
  if (!PcieAspmL1SSData.EnableL12) {
    Engine->Type.Port.LinkAspmL1_2 = 0;
    ret = TRUE;
  }
  return ret;
}


