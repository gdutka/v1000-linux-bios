;*****************************************************************************
;
; Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
;
;*****************************************************************************

.586p
.model  flat
.code

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read MSR
; *
; *  @param[in]  Address  MSR Address
; *  @retval     MSR register Value
; */
public CpmMsrRead
CpmMsrRead      PROC NEAR C USES ECX Address:DWORD
        mov     ecx, Address
        rdmsr
        ret
CpmMsrRead      ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Write MSR
; *
; *  @param[in]  Address    MSR Address
; *  @param[in]  Data       MSR Register Data
; */
public CpmMsrWrite
CpmMsrWrite     PROC NEAR C USES ECX Address:DWORD, Data:QWORD
        mov     ecx, Address
        mov     eax, DWORD PTR Data[0]
        mov     edx, DWORD PTR Data[4]
        wrmsr
        ret
CpmMsrWrite     ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read CPUID Raw Data
; *
; *  @param[in]  Func   CPUID function
; *  @param[in]  DATA   Pointer to CPUID_DATA to save cpuid data
; */
public CpmCpuidRawRead
CpmCpuidRawRead PROC NEAR C  Func:DWORD, DATA:PTR
        pushad
        mov     eax, Func
        cpuid
        mov     esi, DATA
        mov     [esi],   eax
        mov     [esi+4], ebx
        mov     [esi+8], ecx
        mov     [esi+12],edx
        popad
        ret
CpmCpuidRawRead ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Read TSC
; *
; *  @retval     Time stamp counter value
; */
public CpmReadTsc
CpmReadTsc      PROC    NEAR C
        rdtsc
        ret
CpmReadTsc      ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Find the most right hand side non-zero bit.
; *
; * @param[in]  Value       Value
; */
PUBLIC CpmBitScanForward
CpmBitScanForward PROC NEAR C Value:DWORD
    mov eax, Value
    bsf eax, Value
    .if (Zero?)
      mov al,32
    .endif
    ret
CpmBitScanForward  ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Find the most left hand side non-zero bit.
; *
; * @param[in]  Value       Value
; */
PUBLIC CpmBitScanReverse
CpmBitScanReverse PROC NEAR C Value:DWORD
    mov eax, Value
    bsr eax, Value
    .if (Zero?)
      mov al,0FFh
    .endif
    ret
CpmBitScanReverse  ENDP

END
