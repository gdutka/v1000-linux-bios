/* $NoKeywords:$ */
/**
 * @file
 *
 * Platform PCIe Complex Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdNbioSmuV9Pei
 * @e \$Revision: 312065 $   @e \$Date: 2015-01-30 04:23:05 -0600 (Fri, 30 Jan 2015) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include <PiPei.h>
#include <AmdPcieComplexFm15.h>
#include <Ppi/NbioPcieComplexFm15Ppi.h>

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */


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

  PCIe_PORT_DESCRIPTOR  PortList[] = {
    // GFX - x8 slot
    {
      0,
      PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 8, 15),
      PCIE_PORT_DATA_INITIALIZER_V2 (
        PortEnabled,
        ChannelTypeExt6db,
        3,
        1,
        HotplugDisabled,
        PcieGenMaxSupported,
        PcieGenMaxSupported,
        AspmL0sL1,
        1,  // Bristol only uses x8
        0
      )
    },
    // GPP[1:0] - M2 x2
    {
      0,
      PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 4, 5),
      PCIE_PORT_DATA_INITIALIZER_V2 (
        PortEnabled,
        ChannelTypeExt6db,
        2,
        5,
        HotplugDisabled,
        PcieGenMaxSupported,
        PcieGenMaxSupported,
        AspmL0sL1,
        2,
        0
      )
    },

    // UMI - Promontroy x4
    {
      0,
      PCIE_ENGINE_DATA_INITIALIZER (PcieUnusedEngine, 0, 3),
      PCIE_PORT_DATA_INITIALIZER_V2 (
        PortDisabled,
        ChannelTypeExt6db,
        2,
        4,
        HotplugDisabled,
        PcieGenMaxSupported,
        PcieGenMaxSupported,
        AspmL0sL1,
        3,
        0
      )
    },
    // UMI - PCIe x4 slot
    {
      DESCRIPTOR_TERMINATE_LIST,
      PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 0, 3),
      PCIE_PORT_DATA_INITIALIZER_V2 (
        PortEnabled,
        ChannelTypeExt6db,
        2,
        3,
        HotplugDisabled,
        PcieGenMaxSupported,
        PcieGenMaxSupported,
        AspmL0sL1,
        4,
        0
      )
    },
  };

  PCIe_DDI_DESCRIPTOR DdiList[] = {
    // DDI0 - DP
    {
      0,
      PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 16, 19),
      PCIE_DDI_DATA_INITIALIZER (ConnectorTypeDP, Aux1, Hdp1)
    },
    // DDI1 - HDMI
    {
      0,
      PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 20, 23),
      PCIE_DDI_DATA_INITIALIZER (ConnectorTypeHDMI, Aux2, Hdp2)
    },
    // DDI2 - HDMI
    {
      DESCRIPTOR_TERMINATE_LIST,
      PCIE_ENGINE_DATA_INITIALIZER (PcieDdiEngine, 24, 27),
      PCIE_DDI_DATA_INITIALIZER (ConnectorTypeHDMI, Aux3, Hdp3)
    }
  };

  PCIe_COMPLEX_DESCRIPTOR PcieComplex = {
    DESCRIPTOR_TERMINATE_LIST,
    0,
    &PortList[0],
    &DdiList[0],
    NULL
  };

/**
  This service will start a core to fetch its first instructions from the reset
  vector.  This service may only be called from the BSP.

  @param[in]  This        A pointer to the PEI_AMD_NBIO_PCIE_COMPLEX_PPI instance
  @param[in]  UserConfig  Pointer PCIe_COMPLEX_DESCRIPTOR pointer

  @retval EFI_SUCCESS                        The thread was successfully launched.

**/
EFI_STATUS
PcieGetComplex (
  IN       PEI_AMD_NBIO_PCIE_COMPLEX_FM15_PPI *This,
     OUT   PCIe_COMPLEX_DESCRIPTOR            **UserConfig
  )
{
  *UserConfig = &PcieComplex;
  return EFI_SUCCESS;
}

STATIC PEI_AMD_NBIO_PCIE_COMPLEX_FM15_PPI mPcieComplexPpi = {
  AMD_NBIO_PCIE_COMPLEX_FM15_REVISION,  ///< revision
  PcieGetComplex
};

STATIC EFI_PEI_PPI_DESCRIPTOR mPcieComplexDescriptorPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdNbioPcieComplexFm15PpiGuid,
  &mPcieComplexPpi
};

EFI_STATUS
EFIAPI
PcieComplexFm15PeiEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                       Status;
  // Fixup PCIe_COMPLEX_DESCRIPTOR here
  Status = (**PeiServices).InstallPpi (PeiServices, &mPcieComplexDescriptorPpiList);
  return Status;
}

