/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric Resource Manager Service functions
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include <PiPei.h>
#include <Library/FabricResourceManagerLib.h>
#include <Library/FabricResourceSizeForEachRbLib.h>
#include <Ppi/FabricResourceManagerServicesPpi.h>
#include <Filecode.h>
#include "AGESA.h"


#define FILECODE LIBRARY_PEIFABRICRESOURCEMANAGERSERVICESLIB_FABRICRESOURCEMANAGERSERVICESPEI_FILECODE

EFI_STATUS
EFIAPI
PeiFabricAllocateMmio (
  IN OUT   UINT64                     *BaseAddress,
  IN OUT   UINT64                     *Length,
  IN       UINT64                      Alignment,
  IN       FABRIC_TARGET               Target,
  IN OUT   FABRIC_MMIO_ATTRIBUTE      *Attributes
  );

EFI_STATUS
EFIAPI
PeiFabricAllocateIo (
  IN OUT   UINT32                     *BaseAddress,
  IN OUT   UINT32                     *Length,
  IN       FABRIC_TARGET               Target
  );

EFI_STATUS
EFIAPI
PeiFabricGetAvailableResource (
  IN       FABRIC_RESOURCE_FOR_EACH_RB        *ResourceForEachRb
  );


STATIC FABRIC_RESOURCE_MANAGER_PPI  mFabricResourceManagerServicesPpi = {
  AMD_FABRIC_RESOURCE_PPI_REV,
  PeiFabricAllocateMmio,
  PeiFabricAllocateIo,
  PeiFabricGetAvailableResource
};

STATIC EFI_PEI_PPI_DESCRIPTOR mFabricResourceManagerPpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdFabricResourceManagerServicesPpiGuid,
  &mFabricResourceManagerServicesPpi
};


EFI_STATUS
EFIAPI
FabricResourceManagerServicePpiInstall (
  IN       CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS  Status;

  // Install Fabric Resource Manager Services
  Status = (**PeiServices).InstallPpi (PeiServices, &mFabricResourceManagerPpiList);

  return Status;

}

/*---------------------------------------------------------------------------------------*/
/**
 * BSC entry point for allocate MMIO
 *
 * program MMIO base/limit/control registers
 *
 * @param[in, out]    BaseAddress        Starting address of the requested MMIO range.
 * @param[in, out]    Length             Length of the requested MMIO range.
 * @param[in]         Alignment          Alignment bit map.
 * @param[in]         Target             PCI bus number/RootBridge number of the requestor.
 * @param[in, out]    Attributes         Attributes of the requested MMIO range indicating whether
 *                                       it is readable/writable/non-posted
 *
 * @retval            EFI_STATUS         EFI_OUT_OF_RESOURCES - The requested range could not be added because there are not
 *                                                              enough mapping resources.
 *                                       EFI_ABORTED          - One or more input parameters are invalid. For example, the
 *                                                              PciBusNumber does not correspond to any device in the system.
 *                                       EFI_SUCCESS          - MMIO region allocated successfully
 */
EFI_STATUS
EFIAPI
PeiFabricAllocateMmio (
  IN OUT   UINT64                     *BaseAddress,
  IN OUT   UINT64                     *Length,
  IN       UINT64                      Alignment,
  IN       FABRIC_TARGET               Target,
  IN OUT   FABRIC_MMIO_ATTRIBUTE      *Attributes
  )
{
  EFI_STATUS         Status;

  Status = FabricAllocateMmio (BaseAddress, Length, Alignment, Target, Attributes);

  return Status;
}

/*---------------------------------------------------------------------------------------*/
/**
 * BSC entry point for for allocate IO
 *
 * program IO base/limit registers
 *
 * @param[in, out]    BaseAddress        Starting address of the requested MMIO range.
 * @param[in, out]    Length             Length of the requested MMIO range.
 * @param[in]         Target             PCI bus number/RootBridge number of the requestor.
 *
 * @retval            EFI_STATUS         EFI_OUT_OF_RESOURCES - The requested range could not be added because there are not
 *                                                              enough mapping resources.
 *                                       EFI_ABORTED          - One or more input parameters are invalid. For example, the
 *                                                              PciBusNumber does not correspond to any device in the system.
 *                                       EFI_SUCCESS          - IO region allocated successfully
 */
EFI_STATUS
EFIAPI
PeiFabricAllocateIo (
  IN OUT   UINT32                     *BaseAddress,
  IN OUT   UINT32                     *Length,
  IN       FABRIC_TARGET               Target
  )
{
  EFI_STATUS         Status;

  Status = FabricAllocateIo (BaseAddress, Length, Target);

  return Status;
}

/*---------------------------------------------------------------------------------------*/
/**
 * PeiFabricGetAvailableResource
 *
 * Get available DF resource (MMIO/IO) for each RootBridge
 *
 * @param[in, out]    ResourceForEachRb     Avaiable DF resource (MMIO/IO) for each RootBridge
 *
 * @retval            EFI_SUCCESS           Available resources successfully obtained
 *                    EFI_ABORTED           Unable to obtain resource information
 */
EFI_STATUS
EFIAPI
PeiFabricGetAvailableResource (
  IN       FABRIC_RESOURCE_FOR_EACH_RB      *ResourceForEachRb
  )
{
  EFI_STATUS         Status;

  Status = FabricGetAvailableResource (ResourceForEachRb);

  return Status;
}


