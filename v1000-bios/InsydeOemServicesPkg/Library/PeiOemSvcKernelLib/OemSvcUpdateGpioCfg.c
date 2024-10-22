/** @file
  Update GPIO configuration.

;******************************************************************************
;* Copyright (c) 2016, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/PeiOemSvcKernelLib.h>

/**
  This optional function updates the hardware configuration of a single pad(GPIO).
  If the configuration is updated, this function should returned EFI_MEDIA_SUCCESS.
  If the configuration is not updated, then this function should return EFI_UNSUPPORTED.
  If the GPIO should not be programmed by the kernel, this function should return EFI_SUCCESS.

  @param[in, out]  *GpioCfgData       On entry, a point to structure that specifies the build-time configuration for the GPIO. 
                                      On exit, a point to an updated structure that indicates the run-time configuration for the GPIO.
                                      Note: This pointer actually has the type H2O_GPIO_INIT_STRUCT,
                                            but this type cannot be used in the OEM Services header file because it would introduce a dependency on the chipset.

  @retval          EFI_UNSUPPORTED    The GPIO configuration data has not been changed.
  @retval          EFI_SUCCESS        The GPIO configuration should not be updated by the kernel.
  @retval          EFI_MEDIA_CHANGED  The GPIO configuration data has been changed.
**/
EFI_STATUS
OemSvcUpdateGpioCfg (
  IN OUT VOID                                *GpioCfgData
  )
{
  
  return EFI_UNSUPPORTED;
}

