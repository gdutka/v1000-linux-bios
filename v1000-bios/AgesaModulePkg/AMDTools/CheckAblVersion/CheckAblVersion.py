 # *****************************************************************************
 # *
 # * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 # *
 # ******************************************************************************

# ###############################################################################
# Check_ABL_Version.py - Check the version number in the ABL Portion of the
#                        Bootloader Binary Header against APCB/APPB/APOB
#                        
#   Usage:  Check_ABL_Version.py <AgesaBootloader.bin>
#
# AgesaBootloader.bin :  Compilers Agesa Bootloader Binary
#
from sys import *
from getopt import *
from string import *
import os
import re
# ################################################################################
# Definitions
#
ScriptName = "Check_ABL_Version"

#
# APCB/APPB/APOB header files
#
ApcbHeaderPath = "APCB.h"
AppbHeaderPath = "APPB.h"
ApobHeaderPath = "APOB.h"

#
# Search Patterns
#
ApcbVersionDefinition = '^#define\s+APCB_HEADER_VERSION\s+0x([0-9A-F]+)'
AppbVersionDefinition = '^#define\s+APPB_VERSION\s+0x([0-9A-F]+)'
ApobVersionDefinition = '^#define\s+APOB_VERSION\s+0x([0-9A-F]+)'
#
# ABL Binary Header Offsets
#
AblVersionOffset  = 0x60
ApcbVersionOffset = 0xA0
ApobVersionOffset = 0xA4
AppbVersionOffset = 0xA8
SvcVersionOffset  = 0xAC
#
# APCB Binary Header Offset
#
ApcbBinVersionOffset = 0x06
#
# PSP BL Binary Offset
#
PspBlBinSvcVersionOffset = 0xA8
#
#  Table of Input Paramweters
#  This table matched the signature string with the offset in the header where it should be stored
#
VersionTable = [
	[ ApcbHeaderPath, ApcbVersionDefinition, ApcbVersionOffset],
	[ AppbHeaderPath, AppbVersionDefinition, AppbVersionOffset],
	[ ApobHeaderPath, ApobVersionDefinition, ApobVersionOffset]
	]

# ##############################################################
#
#  Display Usage
#
def Usage ():
	print '\n Usage 1: ' + argv[0] + ' --abl_bin=<AgesaBootloader> --apxb_path=<HeaderFilePath> --apcb_bin=<APCB>\n'
	print '  AgesaBootloader :  Compiled Agesa Bootloader Binary'
	print '  HeaderFilePath  :  Path of APCB.h/APPB.h/APOB.h'
	print '  APCB            :  APCB Binary'
	print
	print '\n Usage 2: ' + argv[0] + ' --abl_bin=<AgesaBootloader.bin> --apxb_path=<HeaderFilePath> --apcb_bin=<APCB> --psp_bl_bin=<PspBootloader>\n'
	print '  AgesaBootloader :  Compiled Agesa Bootloader Binary'
	print '  HeaderFilePath  :  Path of APCB.h/APPB.h/APOB.h'
	print '  APCB            :  APCB Binary'
	print '  PspBootloader   :  Compiled PSP Bootloader Binary'
	print
	exit (1)
	return

# ##############################################################
#
# Entry Point

# ##############################################################
#
# Process Arguments
#
print
print ScriptName
print

opts, arge = getopt (argv[1:], "h", ["abl_bin=", "apxb_path=", "apcb_bin=", "psp_bl_bin="])

AblBinNamePassed = 0
ApxbHeaderPathPassed = 0
ApcbBinNamePassed = 0
PspBlBinNamePassed = 0
AblBinName = ''
ApxbHeaderPath = ''
ApcbBinName = ''
PspBlBinName = ''

for op, value in opts:
	if op == '-h':
		Usage ()
		exit (1)
	elif op == '--abl_bin':
		AblBinName = value
		print '\t--abl_bin = ' +  AblBinName
		print
		AblBinNamePassed = 1
	elif op == '--apxb_path':
		ApxbHeaderPath = value
		ApxbHeaderPathPassed = 1
		print '\t--apxb_path = ' +  ApxbHeaderPath
		print
	elif op == '--apcb_bin':
		ApcbBinName = value
		ApcbBinNamePassed = 1
		print '\t--apcb_bin = ' +  ApcbBinName
		print
	elif op == '--psp_bl_bin':
		PspBlBinName = value
		PspBlBinNamePassed = 1
		print '\t--psp_bl_bin = ' +  PspBlBinName
		print

if AblBinNamePassed == 0 or ApxbHeaderPathPassed == 0 or ApcbBinNamePassed == 0:
	Usage ()
	exit (1)

#
# Read in the ABL Binary File 
#
if (os.path.exists (AblBinName)):
	with open (AblBinName,'rb') as AblBinFile:
		AblBinData = AblBinFile.read ()
else:
	print "--> Error: ABL binary file {} is required to determine APCB/APPB/APOB version information.".format (AblBinName)
	exit (1)

print "\tABL Binary File: " + AblBinName

if PspBlBinNamePassed == 1:
	#
	# Read in the PSP BL Binary File 
	#
	if (os.path.exists (PspBlBinName)):
		with open (PspBlBinName,'rb') as PspBlBinFile:
			PspBlBinData = PspBlBinFile.read ()
	else:
		print "--> Error: PSP BL binary file {} is required to determine SVC version information.".format (PspBlBinName)
		exit (1)
	print "\tPSP BL Binary File: " + PspBlBinName
	AblBinSvcVersion = AblBinData[SvcVersionOffset : SvcVersionOffset + 4]
	print "\tABL Binary SVC Version: {}".format (AblBinSvcVersion[::-1].encode('hex'))
	PspBlBinSvcVersion = PspBlBinData[PspBlBinSvcVersionOffset: PspBlBinSvcVersionOffset + 4]
	print "\tPSP BL Binary SVC Version: {}".format (PspBlBinSvcVersion[::-1].encode('hex'))
	if int (AblBinSvcVersion[::-1].encode('hex'), 16) > int (PspBlBinSvcVersion[::-1].encode('hex'), 16):
		print "--> Error: ABL Syscall Version is out of sync with te SysCall version in PSP BL!"
		exit (1)

#
# Read in the APCB Binary File 
#
if (os.path.exists (ApcbBinName)):
	with open (ApcbBinName,'rb') as ApcbBinFile:
		ApcbBinData = ApcbBinFile.read ()
else:
	print "--> Error: APCB binary file {} is required to determine APCB version information.".format (ApcbBinName)
	exit (1)

print "\tAPCB Binary File: " + ApcbBinName

for InputParams in VersionTable:
	DefinitionFound = 0
	APXBName = InputParams[0]
	APXBHeaderName = ApxbHeaderPath + "\\" + InputParams[0]
	APXBVersionDef = InputParams[1]
	APXBVersionOffset = InputParams[2]

	Version = AblBinData[APXBVersionOffset : APXBVersionOffset + 4]
	if (os.path.exists (APXBHeaderName)):
		#
		# Open APXB Header File
		#
		with open (APXBHeaderName,'r') as APXBHeaderFile:
			for line in APXBHeaderFile:
				if re.match (APXBVersionDef, line):
					DefinitionFound = 1
					VersionString = re.match (APXBVersionDef, line)
					print "\tABL Binary {} Version: {}".format (APXBName, Version[::-1].encode('hex'))
					print "\t{} Version: {}".format (APXBName, VersionString.group(1))
					if int (VersionString.group(1), 16) < int (Version[::-1].encode('hex'), 16):
						print "--> Error: ABL {} Version is out of sync with {}!".format (APXBName[0:4], APXBName)
						exit (1)
					#
					# Check against APCB Binary
					#
					if (re.match ('APCB', APXBName)):
						ApcbBinVersion = ApcbBinData[ApcbBinVersionOffset : ApcbBinVersionOffset + 2]
						print "\tAPCB Binary Version: {}".format (ApcbBinVersion[::-1].encode('hex'))
						if int (VersionString.group(1), 16) < int (ApcbBinVersion[::-1].encode('hex'), 16):
							print "--> Error: {} Version is out of sync with {}!".format (APXBName[0:4], APXBName)
							exit (1)
	else:
		print "--> Error: Header file {} is required to determine version information.".format (APXBHeaderName)
		exit (1)

	if DefinitionFound == 0:
		print "--> Error: No version defined in Header file {}".format (APXBHeaderName)
		exit (1)
