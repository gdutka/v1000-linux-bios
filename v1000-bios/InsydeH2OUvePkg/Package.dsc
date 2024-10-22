## @file
# Insyde H2OUve package project build description file.
#
#******************************************************************************
#* Copyright (c) 2016, Insyde Software Corporation. All Rights Reserved.
#*
#* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#* transmit, broadcast, present, recite, release, license or otherwise exploit
#* any part of this publication in any form, by any means, without the prior
#* written permission of Insyde Software Corporation.
#*
#******************************************************************************
#

[Defines]
  DEFINE  INSYDE_H2OUVE_PKG          = InsydeH2OUvePkg

################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################
[LibraryClasses]
  SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf

[Components.$(DXE_ARCH)]
!if gH2OUvePkgTokenSpaceGuid.PcdH2OUveSupported

  $(INSYDE_H2OUVE_PKG)/ConfigUtility/H2OUveConfigUtilDxe/H2OUveConfigUtilDxe.inf
  
!if gH2OUvePkgTokenSpaceGuid.PcdH2OUveSampleFormSupported
  $(INSYDE_H2OUVE_PKG)/ConfigUtility/H2OUveSampleFormUtility/H2OUveSampleFormUtil.inf
!endif  

  $(INSYDE_H2OUVE_PKG)/VariableEditDxe/VariableEditDxe.inf
  $(INSYDE_H2OUVE_PKG)/VariableEditSmm/VariableEditSmm.inf
  
!endif  

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsFeatureFlag]
gH2OUvePkgTokenSpaceGuid.PcdH2OUveSupported|TRUE

[PcdsFixedAtBuild]

#
#Register Ihisi sub function table list.
#Table struct define {CmdNumber, AsciiFuncGuid, Priority}
# UINT8(CmdNumber), Char8[20](AsciiFuncGuid), UINT8(Priority)
##================  ========================  ===============
gH2OUvePkgTokenSpaceGuid.PcdH2OUveIhisiRegisterTable|{ \
  # Register IHISI AH=52h ()
  UINT8(0x52),      "S52OemH2OSmiCallBac",    UINT8(0x80), \
  UINT8(0x52),      "S52OemSetupCBSync00",    UINT8(0x40), \

  # Register IHISI AH=53h ()
  UINT8(0x53),      "S53OemBootInfoSvc00",    UINT8(0x80)  }

  #
  # Use Advanced formset GUID as default so that the H2OUve Config Utility page could be put under Advanced menu.
  #
  gH2OUvePkgTokenSpaceGuid.PcdH2OUveClassGuid|{GUID("C6D4769E-7F48-4D2A-98E9-87ADCCF35CCC")}

[PcdsDynamicExDefault]
  #
  # This setting should match the default value in InsydeModulePkg.dec. 
  # here only changes the properties of the PCD.
  #
  gInsydeTokenSpaceGuid.PcdLegacyBootTypeOrder|{0x01, 0x02, 0x03, 0x05, 0xEE, 0x00}

