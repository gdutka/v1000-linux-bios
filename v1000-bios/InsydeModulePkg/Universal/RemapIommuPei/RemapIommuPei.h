/** @file
  Remap Iommu PEIM

;******************************************************************************
;* Copyright (c) 2017 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _REMAP_IOMMU_H_
#define _REMAP_IOMMU_H_

#include <Pi/PiPeiCis.h>
#include <Ppi/BlockIo.h>
#include <Ppi/BlockIo2.h>
#include <Ppi/Stall.h>
#include <Ppi/NvmeController.h>
#include <Ppi/IoMmu.h>
#include <Protocol/SmmCommunication.h>
#include <Ppi/SmmCommunication.h>
#include <Ppi/PostBootScriptTable.h>

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/VariableLib.h>
#include <Library/LockBoxLib.h>
#include <Library/PcdLib.h>

#include <Protocol/PciIo.h>

#include <SmiTable.h>
extern  EDKII_IOMMU_PPI  *mIoMmu;


#define DEVICE_ADDR_OF_END_OF_RESOURCE_NOTE  0x00
#define HOST_ADDR_OF_END_OF_RESOURCE_NOTE    0x00
#define BYTES_OF_END_OF_RESOURCE_NOTE        0x00

#pragma pack(1)

typedef struct {
  UINT32                                         Bytes;
  EFI_PHYSICAL_ADDRESS                           DeviceAddr;
  EFI_PHYSICAL_ADDRESS                           HostAddr;
} IOMMU_RESOURCE_REMAP;

typedef struct {
  UINT32                                         Bytes;
  EFI_PHYSICAL_ADDRESS                           DeviceAddr;
  EFI_PHYSICAL_ADDRESS                           HostAddr;
} IOMMU_RESOURCE_REMAP_NVME;

typedef struct {
  EFI_PHYSICAL_ADDRESS   Start;
  UINT32                 Size;
} NVME_SMM_PRE_MEM_INFO;

#pragma pack()


/**
  Initialize IOMMU.
**/
VOID
IoMmuInit (
  VOID
  );

/**
  Allocates pages that are suitable for an OperationBusMasterCommonBuffer or
  OperationBusMasterCommonBuffer64 mapping.

  @param  Pages                 The number of pages to allocate.
  @param  HostAddress           A pointer to store the base system memory address of the
                                allocated range.
  @param  DeviceAddress         The resulting map address for the bus master PCI controller to use to
                                access the hosts HostAddress.
  @param  Mapping               A resulting value to pass to Unmap().

  @retval EFI_SUCCESS           The requested memory pages were allocated.
  @retval EFI_UNSUPPORTED       Attributes is unsupported. The only legal attribute bits are
                                MEMORY_WRITE_COMBINE and MEMORY_CACHED.
  @retval EFI_INVALID_PARAMETER One or more parameters are invalid.
  @retval EFI_OUT_OF_RESOURCES  The memory pages could not be allocated.

**/
EFI_STATUS
IoMmuAllocateBuffer (
  IN UINTN                  Pages,
  OUT VOID                  **HostAddress,
  OUT EFI_PHYSICAL_ADDRESS  *DeviceAddress,
  OUT VOID                  **Mapping
  );

/**
  Frees memory that was allocated with AllocateBuffer().

  @param  Pages                 The number of pages to free.
  @param  HostAddress           The base system memory address of the allocated range.
  @param  Mapping               The mapping value returned from Map().

  @retval EFI_SUCCESS           The requested memory pages were freed.
  @retval EFI_INVALID_PARAMETER The memory range specified by HostAddress and Pages
                                was not allocated with AllocateBuffer().

**/
EFI_STATUS
IoMmuFreeBuffer (
  IN UINTN                  Pages,
  IN VOID                   *HostAddress,
  IN VOID                   *Mapping
  );

/**
  Provides the controller-specific addresses required to access system memory from a
  DMA bus master.

  @param  Operation             Indicates if the bus master is going to read or write to system memory.
  @param  HostAddress           The system memory address to map to the PCI controller.
  @param  NumberOfBytes         On input the number of bytes to map. On output the number of bytes
                                that were mapped.
  @param  DeviceAddress         The resulting map address for the bus master PCI controller to use to
                                access the hosts HostAddress.
  @param  Mapping               A resulting value to pass to Unmap().

  @retval EFI_SUCCESS           The range was mapped for the returned NumberOfBytes.
  @retval EFI_UNSUPPORTED       The HostAddress cannot be mapped as a common buffer.
  @retval EFI_INVALID_PARAMETER One or more parameters are invalid.
  @retval EFI_OUT_OF_RESOURCES  The request could not be completed due to a lack of resources.
  @retval EFI_DEVICE_ERROR      The system hardware could not map the requested address.

**/
EFI_STATUS
IoMmuMap (
  IN  EDKII_IOMMU_OPERATION Operation,
  IN VOID                   *HostAddress,
  IN  OUT UINTN             *NumberOfBytes,
  OUT EFI_PHYSICAL_ADDRESS  *DeviceAddress,
  OUT VOID                  **Mapping
  );

/**
  Completes the Map() operation and releases any corresponding resources.

  @param  Mapping               The mapping value returned from Map().

  @retval EFI_SUCCESS           The range was unmapped.
  @retval EFI_INVALID_PARAMETER Mapping is not a value that was returned by Map().
  @retval EFI_DEVICE_ERROR      The data was not committed to the target system memory.
**/
EFI_STATUS
IoMmuUnmap (
  IN VOID                  *Mapping
  );

#endif
