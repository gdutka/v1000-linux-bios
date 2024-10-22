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
;/*++
;
;Routine Description:
;
;  Shifts a UINT64 to the right.
;
;Arguments:
;
;  EDX:EAX - UINT64 value to be shifted
;  CL -      Shift count
;
;Returns:
;
;  EDX:EAX - shifted value
;
;--*/
_allshr  PROC NEAR C PUBLIC
_aullshr  PROC NEAR C PUBLIC
        .if (cl < 64)
            .if (cl >= 32)
                sub    cl, 32
                mov    eax, edx
                xor    edx, edx
            .endif
            shrd   eax, edx, cl
            shr    edx, cl
        .else
            xor    eax, eax
            xor    edx, edx
        .endif
        ret
_aullshr ENDP
_allshr ENDP

;/*++
;
;Routine Description:
;
;  Shifts a UINT64 to the left.
;
;Arguments:
;
;  EDX:EAX - UINT64 value to be shifted
;  CL -      Shift count
;
;Returns:
;
;  EDX:EAX - shifted value
;
;--*/
_allshl  PROC NEAR C PUBLIC USES CX
        .if (cl < 64)
            .if (cl >= 32)
                sub    cl, 32
                mov    edx, eax
                xor    eax, eax
            .endif
            shld   edx, eax, cl
            shl    eax, cl
        .else
            xor    eax, eax
            xor    edx, edx
        .endif
        ret
_allshl ENDP

END

