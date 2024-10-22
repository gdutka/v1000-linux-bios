:: *****************************************************************************
:: *
:: * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
:: *
:: ******************************************************************************

@echo off
::Flag to control use python script or executable file generate from python
::TRUE:   Script file
::FALSE:  Executable file
IF "%USE_PYTHON_SCRIPT%" == "" (
  @set USE_PYTHON_SCRIPT=TRUE
)

@set ABL_BIN=%1
@set APXB_PATH=%2
@set APCB_BIN=%3
@set PSP_BL_BIN=%4

if %USE_PYTHON_SCRIPT% == "TRUE" (
  if exist "%PYTHON_PATH%\python.exe" (
    @set CHECKABLVERSION=%PYTHON_PATH%\python.exe CheckAblVersion.py
  ) else if exist "%PYTHON_PATH%\bin\python.exe" (
    @set CHECKABLVERSION=%PYTHON_PATH%\bin\python.exe CheckAblVersion.py
  ) else (
    @echo !!!PYTHON_PATH system variable is NOT set correctly!!
    goto error
  )
) ELSE (
  @set CHECKABLVERSION=CheckAblVersion.exe
)

@echo %CHECKABLVERSION% --abl_bin=%ABL_BIN% --apxb_path=%APXB_PATH% --apcb_bin=%APCB_BIN% --psp_bl_bin=%PSP_BL_BIN%
%CHECKABLVERSION% --abl_bin=%ABL_BIN% --apxb_path=%APXB_PATH% --apcb_bin=%APCB_BIN% --psp_bl_bin=%PSP_BL_BIN%
if %ERRORLEVEL% neq 0 goto error

goto end

:error
@echo **** ABL Version Check Fail ****
exit
goto end

:end
