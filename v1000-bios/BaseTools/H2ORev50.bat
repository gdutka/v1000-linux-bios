@REM ******************************************************************************
@REM * Copyright (c) 2012 - 2019, Insyde Software Corp. All Rights Reserved.
@REM *
@REM * You may not reproduce, distribute, publish, display, perform, modify, adapt,
@REM * transmit, broadcast, present, recite, release, license or otherwise exploit
@REM * any part of this publication in any form, by any means, without the prior
@REM * written permission of Insyde Software Corporation.
@REM *
@REM ******************************************************************************

@echo off

REM reset tool path environment variable
if not defined PREVIOUSPATH set PREVIOUSPATH=%PATH%
set PATH=%PREVIOUSPATH%
set WORKSPACE_TOOLS_PATH=


if "%TOOL_CHAIN%"=="" goto :AutoSetup
echo Select TOOL_CHAIN : %TOOL_CHAIN%
if "%TOOL_CHAIN%"=="DEVTLS_VC9" goto :ManualSetup
if "%TOOL_CHAIN%"=="DEVTLS_VC10" goto :ManualSetup
if "%TOOL_CHAIN%"=="DEVTLS_VC14" goto :ManualSetup
if "%TOOL_CHAIN%"=="DEVTLS_ARMGCC" goto :ManualSetup
if "%TOOL_CHAIN%"=="DEVTLS_YAGARTO" goto :ManualSetup
:AutoSetup
call %WORKSPACE%\BaseTools\get_vsvars.bat %TOOL_CHAIN%
if defined VCINSTALLDIR goto :AutoToolChainSetup

:ManualSetup
if not defined TOOL_CHAIN goto Error
if not defined DEVTLS_DIR set DEVTLS_DIR=DEVTLS

REM ---------------------------------------------------------------------------------------------
REM TOOL_CHAIN  == DEVTLS_VC14     Insyde DEVTLS with MSVC 2015
REM             == DEVTLS_VC10     Insyde DEVTLS with MSVC 2010
REM             == DEVTLS_VC9      Insyde DEVTLS with MSVC 2008
REM             == VS2017          Microsoft Visual Studio 2017 (native)
REM             == VS2015          Microsoft Visual Studio 2015 (native)
REM             == VS2015x86       Microsoft Visual Studio 2015 32-bit under Windows 64-bit OS
REM             == VS2013          Microsoft Visual Studio 2013 (native)
REM             == VS2013x86       Microsoft Visual Studio 2013 32-bit under Windows 64-bit OS
REM             == VS2012          Microsoft Visual Studio 2012 (native)
REM             == VS2012x86       Microsoft Visual Studio 2012 32-bit under Windows 64-bit OS
REM             == VS2010          Microsoft Visual Studio 2010 (native)
REM             == VS2010x86       Microsoft Visual Studio 2010 32-bit under Windows 64-bit OS
REM             == VS2008          Microsoft Visual Studio 2008 (native)
REM             == VS2008x86       Microsoft Visual Studio 2008 32-bit under Windows 64-bit OS
REM             == DEVTLS_ARMGCC   Code Sourcery G++ for ARM GCC toolchain
REM             == DEVTLS_ARM64GCC Code Sourcery G++ for AARCH64 GCC toolchain
REM             == DEVTLS_YAGARTO  Yet Another ARM Toolchain
REM             == ARMGCC          Code Sourcery G++ for ARM GCC toolchain
REM             == YAGARTO         Yet Another ARM Toolchain
REM             == WINWDK          Microsoft Windows Driver Kit
REM ---------------------------------------------------------------------------------------------
if %TOOL_CHAIN%==DEVTLS_VC14    goto EnvDEVTLS_VC14
if %TOOL_CHAIN%==DEVTLS_VC10    goto EnvDEVTLS_VC10
if %TOOL_CHAIN%==DEVTLS_VC9     goto EnvDEVTLS_VC9
if %TOOL_CHAIN%==VS2017         goto EnvVS2017
if %TOOL_CHAIN%==VS2015         goto EnvVS2015
if %TOOL_CHAIN%==VS2015x86      goto EnvVS2015x86
if %TOOL_CHAIN%==VS2013         goto EnvVS2013
if %TOOL_CHAIN%==VS2013x86      goto EnvVS2013x86
if %TOOL_CHAIN%==VS2012         goto EnvVS2012
if %TOOL_CHAIN%==VS2012x86      goto EnvVS2012x86
if %TOOL_CHAIN%==VS2010         goto EnvVS2010
if %TOOL_CHAIN%==VS2010x86      goto EnvVS2010x86
if %TOOL_CHAIN%==VS2008         goto EnvVS2008
if %TOOL_CHAIN%==VS2008x86      goto EnvVS2008x86
if %TOOL_CHAIN%==DEVTLS_ARMGCC  goto EnvDEVTLS_ARMGCC
if %TOOL_CHAIN%==DEVTLS_ARM64GCC  goto EnvDEVTLS_ARM64GCC
if %TOOL_CHAIN%==DEVTLS_YAGARTO goto EnvDEVTLS_YAGARTO
if %TOOL_CHAIN%==ARMGCC         goto EnvARMGCC
if %TOOL_CHAIN%==YAGARTO        goto EnvYAGARTO
if %TOOL_CHAIN%==WINWDK         goto EnvWINWDK

:Error
echo.
echo Supported Tool Chains
echo ==========================================================================
echo DEVTLS_VC14     Insyde DEVTLS with MSVC 2015
echo DEVTLS_VC10     Insyde DEVTLS with MSVC 2010
echo DEVTLS_VC9      Insyde DEVTLS with MSVC 2008
echo WINWDK          Microsoft Windows Development Kit
echo VS2017          Microsoft Visual Studio 2017
echo VS2015          Microsoft Visual Studio 2015(native)
echo VS2015x86       Microsoft Visual Studio 2015 32-bit under Windows 64-bit OS
echo VS2013          Microsoft Visual Studio 2013 (native)
echo VS2013x86       Microsoft Visual Studio 2013 32-bit under Windows 64-bit OS
echo VS2012          Microsoft Visual Studio 2012 (native)
echo VS2012x86       Microsoft Visual Studio 2012 32-bit under Windows 64-bit OS
echo VS2010          Microsoft Visual Studio 2010 (native)
echo VS2010x86       Microsoft Visual Studio 2010 32-bit under Windows 64-bit OS
echo VS2008          Microsoft Visual Studio 2008 (native)
echo VS2008x86       Microsoft Visual Studio 2008 32-bit under Windows 64-bit OS
echo.
echo DEVTLS_ARMGCC   Code Sourcery G++ for ARM GCC toolchain (Insyde DEVTLS)
echo DEVTLS_YAGARTO  Yet Another ARM Toolchain               (Insyde DEVTLS)
echo ARMGCC          Code Sourcery G++ for ARM GCC toolchain (normal installation)
echo YAGARTO         Yet Another ARM Toolchain               (normal installation)
echo ==========================================================================

pause
goto End

:AutoToolChainSetup
set TOOL_CHAIN_PATH=%VSINSTALLDIR%
set SDK_DIR=%WindowsSdkDir%
goto ToolChainSetting

:EnvDEVTLS_VC14
set TOOL_DIR=%DEVTLS_DIR%\MSVC14
set SDK_DIR=%DEVTLS_DIR%\Microsoft SDKs\Windows\10
set TOOL_CHAIN=DEVTLSxVC14
goto EnvSetting

:EnvDEVTLS_VC10
set TOOL_DIR=%DEVTLS_DIR%\MSVC10
set SDK_DIR=%DEVTLS_DIR%\Microsoft SDKs\Windows\v7.0A
set TOOL_CHAIN=DEVTLSxVC10
goto EnvSetting

:EnvDEVTLS_VC9
set TOOL_DIR=%DEVTLS_DIR%\MSVC9
set SDK_DIR=%DEVTLS_DIR%\Microsoft SDKs\Windows\v6.0A
set TOOL_CHAIN=DEVTLSxVC9
goto EnvSetting

:EnvVS2017
set TOOL_DIR=%ProgramFiles(x86)%\Microsoft Visual Studio
goto VS2017Setting

:EnvVS2015
set TOOL_DIR=Program Files\Microsoft Visual Studio 14.0
set SDK_DIR=Program Files\Windows Kits\8.1
goto EnvSetting

:EnvVS2015x86
set TOOL_DIR=Program Files (x86)\Microsoft Visual Studio 14.0
set SDK_DIR=Program Files (x86)\Windows Kits\8.1
goto EnvSetting

:EnvVS2013
set TOOL_DIR=Program Files\Microsoft Visual Studio 12.0
set SDK_DIR=Program Files\Windows Kits\8.1
goto EnvSetting

:EnvVS2013x86
set TOOL_DIR=Program Files (x86)\Microsoft Visual Studio 12.0
set SDK_DIR=Program Files (x86)\Windows Kits\8.1
goto EnvSetting

:EnvVS2012
set TOOL_DIR=Program Files\Microsoft Visual Studio 11.0
set SDK_DIR=Program Files\Windows Kits\8.0
goto EnvSetting

:EnvVS2012x86
set TOOL_DIR=Program Files (x86)\Microsoft Visual Studio 11.0
set SDK_DIR=Program Files (x86)\Windows Kits\8.0
goto EnvSetting

:EnvVS2010
set TOOL_DIR=Program Files\Microsoft Visual Studio 10.0
set SDK_DIR=Program Files\Microsoft SDKs\Windows\v7.0A
goto EnvSetting

:EnvVS2010x86
set TOOL_DIR=Program Files (x86)\Microsoft Visual Studio 10.0
set SDK_DIR=Program Files (x86)\Microsoft SDKs\Windows\v7.0A
goto EnvSetting

:EnvVS2008
set TOOL_DIR=Program Files\Microsoft Visual Studio 9.0
set SDK_DIR=Program Files\Microsoft SDKs\Windows\v6.0A
goto EnvSetting

:EnvVS2008x86
set TOOL_DIR=Program Files (x86)\Microsoft Visual Studio 9.0
set SDK_DIR=Program Files\Microsoft SDKs\Windows\v6.0A
goto EnvSetting

:EnvDEVTLS_ARMGCC
set ARCH=ARM
set TOOL_DIR=%DEVTLS_DIR%\ARM\CodeSourceryLite
set TOOL_CHAIN=ARMGCC
goto EnvSetting

:EnvDEVTLS_ARM64GCC
set ARCH=AARCH64
set TOOL_DIR=%DEVTLS_DIR%\ARM\GccAARCH64
set TOOL_CHAIN=ARM64GCC
goto EnvSetting

:EnvDEVTLS_YAGARTO
set ARCH=ARM
set TOOL_DIR=%DEVTLS_DIR%\ARM\Yagarto
set TOOL_CHAIN=YAGARTO
goto EnvSetting

:EnvARMGCC
set ARCH=ARM
set TOOL_DIR=Program Files\CodeSourcery\Sourcery G++ Lite
goto EnvSetting

:EnvYAGARTO
set ARCH=ARM
set TOOL_DIR=Program Files\Yagarto
goto EnvSetting

:EnvWINWDK
if not DEFINED TOOL_DIR set TOOL_DIR=WinDDK/7600.16385.1
goto EnvSetting

:DetectToolChainPath
if not DEFINED TOOL_CHAIN_DRV (
  if exist "%1\%TOOL_DIR%" set TOOL_CHAIN_DRV=%1
)
if "%ARCH%"=="ARM" goto :EOF
if "%ARCH%"=="AARCH64" goto :EOF
if DEFINED TOOL_CHAIN_DRV (
  if exist "%TOOL_CHAIN_DRV%\%SDK_DIR%" set "SDK_DIR=%TOOL_CHAIN_DRV%\%SDK_DIR%"
)
goto :EOF


:set_vsvars
for /f "usebackq tokens=1* delims=: " %%i in (`%*`) do (
  if /i "%%i"=="installationPath" (
    call "%%j\VC\Auxiliary\Build\vcvars32.bat"
    goto :EOF
  )
)
if exist "%TOOL_DIR%\2017\WDExpress\VC\Auxiliary\Build\vcvars32.bat" call "%TOOL_DIR%\2017\WDExpress\VC\Auxiliary\Build\vcvars32.bat"
goto :EOF

:VS2017Setting
if not exist "%TOOL_DIR%" (
  if "%TOOL_DIR%"=="%ProgramFiles(x86)%\Microsoft Visual Studio" (set TOOL_DIR=%ProgramFiles%\Microsoft Visual Studio)
  if not exist "%TOOL_DIR%" (
    echo !Cannot find "%TOOL_DIR%" in your computer
    echo !Incorrect tool chain setting for %TOOL_CHAIN%
    goto Error
  )
)
set "TOOL_CHAIN_PATH=%TOOL_DIR%"
call :set_vsvars "%TOOL_DIR%\Installer\vswhere.exe"

if not defined VS150COMNTOOLS (
  if exist "%TOOL_DIR%\Installer\vswhere.exe" (
    for /f "usebackq tokens=1* delims=: " %%i in (`"%TOOL_DIR%\Installer\vswhere.exe"`) do (
      if /i "%%i"=="installationPath" call "%%j\VC\Auxiliary\Build\vcvars32.bat"
    )
  ) else (
    goto SetWinDDK
  )
)

if defined VCToolsInstallDir (
  if not defined VS2017_PREFIX (
    set "VS2017_PREFIX=%VCToolsInstallDir%"
  )
)
if not defined WINSDK10_PREFIX (
  if defined WindowsSdkVerBinPath (
    set "WINSDK10_PREFIX=%WindowsSdkVerBinPath%"
  ) else if exist "%ProgramFiles(x86)%\Windows Kits\10\bin" (
    set "WINSDK10_PREFIX=%ProgramFiles(x86)%\Windows Kits\10\bin\"
  ) else if exist "%ProgramFiles%\Windows Kits\10\bin" (
    set "WINSDK10_PREFIX=%ProgramFiles%\Windows Kits\10\bin\"
  )
)

if defined WindowsSdkDir (
  set "SDK_DIR=%WindowsSdkDir%"
) else if exist "%ProgramFiles(x86)%\Windows Kits\10" (
  set "SDK_DIR=%ProgramFiles(x86)%\Windows Kits\10\"
) else if exist "%ProgramFiles%\Windows Kits\10" (
  set "SDK_DIR=%ProgramFiles%\Windows Kits\10\"
)

goto ToolChainSetting

:EnvSetting
if DEFINED TOOL_DRV call :DetectToolChainPath %TOOL_DRV%
call :DetectToolChainPath C:
call :DetectToolChainPath D:
call :DetectToolChainPath E:
call :DetectToolChainPath F:
call :DetectToolChainPath G:
call :DetectToolChainPath H:
if DEFINED TOOL_CHAIN_DRV goto ToolChainSetting

@REM Error message for incorrect setting of tool chain
echo !Cannot find "%TOOL_DIR%" in your computer
echo !Incorrect tool chain setting for %TOOL_CHAIN%
goto Err%TOOL_CHAIN%
:ErrDevTls
echo Please check DEVTLS settings or change TOOL_CHAIN to one of the following
goto Error

:ErrWinWdk
echo Please check whether Microsoft Windows WDK is correctly installed or change TOOL_CHAIN to one of the following
goto Error

:ErrVS2013
:ErrVS2013x86
echo Please check whether Microsoft Visual Studio 2013 is correctly installed or change TOOL_CHAIN to one of the following
goto Error

:ErrVS2015
:ErrVS2015x86
echo Please check whether Microsoft Visual Studio 2015 is correctly installed or change TOOL_CHAIN to one of the following
goto Error

:ErrVS2012
:ErrVS2012x86
echo Please check whether Microsoft Visual Studio 2012 is correctly installed or change TOOL_CHAIN to one of the following
goto Error

:ErrVS2010
:ErrVS2010x86
echo Please check whether Microsoft Visual Studio 2010 is correctly installed or change TOOL_CHAIN to one of the following
goto Error

:ErrVS2008
:ErrVS2008x86
echo Please check whether Microsoft Visual Studio 2008 is correctly installed or change TOOL_CHAIN to one of the following
goto Error

:ErrDEVTLS_ARMGCC
:ErrARMGCC
echo Please check whether Code Sourcery G++ for ARM is correctly installed or change TOOL_CHAIN to one of the following
goto Error

:ErrDEVTLS_YAGARTO
:ErrYAGARTO
echo Please check whether YAGARTO GCC for ARM is correctly installed or change TOOL_CHAIN to one of the following
goto Error


:ToolChainSetting
set TOOL_DRV=%TOOL_CHAIN_DRV%
if not DEFINED TOOL_CHAIN_PATH set TOOL_CHAIN_PATH=%TOOL_DRV%\%TOOL_DIR%
if not DEFINED ARCH set ARCH=IA32 X64
if not defined ASL_PATH set ASL_PATH=%WORKSPACE%\BaseTools\Bin\Win32
if not exist %ASL_PATH%\iasl.exe set ASL_PATH=%WORKSPACE%\BaseTools\Bin\Win32
if "%ARCH%"=="ARM" goto ArmToolSetting
if "%ARCH%"=="AARCH64" goto ArmToolSetting
if "%TOOL_CHAIN_PATH:~-1%"=="\" (set "TOOL_CHAIN_PATH=%TOOL_CHAIN_PATH:~,-1%")
if "%SDK_DIR:~-1%"=="\" (set "SDK_DIR=%SDK_DIR:~,-1%")
if DEFINED EFI_SOURCE_DIR set EFI_SOURCE=%WORKSPACE%\%EFI_SOURCE_DIR%
if "%TOOL_CHAIN%"=="WINWDK" goto WinWdkSetting
set VC_PATH=%TOOL_CHAIN_PATH%

if "%TOOL_CHAIN%"=="DEVTLSxVC14" goto ToolChainSettingDEVTLSxVC14
goto ToolChainSettingPhase2

:ToolChainSettingDEVTLSxVC14
  @REM
  @REM Refer C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\vcvars32.bat store
  @REM Refer C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\vcvarsqueryregistry.bat
  @REM
  set WindowsSdkDir=%SDK_DIR%\
  set WindowsSDKVersion=10.0.10586.0
  set WindowsSDKLibVersion=%WindowsSDKVersion%
  set UniversalCRTSdkDir=%SDK_DIR%\
  set UCRTVersion=10.0.10586.0
  set VCINSTALLDIR=%TOOL_CHAIN_PATH%\VC\

  @REM rc.exe
  set PATH=%WindowsSdkDir%bin\x86;%PATH%

  goto ToolChainSettingPhase2


:ToolChainSettingPhase2
set INCLUDE=%INCLUDE%;%VC_PATH%\VC\Include;%SDK_DIR%\Include;%SDK_DIR%\Include\um;%SDK_DIR%\Include\shared;
set LIB=%LIB%;%VC_PATH%\VC\Lib;%SDK_DIR%\Lib;%SDK_DIR%\Lib\win8\um\x86;%SDK_DIR%\Lib\winv6.3\um\x86;

if defined WindowsSDKLibVersion (
  set "INCLUDE=%INCLUDE%;%SDK_DIR%\Include\%WindowsSDKLibVersion%;%SDK_DIR%\Include\%WindowsSDKLibVersion%\um;%SDK_DIR%\Include\%WindowsSDKLibVersion%\shared;%SDK_DIR%\Include\%WindowsSDKLibVersion%\ucrt"
  set "LIB=%LIB%;%SDK_DIR%\lib\%WindowsSDKLibVersion%\ucrt\x86;%SDK_DIR%\lib\%WindowsSDKLibVersion%\um\x86"
)
if defined VCToolsInstallDir (
  set "INCLUDE=%INCLUDE%;%VCToolsInstallDir%include"
  set "LIB=%LIB%;%VCToolsInstallDir%lib\x86"
)
path=%TOOL_CHAIN_PATH%\VC\bin;%TOOL_CHAIN_PATH%\Common7\IDE;%SDK_DIR%\bin;%path%
if not defined NASM_PREFIX goto NasmSetup
goto tool_setup

:NasmSetup
@REM Setup NASM_PREFIX to BaseToools\Bin\Win32 if nasm.exe is exist
if exist %WORKSPACE%\BaseTools\Bin\Win32\nasm.exe set NASM_PREFIX=%WORKSPACE%\BaseTools\Bin\Win32\
if defined NASM_PREFIX goto tool_setup
@REM Setup NASM_PREFIX to TOOL_CHAIN_PAATH\NASM if nasm.exe is exist
if exist %TOOL_CHAIN_PATH%\NASM\nasm.exe set NASM_PREFIX=%TOOL_CHAIN_PATH%\NASM\
@echo ===============================================
@echo NASM_PREFIX not setup, NASM will not workable
@echo ===============================================
goto tool_setup

:ArmToolSetting
PATH=%WORKSPACE%\BaseTools\Bin\Win32\ARM;%PATH%;%TOOL_CHAIN_PATH%;%TOOL_CHAIN_PATH%\bin;%PATH%;
goto tool_setup

:WinWdkSetting
set INCLUDE=%VC_PATH%\VC\Include;%TOOL_DRV%\%TOOL_DIR%\inc;%TOOL_DRV%\%TOOL_DIR%\inc\api;%TOOL_DRV%\%TOOL_DIR%\inc\crt
set LIB=%TOOL_DRV%\%TOOL_DIR%\lib\wxp;%TOOL_DRV%\%TOOL_DIR%\lib\wxp\i386;%TOOL_DRV%\%TOOL_DIR%\lib\Crt\i386
path=%TOOL_DRV%\%TOOL_DIR%\bin;%TOOL_DRV%\%TOOL_DIR%\bin\x86;%TOOL_DRV%\%TOOL_DIR%\bin\x86\amd64;%TOOL_DRV%\%TOOL_DIR%\bin\x86\x86;%path%

:tool_setup
if defined NASM_PREFIX set PATH=%PATH%;%NASM_PREFIX%
call %WORKSPACE%\BaseTools\toolsetup.bat

:set_sal_path
@echo off
for %%i in (sal.h) do (
    set SAL_PATH=%%~$INCLUDE:i
)
set SAL_INC_PATH=%SAL_PATH:~0,-6%

nmake gen_release_target
:End