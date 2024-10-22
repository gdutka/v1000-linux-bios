/** @file
 DXE Chipset Services driver.

 This file contains only one function that is FvRecoveryInfo().
 The function FvRecoveryInfo() provide information of recovery FV.

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

#include <FastRecoveryData.h>

//static PEI_FV_DEFINITION mPeiFvs[] =
//{
//  { FixedPcdGet32(PcdFlashFvRecoveryBase),
//    FixedPcdGet32(PcdFlashFvRecoverySize)
//  }
//};
//
//static EFI_GUID mChipsetEmuPeiMaskTable[] =
//{
//  //
//  // End of list
//  //
//  {0xFFFFFFFF, 0xFFFF, 0xFFFF, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}}
//};

/**
 Get information of recovery FV.

 @param[out]        RecoveryRegion        recovery region table
 @param[out]        NumberOfRegion        number of recovery region
 @param[out]        ChipsetEmuPeiMaskList Specified PEIMs don't to dispatch in recovery mode

 @retval            EFI_SUCCESS           This function always return success.
**/
EFI_STATUS
FvRecoveryInfo (
  OUT  PEI_FV_DEFINITION       **RecoveryRegion,
  OUT  UINTN                    *NumberOfRegion,
  OUT  EFI_GUID                **ChipsetEmuPeiMaskList
  )
{
//  *RecoveryRegion = mPeiFvs;
//  *NumberOfRegion = sizeof(mPeiFvs) / sizeof(PEI_FV_DEFINITION);
//  *ChipsetEmuPeiMaskList = mChipsetEmuPeiMaskTable;

  return EFI_UNSUPPORTED;
}
