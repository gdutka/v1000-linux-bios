/** @file
 PEI Chipset Services driver.

 It produces an instance of the PEI Chipset Services PPI to provide the chipset related functions
 which will be used by Kernel or Project code. These PPI functions should be used by calling the
 corresponding functions in PeiChipsetSvcLib to have the protocol size checking

***************************************************************************
* Copyright (c) 2012 - 2019, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

//
// Libraries
//
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/ChipsetConfigLib.h>
#include <Library/PcdLib.h>
#include <PiPei.h>

//
// Produced Ppi
//
#include <Ppi/H2OPeiChipsetServices.h>

//
// Structure for H2O Chipsete Services PPI Descriptor
//
typedef struct {
  EFI_PEI_PPI_DESCRIPTOR    PpiDescriptor;
  H2O_CHIPSET_SERVICES_PPI  H2OChipsetSvcPpi;
} H2O_CHIPSET_SERVICES_PPI_DESCRIPTOR;

//
// H2O Chipsete Services function prototypes
//
EFI_STATUS
EFIAPI
EnableFdWrites (
  IN  BOOLEAN                       EnableWrites
  );

EFI_STATUS
EFIAPI
LegacyRegionAccessCtrl (
  IN  UINT32                        Start,
  IN  UINT32                        Length,
  IN  UINT32                        Mode
  );

EFI_STATUS
EFIAPI
GetSleepState (
  IN OUT  UINT8                    *SleepState
  );

EFI_STATUS
EFIAPI
HdaInitHook (
  IN UINT32                         HdaBar
  );

EFI_STATUS
EFIAPI
PlatformStage1Init (
  VOID
  );

EFI_STATUS
EFIAPI
PlatformStage2Init (
  VOID
  );

EFI_STATUS
EFIAPI
Stage1MemoryDiscoverCallback (
  IN  EFI_BOOT_MODE                 *BootMode
  );

EFI_STATUS
EFIAPI
Stage2MemoryDiscoverCallback (
  IN EFI_BOOT_MODE                  *BootMode
  );

EFI_STATUS
EFIAPI
ModifyMemoryRange (
  IN OUT  PEI_MEMORY_RANGE_OPTION_ROM           *OptionRomMask,
  IN OUT  PEI_MEMORY_RANGE_SMRAM                *SmramMask,
  IN OUT  PEI_MEMORY_RANGE_GRAPHICS_MEMORY      *GraphicsMemoryMask,
  IN OUT  PEI_MEMORY_RANGE_PCI_MEMORY           *PciMemoryMask
  );

EFI_STATUS
EFIAPI
SetSetupVariableDefault (
  IN VOID                           *SystemConfiguration
  );

EFI_STATUS
SetPlatformHardwareSwitch (
  OUT PLATFORM_HARDWARE_SWITCH          **PlatformHardwareSwitchDptr
  );

//[-start-191001-IB06070053-modify]//
/**
 This is the declaration of PEI Chipset Services PEIM entry point.

 @param[in]         FileHandle          Handle of the file being invoked.
 @param[in]         PeiServices         Describes the list of possible PEI Services.

 @retval            EFI_SUCCESS         The operation completed successfully.
 @retval            Others              An unexpected error occurred.
*/
EFI_STATUS
EFIAPI
ChipsetSvcPeiEntryPoint (
  IN EFI_PEI_FILE_HANDLE             FileHandle,
  IN CONST EFI_PEI_SERVICES        **PeiServices
  )
{
  EFI_STATUS                            Status;
  EFI_STATUS                            ConfigStatus;
  UINT32                                Size;
  H2O_CHIPSET_SERVICES_PPI_DESCRIPTOR   *ChipsetSvcPpiDescriptor;
  EFI_PEI_PPI_DESCRIPTOR                *PpiDescriptor;
  VOID                                  *Ppi;
  UINT8                                 RecoveryFlag0;
  UINT8                                 RecoveryFlag1;
  CHIPSET_CONFIGURATION                 ChipsetConfiguration;

  PpiDescriptor = NULL;
  Ppi = NULL;

  //
  // Register so it will be automatically shadowed to memory
  //
  Status = PeiServicesRegisterForShadow (FileHandle);


  //
  // Create an instance of the H2O Chipset Services ppi,
  //
  ChipsetSvcPpiDescriptor = AllocateZeroPool (sizeof (H2O_CHIPSET_SERVICES_PPI_DESCRIPTOR));
  if (ChipsetSvcPpiDescriptor == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Size = sizeof (H2O_CHIPSET_SERVICES_PPI);
  if (Size < sizeof (UINT32)) {         // must at least contain Size field.
    return EFI_INVALID_PARAMETER;
  }

  //
  // Build ChipsetSvcPpi Private Data
  //
  ChipsetSvcPpiDescriptor->PpiDescriptor.Flags   = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  ChipsetSvcPpiDescriptor->PpiDescriptor.Guid    = &gH2OChipsetServicesPpiGuid;
  ChipsetSvcPpiDescriptor->PpiDescriptor.Ppi     = &ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi;
  ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.Size = Size;

  //
  // Populate all functions based on Size.
  //
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PPI, EnableFdWrites) + sizeof (VOID*))) {
    ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.EnableFdWrites = EnableFdWrites;
  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PPI, LegacyRegionAccessCtrl) + sizeof (VOID*))) {
    ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.LegacyRegionAccessCtrl = LegacyRegionAccessCtrl;
  }
//  if (Size >= (OFFSET_OF(H2O_CHIPSET_SERVICES_PPI, OnStartOfTcgPei) + sizeof(VOID*))) {
//    ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.OnStartOfTcgPei = OnStartOfTcgPei;
//  }
//  if (Size >= (OFFSET_OF(H2O_CHIPSET_SERVICES_PPI, ResetSystem) + sizeof(VOID*))) {
//    ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.ResetSystem = ResetSystem;
//  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PPI, GetSleepState) + sizeof (VOID*))) {
    ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.GetSleepState = GetSleepState;
  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PPI, HdaInitHook) + sizeof (VOID*))) {
    ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.HdaInitHook = HdaInitHook;
  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PPI, PlatformStage1Init) + sizeof (VOID*))) {
    ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.PlatformStage1Init = PlatformStage1Init;
  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PPI, Stage1MemoryDiscoverCallback) + sizeof (VOID*))) {
    ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.Stage1MemoryDiscoverCallback = Stage1MemoryDiscoverCallback;
  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PPI, PlatformStage2Init) + sizeof (VOID*))) {
    ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.PlatformStage2Init = PlatformStage2Init;
  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PPI, Stage2MemoryDiscoverCallback) + sizeof (VOID*))) {
    ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.Stage2MemoryDiscoverCallback = Stage2MemoryDiscoverCallback;
  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PPI, ModifyMemoryRange) + sizeof (VOID*))) {
    ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.ModifyMemoryRange = ModifyMemoryRange;
  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PPI, SetSetupVariableDefault) + sizeof (VOID*))) {
    ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.SetSetupVariableDefault = SetSetupVariableDefault;
  }
  if (Size >= (OFFSET_OF (H2O_CHIPSET_SERVICES_PPI, SetPlatformHardwareSwitch) + sizeof (VOID*))) {
    ChipsetSvcPpiDescriptor->H2OChipsetSvcPpi.SetPlatformHardwareSwitch = SetPlatformHardwareSwitch;
  }

  if (Status == EFI_ALREADY_STARTED) {
    //
    // Shadow completed and running from memory
    //
    Status = PeiServicesLocatePpi (
               &gH2OChipsetServicesPpiGuid,
               0,
               &PpiDescriptor,
               &Ppi
               );
    if (!EFI_ERROR (Status)) {
      Status = PeiServicesReInstallPpi (
                 PpiDescriptor,
                 &ChipsetSvcPpiDescriptor->PpiDescriptor
                 );
    }
  } else {
    //
    // Install the Ppi.
    //
    Status = PeiServicesInstallPpi (&ChipsetSvcPpiDescriptor->PpiDescriptor);

    ConfigStatus = GetChipsetConfiguration (&ChipsetConfiguration);
    if (EFI_ERROR(ConfigStatus)) {
      return ConfigStatus;
    }
    if (ChipsetConfiguration.PeiGop == 1 && PcdGetBool(PcdUseFastCrisisRecovery) == 0) {
      PcdSetBool (PcdPeiGopEnable, TRUE);

      //
      // Check Recovery Flag in CMOS.
      //
      IoWrite8 (0x72, 0xA4);
      RecoveryFlag0 = IoRead8 (0x73);
      IoWrite8 (0x72, 0xA5);
      RecoveryFlag1 = IoRead8 (0x73);
      if (RecoveryFlag0 == 0x55 && RecoveryFlag1 == 0xAA) {
        PeiServicesSetBootMode(BOOT_IN_RECOVERY_MODE);
        DEBUG ((EFI_D_ERROR, "Early set BOOT_IN_RECOVERY_MODE\n"));
      }
    }
  }

  return Status;
}
//[-end-191001-IB06070053-modify]//

