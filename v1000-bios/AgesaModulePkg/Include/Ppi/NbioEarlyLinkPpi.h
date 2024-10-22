/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _AMD_NBIO_EARLY_LINK_PPI_H_
#define _AMD_NBIO_EARLY_LINK_PPI_H_

/// EARLY LINK STATUS Structure
typedef struct {
  IN         BOOLEAN      EarlyLinkStatus;      ///< Status of early link training (TRUE == Training Success)
  IN         UINT8        PhysicalRootBridge;   ///< Physical root bridge (NBIO) that the early train link is connected to
  IN         UINT8        LogicalRootBridge;    ///< Logical root bridge (NBIO) that the early train link is connected to
  IN         UINT8        RootPortBus;          ///< Bus number of the root port of the early train link
  IN         UINT8        RootPortDevice;       ///< Device number of the root port of the early train link
  IN         UINT8        RootPortFunction;     ///< Function number of the root port of the early train link
} EARLY_LINK_STATUS;

typedef struct _PEI_AMD_NBIO_EARLY_LINK_PPI PEI_AMD_NBIO_EARLY_LINK_PPI;

//
// PPI prototype
//
/**
  Returns location information about Early Train Link

  EarlyLinkStatus
    A pointer to structure to store the Early Train Link status
**/
typedef
EFI_STATUS
(EFIAPI * AMD_NBIO_EARLY_LINK_CONFIG) (
  OUT EARLY_LINK_STATUS  *EarlyLinkStatus
);


///
/// The Ppi of Early Link services
///
struct _PEI_AMD_NBIO_EARLY_LINK_PPI {
  AMD_NBIO_EARLY_LINK_CONFIG       GetEarlyLinkConfig; ///<
};

extern EFI_GUID gAmdNbioEarlyLinkPpiGuid;
#endif //

