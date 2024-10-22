/** @file
  OemSvc function to run code after all PCI device ready

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

#include <Library/DxeOemSvcChipsetLib.h>

/**
  OemSvc function to run code after all PCI device ready

  @retval EFI_UNSUPPORTED    Returns unsupported by default.
  @retval EFI_SUCCESS        The service is customized in the project.
  @retval EFI_MEDIA_CHANGED  The value of IN OUT parameter is changed. 
  @retval Others             Depends on customization.
**/
EFI_STATUS
OemSvcDxePlatformAfterAllPciInitFinished (
  VOID
  )
{
  return EFI_UNSUPPORTED;
}
