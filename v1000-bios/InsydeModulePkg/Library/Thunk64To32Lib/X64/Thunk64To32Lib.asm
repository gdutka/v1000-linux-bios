;; @file
; 
;  Assembly code that supports x64 for Thunk64To32Lib
;******************************************************************************
;* Copyright (c) 2015, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

include Thunk.inc

text SEGMENT

EXTERNDEF   mIA32Cr3:DWORD

.code

; RCX - MEMORY_THUNK      *IntThunk
; RDX - UINT32            CodePoint
; R8  - UINT32            PeiServicesPoint
InternalThunk64To32  PROC

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

    mov     ax, ( MEMORY_THUNK PTR [r8] ).CodeSeg32offset
    mov     word  ptr[CodeSeg32offset], ax

    sgdt    (MEMORY_THUNK PTR [r8]).x64GdtDesc
    sidt    (MEMORY_THUNK PTR [r8]).x64IdtDesc

    xor     rax, rax                                  ; zero RAX
    mov     ax, ss                                    ; read in the stack segment
    mov     (MEMORY_THUNK PTR [r8]).x64Ss, rax        ; save in data structure

;
; Save flat x64 stack location (RSP).
;
    mov     (MEMORY_THUNK PTR [r8]).x64Esp, rsp
    
;
; patch in the current IntThunkAddr2.
;
    mov     rax, r8
    mov     qword ptr [IntThunkAddr2], rax  ; 8 bytes at IntThunkAddr2 = current address of IntThunk.

;
; Poke the TrampoleanCr3 value into the code.
;
    mov     rax, cr3
    mov     dword ptr[TrampoleanCr3], eax
    
;
; patch in the current ds, fs, gs and cs, so we can use it to return to x64 mode.
;
    xor     rax, rax
    mov     ax, ds
    mov     qword ptr [PatchX64Ds1], rax
    
    mov     ax, fs
    mov     qword ptr [PatchX64Fs1], rax
    
    mov     ax, gs
    mov     qword ptr [PatchX64Gs1], rax

    mov     ax, cs
    mov     word ptr [PatchLongModeCS], ax

;
;   patch the address of OffsetInLongMode.
;
    mov     rax,  offset InLongMode
    mov     dword  ptr[OffsetInLongMode], eax

;
;   patch the address of CompatibilityModeVector.
;    
    mov     rax,  offset CompatibilityMode
    mov     dword  ptr[CompatibilityModeVector], eax 

    mov     rsi, r8

;
;   PeiServicesPoint store in rdx 
;
    mov     rdx, r9

;
;   jmp to x64 Compatibility Mode.
;
    mov     rax,  offset CompatibilityModeVector

    jmp    FWORD PTR[rax]

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
    mov     ecx, 0c0000080h    ; EFER MSR number.
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
    DB      0b8h                ; mov eax, imm32
mIA32Cr3     DD      ?

    mov     cr3, rax

;
; Get the IA32 mode stack
;
    lea     rcx, ( MEMORY_THUNK PTR [0] ).ia32Stack
    add     rcx, rsi

    DB      8bh                 ; mov  ebx, DWORD PTR [ecx]
    DB      19h

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
    DB      052h                ; 'push edx' opcode
    call    rdi
    DB      05Ah                ; 'pop edx' opcode
    
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
    DB      0B8h                ; 'mov eax,imm32' opcode
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
    mov   ecx, 0c0000080h       ; EFER MSR number.
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
    db      067h
    db      0eah                ; Far Jump $+9:Selector to reload CS
OffsetInLongMode:
    dd      00000009            ;   $+9 Offset is ensuing instruction boundary
PatchLongModeCS:
    dw      0000h               ;   Selector is our code selector, 10h
InLongMode:

    DB      048h                ; mov ax, imm16
    DB      0B8h
PatchX64Ds1:
    DQ      0000h
    mov     es, ax
    mov     ss, ax
    mov     ds, ax

; restore fs and gs
    DB      048h                ; mov rax, imm16
    DB      0B8h
PatchX64Fs1:
    DQ      0000h  
    mov     fs, ax
    
    DB      048h                ; mov rax, imm16
    DB      0B8h
PatchX64Gs1:
    DQ      0000h
    mov     gs, ax


    DB      048h
    DB      0BEh                ; 'mov rsi,imm64' opcode (0xB8 + 6)
IntThunkAddr2:
    DQ      0000000000000000h   ; address of IntThunk (imm64)

;
; Restore 64-bit stack
;
    mov     rcx, (MEMORY_THUNK PTR [rsi]).x64Ss
    mov     ss, cx
    
    mov     rsp, (MEMORY_THUNK PTR [rsi]).x64Esp

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
    DD    ?
CodeSeg32offset:
    DW    ?
    
InternalThunk64To32  ENDP

    END
