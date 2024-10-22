## @file
#  Package dscription file for AmdPspFeaturePkg
#
#******************************************************************************
#* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
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
  PLATFORM_NAME                  = AmdPspFeaturePkg
  PLATFORM_GUID                  = a7e607df-a337-443f-be20-7af8fd49a60a
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/AmdPspFeaturePkg
  SUPPORTED_ARCHITECTURES        = IA32|X64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT

################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################
[LibraryClasses]
  BaseLib                               |MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib                         |MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  DebugLib                              |MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  UefiDriverEntryPoint                  |MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiBootServicesTableLib              |MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  MemoryAllocationLib                   |MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  PcdLib                                |MdePkg/Library/DxePcdLib/DxePcdLib.inf
  #
  # AGESA PSP Lib
  #
  PspfTpmLib                            |AmdAgesaPkg/UefiUdk/Psp/PspCZPkg/Library/PspfTpmLib/PspfTpmLib.inf
  PspMboxLib                            |AmdAgesaPkg/UefiUdk/Psp/PspCZPkg/Library/PspMboxLib/PspMboxLib.inf
  AgesaDebugNullLib                     |AmdAgesaPkg/UefiUdk/Psp/PspCZPkg/Library/AgesaDebugNullLib/AgesaDebugNullLib.inf
  #
  # Platform PSP Lib
  #

[LibraryClasses.common.PEIM]
  PeimEntryPoint                        |MdePkg/Library/PeimEntryPoint/PeimEntryPoint.inf
  MemoryAllocationLib                   |MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf
  PcdLib                                |MdePkg/Library/PeiPcdLib/PeiPcdLib.inf
  PeiServicesLib                        |MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
  PeiServicesTablePointerLib            |MdePkg/Library/PeiServicesTablePointerLib/PeiServicesTablePointerLib.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  SmmServicesTableLib                   |MdePkg/Library/SmmServicesTableLib/SmmServicesTableLib.inf
  MemoryAllocationLib                   |MdePkg/Library/SmmMemoryAllocationLib/SmmMemoryAllocationLib.inf
  CsSmmLib                              |AmdAgesaPkg/UefiUdk/Psp/PspCZPkg/Library/CsSmmLib/CsSmmLib.inf

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsFeatureFlag]

[PcdsFixedAtBuild]

[Components.IA32]
  #
  # AGESA PSP Drivers
  #
  AmdAgesaPkg/UefiUdk/Psp/PspCZPkg/Drivers/iTpmPei/iTpmPei.inf
  AmdAgesaPkg/UefiUdk/Psp/PspCZPkg/Drivers/PspPei/PspPei.inf

[Components.X64]
  #
  # AGESA PSP Drivers
  #
  AmdAgesaPkg/UefiUdk/Psp/PspCZPkg/Drivers/iTpmDxe/iTpmDxe.inf
  AmdAgesaPkg/UefiUdk/Psp/PspCZPkg/Drivers/PspDxe/PspDxe.inf
  AmdAgesaPkg/UefiUdk/Psp/PspCZPkg/Drivers/PspSmm/PspSmm.inf
  AmdAgesaPkg/UefiUdk/Psp/PspCZPkg/Drivers/PspP2Cmbox/PspP2Cmbox.inf
  AmdAgesaPkg/UefiUdk/Psp/PspCZPkg/Drivers/CsSmm/CsSmmDriver.inf
  #
  # Platform PSP Driver
  #
  AmdPspFeaturePkg/Drivers/PspPlatformDriver/PspPlatform.inf
  AmdPspFeaturePkg/Drivers/NewRsmSampleDriver/NewRsmSampleDriver.inf
