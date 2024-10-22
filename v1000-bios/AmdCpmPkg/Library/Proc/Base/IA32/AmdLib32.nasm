;*****************************************************************************
;
; Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
;
;*****************************************************************************
%include "NasmBase.inc"

SECTION .text

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read MSR
; *
; *  @param[in]  Address  MSR Address
; *  @retval     MSR register Value
; */
global ASM_PFX(CpmMsrRead)
ASM_PFX(CpmMsrRead):
    push    ebp
    mov     ebp, esp
    push    ecx
    mov     ecx, [ebp + 8]
    rdmsr
    pop     ecx
    leave
    ret

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Write MSR
; *
; *  @param[in]  Address    MSR Address
; *  @param[in]  Data       MSR Register Data
; */
global ASM_PFX(CpmMsrWrite)
ASM_PFX(CpmMsrWrite):
    push    ebp
    mov     ebp, esp
    push    ecx
    mov     ecx, [ebp + 8]
    mov     eax, [ebp + 12]
    mov     edx, [ebp + 16]
    wrmsr
    pop     ecx
    leave
    ret

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read CPUID Raw Data
; *
; *  @param[in]  Func   CPUID function
; *  @param[in]  DATA   Pointer to CPUID_DATA to save cpuid data
; */
global ASM_PFX(CpmCpuidRawRead)
ASM_PFX(CpmCpuidRawRead):
        push    ebp
        mov     ebp, esp
        pushad
        mov     eax, [ebp + 8]
        cpuid
        mov     esi, [ebp + 12]
        mov     [esi],   eax
        mov     [esi+4], ebx
        mov     [esi+8], ecx
        mov     [esi+12],edx
        popad
        leave
        ret

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read TSC
; *
; *  @retval     Time stamp counter value
; */
global ASM_PFX(CpmReadTsc)
ASM_PFX(CpmReadTsc):
        rdtsc
        ret

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Find the most right hand side non-zero bit.
; *
; * @param[in]  Value       Value
; */
global ASM_PFX(CpmBitScanForward)
ASM_PFX(CpmBitScanForward):
    push    ebp
    mov     ebp, esp
    mov eax, [ebp + 8]
    bsf eax, [ebp + 8]
    _if zero
      mov al,32
    _endif
    leave
    ret

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Find the most left hand side non-zero bit.
; *
; * @param[in]  Value       Value
; */
global ASM_PFX(CpmBitScanReverse)
ASM_PFX(CpmBitScanReverse):
    push    ebp
    mov     ebp, esp
    mov eax, [ebp + 8]
    bsr eax, [ebp + 8]
    _if zero
      mov al,0FFh
    _endif
    leave
    ret

