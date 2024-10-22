#!/bin/bash
APCB_TOOL_PATH=$WORKSPACE/AgesaModulePkg/AMDTools/ApcbToolV2/Tools
APCB_PKG_PATH=$WORKSPACE/AgesaPkg/Addendum/Apcb
APCB_INC_PATH=$APCB_PKG_PATH/Inc
APCB_BOARD_PKG_PATH=$APCB_PKG_PATH/RavenFp5Udimm


APCB_TEMP_PATH=$WORKSPACE/Build/$PROJECT_PKG/ApcbToolV2
APCB_TOOL_TEMP_PATH=$APCB_TEMP_PATH/Tools
APCB_INC_TEMP_PATH=$APCB_TEMP_PATH/Inc
APCB_BOARD_PKG_TEMP_PATH=$APCB_TEMP_PATH/External
APCB_BUILD_COMMAND=ApcbCreate.sh



#
# Clean up existing files
#
if [ -d $APCB_TEMP_PATH ]; then
  rm -rf $APCB_TEMP_PATH
fi
mkdir -p $APCB_TOOL_TEMP_PATH
mkdir -p $APCB_INC_TEMP_PATH
mkdir -p $APCB_BOARD_PKG_TEMP_PATH
mkdir -p $APCB_BOARD_PKG_TEMP_PATH/Include

cp -R $APCB_TOOL_PATH/*                      $APCB_TOOL_TEMP_PATH
cp -R $APCB_INC_PATH/*                       $APCB_INC_TEMP_PATH
cp -R $APCB_BOARD_PKG_PATH/*                 $APCB_BOARD_PKG_TEMP_PATH

#
# Override platform specific settings
#
cp $WORKSPACE/$PROJECT_PKG/ApcbCustomization/ApcbCustomizedDefinitionsRvFp5.h $APCB_BOARD_PKG_TEMP_PATH/Include/ApcbCustomizedDefinitions.h
#cp $WORKSPACE/$PROJECT_PKG/ApcbCustomization/$APCB_BUILD_COMMAND $APCB_BOARD_PKG_TEMP_PATH/$APCB_BUILD_COMMAND

#
# Override autogenerated GBS settings
#
cp $WORKSPACE/AmdCbsPkg/Library/Family/0x17/RV/External/ResourceRVFP5/ApcbAutoGenRV.h $APCB_BOARD_PKG_TEMP_PATH/Include/ApcbAutoGen.h

pushd $APCB_BOARD_PKG_TEMP_PATH

#
# Customize the APCB tool directory
#
TOOL_DIR=$APCB_TOOL_TEMP_PATH
export TOOL_DIR

#
# Run ApcbCreate twice to work around an issue in finding the bin files
#
echo $APCB_BOARD_PKG_TEMP_PATH/$APCB_BUILD_COMMAND
chmod -R a+x $APCB_TEMP_PATH
$APCB_BOARD_PKG_TEMP_PATH/$APCB_BUILD_COMMAND CLEAN
$APCB_BOARD_PKG_TEMP_PATH/$APCB_BUILD_COMMAND BUILD
#rem if %errorlevel% neq 0 exit

CUSTOM_APCB_PATH=$APCB_BOARD_PKG_TEMP_PATH/Release/

popd
