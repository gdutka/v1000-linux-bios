## @file
#  Platform Package Description file
#
#******************************************************************************
#* Copyright (c) 2012 - 2018, Insyde Software Corp. All Rights Reserved.
#*
#* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#* transmit, broadcast, present, recite, release, license or otherwise exploit
#* any part of this publication in any form, by any means, without the prior
#* written permission of Insyde Software Corporation.
#*
#******************************************************************************

!import MdePkg/Package.dsc
!import MdeModulePkg/Package.dsc
!import UefiCpuPkg/Package.dsc
# !import PerformancePkg/Package.dsc
#!import CryptoPkg/Package.dsc
!import FatPkg/Package.dsc
!import ShellPkg/Package.dsc
!import SecurityPkg/Package.dsc
!import PcAtChipsetPkg/Package.dsc
!import AMD64ArchFamilyCpuPkg/Package.dsc
# !import EdkCompatibilityPkg/Package.dsc
!import InsydeCrPkg/Package.dsc
!import InsydeOemServicesPkg/Package.dsc
!import InsydeModulePkg/Package.dsc
!import SioDummyPkg/Package.dsc
!import InsydeSetupPkg/Package.dsc
!import InsydeNetworkPkg/Package.dsc
!import InsydeFlashDevicePkg/Package.dsc
!import AmdCpmPkg/Addendum/Oem/Bilby/AmdCpmPkg.inc.dsc
!import AmdPspFeaturePkg/Package.dsc
!import AmdCommonChipsetPkg/Package.dsc
!import PicassoChipsetPkg/Package.dsc
!import SegFeaturePkg/Package.dsc
!import InsydeH2OUvePkg/Package.dsc
#[-start-240426-IB18160212-add-]#
!import InsydePostCodePkg/Package.dsc
#[-end-240426-IB18160212-add-]#

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = Bilby
  PLATFORM_GUID                  = 5b89d367-764c-4c4d-8a8e-573da8590f1e
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/$(PROJECT_PKG)
  SUPPORTED_ARCHITECTURES        = IA32|X64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = Build/$(PROJECT_PKG)/Project.fdf
  CRB_NAME                       = Bilby

  !include $(PROJECT_PKG)/Project.env
  DEFINE SECURE_FLASH_CERTIFICATE_FILE_PATH = $(PROJECT_PKG)/FactoryCopyInfo/SecureFlash.cer

  !if $(PROMONTORY_SUPPORT) == YES
    DEFINE CC_FLAGS              = $(CC_FLAGS) /D PROMONTORY_SUPPORT
    DEFINE ASM_FLAGS             = $(ASM_FLAGS) /D PROMONTORY_SUPPORT
    DEFINE VFR_FLAGS             = $(VFR_FLAGS) /D PROMONTORY_SUPPORT
  !endif

  !if $(CONNECTED_STANDBY_SUPPORT) == YES
    DEFINE CC_FLAGS              = $(CC_FLAGS) -DCONNECTED_STANDBY_SUPPORT
  !endif

  !if $(REDUCED_HARDWARE_SUPPORT) == YES
    DEFINE CC_FLAGS              = $(CC_FLAGS) -DREDUCED_HARDWARE_SUPPORT
  !endif

  EDK_GLOBAL  CSM_VERSION                             = 096

## DEFINE for AmdCpmPkg with oem board support
  DEFINE CPM_PKG_PATH     = AmdCpmPkg/Addendum/Oem/$(PLATFORM_NAME)
  DEFINE CPM_PKG_DEC      = AmdCpmOemPkg

################################################################################
#
# SKU Identification section - list of all SKU IDs supported by this Platform.
#
################################################################################
[SkuIds]
  0|DEFAULT

################################################################################
#
# Include AGESA V9 DSC file
#
################################################################################
!include AgesaModulePkg/AgesaFp5RvModulePkg.inc.dsc
!include AmdCbsPkg/Library/Family/0x17/RV/External/CbsRavenFP5.inc.dsc

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform.
#
################################################################################
[PcdsFeatureFlag]
  gInsydeTokenSpaceGuid.PcdH2ODdtSupported|$(INSYDE_DEBUGGER)
!if $(EFI_DEBUG) == YES
  gInsydeTokenSpaceGuid.PcdStatusCodeUseDdt|$(INSYDE_DEBUGGER)
  gInsydeTokenSpaceGuid.PcdStatusCodeUseUsb|$(USB_DEBUG_SUPPORT)
!if $(USB_DEBUG_SUPPORT) == NO
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseSerial|TRUE
!else
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseSerial|FALSE
!endif

!if $(INSYDE_DEBUGGER) == YES and $(H2O_DDT_DEBUG_IO) == Com
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseSerial|FALSE
!endif
!endif
#[-start-181113-IB06070004-modify]#
!if $(INSYDE_DEBUGGER) == YES and $(H2O_DDT_DEBUG_IO) == Com
  gInsydeCrTokenSpaceGuid.PcdH2OConsoleRedirectionSupported|FALSE
!endif
#[-end-181113-IB06070004-modify]#
  gInsydeTokenSpaceGuid.PcdFrontPageSupported|$(FRONTPAGE_SUPPORT)
  gInsydeTokenSpaceGuid.PcdCrisisRecoverySupported|$(CRISIS_RECOVERY_SUPPORT)
  gInsydeTokenSpaceGuid.PcdUseFastCrisisRecovery|$(USE_FAST_CRISIS_RECOVERY)
  gInsydeTokenSpaceGuid.PcdSecureFlashSupported|$(SECURE_FLASH_SUPPORT)
  gInsydeTokenSpaceGuid.PcdUnsignedFvSupported|$(UNSIGNED_FV_SUPPORT)
  gInsydeTokenSpaceGuid.PcdH2ONetworkSupported|FALSE
  gInsydeTokenSpaceGuid.PcdH2ONetworkIpv6Supported|FALSE
  gInsydeTokenSpaceGuid.PcdH2ONetworkIscsiSupported|FALSE
  gInsydeTokenSpaceGuid.PcdH2OTpmSupported|FALSE
  gInsydeTokenSpaceGuid.PcdH2OTpm2Supported|FALSE
  gAmdPspFeaturePkgTokenSpaceGuid.PcdPspFirmwareTpmSupported|FALSE
  gInsydeTokenSpaceGuid.PcdSysPasswordInCmos|$(SYS_PASSWORD_IN_CMOS)
  gInsydeTokenSpaceGuid.PcdSysPasswordSupportUserPswd|$(SUPPORT_USER_PASSWORD)
  gInsydeTokenSpaceGuid.PcdH2OHddPasswordSupported|FALSE
  gInsydeTokenSpaceGuid.PcdReturnDialogCycle|$(RETURN_DIALOG_CYCLE)
  gInsydeTokenSpaceGuid.PcdFvbAccessThroughSmi|TRUE
  gInsydeTokenSpaceGuid.PcdH2OUsbSupported|TRUE
  gInsydeTokenSpaceGuid.PcdH2OAhciSupported|$(SEAMLESS_AHCI_SUPPORT)
#[-start-210412-IB06070105-modify]#
  gInsydeTokenSpaceGuid.PcdH2OIdeSupported|FALSE
#[-end-210412-IB06070105-modify]#
#[-start-190225-IB06070011-modify]#
  gInsydeTokenSpaceGuid.PcdH2OSdhcSupported|TRUE
#[-end-190225-IB06070011-modify]#
  gInsydeTokenSpaceGuid.PcdSnapScreenSupported|$(SNAPSCREEN_SUPPORT)
  gInsydeTokenSpaceGuid.PcdTextModeFullScreenSupport|$(TEXT_MODE_FULL_SCREEN_SUPPORT)
  gInsydeTokenSpaceGuid.PcdUefiPauseKeyFunctionSupport|$(UEFI_PAUSE_KEY_FUNCTION_SUPPORT)
  gInsydeTokenSpaceGuid.PcdOnlyUsePrimaryMonitorToDisplay|$(ONLY_USE_PRIMARY_MONITOR_TO_DISPLAY)
  gInsydeTokenSpaceGuid.PcdDynamicHotKeySupported|$(DYNAMIC_HOTKEY_SUPPORT)
  gInsydeTokenSpaceGuid.PcdH2OFormBrowserLocalTextDESupported|TRUE
  gInsydeTokenSpaceGuid.PcdH2OFormBrowserLocalMetroDESupported|FALSE

!errif (gInsydeTokenSpaceGuid.PcdH2OFormBrowserLocalTextDESupported == FALSE) and (gInsydeTokenSpaceGuid.PcdH2OFormBrowserLocalMetroDESupported == FALSE), "Must have at least one display engine enabled in Project.dsc"

  gInsydeTokenSpaceGuid.PcdBvdtGenBiosBuildTimeSupported|$(BUILD_TIME_FLAG)
  gPlatformPkgTokenSpaceGuid.PcdRtkUsbLanSupport|$(RTK_USBLAN_SUPPORT)
  gInsydeTokenSpaceGuid.PcdShellBinSupported|TRUE
  gInsydeTokenSpaceGuid.PcdShellBuildSupported|FALSE

  gChipsetPkgTokenSpaceGuid.PcdFchAvalonSupport|$(FCH_AVALON_SUPPORT)
  gAmdCommonChipsetPkgTokenSpaceGuid.PcdMemoryContextSaveRestoreSupport|$(MEMORY_CONTEXT_SAVE_RESTORE_SUPPORT)

  gChipsetPkgTokenSpaceGuid.PcdLegacyFreeSupported|$(LEGACY_FREE_SUPPORT)

  gAmdCpmPkgTokenSpaceGuid.PcdReduceHardwareSupport|$(REDUCED_HARDWARE_SUPPORT)
  gChipsetPkgTokenSpaceGuid.PcdNbIoApicSupport|$(NB_IOAPIC_SUPPORT)
  gAmdCpmPkgTokenSpaceGuid.PcdModernStandbySupport|$(CONNECTED_STANDBY_SUPPORT)
  gChipsetPkgTokenSpaceGuid.PcdCrbOnlySupport|$(CRB_ONLY)
  gChipsetPkgTokenSpaceGuid.PcdXhciSupported|$(XHCI_SUPPORT)

  # For SimNow
  gChipsetPkgTokenSpaceGuid.PcdSpiMmioAccessEnable|FALSE

  #
  # SPI TPM support
  #
  gChipsetPkgTokenSpaceGuid.PcdSpiTpm|TRUE

  #
  # BVM Support
  #
  gPlatformPkgTokenSpaceGuid.PcdBvmSupport|$(BVM_SUPPORT)

  #
  # LBR setting
  #
  gEfiCpuTokenSpaceGuid.PcdCpuLbrFlag|FALSE

  #
  # Disable CSM for Raven emulation
  #

  #
  # Recovery long run test support
  #
  gChipsetPkgTokenSpaceGuid.PcdBdsReoveryLongRun|FALSE

  gH2OFlashDeviceEnableGuid.PcdSst25lf040SpiEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdSst25lf040aSpiEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdSst25lf080aSpiEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdSst25vf080bSpiEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdSst25vf016bSpiEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdAtmel25df041SpiEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdAtmel25df081aSpiEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdAtmel26df161SpiEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdAtmel26df321SpiEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdWinbondW25q64bvSpiEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdWinbondW25q128bvSpiEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdAtmel25df641SpiEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdMxic25l8005SpiEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdMxic25l1605aSpiEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdMxic25l6405dSpiEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdMxic25l12805dSpiEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdMxic25l25635eSpiEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdStM25pe80SpiEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdPmc25lv040SpiEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdGd25lq16SpiEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdGd25lq64cSpiEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdSst49lf008aLpcEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdSst39vf080LpcEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdIntel82802acLpcEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdSst49lf004aLpcEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdPmc49fl004tLpcEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdWinbond39v040fpaLpcEnable|FALSE
  gH2OFlashDeviceEnableGuid.PcdSt50flw040aLpcEnable|FALSE
!if $(EMULATION_ONLY) == YES
  gPlatformPkgTokenSpaceGuid.PcdEmulationOnly|TRUE
!endif
  gAmdPspFeaturePkgTokenSpaceGuid.PcdPspComboSupport|$(PSP_COMBO_SUPPORT)

  gInsydeTokenSpaceGuid.PcdH2OI2cSupported|FALSE
#>>EnableAbove4GBMmio++
  gInsydeTokenSpaceGuid.Pcd64BitAmlSupported|TRUE
#<<EnableAbove4GBMmio++

  gInsydeTokenSpaceGuid.PcdH2OBdsOemBadgingSupported|TRUE

#[-start-181221-IB09860035-add]#
  gChipsetPkgTokenSpaceGuid.PcdH2OGpioCfgSupported|FALSE
#[-end-181221-IB09860035-add]#

#[-start-200528-IB06070090-add]#
  gInsydeTokenSpaceGuid.PcdH2OCustomDefaultSupported|TRUE
#[-end-200528-IB06070090-add]#

#[-start-231225-IB18160180-modify]#
#[-start-210609-IB18160135-add]#
  # Set PcdSecureSysPasswordSupported to TRUE and PcdSysPasswordInCmos to FALSE to use Insyde secure variable
  gInsydeTokenSpaceGuid.PcdSysPasswordInCmos|FALSE
  gInsydeTokenSpaceGuid.PcdSecureSysPasswordSupported|TRUE
#[-end-210609-IB18160135-add]#
#[-end-231225-IB18160180-modify]#

[PcdsDynamicHii]

[PcdsFixedAtBuild]
!if $(EFI_DEBUG) == YES
  gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x07
!else
  gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask|0x01
!endif
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintEnable|$(EFI_DEBUG)
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintSerialPortEnable|$(EFI_DEBUG)
  # PcdSerialPortSelect   # 0 = FCH UART 0
                          # 1 = FCH UART 1
                          # 2 = Simnow ITE8712 Sio Serial Port
                          # 3 = Emulation Port 0x80 out
                          # 4 = Simnow console input from Simnow ITE8712 SIO serial port, and Console output to Simnow Console Output
                          # 5 = SMSC Sio Serial Port
                          # 6 = Simnow SMSC 1100 Sio Serial Port
  !if $(SIMNOW_SERIAL_PORT_ITE) == YES
    gPlatformPkgTokenSpaceGuid.PcdSerialPortSelect|2
  !elseif $(SIMNOW_SERIAL_PORT_SMSC) == YES
    gPlatformPkgTokenSpaceGuid.PcdSerialPortSelect|6
  !elseif $(REDIRECT_SERIAL_MESSAGE_TO_PORT_80) == YES
    gPlatformPkgTokenSpaceGuid.PcdSerialPortSelect|3
  !else
    gPlatformPkgTokenSpaceGuid.PcdSerialPortSelect|0
  !endif

  !if gPlatformPkgTokenSpaceGuid.PcdSerialPortSelect == 0
    gAmdCpmPkgTokenSpaceGuid.PcdFchUartPort|0
    gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|0xFEDC9000
    gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintSerialPort|0xFEDC9000
    gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintSerialPortDetectCableConnection|FALSE
  !elseif gPlatformPkgTokenSpaceGuid.PcdSerialPortSelect == 1
    gAmdCpmPkgTokenSpaceGuid.PcdFchUartPort|1
    gEfiMdeModulePkgTokenSpaceGuid.PcdSerialRegisterBase|0xFEDCA000
    gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintSerialPort|0xFEDCA000
    gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintSerialPortDetectCableConnection|FALSE
  !else
    gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintSerialPort|0x3F8
  !endif

#[-start-231113-IB19090063-add]#
  #
  # PchHsUart driver Byte Address Control (PCICFGCTRL)
  # If platform didn't support this flag to control AccessMode.
  # Set PCD = 1  (Access8bit)
  # Set PCD = 0  (Access32bit)
  #
  gInsydeCrTokenSpaceGuid.PcdH2OCrSerialIoPchPcrAddress|0
#[-end-231113-IB19090063-add]#
#[-start-181112-IB06070003-modify]#
  gInsydeCrTokenSpaceGuid.PcdH2OCrDevice|{  \
    UINT8(0x01), L"UART0"         , L"VenHw(E76FD4E9-0A30-4CA9-9540-D799534CC4FF,0090DCFE00000000)/Uart(9600,8,N,1)",  \
    UINT8(0xFF), UINT8(0xFF)      , UINT8(0xFF) \
    }
  gInsydeCrTokenSpaceGuid.PcdH2OCrPolicy| { \
                        #    Flag Name          |  FALSE(0x00) |   TRUE(0x01)
                        #----------------------------------------------------
    UINT8(0x00),     \  # CRVideoType           |  Color       |  Mono
    UINT8(0x00),     \  # CRTerminalKey         |  Senseless   |  Sensitive
    UINT8(0x01),     \  # CRComboKey            |  Disable     |  Enable
    UINT8(0x00),     \  # Reserve (compatiable) |              |
    UINT8(0x00),     \  # Reserve (compatiable) |              |
    UINT8(0x01),     \  # CRUnKbcSupport        |  Disable     |  Enable
    UINT8(0x00),     \  # Reserve (compatiable) |              |
    UINT8(0x01),     \  # CRManualRefresh       |  Disable     |  Enable
    UINT8(0x01),     \  # CRTerminalCharSet     |  ASCII       |  Graphic
    UINT8(0x01),     \  # CRAsyncTerm           |  Auto        |  Force Async
    UINT8(0x00),     \  # CRScreenOffCheck      |  Disable     |  Enable
    UINT8(0x01),     \  # CRShowHelp            |  Disable     |  Enable
    UINT8(0x00),     \  # CRHeadlessVBuffer     |  Vbuffer     |  EBDA
    UINT8(0x00),     \  # Reserve (compatiable) |              |
    UINT8(0x00),     \  # Reserve (compatiable) |              |
    UINT8(0x00),     \  # Reserve (compatiable) |              |
    UINT8(0x00),     \  # CROpROMLoadOnESegment |  Disable     |  Enable
    UINT8(0xFF),     \  # End of feature flag (0xFF) add new feature flag need before this column.
                        #----------------------------------------------------
    UINT8(0x10),     \  # CRFifoLength          |  [Value]
    UINT8(0x00)}        # CRWriteCharInterval   |  [Value]
  gInsydeCrTokenSpaceGuid.PcdH2OCrHsUartSerialClock|0x2DB4000
  gInsydeCrTokenSpaceGuid.PcdH2OCrPciHsUartDeviceList|{UINT32(0xFF), UINT32(0xFF), UINT32(0xFF), UINT32(0xFFFF), UINT32(0xFFFF)} # All 0xFF indicates end of list.
  gInsydeCrTokenSpaceGuid.PcdH2OCrPchHsUartMmioAddress|{ UINT8(0x01), UINT32(0xFEDC9000), \
                                                       UINT8(0xFF), UINT32(0xFFFFFFFF)} # All 0xFF indicates end of list.
#[-end-181112-IB06070003-modify]#

  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultBaudRate|9600   #Arista change default baud rate to 9600#
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialBaudRate|9600  #Arista change efi debug messages baud rate to 9600#
  gEfiMdeModulePkgTokenSpaceGuid.PcdSerialFifoControl|0x00 #Arista change disable fifo control
 
  gAmdCpmPkgTokenSpaceGuid.PlatformIdOverride|0x0000
#[-start-200811-IB06070097-remove]#
#  gEfiMdeModulePkgTokenSpaceGuid.PcdRealTimeClockUpdateTimeout | 0x10
#[-end-200811-IB06070097-remove]#
!if $(AMD_APP_INSTEAD_EFI_SHELL_SUPPORT) == YES
  ## FFS filename to find the application. EmuLinuxLoader = F94964FD-41EC-FE97-28FB-F08C57C139CC
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdShellFile|{ 0xFD, 0x64, 0x49, 0xF9, 0xEC, 0x41, 0x97, 0xFE, 0x28, 0xFB, 0xF0, 0x8C, 0x57, 0xC1, 0x39, 0xCC }
!endif

  # Make sure both Pcds are set to the same value
  gUefiCpuPkgTokenSpaceGuid.PcdCpuMaxLogicalProcessorNumber|32
  gEfiCpuTokenSpaceGuid.PcdCpuMaxLogicalProcessorNumber|32

  gEfiCpuTokenSpaceGuid.PcdCpuSmmApSyncTimeout|4000
  gEfiMdePkgTokenSpaceGuid.PcdSpinLockTimeout|0
  gEfiCpuTokenSpaceGuid.PcdPeiTemporaryRamStackSize|0x6000
!if $(EMULATION_ONLY) == YES
  gAmdCommonChipsetPkgTokenSpaceGuid.PcdMaxDimms|1
!endif

!if $(FIRMWARE_PERFORMANCE) == YES
  gEfiMdePkgTokenSpaceGuid.PcdPerformanceLibraryPropertyMask|1
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxPeiPerformanceLogEntries|128
!else
  gEfiMdePkgTokenSpaceGuid.PcdPerformanceLibraryPropertyMask|0
!endif

  ## Override for SMBIOS 3.1.1 (Need remove after kernel 05.22.30)
  ## Smbios version
  gEfiMdeModulePkgTokenSpaceGuid.PcdSmbiosVersion|0x0301
  gEfiMdeModulePkgTokenSpaceGuid.PcdSmbiosDocRev|0x1


  #
  # ACPI 64Bit AML
  #
!if $(SUPPORT_64BITS_AML) == YES
  gChipsetPkgTokenSpaceGuid.PcdDsdtRevision|0x02
!else
  gChipsetPkgTokenSpaceGuid.PcdDsdtRevision|0x01
!endif

  #Bilby SLT BIOS Override.
#!if $(SLT_BIOS_SUPPORT) == YES
#  gAmdCpmPkgTokenSpaceGuid.PcdSimpleSltSupport|0x1
#!endif

  #
  # Hot key Configuration
  # Platform Hot key Define
  # ScanCode, ShiftKey, AltKey, CtrlKey
  # ex:
  #    0x54, 0x0, 0x1, 0x0      F1(Combination Key ScanCode) + ShiftKey
  #    0x68, 0x0, 0x2, 0x0      F1(Combination Key ScanCode) + AltKey
  #    0x5f, 0x0, 0x4, 0x0      F1(Combination Key ScanCode) + CtrlKey
  #
  gInsydeTokenSpaceGuid.PcdPlatformKeyList|{ \
    0x3b, 0x0, 0x0, 0x0,                     \ # F1_KEY
    0x3c, 0x0, 0x0, 0x0,                     \ # F2_KEY
    0x53, 0x0, 0x0, 0x0,                     \ # DEL_KEY
    0x44, 0x0, 0x0, 0x0,                     \ # F10_KEY
    0x86, 0x0, 0x0, 0x0,                     \ # F12_KEY
    0x01, 0x0, 0x0, 0x0,                     \ # ESC_KEY
    0x40, 0x0, 0x0, 0x0,                     \ # UP_ARROW_KEY_BIT
    0x3d, 0x0, 0x0, 0x0,                     \ # F3_KEY
    0x43, 0x0, 0x0, 0x0,                     \ # F9_KEY
    0x00, 0x0, 0x0, 0x0}                       # EndEntry

  #
  # Note: If change sizeof(SYSTEM_CONFIGURATION) in SetupConfig.h, must update really structure size in here!!!
  #
  gChipsetPkgTokenSpaceGuid.PcdSetupConfigSize|700

  gInsydeTokenSpaceGuid.PcdScuFormsetGuidList|{ \
    GUID("C1E0B01A-607E-4B75-B8BB-0631ECFAACF2"), \ # Main
    GUID("C6D4769E-7F48-4D2A-98E9-87ADCCF35CCC"), \ # Avance
    GUID("5204F764-DF25-48A2-B337-9EC122B85E0D"), \ # Security
    GUID("A6712873-925F-46C6-90B4-A40F86A0917B"), \ # Power
    GUID("2D068309-12AC-45AB-9600-9187513CCDD8"), \ # Boot
    GUID("B863B959-0EC6-4033-99C1-8FD89F040222"), \ # AmdPbs
    GUID("B04535E3-3004-4946-9EB7-149428983053"), \ # AmdCbs
    GUID("B6936426-FB04-4A7B-AA51-FD49397CDC01"), \ # Exit
    GUID("00000000-0000-0000-0000-000000000000")}

  gInsydeTokenSpaceGuid.PcdScuFormsetFlagList|{ \
    UINT8(0), \ # Main
    UINT8(0), \ # Avance
    UINT8(0), \ # Security
    UINT8(0), \ # Power
    UINT8(0), \ # Boot
    UINT8(0), \ # AmdPbs
    UINT8(0), \ # AmdCbs
    UINT8(0), \ # Exit
    UINT8(0xFF)}

  #
  # SEC Padding for EC boot image descriptor
  # which base is addressed as 0xFFFF.FF00
  #
  gEfiCpuTokenSpaceGuid.PcdSecPad1Raw32|0x00021000
  gEfiCpuTokenSpaceGuid.PcdSecPad2Raw32|0xFFFDEFFF

  #
  # SMbus Address
  #
  gInsydeTokenSpaceGuid.PcdSmbusAddrChA1|0xA0
  gInsydeTokenSpaceGuid.PcdSmbusAddrChA2|0xA4
  gInsydeTokenSpaceGuid.PcdSmbusAddrChB1|0xA2
  gInsydeTokenSpaceGuid.PcdSmbusAddrChB2|0xA6
  #
  # Debug Masks
  #
  # //
  # // Declare bits for PcdDebugPropertyMask
  # //
  # DEBUG_PROPERTY_DEBUG_ASSERT_ENABLED       0x01
  # DEBUG_PROPERTY_DEBUG_PRINT_ENABLED        0x02
  # DEBUG_PROPERTY_DEBUG_CODE_ENABLED         0x04
  # DEBUG_PROPERTY_CLEAR_MEMORY_ENABLED       0x08
  # DEBUG_PROPERTY_ASSERT_BREAKPOINT_ENABLED  0x10
  # DEBUG_PROPERTY_ASSERT_DEADLOOP_ENABLED    0x20

  # //
  # // Declare bits for PcdFixedDebugPrintErrorLevel and the ErrorLevel parameter of DebugPrint()
  # //
  # DEBUG_INIT      0x00000001  // Initialization
  # DEBUG_WARN      0x00000002  // Warnings
  # DEBUG_LOAD      0x00000004  // Load events
  # DEBUG_FS        0x00000008  // EFI File system
  # DEBUG_POOL      0x00000010  // Alloc & Free's
  # DEBUG_PAGE      0x00000020  // Alloc & Free's
  # DEBUG_INFO      0x00000040  // Informational debug messages
  # DEBUG_DISPATCH  0x00000080  // PEI/DXE/SMM Dispatchers
  # DEBUG_VARIABLE  0x00000100  // Variable
  # DEBUG_BM        0x00000400  // Boot Manager
  # DEBUG_BLKIO     0x00001000  // BlkIo Driver
  # DEBUG_NET       0x00004000  // SNI Driver
  # DEBUG_UNDI      0x00010000  // UNDI Driver
  # DEBUG_LOADFILE  0x00020000  // UNDI Driver
  # DEBUG_EVENT     0x00080000  // Event messages
  # DEBUG_GCD       0x00100000  // Global Coherency Database changes
  # DEBUG_CACHE     0x00200000  // Memory range cachability changes
  # DEBUG_VERBOSE   0x00400000  // Detailed debug messages that may significantly impact boot performance
  # DEBUG_ERROR     0x80000000  // Error
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x07
  gEfiMdePkgTokenSpaceGuid.PcdFixedDebugPrintErrorLevel|0x8000004F
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x8000004F

  #
  # SSID
  #
  gAmdCommonChipsetPkgTokenSpaceGuid.PcdAzaliaSsid|0xC7511022

  gInsydeTokenSpaceGuid.PcdI2cControllerTable|{  \# The definition of I2C host controller number lookup table
    UINT64(0x00000000FEDC4000),                  \# [0] I2C2 (I2CC) controller is located on 0xFEDC4000
    UINT64(0x00000000FEDC5000),                  \# [1] I2C3 (I2CD) controller is located on 0xFEDC5000
    UINT64(0x00000000FEDC6000),                  \# [2] I2C4 (I2CE) controller is located on 0xFEDC5000
    UINT64(0x0000000000000000)                   \# End of table
  }

  gInsydeTokenSpaceGuid.PcdI2cBusSpeedTable|{    \# The definition of I2C bus configuration lookup table
    UINT16(100000),                              \# Number 0 stands for 100 Khz
    UINT16(400000),                              \# Number 0 stands for 400 khz
    UINT16(1000000),                             \# Number 0 stands for 1  Mhz
    UINT64(00)                                   \# End of table
  }

  # V9 Pcds
  # IDS Debug Lib Pcds
#  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintEnable|FALSE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintHdtOutEnable|FALSE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintHdtOutForceEnable|TRUE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintRedirectIOEnable|FALSE
#  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintSerialPortEnable|FALSE
#  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintSerialPort|0x3F8

#Register Ihisi sub function table list.
#Table struct define {CmdNumber, FuncSignature, Priority}
# UINT8(CmdNumber), Char8[20](FuncSignature), UINT8(Priority)
##================  ========================  ===============
gInsydeTokenSpaceGuid.PcdIhisiRegisterTable|{ \
  # Register IHISI AH=00h (VATSRead)
  UINT8(0x00),      "S00Kn_VatsRead00000",    UINT8(0x80), \

  # Register IHISI AH=01h (VATSWrite)
  UINT8(0x01),      "S01Kn_VatsWrite0000",    UINT8(0x80), \

  # Register IHISI AH=05h (VATSNext)
  UINT8(0x05),      "S05Kn_VatsGetNext00",    UINT8(0x80), \

  # Register IHISI AH=10h (FBTSGetSupportVersion)
  UINT8(0x10),      "S10Cs_GetPermission",    UINT8(0xE0), \
  UINT8(0x10),      "S10OemGetPermission",    UINT8(0xC0), \
  UINT8(0x10),      "S10OemGetAcStatus00",    UINT8(0xBB), \
  UINT8(0x10),      "S10OemBatterylife00",    UINT8(0xB6), \
  UINT8(0x10),      "S10Kn_GetVersion000",    UINT8(0x80), \
  UINT8(0x10),      "S10Kn_InitOemHelp00",    UINT8(0x7F), \
  UINT8(0x10),      "S10Kn_GetVendorID00",    UINT8(0x7E), \
  UINT8(0x10),      "S10Kn_GetBatteryLow",    UINT8(0x7D), \

  # Register IHISI AH=11h (FBTSGetPlatformInfo)
  UINT8(0x11),      "S11Kn_GetModelName0",    UINT8(0x80), \
  UINT8(0x11),      "S11Kn_GModelVersion",    UINT8(0x7F), \
  UINT8(0x11),      "S11OemFbtsApCheck00",    UINT8(0x40), \
  UINT8(0x11),      "S11Kn_UpExtPlatform",    UINT8(0x20), \

  # Register IHISI AH=12h (FBTSGetPlatformRomMap)
  UINT8(0x12),      "S12Kn_ProtectRomMap",    UINT8(0x80), \
  UINT8(0x12),      "S12Kn_PrivateRomMap",    UINT8(0x7F), \
  UINT8(0x12),      "S12Cs_PlatformRomMp",    UINT8(0x40), \
  UINT8(0x12),      "S12OemPlatformRomMp",    UINT8(0x20), \

  # Register IHISI AH=13h (FBTSGetFlashPartInfo)
  UINT8(0x13),      "S13Kn_FlashPartInfo",    UINT8(0x80), \

  # Register IHISI AH=14h (FBTSRead)
  UINT8(0x14),      "S14Cs_DoBeforeRead0",    UINT8(0xE0), \
  UINT8(0x14),      "S14Kn_FbtsReadProce",    UINT8(0x80), \
  UINT8(0x14),      "S14Cs_DoAfterRead00",    UINT8(0x20), \

  # Register IHISI AH=15h (FBTSWrite)
  UINT8(0x15),      "S15Cs_DoBeforeWrite",    UINT8(0xE0), \
  UINT8(0x15),      "S15Kn_FbtsWriteProc",    UINT8(0x80), \
  UINT8(0x15),      "S15Cs_DoAfterWrite0",    UINT8(0x40), \

  # Register IHISI AH=16h (FBTSComplete)
  UINT8(0x16),      "S16Cs_CApTerminalte",    UINT8(0xE0), \
  UINT8(0x16),      "S16Cs_CNormalFlash0",    UINT8(0xDF), \
  UINT8(0x16),      "S16Cs_CPartialFlash",    UINT8(0xDE), \
  UINT8(0x16),      "S16Kn_PurifyVariabl",    UINT8(0x80), \
  UINT8(0x16),      "S16Cs_FbtsComplete0",    UINT8(0x20), \
  UINT8(0x16),      "S16Cs_FbtsReboot000",    UINT8(0x1F), \
  UINT8(0x16),      "S16Cs_FbtsShutDown0",    UINT8(0x1E), \
  UINT8(0x16),      "S16Cs_FbtsDoNothing",    UINT8(0x1D), \

  # Register IHISI AH=17h (FBTSGetRomFileAndPlatformTable)
  UINT8(0x17),      "S17Cs_GetPlatformTb",    UINT8(0x80), \

  # Register IHISI AH=1Bh (FBTSSkipMcCheckAndBinaryTrans)
  UINT8(0x1B),      "S1BKn_SkipMcCheck00",    UINT8(0x80), \

  # Register IHISI AH=1Ch (FBTSGetATpInformation)
  UINT8(0x1C),      "S1CCs_GetATpInfo000",    UINT8(0x80), \

  # Register IHISI AH=1Eh (FBTSGetWholeBiosRomMap)
  UINT8(0x1E),      "S1EKn_WholeBiosRomp",    UINT8(0x80), \
  UINT8(0x1E),      "S1EOemWholeBiosRomp",    UINT8(0x40), \

  # Register IHISI AH=1Fh (FBTSApHookPoint)
  UINT8(0x1F),      "S1FKn_ApHookforBios",    UINT8(0x80), \
  UINT8(0x1F),      "S1FCs_ApHookForBios",    UINT8(0x40), \

  # Register IHISI AH=20h (FETSWrite)
  UINT8(0x20),      "S20OemDoBeforeWrite",    UINT8(0xE0), \
  UINT8(0x20),      "S20OemEcIdleTrue000",    UINT8(0xC0), \
  UINT8(0x20),      "S20OemFetsWrite0000",    UINT8(0x80), \
  UINT8(0x20),      "S20OemEcIdleFalse00",    UINT8(0x40), \
  UINT8(0x20),      "S20OemDoAfterWrite0",    UINT8(0x20), \
  UINT8(0x20),      "S20Cs_ShutdownMode0",    UINT8(0x1B), \

  # Register IHISI AH=21h (FETSGetEcPartInfo)
  UINT8(0x21),      "S21OemGetEcPartInfo",    UINT8(0x80), \

  # Register IHISI AH=41h (OEMSFOEMExCommunication)
  UINT8(0x41),      "S41Kn_CommuSaveRegs",    UINT8(0xFF), \
  UINT8(0x41),      "S41Cs_ExtDataCommun",    UINT8(0xE0), \
  UINT8(0x41),      "S41OemT01Vbios00000",    UINT8(0xC0), \
  UINT8(0x41),      "S41OemT54LogoUpdate",    UINT8(0xBB), \
  UINT8(0x41),      "S41OemT55CheckSignB",    UINT8(0xB6), \
  UINT8(0x41),      "S41OemReservedFun00",    UINT8(0xB1), \
  UINT8(0x41),      "S41Kn_T51EcIdelTrue",    UINT8(0x85), \
  UINT8(0x41),      "S41Kn_ExtDataCommun",    UINT8(0x80), \
  UINT8(0x41),      "S41Kn_T51EcIdelFals",    UINT8(0x7B), \
  UINT8(0x41),      "S41OemT50Oa30RWFun0",    UINT8(0x40), \

  # Register IHISI AH=42h (OEMSFOEMExDataWrite)
  UINT8(0x42),      "S42Cs_ExtDataWrite0",    UINT8(0xE0), \
  UINT8(0x42),      "S42Kn_T50EcIdelTrue",    UINT8(0x85), \
  UINT8(0x42),      "S42Kn_ExtDataWrite0",    UINT8(0x80), \
  UINT8(0x42),      "S42Kn_T50EcIdelFals",    UINT8(0x7B), \
  UINT8(0x42),      "S42Cs_DShutdownMode",    UINT8(0x20), \

  # Register IHISI AH=47h (OEMSFOEMExDataRead)
  UINT8(0x47),      "S47Cs_ExtDataRead00",    UINT8(0xE0), \
  UINT8(0x47),      "S47Kn_ExtDataRead00",    UINT8(0x80), \

  # Register IHISI AH=48h (FBTSOEMCapsuleSecureFlash)

  UINT8(0x48),      "S48Cs_CpSecureFlash",    UINT8(0xDE), \
  UINT8(0x48),      "S48Kn_CpSecureFlash",    UINT8(0x80), \

  # Register IHISI AH=49h (FBTSCommonCommunication)
  UINT8(0x49),      "S49Kn_ComDataCommun",    UINT8(0x80), \

  # Register IHISI AH=4Bh (FBTSCommonRead)
  UINT8(0x4B),      "S4BKn_ComDataRead00",    UINT8(0x80), \

  # Register IHISI AH=4Dh (FBTSPassImageFromTool)
  UINT8(0x4D),      "S4DCs_ImageCheck000",    UINT8(0xC0), \
#[-start-221013-IB14860855-add]#
  # Register IHISI AH=52h (UveVariableConfirm)
  UINT8(0x52),      "S52Cs_ApcbSync",         UINT8(0x60), \
#[-end-221013-IB14860855-add]#
  # Register IHISI AH=80h (IhisiAuthStatus)
  UINT8(0x80),      "S80Kn_AuthStatus000",    UINT8(0x80), \
  # Register IHISI AH=81h (IhisiAuthLock)
  UINT8(0x81),      "S81Kn_AuthLock00000",    UINT8(0x80), \
  # Register IHISI AH=82h (IhisiAuthUnlock)
  UINT8(0x82),      "S82Kn_AuthUnlock000",    UINT8(0x80), \
  # Register IHISI AH=83h (IhisiGetCmdBuffer)
  UINT8(0x83),      "S83Kn_GetCmdBuf0000",    UINT8(0x80), \
  UINT8(0x83),      "S83Kn_GetImageBuf00",    UINT8(0x79), \
  # Register IHISI AH=84h (IhisiAuth)
  UINT8(0x84),      "S84Kn_Auth000000000",    UINT8(0x80) }

#[-start-190226-IB06070012-add]#
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxPeiPcdCallBackNumberPerPcdEntry|10
#[-end-190226-IB06070012-add]#

################################################################################
#
# Pcd Dynamic Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsDynamicDefault]
  #
  # Discrete TPM support (type 1: SPI TPM (default)  0: LPC TPM)
  #
  gChipsetPkgTokenSpaceGuid.SpiOrLpcTPM|1


  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdLegacyFree|$(LEGACY_FREE_SUPPORT)
!if $(LEGACY_FREE_SUPPORT) == YES
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdEspiKbc6064Enable|FALSE
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdNoneSioKbcSupport|TRUE
!else
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdEspiKbc6064Enable|TRUE
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdNoneSioKbcSupport|FALSE
!endif

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPspEnable|TRUE
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdCfgGnbIoapicAddress|0xFEC01000
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdPeiGopEnable|TRUE

  # XGbe
  # XGbe - Create Dummmy tables for PcdXgbePort0Table and PcdXgbePort1Table PCDs. The table will get updated dynamically in OemTable.c
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdXgbeMdio0|TRUE  #XGbe controller Sideband MDIO0 enable/disable
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdXgbePort0ConfigEn|TRUE #XGbE controller Port Config Enable/disable
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdXgbePort0Table|{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdXgbeMdio1|TRUE #XGbe controller Sideband MDIO1 enable/disable
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdXgbePort1ConfigEn|FALSE #XGbE controller Port Config Enable/disable
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdXgbePort1Table|{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}

#[-start-240426-IB18160212-add-]#
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdEspiEc0Enable|FALSE
#[-end-240426-IB18160212-add-]#

################################################################################
#
# SMBIOS Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsDynamicExDefault]

  #
  # Crisis File name definition
  #
  # New File Path Definition : //Volume_Label\\File_Path\\File_Name
  # Notice : "//" is signature that volume label start definition.
  #
  # Example path : //RECOVERY\\BIOS\\Current\\Bilby.fd
  gInsydeTokenSpaceGuid.PcdPeiRecoveryFile|L"Bilby.fd"
#Arista reset mode PCD
#  gEfiAmdAgesaPkgTokenSpaceGuid.PcdFchFullHardReset|TRUE
#End Arista change


  #
  # SMBIOS Pcd Section (Miscellaneous Types)
  # There are 2 kinds of SMBIOS you should take care of:
  # 1) Single record    - PcdType???Record    , without number suffix.
  # 2) Multiple record  - PcdType???Record*** , with number suffix.
  #
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType000|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType001|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType002|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType003|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType008|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType009|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType011|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType012|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType013|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType021|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType026|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType032|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType040|TRUE
  gSmbiosTokenSpaceGuid.PcdActiveSmbiosType041|TRUE

  gSmbiosTokenSpaceGuid.PcdType000Record | { \
      0x00,                       \ # Type
      0x00,                       \ # Length
      UINT16(0x0000),             \ # Handle
      0xFF,                       \ # Vendor
      0xFF,                       \ # BIOS Version
      UINT16(0xE000),             \ # BIOS Starting Address Segment
      0xFF,                       \ # BIOS Release Date
      0xFF,                       \ # BIOS ROM Size
      UINT64(0x000000004BF99880), \ # BIOS Characteristics
      UINT16(0x0D03),             \ # BIOS Characteristics Extension Bytes
      0xFF,                       \ # System BIOS Major Release
      0xFF,                       \ # System BIOS Minor Release
      0xFF,                       \ # Embedded Controller Firmware Major Release
      0xFF,                       \ # Embedded Controller Firmware Minor Release
      0x00,                       \ # Extended BIOS ROM Size
      0x00                        \ # Extended BIOS ROM Size
  }
  gSmbiosTokenSpaceGuid.PcdType000Strings   |"Insyde Corp.;Bilby.05.38.09.0023;03/04/2024;"

  gSmbiosTokenSpaceGuid.PcdType001Record    |{0x01, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x78, 0x56, 0x34, 0x12, 0x34, 0x12, 0x78, 0x56, 0x90, 0xAB, 0xCD, 0xDE, 0xEF, 0xAA, 0xBB, 0xCC, 0x06, 0x05, 0x06}
  gSmbiosTokenSpaceGuid.PcdType001Strings   |"AMD;Bilby;1;123456789;123456789;Picasso;"

  gSmbiosTokenSpaceGuid.PcdType002Record000 |{0x02, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x09, 0x06, 0x03, 0x00, 0x0A, 0x00, 0x00}
  gSmbiosTokenSpaceGuid.PcdType002Strings000|"AMD;Bilby;Base Board Version;Base Board Serial Number;Base Board Asset Tag;Base Board Chassis Location;"

  gSmbiosTokenSpaceGuid.PcdType003Record000 |{0x03, 0x00, 0x00, 0x00, 0x01, 0x0A, 0x02, 0x03, 0x04, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x05}
  gSmbiosTokenSpaceGuid.PcdType003Strings000|"Chassis Manufacturer;Chassis Version;Chassis Serial Number;Chassis Asset Tag;Chassis SKU;"

  gSmbiosTokenSpaceGuid.PcdType008Record000 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x0F, 0x0D}
  gSmbiosTokenSpaceGuid.PcdType008Strings000|"J2604/J2606;Keyboard;"
  gSmbiosTokenSpaceGuid.PcdType008Record001 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x0F, 0x0E}
  gSmbiosTokenSpaceGuid.PcdType008Strings001|"J2605;Touch pad;"
  gSmbiosTokenSpaceGuid.PcdType008Record002 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x12, 0x10}
  gSmbiosTokenSpaceGuid.PcdType008Strings002|"J1500;USB;"
  gSmbiosTokenSpaceGuid.PcdType008Record003 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x12, 0x10}
  gSmbiosTokenSpaceGuid.PcdType008Strings003|"J1501;USB;"
  gSmbiosTokenSpaceGuid.PcdType008Record004 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x12, 0x10}
  gSmbiosTokenSpaceGuid.PcdType008Strings004|"J1502;USB;"
  gSmbiosTokenSpaceGuid.PcdType008Record005 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x12, 0x10}
  gSmbiosTokenSpaceGuid.PcdType008Strings005|"J1301;USB;"
  gSmbiosTokenSpaceGuid.PcdType008Record006 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x0B, 0x1F}
  gSmbiosTokenSpaceGuid.PcdType008Strings006|"J1300;Network;"
  gSmbiosTokenSpaceGuid.PcdType008Record007 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x22, 0x02, 0x00, 0x20}
  gSmbiosTokenSpaceGuid.PcdType008Strings007|"J1707;Sata HDD;"
  gSmbiosTokenSpaceGuid.PcdType008Record008 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x22, 0x02, 0x00, 0x20}
  gSmbiosTokenSpaceGuid.PcdType008Strings008|"J1705;Sata ODD;"
  gSmbiosTokenSpaceGuid.PcdType008Record009 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x1C}
  gSmbiosTokenSpaceGuid.PcdType008Strings009|"J1100;DP0;"
  gSmbiosTokenSpaceGuid.PcdType008Record010 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x1C}
  gSmbiosTokenSpaceGuid.PcdType008Strings010|"J1101;DP1;"
  gSmbiosTokenSpaceGuid.PcdType008Record010 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x1C}
  gSmbiosTokenSpaceGuid.PcdType008Strings010|"J1102;DP2;"
  gSmbiosTokenSpaceGuid.PcdType008Record011 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x1F, 0x1D}
  gSmbiosTokenSpaceGuid.PcdType008Strings011|"J2106;Microphone In;"
  gSmbiosTokenSpaceGuid.PcdType008Record012 |{0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x1F, 0x1D}
  gSmbiosTokenSpaceGuid.PcdType008Strings012|"J2105;Head Phone;"

  gSmbiosTokenSpaceGuid.PcdType009Record001 |{0x09, 0x00, 0x00, 0x00, 0x01, 0xA6, 0x08, 0x03, 0x03, 0x02, 0x00, 0x04, 0x03, 0x00, 0x00, 0x00, 0x12}
  gSmbiosTokenSpaceGuid.PcdType009Strings001|"J3600;PCIe x1  slot;"
  gSmbiosTokenSpaceGuid.PcdType009Record002 |{0x09, 0x00, 0x00, 0x00, 0x01, 0xA6, 0x08, 0x03, 0x03, 0x03, 0x00, 0x04, 0x03, 0x00, 0x00, 0x00, 0x12}
  gSmbiosTokenSpaceGuid.PcdType009Strings002|"J3700;M.2 PCIe slot;"
  gSmbiosTokenSpaceGuid.PcdType009Record003 |{0x09, 0x00, 0x00, 0x00, 0x01, 0xA9, 0x0B, 0x03, 0x03, 0x03, 0x00, 0x04, 0x03, 0x00, 0x00, 0x00, 0x12}
  gSmbiosTokenSpaceGuid.PcdType009Strings003|"J3605;PCIe x8 slot;"
  gSmbiosTokenSpaceGuid.PcdType009Record004 |{0x09, 0x00, 0x00, 0x00, 0x01, 0x01, 0x08, 0x03, 0x03, 0x04, 0x00, 0x04, 0x03, 0x00, 0x00, 0x00, 0x14}
  gSmbiosTokenSpaceGuid.PcdType009Strings004|"J3702;M.2 WWAN slot;"
  gSmbiosTokenSpaceGuid.PcdType009Record005 |{0x09, 0x00, 0x00, 0x00, 0x01, 0x01, 0x08, 0x03, 0x03, 0x04, 0x00, 0x04, 0x03, 0x00, 0x00, 0x00, 0x15}
  gSmbiosTokenSpaceGuid.PcdType009Strings005|"J3703;M.2 WLAN/BT slot;"
  gSmbiosTokenSpaceGuid.PcdType009Record006 |{0x09, 0x00, 0x00, 0x00, 0x01, 0xA6, 0x08, 0x03, 0x03, 0x04, 0x00, 0x04, 0x03, 0x00, 0x00, 0x00, 0x16}
  gSmbiosTokenSpaceGuid.PcdType009Strings006|"J4000;SD card slot;"

  gSmbiosTokenSpaceGuid.PcdType011Record    |{0x0B, 0x00, 0x00, 0x00, 0x05}
  gSmbiosTokenSpaceGuid.PcdType011Strings   |"String1 for Original Equipment Manufacturer;String2 for Original Equipment Manufacturer;String3 for Original Equipment Manufacturer;String4 for Original Equipment Manufacturer;String5 for Original Equipment Manufacturer;"

  gSmbiosTokenSpaceGuid.PcdType012Record    |{0x0C, 0x00, 0x00, 0x00, 0x04}
  gSmbiosTokenSpaceGuid.PcdType012Strings   |"String1 for Type12 Equipment Manufacturer;String2 for Type12 Equipment Manufacturer;String3 for Type12 Equipment Manufacturer;String4 for Type12 Equipment Manufacturer;"

  gSmbiosTokenSpaceGuid.PcdType013Record    |{0x0D, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}
  gSmbiosTokenSpaceGuid.PcdType013Strings   |"en|US|iso8859-1;fr|FR|iso8859-1;ja|JP|unicode;zh|TW|unicode;"

  gSmbiosTokenSpaceGuid.PcdType021Record000 |{0x15, 0x00, 0x00, 0x00, 0x07, 0x04, 0x04}
  gSmbiosTokenSpaceGuid.PcdType021Strings000|""

  gSmbiosTokenSpaceGuid.PcdType026Record000 |{0x1A, 0x00, 0x00, 0x00, 0x01, 0x42, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80}
  gSmbiosTokenSpaceGuid.PcdType026Strings000|"Voltage Probe Description;"

  gSmbiosTokenSpaceGuid.PcdType032Record    |{0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
  gSmbiosTokenSpaceGuid.PcdType032Strings   |""

  #                                                                         Entry1 ---------------------------, Entry2----------------------------------
  gSmbiosTokenSpaceGuid.PcdType040Record    |{0x28, 0x00, 0x00, 0x00, 0x02, 0x06, 0x14, 0x00, 0x05, 0x01, 0xAA, 0x07, 0x00, 0x00, 0x05, 0x02, 0xDC, 0x05}
  gSmbiosTokenSpaceGuid.PcdType040Strings   |"PCIExpressx16;Compiler Version: VC 9.0;"

  gSmbiosTokenSpaceGuid.PcdType041Record000 |{0x29, 0x00, 0x00, 0x00, 0x01, 0x85, 0x01, 0x00, 0x00, 0x03, 0x00}
  gSmbiosTokenSpaceGuid.PcdType041Strings000|"Broadcom 5762;"
  gSmbiosTokenSpaceGuid.PcdType041Record001 |{0x29, 0x00, 0x00, 0x00, 0x01, 0x87, 0x01, 0xFF, 0x00, 0xFF, 0xFF}
  gSmbiosTokenSpaceGuid.PcdType041Strings001|"Realtek ALC701;"

  gI2cDeviceTokenSpaceGuid.PcdI2cTouchPad0|{                                                \  # The definition of I2C mouse PCD
    GUID({0xDB9DEEC2,0x9D01,0x4357,{0xAD,0xAA,0x0C,0xDF,0xBF,0x2D,0x57,0x11}}),             \  # The unique GUID specific for this device, it will be part of device path node
    UINT32(0x00000020),                                                                     \  # Slave address
    UINT32(0x00000001),                                                                     \  # Hardware revision
    0x00,                                                                                   \  # Interrupt GPIO pin active level, 0 = low active, 1 = high active
    0x09,                                                                                   \  # Interrupt GPIO pin number
    UINT16(0x0020),                                                                         \  # HID descriptor register number
    UINT16(0x0102),                                                                         \  # HID device type, 0x0000 = Non-HID device, 0x0d00 = Touch panel, 0x0102 = Mouse, 0x0106 = Keyboard
    0x00,                                                                                   \  # The index of I2C Host controller to the PcdI2cControllerTable, 0 based, FCH I2C0
    0x01,                                                                                   \  # Bus configuration, 0x00 = V_SPEED_STANDARD, 0x01 = V_SPEED_FAST, 0x02 = V_SPEED_HIGH
    0x00                                                                                    \  # The GPIO host controller
  }

#  gI2cDeviceTokenSpaceGuid.PcdI2cTouchPad1|{                                                \  # The definition of I2C mouse PCD
#    GUID({0x90944AAC,0xF33D,0x4F20,{0x8F,0x43,0x93,0xEB,0x5C,0xE6,0x8F,0x57}}),             \  # The unique GUID specific for this device, it will be part of device path node
#    UINT32(0x00000020),                                                                     \  # Slave address
#    UINT32(0x00000001),                                                                     \  # Hardware revision
#    0x00,                                                                                   \  # Interrupt GPIO pin active level, 0 = low active, 1 = high active
#    0x09,                                                                                   \  # Interrupt GPIO pin number
#    UINT16(0x0020),                                                                         \  # HID descriptor register number
#    UINT16(0x0102),                                                                         \  # HID device type, 0x0000 = Non-HID device, 0x0d00 = Touch panel, 0x0102 = Mouse, 0x0106 = Keyboard
#    0x01,                                                                                   \  # The index of I2C Host controller to the PcdI2cControllerTable, 0 based, FCH I2C1
#    0x01,                                                                                   \  # Bus configuration, 0x00 = V_SPEED_STANDARD, 0x01 = V_SPEED_FAST, 0x02 = V_SPEED_HIGH
#    0x00                                                                                    \  # The GPIO host controller
#  }
#
#  gI2cDeviceTokenSpaceGuid.PcdI2cTouchPad2|{                                                \  # The definition of I2C mouse PCD
#    GUID({0x29EF04B0,0x5A68,0x4A7C,{0x85,0x0B,0x29,0x5E,0xA5,0x70,0x27,0x18}}),             \  # The unique GUID specific for this device, it will be part of device path node
#    UINT32(0x00000020),                                                                     \  # Slave address
#    UINT32(0x00000001),                                                                     \  # Hardware revision
#    0x00,                                                                                   \  # Interrupt GPIO pin active level, 0 = low active, 1 = high active
#    0x09,                                                                                   \  # Interrupt GPIO pin number
#    UINT16(0x0020),                                                                         \  # HID descriptor register number
#    UINT16(0x0102),                                                                         \  # HID device type, 0x0000 = Non-HID device, 0x0d00 = Touch panel, 0x0102 = Mouse, 0x0106 = Keyboard
#    0x02,                                                                                   \  # The index of I2C Host controller to the PcdI2cControllerTable, 0 based, FCH I2C2
#    0x01,                                                                                   \  # Bus configuration, 0x00 = V_SPEED_STANDARD, 0x01 = V_SPEED_FAST, 0x02 = V_SPEED_HIGH
#    0x00                                                                                    \  # The GPIO host controller
#  }
#
#  gI2cDeviceTokenSpaceGuid.PcdI2cTouchPad3|{                                                \  # The definition of I2C mouse PCD
#    GUID({0xD7247D2A,0x43E9,0x4F10,{0xAC,0x02,0xDE,0x47,0x18,0x53,0x8B,0x2D}}),             \  # The unique GUID specific for this device, it will be part of device path node
#    UINT32(0x00000020),                                                                     \  # Slave address
#    UINT32(0x00000001),                                                                     \  # Hardware revision
#    0x00,                                                                                   \  # Interrupt GPIO pin active level, 0 = low active, 1 = high active
#    0x09,                                                                                   \  # Interrupt GPIO pin number
#    UINT16(0x0020),                                                                         \  # HID descriptor register number
#    UINT16(0x0102),                                                                         \  # HID device type, 0x0000 = Non-HID device, 0x0d00 = Touch panel, 0x0102 = Mouse, 0x0106 = Keyboard
#    0x03,                                                                                   \  # The index of I2C Host controller to the PcdI2cControllerTable, 0 based, FCH I2C3
#    0x01,                                                                                   \  # Bus configuration, 0x00 = V_SPEED_STANDARD, 0x01 = V_SPEED_FAST, 0x02 = V_SPEED_HIGH
#    0x00                                                                                    \  # The GPIO host controller
#  }
#
#  gI2cDeviceTokenSpaceGuid.PcdI2cTouchPanel0|{                                              \  # The definition of I2C mouse PCD
#    GUID({ 0x680F59EE, 0x8775, 0x4FE6, {0xB2, 0x67, 0xCC, 0xE0, 0xAB, 0xCA, 0x9C, 0x65} }), \  # The unique GUID specific for this device, it will be part of device path node
#    UINT32(0x0000004A),                                                                     \  # Slave address
#    UINT32(0x00000001),                                                                     \  # Hardware revision
#    0x00,                                                                                   \  # Interrupt GPIO pin active level, 0 = low active, 1 = high active
#    0x0C,                                                                                   \  # Interrupt GPIO pin number
#    UINT16(0x0000),                                                                         \  # HID descriptor register number
#    UINT16(0x0d00),                                                                         \  # HID device type, 0x0000 = Non-HID device, 0x0d00 = Touch panel, 0x0102 = Mouse, 0x0106 = Keyboard
#    0x00,                                                                                   \  # The index of I2C Host controller to the PcdI2cControllerTable, 0 based, FCH I2C0
#    0x02,                                                                                   \  # The index of bus configuration to the PcdI2cBusSpeedTable, 0x00 = V_SPEED_STANDARD, 0x01 = V_SPEED_FAST, 0x02 = V_SPEED_HIGH
#    0x00                                                                                    \  # The number of GPIO controller, 0 based
#  }
#
#  gI2cDeviceTokenSpaceGuid.PcdI2cTouchPanel1|{                                              \  # The definition of I2C mouse PCD
#    GUID({ 0x8BF15CDC, 0xB6C1, 0x42DB, {0xBB, 0x2E, 0x6F, 0xAD, 0x8F, 0xAE, 0x92, 0x0A} }), \  # The unique GUID specific for this device, it will be part of device path node
#    UINT32(0x0000004A),                                                                     \  # Slave address
#    UINT32(0x00000001),                                                                     \  # Hardware revision
#    0x00,                                                                                   \  # Interrupt GPIO pin active level, 0 = low active, 1 = high active
#    0x0C,                                                                                   \  # Interrupt GPIO pin number
#    UINT16(0x0000),                                                                         \  # HID descriptor register number
#    UINT16(0x0d00),                                                                         \  # HID device type, 0x0000 = Non-HID device, 0x0d00 = Touch panel, 0x0102 = Mouse, 0x0106 = Keyboard
#    0x01,                                                                                   \  # The index of I2C Host controller to the PcdI2cControllerTable, 0 based, FCH I2C1
#    0x02,                                                                                   \  # The index of bus configuration to the PcdI2cBusSpeedTable, 0x00 = V_SPEED_STANDARD, 0x01 = V_SPEED_FAST, 0x02 = V_SPEED_HIGH
#    0x00                                                                                    \  # The number of GPIO controller, 0 based
#  }
#
#  gI2cDeviceTokenSpaceGuid.PcdI2cTouchPanel2|{                                              \  # The definition of I2C mouse PCD
#    GUID({ 0x55443ED9, 0xB699, 0x4B29, {0x92, 0x48, 0xF4, 0x33, 0x08, 0x3F, 0x4F, 0xF8} }), \  # The unique GUID specific for this device, it will be part of device path node
#    UINT32(0x0000004A),                                                                     \  # Slave address
#    UINT32(0x00000001),                                                                     \  # Hardware revision
#    0x00,                                                                                   \  # Interrupt GPIO pin active level, 0 = low active, 1 = high active
#    0x0C,                                                                                   \  # Interrupt GPIO pin number
#    UINT16(0x0000),                                                                         \  # HID descriptor register number
#    UINT16(0x0d00),                                                                         \  # HID device type, 0x0000 = Non-HID device, 0x0d00 = Touch panel, 0x0102 = Mouse, 0x0106 = Keyboard
#    0x02,                                                                                   \  # The index of I2C Host controller to the PcdI2cControllerTable, 0 based, FCH I2C2
#    0x02,                                                                                   \  # The index of bus configuration to the PcdI2cBusSpeedTable, 0x00 = V_SPEED_STANDARD, 0x01 = V_SPEED_FAST, 0x02 = V_SPEED_HIGH
#    0x00                                                                                    \  # The number of GPIO controller, 0 based
#  }
#
#  gI2cDeviceTokenSpaceGuid.PcdI2cTouchPanel3|{                                              \  # The definition of I2C mouse PCD
#    GUID({ 0x78BDEE0E, 0x7FB0, 0x4EDC, {0xBB, 0xC3, 0xC6, 0x16, 0xC4, 0x42, 0xD5, 0xBB} }), \  # The unique GUID specific for this device, it will be part of device path node
#    UINT32(0x0000004A),                                                                     \  # Slave address
#    UINT32(0x00000001),                                                                     \  # Hardware revision
#    0x00,                                                                                   \  # Interrupt GPIO pin active level, 0 = low active, 1 = high active
#    0x0C,                                                                                   \  # Interrupt GPIO pin number
#    UINT16(0x0000),                                                                         \  # HID descriptor register number
#    UINT16(0x0d00),                                                                         \  # HID device type, 0x0000 = Non-HID device, 0x0d00 = Touch panel, 0x0102 = Mouse, 0x0106 = Keyboard
#    0x03,                                                                                   \  # The index of I2C Host controller to the PcdI2cControllerTable, 0 based, FCH I2C3
#    0x02,                                                                                   \  # The index of bus configuration to the PcdI2cBusSpeedTable, 0x00 = V_SPEED_STANDARD, 0x01 = V_SPEED_FAST, 0x02 = V_SPEED_HIGH
#    0x00                                                                                    \  # The number of GPIO controller, 0 based
#  }

[PcdsDynamicHii]
  #PcdTokenSpaceGuidCName.PcdCName|VariableName|VariableGuid | VariableOffset [| HiiDefaultValue]
  #Note VariableOffset need be update when the offset changed in variable structure
#Disable PSP  gPspPkgTokenSpaceGuid.PcdPspPkgEnable|L"Setup"|gSystemConfigurationGuid|411|1

[Libraries]

[LibraryClasses]
#   TimerLib|MdePkg/Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf
  SmbusLib|MdePkg/Library/DxeSmbusLib/DxeSmbusLib.inf
#   PlatformVfrLib|InsydeModulePkg/Library/PlatformVfrLibNull/PlatformVfrLibNull.inf
  PciLib|MdePkg/Library/BasePciLibPciExpress/BasePciLibPciExpress.inf

  EcLib|$(PROJECT_PKG)/Library/EcLib/BaseEcLib.inf

!if !gChipsetPkgTokenSpaceGuid.PcdLegacyFreeSupported
  BaseOemSvcKernelLib|$(PROJECT_PKG)/Library/BaseOemSvcKernelLib/BaseOemSvcKernelLib.inf
!endif
#   KernelConfigLib|InsydeModulePkg/Library/KernelConfigLibNull/KernelConfigLibNull.inf
#   FlashDevicesLib|InsydeModulePkg/Library/FlashDeviceSupport/FlashDevicesLib/FlashDevicesLib.inf
#   SpiAccessLib|InsydeModulePkg/Library/FlashDeviceSupport/SpiAccessLibNull/SpiAccessLibNull.inf
#   FlashWriteEnableLib|InsydeModulePkg/Library/FlashDeviceSupport/FlashWriteEnableLibNull/FlashWriteEnableLibNull.inf
#   PlatformBdsLib|InsydeModulePkg/Library/PlatformBdsLibNull/PlatformBdsLibNull.inf

#  OemAgesaPlatformLib|$(PROJECT_PKG)/Library/AgesaPlatformLib/OemAgesaPlatformLibDxe.inf

#[-start-220714-IB14860813-remove]#
#  !disable FchSandstoneDxeLib|AgesaModulePkg/Fch/Sandstone/FchSandstoneCore/FchSandstoneLibDxe.inf
#    FchSandstoneDxeLib|AgesaModulePkg/Fch/Sandstone/FchSandstoneCore/FchSandstoneLibDxe.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(PROJECT_PKG)/Override/AgesaModulePkg/Fch/Sandstone/FchSandstoneCore
#    }
#[-end-220714-IB14860813-remove]#

  # PcdSerialPortSelect   # 0 = FCH UART 0
                          # 1 = FCH UART 1
                          # 2 = Simnow ITE8712 Sio Serial Port
                          # 3 = Emulation Port 0x80 out
                          # 4 = Simnow console input from Simnow ITE8712 SIO serial port, and Console output to Simnow Console Output
                          # 5 = SMSC Sio Serial Port
                          # 6 = Simnow SMSC 1100 Sio Serial Port
!if $(EFI_DEBUG) == YES || $(USB_DEBUG_SUPPORT) == YES || $(INSYDE_DEBUGGER) == YES
  !if (gPlatformPkgTokenSpaceGuid.PcdSerialPortSelect == 0 || gPlatformPkgTokenSpaceGuid.PcdSerialPortSelect == 1 || gPlatformPkgTokenSpaceGuid.PcdSerialPortSelect == 7 || gPlatformPkgTokenSpaceGuid.PcdSerialPortSelect == 8)
    PlatformHookLib|AmdCpmPkg/Library/CommonLib/BasePlatformHookLibAmdFchUart/BasePlatformHookLibAmdFchUart.inf
    SerialPortLib|AmdCpmPkg/Library/CommonLib/BaseSerialPortLib16550AmdFchUart/BaseSerialPortLib16550AmdFchUart.inf
  !elseif gPlatformPkgTokenSpaceGuid.PcdSerialPortSelect == 2
    PlatformHookLib|AmdCpmPkg/Library/CommonLib/BasePlatformHookLibAmdIte8712/BasePlatformHookLibAmdIte8712.inf
  !elseif gPlatformPkgTokenSpaceGuid.PcdSerialPortSelect == 3
    SerialPortLib|$(PROJECT_PKG)/Library/BaseSerialPortLibPort80/BaseSerialPortLibPort80.inf
  !elseif gPlatformPkgTokenSpaceGuid.PcdSerialPortSelect == 6
    PlatformHookLib|AmdCpmPkg/Library/CommonLib/BasePlatformHookLibAmdSmsc1100/BasePlatformHookLibAmdSmsc1100.inf
  !endif
  !if $(USE_AGESA_DEBUGLIB) == YES
    DebugLib|AgesaModulePkg/Library/DebugLibIdsDp/DebugLibIdsDp.inf
  !else
#    DebugLib|MdeModulePkg/Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
  !endif
!endif

#[-start-220714-IB14860813-remove]#
##[-start-191008-IB06070054-add]#
#  CrDeviceVariableLib|InsydeCrPkg/Library/CrDeviceVariableLib/CrDeviceVariableLib.inf {
#    <SOURCE_OVERRIDE_PATH>
#      $(PROJECT_PKG)/Override/InsydeCrPkg/Library/CrDeviceVariableLib/
#  }
##[-end-191008-IB06070054-add]#
##[-start-191108-IB06070061-add]#
#  !disable SetupUtilityLib|InsydeModulePkg/Library/SetupUtilityLib/SetupUtilityLib.inf
#  SetupUtilityLib|InsydeModulePkg/Library/SetupUtilityLib/SetupUtilityLib.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Library/SetupUtilityLib
#  }
##[-end-191108-IB06070061-add]#
##[-start-220415-IB14860774-add]#
#  !disable CapsuleLib|InsydeModulePkg/Library/DxeCapsuleLib/DxeCapsuleLib.inf
#  CapsuleLib|InsydeModulePkg/Library/DxeCapsuleLib/DxeCapsuleLib.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Library/DxeCapsuleLib
#  }
#
#[-start-240426-IB18160212-add-]#
  !disable GenericBdsLib|InsydeModulePkg/Library/GenericBdsLib/GenericBdsLib.inf
  GenericBdsLib|InsydeModulePkg/Library/GenericBdsLib/GenericBdsLib.inf {
    <SOURCE_OVERRIDE_PATH>
    $(PROJECT_PKG)/Override/InsydeModulePkg/Library/GenericBdsLib
  }
#[-end-240426-IB18160212-add-]#
##[-end-220415-IB14860774-add]#
#[-end-220714-IB14860813-remove]#
  CmosLib|$(PROJECT_PKG)/Library/CmosLib/CmosLib.inf
#[-start-240426-IB18160212-add-]#
  !disable PostCodeMiscLib|InsydePostCodePkg/Library/PostCodeMiscLib/PostCodeMiscLib.inf
  PostCodeMiscLib|InsydePostCodePkg/Library/PostCodeMiscLib/PostCodeMiscLib.inf {
    <SOURCE_OVERRIDE_PATH>
      $(PROJECT_PKG)/Override/InsydePostCodePkg/Library/PostCodeMiscLib
  }
#[-end-240426-IB18160212-add-]#

[LibraryClasses.common]

[LibraryClasses.common.SEC]
#   H2ODebugLib|InsydeModulePkg/Library/PeiH2ODebugLib/PeiH2ODebugLib.inf
  SecOemSvcChipsetLib|$(PROJECT_PKG)/Library/SecOemSvcChipsetLib/SecOemSvcChipsetLib.inf

[LibraryClasses.common.PEI_CORE]
  PeiOemSvcKernelLib|$(PROJECT_PKG)/Library/PeiOemSvcKernelLib/PeiOemSvcKernelLib.inf

[LibraryClasses.common.PEIM]
  PeiOemSvcKernelLib|$(PROJECT_PKG)/Library/PeiOemSvcKernelLib/PeiOemSvcKernelLib.inf
  #OemAgesaPlatformLib|$(PROJECT_PKG)/Library/AgesaPlatformLib/OemAgesaPlatformLibPei.inf
  OemAgesaCcxPlatformLib|$(CHIPSET_PKG)/Library/AgesaCcxPlatformLib/AgesaCcxPlatformLib.inf


#[-start-220714-IB14860813-remove]#
#  !disable FchSandstonePeiLib|AgesaModulePkg/Fch/Sandstone/FchSandstoneCore/FchSandstoneLibPei.inf
#    FchSandstonePeiLib|AgesaModulePkg/Fch/Sandstone/FchSandstoneCore/FchSandstoneLibPei.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(PROJECT_PKG)/Override/AgesaModulePkg/Fch/Sandstone/FchSandstoneCore
#    }
#[-end-220714-IB14860813-remove]#

#[-start-181221-IB09860035-add]#
  GpioCfgLib|$(PROJECT_PKG)/Library/GpioCfgLib/GpioCfgLib.inf
#[-end-181221-IB09860035-add]#

[LibraryClasses.common.DXE_CORE]
  DxeOemSvcKernelLib|$(PROJECT_PKG)/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLib.inf
  !if $(USE_AGESA_DEBUGLIB) == YES
    DebugLib|AgesaModulePkg/Library/DebugLibIdsDp/DebugLibIdsDp.inf
  !else
#    DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
  !endif

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  DxeOemSvcKernelLib|$(PROJECT_PKG)/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLib.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf

[LibraryClasses.common.UEFI_DRIVER]
  DxeOemSvcKernelLib|$(PROJECT_PKG)/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLib.inf
  AmdPbsConfigLib|AmdCpmPkg/Addendum/Oem/$(PLATFORM_NAME)/Library/AmdPbsConfigLib/AmdPbsConfigDxeLib.inf

[LibraryClasses.common.DXE_DRIVER]
  DxeOemSvcKernelLib|$(PROJECT_PKG)/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLib.inf

  !if $(EMULATION_ONLY) == NO
    EcLib|$(PROJECT_PKG)/Library/EcLib/DxeEcLib.inf
  !endif

[LibraryClasses.common.DXE_SMM_DRIVER]
  DxeOemSvcKernelLib|$(PROJECT_PKG)/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLib.inf
  SmmOemSvcKernelLib|$(PROJECT_PKG)/Library/SmmOemSvcKernelLib/SmmOemSvcKernelLib.inf
  !if $(EMULATION_ONLY) == NO
    EcLib|$(PROJECT_PKG)/Library/EcLib/DxeEcLib.inf
  !endif
  !if $(USE_AGESA_DEBUGLIB) == YES
    DebugLib|AgesaModulePkg/Library/DebugLibIdsDp/DebugLibIdsDp.inf
  !else
#    DebugLib|$(PROJECT_PKG)/Override/MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
  !endif

[LibraryClasses.common.COMBINED_SMM_DXE]
  DxeOemSvcKernelLib|$(PROJECT_PKG)/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLib.inf
  SmmOemSvcKernelLib|$(PROJECT_PKG)/Library/SmmOemSvcKernelLib/SmmOemSvcKernelLib.inf

[LibraryClasses.common.SMM_CORE]
  DxeOemSvcKernelLib|$(PROJECT_PKG)/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLib.inf
  SmmOemSvcKernelLib|$(PROJECT_PKG)/Library/SmmOemSvcKernelLib/SmmOemSvcKernelLib.inf

[LibraryClasses.common.UEFI_APPLICATION]
  DxeOemSvcKernelLib|$(PROJECT_PKG)/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLib.inf

################################################################################
#
# Platform related components
#
################################################################################
[Components.X64]
!disable AgesaModulePkg/Ras/AmdRasRvApeiDxe/AmdRasRvApeiDxe.inf
 AgesaModulePkg/Ras/AmdRasRvApeiDxe/AmdRasRvApeiDxe.inf  {
!if $(EFI_DEBUG) == NO
  <LibraryClasses>
    SerialPortLib|AmdCpmPkg/Library/CommonLib/BaseSerialPortLib16550AmdFchUart/BaseSerialPortLib16550AmdFchUart.inf
    DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
    DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  <PcdsFixedAtBuild>
    gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2
    gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000000
  <BuildOptions>
    *_*_*_CC_FLAGS  = -UMDEPKG_NDEBUG
!endif
  }
!if $(AMD_APP_INSTEAD_EFI_SHELL_SUPPORT) == YES
  EmulationToolsPkg/EmuLinuxLoader/EmuLinuxLoader.inf
  EmulationToolsPkg/EmuLinuxTest/EmuLinuxTest.inf
!endif

[Components.$(PEI_ARCH)]

!if gAmdCommonChipsetPkgTokenSpaceGuid.PcdMemoryContextSaveRestoreSupport
  AmdCommonChipsetPkg/AmdMemContextSaveRestorePei/AmdMemContextSaveRestorePei.inf
!endif


!disable AmdCpmPkg/Addendum/Oem/$(CRB_NAME)/Pei/AmdCpmOemInitPei/AmdCpm3OemInitPeim.inf
  AmdCpmPkg/Addendum/Oem/$(CRB_NAME)/Pei/AmdCpmOemInitPei/AmdCpm3OemInitPeim.inf {
!if $(BACKUP_BIOS) == NO
  <LibraryClasses>
    SerialPortLib|AmdCpmPkg/Library/CommonLib/BaseSerialPortLib16550AmdFchUart/BaseSerialPortLib16550AmdFchUart.inf
	DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
	DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
	DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  <PcdsFixedAtBuild>
    gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2
	gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000000
  <BuildOptions>
    *_*_*_CC_FLAGS  = -UMDEPKG_NDEBUG
!endif
  <SOURCE_OVERRIDE_PATH>
    $(PROJECT_PKG)/Override/AmdCpmPkg/Addendum/Oem/$(CRB_NAME)/Pei/AmdCpmOemInitPei
}

!if $(USE_FAST_CRISIS_RECOVERY) == NO
  $(CHIPSET_PKG)/GfxInitPei/GfxInitPei.inf
!endif

#[-start-220714-IB14860813-remove]#
##[-start-181114-IB06070004-add]#
#!if $(INSYDE_DEBUGGER) == YES and $(H2O_DDT_DEBUG_IO) == Com
#  !disable InsydeModulePkg/H2ODebug/$(H2O_DDT_DEBUG_IO)DebugIoPei/$(H2O_DDT_DEBUG_IO)DebugIoPei.inf
#  $(PROJECT_PKG)/Override/InsydeModulePkg/H2ODebug/$(H2O_DDT_DEBUG_IO)DebugIoPei/$(H2O_DDT_DEBUG_IO)DebugIoPei.inf
#!endif
##[-end-181114-IB06070004-add]#
#
##[-start-190725-IB06070044-add]#
#!if gInsydeTokenSpaceGuid.PcdH2ONvmeSupported
#  !disable InsydeModulePkg/Bus/Pci/NvmExpressPei/NvmExpressPei.inf
#  $(PROJECT_PKG)/Override/InsydeModulePkg/Bus/Pci/NvmExpressPei/NvmExpressPei.inf
#!endif
##[-end-190725-IB06070044-add]#
#
##[-start-191115-IB06070063-add]#
#!if gInsydeTokenSpaceGuid.PcdH2OUsbPeiSupported
#  !disable InsydeModulePkg/Bus/Pci/XhciPei/XhciPei.inf
#  $(PROJECT_PKG)/Override/InsydeModulePkg/Bus/Pci/XhciPei/XhciPei.inf
#
#  !disable InsydeModulePkg/Bus/Usb/UsbBusPei/UsbBusPei.inf
#  InsydeModulePkg/Bus/Usb/UsbBusPei/UsbBusPei.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Bus/Usb/UsbBusPei
#  }
#
#  !disable InsydeModulePkg/Bus/Usb/UsbMassStoragePei/UsbMassStoragePei.inf
#  InsydeModulePkg/Bus/Usb/UsbMassStoragePei/UsbMassStoragePei.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Bus/Usb/UsbMassStoragePei
#  }
#!endif
##[-end-191115-IB06070063-add]#
#[-end-220714-IB14860813-remove]#

#[-start-240426-IB18160212-add-]#
!if gH2OPostCodePkgTokenSpaceGuid.PcdH2OPostCodeSupported == 1 && $(INSYDE_DEBUGGER) == 0
  !disable  InsydePostCodePkg/PostCodePei/PostCodePei.inf
  InsydePostCodePkg/PostCodePei/PostCodePei.inf {
    <SOURCE_OVERRIDE_PATH>
      $(PROJECT_PKG)/Override/InsydePostCodePkg/PostCodePei
  }
!endif
#[-start-240426-IB18160212-add-]#

[Components.$(DXE_ARCH)]

## <Start-Arista>
## Board Customization Driver - DXE
#  $(PROJECT_PKG)/BoardCustomDxe/BoardCustomDxe.inf
#  $(PROJECT_PKG)/OemAcpiTables/OemAcpiTables.inf
## <End-Arista>

  #
  # Replace DebugLib instance with null lib to avoid USB function not working with efidebug build
  #
  !disable AgesaModulePkg/Fch/Sandstone/FchSandstoneSmmDispatcher/FchSmmDispatcher.inf
  AgesaModulePkg/Fch/Sandstone/FchSandstoneSmmDispatcher/FchSmmDispatcher.inf {
    <LibraryClasses>
      DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  }


##[-start-220714-IB14860813-remove]#
#  !disable InsydeModulePkg/Bus/Pci/PciBusDxe/PciBusDxe.inf
#      InsydeModulePkg/Bus/Pci/PciBusDxe/PciBusDxe.inf {
#      <SOURCE_OVERRIDE_PATH>
#      $(PROJECT_PKG)/Override/InsydeModulePkg/Bus/Pci/PciBusDxe
#  }

#  !disable InsydeModulePkg/Universal/Security/HstiDxe/HstiDxe.inf
#        InsydeModulePkg/Universal/Security/HstiDxe/HstiDxe.inf {
#      <SOURCE_OVERRIDE_PATH>
#       $(PROJECT_PKG)/Override/InsydeModulePkg/Universal/Security/HstiDxe
#  }
#
#  !disable InsydeSetupPkg/Drivers/DisplayTypeDxe/DisplayTypeDxe.inf
#      InsydeSetupPkg/Drivers/DisplayTypeDxe/DisplayTypeDxe.inf {
#      <SOURCE_OVERRIDE_PATH>
#      $(PROJECT_PKG)/Override/InsydeSetupPkg/Drivers/DisplayTypeDxe
#  }
#
##[-start-181219-IB06070005-add]#
#
#  $(PROJECT_PKG)\Override\AgesaModulePkg\SnpDxe\SnpDxePort.inf
##[-end-181219-IB06070005-add]#
#
#[-start-190225-IB06070011-add]#
#!if gInsydeTokenSpaceGuid.PcdH2OSdhcSupported
#  !disable InsydeModulePkg/Bus/Pci/SdhcDxe/SdHostDriverDxe.inf
#  $(PROJECT_PKG)/Override/InsydeModulePkg/Bus/Pci/SdhcDxe/SdHostDriverDxe.inf
#
#  !disable InsydeModulePkg/Bus/Sdhc/SdMmcDxe/SdMmcDeviceDxe.inf
#  $(PROJECT_PKG)/Override/InsydeModulePkg/Bus/Sdhc/SdMmcDxe/SdMmcDeviceDxe.inf
#
#!if gInsydeTokenSpaceGuid.PcdH2OCsmSupported
#  !disable InsydeModulePkg/Csm/SdLegacySmm/SdLegacySmm.inf
#  $(PROJECT_PKG)/Override/InsydeModulePkg/Csm/SdLegacySmm/SdLegacySmm.inf
#!endif
#!endif
##[-end-190225-IB06070011-add]#
#
##[-start-190326-IB06070019-add]#
#!if gInsydeTokenSpaceGuid.PcdH2OUsbSupported
#  !disable InsydeModulePkg/Bus/Pci/XhciDxe/XhciDxe.inf
#  $(PROJECT_PKG)/Override/InsydeModulePkg/Bus/Pci/XhciDxe/XhciDxe.inf
#!endif
##[-end-190326-IB06070019-add]#
#
##[-start-190725-IB06070044-add]#
#!if gInsydeTokenSpaceGuid.PcdH2ONvmeSupported
#  !disable InsydeModulePkg/Bus/Pci/NvmExpressDxe/NvmExpressDxe.inf
#  $(PROJECT_PKG)/Override/InsydeModulePkg/Bus/Pci/NvmExpressDxe/NvmExpressDxe.inf
#
##[-start-200422-IB06070087-add]#
#  !disable InsydeModulePkg/Csm/NvmExpressLegacySmm/NvmExpressLegacySmm.inf
#  $(PROJECT_PKG)/Override/InsydeModulePkg/Csm/NvmExpressLegacySmm/NvmExpressLegacySmm.inf
##[-end-200422-IB06070087-add]#
#!endif
##[-end-190725-IB06070044-add]#
#[-end-220714-IB14860813-remove]#

#[-start-220714-IB14860813-remove]#
##[-start-190821-IB06070046-add]#
#!if gInsydeTokenSpaceGuid.PcdH2OAhciSupported
#  !disable InsydeModulePkg/Bus/Pci/AhciBusDxe/AhciBusDxe.inf
#  $(PROJECT_PKG)/Override/InsydeModulePkg/Bus/Pci/AhciBusDxe/AhciBusDxe.inf
#!endif
##[-end-190821-IB06070046-add]#
#
##[-start-190830-IB06070047-add]#
#  !disable InsydeModulePkg/Universal/UserInterface/HiiDatabaseDxe/HiiDatabaseDxe.inf
#  InsydeModulePkg/Universal/UserInterface/HiiDatabaseDxe/HiiDatabaseDxe.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Universal/UserInterface/HiiDatabaseDxe
#  }
##[-end-190830-IB06070047-add]#
#
##[-start-191231-IB06070075-add]#
#  !disable InsydeModulePkg/Universal/FirmwareVolume/FvbServicesRuntimeDxe/FvbServicesRuntimeDxe.inf
#  InsydeModulePkg/Universal/FirmwareVolume/FvbServicesRuntimeDxe/FvbServicesRuntimeDxe.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Universal/FirmwareVolume/FvbServicesRuntimeDxe
#  }
##[-end-191231-IB06070075-add]#
#
##[-start-200910-IB06070099-add]#
#  !disable AgesaModulePkg/Nbio/NbioALib/AmdNbioAlibRVDxe/AmdNbioAlibRVDxe.inf
#  AgesaModulePkg/Nbio/NbioALib/AmdNbioAlibRVDxe/AmdNbioAlibRVDxe.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/AgesaModulePkg/Nbio/NbioALib/AmdNbioAlibRVDxe
#  }
##[-end-200910-IB06070099-add]#
#
##[-start-211112-IB06070138-add]#
#!if gInsydeTokenSpaceGuid.PcdH2OCsmSupported
#!if gInsydeTokenSpaceGuid.PcdH2OAhciSupported == 1 || gInsydeTokenSpaceGuid.PcdH2OIdeSupported == 1
#  !disable InsydeModulePkg/Csm/AtaLegacySmm/AtaLegacySmm.inf
#  $(PROJECT_PKG)/Override/InsydeModulePkg/Csm/AtaLegacySmm/AtaLegacySmm.inf
#!endif
#!endif
##[-end-211112-IB06070138-add]#
#
##[-start-211112-IB06070139-add]#
#!if gInsydeTokenSpaceGuid.PcdH2OIdeSupported
#  !disable InsydeModulePkg/Bus/Pci/IdeBusDxe/IdeBusDxe.inf
#  $(PROJECT_PKG)/Override/InsydeModulePkg/Bus/Pci/IdeBusDxe/IdeBusDxe.inf
#!endif
##[-end-211112-IB06070139-add]#
#
##[-start-211112-IB06070140-add]#
#!if gInsydeTokenSpaceGuid.PcdH2OHddPasswordSupported
#  !disable InsydeModulePkg/Universal/Security/HddPassword/HddPassword.inf
#  InsydeModulePkg/Universal/Security/HddPassword/HddPassword.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Universal/Security/HddPassword
#  }
#!endif
##[-end-211112-IB06070140-add]#
#[-end-220714-IB14860813-remove]#

  #
  # ACPI
  #
  !disable $(CHIPSET_PKG)/AcpiTables/AcpiTables.inf
  $(CHIPSET_PKG)/AcpiTables/AcpiTables.inf {
    <SOURCE_OVERRIDE_PATH>
      $(PROJECT_PKG)/AcpiTables

    <BuildOptions>
!if gChipsetPkgTokenSpaceGuid.PcdNbIoApicSupport
      *_*_*_ASLCC_FLAGS        = -DNB_IOAPIC_SUPPORT
      *_*_*_ASLPP_FLAGS        = -DNB_IOAPIC_SUPPORT
!endif
!if gChipsetPkgTokenSpaceGuid.PcdLegacyFreeSupported
      *_*_*_ASLCC_FLAGS        = -DLEGACY_FREE_SUPPORT
      *_*_*_ASLPP_FLAGS        = -DLEGACY_FREE_SUPPORT
!endif
!if gAmdCpmPkgTokenSpaceGuid.PcdModernStandbySupport
      *_*_*_ASLCC_FLAGS        = -DCONNECTED_STANDBY_SUPPORT
      *_*_*_ASLPP_FLAGS        = -DCONNECTED_STANDBY_SUPPORT
!endif
!if gPlatformPkgTokenSpaceGuid.PcdEmulationOnly
      *_*_*_ASLCC_FLAGS        = -DEMULATION_ONLY
      *_*_*_ASLPP_FLAGS        = -DEMULATION_ONLY
!endif
!if gAmdCpmPkgTokenSpaceGuid.PcdReduceHardwareSupport
      *_*_*_ASLCC_FLAGS 	   = -DREDUCED_HARDWARE_SUPPORT
      *_*_*_ASLPP_FLAGS 	   = -DREDUCED_HARDWARE_SUPPORT
!endif
  }

#  $(PROJECT_PKG)/PlatformSsdtDxe/Asl/GpioSsdt.inf
#  $(PROJECT_PKG)/PlatformSsdtDxe/PlatformSsdtDxe.inf

#  $(PROJECT_PKG)/Binary/Bcm5762/PxeUefi/Bcm5762UndiX64.inf
#  $(PROJECT_PKG)/Binary/RtkUsbUndiDxe/RtkUsbUndiDxe.inf
#  $(PROJECT_PKG)/Binary/RtkLan/RtkUndiDxe/RtkUndiDxe.inf

  $(PROJECT_PKG)/AmdGopConfigDrvDxe/AmdGopConfigDrvDxe.inf

#[-start-220714-IB14860813-remove]#
#!if gInsydeTokenSpaceGuid.PcdH2OUsbSupported
#!if gInsydeTokenSpaceGuid.PcdH2OCsmSupported
#  !disable InsydeModulePkg/Csm/UsbLegacySmm/UsbLegacySmm.inf
#  $(PROJECT_PKG)/Override/InsydeModulePkg/Csm/UsbLegacySmm/UsbLegacySmm.inf
#!endif
#!endif
#
##[-start-181112-IB06070003-add]#
#!if gInsydeCrTokenSpaceGuid.PcdH2OCrHsuSerialSupported == 1
#  !disable InsydeCrPkg/Uart16550Devices/PchHsUartDxe/PchHsUartDxe.inf
#  InsydeCrPkg/Uart16550Devices/PchHsUartDxe/PchHsUartDxe.inf {
#    <SOURCE_OVERRIDE_PATH>
#      $(PROJECT_PKG)/Override/InsydeCrPkg/Uart16550Devices/PchHsUartDxe
#  }
#!endif
##[-end-181112-IB06070003-add]#
#
##[-start-181114-IB06070004-add]#
#!if $(INSYDE_DEBUGGER) == YES and $(H2O_DDT_DEBUG_IO) == Com
#  !disable InsydeModulePkg/H2ODebug/$(H2O_DDT_DEBUG_IO)DebugIoDxe/$(H2O_DDT_DEBUG_IO)DebugIoDxe.inf
#  $(PROJECT_PKG)/Override/InsydeModulePkg/H2ODebug/$(H2O_DDT_DEBUG_IO)DebugIoDxe/$(H2O_DDT_DEBUG_IO)DebugIoDxe.inf
#!endif
##[-end-181114-IB06070004-add]#
#
##[-start-191008-IB06070054-add]#
#  !disable InsydeCrPkg/ConfigUtility/CrConfigUtilDxe/CrConfigUtil.inf
#  InsydeCrPkg/ConfigUtility/CrConfigUtilDxe/CrConfigUtil.inf {
#    <SOURCE_OVERRIDE_PATH>
#      $(PROJECT_PKG)/Override/InsydeCrPkg/ConfigUtility/CrConfigUtilDxe/
#  }
#
#  !disable InsydeCrPkg/TerminalDxe/TerminalDxe.inf
#  InsydeCrPkg/TerminalDxe/TerminalDxe.inf {
#    <SOURCE_OVERRIDE_PATH>
#      $(PROJECT_PKG)/Override/InsydeCrPkg/TerminalDxe/
#  }
#
#  !disable InsydeCrPkg/Uart16550SerialDxe/Uart16550SerialDxe.inf
#  InsydeCrPkg/Uart16550SerialDxe/Uart16550SerialDxe.inf {
#    <SOURCE_OVERRIDE_PATH>
#      $(PROJECT_PKG)/Override/InsydeCrPkg/Uart16550SerialDxe/
#  }
##[-end-191008-IB06070054-add]#
#[-end-220714-IB14860813-remove]#

#[-start-220714-IB14860813-remove]#
##[-start-191008-IB06070056-add]#
#!if gInsydeTokenSpaceGuid.PcdH2OUsbSupported
#  !disable InsydeModulePkg/Bus/Usb/UsbCoreDxe/UsbCoreDxe.inf
#  InsydeModulePkg/Bus/Usb/UsbCoreDxe/UsbCoreDxe.inf {
#    <SOURCE_OVERRIDE_PATH>
#      $(PROJECT_PKG)/Override/InsydeModulePkg/Bus/Usb/UsbCoreDxe/
#  }
#!endif
##[-end-191008-IB06070056-add]#
#
##[-start-191015-IB06070057-add]#
#  !disable InsydeModulePkg/Universal/IhisiServicesSmm/IhisiServicesSmm.inf
#  InsydeModulePkg/Universal/IhisiServicesSmm/IhisiServicesSmm.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Universal/IhisiServicesSmm/
#  }
##[-end-191015-IB06070057-add]#
#
##[-start-191115-IB06070063-modify]#
#!if gInsydeTokenSpaceGuid.PcdH2OUsbSupported
#  !disable InsydeModulePkg/Bus/Usb/UsbBotDxe/UsbBotDxe.inf
#  InsydeModulePkg/Bus/Usb/UsbBotDxe/UsbBotDxe.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Bus/Usb/UsbBotDxe
#  }
#
#  !disable InsydeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
#  InsydeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Bus/Usb/UsbBusDxe
#  }
#
#  !disable InsydeModulePkg/Bus/Usb/UsbCbiDxe/UsbCbiDxe.inf
#  InsydeModulePkg/Bus/Usb/UsbCbiDxe/UsbCbiDxe.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Bus/Usb/UsbCbiDxe
#  }
#
#  !disable InsydeModulePkg/Bus/Usb/UsbKbDxe/UsbKbDxe.inf
#  InsydeModulePkg/Bus/Usb/UsbKbDxe/UsbKbDxe.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Bus/Usb/UsbKbDxe
#  }
#
#  !disable InsydeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf
#  InsydeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Bus/Usb/UsbMassStorageDxe
#  }
#
##[-start-191024-IB06070058-add]#
#  !disable InsydeModulePkg/Bus/Usb/UsbMouseDxe/UsbMouseDxe.inf
#  InsydeModulePkg/Bus/Usb/UsbMouseDxe/UsbMouseDxe.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Bus/Usb/UsbMouseDxe
#  }
##[-end-191024-IB06070058-add]#
#!endif
##[-end-191115-IB06070063-modify]#
#
##[-start-191108-IB06070061-add]#
#!if gInsydeTokenSpaceGuid.PcdH2OPciOptionRomSecurityControlSupported
#  !disable InsydeModulePkg/Universal/SecureOptionRomControlDxe/SecureOptionRomControlDxe.inf
#  InsydeModulePkg/Universal/SecureOptionRomControlDxe/SecureOptionRomControlDxe.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Universal/SecureOptionRomControlDxe
#  }
#!endif
#
#  !disable InsydeModulePkg/Universal/UserInterface/BootMaintDxe/BootMaintDxe.inf
#  InsydeModulePkg/Universal/UserInterface/BootMaintDxe/BootMaintDxe.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Universal/UserInterface/BootMaintDxe
#  }
#
#  !disable InsydeModulePkg/Universal/UserInterface/BootManagerDxe/BootManagerDxe.inf
#  InsydeModulePkg/Universal/UserInterface/BootManagerDxe/BootManagerDxe.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Universal/UserInterface/BootManagerDxe
#  }
#
#  !disable InsydeModulePkg/Universal/UserInterface/DeviceManagerDxe/DeviceManagerDxe.inf
#  InsydeModulePkg/Universal/UserInterface/DeviceManagerDxe/DeviceManagerDxe.inf {
#    <LibraryClasses>
#      NULL|$(PROJECT_PKG)/Override/InsydeModulePkg/Library/DeviceManagerUiLib/DeviceManagerUiLib.inf
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Universal/UserInterface/DeviceManagerDxe
#  }
#
#  !disable InsydeModulePkg/Universal/UserInterface/DriverHealthManagerDxe/DriverHealthManagerDxe.inf
#  InsydeModulePkg/Universal/UserInterface/DriverHealthManagerDxe/DriverHealthManagerDxe.inf {
#    <LibraryClasses>
#      UefiBootManagerLib|MdeModulePkg/Library/UefiBootManagerLib/UefiBootManagerLib.inf
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Universal/UserInterface/DriverHealthManagerDxe
#  }
#
#!if gInsydeTokenSpaceGuid.PcdH2OSecureBootSupported
#  !disable InsydeModulePkg/Universal/UserInterface/SecureBootMgrDxe/SecureBootMgrDxe.inf
#  InsydeModulePkg/Universal/UserInterface/SecureBootMgrDxe/SecureBootMgrDxe.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Universal/UserInterface/SecureBootMgrDxe
#  }
#!endif
#
#  !disable InsydeSetupPkg/Drivers/HiiLayoutPkgDxe/HiiLayoutPkgDxe.inf
#  InsydeSetupPkg/Drivers/HiiLayoutPkgDxe/HiiLayoutPkgDxe.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeSetupPkg/Drivers/HiiLayoutPkgDxe
#  }
#
#!if gInsydeTokenSpaceGuid.PcdH2OFormBrowserSupported == FALSE
#  !disable InsydeModulePkg/Universal/UserInterface/SetupBrowserDxe/SetupBrowserDxe.inf
#!if gInsydeTokenSpaceGuid.PcdGraphicsSetupSupported
#  !disable InsydeModulePkg/Universal/UserInterface/MicrowindowsDxe/MicrowindowsDxe.inf
#!endif
#!endif
##[-end-191108-IB06070061-add]#
#[-end-220714-IB14860813-remove]#

#[-start-220714-IB14860813-remove]#
##[-start-211112-IB06070136-add]#
#  !disable InsydeModulePkg/Universal/Smbios/PnpSmm/PnpSmm.inf
#  InsydeModulePkg/Universal/Smbios/PnpSmm/PnpSmm.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Universal/Smbios/PnpSmm
#  }
#
#  !disable InsydeModulePkg/Universal/SmmResourceCheckDxe/SmmResourceCheckDxe.inf
#  InsydeModulePkg/Universal/SmmResourceCheckDxe/SmmResourceCheckDxe.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Universal/SmmResourceCheckDxe
#  }
#
#!if gInsydeTokenSpaceGuid.PcdH2OStatusCodeBeepSupported
#  !disable InsydeModulePkg/Universal/StatusCode/BeepStatusCodeDxe/BeepStatusCodeDxe.inf
#  InsydeModulePkg/Universal/StatusCode/BeepStatusCodeDxe/BeepStatusCodeDxe.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Universal/StatusCode/BeepStatusCodeDxe
#  }
#
#  !disable InsydeModulePkg/Universal/StatusCode/BeepStatusCodeSmm/BeepStatusCodeSmm.inf
#  InsydeModulePkg/Universal/StatusCode/BeepStatusCodeSmm/BeepStatusCodeSmm.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Universal/StatusCode/BeepStatusCodeSmm
#  }
#!endif
##[-end-211112-IB06070136-add]#
#
##[-start-211112-IB06070137-add]#
#  !disable InsydeModulePkg/Universal/FirmwareVolume/FwBlockServiceSmm/FwBlockServiceSmm.inf
#  InsydeModulePkg/Universal/FirmwareVolume/FwBlockServiceSmm/FwBlockServiceSmm.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Universal/FirmwareVolume/FwBlockServiceSmm
#    <LibraryClasses>
#      S3BootScriptLib|MdePkg/Library/BaseS3BootScriptLibNull/BaseS3BootScriptLibNull.inf
#  }
##[-end-211112-IB06070137-add]#
#
##[-start-220217-IB06070198-add]#
#  !disable InsydeModulePkg/Universal/Security/StorageSecurityCommandDxe/StorageSecurityCommandDxe.inf
#  InsydeModulePkg/Universal/Security/StorageSecurityCommandDxe/StorageSecurityCommandDxe.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Universal/Security/StorageSecurityCommandDxe
#  }
##[-end-220217-IB06070198-add]#
##[-start-220415-IB14860774-add]#
#  !disable InsydeModulePkg/Universal/BdsDxe/BdsDxe.inf
#  InsydeModulePkg/Universal/BdsDxe/BdsDxe.inf {
#    <SOURCE_OVERRIDE_PATH>
#    $(PROJECT_PKG)/Override/InsydeModulePkg/Universal/BdsDxe
#  }
##[-end-220415-IB14860774-add]#
#[-end-220714-IB14860813-remove]#

#[-start-230923-IB18160151-add]#
  #
  # Vlan Id Supported
  #
  InsydeNetworkPkg/Drivers/VlanConfigDxe/VlanConfigDxe.inf
#[-start-230923-IB18160151-add]#

#[-start-240426-IB18160212-add-]#
!if gH2OPostCodePkgTokenSpaceGuid.PcdH2OPostCodeSupported == 1 && $(INSYDE_DEBUGGER) == 0
  !disable  InsydePostCodePkg/PostCodeDxe/PostCodeDxe.inf
  InsydePostCodePkg/PostCodeDxe/PostCodeDxe.inf {
    <SOURCE_OVERRIDE_PATH>
    $(PROJECT_PKG)/Override/InsydePostCodePkg/PostCodeDxe
  }

  !disable  InsydePostCodePkg/PostCodeSmm/PostCodeSmm.inf
  InsydePostCodePkg/PostCodeSmm/PostCodeSmm.inf {
    <SOURCE_OVERRIDE_PATH>
    $(PROJECT_PKG)/Override/InsydePostCodePkg/PostCodeSmm
  }
!endif
#[-end-240426-IB18160212-add-]#

###################################################################################################
#
# BuildOptions Section - Define the module specific tool chain flags that should be used as
#                        the default flags for a module. These flags are appended to any
#                        standard flags that are defined by the build process. They can be
#                        applied for any modules or only those modules with the specific
#                        module style (EDK or EDKII) specified in [Components] section.
#
###################################################################################################
[BuildOptions.Common.EDK]
!if $(INSYDE_DEBUGGER) == YES
  *_*_IA32_CC_FLAGS    = /D EFI32 $(DSC_GLOBAL_BUILD_OPTIONS) $(CC_FLAGS)
!else
  *_*_IA32_CC_FLAGS    = /D EFI32 $(DSC_GLOBAL_BUILD_OPTIONS) $(CC_FLAGS)
!endif
  *_*_IA32_ASM_FLAGS   = /D EFI32 $(DSC_GLOBAL_BUILD_OPTIONS) $(ASM_FLAGS)
  *_*_IA32_VFRPP_FLAGS = /D EFI32 $(DSC_GLOBAL_BUILD_OPTIONS) $(CC_FLAGS)
  *_*_IA32_APP_FLAGS   = /D EFI32 $(DSC_GLOBAL_BUILD_OPTIONS) $(CC_FLAGS)
  *_*_IA32_PP_FLAGS    = /D EFI32 $(DSC_GLOBAL_BUILD_OPTIONS) $(CC_FLAGS)
  *_*_IA32_ASLPP_FLAGS = /D EFI32 $(DSC_GLOBAL_BUILD_OPTIONS) $(CC_FLAGS)

!if $(INSYDE_DEBUGGER) == YES
  *_*_X64_CC_FLAGS     = /D EFIX64 $(DSC_GLOBAL_BUILD_OPTIONS) $(CC_FLAGS) /wd4028 /wd4031 /wd4334 /wd4718 /wd4090 /wd4213 /D EFI_FIRMWARE_VENDOR="L\"INSYDE Corp.\""  /D MDE_CPU_X64 /Od
!else
  *_*_X64_CC_FLAGS     = /D EFIX64 $(DSC_GLOBAL_BUILD_OPTIONS) $(CC_FLAGS) /wd4028 /wd4031 /wd4334 /wd4718 /wd4090 /wd4213 /D EFI_FIRMWARE_VENDOR="L\"INSYDE Corp.\""  /D MDE_CPU_X64
!endif
  *_*_X64_ASM_FLAGS    = /DEFIX64 $(DSC_GLOBAL_BUILD_OPTIONS) $(ASM_FLAGS)
  *_*_X64_VFRPP_FLAGS  = /D EFIX64 $(DSC_GLOBAL_BUILD_OPTIONS) $(CC_FLAGS)
  *_*_X64_APP_FLAGS    = /D EFIX64 $(DSC_GLOBAL_BUILD_OPTIONS) $(CC_FLAGS)
  *_*_X64_PP_FLAGS     = /D EFIX64 $(DSC_GLOBAL_BUILD_OPTIONS) $(CC_FLAGS)
  *_*_X64_ASLPP_FLAGS  = /D EFIX64 $(DSC_GLOBAL_BUILD_OPTIONS) $(CC_FLAGS)
  *_*_X64_ASLCC_FLAGS  = /D EFIX64 $(DSC_GLOBAL_BUILD_OPTIONS) $(CC_FLAGS)

  *_*_*_VFR_FLAGS   =
  *_*_*_BUILD_FLAGS = -s

[BuildOptions.common.EDKII]
  GCC:RELEASE_*_*_CC_FLAGS       = -DUSE_EDKII_HEADER_FILE -Wno-unused-function -DMDEPKG_NDEBUG -Wno-error
  GCC:DEBUG_*_*_CC_FLAGS         = -DUSE_EDKII_HEADER_FILE -Wno-unused-function -DEFI_DEBUG -Wno-error
  GCC:*_*_IA32_JWASM_FLAGS       =
  GCC:*_*_X64_JWASM_FLAGS        =
  INTEL:*_*_*_CC_FLAGS           = /D MDEPKG_NDEBUG $(CC_FLAGS)
  MSFT:RELEASE_*_*_CC_FLAGS      = /D MDEPKG_NDEBUG $(CC_FLAGS)
  MSFT:DEBUG_*_*_CC_FLAGS        = $(CC_FLAGS)
#[-start-190618-IB06070035-modify]#
!if $(EFI_DEBUG) == NO
  MSFT:DEBUG_*_*_CC_FLAGS        = /D MDEPKG_NDEBUG $(CC_FLAGS)
!else
  MSFT:*_*_*_CC_FLAGS    = /D EFI_DEBUG
!endif
#[-end-190618-IB06070035-modify]#
!if $(INSYDE_DEBUGGER) == YES
  MSFT:DEBUG_*_*_CC_FLAGS        = /Od
!endif
  *_*_*_ASLPP_FLAGS              = $(CC_FLAGS)

!if $(EMULATION_ONLY) == YES
  *_*_*_CC_FLAGS           = -DEMULATION_ONLY
!endif

!if $(BACKUP_BIOS) == YES
  MSFT:*_*_*_CC_FLAGS    = /D BACKUP_BIOS
  GCC:*_*_*_CC_FLAGS     = -DBACKUP_BIOS
!endif