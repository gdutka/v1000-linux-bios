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
#define FILECODE PROC_FCH_SANDSTONE_SSSPI_SSLPCENV_FILECODE

extern VOID  FchInitEnvLpcProgram            (IN VOID  *FchDataPtr);

/**
 * FchInitEnvLpc - Config LPC controller before PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvLpc (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  AGESA_TESTPOINT (TpFchInitEnvLpc, NULL);
  //
  // LPC CFG programming
  //
  //
  // Turn on and configure LPC clock (48MHz)
  //
  //RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG28, AccessWidth32, ~(UINT32) (BIT21 + BIT20 + BIT19), 2 << 19);
//  RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG40, AccessWidth8, ~(UINT32) BIT7, 0);

  //
  // Initialization of pci config space
  //
  FchInitEnvLpcProgram (FchDataPtr);

  //
  // SSID for LPC Controller
  //
  if (LocalCfgPtr->Spi.LpcSsid != NULL ) {
    RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG2C, AccessWidth32, 0x00, LocalCfgPtr->Spi.LpcSsid, StdHeader);
  }
  //
  // LPC MSI
  //
  if ( LocalCfgPtr->Spi.LpcMsiEnable ) {
    RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG78, AccessWidth32, ~(UINT32) BIT1, BIT1, StdHeader);
  }
}


