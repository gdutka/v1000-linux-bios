/** @file
 PEI Chipset Services Library.

 This file contains only one function that is PeiCsSvcPlatformStage2Init().
 The function PeiCsSvcPlatformStage2Init() use chipset services to initialization 
 chipset in stage 2 of PEi phase.

***************************************************************************
* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <PiPei.h>
//
// Libraries
//
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/KernelConfigLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PciExpressLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/VerbTable.h>

#include <YangtzeReg.h>
#include <BiosRamUsage.h>
#include <Fch.h>
#include <MmioAccess.h>
#include <Ppi/AzaliaVerbTable.h>

#include <Ppi/AmdFchInitPpi.h>
#include <Guid/PlatformHardwareSwitch.h>
//
// For Fast recovery.
//
//#define EHCI_RECOVERY_TEMP_MEM_BASE_ADDRESS     0xFC100000
#define SATA_RECOVERY_TEMP_BAR5_ADDRESS         0xFEC01000
#define SATA_RECOVERY_TEMP_BAR0_IOBASE          0x9000

EFI_STATUS
InstallAzaliaVerbTable (
  IN CONST EFI_PEI_SERVICES                      **PeiServices,
  IN UINT32                                      HdaBar
  );
//
// Ppi Definition
//
//[-start-240426-IB18160212-remove-]//
//static
//PEI_Install_AZALIA_VERB_TALBE_PPI mAzaliaVerbTablePpi = {
//  InstallAzaliaVerbTable
//  };

//static
//EFI_PEI_PPI_DESCRIPTOR mInstallAzaliaVerbTablePpi = {
//  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
//  &gPeiInstallAzaliaVerbTablePpiGuid,
//  &mAzaliaVerbTablePpi
//  };
//[-end-240426-IB18160212-remove-]//

//
// Fch Recovery Init
//
EFI_STATUS
EFIAPI
AmdFchRecoveryInit (
  IN EFI_PEI_SERVICES                 **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR        *NotifyDescriptor,
  IN VOID                             *Ppi
  );

STATIC EFI_PEI_NOTIFY_DESCRIPTOR mNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdFchInitPpiGuid,
  AmdFchRecoveryInit
};


/**
 Clear all SMI enable bit in PMIO register

 @param [in]        PeiServices         Describes the list of possible PEI Services
 @param [in]        CpuIo               A pointer to CPU IO Services PPI

 @retval            None
*/
VOID
ClearAllSmiEnableInPmio (
  IN CONST EFI_PEI_SERVICES         **PeiServices,
  IN       EFI_PEI_CPU_IO_PPI       *CpuIo
  )
{
  MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGA0, 0x00);
  MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGA4, 0x00);
  MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGA8, 0x00);
  MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGA4, 0x00);
  MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB0, 0x00);
  MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB4, 0x00);
  MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB8, 0x00);
  MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGBC, 0x00);
  MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGC0, 0x00);
  MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGC4, 0x00);
}

/**
 Clear SB Smi and Wake state

 @param [in]        PeiServices         Describes the list of possible PEI Services
 @param [in]        CpuIo               A pointer to CPU IO Services PPI
 @param [in]        PmBase              Acpi Pm1 baseaddress

 @retval            None
*/
VOID
ClearSBSmiAndWake (
  IN CONST EFI_PEI_SERVICES         **PeiServices,
  IN       EFI_PEI_CPU_IO_PPI       *CpuIo,
  IN       UINT16                   PmBase
  )
{
  UINT16  Pm1Sts;
  UINT32  Pm1Cnt;
  UINT32  Gpe0Sts;

  //
  // Read the ACPI registers
  //
  Pm1Sts  = IoRead16 (PmBase + R_FCH_ACPI_PM1_STATUS);
  Pm1Cnt  = IoRead32 (PmBase + R_FCH_ACPI_PM_CONTROL);
  Gpe0Sts = IoRead32 (PmBase + R_FCH_ACPI_EVENT_STATUS);

  //
  // Clear any SMI or wake state from the boot
  //
  Pm1Sts &= B_PWR_BTN_STATUS + B_WAKEUP_STATUS;
  Pm1Cnt &= ~(B_SCI_EN);

  //
  // Write them back
  //
  IoWrite16 (PmBase + R_FCH_ACPI_PM1_STATUS, Pm1Sts);
  IoWrite32 (PmBase + R_FCH_ACPI_PM_CONTROL, Pm1Cnt);
  IoWrite32 (PmBase + R_FCH_ACPI_EVENT_STATUS, Gpe0Sts);
}
  
VOID
ChipsetPlatformHardwareSwitchHookAgesa (
  VOID
  )
{
  VOID                      *PlatformHardwareSwitchBuffer;
  PLATFORM_HARDWARE_SWITCH  *PlatformHardwareSwitch;

  PlatformHardwareSwitchBuffer = GetFirstGuidHob (&gPlatformHardwareSwitchGuid);

  PlatformHardwareSwitch = NULL;

  if (PlatformHardwareSwitchBuffer == NULL) {
    return;
  }

  //
  // Enable/Disable SATA ports by PlatformHardwareSwitch setting
  //
  PlatformHardwareSwitch = (PLATFORM_HARDWARE_SWITCH *) GET_GUID_HOB_DATA (PlatformHardwareSwitchBuffer);

  PcdSet8 (PcdSataPortPowerPort0, (UINT8) PlatformHardwareSwitch->SataPortSwitch.Config.Port00En);
  PcdSet8 (PcdSataPortPowerPort1, (UINT8) PlatformHardwareSwitch->SataPortSwitch.Config.Port01En);

  return;
}

/**
 Chipset initialization code in stage2 of PEI phase.

 @param[in]         None

 @retval            EFI_SUCCESS         Procedure complete.
*/
EFI_STATUS
PlatformStage2Init (
  VOID
  )
{
  EFI_BOOT_MODE                        BootMode;
  CONST EFI_PEI_SERVICES               **PeiServices;

  PeiServices = GetPeiServicesTablePointer ();

  (**PeiServices).GetBootMode (PeiServices, &BootMode);
  
  if ((BootMode == BOOT_IN_RECOVERY_MODE)) {
    (**PeiServices).NotifyPpi (PeiServices, &mNotifyList);
  }

  //
  //Disable PM1 and GPE0
  //
  if ((BootMode != BOOT_ON_S3_RESUME)) {
    //
    // Clear all pending SMI. On S3 clear power button enable so it wll not generate an SMI
    //
    ClearSBSmiAndWake (PeiServices, (**PeiServices).CpuIo, MmioRead16 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG60));
    ClearAllSmiEnableInPmio (PeiServices, (**PeiServices).CpuIo);

    //
    // Clear the SPI protect flag
    //
    (**PeiServices).CpuIo->IoWrite8 (PeiServices, ((**PeiServices).CpuIo), FCH_IOMAP_REGCD4, SPI_LOCK_PROTECT_STATUS);
    (**PeiServices).CpuIo->IoWrite8 (PeiServices, ((**PeiServices).CpuIo), FCH_IOMAP_REGCD5, 0);

    //
    // Set SATA port enable/disable to Agesa PCD
    //
    ChipsetPlatformHardwareSwitchHookAgesa ();
  }

//  (**PeiServices).InstallPpi (PeiServices, &mInstallAzaliaVerbTablePpi);
  return EFI_SUCCESS;
}


EFI_STATUS
InstallAzaliaVerbTable (
  IN CONST EFI_PEI_SERVICES                      **PeiServices,
  IN UINT32                                      HdaBar
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
AmdFchRecoveryInit(
  IN EFI_PEI_SERVICES                 **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR        *NotifyDescriptor,
  IN VOID                             *Ppi
  ) 
{
  EFI_STATUS            Status;
  AMD_FCH_INIT_PPI      *AmdFchInitPpi;

  Status = (*PeiServices)->LocatePpi  (
                             (CONST EFI_PEI_SERVICES**)PeiServices,
                             &gAmdFchInitPpiGuid,
                             0,
                             NULL,
                             (VOID**)&AmdFchInitPpi
                             );

  if (!EFI_ERROR (Status)) {
//    AmdFchInitPpi->FpFchEhciRecovery (AmdFchInitPpi, EHCI_RECOVERY_TEMP_MEM_BASE_ADDRESS);
    AmdFchInitPpi->FpFchSataRecovery (AmdFchInitPpi, SATA_RECOVERY_TEMP_BAR0_IOBASE, SATA_RECOVERY_TEMP_BAR5_ADDRESS);
    // Clear AHCI Enable
    MmioAndThenOr32 (SATA_RECOVERY_TEMP_BAR5_ADDRESS + FCH_SATA_BAR5_REG04, ~(UINT32) (BIT31), 0);
  }

  return EFI_SUCCESS;  
}

