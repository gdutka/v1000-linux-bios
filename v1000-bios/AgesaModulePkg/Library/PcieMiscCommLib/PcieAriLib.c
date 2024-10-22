/* $NoKeywords:$ */
/**
 * @file
 *
 * Configure ARI for endpoints
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
#include  <Library/PcdLib.h>

#define FILECODE LIBRARY_PCIEMISCCOMMLIB_PCIEARILIB_FILECODE
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
  BOOLEAN                 AriCapabilityFound;
  UINT8                   BusNumber;
} PCIE_ARI_CAPABILITY_DATA;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/**----------------------------------------------------------------------------------------*/
/**
 * Interface to configure Ari
 *
 *
 *
 * @param[in] StdHeader   Standard configuration header
 * @param[in] Ep          Pointer to ep address
 *
 * @retval    AGESA_STATUS
 */
 /*----------------------------------------------------------------------------------------*/
VOID
STATIC
PcieConfigureAri (
  IN       PCI_ADDR              Ep
  )
{
  UINT8  PcieCapPtr;
  UINT32 SrioValue;

  PcieCapPtr = GnbLibFindPciCapability (
      Ep.AddressValue,
      PCIE_CAP_ID,
      NULL
      );
  if (PcieCapPtr != 0) {
    IDS_HDT_CONSOLE (GNB_TRACE, "  Enable SRIO for Device = %d:%d:%d\n",
      Ep.Address.Bus,
      Ep.Address.Device,
      Ep.Address.Function
      );
    GnbLibPciRMW (
      Ep.AddressValue + PcieCapPtr + 8,
      AccessS3SaveWidth32,
      0xFFFFFFFF,
      BIT4,
      NULL
    );
    GnbLibPciRead (Ep.AddressValue + PcieCapPtr + 8,
        AccessWidth32,
        &SrioValue,
        NULL);
    IDS_HDT_CONSOLE (GNB_TRACE, "  SRIO = %08x\n", SrioValue);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Evaluate device ARI Capability - only enable if EVERY downstream device has an ARI capability
 *
 *
 *
 * @param[in]     Device          PCI Address
 * @param[in,out] ScanData        Scan configuration data
 * @retval                        Scan Status of 0
 */

SCAN_STATUS
STATIC
PcieSetAriCapabilityCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  PCIE_ARI_CAPABILITY_DATA    *PcieAriCapabilityData;
  PCIE_DEVICE_TYPE            DeviceType;
  UINT32                      SaveBusConfiguration;
  UINT32                      Value;
  UINT16                      PcieCapPtr;
  UINT8                       BusNumber;

  PcieAriCapabilityData = (PCIE_ARI_CAPABILITY_DATA*) ScanData;
  IDS_HDT_CONSOLE (GNB_TRACE, "  PcieSetAriCapabilityCallback for Device = %d:%d:%d\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function
    );
  DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);
  switch (DeviceType) {
  case  PcieDeviceEndPoint:
    PcieCapPtr = GnbLibFindPcieExtendedCapability (Device.AddressValue, ARI_CAP_ID, ScanData->StdHeader);
    if (PcieCapPtr != 0) {
      PcieAriCapabilityData->AriCapabilityFound = TRUE;
      PcieConfigureAri (Device);
    }
    break;
  case  PcieDeviceRootComplex:
  case  PcieDeviceDownstreamPort:
  case  PcieDeviceUpstreamPort:
    BusNumber = PcieAriCapabilityData->BusNumber;
    GnbLibPciRead (Device.AddressValue | 0x18, AccessWidth32, &SaveBusConfiguration, ScanData->StdHeader);
    Value = (((0xFF << 8) | BusNumber) << 8) | Device.Address.Bus;
    GnbLibPciWrite (Device.AddressValue | 0x18, AccessWidth32, &Value, ScanData->StdHeader);
    PcieAriCapabilityData->BusNumber++;

    GnbLibPciScanSecondaryBus (Device, &PcieAriCapabilityData->ScanData);

    PcieAriCapabilityData->BusNumber--;
    GnbLibPciWrite (Device.AddressValue | 0x18, AccessWidth32, &SaveBusConfiguration, ScanData->StdHeader);
    if (PcieAriCapabilityData->AriCapabilityFound == TRUE) {
      IDS_HDT_CONSOLE (GNB_TRACE, "  Set the ARI Forwarding Enable bit for Device = %d:%d:%d\n",
        Device.Address.Bus,
        Device.Address.Device,
        Device.Address.Function
        );
      GnbLibPciRMW (
        Device.AddressValue | 0x80,
        AccessS3SaveWidth32,
        0xFFFFFFFF,
        BIT5,
        NULL
      );
      PcieAriCapabilityData->AriCapabilityFound = FALSE;
    }
    break;
  case  PcieDeviceLegacyEndPoint:
    break;
  default:
    break;
  }
  return SCAN_SUCCESS;
}

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Evaluate device ARI Capability - only enable if EVERY downstream device has an ARI capability
 *
 *
 *
 * @param[in]     Device          PCI Address
 * @param[in,out] ScanData        Scan configuration data
 * @retval                        Scan Status of 0
 */

SCAN_STATUS
STATIC
PcieGetAriCapabilityCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  PCIE_ARI_CAPABILITY_DATA    *PcieAriCapabilityData;
  PCIE_DEVICE_TYPE            DeviceType;
  UINT32                      SaveBusConfiguration;
  UINT32                      Value;
  UINT16                      PcieCapPtr;
  UINT8                       BusNumber;

  PcieAriCapabilityData = (PCIE_ARI_CAPABILITY_DATA*) ScanData;
  IDS_HDT_CONSOLE (GNB_TRACE, "  PcieGetAriCapabilityCallback for Device = %d:%d:%d\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function
    );
  DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);
  switch (DeviceType) {
  case  PcieDeviceEndPoint:
    PcieCapPtr = GnbLibFindPcieExtendedCapability (Device.AddressValue, ARI_CAP_ID, ScanData->StdHeader);
    if (PcieCapPtr != 0) {
      IDS_HDT_CONSOLE (GNB_TRACE, "  ARI Capability FOUND\n");
      PcieAriCapabilityData->AriCapabilityFound = TRUE;
    }
    break;
  case  PcieDeviceRootComplex:
  case  PcieDeviceDownstreamPort:
  case  PcieDeviceUpstreamPort:
    BusNumber = PcieAriCapabilityData->BusNumber;
    GnbLibPciRead (Device.AddressValue | 0x18, AccessWidth32, &SaveBusConfiguration, ScanData->StdHeader);
    Value = (((0xFF << 8) | BusNumber) << 8) | Device.Address.Bus;
    GnbLibPciWrite (Device.AddressValue | 0x18, AccessWidth32, &Value, ScanData->StdHeader);
    PcieAriCapabilityData->BusNumber++;

    GnbLibPciScanSecondaryBus (Device, &PcieAriCapabilityData->ScanData);

    PcieAriCapabilityData->BusNumber--;
    GnbLibPciWrite (Device.AddressValue | 0x18, AccessWidth32, &SaveBusConfiguration, ScanData->StdHeader);
    break;
  case  PcieDeviceLegacyEndPoint:
    break;
  default:
    break;
  }
  return SCAN_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback to init various features on all active ports
 *
 *
 *
 *
 * @param[in]       Engine          Pointer to engine config descriptor
 * @param[in, out]  Buffer          Not used
 *
 */

VOID
STATIC
PcieAriInitCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer
  )
{
  PCIE_ARI_CAPABILITY_DATA  PcieAriCapabilityData;

  IDS_HDT_CONSOLE (GNB_TRACE, "  PcieAriInitCallback for Device = %d:%d:%d\n",
    Engine->Type.Port.Address.Address.Bus,
    Engine->Type.Port.Address.Address.Device,
    Engine->Type.Port.Address.Address.Function
    );
  if ((!PcieConfigIsSbPcieEngine (Engine)) &&
      (PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS))) {
    PcieAriCapabilityData.AriCapabilityFound = FALSE;
    PcieAriCapabilityData.BusNumber = (UINT8)(5 + Engine->Type.Port.Address.Address.Bus);
    PcieAriCapabilityData.ScanData.StdHeader = NULL;
    PcieAriCapabilityData.ScanData.GnbScanCallback = PcieGetAriCapabilityCallback;
    GnbLibPciScan (Engine->Type.Port.Address, Engine->Type.Port.Address, &PcieAriCapabilityData.ScanData);
    if (PcieAriCapabilityData.AriCapabilityFound == TRUE) {
      PcieAriCapabilityData.AriCapabilityFound = FALSE;
      PcieAriCapabilityData.BusNumber = (UINT8)(5 + Engine->Type.Port.Address.Address.Bus);
      PcieAriCapabilityData.ScanData.StdHeader = NULL;
      PcieAriCapabilityData.ScanData.GnbScanCallback = PcieSetAriCapabilityCallback;
      GnbLibPciScan (Engine->Type.Port.Address, Engine->Type.Port.Address, &PcieAriCapabilityData.ScanData);
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "  PcieAriInitCallback Exit\n");
}

/**----------------------------------------------------------------------------------------*/
/**
 * Interface to configure AriCapabilitySize on PCIE interface
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 * @retval    AGESA_STATUS
 */
 /*----------------------------------------------------------------------------------------*/

VOID
PcieAriInit (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  PCIe_ENGINE_CONFIG   *PcieEngine;
  PCIe_WRAPPER_CONFIG  *PcieWrapper;
  BOOLEAN              PcieAriSupportEnable;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAriInit Enter\n");
  PcieAriSupportEnable = PcdGetBool (PcdCfgPcieAriSupport);
  if (PcieAriSupportEnable) {
    PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
    while (PcieWrapper != NULL) {
      PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
      while (PcieEngine != NULL) {
        if (PcieLibIsEngineAllocated (PcieEngine)) {
          PcieAriInitCallback (PcieEngine, NULL);
        }
        PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
      }
      PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAriInit Exit\n");
}

