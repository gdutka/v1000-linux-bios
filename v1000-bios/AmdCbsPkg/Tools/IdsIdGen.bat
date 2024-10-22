@REM call Ids Id headerfile script tool
@IF EXIST %WORKSPACE%\AGESA\AgesaModulePkg\Include (
  @SET AGESA_INC_PATH=%WORKSPACE%\AGESA\AgesaModulePkg\Include
) ELSE (
  @SET AGESA_INC_PATH=%WORKSPACE%\AgesaModulePkg\Include
)
@REM [-start-231019-IB18160162-modify]
 %WORKSPACE%\AmdCbsPkg\Tools\IdsIdGen.exe -i %WORKSPACE%\AmdCbsPkg\Library -o %WORKSPACE%\AmdCbsPkg\Build
@REM [-end-231019-IB18160162-modify]
xcopy %WORKSPACE%\AmdCbsPkg\Build\*.*      %AGESA_INC_PATH% /S /Y
