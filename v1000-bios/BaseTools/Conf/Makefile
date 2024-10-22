## @file
#  Common Makefile for project build
#
#******************************************************************************
#* Copyright (c) 2013 - 2020, Insyde Software Corp. All Rights Reserved.
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
DDT_BUILD      = -D INSYDE_DEBUGGER=YES
EFI_DEBUG      = -D EFI_DEBUG=YES
DDT_DEBUG      = -D INSYDE_DEBUGGER=YES -D EFI_DEBUG=YES
EFI_DEBUG_DDT  = -D INSYDE_DEBUGGER=YES -D EFI_DEBUG=YES
PERF_BUILD     = -D SYMBOLIC_DEBUG=YES -D INSYDE_DEBUGGER=NO -D EFI_DEBUG=NO -D FIRMWARE_PERFORMANCE=YES
EFI_DEBUG_USB  = -D EFI_DEBUG=YES -D USB_DEBUG_SUPPORT=YES
SECURE_FLASH   = -D SECURE_FLASH_SUPPORT=YES
FAST_CRISIS    = -D USE_FAST_CRISIS_RECOVERY=YES
PEI_CRISIS     = -D USE_FAST_CRISIS_RECOVERY=NO
EMULATOR_NOSCU_BUILD = "-D EMULATOR_NOSCU"
SCA_BUILD      = -D SOURCE_CODE_ANALYSIS_BUILD=YES

!ifndef BUILD_MODE
#
# Available build modes
# -q: Quiet mode - disable all messages except fatal errors, module based messages
# -s: Silent mode - file based messages
# -v: Turn on verbose output with informational messages printed
#
BUILD_MODE     = -q $(PROJECT_BUILD_MODE) --Dynamic-to-DynamicEx

#@Prompt DdtDebug:description about what the DdtDebug is.
!ifdef DdtDebug
BUILD_MODE = $(BUILD_MODE) $(DDT_BUILD)
!endif

#@Prompt EfiDebug: description about what the EfiDebug is.
!ifdef EfiDebug
BUILD_MODE = $(BUILD_MODE) $(EFI_DEBUG)
!endif

#@Prompt FastCrisis:description about what the FastCrisis is.
!ifdef FastCrisis
BUILD_MODE = $(BUILD_MODE) $(SECURE_FLASH) $(FAST_CRISIS)
!endif

#@Prompt PeiCrisis:description about what the PeiCrisis is.
!ifdef PeiCrisis
BUILD_MODE = $(BUILD_MODE) $(SECURE_FLASH) $(PEI_CRISIS)
!endif

#@Prompt ScaBuild:Using cl /analyze for source code analysis
!ifdef ScaBuild
BUILD_MODE = $(BUILD_MODE) $(SCA_BUILD)
!endif

!endif

!ifndef BUILD_REPORT
BUILD_REPORT = -Y PCD -Y DEPEX -Y LIBRARY -y $(WORKSPACE)/Build/$(PROJECT_PKG)/BuildReport.txt
!endif


## @BuildTarget "IA32 X64", RELEASE, Release build for x86 64-bit CPU Arch.
uefi64:
  @set H2O_BUILD_TYPE=$@
  $(MAKE) release BUILD_TARGET=$@ BUILD_MODE="$(BUILD_MODE)"

## @BuildTarget "IA32", RELEASE, Release build for x86 32-bit CPU Arch.
uefi32:
  @set H2O_BUILD_TYPE=$@
  $(MAKE) release BUILD_TARGET=$@ BUILD_MODE="$(BUILD_MODE)"

## @BuildTarget "IA32 X64", DEBUG, Debug build for Insyde Debugger
uefi64ddt:
  @set H2O_BUILD_TYPE=$@
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS="$(DDT_BUILD)"

## @BuildTarget "IA32", DEBUG, Debug build for Insyde Debugger
uefi32ddt:
  @set H2O_BUILD_TYPE=$@
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS="$(DDT_BUILD)"

## @BuildTarget "Common", DEBUG, Debug build with EFI debug messages via serial port
efidebug:
  set H2O_BUILD_TYPE=$@
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS="$(EFI_DEBUG)"

## @BuildTarget "IA32 X64", DEBUG, Debug build with EFI debug messages via serial port
debug64:
  @set H2O_BUILD_TYPE=$@
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS="$(EFI_DEBUG)"

## @BuildTarget "IA32", DEBUG, Debug build with EFI debug messages via serial port
debug32:
  @set H2O_BUILD_TYPE=$@
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS="$(EFI_DEBUG)"

## @BuildTarget "Common", DEBUG, Debug build with EFI debug messages via USB port
efidebugusb:
  @set H2O_BUILD_TYPE=$@
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS="$(EFI_DEBUG_USB)"

## @BuildTarget "IA32 X64", DEBUG, Debug build with EFI debug messages via USB port
debug64usb:
  @set H2O_BUILD_TYPE=$@
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS="$(EFI_DEBUG_USB)"

## @BuildTarget "IA32", DEBUG, Debug build with EFI debug messages via USB port
debug32usb:
  @set H2O_BUILD_TYPE=$@
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS="$(EFI_DEBUG_USB)"

## @BuildTarget "Common", DEBUG, Debug build with EFI debug messages via serial port and Insyde Debugger application
efidebugddt:
  @set H2O_BUILD_TYPE=$@
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS="$(EFI_DEBUG_DDT)"

## @BuildTarget "IA32 X64", DEBUG, Debug build with EFI debug messages via serial port and Insyde Debugger application
debug64ddt:
  @set H2O_BUILD_TYPE=$@
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS="$(EFI_DEBUG_DDT)"

## @BuildTarget "IA32", DEBUG, Debug build with EFI debug messages via serial port and Insyde Debugger application
debug32ddt:
  @set H2O_BUILD_TYPE=$@
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS="$(EFI_DEBUG_DDT)"

## @BuildTarget "Common", DEBUG, Debug build with EFI debug messages via DDT cable and Insyde Debugger application
ddtdebug:
  @set H2O_BUILD_TYPE=$@
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS="$(DDT_DEBUG)"

## @BuildTarget "IA32 X64", DEBUG, Debug build with EFI debug messages via DDT cable and Insyde Debugger application
ddtdebug64:
  @set H2O_BUILD_TYPE=$@
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS="$(DDT_DEBUG)"

## @BuildTarget "IA32", DEBUG, Debug build with EFI debug messages via DDT cable and Insyde Debugger application
ddtdebug32:
  @set H2O_BUILD_TYPE=$@
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS="$(DDT_DEBUG)"

## @BuildTarget "IA32 X64", DEBUG, Performance-log enabled build for x86 64-bit CPU Arch.
uefi64perf:
  @set H2O_BUILD_TYPE=$@
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS="$(PERF_BUILD)"

## @BuildTarget "IA32", DEBUG, Performance-log enabled build for x86 32-bit CPU Arch.
uefi32perf:
  @set H2O_BUILD_TYPE=$@
  @$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS="$(PERF_BUILD)"

## @BuildTarget "IA32 X64", RELEASE, SCA enabled build for x86 64-bit CPU Arch.
uefi64sca:
  @set H2O_BUILD_TYPE=$@
  @$(MAKE) release BUILD_TARGET=$@ BUILD_TARGET_OPTIONS="$(SCA_BUILD)"

## @BuildTarget "IA32", RELEASE, SCA enabled build for x86 32-bit CPU Arch.
uefi32sca:
  @set H2O_BUILD_TYPE=$@
  @$(MAKE) release BUILD_TARGET=$@ BUILD_TARGET_OPTIONS="$(SCA_BUILD)"


## @BuildTarget "Common", RELEASE, Release package build for InsydeModulePkg package with SOURCE_CODE_ANALYSIS_BUILD
kernelsca:
  @$(MAKE) /nologo kernel BUILD_TARGET_OPTIONS="$(SCA_BUILD)" ScaBuild=

## @BuildTarget "Common", RELEASE, Release package build for InsydeModulePkg package
kernel: cleancache
  set PROJECT_PKG=InsydeModulePkg
  nmake /nologo genfont
  nmake /nologo genlayout
  set PROJECT_PKG=
  @build -p InsydeModulePkg\InsydeModulePkg.dsc -N -s -b RELEASE $(BUILD_TARGET_OPTIONS) $(BUILD_PCD_VALUE) --Dynamic-to-DynamicEx
!IFDEF ScaBuild
  @$(MAKE) /nologo genkernelscareport
!ENDIF

genkernelscareport:
  if not exist $(WORKSPACE)\InsydeModulePkg\KernelSourceCodeAnalysis.csv echo.> $(WORKSPACE)\InsydeModulePkg\KernelSourceCodeAnalysis.csv
  echo. > $(WORKSPACE)\Build\InsydeModulePkg\RELEASE_$(TOOL_CHAIN)\ScaBuildLog.txt
  for /f %%i in ('dir /s /b  $(WORKSPACE)\Build\InsydeModulePkg\RELEASE_$(TOOL_CHAIN)\*.analysis.txt') do @type %%i >> $(WORKSPACE)\Build\InsydeModulePkg\RELEASE_$(TOOL_CHAIN)\ScaBuildLog.txt
  SourceCodeAnalyzer --logfile $(WORKSPACE)\Build\InsydeModulePkg\RELEASE_$(TOOL_CHAIN)\ScaBuildLog.txt --inputfile $(WORKSPACE)\InsydeModulePkg\KernelSourceCodeAnalysis.csv --outputfile $(WORKSPACE)\InsydeModulePkg\KernelSourceCodeAnalysis.csv


nt32noscu:
  @$(MAKE) /nologo nt32 BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(EMULATOR_NOSCU_BUILD)

nt64noscu:
  @$(MAKE) /nologo nt64 BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(EMULATOR_NOSCU_BUILD)

## @BuildTarget "IA32", DEBUG, Build NT32 emulation package
## Before buliding DSC file, temporarily set PROJECT_PKG as NtEmulator32 for output folder of genfont and genlayout
nt32: cleancache gen_debug_target clean_debug_var clean_debug_vfr gen_build_pcd_value
!IF "$(PROJECT_PKG)" == ""
  @for /f %%i in ('GetProjectEnv PROJECT_PKG') do $(MAKE) /nologo $@ BUILD_TARGET=$@ %i
!ELSE
!IF "$(BUILD_PCD_VALUE)" == ""
  @for /f "tokens=*" %%a in ($(WORKSPACE)\Build\BuildPcdValue.txt) do $(MAKE) $@ BUILD_PCD_VALUE="%%a"
!ELSE
  nmake /nologo genfont genlayout
  if exist ProjectPreBuild.bat call ProjectPreBuild.bat nt32
  @build -N -q -p Build/$(PROJECT_PKG)/Project.dsc -s -a IA32 -b DEBUG $(BUILD_TARGET_OPTIONS) $(BUILD_PCD_VALUE) --Dynamic-to-DynamicEx -D CHIPSET_REL_PATH=$(CHIPSET_REL_PATH) -D PROJECT_REL_PATH=$(PROJECT_REL_PATH)
  if exist ProjectPostBuild.bat call ProjectPostBuild.bat nt32

  @if not exist $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\IA32\EFI\Dev1\EFI\Microsoft\Boot mkdir $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\IA32\EFI\Dev1\EFI\Microsoft\Boot
  @if not exist $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\IA32\EFI\Dev2\EFI\redhat mkdir $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\IA32\EFI\Dev2\EFI\redhat
  @if not exist $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\IA32\EFI\Dev3\EFI\Boot mkdir $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\IA32\EFI\Dev3\EFI\Boot
  @if not exist $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\IA32\EFI\Dev4\EFI\Boot mkdir $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\IA32\EFI\Dev4\EFI\Boot
  @copy /y $(WORKSPACE)\ShellBinPkg\UefiShell\IA32\Shell.efi $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\IA32\EFI\Dev1\EFI\Microsoft\Boot\bootmgfw.efi
  @copy /y $(WORKSPACE)\ShellBinPkg\UefiShell\IA32\Shell.efi $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\IA32\EFI\Dev2\EFI\redhat\grub.efi
  @copy /y $(WORKSPACE)\ShellBinPkg\UefiShell\IA32\Shell.efi $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\IA32\EFI\Dev3\EFI\Boot\bootx64.efi
  @copy /y $(WORKSPACE)\ShellBinPkg\UefiShell\IA32\Shell.efi $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\IA32\EFI\Dev4\EFI\Boot\bootx64.efi
!ENDIF
!ENDIF

## @BuildTarget "X64", DEBUG, Build NT64 emulation package
## Before buliding DSC file, temporarily set PROJECT_PKG as NtEmulator64 for output folder of genfont and genlayout
nt64: cleancache gen_debug_target clean_debug_var gen_build_pcd_value
!IF "$(PROJECT_PKG)" == ""
  @for /f %%i in ('GetProjectEnv PROJECT_PKG') do $(MAKE) /nologo $@ BUILD_TARGET=$@ %i
!ELSE
!IF "$(BUILD_PCD_VALUE)" == ""
  @for /f "tokens=*" %%a in ($(WORKSPACE)\Build\BuildPcdValue.txt) do $(MAKE) $@ BUILD_PCD_VALUE="%%a"
!ELSE
  nmake /nologo genfont genlayout
  if exist ProjectPreBuild.bat call ProjectPreBuild.bat nt64
  @build -N -q -p Build/$(PROJECT_PKG)/Project.dsc -s -a X64 -b DEBUG $(BUILD_TARGET_OPTIONS) $(BUILD_PCD_VALUE) --Dynamic-to-DynamicEx -D CHIPSET_REL_PATH=$(CHIPSET_REL_PATH) -D PROJECT_REL_PATH=$(PROJECT_REL_PATH)
  if exist ProjectPostBuild.bat call ProjectPostBuild.bat nt64

  @if not exist $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\X64\EFI\Dev1\EFI\Microsoft\Boot mkdir $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\X64\EFI\Dev1\EFI\Microsoft\Boot
  @if not exist $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\X64\EFI\Dev2\EFI\redhat mkdir $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\X64\EFI\Dev2\EFI\redhat
  @if not exist $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\X64\EFI\Dev3\EFI\Boot mkdir $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\X64\EFI\Dev3\EFI\Boot
  @if not exist $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\X64\EFI\Dev4\EFI\Boot mkdir $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\X64\EFI\Dev4\EFI\Boot
  @copy /y $(WORKSPACE)\ShellBinPkg\UefiShell\X64\Shell.efi $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\X64\EFI\Dev1\EFI\Microsoft\Boot\bootmgfw.efi
  @copy /y $(WORKSPACE)\ShellBinPkg\UefiShell\X64\Shell.efi $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\X64\EFI\Dev2\EFI\redhat\grub.efi
  @copy /y $(WORKSPACE)\ShellBinPkg\UefiShell\X64\Shell.efi $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\X64\EFI\Dev3\EFI\Boot\bootx64.efi
  @copy /y $(WORKSPACE)\ShellBinPkg\UefiShell\X64\Shell.efi $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\X64\EFI\Dev4\EFI\Boot\bootx64.efi
!ENDIF
!ENDIF

## @BuildTarget "IA32", DEBUG, Run NT32 emulation
run: cleancache
!IF "$(PROJECT_PKG)" == ""
  @GetProjectEnv PROJECT_PKG > NUL
  @for /f %%i in ('GetProjectEnv PROJECT_PKG') do $(MAKE) $@ %i
!ELSE
  cd %WORKSPACE%\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\IA32 & %WORKSPACE%\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\IA32\SecMain.exe
!ENDIF


## @BuildTarget "X64", DEBUG, Run NT64 emulation
run64: cleancache
!IF "$(PROJECT_PKG)" == ""
  @GetProjectEnv PROJECT_PKG > NUL
  @for /f %%i in ('GetProjectEnv PROJECT_PKG') do $(MAKE) $@ %i
!ELSE
  cd %WORKSPACE%\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\X64 & %WORKSPACE%\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\X64\SecMain.exe
!ENDIF

cleancache:
  @if exist $(WORKSPACE)\Conf\.cache rmdir $(WORKSPACE)\Conf\.cache /s /q

clean: cleancache
  @if exist $(WORKSPACE)\Build rmdir $(WORKSPACE)\Build /s /q

release: cleancache gen_release_target genfont genlayout clean_release_var clean_release_vfr gen_build_pcd_value
!IF exist ($(WORKSPACE)\Build\$(PROJECT_PKG)\RELEASE_$(TOOL_CHAIN)\Makefile) 
  @set H2O_CLEAN_BUILD=FALSE
!ELSE
  @set H2O_CLEAN_BUILD=TRUE
!ENDIF
!IF "$(PROJECT_PKG)" == ""
  @for /f %%i in ('GetProjectEnv PROJECT_PKG') do $(MAKE) $@ %i
!ELSE
!IF "$(BUILD_PCD_VALUE)" == ""
  @for /f "tokens=*" %%a in ($(WORKSPACE)\Build\BuildPcdValue.txt) do $(MAKE) $@ BUILD_PCD_VALUE="%%a"
!ELSE
  @ReportBuildInfo -START 
  @call $(WORKSPACE)\BaseTools\Bin\Win32\BuildTimeReport.bat PreBuildStartTime
  if exist ProjectPreBuild.bat call ProjectPreBuild.bat $(BUILD_TARGET)
  @call $(WORKSPACE)\BaseTools\Bin\Win32\BuildTimeReport.bat PreBuildEndTime
  @call $(WORKSPACE)\BaseTools\Bin\Win32\BuildTimeReport.bat BuildStartTime
  @call <<$(WORKSPACE)\Build\$(PROJECT_PKG)\BuildHandleError.bat  
build -N $(BUILD_MODE) $(BUILD_TARGET_OPTIONS) $(BUILD_PCD_VALUE) $(BUILD_REPORT) $(PROJECT_BUILD_OPTIONS)
@if errorlevel 1 goto error
@goto done
:error
@ReportBuildInfo -FAILED
@exit /b 1
:done
<<NOKEEP 
  @call $(WORKSPACE)\BaseTools\Bin\Win32\BuildTimeReport.bat BuildEndTime
  @call $(WORKSPACE)\BaseTools\Bin\Win32\BuildTimeReport.bat PostBuildStartTime
  if exist ProjectPostBuild.bat call ProjectPostBuild.bat $(BUILD_TARGET)
  @call $(WORKSPACE)\BaseTools\Bin\Win32\BuildTimeReport.bat PostBuildEndTime
  @call $(WORKSPACE)\BaseTools\Bin\Win32\BuildTimeReport.bat
  @ReportBuildInfo -SUCCESS 
!ENDIF
!ENDIF

debug: cleancache gen_debug_target genfont genlayout clean_debug_var clean_debug_vfr gen_build_pcd_value
!IF exist ($(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\Makefile) 
  @set H2O_CLEAN_BUILD=FALSE
!ELSE
  @set H2O_CLEAN_BUILD=TRUE
!ENDIF
!IF "$(PROJECT_PKG)" == ""
  @for /f %%i in ('GetProjectEnv PROJECT_PKG') do $(MAKE) $@ %i
!ELSE
!IF "$(BUILD_PCD_VALUE)" == ""
  @for /f "tokens=*" %%a in ($(WORKSPACE)\Build\BuildPcdValue.txt) do $(MAKE) $@ BUILD_PCD_VALUE="%%a"
!ELSE
  @ReportBuildInfo -START 
  @call $(WORKSPACE)\BaseTools\Bin\Win32\BuildTimeReport.bat PreBuildStartTime
  if exist ProjectPreBuild.bat call ProjectPreBuild.bat $(BUILD_TARGET)
  @call $(WORKSPACE)\BaseTools\Bin\Win32\BuildTimeReport.bat PreBuildEndTime
  @call $(WORKSPACE)\BaseTools\Bin\Win32\BuildTimeReport.bat BuildStartTime
  @call <<$(WORKSPACE)\Build\$(PROJECT_PKG)\BuildHandleError.bat  
build -N $(BUILD_MODE) $(BUILD_TARGET_OPTIONS) $(BUILD_PCD_VALUE) $(BUILD_REPORT) $(PROJECT_BUILD_OPTIONS)
@if errorlevel 1 goto error
@goto done
:error
@ReportBuildInfo -FAILED
@exit /b 1
:done
<<NOKEEP 
  @call $(WORKSPACE)\BaseTools\Bin\Win32\BuildTimeReport.bat BuildEndTime
  @call $(WORKSPACE)\BaseTools\Bin\Win32\BuildTimeReport.bat PostBuildStartTime
  if exist ProjectPostBuild.bat call ProjectPostBuild.bat $(BUILD_TARGET)
  @call $(WORKSPACE)\BaseTools\Bin\Win32\BuildTimeReport.bat PostBuildEndTime
  @call $(WORKSPACE)\BaseTools\Bin\Win32\BuildTimeReport.bat
  @ReportBuildInfo -SUCCESS
!ENDIF
!ENDIF

genfont:
!IF "$(PROJECT_PKG)" != ""
  @create_temporary_font_hpk.bat
!ENDIF

genlayout:
!IF "$(PROJECT_PKG)" != ""
  @create_temporary_layout_hpk.bat
!ENDIF

gen_build_pcd_value:
!IF "$(BUILD_PCD_VALUE)" == ""
#
# Set PCD values if string tokens are defined in Project.uni.
#
# For STR_CCB_VERSION                  , transfer "CCB12.34.56" to 0x12345600 (PcdH2OCcbVersion).
# For STR_ESRT_VERSION                 , transfer "12345678"    to 0x12345678 (PcdH2OEsrtSystemFirmwareVersion).
# For STR_ESRT_LOWEST_SUPPORTED_VERSION, transfer "12345678"    to 0x12345678 (PcdH2OEsrtSystemFirmwareLowestSupportedVersion).
# For STR_ESRT_FIRMWARE_GUID           , transfer "12345678-90ab-cdef-1122-334455667788" to {0x78,0x56,0x34,0x12,0xab,0x90,0xef,0xcd,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88} (PcdH2OEsrtSystemFirmwareGuid).
#
  @if not exist  $(WORKSPACE)\Build mkdir  $(WORKSPACE)\Build
  @if exist $(WORKSPACE)\Build\BuildPcdValue.txt del $(WORKSPACE)\Build\BuildPcdValue.txt
  -@for /f "tokens=1-3 delims=.ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz" %%a in ('GetProjectEnv PROJECT_UNI_STR_VALUE STR_CCB_VERSION') do \
    @set /p="--pcd gInsydeTokenSpaceGuid.PcdH2OCcbVersion=0x%%a%%b%%c00 " <nul >> $(WORKSPACE)\Build\BuildPcdValue.txt
  -@for /f %%a in ('GetProjectEnv PROJECT_UNI_STR_VALUE STR_ESRT_VERSION') do \
    @set /p="--pcd gInsydeTokenSpaceGuid.PcdH2OEsrtSystemFirmwareVersion=0x%%a " <nul >> $(WORKSPACE)\Build\BuildPcdValue.txt
  -@for /f %%a in ('GetProjectEnv PROJECT_UNI_STR_VALUE STR_ESRT_LOWEST_SUPPORTED_VERSION') do \
    @set /p="--pcd gInsydeTokenSpaceGuid.PcdH2OEsrtSystemFirmwareLowestSupportedVersion=0x%%a " <nul >> $(WORKSPACE)\Build\BuildPcdValue.txt
  -@for /f %%a in ('GetProjectEnv PROJECT_UNI_STR_VALUE STR_ESRT_FIRMWARE_GUID') do \
    @call <<String2Guid.bat %%a
      @set g=%1
      @set /p="--pcd gInsydeTokenSpaceGuid.PcdH2OEsrtSystemFirmwareGuid=H{0x%g:~6,2%,0x%g:~4,2%,0x%g:~2,2%,0x%g:~0,2%,0x%g:~11,2%,0x%g:~9,2%,0x%g:~16,2%,0x%g:~14,2%,0x%g:~19,2%,0x%g:~21,2%,0x%g:~24,2%,0x%g:~26,2%,0x%g:~28,2%,0x%g:~30,2%,0x%g:~32,2%,0x%g:~34,2%} " <nul >> $(WORKSPACE)\Build\BuildPcdValue.txt
<<

  @if not exist $(WORKSPACE)\Build\BuildPcdValue.txt @echo --pcd gInsydeTokenSpaceGuid.PcdH2OCcbVersion=0 > $(WORKSPACE)\Build\BuildPcdValue.txt
!ELSE
  @if exist $(WORKSPACE)\Build\BuildPcdValue.txt del $(WORKSPACE)\Build\BuildPcdValue.txt
!ENDIF

clean_release_var:
  @if exist $(WORKSPACE)\Build\$(PROJECT_PKG)\RELEASE_$(TOOL_CHAIN)\FV\VarFile rmdir $(WORKSPACE)\Build\$(PROJECT_PKG)\RELEASE_$(TOOL_CHAIN)\FV\VarFile /s /q

clean_debug_var:
  @if exist $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\FV\VarFile rmdir $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\FV\VarFile /s /q

clean_release_vfr:
  @if exist $(WORKSPACE)\Build\$(PROJECT_PKG)\RELEASE_$(TOOL_CHAIN)\FV\VfrFile rmdir $(WORKSPACE)\Build\$(PROJECT_PKG)\RELEASE_$(TOOL_CHAIN)\FV\VfrFile /s /q

clean_debug_vfr:
  @if exist $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\FV\VfrFile rmdir $(WORKSPACE)\Build\$(PROJECT_PKG)\DEBUG_$(TOOL_CHAIN)\FV\VfrFile /s /q


gen_release_target:
  @echo ACTIVE_PLATFORM              = Build/$(PROJECT_PKG)/Project.dsc >  $(WORKSPACE)\Conf\target.txt
  @echo TARGET_ARCH                  = $(ARCH)                          >> $(WORKSPACE)\Conf\target.txt
  @echo TARGET                       = RELEASE                          >> $(WORKSPACE)\Conf\target.txt
  @echo TOOL_CHAIN_CONF              = Conf/tools_def.txt               >> $(WORKSPACE)\Conf\target.txt
  @echo TOOL_CHAIN_TAG               = $(TOOL_CHAIN)                    >> $(WORKSPACE)\Conf\target.txt
  @echo MAX_CONCURRENT_THREAD_NUMBER = $(NUMBER_OF_PROCESSORS)          >> $(WORKSPACE)\Conf\target.txt
  @echo BUILD_RULE_CONF              = Conf/build_rule.txt              >> $(WORKSPACE)\Conf\target.txt

gen_debug_target:
  @echo ACTIVE_PLATFORM              = Build/$(PROJECT_PKG)/Project.dsc >  $(WORKSPACE)\Conf\target.txt
  @echo TARGET_ARCH                  = $(ARCH)                          >> $(WORKSPACE)\Conf\target.txt
  @echo TARGET                       = DEBUG                            >> $(WORKSPACE)\Conf\target.txt
  @echo TOOL_CHAIN_CONF              = Conf/tools_def.txt               >> $(WORKSPACE)\Conf\target.txt
  @echo TOOL_CHAIN_TAG               = $(TOOL_CHAIN)                    >> $(WORKSPACE)\Conf\target.txt
  @echo MAX_CONCURRENT_THREAD_NUMBER = $(NUMBER_OF_PROCESSORS)          >> $(WORKSPACE)\Conf\target.txt
  @echo BUILD_RULE_CONF              = Conf/build_rule.txt              >> $(WORKSPACE)\Conf\target.txt

