@REM
@REM  Batch file to generate build time report
@REM
@REM ******************************************************************************
@REM * Copyright (c) 2020, Insyde Software Corp. All Rights Reserved.
@REM *
@REM * You may not reproduce, distribute, publish, display, perform, modify, adapt,
@REM * transmit, broadcast, present, recite, release, license or otherwise exploit
@REM * any part of this publication in any form, by any means, without the prior
@REM * written permission of Insyde Software Corporation.
@REM *
@REM ******************************************************************************
@echo off

if /I "%1" == "PreBuildStartTime"  goto SetTime
if /I "%1" == "PreBuildEndTime"    goto SetTime
if /I "%1" == "BuildStartTime"     goto SetTime
if /I "%1" == "BuildEndTime"       goto SetTime
if /I "%1" == "PostBuildStartTime" goto SetTime
if /I "%1" == "PostBuildEndTime"   goto SetTime

goto GenerateBuildTime

:SetTime
REM
REM convert time from [H| ]H:[M| ]M:[S| ]S.ss to HH:MM:SS
REM ex: 8:23:45.33 to 08:23:45
REM
set CurrentTime=%time%
set Hour=%CurrentTime:~0,2%
if "%Hour:~0,1%" == " " set Hour=0%Hour:~1,1%
set Min=%CurrentTime:~3,2%
if "%Min:~0,1%" == " " set Min=0%Min:~1,1%
set Sec=%CurrentTime:~6,2%
if "%Sec:~0,1%" == " " set Sec=0%Sec:~1,1%
set CurrentTime=%Hour%:%Min%:%Sec%
REM
REM Record the build time information to H2OBuildTimeReport
REM
if "%1" == "PreBuildStartTime"     echo %1 : %CurrentTime%    >  %WORKSPACE%\Build\%PROJECT_PKG%\H2OBuildTimeReport
if not "%1" == "PreBuildStartTime" echo %1 : %CurrentTime%    >> %WORKSPACE%\Build\%PROJECT_PKG%\H2OBuildTimeReport
goto :eof

:GenerateBuildTime
REM
REM Get the build time information from H2OBuildTimeReport
REM
for /f "tokens=1,3 delims=. " %%i in (%WORKSPACE%\Build\%PROJECT_PKG%\H2OBuildTimeReport) do set %%i=%%j
REM
REM print build time information
REM
echo.
echo ==========================================================================
call:PrintBuildTime %PreBuildStartTime%  %PreBuildEndTime%  "Pre Build Time   "
call:PrintBuildTime %BuildStartTime%     %BuildEndTime%     "Build Time       "
call:PrintBuildTime %PostBuildStartTime% %PostBuildEndTime% "Post Build Time  "
echo ==========================================================================
call:PrintBuildTime %PreBuildStartTime%  %PostBuildEndTime% "Total Build Time "
goto :eof

:PrintBuildTime
set StartTime=%~1
set EndTime=%~2
REM
REM step 1 to calculate build time by seconds
REM
set /A StartTimeSec=(1%StartTime:~0,2%-100)*3600 + (1%StartTime:~3,2%-100)*60 + (1%StartTime:~6,2%-100)
set /A EndTimeSec=(1%EndTime:~0,2%-100)*3600 + (1%EndTime:~3,2%-100)*60 + (1%EndTime:~6,2%-100)
REM
REM Calculate the duration and adjust the duration if the time inbetween days
REM
set /A BuildDuration=%EndTimeSec%-%StartTimeSec%
if %EndTimeSec% LSS %StartTimeSec% set /A BuildDuration=%StartTimeSec%-%EndTimeSec%
REM
REM Calculate duration hours, minutes and seconds
REM
set /A BuildDurationH=%BuildDuration% / 3600
set /A BuildDurationM=(%BuildDuration% - %BuildDurationH%*3600) / 60
set /A BuildDurationS=(%BuildDuration% - %BuildDurationH%*3600 - %BuildDurationM%*60)
REM
REM Adjust format ex: 9 -> 09, 12 -> 12
REM
if %BuildDurationH% LSS 10 set BuildDurationH=0%BuildDurationH%
if %BuildDurationM% LSS 10 set BuildDurationM=0%BuildDurationM%
if %BuildDurationS% LSS 10 set BuildDurationS=0%BuildDurationS%
REM
REM Print the build time information
REM
echo %~3: %BuildDurationH%:%BuildDurationM%:%BuildDurationS% (From %StartTime% to %EndTime%)
goto :eof