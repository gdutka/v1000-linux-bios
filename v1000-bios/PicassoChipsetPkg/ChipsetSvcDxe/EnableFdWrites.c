/** @file
 DXE Chipset Services Library.
 	
 This file contains only one function that is DxeCsSvcEnableFdWrites().
 The function DxeCsSvcEnableFdWrites() use chipset services to enable/disable 
 flash device write access.
	
***************************************************************************
* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
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
#include <Library/PciCf8Lib.h>
#include <Protocol/SmmFwBlockService.h>

UINT8  mFlashMode = SMM_FW_DEFAULT_MODE;

/**
 Platform specific function to enable/disable flash device write access.

 @param[in]         EnableWrites        TRUE  - Enable
                                        FALSE - Disable

 @retval            EFI_SUCCESS         Function always returns successfully
*/
EFI_STATUS
EnableFdWrites (
  IN  BOOLEAN           EnableWrites
  )
{
  return EFI_SUCCESS;
}