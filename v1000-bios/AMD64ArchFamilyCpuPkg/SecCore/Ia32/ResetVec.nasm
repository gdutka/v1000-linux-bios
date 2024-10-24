;------------------------------------------------------------------------------
;
; Copyright (c) 2014, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
; Module Name:
;
;  ResetVec.nasmb
;
; Abstract:
;
;  Reset Vector Data structure
;  This structure is located at 0xFFFFF000
;
;------------------------------------------------------------------------------

extern   ASM_PFX(PcdGet32 (PcdSecPad1Raw32))
extern   ASM_PFX(PcdGet32 (PcdSecPad2Raw32))

;    .stack  0x0
;    SECTION .text
BITS    16
ALIGN   16

USE16
;
; The layout of this file is fixed. The build tool makes assumption of the layout.
;

;    ORG     0h ;Offset of $$

;
; 0xFFFFF000
;
    DD      ASM_PFX(PcdGet32 (PcdSecPad1Raw32))
    DD      ASM_PFX(PcdGet32 (PcdSecPad2Raw32))

USE16
    TIMES 0xFD0-($-$$) nop
;
; This is located at 0xFFFFFFD0
;
    mov     di, "PA"
    jmp     ApStartup

    TIMES 0xFE0-($-$$) nop
;
; Pointer to the entry point of the PEI core
; It is located at 0xFFFFFFE0, and is fixed up by some build tool
; So if the value 8..1 appears in the final FD image, tool failure occurs.
;
PeiCoreEntryPoint:       DD      87654321h

;
; This is the handler for all kinds of exceptions. Since it's for debugging
; purpose only, nothing except a dead loop would be done here. Developers could
; analyze the cause of the exception if a debugger had been attached.
;
global ASM_PFX(InterruptHandler)
ASM_PFX(InterruptHandler):
    jmp     $
    iret

    TIMES 0xFF0-($-$$) nop
;
; For IA32, the reset vector must be at 0xFFFFFFF0, i.e., 4G-16 byte
; Execution starts here upon power-on/platform-reset.
;
ResetHandler:
    nop
    nop
ApStartup:
    ;
    ; Jmp Rel16 instruction
    ; Use machine code directly in case of the assembler optimization
    ; SEC entry point relative address will be fixed up by some build tool.
    ;
    ; Typically, SEC entry point is the function _ModuleEntryPoint() defined in
    ; SecEntry.asm
    ;
    DB      0e9h
    DW      -3


    TIMES 0xFF8-($-$$) nop
;
; Ap reset vector segment address is at 0xFFFFFFF8
; This will be fixed up by some build tool,
; so if the value 1..8 appears in the final FD image,
; tool failure occurs
;
ApSegAddress:    dd      12345678h

    TIMES 0xFFC-($-$$) nop
;
; BFV Base is at 0xFFFFFFFC
; This will be fixed up by some build tool,
; so if the value 1..8 appears in the final FD image,
; tool failure occurs.
;
BfvBase:     DD      12345678h

;
; Nothing can go here, otherwise the layout of this file would change.
;
