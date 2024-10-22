/**
 InstallPeiMemoryInternal.h

 This file provides the prototype of all PEI InstallPeiMemory help functions,
 definition, include files and data structure.

***************************************************************************
* Copyright (c) 2016, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#ifndef _INSTALL_PEI_MEMORY_INTERNAL_H_
#define _INSTALL_PEI_MEMORY_INTERNAL_H_

#include <PiPei.h>
//
// Libraries
//
#include <Library/ReportStatusCodeLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>

#include <Guid/UmaExistHob.h>
#include <Guid/AcpiS3Context.h>
#include <Guid/AcpiVariableSet.h>
#include <Guid/SmramMemoryReserve.h>
#include <Guid/MemoryTypeInformation.h>

//
// Consume Ppi
//
#include <Ppi/Speaker.h>
#include <Ppi/Capsule.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/PlatformMemorySize.h>
#include <Ppi/PlatformMemoryRange.h>

#include <CommonSmbiosMemory.h>
#include <PostCode.h>
#include <AGESA.h>
//#include <heapManager.h>
#include <Fch.h>


typedef enum {
  SmramSize0,
  SmramSize128K,
  SmramSize256K,
  SmramSize512K,
  SmramSize1024K,
  SmramSize2048K,
  SmramSize4096K,
  SmramSize8192K
} PEI_SMRAM_SIZE_TYPE;

//
// SMRAM range definitions
//
#define MC_ABSEG_HSEG_PHYSICAL_START        0x000A0000
#define MC_ABSEG_HSEG_LENGTH                0x00020000
#define MC_ABSEG_CPU_START                  0x000A0000
#define MC_HSEG_CPU_START                   0xFEDA0000

//
// Maximum number of memory ranges supported by the memory controller
//
#define MAX_RANGES  (MAX_ROWS + 10)

//
// Memory range types
//
typedef enum {
  DualChannelDdrMainMemory,
  DualChannelDdrSmramCacheable,
  DualChannelDdrSmramNonCacheable,
  DualChannelDdrGraphicsMemoryCacheable,
  DualChannelDdrGraphicsMemoryNonCacheable,
  DualChannelDdrReservedMemory,
  DualChannelDdrMaxMemoryRangeType
} PEI_DUAL_CHANNEL_DDR_MEMORY_RANGE_TYPE;

//
// Memory map range information
//
typedef struct {
  UINT8                                         RowNumber;
  EFI_PHYSICAL_ADDRESS                          PhysicalAddress;
  EFI_PHYSICAL_ADDRESS                          CpuAddress;
  EFI_PHYSICAL_ADDRESS                          RangeLength;
  PEI_DUAL_CHANNEL_DDR_MEMORY_RANGE_TYPE        Type;
} PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE;

//
// Help functions prototype
//
/**
 This function installs memory for all paths except S3 resume. 
 
 @param[in]         PeiServices         Describes the list of possible PEI Services.
 @param[in]         BootMode            The specific boot path that is being followed.

 @retval            EFI_SUCCESS         The operation completed successfully.
 @retval            Others              An unexpected error occurred.
*/
EFI_STATUS
InstallEfiMemory (
  IN CONST EFI_PEI_SERVICES                   **PeiServices,
  IN       EFI_BOOT_MODE                      BootMode
);

/**
 This function installs memory for the S3 resume path.
 
 @param[in]         PeiServices         Describes the list of possible PEI Services.
 @param[in]         VariableServices    Read only variable PPI.

 @retval            EFI_SUCCESS         The operation completed successfully.
 @retval            Others              An unexpected error occurred.
*/
EFI_STATUS
InstallS3Memory (
  IN CONST EFI_PEI_SERVICES                   **PeiServices,
  IN       EFI_PEI_READ_ONLY_VARIABLE2_PPI    *VariableServices
);

/**
 Config T-seg address and size.

 @param[in]         PeiServices         Describes the list of possible PEI Services.
 @param[in]         SmramSize           Handle of the file being invoked.

 @retval            None
*/
VOID
TurnOnSmram  (
  IN CONST EFI_PEI_SERVICES                   **PeiServices,
  IN       PEI_SMRAM_SIZE_TYPE                SmramSize
);

/**
 Report no memory. This is platform-specific but to conserve space, it is being built in line.

 @param[in]         PeiServices         Describes the list of possible PEI Services.
 @param[in]         ErrorCode           Handle of the file being invoked.

 @retval            None
*/
VOID
ReportMemoryError (
  IN CONST EFI_PEI_SERVICES                   **PeiServices,
  IN       UINTN                              ErrorCode
);

#endif
