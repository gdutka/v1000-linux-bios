/* $NoKeywords:$ */
/**
 * @file
 *
 * Fch SATA controller Library
 *
 * SATA Library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project   FCH SMM Driver
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include "FchPlatform.h"

#define MAX_RETRY_NUM   100

typedef struct _SATA_REG_ENTRY {
  UINT32       SmnAddress;
  UINT32       Value;
} SATA_REG_ENTRY;

typedef union {
  struct {
    UINT32      PxDET:4;
    UINT32      PxSPD:4;
    UINT32      PxIPM:4;
    UINT32      Reserved:20;
  } Field;
  UINT32 Value;
} AHCI_PxSSTS;

SATA_REG_ENTRY  mFchSataD3ColdSaveRestoreList[] = {
  { FCH_SS_SMN_SATA_CONTROL_BAR5 + 0x00, 0},
  { FCH_SS_SMN_SATA_CONTROL_BAR5 + 0x0C, 0},
  { FCH_SS_SMN_SATA_CONTROL_BAR5 + 0x10, 0},
  { FCH_SS_SMN_SATA_CONTROL_BAR5 + 0xF4, 0},
  { FCH_SS_SMN_SATA_CONTROL_BAR5 + 0xF8, 0},
  { FCH_SS_SMN_SATA_CONTROL_BAR5 + 0xFC, 0},
  { FCH_SS_SMN_SATA_CONTROL_BAR5 + 0x128, 0},
  { FCH_SS_SMN_SATA_CONTROL_BAR5 + 0x1A8, 0},

  { FCH_SS_SMN_SATA_CONTROL_SLOR + 0x00, 0},
  { FCH_SS_SMN_SATA_CONTROL_SLOR + 0x04, 0},
  { FCH_SS_SMN_SATA_CONTROL_SLOR + 0x08, 0},
  { FCH_SS_SMN_SATA_CONTROL_SLOR + 0x0C, 0},
  { FCH_SS_SMN_SATA_CONTROL_SLOR + 0x10, 0},
  { FCH_SS_SMN_SATA_CONTROL_SLOR + 0x14, 0},
  { FCH_SS_SMN_SATA_CONTROL_SLOR + 0x1C, 0},
  { FCH_SS_SMN_SATA_CONTROL_SLOR + 0x40, 0},
  { FCH_SS_SMN_SATA_CONTROL_SLOR + 0x48, 0},
  { FCH_SS_SMN_SATA_CONTROL_SLOR + 0x4C, 0},
  { FCH_SS_SMN_SATA_CONTROL_SLOR + 0x50, 0},
  { FCH_SS_SMN_SATA_CONTROL_SLOR + 0x54, 0},
  { FCH_SS_SMN_SATA_CONTROL_SLOR + 0x88, 0},
  { FCH_SS_SMN_SATA_CONTROL_SLOR + 0x8C, 0},
  { FCH_SS_SMN_SATA_CONTROL_SLOR + 0x98, 0},
  { FCH_SS_SMN_SATA_CONTROL_SLOR + 0xA8, 0},
  { FCH_SS_SMN_SATA_CONTROL_SLOR + 0xAC, 0},
  { FCH_SS_SMN_SATA_CONTROL_SLOR + 0xB8, 0},
};

#define NUM_OF_STORE_REGISTERS  sizeof (mFchSataD3ColdSaveRestoreList) / sizeof (SATA_REG_ENTRY)


VOID FchSataD3ColdOverrideDevSlpPad ()
{
  UINT8          Port;
  AHCI_PxSSTS    Px_SSTS;
  for (Port = 0; Port < 2; Port++) {
    FchSmnRead (0, FCH_SS_SMN_SATA_CONTROL_BAR5 + FCH_SATA_BAR5_REG128 + Port * 0x80, &Px_SSTS.Value, NULL);
    if ( Px_SSTS.Field.PxIPM == 8) {
      ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGDC) |= 1 << Port;
    }
  }
}

VOID FchSataD3ColdRemoveOverrideDevSlpPad ()
{
  ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGDC) &= 0xFC;
}

VOID FchSataD3ColdOfflineMode ()
{
  UINT8          Port;
  AHCI_PxSSTS    Px_SSTS;
  for (Port = 0; Port < 2; Port++) {
    FchSmnRead (0, FCH_SS_SMN_SATA_CONTROL_BAR5 + FCH_SATA_BAR5_REG128 + Port * 0x80, &Px_SSTS.Value, NULL);
    if ((Px_SSTS.Field.PxIPM != 8) && (Px_SSTS.Field.PxIPM != 6) && (Px_SSTS.Field.PxDET != 4)) {
      FchSmnRW (0, FCH_SS_SMN_SATA_CONTROL_BAR5 + FCH_SATA_BAR5_REG12C + Port * 0x80, 0xFFFFFFF0, 0x4, NULL);
    }
  }
}

VOID FchSataD3ColdPollingOffline ()
{
  UINT8          Port;
  UINT32         Retry;
  AHCI_PxSSTS    Px_SSTS;
  for (Port = 0; Port < 2; Port++) {
    for (Retry = 0; Retry < MAX_RETRY_NUM; Retry++) {
      FchSmnRead (0, FCH_SS_SMN_SATA_CONTROL_BAR5 + FCH_SATA_BAR5_REG128 + Port * 0x80, &Px_SSTS.Value, NULL);
      if ((Px_SSTS.Field.PxIPM == 8) || (Px_SSTS.Field.PxIPM == 6) || (Px_SSTS.Field.PxDET == 4)) {
        break;
      }
      FchStall (10, NULL);
    }
  }
}

VOID FchSataD3ColdSaveRegisters ()
{
  UINT8          i;
  for (i = 0; i < NUM_OF_STORE_REGISTERS; i++) {
    FchSmnRead (0, mFchSataD3ColdSaveRestoreList[i].SmnAddress, &mFchSataD3ColdSaveRestoreList[i].Value, NULL);
  }
}

VOID FchSataD3ColdRestoreRegisters ()
{
  UINT8          i;

  FchSmnRW (0, FCH_SS_SMN_SATA_CONTROL_SLOR + 00, ~(UINT32)BIT0, BIT0, NULL);
  for (i = 0; i < NUM_OF_STORE_REGISTERS; i++) {
    FchSmnRW (0, mFchSataD3ColdSaveRestoreList[i].SmnAddress, 0x00, mFchSataD3ColdSaveRestoreList[i].Value, NULL);
  }
  FchSmnRW (0, FCH_SS_SMN_SATA_CONTROL_SLOR + 00, ~(UINT32)BIT0, 0, NULL);
}

VOID FchSataD3ColdBlockReset ()
{
  FchSmnRW (0, FCH_SS_SATA_AOAC_CONTROL, ~(UINT32)BIT11, BIT11, NULL);
}

VOID FchSataD3ColdUnBlockReset ()
{
  FchSmnRW (0, FCH_SS_SATA_AOAC_CONTROL, ~(UINT32)BIT11, 0, NULL);
}

VOID FchSataD3ColdEnable100MClk ()
{
  FchSmnRW (0, FCH_SS_SMN_MISC_BASE + 0x4C, ~(UINT32)BIT16, BIT16, NULL);
}

VOID FchSataD3ColdBlockOOB ()
{
  FchSmnRW (0, FCH_SS_SATA_AOAC_CONTROL, ~(UINT32)BIT10, BIT10, NULL);
}

VOID FchSataD3ColdUnBlockOOB ()
{
  FchSmnRW (0, FCH_SS_SATA_AOAC_CONTROL, ~(UINT32)BIT10, 0, NULL);
}

BOOLEAN FchSataD3ColdClockOn ()
{
  UINT32                      MicsClkCntl4;
  FchSmnRead (0, FCH_SS_SMN_MISC_BASE + 0x4C, &MicsClkCntl4, NULL);
  if (MicsClkCntl4 & BIT16) {
    return TRUE;
  } else {
    return FALSE;
  }
}

BOOLEAN FchSataD3ColdPowerStateOn ()
{
  UINT32                      Pgfsm_RegData;
  FchSmnRW (0, FCH_SS_RSMU_SATA_PGFSM_CNTL, 0x00, PGFSM_SELECT_0 + PGFSM_CMD_POWER_READ + PGFSM_SOFTWARE_CONTROL, NULL);  // read Status register 0x00
  FchSmnRead (0, FCH_SS_RSMU_SATA_PGFSM_RD_DATA, &Pgfsm_RegData, NULL);
  if ((Pgfsm_RegData & PGFSM_STATUS_PWR_STS) == PGFSM_STATUS_PWR_ON) {
    return TRUE;
  } else {
    return FALSE;
  }
}

VOID FchSataD3ColdPowerDown ()
{
  UINT32                      SataRegData;

  // 6, SW must program SLOR_Reg 0x200[4] register PhyRefClk_Off_sw  to 1 if PHY belong to SATA. Wait for PhyRefClk_Off_sts  SLOR_Reg 0x200h[20] change to value 1.
  FchSmnRW (0, FCH_SS_SMN_SATA_CONTROL_SLOR + 0x200, ~(UINT32)BIT0, BIT0, NULL);  //for testing
  FchSmnRW (0, FCH_SS_SMN_SATA_CONTROL_SLOR + 0x200, ~(UINT32)BIT4, BIT4, NULL);

  do {
    FchSmnRead (0, FCH_SS_SMN_SATA_CONTROL_SLOR + 0x200, &SataRegData, NULL);
  } while ((SataRegData & BIT20) == 0);

  //7, start PGFSM power down sequence for D3cold
  FchSmnRW (0, FCH_SS_RSMU_SATA_PGFSM_CNTL, 0x00, PGFSM_SELECT_0 + PGFSM_CMD_POWER_DOWN + PGFSM_SOFTWARE_CONTROL, NULL);
  FchSmnRW (0, FCH_SS_RSMU_SATA_PGFSM_CNTL, 0x00, PGFSM_SELECT_0 + PGFSM_CMD_POWER_READ + PGFSM_SOFTWARE_CONTROL, NULL);  // read Status register 0x00
  do {
    FchSmnRead (0, FCH_SS_RSMU_SATA_PGFSM_RD_DATA, &SataRegData, NULL);
  } while ((SataRegData & PGFSM_STATUS_PWR_STS) != PGFSM_STATUS_PWR_OFF);

  //8, Stop clock: SW programs LOCAL_RSMU_Reg: 04h[8] = 1
  FchSmnRW (0, FCH_SS_SATA_AOAC_CONTROL, ~(UINT32)BIT8, BIT8, NULL);

  //9, Stop FCH_SATA_100M_CLK: sw program ACPI Misc_Reg 0x4C[16] to 0
  FchSmnRW (0, FCH_SS_SMN_MISC_BASE + 0x4C, ~(UINT32)BIT16, 0x00, NULL);
}

VOID FchSataD3ColdPowerUp ()
{
  UINT32                      SataRegData;
  //4. Starts clock: SW programs LOCAL_RSMU_Reg: 04h[8] = 0
  FchSmnRW (0, FCH_SS_SATA_AOAC_CONTROL, ~(UINT32)BIT8, 0, NULL);
  //5. Turn On PHY Reference Clock
  //Program PhyRefClk_Off_sw SLOR_Reg 0x200[4]  to 0, and wait for PhyRefClk_Off_sts SLOR_Reg 0x200[20] change to value 0;
  FchSmnRW (0, FCH_SS_SMN_SATA_CONTROL_SLOR + 0x200, ~(UINT32)BIT0, BIT0, NULL);  //for testing
  FchSmnRW (0, FCH_SS_SMN_SATA_CONTROL_SLOR + 0x200, ~(UINT32)BIT4, 0, NULL);

  do {
    FchSmnRead (0, FCH_SS_SMN_SATA_CONTROL_SLOR + 0x200, &SataRegData, NULL);
  } while ((SataRegData & BIT20) != 0);

  //6. Negate PGFSM_hard_resetb: Through trigger PGFSM state machine transition when clock starts, power is off, ISO is on, and reset is asserted. 
  //PGFSM Start PowerUp process triggered by SW programs RSMU_PGFSM_CONTROL = 16'b0103h
  //SW waits for power-on process done by polling PGFSM_Status[1:0]: Power_Stats_P1= 2'b00 -Power is On.
  FchSmnRW (0, FCH_SS_RSMU_SATA_PGFSM_CNTL, 0x00, PGFSM_SELECT_0 + PGFSM_CMD_POWER_UP + PGFSM_SOFTWARE_CONTROL, NULL);
  FchSmnRW (0, FCH_SS_RSMU_SATA_PGFSM_CNTL, 0x00, PGFSM_SELECT_0 + PGFSM_CMD_POWER_READ + PGFSM_SOFTWARE_CONTROL, NULL);  // read Status register 0x00
  do {
    FchSmnRead (0, FCH_SS_RSMU_SATA_PGFSM_RD_DATA, &SataRegData, NULL);
  } while ((SataRegData & PGFSM_STATUS_PWR_STS) != PGFSM_STATUS_PWR_ON);
}


