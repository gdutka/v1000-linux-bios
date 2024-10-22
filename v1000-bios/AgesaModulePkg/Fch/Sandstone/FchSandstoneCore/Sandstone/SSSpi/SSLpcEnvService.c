/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch LPC controller
 *
 * Init LPC Controller features.
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
#define FILECODE PROC_FCH_SANDSTONE_SSSPI_SSLPCENVSERVICE_FILECODE

/**
 * FchInitTaishanEnvLpcPciTable - PCI device registers initial
 * during early POST.
 *
 */
REG8_MASK FchInitSandstoneEnvLpcPciTable[] =
{
  //
  // LPC Device (Bus 0, Dev 20, Func 3)
  //
  {0x00, LPC_BUS_DEV_FUN, 0},
  {FCH_LPC_REG40, 0xFB, BIT2},                              /// Enabling LPC DMA Function  0x40[2]=1b 0x78[0]=0b
  {FCH_LPC_REG48, 0x00, BIT0 + BIT1 + BIT2},
  {FCH_LPC_REG78, 0xFC, 00},                                /// Enabling LPC DMA Function  0x40[2]=1b 0x78[0]=0b / Disables MSI capability
  {FCH_LPC_REGBB, 0xF3, BIT2 + BIT3},                       /// Enabled SPI Prefetch from HOST.
  {0xFF, 0xFF, 0xFF},
};

/**
 * FchInitEnvLpcProgram - Config LPC controller before PCI
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvLpcProgram (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;
  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  ProgramPciByteTable ((REG8_MASK*) (&FchInitSandstoneEnvLpcPciTable[0]), sizeof (FchInitSandstoneEnvLpcPciTable) / sizeof (REG8_MASK), StdHeader);

  //RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG28, AccessWidth32, ~(UINT32) (BIT21 + BIT20 + BIT19), 0);
}


