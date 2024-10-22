;;******************************************************************************
;;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
;;*
;;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;;* transmit, broadcast, present, recite, release, license or otherwise exploit
;;* any part of this publication in any form, by any means, without the prior
;;* written permission of Insyde Software Corporation.
;;*
;;******************************************************************************

  .XLIST
  INCLUDE AmdUefiStack.inc
  .LIST

  .586p
  .XMM
  .MODEL FLAT, STDCALL
  .CODE

;======================================================================
; VOID
; AmdDisableCarBx (
;   );
;======================================================================
AmdDisableCarBx PROC NEAR C PUBLIC
    pushad
    mov     ebx, esp
    AMD_DISABLE_UEFI_STACK2
    mov     esp, ebx
    popad
    ret
AmdDisableCarBx ENDP

END
