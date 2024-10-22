## @file
#
#  SIO Package Description file
#
#******************************************************************************
#* Copyright 2017 Insyde Software Corp. All Rights Reserved.
#*
#* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#* transmit, broadcast, present, recite, release, license or otherwise exploit
#* any part of this publication in any form, by any means, without the prior
#* written permission of Insyde Software Corp.
#*
#******************************************************************************

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]

################################################################################
#
# Library Class section - list of all Library Classes needed by this Platform.
#
################################################################################
[LibraryClasses]
  IoDecodeLib|SioDummyPkg/Library/IoDecodeLibNull/IoDecodeLibNull.inf

[LibraryClasses.common.PEIM]
  SioDummyPrivateLib|SioDummyPkg/Library/PeiSioDummyPrivateLib/PeiSioDummyPrivateLib.inf

[LibraryClasses.common.DXE_CORE]
  SioDummyPrivateLib|SioDummyPkg/Library/DxeSioDummyPrivateLib/DxeSioDummyPrivateLib.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  SioDummyPrivateLib|SioDummyPkg/Library/DxeSioDummyPrivateLib/DxeSioDummyPrivateLib.inf

[LibraryClasses.common.UEFI_DRIVER]
  SioDummyPrivateLib|SioDummyPkg/Library/DxeSioDummyPrivateLib/DxeSioDummyPrivateLib.inf

[LibraryClasses.common.DXE_DRIVER]
  SioDummyPrivateLib|SioDummyPkg/Library/DxeSioDummyPrivateLib/DxeSioDummyPrivateLib.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  SioDummyPrivateLib|SioDummyPkg/Library/DxeSioDummyPrivateLib/DxeSioDummyPrivateLib.inf

[LibraryClasses.common.UEFI_APPLICATION]
  SioDummyPrivateLib|SioDummyPkg/Library/DxeSioDummyPrivateLib/DxeSioDummyPrivateLib.inf

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################
[PcdsFeatureFlag]
  gSioGuid.PcdH2OSioDummySupported|TRUE
  gSioGuid.PcdH2OSioDummyChip1Supported|TRUE

[PcdsFixedAtBuild]
  gSioGuid.PcdH2OSioDummyChip1Uart1Supported|TRUE
  gSioGuid.PcdH2OSioDummyChip1Uart2Supported|TRUE
  gSioGuid.PcdH2OSioDummyChip1KbcSupported|TRUE

[PcdsFixedAtBuild]
#  gSioGuid.PcdH2OSioDummyChip1IsaDeviceUid| { \
#  # Type | DI | UID
#  #------------------------------------------------------------------
#                                  #    Type           |      DI       |     C.R. (UID)    |  OS (UID)  |
#    0x02, 0x01, UINT16(0xFFFF), \ # SioParallelDev    | DevInstance1  | ----------Auto Config--------- |
#    0x03, 0x01, UINT16(0x00),   \ # SioUartDev        | DevInstance1  | COM1              |   COM1     |
#    0x03, 0x02, UINT16(0x01),   \ # SioUartDev        | DevInstance2  | COM2              |   COM2     |
#    0x03, 0x03, UINT16(0x02),   \ # SioUartDev        | DevInstance3  | ISA_UART (COM3)   |   COM3     |
#    0x03, 0x04, UINT16(0x03),   \ # SioUartDev        | DevInstance4  | ISA_UART (COM4)   |   COM4     |
#    0x03, 0x05, UINT16(0x04),   \ # SioUartDev        | DevInstance5  | ISA_UART (COM5)   |   COM5     |
#    0x03, 0x06, UINT16(0x05),   \ # SioUartDev        | DevInstance6  | ISA_UART (COM6)   |   COM6     |
#    0x04, 0x01, UINT16(0xFFFF), \ # SioInfraredDev    | DevInstance1  | ----------Auto Config--------- |
#    0x04, 0x02, UINT16(0xFFFF)  \ # SioInfraredDev    | DevInstance2  | ----------Auto Config--------- |                        
#  }

[Components.$(PEI_ARCH)]
!if gSioGuid.PcdH2OSioDummySupported
!if gSioGuid.PcdH2OSioDummyChip1Supported
  SioDummyPkg/SioDummyPei/SioDummyPei.inf
!endif
!endif

[Components.$(DXE_ARCH)]

  !disable InsydeModulePkg/Universal/IsaAcpiDxe/IsaAcpi.inf
  InsydeModulePkg/Universal/IsaAcpiDxe/IsaAcpi.inf {
    <SOURCE_OVERRIDE_PATH>
      SioDummyPkg/Override/InsydeModulePkg/Universal/IsaAcpiDxe
  }
  
!if gSioGuid.PcdH2OSioDummySupported
!if gSioGuid.PcdH2OSioDummyChip1Supported
  SioDummyPkg/SioChip1InitDxe/SioChip1InitDxe.inf
!endif
  SioDummyPkg/SioDummyDxe/SioDummyDxe.inf
!endif
