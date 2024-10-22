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

if not exist "%1\" (
  set SRC_BIN_PATH=%1
  set HEADER_FILE_FOLDER=%2
  set HEADER_FILE_DRIVER=%~d2
  set INPUTXML_NAME=%~n1_out.xml
) else (
  set HEADER_FILE_FOLDER=%1
  set SRC_BIN_PATH=%2
  set HEADER_FILE_DRIVER=%~d1
  set INPUTXML_NAME=%~n2_out.xml
)

echo Header file folder = %HEADER_FILE_FOLDER%
echo SBIOS binary = %SRC_BIN_PATH%

for /R %HEADER_FILE_FOLDER%  %%s in (*.h) do ( 
  set INPUT_HEADER_FILE=!INPUT_HEADER_FILE!;%%s
) 
set INPUT_HEADER_FILE=!INPUT_HEADER_FILE:~1!

echo ApcbV3Editor.bat -w %SRC_BIN_PATH% %SRC_BIN_PATH% "!INPUT_HEADER_FILE!" %INPUTXML_NAME%

call ApcbV3Editor.bat -w %SRC_BIN_PATH% %SRC_BIN_PATH% "!INPUT_HEADER_FILE!" %INPUTXML_NAME%

@echo Write complete, Press any key to exit.
@pause>nul
@echo on