@echo off
pushd %1%

@REM #
@REM # Create VfrList.txt which contains all of *.lst file from VfrCompiler
@REM #
if exist VfrList.txt (
  del /f /q VfrList.txt
)
if exist VfrFile (
  dir /b VfrFile > VfrFile.txt
  move /y VfrFile.txt VfrFile\VfrFile.txt
)

@REM #
@REM # Create a merged VFR file
@REM #
if exist TotalVfrFile.vfr del /f /q TotalVfrFile.vfr
if exist TotalVfrFileWithFileInfo.vfr del /f /q TotalVfrFileWithFileInfo.vfr

if exist VfrFile (
  for /f %%i in ('dir /b VfrFile\*.ii') do (
    type VfrFile\%%i >> TotalVfrFileWithFileInfo.vfr
  )
)

findstr /V /c:"#line " TotalVfrFileWithFileInfo.vfr > TotalVfrFile.vfr


popd