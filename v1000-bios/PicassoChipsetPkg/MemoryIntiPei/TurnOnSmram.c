/** @file
 Help function for MemoryInitPei to config T-seg address and size. 
 
***************************************************************************
* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include "InstallPeiMemoryInternal.h"

/**
 Config T-seg address and size.

 @param[in]         PeiServices         Describes the list of possible PEI Services.
 @param[in]         SmramSize           Handle of the file being invoked.

 @retval            None
*/
VOID
TurnOnSmram  (
  IN CONST EFI_PEI_SERVICES                   **PeiServices,
  IN       PEI_SMRAM_SIZE_TYPE                SmramSize
  )
{
  EFI_HOB_GUID_TYPE             *GuidHob;
  EFI_UMA_EXIST_HOB             *UmaExistHob;
  UINT64                        TopMem;
  UINT64                        TsegAddress;
  UINT64                        TsegMask;

  GuidHob     = NULL;
  UmaExistHob = NULL;
  GuidHob = GetFirstGuidHob (&gUmaExistHobGuid);
  if (GuidHob != NULL) {
    UmaExistHob = GET_GUID_HOB_DATA (GuidHob);
  }

  TopMem      = AsmReadMsr64 (TOP_MEM);
  TsegAddress = 0;
  TsegMask    = 0x0000010000000000;

  switch (SmramSize) {

  case SmramSize128K:
    TsegAddress = TopMem - (128 * 1024);
    TsegMask -= (128 * 1024);
    break;

  case SmramSize256K:
    TsegAddress = TopMem - (256 * 1024);
    TsegMask -= (256 * 1024);
    break;

  case SmramSize512K:
    TsegAddress = TopMem - (512 * 1024);
    TsegMask -= (512 * 1024);
    break;

  case SmramSize1024K:
    TsegAddress = TopMem - (1024 * 1024);
    TsegMask -= (1024 * 1024);
    break;

  case SmramSize2048K:
    TsegAddress = TopMem - (2048 * 1024);
    TsegMask -= (2048 * 1024);
    break;

  case SmramSize4096K:
    TsegAddress = TopMem - (4096 * 1024);
    TsegMask -= (4096 * 1024);
    break;

  case SmramSize8192K:
    TsegAddress = TopMem - (8192 * 1024);
    TsegMask -= (8192 * 1024);
    break;
//[-start-240426-IB18160212-add-]//
  case SmramSize0:
    break;
//[-start-240426-IB18160212-add-]//
  }

  //
  // Check UMA setting to adject T-seg
  //
  if (UmaExistHob->UmaExistFlag & UMA_ENABLE) {
    if (FeaturePcdGet (PcdUmaBaseUpperThan4G)) {
      if (UmaExistHob->UmaExistFlag & UMA_UNDER_4G) { //UMA is under 4G
        TsegAddress -= UmaExistHob->UmaSize;
      }
    } else {
      TsegAddress -= UmaExistHob->UmaSize;
    }
  }

  //
  // Turn On Smram
  //
  TsegMask |= 0x4403;  //SMM WT
  AsmWriteMsr64 (0xC0010112, TsegAddress);
  AsmWriteMsr64 (0xC0010113, TsegMask);

  //
  // Make sure Memory 0~640K MTRR is 1E to get sync with AP
  //
  AsmWriteMsr64 (SYS_CFG, (AsmReadMsr64 (SYS_CFG) | (1 << 19)));
  AsmWriteMsr64 (AMD_AP_MTRR_FIX64k_00000, 0x1E1E1E1E1E1E1E1E);
  AsmWriteMsr64 (AMD_AP_MTRR_FIX16k_80000, 0x1E1E1E1E1E1E1E1E);
  AsmWriteMsr64 (SYS_CFG, (AsmReadMsr64 (SYS_CFG) & (~(1 << 19))));
}
