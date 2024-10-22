/** @file
 PEI Chipset Services Library.

 This file contains only one function that is PeiCsSvcModifyMemoryRange().
 The function PeiCsSvcModifyMemoryRange() use chipset services to modify
 memory range setting.

***************************************************************************
* Copyright (c) 2012 - 2018, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Library/HobLib.h>
#include <Library/BaseLib.h>
#include <Guid/UmaExistHob.h>
#include <Guid/AmdMemoryInfoHob.h>
#include <Ppi/PlatformMemoryRange.h>

/**
 To modify memory range setting.


 @param[in, out]    OptionRomMask       A pointer to PEI_MEMORY_RANGE_OPTION_ROM
 @param[in, out]    SmramMask           A pointer to PEI_MEMORY_RANGE_SMRAM
 @param[in, out]    GraphicsMemoryMask  A pointer to PEI_MEMORY_RANGE_GRAPHICS_MEMORY
 @param[in, out]    PciMemoryMask       A pointer to PEI_MEMORY_RANGE_PCI_MEMORY

 @return            EFI_SUCCESS         Always return successfully
*/
EFI_STATUS
ModifyMemoryRange (
  IN OUT  PEI_MEMORY_RANGE_OPTION_ROM           *OptionRomMask,
  IN OUT  PEI_MEMORY_RANGE_SMRAM                *SmramMask,
  IN OUT  PEI_MEMORY_RANGE_GRAPHICS_MEMORY      *GraphicsMemoryMask,
  IN OUT  PEI_MEMORY_RANGE_PCI_MEMORY           *PciMemoryMask
  )
{
  EFI_HOB_GUID_TYPE             *GuidHob;
  AMD_MEMORY_INFO_HOB           *MemInfoHob;
  AMD_MEMORY_RANGE_DESCRIPTOR   *MemRangDesc;
  UINT32                        NumberOfDescriptor;
  UINT32                        Index;
  EFI_UMA_EXIST_HOB             UmaExistHob;
  UINT32                        TsegSize;

  //
  // Choose one or none.
  //
  *SmramMask |= PEI_MR_SMRAM_ABSEG_128K_NOCACHE;

  //
  // Choose one or none of the following:
  //   PEI_MR_SMRAM_TSEG_2048K_CACHE
  //   PEI_MR_SMRAM_TSEG_4096K_CACHE
  //   PEI_MR_SMRAM_TSEG_8192K_CACHE
  //
  TsegSize = FixedPcdGet32 (PcdTsegSize) >> 17;
  switch (TsegSize) {
  
  case PEI_MR_SMRAM_SIZE_4096K_MASK:
    *SmramMask |= PEI_MR_SMRAM_TSEG_4096K_CACHE;
    break;
  case PEI_MR_SMRAM_SIZE_8192K_MASK:
    *SmramMask |= PEI_MR_SMRAM_TSEG_8192K_CACHE;
    break;  	
  }

  GuidHob    = NULL;
  MemInfoHob = NULL;
  GuidHob = GetFirstGuidHob (&gAmdMemoryInfoHobGuid);
  if (GuidHob != NULL) {
    MemInfoHob = GET_GUID_HOB_DATA (GuidHob);
  }

  *GraphicsMemoryMask      = PEI_MR_GRAPHICS_MEMORY_NONE;
  MemRangDesc              = NULL;
  NumberOfDescriptor       = 0;
  UmaExistHob.UmaExistFlag = FALSE;
  UmaExistHob.UmaSize      = 0;
  if (MemInfoHob != NULL) {
    NumberOfDescriptor = MemInfoHob->NumberOfDescriptor;
    MemRangDesc = &MemInfoHob->Ranges[0];
    for (Index = 0; Index <= NumberOfDescriptor; Index++) {
      if (MemRangDesc->Attribute == AMD_MEMORY_ATTRIBUTE_UMA) {
        UmaExistHob.UmaExistFlag |= BIT0;
        UmaExistHob.UmaSize = (UINT32) MemRangDesc->Size;
        *GraphicsMemoryMask = (PEI_MEMORY_RANGE_GRAPHICS_MEMORY) RShiftU64 ((UmaExistHob.UmaSize), 20);
        *GraphicsMemoryMask = *GraphicsMemoryMask * 2;
        if(MemRangDesc->Base < 0x100000000) {
          UmaExistHob.UmaExistFlag |= BIT1;
          *GraphicsMemoryMask |= (1 << 30); // PEI_MR_GRAPHICS_MEMORY_BELOW4G;
        }
        break;
      }
      MemRangDesc++;
    }
  }

  BuildGuidDataHob (&gUmaExistHobGuid, &UmaExistHob, sizeof (EFI_UMA_EXIST_HOB));

  return EFI_SUCCESS;
}