/* $NoKeywords:$ */
/**
 * @file
 *
 * Configure Max Payload
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision:
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
#include  "GnbPcieConfig.h"
#include  "PcieMaxPayload.h"
#include  "OptionGnb.h"
#include  "GnbFamServices.h"

#define FILECODE LIBRARY_GNBCZLIB_MODULES_GNBPCIEMISC_PCIEMAXPAYLOAD_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern GNB_BUILD_OPTIONS  GnbBuildOptions;


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
typedef struct {
  GNB_PCI_SCAN_DATA       ScanData;
  UINT8                   MaxPayload;
} PCIE_MAX_PAYLOAD_DATA;


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


SCAN_STATUS
PcieGetMaxPayloadCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  );

SCAN_STATUS
PcieSetMaxPayloadCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  );

AGESA_STATUS
PciePayloadBlackListFeature (
  IN       PCI_ADDR               Device,
  IN       UINT8                  *MaxPayload,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Determine maximum payload size for PCIe segment
 *
 *  Scan through all link in segment to determine maximum payload by EPs.
 *
 * @param[in]   DownstreamPort    PCI address of PCIe port
 * @param[in]   EngineMaxPayload  MaxPayload supported by the engine
 * @param[in]   StdHeader         Standard configuration header
 *
 */

VOID
PcieSetMaxPayload (
  IN       PCI_ADDR                DownstreamPort,
  IN       UINT8                   EngineMaxPayload,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  PCIE_MAX_PAYLOAD_DATA  PcieMaxPayloadData;

  IDS_HDT_CONSOLE (GNB_TRACE, "  PcieSetMaxPayload for Device = %d:%d:%d\n",
    DownstreamPort.Address.Bus,
    DownstreamPort.Address.Device,
    DownstreamPort.Address.Function
    );
  PcieMaxPayloadData.MaxPayload = EngineMaxPayload;
  PcieMaxPayloadData.ScanData.StdHeader = StdHeader;
  PcieMaxPayloadData.ScanData.GnbScanCallback = PcieGetMaxPayloadCallback;
  GnbLibPciScan (DownstreamPort, DownstreamPort, &PcieMaxPayloadData.ScanData);
  PcieMaxPayloadData.ScanData.GnbScanCallback = PcieSetMaxPayloadCallback;
  GnbLibPciScan (DownstreamPort, DownstreamPort, &PcieMaxPayloadData.ScanData);
  IDS_HDT_CONSOLE (GNB_TRACE, "  PcieSetMaxPayloadExit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Evaluate device Max Payload - save SMALLEST Max Payload for PCIe Segment
 *
 *
 *
 * @param[in]     Device          PCI Address
 * @param[in,out] ScanData        Scan configuration data
 * @retval                        Scan Status of 0
 */

SCAN_STATUS
PcieGetMaxPayloadCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  PCIE_MAX_PAYLOAD_DATA   *PcieMaxPayloadData;
  PCIE_DEVICE_TYPE        DeviceType;
  UINT32                  Value;
  UINT8                   PcieCapPtr;
  UINT8                   DeviceMaxPayload;

  PcieMaxPayloadData = (PCIE_MAX_PAYLOAD_DATA*) ScanData;
  IDS_HDT_CONSOLE (GNB_TRACE, "  PcieGetMaxPayloadCallback for Device = %d:%d:%d\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function
    );
  PcieCapPtr = GnbLibFindPciCapability (Device.AddressValue, PCIE_CAP_ID, ScanData->StdHeader);
  if (PcieCapPtr != 0) {
    GnbLibPciRead (
      Device.AddressValue | (PcieCapPtr + PCIE_DEVICE_CAP_REGISTER),
      AccessWidth32,
      &Value,
      ScanData->StdHeader
    );
    DeviceMaxPayload = (UINT8) (Value & 0x7);
    PciePayloadBlackListFeature (Device, &DeviceMaxPayload, ScanData->StdHeader);
    IDS_HDT_CONSOLE (GNB_TRACE, "  Found DeviceMaxPayload as %d (Value = %x\n", DeviceMaxPayload, Value);
    if (DeviceMaxPayload < PcieMaxPayloadData->MaxPayload) {
      PcieMaxPayloadData->MaxPayload = DeviceMaxPayload;
    }
  }
  DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);
  switch (DeviceType) {
  case  PcieDeviceRootComplex:
  case  PcieDeviceDownstreamPort:
  case  PcieDeviceUpstreamPort:
    GnbLibPciScanSecondaryBus (Device, &PcieMaxPayloadData->ScanData);
    break;
  case  PcieDeviceEndPoint:
  case  PcieDeviceLegacyEndPoint:
    break;
  default:
    break;
  }
  return SCAN_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Configure the Max Payload setting to all devices in the PCIe Segment
 *
 *
 *
 * @param[in]     Device          PCI Address
 * @param[in,out] ScanData        Scan configuration data
 * @retval                        Scan Status of 0
 */

SCAN_STATUS
PcieSetMaxPayloadCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  PCIE_MAX_PAYLOAD_DATA   *PcieMaxPayloadData;
  PCIE_DEVICE_TYPE        DeviceType;
  UINT8                   PcieCapPtr;

  PcieMaxPayloadData = (PCIE_MAX_PAYLOAD_DATA*) ScanData;
  IDS_HDT_CONSOLE (GNB_TRACE, "  PcieSetMaxPayloadCallback for Device = %d:%d:%d to %d\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function,
    PcieMaxPayloadData->MaxPayload
    );
  PcieCapPtr = GnbLibFindPciCapability (Device.AddressValue, PCIE_CAP_ID, ScanData->StdHeader);
  if (PcieCapPtr != 0) {
    GnbLibPciRMW (
      Device.AddressValue | (PcieCapPtr + PCIE_DEVICE_CTRL_REGISTER),
      AccessWidth32,
      ~(UINT32) (0x7 << 5),
      ((UINT32)PcieMaxPayloadData->MaxPayload << 5),
      ScanData->StdHeader
    );
  }
  DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);
  switch (DeviceType) {
  case  PcieDeviceRootComplex:
  case  PcieDeviceDownstreamPort:
  case  PcieDeviceUpstreamPort:
    GnbLibPciScanSecondaryBus (Device, &PcieMaxPayloadData->ScanData);
    break;
  case  PcieDeviceEndPoint:
  case  PcieDeviceLegacyEndPoint:
    break;
  default:
    break;
  }
  return SCAN_SUCCESS;
}

UINT16  PayloadBlacklistDeviceTable[] = {
  0x1969, 0x1083, (UINT16) MAX_PAYLOAD_128,
  0x1B21, 0x0612, (UINT16) MAX_PAYLOAD_128
};

/*----------------------------------------------------------------------------------------*/
/**
 * Pcie Max_Payload_Size Black List
 *
 *
 *
 * @param[in] Device              PCI_ADDR of PCIe Device to evaluate
 * @param[in] MaxPayload          Pointer to Max_Payload_Size value
 * @param[in] StdHeader           Standard configuration header
 * @retval     AGESA_STATUS
 */

AGESA_STATUS
PciePayloadBlackListFeature (
  IN       PCI_ADDR               Device,
  IN       UINT8                  *MaxPayload,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32  TargetDeviceId;
  UINTN   i;
  UINT32  DeviceId;
  UINT32  VendorId;

  GnbLibPciRead (Device.AddressValue, AccessWidth32, &TargetDeviceId, StdHeader);
  for (i = 0; i < (sizeof (PayloadBlacklistDeviceTable) / sizeof (UINT16)); i = i + 3) {
    VendorId = PayloadBlacklistDeviceTable[i];
    DeviceId = PayloadBlacklistDeviceTable[i + 1];
    if (VendorId == (UINT16)TargetDeviceId) {
      if (DeviceId == 0xFFFF || DeviceId == (TargetDeviceId >> 16)) {
        *MaxPayload = (UINT8) PayloadBlacklistDeviceTable[i + 2];
      }
    }
  }
  return AGESA_SUCCESS;
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
 * @param[in]       Pcie            Pointer to global PCIe configuration
 *
 */

VOID
STATIC
PcieMaxPayloadInitCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PCIE_MAXPAYLOAD_SERVICE   *PcieMaxPayloadProtocol;
  UINT8                     EngineMaxPayload;
  AGESA_STATUS              Status;
  PCIe_COMPLEX_CONFIG       *Complex;



  if ((GnbBuildOptions.CfgMaxPayloadEnable != 0) &&
      (!PcieConfigIsSbPcieEngine (Engine)) &&
      (PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS))) {
    EngineMaxPayload =  MAX_PAYLOAD;
    Complex = (PCIe_COMPLEX_CONFIG *) PcieConfigGetParent (DESCRIPTOR_COMPLEX, &Engine->Header);
    Status = GnbLibLocateService (GnbPcieMaxPayloadService, Complex->SocketId, &PcieMaxPayloadProtocol, GnbLibGetHeader (Pcie));
    if (Status ==  AGESA_SUCCESS) {
      EngineMaxPayload = PcieMaxPayloadProtocol->SetMaxPayload (Engine);
    }
    PcieSetMaxPayload (Engine->Type.Port.Address, EngineMaxPayload, GnbLibGetHeader (Pcie));
  }
}

/**----------------------------------------------------------------------------------------*/
/**
 * Interface to configure MaxPayloadSize on PCIE interface
 *
 *
 *
 * @param[in] StdHeader   Standard configuration header
 *
 * @retval    AGESA_STATUS
 */
 /*----------------------------------------------------------------------------------------*/
AGESA_STATUS
PcieMaxPayloadInterface (
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{
  AGESA_STATUS          AgesaStatus;
  PCIe_PLATFORM_CONFIG  *Pcie;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMaxPayloadInterface Enter\n");
  AgesaStatus = PcieLocateConfigurationData (StdHeader, &Pcie);
  if (AgesaStatus == AGESA_SUCCESS) {
    PcieConfigRunProcForAllEngines (
      DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
      PcieMaxPayloadInitCallback,
      NULL,
      Pcie
      );
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMaxPayloadInterface Exit [0x%x]\n", AgesaStatus);
  return  AgesaStatus;
}


