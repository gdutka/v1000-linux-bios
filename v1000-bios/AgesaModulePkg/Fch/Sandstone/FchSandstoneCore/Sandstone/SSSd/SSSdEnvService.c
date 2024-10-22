/* $NoKeywords:$ */
/**
 * @file
 *
 * Config SANDSTONE SD
 *
 * Init SD Controller.
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
#define FILECODE PROC_FCH_SANDSTONE_SSSD_SSSDENVSERVICE_FILECODE
/**
 * FchInitEnvSdProgram - Config SD controller before PCI
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvSdProgram (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  // SD Configuration
  //
  if (LocalCfgPtr->Sd.SdConfig == SdDisable) {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGD3, AccessWidth8, 0xBF, 0x00);
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGE8, AccessWidth8, 0xFE, 0x00);
    ACPIMMIO8 (FCH_AOACx72_SD1_D3_CONTROL) &= ~ AOAC_PWR_ON_DEV;
  } else if (LocalCfgPtr->Sd.SdConfig != SdDump) {
    //ACPIMMIO8 (FCH_PMxE8_SDFlashCntrl) |= FCH_PMxE8_SDFlashCntrl_SdCd2Acpi; //Enable SD_CD0
    //ACPIMMIO8 (FCH_IOMUXx66_SD0_PWR_CTRL_AGPIO102) = 0; //ENH448923
    //ACPIMMIO32(FCH_MISCx68_MEMORY_POWER_SAVING_CONTROL) &= ~ BIT18; //SdBypassMemDsd

    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGD3, AccessWidth8, 0xBF, 0x40);
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGE8, AccessWidth8, 0xFE, BIT0);

    if (LocalCfgPtr->Sd.SdSsid != NULL ) {
      RwPci ((SANDSTONE_SD_BUS_DEV_FUN << 16) + SD_PCI_REG2C, AccessWidth32, 0, LocalCfgPtr->Sd.SdSsid, StdHeader);
    }

    RwPci ((SANDSTONE_SD_BUS_DEV_FUN << 16) + SD_PCI_REGA4, AccessWidth32, 0x3FFFFFFF, ((UINT32) (LocalCfgPtr->Sd.SdSlotType) << 30), StdHeader);

    if ( LocalCfgPtr->Sd.SdConfig == SdVer2) {
// SD 2.0
//The following programming sequence sets up SD controller to operate in SD 2.0 mode. (UBTS538428).
//A. Program D14F7xA4[BaseFreq]= 32h.
//B. Program D14F7xA8=[Ddr50Sup]=0.
//C. Program D14F7xA8=[Sdr100Sup]=0.
//D. Program D14F7xA8=[Sdr50Sup]=0.
//E. Program D14F7xB0=[HostVersionSel]=1h.
      RwPci ((SANDSTONE_SD_BUS_DEV_FUN << 16) + SD_PCI_REGA4, AccessWidth32, ~ (UINT32) (0xFF << 8), (UINT32) (0x32 << 8), StdHeader);
      RwPci ((SANDSTONE_SD_BUS_DEV_FUN << 16) + SD_PCI_REGA8, AccessWidth32, ~ (UINT32) (0x01 << 2), (UINT32) (0x00 << 2), StdHeader);
      RwPci ((SANDSTONE_SD_BUS_DEV_FUN << 16) + SD_PCI_REGA8, AccessWidth32, ~ (UINT32) (0x01 << 1), (UINT32) (0x00 << 1), StdHeader);
      RwPci ((SANDSTONE_SD_BUS_DEV_FUN << 16) + SD_PCI_REGA8, AccessWidth32, ~ (UINT32) (0x01 << 0), (UINT32) (0x00 << 0), StdHeader);
      RwPci ((SANDSTONE_SD_BUS_DEV_FUN << 16) + SD_PCI_REGB0, AccessWidth32, ~ (UINT32) (0xFF << 24), (UINT32) (0x01 << 24), StdHeader);
    } else {
      //  no SD3.0 POR on TS
    }
    if ( LocalCfgPtr->Sd.SdForce18 ) {
      RwPci ((SANDSTONE_SD_BUS_DEV_FUN << 16) + SD_PCI_REGD0, AccessWidth32, ~ ((UINT32) (0x01 << 17)), ((UINT32) (0x01) << 17), StdHeader);
    }

    if (LocalCfgPtr->Sd.SdDbgConfig == 1) {
      //DMA clear BIT19,BIT20
      RwPci ((SANDSTONE_SD_BUS_DEV_FUN << 16) + SD_PCI_REGA4, AccessWidth32, ~ (UINT32) (BIT19 + BIT20), 0, StdHeader);
    }

    if (LocalCfgPtr->Sd.SdDbgConfig == 2) {
      //PIO clear BIT19,BIT20,BIT22
      RwPci ((SANDSTONE_SD_BUS_DEV_FUN << 16) + SD_PCI_REGA4, AccessWidth32, ~ (UINT32) (BIT19 + BIT20 + BIT22), 0, StdHeader);
    }
  }
}

