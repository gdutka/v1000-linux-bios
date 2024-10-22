/** @file
 IPMI external library header file.

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

#ifndef _IPMI_EXTERNAL_SUPPORT_LIB_H_
#define _IPMI_EXTERNAL_SUPPORT_LIB_H_

#include <Uefi/UefiBaseType.h>

/**
 This function can set power policy to BMC  

 @param[in]         PowerPolicy         Power policy.

 @retval EFI_SUCCESS                    Set power policy to BMC success.
 @return EFI_ERROR (Status)             Set power policy to BMC fail.
*/
EFI_STATUS
IpmiExternalLibGetPowerPolicy (
  IN OUT BOOLEAN        *SetAfterG3On
  );

#endif

