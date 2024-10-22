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

.586p
.model  flat
.code
;/*---------------------------------------------------------------------------------------*/
;/**
; *  IDS IO port write
; *
; * @param[in]  Address     IO Port Address
; * @param[in]  Value       Value to write
; * @param[in]  Flag        IDS flags
; *
; */

HdtOutIoWrite PROC NEAR C PUBLIC ; Address:DWORD, Value:DWORD ,Flag:DWORD
    push ebp
    mov  ebp, esp
    push edx
    push eax
    push ebx
    mov edx, [ebp + 8]  ; Address
    mov eax, [ebp + 12] ; Value
    mov ebx, [ebp + 16] ; Flag
    out dx, eax
    pop ebx
    pop eax
    pop edx
    pop ebp
    ret
HdtOutIoWrite  ENDP

END

