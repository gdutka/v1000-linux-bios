#!/bin/bash

FD_NAME="BILBY"

# Default Value
if [ -z "$KEY_MODE" ]; then
  KEY_MODE="NOSIGN"
fi

echo "KEY_MODE $KEY_MODE"

PSP_PLATFORM_PATH="$WORKSPACE/$PROJECT_PKG/PlatformConfig"

if [ "$KEY_MODE" == "PK" ]; then
  PSP_FW_PATH="$WORKSPACE/AgesaModulePkg/Firmwares/RV"
  PSP_CONFIG_FILE_PATH="$PSP_PLATFORM_PATH"
fi

if [ "$KEY_MODE" == "NOSIGN" ]; then
  PSP_FW_PATH="$WORKSPACE/AgesaModulePkg/Firmwares/RV"
  PSP_CONFIG_FILE_PATH="$PSP_PLATFORM_PATH"
fi

PSPKIT_PATH="$WORKSPACE/AgesaModulePkg/AMDTools/NewPspKit"
PSP_TEMP_PATH="$WORKSPACE/Build/$PROJECT_PKG/NewPspKit"

APCB_BOARD_PKG_TEMP_PATH="$WORKSPACE/Build/$PROJECT_PKG/ApcbToolV2/External/"
CUSTOM_APCB_PATH="$APCB_BOARD_PKG_TEMP_PATH/Release/"

if [ ! -e "$CUSTOM_APCB_PATH/APCB_RV_D4.bin" ]; then
  CUSTOM_APCB_PATH=""
fi

if [ -z "$SPCLIENT_PATH" ]; then
  SPCLIENT_PATH="$WORKSPACE/$PROJECT_PKG/Tools/SPClient"
fi

OPENSSL_PATH="$WORKSPACE/$PROJECT_PKG/Tools/OpenSSL-Win32"

TARGET=$(awk -F" " '/TARGET =/ {print $3}' "$WORKSPACE/Conf/target.txt")
TOOL_CHAIN_TAG=$(awk -F" " '/TOOL_CHAIN_TAG/ {print $3}' "$WORKSPACE/Conf/target.txt")
BIOS_FV_PATH="$WORKSPACE/Build/$PROJECT_PKG/$TARGET"_"$TOOL_CHAIN_TAG/FV"

if [ -d "$PSP_TEMP_PATH" ]; then
  rm -rf "$PSP_TEMP_PATH"
fi

rsync -av "$PSPKIT_PATH/" "$PSP_TEMP_PATH/"
cp "$BIOS_FV_PATH/$FD_NAME.fd" "$PSP_TEMP_PATH/"

PSPKIT_PATH="$PSP_TEMP_PATH"
SPCLIENT_PATH="$SPCLIENT_PATH"
SP_FUNCTION="Sign Raven BIOS"

RTM_FILE="RECOVERYFV.Fv"
cp "$BIOS_FV_PATH/$RTM_FILE" "$PSP_TEMP_PATH/"

BIOS_IMAGE_CONFIG_FILE="$PSP_CONFIG_FILE_PATH/BIOSImageDirectoryRV1PCO-Backup.xml"

if [ "$KEY_MODE" == "NOSIGN" ]; then
  # Remove the line that contains RTMSignature string for NOSIGN mode
  grep -v "RTMSignature" "$BIOS_IMAGE_CONFIG_FILE" > "$PSP_CONFIG_FILE_PATH/BIOSImageDirectory-NOSIGN.xml"
  BIOS_IMAGE_CONFIG_FILE="$PSP_CONFIG_FILE_PATH/BIOSImageDirectory-NOSIGN.xml"
fi

OUTPUT_BIOS="$BIOSNAME.fd"
export PSPKIT_PATH
export PSP_FW_PATH
export CUSTOM_APCB_PATH
"$PSP_TEMP_PATH/BuildPspImage.sh" "$FD_NAME.fd" "$RTM_FILE" "$BIOS_IMAGE_CONFIG_FILE" "$OUTPUT_BIOS" "$KEY_MODE"

if [ $? -ne 0 ]; then
  echo "Error occurred during PSP image build."
  exit 2
fi

cp "$PSP_TEMP_PATH/$OUTPUT_BIOS" "$WORKSPACE/$PROJECT_PKG/BIOS"
cp "$PSP_TEMP_PATH/$OUTPUT_BIOS" "$BIOS_FV_PATH/$FD_NAME.fd"

echo "Final BIOS @ $WORKSPACE/$PROJECT_PKG/BIOS/$OUTPUT_BIOS"

exit 0
