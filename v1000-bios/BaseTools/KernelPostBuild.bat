@REM
@REM Kernel post-build batch file
@REM
@REM ******************************************************************************
@REM * Copyright (c) 2013 - 2020, Insyde Software Corp. All Rights Reserved.
@REM *
@REM * You may not reproduce, distribute, publish, display, perform, modify, adapt,
@REM * transmit, broadcast, present, recite, release, license or otherwise exploit
@REM * any part of this publication in any form, by any means, without the prior
@REM * written permission of Insyde Software Corporation.
@REM *
@REM ******************************************************************************

@echo off

@if exist %WORKSPACE%\Build\%PROJECT_PKG%\BuildProject.dsc copy %WORKSPACE%\Build\%PROJECT_PKG%\BuildProject.dsc %WORKSPACE%\Build\%PROJECT_PKG%\Project.dsc /y
@if exist %WORKSPACE%\Build\%PROJECT_PKG%\BuildProject.fdf copy %WORKSPACE%\Build\%PROJECT_PKG%\BuildProject.fdf %WORKSPACE%\Build\%PROJECT_PKG%\Project.fdf /y
@if exist %WORKSPACE%\InsydeModulePkg\Tools\GenUnsignedFv\GenUnsignedFv.exe (
  @FOR /f "tokens=3" %%a IN ('findstr /R "\<TARGET\>" %WORKSPACE%\Conf\target.txt') DO @(
    @copy %WORKSPACE%\InsydeModulePkg\Tools\GenUnsignedFv\GenUnsignedFv.exe %WORKSPACE%\Build\%PROJECT_PKG%\%%a_%TOOL_CHAIN%\IA32\GenUnsignedFv.exe /y
  )
)



@setlocal enabledelayedexpansion

@ REM
@ REM Generate ProjectSourceCodeAnalysis.csv file
@ REM

@for /f "tokens=3" %%a in ('findstr /R "\<TARGET\>" %WORKSPACE%\Conf\target.txt') do (
  findstr /C:"'SOURCE_CODE_ANALYSIS_BUILD': 'YES'" %WORKSPACE%\Build\%PROJECT_PKG%\%%a_%TOOL_CHAIN%\BuildOptions > nul
  if !ERRORLEVEL! == 0 (
    if not exist %WORKSPACE%\%PROJECT_PKG%\ProjectSourceCodeAnalysis.csv echo.> %WORKSPACE%\%PROJECT_PKG%\ProjectSourceCodeAnalysis.csv
    echo. > %WORKSPACE%\Build\%PROJECT_PKG%\%%a_%TOOL_CHAIN%\ScaBuildLog.txt
    for /f %%i in ('dir /s /b  %WORKSPACE%\Build\%PROJECT_PKG%\%%a_%TOOL_CHAIN%\*.analysis.txt') do @type %%i >> %WORKSPACE%\Build\%PROJECT_PKG%\%%a_%TOOL_CHAIN%\ScaBuildLog.txt
    SourceCodeAnalyzer --logfile %WORKSPACE%\Build\%PROJECT_PKG%\%%a_%TOOL_CHAIN%\ScaBuildLog.txt --inputfile %WORKSPACE%\%PROJECT_PKG%\ProjectSourceCodeAnalysis.csv --outputfile %WORKSPACE%\%PROJECT_PKG%\ProjectSourceCodeAnalysis.csv
    echo.
    echo SCA Analyzer result in %WORKSPACE%\%PROJECT_PKG%\ProjectSourceCodeAnalysis.csv
    echo.
  )
)

@REM
@REM Return build fail if there is a new issue in ProjectSourceCodeAnalysis.csv
@REM don't merge below code to previous code, becuase the ProjectSourceCodeAnalysis.csv file still be opened.
@REM


if "%SCA_BUILD_NEW_ISSUE_ASSERT%" == "" set SCA_BUILD_NEW_ISSUE_ASSERT= 0
@set SCA_BUILD_FAIL_MSG=There is a new issue in ProjectSourceCodeAnalysis.csv !!!

@for /f "tokens=3" %%a in ('findstr /R "\<TARGET\>" %WORKSPACE%\Conf\target.txt') do (
  findstr /C:"'SOURCE_CODE_ANALYSIS_BUILD': 'YES'" %WORKSPACE%\Build\%PROJECT_PKG%\%%a_%TOOL_CHAIN%\BuildOptions > nul
  if !ERRORLEVEL! == 0 (
    if "%SCA_BUILD_NEW_ISSUE_ASSERT%" == "1" (
      findstr ",New," %WORKSPACE%\%PROJECT_PKG%\ProjectSourceCodeAnalysis.csv > nul
      if !ERRORLEVEL! == 0 (
        echo.
        echo !SCA_BUILD_FAIL_MSG!
        echo.
        exit 1
      )
    )
  )
)

@setlocal disabledelayedexpansion

