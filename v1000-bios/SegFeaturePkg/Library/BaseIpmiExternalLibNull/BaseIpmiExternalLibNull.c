/** @file
 Base IPMI external library implement code.

;******************************************************************************
;* Copyright (c) 2019, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/
#include <Library/BaseLib.h>
#include <Library/BaseIpmiExternalLib.h>

/**
 This function can get power policy that need to be set
 Chipset should call this function before setting StateAfterG3 to PCH

 @param[in out]       SetAfterG3On      in : Current caller's power policy.
                                        out: Return the power policy that need to be set.

 @retval EFI_UNSUPPORTED                Null libraey always return unsupport.
*/
EFI_STATUS
IpmiExternalLibGetPowerPolicy (
  IN OUT BOOLEAN        *SetAfterG3On
  )
{
  return EFI_UNSUPPORTED;
}

