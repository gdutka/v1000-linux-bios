#;*****************************************************************************
#;
#; Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
#;
#;******************************************************************************



[Defines]

[LibraryClasses.Common.PEIM]
  ## CCX
  CcxZenRvIdsHookLibPei|AgesaModulePkg/Library/CcxZenRvIdsHookLib/Pei/Internal/CcxZenRvIdsHookLibIntPei.inf
  ## DF
  FabricIdsHookRvLibPei|AgesaModulePkg/Library/FabricIdsHookRvLib/Pei/Internal/FabricIdsHookRvLibIntPei.inf

[LibraryClasses.Common.DXE_DRIVER]
  ## CCX
  CcxZenRvIdsHookLibDxe|AgesaModulePkg/Library/CcxZenRvIdsHookLib/Dxe/Internal/CcxZenRvIdsHookLibIntDxe.inf
  ## DF
  FabricIdsHookRvLibDxe|AgesaModulePkg/Library/FabricIdsHookRvLib/Dxe/Internal/FabricIdsHookRvLibIntDxe.inf

[LibraryClasses.Common.DXE_SMM_DRIVER]
  ## CCX
  CcxZenRvIdsHookLibSmm|AgesaModulePkg/Library/CcxZenRvIdsHookLib/Smm/Internal/CcxZenRvIdsHookLibIntSmm.inf

[LibraryClasses]
  NbioIdsHookRVLib|AgesaModulePkg/Library/NbioIdsHookRVLib/Internal/NbioIdsHookRVLibInt.inf
  SocCmnIdsHookRvLibPei|AgesaModulePkg/Library/SocCmnIdsHookRvLib/Pei/Internal/SocCmnIdsHookRvLibIntPei.inf
  SocCmnIdsHookRvLibDxe|AgesaModulePkg/Library/SocCmnIdsHookRvLib/Dxe/Internal/SocCmnIdsHookRvLibIntDxe.inf
  PspIdsHookLibDxe|AgesaModulePkg/Library/PspIdsHookLib/Dxe/Internal/PspIdsHookLibDxeInt.inf


[Components.IA32]

[Components.X64]


