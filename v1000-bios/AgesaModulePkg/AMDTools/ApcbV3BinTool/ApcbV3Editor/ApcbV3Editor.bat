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
pushd %cd%

set origin=%cd%
set drive=%~d0
set root=%~dp0
IF "%PspKitPath%" == "" (
  SET PspKitPath=%root%..\NewPspKit  
  call :GetFullPath !PspKitPath!
  set PspKitPath=!paramFullPathName!


  IF NOT EXIST !PspKitPath!\BuildPspImage.bat (
     SET PspKitPath=%root%..\..\NewPspKit
     call :GetFullPath !PspKitPath!
     set PspKitPath=!paramFullPathName!
  )
  

  ::CHECK_PATH
  IF NOT EXIST !PspKitPath!\BuildPspImage.bat (
    @echo !!!PspKitPath system variable is NOT set correctly!!
    goto ERROR
  )
)


SET PspToolPath=%PspKitPath%\PspDirectoryTool\

set APCB_BIN=NULL
set LOOP_END=FALSE
if %1% == -r goto READ
if %1% == -w goto WRITE
goto END

:GetFullPath
set paramFullPathName=%~f1
EXIT /B 0

:READ
set SOC_NAME=%2
set INPUT_BIN=%3
set HEADER_FILE=%4
set XML_NAME=%5
del %PspKitPath%\input.rom
copy %INPUT_BIN% %PspKitPath%\input.rom
%PspToolPath%BuildPspDirectory.exe -o %PspToolPath%output dp -p %SOC_NAME% -b -d -x %PspKitPath%\input.rom
%drive%
cd %PspToolPath%output

for /F "tokens=2 delims=<>" %%a in ('findstr "Type=\"0x68\"" PspDirInfo.xml') do (
  set str=%%a
  set instanceID="0x8"
  set fileflag=false
  set instanceflag=false
  call :XML_LINE_PARSE
)

for /F "tokens=2 delims=<>" %%a in ('findstr "Type=\"0x60\"" PspDirInfo.xml') do (
  set str=%%a
  set instanceID="0x1"
  set fileflag=false
  set instanceflag=false
  call :XML_LINE_PARSE
)

for /F "tokens=2 delims=<>" %%a in ('findstr "Type=\"0x60\"" PspDirInfo.xml') do (
  set str=%%a
  set instanceID="0x0"
  set fileflag=false
  set instanceflag=false
  call :XML_LINE_PARSE
)

for /F "tokens=2 delims=<>" %%a in ('findstr "Type=\"0x68\"" PspDirInfo.xml') do (
  set str=%%a
  set instanceID="0x0"
  set fileflag=false
  set instanceflag=false
  call :XML_LINE_PARSE
)

del %PspKitPath%\input.rom
%drive%
cd %root%

echo ApcbV3Editor.exe -r "!APCB_BIN!" %HEADER_FILE% %XML_NAME%
ApcbV3Editor.exe -r "!APCB_BIN!" %HEADER_FILE% %XML_NAME%
goto END

:WRITE
set SRC_BIN=%2
set OUT_BIN=%3
set HEADER_FILE=%4
set XML_NAME=%5
%drive%
cd %PspToolPath%output

for /F "tokens=2 delims=<>" %%a in ('findstr "Type=\"0x60\"" PspDirInfo.xml') do (
  set str=%%a
  set instanceID=""
  set fileflag=false
  set instanceflag=false
  call :XML_LINE_PARSE
)

for /F "tokens=2 delims=<>" %%a in ('findstr "Type=\"0x68\"" PspDirInfo.xml') do (
  set str=%%a
  set instanceID=""
  set fileflag=false
  set instanceflag=false
  call :XML_LINE_PARSE
)

cd %root%

echo ApcbV3Editor.exe -w "!APCB_BIN!" %HEADER_FILE% %XML_NAME%
ApcbV3Editor.exe -w "!APCB_BIN!" %HEADER_FILE% %XML_NAME%
copy %SRC_BIN% %PspKitPath%\input.rom
%drive%
cd %PspKitPath%
call %PspKitPath%\BuildPspImage.bat "input.rom" "NotFound" "%PspToolPath%output\PspDirectory.xml" output.rom NOSIGN

popd
copy %PspKitPath%\output.rom %OUT_BIN%
del %PspKitPath%\input.rom
del %PspKitPath%\output.rom
goto END

:END
popd
@echo on
@goto EOF

:XML_LINE_PARSE
for /F "tokens=1* delims==" %%b in ("!str!") do (
  set name=%%b
  set str=%%c
  if !instanceID! == "" (
    set instanceflag=true
  ) else (
    if "!name:~-8!" == "Instance" (
      for /F "tokens=1" %%x in ("%%c") do (
        set instance=%%x
        set instance=!instance:~1,-1!
        if "!instance!" == !instanceID! (
          set instanceflag=true
        )
      )
    )
  )

  if "!name:~-4!" == "File" (
    for /F "tokens=1" %%x in ("%%c") do (
      set file=%%x
      set file=!file:~1,-1!
      if "!file:~0,9!" == "BiosDirL2" (
        set fileflag=true
      )
    )
  )
  if !fileflag!==true if !instanceflag!==true (
    echo Input binary !file! found.
    if "!APCB_BIN!" == "NULL" (
      set APCB_BIN=%PspToolPath%output\!file!
      set LOOP_END=TRUE
    ) else (
      set APCB_BIN=!APCB_BIN!;%PspToolPath%output\!file!
      set LOOP_END=TRUE
    )
  )
)
if "!str!" == "" (
  goto EOF
) else (
  if "!LOOP_END!" == "FALSE" (
    CALL :XML_LINE_PARSE
    goto EOF
  ) else (
    set LOOP_END=FALSE
    goto EOF
  )
)

:ERROR
@echo **** Fail ****
@exit /B 2
GOTO EOF

:EOF


