## @file
# Platform Package Description file
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
#

[Defines]
!include MdeModulePkg/Package.env

[PcdsFixedAtBuild]
  gEfiMdeModulePkgTokenSpaceGuid.PcdMemoryProfilePropertyMask|0x00

[LibraryClasses]
  SerialPortLib|MdeModulePkg/Library/BaseSerialPortLib16550/BaseSerialPortLib16550.inf
  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  OemHookStatusCodeLib|MdeModulePkg/Library/OemHookStatusCodeLibNull/OemHookStatusCodeLibNull.inf
  SecurityManagementLib|MdeModulePkg/Library/DxeSecurityManagementLib/DxeSecurityManagementLib.inf
!if $(INSYDE_DEBUGGER) == YES
!else
  DebugAgentLib|MdeModulePkg/Library/DebugAgentLibNull/DebugAgentLibNull.inf
!endif
  S3BootScriptLib|MdeModulePkg/Library/PiDxeS3BootScriptLib/DxeS3BootScriptLib.inf {
    <SOURCE_OVERRIDE_PATH>
    MdeModulePkg/Override/Library/PiDxeS3BootScriptLib
  }

  OemHookStatusCodeLib|MdeModulePkg/Library/OemHookStatusCodeLibNull/OemHookStatusCodeLibNull.inf
  CpuExceptionHandlerLib|MdeModulePkg/Library/CpuExceptionHandlerLibNull/CpuExceptionHandlerLibNull.inf
  LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxDxeLib.inf
  PlatformHookLib|MdeModulePkg/Library/BasePlatformHookLibNull/BasePlatformHookLibNull.inf
  UefiBootManagerLib|MdeModulePkg/Library/UefiBootManagerLib/UefiBootManagerLib.inf
  FrameBufferBltLib|MdeModulePkg/Library/FrameBufferBltLib/FrameBufferBltLib.inf
  FileExplorerLib|MdeModulePkg/Library/FileExplorerLib/FileExplorerLib.inf

  NonDiscoverableDeviceRegistrationLib|MdeModulePkg/Library/NonDiscoverableDeviceRegistrationLib/NonDiscoverableDeviceRegistrationLib.inf

  FmpAuthenticationLib|MdeModulePkg/Library/FmpAuthenticationLibNull/FmpAuthenticationLibNull.inf

[LibraryClasses.IA32, LibraryClasses.X64]
  NULL|InsydeModulePkg/Library/IntrinsicLib/IntrinsicLib.inf

[LibraryClasses.common.SEC]
  ReportStatusCodeLib|MdeModulePkg/Library/PeiReportStatusCodeLib/PeiReportStatusCodeLib.inf
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf

[LibraryClasses.common.PEI_CORE]
  ReportStatusCodeLib|MdeModulePkg/Library/PeiReportStatusCodeLib/PeiReportStatusCodeLib.inf
  OemHookStatusCodeLib|MdeModulePkg/Library/OemHookStatusCodeLibNull/OemHookStatusCodeLibNull.inf
  PerformanceLib|MdeModulePkg/Library/PeiPerformanceLib/PeiPerformanceLib.inf

[LibraryClasses.common.PEIM]
  ReportStatusCodeLib|MdeModulePkg/Library/PeiReportStatusCodeLib/PeiReportStatusCodeLib.inf
  OemHookStatusCodeLib|MdeModulePkg/Library/OemHookStatusCodeLibNull/OemHookStatusCodeLibNull.inf
  PerformanceLib|MdeModulePkg/Library/PeiPerformanceLib/PeiPerformanceLib.inf
  LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxPeiLib.inf

[LibraryClasses.common.DXE_CORE]
  MemoryAllocationLib|MdeModulePkg/Library/DxeCoreMemoryAllocationLib/DxeCoreMemoryAllocationLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  PerformanceLib|MdeModulePkg/Library/DxeCorePerformanceLib/DxeCorePerformanceLib.inf
  DebugPrintErrorLevelLib|MdeModulePkg/Library/DxeDebugPrintErrorLevelLib/DxeDebugPrintErrorLevelLib.inf

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  ReportStatusCodeLib|MdeModulePkg/Library/RuntimeDxeReportStatusCodeLib/RuntimeDxeReportStatusCodeLib.inf
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
  LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxDxeLib.inf
  DebugPrintErrorLevelLib|MdeModulePkg/Library/DxeDebugPrintErrorLevelLib/DxeDebugPrintErrorLevelLib.inf

[LibraryClasses.common.UEFI_DRIVER]
  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf

[LibraryClasses.common.DXE_DRIVER]
  ReportStatusCodeLib|MdeModulePkg/Library/DxeReportStatusCodeLib/DxeReportStatusCodeLib.inf
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
  DebugPrintErrorLevelLib|MdeModulePkg/Library/DxeDebugPrintErrorLevelLib/DxeDebugPrintErrorLevelLib.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  ReportStatusCodeLib|MdeModulePkg/Library/SmmReportStatusCodeLib/SmmReportStatusCodeLib.inf
  PerformanceLib|MdeModulePkg/Library/SmmPerformanceLib/SmmPerformanceLib.inf
  LockBoxLib|MdeModulePkg/Library/SmmLockBoxLib/SmmLockBoxSmmLib.inf

[LibraryClasses.common.COMBINED_SMM_DXE]
  PerformanceLib|MdePkg/Library/BasePerformanceLibNull/BasePerformanceLibNull.inf

[LibraryClasses.common.SMM_CORE]
  SmmServicesTableLib|MdeModulePkg/Library/PiSmmCoreSmmServicesTableLib/PiSmmCoreSmmServicesTableLib.inf
  ReportStatusCodeLib|MdeModulePkg/Library/SmmReportStatusCodeLib/SmmReportStatusCodeLib.inf
  MemoryAllocationLib|MdeModulePkg/Library/PiSmmCoreMemoryAllocationLib/PiSmmCoreMemoryAllocationLib.inf {
    <SOURCE_OVERRIDE_PATH>
    MdeModulePkg/Override/Library/PiSmmCoreMemoryAllocationLib
  }
  SmmCorePlatformHookLib|MdeModulePkg/Library/SmmCorePlatformHookLibNull/SmmCorePlatformHookLibNull.inf

[LibraryClasses.common.UEFI_APPLICATION]
  PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf

[PcdsFeatureFlag]
!if $(EFI_DEBUG) == YES
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseSerial|TRUE
!else
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseSerial|FALSE
!endif
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseMemory|FALSE
  gEfiMdeModulePkgTokenSpaceGuid.PcdDxeIplSupportUefiDecompress|TRUE
!if $(DXE_ARCH) == X64
  gEfiMdeModulePkgTokenSpaceGuid.PcdDxeIplSwitchToLongMode|TRUE
!else
  gEfiMdeModulePkgTokenSpaceGuid.PcdDxeIplSwitchToLongMode|FALSE
!endif
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutGopSupport|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdConOutUgaSupport|TRUE
  gEfiMdeModulePkgTokenSpaceGuid.PcdSupportUpdateCapsuleReset|TRUE


[PcdsFixedAtBuild]
  gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeMemorySize|1
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxVariableSize|0x1000
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxHardwareErrorVariableSize|0x8000
  gEfiMdeModulePkgTokenSpaceGuid.PcdVpdBaseAddress|0x0
!if $(FIRMWARE_PERFORMANCE) == YES
  gEfiMdeModulePkgTokenSpaceGuid.PcdMaxPeiPerformanceLogEntries|128
!endif
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVendor|L"Insyde"

[PcdsDynamicDefault]
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareRevision|0x00000000
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareVersionString|L""|VOID*|64
  gEfiMdeModulePkgTokenSpaceGuid.PcdFirmwareReleaseDateString|L""|VOID*|64

[PcdsDynamicExDefault]
  gEfiMdeModulePkgTokenSpaceGuid.PcdS3BootScriptTablePrivateDataPtr|0x0
  gEfiMdeModulePkgTokenSpaceGuid.PcdS3BootScriptTablePrivateSmmDataPtr|0x0

[Components.$(PEI_ARCH)]

  MdeModulePkg/Library/PeiPerformanceLib/PeiPerformanceLib.inf {
    <SOURCE_OVERRIDE_PATH>
      MdeModulePkg/Override/Library/PeiPerformanceLib
  }
  MdeModulePkg/Library/DxeCorePerformanceLib/DxeCorePerformanceLib.inf {
    <SOURCE_OVERRIDE_PATH>
      MdeModulePkg/Override/Library/DxeCorePerformanceLib
  }
  MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf {
    <SOURCE_OVERRIDE_PATH>
      MdeModulePkg/Override/Library/DxePerformanceLib
  }

  MdeModulePkg/Core/Pei/PeiMain.inf {
    <SOURCE_OVERRIDE_PATH>
      MdeModulePkg/Override/Core/Pei
    <BuildOptions>
      *_*_*_CC_FLAGS = -D MDEPKG_NDEBUG
  }

  MdeModulePkg/Universal/PCD/Pei/Pcd.inf  {
    <LibraryClasses>
      PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
    <SOURCE_OVERRIDE_PATH>
      MdeModulePkg/Override/Universal/PCD/Pei
  }
  MdeModulePkg/Universal/PcatSingleSegmentPciCfg2Pei/PcatSingleSegmentPciCfg2Pei.inf
!if gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseSerial == 1 || gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseMemory == 1
  MdeModulePkg/Universal/StatusCodeHandler/Pei/StatusCodeHandlerPei.inf {
    <SOURCE_OVERRIDE_PATH>
      MdeModulePkg/Override/Universal/StatusCodeHandler/Pei
    <Packages>
      InsydeModulePkg/InsydeModulePkg.dec
    <PcdsFixedAtBuild>
      gInsydeTokenSpaceGuid.PcdSerialUsage|1
  }
!endif
  MdeModulePkg/Universal/Acpi/FirmwarePerformanceDataTablePei/FirmwarePerformancePei.inf {
    <SOURCE_OVERRIDE_PATH>
      MdeModulePkg/Override/Universal/Acpi/FirmwarePerformanceDataTablePei
  }

  MdeModulePkg/Universal/CapsulePei/CapsulePei.inf

[Components.$(DXE_ARCH)]

  MdeModulePkg/Library/PeiPerformanceLib/PeiPerformanceLib.inf {
    <SOURCE_OVERRIDE_PATH>
      MdeModulePkg/Override/Library/PeiPerformanceLib
  }
  MdeModulePkg/Library/DxeCorePerformanceLib/DxeCorePerformanceLib.inf {
    <SOURCE_OVERRIDE_PATH>
      MdeModulePkg/Override/Library/DxeCorePerformanceLib
  }
  MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf {
    <SOURCE_OVERRIDE_PATH>
      MdeModulePkg/Override/Library/DxePerformanceLib
  }

  MdeModulePkg/Core/Dxe/DxeMain.inf {
    <SOURCE_OVERRIDE_PATH>
      MdeModulePkg/Override/Core/Dxe
  }
  MdeModulePkg/Universal/Metronome/Metronome.inf
  MdeModulePkg/Core/RuntimeDxe/RuntimeDxe.inf
  MdeModulePkg/Universal/WatchdogTimerDxe/WatchdogTimer.inf
!if gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseSerial == 1 || gEfiMdeModulePkgTokenSpaceGuid.PcdStatusCodeUseMemory == 1
  MdeModulePkg/Universal/StatusCodeHandler/RuntimeDxe/StatusCodeHandlerRuntimeDxe.inf {
    <Packages>
      InsydeModulePkg/InsydeModulePkg.dec
    <PcdsFixedAtBuild>
      gInsydeTokenSpaceGuid.PcdSerialUsage|1
  }
!endif
  MdeModulePkg/Universal/Acpi/FirmwarePerformanceDataTableDxe/FirmwarePerformanceDxe.inf {
    <SOURCE_OVERRIDE_PATH>
      MdeModulePkg/Override/Universal/Acpi/FirmwarePerformanceDataTableDxe
  }
  MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf
  MdeModulePkg/Universal/EbcDxe/EbcDxe.inf
  MdeModulePkg/Core/PiSmmCore/PiSmmIpl.inf
  MdeModulePkg/Core/PiSmmCore/PiSmmCore.inf {
    <SOURCE_OVERRIDE_PATH>
      MdeModulePkg/Override/Core/PiSmmCore
  }
  MdeModulePkg/Universal/StatusCodeHandler/Smm/StatusCodeHandlerSmm.inf {
    <Packages>
      InsydeModulePkg/InsydeModulePkg.dec
    <PcdsFixedAtBuild>
      gInsydeTokenSpaceGuid.PcdSerialUsage|1
  }
  MdeModulePkg/Universal/Acpi/FirmwarePerformanceDataTableSmm/FirmwarePerformanceSmm.inf {
    <SOURCE_OVERRIDE_PATH>
      MdeModulePkg/Override/Universal/Acpi/FirmwarePerformanceDataTableSmm
  }

  MdeModulePkg/Universal/Acpi/SmmS3SaveState/SmmS3SaveState.inf
  MdeModulePkg/Universal/LockBox/SmmLockBox/SmmLockBox.inf
  MdeModulePkg/Universal/Console/ConPlatformDxe/ConPlatformDxe.inf
  MdeModulePkg/Universal/PrintDxe/PrintDxe.inf
  MdeModulePkg/Universal/Disk/DiskIoDxe/DiskIoDxe.inf

  MdeModulePkg/Universal/SmmCommunicationBufferDxe/SmmCommunicationBufferDxe.inf
!if gEfiMdeModulePkgTokenSpaceGuid.PcdMemoryProfilePropertyMask
  MdeModulePkg/Application/MemoryProfileInfo/MemoryProfileInfo.inf
!endif
  MdeModulePkg/Universal/BootManagerPolicyDxe/BootManagerPolicyDxe.inf  {
    <LibraryClasses>
      UefiBootManagerLib|MdeModulePkg/Library/UefiBootManagerLib/UefiBootManagerLib.inf
  }
  MdeModulePkg/Universal/TimestampDxe/TimestampDxe.inf