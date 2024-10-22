/** @file
  Provide a hook OEM services function for IHISI 52H setup callback function.

;******************************************************************************
;* Copyright (c) 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/SmmOemSvcKernelLib.h>

/**
 This function offers an interface to Hook IHISI Sub function AH=52h,function "SetupCallbackSyncCallbackIhisi"

 @param[in, out]    VarBuffer           On entry, pointer to Variable data buffer.
                                        On exit, points to updated Variable data buffer.
 @param[in]         CallbackList        Pointer to Callback List data buffer.
 @param[in, out]    ResultArray         On entry, pointer to Result data buffer.
                                        On exit, points to updated Result data buffer.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcIhisiS52HookSetupCallbackSyncCallbackIhisi (
  IN OUT VATS_VARIABLE_BUFFER_STRUCTURE *VarBuffer,
  IN VATS_CALLBACK_LIST_STRUCTURE       *CallbackList,
  IN OUT VATS_RESULT_LIST_STRUCTURE     *ResultArray
  )
{
/*++
  Todo:
  Add project specific code in here.
--*/

  return EFI_UNSUPPORTED;
}

