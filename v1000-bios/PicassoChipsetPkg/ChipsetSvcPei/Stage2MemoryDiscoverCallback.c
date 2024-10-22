/** @file
 PEI Chipset Services Library.

 This file contains only one function that is PeiCsSvcStage2MemoryDiscoverCallback().
 The function PeiCsSvcStage2MemoryDiscoverCallback() use chipset services to install
 Firmware Volume Hob's once there is main memory.

***************************************************************************
* Copyright (c) 2016 - 2017, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <PiPei.h>
#include <ChipsetSetupConfig.h>
//
// Libraries
//
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/PciExpressLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/AmdPspFtpmPpi.h>

//
// Consume Ppi
//
#include <Ppi/Cache.h>
#include <Ppi/PlatformMemoryRange.h>

#include <AGESA.h>
#include <Hudson-2Reg.h>


typedef struct _PEI_SET_MEMORY_RANGE{
  UINTN                         Address;
  UINTN                         Length;
  EFI_MEMORY_CACHE_TYPE         Type;
} PEI_SET_MEMORY_RANGE;

PEI_SET_MEMORY_RANGE    mPeiSetMemory[] = {
  {0x00000,  0xA0000,  EfiCacheWriteBack},
  {0xA0000,  0x20000,  EfiCacheUncacheable},
  {0xC0000,  0x40000,  EfiCacheWriteThrough},
};

typedef struct _MEM_MAP {
  EFI_PHYSICAL_ADDRESS          BaseAddress;
  UINT64                        Length;
} MEM_MAP;

/**
 Retrieve the base address for the SPI ROM controller
 for LPC Bridge Configuration Registers.

 @param [in]        None

 @retval            EFI_PHYSICAL_ADDRESS The base address for the SPI ROM controller
*/
EFI_PHYSICAL_ADDRESS
GetFchSpiBar (
  VOID
  )
{
  EFI_PHYSICAL_ADDRESS  SpiBar;

  SpiBar = PciExpressRead32 (PCI_EXPRESS_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, FCH_LPC_REGA0));
  SpiBar &= 0x00000000FFFFFF00; // Raven: Bit[31:8]: Spi_eSpi_BaseAddr.

  return SpiBar;
}

/**
 Turn off system if needed

 @param [in]        PeiServices         Describes the list of possible PEI Services
 @param [in]        CpuIo               A pointer to CPU IO Services PPI.
 @param [in]        StartMemoryAddr     Start Address of memory which wants to cache
 @param [in]        MemoryLength        Memory length want to be cache

 @retval            None
*/
VOID
CheckPowerOffNow (
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_CPU_IO_PPI           *CpuIo
  )
{
  UINT16  PmBase;
  UINT16  Pm1Sts;

  PmBase = MmioRead16 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG60);
  //
  // Read the ACPI registers
  //
  Pm1Sts = IoRead16 (PmBase + R_FCH_ACPI_PM1_STATUS);
  if ((Pm1Sts & B_PWR_BTN_STATUS) == B_PWR_BTN_STATUS) {
    IoWrite16 (PmBase + R_FCH_ACPI_PM1_STATUS, B_PWR_BTN_STATUS);
    IoWrite16 (PmBase + R_FCH_ACPI_PM_CONTROL, V_SLP_TYPE_S5);
    IoWrite16 (PmBase + R_FCH_ACPI_PM_CONTROL, V_SLP_TYPE_S5 + V_ACPI_SLP_EN);
  }
}

/**
 Set Variable Mtrr by StartMemoryAddr and MemoryLength

 @param [in]        PeiServices         Describes the list of possible PEI Services
 @param [in]        CachePpi            A pointer to Cache Services PPI
 @param [in]        StartMemoryAddr     Start Address of memory which wants to cache
 @param [in]        MemoryLength        Memory length want to be cache

 @retval            None
*/
VOID
SetVariableMtrr (
  IN CONST EFI_PEI_SERVICES             **PeiServices,
  IN       PEI_CACHE_PPI                *CachePpi,
  IN       UINT64                       StartMemoryAddr,
  IN       UINT64                       MemoryLength,
  IN       EFI_MEMORY_CACHE_TYPE        MemoryCacheType
  )
{
  UINT64 Memory1, Memory2, Memory3, Memory4;

  Memory1 = Memory2 = Memory3 = Memory4 = 0;
  Memory1 = MemoryLength;
  Memory2 = StartMemoryAddr;
  do {
    Memory3 = GetPowerOfTwo64 (Memory1);
    CachePpi->SetCache (
                (EFI_PEI_SERVICES**)PeiServices,
                CachePpi,
                Memory2,
                Memory3,
                MemoryCacheType
                );
    Memory4 = Memory1 - Memory3;
    Memory2 += Memory3;
    Memory1 = Memory4;
  } while (Memory1 > 0);
}


/**
 Check whether gAmdPspFtpmFactoryResetPpiGuid PPI is installed or not

 @param [in]        PeiServices         Describes the list of possible PEI Services

 @retval            EFI_SUCCESS         gAmdPspFtpmFactoryResetPpiGuid have been installed
*/
EFI_STATUS
CheckPspFtpmFactoryReset (
  IN CONST EFI_PEI_SERVICES             **PeiServices
)
{
  EFI_STATUS                            Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI       *VariableServices;
  UINTN                                 VariableSize;
  VOID                                  *SystemConfiguration;
  EFI_GUID                              SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
  PSP_FTPM_FACTORY_RESET_PPI            *AmdPspFtpmFactoryResetPpi;

  //
  // By default set PcdAmdPspFactoryReset to FALSE, set it to TRUE only
  // when both setup option is enabled and gAmdPspFtpmFactoryResetPpiGuid is published
  //
  PcdSetBool (PcdAmdPspFactoryReset, FALSE);

  VariableServices = NULL;
  //
  // Locate Variable Ppi
  //
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **)&VariableServices
             );
  ASSERT_EFI_ERROR (Status);

  //
  // Get Setup Variable
  //
  VariableSize = sizeof(CHIPSET_CONFIGURATION);
  SystemConfiguration = AllocateZeroPool (VariableSize);

  Status = VariableServices->GetVariable (
                               VariableServices,
                               L"Setup",
                               &SystemConfigurationGuid,
                               NULL,
                               &VariableSize,
                               SystemConfiguration
                               );

  if (EFI_ERROR (Status)) {
    return Status;
  }



  if(((CHIPSET_CONFIGURATION *)SystemConfiguration)->ErasefTPMNV != 1) {
  	return EFI_NOT_FOUND;
  }
  DEBUG ((EFI_D_INFO, "ErasefTPMNV of SETUP iS enable\n"));

  Status = (**PeiServices).LocatePpi (
                             PeiServices,
                             &gAmdPspFtpmFactoryResetPpiGuid,
                             0,
                             NULL,
                             (VOID**)&AmdPspFtpmFactoryResetPpi
                             );
  if (EFI_ERROR (Status)) {
   return Status;
  }

  DEBUG ((EFI_D_INFO, "gAmdPspFtpmFactoryResetPpiGuid is install, PcdAmdPspFactoryReset will be set TRUE\n"));
  PcdSetBool (PcdAmdPspFactoryReset, TRUE);

  return EFI_SUCCESS;

}

/**
 Install Firmware Volume Hob's once there is main memory

 @param [in]        BootMode            A pointer to boot mode status

 @retval            EFI_SUCCESS         if the interface could be successfully installed
*/
EFI_STATUS
Stage2MemoryDiscoverCallback (
  IN       EFI_BOOT_MODE                *BootMode
  )
{
  EFI_STATUS                            Status;
  CONST EFI_PEI_SERVICES                **PeiServices;
  PEI_CACHE_PPI                         *CachePpi;
  PEI_PLATFORM_MEMORY_RANGE_PPI         *MemoryRangePpi;
  PEI_MEMORY_RANGE_GRAPHICS_MEMORY      GraphicsMemoryMask;
  PEI_MEMORY_RANGE_PCI_MEMORY           PciMemoryMask;
  PEI_MEMORY_RANGE_OPTION_ROM           OptionRomMask;
  PEI_MEMORY_RANGE_SMRAM                SmramMask;
  EFI_PEI_HOB_POINTERS                  Hob;
  UINT64                                MemoryLength;
  UINT64                                MaxMemoryLength;
  UINT64                                RemapMemoryLength;
  UINT64                                RemapMemoryBase;
  UINT64                                TSegmentSize;
  UINT32                                CC6SaveEn;
  UINT16                                Index;
  MEM_MAP                               MmioMap[5];
  UINT8                                 TpmType;
  EFI_PHYSICAL_ADDRESS                  FVMain;

  PeiServices = GetPeiServicesTablePointer ();

  if (*BootMode != BOOT_ON_S3_RESUME) {
    CheckPowerOffNow (PeiServices, (**PeiServices).CpuIo);
  }

  Status = (**PeiServices).LocatePpi (
                             PeiServices,
                             &gPeiCachePpiGuid,
                             0,
                             NULL,
                             (VOID**)&CachePpi
                             );
  ASSERT_EFI_ERROR (Status);

  //
  // Clear the Cache as Ram (CAR) settings.
  //
  CachePpi->ResetCache (
              (EFI_PEI_SERVICES **)PeiServices,
              CachePpi
              );
  //
  // Mtrr : Bios Rom
  //
  CachePpi->SetCache (
                (EFI_PEI_SERVICES **)PeiServices,
                CachePpi,
                FixedPcdGet32 (PcdFlashAreaBaseAddress),
                FixedPcdGet32 (PcdFlashAreaSize),
                EfiCacheWriteProtected
                );

  //BugBug!! IB0274xxxx
//   if (*BootMode !=  BOOT_ON_S3_RESUME) {
//     MemoryLength  = SIZE_1MB;
//   } else {
//     MemoryLength  = 0;
//   }

  //
  // Set to 1 MB. Since 1MB cacheability will always be set
  // until override by CSM
  //
  MemoryLength      = SIZE_1MB;
  RemapMemoryLength = 0;
  RemapMemoryBase   = BASE_4GB;
  Status = (*PeiServices)->GetHobList (PeiServices, (VOID**)&Hob.Raw);
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
      if (Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY) {
        //
        // Skip the memory region below 1MB
        //
        if ((Hob.ResourceDescriptor->PhysicalStart >= SIZE_1MB) &&
            (Hob.ResourceDescriptor->PhysicalStart < SIZE_4GB)) {
          MemoryLength += Hob.ResourceDescriptor->ResourceLength;
        } else if (Hob.ResourceDescriptor->PhysicalStart >= SIZE_4GB) {
          if (Hob.ResourceDescriptor->PhysicalStart < RemapMemoryBase) {
            RemapMemoryBase = Hob.ResourceDescriptor->PhysicalStart;
          }
          RemapMemoryLength += Hob.ResourceDescriptor->ResourceLength;
        }
      }
    }
    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  MaxMemoryLength = MemoryLength;
  TSegmentSize    = 0;

  //
  // Get platform memory range service
  //
  Status = (**PeiServices).LocatePpi (
                               PeiServices,
                               &gPeiPlatformMemoryRangePpiGuid,
                               0,
                               NULL,
                               (VOID**)&MemoryRangePpi
                               );
  ASSERT_EFI_ERROR (Status);

  Status = MemoryRangePpi->ChooseRanges (
                               (EFI_PEI_SERVICES**)PeiServices,
                               MemoryRangePpi,
                               &OptionRomMask,
                               &SmramMask,
                               &GraphicsMemoryMask,
                               &PciMemoryMask
                               );

  if (SmramMask & PEI_MR_SMRAM_TSEG_MASK) {
    TSegmentSize = ((SmramMask & PEI_MR_SMRAM_SIZE_MASK & (~(1 << 0))) * 128 * 1024);
  }
  DEBUG ((DEBUG_INFO, " RemapMemoryLength = 0x%LX\n", (UINT64)RemapMemoryLength));
  DEBUG ((DEBUG_INFO, " MaxMemoryLength = 0x%LX\n", (UINT64)MaxMemoryLength));
  DEBUG ((DEBUG_INFO, " TSegmentSize = 0x%LX\n", (UINT64)TSegmentSize));
  if (RemapMemoryLength == 0) { // No memory above 4G
    DEBUG ((DEBUG_INFO, " No memory above 4G\n"));
    CC6SaveEn = PciExpressRead32 (PCI_EXPRESS_LIB_ADDRESS (0, 0x18, 0x02, 0x118));
    DEBUG ((DEBUG_INFO, " CC6SaveEn = 0x%X\n", CC6SaveEn));

    if (CC6SaveEn & BIT18) { // C6 is enabled
      DEBUG ((DEBUG_INFO, " C6 is enabled\n"));
        //
        //  _2GB_Physical DRAM______________________________
        //  |         C6 stroage region (16MB)             |
        //  |TOM___________________________________________|
        //  |                                              |
        //  |         UMA               (512MB)            |
        //  |                                              |<-- MaxMemoryLength + TSegmentSize + MEMORY_SIZE_16M, under this address set to WB
        //  |______________________________________________|<-- This 16MB region will be set to WB, but UMA must be UC
        //  |         T-seg             (TSegmentSize)     |
        //  |_____________________________MaxMemoryLength__|
        //  |                                              |
        //  |         System memory                        |
        //  |                                              |
        //  |                                              |
        //  |                                              |
        //
        // C6 enabled and C6 storage region address is under 4G
        //
      SetVariableMtrr (PeiServices, CachePpi, 0, (MaxMemoryLength + TSegmentSize + SIZE_16MB), EfiCacheWriteBack);
      //
      // Set overlap UMA region to UC
      //
      SetVariableMtrr (PeiServices, CachePpi, MaxMemoryLength + TSegmentSize, SIZE_16MB, EfiCacheUncacheable);
    } else {
      DEBUG ((DEBUG_INFO, " C6 is disabled\n"));
      //
      // C6 disable when memory less than 4G
      //
      SetVariableMtrr (PeiServices, CachePpi, 0, (MaxMemoryLength + TSegmentSize), EfiCacheWriteBack);
    }
  } else {
    DEBUG ((DEBUG_INFO, " Memory is Over than 4G\n"));
    //
    // Memory is Over than 4G
    //
    SetVariableMtrr (PeiServices, CachePpi, 0, (MaxMemoryLength + TSegmentSize), EfiCacheWriteBack);
  }

  //
  // MTRR : Over 4G
  //
  if (RemapMemoryLength != 0) {
    //
    // Set AMD MSR SYS_CFG (0xC0010010) Bit 22 Tom2ForceMemTypeWB to 1 to instead of Variable MTRR setting
    //
    UINT64      MsrData;

    MsrData = AsmReadMsr64 (SYS_CFG);
    MsrData |= BIT22;
    AsmWriteMsr64 (SYS_CFG, MsrData);
  }

  //
  // MTRR : Below 1M
  //
  for (Index = 0; Index < (sizeof(mPeiSetMemory)/sizeof(PEI_SET_MEMORY_RANGE)); Index++) {
    CachePpi->SetCache(
                  (EFI_PEI_SERVICES **)PeiServices,
                  CachePpi,
                  mPeiSetMemory[Index].Address,
                  mPeiSetMemory[Index].Length,
                  mPeiSetMemory[Index].Type
                  );
  }

  //
  // Install resource descriptor hobs for each Mmio range.
  //
  MmioMap [0].BaseAddress = PcdGet64 (PcdPciExpressBaseAddress);
  MmioMap [0].Length      = PcdGet32 (PcdPciExpressSize);
  MmioMap [1].BaseAddress = PcdGet32 (PcdIoApicBaseAddress);
  MmioMap [1].Length      = SIZE_4KB;
  MmioMap [2].BaseAddress = PcdGet32 (PcdCpuLocalApicBaseAddress);
  MmioMap [2].Length      = SIZE_4KB;
  MmioMap [3].BaseAddress = ACPI_MMIO_BASE;
  MmioMap [3].Length      = SIZE_4KB;
  MmioMap [4].BaseAddress = GetFchSpiBar ();
  MmioMap [4].Length      = SIZE_4KB;

  for (Index = 0; Index < (sizeof(MmioMap)/(sizeof(MEM_MAP))); Index++) {
    DEBUG ((DEBUG_INFO, " BuildResourceDescriptorHob EFI_RESOURCE_MEMORY_MAPPED_IO from 0x%LX to 0x%LX\n", (UINT64)MmioMap[Index].BaseAddress, (UINT64)(MmioMap[Index].BaseAddress + MmioMap[Index].Length - 1)));
    BuildResourceDescriptorHob (
      EFI_RESOURCE_MEMORY_MAPPED_IO,
      (EFI_RESOURCE_ATTRIBUTE_PRESENT     |
       EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
       EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
      MmioMap[Index].BaseAddress,
      MmioMap[Index].Length
      );

    DEBUG ((DEBUG_INFO, " BuildMemoryAllocationHob EfiMemoryMappedIO from 0x%LX to 0x%LX\n", (UINT64)MmioMap[Index].BaseAddress, (UINT64)(MmioMap[Index].BaseAddress + MmioMap[Index].Length - 1)));
    BuildMemoryAllocationHob (
      MmioMap[Index].BaseAddress,
      MmioMap[Index].Length,
      EfiMemoryMappedIO
      );
  }

    DEBUG ((EFI_D_ERROR, "Family 17: Reserved memory for BFV\n"));
    DEBUG ((DEBUG_INFO, " BuildMemoryAllocationHob EfiReservedMemoryType from 0x%LX to 0x%LX\n", (UINT64)PcdGet32(PcdBootFirmwareVolumeBase), (UINT64)(PcdGet32(PcdBootFirmwareVolumeBase) + PcdGet32(PcdBootFirmwareVolumeSize) - 1)));
    BuildMemoryAllocationHob (
      PcdGet32(PcdBootFirmwareVolumeBase),
      PcdGet32(PcdBootFirmwareVolumeSize),
      EfiReservedMemoryType
      );
  // If TPM is enabled, copy FV_MAIN from SPI to memory before measurement
  TpmType = PcdGet8(PcdTpmType);
  if ( TpmType != 0){
    if(PcdGet8(SpiOrLpcTPM) && TpmType == 1) {
      // If dTPM is enabled, enable SPI Pre-Fetch before copying FV_MAIN
      PciExpressOr32 (
        PCI_EXPRESS_LIB_ADDRESS (
          FCH_LPC_BUS,
          FCH_LPC_DEV,
          FCH_LPC_FUNC,
          R_FCH_LPC_ROM_DMA_CONTROL_REGISTER
        ),
        V_PREFETCH_SPI_ENABLE
      );
    }

if (*BootMode != BOOT_IN_RECOVERY_MODE) {
    Status = PeiServicesAllocatePages (
               EfiBootServicesData,
               EFI_SIZE_TO_PAGES (FixedPcdGet32 (PcdFlashFvMainSize)),
               &FVMain
               );
    if (!EFI_ERROR (Status)) {
      CopyMem (
        (UINT8 *)(UINTN)FVMain,
        (UINT8 *)(UINTN)(FixedPcdGet32 (PcdFlashFvMainBase)),
        FixedPcdGet32 (PcdFlashFvMainSize)
      );

      BuildFvHob (
        (UINTN)FVMain,
        FixedPcdGet32 (PcdFlashFvMainSize)
      );
    }
}
    if(PcdGet8(SpiOrLpcTPM) && TpmType == 1) {
      // If dTPM is disabled, disble SPI Prefetch
      PciExpressAnd32 (
        PCI_EXPRESS_LIB_ADDRESS (
          FCH_LPC_BUS,
          FCH_LPC_DEV,
          FCH_LPC_FUNC,
          R_FCH_LPC_ROM_DMA_CONTROL_REGISTER
        ),
        ~(UINT32) B_PREFETCH_EN_SPI_FROM_HOST
      );
    }
  }

  CheckPspFtpmFactoryReset (PeiServices);

  return EFI_SUCCESS;
}
