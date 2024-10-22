set FD_NAME=BILBY
:: Default Value
if "%KEY_MODE%"=="" (
  set KEY_MODE=NOSIGN
)


@echo KEY_MODE %KEY_MODE%
set PSP_PLATFORM_PATH=%WORKSPACE%\%PROJECT_PKG%\PlatformConfig

if "%KEY_MODE%"=="PK" (
  set PSP_FW_PATH=%WORKSPACE%\AgesaModulePkg\Firmwares\RV
  set PSP_CONFIG_FILE_PATH=%PSP_PLATFORM_PATH%
)

if "%KEY_MODE%"=="NOSIGN" (
  set PSP_FW_PATH=%WORKSPACE%\AgesaModulePkg\Firmwares\RV
  set PSP_CONFIG_FILE_PATH=%PSP_PLATFORM_PATH%
)


set PSPKIT_PATH=%WORKSPACE%\AgesaModulePkg\AMDTools\NewPspKit
set PSP_TEMP_PATH=%WORKSPACE%\Build\%PROJECT_PKG%\NewPspKit

set APCB_BOARD_PKG_TEMP_PATH=%WORKSPACE%\Build\%PROJECT_PKG%\ApcbToolV2\External\
set CUSTOM_APCB_PATH=%APCB_BOARD_PKG_TEMP_PATH%\Release\
IF NOT EXIST %CUSTOM_APCB_PATH%\APCB_RV_D4.bin set CUSTOM_APCB_PATH=

IF "%SPCLIENT_PATH%" == "" (
  set SPCLIENT_PATH=%WORKSPACE%\%PROJECT_PKG%\Tools\SPClient
)
set OPENSSL_PATH=%WORKSPACE%\%PROJECT_PKG%\Tools\OpenSSL-Win32


@for /f "tokens=3" %%a in ('find "TARGET =" %WORKSPACE%\Conf\target.txt') do @set TARGET=%%a
@for /f "tokens=3" %%a in ('find "TOOL_CHAIN_TAG" %WORKSPACE%\Conf\target.txt') do @set TOOL_CHAIN_TAG=%%a
set BIOS_FV_PATH=%WORKSPACE%\Build\%PROJECT_PKG%\%TARGET%_%TOOL_CHAIN_TAG%\FV


  if exist %PSP_TEMP_PATH% (
    rd %PSP_TEMP_PATH% /S /Q
  )
  echo D | xcopy %PSPKIT_PATH%\*.*                    %PSP_TEMP_PATH%\. /S /Y
  copy %BIOS_FV_PATH%\%FD_NAME%.fd                    %PSP_TEMP_PATH%\. /Y
  set PSPKIT_PATH=%PSP_TEMP_PATH%
  set SPCLIENT_PATH=%SPCLIENT_PATH%
  set SP_FUNCTION=Sign Raven BIOS

  set RTM_FILE=RECOVERYFV.Fv
  copy %BIOS_FV_PATH%\%RTM_FILE%                      %PSP_TEMP_PATH%\. /Y
  set BIOS_IMAGE_CONFIG_FILE=%PSP_CONFIG_FILE_PATH%\BIOSImageDirectoryPCORV.xml
if "%KEY_MODE%"=="NOSIGN" (
  REM remove the line that conatins RTMSignature string for NOSIGN mode
  @findstr /v "RTMSignature" %BIOS_IMAGE_CONFIG_FILE% > %PSP_CONFIG_FILE_PATH%\BIOSImageDirectory-NOSIGN.xml
  set BIOS_IMAGE_CONFIG_FILE=%PSP_CONFIG_FILE_PATH%\BIOSImageDirectory-NOSIGN.xml
)
  set OUTPUT_BIOS=%BIOSNAME%.FD
  call %PSP_TEMP_PATH%\BuildPspImage.bat %FD_NAME%.fd %RTM_FILE% %BIOS_IMAGE_CONFIG_FILE% %OUTPUT_BIOS% %KEY_MODE%
  if %ERRORLEVEL% NEQ 0 goto ERR_END
@REM  copy %PSP_TEMP_PATH%\%OUTPUT_BIOS% %WORKSPACE%\ /Y /B
  copy %PSP_TEMP_PATH%\%OUTPUT_BIOS% %WORKSPACE%\%PROJECT_PKG%\BIOS /Y /B
  ::Copy back the updated binary for further operation
  copy %PSP_TEMP_PATH%\%OUTPUT_BIOS% %BIOS_FV_PATH%\%FD_NAME%.fd /Y /B
  @echo Final BIOS @ %WORKSPACE%\%PROJECT_PKG%\BIOS\%OUTPUT_BIOS%

goto END

:ERR_END
  @exit /B 2

:END
