/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe port initialization service procedure
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 322247 $   @e \$Date: 2015-07-09 15:18:54 +0800 (Thu, 09 Jul 2015) $
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
#include  "GnbPcieFamServices.h"
#include  "GnbSbLib.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbPcieInitLib.h"
#include  "GnbRegistersCommon.h"

#define FILECODE LIBRARY_GNBCZLIB_MODULES_GNBPCIEINITLIB_PCIEPORTSERVICES_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
UINT8 L1State = 0x1b;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Set completion timeout
 *
 *
 *
 * @param[in]  Engine          Pointer to engine config descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 *
 */

VOID
PcieCompletionTimeout (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  GnbLibPciRMW (
    Engine->Type.Port.Address.AddressValue | DxF0x80_ADDRESS,
    AccessWidth32,
    0xffffffff,
    0x6 << DxF0x80_CplTimeoutValue_OFFSET,
    GnbLibGetHeader (Pcie)
    );
  if (Engine->Type.Port.PortData.LinkHotplug != HotplugDisabled) {
    PciePortRegisterWriteField (
      Engine,
      DxF0xE4_x20_ADDRESS,
      DxF0xE4_x20_TxFlushTlpDis_OFFSET,
      DxF0xE4_x20_TxFlushTlpDis_WIDTH,
      0x0,
      TRUE,
      Pcie
      );
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set misc slot capability
 *
 *
 *
 * @param[in]  Engine          Pointer to engine config descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 *
 */

VOID
PcieLinkSetSlotCap (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  GnbLibPciRMW (
    Engine->Type.Port.Address.AddressValue | DxF0x58_ADDRESS,
    AccessWidth32,
    0xffffffff,
    1 << DxF0x58_SlotImplemented_OFFSET,
    GnbLibGetHeader (Pcie)
    );
  GnbLibPciRMW (
    Engine->Type.Port.Address.AddressValue | DxF0x3C_ADDRESS,
    AccessWidth32,
    0xffffffff,
    1 << DxF0x3C_IntPin_OFFSET,
    GnbLibGetHeader (Pcie)
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Safe mode to force link advertize Gen1 only capability in TS
 *
 *
 *
 * @param[in]  Engine          Pointer to engine config descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 *
 */

VOID
PcieLinkSafeMode (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PcieFmSetLinkSpeedCap (PcieGen1, Engine, Pcie);
  PciePortRegisterRMW (
    Engine,
    DxF0xE4_xA2_ADDRESS,
    DxF0xE4_xA2_LcUpconfigureDis_MASK,
    (1 << DxF0xE4_xA2_LcUpconfigureDis_OFFSET),
    FALSE,
    Pcie
    );
}


/*----------------------------------------------------------------------------------------*/
/**
 * Set current link speed
 *
 *
 * @param[in]  Engine               Pointer to engine configuration descriptor
 * @param[in]  Pcie                 Pointer to global PCIe configuration
 *
 */
VOID
PcieSetLinkWidthCap (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PciePortRegisterRMW (
    Engine,
    DxF0xE4_xA2_ADDRESS,
    DxF0xE4_xA2_LcUpconfigureDis_MASK,
    0,
    FALSE,
    Pcie
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Force compliance
 *
 *
 *
 * @param[in]  Engine          Pointer to engine config descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 *
 */

VOID
PcieForceCompliance (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  if (Engine->Type.Port.PortData.LinkSpeedCapability >= PcieGen2) {
    GnbLibPciRMW (
      Engine->Type.Port.Address.AddressValue | DxF0x88_ADDRESS,
      AccessWidth32,
      0xffffffff,
      0x1 << DxF0x88_EnterCompliance_OFFSET,
      GnbLibGetHeader (Pcie)
      );
  } else if (Engine->Type.Port.PortData.LinkSpeedCapability == PcieGen1) {
    PciePortRegisterWriteField (
      Engine,
      DxF0xE4_xC0_ADDRESS,
      DxF0xE4_xC0_StrapForceCompliance_OFFSET,
      DxF0xE4_xC0_StrapForceCompliance_WIDTH,
      0x1,
      FALSE,
      Pcie
      );
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Enable ASPM on SB link
 *
 *
 *
 * @param[in]  Engine          Pointer to engine config descriptor
 * @param[in]  Pcie            Pointer to global PCIe configuration
 *
 */

VOID
PcieEnableAspm (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  if (Engine->Type.Port.PortData.LinkAspm != AspmDisabled) {
    if (PcieConfigIsSbPcieEngine (Engine)) {
      SbPcieLinkAspmControl (Engine, Pcie);
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set current link speed
 *
 *
 * @param[in]  LinkSpeedCapability  Link Speed Capability
 * @param[in]  Engine               Pointer to engine configuration descriptor
 * @param[in]  Pcie                 Pointer to global PCIe configuration
 *
 */
VOID
PcieSetLinkSpeedCapV4 (
  IN      PCIE_LINK_SPEED_CAP   LinkSpeedCapability,
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  DxF0xE4_xA4_STRUCT  DxF0xE4_xA4;
  DxF0xE4_xC0_STRUCT  DxF0xE4_xC0;
  DxF0x88_STRUCT      DxF0x88;
  GnbLibPciRead (
    Engine->Type.Port.Address.AddressValue | DxF0x88_ADDRESS,
    AccessWidth32,
    &DxF0x88.Value,
    GnbLibGetHeader (Pcie)
   );
  DxF0xE4_xA4.Value = PciePortRegisterRead (
                        Engine,
                        DxF0xE4_xA4_ADDRESS,
                        Pcie
                        );
  DxF0xE4_xC0.Value = PciePortRegisterRead (
                        Engine,
                        DxF0xE4_xC0_ADDRESS,
                        Pcie
                        );

  switch (LinkSpeedCapability) {
  case PcieGen3:
    DxF0xE4_xA4.Field.LcGen2EnStrap = 0x1;
    DxF0xE4_xA4.Field.LcGen3EnStrap = 0x1;
    DxF0xE4_xA4.Field.LcMultUpstreamAutoSpdChngEn = 0x1;
    DxF0x88.Field.TargetLinkSpeed = 0x3;
    DxF0x88.Field.HwAutonomousSpeedDisable = 0x0;
    break;
  case PcieGen2:
    DxF0xE4_xA4.Field.LcGen2EnStrap = 0x1;
    DxF0xE4_xA4.Field.LcGen3EnStrap = 0x0;
    DxF0xE4_xA4.Field.LcMultUpstreamAutoSpdChngEn = 0x1;
    DxF0x88.Field.TargetLinkSpeed = 0x2;
    DxF0x88.Field.HwAutonomousSpeedDisable = 0x0;
    break;
  case PcieGen1:
    DxF0xE4_xA4.Field.LcGen2EnStrap = 0x0;
    DxF0xE4_xA4.Field.LcGen3EnStrap = 0x0;
    DxF0xE4_xA4.Field.LcMultUpstreamAutoSpdChngEn = 0x0;
    DxF0x88.Field.TargetLinkSpeed = 0x1;
    DxF0x88.Field.HwAutonomousSpeedDisable = 0x1;
    PcieRegisterWriteField (
      PcieConfigGetParentWrapper (Engine),
      WRAP_SPACE (PcieConfigGetParentWrapper (Engine)->WrapId, D0F0xE4_WRAP_0803_ADDRESS + 0x100 * Engine->Type.Port.PortId),
      D0F0xE4_WRAP_0803_StrapBifDeemphasisSel_OFFSET,
      D0F0xE4_WRAP_0803_StrapBifDeemphasisSel_WIDTH,
      0,
      FALSE,
      Pcie
      );
    break;
  default:
    ASSERT (FALSE);
    break;
  }

  if ((Pcie->PsppPolicy == PsppDisabled) || (PcieConfigIsSbPcieEngine (Engine))) {
    DxF0xE4_xC0.Field.StrapAutoRcSpeedNegotiationDis = 0x0;
  } else {
    DxF0xE4_xC0.Field.StrapAutoRcSpeedNegotiationDis = 0x1;
  }

  PciePortRegisterWrite (
    Engine,
    DxF0xE4_xA4_ADDRESS,
    DxF0xE4_xA4.Value,
    FALSE,
    Pcie
    );
  PciePortRegisterWrite (
    Engine,
    DxF0xE4_xC0_ADDRESS,
    DxF0xE4_xC0.Value,
    FALSE,
    Pcie
    );
  GnbLibPciWrite (
    Engine->Type.Port.Address.AddressValue | DxF0x88_ADDRESS,
    AccessWidth32,
    &DxF0x88.Value,
    GnbLibGetHeader (Pcie)
   );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set slot power limit
 *
 *
 *
 * @param[in]  Engine              Pointer to engine configuration
 * @param[in]  Pcie                Pointer to PCIe configuration
 */


VOID
PcieEnableSlotPowerLimitV5 (
  IN      PCIe_ENGINE_CONFIG     *Engine,
  IN      PCIe_PLATFORM_CONFIG   *Pcie
  )
{
  PCIe_SILICON_CONFIG   *Silicon;
  if (PcieLibIsEngineAllocated (Engine) && Engine->Type.Port.PortData.PortPresent != PortDisabled && !PcieConfigIsSbPcieEngine (Engine)) {
    IDS_HDT_CONSOLE (PCIE_MISC, "   Enable Slot Power Limit for Port % d\n", Engine->Type.Port.Address.Address.Device);
    Silicon = PcieConfigGetParentSilicon (Engine);
    GnbLibPciIndirectRMW (
      Silicon->Address.AddressValue | D0F0xC8_ADDRESS,
      D0F0xCC_x01_ADDRESS | ((Engine->Type.Port.PortData.DeviceNumber << 3 | Engine->Type.Port.PortData.FunctionNumber)  << D0F0xC8_NB_DEV_IND_SEL_OFFSET),
      AccessS3SaveWidth32,
      0xffffffff,
      1 << D0F0xCC_x01_SetPowEn_OFFSET,
      GnbLibGetHeader (Pcie)
    );
  }
}

