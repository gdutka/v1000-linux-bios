@REM
@REM Project post-build batch file
@REM
@REM ******************************************************************************
@REM * Copyright (c) 2012 - 2022, Insyde Software Corp. All Rights Reserved.
@REM *
@REM * You may not reproduce, distribute, publish, display, perform, modify, adapt,
@REM * transmit, broadcast, present, recite, release, license or otherwise exploit
@REM * any part of this publication in any form, by any means, without the prior
@REM * written permission of Insyde Software Corporation.
@REM *
@REM ******************************************************************************

@REM ============================================
@REM   Run Chipset specific post-build process
@REM ============================================
@GetProjectEnv CHIPSET_PKG > NUL && for /f %%a in ('GetProjectEnv CHIPSET_PKG') do set %%a
@if exist %WORKSPACE%\%CHIPSET_PKG%\ChipsetPostBuild.bat call %WORKSPACE%\%CHIPSET_PKG%\ChipsetPostBuild.bat

@REM Update hash vale of FDM entries at last step of building
for /f "tokens=3" %%a in ('findstr /R "\<TARGET\>" %WORKSPACE%\Conf\target.txt') do (
  PatchFdmHash %WORKSPACE%/Build/%PROJECT_PKG%\%%a_%TOOL_CHAIN%/FV/BILBY.fd %WORKSPACE%/Build/%PROJECT_PKG%\%%a_%TOOL_CHAIN%/FV/BILBY.fdm
)
@REM ============================================
@REM   Delete build files in AGESA
@REM ============================================
@if exist %WORKSPACE%\AmdAgesaPkgConfig\AmdAgesaPkgConfig.bat call %WORKSPACE%\AmdAgesaPkgConfig\AmdAgesaPkgConfig.bat POSTBUILD
@if exist %WORKSPACE%\AmdAgesaPkg\OptionsIds.h del %WORKSPACE%\AmdAgesaPkg\OptionsIds.h

@REM ============================================
@REM   Delete build files in CPM
@REM ============================================
@if exist %WORKSPACE%\AmdCpmPkgConfig\AmdCpmPkgConfig.bat call %WORKSPACE%\AmdCpmPkgConfig\AmdCpmPkgConfig.bat POSTBUILD

@REM ============================================
@REM   Delete build files in CBS
@REM ============================================
@REM @if exist %WORKSPACE%\AmdCbsPkg\Library\Family\0x17\RV\External\xmlparse.bat call %WORKSPACE%\AmdCbsPkg\Library\Family\0x17\RV\External\xmlparse.bat POSTBUILD

@REM FvAlignChecker.exe for 4K Align Check
FvAlignChecker.exe --PCD-report-file %WORKSPACE%/Build/%PROJECT_PKG%/BuildReport.txt

@REM ============================================
@REM   Run kernel post-build process
@REM ============================================
@if exist %WORKSPACE%\BaseTools\KernelPostBuild.bat call %WORKSPACE%\BaseTools\KernelPostBuild.bat %1

@REM ============================================
@REM   Run Project specific post-build process
@REM ============================================
@if exist %WORKSPACE%\%PROJECT_PKG%\ProjectOverride.bat call %WORKSPACE%\%PROJECT_PKG%\ProjectOverride.bat POSTBUILD

@for /f "tokens=3" %%a in ('find "TARGET =" %WORKSPACE%\Conf\target.txt') do @set TARGET=%%a
@for /f "tokens=3" %%a in ('find "TOOL_CHAIN_TAG" %WORKSPACE%\Conf\target.txt') do @set TOOL_CHAIN_TAG=%%a

@for /f "tokens=4" %%a in ('find "RAVEN_16M" Project.env') do @set RAVEN_16M=%%a
@set FV_OUTPUT_PATH=%WORKSPACE%\Build\%PROJECT_PKG%\%TARGET%_%TOOL_CHAIN_TAG%\FV

Set PEI_FD=%FV_OUTPUT_PATH%\BILBYPEI.fd
Set PEI_COMPRESS_FD=%FV_OUTPUT_PATH%\BILBYPEI_COMPRESS.fd
Set PEI_COMPRESS_SIZE=B0000
set COMPRESS_TOOL_PATH=%WORKSPACE%\AgesaModulePkg\AMDTools\CompressBios

@if %RAVEN_16M% == NO (
@echo ###Compress PEI FV ######
@echo %COMPRESS_TOOL_PATH%\CompressBios.exe %PEI_FD% %PEI_COMPRESS_FD% %PEI_COMPRESS_SIZE%
%COMPRESS_TOOL_PATH%\CompressBios.exe %PEI_FD% %PEI_COMPRESS_FD% %PEI_COMPRESS_SIZE%
Set PEI_FD=%PEI_COMPRESS_FD%
)

@if %RAVEN_16M% == YES (
  @call %WORKSPACE%\%PROJECT_PKG%\BIOS\Tools\crc32.exe %FV_OUTPUT_PATH%\FVMAIN_COMPACT.Fv
  @call %WORKSPACE%\%PROJECT_PKG%\BIOS\Tools\dd_crc32.sh %FV_OUTPUT_PATH%\BILBY.fd %FV_OUTPUT_PATH%\FVMAIN_COMPACT.Fv_crc
  copy /b %FV_OUTPUT_PATH%\BILBY.fd+%PEI_FD%  %FV_OUTPUT_PATH%\BILBY.fd
  @call PspBuild16M.bat
) else (
  copy /b %FV_OUTPUT_PATH%\BILBY.fd+%PEI_FD%  %FV_OUTPUT_PATH%\BILBY.fd
  @call PspBuild.bat
)
@echo RAVEN_16M = %RAVEN_16M%

