#call CBS XML parse tool
CBS_LIB_FAMILY_PATH=Library/Family/0x17/RV/External
CBS_AGESA_PATH=$WORKSPACE/AgesaModulePkg/Include
if [ ! -d "$CBS_AGESA_PATH" ]; then
  CBS_AGESA_PATH=$WORKSPACE/AGESA/AgesaModulePkg/Include
fi
FIRMWARE_PATH=$WORKSPACE/AgesaPkg/Addendum/Apcb/Inc/RV
if [ ! -d "$FIRMWARE_PATH" ]; then
  FIRMWARE_PATH=$WORKSPACE/AGESA/AgesaPkg/Addendum/Apcb/Inc/RV
fi

if [ "$1" = "" ]; then
  #save previous AmdCbsVariable.h as base
  cp $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVFP5/AmdCbsVariable.h $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVFP5/AmdCbsVariable.h.Ori
  perl -I$WORKSPACE/AmdCbsPkg/Tools -I$WORKSPACE/AmdCbsPkg/Tools/Perl/lib $WORKSPACE/AmdCbsPkg/Tools/CBSgenerate.pl -i $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/Raven/eSetupRaven.xml -s $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/Raven/eRavenSetting.xml --apcbfile $FIRMWARE_PATH/APCB.h -o $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVFP5 --version external -b enable -t 1  --variable-file $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVFP5/AmdCbsVariable.h.Ori
  #Only copy files if they differ.  Prevents needless rebuild of files.
  if [ -f "$WORKSPACE/AmdCbsPkg/Include/Library/AmdCbsVariableRVFP5.h" ]; then
    DIFF=$(diff $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVFP5/AmdCbsVariable.h $WORKSPACE/AmdCbsPkg/Include/Library/AmdCbsVariableRVFP5.h)
    if [ "$DIFF" != "" ]; then
      cp $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVFP5/AmdCbsVariable.h $WORKSPACE/AmdCbsPkg/Include/Library/AmdCbsVariableRVFP5.h
    fi
  else
    cp $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVFP5/AmdCbsVariable.h $WORKSPACE/AmdCbsPkg/Include/Library/AmdCbsVariableRVFP5.h
  fi
  if [ -f "$CBS_AGESA_PATH/IdsNvDefRV.h" ]; then
    DIFF=$(diff $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVFP5/IdsNvDefRV.h $CBS_AGESA_PATH/IdsNvDefRV.h)
    if [ "$DIFF" != "" ]; then
      cp $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVFP5/IdsNvDefRV.h $CBS_AGESA_PATH/IdsNvDefRV.h
    fi
  else
    cp $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVFP5/IdsNvDefRV.h $CBS_AGESA_PATH/IdsNvDefRV.h
  fi
elif [ "$1" = "RVAM4" ]; then
  #save previous AmdCbsVariable.h as base
  cp $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVAM4/AmdCbsVariable.h $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVAM4/AmdCbsVariable.h.Ori
  perl -I$WORKSPACE/AmdCbsPkg/Tools -I$WORKSPACE/AmdCbsPkg/Tools/Perl/lib $WORKSPACE/AmdCbsPkg/Tools/CBSgenerate.pl -i $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/Raven/eSetupRavenAM4.xml -s $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/Raven/eRavenSetting.xml --apcbfile $FIRMWARE_PATH/APCB.h -o $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVAM4 --version external -b enable -t 1  --variable-file $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVAM4/AmdCbsVariable.h.Ori
  #Only copy files if they differ.  Prevents needless rebuild of files.
  if [ -f "$WORKSPACE/AmdCbsPkg/Include/Library/AmdCbsVariableRVAM4.h" ]; then
    DIFF=$(diff $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVAM4/AmdCbsVariable.h $WORKSPACE/AmdCbsPkg/Include/Library/AmdCbsVariableRVAM4.h)
    if [ "$DIFF" != "" ]; then
      cp $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVAM4/AmdCbsVariable.h $WORKSPACE/AmdCbsPkg/Include/Library/AmdCbsVariableRVAM4.h
    fi
  else
    cp $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVAM4/AmdCbsVariable.h $WORKSPACE/AmdCbsPkg/Include/Library/AmdCbsVariableRVAM4.h
  fi
  if [ -f "$CBS_AGESA_PATH/IdsNvDefRV.h" ]; then
    DIFF=$(diff $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVAM4/IdsNvDefRV.h $CBS_AGESA_PATH/IdsNvDefRV.h)
    if [ "$DIFF" != "" ]; then
      cp $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVAM4/IdsNvDefRV.h $CBS_AGESA_PATH/IdsNvDefRV.h
    fi
  else
    cp $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVAM4/IdsNvDefRV.h $CBS_AGESA_PATH/IdsNvDefRV.h
  fi
elif [ "$1" = "BILBY" ]; then
  echo "fgh"
  #save previous AmdCbsVariable.h as base
  cp $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVFP5/AmdCbsVariable.h $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVFP5/AmdCbsVariable.h.Ori
  perl -I$WORKSPACE/AmdCbsPkg/Tools -I$WORKSPACE/AmdCbsPkg/Tools/Perl/lib $WORKSPACE/AmdCbsPkg/Tools/CBSgenerate.pl -i $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/Raven/eSetupRavenBilby.xml -s $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/Raven/eRavenSetting.xml --apcbfile $FIRMWARE_PATH/APCB.h -o $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVFP5 --version external -b enable -t 1  --variable-file $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVFP5/AmdCbsVariable.h.Ori
  #Only copy files if they differ.  Prevents needless rebuild of files.
  if [ -f "$WORKSPACE/AmdCbsPkg/Include/Library/AmdCbsVariable.h" ]; then
    DIFF=$(diff $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVFP5/AmdCbsVariable.h $WORKSPACE/AmdCbsPkg/Include/Library/AmdCbsVariable.h)
    if [ "$DIFF" != "" ]; then
      cp $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVFP5/AmdCbsVariable.h $WORKSPACE/AmdCbsPkg/Include/Library/AmdCbsVariable.h
    fi
  else
    cp $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVFP5/AmdCbsVariable.h $WORKSPACE/AmdCbsPkg/Include/Library/AmdCbsVariable.h
  fi
  if [ -f "$WORKSPACE/AmdCbsPkg/Include/Library/AmdCbsVariableRVFP5.h" ]; then
    DIFF=$(diff $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVFP5/AmdCbsVariable.h $WORKSPACE/AmdCbsPkg/Include/Library/AmdCbsVariableRVFP5.h)
    if [ "$DIFF" != "" ]; then
      cp $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVFP5/AmdCbsVariable.h $WORKSPACE/AmdCbsPkg/Include/Library/AmdCbsVariableRVFP5.h
    fi
  else
    cp $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVFP5/AmdCbsVariable.h $WORKSPACE/AmdCbsPkg/Include/Library/AmdCbsVariableRVFP5.h
  fi
  if [ -f "$CBS_AGESA_PATH/IdsNvDefRV.h" ]; then
    DIFF=$(diff $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVFP5/IdsNvDefRV.h $CBS_AGESA_PATH/IdsNvDefRV.h)
    if [ "$DIFF" != "" ]; then
      cp $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVFP5/IdsNvDefRV.h $CBS_AGESA_PATH/IdsNvDefRV.h
    fi
  else
    cp $WORKSPACE/AmdCbsPkg/$CBS_LIB_FAMILY_PATH/ResourceRVFP5/IdsNvDefRV.h $CBS_AGESA_PATH/IdsNvDefRV.h
  fi
fi


