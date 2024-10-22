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
; * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
; */
;*****************************************************************************
;
; Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
;
;******************************************************************************

.586p
.model  flat
ASSUME FS:NOTHING
.code
; void *memcpy( void *dest, void *src, size_t count );
;
; Copy count bytes from src to dest, returning dest.
; ("c" is not legal as an assembly parameter name, replaced with value.)
; Assume ES is set appropriately, 32 bit flat.
;
public memcpy
memcpy  PROC NEAR C PUBLIC USES ECX EDI ESI dest:DWORD, src:DWORD, count:DWORD
        pushf
        cld     ; We will increment through *dest
        mov     edi, dest
        mov     esi, src
        mov     ecx, count
        rep movsb
        mov eax, dest
        popf
        ret
memcpy ENDP

; void *memset( void *dest, int c, size_t count );
;
; At dest, set count bytes to byte value, returning dest.
; ("c" is not legal as an assembly parameter name, replaced with value.)
; Assume ES is set appropriately, 32 bit flat.
;
public memset
memset  PROC NEAR C PUBLIC USES ECX EDI dest:DWORD, value:DWORD, count:DWORD
        pushf
        cld     ; We will increment through *dest
        mov     edi, dest
        mov     eax, value
        mov     ecx, count
        rep stosb
        mov     eax, edi
        popf
        ret
memset ENDP

END

