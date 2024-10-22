;/**
; * @file
; *
; * Agesa library 32bit
; *
; * Contains AMD AGESA Library
; *
; * @xrefitem bom "File Content Label" "Release Content"
; * @e project:      AGESA
; * @e sub-project:  Lib
; * @e \$Revision: 312770 $   @e \$Date: 2015-02-11 13:24:42 +0800 (Wed, 11 Feb 2015) $
; */
;*****************************************************************************
;
; Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
;
;******************************************************************************

.586p
.xmm
.model  flat
ASSUME FS:NOTHING
.code

;/*---------------------------------------------------------------------------------------*/
;/**
; *  Enter debugger on SimNow
; *
; * @retval  Flag to indicate execution of Port 80 loop code.
; *
; */
PUBLIC LibAmdSimNowEnterDebugger
LibAmdSimNowEnterDebugger PROC NEAR C USES EBX ECX EDX ESI
    mov     si, 0                   ; Clear SI to indicate display message on port 80
    mov     eax, 0BACCD00Bh         ; Backdoor in SimNow
    mov     ebx, 2                  ; Select breakpoint feature
    cpuid

    mov     ax, si                  ; Return value in SI
    ret
LibAmdSimNowEnterDebugger ENDP


;/*---------------------------------------------------------------------------------------*/
;/**
; *  Force breakpoint on HDT
; *
; *
; */
PUBLIC LibAmdHDTBreakPoint
LibAmdHDTBreakPoint PROC NEAR C

    pushad

    mov ecx, 0C001100Ah             ;bit 0 = HDT redirect
    mov edi, 09C5A203Ah             ;Password
    RDMSR                           ;
    or al, 1                        ;
    WRMSR                           ;
    mov al, 0B2h                    ;Marker = B2
    db 0F1h                         ;ICEBP

    popad
    ret

LibAmdHDTBreakPoint ENDP

;/*---------------------------------------------------------------------------------------*/
;/**
; *  FPU init
; *
; *
; */

PUBLIC LibAmdFinit
LibAmdFinit  PROC NEAR C
    finit
    ret
LibAmdFinit  ENDP

END

