;/**
; * @file
; *
; * AMD Family_17 Zepplen boot time calibration code
; *
; * @xrefitem bom "File Content Label" "Release Content"
; * @e project:      AGESA
; * @e sub-project:
; * @e \$Revision: 312756 $
; */
;*****************************************************************************
;
; Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
;
;******************************************************************************
 text  SEGMENT

XSET_BV MACRO
        db 00fh, 001h, 0d1h
ENDM

XGET_BV MACRO
        db 00fh, 001h, 0d0h
ENDM


;======================================================================
; ExecuteF17RvBtc:  Wrapper code for boot time calibration workloads
;
;   In:
;       rdx - buffer address for workloads
;       rcx - test to run
;
;   Out:
;       None
;
;   Destroyed:
;       None
;
;======================================================================
ExecuteHalt PROC PUBLIC
cli
hlt
jmp ExecuteHalt
ExecuteHalt ENDP


;======================================================================
; ExecuteF17RvBtc:  Wrapper code for boot time calibration workloads
;
;   In:
;       rdx - buffer address for workloads
;       rcx - test to run
;
;   Out:
;       None
;
;   Destroyed:
;       None
;
;======================================================================
ExecuteF17RvBtc PROC PUBLIC
  push  rax
  push  rbx
  push  rcx
  push  rdx
  push  rdi
  push  rsi
  push  rbp
  push  r8
  push  r9
  push  r10
  push  r11
  push  r12
  push  r13
  push  r14
  push  r15
  pushfq

;;EGH do this alignment in c code before calling
;  add   rdx, 63
;  and   rdx, 0FFFFFFFFFFFFFFC0h
  push  rcx
  push  rdx
  mov   rax, 1
  cpuid
  bt    rcx, 26
  jc    SetupAVX
  pop   rdx
  pop   rcx
  jmp   NoAVX

SetupAVX:
  mov   r10, cr4
  mov   r11, r10
  bts   r10, 18
  mov   cr4, r10
  xor   rcx, rcx
  XGET_BV
  mov   r12, rax
  mov   r13, rdx
  mov   rax, 0dh
  cpuid
  xor   rdx, rdx
  xor   rcx, rcx
  XSET_BV
  pop   rdx
  pop   rcx
  push  r11
  push  r12
  push  r13
  call  cpuF17RvBtc
  pop   rdx
  pop   rax
  xor   rcx, rcx
  XSET_BV
  pop   r11
  mov   cr4, r11

NoAVX:
  popfq
  pop   r15
  pop   r14
  pop   r13
  pop   r12
  pop   r11
  pop   r10
  pop   r9
  pop   r8
  pop   rbp
  pop   rsi
  pop   rdi
  pop   rdx
  pop   rcx
  pop   rbx
  pop   rax
  ret
ExecuteF17RvBtc ENDP

;======================================================================
; cpuF17RvBtc: The workloads.
;
;   In:
;       rdx - buffer address for workloads
;       rcx - test to run
;
;   Out:
;       None
;
;   Destroyed:
;       Many
;
;======================================================================
cpuF17RvBtc PROC PUBLIC
include cpuF17RvBtc64.inc
cpuF17RvBtc ENDP

END


