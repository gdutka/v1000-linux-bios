/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch HwAcpi controller
 *
 * Init HwAcpi Controller features (PEI phase).
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 309083 $   @e \$Date: 2014-12-09 09:28:24 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*/
#include "FchPlatform.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_SANDSTONE_SSHWACPI_SSHWACPIRESET_FILECODE

extern ACPI_REG_WRITE FchInitResetAcpiMmioTable[];
extern VOID  ProgramFchHwAcpiResetP (IN VOID  *FchDataPtr);

/**
 * FchInitResetHwAcpiP - Config HwAcpi controller ( Preliminary
 * ) during Power-On
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitResetHwAcpiP (
  IN  VOID     *FchDataPtr
  )
{
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;
  AMD_CONFIG_PARAMS         *StdHeader;

  AGESA_TESTPOINT (TpFchInitResetHwAcpiP, NULL);

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;

  StdHeader = LocalCfgPtr->StdHeader;
  RwPmio (FCH_PMIOA_REG08 + 2, AccessWidth8, 0xFF, BIT3, StdHeader);
  //
  // Enabled (Mmio_mem_enable)
  //
  RwPmio (FCH_PMIOA_REG04, AccessWidth8, 0xFF, BIT1, StdHeader);

  ACPIMMIO32 (FCH_AOACx94S0I3_CONTROL) &= ~ (FCH_AOACx94S0I3_CONTROL_ARBITER_DIS + FCH_AOACx94S0I3_CONTROL_INTERRUPT_DIS);

  RwPmio (FCH_PMIOA_REG08 + 2, AccessWidth8, 0xFF, BIT4, StdHeader);

  ProgramFchHwAcpiResetP (FchDataPtr);

  //
  // enable CF9
  //
  RwPmio (FCH_PMIOA_REGD2, AccessWidth8, ~(UINT32) BIT6, 0, StdHeader);
  //
  // PLAT-30946 
  // enable LpcClockDriveStrength
  //
  if ( LocalCfgPtr->Emmc.EmmcEnable == 0 ) {
    //RwPmio (FCH_PMIOA_REGD2, AccessWidth8, ~(UINT32) ( BIT4 + BIT5 ), (UINT8) ((LocalCfgPtr->LpcClockDriveStrength & 3) << 4), StdHeader);
    ProgramLpcEmmcPins (FALSE);
    ACPIMMIO8 (FCH_EMMC_CFG_REGBB) = (LocalCfgPtr->LpcClockDriveStrengthRiseTime & 0xf) | ((LocalCfgPtr->LpcClockDriveStrengthFallTime & 0xf) << 4);
  }

  RwPmio (FCH_PMIOA_REGC0, AccessWidth8, 0, BIT1, StdHeader);
}

/**
 * FchInitResetHwAcpi - Config HwAcpi controller during Power-On
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitResetHwAcpi (
  IN  VOID     *FchDataPtr
  )
{
  UINT16       SmbusBase;
  UINT8        Value;
  UINT16       AsfPort;
  UINT32       GeventEnableBits;
  UINT32       GeventValue;
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;
  AMD_CONFIG_PARAMS         *StdHeader;

  AGESA_TESTPOINT (TpFchInitResetHwAcpi, NULL);

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  // Set Build option into SB
  //
  WritePci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG64, AccessWidth16, &(LocalCfgPtr->FchBldCfg.CfgSioPmeBaseAddress), StdHeader);

  //
  // Clear PMx40[1,12] to enable eSPI IRQ1/12 from eSPI.
  //
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG40, AccessWidth32, ~(UINT32) (BIT12 + BIT1), 0);

  //
  // Enabled SMBUS0/SMBUS1 (ASF) Base Address
  //
  SmbusBase = LocalCfgPtr->FchBldCfg.CfgSmbus0BaseAddress;
  SmbusBase &= 0xFF00;
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG00, AccessWidth16, 0x00FF, SmbusBase + BIT4);
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG60, AccessWidth16, 00, (LocalCfgPtr->FchBldCfg.CfgAcpiPm1EvtBlkAddr));
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG62, AccessWidth16, 00, (LocalCfgPtr->FchBldCfg.CfgAcpiPm1CntBlkAddr));
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG64, AccessWidth16, 00, (LocalCfgPtr->FchBldCfg.CfgAcpiPmTmrBlkAddr));
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG66, AccessWidth16, 00, (LocalCfgPtr->FchBldCfg.CfgCpuControlBlkAddr));
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG68, AccessWidth16, 00, (LocalCfgPtr->FchBldCfg.CfgAcpiGpe0BlkAddr));
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG6A, AccessWidth16, 00, (LocalCfgPtr->FchBldCfg.CfgSmiCmdPortAddr));
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG6E, AccessWidth16, 00, (LocalCfgPtr->FchBldCfg.CfgSmiCmdPortAddr) + 8);
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG6C, AccessWidth16, 00, 0xFFFF);

  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG00, AccessWidth32, ~(UINT32) (BIT19 + BIT20), 0);
  Value = 0x00;
  LibAmdIoWrite (AccessWidth8, SmbusBase + 0x14, &Value, StdHeader);

  ProgramFchAcpiMmioTbl ((ACPI_REG_WRITE*) (&FchInitResetAcpiMmioTable[0]), StdHeader);

  FchInitEnableWdt (LocalCfgPtr);

  ProgramResetRtcExt (LocalCfgPtr);

  if (LocalCfgPtr->FchBldCfg.CfgFchSciMapControl != NULL) {
    ProgramFchSciMapTbl ((LocalCfgPtr->FchBldCfg.CfgFchSciMapControl), LocalCfgPtr);
  }

  if (LocalCfgPtr->FchBldCfg.CfgFchGpioControl != NULL) {
    ProgramFchGpioTbl ((LocalCfgPtr->FchBldCfg.CfgFchGpioControl), LocalCfgPtr);
  }

  if (LocalCfgPtr->FchBldCfg.CfgFchSataPhyControl != NULL) {
    ProgramFchSataPhyTbl ((LocalCfgPtr->FchBldCfg.CfgFchSataPhyControl), LocalCfgPtr);
  }
  //
  // RTC Workaround for Daylight saving time enable bit
  //
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG5E, AccessWidth8, 0, 0);
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG5F, AccessWidth8, 0xFE, BIT0 );   // Enable DltSavEnable
  Value = 0x0B;
  LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REG70, &Value, StdHeader);
  LibAmdIoRead (AccessWidth8, FCH_IOMAP_REG71, &Value, StdHeader);
  Value &= 0xFE;
  LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REG71, &Value, StdHeader);
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG5E, AccessWidth8, 0, 0);
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG5F, AccessWidth8, 0xFE, 0 );      // Disable DltSavEnable
  //
  // Prevent RTC error
  //
  Value = 0x0A;
  LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REG70, &Value, StdHeader);
  LibAmdIoRead (AccessWidth8, FCH_IOMAP_REG71, &Value, StdHeader);
  Value &= 0xEF;
  LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REG71, &Value, StdHeader);

  if ( LocalCfgPtr->FchBldCfg.CfgFchRtcWorkAround ) {
    Value = RTC_WORKAROUND_SECOND;
    LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REG70, &Value, StdHeader);
    LibAmdIoRead (AccessWidth8, FCH_IOMAP_REG71, &Value, StdHeader);
    if ( Value > RTC_VALID_SECOND_VALUE ) {
      Value = RTC_SECOND_RESET_VALUE;
      LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REG71, &Value, StdHeader);
    }
    LibAmdIoRead (AccessWidth8, FCH_IOMAP_REG71, &Value, StdHeader);
    Value &= RTC_SECOND_LOWER_NIBBLE;
    if ( Value > RTC_VALID_SECOND_VALUE_LN ) {
      LibAmdIoRead (AccessWidth8, FCH_IOMAP_REG71, &Value, StdHeader);
      Value = RTC_SECOND_RESET_VALUE;
      LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REG71, &Value, StdHeader);
    }
  }

  Value = 0x08;
  LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGC00, &Value, StdHeader);
  LibAmdIoRead (AccessWidth8, FCH_IOMAP_REGC01, &Value, StdHeader);

  if ( !LocalCfgPtr->EcKbd ) {
    //
    // Route SIO IRQ1/IRQ12 to USB IRQ1/IRQ12 input
    //
    Value = Value | 0x0A;
  }
  Value = Value | 0xF0;  // IRQ14&IRQ15 should be released from SATA controller by default
  LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGC01, &Value, StdHeader);

  Value = 0x09;
  LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGC00, &Value, StdHeader);
  LibAmdIoRead (AccessWidth8, FCH_IOMAP_REGC01, &Value, StdHeader);
  if ( !LocalCfgPtr->EcKbd ) {
    //
    // Route SIO IRQ1/IRQ12 to USB IRQ1/IRQ12 input
    //
    Value = Value & 0xF9;
  }

//PLAT-12168  if ( LocalCfgPtr->LegacyFree ) {
    //
    // Disable IRQ1/IRQ12 filter enable for Legacy free with USB KBC emulation.
    //
    Value = Value & 0x9F;
//PLAT-12168  }
  //
  // Enabled IRQ input
  //
  Value = Value | BIT4;
  LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGC01, &Value, StdHeader);

  AsfPort = SmbusBase + 0x20;
  if ( AsfPort != 0 ) {
    UINT8  dbValue;
    dbValue = 0x2F;
    LibAmdIoWrite (AccessWidth8, AsfPort + 0x0A, &dbValue, StdHeader);
  }
  //
  // PciExpWakeStatus workaround
  //
  ReadMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG60, AccessWidth16, &AsfPort);
  AsfPort++;
  ReadMem (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG04, AccessWidth32, &GeventEnableBits);
  ReadMem (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG00, AccessWidth32, &GeventValue);
  if ( (GeventValue & GeventEnableBits) != 0 ) {
    Value = 0x40;
    LibAmdIoWrite (AccessWidth8, AsfPort, &Value, StdHeader);
  }
  LibAmdIoRead (AccessWidth8, AsfPort, &Value, StdHeader);
  if ((Value & (BIT2 + BIT0)) != 0) {
    Value = 0x40;
    LibAmdIoWrite (AccessWidth8, AsfPort, &Value, StdHeader);
  }

  if ( LocalCfgPtr->FchOscout1ClkContinous ) {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG54, AccessWidth8, 0xBF, 0);
  }
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG54, AccessWidth8, 0x7F, BIT7);

  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGED, AccessWidth8, ~(UINT32) (BIT1), BIT1);  //PLAT-15526
  ACPIMMIO8 (FCH_MISCx4C_MiscClkCntl4) |= FCH_MISCx4C_MiscClkCntl4_TMDP_100M_Refclk_Driver_PWDN_BIT6; //PLAT-23962

  if ( LocalCfgPtr->ToggleAllPwrGoodOnCf9 ) { //PLAT-27965
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG10, AccessWidth8, 0xFD, 2);
  } else {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG10, AccessWidth8, 0xFD, 0);
  }

}



