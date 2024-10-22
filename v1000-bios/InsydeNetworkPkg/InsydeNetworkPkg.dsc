## @file
#  Package dscription file for InsydeNetworkPkg
#
#******************************************************************************
#* Copyright (c) 2012 - 2019, Insyde Software Corp. All Rights Reserved.
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
  PLATFORM_NAME                  = InsydeNetwork
  PLATFORM_GUID                  = FC916EFA-EE77-4824-9756-E8B617047A84
  PLATFORM_VERSION               = 1.0
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/InsydeNetworkPkg
  SUPPORTED_ARCHITECTURES        = IA32|X64|ARM
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT

[BuildOptions]
  GCC:*_*_*_CC_FLAGS             = -DMDEPKG_NDEBUG
  INTEL:*_*_*_CC_FLAGS           = /D MDEPKG_NDEBUG
  MSFT:RELEASE_*_*_CC_FLAGS      = /D MDEPKG_NDEBUG

[Packages]
  CryptoPkg/CryptoPkg.dec|InsydeModulePkg/InsydeModulePkg.dec

################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################
[LibraryClasses]
  BaseMemoryLib|MdePkg/Library/BaseMemoryLibRepStr/BaseMemoryLibRepStr.inf
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf {
    <SOURCE_OVERRIDE_PATH>
      MdePkg/Override/Library/BaseLib
  }
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  DxeServicesLib|MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  PeCoffGetEntryPointLib|MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  DxeServicesTableLib|MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  SafeIntLib|MdePkg/Library/BaseSafeIntLib/BaseSafeIntLib.inf
  TimerLib|MdePkg/Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf

  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  UefiBootManagerLib|MdeModulePkg/Library/UefiBootManagerLib/UefiBootManagerLib.inf

  ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf

  BaseCryptLib|InsydeModulePkg/Library/BaseCryptLib/RuntimeCryptLib.inf
  KernelConfigLib|InsydeModulePkg/Library/DxeKernelConfigLib/DxeKernelConfigLib.inf
  H2ODebugLib|InsydeModulePkg/Library/DxeH2ODebugLib/DxeH2ODebugLib.inf
  VariableLib|InsydeModulePkg/Library/DxeVariableLib/DxeVariableLib.inf
  IrsiRegistrationLib|InsydeModulePkg/Library/Irsi/IrsiRegistrationLibNull/IrsiRegistrationLibNull.inf
  VariableSupportLib|InsydeModulePkg/Library/VariableSupportLib/VariableSupportLib.inf
  OemGraphicsLib|InsydeModulePkg/Library/OemGraphicsLib/OemGraphicsLib.inf
  FlashRegionLib|InsydeModulePkg/Library/FlashRegionLib/DxeFlashRegionLib/DxeFlashRegionLib.inf
  H2OCpLib|InsydeModulePkg/Library/DxeH2OCpLib/DxeH2OCpLib.inf
  PostCodeLib|InsydeModulePkg/Library/BasePostCodeLib/BasePostCodeLib.inf
  IntrinsicLib|InsydeModulePkg/Library/IntrinsicLib/IntrinsicLib.inf
  GenericUtilityLib|InsydeModulePkg/Library/GenericUtilityLib/GenericUtilityLib.inf

  DxeOemSvcKernelLibDefault|InsydeOemServicesPkg/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLibDefault.inf
  DxeOemSvcKernelLib|InsydeOemServicesPkg/Library/DxeOemSvcKernelLib/DxeOemSvcKernelLib.inf

  DpcLib|InsydeNetworkPkg/Library/DxeDpcLib/DxeDpcLib.inf
  NetLib|InsydeNetworkPkg/Library/DxeNetLib/DxeNetLib.inf
  IpIoLib|InsydeNetworkPkg/Library/DxeIpIoLib/DxeIpIoLib.inf
  UdpIoLib|InsydeNetworkPkg/Library/DxeUdpIoLib/DxeUdpIoLib.inf
  TcpIoLib|InsydeNetworkPkg/Library/DxeTcpIoLib/DxeTcpIoLib.inf
  HttpLib|InsydeNetworkPkg/Library/DxeHttpLib/DxeHttpLib.inf

  InsydeNetworkLib|InsydeNetworkPkg/Library/InsydeNetworkLib/InsydeNetworkLib.inf
  OpensslLib|InsydeNetworkPkg/Library/OpensslLib/OpensslLib.inf
  TlsLib|InsydeNetworkPkg/Library/TlsLib/TlsLib.inf

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsFeatureFlag]

[PcdsFixedAtBuild]

[PcdsPatchableInModule]

[PcdsDynamicExDefault]
  #
  # PXE procedure cancel hot key definition
  # Key struct definition {KeyCode, ScanCode, [Next hot key], 0x00, 0x00[END]}
  #
  gInsydeTokenSpaceGuid.PcdPxeCancelHotKeyDefinition|{ \
    0x00, 0x17, \ # ESC
    0x00, 0x00  \ # list end
    }
  gInsydeTokenSpaceGuid.PcdPxeCancelHotKeyString|"[ESC]"

  #
  # Define how much retry times to detect network media.
  # Set to 0, it will detect network media once.
  #
  gInsydeTokenSpaceGuid.PcdH2OPxeCheckMediaRetryCount|2

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
  #InsydeNetworkPkg/Drivers/NetworkLockerDxe/NetworkLockerDxe.inf
  #InsydeNetworkPkg/Drivers/DhcpDummyDxe/DhcpDummyDxe.inf
  #InsydeNetworkPkg/Drivers/PxeDummyDxe/PxeDummyDxe.inf
  #InsydeNetworkPkg/Drivers/DpcDxe/DpcDxe.inf
  #InsydeNetworkPkg/Drivers/MnpDxe/MnpDxe.inf
  #InsydeNetworkPkg/Drivers/ArpDxe/ArpDxe.inf
  #InsydeNetworkPkg/Drivers/SnpDxe/SnpDxe.inf
  #InsydeNetworkPkg/Drivers/Ip4Dxe/Ip4Dxe.inf
  #InsydeNetworkPkg/Drivers/Ip4ConfigDxe/Ip4ConfigDxe.inf
  #InsydeNetworkPkg/Drivers/Udp4Dxe/Udp4Dxe.inf
  #InsydeNetworkPkg/Drivers/Dhcp4Dxe/Dhcp4Dxe.inf
  #InsydeNetworkPkg/Drivers/Mtftp4Dxe/Mtftp4Dxe.inf
  #InsydeNetworkPkg/Drivers/Ip6Dxe/Ip6Dxe.inf
  #InsydeNetworkPkg/Drivers/Udp6Dxe/Udp6Dxe.inf
  #InsydeNetworkPkg/Drivers/Dhcp6Dxe/Dhcp6Dxe.inf
  #InsydeNetworkPkg/Drivers/Mtftp6Dxe/Mtftp6Dxe.inf
  #InsydeNetworkPkg/Drivers/UefiPxeBcDxe/UefiPxeBcDxe.inf
  #InsydeNetworkPkg/Drivers/TcpDxe/TcpDxe.inf
  #InsydeNetworkPkg/Drivers/IScsiDxe/IScsiDxe.inf
  #InsydeNetworkPkg/Drivers/DnsDxe/DnsDxe.inf
  #InsydeNetworkPkg/Drivers/HttpDxe/HttpDxe.inf
  #InsydeNetworkPkg/Drivers/HttpUtilitiesDxe/HttpUtilitiesDxe.inf
  #InsydeNetworkPkg/Drivers/HttpBootDxe/HttpBootDxe.inf
  #InsydeNetworkPkg/Drivers/RestDxe/RestDxe.inf
  #InsydeNetworkPkg/Drivers/TlsDxe/TlsDxe.inf

[Components.IA32, Components.X64]
  #InsydeNetworkPkg/Application/ImportCA/ImportCA.inf
  #InsydeNetworkPkg/Application/RestTest/RestTest.inf
