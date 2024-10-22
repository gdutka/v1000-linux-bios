;; @file
;
;  Assembly code that supports x64 for Thunk64To32Lib
;******************************************************************************
;* Copyright (c) 2015 - 2018, Insyde Software Corporation. All Rights Reserved.
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

; RCX - MEMORY_THUNK      *IntThunk
; RDX - UINT32            CodePoint
; R8  - UINT32            PeiServicesPoint
global ASM_PFX(InternalThunk64To32)
ASM_PFX(InternalThunk64To32):

    pushf                         ; Save flags (note that this is before the cli)
    cli

    push    rsp
    push    rdi
    push    rsi
    push    rbp
    push    rax
    push    rbx
    push    rcx
    push    rdx
    push    r8
    push    r9
    push    r10
    push    r11
    push    r12
    push    r13
    push    r14
    push    r15

;
; Save PeiServicesPoint in r9
;
    mov     r9, r8
;
; Save IntThunk and CodePoint
;
    mov     r8, rcx
    mov     rdi, rdx

    mov     ax, [r8 + MEMORY_THUNK.CodeSeg32offset]
    mov     word[CodeSeg32offset], ax

    sgdt    [r8 + MEMORY_THUNK.x64GdtDesc]
    sidt    [r8 + MEMORY_THUNK.x64IdtDesc]

    xor     rax, rax                                  ; zero RAX
    mov     ax, ss                                    ; read in the stack segment
    mov     [r8 + MEMORY_THUNK.x64Ss], rax        ; save in data structure

;
; Save flat x64 stack location (RSP).
;
    mov     [r8 + MEMORY_THUNK.x64Esp], rsp

;
; patch in the current IntThunkAddr2.
;
    mov     rax, r8
    mov     qword [IntThunkAddr2], rax  ; 8 bytes at IntThunkAddr2 = current address of IntThunk.

;
; Poke the TrampoleanCr3 value into the code.
;
    mov     rax, cr3
    mov     dword[TrampoleanCr3], eax

;
; patch in the current ds, fs, gs and cs, so we can use it to return to x64 mode.
;
    xor     rax, rax
    mov     ax, ds
    mov     qword [PatchX64Ds1], rax

    mov     ax, fs
    mov     qword [PatchX64Fs1], rax

    mov     ax, gs
    mov     qword [PatchX64Gs1], rax

    mov     ax, cs
    mov     word [PatchLongModeCS], ax

;
;   patch the address of OffsetInLongMode.
;
    mov     rax, InLongMode
    mov     dword[OffsetInLongMode], eax

;
;   patch the address of CompatibilityModeVector.
;
    mov     rax, CompatibilityMode
    mov     dword[CompatibilityModeVector], eax

    mov     rsi, r8

;
;   PeiServicesPoint store in rdx
;
    mov     rdx, r9

;
;   jmp to x64 Compatibility Mode.
;
    mov     rax, CompatibilityModeVector

;    jmp    FWORD PTR[rax]
    DB      0xFF
    DB      0x28

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;     now in 64-bit compatibility mode           ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
CompatibilityMode:

;
; disable paging
;
    mov    rax, cr0
    btr    eax, 31             ; set PG=0
    mov    cr0, rax

;
;  set EFER.LME = 0 to leave long mode
;
    push    rdx
    mov     ecx, 0xc0000080    ; EFER MSR number.
    rdmsr                      ; Read EFER.
    btr     eax, 8             ; Set LME=0.
    wrmsr                      ; Write EFER.
    pop     rdx
    jmp     Legacy32bitProtectedMode
Legacy32bitProtectedMode:

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;     now in 32-bit legacy mode                  ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;
; This is the Trapolean Page Tables that are guarenteed
;  under 4GB.
;
    xor     eax, eax
    DB      0xb8                ; mov eax, imm32
global mIA32Cr3
mIA32Cr3: DD 0

    mov     cr3, rax

;
; Get the IA32 mode stack
;
    lea     rcx, [abs 0 + MEMORY_THUNK.ia32Stack]
    add     rcx, rsi

    DB      0x8b                 ; mov  ebx, DWORD PTR [ecx]
    DB      0x19

    mov     esp,ebx             ;set stack

;
; Enable paging to activate IA32 mode (set CR0.PG=1)
;
    mov   rax, cr0              ; Read CR0.
    bts   eax, 31               ; Set PG=1.
    mov   cr0, rax              ; Write CR0.

;
;   Prepare parameter : PeiServicesPoint
;
    DB      0x52                ; 'push edx' opcode
    call    rdi
    DB      0x5A                ; 'pop edx' opcode

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;
; Go Long Mode HERE!!!!!
;

;
; disable paging
;
    mov    rax, cr0
    btr    eax, 31              ; set PG=0
    mov    cr0, rax

;
; Switch back to the original page tables
;
;;; mov eax, imm32
    DB      0xB8                ; 'mov eax,imm32' opcode
TrampoleanCr3:
    DD  00000000                ; imm32
    mov cr3, rax
    ;
    ; Enable the 64-bit page-translation-table entries by
    ; setting CR4.PAE=1 (this is _required_ before activating
    ; long mode). Paging is not enabled until after long mode
    ; is enabled.
    ;
    mov rax, cr4
    bts eax, 5
    mov cr4, rax

    ;
    ; Enable long mode (set EFER.LME=1).
    ;
    mov   ecx, 0xc0000080       ; EFER MSR number.
    rdmsr                       ; Read EFER.
    bts   eax, 8                ; Set LME=1.
    wrmsr                       ; Write EFER.

    ;
    ; Enable paging to activate long mode (set CR0.PG=1)
    ;
    mov   rax, cr0              ; Read CR0.
    bts   eax, 31               ; Set PG=1.
    mov   cr0, rax              ; Write CR0.

    ;
    ; This is the next instruction after enabling paging.  Jump to long mode
    ;
    db      0x67
    db      0xea                ; Far Jump $+9:Selector to reload CS
OffsetInLongMode:
    dd      00000009            ;   $+9 Offset is ensuing instruction boundary
PatchLongModeCS:
    dw      0x0               ;   Selector is our code selector, 10h
InLongMode:

    DB      0x48                ; mov ax, imm16
    DB      0xB8
PatchX64Ds1:
    DQ      0x0
    mov     es, ax
    mov     ss, ax
    mov     ds, ax

; restore fs and gs
    DB      0x48                ; mov rax, imm16
    DB      0xB8
PatchX64Fs1:
    DQ      0x0
    mov     fs, ax

    DB      0x48                ; mov rax, imm16
    DB      0xB8
PatchX64Gs1:
    DQ      0x0
    mov     gs, ax

    DB      0x48
    DB      0xBE                ; 'mov rsi,imm64' opcode (0xB8 + 6)
IntThunkAddr2:
    DQ      0x0   ; address of IntThunk (imm64)

;
; Restore 64-bit stack
;
    mov     rcx, [rsi + MEMORY_THUNK.x64Ss]
    mov     ss, cx

    mov     rsp, [rsi + MEMORY_THUNK.x64Esp]

;
; Restore C regs
;
    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdx
    pop     rcx
    pop     rbx
    pop     rax
    pop     rbp
    pop     rsi
    pop     rdi
    pop     rsp
    popf

    ret

    align    16
CompatibilityModeVector:
    DD    0
CodeSeg32offset:
    DW    0

