## @file
#  Package dscription file for ChipsetPkg
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

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = PicassoChipsetPkg
  PLATFORM_GUID                  = DF131C26-D59D-45c8-A8EB-2BFD518D8BA9
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/PicassoChipsetPkg
  SUPPORTED_ARCHITECTURES        = IA32|X64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT

  DEFINE   AGESA_PKG_PATH = AgesaModulePkg
  DEFINE   AGESA_PKG_DEC  = AgesaFp5RvModulePkg
  DEFINE   FCH_PKG_PATH   = $(AGESA_PKG_PATH)/Fch/Sandstone
  DEFINE   FCH_PKG_DEC    = FchSandstone
  DEFINE   CHIPSET_PKG = PicassoChipsetPkg
  !include $(CHIPSET_PKG)/Package.env

################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################

[LibraryClasses.IA32, LibraryClasses.X64]
  NULL|MdeModulePkg/Override/Library/CompilerStubLib/CompilerStubLib.inf

[LibraryClasses]
  #
  # Entry point
  #
  PeiCoreEntryPoint|MdePkg/Library/PeiCoreEntryPoint/PeiCoreEntryPoint.inf
  PeimEntryPoint|MdePkg/Library/PeimEntryPoint/PeimEntryPoint.inf
  DxeCoreEntryPoint|MdePkg/Library/DxeCoreEntryPoint/DxeCoreEntryPoint.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  #
  # Basic
  #
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLibRepStr/BaseMemoryLibRepStr.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  CpuLib|MdePkg/Library/BaseCpuLib/BaseCpuLib.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  PciLib|MdePkg/Library/BasePciLibCf8/BasePciLibCf8.inf
  PciCf8Lib|MdePkg/Library/BasePciCf8Lib/BasePciCf8Lib.inf
  PciExpressLib|MdePkg/Library/BasePciExpressLib/BasePciExpressLib.inf
  CacheMaintenanceLib|MdePkg/Library/BaseCacheMaintenanceLib/BaseCacheMaintenanceLib.inf
  PeCoffLib|MdePkg/Library/BasePeCoffLib/BasePeCoffLib.inf
  PeCoffGetEntryPointLib|MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  PeCoffExtraActionLib|MdePkg/Library/BasePeCoffExtraActionLibNull/BasePeCoffExtraActionLibNull.inf
  #
  # UEFI & PI
  #
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  UefiRuntimeLib|MdePkg/Library/UefiRuntimeLib/UefiRuntimeLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  UefiDecompressLib|MdePkg/Library/BaseUefiDecompressLib/BaseUefiDecompressLib.inf
  PeiServicesTablePointerLib|MdePkg/Library/PeiServicesTablePointerLib/PeiServicesTablePointerLib.inf
  PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  UefiCpuLib|UefiCpuPkg/Library/BaseUefiCpuLib/BaseUefiCpuLib.inf
  #
  # Generic Modules
  #
  UefiUsbLib|MdePkg/Library/UefiUsbLib/UefiUsbLib.inf
  UefiScsiLib|MdePkg/Library/UefiScsiLib/UefiScsiLib.inf
  NetLib|MdeModulePkg/Library/DxeNetLib/DxeNetLib.inf
  IpIoLib|MdeModulePkg/Library/DxeIpIoLib/DxeIpIoLib.inf
  UdpIoLib|MdeModulePkg/Library/DxeUdpIoLib/DxeUdpIoLib.inf
  DpcLib|MdeModulePkg/Library/DxeDpcLib/DxeDpcLib.inf
  OemHookStatusCodeLib|MdeModulePkg/Library/OemHookStatusCodeLibNull/OemHookStatusCodeLibNull.inf
  GenericBdsLib|IntelFrameworkModulePkg/Library/GenericBdsLib/GenericBdsLib.inf
  SecurityManagementLib|MdeModulePkg/Library/DxeSecurityManagementLib/DxeSecurityManagementLib.inf
  TimerLib|MdePkg/Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf
  SerialPortLib|MdePkg/Library/BaseSerialPortLibNull/BaseSerialPortLibNull.inf
  CapsuleLib|MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf
  VariableLib|InsydeModulePkg/Library/DxeVariableLib/DxeVariableLib.inf
  FdSupportLib|InsydeModulePkg/Library/FlashDeviceSupport/FdSupportLib/FdSupportLib.inf
  #
  # Misc
  #
!if $(EFI_DEBUG) == YES || $(USB_DEBUG_SUPPORT) == YES
  DebugLib|IntelFrameworkModulePkg/Library/PeiDxeDebugLibReportStatusCode/PeiDxeDebugLibReportStatusCode.inf
!else
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
!endif
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  DebugAgentLib|MdeModulePkg/Library/DebugAgentLibNull/DebugAgentLibNull.inf
  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  CmosLib|InsydeModulePkg/Library/CmosLib/CmosLib.inf
  S3BootScriptLib|MdeModulePkg/Library/PiDxeS3BootScriptLib/DxeS3BootScriptLib.inf
  S3IoLib|MdePkg/Library/BaseS3IoLib/BaseS3IoLib.inf
  S3PciLib|MdePkg/Library/BaseS3PciLib/BaseS3PciLib.inf
  SmbusLib|MdePkg/Library/DxeSmbusLib/DxeSmbusLib.inf
  LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxDxeLib.inf
  PostCodeLib|MdePkg/Library/BasePostCodeLibPort80/BasePostCodeLibPort80.inf

  MtrrLib|UefiCpuPkg/Library/MtrrLib/MtrrLib.inf
  LocalApicLib|UefiCpuPkg/Library/BaseXApicX2ApicLib/BaseXApicX2ApicLib.inf
  BvdtLib|InsydeModulePkg/Library/BvdtLib/BvdtLib.inf
  OemGraphicsLib|InsydeModulePkg/Library/OemGraphicsLib/OemGraphicsLib.inf
  SetupUtilityLib|InsydeModulePkg/Library/SetupUtilityLib/SetupUtilityLib.inf
  DxeOemSvcKernelLib|InsydeOemServicesPkg/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLib.inf
  DxeOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLibDefault.inf
  BaseSetupDefaultLib|InsydeModulePkg/Library/SetupDefaultLib/SetupDefaultLib.inf
  KernelConfigLib|InsydeModulePkg/Library/DxeKernelConfigLib/DxeKernelConfigLib.inf
  ChipsetConfigLib|$(CHIPSET_PKG)/Library/ChipsetConfigLib/DxeChipsetConfigLib.inf
  DxeChipsetSvcLib|InsydeModulePkg/Library/DxeChipsetSvcLib/DxeChipsetSvcLib.inf
  AmdExtCmosLib|$(CHIPSET_PKG)/Library/AmdExtCmosLib/AmdExtCmosLib.inf
  BaseOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/BaseOemSvcKernelLib/BaseOemSvcKernelLibDefault.inf
  BaseOemSvcKernelLib|InsydeOemServicesPkg/Library/BaseOemSvcKernelLib/BaseOemSvcKernelLib.inf
  CommonEcLib|InsydeModulePkg/Library/CommonEcLib/CommonEcLib.inf
  IntrinsicLib|CryptoPkg/Library/IntrinsicLib/IntrinsicLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  SpiAccessLib|$(CHIPSET_PKG)/Library/FlashDeviceSupport/SpiAccessLib/SpiAccessLib.inf
  FlashWriteEnableLib|$(CHIPSET_PKG)/Library/FlashDeviceSupport/FlashWriteEnableLib/FlashWriteEnableLib.inf
  SpiAccessInitLib|$(CHIPSET_PKG)/Library/FlashDeviceSupport/DxeSpiAccessInitLib/DxeSpiAccessInitLib.inf
  IrsiRegistrationLib|InsydeModulePkg/Library/Irsi/IrsiRegistrationLib/IrsiRegistrationLib.inf
  VariableSupportLib|InsydeModulePkg/Library/VariableSupportLib/VariableSupportLib.inf
  FlashDevicesLib|InsydeFlashDevicePkg/Library/FlashDevicesLib/FlashDevicesLib.inf
  SeamlessRecoveryLib|InsydeModulePkg/Library/SeamlessRecoveryLib/SeamlessRecoveryLib.inf


[LibraryClasses.common.SEC]
  PlatformSecLib|$(CHIPSET_PKG)/Library/PlatformSecLib/PlatformSecLib.inf
  SecOemSvcChipsetLibDefault|$(CHIPSET_PKG)/Library/SecOemSvcChipsetLib/SecOemSvcChipsetLibDefault.inf
  SecOemSvcChipsetLib|$(CHIPSET_PKG)/Library/SecOemSvcChipsetLib/SecOemSvcChipsetLib.inf

[LibraryClasses.common.PEIM]
  HobLib|MdePkg/Library/PeiHobLib/PeiHobLib.inf
  MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/PeiReportStatusCodeLib/PeiReportStatusCodeLib.inf
  SmbusLib|MdePkg/Library/PeiSmbusLibSmbus2Ppi/PeiSmbusLibSmbus2Ppi.inf
  KernelConfigLib|$(CHIPSET_PKG)/Library/PeiKernelConfigLib/PeiKernelConfigLib.inf
  ChipsetConfigLib|$(CHIPSET_PKG)/Library/ChipsetConfigLib/PeiChipsetConfigLib.inf
  AmdExtCmosLib|$(CHIPSET_PKG)/Library/AmdExtCmosLib/AmdExtCmosLib.inf
  HobLib|MdePkg/Library/PeiHobLib/PeiHobLib.inf
  SeamlessRecoveryLib|InsydeModulePkg/Library/PeiSeamlessRecoveryLib/SeamlessRecoveryLib.inf
  PeiOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/PeiOemSvcKernelLib/PeiOemSvcKernelLibDefault.inf
  PeiChipsetSvcLib|InsydeModulePkg/Library/PeiChipsetSvcLib/PeiChipsetSvcLib.inf
  PeiOemSvcKernelLib|InsydeOemServicesPkg/Library/PeiOemSvcKernelLib/PeiOemSvcKernelLib.inf

[LibraryClasses.common.DXE_DRIVER]
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  BadgingSupportLib|InsydeModulePkg/Library/CommonPlatformLib/BadgingSupportLib/BadgingSupportLib.inf
  AcpiPlatformLib|InsydeModulePkg/Library/CommonPlatformLib/AcpiPlatformLib/AcpiPlatformLib.inf
  EfiRegTableLib|$(CHIPSET_PKG)/Library/EfiRegTableLib/EfiRegTableLib.inf
  DxeOemSvcChipsetLibDefault|$(CHIPSET_PKG)/Library/DxeOemSvcChipsetLib/DxeOemSvcChipsetLibDefault.inf
  DxeOemSvcChipsetLib|$(CHIPSET_PKG)/Library/DxeOemSvcChipsetLib/DxeOemSvcChipsetLib.inf
  ChipsetConfigLib|$(CHIPSET_PKG)/Library/ChipsetConfigLib/DxeChipsetConfigLib.inf
  BaseCryptLib|InsydeModulePkg/Library/BaseCryptLib/RuntimeCryptLib.inf

[LibraryClasses.common.SMM_CORE]
  SmmServicesTableLib|MdeModulePkg/Library/PiSmmCoreSmmServicesTableLib/PiSmmCoreSmmServicesTableLib.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  SmmServicesTableLib|MdePkg/Library/SmmServicesTableLib/SmmServicesTableLib.inf
  SmmLib|$(CHIPSET_PKG)/Library/FchSmmLib/FchSmmLib.inf
  SmmChipsetSvcLib|InsydeModulePkg/Library/SmmChipsetSvcLib/SmmChipsetSvcLib.inf
  SmmOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/SmmOemSvcKernelLib/SmmOemSvcKernelLibDefault.inf
  SmmOemSvcKernelLib|InsydeOemServicesPkg/Library/SmmOemSvcKernelLib/SmmOemSvcKernelLib.inf
  IhisiLib|InsydeModulePkg/Library/IhisiLib/IhisiLib.inf
  LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxSmmLib.inf

[LibraryClasses.common.COMBINED_SMM_DXE]
  SmmChipsetSvcLib|InsydeModulePkg/Library/SmmChipsetSvcLib/SmmChipsetSvcLib.inf
  SmmServicesTableLib|InsydeModulePkg/Library/SmmServicesTableLib/SmmServicesTableLib.inf

###################################################################################################
#
# Components Section - list of the modules and components that will be processed by compilation
#                      tools and the EDK II tools to generate PE32/PE32+/Coff image files.
#
# Note: The EDK II DSC file is not used to specify how compiled binary images get placed
#       into firmware volume images. This section is just a list of modules to compile from
#       source into UEFI-compliant binaries.
#       It is the FDF file that contains information on combining binary files into firmware
#       volume images, whose concept is beyond UEFI and is described in PI specification.
#       Binary modules do not need to be listed in this section, as they should be
#       specified in the FDF file. For example: Shell binary (Shell_Full.efi), FAT binary (Fat.efi),
#       Logo (Logo.bmp), and etc.
#       There may also be modules listed in this section that are not required in the FDF file,
#       When a module listed here is excluded from FDF file, then UEFI-compliant binary will be
#       generated for it, but the binary will not be put into any firmware volume.
#
###################################################################################################

[Components]
#[-start-220513-IB14860781-add]#
  $(CHIPSET_PKG)/Override/InsydeModulePkg/Universal/UserInterface/Fonts/TrueTypeDecoderDxe/TrueTypeDecoderDxe.inf
#[-end-220513-IB14860781-add]#

[Components.IA32]
  #$(CHIPSET_PKG)/Tools/GenOemRomSig/GenOemRomSig.inf
  $(CHIPSET_PKG)/DisableCacheAsRamPei/DisableCacheAsRamPeiBin.inf
  $(CHIPSET_PKG)/ChipsetSvcPei/ChipsetSvcPei.inf
  $(CHIPSET_PKG)/PlatformSpdReadPei/PlatformSpdReadPei.inf
  $(CHIPSET_PKG)/MemoryIntiPei/CpuInstallEfiMemoryPei.inf
  $(CHIPSET_PKG)/MemoryIntiPei/CrisisMemoryInitPei.inf
#  $(CHIPSET_PKG)/SioInitPei/SioInitPei.inf
  $(CHIPSET_PKG)/RecoveryCpuInitPei/RecoveryCpuInitPei.inf  
  $(CHIPSET_PKG)/RecoveryFchInitPei/RecoveryFchInitPei.inf
  $(CHIPSET_PKG)/Compatibility/AcpiVariableHobOnSmramReserveHobThunk/AcpiVariableHobOnSmramReserveHobThunk.inf
  # $(CHIPSET_PKG)/PlatformNotifyDispatchPei/PlatformNotifyDispatchPei.inf
  $(CHIPSET_PKG)/EarlySetPlatformHardwareSwitchPei/EarlySetPlatformHardwareSwitchPei.inf
  $(CHIPSET_PKG)/PlatformCustomizePei/PlatformCustomizePei.inf

[Components.X64]
  $(CHIPSET_PKG)/Tpm/PhysicalPresence/TpmDriverDxe.inf
  $(CHIPSET_PKG)/ChipsetSvcDxe/ChipsetSvcDxe.inf
  $(CHIPSET_PKG)/ChipsetSvcSmm/ChipsetSvcSmm.inf
#   $(CHIPSET_PKG)/SmartTimerDxe/SmartTimerDxe.inf
  $(CHIPSET_PKG)/LegacyRegionDxe/LegacyRegionDxe.inf
  $(CHIPSET_PKG)/PciHostBridgeDxe/PciHostBridgeDxe.inf
  $(CHIPSET_PKG)/SataControllerDxe/SataControllerDxe.inf
  $(CHIPSET_PKG)/SmmAccess2Dxe/SmmAccess2Dxe.inf
  $(CHIPSET_PKG)/OemBadgingSupportDxe/OEMBadgingSupportDxe.inf
  $(CHIPSET_PKG)/UnexpectedIrqWADxe/UnexpectedIrqWA.inf
  $(CHIPSET_PKG)/PlatformSmm/PlatformSmm.inf
  #$(CHIPSET_PKG)/SmmIhisi/IhisiSmm.inf
  $(CHIPSET_PKG)/AcpiTables/AcpiTables.inf
#   $(CHIPSET_PKG)/UefiSetupUtilityDxe/SetupUtilityDxe.inf
  $(CHIPSET_PKG)/PlatformDxe/PlatformDxe.inf
  $(CHIPSET_PKG)/LegacyBiosPlatformHookDxe/LegacyBiosPlatformHookDxe.inf
  #
  # Support UEFI AMD AHCI driver 1.2.1.0
  #
  $(CHIPSET_PKG)/Binary/UefiAhci/1.2.1.0/AhciBus.inf

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
  $(CHIPSET_PKG)/UsbLegacyControl/UsbLegacyControl.inf

  #
  # These module is for CRB only.
  #
  $(CHIPSET_PKG)/CrbAcpiPlatformDxe/CrbAcpiPlatformDxe.inf

  $(CHIPSET_PKG)/Compatibility/FvOnFv2Thunk/FvOnFv2Thunk.inf
  $(CHIPSET_PKG)/Compatibility/LegacyRegion2OnLegacyRegionThunk/LegacyRegion2OnLegacyRegionThunk.inf
  $(CHIPSET_PKG)/Compatibility/DxeOemServicesThunk/DxeOemServicesThunk.inf

  #
  # STIBP feature
  #
  $(CHIPSET_PKG)/StibpFeatureDxe/StibpFeatureDxe.inf
  
