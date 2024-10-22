/** @file
 DXE Chipset Services Library.

 This file contains only one function that is DxeCsSvcSetIsaInfo().
 The function DxeCsSvcSetIsaInfo() use chipset services to set ISA
 information for common ISA driver utilization.

***************************************************************************
* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Fch.h>

/**
 To set ISA information for common ISA driver utilization.

 @param[in, out]    Function            ISA function value
 @param[in, out]    Device              ISA device value

 @retval            EFI_SUCCESS         This function always return successfully.
*/
EFI_STATUS
SetIsaInfo (
  IN OUT UINT8    *Function,
  IN OUT UINT8    *Device
  )
{
  *Function = (UINT8)FCH_LPC_FUNC;
  *Device   = (UINT8)FCH_LPC_DEV;

  return EFI_SUCCESS;
}