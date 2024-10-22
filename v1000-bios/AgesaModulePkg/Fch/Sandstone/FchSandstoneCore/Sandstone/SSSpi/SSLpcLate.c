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
#define FILECODE PROC_FCH_SANDSTONE_SSSPI_SSLPCLATE_FILECODE

/**
 * FchInitLateLpc - Prepare Ir controller to boot to OS.
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitLateLpc (
  IN  VOID     *FchDataPtr
  )
{
    FCH_DATA_BLOCK         *LocalCfgPtr;
    AMD_CONFIG_PARAMS      *StdHeader;

    LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
    StdHeader = LocalCfgPtr->StdHeader;

    if ( LocalCfgPtr->FchResetDataBlock.Spi.LpcClk0 ) {
      RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGD0 + 1, AccessWidth8, 0xDF, 0x20, StdHeader);
    } else {
      RwPci (((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG44), AccessWidth32, 00, 0, StdHeader);
      RwPci (((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG48), AccessWidth32, 0xFF00, 0, StdHeader);
      RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGD0 + 1, AccessWidth8, 0xDF, 0, StdHeader);
    }
    //Arista
    RwPci (((LPC_BUS_DEV_FUN << 16) + 0xa0), AccessWidth32, 0xFFFFFFFF, 8, StdHeader);
    RwPci (((LPC_BUS_DEV_FUN << 16) + 0x7c), AccessWidth32, 0xFFFFFFFF, 5, StdHeader);
    // Config SPI_TPM_CS_L
    MmioWrite8 (ACPI_MMIO_BASE + IOMUX_BASE + 0x1D, 0);	 //Arista: was 0x1C, changed it to 0x1D	for SPI_TPM_CS_L

}

