;; @file
;
;  Assembly code that supports IA32 for Thunk64To32Lib
;******************************************************************************
;* Copyright (c) 2012 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

SECTION .text
DEFAULT REL

;
; Data Types
;
%define UINT64      RESQ
%define UINT32      RESD
%define UINT16      RESW
%define UINT8       RESB
%define CHAR8       RESB

; 
;  Define what a processor GDT looks like
;   Keep in sync with Thunk.c
; 
struc    GDT_ENTRY
.LimitLow:    UINT16    1
.BaseLow:     UINT16    1
.BaseMid:     UINT8     1
.Attribute:   UINT8     1
.LimitHi:     UINT8     1
.BaseHi:      UINT8     1
endstruc

; 
; Define what a processor descriptor looks like
; Keep in sync with Thunk.c definition
; 
struc DESCRIPTOR64
.Limit:       UINT16      1
.Base:        UINT64      1
endstruc

struc DESCRIPTOR32
.Limit:       UINT16      1
.Base:        UINT32      1
endstruc

;
; LOW_MEMORY_THUNK - Keep in sync with Thunc.c definition
;
struc MEMORY_THUNK
.x64GdtDesc: ISTRUC     DESCRIPTOR64
AT DESCRIPTOR64.Limit, UINT16 1
AT DESCRIPTOR64.Base,  UINT64 1
IEND
.x64IdtDesc: ISTRUC     DESCRIPTOR64
AT DESCRIPTOR64.Limit, UINT16  1
AT DESCRIPTOR64.Base,  UINT64  1
IEND
.x64Ss:                 UINT64 1
.x64Esp:                UINT64 1
.ia32Stack:             UINT64 1
.ia32IdtDesc: ISTRUC    DESCRIPTOR32
AT DESCRIPTOR32.Limit,  UINT16 1
AT DESCRIPTOR32.Base,   UINT32 1
IEND
.ia32GdtDesc: ISTRUC    DESCRIPTOR32
AT DESCRIPTOR32.Limit,  UINT16 1
AT DESCRIPTOR32.Base,   UINT32 1
IEND
.CodeSeg32offset:       UINT16 1
endstruc

;  IN MEMORY_THUNK  *IntThunk,
;  IN UINT32        FunctionPoint
;  IN UINT32        PeiServicesPoint
;  );
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
global ASM_PFX(InternalThunk64To32)
ASM_PFX(InternalThunk64To32):

    push    ebp
    mov     ebp, esp
    push    ebx
    push    ecx
    push    edx
    push    edi
    push    esi

    mov     ecx,  [ebp + 8]      ;*IntThunk
    mov     edx,  [ebp + 12]     ;FunctionPoint
    mov     ebx, esp

    lea     eax, [ecx + MEMORY_THUNK.ia32Stack]
    mov     esp,  DWORD [eax]

    push    ebx

    mov     eax,  [ebp + 16]     ;PeiServicesPoint
    push    eax

    call    edx

    pop     eax

    pop     ebx
    mov     esp, ebx

    pop     esi
    pop     edi
    pop     edx
    pop     ecx
    pop     ebx
    pop     ebp

    ret

