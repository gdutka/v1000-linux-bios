/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric Resource Manager Services
 *
 * Contains code that provides fabric resource manager services
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
#include <PiDxe.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/FabricResourceManagerLib.h>
#include <Library/FabricResourceSizeForEachRbLib.h>
#include <Library/AmdHeapLib.h>
#include <Protocol/FabricResourceManagerServicesProtocol.h>
#include <Filecode.h>
#include <AGESA.h>

#define FILECODE LIBRARY_DXEFABRICRESOURCEMANAGERSERVICESLIB_FABRICRESOURCEMANAGERSERVICESDXE_FILECODE

EFI_STATUS
EFIAPI
DxeFabricAllocateMmio (
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL *This,
  IN OUT   UINT64                     *BaseAddress,
  IN OUT   UINT64                     *Length,
  IN       UINT64                      Alignment,
  IN       FABRIC_TARGET               Target,
  IN OUT   FABRIC_MMIO_ATTRIBUTE      *Attributes
  );

EFI_STATUS
EFIAPI
DxeFabricAllocateIo (
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL *This,
  IN OUT   UINT32                     *BaseAddress,
  IN OUT   UINT32                     *Length,
  IN       FABRIC_TARGET               Target
  );

EFI_STATUS
EFIAPI
DxeFabricGetAvailableResource (
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL   *This,
  IN       FABRIC_RESOURCE_FOR_EACH_RB        *ResourceForEachRb
  );

EFI_STATUS
EFIAPI
DxeFabricReallocateResourceForEachRb (
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL  *This,
  IN       FABRIC_RESOURCE_FOR_EACH_RB       *ResourceSizeForEachRb,
  IN       FABRIC_ADDR_SPACE_SIZE            *SpaceStatus
  );

EFI_STATUS
EFIAPI
DxeFabricResourceRestoreDefault (
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL     *This
  );

EFI_STATUS
EFIAPI
DxeFabricEnableVgaMmio (
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL  *This,
  IN       FABRIC_TARGET          Target
  );

STATIC FABRIC_RESOURCE_MANAGER_PROTOCOL  mAmdFabricResourceManager = {
  AMD_FABRIC_RESOURCE_PROTOCOL_REV,
  DxeFabricAllocateMmio,
  DxeFabricAllocateIo,
  DxeFabricGetAvailableResource,
  DxeFabricReallocateResourceForEachRb,
  DxeFabricResourceRestoreDefault,
  DxeFabricEnableVgaMmio
};


EFI_STATUS
EFIAPI
FabricResourceManagerServiceProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_HANDLE  Handle;
  EFI_STATUS  Status;
  LOCATE_HEAP_PTR           LocateHeapParams;
  FABRIC_MMIO_MANAGER      *FabricMmioManager;

  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gAmdFabricResourceManagerServicesProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mAmdFabricResourceManager
                  );

  FabricMmioManager = NULL;
  // Find MMIO manager in heap
  LocateHeapParams.BufferHandle = AMD_MMIO_MANAGER;

  if (HeapLocateBuffer (&LocateHeapParams, NULL) == AGESA_SUCCESS) {
    FabricMmioManager = (FABRIC_MMIO_MANAGER *) LocateHeapParams.BufferPtr;
  }

  if (FabricMmioManager != NULL) {
    FabricMmioManager->ImageHandle = (UINT64) ImageHandle;
  }
  return Status;
}

/*---------------------------------------------------------------------------------------*/
/**
 * BSC entry point for allocate MMIO
 *
 * program MMIO base/limit/control registers
 *
 * @param[in]         This               Function pointer to FABRIC_RESOURCE_MANAGER_PROTOCOL.
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
DxeFabricAllocateMmio (
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL *This,
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
 * @param[in]         This               Function pointer to FABRIC_RESOURCE_MANAGER_PROTOCOL.
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
DxeFabricAllocateIo (
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL *This,
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
 * DxeFabricGetAvailableResource
 *
 * Get available DF resource (MMIO/IO) for each RootBridge
 *
 * @param[in]         This                  Function pointer to FABRIC_RESOURCE_MANAGER_PROTOCOL.
 * @param[in]         ResourceForEachRb     Avaiable DF resource (MMIO/IO) for each RootBridge
 *
 * @retval            EFI_SUCCESS           Available resources successfully obtained
 *                    EFI_ABORTED           Unable to obtain resource information
 */
EFI_STATUS
EFIAPI
DxeFabricGetAvailableResource (
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL  *This,
  IN       FABRIC_RESOURCE_FOR_EACH_RB      *ResourceForEachRb
  )
{
  EFI_STATUS         Status;

  Status = FabricGetAvailableResource (ResourceForEachRb);

  return Status;
}

/*---------------------------------------------------------------------------------------*/
/**
 * DxeFabricReallocateResourceForEachRb
 *
 * ResourceSize = UsedSize + UserRequestSize (input)
 * Save ResourceSize to NV variable
 *
 * @param[in]         This                      Function pointer to FABRIC_RESOURCE_MANAGER_PROTOCOL.
 * @param[in]         ResourceSizeForEachRb     Avaiable DF resource (MMIO/IO) size for each RootBridge
 * @param[in]         SpaceStatus               Current status
 *
 * @retval            EFI_SUCCESS               Save user's request to NV variable successfully
 *                    EFI_OUT_OF_RESOURCES      Not enough resource
 */
EFI_STATUS
EFIAPI
DxeFabricReallocateResourceForEachRb (
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL  *This,
  IN       FABRIC_RESOURCE_FOR_EACH_RB      *ResourceSizeForEachRb,
  IN       FABRIC_ADDR_SPACE_SIZE            *SpaceStatus
  )
{
  EFI_STATUS         Status;

  Status = FabricReallocateResourceForEachRb (ResourceSizeForEachRb, SpaceStatus);

  return Status;
}


/*---------------------------------------------------------------------------------------*/
/**
 * DxeFabricResourceRestoreDefault
 *
 * Restore default MMIO/IO distribution strategy by clearing NV variable
 *
 * @param[in]         This                      Function pointer to FABRIC_RESOURCE_MANAGER_PROTOCOL.
 *
 * @retval            EFI_SUCCESS               NV variables successfully restored to default settings
 */
EFI_STATUS
EFIAPI
DxeFabricResourceRestoreDefault (
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL     *This
  )
{
  FabricResourceRestoreDefault ();

  return EFI_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 * DxeFabricEnableVgaMmio
 *
 * Set VGA Enable register
 *
 * @param[in]         This                      Function pointer to FABRIC_RESOURCE_MANAGER_PROTOCOL.
 * @param[in]         Target                    PCI bus number/RootBridge number of the requestor.
 *
 * @retval            EFI_SUCCESS               VGA registers successfully enabled
 *                    EFI_ABORTED               Can't find destination
 */
EFI_STATUS
EFIAPI
DxeFabricEnableVgaMmio (
  IN       FABRIC_RESOURCE_MANAGER_PROTOCOL  *This,
  IN       FABRIC_TARGET          Target
  )
{
  EFI_STATUS         Status;

  Status = FabricEnableVgaMmio (Target);

  return Status;
}


