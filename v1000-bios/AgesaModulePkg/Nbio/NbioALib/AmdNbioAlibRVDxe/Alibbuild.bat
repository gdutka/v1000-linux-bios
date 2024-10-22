REM #
REM #Alib build batch file
REM #c:\asl\asl.exe /Fo=.\alibv2.aml  alibv2.asl
REM #

REM # remove build folder
rmdir /S /Q BUILD

REM #
REM #set AGESA_ROOT=EFI_SOURCE\AGESA
REM #
set AGESA_ROOT=%CD%\..\..\..\..

set path=c:\ASL;%path%
nmake -f AlibMake.mak
