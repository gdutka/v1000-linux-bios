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

;======================================================================
; SynchronizeAllApsF17Rv:  Wrapper code for boot time calibration workloads
;
;   In:
;       rdx - monitor address
;       rcx - increment address
;
;   Out:
;       None
;
;   Destroyed:
;       None
;
;======================================================================
SynchronizeAllApsF17Rv PROC PUBLIC
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

;  cli
  lock  inc dword Ptr [rcx]
;  mov   rax, rdx
;  mov   rcx, 0
;  mov   rdx, 0
;  monitor rax, rcx, rdx
;  mov   rax, 0
;  mov   rcx, 0
;  mwait rax, rcx

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
SynchronizeAllApsF17Rv ENDP

END


