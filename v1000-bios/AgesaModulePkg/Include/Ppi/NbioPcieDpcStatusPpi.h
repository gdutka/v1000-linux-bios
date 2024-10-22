/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _AMD_NBIO_PCIE_DPCSTATUS_PPI_H_
#define _AMD_NBIO_PCIE_DPCSTATUS_PPI_H_

#define MAX_NUMBER_DPCSTATUS      128
/// PCIE_DPC_STATUS
typedef struct {
  UINT8                           SocketId;                       ///< Socket ID
  UINT8                           DieID;                          ///< Die ID
  UINT8                           RBIndex;                        ///< Node to which GNB connected
  UINT8                           BusNumber;                      ///< PCI Bus Number
  UINT8                           PCIeCoreID;                     ///< PCIe core ID
  UINT8                           PCIePortID;                     ///< PCIe port ID
  UINT16                          DpcStatus;                      ///< PCIe DPC status
} PCIe_DPC_STATUS_RECORD;

/// PCIE_DPC_STATUS
typedef struct {
  UINT8                           size;                                 ///< number of PCIe DPC status
  PCIe_DPC_STATUS_RECORD          DpcStatusArray[MAX_NUMBER_DPCSTATUS]; ///< PCIe DPC status Array
} PCIe_DPC_STATUS_DATA;

typedef struct _PEI_AMD_NBIO_PCIE_DPCSTATUS_PPI PEI_AMD_NBIO_PCIE_DPCSTATUS_PPI;

//
// PPI prototype
//
/**
  Returns the DPC Status data structure

  This
    A pointer to the PEI_AMD_NBIO_PCIE_DPCSTATUS_PPI instance.
  DpcStatus
    A pointer to a pointer to store the address of DPC Status data array

**/
typedef
EFI_STATUS
(EFIAPI * AMD_NBIO_PCIE_GET_DPCSTATUS) (
  IN  PEI_AMD_NBIO_PCIE_DPCSTATUS_PPI  *This,
  OUT PCIe_DPC_STATUS_DATA             **DpcStatus
);

///
/// The Ppi of DPC Status sevices
///
typedef struct _PEI_AMD_NBIO_PCIE_DPCSTATUS_PPI {
  AMD_NBIO_PCIE_GET_DPCSTATUS     GetDpcStatus; ///<
} PEI_AMD_NBIO_PCIE_DPCSTATUS_PPI;

///{7E70BBC8-9E8D-4BCB-8A3F-AF0CDE679D92}
extern EFI_GUID gAmdNbioPcieDpcStatusPpiGuid ;

#endif //

