;*****************************************************************************
;
; Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
;
;*****************************************************************************

.code
;/*++

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read MSR
; *
; *  @param[in]  RCX      MSR Address
; *  @retval     RAX      MSR Register Value
; */
PUBLIC CpmMsrRead
CpmMsrRead      PROC
        rdmsr
        and     rax, 0ffffffffh
        shl     rdx, 32
        or      rax, rdx
        ret
CpmMsrRead      ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Write MSR
; *
; *  @param[in]  RCX        MSR Address
; *  @param[in]  RDX        MSR Register Data
; */
PUBLIC CpmMsrWrite
CpmMsrWrite     PROC
        mov     rax, rdx
        and     rax, 0ffffffffh
        shr     rdx, 32
        wrmsr
        ret
CpmMsrWrite     ENDP


;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read CPUID Raw Data
; *
; *  @param[in]  RCX    CPUID function
; *  @param[in]  RDX    Pointer to CPUID_DATA to save cpuid data
; */
PUBLIC CpmCpuidRawRead
CpmCpuidRawRead   PROC
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
CpmCpuidRawRead   ENDP


;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read TSC
; *
; *  @retval    RAX Time stamp counter value
; */

PUBLIC CpmReadTsc
CpmReadTsc      PROC
        rdtsc
        and     rax, 0ffffffffh
        shl     rdx, 32
        or      rax, rdx
        ret
CpmReadTsc      ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Find the most right hand side non-zero bit with
; *
; * @param[in]  ECX       Value
; */
PUBLIC CpmBitScanForward
CpmBitScanForward PROC
    bsf eax, ecx
    jnz nonZeroSource
    mov al,32
nonZeroSource:
    ret
CpmBitScanForward  ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Find the most left hand side non-zero bit.
; *
; * @param[in]  ECX       Value
; */
PUBLIC CpmBitScanReverse
CpmBitScanReverse PROC
    bsr eax, ecx
    jnz nonZeroSource
    mov al,0FFh
nonZeroSource:
    ret
CpmBitScanReverse  ENDP

END
