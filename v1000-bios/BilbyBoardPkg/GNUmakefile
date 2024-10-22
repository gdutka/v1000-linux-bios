## @file
#  Common Makefile for project build
#
#******************************************************************************
#* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
#*
#* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#* transmit, broadcast, present, recite, release, license or otherwise exploit
#* any part of this publication in any form, by any means, without the prior
#* written permission of Insyde Software Corporation.
#*
#******************************************************************************

#
# Build options for build targets
#

export BIOSNAME = BILBY
export WORKSPACE = $(shell dirname $$PWD)
export GBS_FILE_NAME=iRavenSetting.xml
export TARGET=$(word 3, $(shell grep 'TARGET ='  $(WORKSPACE)/Conf/target.txt))
export TOOL_CHAIN_TAG=$(word 3, $(shell grep 'TOOL_CHAIN_TAG '  $(WORKSPACE)/Conf/target.txt))
export RAVEN_16M=$(word 4, $(shell grep 'RAVEN_16M'  Project.env))

include $(WORKSPACE)/BaseTools/Conf/GNUmakefile

buildcbs:
	$(shell chmod a+x $(WORKSPACE)/AmdCbsPkg/Tools/IdsIdGen.sh)
	$(shell chmod a+x $(WORKSPACE)/AmdCbsPkg/Library/Family/0x17/RV/External/xmlparse.sh)
	@$(WORKSPACE)/AmdCbsPkg/Tools/IdsIdGen.sh
	@$(WORKSPACE)/AmdCbsPkg/Library/Family/0x17/RV/External/xmlparse.sh BILBY

project_prebuild:
	#
	# Copy OverrideHeaderFile to BIOS code root $(WORKSPACE)
	#
	echo "Start: Copy OverrideHeaderFile to $(WORKSPACE) "
	$(shell chmod a+x $(WORKSPACE))
	cp -r $(WORKSPACE)/$(PROJECT_PKG)/OverrideHeaderFile/* $(WORKSPACE)
	echo "End:   Copy OverrideHeaderFile to $(WORKSPACE) "
	#
	# AMD APCB
	#
	$(shell chmod a+x $(WORKSPACE)/$(PROJECT_PKG)/GenApcbBinaryRV.sh)
	$(WORKSPACE)/$(PROJECT_PKG)/GenApcbBinaryRV.sh

project_postbuild:
	$(WORKSPACE)/BaseTools/Bin/Linux/PatchFdmHash $(WORKSPACE)/Build/$(PROJECT_PKG)/$(word 3, $(shell grep 'TARGET '  $(WORKSPACE)/Conf/target.txt))_$(TOOL_CHAIN)/FV/BILBY.fd  $(WORKSPACE)/Build/$(PROJECT_PKG)/$(word 3, $(shell grep 'TARGET '  $(WORKSPACE)/Conf/target.txt))_$(TOOL_CHAIN)/FV/BILBY.fdm 

	$(shell chmod a+x $(WORKSPACE)/$(PROJECT_PKG)/PspBuild.sh)
	$(WORKSPACE)/$(PROJECT_PKG)/PspBuild.sh
