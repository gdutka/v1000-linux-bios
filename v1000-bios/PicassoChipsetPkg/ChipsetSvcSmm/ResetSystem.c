/** @file
 SMM Chipset Services Library.

 This file contains only one function that is SmmCsSvcResetSystem().
 The function SmmCsSvcResetSystem() use chipset services to reset system.
  
***************************************************************************
* Copyright (c) 2015, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************

*/

#include <Library/BaseLib.h>
#include <Library/IoLib.h>

/**
 Resets the system.

 @param[in]         ResetType           The type of reset to perform.

 @retval            EFI_UNSUPPORTED     ResetType is invalid.
 @retval            others              Error occurs
*/
EFI_STATUS
EFIAPI
ResetSystem (
  IN EFI_RESET_TYPE   ResetType
  )
{
  UINT8               InitialData;
  UINT8               OutputData;

  switch(ResetType) {

  case EfiResetWarm:
  case EfiResetCold:
    InitialData  = 0x02;
    OutputData   = 0x06;
    break;

  default:
    return EFI_UNSUPPORTED;
  }

  IoWrite8 (0xCF9, InitialData);
  IoWrite8 (0xCF9, OutputData);
  
  //
  // Given we should have reset getting here would be bad
  //
  CpuDeadLoop ();
  return EFI_DEVICE_ERROR;
}