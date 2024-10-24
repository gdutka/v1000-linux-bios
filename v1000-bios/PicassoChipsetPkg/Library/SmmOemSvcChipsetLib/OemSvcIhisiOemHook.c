/** @file
 This function offers an interface that supply for OEM hook before invoking each IHISI Subfunction.

;******************************************************************************
;* Copyright (c) 2015, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/SmmOemSvcChipsetLib.h>

/**
 This function provides a hook before invoking each IHISI Subfunction.
 Please refer to Insyde H2O internal Soft-SMI interface (IHISI) Specification.

 @param[in]         Cmd                 IHISI Sub-function number.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcIhisiOemHook (
  IN UINTN          Cmd
  )
{
/*++
  Todo:
  Add project specific code in here.
--*/

  return EFI_UNSUPPORTED;
}
