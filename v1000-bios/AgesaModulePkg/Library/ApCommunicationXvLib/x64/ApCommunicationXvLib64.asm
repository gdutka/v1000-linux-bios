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
SetIdtr PROC PUBLIC
    ; This stub function is here to avoid compilation errors.
    ; At this time, there is no need to provide a 64 bit function.
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
NmiHandler PROC PUBLIC
    iretq
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
GetCsSelector PROC PUBLIC
    ; This stub function is here to avoid compilation errors.
    ; At this time, there is no need to provide a 64 bit function.
    ret
GetCsSelector ENDP

END

