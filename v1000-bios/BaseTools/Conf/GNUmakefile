## @file
#  Common Makefile for project build
#
#******************************************************************************
#* Copyright (c) 2015 - 2020, Insyde Software Corp. All Rights Reserved.
#*
#* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#* transmit, broadcast, present, recite, release, license or otherwise exploit
#* any part of this publication in any form, by any means, without the prior
#* written permission of Insyde Software Corporation.
#*
#******************************************************************************

export EDK_TOOLS_PATH = $(WORKSPACE)/BaseTools
export PATH = $(EDK_TOOLS_PATH)/Bin/Linux:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
export PROJECT_PKG = $(shell basename $$PWD)
#
# Set PCD values if string tokens are defined in Project.uni.
#
export CCB_VER_PCD_VALUE=0x$(shell GetProjectEnv PROJECT_UNI_STR_VALUE STR_CCB_VERSION | grep -o '^\s*[A-Za-z]\{0,3\}[0-9]\{2\}\.[0-9]\{2\}\.[0-9]\{2\}' | sed 's/[^0-9]//g')00
export CCB_VER_PCD_BUILD_OPTION=--pcd gInsydeTokenSpaceGuid.PcdH2OCcbVersion=$(CCB_VER_PCD_VALUE)
export ESRT_VERSION_PCD_VALUE=$(shell GetProjectEnv PROJECT_UNI_STR_VALUE STR_ESRT_VERSION)
ifneq ($(ESRT_VERSION_PCD_VALUE),)
export ESRT_VERSION_PCD_BUILD_OPTION=--pcd gInsydeTokenSpaceGuid.PcdH2OEsrtSystemFirmwareVersion=0x$(ESRT_VERSION_PCD_VALUE)
endif
export ESRT_LOWEST_SUPPORTED_VERSION_PCD_VALUE=$(shell GetProjectEnv PROJECT_UNI_STR_VALUE STR_ESRT_LOWEST_SUPPORTED_VERSION)
ifneq ($(ESRT_LOWEST_SUPPORTED_VERSION_PCD_VALUE),)
export ESRT_LOWEST_SUPPORTED_VERSION_PCD_BUILD_OPTION=--pcd gInsydeTokenSpaceGuid.PcdH2OEsrtSystemFirmwareLowestSupportedVersion=0x$(ESRT_LOWEST_SUPPORTED_VERSION_PCD_VALUE)
endif
export ESRT_FIRMWARE_GUID_PCD_VALUE=$(shell GetProjectEnv PROJECT_UNI_STR_VALUE STR_ESRT_FIRMWARE_GUID | awk -F- '{print "H{0x"substr($$1,7,2)",0x"substr($$1,5,2)",0x"substr($$1,3,2)",0x"substr($$1,1,2)",0x"substr($$2,3,2)",0x"substr($$2,1,2)",0x"substr($$3,3,2)",0x"substr($$3,1,2)",0x"substr($$4,1,2)",0x"substr($$4,3,2)",0x"substr($$5,1,2)",0x"substr($$5,3,2)",0x"substr($$5,5,2)",0x"substr($$5,7,2)",0x"substr($$5,9,2)",0x"substr($$5,11,2)"}"}')
ifneq ($(ESRT_FIRMWARE_GUID_PCD_VALUE),)
export ESRT_FIRMWARE_GUID_PCD_BUILD_OPTION=--pcd gInsydeTokenSpaceGuid.PcdH2OEsrtSystemFirmwareGuid=$(ESRT_FIRMWARE_GUID_PCD_VALUE)
endif
export BUILD_PCD_VALUE=$(CCB_VER_PCD_BUILD_OPTION) $(ESRT_VERSION_PCD_BUILD_OPTION) $(ESRT_LOWEST_SUPPORTED_VERSION_PCD_BUILD_OPTION) $(ESRT_FIRMWARE_GUID_PCD_BUILD_OPTION)

#
# Set-up the execution authority of all the build tools.
#
$(shell chmod -R a+x $(EDK_TOOLS_PATH)/Bin/Linux)
$(shell chmod a+x $(WORKSPACE)/InsydeModulePkg/Tools/CertToPublicKey/CertToPublicKey)
$(shell chmod a+x $(WORKSPACE)/InsydeModulePkg/Tools/GenBvdt/GenBvdt)
$(shell chmod a+x $(WORKSPACE)/InsydeModulePkg/Tools/GenFactoryCopy/GenFactoryCopy)
$(shell chmod a+x $(WORKSPACE)/InsydeModulePkg/Tools/GenVariableFvs/GenVariableFvs)

#
# Set-up library path for tool modules
#
ifeq ($(LIB32_PATH),)
ifeq ("$(shell uname -m)", "x86_64")
#
# For i386 docker on x86_64, check /usr/lib32 firstly
#
ifneq ($(wildcard /usr/lib32),)
export LIB32_PATH=/usr/lib32
else
export LIB32_PATH=/usr/lib/i386-linux-gnu
endif
else
export LIB32_PATH=/usr/lib/i386-linux-gnu
endif
endif

#
# Setup NASM path
#
ifeq ($(NASM_PREFIX),)
export NASM_PREFIX=/usr/bin/
endif

#
# Build options for build targets
#
DDT_BUILD      = "-D INSYDE_DEBUGGER=YES"
EFI_DEBUG      = "-D EFI_DEBUG=YES"
DDT_DEBUG      = "-D INSYDE_DEBUGGER=YES -D EFI_DEBUG=YES"
EFI_DEBUG_DDT  = "-D INSYDE_DEBUGGER=YES -D EFI_DEBUG=YES"
PERF_BUILD     = "-D SYMBOLIC_DEBUG=YES -D INSYDE_DEBUGGER=NO -D EFI_DEBUG=NO -D FIRMWARE_PERFORMANCE=YES"
EFI_DEBUG_USB  = "-D EFI_DEBUG=YES -D USB_DEBUG_SUPPORT=YES"

ifeq ($(BUILD_MODE),)
#
# Available build modes
# -q: Quiet mode - disable all messages except fatal errors, module based messages
# -s: Silent mode - file based messages
# -v: Turn on verbose output with informational messages printed
#
BUILD_MODE     = -s --Dynamic-to-DynamicEx
endif

ifeq ($(ARCH),)
ARCH = IA32 X64
endif
export ARCH

ifeq ($(TOOL_CHAIN),)
TOOL_CHAIN = GCC48
endif

ifeq ($(BUILD_REPORT),)
BUILD_REPORT = -Y PCD -Y DEPEX -Y LIBRARY -y $(WORKSPACE)/Build/$(PROJECT_PKG)/BuildReport.txt
endif

## @BuildTarget "IA32 X64", RELEASE, Release build for x86 64-bit CPU Arch.
uefi64:
	$(MAKE) release BUILD_TARGET=$@

## @BuildTarget "IA32", RELEASE, Release build for x86 32-bit CPU Arch.
uefi32:
	$(MAKE) release BUILD_TARGET=$@

## @BuildTarget "IA32 X64", DEBUG, Debug build for Insyde Debugger
uefi64ddt:
	@$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(DDT_BUILD)

## @BuildTarget "IA32", DEBUG, Debug build for Insyde Debugger
uefi32ddt:
	@$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(DDT_BUILD)

## @BuildTarget "Common", DEBUG, Debug build with EFI debug messages via serial port
efidebug:
	@$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(EFI_DEBUG)

## @BuildTarget "IA32 X64", DEBUG, Debug build with EFI debug messages via serial port
debug64:
	@$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(EFI_DEBUG)

## @BuildTarget "IA32", DEBUG, Debug build with EFI debug messages via serial port
debug32:
	@$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(EFI_DEBUG)

## @BuildTarget "Common", DEBUG, Debug build with EFI debug messages via USB port
efidebugusb:
	@$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(EFI_DEBUG_USB)

## @BuildTarget "IA32 X64", DEBUG, Debug build with EFI debug messages via USB port
debug64usb:
	@$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(EFI_DEBUG_USB)

## @BuildTarget "IA32", DEBUG, Debug build with EFI debug messages via USB port
debug32usb:
	@$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(EFI_DEBUG_USB)

## @BuildTarget "Common", DEBUG, Debug build with EFI debug messages via serial port and Insyde Debugger application
efidebugddt:
	@$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(EFI_DEBUG_DDT)

## @BuildTarget "IA32 X64", DEBUG, Debug build with EFI debug messages via serial port and Insyde Debugger application
debug64ddt:
	@$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(EFI_DEBUG_DDT)

## @BuildTarget "IA32", DEBUG, Debug build with EFI debug messages via serial port and Insyde Debugger application
debug32ddt:
	@$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(EFI_DEBUG_DDT)

## @BuildTarget "Common", DEBUG, Debug build with EFI debug messages via DDT cable and Insyde Debugger application
ddtdebug:
	@$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(DDT_DEBUG)

## @BuildTarget "IA32 X64", DEBUG, Debug build with EFI debug messages via DDT cable and Insyde Debugger application
ddtdebug64:
	@$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(DDT_DEBUG)

## @BuildTarget "IA32", DEBUG, Debug build with EFI debug messages via DDT cable and Insyde Debugger application
ddtdebug32:
	@$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(DDT_DEBUG)

## @BuildTarget "IA32 X64", DEBUG, Performance-log enabled build for x86 64-bit CPU Arch.
uefi64perf:
	@$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(PERF_BUILD)

## @BuildTarget "IA32", DEBUG, Performance-log enabled build for x86 32-bit CPU Arch.
uefi32perf:
	@$(MAKE) debug BUILD_TARGET=$@ BUILD_TARGET_OPTIONS=$(PERF_BUILD)

## @BuildTarget "Common", RELEASE, Release package build for InsydeModulePkg package
kernel: cleancache gen_release_target
	@build -D GCC=Gcc -p InsydeModulePkg/InsydeModulePkg.dsc -N -s -b RELEASE --Dynamic-to-DynamicEx

kerneldebug: cleancache gen_debug_target
	@build -D GCC=Gcc -p InsydeModulePkg/InsydeModulePkg.dsc -N -s -b DEBUG --Dynamic-to-DynamicEx

cleancache:
	@rm $(WORKSPACE)/Conf/.cache -rf

clean: cleancache
	@rm $(WORKSPACE)/Build/$(PROJECT_PKG) -rf

release: gen_release_target clean_release_var prebuild build postbuild
	@if test $(WORKSPACE)/Build/$(PROJECT_PKG)/BuildDuration; then echo Build time :; tail $(WORKSPACE)/Build/$(PROJECT_PKG)/BuildDuration; echo ; fi;

debug: gen_debug_target clean_debug_var prebuild build postbuild
	@if test $(WORKSPACE)/Build/$(PROJECT_PKG)/BuildDuration; then echo Build time :; tail $(WORKSPACE)/Build/$(PROJECT_PKG)/BuildDuration; echo ; fi;

build:
	build -D GCC=Gcc -D LIB32_PATH=$(LIB32_PATH) -N $(BUILD_MODE) $(BUILD_TARGET_OPTIONS) $(BUILD_PCD_VALUE) $(BUILD_REPORT) $(PROJECT_BUILD_OPTIONS)

gen_release_target:
	@echo ACTIVE_PLATFORM              = Build/$(PROJECT_PKG)/Project.dsc >  $(WORKSPACE)/Conf/target.txt
	@echo TARGET_ARCH                  = $(ARCH)                          >> $(WORKSPACE)/Conf/target.txt
	@echo TARGET                       = RELEASE                          >> $(WORKSPACE)/Conf/target.txt
	@echo TOOL_CHAIN_CONF              = Conf/tools_def.txt               >> $(WORKSPACE)/Conf/target.txt
	@echo TOOL_CHAIN_TAG               = $(TOOL_CHAIN)                    >> $(WORKSPACE)/Conf/target.txt
	@echo MAX_CONCURRENT_THREAD_NUMBER = $(shell nproc)                   >> $(WORKSPACE)/Conf/target.txt
	@echo BUILD_RULE_CONF              = Conf/build_rule.txt              >> $(WORKSPACE)/Conf/target.txt
	@mkdir -p $(WORKSPACE)/Build/$(PROJECT_PKG)/RELEASE_$(TOOL_CHAIN)/FV
	@cp /dev/null $(WORKSPACE)/Build/$(PROJECT_PKG)/RELEASE_$(TOOL_CHAIN)/FV/Font.hpk
	@cp /dev/null $(WORKSPACE)/Build/$(PROJECT_PKG)/RELEASE_$(TOOL_CHAIN)/FV/Layout.hpk

gen_debug_target:
	@echo ACTIVE_PLATFORM              = Build/$(PROJECT_PKG)/Project.dsc >  $(WORKSPACE)/Conf/target.txt
	@echo TARGET_ARCH                  = $(ARCH)                          >> $(WORKSPACE)/Conf/target.txt
	@echo TARGET                       = DEBUG                            >> $(WORKSPACE)/Conf/target.txt
	@echo TOOL_CHAIN_CONF              = Conf/tools_def.txt               >> $(WORKSPACE)/Conf/target.txt
	@echo TOOL_CHAIN_TAG               = $(TOOL_CHAIN)                    >> $(WORKSPACE)/Conf/target.txt
	@echo MAX_CONCURRENT_THREAD_NUMBER = $(shell nproc)                   >> $(WORKSPACE)/Conf/target.txt
	@echo BUILD_RULE_CONF              = Conf/build_rule.txt              >> $(WORKSPACE)/Conf/target.txt
	@mkdir -p $(WORKSPACE)/Build/$(PROJECT_PKG)/DEBUG_$(TOOL_CHAIN)/FV
	@cp /dev/null $(WORKSPACE)/Build/$(PROJECT_PKG)/DEBUG_$(TOOL_CHAIN)/FV/Font.hpk
	@cp /dev/null $(WORKSPACE)/Build/$(PROJECT_PKG)/DEBUG_$(TOOL_CHAIN)/FV/Layout.hpk

#
# Pre-build processes
#
kernel_prebuild:
	@echo Processing Kernel Pre-build
	@cp $(WORKSPACE)/$(PROJECT_PKG)/Project.dsc $(WORKSPACE)/Build/$(PROJECT_PKG)/Project.dsc
	@cp $(WORKSPACE)/$(PROJECT_PKG)/Project.fdf $(WORKSPACE)/Build/$(PROJECT_PKG)/Project.fdf
	GenFlashSetting Build/$(PROJECT_PKG)/AutoGen

chipset_prebuild:
	@echo Dummy Chipset PreBuild

project_prebuild:
	@echo Dummy Project PreBuild

clean_release_var:
	@if test -d $(WORKSPACE)/Build/$(PROJECT_PKG)/RELEASE_$(TOOL_CHAIN)/FV/VarFile; then rm -r $(WORKSPACE)/Build/$(PROJECT_PKG)/RELEASE_$(TOOL_CHAIN)/FV/VarFile; fi;

clean_debug_var:
	@if test -d $(WORKSPACE)/Build/$(PROJECT_PKG)/DEBUG_$(TOOL_CHAIN)/FV/VarFile; then rm -r $(WORKSPACE)/Build/$(PROJECT_PKG)/DEBUG_$(TOOL_CHAIN)/FV/VarFile; fi;

#
# Post-build processes
#
kernel_postbuild:
	@echo Dummy Kernel PostBuild
	@if test $(WORKSPACE)/Build/$(PROJECT_PKG)/BuildProject.dsc; then cp $(WORKSPACE)/Build/$(PROJECT_PKG)/BuildProject.dsc $(WORKSPACE)/Build/$(PROJECT_PKG)/Project.dsc -f; fi;
	@if test $(WORKSPACE)/Build/$(PROJECT_PKG)/BuildProject.fdf; then cp $(WORKSPACE)/Build/$(PROJECT_PKG)/BuildProject.fdf $(WORKSPACE)/Build/$(PROJECT_PKG)/Project.fdf -f; fi;

chipset_postbuild:
	@echo Dummy Chipset PostBuild

project_postbuild:
	@echo Dummy Project PostBuild

prebuild:  kernel_prebuild  chipset_prebuild  project_prebuild
postbuild: kernel_postbuild  chipset_postbuild  project_postbuild
