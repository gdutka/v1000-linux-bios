## @file
#  Platform Package Description file
#
#******************************************************************************
#* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
#*
#* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#* transmit, broadcast, present, recite, release, license or otherwise exploit
#* any part of this publication in any form, by any means, without the prior
#* written permission of Insyde Software Corporation.
#*
#******************************************************************************

[Defines]

[LibraryClasses]
  UefiCpuLib|UefiCpuPkg/Library/BaseUefiCpuLib/BaseUefiCpuLib.inf
  MtrrLib|UefiCpuPkg/Library/MtrrLib/MtrrLib.inf
  LocalApicLib|UefiCpuPkg/Library/BaseXApicX2ApicLib/BaseXApicX2ApicLib.inf
  SmmCpuPlatformHookLib|UefiCpuPkg/Library/SmmCpuPlatformHookLibNull/SmmCpuPlatformHookLibNull.inf
  SmmCpuFeaturesLib|UefiCpuPkg/Library/SmmCpuFeaturesLib/SmmCpuFeaturesLib.inf


[LibraryClasses.common.SEC]
  PlatformSecLib|UefiCpuPkg/Library/PlatformSecLibNull/PlatformSecLibNull.inf
  CpuExceptionHandlerLib|UefiCpuPkg/Library/CpuExceptionHandlerLib/SecPeiCpuExceptionHandlerLib.inf

[LibraryClasses.common.PEI_CORE]

[LibraryClasses.common.PEIM]
  MpInitLib|UefiCpuPkg/Library/MpInitLib/PeiMpInitLib.inf
  RegisterCpuFeaturesLib|UefiCpuPkg/Library/RegisterCpuFeaturesLib/PeiRegisterCpuFeaturesLib.inf

[LibraryClasses.IA32.PEIM, LibraryClasses.X64.PEIM]
  CpuExceptionHandlerLib|UefiCpuPkg/Library/CpuExceptionHandlerLib/PeiCpuExceptionHandlerLib.inf


[LibraryClasses.common.DXE_CORE]

[LibraryClasses.common.DXE_RUNTIME_DRIVER]

[LibraryClasses.common.UEFI_DRIVER]

[LibraryClasses.common.DXE_DRIVER]
  CpuExceptionHandlerLib|UefiCpuPkg/Library/CpuExceptionHandlerLib/DxeCpuExceptionHandlerLib.inf
  MpInitLib|UefiCpuPkg/Library/MpInitLib/DxeMpInitLib.inf
  RegisterCpuFeaturesLib|UefiCpuPkg/Library/RegisterCpuFeaturesLib/DxeRegisterCpuFeaturesLib.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  CpuExceptionHandlerLib|UefiCpuPkg/Library/CpuExceptionHandlerLib/SmmCpuExceptionHandlerLib.inf

[LibraryClasses.common.COMBINED_SMM_DXE]

[LibraryClasses.common.SMM_CORE]

[LibraryClasses.common.UEFI_APPLICATION]

[PcdsFeatureFlag]

[PcdsFixedAtBuild]

[PcdsDynamicDefault]

[Components]
  UefiCpuPkg/Library/MpInitLib/PeiMpInitLib.inf {
    <SOURCE_OVERRIDE_PATH>  
        UefiCpuPkg/Override/Library/MpInitLib    
  }
  
  UefiCpuPkg/Library/MpInitLib/DxeMpInitLib.inf {
    <SOURCE_OVERRIDE_PATH>  
        UefiCpuPkg/Override/Library/MpInitLib    
  }  

[Components.$(PEI_ARCH)]
  UefiCpuPkg/CpuIoPei/CpuIoPei.inf


[Components.$(DXE_ARCH)]
  UefiCpuPkg/CpuIo2Dxe/CpuIo2Dxe.inf
  UefiCpuPkg/CpuIo2Smm/CpuIo2Smm.inf
  #UefiCpuPkg/PiSmmCpuDxeSmm/PiSmmCpuDxeSmm.inf {
  #      <SOURCE_OVERRIDE_PATH>
  #          UefiCpuPkg/Override/PiSmmCpuDxeSmm
  #}
