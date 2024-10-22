;; @file
;;  This Function offers an interface for chipset code to call before "CallPeiCoreEntryPoint".

;;******************************************************************************
;;* Copyright (c) 2015, Insyde Software Corporation. All Rights Reserved.
;;*
;;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;;* transmit, broadcast, present, recite, release, license or otherwise exploit
;;* any part of this publication in any form, by any means, without the prior
;;* written permission of Insyde Software Corporation.
;;*
;;******************************************************************************

  SECTION .text

DEFAULT REL

;
; This Function offers an interface for chipset code to call before "CallPeiCoreEntryPoint".
;
; @param        None
;
; @retval           al = 3  (EFI_UNSUPPORTED)     Returns unsupported by default.
; @retval           al = 13 (EFI_MEDIA_CHANGED)   Alter the Configuration Parameter.
; @retval           al = 0  (EFI_SUCCESS)         The function performs the same operation as caller.
;                                                 The caller will skip the specified behavior and assuming
;                                                 that it has been handled completely by this function.
;OemSvcHookBeforePeiCoreEntryPoint  PROC  NEAR  PUBLIC
global ASM_PFX(OemSvcHookBeforePeiCoreEntryPoint)
ASM_PFX(OemSvcHookBeforePeiCoreEntryPoint):
  ;
  ; Add SEC customization code at here
  ;

  ;
  mov      al, 3                        ; Return value
  ret
;OemSvcHookBeforePeiCoreEntryPoint  ENDP


;END
