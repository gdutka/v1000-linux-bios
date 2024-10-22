#!/bin/bash

# ============================================================================
# ============================================================================
# ============================================================================
# Start of customization section
# ============================================================================
# ============================================================================
# ============================================================================

# ============================================================================
# Customize the APCB output binary and input data files
# ============================================================================

function main() {
	# ----------------------------------------------------------------------------
	# First thing, setup ApcbToolV2.exe
	# ----------------------------------------------------------------------------
	TOOL_DIR=${TOOL_DIR:-$(pwd)/../../../../AgesaModulePkg/AMDTools/ApcbToolV2/Tools}

	# ----------------------------------------------------------------------------
	# Target APCB binary will be put in RELEASE_DIR
	# ----------------------------------------------------------------------------
	APCB_BIN_FILE_BASE_NAME=APCB_RV_D4

	# ----------------------------------------------------------------------------
	# Reset variable then start adding APCB data source C files (base name only)
	# ----------------------------------------------------------------------------
	APCB_DATA_DIR=ApcbData
	APCB_DATA_TYPE_FILE_LIST="\
	  ApcbData_RV_GID_0x1706_Type_ExtDefParams\
	  ApcbData_RV_GID_0x1706_Type_ExtParams\
	  ApcbData_RV_GID_0x1704_Type_SpdInfo\
	  ApcbData_RV_GID_0x1703_Type_ExtDefParams\
	  ApcbData_RV_GID_0x1703_Type_ExtParams\
	  ApcbData_RV_GID_0x1704_Type_ConsoleOutControl\
	  ApcbData_RV_GID_0x1704_Type_DimmInfoSpd\
	  ApcbData_RV_GID_0x1704_Type_ErrorOutControl\
	  ApcbData_RV_GID_0x1704_Type_ExtDefParams\
	  ApcbData_RV_GID_0x1704_Type_ExtParams\
	  ApcbData_RV_GID_0x1704_Type_ExtVoltageControl\
	  ApcbData_RV_GID_0x1704_Type_PsoOverride\
	  ApcbData_RV_GID_0x1704_Type_PsOdtPatUDIMMDdr4\
	  ApcbData_RV_GID_0x1704_Type_PsCadBusUDIMMDdr4 \
	  ApcbData_RV_GID_0x1704_Type_PsDataBusUDIMMDdr4\
	  ApcbData_RV_GID_0x1704_Type_PsMaxFreqUDIMMDdr4\
	  ApcbData_RV_GID_0x1704_Type_PsStretchFreqUDIMMDdr4\
	  ApcbData_RV_GID_0x1707_Type_IntDefParams\
	  ApcbData_RV_GID_0x1707_Type_IntParams"

	BAT_TOOL_NAME=$(basename "$0")

	DISP_HEADER
	# clear
	ENV_CHECK
	GET_WORK_DIR

	if [ "$1" = "" ] || [ "${1,,}" = "-h" ]; then DISP_HELP; fi

	BUILD_DIR=Build
	RELEASE_DIR=Release
	LOG_DIR=Log

	# Force to use parameter to continue the build
	if [ "${1,,}" = "clean" ]; then CLEAN_OUTPUT; fi
	if [ "${1,,}" = "build" ]; then START_BUILDING; fi
}

# ----------------------------------------------------------------------------
# BoardIdGettingMethod applies to multiple APCB only, please remove it if
# only 1 APCB instance is needed
# ----------------------------------------------------------------------------
# SET APCB_DATA_TYPE_FILE_LIST=$APCB_DATA_TYPE_FILE_LIST ApcbData_RV_GID_0x1704_Type_BoardIdGettingMethod

# ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
# End of customization section
# ============================================================================
# ============================================================================
# ============================================================================


# ============================================================================
# Local functions
# ============================================================================

# ----------------------------------------------------------------------------
# Environment Checks
# ----------------------------------------------------------------------------
function ENV_CHECK() {
	APCB_TOOL_NAME=ApcbToolV2Ubuntu
	if [ -f "$TOOL_DIR/$APCB_TOOL_NAME" ]; then
		echo Found $APCB_TOOL_NAME at "$TOOL_DIR".
	else
		ERROR_TOOLDIR
		exit 1
	fi
}

# ----------------------------------------------------------------------------
# Display help info
# ----------------------------------------------------------------------------
function DISP_HELP() {
	echo
	echo "$BAT_TOOL_NAME" help info
	echo
	echo USAGE:
	echo "      For clean up output directories (Release, Build and Log directoies)"
	echo		"$BAT_TOOL_NAME"  clean
	echo
	echo       For build APCB data and tool
	echo		"$BAT_TOOL_NAME"  build
	echo
	exit 1
}

# ----------------------------------------------------------------------------
# Get current working director and set variable APCB_LOCAL_WORK_DIR for later use
# ----------------------------------------------------------------------------
function GET_WORK_DIR() {
	: "${APCB_LOCAL_WORK_DIR:=$PWD}"
}

# ----------------------------------------------------------------------------
# Clean up output folder
# ----------------------------------------------------------------------------
function CLEAN_OUTPUT() {
	for dir in $APCB_LOCAL_WORK_DIR/$BUILD_DIR $APCB_LOCAL_WORK_DIR/$LOG_DIR $APCB_LOCAL_WORK_DIR/$RELEASE_DIR
	do
		if [ -d "$dir" ]; then
			echo Deleting directory "$dir" ...
			rm -r "$dir"
		fi
	done
}

# ----------------------------------------------------------------------------
# Prepare data file list in tool parameter form
# ----------------------------------------------------------------------------
function PREPARE_DATA_FILE_LIST_IN_TOOL_PARAM_FORM() {
	APCB_DATA_TYPE_FILE_LIST_TOOL_PARAM=
	for i in $APCB_DATA_TYPE_FILE_LIST
	do
		APPEND_DATA_FILE_LIST_IN_TOOL_PARAM_FORM "$i"
	done
}

# ----------------------------------------------------------------------------
# Append data file list in tool parameter form
# ----------------------------------------------------------------------------
function APPEND_DATA_FILE_LIST_IN_TOOL_PARAM_FORM() {
	# Specify the build data file extension
	APCB_DATA_OUTPUT_FILE_EXT=obj
	APCB_DATA_TYPE_FILE_LIST_TOOL_PARAM="$APCB_DATA_TYPE_FILE_LIST_TOOL_PARAM -I $1.$APCB_DATA_OUTPUT_FILE_EXT"
}

# ----------------------------------------------------------------------------
# Build all C source files in APCB_DATA_TYPE_FILE_LIST
# ----------------------------------------------------------------------------
function BUILD_ALL_C_SOURCES() {
	# dont't run linker, force 32 bit mode for correct alignment in APCB binary
	C_FLAGS="-c -m32"
	C_INCLUDE_PATH_FLAG="-I./Include -I../Inc -I../../Inc"
	local gcc_prefix_var=${TOOL_CHAIN}_BIN
	#CC=${!gcc_prefix_var}gcc
    	CC=$(which gcc)
	echo  Resulting GCC path: "$CC"
	# --------------------------------------------------------------------
	if [ ! -f "$CC" ]; then
		echo ERROR: Cannot find GCC specified by TOOL_CHAIN env variable
		echo   make sure it is set correctly. Resulting GCC path: "$CC"
		#ERR_END
	fi
	if [ ! -d "$APCB_LOCAL_WORK_DIR/$BUILD_DIR" ]; then
		mkdir "$APCB_LOCAL_WORK_DIR/$BUILD_DIR" || ERR_END
	fi
	if [ ! -d "$APCB_LOCAL_WORK_DIR/$RELEASE_DIR" ];  then
		mkdir "$APCB_LOCAL_WORK_DIR/$RELEASE_DIR" || ERR_END
	fi
	if [ ! -d  "$APCB_LOCAL_WORK_DIR/$LOG_DIR" ]; then
		mkdir "$APCB_LOCAL_WORK_DIR/$LOG_DIR" || ERR_END
	fi
	# --------------------------------------------------------------------
	# Build APCB data files
	# --------------------------------------------------------------------
	for Src in $APCB_DATA_TYPE_FILE_LIST
	do
		echo Compiling "$Src"
		$CC $C_FLAGS "$APCB_DATA_DIR/$Src.c" $C_INCLUDE_PATH_FLAG $EXTRA_C_FLAGS -o "$APCB_LOCAL_WORK_DIR/$BUILD_DIR/$Src.obj" || ERR_END
	done
}

# ----------------------------------------------------------------------------
# Run APCB Tool and generate log
#
# INPUT:
#        BUILD_DIR
#        APCB_BIN_FILE
#        APCB_DATA_TYPE_FILE_LIST_TOOL_PARAM
#        APCB_EXE_LOG
# OUTPUT:
#        APCB_BIN_FILE - APCB binary file specified
#        APCB_EXE_LOG  - Execution log specified (create a new file)
# ----------------------------------------------------------------------------
function RUN_APCB_TOOL_WITH_EXE_LOG() {
	APCB_EXE_LOG_INPUT=$APCB_EXE_LOG
	# --------------------------------------------------------------------
	APCB_EXE_LOG=$APCB_LOCAL_WORK_DIR/$LOG_DIR/$APCB_EXE_LOG_INPUT.txt
	echo "Run APCB tool with log ...      (Log:$APCB_EXE_LOG)"
	pushd "$APCB_LOCAL_WORK_DIR/$BUILD_DIR" || ERR_END
	APCB_TOOL || ERR_END
	popd || ERR_END
	# --------------------------------------------------------------------
	# Restore variables
	APCB_EXE_LOG=$APCB_EXE_LOG_INPUT
	APCB_EXE_LOG_INPUT=
}

# ----------------------------------------------------------------------------
# Run APCB Generator tool V2 by ApcbToolV2
#
# INPUT:
#        BUILD_DIR
#        APCB_BIN_FILE
#        APCB_DATA_TYPE_FILE_LIST_TOOL_PARAM
#        APCB_EXE_LOG
# OUTPUT:
#        APCB_BIN_FILE - APCB binary file specified
#        APCB_EXE_LOG  - Execution log specified (create a new file)
# ----------------------------------------------------------------------------
function APCB_TOOL() {
	"$TOOL_DIR/$APCB_TOOL_NAME" --outputfile "$APCB_BIN_FILE" $APCB_DATA_TYPE_FILE_LIST_TOOL_PARAM > "$APCB_EXE_LOG" || ERR_END
}


# ============================================================================
function ERROR_TOOLDIR() {
	echo
	echo
	echo "***********************************************************************"
	echo  "ERROR : $APCB_TOOL_NAME cannot be found at $TOOL_DIR (TOOL_DIR)"
	echo
	echo  Environment variable TOOL_DIR should be set properly.
	echo  $APCB_TOOL_NAME should be found under folder of "AgesaModulePkg/AMDTools/ApcbToolV2\Tools".
	echo "***********************************************************************"
	echo
	echo
	exit 1
}

# ============================================================================
function ERR_END() {
	echo
	echo "**************************"
	echo  Exit with error!
	echo "**************************"
	exit 1
}

function END() {
	echo
	echo "**************************"
	echo  "$BAT_TOOL_NAME" completed!
	echo "**************************"
	echo
}


# ----------------------------------------------------------------------------
# Display header
# ----------------------------------------------------------------------------
function DISP_HEADER() {
	echo
	echo "**************************"
	echo  Start "$BAT_TOOL_NAME"
	echo "**************************"
	echo
}

# ============================================================================
# Start building if input proper parameter
# ============================================================================
function START_BUILDING() {

	# ============================================================================
	echo
	echo Building APCB data ...
	# ============================================================================
	BUILD_ALL_C_SOURCES || ERR_END

	# ============================================================================
	echo
	echo Prepare APCB data for building ...
	# ============================================================================

	# ============================================================================
	# Prepare parameter for data files from APCB_DATA_TYPE_FILE_LIST
	# ============================================================================
	PREPARE_DATA_FILE_LIST_IN_TOOL_PARAM_FORM

	# ============================================================================
	echo
	echo APCB binary generation ...
	# ============================================================================
	APCB_BIN_FILE=$APCB_LOCAL_WORK_DIR/$RELEASE_DIR/$APCB_BIN_FILE_BASE_NAME.bin
	# File extension .txt will be added
	APCB_EXE_LOG=ApcbBuild_ExeLog
	RUN_APCB_TOOL_WITH_EXE_LOG || ERR_END
	END
}



main "$@"