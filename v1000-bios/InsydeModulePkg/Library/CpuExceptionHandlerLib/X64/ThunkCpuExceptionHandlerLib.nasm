;; @file
;  Assembly code that supports x64 for CpuExceptionHandlerLib
;
;******************************************************************************
;* Copyright (c) 2012 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

DEFAULT REL
SEGMENT .text

;
; Save Timer0 callback function pointer.
;
TimerCallbackFunctionPtr: dq 0

global ASM_PFX(InitializeTimerCallbackPtr)
ASM_PFX(InitializeTimerCallbackPtr):
  mov  [TimerCallbackFunctionPtr], rcx
  ret

%macro POPA_64 0
	pop	r15
	pop	r14
	pop	r13
	pop	r12
	pop	r11
	pop	r10
	pop	r9
	pop	r8
	pop	rdi
	pop	rsi
	pop	rdx
	pop	rcx
	pop	rbx
	pop	rax
	pop	rbp
	pop	rsp
%endmacro

%macro PUSHA_64 0
	push	rsp
	push	rbp
	push	rax
	push	rbx
	push	rcx
	push	rdx
	push	rsi
	push	rdi
	push	r8
	push	r9
	push	r10
	push	r11
	push	r12
	push	r13
	push	r14
	push	r15
%endmacro

;
;---------------------------------------;
; InterruptEntry                        ;
;---------------------------------------;
; IRQ0 interrupt serivce routine.
;
global ASM_PFX(InterruptEntry)
ASM_PFX(InterruptEntry):
  cli
  PUSHA_64
  ;
  ; 8259 IRQ0 EOI
  ;
  mov     al, 0x20
  out     0x20, al

  mov     rax, [TimerCallbackFunctionPtr]

  sub     rsp, 0x28
  call    rax
  cli
  add     rsp, 0x28

  POPA_64
  iretq

