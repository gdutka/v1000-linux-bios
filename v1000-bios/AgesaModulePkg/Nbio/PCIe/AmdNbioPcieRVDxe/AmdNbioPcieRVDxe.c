/* $NoKeywords:$ */
/**
 * @file
 *
 * AmdNbioPcieRVDxe Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdNbioPcieZPDxe
 * @e \$Revision: 312065 $   @e \$Date: 2015-01-30 04:23:05 -0600 (Fri, 30 Jan 2015) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include <PiDxe.h>
#include <Filecode.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/AmdNbioPcieServicesProtocol.h>
#include <Protocol/AmdNbioPcieAerProtocol.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Guid/HobList.h>
#include <Protocol/PciIo.h>
#include <Library/PcieConfigLib.h>
#include <Library/PcieMiscCommLib.h>
#include <Library/NbioHandleLib.h>
#include <GnbRegistersRV.h>
#include <Library/NbioRegisterAccLib.h>
#include "AmdNbioPcieRVDxe.h"
#include "AmdNbioPcieAerRV.h"

#define FILECODE        NBIO_PCIE_AMDNBIOPCIERVDXE_AMDNBIOPCIERVDXE_FILECODE

extern AMD_NBIO_PCIE_AER_PROTOCOL mAmdNbioPcieAerProtocol;
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
VOID
PcieAspmBlackListInterface (
  IN       GNB_HANDLE  *GnbHandle
  );

VOID
PcieL1ExitLatencyInterface (
  IN       GNB_HANDLE  *GnbHandle
  );

EFI_STATUS
AmdPcieGetTopology (
  IN       DXE_AMD_NBIO_PCIE_SERVICES_PROTOCOL  *This,
     OUT   UINT32                               **DebugOptions
  )
{
  EFI_STATUS                           Status;
  GNB_PCIE_INFORMATION_DATA_HOB        *GnbPcieInfoDataHob;
  //
  // Get information data from HOB.
  //
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID**)&GnbPcieInfoDataHob);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  GnbPcieInfoDataHob = GetNextGuidHob (&gGnbPcieHobInfoGuid, GnbPcieInfoDataHob);
  if (GnbPcieInfoDataHob == NULL) {
    return EFI_NOT_FOUND;
  }

  *DebugOptions = (UINT32*) GnbPcieInfoDataHob;
  return Status;
}

DXE_AMD_NBIO_PCIE_SERVICES_PROTOCOL mDxeAmdNbioPcieServicesProtocol = {
  AmdPcieGetTopology
};

/*----------------------------------------------------------------------------------------*/
/**
 * Clock gating
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 */

VOID
PciePwrClockGating (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  PCIe_WRAPPER_CONFIG            *Wrapper;
  PCIEDIR_460_STRUCT             PcieDir460;
  PCIEDIR_044_STRUCT             PcieDir044;
  PCIECORE_18C_STRUCT            PcieCore18C;
  UINT32                         Address0;
  UINT32                         Address1;
  UINT32                         Address2;

  IDS_HDT_CONSOLE (GNB_TRACE, "PciePwrClockGating Enter\n");

  Wrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (Wrapper != NULL) {
    // LCLK Gating
    Address0 = CPM_CONTROL_PCIECORE0_ADDRESS;
    Address1 = PCIE_CONFIG_CNTL_PCIECORE0_ADDRESS;
    Address2 = PCIE_SDP_CTRL_PCIECORE0_ADDRESS;

    NbioRegisterRead (GnbHandle, CPM_CONTROL_TYPE, Address0, &PcieDir460.Value, 0);
    NbioRegisterRead (GnbHandle, PCIE_CONFIG_CNTL_TYPE, Address1, &PcieDir044.Value, 0);
    NbioRegisterRead (GnbHandle, PCIE_SDP_CTRL_TYPE, Address2, &PcieCore18C.Value, 0);

    if (Wrapper->Features.LclkGating == 1) {
      PcieDir460.Field.LCLK_DYN_GATE_ENABLE = 0x1;
      PcieDir460.Field.LCLK_GATE_TXCLK_FREE = 0x0;
      PcieDir460.Field.LCLK_DYN_GATE_LATENCY = 0x1;
    }
    // TXCLK+REFCLK
    if (Wrapper->Features.ClkGating == 1) {
      // TXCLK_PERM gating enable
      PcieDir460.Field.TXCLK_PERM_GATE_ENABLE = 0x1;
      PcieDir460.Field.TXCLK_PERM_GATE_LATENCY = 0x1;
      // TXCLK DYN gating enable
      PcieDir460.Field.TXCLK_DYN_GATE_ENABLE = 0x1;
      // TXCLK REGS gating enable
      PcieDir460.Field.TXCLK_REGS_GATE_ENABLE = 0x1;
      PcieDir460.Field.TXCLK_REGS_GATE_LATENCY = 0x1;
      PcieDir460.Field.LCLK_GATE_ALLOW_IN_L1 = 1;

      PcieCore18C.Field.CI_SLV_REQR_PART_DISCONNECT_EN = 0x1;
      PcieCore18C.Field.CI_MSTSDP_CLKGATE_ONESIDED_ENABLE = 0x1;
      PcieCore18C.Field.CI_MSTSDP_DISCONNECT_RSP_ON_PARTIAL = 0x1;
    }

    PcieDir460.Field.CLKREQb_UNGATE_TXCLK_ENABLE = 0x1;
    PcieDir044.Field.CI_MAX_READ_REQUEST_SIZE_MODE = 1;
    PcieDir044.Field.CI_PRIV_MAX_READ_REQUEST_SIZE = 1;

    NbioRegisterWrite (GnbHandle, CPM_CONTROL_TYPE, Address0, &PcieDir460.Value, GNB_REG_ACC_FLAG_S3SAVE);
    NbioRegisterWrite (GnbHandle, PCIE_CONFIG_CNTL_TYPE, Address1, &PcieDir044.Value, GNB_REG_ACC_FLAG_S3SAVE);
    NbioRegisterWrite (GnbHandle, PCIE_SDP_CTRL_TYPE, Address2, &PcieCore18C.Value, GNB_REG_ACC_FLAG_S3SAVE);

    Wrapper = PcieLibGetNextDescriptor (Wrapper);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePwrClockGating Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * PCIe PHY Power gating
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 */

VOID
PciePhyPowerGating (
  IN       GNB_HANDLE  *GnbHandle
  )
{

  if (PcdGetBool (PcdCfgPCIePHYPowerGating)) {
    IDS_HDT_CONSOLE (GNB_TRACE, "PciePhyPowerGating Enable\n");
    NbioRegisterRMW (GnbHandle, TYPE_SMN, PCS_C10_POWER_GATING_REGISTER_Address, (UINT32) ~(BIT1 | BIT2), (BIT1 | BIT2), GNB_REG_ACC_FLAG_S3SAVE);
    NbioRegisterRMW (GnbHandle, TYPE_SMN, RSMU_POWER_GATING_CONTROL_Address, (UINT32) ~(BIT0 | BIT1), BIT1, GNB_REG_ACC_FLAG_S3SAVE);
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     PCIE0_CPM_CONTROL_ADDRESS,
                     (UINT32) ~(CPM_CONTROL_TXCLK_LCNT_GATE_ENABLE_MASK | CPM_CONTROL_TXCLK_PRBS_GATE_ENABLE_MASK),
                     (1 << CPM_CONTROL_TXCLK_LCNT_GATE_ENABLE_OFFSET) | (1 << CPM_CONTROL_TXCLK_PRBS_GATE_ENABLE_OFFSET),
                     GNB_REG_ACC_FLAG_S3SAVE);
    NbioRegisterRMW (GnbHandle, TYPE_SMN, PCIE_PGMST_CNTL_Address, (UINT32) ~(BIT8), BIT8, GNB_REG_ACC_FLAG_S3SAVE);
  } else {
    NbioRegisterRMW (GnbHandle, TYPE_SMN, PCS_C10_POWER_GATING_REGISTER_Address, (UINT32) ~(BIT1 | BIT0), BIT0, GNB_REG_ACC_FLAG_S3SAVE);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Lock registers
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 */

VOID
PcieLockRegisters (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  NbioRegisterRMW (GnbHandle, TYPE_SMN, PCIE0_CNTL_ADDRESS, (UINT32) ~(BIT0), BIT0, GNB_REG_ACC_FLAG_S3SAVE);
}

/**----------------------------------------------------------------------------------------*/
/**
 * Interface to enable PM L1
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 */
 /*----------------------------------------------------------------------------------------*/
VOID
PCieAspmL1Interfcae (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  PCIe_ENGINE_CONFIG             *PcieEngine;
  PCIe_WRAPPER_CONFIG            *PcieWrapper;
  UINT32                         SmnAddress;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAspmL1Interface Enter\n");
  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      if (PcieLibIsEngineAllocated (PcieEngine)) {

        if (PcieEngine->Type.Port.PortData.LinkAspm == AspmL1) {
          SmnAddress = (PcieWrapper->WrapId == 0) ? 0x111402ec : 0x112402ec;
          SmnAddress |= (PcieEngine->Type.Port.PortId % 8) << 12;
          NbioRegisterRMW (GnbHandle, TYPE_SMN, SmnAddress, (UINT32) ~(BIT21), BIT21, GNB_REG_ACC_FLAG_S3SAVE);
        }

      }
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }
    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAspmL1Interface Exit\n");
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  AmdPcieMiscInit
 *
 *  Description:
 *     Calls AmdInitMid as part of Agesa DXE Driver Initialization.
 *
 *  Parameters:
 *    @param[in]     Event
 *    @param[in]     *Context
 *
 *    @retval         VOID
 *
 *---------------------------------------------------------------------------------------
 **/

VOID
EFIAPI
AmdPcieMiscInit (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                          Status;
  PCIe_PLATFORM_CONFIG                *Pcie;
  DXE_AMD_NBIO_PCIE_SERVICES_PROTOCOL *PcieServicesProtocol;
  GNB_PCIE_INFORMATION_DATA_HOB       *PciePlatformConfigHobData;
  GNB_HANDLE                          *GnbHandle;

  AGESA_TESTPOINT (TpAmdPcieMiscInitEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdPcieMiscInit Entry\n");
  Status = gBS->LocateProtocol (
                  &gAmdNbioPcieServicesProtocolGuid,
                  NULL,
                  (VOID**)&PcieServicesProtocol
                  );

  if (Status == EFI_SUCCESS) {
    PcieServicesProtocol->PcieGetTopology (PcieServicesProtocol, (UINT32 **) &PciePlatformConfigHobData);
    Pcie = &(PciePlatformConfigHobData->PciePlatformConfigHob);
    GnbHandle = NbioGetHandle (Pcie);
    while (GnbHandle != NULL) {
      AmdPsppConfig (Pcie);
      // Slot power limit
      PcieSlotPowerLimitInterface (GnbHandle);
      // Enabling PCIe Core Power Saving Features
      PciePwrClockGating (GnbHandle);
      // Initialize AER
      NbioPcieAerInitRV (GnbHandle);
      // configure MaxReadRequestSize on PCIE interface
      PcieMaxReadRequestInterface (GnbHandle);
      // configure MaxPayloadSize on PCIE interface
      PcieMaxPayloadInterface (GnbHandle);
      // enable Clock Power Managment
      PcieClkPmInterface (GnbHandle);

      PcieSrisInit (GnbHandle);
      PcieAspmBlackListInterface (GnbHandle);
      PCieAspmL1Interfcae (GnbHandle);  //RV only
      PcieAspmL1SSInterface (GnbHandle);
      PcieClkReqInterface (GnbHandle);
      if (PcdGetBool (PcdPciePhyClkCntrlEnable)) {
        AmdPciePhyClkCntrl (Pcie);    // RV2 workaround
      }
      PcieAspmInterface (GnbHandle);
      if (PcdGetBool (PcdCfgPCIeLTREnable)) {
        PcieLTRInterface (GnbHandle);
      }
      // program common clock configuration
      PcieCommClkCfgInterface (GnbHandle);
      // Set L1 Exit Latency for PT
      PcieL1ExitLatencyInterface (GnbHandle);
      AmdAPlusAConfig (Pcie);

      PcieLockRegisters (GnbHandle);
      PciePhyPowerGating (GnbHandle);
      GnbHandle = GnbGetNextHandle (GnbHandle);
    }
  }

  gBS->CloseEvent (Event);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdPcieMiscInit Exit\n");
  AGESA_TESTPOINT (TpAmdPcieMiscInitExit, NULL);

  return;
}

EFI_STATUS
EFIAPI
AmdNbioPcieRVDxeEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS          Status;
  EFI_HANDLE          Handle;
  EFI_EVENT           PciIoEvent;
  VOID                *Registration;

  AGESA_TESTPOINT (TpNbioPCIeDxeEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioPcieRVDxeEntry\n");

  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gAmdNbioPcieServicesProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mDxeAmdNbioPcieServicesProtocol
                  );

  //
  // produce AMD_NBIO_PCIE_AER_PROTOCOL
  //
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gAmdNbioPcieAerProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mAmdNbioPcieAerProtocol
                  );

  //
  // Register the event handling function for AmdPcieMiscInit
  //
  Status = gBS->CreateEventEx (
             EVT_NOTIFY_SIGNAL,
             TPL_NOTIFY,
             AmdPcieMiscInit,
             NULL,
             NULL,
             &PciIoEvent
             );

  Status = gBS->RegisterProtocolNotify (
              &gEfiPciIoProtocolGuid,
              PciIoEvent,
              &Registration
              );

  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioPcieRVDxeExit\n");
  AGESA_TESTPOINT (TpNbioPCIeDxeExit, NULL);
  return EFI_SUCCESS;
}

