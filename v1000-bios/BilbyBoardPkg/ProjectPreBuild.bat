@REM
@REM Project pre-build batch file
@REM
@REM ******************************************************************************
@REM * Copyright (c) 2012 - 2017, Insyde Software Corp. All Rights Reserved.
@REM *
@REM * You may not reproduce, distribute, publish, display, perform, modify, adapt,
@REM * transmit, broadcast, present, recite, release, license or otherwise exploit
@REM * any part of this publication in any form, by any means, without the prior
@REM * written permission of Insyde Software Corporation.
@REM *
@REM ******************************************************************************

@REM ============================================
@REM   Copy Package files for AGESA
@REM ============================================
@if exist %WORKSPACE%\AmdAgesaPkgConfig\AmdAgesaPkgConfig.bat call %WORKSPACE%\AmdAgesaPkgConfig\AmdAgesaPkgConfig.bat PREBUILD
@if exist %WORKSPACE%\%PROJECT_PKG%\Library\AgesaPlatformLib\OptionsIds.h copy %WORKSPACE%\%PROJECT_PKG%\Library\AgesaPlatformLib\OptionsIds.h %WORKSPACE%\AmdAgesaPkg\OptionsIds.h /y

@REM ============================================
@REM   Copy Override files for AGESA
@REM ============================================

@REM ============================================
@REM   Copy Package files for CPM
@REM ============================================
@if exist %WORKSPACE%\AmdCpmPkgConfig\AmdCpmPkgConfig.bat call %WORKSPACE%\AmdCpmPkgConfig\AmdCpmPkgConfig.bat PREBUILD

@REM ============================================
@REM   Copy Package files for CBS
@REM ============================================
@REM CBS XML parse tool
@set GBS_FILE_NAME=iRavenSetting.xml

@REM [-start-231019-IB18160162-modify]
@if exist %WORKSPACE%\AmdCbsPkg\Tools\IdsIdGen.bat call %WORKSPACE%\AmdCbsPkg\Tools\IdsIdGen.bat
@if exist %WORKSPACE%\AmdCbsPkg\Library\Family\0x17\RV\External\xmlparse.bat call %WORKSPACE%\AmdCbsPkg\Library\Family\0x17\RV\External\xmlparse.bat BILBY
@if exist %wORKSPACE%\%CHIPSET_PKG%\Tools\AdjustCbsFormClassGuid\AdjustCbsFormClassGuid.py ( python %wORKSPACE%\%CHIPSET_PKG%\Tools\AdjustCbsFormClassGuid\AdjustCbsFormClassGuid.py )
@REM [-end-231019-IB18160162-modify]

@call GenApcbBinaryRV.bat

@REM ============================================
@REM   Run kernel pre-build process
@REM ============================================
@if exist %WORKSPACE%\BaseTools\KernelPreBuild.bat call %WORKSPACE%\BaseTools\KernelPreBuild.bat %1
@if not errorlevel 0 goto end

@REM ============================================
@REM   Run Chipset specific pre-build process
@REM ============================================
@GetProjectEnv CHIPSET_PKG > NUL && for /f %%a in ('GetProjectEnv CHIPSET_PKG') do set %%a
@if exist %WORKSPACE%\%CHIPSET_PKG%\ChipsetPreBuild.bat call %WORKSPACE%\%CHIPSET_PKG%\ChipsetPreBuild.bat %1

@REM ============================================
@REM   Run Project specific pre-build process
@REM ============================================
@if exist %WORKSPACE%\%PROJECT_PKG%\ProjectOverride.bat call %WORKSPACE%\%PROJECT_PKG%\ProjectOverride.bat PREBUILD

@REM[-start-191115-IB06070063-add]
@REM !!!Bugbug
@REM Here is temp solution to support .h file overwrite for kernel module,
@REM Kernel will provide a soltuion for chipset or OEM project to overwrite .h file in the future,
@REM than BIOS has to remove below code.
if exist %WORKSPACE%\%PROJECT_PKG%\OverrideHeaderFile\* xcopy %WORKSPACE%\%PROJECT_PKG%\OverrideHeaderFile\* %WORKSPACE%\ /e /y
@REM[-end-191115-IB06070063-add]

:end
