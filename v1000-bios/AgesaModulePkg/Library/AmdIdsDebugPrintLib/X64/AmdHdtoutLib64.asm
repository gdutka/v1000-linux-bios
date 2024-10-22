;/**
; * @file
; *
; * HDTOUT library 32bit
; *
; * Contains HDTOUT Library
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

.code
;/*---------------------------------------------------------------------------------------*/
;/**
; *  IDS IO port write
; *
; * @param[in]  ECX     IO Port Address
; * @param[in]  EDX     Value to write
; * @param[in]  R8D     IDS flags
; *
; */

HdtOutIoWrite PROC PUBLIC
    push rbx
    push rax

    mov ebx, r8d
    mov eax, edx
    mov edx, ecx
    out dx, eax

    pop rax
    pop rbx
    ret
HdtOutIoWrite  ENDP

END

