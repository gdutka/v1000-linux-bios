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

export BIOSNAME = AristaVlh
export WORKSPACE = $(shell dirname $$PWD)
export GBS_FILE_NAME=iRavenSetting.xml
export TARGET=$(word 3, $(shell grep 'TARGET ='  $(WORKSPACE)/Conf/target.txt))
export TOOL_CHAIN_TAG=$(word 3, $(shell grep 'TOOL_CHAIN_TAG '  $(WORKSPACE)/Conf/target.txt))
export RAVEN_16M=$(word 4, $(shell grep 'RAVEN_16M'  Project.env))
export BACKUP_BIOS=$(word 4, $(shell grep 'BACKUP_BIOS'  Project.env))
FV_OUTPUT_PATH=$(WORKSPACE)/Build/$(PROJECT_PKG)/$(TARGET)_$(TOOL_CHAIN_TAG)/FV

#
#	Option Variable for gcc path if needed to use specific gcc version
#
export GCC_PATH=/usr/bin

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

	@if [ "$(BACKUP_BIOS)" = "YES" ]; then \
		$(WORKSPACE)/$(PROJECT_PKG)/BIOS/Tools/dd_Backup.sh "../$(BIOSNAME)" && \
		rm -f "$(WORKSPACE)/$(PROJECT_PKG)/BIOS/$(BIOSNAME).fd"; \
	else \
		$(WORKSPACE)/$(PROJECT_PKG)/BIOS/Tools/dd.sh "../$(BIOSNAME)" && \
		rm -f "$(WORKSPACE)/$(PROJECT_PKG)/BIOS/$(BIOSNAME).fd"; \
	fi

	@echo "BACKUP_BIOS = $(BACKUP_BIOS)"

