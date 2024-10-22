;; @file
;  This file is the PostCodeFeatureHandlerEntry () implementation
;  It is Ia32 architecture implement
;
;******************************************************************************
;* Copyright (c) 2016 - 2017, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

.686p
.model  flat

EXTRN _PostCodeFeatureHandler:NEAR

.code

;---------------------------------------;
; PostCodeFeatureHandlerEntry           ;
;---------------------------------------;
;This entry will be registered to the debug exception entry of IDT by PostCodePei module
;And it will call PostCodeFeatureHandler () to perform PostCode feature 

; +---------------------+ 
; +    Old SS           +
; +---------------------+
; +    Old ESP          +
; +---------------------+
; +    EFlags           +
; +---------------------+
; +    CS               +
; +---------------------+
; +    EIP              +
; +---------------------+
; +    EBP              +
; +---------------------+<-- EBP
;
_PostCodeFeatureHandlerEntry PROC 

  push    ebp
  mov     ebp, esp

;Push scratch registers
;The processor will enter this exception when it writes PostCode to port 80
;And Eax is the PostCode data because of the Out instruction (ex. Out Dx,Eax)
  push    edx
  push    ecx
  push    dword ptr [ebp + 4]
  push    eax 

;Call PostCodeFeatureHandler ()
  call    _PostCodeFeatureHandler

;Pop scratch registers
  pop     eax
  add     esp, 4
  pop     ecx
  pop     edx

  pop     ebp
  
  iretd

_PostCodeFeatureHandlerEntry ENDP
END

