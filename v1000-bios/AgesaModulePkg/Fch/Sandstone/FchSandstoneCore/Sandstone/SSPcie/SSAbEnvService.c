/* $NoKeywords:$ */
/**
 * @file
 *
 * Config TAISHAN AB
 *
 * Init AB bridge.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 312251 $   @e \$Date: 2015-02-03 09:52:19 -0800 (Tue, 03 Feb 2015) $
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
#define FILECODE PROC_FCH_SANDSTONE_SSPCIE_SSABENVSERVICE_FILECODE

//
// Declaration of local functions
//
VOID AbCfgTbl (IN AB_TBL_ENTRY  *ABTbl, IN AMD_CONFIG_PARAMS *StdHeader);

/**
 * TAISHANInitEnvAbTable - AB-Link Configuration Table for
 * TAISHAN
 *
 */
AB_TBL_ENTRY SandstoneInitEnvAbTable[] =
{
  //
  // Setting B-Link Prefetch Mode (ABCFG 0x80 [18:17] = 11)
  // BlPrefEn [7:0] = 1
  //
  {ABCFG, FCH_ABCFG_REG80, BIT17 + BIT18, BIT17 + BIT18 + 0x01},

  //
  // Enabled SMI ordering enhancement. ABCFG 0x90[21]
  // USB Delay A-Link Express L1 State. ABCFG 0x90[17]
  //
  // Wait for BKDG update {ABCFG, FCH_ABCFG_REG90, BIT21 + BIT17, BIT21 + BIT17},
  {ABCFG, FCH_ABCFG_REG90, BIT21, BIT21},

  //
  // Enabling Detection of Upstream Interrupts ABCFG 0x94 [20] = 1
  // ABCFG 0x94 [19:0] = cpu interrupt delivery address [39:20]
  //
  {ABCFG, FCH_ABCFG_REG94, BIT20, BIT20 + 0x00FEE},

  //
  // Programming cycle delay for AB and BIF clock gating
  // Enable the AB and BIF clock-gating logic.
  // Enable the A-Link int_arbiter enhancement to allow the A-Link bandwidth to be used more efficiently
  //
  // Wait for BKDG update {ABCFG, FCH_ABCFG_REG10054,  0x00FFFFFF, 0x010407FF},
  {ABCFG, FCH_ABCFG_REG10054,  0x00FFFFFF, 0x000007FF},
  //{ABCFG, FCH_ABCFG_REG98,  0x0003FF00, 0x00034700},
  // Wait for BKDG update {ABCFG, FCH_ABCFG_REG54,  0x00FF0000, 0x00040000},

  //
  // Host Outstanding Completion Clock Gating
  //
  // Wait for BKDG update {ABCFG, FCH_ABCFG_REG208,  0xFFFFFFEF, 0x00081000},

  //
  // SD ALink prefetch
  //
  {ABCFG, FCH_ABCFG_REG10060, 0xFBFFFFFF, 0x02000000},

  //
  // Enable the IO trap delay logic for the SMI message to ensure that the SMI messages are sent to the CPU in the right order.
  //
  {ABCFG, FCH_ABCFG_REG10090, BIT16, BIT16},
  {ABCFG, 0, 0, (UINT8) 0xFF},                                                 /// This dummy entry is to clear ab index
  { (UINT8)0xFF, (UINT8)0xFF, (UINT8)0xFF, (UINT8)0xFF},
};

/**
 * FchInitEnvAbLinkInit - Set ABCFG registers before PCI
 * emulation.
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvAbLinkInit (
  IN  VOID     *FchDataPtr
  )
{
  UINT8                  AbValue8;
  AB_TBL_ENTRY           *AbTblPtr;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  // AB CFG programming
  //
  if ( LocalCfgPtr->Ab.SlowSpeedAbLinkClock ) {
    RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG40, AccessWidth8, ~(UINT32) BIT1, BIT1);
  } else {
    RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG40, AccessWidth8, ~(UINT32) BIT1, 0);
  }

  AbTblPtr = (AB_TBL_ENTRY *) (&SandstoneInitEnvAbTable[0]);
  AbCfgTbl (AbTblPtr, StdHeader);

  if ( LocalCfgPtr->Ab.ResetCpuOnSyncFlood ) {
    RwAlink (FCH_ABCFG_REG10050 | (UINT32) (ABCFG << 29), ~(UINT32) BIT2, BIT2, StdHeader);
  } else {
    RwAlink (FCH_ABCFG_REG10050 | (UINT32) (ABCFG << 29), ~(UINT32) BIT2, 0, StdHeader);
  }

  if ( LocalCfgPtr->Ab.AbClockGating ) {
    RwAlink (FCH_ABCFG_REG54 | (UINT32) (ABCFG << 29), ~ (UINT32) (0xFF << 16), (UINT32) (0x10 << 16), StdHeader);
    RwAlink (FCH_ABCFG_REG10054 | (UINT32) (ABCFG << 29), ~ (UINT32) (0xFF << 16), (UINT32) (0x10 << 16), StdHeader);
    RwAlink (FCH_ABCFG_REG54 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 4), (UINT32) (0x1 << 4), StdHeader);
    RwAlink (FCH_ABCFG_REG54 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 24), (UINT32) (0x1 << 24), StdHeader);
    RwAlink (FCH_ABCFG_REG10054 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x3 << 24), (UINT32) (0x3 << 24), StdHeader);
  } else {
    RwAlink (FCH_ABCFG_REG10054 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x3 << 24), 0, StdHeader);
    RwAlink (FCH_ABCFG_REG54 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 24), 0, StdHeader);
    RwAlink (FCH_ABCFG_REG54 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 4), 0, StdHeader);
  }

  if ( LocalCfgPtr->Ab.AbDmaMemoryWrtie3264B ) {
    RwAlink (FCH_ABCFG_REG54 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 0), (UINT32) (0x0  << 0), StdHeader);
    RwAlink (FCH_ABCFG_REG54 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 2), (UINT32) (0x1  << 2), StdHeader);
  } else {
    RwAlink (FCH_ABCFG_REG54 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 0), (UINT32) (0x1  << 0), StdHeader);
    RwAlink (FCH_ABCFG_REG54 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 2), (UINT32) 0x0, StdHeader);
  }

  if ( LocalCfgPtr->Ab.AbMemoryPowerSaving ) {
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x68, AccessWidth8, 0xFB, 0x00);
    RwAlink (FCH_ABCFG_REGBC | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 4), (UINT32) (0x1  << 4), StdHeader);
    RwAlink (FCH_ABCFG_REG58 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 29), (UINT32) (0x1  << 29), StdHeader);
    RwAlink (FCH_ABCFG_REG58 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 31), (UINT32) (0x1  << 31), StdHeader);
  } else {
    RwAlink (FCH_ABCFG_REG58 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x5 << 29), (UINT32) 0x0, StdHeader);
    RwAlink (FCH_ABCFG_REGBC | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 4), (UINT32) 0x0, StdHeader);
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x68, AccessWidth8, 0xFB, 0x04);
  }

  //
  // A/B Clock Gate-OFF
  //
  if ( LocalCfgPtr->Ab.ALinkClkGateOff ) {
    RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG2C + 2, AccessWidth8, 0xFE, BIT0);
  } else {
    RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG2C + 2, AccessWidth8, 0xFE, 0);
  }
  if ( LocalCfgPtr->Ab.BLinkClkGateOff ) {
    RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG2C + 2, AccessWidth8, 0xFD, BIT1);
  } else {
    RwMem (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG2C + 2, AccessWidth8, 0xFD, 0);
  }
  if ( LocalCfgPtr->Ab.ALinkClkGateOff | LocalCfgPtr->Ab.BLinkClkGateOff ) {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG04 + 2, AccessWidth8, 0xFE, BIT0);
  } else {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG04 + 2, AccessWidth8, 0xFE, 0);
  }

  if ( LocalCfgPtr->Ab.SbgMemoryPowerSaving ) {
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x68, AccessWidth8, 0xFD, 0x00);
    RwAlink (FCH_ABCFG_REG208 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x5 << 7), (UINT32) (0x5 << 7), StdHeader);
  } else {
    RwAlink (FCH_ABCFG_REG208 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x5 << 7), (UINT32) 0x0, StdHeader);
    RwMem (ACPI_MMIO_BASE + MISC_BASE + 0x68, AccessWidth8, 0xFD, 0x02);
  }

  //
  // SBG Internal Clock Gating
  //
  if ( LocalCfgPtr->Ab.SbgClockGating ) {
    RwAlink (FCH_ABCFG_REG208 | (UINT32) (ABCFG << 29), ~ (UINT32) (BIT4 + BIT15 + BIT22), (UINT32) (BIT4 + BIT15 + BIT22), StdHeader);
  } else {
    RwAlink (FCH_ABCFG_REG208 | (UINT32) (ABCFG << 29), ~ (UINT32) (BIT4 + BIT15 + BIT22), (UINT32) 0x0, StdHeader);
  }
  //
  // XDMA DMA Write 16 byte Mode
  //
  if ( LocalCfgPtr->Ab.XdmaDmaWrite16ByteMode ) {
    RwAlink (FCH_ABCFG_REG180 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 0), (UINT32) (0x1  << 0), StdHeader);
  } else {
    RwAlink (FCH_ABCFG_REG180 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 0), (UINT32) (0x0), StdHeader);
  }
  //
  // XDMA Memory Power Saving
  //
  if ( LocalCfgPtr->Ab.XdmaMemoryPowerSaving ) {
    RwAlink (FCH_ABCFG_REG184 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 2), (UINT32) (0x1  << 2), StdHeader);
  } else {
    RwAlink (FCH_ABCFG_REG184 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 2), (UINT32) 0x0, StdHeader);
  }
  //
  // XDMA Pending NPR Threshold
  //
  if ( LocalCfgPtr->Ab.XdmaPendingNprThreshold ) {
    AbValue8 = LocalCfgPtr->Ab.XdmaPendingNprThreshold;
    RwAlink (FCH_ABCFG_REG180 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1F << 8), (UINT32) (AbValue8  << 8), StdHeader);
  } else {
    RwAlink (FCH_ABCFG_REG180 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1F << 8), (UINT32) (0x0), StdHeader);
  }
  //
  // XDMA DNCPL Order Dis
  //
  if ( LocalCfgPtr->Ab.XdmaDncplOrderDis ) {
    RwAlink (FCH_ABCFG_REG180 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 5), (UINT32) (0x1  << 5), StdHeader);
  } else {
    RwAlink (FCH_ABCFG_REG180 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 5), (UINT32) (0x0), StdHeader);
  }
  //
  // SDPHOST_BYPASS_DATA_PAC
  //
  if ( LocalCfgPtr->Ab.SdphostBypassDataPack ) {
    RwAlink (FCH_ABCFG_REG22C | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 11), (UINT32) (0x1  << 11), StdHeader);
  } else {
    RwAlink (FCH_ABCFG_REG22C | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 11), (UINT32) (0x0), StdHeader);
  }
  //
  // SDPHOST_DIS_NPMWR_PROTECT
  //
  if ( LocalCfgPtr->Ab.SdphostDisNpmwrProtect ) {
    RwAlink (FCH_ABCFG_REG22C | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 14), (UINT32) (0x1  << 14), StdHeader);
  } else {
    RwAlink (FCH_ABCFG_REG22C | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 14), (UINT32) (0x0), StdHeader);
  }

  //
  // Enable SDP Disconnect
  //
  RwAlink (FCH_ABCFG_REG228 | (UINT32) (ABCFG << 29), ~ (UINT32) (BIT1 + BIT3), (UINT32) (BIT1 + BIT3), StdHeader);
}

/**
 * AbCfgTbl - Program ABCFG by input table.
 *
 *
 * @param[in] ABTbl  ABCFG config table.
 * @param[in] StdHeader
 *
 */
VOID
AbCfgTbl (
  IN  AB_TBL_ENTRY     *ABTbl,
  IN AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32   AbValue;

  while ( (ABTbl->RegType) != 0xFF ) {
    if ( ABTbl->RegType == AXINDC ) {
      AbValue = 0x30 | (ABTbl->RegType << 29);
      WriteAlink (AbValue, (ABTbl->RegIndex & 0x00FFFFFF), StdHeader);
      AbValue = 0x34 | (ABTbl->RegType << 29);
      WriteAlink (AbValue, ((ReadAlink (AbValue, StdHeader)) & (0xFFFFFFFF^ (ABTbl->RegMask))) | ABTbl->RegData, StdHeader);
    } else if ( ABTbl->RegType == AXINDP ) {
      AbValue = 0x38 | (ABTbl->RegType << 29);
      WriteAlink (AbValue, (ABTbl->RegIndex & 0x00FFFFFF), StdHeader);
      AbValue = 0x3C | (ABTbl->RegType << 29);
      WriteAlink (AbValue, ((ReadAlink (AbValue, StdHeader)) & (0xFFFFFFFF^ (ABTbl->RegMask))) | ABTbl->RegData, StdHeader);
    } else {
      AbValue = ABTbl->RegIndex | (ABTbl->RegType << 29);
      WriteAlink (AbValue, ((ReadAlink (AbValue, StdHeader)) & (0xFFFFFFFF^ (ABTbl->RegMask))) | ABTbl->RegData, StdHeader);
    }

    ++ABTbl;
  }

  //
  //Clear ALink Access Index
  //
  AbValue = 0;
  LibAmdIoWrite (AccessWidth32, ALINK_ACCESS_INDEX, &AbValue, StdHeader);
}

