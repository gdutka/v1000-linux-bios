#!/bin/bash

# cp eSetupRavenBilby.xml
cp -r "$WORKSPACE/$PROJECT_PKG/AmdCbsCustomization/Raven/eSetupRavenBilby.xml" "$WORKSPACE/AmdCbsPkg/Library/Family/0x17/RV/External/Raven/"

# cp AmdCbsDefault.c
cp -r "$WORKSPACE/$PROJECT_PKG/AmdCbsCustomization/ResourceRVFP5/AmdCbsDefault.c" "$WORKSPACE/AmdCbsPkg/Library/Family/0x17/RV/External/ResourceRVFP5/"

# cp AmdCbsForm.vfr
cp -r "$WORKSPACE/$PROJECT_PKG/AmdCbsCustomization/ResourceRVFP5/AmdCbsForm.vfr" "$WORKSPACE/AmdCbsPkg/Library/Family/0x17/RV/External/ResourceRVFP5/"
