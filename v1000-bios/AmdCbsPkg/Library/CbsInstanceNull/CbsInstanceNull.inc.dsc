#;*****************************************************************************
#;
#; Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
#;
#;******************************************************************************

[LibraryClasses.Common.PEIM]
  CbsPeiFuncLib|AmdCbsPkg/Library/CbsFuncLibNull/CbsFuncLibPei.inf

[LibraryClasses.Common.DXE_DRIVER]
  CbsSetupLib|AmdCbsPkg/Library/CbsSetupLibNull/CbsSetupLib.inf
  CbsDxeFuncLib|AmdCbsPkg/Library/CbsFuncLibNull/CbsFuncLibDxe.inf

[Components.IA32]
  AmdCbsPkg/CbsBasePei/CbsBasePei.inf {
    <LibraryClasses>
       CbsPeiFuncLib|AmdCbsPkg/Library/CbsFuncLibNull/CbsFuncLibPei.inf
  }

[Components.X64]
  AmdCbsPkg/CbsSetupDxe/CbsSetupDxe.inf {
    <LibraryClasses>
       CbsSetupLib|AmdCbsPkg/Library/CbsSetupLibNull/CbsSetupLib.inf
       CbsDxeFuncLib|AmdCbsPkg/Library/CbsFuncLibNull/CbsFuncLibDxe.inf
  }
  AmdCbsPkg/CbsBaseDxe/CbsBaseDxe.inf {
    <LibraryClasses>
       CbsDxeFuncLib|AmdCbsPkg/Library/CbsFuncLibNull/CbsFuncLibDxe.inf
  }

