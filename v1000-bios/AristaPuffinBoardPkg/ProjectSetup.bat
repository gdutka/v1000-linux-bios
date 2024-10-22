@REM
@REM Project setup batch file
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

@echo off

REM ---------------------------------------------------------------------------------------------
REM Auto setting of WORKSPACE environment variable
REM ---------------------------------------------------------------------------------------------
pushd \ && set ROOT_DIR=%cd% && popd && pushd .
:SetWorkSpace
if "%cd%" == "%ROOT_DIR%" goto Next
cd ..
if not exist %cd%\BaseTools goto SetWorkSpace
set WORKSPACE=%cd%
:Next
set ROOT_DIR= && popd
%WORKSPACE%\BaseTools\Bin\Win32\GetProjectEnv WORKSPACE > NUL && for /f %%i in ('%WORKSPACE%\BaseTools\Bin\Win32\GetProjectEnv WORKSPACE') do set %%i

@REM ---------------------------------------------------------------------------------------------
@REM Support Module build
@REM Build command sample:
@REM                      build -q -p ..\Build\SkylakeMultiBoardPkg\Project.dsc -m InsydeModulePkg/Universal/MemoryTest/GenericMemoryTestDxe/GenericMemoryTestDxe.inf
@REM ---------------------------------------------------------------------------------------------
%WORKSPACE%\BaseTools\Bin\Win32\GetProjectEnv PROJECT_PKG > NUL && for /f %%i in ('%WORKSPACE%\BaseTools\Bin\Win32\GetProjectEnv PROJECT_PKG') do set %%i

REM ---------------------------------------------------------------------------------------------
REM Project dependent parameters
REM ---------------------------------------------------------------------------------------------
set TOOL_CHAIN=DEVTLS_VC10
set ASL_PATH=D:\DEVTLS\Asl\Acpi5.0
set ARCH=IA32 X64
set EFI_SOURCE_DIR=InsydeModulePkg
set BIOSNAME=AristaPuffin

call %WORKSPACE%\BaseTools\H2ORev50.bat
