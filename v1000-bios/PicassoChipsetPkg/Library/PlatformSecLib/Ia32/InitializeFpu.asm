;;******************************************************************************
;;* Copyright (c) 2015, Insyde Software Corporation. All Rights Reserved.
;;*
;;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;;* transmit, broadcast, present, recite, release, license or otherwise exploit
;;* any part of this publication in any form, by any means, without the prior
;;* written permission of Insyde Software Corporation.
;;*
;;******************************************************************************


    .686
    .model  flat,C
    .const
;
; Float control word initial value: 
; all exceptions masked, double-precision, round-to-nearest
;
mFpuControlWord       DW      027Fh
;
; Multimedia-extensions control word:
; all exceptions masked, round-to-nearest, flush to zero for masked underflow
;
mMmxControlWord       DD      01F80h 
;
; Define BIT9.
;
;BIT9                  DD      00200h

    .xmm
    .code

;
; Initializes floating point units for requirement of UEFI specification.
;
; This function initializes floating-point control word to 0x027F (all exceptions
; masked,double-precision, round-to-nearest) and multimedia-extensions control word
; (if supported) to 0x1F80 (all exceptions masked, round-to-nearest, flush to zero
; for masked underflow).
;
InitializeFloatingPointUnits PROC PUBLIC

    push    ebx

    ;
    ; Initialize floating point units
    ;
    finit
    fldcw   mFpuControlWord
    
    ;
    ; Use CpuId instructuion (CPUID.01H:EDX.SSE[bit 25] = 1) to test
    ; whether the processor supports SSE instruction.
    ;
    mov     eax, 1
    cpuid
    bt      edx, 25
    jnc     Done
    
    ;
    ; Set OSFXSR bit 9 in CR4
    ;
    mov     eax, cr4
    or      eax, BIT9
    mov     cr4, eax
    
    ;
    ; The processor should support SSE instruction and we can use
    ; ldmxcsr instruction
    ;
    ldmxcsr mMmxControlWord
Done:
    pop     ebx

    ret

InitializeFloatingPointUnits ENDP

END
