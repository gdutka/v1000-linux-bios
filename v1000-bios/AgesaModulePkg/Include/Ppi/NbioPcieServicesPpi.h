/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _AMD_NBIO_PCIE_SERVICES_PPI_H_
#define _AMD_NBIO_PCIE_SERVICES_PPI_H_

#include <GnbDxio.h>
#include <Guid/GnbPcieInfoHob.h>

// Current PPI revision
#define AMD_NBIO_PCIE_SERVICES_REVISION   0x00

typedef struct _PEI_AMD_NBIO_PCIE_SERVICES_PPI PEI_AMD_NBIO_PCIE_SERVICES_PPI;

//
// PPI prototype
//
/**
  Returns the NBIO debug options configuration structure

  This
    A pointer to the PEI_AMD_NBIO_SMU_SERVICES_PPI instance.
  DebugOptions
    A pointer to a pointer to store the address of the PCIe topology structure

**/
typedef
EFI_STATUS
(EFIAPI * AMD_NBIO_PCIE_GET_TOPOLOGY_STRUCT) (
  IN  PEI_AMD_NBIO_PCIE_SERVICES_PPI  *This,
  OUT GNB_PCIE_INFORMATION_DATA_HOB   **DebugOptions
);

//
// PPI prototype
//
/**
  Sets the operating speed for a single PCIe port

  This
    A pointer to the PEI_AMD_NBIO_SMU_SERVICES_PPI instance.
  PciAddress
    PCI Address of the target port
  TargetSpeed
    Desired speed of the target port (Gen1, Gen2, Gen3)

**/
typedef
EFI_STATUS
(EFIAPI * AMD_NBIO_PCIE_SET_TARGET_SPEED_STRUCT) (
  IN  PEI_AMD_NBIO_PCIE_SERVICES_PPI  *This,
  IN  CONST EFI_PEI_SERVICES          **PeiServices,
  IN  UINT8                           PciDevice,
  IN  UINT8                           PciFunction,
  IN  UINT8                           TargetSpeed
);

///
/// The Ppi of Pcie sevices
///
typedef struct _PEI_AMD_NBIO_PCIE_SERVICES_PPI {
  UINT32 Revision;                                       ///< revision
  AMD_NBIO_PCIE_GET_TOPOLOGY_STRUCT     PcieGetTopology; ///<
  AMD_NBIO_PCIE_SET_TARGET_SPEED_STRUCT PcieSetSpeed;    ///<
} PEI_AMD_NBIO_PCIE_SERVICES_PPI;

extern EFI_GUID gAmdNbioPcieServicesPpiGuid ;

#endif //

