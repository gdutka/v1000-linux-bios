/** @file
  This file is Auto-generated by GenGloblNvs tool.
  Information passed from BIOS
;*******************************************************************************
;* Copyright (c) 2015, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/


OperationRegion(GNVS, SystemMemory, 0x55aa55aa, 0x55AA55AA)
  Field(GNVS, AnyAcc, NoLock, Preserve) {
    Offset(0),
    SMIF, 8,       // (0x00) SmiFuncCall : SMI Function Call (ASL to SMI via I/O Trap)
    PRM0, 8,       // (0x01) SmiFuncCallPram0 : SMIF - Parameter 0
    PRM1, 8,       // (0x02) SmiFuncCallPram1 : SMIF - Parameter 1
    BRTL, 8,       // (0x03) BrightnessLevel : Brightness Level
    TLST, 8,       // (0x04) DispToggleList : Display Toggle List Selection
    IGDS, 8,       // (0x05) IgdStatus : IGD State (Primary Display = 1)
    LCDA, 16,      // (0x06) LcdAddr : LCD Address (Default = 0x400)
    CSTE, 16,      // (0x08) CurrentDispState : Current Display State
    NSTE, 16,      // (0x0A) NextDispState : Next Display State
    CADL, 16,      // (0x0C) CurrentAttachedDevList : Current Attached Device List
    PADL, 16,      // (0x0E) PreAttachedDevList : Previous Attached Device List
    LIDS, 8,       // (0x10) LidStatus : Lid State (Lid Open = 1)
    PWRS, 8,       // (0x11) PowerModeStatus : Power State (AC Mode = 1)
    BVAL, 32,      // (0x12) BatteryValue : 32-bit Battery Value returned in _BST
    ADDL, 16,      // (0x16) AvailDispDevList : Available Display Device List
    BCMD, 8,       // (0x18) SmiCommand : SMI command
    SBFN, 8,       // (0x19) SmiCmdSubFunc : Sub-function for SMI command
    DID,  32,      // (0x1A) FuncIdForAtif : Function ID for ATIF
    INFO, 2048,    // (0x1E) AtifInfo : Information buffer for ATIF
    TOML, 8,       // (0x11E) TopOfMemLo : Low byte of Top of memory register
    TOMH, 8,       // (0x11F) TopOfMemHi : High byte of Top of memory register
    CEBP, 8,       // (0x120) EnabledCpuBitMap : A bit map to indicate which CPU has support PowerNow
    C0LS, 8,       // (0x121) Cpu0LowestState : The Lowest P-state number of CPU0
    C1LS, 8,       // (0x122) Cpu1LowestState : The Lowest P-state number of CPU1
    C0HS, 8,       // (0x123) Cpu0HighestState : The Highest P-state number of CPU0
    C1HS, 8,       // (0x124) Cpu1HighestState : The Highest P-state number of CPU1
    ROMS, 32,      // (0x125) RomSize : Rom Size
    MUXF, 8,       // (0x129) MuxFlag : Mux Flag
    PDDN, 8,       // (0x12A) PriDispDevNum : Primary display device number
    CNSB, 8,       // (0x12B) ConnectStandbySupport : Connect Standby support
    RDHW, 8,       // (0x12C) AcpiReduceHwSupport : Reduced Hardware Support
    DAS3, 8,       // (0x12D) DisableAcpiS3
    TNBH, 8,       // (0x12E) amdTNBHystersis
    TCP0, 8,       // (0x12F) amdTCPU0
    TCP1, 8,       // (0x130) amdTCPU1
    ATNB, 8,       // (0x131) amdTNB
    PCP0, 8,       // (0x132) amdPwmCPU0
    PCP1, 8,       // (0x133) amdPwmCPU1
    PWMN, 8,       // (0x134) amdPwmNb
    LPTY, 8,       // (0x135) amdLcdPanelType
    M92D, 8,       // (0x136) M92Disable
    WKPM, 8,       // (0x137) WakeOnPME
    ALST, 8,       // (0x138) LightSensor
    AFUC, 8,       // (0x139) amdFusionUtilityControl
    EXUS, 8,       // (0x13A) amdExternalUSBController
    GV0E, 8,       // (0x13B) GlobalNvsData0E
    WLSH, 8,       // (0x13C) amdWirelessSwitchSetting
    TSSS, 8,       // (0x13D) amdThermalSensorSelect
    AOZP, 8,       // (0x13E) ZeroPowerOddEn
    TZFG, 8,       // (0x13F) AcpiThermalZone
    BPS0, 8,       // (0x140) amdSystemBootWithPS0
    NAPC, 8,       // (0x141) NbIoApic
    PCBA, 32,      // (0x142) PcieBaseAddress
    PCBL, 32,      // (0x143) PcieBaseLimit
    WLAN, 8,       // (0x144) AmdWlanWwan
    BLTH, 8,       // (0x145) AmdBlueTooth
    GPSS, 8,       // (0x146) AmdGpsSensor
    NFCS, 8,       // (0x147) AmdNfcSensor
    SBTY, 8,       // (0x148) amdSecondaryBatterySupport
    BDID, 16,      // (0x149) BoardId
    MWTT, 8,       // (0x150) MITTWITTControl
    ACPM, 8,       // (0x151) AcpMode
    KBCS, 8,       // (0x152) KbcSupport
    ACEC, 8,       // (0x153) EcSupport
    DPTC, 8,       // (0x154) AmdDptcControl
//>>EnableAbove4GBMmio++
    MM64, 8,       // (0x154) Enable/Disable 64bits MMIO support
    HMB1, 64,      // (0x155) 64 bits MMIO base address
    HMB2, 64,      // (0x163) 64 bits MMIO base address
    HMM1, 64,      // (0x171) 64 bits MMIO limit
    HMM2, 64,      // (0x179) 64 bits MMIO limit
    HML1, 64,      // (0x187) 64 bits MMIO length
    HML2, 64,      // (0x195) 64 bits MMIO length
//<<EnableAbove4GBMmio++
  }


