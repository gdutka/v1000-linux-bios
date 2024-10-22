/** @file
  The instance of SmmCorePlatformHookLib Library

;******************************************************************************
;* Copyright (c) 2016 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/SmmCorePlatformHookLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Protocol/SmmDebugRegisterConfig.h>

SMM_DEBUG_REGISTER_CONFIG_PROTOCOL *mSmmDebugRegisterConfig = NULL;

/**
  Performs platform specific tasks before invoking registered SMI handlers.
  
  The debug registers DR6 and DR7 will be cleaned when processor enters SMM.
  If we want to trap port 80 in SMM, it needs to configure debug registers every time 
  processor enters SMM.
  
  @retval EFI_SUCCESS    The platform hook completes successfully.
  @retval Other values   The paltform hook cannot complete due to some error.

**/
EFI_STATUS
EFIAPI
PlatformHookBeforeSmmDispatch (
  VOID
  )
{
  EFI_STATUS Status;

  //
  // Locate Debug Register Configure protocol which is provided by PostCodeSmm driver
  //
  if (mSmmDebugRegisterConfig == NULL) {
    Status = gSmst->SmmLocateProtocol (
                      &gH2OPostCodeSmmDebugRegProtocolGuid,
                      NULL,
                      &mSmmDebugRegisterConfig
                      );
    if(EFI_ERROR(Status)) {
      return Status;
    }
  }

  //
  // Configure the debug registers to trap port 80
  // It may not configure the debug registers when PostCodeSmm driver disable the PostCode feature 
  //
  mSmmDebugRegisterConfig->ConfigDebugRegister ();

  return EFI_SUCCESS;
}


/**
  Performs platform specific tasks after invoking registered SMI handlers.
  
  This function performs platform specific tasks after invoking registered SMI handlers.
  
  @retval EFI_SUCCESS    The platform hook completes successfully.
  @retval Other values   The paltform hook cannot complete due to some error.

**/
EFI_STATUS
EFIAPI
PlatformHookAfterSmmDispatch (
  VOID
  )
{
  return EFI_SUCCESS;
}
