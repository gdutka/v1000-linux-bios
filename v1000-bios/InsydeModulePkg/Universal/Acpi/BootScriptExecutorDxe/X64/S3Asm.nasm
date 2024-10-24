;; @file
;******************************************************************************
;* Copyright (c) 2012 - 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
;   This is the assembly code for transferring to control to OS S3 waking vector
;   for X64 platform
;
; Copyright (c) 2006 - 2012, Intel Corporation. All rights reserved.<BR>
;
; This program and the accompanying materials
; are licensed and made available under the terms and conditions of the BSD License
; which accompanies this distribution.  The full text of the license may be found at
; http://opensource.org/licenses/bsd-license.php
;
; THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
; WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
;
;;

extern ASM_PFX(mOriginalHandler)
extern ASM_PFX(PageFaultHandler)

    DEFAULT REL
    SECTION .text

global ASM_PFX(AsmFixAddress16)
global ASM_PFX(AsmJmpAddr32)
global ASM_PFX(AsmTransferControl)
ASM_PFX(AsmTransferControl):
    ; rcx S3WakingVector    :DWORD
    ; rdx AcpiLowMemoryBase :DWORD
    lea   eax, [.tt]
    mov   r8, 0x2800000000
    or    rax, r8
    push  rax
    shrd  ebx, ecx, 20
    and   ecx, 0xf
    mov   bx, cx
    mov   [jmp_addr], ebx
    retf
.tt:
    DB    0xb8, 0x30, 0      ; mov ax, 30h as selector
    mov   ds, eax
    mov   es, eax
    mov   fs, eax
    mov   gs, eax
    mov   ss, eax
    mov   rax, cr0
    mov   rbx, cr4
    DB    0x66
    and   eax, ((~ 0x80000001) & 0xffffffff)
    and   bl, ~ (1 << 5)
    mov   cr0, rax
    DB    0x66
    mov   ecx, 0xc0000080
    rdmsr
    and   ah, ~ 1
    wrmsr
    mov   cr4, rbx
    DB    0xea              ; jmp far jmp_addr
jmp_addr: DD 0

global ASM_PFX(AsmTransferControl32)
ASM_PFX(AsmTransferControl32):
    ; S3WakingVector    :DWORD
    ; AcpiLowMemoryBase :DWORD
    push  rbp
    mov   ebp, esp
    DB    0x8d, 0x5          ;  lea   eax, AsmTransferControl16
AsmFixAddress16: DD 0
    push  0x28               ; CS
    push  rax
    retf

global ASM_PFX(AsmTransferControl16)
ASM_PFX(AsmTransferControl16):
    DB    0xb8, 0x30, 0      ; mov ax, 30h as selector
    mov   ds, ax
    mov   es, ax
    mov   fs, ax
    mov   gs, ax
    mov   ss, ax
    mov   rax, cr0          ; Get control register 0
    DB    0x66
    DB    0x83, 0xe0, 0xfe   ; and    eax, 0fffffffeh  ; Clear PE bit (bit #0)
    DB    0xf, 0x22, 0xc0    ; mov    cr0, eax         ; Activate real mode
    DB    0xea              ; jmp far AsmJmpAddr32
AsmJmpAddr32: DD 0

global ASM_PFX(PageFaultHandlerHook)
ASM_PFX(PageFaultHandlerHook):
    push    rax                         ; save all volatile registers
    push    rcx
    push    rdx
    push    r8
    push    r9
    push    r10
    push    r11
    ; save volatile fp registers
    add     rsp, -0x68
    stmxcsr [rsp + 0x60]
    movdqa  [rsp + 0x0], xmm0
    movdqa  [rsp + 0x10], xmm1
    movdqa  [rsp + 0x20], xmm2
    movdqa  [rsp + 0x30], xmm3
    movdqa  [rsp + 0x40], xmm4
    movdqa  [rsp + 0x50], xmm5

    add     rsp, -0x20
    call    ASM_PFX(PageFaultHandler)
    add     rsp, 0x20

    ; load volatile fp registers
    ldmxcsr [rsp + 0x60]
    movdqa  xmm0,  [rsp + 0x0]
    movdqa  xmm1,  [rsp + 0x10]
    movdqa  xmm2,  [rsp + 0x20]
    movdqa  xmm3,  [rsp + 0x30]
    movdqa  xmm4,  [rsp + 0x40]
    movdqa  xmm5,  [rsp + 0x50]
    add     rsp, 0x68

    test    al, al

    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdx
    pop     rcx
    pop     rax                         ; restore all volatile registers
    jnz     .1
    jmp     ASM_PFX(mOriginalHandler)
.1:
    add     rsp, 0x8                    ; skip error code for PF
    iretq

