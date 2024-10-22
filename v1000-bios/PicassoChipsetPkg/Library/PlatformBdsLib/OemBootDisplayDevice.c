/** @file
  PlatformBdsLib

;******************************************************************************
;* Copyright (c) 2012 - 2015, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "OemBootDisplayDevice.h"


/**
  Oem Boot Display Device CallBack Function

  @param  SetupNVRam     
  @param  LegacyBios     

  @retval TRUE           There was a BIOS erro in the target code.
  @retval FALSE          Thunk completed, and there were no BIOS errors in the target code.
                         See Regs for status.

**/
EFI_STATUS
OemBootDisplayDeviceCallBack (
  IN  CHIPSET_CONFIGURATION     *SetupNVRam,
  IN  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios
  )
{
  return EFI_SUCCESS;
}
