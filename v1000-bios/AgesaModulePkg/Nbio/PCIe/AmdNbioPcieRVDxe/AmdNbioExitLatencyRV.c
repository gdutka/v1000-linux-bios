/* $NoKeywords:$ */
/**
 * @file
 *
 * Service procedure to Set PCIe topology segment L0s exit latency
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
#include <GnbDxio.h>
#include <OptionGnb.h>
#include <Filecode.h>
#include <Library/IdsLib.h>
#include <Library/GnbPcieConfigLib.h>
#include <Library/GnbCommonLib.h>
#include <Library/NbioHandleLib.h>
#include <GnbRegistersRV.h>
#include <Library/NbioRegisterAccLib.h>

#define FILECODE NBIO_PCIE_AMDNBIOPCIERVDXE_AMDNBIOEXITLATENCYRV_FILECODE

//<Embedded_Override_Start>
/*----------------------------------------------------------------------------------------*/
/**
 * FALSE - RV2
 * TRUE  - RV
 */
BOOLEAN
CheckRvOpn (
  )
{
  UINT32  TempData32;

  TempData32 = (PcdGet32 (PcdGetRVFamilyOPN) >> 30) & 0x3;

  if ((TempData32 == RV1_SINGLE_SOURCE) || (TempData32 == RV1_B4_DUAL_SOURCE)) {
    return TRUE;
  }
  return FALSE;
}
//<Embedded_Override_End>
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

typedef struct {
  GNB_PCI_SCAN_DATA ScanData;
  PCI_ADDR          DownstreamPort;
  UINT16            L1ExitLatency;
  BOOLEAN           BlackList;
} PCIE_EXIT_LATENCY_DATA;

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
  IN       PCIE_EXIT_LATENCY_DATA *LinkL1ExitLatency
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
      break;
    }
  }
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
PcieL1ExitLatencyCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  SCAN_STATUS             ScanStatus;
  PCIE_EXIT_LATENCY_DATA  *PcielinkData;
  PCIE_DEVICE_TYPE        DeviceType;

  ScanStatus = SCAN_SUCCESS;
  PcielinkData = (PCIE_EXIT_LATENCY_DATA *) ScanData;
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
PcieLinkL1ExitLatency (
  IN       PCIe_ENGINE_CONFIG      *Engine,
  IN OUT   UINT16                  *L1ExitLatency
  )
{
  PCIE_EXIT_LATENCY_DATA PcieL1ExitLatencyData;

  PcieL1ExitLatencyData.ScanData.StdHeader = NULL;
  PcieL1ExitLatencyData.ScanData.GnbScanCallback = PcieL1ExitLatencyCallback;
  PcieL1ExitLatencyData.L1ExitLatency = 0xFFFF;
  PcieL1ExitLatencyData.DownstreamPort = Engine->Type.Port.Address;
  PcieL1ExitLatencyData.BlackList = FALSE;
  GnbLibPciScan (Engine->Type.Port.Address, Engine->Type.Port.Address, &PcieL1ExitLatencyData.ScanData);
  *L1ExitLatency = PcieL1ExitLatencyData.L1ExitLatency;

  return PcieL1ExitLatencyData.BlackList;
}

/**----------------------------------------------------------------------------------------*/
/**
 * Interface to set L1 Exit Latency
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 */
 /*----------------------------------------------------------------------------------------*/
VOID
PcieL1ExitLatencyInterface (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  UINT32                 Index;
  UINT32                 Value;
  UINT16                 L1ExitLatency;
  PCIe_ENGINE_CONFIG     *PcieEngine;
  PCIe_WRAPPER_CONFIG    *PcieWrapper;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieL1ExitLatencyInterface Enter\n");
  L1ExitLatency = 0xFFFF;
  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      if (PcieLibIsEngineAllocated (PcieEngine) && PcieConfigCheckPortStatus (PcieEngine, INIT_STATUS_PCIE_TRAINING_SUCCESS)) {
        if (TRUE == PcieLinkL1ExitLatency (PcieEngine, (UINT16*)&L1ExitLatency)) {
          if (L1ExitLatency != 0xFFFF) {
            IDS_HDT_CONSOLE (GNB_TRACE, "  Found Low power PT Device, set L1ExitLatency = %d to Device = %d:%d:%d\n", 
                              L1ExitLatency,
                              PcieEngine->Type.Port.Address.Address.Bus,
                              PcieEngine->Type.Port.Address.Address.Device,
                              PcieEngine->Type.Port.Address.Address.Function
                              );
            //<Embedded_Override_Start>
            //STRAP_BIF_L1_EXIT_LATENCY_A offset values are different for RV1 and RV2.Update the strap index value accordingly
            if(CheckRvOpn()) {
              Index = ( FUSE_ACCESS_LOCATION + STRAP_BIF_L1_EXIT_LATENCY_A_OFFSET_RV1 ) + (PcieEngine->Type.Port.PortId * 0x51);
            }
            else {
              Index = ( FUSE_ACCESS_LOCATION + STRAP_BIF_L1_EXIT_LATENCY_A_OFFSET_RV2 ) + (PcieEngine->Type.Port.PortId * 0x51);
            }
            //<Embedded_Override_End>
            NbioRegisterWrite (GnbHandle,
                               TYPE_SMN,
                               PCIE0_STRAP_INDEX_ADDRESS,
                               &Index,
                               0
                               );
            Value = L1ExitLatency;
            NbioRegisterWrite (GnbHandle,
                               TYPE_SMN,
                               PCIE0_STRAP_DATA_ADDRESS,
                               &Value,
                               0
                               );
          }
        }
      }
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }
    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieL1ExitLatencyInterface Exit\n");
}

