#!/bin/bash
FV_OUTPUT_PATH=$WORKSPACE/Build/$PROJECT_PKG/${TARGET}_${TOOL_CHAIN_TAG}/FV
PEI_FD=$FV_OUTPUT_PATH/BILBYPEI.fd
PEI_COMPRESS_FD=$FV_OUTPUT_PATH/BILBYPEI_COMPRESS.fd
PEI_COMPRESS_SIZE=C0000
COMPRESS_TOOL_PATH=$WORKSPACE/AgesaModulePkg/AMDTools/CompressBios

if [ $RAVEN_16M = "NO" ]; then
echo ###Compress PEI FV ######
echo perl $COMPRESS_TOOL_PATH/CompressBios.pl  $PEI_FD $PEI_COMPRESS_FD $PEI_COMPRESS_SIZE
perl $COMPRESS_TOOL_PATH/CompressBios.pl $PEI_FD $PEI_COMPRESS_FD $PEI_COMPRESS_SIZE
PEI_FD=$PEI_COMPRESS_FD
fi

echo $PEI_FD

cat $FV_OUTPUT_PATH/BILBY.fd $PEI_FD > $FV_OUTPUT_PATH/BILBY_temp.fd
mv $FV_OUTPUT_PATH/BILBY_temp.fd $FV_OUTPUT_PATH/BILBY.fd

FD_NAME=BILBY
#Default Value
if [ -z $KEY_MODE ]; then
  KEY_MODE=NOSIGN
fi

echo KEY_MODE $KEY_MODE
PSP_PLATFORM_PATH=$WORKSPACE/$PROJECT_PKG/PlatformConfig

if [ $KEY_MODE = "PK" ]; then
  PSP_FW_PATH=$WORKSPACE/AgesaModulePkg/Firmwares/RV
  PSP_CONFIG_FILE_PATH=$PSP_PLATFORM_PATH
fi

if [ $KEY_MODE = "NOSIGN" ]; then
  PSP_FW_PATH=$WORKSPACE/AgesaModulePkg/Firmwares/RV
  PSP_CONFIG_FILE_PATH=$PSP_PLATFORM_PATH
fi

PSPKIT_PATH=$WORKSPACE/AgesaModulePkg/AMDTools/NewPspKit
PSP_TEMP_PATH=$WORKSPACE/Build/$PROJECT_PKG/NewPspKit

APCB_BOARD_PKG_TEMP_PATH=$WORKSPACE/Build/$PROJECT_PKG/ApcbToolV2/External
CUSTOM_APCB_PATH=$APCB_BOARD_PKG_TEMP_PATH/Release
if [ ! -f $CUSTOM_APCB_PATH/APCB_RV_D4.bin ]; then
  CUSTOM_APCB_PATH=
fi

if [ -z $SPCLIENT_PATH ]; then
  SPCLIENT_PATH=$WORKSPACE/$PROJECT_PKG/Tools/SPClient
fi
#set OPENSSL_PATH=%WORKSPACE%\%PROJECT_PKG%\Tools\OpenSSL-Win32

BIOS_FV_PATH=$FV_OUTPUT_PATH

  if [ -d $PSP_TEMP_PATH ]; then
    rm -rf $PSP_TEMP_PATH
  fi
  mkdir -p $PSP_TEMP_PATH
  cp -R $PSPKIT_PATH/*                                $PSP_TEMP_PATH
  cp $BIOS_FV_PATH/$FD_NAME.fd                        $PSP_TEMP_PATH
  PSPKIT_PATH=$PSP_TEMP_PATH
  SPCLIENT_PATH=$SPCLIENT_PATH
  SP_FUNCTION='Sign Raven BIOS'

  RTM_FILE=RECOVERYFV.Fv
  cp $BIOS_FV_PATH/$RTM_FILE                          $PSP_TEMP_PATH
  chmod -R 777 $PSP_TEMP_PATH
  BIOS_IMAGE_CONFIG_FILE=$PSP_CONFIG_FILE_PATH/BIOSImageDirectory.xml
if [ $KEY_MODE = "NOSIGN" ]; then
#  REM remove the line that conatins RTMSignature string for NOSIGN mode
#  @findstr /v "RTMSignature" %BIOS_IMAGE_CONFIG_FILE% > %PSP_CONFIG_FILE_PATH%\BIOSImageDirectory-NOSIGN.xml
  BIOS_IMAGE_CONFIG_FILE=$PSP_CONFIG_FILE_PATH/BIOSImageDirectory-NOSIGN.xml
fi
  OUTPUT_BIOS=${BIOSNAME}.fd

  export PSPKIT_PATH
  export PSP_FW_PATH
  export CUSTOM_APCB_PATH
  $PSP_TEMP_PATH/BuildPspImage.sh $FD_NAME.fd $RTM_FILE $BIOS_IMAGE_CONFIG_FILE $OUTPUT_BIOS $KEY_MODE
#  if %ERRORLEVEL% NEQ 0 goto ERR_END
#@REM  copy %PSP_TEMP_PATH%\%OUTPUT_BIOS% %WORKSPACE%\ /Y /B
  cp $PSP_TEMP_PATH/$OUTPUT_BIOS $WORKSPACE/$PROJECT_PKG/BIOS
#  ::Copy back the updated binary for further operation
  cp $PSP_TEMP_PATH/$OUTPUT_BIOS $BIOS_FV_PATH/$FD_NAME.fd
#  @echo Final BIOS @ %WORKSPACE%\%PROJECT_PKG%\BIOS\%OUTPUT_BIOS%
