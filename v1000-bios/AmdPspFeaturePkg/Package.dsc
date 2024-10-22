## @file
#  Platform Package Description file
#
#******************************************************************************
#* Copyright (c) 2012 - 2017, Insyde Software Corp. All Rights Reserved.
#*
#* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#* transmit, broadcast, present, recite, release, license or otherwise exploit
#* any part of this publication in any form, by any means, without the prior
#* written permission of Insyde Software Corporation.
#*
#******************************************************************************

[Defines]
  !include AmdPspFeaturePkg/Package.env

[LibraryClasses]
  #
  # AGESA PSP Lib
  #
#  PspfTpmLib|AmdAgesaPkg/UefiUdk/Psp/PspCZPkg/Library/PspfTpmLib/PspfTpmLib.inf
##  PspMboxLib|AmdAgesaPkg/UefiUdk/Psp/PspCZPkg/Library/PspMboxLib/PspMboxLib.inf
##  AgesaDebugNullLib|AmdAgesaPkg/UefiUdk/Psp/PspCZPkg/Library/AgesaDebugNullLib/AgesaDebugNullLib.inf 
  #
  # Platform PSP Lib
  #


[LibraryClasses.common.PEIM]

[LibraryClasses.common.DXE_SMM_DRIVER]
##  CsSmmLib|AmdAgesaPkg/UefiUdk/Psp/PspCZPkg/Library/CsSmmLib/CsSmmLib.inf
AmdPspFlashAccLib|AmdPspFeaturePkg/Library/AmdPspFlashAccLibSmm/AmdPspFlashAccLibSmm.inf

[PcdsFeatureFlag]
  gAmdPspFeaturePkgTokenSpaceGuid.PcdPspFirmwareTpmSupported|TRUE
  gAmdPspFeaturePkgTokenSpaceGuid.PcdVerifyFvSupported|FALSE

[Components.$(PEI_ARCH)]
  #
  # AGESA PSP Drivers
  #
!if gAmdPspFeaturePkgTokenSpaceGuid.PcdPspFirmwareTpmSupported
##  AmdAgesaPkg/UefiUdk/Psp/PspCZPkg/Drivers/iTpmPei/iTpmPei.inf
  AmdPspFeaturePkg/Drivers/fTPMTisPei/fTPMTisPei.inf
!endif
#  AmdAgesaPkg/UefiUdk/Psp/PspCZPkg/Drivers/PspPei/PspPei.inf
  #
  # Platform PSP Driver
  #
!if gAmdPspFeaturePkgTokenSpaceGuid.PcdVerifyFvSupported
  AmdPspFeaturePkg/Drivers/VerifyFvPei/VerifyFvPei.inf
!endif
  #
  # Helper tool modules
  #
!if gAmdPspFeaturePkgTokenSpaceGuid.PcdVerifyFvSupported
  AmdPspFeaturePkg/Tools/GenFvHash/GenFvHash.inf
  AmdPspFeaturePkg/Tools/PatchFvHash/PatchFvHash.inf
!endif
  
[Components.$(DXE_ARCH)]
  #
  # AGESA PSP Drivers
  #
!if gAmdPspFeaturePkgTokenSpaceGuid.PcdPspFirmwareTpmSupported
##  AmdAgesaPkg/UefiUdk/Psp/PspCZPkg/Drivers/iTpmDxe/iTpmDxe.inf
  AmdPspFeaturePkg/Drivers/fTPMTisDxe/fTPMTisDxe.inf {
    <LibraryClasses>
      AmdPspCommonLib|AgesaModulePkg/Library/AmdPspCommonLibDxe/AmdPspCommonLibDxe.inf
  }
  AmdPspFeaturePkg/Drivers/fTPMTcg2Smm/fTPMTcg2Smm.inf
  AmdPspFeaturePkg/Drivers/PspFactoryReset/PspFactoryReset.inf
!endif
#  AmdAgesaPkg/UefiUdk/Psp/PspCZPkg/Drivers/PspDxe/PspDxe.inf
#  AmdAgesaPkg/UefiUdk/Psp/PspCZPkg/Drivers/PspSmm/PspSmm.inf
#  AmdAgesaPkg/UefiUdk/Psp/PspCZPkg/Drivers/PspP2Cmbox/PspP2Cmbox.inf
#  AmdAgesaPkg/UefiUdk/Psp/PspCZPkg/Drivers/CsSmm/CsSmmDriver.inf

  #
  # Platform PSP Driver
  #
  #AmdPspFeaturePkg/Drivers/NewRsmSampleDriver/NewRsmSampleDriver.inf
  #AmdPspFeaturePkg/Drivers/PspSaveApob/PspSaveApob.inf 