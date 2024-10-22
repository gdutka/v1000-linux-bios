 # *****************************************************************************
 # *
 # * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 # *
 # *
 # ***************************************************************************

#!/bin/bash
# BuildPspImage InputBiosFile rtmfile PspDirectoryCfg OutputBiosFile KeyMode

#HELP
function HELP {
  echo "Embed PSP contents to Specific BIOS image"
  echo ""
  echo "BuildPspImage.bat rtmfile PspDirectoryCfg OutputBiosFile"
  echo "  Positional parameters:"
  echo "    rtmfile           The Root trunk module of BIOS, commonly it is the"
  echo "                      SEC,PEI combined FV image"
  echo "    PspDirectoryCfg   Config file which describe PSP directory layout,"
  echo "                      and input bios informations"
  echo "    OutputBiosFile    The output bios image after embed the PSP contents"
  echo "    KeyMode           'TK|PK' Specify which key will be used, test key or"
  echo "                      production key"
  exit 1
}

#ERROR
function ERROR {
  echo "**** BuildPspImage.sh: Build PSP Image Fail ****"
  exit 2
}

#Input parameter check
if [ "$5" == "" ]; then
	HELP
fi

#System variable Check
if [ ! $PSPKIT_PATH ]; then
	PSPKIT_PATH=$PWD
fi

if [ ! $PSPKIT_PATH/BuildPspImage.sh ]; then
	echo "!!!PSPKIT_PATH system variable is NOT set correctly!!"
	exit 1
fi

if [ ! $PSP_FW_PATH ]; then
	PSP_FW_PATH=$PWD/AmdPlatformAmurPkg/Psp/Firmware
fi


###START
#BIOS images
InputBiosFile=$1
RTM_FILE=$2
PspDirectoryCfg=$3
FINAL_BIOS=$4

#Related PATH
PSP_DIR_TOOL_PATH=$PSPKIT_PATH/PspDirectoryTool
TEMP_PATH=$PSPKIT_PATH/Temp

#Create Temporary folder, and copy all files to it
rm -rf $TEMP_PATH
mkdir $TEMP_PATH
cp $PSP_DIR_TOOL_PATH/* $TEMP_PATH/
cp $PSP_FW_PATH/*.* $TEMP_PATH/
cp $PSPKIT_PATH/*.* $TEMP_PATH/
if [ ! -z $CUSTOM_APCB_PATH ]; then
  cp $CUSTOM_APCB_PATH/*.* $TEMP_PATH/
fi

#TOOLS
BUILDPSPDIRECTORY=BuildPspDirectory.py
#Binaries
RTM_PRIVATE_KEY=TestRtmPrivateKey.pem
PSP_DIR_FILE=Output/BiosDirHeader.bin
RTM_PSPDIR_COMBINE_FILE=BiosRtmPspCombine.bin
RTM_FILE_SIGNATURE=RTMSignature.bin

pushd $TEMP_PATH

echo ""
echo "[Generate PSP Directory]"
#this_command="python2 $BUILDPSPDIRECTORY bd $InputBiosFile $PspDirectoryCfg"
#echo "$this_command"
#eval $this_command || ERROR


echo ""
echo "[Finally build the Final BiosImage]"
this_command="python2 $BUILDPSPDIRECTORY bb $InputBiosFile $PspDirectoryCfg $FINAL_BIOS"
echo "$this_command"
eval $this_command || ERROR
cp Output/$FINAL_BIOS ../
popd

echo ""
echo "PSP contents have been embed to $FINAL_BIOS"
