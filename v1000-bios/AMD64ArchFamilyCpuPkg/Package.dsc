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
  CpuConfigLib|AMD64ArchFamilyCpuPkg/Library/CpuConfigLib/CpuConfigLib.inf
  PlatformSecLib|AMD64ArchFamilyCpuPkg/Library/PlatformSecLibNull/PlatformSecLibNull.inf
  SmmLib|MdePkg/Library/SmmLibNull/SmmLibNull.inf

[LibraryClasses.common.SEC]

[LibraryClasses.common.PEI_CORE]

[LibraryClasses.common.PEIM]

[LibraryClasses.common.DXE_CORE]

[LibraryClasses.common.DXE_RUNTIME_DRIVER]

[LibraryClasses.common.UEFI_DRIVER]

[LibraryClasses.common.DXE_DRIVER]

[LibraryClasses.common.DXE_SMM_DRIVER]

[LibraryClasses.common.COMBINED_SMM_DXE]

[LibraryClasses.common.SMM_CORE]

[LibraryClasses.common.UEFI_APPLICATION]

[PcdsFeatureFlag]
  gEfiCpuTokenSpaceGuid.PcdCpuSmmEnableBspElection|FALSE

[PcdsFixedAtBuild]
  gEfiCpuTokenSpaceGuid.PcdTemporaryRamBase|0xFEF00000
  gEfiCpuTokenSpaceGuid.PcdTemporaryRamSize|0x00010000
  gEfiCpuTokenSpaceGuid.PcdCpuSmmStackSize|0x8000

[PcdsDynamicDefault]

[Components.$(PEI_ARCH)]
  AMD64ArchFamilyCpuPkg/SecCore/SecCore.inf {
    <BuildOptions>
        DEBUG_DEVTLS_*_DLINK_FLAGS == /NOLOGO /IGNORE:4086 /OPT:REF /OPT:ICF=10 /MAP /ALIGN:32 /SECTION:.xdata,D /SECTION:.pdata,D /MACHINE:I386 /LTCG /DLL /ENTRY:$(IMAGE_ENTRY_POINT) /SUBSYSTEM:CONSOLE /SAFESEH:NO /BASE:0 /DRIVER /DEBUG /PDB:$(DEBUG_DIR)/$(BASE_NAME).pdb
  }
  AMD64ArchFamilyCpuPkg/CpuPei/CpuPei.inf
  AMD64ArchFamilyCpuPkg/PiSmmCommunication/PiSmmCommunicationPei.inf

[Components.$(DXE_ARCH)]
  AMD64ArchFamilyCpuPkg/CpuArchDxe/CpuArchDxe.inf
  AMD64ArchFamilyCpuPkg/CpuMpDxe/CpuMpDxe.inf
  AMD64ArchFamilyCpuPkg/PiSmmCpuDxeSmm/PiSmmCpuDxeSmm.inf {
    <LibraryClasses>
      PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf
  }
  AMD64ArchFamilyCpuPkg/PiSmmCommunication/PiSmmCommunicationSmm.inf
