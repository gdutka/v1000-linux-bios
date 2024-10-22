/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

Mutex(Z009,0)
Mutex(QEVT,0)
OperationRegion(ERAM, EmbeddedControl, 0, 0xFF)
Field(ERAM, ByteAcc, NoLock, Preserve)
{
                 // 0x00 ~ 0x31 Reserved
  Offset(0x60),  // UCSI
  VERS,16,       // PPM->OPM Version         ( 2 bytes)
      ,16,       // Reserved                 ( 2 bytes)
  CCI, 32,       // PPM->OPM CCI indicator   ( 4 bytes)
  CTRL,64,       // OPM->PPM Control message ( 8 bytes)
  MSGI,128,      // OPM->PPM Message In      (16 bytes)
  MSGO,128,      // PPM->OPM Message Out     (16 bytes)
  Offset(0x90),
  PMC0,8,        // PMIC register index
  PMC1,8,        // value write to/read from PMIC
  PMC2,8,        // on write, this field indicates the 8-bit PMIC slave device address. 
                 //           LSB is 1 to trigger read operation, 0 to trigger write operation.
                 // on read,  returns the status of last operation. 
                 //           0xAC - success, 0xE2 - error, 0xCC - ongoing
  Offset(0x93),
  BDNE,64,
  CHTY,64,
  MANU,64,
  Offset(0xAC),
  GPO2,8,         // GPIO group 2 (Input)
                  // Bit[0] - Board_ID0
                  // Bit[1] - Board_ID1
                  // Bit[2] - Board_ID2
                  // Bit[3] - Board_ID3
                  // Bit[4] - Board_ID4
                  // Bit[5] - DT_PRESENT# - PCIe X4 device is present, low active
                  // Bit[6] - ODD_PLUGIN# - ODD is plugged, low active
                  // Bit[7] - HDA_SDW_SEL - HDA or SDW is selected, High HDA, low SDW
  Offset(0xAD),
  GPO3,8,         // GPIO group 3 (Input)
                  // Bit[0] - FCH_ODD_DA# - ODD device attention, low active
                  // Bit[1] - DOCK_IN# - Dock is present or not, low active
                  // Bit[2] - EXT_TALERT# - EVAL card's thermal alert is triggered, low active
                  // Bit[3] - LID_CLOSED# - Lid is closed or not, low active
                  // Bit[4] - MEDIA_SENSE - Lan's cable is plugged in, high active
                  // Bit[5] - APU_THERMTRIP# - APU's thermtrip is triggered, low active
                  // Bit[6] - EVAL_PRESENT# - EVAL is present, low active
                  // Bit[7] - PCIE_RST# - Reserved PCIe reset
  Offset(0xAE),
  GPO4,8,         // GPIO group 4 (Input)
                  // Bit[0] - PLUG_EVENT - EC GPIO104 for xConnect
                  // Bit[7:1] - Reserved
  Offset(0xAF),
  GPO5,8,					// GPIO group 5 (Input/Reserved)
  Offset(0xB0),
  GPO6,8,         // GPIO group 6 (Output)
                  // Bit[0] - BT_RADIO_DISABLE# - To disable Bluetooth radio, low active
                  // Bit[1] - WL_RADIO_DISABLE#/WL_R_RADIO_DISABLE# - To disable WLAN's raido, low active
                  // Bit[2] - WLAN_PWR_EN/WLAN_R_PWR_EN - To enable WLAN/WLAN_R's powr rail, high active
                  // Bit[3] - GNSS_RADIO_DISABLE# - To disable WWAN's GNSS raido, low active
                  // Bit[4] - MAIN_RADIO_DIASBLE# - To disable WWAN's radio, low active
                  // Bit[5] - WWAN_CARD_POWER_OFF# - To power off WWAN, low active
                  // Bit[6] - PCIEWWAN_PWREN - To enable WWAN's power rail, high active
                  // Bit[7] - LAN_LOW_PWR - To make GbE enter into low power mode, high active, default should be low
	Offset(0xB1),
  GPO7,8,         // GPIO group 7 (Output)
                  // Bit[3:0] - Reserved
                  // Bit[4] - LOM_RESET#_AUX - AUX reset for GbE, low active
                  // Bit[5] - PCIE_WWAN_RST# - AUX Reset for WWAN, low active
                  // Bit[6] - DT_RESET#_AUX - AUX Reset for DT slot, low active
                  // Bit[7] - WLAN_RESET#_AUX/WLAN_R_RESET#_AUX - To reset WLAN/WLAN_R
  Offset(0xB2),
  GPO8,8,         // GPIO group 8 (Output)
                  // Bit[0] - AC_DC_SW - High to gate ACIN when AC adatper is plugged in, default should be low
                  // Bit[1] - DT_PWREN - To enable DT slot's power rail, high active
                  // Bit[2] - LOM_PWR_EN - To enable GbE's power rail, high active
                  // Bit[3] - HDD_PWR_EN - To enable HDD power rail
                  // Bit[4] - ODD_PWR_EN - To enable ODD power rail
                  // Bit[5] - FORCE_PWR - EC GPIO105 for xConnect
                  // Bit[7:5] - Reserved
  Offset(0xB3),
  GPO9,8,         // GPIO group 9 (extended GPIOs, reads through I2C. Long latency is expected)
                  // Bit[0] - EC_UPD0_PWRDN - To disable UPD0, High active
                  // Bit[1] - EC_UPD1_PWRDN - To disable UPD1, High active
                  // Bit[2] - DC_S5_ALW_EN_CTRL - 1: ALW power on in DC mode; 0: ALW power off in DC mode, default.
                  // Bit[7:3] - Reserved
  Offset(0xB6),
  SW01,8,         // Switch control 0
                  // Bit[0] - WIFI/BT radio permitted
                  // Bit[1] - GPS/WWAN radio permitted
                  // Bit[2] - Reserved (was 'ALS notification switch')
                  // Bit[3] - WLAN_PWR_EN (WIFI) simpling - 1: Input; 0: Output
                  // Bit[4] - PCIEWWAN_PWREN (WWAN) simpling - 1: Input; 0: Output
                  // Bit[5] - LOM_PWR_EN (Giga LAN) simpling - 1: Input; 0: Output
                  // Bit[6] - DP/HDMI of USBC Port 0 - 1: HDMI; 0: DP
                  // Bit[7] - DP/HDMI of USBC Port 1 - 1: HDMI; 0: DP
  SW02,8,         // Switch control 1
                  // Bit[0] - LAN low power mode enable - 1: enable; 0: disable
                  // Bit[7:1] - Reserved
  Offset (0xB9),
  VERL, 40,       // EC library version   "0304 "
  Offset (0xC2),
  ECRV, 40,       // EC firmware revision "0.x.x"
  Offset(0xC7),
  DCTM, 8,        // Set DC time for AC/DC switch, maximum is 255 seconds
  ACTM, 8,        // Set AC time for AC/DC switch, maximum is 255 seconds
  Offset(0xC9),
  BTPL, 8,        // LSB of BTP (Battery Trip Point)
  BTPH, 8,        // MSB of BTP (Battery Trip Point)
  Offset(0xCB),
  MERL, 8,        // LSB of Measurement Accuracy  - BIX only
  MERH, 8,        // MSB of Measurement Accuracy  - BIX only
  Offset(0xCD),
  CCTL, 8,        // LSB of battery CycleCount    - BIX only
  CCTH, 8,        // MSB of battery CycleCount    - BIX only
  Offset(0xCF),   // Miscellaneous Status and Control
  STAS,8,         // Bit[1] - 1, BIOS enable AC/DC switch
                  //          0, BIOS disable AC/DC switch
                  // Bit[2] - 1, BIOS enable ACPI mode
                  //          0, BIOS disable ACPI mode 
  Offset(0xE0),   // Adaptive Sleep
  WKTR, 16,       // Wake timer value in unit of 1 munute
  S5TR, 16,       // Reserved
  AS4F, 8,        // AS4 flags
  Offset(0xE6),   // Thermal margin of Dynamic Power and Thermal Control
                  // The encoding is exactly following SB-TSI number by offset 49.
                  // Valid range is from -49.0 to 206.875.
                  // To perfrom an atomic read, host should always read the integer part first.
  MDEC,8,         // decimal part
  MINT,8,         // integer part
  Offset(0xEA),
  LUXR,16,        // Light Sensor illuminance reading in Lux
  Offset(0xEC),
  BRTL,8,         // Brightness Level, not support in Gardenia/Jadeite.
  Offset(0xED),
  BATS,8,         // Bit[0] = 1 if battery is charging or 0 for discharging
                  // Bit[7] = 1 if battery was tripped. Write 1 to clear. _Q3E will be issued unless it be cleared.
  CAPL,8,         // LSB of battery remaining capacity value returned by Smart Battery command, RemainingCapacity( ) (0x0F)
  CAPH,8,         // MSB of battery remaining capacity value returned by Smart Battery command, RemainingCapacity( )
  BTVL,8,         // LSB of battery terminal voltage value returned by Smart Battery command, Voltage( ) (0x09)
  BTVH,8,         // MSB of battery terminal voltage value returned by Smart Battery command, Voltage( )
  BTCL,8,         // LSB of battery terminal current value returned by Smart Battery command, Current( ) (0x0A)
  BTCH,8,         // MSB of battery terminal current value returned by Smart Battery command, Current( )
  DECL,8,         // LSB of DesignCapacity() (0x18) 
  DECH,8,         // MSB of DesignCapacity() 
  DEVL,8,         // LSB of DesignVoltage() (0x19)
  DEVH,8,         // MSB of DesignVoltage() 
  Offset (0xF8),
  LFCL,8,         // LSB of fully charged battery capacity value returned by Smart Battery command, FullChargeCapacity( ) (0x10)
  LFCH,8,         // MSB of fully charged battery capacity value returned by Smart Battery command, FullChargeCapacity( )
  CHSH,8          // Bit[6] = 1 if battery is connected
                  // Bit[7] = 1 if AC is connected
}
