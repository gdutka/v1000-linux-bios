/** @file
 PEI Chipset Services Library.

 This file contains only one function that is PeiCsSvcPlatformStage1Init().
 The function PeiCsSvcPlatformStage1Init() use chipset services to be
 Platform initialization in PEI phase stage 1.

***************************************************************************
* Copyright (c) 2012 - 2017, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PciExpressLib.h>
#include <Library/ChipsetConfigLib.h>
#include <Library/VariableLib.h>
#include <ChipsetSetupConfig.h>
#include <Hudson-2Reg.h>
#include <TpmPolicy.h>
//[-start-210505-IB06070106-add]//
#include <Library/AmdPspFtpmLib.h>
//[-end-210505-IB06070106-add]//

#define TPM_ALL_DISABLE              0x0
#define TPM_CHIP_DEVICE_ENABLE       0x1
#define TPM_FIRMWARE_DEVICE_ENABLE   0x2

/**
  Check if DTPM establishment is required.

  @retval TRUE        Required
  @retval FALSE       Not required
**/
STATIC
BOOLEAN
IsEstablishDtpmRequired (
  VOID
  )
{
  if (!FeaturePcdGet (PcdH2OPreferDtpmBootSupported)) {
    return FALSE;
  }

  //
  // Check if system is on the first boot.
  //
  return !IsVariableInVariableStoreRegion (SETUP_VARIABLE_NAME, &gSystemConfigurationGuid);
}

/**
  Manage decoder in order to detect DTPM.

  @param[in,out] SystemConfiguration       Pointer to Setup variable.

  @retval EFI_SUCCESS          Operation completed successfully
  @retval Others               Operation failed
**/
STATIC
VOID
EstablishDtpm (
  IN OUT CHIPSET_CONFIGURATION    *SystemConfiguration
  )
{
  //
  // Detect LPC TPM
  // Enable DTPM decoder and check if TPM presented
  //
  DEBUG ((EFI_D_INFO, "Enable DTPM decorder and check if TPM presented\n"));
  //Stall time depends on project request.
  //Stall->Stall (PeiServices, NULL, 100000);
  PciExpressOr8 (
    PCI_EXPRESS_LIB_ADDRESS (
      FCH_LPC_BUS,
      FCH_LPC_DEV,
      FCH_LPC_FUNC,
      R_FCH_LPC_TPM_REGISTER
    ),
    (V_TPM12_ENABLE + V_TPM_LEGACY_ENABLE + V_WIDER_TPM_ENABLE)
    );
//Arista: Always force TPM to SPI TPM
    SystemConfiguration->SpiOrLpcTPM = 1; // 0: LPC TPM  1: SPI TPM

  if (MmioRead8 ((UINTN) PcdGet64 (PcdTpmBaseAddress)) != 0xFF) {
    DEBUG ((EFI_D_INFO, "LPC DTPM is detected\n"));
    SystemConfiguration->EnableTPM = TPM_CHIP_DEVICE_ENABLE;
//Arista    SystemConfiguration->SpiOrLpcTPM = 0; // 0: LPC TPM  1: SPI TPM
  } else {
    //
    // Detect SPI TPM.
    // Enable RouteTpm2Spi
    //
    PciExpressOr8 (
      PCI_EXPRESS_LIB_ADDRESS (
        FCH_LPC_BUS,
        FCH_LPC_DEV,
        FCH_LPC_FUNC,
        R_FCH_LPC_SPI_BASE_ADDRESS
      ),
      V_ROUTE_TPM_TO_SPI
    );

    if (MmioRead8 ((UINTN) PcdGet64 (PcdTpmBaseAddress)) != 0xFF) {
      DEBUG ((EFI_D_INFO, "SPI DTPM is detected\n"));
      SystemConfiguration->EnableTPM = TPM_CHIP_DEVICE_ENABLE;
      SystemConfiguration->SpiOrLpcTPM = 1; // 0: LPC TPM  1: SPI TPM
    }
    //
    // Disabling RouteTpm2Spi
    //
    PciExpressAnd8 (
      PCI_EXPRESS_LIB_ADDRESS (
        FCH_LPC_BUS,
        FCH_LPC_DEV,
        FCH_LPC_FUNC,
        R_FCH_LPC_SPI_BASE_ADDRESS
      ),
      (UINT8)(~V_ROUTE_TPM_TO_SPI)
    );
  }
  //
  // Detection done
  // Disabling TPM decoder
  //
  PciExpressAnd8 (
    PCI_EXPRESS_LIB_ADDRESS (
      FCH_LPC_BUS,
      FCH_LPC_DEV,
      FCH_LPC_FUNC,
      R_FCH_LPC_TPM_REGISTER
    ),
    (UINT8)(~(V_TPM12_ENABLE + V_TPM_LEGACY_ENABLE + V_WIDER_TPM_ENABLE))
    );
}

//[-start-210505-IB06070106-modify]//
EFI_STATUS
EnableTpm (
  IN CHIPSET_CONFIGURATION*  SystemConfiguration
  )
/*++
Routine Description:
  Enable TPM Decode
Arguments:
  PeiServices - PEI Service pointer.
Returns:
  None
--*/
{
  UINT32    SpiBaseAddress;
  UINT16    *SpiTpmRegister;
  UINT16    SpiTpmRegistrVaule;
  UINT32    SetTpmPolicy;

  DEBUG ((EFI_D_ERROR, "Configure TPM decoder... %x\n", SystemConfiguration->EnableTPM));

  if (SystemConfiguration->SetupVariableInvalid) {
    DEBUG ((EFI_D_ERROR, "SetupVariableInvalid deteted \n"));
    SystemConfiguration->EnableTPM = TPM_ALL_DISABLE;
  }

  //
  // Perfer using DTPM on the first boot
  // Process if required
  //
  if (IsEstablishDtpmRequired ()) {
    EstablishDtpm (SystemConfiguration);
  }

  PcdSet8 (PcdTpmType, SystemConfiguration->EnableTPM);
  DEBUG ((EFI_D_ERROR, " TPM setting = %x\n", PcdGet8 (PcdTpmType)));
  PcdSet8 (SpiOrLpcTPM, SystemConfiguration->SpiOrLpcTPM);
  DEBUG ((EFI_D_ERROR, " SpiOrLpc TPM = %x\n", PcdGet8 (SpiOrLpcTPM)));

  PcdSet8 (PcdAmdPspSystemTpmConfig, SYSTEM_TPM_CONFIG_NONE);

  if (SystemConfiguration->EnableTPM == TPM_CHIP_DEVICE_ENABLE) {
    DEBUG ((EFI_D_ERROR, "Enabling TPM decoder...\n"));
    //Stall time depends on project request.
    //Stall->Stall (PeiServices, NULL, 100000);
    PciExpressOr8 (
      PCI_EXPRESS_LIB_ADDRESS (
        FCH_LPC_BUS,
        FCH_LPC_DEV,
        FCH_LPC_FUNC,
        R_FCH_LPC_TPM_REGISTER
      ),
      (V_TPM12_ENABLE + V_TPM_LEGACY_ENABLE + V_WIDER_TPM_ENABLE)
    );

    if(PcdGet8(SpiOrLpcTPM)) {          // PcdsDynamicHii type 1: SPI TPM (default)  0: LPC TPM      Defined in AdvanceVfr.vfr
      DEBUG ((EFI_D_ERROR, "SpiOrLpcTPM:1 - SPI TPM\n"));
      // Enable RouteTpm2Spi
      PciExpressOr8 (
        PCI_EXPRESS_LIB_ADDRESS (
          FCH_LPC_BUS,
          FCH_LPC_DEV,
          FCH_LPC_FUNC,
          R_FCH_LPC_SPI_BASE_ADDRESS
        ),
        V_ROUTE_TPM_TO_SPI
      );

      // Config SPI_TPM_CS_L
//[-start-230516-IB14860916-modify]//
//    MmioWrite8 (ACPI_MMIO_BASE + IOMUX_BASE + 0x1C, 0);
      MmioWrite8 (ACPI_MMIO_BASE + IOMUX_BASE + 0x1D, 0);	 //Arista: was 0x1C, changed it to 0x1D	for SPI_TPM_CS_L
//[-end-230516-IB14860916-modify]//

      SpiBaseAddress = PciExpressRead32 (
                         PCI_EXPRESS_LIB_ADDRESS (
                           FCH_LPC_BUS,
                           FCH_LPC_DEV,
                           FCH_LPC_FUNC,
                           R_FCH_LPC_SPI_BASE_ADDRESS
                         )
                       );
      SpiBaseAddress = SpiBaseAddress & 0xFFFFFF00; //bit 0~7 disable
      SpiTpmRegister      = (UINT16*) (SpiBaseAddress + FCH_SPI_MMIO_REG22);
//      SpiTpmRegistrVaule  = *SpiTpmRegister & 0xFFF0; //Bit 0~3 diable
      SpiTpmRegistrVaule  = *SpiTpmRegister & 0xF000; //Bit 0~11 diable
//      SpiTpmRegistrVaule  = SpiTpmRegistrVaule | 0x01;      // 33.33MHz
      SpiTpmRegistrVaule  = SpiTpmRegistrVaule | 0x0131;      // Force SPI and TPM access to 33.33MHz
      *SpiTpmRegister     = SpiTpmRegistrVaule;

      // Disable SPI Pre-Fetch
      PciExpressAnd32 (
        PCI_EXPRESS_LIB_ADDRESS (
          FCH_LPC_BUS,
          FCH_LPC_DEV,
          FCH_LPC_FUNC,
          R_FCH_LPC_ROM_DMA_CONTROL_REGISTER
        ),
        ~(UINT32)B_PREFETCH_EN_SPI_FROM_HOST
      );
    } else {
      DEBUG ((EFI_D_ERROR, "SpiOrLpcTPM:0 - LPC TPM\n"));
    }
    PcdSet8 (PcdAmdPspSystemTpmConfig, SYSTEM_TPM_CONFIG_DTPM);
  } else {
    DEBUG ((EFI_D_ERROR, "Disabling TPM decoder...\n"));
    PciExpressAnd8 (
      PCI_EXPRESS_LIB_ADDRESS (
        FCH_LPC_BUS,
        FCH_LPC_DEV,
        FCH_LPC_FUNC,
        R_FCH_LPC_TPM_REGISTER
      ),
      (UINT8) (~(V_TPM12_ENABLE | V_TPM_LEGACY_ENABLE | V_WIDER_TPM_ENABLE))
    );
  }

  if (SystemConfiguration->EnableTPM == TPM_FIRMWARE_DEVICE_ENABLE) {
    DEBUG ((EFI_D_ERROR, "Set fTPM Pcds...\n"));
    SetTpmPolicy = PcdGet32(PcdPeiTpmPolicy) | TPM2_STARTUP_IN_MP;
    PcdSet32(PcdPeiTpmPolicy, SetTpmPolicy);
    PcdSet8 (PcdAmdPspSystemTpmConfig, SYSTEM_TPM_CONFIG_PSP_FTPM);
  }

  DEBUG ((EFI_D_ERROR, " System TPM Config = %x\n", PcdGet8 (PcdAmdPspSystemTpmConfig)));
  return EFI_SUCCESS;
}
//[-end-210505-IB06070106-modify]//

//#ifndef MDEPKG_NDEBUG
EFI_STATUS
EnableSerialPort (
  )
{
  PciExpressOr8 (
    PCI_EXPRESS_LIB_ADDRESS (FCH_LPC_BUS,
    FCH_LPC_DEV,
    FCH_LPC_FUNC,
    R_FCH_LPC_IO_PORT_DECODE_ENABLE_REGISTER1),
    V_SERIAL_PORT_ENABLE_0_ENABLE
  );
  return EFI_SUCCESS;
}
//#endif
/**
 Platform initialization in PEI phase stage 1.

 @param[in]         None

 @retval            EFI_SUCCESS         This function alway return successfully
*/
EFI_STATUS
PlatformStage1Init (
  VOID
  )
{
  EFI_STATUS                       Status;
  CHIPSET_CONFIGURATION            ChipsetConfiguration;
//[-start-191121-IB06070064-remove]//
//  UINT32  Value32;
//
//  //Blink function
//  Value32 = MmioRead32 (ACPI_MMIO_BASE + 0x3E4);
//  Value32 &= ~(BIT0 + BIT1);
//  MmioWrite32 (ACPI_MMIO_BASE + 0x3E4, Value32);
//[-end-191121-IB06070064-remove]//

//#ifndef MDEPKG_NDEBUG
  EnableSerialPort ();
//#endif

  DEBUG ((EFI_D_ERROR, "\nPlatformStage1Init Start\n"));

  Status = GetChipsetConfiguration(&ChipsetConfiguration);

  PcdSet8(PcdSVMEnable, ChipsetConfiguration.EnableSVM);
  PcdSet8(PcdSVMLock, ChipsetConfiguration.SVMLock);
  if (FeaturePcdGet(PcdH2OTpmSupported) || FeaturePcdGet(PcdH2OTpm2Supported)) {
    EnableTpm (&ChipsetConfiguration);
  }

  DEBUG ((EFI_D_ERROR, "PlatformStage1Init End\n"));

  return EFI_SUCCESS;
}
