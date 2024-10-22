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
#include  "OptionGnb.h"
#include  "GnbFamServices.h"
#include  "PcieAriInitCZ.h"
#include  "Library/AmdHeapLib.h"

#define FILECODE LIBRARY_GNBCZLIB_MODULES_GNBINITCZ_PCIEARIINITCZ_FILECODE

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
  BOOLEAN                 AriCapabilityNotFound;
} PCIE_ARI_CAPABILITY_DATA;

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
  PCIE_ARI_CAPABILITY_DATA   *PcieAriCapabilityData;
  PCIE_DEVICE_TYPE        DeviceType;
  UINT16                   PcieCapPtr;

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
    if (PcieCapPtr == 0) {
      IDS_HDT_CONSOLE (GNB_TRACE, "  ARI Capability NOT FOUND\n");
      PcieAriCapabilityData->AriCapabilityNotFound = TRUE;
    } else {
      IDS_HDT_CONSOLE (GNB_TRACE, "  ARI Capability FOUND\n");
      PcieAriCapabilityData->AriCapabilityFound = TRUE;
    }
    break;
  case  PcieDeviceRootComplex:
  case  PcieDeviceDownstreamPort:
  case  PcieDeviceUpstreamPort:
    GnbLibPciScanSecondaryBus (Device, &PcieAriCapabilityData->ScanData);
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
 * @param[in]       Pcie            Pointer to global PCIe configuration
 *
 */

VOID
STATIC
PcieAriInitCallbackCZ (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PCIE_ARI_CAPABILITY_DATA  PcieAriCapabilityData;

  IDS_HDT_CONSOLE (GNB_TRACE, "  PcieAriInitCallbackCZ for Device = %d:%d:%d\n",
    Engine->Type.Port.Address.Address.Bus,
    Engine->Type.Port.Address.Address.Device,
    Engine->Type.Port.Address.Address.Function
    );
  if ((!PcieConfigIsSbPcieEngine (Engine)) &&
      (PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS))) {
    PcieAriCapabilityData.AriCapabilityFound = FALSE;
    PcieAriCapabilityData.AriCapabilityNotFound = FALSE;
    PcieAriCapabilityData.ScanData.StdHeader = GnbLibGetHeader (Pcie);
    PcieAriCapabilityData.ScanData.GnbScanCallback = PcieGetAriCapabilityCallback;
    GnbLibPciScan (Engine->Type.Port.Address, Engine->Type.Port.Address, &PcieAriCapabilityData.ScanData);
    if ((PcieAriCapabilityData.AriCapabilityFound == TRUE) &&
        (PcieAriCapabilityData.AriCapabilityNotFound == FALSE)) {
      IDS_HDT_CONSOLE (GNB_TRACE, "  Enable Ari for Device = %d:%d:%d\n"),
      GnbLibPciRMW (
        Engine->Type.Port.Address.AddressValue | 0x80,
        AccessWidth32,
        0xFFFFFFFF,
        BIT5,
        GnbLibGetHeader (Pcie)
      );
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "  PcieAriInitCallbackCZ Exit\n");
}

/**----------------------------------------------------------------------------------------*/
/**
 * Interface to configure AriCapabilitySize on PCIE interface
 *
 *
 *
 * @param[in] StdHeader   Standard configuration header
 * @param[in] Pcie        Pointer to global PCIe configuration
 *
 * @retval    AGESA_STATUS
 */
 /*----------------------------------------------------------------------------------------*/
AGESA_STATUS
PcieAriInitCZ (
  IN       AMD_CONFIG_PARAMS     *StdHeader,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  BOOLEAN   AriSupportEnable;
  GNB_BUILD_OPTIONS_CZ      *GnbBuildOptionData;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAriInitCZ Enter\n");

  GnbBuildOptionData = GnbLocateHeapBuffer (AMD_GNB_BUILD_OPTIONS_HANDLE, StdHeader);
  ASSERT (GnbBuildOptionData != NULL);

  AriSupportEnable = GnbBuildOptionData->CfgAriSupport;
  if (AriSupportEnable == TRUE) {
    PcieConfigRunProcForAllEngines (
      DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
      PcieAriInitCallbackCZ,
      NULL,
      Pcie
      );
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAriInitCZ Exit\n");
  return  AGESA_SUCCESS;
}


