#;*****************************************************************************
#;
#; Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
#;
#;*****************************************************************************

[Defines]

[LibraryClasses]
  #
  # Cpm specific common libraries
  #
  AmdCpmBaseLib|AmdCpmPkg/Library/Proc/Base/AmdCpm3BaseLib.inf
  AmdCpmCpuLib|AmdCpmPkg/Library/Proc/Cpu/AmdCpm3Cpu.inf
  AmdCpmFchLib|AmdCpmPkg/Library/Proc/Fch/AmdCpm3Fch.inf

[LibraryClasses.common.PEIM]
  OemAgesaCcxPlatformLib|AmdCpmPkg/Addendum/Oem/Bilby/Library/AgesaCcxPlatformLib/AgesaCcxPlatformLib.inf
  FchInitHookLibPei|AmdCpmPkg/Addendum/Oem/Bilby/Library/FchInitHookLib/FchInitHookPeiLib.inf
  AmdCalloutLib|AmdCpmPkg/Addendum/Oem/Bilby/Library/AmdCalloutLib/AmdCalloutPeiLib.inf
  AmdPbsConfigLib|AmdCpmPkg/Addendum/Oem/Bilby/Library/AmdPbsConfigLib/AmdPbsConfigPeiLib.inf
  OemGpioResetControlLib|AmdCpmPkg/Addendum/Oem/Bilby/Library/GpioResetControlLib/GpioResetControlLib.inf

[LibraryClasses.common.DXE_DRIVER]
  FchInitHookLibDxe|AmdCpmPkg/Addendum/Oem/Bilby/Library/FchInitHookLib/FchInitHookDxeLib.inf
  AmdCalloutLib|AmdCpmPkg/Addendum/Oem/Bilby/Library/AmdCalloutLib/AmdCalloutDxeLib.inf
  AmdPbsConfigLib|AmdCpmPkg/Addendum/Oem/Bilby/Library/AmdPbsConfigLib/AmdPbsConfigDxeLib.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  AmdPbsConfigLib|AmdCpmPkg/Addendum/Oem/Bilby/Library/AmdPbsConfigLib/AmdPbsConfigDxeLib.inf

[Components.IA32]
  AmdCpmPkg/Addendum/Oem/Bilby/Pei/AmdCpmOemInitPei/AmdCpm3OemInitPeim.inf
  AmdCpmPkg/Kernel/Pei/AmdCpm3InitPeim.inf
  AmdCpmPkg/Features/GpioInit/Pei/AmdCpm3GpioInitPeim.inf
  #AmdCpmPkg/Features/DisplayFeature/Pei/AmdCpm3DisplayFeaturePeim.inf
  AmdCpmPkg/Features/PcieInit/Pei/AmdCpm3PcieInitPeim.inf
  AmdCpmPkg/Addendum/Oem/Bilby/Features/I2cMaster/Pei/AmdI2cMasterPei.inf
  AmdCpmPkg/Addendum/Oem/Bilby/Devices/M24LC128/Pei/M24Lc128Pei.inf
  AmdCpmPkg/Addendum/Oem/Bilby/Devices/Pca9535a/Pei/Pca9535aPei.inf
  AmdCpmPkg/Addendum/Oem/Bilby/Devices/Pca9545a/Pei/Pca9545aPei.inf
  AmdCpmPkg/Addendum/Oem/Bilby/Pei/PlatformMemoryConfigurationPei/PlatformMemoryConfigurationPei.inf
  AmdCpmPkg/Addendum/Oem/Bilby/Pei/PlatformCustomizePei/PlatformCustomizePei.inf
  #AmdCpmPkg/Addendum/Oem/Bilby/Pei/AmdXgbeAndDisplayConfigure/AmdXgbeAndDisplayConfigure.inf
  AmdCpmPkg/Addendum/Oem/Bilby/Pei/AmdXgbeWorkaround/AmdXgbeWorkaround.inf
  
[Components.X64]
  AmdCpmPkg/Kernel/Asl/AmdCpm3InitAsl.inf
  AmdCpmPkg/Kernel/Dxe/AmdCpm3InitDxe.inf
  AmdCpmPkg/Kernel/Smm/AmdCpm3InitSmm.inf
  AmdCpmPkg/Features/AcpPowerGating/Asl/AmdCpmAcpPowerGatingAsl.inf
  AmdCpmPkg/Features/AcpPowerGating/Dxe/AmdCpmAcpPowerGatingDxe.inf
  #AmdCpmPkg/Features/DisplayFeature/Asl/DGpuD3Cold/AmdCpm3DGpuD3ColdAsl.inf
  #AmdCpmPkg/Features/DisplayFeature/Asl/DisplayFeatureI/AmdCpm3DisplayFeatureIAsl.inf
  #AmdCpmPkg/Features/DisplayFeature/Asl/DisplayFeatureI2/AmdCpm3DisplayFeatureI2Asl.inf
  #AmdCpmPkg/Features/DisplayFeature/Asl/DisplayFeatureNoI/AmdCpm3DisplayFeatureNoIAsl.inf
  #AmdCpmPkg/Features/DisplayFeature/Asl/DisplayFeatureD/AmdCpm3DisplayFeatureDAsl.inf
  #AmdCpmPkg/Features/DisplayFeature/Asl/DisplayFeatureD2/AmdCpm3DisplayFeatureD2Asl.inf
  #AmdCpmPkg/Features/DisplayFeature/Asl/MxmOverT/AmdCpm3DisplayFeatureMxmOverTAsl.inf
  #AmdCpmPkg/Features/DisplayFeature/Asl/DisplayConnectEvent/AmdCpm3DisplayFeatureDisplayConnectEventAsl.inf
  #AmdCpmPkg/Features/DisplayFeature/Dxe/AmdCpm3DisplayFeatureDxe.inf
  #AmdCpmPkg/Features/DisplayFeature/Smm/AmdCpm3DisplayFeatureSmm.inf
  #AmdCpmPkg/Features/ZeroPowerOdd/Asl/Fm15/AmdCpm3ZeroPowerOddFm15Asl.inf
  #AmdCpmPkg/Features/ZeroPowerOdd/Asl/Fm17/AmdCpm3ZeroPowerOddFm17Asl.inf
  #AmdCpmPkg/Features/ZeroPowerOdd/Dxe/AmdCpm3ZeroPowerOddDxe.inf
  AmdCpmPkg/Features/PcieInit/Asl/ExpressCard/AmdCpmExpressCardAsl.inf
  #AmdCpmPkg/Features/PcieInit/Asl/OtherHotplug/AmdCpmOtherHotplugAsl.inf
  #AmdCpmPkg/Features/PcieInit/Asl/WirelessButton/AmdCpmWirelessButtonAsl.inf
  AmdCpmPkg/Features/PcieInit/Dxe/AmdCpm3PcieInitDxe.inf
  AmdCpmPkg/Features/GpioInit/Dxe/AmdCpm3GpioInitDxe.inf
  AmdCpmPkg/Features/GpioInit/Smm/AmdCpm3GpioInitSmm.inf
  AmdCpmPkg/Addendum/Oem/Bilby/Features/I2cMaster/Dxe/AmdI2cMasterDxe.inf
  AmdCpmPkg/Addendum/Oem/Bilby/Features/xGbEI2cMaster/xGbEI2cMasterDxe.inf
  #AmdCpmPkg/Features/Ucsi/Asl/AmdCpm3UcsiAsl.inf
  #AmdCpmPkg/Features/Ucsi/Dxe/AmdCpm3UcsiDxe.inf

  AmdCpmPkg/Addendum/Oem/Bilby/Devices/Pca9535a/Dxe/Pca9535aDxe.inf
  AmdCpmPkg/Addendum/Oem/Bilby/Devices/Pca9545a/Dxe/Pca9545aDxe.inf
  AmdCpmPkg/Addendum/Oem/Bilby/Dxe/PspPlatformDriver/PspPlatform.inf
  AmdCpmPkg/Addendum/Oem/Bilby/Dxe/AmdPbsSetupDxe/AmdPbsSetupDxe.inf
  AmdCpmPkg/Addendum/Oem/Bilby/Dxe/AmdCpmOemInitDxe/AmdCpmOemInitDxe.inf
  AmdCpmPkg/Addendum/Oem/Bilby/Dxe/AmdXgbeWorkaround/AmdXgbeWorkaround.inf
  AmdCpmPkg/Addendum/Oem/Bilby/Dxe/FanPolicyDxe/FanPolicyDxe.inf
  AmdCpmPkg/Addendum/Oem/Bilby/Smm/AmdCpmOemSmm.inf

[PcdsFixedAtBuild]
  gAmdCpmPkgTokenSpaceGuid.ApuSupportMask|0x00001800  # RV==11 & RV2==12
  #gAmdCpmPkgTokenSpaceGuid.EcRamBaseAddr|0x00662
  #gAmdCpmPkgTokenSpaceGuid.EcMutexAslName|0x3930305A
  gAmdCpmPkgTokenSpaceGuid.PcdSerialIoDecode|TRUE
  gAmdCpmPkgTokenSpaceGuid.PcdBspStackSize|0x20000

  # V9 Pcds
  # IDS Debug Lib Pcds
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintEnable|TRUE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintHdtOutEnable|FALSE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintHdtOutForceEnable|FALSE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintRedirectIOEnable|FALSE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintSerialPortEnable|TRUE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintSerialPort|0x3F8
# TEST_POINT(60) MAIN_FLOW(50) FCH_TRACE(40) PSP_TRACE(31) GFX_MISC(27) NB_MISC(25) PCIE_MISC(21) GNB_TRACE(20) CPU_TRACE(10) MEM_STATUS(3)
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintFilter|0x100401008A300408
# MAIN_FLOW(50) MEMORY_TRACE_RSV6(9) MEMORY_TRACE_RSV5(8) MEMORY_TRACE_RSV4(7) MEMORY_TRACE_RSV3(6) MEM_PMU(5) MEM_UNDEF_BF(4) MEM_STATUS(3) MEM_FLOW(2) MEM_GETREG(1) MEM_SETREG(0)
# gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintFilter|0x000400000000003C

  # Customized DIMM Vendor Strings
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord0|0x2C00
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord1|0xAD00
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord2|0xC100
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord3|0xCE00
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord4|0x4F00
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord5|0x9801
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord6|0xFE02
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord7|0x0B03
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord8|0x2503
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord9|0x8303
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord10|0xCB04
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorWord11|0xC106

  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr0|"Micron Technology"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr1|"Hynix"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr2|"Infineon (Siemens)"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr3|"Samsung"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr4|"Transcend Information"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr5|"Kingston"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr6|"ELPIDA"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr7|"Nanya Technology"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr8|"Kingmax Semiconductor"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr9|"Buffalo (Formerly Melco)"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr10|"A-DATA Technology"
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosDimmVendorStr11|"ASint Technology"

  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdSmbiosSocketDesignationSocket0|"FP5"


[PcdsDynamicDefault]
  ## Customize AGESA GNB
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdCfgGnbIoapicAddress|0xFEC01000

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdCfgGnbIoapicId|0x22
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdCfgFchIoapicId|0x21

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdCfgAzaliaSsid|0xD0011022
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdIvrsRelativeAddrNamesSupport|TRUE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdPsppPolicy|0
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdPhyA0DynamicControl|0
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdPhyA1DynamicControl|0
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdPhyA2DynamicControl|0
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdPhyA34DynamicControl|0

  ## Customize AGESA FCH
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdLegacyFree|FALSE

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdHpetEnable|TRUE
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdHpetMsiDis|FALSE

##ACPI_REG_WRITE OEMFchPmioEPostInitTable[] =
##{
##  {00, 00, 0xB0, 0xAC}, // Signature
##  {SMI_BASE >> 8, FCH_SMI_Gevent8,   0, 8},  // map GEVENT8/WAKE_L to ACPI _L08
##  {SMI_BASE >> 8, FCH_SMI_xHC0Pme,   0, 25}, // map USB device 10 func0 PME to ACPI _L18
##  {SMI_BASE >> 8, FCH_SMI_xHC1Pme,   0, 25}, // map USB device 10 func1 PME to ACPI _L18
##  {SMI_BASE >> 8, FCH_SMI_Usbwakup0, 0, 24}, // map USB device 18 PME to ACPI _L18
##  {SMI_BASE >> 8, FCH_SMI_Usbwakup1, 0, 24}, // map USB device 19 PME to ACPI _L18
##  {SMI_BASE >> 8, FCH_SMI_Usbwakup2, 0, 24}, // map USB device 22 PME to ACPI _L18
##  {SMI_BASE >> 8, FCH_SMI_TWARN,     0,  1},
##  {0xFF, 0xFF, 0xFF, 0xFF},
##};
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdOemProgrammingTablePtr|{0x00, 0x00, 0xB0, 0xAC,  0x02, 0x48, 0x00, 0x08,  0x02, 0x78, 0x00, 0x19,  0x02, 0x79, 0x00, 0x19,  0x02, 0x58, 0x00, 0x18,  0x02, 0x59, 0x00, 0x18,  0x02, 0x5A, 0x00, 0x18,  0x02, 0x70, 0x00, 0x01,  0xFF, 0xFF, 0xFF, 0xFF}

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdNativePcieSupport|TRUE
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdNoneSioKbcSupport|FALSE

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataEnable|TRUE
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataClass|0x06

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdXhci0Enable|TRUE

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdLpcClockDriveStrength|0x3

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSmbusSsid|0x00000000
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataAhciSsid|0x00000000
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataRaidSsid|0x00000000
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataRaid5Ssid|0x00000000
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSataIdeSsid|0x00000000
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdXhciSsid|0x00000000
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdLpcSsid|0x00000000
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdSdSsid|0x00000000

  gEfiAmdAgesaModulePkgTokenSpaceGuid.FchRTSataDevSlpPort0S5Pin|0x18 #GPIO24
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdResetCpuOnSyncFlood|TRUE

  # Disable UMA above 4 GB
 ## gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdMemCfgUmaAbove4G|FALSE

  ## Enable Ftpm
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPspSystemTpmConfig|0x1

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdEspiEnable|TRUE
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdEspiEc0Enable|TRUE
##  gEfiAmdAgesaPkgTokenSpaceGuid.PcdEspiIoMmioDecode|{0x01, 0x4F, 0x00, 0x00, 0x01, 0x4E, 0x00, 0x00, 0x01, 0x00, 0x06, 0xFF, 0x01, 0x4C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x20, 0xEC, 0xFE, 0xFF, 0x00}
## Arista change:
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdEspiIoMmioDecode|{0x01, 0x4F, 0x00, 0x00, 0x01, 0x4E, 0x00, 0x00, 0x01, 0x00, 0x06, 0xFF, 0x01, 0x4C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0xED, 0xFE, 0xFF, 0xFF}
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdEspiIo80Enable|FALSE
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdEspiKbc6064Enable|TRUE

  # Turn off BP_X48M0 Clock Output - Mandolin doesn't use BP_X48M0 clock, it can be disabled.
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdBpX48M0OutputEnable|TRUE

  #Turn on AGESA APCB recovery mechanism
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPspApcbRecoveryEnable|TRUE

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdToggleAllPwrGoodOnCf9|TRUE
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdDmaDsXgbeEnable|FALSE
  
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdXgbeMdio0|TRUE
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdXgbeMdio1|TRUE
  # enable this once user selects 10G SFP+.
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdXgbeSfp|TRUE
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdXgbePort0ConfigEn|TRUE
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdXgbePort1ConfigEn|TRUE
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdXgbePort0Table|{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdXgbePort1Table|{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}