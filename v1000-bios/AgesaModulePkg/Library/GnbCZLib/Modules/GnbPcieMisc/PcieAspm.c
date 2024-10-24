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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "Gnb.h"
#include  "GnbPcieConfig.h"
#include  "OptionGnb.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieInitLib.h"
#include  "PcieAspmBlackList.h"

#define FILECODE LIBRARY_GNBCZLIB_MODULES_GNBPCIEMISC_PCIEASPM_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern GNB_BUILD_OPTIONS GnbBuildOptions;
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

typedef struct {
  GNB_PCI_SCAN_DATA       ScanData;
  PCIE_ASPM_TYPE          Aspm;
  PCI_ADDR                DownstreamPort;
  BOOLEAN                 AspmL0sBlackList;
} PCIE_ASPM_DATA;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


SCAN_STATUS
PcieAspmCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  );

VOID
PcieAspmEnableOnLink (
  IN       PCI_ADDR                 Downstream,
  IN       PCI_ADDR                 Upstream,
  IN       PCIE_ASPM_TYPE           Aspm,
  IN OUT   BOOLEAN                  *AspmL0sBlackList,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

VOID
PcieAspmEnableOnDevice (
  IN       PCI_ADDR             Device,
  IN       PCIE_ASPM_TYPE       Aspm,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

PCIE_ASPM_TYPE
PcieAspmGetPmCapability (
  IN       PCI_ADDR                 Device,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

BOOLEAN
STATIC
PcieAspmOptionalCompliance (
  IN       PCI_ADDR                 Device,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );


/*----------------------------------------------------------------------------------------*/
/**
 * Enable PCIE Advance state power management
 *
 *
 *
 * @param[in] DownstreamPort      PCI Address of the downstream port
 * @param[in] Aspm                ASPM type
 * @param[in] StdHeader           Standard configuration header
 * @retval     AGESA_STATUS
 */

VOID
PcieLinkAspmEnable (
  IN       PCI_ADDR                DownstreamPort,
  IN       PCIE_ASPM_TYPE          Aspm,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  PCIE_ASPM_DATA PcieAspmData;
  PcieAspmData.Aspm = Aspm;
  PcieAspmData.ScanData.StdHeader = StdHeader;
  PcieAspmData.ScanData.GnbScanCallback = PcieAspmCallback;
  PcieAspmData.AspmL0sBlackList = FALSE;
  GnbLibPciScan (DownstreamPort, DownstreamPort, &PcieAspmData.ScanData);
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
PcieAspmCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  SCAN_STATUS             ScanStatus;
  PCIE_ASPM_DATA          *PcieAspmData;
  PCIE_DEVICE_TYPE        DeviceType;
  ScanStatus = SCAN_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "  PcieAspmCallback for Device = %d:%d:%d\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function
    );
  PcieAspmData = (PCIE_ASPM_DATA *) ScanData;
  ScanStatus = SCAN_SUCCESS;
  DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);
  switch (DeviceType) {
  case  PcieDeviceRootComplex:
  case  PcieDeviceDownstreamPort:
    PcieAspmData->DownstreamPort = Device;
    //PcieExitLatencyData->LinkCount++;
    GnbLibPciRMW (Device.AddressValue | 0x18, AccessS3SaveWidth32, 0xffffffffull, 0x0, ScanData->StdHeader);
    GnbLibPciScanSecondaryBus (Device, &PcieAspmData->ScanData);
    //PcieExitLatencyData->LinkCount--;

    //Pcie ASPM Black List for L0s with HW method change
    if ((DeviceType == PcieDeviceRootComplex) && (PcieAspmData->AspmL0sBlackList == TRUE)) {
      IDS_HDT_CONSOLE (GNB_TRACE, "  Black List L0s disabled = %d:%d:%d\n", Device.Address.Bus, Device.Address.Device, Device.Address.Function);
      GnbLibPciIndirectRMW (Device.AddressValue | 0xE0, 0xA0, AccessS3SaveWidth32, 0xfffff0ff, 0, ScanData->StdHeader);
    }
    break;
  case  PcieDeviceUpstreamPort:
    PcieAspmEnableOnLink (
      PcieAspmData->DownstreamPort,
      Device,
      PcieAspmData->Aspm,
      &PcieAspmData->AspmL0sBlackList,
      ScanData->StdHeader
      );
    GnbLibPciRMW (Device.AddressValue | 0x18, AccessS3SaveWidth32, 0xffffffffull, 0x0, ScanData->StdHeader);
    GnbLibPciScanSecondaryBus (Device, &PcieAspmData->ScanData);
    ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
    break;
  case  PcieDeviceEndPoint:
  case  PcieDeviceLegacyEndPoint:
    PcieAspmEnableOnLink (
      PcieAspmData->DownstreamPort,
      Device,
      PcieAspmData->Aspm,
      &PcieAspmData->AspmL0sBlackList,
      ScanData->StdHeader
      );
    ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
    break;
  default:
    break;
  }
  return ScanStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set  ASPM State on PCIe device function
 *
 *
 *
 * @param[in] Function    PCI address of function.
 * @param[in] Aspm                Aspm capability to enable
 * @param[in] StdHeader           Standard configuration header
 *
 */
 /*----------------------------------------------------------------------------------------*/
VOID
PcieAspmEnableOnFunction (
  IN       PCI_ADDR             Function,
  IN       PCIE_ASPM_TYPE       Aspm,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  UINT8       PcieCapPtr;
  PcieCapPtr = GnbLibFindPciCapability (Function.AddressValue, PCIE_CAP_ID, StdHeader);
  if (PcieCapPtr != 0) {
    GnbLibPciRMW (
      Function.AddressValue | (PcieCapPtr + PCIE_LINK_CTRL_REGISTER) ,
      AccessS3SaveWidth8,
      ~(UINT32) (BIT0 | BIT1),
      Aspm,
      StdHeader
      );
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set ASPM State on all function of PCI device
 *
 *
 *
 * @param[in] Device    PCI address of device.
 * @param[in] Aspm                Aspm capability to enable
 * @param[in] StdHeader           Standard configuration header
 *
 */
 /*----------------------------------------------------------------------------------------*/
VOID
PcieAspmEnableOnDevice (
  IN       PCI_ADDR             Device,
  IN       PCIE_ASPM_TYPE       Aspm,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  UINT8 MaxFunc;
  UINT8 CurrentFunc;
  MaxFunc = GnbLibPciIsMultiFunctionDevice (Device.AddressValue, StdHeader) ? 7 : 0;
  for (CurrentFunc = 0; CurrentFunc <= MaxFunc; CurrentFunc++) {
    Device.Address.Function = CurrentFunc;
    if (GnbLibPciIsDevicePresent (Device.AddressValue, StdHeader)) {
      PcieAspmEnableOnFunction (Device, Aspm, StdHeader);
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 *  Enable ASPM on link
 *
 *
 *
 * @param[in]       Downstream          PCI Address of downstrteam port
 * @param[in]       Upstream            PCI Address of upstream port
 * @param[in]       Aspm                Aspm capability to enable
 * @param[in, out]  AspmL0sBlackList    AspmBlackList
 * @param[in]       StdHeader           Standard configuration header
 */

VOID
PcieAspmEnableOnLink (
  IN       PCI_ADDR                 Downstream,
  IN       PCI_ADDR                 Upstream,
  IN       PCIE_ASPM_TYPE           Aspm,
  IN OUT   BOOLEAN                  *AspmL0sBlackList,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{
  PCIe_LINK_ASPM  LinkAspm;
  PCIE_ASPM_TYPE  DownstreamCap;
  PCIE_ASPM_TYPE  UpstreamCap;
  BOOLEAN         UpstreamOpComp;

  LinkAspm.DownstreamPort = Downstream;
  DownstreamCap = PcieAspmGetPmCapability (Downstream, StdHeader);
  UpstreamOpComp = PcieAspmOptionalCompliance (Upstream, StdHeader);
  LinkAspm.UpstreamPort = Upstream;
  UpstreamCap = PcieAspmGetPmCapability (Upstream, StdHeader);
  LinkAspm.DownstreamAspm = DownstreamCap & UpstreamCap & Aspm;
  LinkAspm.UpstreamAspm = LinkAspm.DownstreamAspm;
  LinkAspm.RequestedAspm = Aspm;
  if (!UpstreamOpComp) {
    LinkAspm.UpstreamAspm &= AspmL1;
    if ((UpstreamCap & Aspm & AspmL0s) != 0) {
      LinkAspm.UpstreamAspm |= AspmL0s;
    }
    LinkAspm.DownstreamAspm &= AspmL1;
    if ((DownstreamCap & Aspm & AspmL0s) != 0) {
      LinkAspm.DownstreamAspm |= AspmL0s;
    }
  }
  if (GnbBuildOptions.PcieAspmBlackListEnable == 1) {
    PcieAspmBlackListFeature (&LinkAspm, StdHeader);
    if ((LinkAspm.DownstreamAspm & AspmL0s) == 0) {
      *AspmL0sBlackList = LinkAspm.BlackList;
    }
  }
  //AgesaPcieLinkAspm (&LinkAspm, StdHeader);
  IDS_HDT_CONSOLE (GNB_TRACE, "  Set ASPM [%d]  for Device = %d:%d:%d\n",
    (LinkAspm.UpstreamAspm) ,
    LinkAspm.UpstreamPort.Address.Bus,
    LinkAspm.UpstreamPort.Address.Device,
    LinkAspm.UpstreamPort.Address.Function
    );
  IDS_HDT_CONSOLE (GNB_TRACE, "  Set ASPM [%d]  for Device = %d:%d:%d\n",
    (LinkAspm.DownstreamAspm) ,
    LinkAspm.DownstreamPort.Address.Bus,
    LinkAspm.DownstreamPort.Address.Device,
    LinkAspm.DownstreamPort.Address.Function
    );
  // Disable ASPM  Downstream component
  PcieAspmEnableOnDevice (Upstream, AspmDisabled, StdHeader);
  // Enable  ASPM  Upstream component
  PcieAspmEnableOnFunction (Downstream, LinkAspm.DownstreamAspm, StdHeader);
  // Enable  ASPM  Downstream component
  PcieAspmEnableOnDevice (Upstream, LinkAspm.UpstreamAspm, StdHeader);
}



/**----------------------------------------------------------------------------------------*/
/**
 * Port/Endpoint ASPM capability
 *
 *
 *
 * @param[in] Device      PCI address of downstream port
 * @param[in] StdHeader   Standard configuration header
 *
 * @retval    PCIE_ASPM_TYPE
 */
 /*----------------------------------------------------------------------------------------*/
PCIE_ASPM_TYPE
PcieAspmGetPmCapability (
  IN       PCI_ADDR                 Device,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{
  UINT8   PcieCapPtr;
  UINT32  Value;
  PcieCapPtr = GnbLibFindPciCapability (Device.AddressValue, PCIE_CAP_ID, StdHeader);
  if (PcieCapPtr == 0) {
    return 0;
  }
  GnbLibPciRead (
    Device.AddressValue | (PcieCapPtr + PCIE_LINK_CAP_REGISTER),
    AccessWidth32,
    &Value,
    StdHeader
    );
  return (Value >> 10) & 3;
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
PcieAspmPortInitCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  if (Engine->Type.Port.PortData.LinkAspm != AspmDisabled &&
      !PcieConfigIsSbPcieEngine (Engine) &&
      PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS)) {
    PcieLinkAspmEnable (
      Engine->Type.Port.Address,
      Engine->Type.Port.PortData.LinkAspm,
      GnbLibGetHeader (Pcie)
      );
  }
}


/**----------------------------------------------------------------------------------------*/
/**
 * Interface to enable Clock Power Managment
 *
 *
 *
 * @param[in] StdHeader   Standard configuration header
 *
 * @retval    AGESA_STATUS
 */
 /*----------------------------------------------------------------------------------------*/
AGESA_STATUS
PcieAspmInterface (
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{
  AGESA_STATUS          AgesaStatus;
  PCIe_PLATFORM_CONFIG  *Pcie;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAspmInterface Enter\n");
  AgesaStatus = PcieLocateConfigurationData (StdHeader, &Pcie);
  if (AgesaStatus == AGESA_SUCCESS) {
    PcieConfigRunProcForAllEngines (
      DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
      PcieAspmPortInitCallback,
      NULL,
      Pcie
      );
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAspmInterface Exit [0x%x]\n", AgesaStatus);
  return  AgesaStatus;
}

/**----------------------------------------------------------------------------------------*/
/**
 * Port/Endpoint ASPM optionality compliance
 *
 *
 *
 * @param[in] Device      PCI address of downstream port
 * @param[in] StdHeader   Standard configuration header
 *
 * @retval    BOOLEAN
 */
 /*----------------------------------------------------------------------------------------*/
BOOLEAN
STATIC
PcieAspmOptionalCompliance (
  IN       PCI_ADDR                 Device,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{
  UINT8   PcieCapPtr;
  UINT32  Value;
  PcieCapPtr = GnbLibFindPciCapability (Device.AddressValue, PCIE_CAP_ID, StdHeader);
  if (PcieCapPtr == 0) {
    return 0;
  }
  GnbLibPciRead (
    Device.AddressValue | (PcieCapPtr + PCIE_LINK_CAP_REGISTER),
    AccessWidth32,
    &Value,
    StdHeader
    );
  return (BOOLEAN) ((Value >> 22) & 1);
}

