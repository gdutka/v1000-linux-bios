;** @file
;
;  Functions for relocating SMBASE's for all processors.
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

%include    "StuffRsbNasm.inc"

%define SSM_SMBAS 0xFF00


extern   ASM_PFX(SmmInitHandler)                     ;EXTERNDEF   SmmInitHandler:PROC
global   ASM_PFX(gSmmCr0)                            ;EXTERNDEF   gSmmCr0:DWORD
global   ASM_PFX(gSmmCr3)                            ;EXTERNDEF   gSmmCr3:DWORD
global   ASM_PFX(gSmmCr4)                            ;EXTERNDEF   gSmmCr4:DWORD
global   ASM_PFX(gSmmJmpAddr)                        ;EXTERNDEF   gSmmJmpAddr:QWORD
global   ASM_PFX(gcSmmInitTemplate)                  ;EXTERNDEF   gcSmmInitTemplate:BYTE
global   ASM_PFX(gcSmmInitSize)                      ;EXTERNDEF   gcSmmInitSize:WORD
extern   ASM_PFX(mRebasedFlag)                       ;EXTERNDEF   mRebasedFlag:PTR BYTE
extern   ASM_PFX(mSmmRelocationOriginalAddress)      ;EXTERNDEF   mSmmRelocationOriginalAddress:QWORD
global   ASM_PFX(mRebasedFlagAddr32)                 ;EXTERNDEF   mRebasedFlagAddr32:DWORD
global   ASM_PFX(mSmmRelocationOriginalAddressPtr32) ;EXTERNDEF   mSmmRelocationOriginalAddressPtr32:DWORD
global   ASM_PFX(gSmmInitStack)                      ;EXTERNDEF   gSmmInitStack:QWORD

SECTION .data

ASM_PFX(NullSeg): DQ 0                  ; reserved by architecture
CodeSeg64:;CodeSeg64   LABEL   QWORD
            DW      -1                  ; LimitLow
            DW      0                   ; BaseLow
            DB      0                   ; BaseMid
            DB      0x9b
            DB      0xaf                ; LimitHigh
            DB      0                   ; BaseHigh
GDT_SIZE equ $ - NullSeg ;GDT_SIZE = $ - NullSeg


DEFAULT REL
SECTION .text

;BITS 16
ASM_PFX(GdtDesc): ;GdtDesc     LABEL   FWORD
            DW      GDT_SIZE
            DQ      ASM_PFX(NullSeg)

;SmmStartup  PROC
global ASM_PFX(SmmStartup)

ASM_PFX(SmmStartup):
    DB      0x66, 0xb8                   ; mov eax, imm32
ASM_PFX(gSmmCr3): DD 0
    mov     cr3, rax
    DB      0x66
    lgdt    [cs:ebp + (ASM_PFX(GdtDesc) - ASM_PFX(SmmStartup))]
    DB      0x66, 0xb8                   ; mov eax, imm32
ASM_PFX(gSmmCr4): DD 0
    mov     cr4, rax
    DB      0x66
    mov     ecx, 0xc0000080             ; IA32_EFER MSR
    rdmsr
    or      ah, 1                       ; set LME bit
    wrmsr
    DB      0x66, 0xb8                   ; mov eax, imm32
ASM_PFX(gSmmCr0): DD 0
    mov     cr0, rax                    ; enable protected mode & paging
    DB      66h, 0eah                   ; far jmp to long mode
ASM_PFX(gSmmJmpAddr): DQ @LongMode

;BITS 64
@LongMode:                              ; long-mode starts here
    DB      0x48, 0xbc                   ; mov rsp, imm64
ASM_PFX(gSmmInitStack): DQ 0
    and     sp, 0xfff0                  ; make sure RSP is 16-byte aligned
    ;
    ; According to X64 calling convention, XMM0~5 are volatile, we need to save
    ; them before calling C-function.
    ;
    sub     rsp, 0x60
    movdqa  [rsp], xmm0
    movdqa  [rsp + 0x10], xmm1
    movdqa  [rsp + 0x20], xmm2
    movdqa  [rsp + 0x30], xmm3
    movdqa  [rsp + 0x40], xmm4
    movdqa  [rsp + 0x50], xmm5

    add     rsp, -0x20
    call    ASM_PFX(SmmInitHandler)
    add     rsp, 0x20

    ;
    ; Restore XMM0~5 after calling C-function.
    ;
    movdqa  xmm0, [rsp]
    movdqa  xmm1, [rsp + 0x10]
    movdqa  xmm2, [rsp + 0x20]
    movdqa  xmm3, [rsp + 0x30]
    movdqa  xmm4, [rsp + 0x40]
    movdqa  xmm5, [rsp + 0x50]

    StuffRsb64
    rsm
;SmmStartup  ENDP

BITS 16
;gcSmmInitTemplate   LABEL   BYTE
;_SmmInitTemplate    PROC
ASM_PFX(gcSmmInitTemplate):
ASM_PFX(_SmmInitTemplate):
    mov ebp, [cs:@L1 - ASM_PFX(gcSmmInitTemplate) + 0x8000]
    sub ebp, [cs:SSM_SMBAS]
    jmp     ebp                          ; jmp ebp actually
@L1:
    DQ     ASM_PFX(SmmStartup)
;_SmmInitTemplate    ENDP

ASM_PFX(gcSmmInitSize):   DW  $ - ASM_PFX(gcSmmInitTemplate)

BITS 64
;SmmRelocationSemaphoreComplete PROC
global ASM_PFX(SmmRelocationSemaphoreComplete)
ASM_PFX(SmmRelocationSemaphoreComplete):
    push    rax
    mov     rax, [ASM_PFX(mRebasedFlag)]
    mov     byte [rax], 1
    pop     rax
    jmp     [ASM_PFX(mSmmRelocationOriginalAddress)]
;SmmRelocationSemaphoreComplete ENDP

;
; Semaphore code running in 32-bit mode
;
;BITS 32
;SmmRelocationSemaphoreComplete32 PROC
global ASM_PFX(SmmRelocationSemaphoreComplete32)
ASM_PFX(SmmRelocationSemaphoreComplete32):
    ;
    ; mov byte ptr [], 1
    ;
    db      0xc6, 0x5
ASM_PFX(mRebasedFlagAddr32):    dd      0
    db      1
    ;
    ; jmp dword ptr []
    ;
    db      0xff, 0x25
ASM_PFX(mSmmRelocationOriginalAddressPtr32): dd 0
;SmmRelocationSemaphoreComplete32 ENDP

;    END
