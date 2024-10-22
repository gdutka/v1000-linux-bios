/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _AMD_NBIO_PCIE_COMPLEX_PPI_H_
#define _AMD_NBIO_PCIE_COMPLEX_PPI_H_

// Current PPI revision
#define AMD_NBIO_PCIE_COMPLEX_REVISION   0x01

typedef struct _PEI_AMD_NBIO_PCIE_COMPLEX_PPI PEI_AMD_NBIO_PCIE_COMPLEX_PPI;

//
// PPI prototype
//
/**
  Returns the PCIe complex structure
  This
    A pointer to the _PEI_AMD_NBIO_PCIE_COMPLEX_PPI instance.
  UserConfig
    A pointer to a pointer to store the address of the Platform PCIe Complex Structure

**/
typedef
EFI_STATUS
(EFIAPI * AMD_NBIO_PCIE_GET_COMPLEX_STRUCT) (
  IN     PEI_AMD_NBIO_PCIE_COMPLEX_PPI   *This,
     OUT DXIO_COMPLEX_DESCRIPTOR         **UserConfig
);

///
///< The Ppi to return PCIe Complex Descriptor
///
typedef struct _PEI_AMD_NBIO_PCIE_COMPLEX_PPI {
  UINT32 Revision;         ///< PPI revision
  AMD_NBIO_PCIE_GET_COMPLEX_STRUCT     PcieGetComplex;
} PEI_AMD_NBIO_PCIE_COMPLEX_PPI;

extern EFI_GUID gAmdNbioPcieComplexPpiGuid;

#endif //

