/* $NoKeywords:$ */
/**
 * @file
 *
 * DxioCfgPointsRV - Configuration entry points for the DXIO subsystem
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdNbioBaseRVPei
 * @e \$Revision: 312065 $   @e \$Date: 2015-01-30 04:23:05 -0600 (Fri, 30 Jan 2015) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include <AMD.h>
#include <Gnb.h>
#include <PiPei.h>
#include <Filecode.h>
#include <GnbDxio.h>
#include <GnbRegistersRV.h>

//From AgesaPkg
#include <AmdPcieComplex.h>
#include <Ppi/NbioPcieComplexPpi.h>


#include "AmdNbioPcieRVPei.h"

#include <Guid/GnbPcieInfoHob.h>
#include <Ppi/NbioPcieServicesPpi.h>
#include <Ppi/NbioPcieTrainingPpi.h>
#include <Ppi/NbioSmuServicesPpi.h>
#include <Ppi/SocLogicalIdPpi.h>
#include <Library/PeiServicesLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/NbioHandleLib.h>
//Dependent on AgesaPkg
#include <Library/DxioLibV1.h>
#include <IdsHookId.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioAzaliaLib.h>
#include <Library/GnbPciAccLib.h>
#include <Library/GnbLib.h>
#include <CcxRegistersRv.h>

#define FILECODE        NBIO_PCIE_AMDNBIOPCIERVPEI_DXIOCFGPOINTSRV_FILECODE

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

// Comment this line to enable extended debug output if tracing is enabled
#undef GNB_TRACE_ENABLE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                    P P I   N O T I F Y   D E S C R I P T O R S
 *----------------------------------------------------------------------------------------
 */
BOOLEAN CheckRvOpn(VOID);

//=========================================================================================
// Timepoint after port mapping and before reconfig
//=========================================================================================

/*----------------------------------------------------------------------------------------*/
/**
 * Per-Engine Callback for
 *
 *
 *
 * @param[in]     Engine  Engine configuration info
 * @param[in,out] Buffer  Buffer pointer
 * @param[in]     Pcie    PCIe configuration info
 */
VOID
STATIC
DxioCfgBeforeReconfigCallback (
  IN       GNB_HANDLE            *GnbHandle,
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer
  )
{
  IDS_HDT_CONSOLE (GNB_TRACE, "DxioCfgBeforeReconfigCallback Enter\n");
  return;
}

/**----------------------------------------------------------------------------------------*/
/**
 * Interface to configure DXIO/PCIe ports after ports are mapped and before reconfig
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 * @retval    AGESA_STATUS
 */
 /*----------------------------------------------------------------------------------------*/

VOID
DxioCfgBeforeReconfig (
  IN       GNB_HANDLE       *GnbHandle
  )
{
  PCIe_ENGINE_CONFIG        *PcieEngine;
  PCIe_WRAPPER_CONFIG       *PcieWrapper;

  IDS_HDT_CONSOLE (GNB_TRACE, "DxioCfgBeforeReconfig Enter\n");
  IDS_HOOK (IDS_HOOK_NBIO_PCIE_TOPOLOGY, (VOID *) GnbHandle, (VOID *) NULL);

  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      DxioCfgBeforeReconfigCallback (GnbHandle, PcieEngine, NULL );
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }
    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }
  return;
}


//=========================================================================================
// Timepoint after reconfig and before port training
//=========================================================================================

/*----------------------------------------------------------------------------------------*/
/**
 * Per-Engine Callback for
 *
 *
 *
 * @param[in]     Engine  Engine configuration info
 * @param[in,out] Buffer  Buffer pointer
 * @param[in]     Pcie    PCIe configuration info
 */
VOID
STATIC
DxioCfgAfterReconfigCallback (
  IN       GNB_HANDLE            *GnbHandle,
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer
  )
{
  PCI_ADDR                            PciAddress;
  UINT16                              Index;
  PCIE_LANE_EQUALIZATION_CNTL_STRUCT  LaneEq;
  UINT32                              Value32;

  if (PcieLibIsEngineAllocated(Engine)) {
    IDS_HDT_CONSOLE (GNB_TRACE, "DxioCfgAfterReconfigCallback Enter\n");
    IDS_HDT_CONSOLE (GNB_TRACE, "DsTxPreset = 0x%x\n", Engine->Type.Port.LaneEqualizationCntl.DsTxPreset);
    IDS_HDT_CONSOLE (GNB_TRACE, "DsRxPresetHint = 0x%x\n", Engine->Type.Port.LaneEqualizationCntl.DsRxPresetHint);
    IDS_HDT_CONSOLE (GNB_TRACE, "UsTxPreset = 0x%x\n", Engine->Type.Port.LaneEqualizationCntl.UsTxPreset);
    IDS_HDT_CONSOLE (GNB_TRACE, "UsRxPresetHint = 0x%x\n", Engine->Type.Port.LaneEqualizationCntl.UsRxPresetHint);

    PciAddress.AddressValue = Engine->Type.Port.Address.AddressValue;
    PciAddress.Address.Register = 0x27C;

    if ((Engine->Type.Port.LaneEqualizationCntl.DsTxPreset != 0) &&
       (Engine->Type.Port.LaneEqualizationCntl.DsRxPresetHint != 0) &&
       (Engine->Type.Port.LaneEqualizationCntl.UsTxPreset != 0) &&
       (Engine->Type.Port.LaneEqualizationCntl.UsRxPresetHint != 0)) {
       // CBS override, no need to initial registers
    } else {
      if ((Engine->Type.Port.PortData.LinkSpeedCapability == PcieGen3) ||
        (Engine->Type.Port.PortData.LinkSpeedCapability == PcieGenMaxSupported)) {
        // Gen3 cap enabled
        Engine->Type.Port.LaneEqualizationCntl.DsTxPreset = 7;
        Engine->Type.Port.LaneEqualizationCntl.DsRxPresetHint = 3;
        Engine->Type.Port.LaneEqualizationCntl.UsTxPreset = 7;
        Engine->Type.Port.LaneEqualizationCntl.UsRxPresetHint = 3;
      }
    }
    GnbLibPciRead(PciAddress.AddressValue, AccessWidth16, &LaneEq.Value, NULL);
    LaneEq.Field.DOWNSTREAM_PORT_TX_PRESET = Engine->Type.Port.LaneEqualizationCntl.DsTxPreset;
    LaneEq.Field.DOWNSTREAM_PORT_RX_PRESET_HINT = Engine->Type.Port.LaneEqualizationCntl.DsRxPresetHint;
    LaneEq.Field.UPSTREAM_PORT_TX_PRESET = Engine->Type.Port.LaneEqualizationCntl.UsTxPreset;
    LaneEq.Field.UPSTREAM_PORT_RX_PRESET_HINT = Engine->Type.Port.LaneEqualizationCntl.UsRxPresetHint;
    Value32 = (UINT32) LaneEq.Value + ((UINT32) LaneEq.Value << 16);
    for (Index = 0; Index < 8; Index++) {
      IDS_HDT_CONSOLE (GNB_TRACE, "Writing Address = 0x%x\n", PciAddress.AddressValue);
      GnbLibPciWrite(PciAddress.AddressValue, AccessWidth32, &Value32, NULL);
      PciAddress.Address.Register += 4;
    }

    //
    //Update the STRAP_BIF_PCIE_LANE_EQUALIZATION_CNTL_DOWNSTREAM_PORT_RX_PRESET_HINT , STRAP_BIF_PCIE_LANE_EQUALIZATION_CNTL_UPSTREAM_PORT_RX_PRESET_HINT
    //STRAP_BIF_PCIE_LANE_EQUALIZATION_CNTL_DOWNSTREAM_PORT_TX_PRESET & STRAP_BIF_PCIE_LANE_EQUALIZATION_CNTL_UPSTREAM_PORT_TX_PRESET strap index offset values for RV1 an RV2.
    //
    //<Embedded_Override_Start>
    if(CheckRvOpn()) {
      Value32 = FUSE_ACCESS_LOCATION + STRAP_BIF_PCIE_LANE_EQUALIZATION_CNTL_DOWNSTREAM_PORT_RX_PRESET_HINT_OFFSET_RV1;
    } else {
      Value32 = FUSE_ACCESS_LOCATION + STRAP_BIF_PCIE_LANE_EQUALIZATION_CNTL_DOWNSTREAM_PORT_RX_PRESET_HINT_OFFSET_RV2;
    }
    //<Embedded_Override_End>
    NbioRegisterWrite (GnbHandle,
                    TYPE_SMN,
                    PCIE0_STRAP_INDEX_ADDRESS,
                    &Value32,
                    0
                    );
    Value32 = Engine->Type.Port.LaneEqualizationCntl.DsRxPresetHint;
    NbioRegisterWrite (GnbHandle,
                    TYPE_SMN,
                    PCIE0_STRAP_DATA_ADDRESS,
                    &Value32,
                    0
                    );

    //<Embedded_Override_Start>
    if(CheckRvOpn()) {
      Value32 = FUSE_ACCESS_LOCATION + STRAP_BIF_PCIE_LANE_EQUALIZATION_CNTL_UPSTREAM_PORT_RX_PRESET_HINT_OFFSET_RV1;
    } else {
      Value32 = FUSE_ACCESS_LOCATION + STRAP_BIF_PCIE_LANE_EQUALIZATION_CNTL_UPSTREAM_PORT_RX_PRESET_HINT_OFFSET_RV2;
    }
    //<Embedded_Override_End>
    NbioRegisterWrite (GnbHandle,
                      TYPE_SMN,
                      PCIE0_STRAP_INDEX_ADDRESS,
                      &Value32,
                      0
                      );
    Value32 = Engine->Type.Port.LaneEqualizationCntl.UsRxPresetHint;
    NbioRegisterWrite (GnbHandle,
                      TYPE_SMN,
                      PCIE0_STRAP_DATA_ADDRESS,
                      &Value32,
                      0
                      );

    //<Embedded_Override_Start>
    if(CheckRvOpn()) {
      Value32 = FUSE_ACCESS_LOCATION + STRAP_BIF_PCIE_LANE_EQUALIZATION_CNTL_DOWNSTREAM_PORT_TX_PRESET_OFFSET_RV1;
    } else {
      Value32 = FUSE_ACCESS_LOCATION + STRAP_BIF_PCIE_LANE_EQUALIZATION_CNTL_DOWNSTREAM_PORT_TX_PRESET_OFFSET_RV2;
    }
    //<Embedded_Override_End>
    NbioRegisterWrite (GnbHandle,
                      TYPE_SMN,
                      PCIE0_STRAP_INDEX_ADDRESS,
                      &Value32,
                      0
                      );
    Value32 = Engine->Type.Port.LaneEqualizationCntl.DsTxPreset;
    NbioRegisterWrite (GnbHandle,
                      TYPE_SMN,
                      PCIE0_STRAP_DATA_ADDRESS,
                      &Value32,
                      0
                      );

    //<Embedded_Override_Start>
    if(CheckRvOpn()) {
      Value32 = FUSE_ACCESS_LOCATION + STRAP_BIF_PCIE_LANE_EQUALIZATION_CNTL_UPSTREAM_PORT_TX_PRESET_OFFSET_RV1;
    } else {
      Value32 = FUSE_ACCESS_LOCATION + STRAP_BIF_PCIE_LANE_EQUALIZATION_CNTL_UPSTREAM_PORT_TX_PRESET_OFFSET_RV2;
    }
    //<Embedded_Override_End>
    NbioRegisterWrite (GnbHandle,
                      TYPE_SMN,
                      PCIE0_STRAP_INDEX_ADDRESS,
                      &Value32,
                      0
                      );
    Value32 = Engine->Type.Port.LaneEqualizationCntl.UsTxPreset;
    NbioRegisterWrite (GnbHandle,
                      TYPE_SMN,
                      PCIE0_STRAP_DATA_ADDRESS,
                      &Value32,
                      0
                      );
    }
  return;
}

/**----------------------------------------------------------------------------------------*/
/**
 * Interface to configure DXIO/PCIe ports after reconfig and before training
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 * @retval    AGESA_STATUS
 */
 /*----------------------------------------------------------------------------------------*/

VOID
DxioCfgAfterReconfig (
  IN       GNB_HANDLE       *GnbHandle
  )
{
  PCIe_ENGINE_CONFIG        *PcieEngine;
  PCIe_WRAPPER_CONFIG       *PcieWrapper;
  IDS_HDT_CONSOLE (GNB_TRACE, "DxioCfgAfterReconfig Enter\n");
  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      DxioCfgAfterReconfigCallback (GnbHandle, PcieEngine, NULL );
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }
    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }
  return;
}


