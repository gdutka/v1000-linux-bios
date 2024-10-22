/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric resource manager PPI prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _FABRIC_RESOURCE_MANAGER_SERVICES_PPI_H_
#define _FABRIC_RESOURCE_MANAGER_SERVICES_PPI_H_
#include <FabricResourceManagerCmn.h>

#pragma pack (push, 1)

/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */
#define AMD_FABRIC_RESOURCE_PPI_REV 2

/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */
typedef struct _FABRIC_RESOURCE_MANAGER_PPI FABRIC_RESOURCE_MANAGER_PPI;

/// Function prototype
typedef EFI_STATUS (EFIAPI *FABRIC_PEI_ALLOCATE_MMIO) (
  IN OUT   UINT64                           *BaseAddress,  ///< Starting address of the requested MMIO range.
  IN OUT   UINT64                           *Length,       ///< Length of the requested MMIO range.
  IN       UINT64                            Alignment,    ///< Alignment bit map. 0xFFFFF means 1MB align
  IN       FABRIC_TARGET                     Target,       ///< Indicator target type
                                                           ///< 0 - TARGET_PCI_BUS - allocate an MMIO region for the device on a certain PCI bus
                                                           ///< 1 - TARGET_RB      - allocate an MMIO region for the device on a certain Socket, RootBridge
  IN OUT   FABRIC_MMIO_ATTRIBUTE            *Attributes    ///< We have 6 pools per RootBridge. 0) MMIO_BELOW_4G 1) MMIO_ABOVE_4G 2) P_MMIO_BELOW_4G 3) P_MMIO_ABOVE_4G 4) NON_PCI_DEVICE_BELOW_4G 5) NON_PCI_DEVICE_ABOVE_4G
                                                           ///< This function is restricted to support NON_PCI_DEVICE_BELOW_4G & NON_PCI_DEVICE_ABOVE_4G only
                                                           ///< Other 4 pools are supported by FabricGetAvailableResource
);

/// Function prototype
typedef EFI_STATUS (EFIAPI *FABRIC_PEI_ALLOCATE_IO) (
  IN OUT   UINT32                           *BaseAddress,  ///< Starting address of the requested IO range.
  IN OUT   UINT32                           *Length,       ///< Length of the requested IO range.
  IN       FABRIC_TARGET                     Target        ///< Indicator target type
                                                           ///< 0 - TARGET_PCI_BUS - allocate an IO region for the device on a certain PCI bus
                                                           ///< 1 - TARGET_RB     - allocate an IO region for the device on a certain Socket, RootBridge
);

/// Function prototype
typedef EFI_STATUS (EFIAPI *FABRIC_PEI_GET_AVAILABLE_RESOURCE) (
  IN       FABRIC_RESOURCE_FOR_EACH_RB       *ResourceForEachRb                 ///< Get available DF resource size for each RootBridge
);

/// Fabric resource manager Protocol Structure
struct _FABRIC_RESOURCE_MANAGER_PPI {                                           //See the Forward Declaration above
  UINTN                                      Revision;                          ///< Revision
  FABRIC_PEI_ALLOCATE_MMIO                   FabricAllocateMmio;                ///< Pointer to function
  FABRIC_PEI_ALLOCATE_IO                     FabricAllocateIo;                  ///< Pointer to function
  FABRIC_PEI_GET_AVAILABLE_RESOURCE          FabricGetAvailableResource;        ///< Pointer to function
};

extern EFI_GUID gAmdFabricResourceManagerServicesPpiGuid;
extern EFI_GUID gAmdResourceSizeForEachRbGuid;
extern EFI_GUID gAmdResourceDistributionGuid;

#pragma pack (pop)
#endif // _FABRIC_RESOURCE_MANAGER_SERVICES_PPI_H_
