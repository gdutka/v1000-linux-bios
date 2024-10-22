/** @file
 Provide Install memory help functions for Pei phase.

***************************************************************************
* Copyright (c) 2016-2018, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Base.h>
#include <Library/HobLib.h>
#include <Guid/AmdMemoryInfoHob.h>
#include "InstallPeiMemoryInternal.h"
//[-start-231030-IB18160164-add]//
#include <Library/AmdCbsVariable.h>
//[-end-231030-IB18160164-add]//

//
// This is the minimum memory required by DxeCore initialization. When LMFA feature enabled,
// This part of memory still need reserved on the very top of memory so that the DXE Core could
// use these memory for data initialization. This macro should be sync with the same marco
// defined in DXE Core.
//
#define MINIMUM_INITIAL_MEMORY_SIZE 0x10000

BOOLEAN  mInRecoveryPei = FALSE;

/*
 Gets the next highest address range of the AMD Memory HOB records
 @param[in]         AmdMemoryInfo       AMD Memory information structure
 @param[in, out]    NextRecord          Record with next highest address

 @retval            EFI_SUCCESS         NextRecord is valid
 @retval            EFI_NOT_FOUND       No additional records
 */
EFI_STATUS
EFIAPI
GetNextAmdMemInfoRecord (
    IN       AMD_MEMORY_INFO_HOB                   *AmdMemoryInfo,
    IN OUT   AMD_MEMORY_RANGE_DESCRIPTOR           **AmdMemoryInfoRange
    )
{
  UINTN                               Status;
  UINTN                               Index;
  AMD_MEMORY_RANGE_DESCRIPTOR         *CurrentAmdMemoryInfoRange;
  AMD_MEMORY_RANGE_DESCRIPTOR         *NextAmdMemoryInfoRange;

  CurrentAmdMemoryInfoRange = NULL;
  NextAmdMemoryInfoRange = NULL;

  for (Index = 0; Index < AmdMemoryInfo->NumberOfDescriptor; Index++) {
    CurrentAmdMemoryInfoRange = &(AmdMemoryInfo->Ranges[Index]);
    if (*AmdMemoryInfoRange == NULL) {
      // Find lowest base if *AmdMemoryInfoRange is NULL
      if (NextAmdMemoryInfoRange == NULL ||
          CurrentAmdMemoryInfoRange->Base < NextAmdMemoryInfoRange->Base) {
        NextAmdMemoryInfoRange = CurrentAmdMemoryInfoRange;
      }
    } else if (CurrentAmdMemoryInfoRange->Base > (*AmdMemoryInfoRange)->Base) {
      // Find next lowest from passed in *AmdMemoryInfoRange
      if (NextAmdMemoryInfoRange == NULL ||
          CurrentAmdMemoryInfoRange->Base < NextAmdMemoryInfoRange->Base) {
        NextAmdMemoryInfoRange = CurrentAmdMemoryInfoRange;
      }
    }
  }

  Status = NextAmdMemoryInfoRange == NULL ? EFI_NOT_FOUND: EFI_SUCCESS;
  *AmdMemoryInfoRange = NextAmdMemoryInfoRange;
  return (Status);
}


/**
 This function returns the memory ranges to be enabled, along with information
 describing how the range should be used. The MemoryMap buffer will be filled in and
 NumRanges will contain the actual number of memory ranges that are to be enabled.

 @param[in]         PeiServices         Describes the list of possible PEI Services.
 @param[in, out]    MemoryMap           Buffer to record details of the memory ranges to be enabled.
 @param[in, out]    NumRanges           On input, this contains the maximum number of memory ranges that
                                        can be described in the MemoryMap buffer.

 @retval            EFI_SUCCESS         The operation completed successfully.
 @retval            Others              An unexpected error occurred.
*/
EFI_STATUS
McGetMemoryMap (
  IN CONST EFI_PEI_SERVICES                       **PeiServices,
  IN OUT   PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE  *MemoryMap,
  IN OUT   UINT8                                  *NumRanges
  )
{
  EFI_STATUS                        Status;
  PEI_PLATFORM_MEMORY_RANGE_PPI     *MemoryRangePpi;
  PEI_MEMORY_RANGE_GRAPHICS_MEMORY  GraphicsMemoryMask;
  PEI_MEMORY_RANGE_PCI_MEMORY       PciMemoryMask;
  PEI_MEMORY_RANGE_OPTION_ROM       OptionRomMask;
  PEI_MEMORY_RANGE_SMRAM            SmramMask;
  PEI_SMRAM_SIZE_TYPE               SmramSize;
  BOOLEAN                           EnableSmram;
  UINT8                             GraphicMemoryIndex;
  UINT64                            GraphicMemoryBase;
  UINT64                            GraphicMemoryLength;
  UINT64                            SmRamTsegBase;
  UINT64                            SmRamTsegLength;
  DDR_ROW_CONFIG                    RowConfArray[MAX_ROWS];
  AMD_MEMORY_INFO_HOB               *AmdMemoryInfo;
  UINTN                             AmdMemoryInfoSize;
  AMD_MEMORY_RANGE_DESCRIPTOR       *AmdMemoryInfoRange;

  DEBUG ((EFI_D_INFO, "McGetMemoryMap-Start\n"));
  // Get Pointer to HOB
  AmdMemoryInfo = GetFirstGuidHob (&gAmdMemoryInfoHobGuid);
  ASSERT (AmdMemoryInfo != NULL);
  // Get HOB Data
  AmdMemoryInfoSize = GET_GUID_HOB_DATA_SIZE (AmdMemoryInfo);
  AmdMemoryInfo = GET_GUID_HOB_DATA (AmdMemoryInfo);

  //
  // Init and error check
  //
  ZeroMem (RowConfArray, sizeof (DDR_ROW_CONFIG) * MAX_ROWS);

  GraphicMemoryIndex  = 0;
  GraphicMemoryBase   = 0;
  GraphicMemoryLength = 0;
  AmdMemoryInfoRange = NULL;
  while (GetNextAmdMemInfoRecord (AmdMemoryInfo, &AmdMemoryInfoRange) == EFI_SUCCESS) {
    if (AmdMemoryInfoRange->Attribute == AMD_MEMORY_ATTRIBUTE_AVAILABLE) {
      if ((AmdMemoryInfoRange->Base < SIZE_4GB) && (AmdMemoryInfoRange->Size > RowConfArray[0].RowLength)) {
        RowConfArray[0].BaseAddress = AmdMemoryInfoRange->Base;
        RowConfArray[0].RowLength = AmdMemoryInfoRange->Size;
        DEBUG ((EFI_D_INFO, "  RowConfArray[0].BaseAddress = 0x%LX\n", RowConfArray[0].BaseAddress));
        DEBUG ((EFI_D_INFO, "  RowConfArray[0].RowLength   = 0x%LX\n", RowConfArray[0].RowLength));
      } else if ((AmdMemoryInfoRange->Base >= SIZE_4GB) && (AmdMemoryInfoRange->Size > RowConfArray[1].RowLength)) {
        RowConfArray[1].BaseAddress = AmdMemoryInfoRange->Base;
        RowConfArray[1].RowLength = AmdMemoryInfoRange->Size;
        DEBUG ((EFI_D_INFO, "  RowConfArray[1].BaseAddress = 0x%LX\n", RowConfArray[1].BaseAddress));
        DEBUG ((EFI_D_INFO, "  RowConfArray[1].RowLength   = 0x%LX\n", RowConfArray[1].RowLength));
      }
    } else if (AmdMemoryInfoRange->Attribute == AMD_MEMORY_ATTRIBUTE_UMA) {
      GraphicMemoryLength = AmdMemoryInfoRange->Size;
      GraphicMemoryBase = AmdMemoryInfoRange->Base;
      DEBUG ((EFI_D_INFO, "  GraphicMemoryBase           = 0x%LX\n", GraphicMemoryBase));
      DEBUG ((EFI_D_INFO, "  GraphicMemoryLength         = 0x%LX\n", GraphicMemoryLength));
      if (AmdMemoryInfoRange->Base < SIZE_4GB) {
        GraphicMemoryIndex = 0;
        DEBUG ((EFI_D_INFO, "  GraphicMemoryIndex          = 0x%X\n", GraphicMemoryIndex));
      } else {
        GraphicMemoryIndex = 1;
        DEBUG ((EFI_D_INFO, "  GraphicMemoryIndex          = 0x%X\n", GraphicMemoryIndex));
      }
    }
  }

  if (mInRecoveryPei) {
    RowConfArray[0].RowLength = DivU64x32 (RowConfArray[0].RowLength, 2);
    DEBUG ((EFI_D_INFO, "  RowConfArray[0].RowLength   = 0x%LX\n", RowConfArray[0].RowLength));
  }
  if ((*NumRanges) < 14) {
    DEBUG ((EFI_D_INFO, "McGetMemoryMap-End-1-EFI_BUFFER_TOO_SMALL\n"));
    return EFI_BUFFER_TOO_SMALL;
  }
  if (RowConfArray[0].RowLength == 0) {
    DEBUG ((EFI_D_INFO, "McGetMemoryMap-End-2-EFI_DEVICE_ERROR\n"));
    return EFI_DEVICE_ERROR;
  }

  SmramSize  = SmramSize0;
  *NumRanges = 0;

  //
  // Get platform memory range service
  //
  Status = PeiServicesLocatePpi (
                            &gPeiPlatformMemoryRangePpiGuid,
                            0,
                            NULL,
                            (VOID**)&MemoryRangePpi
                            );
  ASSERT_EFI_ERROR (Status);

  //
  // Find out which memory ranges to reserve on this platform
  //
  Status = MemoryRangePpi->ChooseRanges (
                             (EFI_PEI_SERVICES**) PeiServices,
                             MemoryRangePpi,
                             &OptionRomMask,
                             &SmramMask,
                             &GraphicsMemoryMask,
                             &PciMemoryMask
                             );
  ASSERT_EFI_ERROR (Status);

  //
  // Generate memory ranges for the memory map.
  //
  EnableSmram         = TRUE; //FALSE;
  SmRamTsegBase       = 0;
  SmRamTsegLength     = 0;

  //
  // Add memory below 640KB to the memory map. Make sure memory between
  // 640KB and 1MB are reserved, even if not used for SMRAM
  //
  MemoryMap[*NumRanges].RowNumber       = 0;
  MemoryMap[*NumRanges].PhysicalAddress = 0;
  MemoryMap[*NumRanges].CpuAddress      = 0;
  MemoryMap[*NumRanges].RangeLength     = 0xA0000;
  MemoryMap[*NumRanges].Type            = DualChannelDdrMainMemory;
  (*NumRanges)++;

  //
  // Reserve ABSEG or HSEG SMRAM if needed
  //
  if (SmramMask & (PEI_MR_SMRAM_ABSEG_MASK | PEI_MR_SMRAM_HSEG_MASK)) {
    EnableSmram                           = TRUE;
    MemoryMap[*NumRanges].PhysicalAddress = MC_ABSEG_HSEG_PHYSICAL_START;
    MemoryMap[*NumRanges].RangeLength     = MC_ABSEG_HSEG_LENGTH;
    MemoryMap[*NumRanges].CpuAddress      = (SmramMask & PEI_MR_SMRAM_ABSEG_MASK) ?
                                            MC_ABSEG_CPU_START : MC_HSEG_CPU_START;
    //
    // Chipset only supports cacheable SMRAM
    //
    MemoryMap[*NumRanges].Type = DualChannelDdrSmramCacheable;
  } else {
    //
    // Just mark this range reserved
    //
    MemoryMap[*NumRanges].PhysicalAddress = 0xA0000;
    MemoryMap[*NumRanges].CpuAddress      = 0xA0000;
    MemoryMap[*NumRanges].RangeLength     = 0x60000;
    MemoryMap[*NumRanges].Type            = DualChannelDdrReservedMemory;
  }
  MemoryMap[*NumRanges].RowNumber = 0;
  (*NumRanges)++;

  //
  // Get TSEG size
  //
  if (SmramMask & PEI_MR_SMRAM_TSEG_MASK) {
    SmRamTsegLength = ((SmramMask & PEI_MR_SMRAM_SIZE_MASK & (~(1 << 0))) * 128 * 1024);
    SmRamTsegBase = RowConfArray[0].RowLength - SmRamTsegLength;
  }

  //
  // Memory Map information below than 4G
  //
  MemoryMap[*NumRanges].RowNumber       = 0;
  MemoryMap[*NumRanges].PhysicalAddress = 0x100000; //Start from 1M
  MemoryMap[*NumRanges].CpuAddress      = 0x100000; //Start from 1M
  MemoryMap[*NumRanges].RangeLength     = (RowConfArray[0].RowLength - 0x100000 - SmRamTsegLength);
  MemoryMap[*NumRanges].Type            = DualChannelDdrMainMemory;
  (*NumRanges)++;

  //
  // Memory Map information Upper than 4G
  //
  if (RowConfArray[1].RowLength > 0) {
    MemoryMap[*NumRanges].RowNumber       = 1;
    MemoryMap[*NumRanges].PhysicalAddress = 0x100000000; //Start from 4G
    MemoryMap[*NumRanges].CpuAddress      = 0x100000000; //Start from 4G
    MemoryMap[*NumRanges].RangeLength     = (RowConfArray[1].RowLength);
    MemoryMap[*NumRanges].Type            = DualChannelDdrMainMemory;
    if (MemoryMap[*NumRanges].RangeLength > 0) {
      (*NumRanges)++;
    }
  }

  //
  // Memory Map Information for UMA memory
  //
  if (GraphicMemoryLength > 0) {
    MemoryMap[*NumRanges].RowNumber       = GraphicMemoryIndex;
    MemoryMap[*NumRanges].PhysicalAddress = GraphicMemoryBase;
    MemoryMap[*NumRanges].CpuAddress      = GraphicMemoryBase;
    MemoryMap[*NumRanges].RangeLength     = GraphicMemoryLength;
    MemoryMap[*NumRanges].Type            = (GraphicsMemoryMask & PEI_MR_GRAPHICS_MEMORY_CACHEABLE) ?
                                            DualChannelDdrGraphicsMemoryCacheable : DualChannelDdrGraphicsMemoryNonCacheable;
    (*NumRanges)++;
  }

  //
  // Memory Map Information for TSEG memory
  //
  if (SmRamTsegLength > 0) {
    MemoryMap[*NumRanges].RowNumber       = 0;
    MemoryMap[*NumRanges].PhysicalAddress = SmRamTsegBase;
    MemoryMap[*NumRanges].CpuAddress      = SmRamTsegBase;
    MemoryMap[*NumRanges].RangeLength     = SmRamTsegLength;
    //
    // Chipset only supports cacheable SMRAM
    //
    MemoryMap[*NumRanges].Type = DualChannelDdrSmramCacheable;
    (*NumRanges)++;
    //
    // Get TSEG size for later using.
    //
    switch ((SmramMask & PEI_MR_SMRAM_SIZE_MASK & (~(1 << 0)))) {

    case PEI_MR_SMRAM_SIZE_256K_MASK:
      SmramSize = SmramSize256K;
      break;

    case PEI_MR_SMRAM_SIZE_512K_MASK:
      SmramSize = SmramSize512K;
      break;

    case PEI_MR_SMRAM_SIZE_1024K_MASK:
      SmramSize = SmramSize1024K;
      break;

    case PEI_MR_SMRAM_SIZE_2048K_MASK:
      SmramSize = SmramSize2048K;
      break;

    case PEI_MR_SMRAM_SIZE_4096K_MASK:
      SmramSize = SmramSize4096K;
      break;

    case PEI_MR_SMRAM_SIZE_8192K_MASK:
      SmramSize = SmramSize8192K;
      break;

    default:
      // Non supported size. Set to 0.
      SmramSize = SmramSize0;
      break;
    }
  }

  //
  // Turn on SMRAM if required
  //
  if (EnableSmram) {
    TurnOnSmram (PeiServices, SmramSize);
  }

  DEBUG ((EFI_D_INFO, "McGetMemoryMap-End-3-EFI_SUCCESS\n"));
  return EFI_SUCCESS;
}

/**
 Determine the memory size desired based on HOB memory information.

 @param[in]         PeiServices         Describes the list of possible PEI Services.
 @param[in, out]    Size                The memory size to return.

 @retval            None
*/
VOID
RetrieveRequiredMemorySize (
  IN CONST EFI_PEI_SERVICES                   **PeiServices,
  IN OUT   UINTN                              *Size
  )
{
  EFI_STATUS                            Status;
  EFI_HOB_GUID_TYPE                     *GuidHob;
  EFI_MEMORY_TYPE_INFORMATION           *MemoryData;
  UINT8                                 Index;
  UINTN                                 TempPageNum;
  EFI_BOOT_MODE                         BootMode;

  GuidHob    = NULL;
  MemoryData = NULL;
  GuidHob = GetFirstGuidHob (&gEfiMemoryTypeInformationGuid);
  if (GuidHob != NULL) {
    MemoryData = GET_GUID_HOB_DATA (GuidHob);
  }

  //
  // Platform PEIM should supply such a information. Generic PEIM doesn't assume any default value
  //
  if (MemoryData == NULL) {
    return;
  }

  TempPageNum = 0;
  for (Index = 0; MemoryData[Index].Type != EfiMaxMemoryType; Index++) {
    //
    // Accumulate default memory size requirements
    //
    TempPageNum += MemoryData[Index].NumberOfPages;
  }

  if (TempPageNum == 0) {
    return;
  }

  //
  // This is the minimum memory required by DxeCore initialization.
  //
  (*Size) = (TempPageNum * EFI_PAGE_SIZE) + MINIMUM_INITIAL_MEMORY_SIZE;
  Status = PeiServicesGetBootMode (&BootMode);
  if (!EFI_ERROR (Status)) {
    if (BootMode == BOOT_IN_RECOVERY_MODE) {
      (*Size) += 0x2000 * EFI_PAGE_SIZE;
    }
  }
}

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
  )
{
  EFI_STATUS                                Status = EFI_SUCCESS;
  EFI_PHYSICAL_ADDRESS                      PeiMemoryBaseAddress;
  PEI_PLATFORM_MEMORY_SIZE_PPI              *MemSize;
  UINTN                                     PeiMemoryIndex;
  UINT64                                    PeiMemoryLength;
  UINTN                                     RequiredMemSize;
  PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE     MemoryMap[MAX_RANGES];
  UINT8                                     NumRanges;
  UINT8                                     Index;
  PEI_CAPSULE_PPI                           *Capsule;
  VOID                                      *CapsuleBuffer;
  UINTN                                     CapsuleBufferLength;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK            *SmramHobDescriptorBlock;
  UINTN                                     BufferSize;
  UINT8                                     SmramIndex;
  UINT8                                     SmramRanges;
//[-start-231030-IB18160164-add]//
  EFI_PEI_READ_ONLY_VARIABLE2_PPI           *Variable;
  CBS_CONFIG                                CbsConfig;
  UINTN                                     CbsConfigSize;
  EFI_PHYSICAL_ADDRESS                      PersistentMemoryBase;
  EFI_PHYSICAL_ADDRESS                      PersistentMemorySize;
//[-end-231030-IB18160164-add]//

//[-start-231030-IB18160164-add]//
  PersistentMemoryBase = 0;
  PersistentMemorySize = 0;

  Status = (**PeiServices).LocatePpi (PeiServices, &gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, &Variable);

  CbsConfigSize = sizeof (CBS_CONFIG);
  Status = Variable->GetVariable (
                       Variable,
                       L"AmdSetup",
                       &gCbsSystemConfigurationGuid,
                       NULL,
                       &CbsConfigSize,
                       (VOID *) &CbsConfig
                       );
  if (!EFI_ERROR (Status)) {
    PersistentMemoryBase = ((EFI_PHYSICAL_ADDRESS)CbsConfig.CbsCmnPersistentHigh << 32) | CbsConfig.CbsCmnPersistentLow;
    PersistentMemorySize = ((UINT64)CbsConfig.CbsCmnPersistentSizeHigh << 32) | (UINT64)CbsConfig.CbsCmnPersistentSizeLow;
  }
//[-end-231030-IB18160164-add]//

  //
  // Get the Memory Map
  //
  NumRanges = MAX_RANGES;
  ZeroMem (MemoryMap, sizeof (PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE) * NumRanges);

  Status = McGetMemoryMap (PeiServices, MemoryMap, &NumRanges);
  ASSERT_EFI_ERROR (Status);

  //
  // Find the highest memory range in processor native address space to give to
  // PEI. Then take the top.
  // If this algorithm changes, then we need to fix the capsule memory
  // selection algorithm below.
  //
  PeiMemoryBaseAddress = 0;

  //
  // Query the platform for the minimum memory size
  //
  Status = PeiServicesLocatePpi (
                            &gPeiPlatformMemorySizePpiGuid,
                            0,
                            NULL,
                            (VOID**)&MemSize
                            );
  ASSERT_EFI_ERROR (Status);

  Status = MemSize->GetPlatformMemorySize ((EFI_PEI_SERVICES**) PeiServices, MemSize, &PeiMemoryLength);
  ASSERT_EFI_ERROR (Status);

  //
  // Get required memory size for ACPI use. This helps to put ACPI memory on the top
  //
  RequiredMemSize = 0;
  RetrieveRequiredMemorySize (PeiServices, &RequiredMemSize);
    DEBUG ((EFI_D_INFO, "  RequiredMemSize = 0x%X\n", RequiredMemSize));
    DEBUG ((EFI_D_INFO, "  PeiMemoryLength = 0x%LX\n", PeiMemoryLength));

  PeiMemoryIndex = 0;
  for (Index = NumRanges - 1; (Index < NumRanges) && (Index >= 0); Index--) {

    DEBUG ((EFI_D_INFO, "  MemoryMap[%X].RowNumber       = 0x%X\n", Index, MemoryMap[Index].RowNumber));
    DEBUG ((EFI_D_INFO, "  MemoryMap[%X].PhysicalAddress = 0x%LX\n", Index, MemoryMap[Index].PhysicalAddress));
    DEBUG ((EFI_D_INFO, "  MemoryMap[%X].CpuAddress      = 0x%LX\n", Index, MemoryMap[Index].CpuAddress));
    DEBUG ((EFI_D_INFO, "  MemoryMap[%X].RangeLength     = 0x%LX\n", Index, MemoryMap[Index].RangeLength));
    DEBUG ((EFI_D_INFO, "  MemoryMap[%X].Type            = 0x%X\n\n", Index, MemoryMap[Index].Type));

    if ((MemoryMap[Index].Type == DualChannelDdrMainMemory) &&
        (MemoryMap[Index].PhysicalAddress + MemoryMap[Index].RangeLength < MAX_ADDRESS) &&
        (MemoryMap[Index].PhysicalAddress >= PeiMemoryBaseAddress) &&
        (MemoryMap[Index].RangeLength >= PeiMemoryLength)) {
      PeiMemoryBaseAddress = MemoryMap[Index].PhysicalAddress + MemoryMap[Index].RangeLength - PeiMemoryLength;
      PeiMemoryIndex = Index;
      DEBUG ((EFI_D_INFO, "  PeiMemoryBaseAddress          = 0x%LX\n", PeiMemoryBaseAddress));
      DEBUG ((EFI_D_INFO, "  PeiMemoryIndex                = 0x%X\n\n", PeiMemoryIndex));
    }
  }

  Capsule             = NULL;
  CapsuleBuffer       = NULL;
  CapsuleBufferLength = 0;
  if (BootMode == BOOT_ON_FLASH_UPDATE) {
    Status = PeiServicesLocatePpi (
                              &gPeiCapsulePpiGuid,
                              0,
                              NULL,
                              (VOID**)&Capsule
                              );
    ASSERT_EFI_ERROR (Status);
    if (Status == EFI_SUCCESS) {
      //
      // Find the largest memory range excluding that given to PEI
      //
      for (Index = 0; Index < NumRanges; Index++) {
        if ((MemoryMap[Index].Type == DualChannelDdrMainMemory) &&
            (MemoryMap[Index].PhysicalAddress + MemoryMap[Index].RangeLength < MAX_ADDRESS)
            ) {
          if (Index != PeiMemoryIndex) {
            if (MemoryMap[Index].RangeLength > CapsuleBufferLength) {
              CapsuleBuffer = (VOID *) ((UINTN) MemoryMap[Index].PhysicalAddress);
              CapsuleBufferLength = (UINTN) MemoryMap[Index].RangeLength;
            }
          } else {
             if ((MemoryMap[Index].RangeLength - PeiMemoryLength) >= CapsuleBufferLength) {
              CapsuleBuffer = (VOID *) ((UINTN) MemoryMap[Index].PhysicalAddress);
              CapsuleBufferLength = (UINTN) (MemoryMap[Index].RangeLength - PeiMemoryLength);
            }
          }
        }
      }

      //
      // Call the Capsule PPI Coalesce function to coalesce the capsule data.
      //
      Status = Capsule->Coalesce ((EFI_PEI_SERVICES**) PeiServices, &CapsuleBuffer, &CapsuleBufferLength);
    }

    //
    // If it failed, then NULL out our capsule PPI pointer so that the capsule
    // HOB does not get created below.
    //
    if (Status != EFI_SUCCESS) {
      Capsule = NULL;
    }
  }

  //
  // Carve out the top memory reserved for ACPI
  //
  DEBUG ((EFI_D_INFO, "  BuildResourceDescriptorHob PeiMemoryBaseAddress = 0x%LX PeiMemoryLength = 0x%LX\n", PeiMemoryBaseAddress, PeiMemoryLength));
  BuildResourceDescriptorHob (
    EFI_RESOURCE_SYSTEM_MEMORY,
    (EFI_RESOURCE_ATTRIBUTE_PRESENT                 |
     EFI_RESOURCE_ATTRIBUTE_INITIALIZED             |
     EFI_RESOURCE_ATTRIBUTE_TESTED                  |
     EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE             |
     EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE       |
     EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
     EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE),
    PeiMemoryBaseAddress,
    PeiMemoryLength
    );

  //
  // Install physical memory descriptor hobs for each memory range.
  //
  SmramRanges = 0;
  for (Index = 0; Index < NumRanges; Index++) {
    if (MemoryMap[Index].Type == DualChannelDdrMainMemory) {
      DEBUG ((EFI_D_INFO, "  MemoryMap[%X].Type == DualChannelDdrMainMemory\n", Index));
      if (Index == PeiMemoryIndex) {
        DEBUG ((EFI_D_INFO, "  Index == PeiMemoryIndex\n"));
        //
        // This is a partially tested Main Memory range, give it to EFI
        //
        DEBUG ((EFI_D_INFO, "  BuildResourceDescriptorHob MemoryMap[%X].PhysicalAddress = 0x%LX Length = 0x%LX\n", Index, MemoryMap[Index].PhysicalAddress, (MemoryMap[Index].RangeLength - PeiMemoryLength)));
        BuildResourceDescriptorHob (
          EFI_RESOURCE_SYSTEM_MEMORY,
          (EFI_RESOURCE_ATTRIBUTE_PRESENT                 |
           EFI_RESOURCE_ATTRIBUTE_INITIALIZED             |
           EFI_RESOURCE_ATTRIBUTE_TESTED                  |
           EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE             |
           EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE       |
           EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
           EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE),
          MemoryMap[Index].PhysicalAddress,
          MemoryMap[Index].RangeLength - PeiMemoryLength
          );
      } else {
        DEBUG ((EFI_D_INFO, "  Index != PeiMemoryIndex\n"));
        //
        // This is an untested Main Memory range, give it to EFI
        //
//[-start-231030-IB18160164-modify]//
        if ((PersistentMemoryBase >= 0x0000000100000000) &&
            (MemoryMap[Index].PhysicalAddress <= PersistentMemoryBase) &&
            ((MemoryMap[Index].PhysicalAddress + MemoryMap[Index].RangeLength) >= (PersistentMemoryBase + PersistentMemorySize))) {
          if (MemoryMap[Index].PhysicalAddress < PersistentMemoryBase) {
            DEBUG ((EFI_D_INFO, "  BuildResourceDescriptorHob MemoryMap[%X].PhysicalAddress = 0x%LX Length = 0x%LX\n", Index, MemoryMap[Index].PhysicalAddress, PersistentMemoryBase - MemoryMap[Index].PhysicalAddress));
            BuildResourceDescriptorHob (
              EFI_RESOURCE_SYSTEM_MEMORY,                      // MemoryType,
              (EFI_RESOURCE_ATTRIBUTE_PRESENT                 |
               EFI_RESOURCE_ATTRIBUTE_INITIALIZED             |
               EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE             |
               EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE       |
               EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
               EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE),   // MemoryAttribute
              MemoryMap[Index].PhysicalAddress,                // MemoryBegin
              PersistentMemoryBase - MemoryMap[Index].PhysicalAddress // MemoryLength
              );
          }
          DEBUG ((EFI_D_INFO, "  BuildResourceDescriptorHob MemoryMap[%X].PhysicalAddress = 0x%LX Length = 0x%LX\n", Index, PersistentMemoryBase, PersistentMemorySize));
          BuildResourceDescriptorHob (
            EFI_RESOURCE_SYSTEM_MEMORY,                      // MemoryType,
            (EFI_RESOURCE_ATTRIBUTE_PRESENT                 |
             EFI_RESOURCE_ATTRIBUTE_INITIALIZED             |
             EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE             |
             EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE       |
             EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
             EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE    |
             EFI_RESOURCE_ATTRIBUTE_PERSISTENT),             // MemoryAttribute
            PersistentMemoryBase,                            // MemoryBegin
            PersistentMemorySize                             // MemoryLength
            );
          if ((MemoryMap[Index].PhysicalAddress + MemoryMap[Index].RangeLength) > (PersistentMemoryBase + PersistentMemorySize)) {
            DEBUG ((EFI_D_INFO, "  BuildResourceDescriptorHob MemoryMap[%X].PhysicalAddress = 0x%LX Length = 0x%LX\n", Index, PersistentMemoryBase + PersistentMemorySize, (MemoryMap[Index].PhysicalAddress + MemoryMap[Index].RangeLength) - (PersistentMemoryBase + PersistentMemorySize)));
            BuildResourceDescriptorHob (
              EFI_RESOURCE_SYSTEM_MEMORY,                      // MemoryType,
              (EFI_RESOURCE_ATTRIBUTE_PRESENT                 |
               EFI_RESOURCE_ATTRIBUTE_INITIALIZED             |
               EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE             |
               EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE       |
               EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
               EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE),   // MemoryAttribute
              PersistentMemoryBase + PersistentMemorySize,     // MemoryBegin
              (MemoryMap[Index].PhysicalAddress + MemoryMap[Index].RangeLength) - (PersistentMemoryBase + PersistentMemorySize) // MemoryLength
              );
          }
        } else {
          DEBUG ((EFI_D_INFO, "  BuildResourceDescriptorHob MemoryMap[%X].PhysicalAddress = 0x%LX Length = 0x%LX\n", Index, MemoryMap[Index].PhysicalAddress, MemoryMap[Index].RangeLength));
          BuildResourceDescriptorHob (
            EFI_RESOURCE_SYSTEM_MEMORY,                      // MemoryType,
            (EFI_RESOURCE_ATTRIBUTE_PRESENT                 |
             EFI_RESOURCE_ATTRIBUTE_INITIALIZED             |
             EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE             |
             EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE       |
             EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
             EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE),   // MemoryAttribute
            MemoryMap[Index].PhysicalAddress,                // MemoryBegin
            MemoryMap[Index].RangeLength                     // MemoryLength
            );
        }
//[-end-231030-IB18160164-modify]//
      }
    } else {
      DEBUG ((EFI_D_INFO, "  MemoryMap[%X].Type != DualChannelDdrMainMemory\n", Index));
      if ((MemoryMap[Index].Type == DualChannelDdrSmramCacheable) ||
          (MemoryMap[Index].Type == DualChannelDdrSmramNonCacheable)) {
        DEBUG ((EFI_D_INFO, "  MemoryMap[%X].Type == DualChannelDdrSmramCacheable or DualChannelDdrSmramNonCacheable\n", Index));
        //
        // Only count and report TSEG.
        //
        SmramRanges++;
      }
      //
      // Make sure non-system memory is marked as reserved.
      //
      DEBUG ((EFI_D_INFO, "  BuildResourceDescriptorHob MemoryMap[%X].PhysicalAddress = 0x%LX Length = 0x%LX\n", Index, MemoryMap[Index].PhysicalAddress, MemoryMap[Index].RangeLength));
      BuildResourceDescriptorHob (
        EFI_RESOURCE_MEMORY_RESERVED,     // MemoryType,
        0,                                // MemoryAttribute
        MemoryMap[Index].PhysicalAddress, // MemoryBegin
        MemoryMap[Index].RangeLength      // MemoryLength
        );
    }
  }

  //
  // InstallPeiMemory
  //
  DEBUG ((EFI_D_INFO, "  PeiMemoryBaseAddress = 0x%LX PeiMemoryLength = 0x%LX RequiredMemSize = 0x%LX\n", PeiMemoryBaseAddress, PeiMemoryLength, RequiredMemSize));
  DEBUG ((EFI_D_INFO, "  InstallPeiMemory MemoryBegin = 0x%LX MemoryLength = 0x%LX\n", PeiMemoryBaseAddress, (PeiMemoryLength - RequiredMemSize)));
  Status = (*PeiServices)->InstallPeiMemory (
                             PeiServices,
                             PeiMemoryBaseAddress,
                             PeiMemoryLength - RequiredMemSize
                             );
  ASSERT_EFI_ERROR (Status);

  //
  // Install physical memory descriptor hobs for Smram Memory Reserve.
  //
  BufferSize = sizeof (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK);
  if (SmramRanges > 0) {
    BufferSize += ((SmramRanges - 1) * sizeof (EFI_SMRAM_DESCRIPTOR));
  }

  SmramHobDescriptorBlock = BuildGuidHob (&gEfiSmmPeiSmramMemoryReserveGuid, BufferSize);
  if (SmramHobDescriptorBlock == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  SmramHobDescriptorBlock->NumberOfSmmReservedRegions = SmramRanges;

  SmramIndex = 0;
  for (Index = 0; Index < NumRanges; Index++) {
    if ((MemoryMap[Index].Type == DualChannelDdrSmramCacheable) ||
        (MemoryMap[Index].Type == DualChannelDdrSmramNonCacheable)) {
      //
      // This is an SMRAM range, create an SMRAM descriptor
      //
      SmramHobDescriptorBlock->Descriptor[SmramIndex].PhysicalStart = MemoryMap[Index].PhysicalAddress;
      SmramHobDescriptorBlock->Descriptor[SmramIndex].CpuStart      = MemoryMap[Index].CpuAddress;
      SmramHobDescriptorBlock->Descriptor[SmramIndex].PhysicalSize  = MemoryMap[Index].RangeLength;
      if (MemoryMap[Index].Type == DualChannelDdrSmramCacheable) {
        SmramHobDescriptorBlock->Descriptor[SmramIndex].RegionState = EFI_SMRAM_CLOSED | EFI_CACHEABLE;
      } else {
        SmramHobDescriptorBlock->Descriptor[SmramIndex].RegionState = EFI_SMRAM_CLOSED;
      }
      SmramIndex++;
    }
  }

  //
  // If we found the capsule PPI (and we didn't have errors), then
  // call the capsule PEIM to allocate memory for the capsule.
  //
  if (Capsule != NULL) {
    Status = Capsule->CreateState (
                        (EFI_PEI_SERVICES**) PeiServices,
                        CapsuleBuffer,
                        CapsuleBufferLength
                        );
  }

  return EFI_SUCCESS;
}

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
  )
{
  EFI_STATUS                      Status;
  UINTN                           VarSize;
  UINTN                           VarAttrib;
  ACPI_VARIABLE_SET               *AcpiVariableSet;
  UINTN                           S3MemoryBase;
  UINTN                           S3MemorySize;
  UINT64                          TempAddress;
  UINT64                          MemoryLengthBelow4G;
  UINT64                          MemoryLengthOver4G;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK            *SmramHobDescriptorBlock;
  UINTN                                     BufferSize;
  UINT8                                     SmramRanges;
  UINT64                                    TsegBase;
  UINT64                                    TsegMask;

  MemoryLengthBelow4G = 0;
  MemoryLengthOver4G  = 0;

  VarSize         = sizeof (UINT32);
  VarAttrib       = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;
  AcpiVariableSet = NULL;
  Status = VariableServices->GetVariable (
                               VariableServices,
                               ACPI_GLOBAL_VARIABLE,
                               &gAcpiVariableSetGuid,
                               &VarAttrib,
                               &VarSize,
                               &AcpiVariableSet
                               );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    VarSize = sizeof (UINT64);
    Status = VariableServices->GetVariable (
                                 VariableServices,
                                 ACPI_GLOBAL_VARIABLE,
                                 &gAcpiVariableSetGuid,
                                 &VarAttrib,
                                 &VarSize,
                                 &TempAddress
                                 );
    if (!EFI_ERROR (Status)) {
      AcpiVariableSet = (ACPI_VARIABLE_SET *)(UINTN)TempAddress;
    }
  }
  if (EFI_ERROR (Status) || (AcpiVariableSet == NULL)) {
    return EFI_OUT_OF_RESOURCES;
  }

//  wait for porting
//   if (AcpiVariableSet->Signature != ACPI_VARIABLE_SIGNATURE) {
//     return EFI_DEVICE_ERROR;
//   }

//   BuildGuidDataHob (&gEfiAcpiVariableGuid, AcpiVariableSet, sizeof (ACPI_VARIABLE_SET));

  S3MemoryBase = (UINTN)(AcpiVariableSet->AcpiReservedMemoryBase);
  S3MemorySize = (UINTN)(AcpiVariableSet->AcpiReservedMemorySize);
  Status = (*PeiServices)->InstallPeiMemory (
                             PeiServices,
                             S3MemoryBase,   // MemoryBegin
                             S3MemorySize    // MemoryLength
                             );
  ASSERT_EFI_ERROR (Status);

  //
  // Retrieve the system memory length and build memory hob for the system
  // memory above 1MB. So Memory Callback can set cache for the system memory
  // correctly on S3 boot path, just like it does on Normal boot path.
  //
  ASSERT ((AcpiVariableSet->SystemMemoryLengthBelow4GB - 0x100000) > 0);
  MemoryLengthBelow4G = AcpiVariableSet->SystemMemoryLengthBelow4GB;
  MemoryLengthOver4G = AcpiVariableSet->SystemMemoryLengthAbove4GB;

  BuildResourceDescriptorHob (
    EFI_RESOURCE_SYSTEM_MEMORY,
    (EFI_RESOURCE_ATTRIBUTE_PRESENT                 |
     EFI_RESOURCE_ATTRIBUTE_INITIALIZED             |
     EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE             |
     EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE       |
     EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
     EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE),
    0x100000,
    MemoryLengthBelow4G - 0x100000
    );

  if (MemoryLengthOver4G > 0){
    BuildResourceDescriptorHob (
      EFI_RESOURCE_SYSTEM_MEMORY,
      (EFI_RESOURCE_ATTRIBUTE_PRESENT                 |
       EFI_RESOURCE_ATTRIBUTE_INITIALIZED             |
       EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE             |
       EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE       |
       EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
       EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE),
      0x100000000,
      MemoryLengthOver4G
      );
  }

  //
  // Report SMRAM ranges
  //
  SmramRanges = 1;

  BufferSize = sizeof (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK);
  if (SmramRanges > 0) {
    BufferSize += ((SmramRanges - 1) * sizeof (EFI_SMRAM_DESCRIPTOR));
  }

  SmramHobDescriptorBlock = BuildGuidHob (&gEfiSmmPeiSmramMemoryReserveGuid, BufferSize);
  if (SmramHobDescriptorBlock == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  SmramHobDescriptorBlock->NumberOfSmmReservedRegions = SmramRanges;
  //
  // Todo: calculate uma size and cc6 to determine t-seg
  //
//  SmramHobDescriptorBlock->Descriptor[0].PhysicalStart = MemoryLengthBelow4G;
//  SmramHobDescriptorBlock->Descriptor[0].CpuStart      = MemoryLengthBelow4G;
  TsegBase = AsmReadMsr64 (0xC0010112) & 0xFFFFFFFE0000ul;
  TsegMask = (UINT64)FixedPcdGet32 (PcdTsegSize);

  SmramHobDescriptorBlock->Descriptor[0].PhysicalStart = TsegBase;
  SmramHobDescriptorBlock->Descriptor[0].CpuStart      = TsegBase;
  SmramHobDescriptorBlock->Descriptor[0].PhysicalSize  = TsegMask;
  SmramHobDescriptorBlock->Descriptor[0].RegionState   = EFI_SMRAM_CLOSED | EFI_CACHEABLE;

  return EFI_SUCCESS;
}
