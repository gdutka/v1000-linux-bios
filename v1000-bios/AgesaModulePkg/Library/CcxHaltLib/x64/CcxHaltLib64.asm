;/**
; * @file
; *
; * Agesa pre-memory miscellaneous support, including ap halt loop.
; *
; * @xrefitem bom "File Content Label" "Release Content"
; * @e project:      AGESA
; * @e sub-project:  CPU
; * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 12:28:05 -0600 (Tue, 09 Dec 2014) $
; */
;*****************************************************************************
;
; Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
;
;******************************************************************************

 text  SEGMENT

;======================================================================
; ExecuteHltInstruction:  Performs a hlt instruction.
;
;   In:
;       None
;
;   Out:
;       None
;
;   Destroyed:
;       eax, ebx, ecx, edx, esp
;
;======================================================================
ExecuteHltInstruction PROC PUBLIC
    cli
    hlt
    ret
ExecuteHltInstruction ENDP

END
