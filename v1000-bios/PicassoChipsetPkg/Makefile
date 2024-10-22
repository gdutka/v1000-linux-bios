## @file
#  Chipset Makefile for project build
#
#******************************************************************************
#* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
#*
#* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#* transmit, broadcast, present, recite, release, license or otherwise exploit
#* any part of this publication in any form, by any means, without the prior
#* written permission of Insyde Software Corporation.
#*
#******************************************************************************

#
# Build options for build targets
#
SIMNOW_BUILD   = "-D SYMBOLIC_DEBUG=YES -D INSYDE_DEBUGGER=NO -D EFI_DEBUG=NO -D EFI_USE_CUSTOM_COMPRESS=NO -D AMD_SIMNOW_ENABLE=YES"
DDT_BUILD      = "-D INSYDE_DEBUGGER=YES"
EFI_DEBUG      = "-D EFI_DEBUG=YES"

## @BuildTarget "IA32 X64", DEBUG, Debug build on AMD SimNow for x86 64-bit CPU Arch.
uefi64simnow:
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(SIMNOW_BUILD)

## @BuildTarget "Common", RELEASE, Release package build for CarrizoChipsetPkg package
chipset: cleancache
  @build -p CarrizoChipsetPkg\CarrizoChipsetPkg.dsc -N -s -b RELEASE

## @BuildTarget "Common", RELEASE, Release package build for AmdCommonChipsetPkg package
CommonChipset: cleancache
  @build -p AmdCommonChipsetPkg\AmdCommonChipsetPkg.dsc -N -s -b RELEASE -D CHIPSET_PKG=CarrizoChipsetPkg

## @BuildTarget "Common", RELEASE, Release package build for Agesa package
Agesa: cleancache
  @build -p AmdAgesaPkg\AmdAgesaPkg.dsc -N -s -b RELEASE -D SOLUTION=CarrizoFp4
  @build -p AmdCpmPkg\AmdCpmPkg.dsc -N -s -b RELEASE -D SOLUTION=CarrizoFp4

## @BuildTarget "Common", RELEASE, Release package build for AmdPspFeaturePkg package
AmdPspFeaturePkg: cleancache
  @build -p AmdPspFeaturePkg\AmdPspFeaturePkg.dsc -N -s -b RELEASE

