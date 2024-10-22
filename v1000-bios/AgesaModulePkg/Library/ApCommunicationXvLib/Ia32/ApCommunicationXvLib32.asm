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
;    .XLIST
;    .LIST
    .586P
    .xmm

;===============================================
;===============================================
;==
;== M E M O R Y   A B S E N T   S E G M E N T
;==
;===============================================
;===============================================
    .MODEL flat
    ASSUME FS:NOTHING
    .CODE
;======================================================================
; SetIdtr:
;
;   In:
;       @param[in]   IdtPtr Points to IDT table
;
;   Out:
;       None
;
;   Destroyed:
;       none
;
;======================================================================
PUBLIC  SetIdtr
SetIdtr PROC NEAR C USES EBX, IdtPtr:PTR
  mov   ebx, IdtPtr
  lidt  fword ptr ss:[ebx]
  ret
SetIdtr ENDP

;======================================================================
; NmiHandler:  Simply performs an IRET.
;
;   In:
;       None
;
;   Out:
;       None
;
;   Destroyed:
;       None
;
;======================================================================
PUBLIC  NmiHandler
NmiHandler PROC NEAR C
  iretd
NmiHandler ENDP

;======================================================================
; GetCsSelector:  Returns the current protected mode CS selector.
;
;   In:
;       None
;
;   Out:
;       None
;
;   Destroyed:
;       None
;
;======================================================================
PUBLIC  GetCsSelector
GetCsSelector PROC NEAR C, CsSelector:PTR
  push ax
  push ebx

  call FarCallGetCs
  mov  ebx, CsSelector
  mov  [ebx], ax
  pop  ebx
  pop  ax
  ret
GetCsSelector ENDP

;======================================================================
; FarCallGetCs:
;
;   In:
;       None
;
;   Out:
;       None
;
;   Destroyed:
;       none
;
;======================================================================
FarCallGetCs PROC FAR PRIVATE

  mov  ax, ss:[esp + 4]
  retf

FarCallGetCs ENDP
END

