/** @file
  Provide hook function for OEM to initialize EC.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include <Library/EcLib.h>
#include <Library/BaseOemSvcKernelLib.h>

/**
  EC initialize.

  @retval   EFI_UNSUPPORTED       Returns unsupported by default.
  @retval   EFI_SUCCESS           The service is customized in the project.
  @retval   EFI_MEDIA_CHANGED     The value of IN OUT parameter is changed.
**/
EFI_STATUS
OemSvcEcInit (
  )
{
  if (!FeaturePcdGet (PcdLegacyFreeSupported)) {
    if (!FeaturePcdGet (PcdEmulationOnly)) {  //EMULATION_ONLY
      EcInit ();
    }  //EMULATION_ONLY
    return EFI_SUCCESS;
  } else {
    return EFI_UNSUPPORTED;
  }
}
