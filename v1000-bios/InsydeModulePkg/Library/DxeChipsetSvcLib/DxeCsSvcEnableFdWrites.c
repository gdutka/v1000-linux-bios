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

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Protocol/H2ODxeChipsetServices.h>

//
// Global Variables (This Source File Only)
//
extern H2O_CHIPSET_SERVICES_PROTOCOL *mChipsetSvc;

/**
 Platform specific function to enable/disable flash device write access 

 @param[in]         EnableWrites        TRUE  - Enable
                                        FALSE - Disable
                    
 @retval            EFI_SUCCESS         Function returns successfully
 @retval            EFI_UNSUPPORTED     If the function is not implemented.
*/
EFI_STATUS
DxeCsSvcEnableFdWrites (
  IN  BOOLEAN                  EnableWrites
  )
{
  //
  // Verify that the protocol interface structure contains the function 
  // pointer and whether that function pointer is non-NULL. If not, return
  // an error.
  //
  if (mChipsetSvc == NULL ||
      mChipsetSvc->Size < (OFFSET_OF (H2O_CHIPSET_SERVICES_PROTOCOL, EnableFdWrites) + sizeof (VOID*)) || 
      mChipsetSvc->EnableFdWrites == NULL) {
    DEBUG ((EFI_D_ERROR, "H2O DXE Chipset Services can not be found or member EnableFdWrites() isn't implement!\n"));
    return EFI_UNSUPPORTED;
  }
  return mChipsetSvc->EnableFdWrites (EnableWrites);   
}  
