;** @file
;
;  IDT vector entry.
;
;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

    .586p
    .model  flat,C
    .code

;------------------------------------------------------------------------------
;  Generic IDT Vector Handlers for the Host. 
;
;------------------------------------------------------------------------------

ALIGN   8
PUBLIC  AsmGetVectorTemplatInfo


@VectorTemplateBase:
REPEAT  34
        db    6ah       ; push #VectorNumber
        db    ($ - @VectorTemplateBase) / ((@VectorTemplateEnd - @VectorTemplateBase) / 34) ; VectorNum
        push  eax
        mov   eax, SecCommonInterruptEntry
        jmp   eax
ENDM
@VectorTemplateEnd:


AsmGetVectorTemplatInfo PROC
        mov   ecx, [esp + 4]
        mov   [ecx], @VectorTemplateBase
        mov   eax, (@VectorTemplateEnd - @VectorTemplateBase) / 34
        ret
AsmGetVectorTemplatInfo ENDP


;---------------------------------------;
; SecCommonInterruptEntry               ;
;---------------------------------------;
; The follow algorithm is used for the common interrupt routine.

; Stack:
; +---------------------+
; +    EFlags           +
; +---------------------+
; +    CS               +
; +---------------------+
; +    EIP              +
; +---------------------+
; +    Error Code       +
; +---------------------+
; +    Vector Number    +
; +---------------------+

SecCommonInterruptEntry PROC
  cli
  pop    eax
  jmp $
SecCommonInterruptEntry ENDP

END