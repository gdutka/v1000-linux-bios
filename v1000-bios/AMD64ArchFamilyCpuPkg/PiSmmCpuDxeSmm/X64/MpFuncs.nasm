;** @file
;
;  This is the assembly code for Multi-processor S3 support.
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

extern  InitializeFloatingPointUnits

VacantFlag             Equ   00h
NotVacantFlag          Equ   0ffh

LockLocation                  equ        RendezvousFunnelProcEnd - RendezvousFunnelProcStart
StackStartAddressLocation     equ        LockLocation + 08h
StackSizeLocation             equ        LockLocation + 10h
CProcedureLocation            equ        LockLocation + 18h
GdtrLocation                  equ        LockLocation + 20h
IdtrLocation                  equ        LockLocation + 2Ah
BufferStartLocation           equ        LockLocation + 34h
Cr3OffsetLocation             equ        LockLocation + 38h
%define InitializeFloatingPointUnitsAddress LockLocation + 0x3C

;-------------------------------------------------------------------------------------
;RendezvousFunnelProc  procedure follows. All APs execute their procedure. This
;procedure serializes all the AP processors through an Init sequence. It must be
;noted that APs arrive here very raw...ie: real mode, no stack.
;ALSO THIS PROCEDURE IS EXECUTED BY APs ONLY ON 16 BIT MODE. HENCE THIS PROC
;IS IN MACHINE CODE.
;-------------------------------------------------------------------------------------
;RendezvousFunnelProc (&WakeUpBuffer,MemAddress);

;text      SEGMENT
DEFAULT REL
SECTION .text

BITS 16
global ASM_PFX(RendezvousFunnelProc)
ASM_PFX(RendezvousFunnelProc):
RendezvousFunnelProcStart:

; At this point CS = 0x(vv00) and ip= 0x0.

        mov        ax,  cs
        mov        ds,  ax
        mov        es,  ax
        mov        ss,  ax
        xor        ax,  ax
        mov        fs,  ax
        mov        gs,  ax

flat32Start:

        mov        si, BufferStartLocation
        mov        edx,dword [si]          ; EDX is keeping the start address of wakeup buffer

        mov        si, Cr3OffsetLocation
        mov        ecx,dword [si]          ; ECX is keeping the value of CR3

        mov        si, GdtrLocation
o32     lgdt       [cs:si]

        mov        si, IdtrLocation
o32     lidt       [cs:si]

        xor        ax,  ax
        mov        ds,  ax

        mov        eax, cr0                    ; Get control register 0
        or         eax, 0x000000001            ; Set PE bit (bit #0)
        mov        cr0, eax

FLAT32_JUMP:

a32     jmp   dword 0x20:0x0

BITS 32
NemInit:                             ; protected mode entry point

        mov        ax,  0x18
o16     mov        ds,  ax
o16     mov        es,  ax
o16     mov        fs,  ax
o16     mov        gs,  ax
o16     mov        ss,  ax                     ; Flat mode setup.

        mov        eax, cr4
        bts        eax, 5
        mov        cr4, eax

        mov        cr3, ecx

        mov        esi, edx                    ; Save wakeup buffer address

        mov        ecx, 0xc0000080             ; EFER MSR number.
        rdmsr                                  ; Read EFER.
        bts        eax, 8                      ; Set LME=1.
        wrmsr                                  ; Write EFER.

        mov        eax, cr0                    ; Read CR0.
        bts        eax, 31                     ; Set PG=1.
        mov        cr0, eax                    ; Write CR0.

LONG_JUMP:

a16     jmp   dword 0x38:0x0

BITS 64 
LongModeStart:

        mov         ax,  30h
o16     mov         ds,  ax
o16     mov         es,  ax
o16     mov         ss,  ax

        mov  edi, esi
        add  edi, LockLocation
        mov  al,  NotVacantFlag
TestLock:
        xchg byte [edi], al
        cmp  al, NotVacantFlag
        jz   TestLock

ProgramStack:

        mov  edi, esi
        add  edi, StackSizeLocation
        mov  rax, qword [edi]
        mov  edi, esi
        add  edi, StackStartAddressLocation
        add  rax, qword [edi]
        mov  rsp, rax
        mov  qword [edi], rax

Releaselock:

        mov  al,  VacantFlag
        mov  edi, esi
        add  edi, LockLocation
        xchg byte [edi], al

        ;
        ; Call assembly function to initialize FPU.
        ;
        mov         rax, InitializeFloatingPointUnits
        sub         rsp, 20h
        call        rax
        add         rsp, 20h

        ;
        ; Call C Function
        ;
        mov         edi, esi
        add         edi, CProcedureLocation
        mov         rax, qword [edi]

        test        rax, rax
        jz          GoToSleep

        sub         rsp, 20h
        call        rax
        add         rsp, 20h
        
GoToSleep:
        cli
        hlt
        jmp         $-2
        
RendezvousFunnelProcEnd:
;RendezvousFunnelProc   ENDP


;-------------------------------------------------------------------------------------
;  AsmGetAddressMap (&AddressMap);
;-------------------------------------------------------------------------------------
; comments here for definition of address map
;AsmGetAddressMap   PROC
global ASM_PFX(AsmGetAddressMap)
ASM_PFX(AsmGetAddressMap):
        mov         rax, RendezvousFunnelProcStart
        mov         qword [rcx], rax
        mov         qword [rcx+8h], NemInit - RendezvousFunnelProcStart
        mov         qword [rcx+10h], FLAT32_JUMP - RendezvousFunnelProcStart
        mov         qword [rcx+18h], RendezvousFunnelProcEnd - RendezvousFunnelProcStart
        mov         qword [rcx+20h], LongModeStart - RendezvousFunnelProcStart
        mov         qword [rcx+28h], LONG_JUMP - RendezvousFunnelProcStart
        ret
        
;AsmGetAddressMap   ENDP

;END
