#call Ids Id headerfile script tool
if [ -d "$WORKSPACE/AGESA/AgesaModulePkg/Include" ]; then
  AGESA_INC_PATH=$WORKSPACE/AGESA/AgesaModulePkg/Include
else
  AGESA_INC_PATH=$WORKSPACE/AgesaModulePkg/Include
fi

perl -I$WORKSPACE/AmdCbsPkg/Tools -I$WORKSPACE/AmdCbsPkg/Tools/Perl/lib $WORKSPACE/AmdCbsPkg/Tools/IdsIdGen.pl -i $WORKSPACE/AmdCbsPkg/Library -o $WORKSPACE/AmdCbsPkg/Build
cp -Rf $WORKSPACE/AmdCbsPkg/Build/*.*      $AGESA_INC_PATH
