;/**
; * @file
; *
; * Agesa pre-memory miscellaneous support, including AP halt loop.
; *
; * @xrefitem bom "File Content Label" "Release Content"
; * @e project:      AGESA
; * @e sub-project:  Lib
; * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 12:28:05 -0600 (Tue, 09 Dec 2014) $
; */
;*****************************************************************************
;
; Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
;
;******************************************************************************
    .XLIST
    INCLUDE CcxHalt.inc
    .LIST

    .586P

;===============================================
;===============================================
;==
;== M E M O R Y   A B S E N T   S E G M E N T
;==
;===============================================
;===============================================
    .MODEL flat
    .CODE

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
PUBLIC  ExecuteHltInstruction
ExecuteHltInstruction PROC NEAR C
  cli
  hlt
  ret
ExecuteHltInstruction ENDP

END
