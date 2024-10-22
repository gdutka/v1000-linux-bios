/* $NoKeywords:$ */
/**
 * @file
 *
 * NB IOAPIC Initialization
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
#include  "GnbCommonLib.h"
#include  "PcieConfigLib.h"
#include  "Filecode.h"
#include  "GnbRegistersCommonV2.h"
#define FILECODE LIBRARY_GNBCZLIB_MODULES_GNBIOAPIC_GNBIOAPIC_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

AGESA_STATUS
GnbNbIoapicInterface (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Callback to init IOAPIC on GNB port
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
IoapicInitCallbackV5 (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PCI_ADDR            GnbPciAddress;
  D0F0xFC_x10_STRUCT  D0F0xFC_x10;

  GnbPciAddress = GnbGetHostPciAddress ((GNB_HANDLE *) PcieConfigGetParent (DESCRIPTOR_SILICON, &Engine->Header));
  D0F0xFC_x10.Value = 0x0;

  // Bounds check values - make sure the value is small enough to fit the field size
  ASSERT (Engine->Type.Port.PortData.ApicDeviceInfo.GroupMap < (1 << D0F0xFC_x10_BrExtIntrGrp_WIDTH));
  ASSERT (Engine->Type.Port.PortData.ApicDeviceInfo.Swizzle < (1 << D0F0xFC_x10_BrExtIntrSwz_WIDTH));
  ASSERT (Engine->Type.Port.PortData.ApicDeviceInfo.BridgeInt < (1 << D0F0xFC_x10_BrIntIntrMap_WIDTH));

  // Get the configuration from the PCIe_PORT_DATA APIC_DEVICE_INFO struct
  D0F0xFC_x10.Field.BrExtIntrGrp = Engine->Type.Port.PortData.ApicDeviceInfo.GroupMap;
  D0F0xFC_x10.Field.BrExtIntrSwz = Engine->Type.Port.PortData.ApicDeviceInfo.Swizzle;
  D0F0xFC_x10.Field.BrIntIntrMap = Engine->Type.Port.PortData.ApicDeviceInfo.BridgeInt;

  // Write the register
  GnbLibPciIndirectWrite (
    GnbPciAddress.AddressValue | D0F0xF8_ADDRESS,
    D0F0xFC_x10_ADDRESS + Engine->Type.Port.LogicalBridgeId,
    AccessS3SaveWidth32,
    &D0F0xFC_x10.Value,
    GnbLibGetHeader (Pcie)
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback to Enable IOAPIC on GNB
 *
 *
 *
 * @param[in]  Descriptor          Silicon descriptor
 * @param[in]  Buffer              Pointer to buffer
 * @param[in]  Pcie                Pointer to global PCIe configuration
 * @retval     AGESA_STATUS
 */
AGESA_STATUS
STATIC
IoapicEnableCallbackV5 (
  IN       PCIe_DESCRIPTOR_HEADER            *Descriptor,
  IN OUT   VOID                              *Buffer,
  IN       PCIe_PLATFORM_CONFIG              *Pcie
  )
{
  PCI_ADDR            GnbPciAddress;
  D0F0xFC_x00_STRUCT  D0F0xFC_x00;
  UINT32              *AddressPtr;
  UINT32              AddressLow;
  UINT32              AddressHigh;

  D0F0xFC_x00.Value = 0x0;
  D0F0xFC_x00.Field.IoapicEnable = 1;
  // Set the extended ID enable (default)
  D0F0xFC_x00.Field.IoapicIdExtEn = 1;
  // Enable SB feature for every APIC.  ACPI OS may disable this once the OS boots
  D0F0xFC_x00.Field.IoapicSbFeatureEn = 1;
  AddressPtr = (UINT32*) Buffer;
  AddressLow = AddressPtr[0] & 0xFFFFFF00;
  AddressHigh = AddressPtr[1];
  // Get the PCI address of the GNB
  GnbPciAddress = GnbGetHostPciAddress (GnbGetHandle (GnbLibGetHeader (Pcie)));
  // If the BLDCFG base address is null, assume that the base address of the APIC has already been programmed
  // If base address is defined in BLDCFG, program it here
  if ((AddressLow != NULL) || (AddressHigh != NULL)) {
    GnbLibPciIndirectWrite (
      GnbPciAddress.AddressValue | D0F0xF8_ADDRESS,
      D0F0xFC_x01_ADDRESS,
      AccessS3SaveWidth32,
      &AddressLow,
      GnbLibGetHeader (Pcie)
      );
    GnbLibPciIndirectWrite (
      GnbPciAddress.AddressValue | D0F0xF8_ADDRESS,
      D0F0xFC_x02_ADDRESS,
      AccessS3SaveWidth32,
      &AddressHigh,
      GnbLibGetHeader (Pcie)
      );
    IDS_HDT_CONSOLE (GNB_TRACE, "GNB IOAPIC base address is at high %x, low %x\n", AddressHigh, AddressLow);
    // Enable the IOAPIC.
    GnbLibPciIndirectWrite (
      GnbPciAddress.AddressValue | D0F0xF8_ADDRESS,
      D0F0xFC_x00_ADDRESS,
      AccessS3SaveWidth32,
      &D0F0xFC_x00.Value,
      GnbLibGetHeader (Pcie)
      );
  }
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Init NB IOAPIC
 *
 *
 *
 * @param[in] StdHeader             Standard Configuration Header
 */

AGESA_STATUS
GnbNbIoapicInterface (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  AMD_MID_PARAMS        *MidParamsPtr;
  AGESA_STATUS          Status;
  PCIe_PLATFORM_CONFIG  *Pcie;
  UINT64                GnbNbIoapicAddress;
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbNbIoapicInterface Enter\n");
  Status = PcieLocateConfigurationData (StdHeader, &Pcie);
  if (Status == AGESA_SUCCESS) {
    MidParamsPtr = (AMD_MID_PARAMS *) StdHeader;
    GnbNbIoapicAddress = MidParamsPtr->GnbMidConfiguration.GnbIoapicAddress;
    // For each ENGINE, configure the group, swizzle, and pin per APIC_DEVICE_INFO
    PcieConfigRunProcForAllEngines (DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE, IoapicInitCallbackV5, NULL, Pcie);

    // For each GNB, configure the IOAPIC Enable, ID Size, and SB Feature Enable
    Status = PcieConfigRunProcForAllDescriptors (DESCRIPTOR_SILICON, 0, DESCRIPTOR_TERMINATE_TOPOLOGY, IoapicEnableCallbackV5, &GnbNbIoapicAddress, Pcie);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbNbIoapicInterface Exit\n");
  return Status;
}



