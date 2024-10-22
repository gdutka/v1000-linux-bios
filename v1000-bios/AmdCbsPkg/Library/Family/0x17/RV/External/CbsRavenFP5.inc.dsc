#;*****************************************************************************
#;
#; Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
#;
#;******************************************************************************

[Defines]

[LibraryClasses]

[LibraryClasses.Common.PEIM]
  CbsPeiFuncLib|AmdCbsPkg/Library/Family/0x17/RV/External/CbsFuncLibPeiFP5.inf
  AmdIdsHookExtLib|AmdCbsPkg/Library/Family/0x17/RV/External/CbsIdsLibPeiFP5.inf
  CbsSetAgesaPcdLib|AmdCbsPkg/Library/Family/0x17/RV/External/ResourceRVFP5/CbsSetAgesaPcdLibRV.inf

[LibraryClasses.Common.DXE_DRIVER]
  CbsSetupLib|AmdCbsPkg/Library/Family/0x17/RV/External/CbsSetupLibFP5.inf
  CbsDxeFuncLib|AmdCbsPkg/Library/Family/0x17/RV/External/CbsFuncLibDxeFP5.inf
  AmdIdsHookExtLib|AmdCbsPkg/Library/Family/0x17/RV/External/CbsIdsLibDxeFP5.inf

[LibraryClasses.Common.DXE_SMM_DRIVER]
  CbsBctSmmLib|AmdCbsPkg/Library/Family/0x17/RV/External/CbsBctSmmLibFP5.inf

[Components.IA32]
  AmdCbsPkg/CbsBasePei/CbsBasePeiRV.inf {
    <LibraryClasses>
       CbsPeiFuncLib|AmdCbsPkg/Library/Family/0x17/RV/External/CbsFuncLibPeiFP5.inf
       CbsSetAgesaPcdLib|AmdCbsPkg/Library/Family/0x17/RV/External/ResourceRVFP5/CbsSetAgesaPcdLibRV.inf
  }

[Components.X64]
  AmdCbsPkg/BctBaseSmm/BctBaseSmmRV.inf {
    <LibraryClasses>
       CbsBctSmmLib|AmdCbsPkg/Library/Family/0x17/RV/External/CbsBctSmmLibFP5.inf
  }
  AmdCbsPkg/CbsSetupDxe/CbsSetupDxeRV.inf {
    <LibraryClasses>
       CbsSetupLib|AmdCbsPkg/Library/Family/0x17/RV/External/CbsSetupLibFP5.inf
       CbsDxeFuncLib|AmdCbsPkg/Library/Family/0x17/RV/External/CbsFuncLibDxeFP5.inf
  }
  AmdCbsPkg/CbsBaseDxe/CbsBaseDxeRV.inf {
    <LibraryClasses>
       CbsDxeFuncLib|AmdCbsPkg/Library/Family/0x17/RV/External/CbsFuncLibDxeFP5.inf
  }

