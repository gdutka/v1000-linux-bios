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
#include <AmdPcieComplex.h>
#include <Ppi/NbioPcieComplexPpi.h>

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

  DXIO_PORT_DESCRIPTOR  PortList[] = {
    // GFX - x8 slot
    {
      0,
      DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 8, 15, 0, 0),
      DXIO_PORT_DATA_INITIALIZER_PCIE (
        DxioPortEnabled,                      // Port Present
        3,                                    // Requested Device
        1,                                    // Requested Function
        DxioHotplugDisabled,                  // Hotplug
        DxioGenMaxSupported,                  // Max Link Speed
        DxioGenMaxSupported,                  // Max Link Capability
        DxioAspmL0sL1,                        // ASPM
        0,                                    // ASPM L1.1 disabled
        0,                                    // ASPM L1.2 disabled
        0                                     // Clock PM
      )
    },
    // GPP[1:0] - M2 x2
    {
      DESCRIPTOR_TERMINATE_LIST,
      DXIO_ENGINE_DATA_INITIALIZER (DxioPcieEngine, 4, 5, 0, 0),
      DXIO_PORT_DATA_INITIALIZER_PCIE (
        DxioPortEnabled,                      // Port Present
        2,                                    // Requested Device
        5,                                    // Requested Function
        DxioHotplugDisabled,                  // Hotplug
        DxioGenMaxSupported,                  // Max Link Speed
        DxioGenMaxSupported,                  // Max Link Capability
        DxioAspmL0sL1,                        // ASPM
        0,                                    // ASPM L1.1 disabled
        0,                                    // ASPM L1.2 disabled
        0                                     // Clock PM
      )
    }
  };

  DXIO_COMPLEX_DESCRIPTOR PcieComplex = {
    DESCRIPTOR_TERMINATE_LIST,
    0,
    &PortList[0],
    NULL,
    NULL
  };

/**
* This service will return a pointer to the DXIO_COMPLEX_DESCRIPTOR.
*
*  @param[in]  This        A pointer to the PEI_AMD_NBIO_PCIE_COMPLEX_PPI instance
*  @param[in]  UserConfig  Pointer DXIO_COMPLEX_DESCRIPTOR pointer
*
*  @retval EFI_SUCCESS     The thread was successfully launched.
*
**/
EFI_STATUS
PcieGetComplex (
  IN        PEI_AMD_NBIO_PCIE_COMPLEX_PPI  *This,
     OUT    DXIO_COMPLEX_DESCRIPTOR        **UserConfig
  )
{
  *UserConfig = &PcieComplex;
  return EFI_SUCCESS;
}

STATIC PEI_AMD_NBIO_PCIE_COMPLEX_PPI mPcieComplexPpi = {
  AMD_NBIO_PCIE_COMPLEX_REVISION,  ///< revision
  PcieGetComplex
};

STATIC EFI_PEI_PPI_DESCRIPTOR mPcieComplexDescriptorPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdNbioPcieComplexPpiGuid,
  &mPcieComplexPpi
};

EFI_STATUS
EFIAPI
PcieComplexPeiEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                       Status;
  // Fixup DXIO_COMPLEX_DESCRIPTOR here
  Status = (**PeiServices).InstallPpi (PeiServices, &mPcieComplexDescriptorPpiList);
  return Status;
}

