## @file
#  Platform Package Description file
#
#*******************************************************************************
#* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
#*
#* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#* transmit, broadcast, present, recite, release, license or otherwise exploit
#* any part of this publication in any form, by any means, without the prior
#* written permission of Insyde Software Corporation.
#*
#*******************************************************************************

[Defines]

[LibraryClasses]
  AmdExtCmosLib|AmdCommonChipsetPkg/Library/AmdExtCmosLibNull/AmdExtCmosLibNull.inf

[LibraryClasses.common.SEC]

[LibraryClasses.common.PEI_CORE]

[LibraryClasses.common.PEIM]
  AmdExtCmosLib|AmdCommonChipsetPkg/Library/AmdExtCmosLibNull/AmdExtCmosLibNull.inf

[LibraryClasses.common.DXE_CORE]

[LibraryClasses.common.DXE_RUNTIME_DRIVER]

[LibraryClasses.common.UEFI_DRIVER]

[LibraryClasses.common.DXE_DRIVER]

[LibraryClasses.common.DXE_SMM_DRIVER]

[LibraryClasses.common.COMBINED_SMM_DXE]

[LibraryClasses.common.SMM_CORE]

[LibraryClasses.common.UEFI_APPLICATION]

[PcdsFeatureFlag]

[PcdsFixedAtBuild]

[PcdsDynamicDefault]

[Components.$(PEI_ARCH)]
  AmdCommonChipsetPkg/InstallVerbTablePei/InstallVerbTablePei.inf
  AmdCommonChipsetPkg/SpdDataReadPei/SpdDataReadPei.inf
  #AmdCommonChipsetPkg/SpeakerPei/LegacySpeakerPei.inf

[Components.$(DXE_ARCH)]
  AmdCommonChipsetPkg/SmartTimerDxe/SmartTimerDxe.inf
  AmdCommonChipsetPkg/InstallVerbTableDxe/InstallVerbTableDxe.inf
!if gAmdCommonChipsetPkgTokenSpaceGuid.PcdMemoryContextSaveRestoreSupport
  AmdCommonChipsetPkg/AmdMemContextSaveRestoreDxe/AmdMemContextSaveRestoreDxe.inf
!endif
  AmdCommonChipsetPkg/SaveSpdToRomDxe/SaveSpdToRomDxe.inf
  #AmdCommonChipsetPkg/SpeakerDxe/LegacySpeakerDxe.inf
  AmdCommonChipsetPkg/SetSsidSvidDxe/SetSsidSvidDxe.inf
