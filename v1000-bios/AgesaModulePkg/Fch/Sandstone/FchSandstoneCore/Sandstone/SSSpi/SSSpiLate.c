/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch Spi (Lpc) controller
 *
 * Init Spi (Lpc) Controller features.
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
#define FILECODE PROC_FCH_SANDSTONE_SSSPI_SSSPILATE_FILECODE

/**
 * FchInitLateSpi - Prepare Spi controller to boot to OS.
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitLateSpi (
  IN  VOID     *FchDataPtr
  )
{
  AGESA_TESTPOINT (TpFchInitLateSpi, NULL);
  FchInitLateLpc (FchDataPtr);
  AmdSpiSemaphoreControl (clear_all_ownerships, 0);
}

/**
 * FchSpiUnlock - Fch SPI Unlock
 *
 *
 * @param[in] FchDataPtr
 *
 */
VOID
FchSpiUnlock (
  IN  VOID     *FchDataPtr
  )
{
  UINT32                  SpiRomBase;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;
  SpiRomBase = LocalCfgPtr->Spi.RomBaseAddress;

  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG50, AccessWidth32, ~(UINT32) (BIT0 + BIT1), 0, StdHeader);
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG54, AccessWidth32, ~(UINT32) (BIT0 + BIT1), 0, StdHeader);
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG58, AccessWidth32, ~(UINT32) (BIT0 + BIT1), 0, StdHeader);
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG5C, AccessWidth32, ~(UINT32) (BIT0 + BIT1), 0, StdHeader);
  RwMem (SpiRomBase + FCH_SPI_MMIO_REG00, AccessWidth32, ~(UINT32) (BIT22 + BIT23), (BIT22 + BIT23));
}

/**
 * FchSpiLock - Fch SPI lock
 *
 *
 * @param[in] FchDataPtr
 *
 */
VOID
FchSpiLock (
  IN  VOID     *FchDataPtr
  )
{
  UINT32                  SpiRomBase;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;
  SpiRomBase = LocalCfgPtr->Spi.RomBaseAddress;

  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG50, AccessWidth32, ~(UINT32) (BIT0 + BIT1), (BIT0 + BIT1), StdHeader);
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG54, AccessWidth32, ~(UINT32) (BIT0 + BIT1), (BIT0 + BIT1), StdHeader);
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG58, AccessWidth32, ~(UINT32) (BIT0 + BIT1), (BIT0 + BIT1), StdHeader);
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG5C, AccessWidth32, ~(UINT32) (BIT0 + BIT1), (BIT0 + BIT1), StdHeader);
  RwMem (SpiRomBase + FCH_SPI_MMIO_REG00, AccessWidth32, ~(UINT32) (BIT22 + BIT23), 0);
}
