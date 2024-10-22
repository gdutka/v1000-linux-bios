;** @file
;
;  Exception handlers used in SM mode.
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

;EXTERNDEF   gSmiMtrrs:QWORD
;EXTERNDEF   gcSmiIdtr:FWORD
;EXTERNDEF   gcSmiGdtr:FWORD
;EXTERNDEF   gcPsd:BYTE
;EXTERNDEF   gPhyMask:QWORD
;EXTERNDEF   FeaturePcdGet (PcdCpuSmmStackGuard):BYTE
;EXTERNDEF   gSavedDebugExceptionIdtEntry:QWORD
;EXTERNDEF   gSmiExceptionHandlers:QWORD
;EXTERNDEF   FeaturePcdGet (PcdCpuSmmProfileEnable):BYTE
extern   ASM_PFX(gSmiMtrrs)
global   ASM_PFX(gcSmiIdtr)
global   ASM_PFX(gcSmiGdtr)
global   ASM_PFX(gcPsd)
global   ASM_PFX(gPhyMask)
extern   ASM_PFX(FeaturePcdGet(PcdCpuSmmStackGuard))
global   ASM_PFX(gSavedDebugExceptionIdtEntry)
global   ASM_PFX(gSmiExceptionHandlers)

;CpuDeadLoop     PROTO
;SmiPFHandler    PROTO
extern ASM_PFX(CpuDeadLoop)
extern ASM_PFX(SmiPFHandler)

section .rdata

NullSeg:    DQ      0                   ; reserved by architecture
            DQ      0                   ; reserved for future use
;CodeSeg32   LABEL   QWORD
global ASM_PFX(CodeSeg32)
ASM_PFX(CodeSeg32):
            DW      -1                  ; LimitLow
            DW      0                   ; BaseLow
            DB      0                   ; BaseMid
            DB      9bh
            DB      0cfh                ; LimitHigh
            DB      0                   ; BaseHigh
;DataSeg32   LABEL   QWORD
global ASM_PFX(DataSeg32)
ASM_PFX(DataSeg32):
            DW      -1                  ; LimitLow
            DW      0                   ; BaseLow
            DB      0                   ; BaseMid
            DB      93h
            DB      0cfh                ; LimitHigh
            DB      0                   ; BaseHigh
            DQ      0                   ; reserved for future use
;CodeSeg16   LABEL   QWORD
global ASM_PFX(CodeSeg16)
ASM_PFX(CodeSeg16):
            DW      -1
            DW      0
            DB      0
            DB      9bh
            DB      8fh
            DB      0
;DataSeg16   LABEL   QWORD
global ASM_PFX(DataSeg16)
ASM_PFX(DataSeg16):
            DW      -1
            DW      0
            DB      0
            DB      93h
            DB      8fh
            DB      0
;CodeSeg64   LABEL   QWORD
global ASM_PFX(CodeSeg64)
ASM_PFX(CodeSeg64):
            DW      -1                  ; LimitLow
            DW      0                   ; BaseLow
            DB      0                   ; BaseMid
            DB      9bh
            DB      0afh                ; LimitHigh
            DB      0                   ; BaseHigh
; TSS Segment for X64 specially
;TssSeg      LABEL   QWORD
global ASM_PFX(TssSeg)
ASM_PFX(TssSeg):
            DW      TSS_DESC_SIZE       ; LimitLow
            DW      0                   ; BaseLow
            DB      0                   ; BaseMid
            DB      89h
            DB      080h                ; LimitHigh
            DB      0                   ; BaseHigh
            DD      0                   ; BaseUpper
            DD      0                   ; Reserved
GDT_SIZE    equ     $ - NullSeg         ;GDT_SIZE = $ - NullSeg

; Create TSS Descriptor just after GDT
;TssDescriptor LABEL BYTE
global ASM_PFX(TssDescriptor)
ASM_PFX(TssDescriptor):
            DD      0                   ; Reserved
            DQ      0                   ; RSP0
            DQ      0                   ; RSP1
            DQ      0                   ; RSP2
            DD      0                   ; Reserved
            DD      0                   ; Reserved
            DQ      0                   ; IST1
            DQ      0                   ; IST2
            DQ      0                   ; IST3
            DQ      0                   ; IST4
            DQ      0                   ; IST5
            DQ      0                   ; IST6
            DQ      0                   ; IST7
            DD      0                   ; Reserved
            DD      0                   ; Reserved
            DW      0                   ; Reserved
            DW      0                   ; I/O Map Base Address
TSS_DESC_SIZE equ   $ - ASM_PFX(TssDescriptor) ;TSS_DESC_SIZE = $ - TssDescriptor

;
; This structure serves as a template for all processors.
;
;gcPsd     LABEL   BYTE
ASM_PFX(gcPsd):
            DB      'PSDSIG  '
            DW      PSD_SIZE
            DW      2
            DW      1 << 2 ; DW      1 SHL 2
            DW      CODE_SEL
            DW      DATA_SEL
            DW      DATA_SEL
            DW      DATA_SEL
            DW      0
            DQ      0
            DQ      0
            DQ      0                   ; fixed in InitializeMpServiceData()
            DQ      NullSeg
            DD      GDT_SIZE
            DD      0
            times 24 DB 0 ; DB      24 dup (0)
            DQ      gSmiMtrrs
PSD_SIZE    equ     $ - gcPsd ;PSD_SIZE  = $ - gcPsd

;
; CODE & DATA segments for SMM runtime
;
CODE_SEL    equ     CodeSeg64 - NullSeg ;CODE_SEL    = CodeSeg64 - NullSeg
DATA_SEL    equ     DataSeg32 - NullSeg ;DATA_SEL    = DataSeg32 - NullSeg

;gcSmiGdtr   LABEL   FWORD
ASM_PFX(gcSmiGdtr):
    DW      GDT_SIZE - 1
    DQ      NullSeg

;gcSmiIdtr   LABEL   FWORD
ASM_PFX(gcSmiIdtr):
    DW      IDT_SIZE - 1
    DQ      ASM_PFX(_SmiIDT)

section .data

;
; Here is the IDT. There are 32 (not 255) entries in it since only processor
; generated exceptions will be handled.
;
ASM_PFX(_SmiIDT):
%rep 32 ; REPEAT      32
    DW      0                           ; Offset 0:15
    DW      CODE_SEL                    ; Segment selector
    DB      0                           ; Unused
    DB      8eh                         ; Interrupt Gate, Present
    DW      0                           ; Offset 16:31
    DQ      0                           ; Offset 32:63
%endrep ;%endmacro ;ENDM
ASM_PFX(_SmiIDTEnd):

IDT_SIZE equ (ASM_PFX(_SmiIDTEnd) - ASM_PFX(_SmiIDT)) ;IDT_SIZE = (_SmiIDTEnd - _SmiIDT)


;
;  Saved IDT Entry for INT 1
;
;gSavedDebugExceptionIdtEntry    LABEL   QWORD
ASM_PFX(gSavedDebugExceptionIdtEntry):
    DQ      0                        
    DQ      0 

;
; CpuDeadLoop() is the default exception handler since it preserves the processor
; branch log.
;
;gSmiExceptionHandlers   LABEL   QWORD
ASM_PFX(gSmiExceptionHandlers):
times 14 DQ (CpuDeadLoop) ;DQ      14  dup (CpuDeadLoop)
         DQ SmiPFHandler                 ; Page Fault handler
times 17 DQ (CpuDeadLoop) ;DQ      17  dup (CpuDeadLoop)

;
; Here are the global variables used by #PF exception handler.
;
_PFPML4     DD      0
_PFPDP      DD      0
_PFLOCK     DB      0

;    .code

DEFAULT REL
SECTION .text

;------------------------------------------------------------------------------
; _SmiExceptionEntryPoints is the collection of exception entrypoints followed
; by a common exception handler.
;
; Stack frame would be as follows as specified in IA32 manuals:
;   SS      +68h
;   RSP     +60h
;   RFLAGS  +58h
;   CS      +50h
;   RIP     +48h
;   ErrCode +40h
;   INT#    +38h
;   RAX     +30h
;   RCX     +28h
;   RDX     +20h
;   R8      +18h
;   R9      +10h
;   R10     +8
;   R11     +0
;
; RSP set to odd multiple of 8 at @CommonEntryPoint means ErrCode PRESENT
;------------------------------------------------------------------------------
;_SmiExceptionEntryPoints    PROC
global ASM_PFX(_SmiExceptionEntryPoints)
ASM_PFX(_SmiExceptionEntryPoints):
IHDLRIDX equ 0
    %rep    31 ;REPEAT      31          ; handler for INT0~INT30
    push    qword IHDLRIDX
    jmp     @CommonEntryPoint
    %assign IHDLRIDX IHDLRIDX+1 ; IHDLRIDX = IHDLRIDX + 1
    %endrep ;%endmacro;ENDM
    push    qword IHDLRIDX              ; handler for INT31
@CommonEntryPoint:
    test    spl, 8                      ; odd multiple of 8 => ErrCode present
    jnz     .0
    push    qword [rsp]                       ; duplicate INT# if no ErrCode
.0:
    push    rax                         ; save all volatile registers
    push    rcx
    push    rdx
    push    r8
    push    r9
    push    r10
    push    r11

; UEFI calling convention for x64 requires that Direction flag in EFLAGs is clear
    cld

    mov     rcx, [rsp + 38h]            ; INT#
    mov     rdx, [rsp + 40h]            ; ErrCode (if any)
    mov     r8,  [rsp + 48h]            ; RIP
    add     rsp, -20h
    lea     rax, [ASM_PFX(gSmiExceptionHandlers)]
    call    qword [rax + rcx*8]
    add     rsp, 20h

; Set single step DB# if SMM profile is enabled and page fault exception happens
;    mov     rax, offset FeaturePcdGet (PcdCpuSmmProfileEnable)
;    cmp     byte ptr [rax], 0
;    jz      @Done
; Check if this is page fault exception
    mov     rax, [rsp + 38h]
    cmp     rax, 0eh
    jnz     .1
; Enable TF bit after page fault handler runs
    mov     rax, [rsp + 58h] ; RFLAGS
    bts     eax, 8
    mov     [rsp + 58h], rax
    jmp     @Done
.1:
; Check if this is INT 1 exception
    mov     rax, [rsp + 38h]
    cmp     rax, 1h
    jnz     @Done
; Clear TF bit after INT1 handler runs
    mov     rax, [rsp + 58h] ; RFLAGS
    btc     eax, 8
    mov     [rsp + 58h], rax        

@Done:
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdx
    pop     rcx
    pop     rax                         ; restore all volatile registers
    add     rsp, 10h                    ; skip INT# & ErrCode
    iretq
;_SmiExceptionEntryPoints    ENDP

;InitializeIDT   PROC
global ASM_PFX(InitializeIDT)
ASM_PFX(InitializeIDT):
    mov     ecx, (ASM_PFX(_SmiIDTEnd) - ASM_PFX(_SmiIDT)) >> 2 ; mov     ecx, (_SmiIDTEnd - _SmiIDT) SHR 2
    lea     rdx, [ASM_PFX(_SmiIDT) - 16]
    lea     r10, [ASM_PFX(_SmiExceptionEntryPoints) - 4]
.0:
    lea     rax, [r10 + rcx]            ; rax <- handler address
    mov     [rdx + rcx*4], ax
    shr     rax, 16
    mov     [rdx + rcx*4 + 6], ax
    shr     rax, 16
    mov     [rdx + rcx*4 + 8], eax
    add     ecx, -4
    jnz     .0

; Get absolute address. Avoid RIP relative addressing
    mov     rax, ASM_PFX(FeaturePcdGet(PcdCpuSmmStackGuard))
    cmp     byte [rax], 0
    jz      .1

;
; If SMM Stack Guard feature is enabled, set the IST field of
; the interrupe gate for Page Fault Exception to be 1
;
    lea     rax, [ASM_PFX(_SmiIDT) + 14 * 16]
    mov     byte [rax + 4], 1
;@@:
;    mov     rax, offset FeaturePcdGet (PcdCpuSmmProfileEnable)
;    cmp     byte ptr [rax], 0
;    jz      @F

;
; Save INT 1 IDT entry in gSavedDebugExceptionIdtEntry
;
    lea     rcx, [ASM_PFX(_SmiIDT) + 1 * 16]
    lea     rdx, [ASM_PFX(gSavedDebugExceptionIdtEntry)]
    mov     rax, [rcx]
    mov     [rdx], rax
    mov     rax, [rcx + 8]
    mov     [rdx + 8], rax
.1:    
    ret
;InitializeIDT   ENDP

;    END
