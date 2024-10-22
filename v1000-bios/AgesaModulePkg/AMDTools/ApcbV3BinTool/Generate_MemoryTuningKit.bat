:: Copyright 2018 by AMD Inc.  All rights reserved.
::
:: This document contains proprietary, confidential information that
:: may be used, copied and/or disclosed only as authorized by a
:: valid licensing agreement with AMD Inc. This copyright
:: notice must be retained on all authorized copies.
::
:: This code is provided "as is".  AMD Inc. makes, and
:: the end user receives, no warranties or conditions, express,
:: implied, statutory or otherwise, and ATI Technologies Inc.
:: specifically disclaims any implied warranties of merchantability,
:: non-infringement, or fitness for a particular purpose.
@echo off
setlocal EnableDelayedExpansion

set file=config.ini
set area=[GUI Parameters]
set PSPKIT_FOLDER_PATH=
set APCB_HEADER_FILE_FOLDER_PATH=

set currarea=
for /f "usebackq delims=" %%a in ("!file!") do (
    set ln=%%a
    if "x!ln:~0,1!"=="x[" (
        set currarea=!ln!
    ) else (
        for /f "tokens=1,2 delims==" %%b in ("!ln!") do (
            set currkey=%%b
            set currval=%%c
            if "x!area!"=="x!currarea!" if "xPSPKIT_FOLDER_PATH"=="x!currkey!" (
            	set PSPKIT_FOLDER_PATH=!currval!
                echo PSPKIT_FOLDER_PATH=!currval!
            )
            if "x!area!"=="x!currarea!" if "xAPCB_HEADER_FILE_FOLDER_PATH"=="x!currkey!" (
            	set APCB_HEADER_FILE_FOLDER_PATH=!currval!
                echo APCB_HEADER_FILE_FOLDER_PATH=!currval!
            )
        )
    )
)

cd %~dp0
mkdir MemoryTuningKit
xcopy ApcbV3Editor\* MemoryTuningKit\ApcbV3Editor\ /s /y 
copy .\ApcbV3BinTool.exe .\MemoryTuningKit\
copy .\config_MemoryTuningKit.ini .\MemoryTuningKit\config.ini 



for %%a in (!PSPKIT_FOLDER_PATH!) do set FOLDER_NAME=%%~nxa


echo FOLDER_NAME=%FOLDER_NAME%
echo %PSPKIT_FOLDER_PATH% MemoryTuningKit\%FOLDER_NAME% /s /y /i
xcopy %PSPKIT_FOLDER_PATH% MemoryTuningKit\%FOLDER_NAME% /s /y /i

for %%a in (!APCB_HEADER_FILE_FOLDER_PATH!) do set FOLDER_NAME=%%~nxa
echo FOLDER_NAME=%FOLDER_NAME%
echo %APCB_HEADER_FILE_FOLDER_PATH% MemoryTuningKit\%FOLDER_NAME% /s /y /i
xcopy %APCB_HEADER_FILE_FOLDER_PATH% MemoryTuningKit\%FOLDER_NAME% /s /y /i


@echo Press any key to exit.

@echo on