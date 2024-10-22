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

[Defines]
  DEFINE   AGESA_PKG_PATH = AgesaModulePkg
  DEFINE   AGESA_PKG_DEC  = AgesaFp5RvModulePkg
  DEFINE   FCH_PKG_PATH   = $(AGESA_PKG_PATH)/Fch/Sandstone
  DEFINE   FCH_PKG_DEC    = FchSandstone
  DEFINE   CHIPSET_PKG    = PicassoChipsetPkg
!include $(CHIPSET_PKG)/Package.env

[Packages]
  IntelFrameworkPkg/IntelFrameworkPkg.dec|InsydeModulePkg/InsydeModulePkg.dec


[LibraryClasses]
  PostCodeLib|$(CHIPSET_PKG)/Library/BasePostCodeLib/BasePostCodeLib.inf
# PlatformHookLib|$(CHIPSET_PKG)/Library/BasePlatformHookLib/BasePlatformHookLib.inf
# PlatformHookLib|AmdCpmPkg/Library/CommonLib/BasePlatformHookLibAmdSmsc1100/BasePlatformHookLibAmdSmsc1100.inf
  PlatformHookLib|AmdCpmPkg/Library/CommonLib/BasePlatformHookLibAmdFchUart/BasePlatformHookLibAmdFchUart.inf
  TimerLib|$(CHIPSET_PKG)/Library/TscTimerLib/BaseTscTimerLib.inf
  LocalApicLib|$(CHIPSET_PKG)/Library/BaseXApicX2ApicLib/BaseXApicX2ApicLib.inf
  PlatformBdsLib|$(CHIPSET_PKG)/Library/PlatformBdsLib/PlatformBdsLib.inf
  SpiAccessLib|$(CHIPSET_PKG)/Library/FlashDeviceSupport/SpiAccessLib/SpiAccessLib.inf
  FlashWriteEnableLib|$(CHIPSET_PKG)/Library/FlashDeviceSupport/FlashWriteEnableLib/FlashWriteEnableLib.inf
  SpiAccessInitLib|$(CHIPSET_PKG)/Library/FlashDeviceSupport/DxeSpiAccessInitLib/DxeSpiAccessInitLib.inf
  AmdExtCmosLib|$(CHIPSET_PKG)/Library/AmdExtCmosLib/AmdExtCmosLib.inf
  FchInitHookLib|$(CHIPSET_PKG)/Library/FchInitHookLib/FchInitHookLib.inf
  ProcessPendingCapsuleLib|$(CHIPSET_PKG)/Library/ProcessPendingCapsuleLib/BaseProcessPendingCapsuleLib.inf
#[-start-231113-IB19090063-modify]#
  ResetSystemLib|AgesaModulePkg/Library/FchBaseResetSystemLib/FchBaseResetSystemLib.inf
#[-end-231113-IB19090063-modify]#
  #
  # Agesa specific common libraries
  #
#  AgesaPlatformLib|$(CHIPSET_PKG)/Library/AgesaPlatformLib/DefaultAgesaPlatformLibDxe.inf

  #
  # Overrided LibraryClasses
  #
  CmosLib|InsydeModulePkg/Library/CmosLib/CmosLib.inf {
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/InsydeModulePkg/Library/CmosLib
  }


#[-start-220714-IB14860813-add]#
  !disable FchSandstoneDxeLib|AgesaModulePkg/Fch/Sandstone/FchSandstoneCore/FchSandstoneLibDxe.inf
  FchSandstoneDxeLib|AgesaModulePkg/Fch/Sandstone/FchSandstoneCore/FchSandstoneLibDxe.inf {
   <SOURCE_OVERRIDE_PATH>
     $(CHIPSET_PKG)/Override/AgesaModulePkg/Fch/Sandstone/FchSandstoneCore
  }
#[-start-191008-IB06070054-add]#
  CrDeviceVariableLib|InsydeCrPkg/Library/CrDeviceVariableLib/CrDeviceVariableLib.inf {
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/InsydeCrPkg/Library/CrDeviceVariableLib/
  }
#[-end-191008-IB06070054-add]#
#[-start-191108-IB06070061-add]#
  !disable SetupUtilityLib|InsydeModulePkg/Library/SetupUtilityLib/SetupUtilityLib.inf
  SetupUtilityLib|InsydeModulePkg/Library/SetupUtilityLib/SetupUtilityLib.inf {
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/InsydeModulePkg/Library/SetupUtilityLib
  }
#[-end-191108-IB06070061-add]#
#[-start-231113-IB19090063-remove]#
# #[-start-220415-IB14860774-add]#
#   !disable CapsuleLib|InsydeModulePkg/Library/DxeCapsuleLib/DxeCapsuleLib.inf
#   CapsuleLib|InsydeModulePkg/Library/DxeCapsuleLib/DxeCapsuleLib.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Library/DxeCapsuleLib
#   }
#
# !disable GenericBdsLib|InsydeModulePkg/Library/GenericBdsLib/GenericBdsLib.inf
# GenericBdsLib|InsydeModulePkg/Library/GenericBdsLib/GenericBdsLib.inf {
#   <SOURCE_OVERRIDE_PATH>
#     $(CHIPSET_PKG)/Override/InsydeModulePkg/Library/GenericBdsLib
# }
# #[-end-220415-IB14860774-add]#
#[-end-231113-IB19090063-remove]#
#[-end-220714-IB14860813-add]#


#[-start-200120-IB06070077-add]#
  IoDecodeLib|$(CHIPSET_PKG)/Library/AmdIoDecodeLib/AmdIoDecodeLib.inf
#[-end-200120-IB06070077-add]#

#[-start-220316-IB14860767-add]#
  LzmaDecompressLib|MdeModulePkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf {
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/MdeModulePkg/Library/LzmaCustomDecompressLib
  }
  LzmaArchDecompressLib|MdeModulePkg/Library/LzmaCustomDecompressLib/LzmaArchCustomDecompressLib.inf {
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/MdeModulePkg/Library/LzmaCustomDecompressLib
  }
#[-end-220316-IB14860767-add]#
#[-start-231113-IB19090063-modify]#
#[-start-221229-IB14860881-add]#
  DxeFlashRegionLib|InsydeModulePkg/Library/FlashRegionLib/DxeFlashRegionLib/DxeFlashRegionLib.inf
#[-end-221229-IB14860881-add]#
#[-end-231113-IB19090063-modify]#

  !disable SetupUtilityLib|InsydeModulePkg/Library/SetupUtilityLib/SetupUtilityLib.inf
  SetupUtilityLib|InsydeModulePkg/Library/SetupUtilityLib/SetupUtilityLib.inf {
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/InsydeModulePkg/Library/SetupUtilityLib
  }

[LibraryClasses.common.SEC]
  PlatformSecLib|$(CHIPSET_PKG)/Library/PlatformSecLib/PlatformSecLib.inf
  TimerLib|$(CHIPSET_PKG)/Library/TscTimerLib/BaseTscTimerLib.inf

  #
  # Chipset level Oem service libraries
  #
  SecOemSvcChipsetLibDefault|$(CHIPSET_PKG)/Library/SecOemSvcChipsetLib/SecOemSvcChipsetLibDefault.inf
  SecOemSvcChipsetLib|$(CHIPSET_PKG)/Library/SecOemSvcChipsetLib/SecOemSvcChipsetLib.inf

[LibraryClasses.common.PEI_CORE]
  TimerLib|$(CHIPSET_PKG)/Library/TscTimerLib/PeiTscTimerLib.inf

[LibraryClasses.common.PEIM]
  TimerLib|$(CHIPSET_PKG)/Library/TscTimerLib/PeiTscTimerLib.inf
  SmbusLib|MdePkg/Library/PeiSmbusLibSmbus2Ppi/PeiSmbusLibSmbus2Ppi.inf
  KernelConfigLib|$(CHIPSET_PKG)/Library/PeiKernelConfigLib/PeiKernelConfigLib.inf
  ChipsetConfigLib|$(CHIPSET_PKG)/Library/ChipsetConfigLib/PeiChipsetConfigLib.inf
  AmdExtCmosLib|$(CHIPSET_PKG)/Library/AmdExtCmosLib/AmdExtCmosLib.inf

  #
  # Agesa specific common.PEIM libraries
  #
  #AgesaPlatformLib|$(CHIPSET_PKG)/Library/AgesaPlatformLib/DefaultAgesaPlatformLibPei.inf
  SCUVarInfoLib|$(CHIPSET_PKG)/Library/PeiSCUVarInfoLib/PeiSCUVarInfoLib.inf
#[-start-220714-IB14860813-add]#
  !disable FchSandstonePeiLib|AgesaModulePkg/Fch/Sandstone/FchSandstoneCore/FchSandstoneLibPei.inf
  FchSandstonePeiLib|AgesaModulePkg/Fch/Sandstone/FchSandstoneCore/FchSandstoneLibPei.inf {
   <SOURCE_OVERRIDE_PATH>
     $(CHIPSET_PKG)/Override/AgesaModulePkg/Fch/Sandstone/FchSandstoneCore
  }
#[-end-220714-IB14860813-add]#

[LibraryClasses.common.DXE_CORE]
  TimerLib|$(CHIPSET_PKG)/Library/TscTimerLib/DxeTscTimerLib.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  TimerLib|$(CHIPSET_PKG)/Library/TscTimerLib/DxeTscTimerLib.inf
#[-start-231113-IB19090063-modify]#
#[-start-221031-IB14860859-add]#
  PcdLib|InsydeModulePkg/Library/SmmDxePcdLib/SmmDxePcdLib.inf
#[-end-221031-IB14860859-add]#
#[-end-231113-IB19090063-modify]#

[LibraryClasses.common.UEFI_DRIVER]
  DebugPrintErrorLevelLib|InsydeModulePkg/Library/DxeDebugPrintErrorLevelLib/DxeDebugPrintErrorLevelLib.inf
  TimerLib|$(CHIPSET_PKG)/Library/TscTimerLib/DxeTscTimerLib.inf

[LibraryClasses.common.DXE_DRIVER]
  TimerLib|$(CHIPSET_PKG)/Library/TscTimerLib/DxeTscTimerLib.inf
  ChipsetConfigLib|$(CHIPSET_PKG)/Library/ChipsetConfigLib/DxeChipsetConfigLib.inf
  EfiRegTableLib|$(CHIPSET_PKG)/Library/EfiRegTableLib/EfiRegTableLib.inf

  #
  # Chipset level Oem service libraries
  #
  DxeOemSvcChipsetLibDefault|$(CHIPSET_PKG)/Library/DxeOemSvcChipsetLib/DxeOemSvcChipsetLibDefault.inf
  DxeOemSvcChipsetLib|$(CHIPSET_PKG)/Library/DxeOemSvcChipsetLib/DxeOemSvcChipsetLib.inf

  SCUVarInfoLib|$(CHIPSET_PKG)/Library/DxeSCUVarInfoLib/DxeSCUVarInfoLib.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  SmmLib|$(CHIPSET_PKG)/Library/FchSmmLib/FchSmmLib.inf
#  SmmChipsetSvcLib|$(CHIPSET_PKG)/Library/SmmChipsetSvcLib/SmmChipsetSvcLib.inf
  #
  # Chipset level Oem service libraries
  #
  SmmOemSvcChipsetLibDefault|$(CHIPSET_PKG)/Library/SmmOemSvcChipsetLib/SmmOemSvcChipsetLibDefault.inf
  SmmOemSvcChipsetLib|$(CHIPSET_PKG)/Library/SmmOemSvcChipsetLib/SmmOemSvcChipsetLib.inf

#[-start-220825-IB14860831-add]#
  # Fix CVE issue (CVE-2017-5715)(ITS98862)
  SmmCpuFeaturesLib|UefiCpuPkg/Library/SmmCpuFeaturesLib/SmmCpuFeaturesLib.inf {
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/UefiCpuPkg/Library/SmmCpuFeaturesLib
  }
#[-end-220825-IB14860831-add]#
#[-start-231113-IB19090063-modify]#
#[-start-221031-IB14860859-add]#
  PcdLib|InsydeModulePkg/Library/SmmDxePcdLib/SmmDxePcdLib.inf
#[-end-221031-IB14860859-add]#
#[-end-231113-IB19090063-modify]#

[LibraryClasses.common.COMBINED_SMM_DXE]
#[-start-231113-IB19090063-modify]#
#[-start-221031-IB14860859-add]#
  PcdLib|InsydeModulePkg/Library/SmmDxePcdLib/SmmDxePcdLib.inf
#[-end-221031-IB14860859-add]#
#[-end-231113-IB19090063-modify]#

[LibraryClasses.common.SMM_CORE]
  #
  # due to DDT will restart when boot into OS and in SMM trace code,
  # so stop WDT when in SMM.
  #
!if $(INSYDE_DEBUGGER) == YES
  SmmCorePlatformHookLib|$(CHIPSET_PKG)/Library/SmmCorePlatformHookLib/SmmCorePlatformHookLib.inf
!endif
[LibraryClasses.common.UEFI_APPLICATION]
  TimerLib|$(CHIPSET_PKG)/Library/TscTimerLib/DxeTscTimerLib.inf

[PcdsFeatureFlag]
  gInsydeTokenSpaceGuid.PcdH2OBdsCpUpdateAcpiDescHdrSupported|TRUE
  gInsydeTokenSpaceGuid.PcdFvbAccessThroughSmi|TRUE

[PcdsFixedAtBuild]
  #
  # CPU SMBIOS (Type4, 7)
  #
  gEfiCpuTokenSpaceGuid.PcdCpuSocketName|"Socket FP4"
  gEfiCpuTokenSpaceGuid.PcdCpuSerialNumber|"Unknown"
  gEfiCpuTokenSpaceGuid.PcdCpuAssetTag|"Unknown"
  gEfiCpuTokenSpaceGuid.PcdCpuPartNumber|"Unknown"

[PcdsFixedAtBuild]

  gInsydeTokenSpaceGuid.PcdTemporaryRamSizeWhenRecovery|0x10000

  gInsydeTokenSpaceGuid.PcdSoftwareSmiPort|0xB0
  gInsydeTokenSpaceGuid.PcdSmBusBaseAddress|0x0B00
  gInsydeTokenSpaceGuid.PcdSmBusBusDevFunc|0x140000             # bit[31:24] : bus, bit[23:16] : device, bit[15:8] : function, bit[0:7] : register
  #
  # CMOS Debug Code Table, wait for implementation
  #
  gInsydeTokenSpaceGuid.PcdPeiChipsetDebugCodeTable|{0}
  gInsydeTokenSpaceGuid.PcdDxeChipsetDebugCodeTable|{0}
  gInsydeTokenSpaceGuid.PcdMemorySpaceSize|48
  gEfiMdeModulePkgTokenSpaceGuid.PcdUse1GPageTable|TRUE
  #
  # AMD I/O Virtualization Technology (IOMMU) is not supported on
  # non-FM2 APU (FS1r2 and FP2). It is only supported on Trinity
  # FM2 APU (Virgo).
  # Therefore, all of the PCI-SIG I/O Virtualizations need to be
  # turned off to avoid a system hang on PC 0x0213.
  #
  gEfiMdeModulePkgTokenSpaceGuid.PcdSrIovSupport|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdAriSupport|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdMrIovSupport|FALSE
  #
  # SEC Padding for EC boot image descriptor
  # which base is addressed as 0xFFFF.FF00
  #
  gEfiCpuTokenSpaceGuid.PcdSecPad1Raw32|0xFFFF0000
  #
  # Pcie MMIO range
  # Auto arrange PciExpressBaseAddress according to PcdPciExpressSize
  #
!ifdef $(PCI_EXPRESS_SIZE)
  gChipsetPkgTokenSpaceGuid.PcdPciExpressSize|$(PCI_EXPRESS_SIZE)
!else
  gChipsetPkgTokenSpaceGuid.PcdPciExpressSize|0x4000000
!endif

!if gChipsetPkgTokenSpaceGuid.PcdPciExpressSize == 0x4000000
  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress|0xF8000000
!elseif gChipsetPkgTokenSpaceGuid.PcdPciExpressSize == 0x8000000
  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress|0xF0000000
!elseif gChipsetPkgTokenSpaceGuid.PcdPciExpressSize == 0x10000000
  gEfiMdePkgTokenSpaceGuid.PcdPciExpressBaseAddress|0xE0000000
!endif

  #
  # AmdCommonPcd
  #
  gAmdCommonChipsetPkgTokenSpaceGuid.PcdAzaliaDev|0x09
  gAmdCommonChipsetPkgTokenSpaceGuid.PcdAzaliaFunc|0x02
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdCfgIommuMMIOAddressReservedEnable|TRUE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdBottomMmioReservedForPrimaryRb|0xFEC00000

  # Increase S3 ACPI reserve memory size to 2MB
  gEfiIntelFrameworkModulePkgTokenSpaceGuid.PcdS3AcpiReservedMemorySize|0x200000
!if gInsydeTokenSpaceGuid.PcdH2ODdtSupported == 1
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseSerial|FALSE
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdIdsDebugPrintSerialPortEnable|FALSE
!endif

  gEfiAmdAgesaPkgTokenSpaceGuid.PcdFchOemBeforePciRestoreSwSmi|0xEA
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdFchOemAfterPciRestoreSwSmi|0xEB
  gAmdCpmPkgTokenSpaceGuid.AcpiDisableDgpuAudioSwSmi|0xED

  ## Stack size in the temporary RAM.
  #   0 means half of PcdTemporaryRamSizeWhenRecovery.
  gInsydeTokenSpaceGuid.PcdPeiTemporaryRamStackSizeWhenRecovery|0x8000

!if $(RAVEN_16M) == YES
  gChipsetPkgTokenSpaceGuid.PcdFchLimitProtectSizePerSet|0x1000000
!else
  gChipsetPkgTokenSpaceGuid.PcdFchLimitProtectSizePerSet|0x800000
!endif

  ## Specifies the initial value for Register_D in RTC.
  # @Prompt Initial value for Register_D in RTC.
  gPcAtChipsetPkgTokenSpaceGuid.PcdInitialValueRtcRegisterD|0x80

#
# Chipset value of reserved memroy size for secure flash (Kernel Default: 9MB)
#
!if $(RAVEN_16M) == YES
  gInsydeTokenSpaceGuid.PcdReservedMemorySizeForSecureFlash|0x01200000
!else
  gInsydeTokenSpaceGuid.PcdReservedMemorySizeForSecureFlash|0x00A00000
!endif

  #
  # For Support Sata Freeze feature (Related with Chipset ASL code)
  #
  gChipsetPkgTokenSpaceGuid.PcdSataFreezeByAsl|FALSE

  #
  # For Clear CMOS Load Default Support
  #
  gChipsetPkgTokenSpaceGuid.PcdH2OCmosLoadDefaultSupported|TRUE

[PcdsDynamicDefault]
  #
  # Preserve Memory Table
  #
  gInsydeTokenSpaceGuid.PcdPreserveMemoryTable|{ \
     UINT32(0x09), UINT32(0x80  ),  \ # Preserve 512K(0x80 pages) for ASL
     UINT32(0x0A), UINT32(0x8F0 ),  \ # Preserve 2M(0x200 pages) for S3, SMM, etc
     UINT32(0x00), UINT32(0x4F0 ),  \ # Preserve 4M(0x400 pages) for BIOS reserved memory
     UINT32(0x06), UINT32(0x880 ),  \ # Preserve 5.75M(0x5C0 pages) for UEFI OS runtime data to make S4 memory consistency
     UINT32(0x05), UINT32(0x3D0 ),  \ # Preserve 3.8M(0x3D0 pages) for UEFI OS runtime drivers to make S4 memory consistency
     UINT32(0x03), UINT32(0xE000),  \ # Preserve 224M(0xE000 pages) for boot service drivers to reduce memory fragmental
     UINT32(0x04), UINT32(0x8000),  \ # Preserve 128M(0x8000 pages) for boot service data to reduce memory fragmental
     UINT32(0x01), UINT32(0x200 ),  \ # Preserve 2M(0x200 pages) for UEFI OS boot loader to keep on same address
     UINT32(0x02), UINT32(0x00  ),  \
     UINT32(0x0e), UINT32(0x00  )}    #EndEntry

  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdFchWdtEnable|TRUE
[PcdsDynamicExDefault]
  #
  # Port number mapping table Define
  #
  gInsydeTokenSpaceGuid.PcdPortNumberMapTable|{ \
0x00, 0x11, 0x00, 0, 0, 0x00, \
0x00, 0x11, 0x00, 0, 1, 0x02, \
0x00, 0x11, 0x00, 1, 0, 0x01, \
0x00, 0x11, 0x00, 1, 1, 0x03, \
0x00, 0x14, 0x01, 0, 0, 0x04, \
0x00, 0x14, 0x01, 0, 1, 0x05, \
0x00, 0x14, 0x01, 1, 0, 0x06, \
0x00, 0x14, 0x01, 1, 1, 0x07, \
0x00, 0x00, 0x00, 0, 0, 0x00} #EndEntry

  # PIRQ link value                                PIRQA, PIRQB, PIRQC, PIRQD, PIRQE, PIRQF, PIRQG, PIRQH
  gChipsetPkgTokenSpaceGuid.PcdPirqLinkValueArray|{0x00,  0x01,  0x02,  0x03,  0x04,  0x05,  0x06,  0x07} #EndEntry                                                 0x00, 0x01, 0x02, 0x03, \

  # List all bridges in the system.
  # The ones that require a bus fix up, you need to put the multibus attribute on them.
  # Also, to support a bridge behind a bridge behind a root bridge, the order IS important.
  # The parent bridges must come first in the table.
  gChipsetPkgTokenSpaceGuid.PcdVirtualBusTable|{  0x00, 0x01, 0x01, 0x01, \
                                                  0x00, 0x01, 0x02, 0x02, \
                                                  0x00, 0x01, 0x03, 0x03, \
                                                  0x00, 0x01, 0x04, 0x04, \
                                                  0x00, 0x01, 0x05, 0x05, \
                                                  0x00, 0x01, 0x06, 0x06, \
                                                  0x00, 0x01, 0x07, 0x07, \
                                                  0x00, 0x02, 0x01, 0x08, \
                                                  0x00, 0x03, 0x01, 0x09, \
                                                  0x00, 0x03, 0x02, 0x0A, \
                                                  0x00, 0x03, 0x03, 0x0B, \
                                                  0x00, 0x03, 0x04, 0x0C, \
                                                  0x00, 0x03, 0x05, 0x0D, \
                                                  0x00, 0x03, 0x06, 0x0E, \
                                                  0x00, 0x03, 0x07, 0x0F, \
                                                  0x00, 0x04, 0x01, 0x10, \
                                                  0x00, 0x07, 0x01, 0x11, \
                                                  0x00, 0x08, 0x01, 0x12} #EndEntry

#0x80 => Dev 0x10
#0x88 => Dev 0x11
#0x90 => Dev 0x12
#0xA0 => Dev 0x14

#0x08 => Dev 0x01 IGD
#0x10 => Dev 0x02
#0x18 => Dev 0x03
#0x20 => Dev 0x04

#0x38 => Dev 0x07
#0x40 => Dev 0x08

#0x48 => Dev 0x09

  #
  #Bus, Dev,  INT#A,IrqMask,        INT#B,IrqMask,        INT#C,IrqMask,        INT#D,IrqMask,        Slot, Reserved, DevIpRegValue, ProgrammableIrq.
  #
  gChipsetPkgTokenSpaceGuid.PcdControllerDeviceIrqRoutingEntry| { \
  0x00, 0x08, 0x00, UINT16(0xDC90), 0x01, UINT16(0xDC90), 0x02, UINT16(0xDC90), 0x03, UINT16(0xDC90), 0x00, 0x00, UINT32(0x00000000), 0x00, \
  0x00, 0x40, 0x03, UINT16(0xDC90), 0x04, UINT16(0xDC90), 0x00, UINT16(0xDC90), 0x00, UINT16(0xDC90), 0x00, 0x00, UINT32(0x00000000), 0x00, \
  0x00, 0xA0, 0x00, UINT16(0xDC90), 0x01, UINT16(0xDC90), 0x02, UINT16(0xDC90), 0x03, UINT16(0xDC90), 0x00, 0x00, UINT32(0x00000000), 0x00, \

  0x01, 0x00, 0x00, UINT16(0xDC90), 0x01, UINT16(0xDC90), 0x02, UINT16(0xDC90), 0x03, UINT16(0xDC90), 0x01, 0xFF, UINT32(0x00000000), 0x00, \
  0x02, 0x00, 0x04, UINT16(0xDC90), 0x05, UINT16(0xDC90), 0x06, UINT16(0xDC90), 0x07, UINT16(0xDC90), 0x02, 0xFF, UINT32(0x00000000), 0x00, \
  0x03, 0x00, 0x00, UINT16(0xDC90), 0x01, UINT16(0xDC90), 0x02, UINT16(0xDC90), 0x03, UINT16(0xDC90), 0x03, 0xFF, UINT32(0x00000000), 0x00, \
  0x04, 0x00, 0x04, UINT16(0xDC90), 0x05, UINT16(0xDC90), 0x06, UINT16(0xDC90), 0x07, UINT16(0xDC90), 0x04, 0xFF, UINT32(0x00000000), 0x00, \
  0x05, 0x00, 0x00, UINT16(0xDC90), 0x01, UINT16(0xDC90), 0x02, UINT16(0xDC90), 0x03, UINT16(0xDC90), 0x05, 0xFF, UINT32(0x00000000), 0x00, \
  0x06, 0x00, 0x04, UINT16(0xDC90), 0x05, UINT16(0xDC90), 0x06, UINT16(0xDC90), 0x07, UINT16(0xDC90), 0x06, 0xFF, UINT32(0x00000000), 0x00, \
  0x07, 0x00, 0x00, UINT16(0xDC90), 0x01, UINT16(0xDC90), 0x02, UINT16(0xDC90), 0x03, UINT16(0xDC90), 0x07, 0xFF, UINT32(0x00000000), 0x00, \

  0x08, 0x00, 0x04, UINT16(0xDC90), 0x05, UINT16(0xDC90), 0x06, UINT16(0xDC90), 0x07, UINT16(0xDC90), 0x08, 0xFF, UINT32(0x00000000), 0x00, \
  0x09, 0x00, 0x06, UINT16(0xDC90), 0x07, UINT16(0xDC90), 0x04, UINT16(0xDC90), 0x05, UINT16(0xDC90), 0x09, 0xFF, UINT32(0x00000000), 0x00} #EndEntry

  gChipsetPkgTokenSpaceGuid.PcdIrqPoolTable|{03, 0x00, \ #IRQ03
                                             05, 0x00, \ #IRQ05
                                             06, 0x00, \ #IRQ06
                                             10, 0x00, \ #IRQ10
                                             11, 0x00, \ #IRQ11
                                             00, 0x00}   #IRQ00

  gChipsetPkgTokenSpaceGuid.PcdPirqPriorityTable|{0,  \# PIRQ A
                                                  0,  \# PIRQ B
                                                  0,  \# PIRQ C
                                                  10, \# PIRQ D
                                                  0,  \# PIRQ E
                                                  0,  \# PIRQ F
                                                  0,  \# PIRQ G
                                                  0}   # PIRQ H

  ## UpdateOemTableID.c
  #
  #  PcdOemId that is updated by Oem and max length is 6 bytes
  #  PcdOemTableId that is updated by Oem and max length is 8 bytes
  #
  gEfiMdeModulePkgTokenSpaceGuid.PcdAcpiDefaultOemId|"INSYDE"|VOID*|0x00000037

[PcdsDynamicExDefault]
  gH2OFlashDeviceMfrNameGuid.PcdMxic25l6436eSpiMfrName|"Mxic"
  gH2OFlashDevicePartNameGuid.PcdMxic25l6436eSpiPartName|"25L6436E"
  gH2OFlashDeviceGuid.PcdMxic25l6436eSpi|{ \ # Mxic  25L6436E
    0x03, 0x00, 0x00, 0x00, 0xc2, 0x20, 0x17, 0x00,  \  # DeviceType = 03  Id = 001720C2
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x08   \  # ExtId = 00000000  BlockSize = 0010  Multiple = 0800
  }

  gH2OFlashDeviceConfigGuid.PcdMxic25l6436eSpiConfig|{ \ # Mxic  25L6436E
    0x28, 0x00, 0x00, 0x00, 0x9f, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 9F  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7c, 0xf2, 0x06, 0x06,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27C  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 06
    0x01, 0x00, 0x00, 0x01, 0x03, 0x01, 0x01, 0x00,  \  # GlobalProtectAvailable = 01  BLockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 01  BytesOfId = 03  MinBytePerProgRead = 01  NoVolStatusAvailable = 01
    0x3c, 0xc3, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00,  \  # GlobalProtectCode = 3C  GlobalUnprotectCode = C3  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 00800000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

  gH2OFlashDeviceMfrNameGuid.PcdMxic25u6435fSpiMfrName|"Mxic"
  gH2OFlashDevicePartNameGuid.PcdMxic25u6435fSpiPartName|"25U6435F"
  gH2OFlashDeviceGuid.PcdMxic25u6435fSpi|{ \ # Mxic  25U6435F
    0x03, 0x00, 0x00, 0x00, 0xc2, 0x25, 0x37, 0x00,  \  # DeviceType = 03  Id = 001720C2
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x08   \  # ExtId = 00000000  BlockSize = 0010  Multiple = 0800
  }

  gH2OFlashDeviceConfigGuid.PcdMxic25u6435fSpiConfig|{ \ # Mxic  25L6436E
    0x28, 0x00, 0x00, 0x00, 0x9f, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 9F  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7c, 0xf2, 0x06, 0x06,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27C  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 06
    0x01, 0x00, 0x00, 0x01, 0x03, 0x01, 0x01, 0x00,  \  # GlobalProtectAvailable = 01  BLockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 01  BytesOfId = 03  MinBytePerProgRead = 01  NoVolStatusAvailable = 01
    0x3c, 0xc3, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00,  \  # GlobalProtectCode = 3C  GlobalUnprotectCode = C3  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 00800000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

  gH2OFlashDeviceMfrNameGuid.PcdMxic25u12835fSpiMfrName|"MXIC"
  gH2OFlashDevicePartNameGuid.PcdMxic25u12835fSpiPartName|"25U12835F"
  gH2OFlashDeviceGuid.PcdMxic25u12835fSpi|{ \ # MXIC  25U12835F
    0x03, 0x00, 0x00, 0x00, 0xc2, 0x25, 0x38, 0x00,  \  # DeviceType = 03  Id = 003825C2
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x10   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 1000
  }

  gH2OFlashDeviceConfigGuid.PcdMxic25u12835fSpiConfig|{ \ # MXIC  25U12835F
    0x28, 0x00, 0x00, 0x00, 0x9f, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 9F  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7c, 0xf2, 0x06, 0x06,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27C  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 06
    0x00, 0x00, 0x00, 0x01, 0x03, 0x01, 0x01, 0x00,  \  # GlobalProtectAvailable = 00  BlockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 01  BytesOfId = 03  MinBytePerProgRead = 01  NoVolStatusAvailable = 01
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,  \  # GlobalProtectCode = 00  GlobalUnprotectCode = 00  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 00800000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

#[-start-231004-IB18160156-add]#
  gH2OFlashDeviceMfrNameGuid.PcdMxic77u12851fSpiMfrName|"MXIC"
  gH2OFlashDevicePartNameGuid.PcdMxic77u12851fSpiPartName|"77U12851F"
  gH2OFlashDeviceGuid.PcdMxic77u12851fSpi|{ \ # MXIC  77U12851F
    0x03, 0x00, 0x00, 0x00, 0xc2, 0x75, 0x38, 0x00,  \  # DeviceType = 03  Id = 003825C2
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x10   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 1000
  }

  gH2OFlashDeviceConfigGuid.PcdMxic77u12851fSpiConfig|{ \ # MXIC  77U12851F
    0x28, 0x00, 0x00, 0x00, 0x9f, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 9F  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7c, 0xf2, 0x06, 0x06,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27C  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 06
    0x00, 0x00, 0x00, 0x01, 0x03, 0x01, 0x01, 0x00,  \  # GlobalProtectAvailable = 00  BlockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 01  BytesOfId = 03  MinBytePerProgRead = 01  NoVolStatusAvailable = 01
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,  \  # GlobalProtectCode = 00  GlobalUnprotectCode = 00  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 01000000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

  gH2OFlashDeviceMfrNameGuid.PcdMxic77u25650fSpiMfrName|"MXIC"
  gH2OFlashDevicePartNameGuid.PcdMxic77u25650fSpiPartName|"77U25650F"
  gH2OFlashDeviceGuid.PcdMxic77u25650fSpi|{ \ # MXIC  77U25650F
    0x03, 0x00, 0x00, 0x00, 0xc2, 0x75, 0x39, 0x00,  \  # DeviceType = 03  Id = 003975C2
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x20   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 2000
  }

  gH2OFlashDeviceConfigGuid.PcdMxic77u25650fSpiConfig|{ \ # MXIC  77U25650F
    0x28, 0x00, 0x00, 0x00, 0x9f, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 9F  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7c, 0xf2, 0x06, 0x06,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27C  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 06
    0x00, 0x00, 0x00, 0x01, 0x03, 0x01, 0x01, 0x00,  \  # GlobalProtectAvailable = 00  BlockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 01  BytesOfId = 03  MinBytePerProgRead = 01  NoVolStatusAvailable = 01
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,  \  # GlobalProtectCode = 00  GlobalUnprotectCode = 00  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 02000000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }
#[-end-231004-IB18160156-add]#

  gH2OFlashDeviceMfrNameGuid.PcdMicronN25q128aSpiMfrName|"MICRON"
  gH2OFlashDevicePartNameGuid.PcdMicronN25q128aSpiPartName|"N25Q128A"
  gH2OFlashDeviceGuid.PcdMicronN25q128aSpi|{ \ # MICRON  N25Q128A
    0x03, 0x00, 0x00, 0x00, 0x20, 0xbb, 0x18, 0x00,  \  # DeviceType = 03  Id = 0018BB20
    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x10   \  # ExtId = 00000000  BlockSize = 0010  BlockCount = 1000
  }

  gH2OFlashDeviceConfigGuid.PcdMicronN25q128aSpiConfig|{ \ # MICRON  N25Q128A
    0x28, 0x00, 0x00, 0x00, 0x9f, 0x20, 0x02, 0x01,  \  # Size = 00000028  ReadIdOp = 9F  EraseOp = 20  WriteOp = 02  WriteStatusOp = 01
    0x03, 0x05, 0x00, 0x00, 0x7c, 0xf2, 0x06, 0x06,  \  # ReadOp = 03  ReadStatusOp = 05  OpType = F27C  WriteEnablePrefix = 06  WriteStatusEnablePrefix = 06
    0x00, 0x00, 0x00, 0x01, 0x03, 0x01, 0x01, 0x00,  \  # GlobalProtectAvailable = 00  BlockProtectAvailable = 00  BlockProtectCodeRequired = 00  MultiByteProgramAvailable = 01  BytesOfId = 03  MinBytePerProgRead = 01  NoVolStatusAvailable = 01
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,  \  # GlobalProtectCode = 00  GlobalUnprotectCode = 00  BlockProtectCode = 00  BlockUnprotectCode = 00  DeviceSize = 01000000
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00   \  # BlockEraseSize = 00001000  BlockProtectSize = 00000000
  }

  #
  # The PCDs are used to control the Windows SMM Security Mitigations Table - Protection Flags
  #
  # BIT0: FIXED_COMM_BUFFERS, If set, expresses that for all synchronous SMM entries,SMM will validate that input and output buffers lie entirely within the expected fixed memory regions.
  # BIT1: COMM_BUFFER_NESTED_PTR_PROTECTION. If set, expresses that for all synchronous SMM entries, SMM will validate that input and output pointers embedded within the fixed communication buffer only refer to address ranges \
  #       that lie entirely within the expected fixed memory regions.
  # BIT2: SYSTEM_RESOURCE_PROTECTION, Firmware setting this bit is an indication that it will not allow reconfiguration of system resources via non-architectural mechanisms.
  # BIT3-31: Reserved, must return 0 when read.
  #
  gChipsetPkgTokenSpaceGuid.PcdWsmtProtectionFlags|0x07

[Components.$(PEI_ARCH)]
  #
  # Helper tool modules
  #
  $(CHIPSET_PKG)/Tools/GenOemRomSig/GenOemRomSig.inf
#  $(CHIPSET_PKG)/Tools/GenGloblNvs/GenGloblNvs.inf

  $(CHIPSET_PKG)/DisableCacheAsRamPei/DisableCacheAsRamPeiBin.inf
  $(CHIPSET_PKG)/ChipsetSvcPei/ChipsetSvcPei.inf
  $(CHIPSET_PKG)/PlatformSpdReadPei/PlatformSpdReadPei.inf
  $(CHIPSET_PKG)/MemoryIntiPei/CpuInstallEfiMemoryPei.inf
  $(CHIPSET_PKG)/MemoryIntiPei/CrisisMemoryInitPei.inf
#  $(CHIPSET_PKG)/SioInitPei/SioInitPei.inf
  $(CHIPSET_PKG)/SmmAccessPei/SmmAccessPei.inf
  $(CHIPSET_PKG)/SmmControlPei/SmmControlPei.inf
  $(CHIPSET_PKG)/S3NotifyDispatchPei/S3NotifyDispatchPei.inf
#  $(CHIPSET_PKG)/PlatformNotifyDispatchPei/PlatformNotifyDispatchPei.inf
!if gChipsetPkgTokenSpaceGuid.PcdH2OCmosLoadDefaultSupported
  $(CHIPSET_PKG)/NvVariableFailDetectPei/NvVariableFailDetectPei.inf
!endif
!if gInsydeTokenSpaceGuid.PcdCrisisRecoverySupported
  $(CHIPSET_PKG)/RecoveryCpuInitPei/RecoveryCpuInitPei.inf
  $(CHIPSET_PKG)/RecoveryFchInitPei/RecoveryFchInitPei.inf
!endif
  $(CHIPSET_PKG)/EarlySetPlatformHardwareSwitchPei/EarlySetPlatformHardwareSwitchPei.inf
!if gInsydeTokenSpaceGuid.PcdMultiConfigSupported
  $(CHIPSET_PKG)/MultiConfigVarHookPei/MultiConfigVarHookPei.inf
!endif
#[-start-181114-IB06070004-remove]#
#!if gInsydeTokenSpaceGuid.PcdH2ODdtSupported
#  $(CHIPSET_PKG)/FchUartInitPei/FchUartInitPei.inf
#!endif
#[-end-181114-IB06070004-remove]#

  #
  # Compatibility
  #
  $(CHIPSET_PKG)/Compatibility/AcpiVariableHobOnSmramReserveHobThunk/AcpiVariableHobOnSmramReserveHobThunk.inf

################################################################################
#
# Components.IA32 Override
#
################################################################################
  #
  # TPM2 Pei module dependency RC code FchInitReset finish. So Change TPM2pei.inf Dex same as TcgPei.inf
  #
  !disable InsydeModulePkg/Universal/Security/Tcg/TisPei/TisPei.inf
!if gInsydeTokenSpaceGuid.PcdH2OTpm2Supported
  InsydeModulePkg/Universal/Security/Tcg/TisPei/TisPei.inf {
    <LibraryClasses>
      NULL|$(CHIPSET_PKG)/Library/PeiTisDepexLib/PeiTisDepexLib.inf
  }

!endif

!disable InsydeModulePkg/Core/DxeIplPeim/DxeIpl.inf
  InsydeModulePkg/Core/DxeIplPeim/DxeIpl.inf {
    <LibraryClasses>
      NULL|MdeModulePkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/InsydeModulePkg/Core/DxeIplPeim/
  }

  !disable InsydeModulePkg/Universal/Recovery/CrisisRecoveryPei/CrisisRecoveryPei.inf
  InsydeModulePkg/Universal/Recovery/CrisisRecoveryPei/CrisisRecoveryPei.inf {
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/Recovery/CrisisRecoveryPei/
  }

#[-start-220714-IB14860813-add]#
#[-start-181114-IB06070004-add]#
!if $(INSYDE_DEBUGGER) == YES and $(H2O_DDT_DEBUG_IO) == Com
  !disable InsydeModulePkg/H2ODebug/$(H2O_DDT_DEBUG_IO)DebugIoPei/$(H2O_DDT_DEBUG_IO)DebugIoPei.inf
  $(CHIPSET_PKG)/Override/InsydeModulePkg/H2ODebug/$(H2O_DDT_DEBUG_IO)DebugIoPei/$(H2O_DDT_DEBUG_IO)DebugIoPei.inf
!endif
#[-end-181114-IB06070004-add]#

#[-start-231113-IB19090063-remove]#
# #[-start-190725-IB06070044-add]#
# !if gInsydeTokenSpaceGuid.PcdH2ONvmeSupported
#   !disable InsydeModulePkg/Bus/Pci/NvmExpressPei/NvmExpressPei.inf
#   $(CHIPSET_PKG)/Override/InsydeModulePkg/Bus/Pci/NvmExpressPei/NvmExpressPei.inf
# !endif
# #[-end-190725-IB06070044-add]#
#
# #[-start-191115-IB06070063-add]#
# !if gInsydeTokenSpaceGuid.PcdH2OUsbPeiSupported
#   !disable InsydeModulePkg/Bus/Pci/EhciPei/EhciPei.inf
#   $(CHIPSET_PKG)/Override/InsydeModulePkg/Bus/Pci/EhciPei/EhciPei.inf
#
#   !disable InsydeModulePkg/Bus/Pci/XhciPei/XhciPei.inf
#   $(CHIPSET_PKG)/Override/InsydeModulePkg/Bus/Pci/XhciPei/XhciPei.inf
#
#   !disable InsydeModulePkg/Bus/Usb/UsbBusPei/UsbBusPei.inf
#   InsydeModulePkg/Bus/Usb/UsbBusPei/UsbBusPei.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Bus/Usb/UsbBusPei
#   }
#
#   !disable InsydeModulePkg/Bus/Usb/UsbMassStoragePei/UsbMassStoragePei.inf
#   InsydeModulePkg/Bus/Usb/UsbMassStoragePei/UsbMassStoragePei.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Bus/Usb/UsbMassStoragePei
#   }
# !endif
# #[-end-191115-IB06070063-add]#
#[-end-231113-IB19090063-remove]#
# #[-end-220714-IB14860813-add]#
#[-start-231113-IB19090063-remove]#
# #[-start-220927-IB14860848-add]#
#   !disable InsydeModulePkg/Universal/RemapIommuPei/RemapIommuPei.inf
#   InsydeModulePkg/Universal/RemapIommuPei/RemapIommuPei.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/RemapIommuPei
#   }
# #[-end-220927-IB14860848-add]#
# #[-start-230220-IB14860898-add]#
# !if gInsydeTokenSpaceGuid.PcdH2OTpm2Supported
#   !disable InsydeModulePkg/Universal/Security/Tcg/Tcg2Pei/Tcg2Pei.inf
#   InsydeModulePkg/Universal/Security/Tcg/Tcg2Pei/Tcg2Pei.inf {
#     <LibraryClasses>
#       NULL|SecurityPkg/Library/HashInstanceLibSha1/HashInstanceLibSha1.inf
#       NULL|SecurityPkg/Library/HashInstanceLibSha256/HashInstanceLibSha256.inf
#       NULL|SecurityPkg/Library/HashInstanceLibSha384/HashInstanceLibSha384.inf
#       NULL|SecurityPkg/Library/HashInstanceLibSha512/HashInstanceLibSha512.inf
#       NULL|InsydeModulePkg/Library/HashInstanceLibSm3/HashInstanceLibSm3.inf
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/Security/Tcg/Tcg2Pei
#   }
# !endif
# #[-end-230220-IB14860898-add]#
#[-end-231113-IB19090063-remove]#


[Components.$(DXE_ARCH)]
  $(CHIPSET_PKG)/ChipsetSvcDxe/ChipsetSvcDxe.inf
  $(CHIPSET_PKG)/ChipsetSvcSmm/ChipsetSvcSmm.inf
  $(CHIPSET_PKG)/LegacyRegion2Dxe/LegacyRegion2Dxe.inf
  $(CHIPSET_PKG)/PciHostBridgeDxe/PciHostBridgeDxe.inf {
    <LibraryClasses>
      SmbusLib|MdePkg/Library/BaseSmbusLibNull/BaseSmbusLibNull.inf
  }
  $(CHIPSET_PKG)/I2cBus/Dxe/I2cMasterDxe.inf
  $(CHIPSET_PKG)/SataControllerDxe/SataControllerDxe.inf
  $(CHIPSET_PKG)/SmmAccess2Dxe/SmmAccess2Dxe.inf
  $(CHIPSET_PKG)/OemBadgingSupportDxe/OEMBadgingSupportDxe.inf
  $(CHIPSET_PKG)/UnexpectedIrqWADxe/UnexpectedIrqWA.inf
  $(CHIPSET_PKG)/PlatformSmm/PlatformSmm.inf
  $(CHIPSET_PKG)/SmmIhisi/IhisiSmm.inf
  $(CHIPSET_PKG)/AcpiTables/AcpiTables.inf
  $(CHIPSET_PKG)/UefiSetupUtilityDxe/SetupUtilityDxe.inf
  $(CHIPSET_PKG)/FrontPageDxe/FrontPageDxe.inf
  $(CHIPSET_PKG)/PlatformDxe/PlatformDxe.inf
  $(CHIPSET_PKG)/LegacyBiosPlatformHookDxe/LegacyBiosPlatformHookDxe.inf
  !disable InsydeModulePkg/Universal/Security/Tcg/PhysicalPresenceDxe/PhysicalPresenceDxe.inf
  InsydeModulePkg/Universal/Security/Tcg/PhysicalPresenceDxe/PhysicalPresenceDxe.inf {
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/Security/Tcg/PhysicalPresenceDxe
  }
  !disable InsydeModulePkg/Csm/LegacyBiosDxe/LegacyBiosDxe.inf
  InsydeModulePkg/Csm/LegacyBiosDxe/LegacyBiosDxe.inf {
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/InsydeModulePkg/Csm/LegacyBiosDxe
  }
#[-start-231113-IB19090063-add]#
  !disable InsydeModulePkg/Universal/CommonPolicy/LegacyBiosPlatformDxe/LegacyBiosPlatformDxe.inf {
  InsydeModulePkg/Universal/CommonPolicy/LegacyBiosPlatformDxe/LegacyBiosPlatformDxe.inf {
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/CommonPolicy/LegacyBiosPlatformDxe
  }
#[-end-231113-IB19090063-add]#
  #
  # Seamless USB DXE drivers
  #
#[-start-231113-IB19090063-remove]#
# !if gInsydeTokenSpaceGuid.PcdH2OUsbSupported
#   InsydeModulePkg/Bus/Pci/OhciDxe/OhciDxe.inf
# !endif
#[-end-231113-IB19090063-remove]#
  #
  # Support UEFI AMD AHCI driver 1.3.0.0
  #
!if gInsydeTokenSpaceGuid.PcdH2OAhciSupported == 0
  $(CHIPSET_PKG)/Binary/UefiAhci/1.3.0.0/AhciBus.inf
!endif

  #
  # PXE Binary
  #
#  $(CHIPSET_PKG)/Binary/UEFIPxeDriver/LxUndiDxe.inf

  #
  # Install Bios Protect Interface
  #
  $(CHIPSET_PKG)/BiosWriteProtectSmm/SpiLockSmi.inf
  $(CHIPSET_PKG)/BiosRegionLockDxe/BiosRegionLockDxe.inf

  #
  # USB legacy support
  #
!if gChipsetPkgTokenSpaceGuid.PcdLegacyFreeSupported == 1
  $(CHIPSET_PKG)/UsbLegacyControl/UsbLegacyControl.inf
!endif

  #
  # These module is for OEM to customize
  #
  $(CHIPSET_PKG)/OemAcpiPlatformDxe/OemAcpiPlatformDxe.inf

#[-start-140325-IB13080003-add]#
!if gInsydeTokenSpaceGuid.PcdDynamicHotKeySupported
 $(CHIPSET_PKG)/DynamicHotKeyDxe/DynamicHotKeyDxe.inf
!endif
#[-end-140325-IB13080003-add]#

  #
  # STIBP feature
  #
  $(CHIPSET_PKG)/StibpFeatureDxe/StibpFeatureDxe.inf

#[-start-231113-IB19090063-remove]#
# #[-start-230609-IB18160133-add]#
#  !disable InsydeModulePkg/Universal/Security/Tcg/TrEEConfigDxe/TrEEConfigDxe.inf
#   InsydeModulePkg/Universal/Security/Tcg/TrEEConfigDxe/TrEEConfigDxe.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/Security/Tcg/TrEEConfigDxe
#   }
# #[-end-230609-IB18160133-add]#
#[-end-231113-IB19090063-remove]#

################################################################################
#
# Components.X64 Override
#
################################################################################

#[-start-220802-IB14860825-remove]#
#  #
#  # Enhance depex of Agesa driver AmdPspSmmV2
#  #
#  !disable AgesaModulePkg/Psp/AmdPspSmmV2/AmdPspSmmV2.inf
#  AgesaModulePkg/Psp/AmdPspSmmV2/AmdPspSmmV2.inf {
#    <SOURCE_OVERRIDE_PATH>
#      $(CHIPSET_PKG)/Override/AgesaModulePkg/Psp/AmdPspSmmV2
#  }
#[-end-220802-IB14860825-remove]#

#[-start-220714-IB14860813-add]#
  !disable InsydeModulePkg/Bus/Pci/PciBusDxe/PciBusDxe.inf
  InsydeModulePkg/Bus/Pci/PciBusDxe/PciBusDxe.inf {
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/InsydeModulePkg/Bus/Pci/PciBusDxe
  }

  !disable InsydeModulePkg/Universal/Security/HstiDxe/HstiDxe.inf
  InsydeModulePkg/Universal/Security/HstiDxe/HstiDxe.inf {
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/Security/HstiDxe
  }

  !disable InsydeSetupPkg/Drivers/DisplayTypeDxe/DisplayTypeDxe.inf
  InsydeSetupPkg/Drivers/DisplayTypeDxe/DisplayTypeDxe.inf {
      <SOURCE_OVERRIDE_PATH>
        $(CHIPSET_PKG)/Override/InsydeSetupPkg/Drivers/DisplayTypeDxe
  }

#[-start-231113-IB19090063-remove]#
# !if gInsydeTokenSpaceGuid.PcdH2OUsbSupported
# #[-start-190326-IB06070019-add]#
#   !disable InsydeModulePkg/Bus/Pci/EhciDxe/EhciDxe.inf
#   $(CHIPSET_PKG)/Override/InsydeModulePkg/Bus/Pci/EhciDxe/EhciDxe.inf
#
#   !disable InsydeModulePkg/Bus/Pci/XhciDxe/XhciDxe.inf
#   $(CHIPSET_PKG)/Override/InsydeModulePkg/Bus/Pci/XhciDxe/XhciDxe.inf
# #[-end-190326-IB06070019-add]#
# #[-start-191008-IB06070056-add]#
#   !disable InsydeModulePkg/Bus/Usb/UsbCoreDxe/UsbCoreDxe.inf
#   InsydeModulePkg/Bus/Usb/UsbCoreDxe/UsbCoreDxe.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Bus/Usb/UsbCoreDxe
#   }
# #[-end-191008-IB06070056-add]#
# #[-start-191115-IB06070063-modify]#
#   !disable InsydeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
#   InsydeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Bus/Usb/UsbBusDxe
#   }
#
#   !disable InsydeModulePkg/Bus/Usb/UsbBotDxe/UsbBotDxe.inf
#   InsydeModulePkg/Bus/Usb/UsbBotDxe/UsbBotDxe.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Bus/Usb/UsbBotDxe
#   }
#
#   !disable InsydeModulePkg/Bus/Usb/UsbCbiDxe/UsbCbiDxe.inf
#   InsydeModulePkg/Bus/Usb/UsbCbiDxe/UsbCbiDxe.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Bus/Usb/UsbCbiDxe
#   }
#
#   !disable InsydeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf
#   InsydeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Bus/Usb/UsbMassStorageDxe
#   }
#
#   !disable InsydeModulePkg/Bus/Usb/UsbKbDxe/UsbKbDxe.inf
#   InsydeModulePkg/Bus/Usb/UsbKbDxe/UsbKbDxe.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Bus/Usb/UsbKbDxe
#   }
# #[-start-191024-IB06070058-add]#
#   !disable InsydeModulePkg/Bus/Usb/UsbMouseDxe/UsbMouseDxe.inf
#   InsydeModulePkg/Bus/Usb/UsbMouseDxe/UsbMouseDxe.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Bus/Usb/UsbMouseDxe
#   }
# #[-end-191024-IB06070058-add]#
# #[-end-191115-IB06070063-modify]#
#   !if gInsydeTokenSpaceGuid.PcdH2OCsmSupported
#     !disable InsydeModulePkg/Csm/UsbLegacySmm/UsbLegacySmm.inf
#     $(CHIPSET_PKG)/Override/InsydeModulePkg/Csm/UsbLegacySmm/UsbLegacySmm.inf
#   !endif
# !endif
#[-end-231113-IB19090063-remove]#
#[-start-231113-IB19090063-remove]#
# #[-start-181112-IB06070003-add]#
# !if gInsydeCrTokenSpaceGuid.PcdH2OCrHsuSerialSupported == 1
#   !disable InsydeCrPkg/Uart16550Devices/PchHsUartDxe/PchHsUartDxe.inf
#   InsydeCrPkg/Uart16550Devices/PchHsUartDxe/PchHsUartDxe.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeCrPkg/Uart16550Devices/PchHsUartDxe
#   }
# !endif
# #[-end-181112-IB06070003-add]#
#[-end-231113-IB19090063-remove]#
#[-start-181114-IB06070004-add]#
!if $(INSYDE_DEBUGGER) == YES and $(H2O_DDT_DEBUG_IO) == Com
  !disable InsydeModulePkg/H2ODebug/$(H2O_DDT_DEBUG_IO)DebugIoDxe/$(H2O_DDT_DEBUG_IO)DebugIoDxe.inf
  $(CHIPSET_PKG)/Override/InsydeModulePkg/H2ODebug/$(H2O_DDT_DEBUG_IO)DebugIoDxe/$(H2O_DDT_DEBUG_IO)DebugIoDxe.inf
!endif
#[-end-181114-IB06070004-add]#
#[-start-181219-IB06070005-add]#
  $(CHIPSET_PKG)/Override/AgesaModulePkg/SnpDxe/SnpDxePort.inf
#[-end-181219-IB06070005-add]#
#[-start-190225-IB06070011-add]#
!if gInsydeTokenSpaceGuid.PcdH2OSdhcSupported
#[-start-231113-IB19090063-remove]#
#   !disable InsydeModulePkg/Bus/Pci/SdhcDxe/SdHostDriverDxe.inf
#   $(CHIPSET_PKG)/Override/InsydeModulePkg/Bus/Pci/SdhcDxe/SdHostDriverDxe.inf
#
#   !disable InsydeModulePkg/Bus/Sdhc/SdMmcDxe/SdMmcDeviceDxe.inf
#   $(CHIPSET_PKG)/Override/InsydeModulePkg/Bus/Sdhc/SdMmcDxe/SdMmcDeviceDxe.inf
#[-end-231113-IB19090063-remove]#
!if gInsydeTokenSpaceGuid.PcdH2OCsmSupported
  !disable InsydeModulePkg/Csm/SdLegacySmm/SdLegacySmm.inf
  $(CHIPSET_PKG)/Override/InsydeModulePkg/Csm/SdLegacySmm/SdLegacySmm.inf
!endif
!endif
#[-end-190225-IB06070011-add]#

#[-start-231113-IB19090063-remove]#
# #[-start-190725-IB06070044-add]#
# !if gInsydeTokenSpaceGuid.PcdH2ONvmeSupported
#   !disable InsydeModulePkg/Bus/Pci/NvmExpressDxe/NvmExpressDxe.inf
#   $(CHIPSET_PKG)/Override/InsydeModulePkg/Bus/Pci/NvmExpressDxe/NvmExpressDxe.inf
#
# #[-start-200422-IB06070087-add]#
#   !disable InsydeModulePkg/Csm/NvmExpressLegacySmm/NvmExpressLegacySmm.inf
#   $(CHIPSET_PKG)/Override/InsydeModulePkg/Csm/NvmExpressLegacySmm/NvmExpressLegacySmm.inf
# #[-end-200422-IB06070087-add]#
# !endif
# #[-end-190725-IB06070044-add]#
#[-end-231113-IB19090063-remove]#
#[-start-231113-IB19090063-remove]#
# #[-start-190821-IB06070046-add]#
# !if gInsydeTokenSpaceGuid.PcdH2OAhciSupported
#   !disable InsydeModulePkg/Bus/Pci/AhciBusDxe/AhciBusDxe.inf
#   $(CHIPSET_PKG)/Override/InsydeModulePkg/Bus/Pci/AhciBusDxe/AhciBusDxe.inf
#!if $(EFI_DEBUG) == NO
#  <PcdsFixedAtBuild>
#    gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0x2
#    gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x80000000
#  <BuildOptions>
#    MSFT:*_*_*_CC_FLAGS = /UMDEPKG_NDEBUG
#!endif
#  }
# !endif
# #[-end-190821-IB06070046-add]#
#[-end-231113-IB19090063-remove]#

#[-start-190830-IB06070047-add]#
  !disable InsydeModulePkg/Universal/UserInterface/HiiDatabaseDxe/HiiDatabaseDxe.inf
  InsydeModulePkg/Universal/UserInterface/HiiDatabaseDxe/HiiDatabaseDxe.inf {
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/UserInterface/HiiDatabaseDxe
  }
#[-end-190830-IB06070047-add]#
#[-start-231113-IB19090063-remove]#
# #[-start-191008-IB06070054-add]#
#  !disable InsydeCrPkg/ConfigUtility/CrConfigUtilDxe/CrConfigUtil.inf
#  InsydeCrPkg/ConfigUtility/CrConfigUtilDxe/CrConfigUtil.inf {
#    <SOURCE_OVERRIDE_PATH>
#      $(CHIPSET_PKG)/Override/InsydeCrPkg/ConfigUtility/CrConfigUtilDxe
#  }
#
#  !disable InsydeCrPkg/TerminalDxe/TerminalDxe.inf
#  InsydeCrPkg/TerminalDxe/TerminalDxe.inf {
#    <SOURCE_OVERRIDE_PATH>
#      $(CHIPSET_PKG)/Override/InsydeCrPkg/TerminalDxe
#  }
#
#  !disable InsydeCrPkg/Uart16550SerialDxe/Uart16550SerialDxe.inf
#  InsydeCrPkg/Uart16550SerialDxe/Uart16550SerialDxe.inf {
#    <SOURCE_OVERRIDE_PATH>
#      $(CHIPSET_PKG)/Override/InsydeCrPkg/Uart16550SerialDxe
#  }
# #[-end-191008-IB06070054-add]#
#[-end-231113-IB19090063-remove]#

#[-start-191015-IB06070057-add]#
  !disable InsydeModulePkg/Universal/IhisiServicesSmm/IhisiServicesSmm.inf
  InsydeModulePkg/Universal/IhisiServicesSmm/IhisiServicesSmm.inf {
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/IhisiServicesSmm
  }
#[-end-191015-IB06070057-add]#

#[-start-191108-IB06070061-add]#
#[-start-231113-IB19090063-remove]#
# !if gInsydeTokenSpaceGuid.PcdH2OPciOptionRomSecurityControlSupported
#   !disable InsydeModulePkg/Universal/SecureOptionRomControlDxe/SecureOptionRomControlDxe.inf
#   InsydeModulePkg/Universal/SecureOptionRomControlDxe/SecureOptionRomControlDxe.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/SecureOptionRomControlDxe
#   }
# !endif
#[-end-231113-IB19090063-remove]#

  !disable InsydeModulePkg/Universal/UserInterface/BootMaintDxe/BootMaintDxe.inf
  InsydeModulePkg/Universal/UserInterface/BootMaintDxe/BootMaintDxe.inf {
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/UserInterface/BootMaintDxe
  }

  !disable InsydeModulePkg/Universal/UserInterface/BootManagerDxe/BootManagerDxe.inf
  InsydeModulePkg/Universal/UserInterface/BootManagerDxe/BootManagerDxe.inf {
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/UserInterface/BootManagerDxe
  }

  !disable InsydeModulePkg/Universal/UserInterface/DeviceManagerDxe/DeviceManagerDxe.inf
  InsydeModulePkg/Universal/UserInterface/DeviceManagerDxe/DeviceManagerDxe.inf {
    <LibraryClasses>
      NULL|$(CHIPSET_PKG)/Override/InsydeModulePkg/Library/DeviceManagerUiLib/DeviceManagerUiLib.inf
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/UserInterface/DeviceManagerDxe
  }

  !disable InsydeModulePkg/Universal/UserInterface/DriverHealthManagerDxe/DriverHealthManagerDxe.inf
  InsydeModulePkg/Universal/UserInterface/DriverHealthManagerDxe/DriverHealthManagerDxe.inf {
    <LibraryClasses>
      UefiBootManagerLib|MdeModulePkg/Library/UefiBootManagerLib/UefiBootManagerLib.inf
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/UserInterface/DriverHealthManagerDxe
  }

!if gInsydeTokenSpaceGuid.PcdH2OSecureBootSupported
  !disable InsydeModulePkg/Universal/UserInterface/SecureBootMgrDxe/SecureBootMgrDxe.inf
  InsydeModulePkg/Universal/UserInterface/SecureBootMgrDxe/SecureBootMgrDxe.inf {
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/UserInterface/SecureBootMgrDxe
  }
!endif

  !disable InsydeSetupPkg/Drivers/HiiLayoutPkgDxe/HiiLayoutPkgDxe.inf
  InsydeSetupPkg/Drivers/HiiLayoutPkgDxe/HiiLayoutPkgDxe.inf {
    <SOURCE_OVERRIDE_PATH>
    $(CHIPSET_PKG)/Override/InsydeSetupPkg/Drivers/HiiLayoutPkgDxe
  }

!if gInsydeTokenSpaceGuid.PcdH2OFormBrowserSupported == FALSE
  !disable InsydeModulePkg/Universal/UserInterface/SetupBrowserDxe/SetupBrowserDxe.inf
!if gInsydeTokenSpaceGuid.PcdGraphicsSetupSupported
  !disable InsydeModulePkg/Universal/UserInterface/MicrowindowsDxe/MicrowindowsDxe.inf
!endif
!endif
#[-end-191108-IB06070061-add]#
#[-start-231113-IB19090063-remove]#
# #[-start-191231-IB06070075-add]#
#   !disable InsydeModulePkg/Universal/FirmwareVolume/FvbServicesRuntimeDxe/FvbServicesRuntimeDxe.inf
#   InsydeModulePkg/Universal/FirmwareVolume/FvbServicesRuntimeDxe/FvbServicesRuntimeDxe.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/FirmwareVolume/FvbServicesRuntimeDxe
#   }
# #[-end-191231-IB06070075-add]#
#[-end-231113-IB19090063-remove]#
#[-start-200910-IB06070099-add]#
  !disable AgesaModulePkg/Nbio/NbioALib/AmdNbioAlibRVDxe/AmdNbioAlibRVDxe.inf
  AgesaModulePkg/Nbio/NbioALib/AmdNbioAlibRVDxe/AmdNbioAlibRVDxe.inf {
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/AgesaModulePkg/Nbio/NbioALib/AmdNbioAlibRVDxe
  }
#[-end-200910-IB06070099-add]#
#[-start-231113-IB19090063-remove]#
# #[-start-211112-IB06070136-add]#
#   !disable InsydeModulePkg/Universal/Smbios/PnpSmm/PnpSmm.inf
#   InsydeModulePkg/Universal/Smbios/PnpSmm/PnpSmm.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/Smbios/PnpSmm
#   }
#
#   !disable InsydeModulePkg/Universal/SmmResourceCheckDxe/SmmResourceCheckDxe.inf
#   InsydeModulePkg/Universal/SmmResourceCheckDxe/SmmResourceCheckDxe.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/SmmResourceCheckDxe
#   }
#
# !if gInsydeTokenSpaceGuid.PcdH2OStatusCodeBeepSupported
#   !disable InsydeModulePkg/Universal/StatusCode/BeepStatusCodeDxe/BeepStatusCodeDxe.inf
#   InsydeModulePkg/Universal/StatusCode/BeepStatusCodeDxe/BeepStatusCodeDxe.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/StatusCode/BeepStatusCodeDxe
#   }
#
#   !disable InsydeModulePkg/Universal/StatusCode/BeepStatusCodeSmm/BeepStatusCodeSmm.inf
#   InsydeModulePkg/Universal/StatusCode/BeepStatusCodeSmm/BeepStatusCodeSmm.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/StatusCode/BeepStatusCodeSmm
#   }
# !endif
# #[-end-211112-IB06070136-add]#
#[-end-231113-IB19090063-remove]#
#[-start-231113-IB19090063-remove]#
# #[-start-211112-IB06070137-add]#
#   !disable InsydeModulePkg/Universal/FirmwareVolume/FwBlockServiceSmm/FwBlockServiceSmm.inf
#   InsydeModulePkg/Universal/FirmwareVolume/FwBlockServiceSmm/FwBlockServiceSmm.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/FirmwareVolume/FwBlockServiceSmm
#     <LibraryClasses>
#       S3BootScriptLib|MdePkg/Library/BaseS3BootScriptLibNull/BaseS3BootScriptLibNull.inf
#   }
# #[-end-211112-IB06070137-add]#
# #[-start-220217-IB06070198-add]#
#   !disable InsydeModulePkg/Universal/Security/StorageSecurityCommandDxe/StorageSecurityCommandDxe.inf
#   InsydeModulePkg/Universal/Security/StorageSecurityCommandDxe/StorageSecurityCommandDxe.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/Security/StorageSecurityCommandDxe
#   }
# #[-end-220217-IB06070198-add]#
# #[-start-220415-IB14860774-add]#
#   !disable InsydeModulePkg/Universal/BdsDxe/BdsDxe.inf
#   InsydeModulePkg/Universal/BdsDxe/BdsDxe.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/BdsDxe
#   }
# #[-end-220415-IB14860774-add]#
# #[-start-211112-IB06070138-add]#
# !if gInsydeTokenSpaceGuid.PcdH2OCsmSupported
# !if gInsydeTokenSpaceGuid.PcdH2OAhciSupported == 1 || gInsydeTokenSpaceGuid.PcdH2OIdeSupported == 1
#   !disable InsydeModulePkg/Csm/AtaLegacySmm/AtaLegacySmm.inf
#   $(CHIPSET_PKG)/Override/InsydeModulePkg/Csm/AtaLegacySmm/AtaLegacySmm.inf
# !endif
# !endif
# #[-end-211112-IB06070138-add]#
# #[-start-211112-IB06070139-add]#
# !if gInsydeTokenSpaceGuid.PcdH2OIdeSupported
#   !disable InsydeModulePkg/Bus/Pci/IdeBusDxe/IdeBusDxe.inf
#   $(CHIPSET_PKG)/Override/InsydeModulePkg/Bus/Pci/IdeBusDxe/IdeBusDxe.inf
# !endif
# #[-end-211112-IB06070139-add]#
# #[-start-211112-IB06070140-add]#
# !if gInsydeTokenSpaceGuid.PcdH2OHddPasswordSupported
#   !disable InsydeModulePkg/Universal/Security/HddPassword/HddPassword.inf
#   InsydeModulePkg/Universal/Security/HddPassword/HddPassword.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/Security/HddPassword
#   }
# !endif
# #[-end-211112-IB06070140-add]#
#[-end-231113-IB19090063-remove]#
#[-end-220714-IB14860813-add]#

  #
  # Enhance Smm performance
  #
  !disable MdeModulePkg/Core/PiSmmCore/PiSmmIpl.inf
  MdeModulePkg/Core/PiSmmCore/PiSmmIpl.inf {
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/MdeModulePkg/Core/PiSmmCore
  }
  !disable MdeModulePkg/Core/PiSmmCore/PiSmmCore.inf
  MdeModulePkg/Core/PiSmmCore/PiSmmCore.inf{
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/MdeModulePkg/Core/PiSmmCore
  }
#[-start-231113-IB19090063-remove]#
# #[-start-220315-IB14860764-add]#
#   !disable InsydeModulePkg/Universal/Security/SysPasswordDxe/SysPasswordDxe.inf
#   InsydeModulePkg/Universal/Security/SysPasswordDxe/SysPasswordDxe.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/Security/SysPasswordDxe
#   }
# #[-end-220315-IB14860764-add]#
# #[-start-220316-IB14860765-add]#
#   !disable MdeModulePkg/Core/Dxe/DxeMain.inf
#   MdeModulePkg/Core/Dxe/DxeMain.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/MdeModulePkg/Core/Dxe
#   }
# #[-end-220316-IB14860765-add]#
# #[-start-220513-IB14860781-add]#
#   !disable InsydeModulePkg/Universal/UserInterface/Fonts/TrueTypeDecoderDxe/TrueTypeDecoderDxe.inf
#   $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/UserInterface/Fonts/TrueTypeDecoderDxe/TrueTypeDecoderDxe.inf
# #[-end-220513-IB14860781-add]#
# #[-start-220629-IB14860802-add]#
#   !disable InsydeModulePkg/Universal/PcdSmmDxe/Pcd.inf
#   InsydeModulePkg/Universal/PcdSmmDxe/Pcd.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/PcdSmmDxe
#     <LibraryClasses>
#       PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
#   }
# #[-end-220629-IB14860802-add]#
# #[-start-220629-IB14860806-add]#
# !if gInsydeTokenSpaceGuid.PcdH2OCsmSupported
#   !disable InsydeModulePkg/Csm/BiosThunk/Int15ServiceSmm/Int15ServiceSmm.inf
#   InsydeModulePkg/Csm/BiosThunk/Int15ServiceSmm/Int15ServiceSmm.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Csm/BiosThunk/Int15ServiceSmm
#   }
# !endif
# #[-end-220629-IB14860806-add]#
# #[-start-220722-IB14860824-add]#
#   !disable InsydeModulePkg/Universal/Smbios/SmbiosDxe/SmbiosDxe.inf
#   InsydeModulePkg/Universal/Smbios/SmbiosDxe/SmbiosDxe.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/Smbios/SmbiosDxe
#   }
# #[-end-220722-IB14860824-add]#
#[-end-231113-IB19090063-remove]#
#[-start-220926-IB14860843-add]#
  !disable InsydeModulePkg/Universal/Variable/VariableRuntimeDxe/VariableRuntimeDxe.inf
  InsydeModulePkg/Universal/Variable/VariableRuntimeDxe/VariableRuntimeDxe.inf {
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/Variable/VariableRuntimeDxe
  }
#[-end-220926-IB14860843-add]#
#[-start-221013-IB14860855-add]#
  !disable InsydeModulePkg/Universal/UserInterface/HiiDatabaseDxe/HiiDatabaseDxe.inf
  InsydeModulePkg/Universal/UserInterface/HiiDatabaseDxe/HiiDatabaseDxe.inf {
    <SOURCE_OVERRIDE_PATH>
      $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/UserInterface/HiiDatabaseDxe
  }
#[-end-221013-IB14860855-add]#
#[-start-231113-IB19090063-remove]#
# #[-start-221114-IB14860861-add]#
#   !disable InsydeModulePkg/Universal/Console/PngDecoderDxe/PngDecoderDxe.inf
#   $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/Console/PngDecoderDxe/PngDecoderDxe.inf
# #[-end-221114-IB14860861-add]#
# #[-start-230908-IB18160150-add]#
#   !disable InsydeModulePkg/Universal/Console/TgaDecoderDxe/TgaDecoderDxe.inf
#   InsydeModulePkg/Universal/Console/TgaDecoderDxe/TgaDecoderDxe.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/Console/TgaDecoderDxe
#   }
#   !disable InsydeModulePkg/Universal/Console/PcxDecoderDxe/PcxDecoderDxe.inf
#   InsydeModulePkg/Universal/Console/PcxDecoderDxe/PcxDecoderDxe.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/Console/PcxDecoderDxe
#   }
#   !disable InsydeModulePkg/Universal/Console/JpegDecoderDxe/JpegDecoderDxe.inf
#   InsydeModulePkg/Universal/Console/JpegDecoderDxe/JpegDecoderDxe.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/Console/JpegDecoderDxe
#   }
#   !disable InsydeModulePkg/Universal/Console/GifDecoderDxe/GifDecoderDxe.inf
#   InsydeModulePkg/Universal/Console/GifDecoderDxe/GifDecoderDxe.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/Console/GifDecoderDxe
#   }
#   !disable InsydeModulePkg/Universal/Console/BmpDecoderDxe/BmpDecoderDxe.inf
#   InsydeModulePkg/Universal/Console/BmpDecoderDxe/BmpDecoderDxe.inf {
#     <SOURCE_OVERRIDE_PATH>
#       $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/Console/BmpDecoderDxe
#   }
# #[-end-230908-IB18160150-add]#
#[-end-231113-IB19090063-add]#
#[-start-230220-IB14860896-add]#
!if gInsydeTokenSpaceGuid.PcdH2ONetworkSupported
!if gInsydeTokenSpaceGuid.PcdH2ONetworkIscsiSupported
  !disable InsydeNetworkPkg/Drivers/IScsiDxe/IScsiDxe.inf
  $(CHIPSET_PKG)/Override/InsydeNetworkPkg/Drivers/IScsiDxe
!endif
!endif
#[-end-230220-IB14860896-add]#


#[-start-240426-IB18160212-add-]#
!disable  InsydeModulePkg/Universal/Acpi/AcpiPlatformDxe/AcpiPlatformDxe.inf
InsydeModulePkg/Universal/Acpi/AcpiPlatformDxe/AcpiPlatformDxe.inf {
  <SOURCE_OVERRIDE_PATH>
    $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/Acpi/AcpiPlatformDxe
}

!disable  InsydeModulePkg/Universal/Console/ConSplitterDxe/ConSplitterDxe.inf
InsydeModulePkg/Universal/Console/ConSplitterDxe/ConSplitterDxe.inf {
  <SOURCE_OVERRIDE_PATH>
    $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/Console/ConSplitterDxe
}

!disable InsydeModulePkg/Universal/SmmResourceCheckDxe/SmmResourceCheckDxe.inf
InsydeModulePkg/Universal/SmmResourceCheckDxe/SmmResourceCheckDxe.inf {
  <SOURCE_OVERRIDE_PATH>
  $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/SmmResourceCheckDxe
}

#[-end-240426-IB18160212-add-]#
