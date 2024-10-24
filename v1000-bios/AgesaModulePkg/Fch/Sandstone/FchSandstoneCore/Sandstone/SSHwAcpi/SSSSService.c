/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch HwAcpi controller
 *
 * Init Spread Spectrum features.
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
#define FILECODE PROC_FCH_SANDSTONE_SSHWACPI_SSSSSERVICE_FILECODE

/**
 * FchInitResetAcpiMmioTable - Fch ACPI MMIO initial
 * during the power on stage.
 *
 *
 *
 *
 */
ACPI_REG_WRITE FchInitResetAcpiMmioTable[] =
{
  {00, 00, 0xB0, 0xAC},                                         /// Signature

  {PMIO_BASE >> 8,  FCH_PMIOA_REG5D, 0xFC, BIT1},
  {PMIO_BASE >> 8,  FCH_PMIOA_REGD2, 0xCF, 0x00},
  {SMBUS_BASE >> 8, FCH_SMBUS_REG12, 0x00, BIT0},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG44 + 3, 0x67, 0x88},            /// Stop Boot timer
  {PMIO_BASE >> 8,  FCH_PMIOA_REG00, 0xF7, 0x77},
  {PMIO_BASE >> 8,  FCH_PMIOA_REGEC, 0xFB, BIT2},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG08, 0xFE, BIT2 + BIT4},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG08 + 1, 0xFF, BIT0},
  //{PMIO_BASE >> 8,  FCH_PMIOA_REG10, 0xFC, 0x02},             //PLAT-27490  //PLAT-27965
  {PMIO_BASE >> 8,  FCH_PMIOA_REG54, 0x00, BIT4 + BIT6},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG74, 0xF6, BIT0},
  {PMIO_BASE >> 8,  FCH_PMIOA_REGC4, 0xEE, 0x04},                 /// Release NB_PCIE_RST
  {PMIO_BASE >> 8,  FCH_PMIOA_REGC0 + 2, 0xBF, 0x40},
  {PMIO_BASE >> 8,  FCH_PMIOA_REGBE, 0xDF, BIT5},
  {PMIO_BASE >> 8,  FCH_PMIOA_REGC4, 0xFB, 0},

  {MISC_BASE >> 8,  FCH_MISC_REG6C + 2, 0x7F, BIT7},             // MISC 0x6C BIT23
  {MISC_BASE >> 8,  FCH_MISC_REG6C + 3, 0xF7, BIT3},             // MISC 0x6C BIT27

  {0xFF, 0xFF, 0xFF, 0xFF},
};

ACPI_REG_WRITE FchSandStoneInitResetRtcextTable[] =
{
  {00, 00, 0xB0, 0xAC},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5E, 0x00, 0x00},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5F, 0x00, 0x00},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5E, 0x00, 0x01},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5F, 0x00, 0x00},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5E, 0x00, 0x02},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5F, 0x00, 0x00},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5E, 0x00, 0x03},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5F, 0x00, 0x00},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5E, 0x00, 0x04},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5F, 0x00, 0x00},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5E, 0x00, 0x10},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5F, 0x00, 0x00},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5E, 0x00, 0x11},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5F, 0x00, 0x00},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5E, 0x00, 0x12},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5F, 0x00, 0x00},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5E, 0x00, 0x13},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5F, 0x00, 0x00},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5E, 0x00, 0x14},
  {PMIO_BASE >> 8,  FCH_PMIOA_REG5F, 0x00, 0x00},
  {0xFF, 0xFF, 0xFF, 0xFF},
};

/**
 * ProgramFchHwAcpiResetP  - Config SpreadSpectrum before PCI
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
ProgramFchHwAcpiResetP (
  IN VOID  *FchDataPtr
  )
{
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;
  AMD_CONFIG_PARAMS         *StdHeader;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  // Clear UseAcpiStraps, PMIO_C8[4]
  //
  RwPmio (FCH_PMIOA_REGC8, AccessWidth8, 0xEF, 0x0, StdHeader);
}

VOID
FchInitEnableWdt (
  IN  VOID     *FchDataPtr
  )
{
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;

  if (LocalCfgPtr->WdtEnable) {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG00, AccessWidth8, 0x7F, BIT7);
  } else {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG00, AccessWidth8, 0x7F, 0);
  }
}

/**
 * ProgramResetRtcExt - Config RTC External registers
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
ProgramResetRtcExt (
  IN  VOID     *FchDataPtr
  )
{
  UINT8                  RtcExtData;
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;
  AMD_CONFIG_PARAMS         *StdHeader;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //check if RTCext data lost
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG5E, AccessWidth8, 0, 0x01);
  ReadMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG5F, AccessWidth8, &RtcExtData);
  if ( RtcExtData == 0xFF ) {
    ProgramFchAcpiMmioTbl ((ACPI_REG_WRITE*) (&FchSandStoneInitResetRtcextTable[0]), StdHeader);
  }
}


