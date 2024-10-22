/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe Slot Power Limit
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
#include  <OptionGnb.h>
#include  <Filecode.h>
#include  <Library/IdsLib.h>
#include  <Library/GnbPcieConfigLib.h>
#include  <Library/GnbCommonLib.h>
#include  <Library/PcieMiscCommLib.h>
#include  <Library/PcdLib.h>

#define FILECODE LIBRARY_PCIEMISCCOMMLIB_PCIESLOTPWRLIMIT_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Callback to init various features on all active ports
 *
 *
 *
 *
 * @param[in]       Engine          Pointer to engine config descriptor
 * @param[in]       PowerLimit      Power limit value
 *
 */

VOID
PcieSlotPwrLimitCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN       UINT8                 PowerLimit,
  IN       UINT8                 PowerLimitScale
  )
{
  UINT8    PcieCapPtr;
  UINT32   Value;
  PCI_ADDR Device;

  Device = Engine->Type.Port.Address;
  PcieCapPtr = GnbLibFindPciCapability (Device.AddressValue, PCIE_CAP_ID, NULL);
  if (PcieCapPtr != 0) {
    GnbLibPciRead ( Device.AddressValue | PcieCapPtr,
                    AccessWidth32,
                    &Value,
                    NULL
                    );
    if (Value & BIT24) {
      GnbLibPciRead (
        Device.AddressValue | (PcieCapPtr + PCIE_SLOT_CAP_REGISTER),
        AccessWidth32,
        &Value,
        NULL
      );
      if (PowerLimit != 0) {
        Value = (Value & 0xFFFF807F) | (UINT32)(PowerLimit << 7);
      }
      if (PowerLimitScale != 0) {
        Value = (Value & 0xFFFE7FFF) | (UINT32)(PowerLimitScale << 15);
      }
      GnbLibPciWrite (
        Device.AddressValue | (PcieCapPtr + PCIE_SLOT_CAP_REGISTER),
        AccessS3SaveWidth32,
        &Value,
        NULL
        );
    }
  }
}

/**----------------------------------------------------------------------------------------*/
/**
 * Interface to set Slot power limit
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 */
 /*----------------------------------------------------------------------------------------*/
VOID
PcieSlotPowerLimitInterface (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  PCIe_ENGINE_CONFIG             *PcieEngine;
  PCIe_WRAPPER_CONFIG            *PcieWrapper;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieSlotPowerLimitInterface Enter\n");
  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      if ((PcieLibIsEngineAllocated (PcieEngine)) && ((PcieEngine->Type.Port.SlotPowerLimit != 0) || (PcieEngine->Type.Port.SlotPowerLimitScale != 0))) {
        PcieSlotPwrLimitCallback (PcieEngine, PcieEngine->Type.Port.SlotPowerLimit, PcieEngine->Type.Port.SlotPowerLimitScale);
      }
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }
    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieSlotPowerLimitInterface Exit\n");
}

