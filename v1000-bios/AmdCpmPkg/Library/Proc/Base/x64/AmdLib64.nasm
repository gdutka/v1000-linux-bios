;*****************************************************************************
;
; Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
;
;*****************************************************************************

SECTION .text
;/*++

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read MSR
; *
; *  @param[in]  RCX      MSR Address
; *  @retval     RAX      MSR Register Value
; */
global ASM_PFX(CpmMsrRead)
ASM_PFX(CpmMsrRead):
        rdmsr
        and     rax, 0ffffffffh
        shl     rdx, 32
        or      rax, rdx
        ret

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Write MSR
; *
; *  @param[in]  RCX        MSR Address
; *  @param[in]  RDX        MSR Register Data
; */
global ASM_PFX(CpmMsrWrite)
ASM_PFX(CpmMsrWrite):
        mov     rax, rdx
        and     rax, 0ffffffffh
        shr     rdx, 32
        wrmsr
        ret


;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read CPUID Raw Data
; *
; *  @param[in]  RCX    CPUID function
; *  @param[in]  RDX    Pointer to CPUID_DATA to save cpuid data
; */
global ASM_PFX(CpmCpuidRawRead)
ASM_PFX(CpmCpuidRawRead):
    push rbx
    push rsi
    mov  rsi, rdx
    mov  rax, rcx
    cpuid
    mov [rsi],   eax
    mov [rsi+4], ebx
    mov [rsi+8], ecx
    mov [rsi+12],edx
    pop rsi
    pop rbx
    ret


;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read TSC
; *
; *  @retval    RAX Time stamp counter value
; */

global ASM_PFX(CpmReadTsc)
ASM_PFX(CpmReadTsc):
        rdtsc
        and     rax, 0ffffffffh
        shl     rdx, 32
        or      rax, rdx
        ret

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Find the most right hand side non-zero bit with
; *
; * @param[in]  ECX       Value
; */
global ASM_PFX(CpmBitScanForward)
ASM_PFX(CpmBitScanForward):
    bsf eax, ecx
    jnz nonZeroSource1
    mov al,32
nonZeroSource1:
    ret

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Find the most left hand side non-zero bit.
; *
; * @param[in]  ECX       Value
; */
global ASM_PFX(CpmBitScanReverse)
ASM_PFX(CpmBitScanReverse):
    bsr eax, ecx
    jnz nonZeroSource2
    mov al,0FFh
nonZeroSource2:
    ret
