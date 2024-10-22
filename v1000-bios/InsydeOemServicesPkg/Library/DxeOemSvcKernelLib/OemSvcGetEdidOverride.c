/** @file
  Provide OEM to customize EdidOverride. .

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/DxeOemSvcKernelLib.h>

/**
  Get OEM customization EdidOverride. 
  Customization EdidOverride provides OEM to define EdidOverride.

  @param[out]  *GetEdidOverride            A pointer to OEM_GET_EDID_OVERRIDE_DEFINITION

  @retval      EFI_UNSUPPORTED             Returns unsupported by default.
  @retval      EFI_SUCCESS                 Get EdidOverride information successful.
  @retval      EFI_MEDIA_CHANGED           The value of IN OUT parameter is changed. 
  @retval      Others                      Base on OEM design.
**/
EFI_STATUS
OemSvcGetEdidOverride (
  IN OUT OEM_GET_EDID_OVERRIDE_DEFINITION *GetEdidOverride
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}

