;** @file
;
;  Code template of the SMI handler for a particular processor.
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
;
; Constants relating to PROCESSOR_SMM_DESCRIPTOR
;
%define DSC_OFFSET         0xFDB0
%define DSC_GDTPTR         0x30
%define DSC_GDTSIZ         0x38
%define DSC_CS             14
%define DSC_DS             16
%define DSC_SS             18
%define DSC_OTHERSEG       20

;
; Constants relating to CPU State Save Area
;
%define SSM_SMBAS            0xFF00
%define SSM_DR6              0xFF68
%define SSM_DR7              0xFF60

;
; Variables referrenced by C code
;
extern SmiRendezvous                 ;EXTERNDEF   SmiRendezvous:PROC
global ASM_PFX(gcSmiHandlerTemplate) ;EXTERNDEF   gcSmiHandlerTemplate:BYTE
global ASM_PFX(gcSmiHandlerSize) ;EXTERNDEF   gcSmiHandlerSize:WORD
global ASM_PFX(gSmiCr3)          ;EXTERNDEF   gSmiCr3:DWORD
global ASM_PFX(gcSmiHandlerOffset) ;EXTERNDEF   gcSmiHandlerOffset:WORD
global ASM_PFX(gSmiStack) ;EXTERNDEF   gSmiStack:QWORD
extern ASM_PFX(FeaturePcdGet(PcdCpuSmmDebug));EXTERNDEF   FeaturePcdGet (PcdCpuSmmDebug):BYTE

%define PROTECT_MODE_CS      0x10
%define PROTECT_MODE_DS      0x18
%define TSS_SEGMENT          0x40
%define GDT_SIZE             0x50

section .rdata

ASM_PFX(gcSmiHandlerOffset):  DW      ASM_PFX(_SmiHandler) - ASM_PFX(_SmiEntryPoint) + 8000h

DEFAULT REL
SECTION .text

BITS 16
;gcSmiHandlerTemplate    LABEL   BYTE
ASM_PFX(gcSmiHandlerTemplate):

;_SmiEntryPoint  PROC
global ASM_PFX(_SmiEntryPoint)
ASM_PFX(_SmiEntryPoint):
    ;
    ; The encoding of BX in 16-bit addressing mode is the same as of RDI in 64-
    ; bit addressing mode. And that coincidence has been used in the following
    ; "64-bit like" 16-bit code. Be aware that once RDI is referrenced as a
    ; base address register, it is actually BX that is referrenced.
    ;
    mov     bx, _GdtDesc - _SmiEntryPoint + 0x8000

; fix GDT TSS table
    mov     eax, [cs:DSC_OFFSET + DSC_GDTPTR] ; eax = GDT base
    mov     edx, eax
    mov     ebp, eax                      ; ebp = GDT base
    add     edx, GDT_SIZE                ; edx = TSS descriptor base
    mov     [eax + TSS_SEGMENT + 2], dx
    shr     edx, 16
    mov     [eax + TSS_SEGMENT + 4], dl
    mov     [eax + TSS_SEGMENT + 7], dh
; fix GDT descriptor
    mov     ax, [cs:DSC_OFFSET + DSC_GDTSIZ]
    dec ax
    mov     [cs:bx], ax
    mov     eax, [cs:DSC_OFFSET + DSC_GDTPTR]
    mov     [cs:bx + 2], eax
o32 lgdt    [cs:bx]
    DB      66h, 0b8h                   ; mov eax, imm32
ASM_PFX(gSmiCr3): DD 0
    mov     cr3, eax
    mov     eax, 668h                   ; as cr4.PGE is not set here, refresh cr3
    mov     cr4, eax                    ; in PreModifyMtrrs() to flush TLB.
; Patch LongMode Segment
    mov     ax, [cs:DSC_OFFSET + DSC_CS]
    mov     [cs:bx + 6 + (@LongMode - @ProtectedMode) - 2], ax            ; mov cs:[bx - 2], ax
; Patch ProtectedMode Segment
    mov     ax, PROTECT_MODE_CS
    mov     [cs:bx - 2], ax
; Patch LongMode entry
    mov edi, [cs:SSM_SMBAS]
    lea     eax, [edi + (@LongMode - _SmiEntryPoint) + 8000h]
    mov     [cs:bx + 6 + (@LongMode - @ProtectedMode) - 6], eax
; Patch ProtectedMode entry
    lea     eax, [edi + (@ProtectedMode - _SmiEntryPoint) + 8000h]
    mov     [cs:bx - 6], eax
; Switch into @ProtectedMode
    mov     ecx, 0c0000080h
    mov     ebx, cr0
    and     ebx, 9ffafff3h
    or      ebx, 00000023h

    mov     cr0, ebx
    jmp     dword 0x0:0x0

ASM_PFX(_GdtDesc):
    DD      0
    DW      0
BITS 32
@ProtectedMode:
    mov     ax, PROTECT_MODE_DS
    mov     ds, eax
    mov     es, eax
    mov     ss, eax

; Load TSS
    mov     eax, ebp                    ; ebp = GDT base
    mov     edx, eax
    mov     dl, 89h
    mov     [eax + TSS_SEGMENT + 5], dl ; clear busy flag

    mov     eax, TSS_SEGMENT
    ltr     ax

; Switch into @LongMode
    rdmsr
    or      ah, 1
    wrmsr
    bts     ebx, 31
    mov     cr0, ebx
    DB      67h, 0eah
    DD      0 ; DD      ?
    DW      0 ; DW      ?

BITS 64
@LongMode:                              ; long mode (64-bit code) starts here
    lea     ebx, [rdi + DSC_OFFSET]
    mov     ax, [rbx + DSC_DS]
    mov     ds, eax
    mov     ax, [rbx + DSC_OTHERSEG]
    mov     es, eax
    mov     fs, eax
    mov     gs, eax
    mov     ax, [rbx + DSC_SS]
    mov     ss, eax
;   jmp     _SmiHandler                 ; instruction is not needed
;_SmiEntryPoint  ENDP

;_SmiHandler PROC
;global ASM_PFX(_SmiHandler)
ASM_PFX(_SmiHandler):
    DB      48h, 0bch                   ; mov rsp, imm64
ASM_PFX(gSmiStack): DQ 0
;
; The following lines restore DR6 & DR7 before running C code. They are useful
; when you want to enable hardware breakpoints in SMM without setting
; smmentrybreak=true in ITP.
;
; NOTE: These lines might not be appreciated in runtime since they might
;       conflict with OS debugging facilities. Turn them off in RELEASE.
;
    mov     rax, ASM_PFX(FeaturePcdGet(PcdCpuSmmDebug)) ;Get absolute address. Avoid RIP relative addressing
    cmp     byte [rax], 0
    jz      @1
    DB      48h, 8bh, 0dh               ; mov rcx, [rip + disp32]
    DD      SSM_DR6 - ($ + 4 - _SmiEntryPoint + 8000h)
    DB      48h, 8bh, 15h               ; mov rdx, [rip + disp32]
    DD      SSM_DR7 - ($ + 4 - _SmiEntryPoint + 8000h)
    mov     dr6, rcx
    mov     dr7, rdx
@1:
    mov     rcx, [rsp]                  ; rcx <- CpuIndex
    mov     rax, SmiRendezvous          ; rax <- absolute addr of SmiRedezvous

    ;
    ; Save FP registers
    ;
    sub     rsp, 208h
    fxsave64 [rsp]
    
    add     rsp, -20h
    call    rax
    add     rsp, 20h

    ;
    ; Restore FP registers
    ;
    fxrstor64 [rsp]  
 
    mov     rax, ASM_PFX(FeaturePcdGet(PcdCpuSmmDebug)) ;Get absolute address. Avoid RIP relative addressing
    cmp     byte [rax], 0
    jz      @2
    mov     rdx, dr7
    mov     rcx, dr6
    DB      48h, 89h, 15h               ; mov [rip + disp32], rdx
    DD      SSM_DR7 - ($ + 4 - _SmiEntryPoint + 8000h)
    DB      48h, 89h, 0dh               ; mov [rip + disp32], rcx
    DD      SSM_DR6 - ($ + 4 - _SmiEntryPoint + 8000h)
@2:
    StuffRsb64
    rsm
;_SmiHandler ENDP

ASM_PFX(gcSmiHandlerSize):    DW      $ - _SmiEntryPoint

;    END
