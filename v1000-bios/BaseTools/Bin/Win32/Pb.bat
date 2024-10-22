@echo off
@REM
@REM Project Build
@REM
@REM ******************************************************************************
@REM * Copyright (c) 2017, Insyde Software Corp. All Rights Reserved.
@REM *
@REM * You may not reproduce, distribute, publish, display, perform, modify, adapt,
@REM * transmit, broadcast, present, recite, release, license or otherwise exploit
@REM * any part of this publication in any form, by any means, without the prior
@REM * written permission of Insyde Software Corporation.
@REM *
@REM ******************************************************************************
@set GEN_PROJECT_CFG=GenProjectCfg.exe
@REM set GEN_PROJECT_CFG=python D:\Efi\Tools_5.0\BaseTools\Source\GenProjectCfgPython\GenProjectCfg.py
@set GEN_PROJECT_CFG_ARCHS=--cfg %WORKSPACE%\%PROJECT_PKG%\Project.cfg --bat %WORKSPACE%/Build/%PROJECT_PKG% --Dynamic-to-DynamicEx
@set GEN_PROJECT_ARCHS=--gen %1
@if "%WORKSPACE%"=="" goto workspace_error
@if "%PROJECT_PKG%"=="" goto setup_project_pkg
@if "%1"=="list" goto LIST_CFG
@if "%1"=="detail" goto DETAIL_CFG
@if "%1"=="help" goto GEN_PROJECT_CFG_HELP
:PARSING_CFG
@REM ###############
@REM Parsing CFG file and output multiboard makefile to Build output folder
@REM ###############
@echo %GEN_PROJECT_CFG% %GEN_PROJECT_CFG_ARCHS% %GEN_PROJECT_ARCHS% 
@%GEN_PROJECT_CFG% %GEN_PROJECT_CFG_ARCHS% %GEN_PROJECT_ARCHS%
@echo ERRORLEVEL %ERRORLEVEL%
@if not %ERRORLEVEL% == 0 goto GEN_PROJECT_CFG_HELP
@REM ###############
@REM Make target
@REM ###############
@echo nmake -f %WORKSPACE%\Build\%PROJECT_PKG%\MultiboardMakefile %1
@nmake -f %WORKSPACE%\Build\%PROJECT_PKG%\MultiboardMakefile %1
@REM @echo Make Status %ERRORLEVEL%
@echo on
@goto :eof

:DETAIL_CFG
:LIST_CFG
@set GEN_PROJECT_ARCHS=--list %2
:DISPLAY_CFG
@echo %GEN_PROJECT_CFG% %GEN_PROJECT_CFG_ARCHS% %GEN_PROJECT_ARCHS% 
@%GEN_PROJECT_CFG% %GEN_PROJECT_CFG_ARCHS% %GEN_PROJECT_ARCHS%
@echo on
@goto :eof

:setup_project_pkg
@for /f %%i in ('GetProjectEnv PROJECT_PKG') do echo %%i
@for /f %%i in ('GetProjectEnv PROJECT_PKG') do set %%i
@if not "%PROJECT_PKG%" == "" goto PARSING_CFG
@echo PROJECT_PKG is not define
@echo on
@goto :eof

:workspace_error
@echo WORKSPACE is not define
@echo on
@goto :eof

:GEN_PROJECT_CFG_HELP
@REM %GEN_PROJECT_CFG% --help
@ECHO.
@ECHO Usage: pb.bat/pb.sh [options]
@ECHO.
@ECHO Options:
@ECHO   all
@ECHO     Build all configurations specified in Project.cfg.
@ECHO     The build environment will be cleaned between each configuration, excpet for the output flash device images.
@ECHO   clean
@ECHO     Delete all build files associated with all of configurations.
@ECHO   list
@ECHO     List all configurations specified in Project.cfg, one line per configutation.
@ECHO   detail [Configuration]
@ECHO     Provide help either for this script file or else display more information about a specific configuration.
@ECHO     The configuration is specified either using a number representing the configuration instance in project.cfg,
@ECHO     or using an identifier that matches the configuration name in Project.cfg.
goto :eof