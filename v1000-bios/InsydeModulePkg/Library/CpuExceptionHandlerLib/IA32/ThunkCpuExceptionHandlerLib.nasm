;; @file
;  Assembly code that supports IA32 for CpuExceptionHandlerLib
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

SECTION .data
TimerCallbackFunctionPtr: DW 0  ; Save Timer0 callback function pointer.

SECTION .text

global ASM_PFX(InitializeTimerCallbackPtr)
ASM_PFX(InitializeTimerCallbackPtr):
  push    ebp                  ; C prolog
  mov     ebp, esp

  mov     eax, [ebp+8]         ; Get callback function pointer
  mov     [TimerCallbackFunctionPtr], eax

  pop     ebp
  ret

;
;---------------------------------------;
; InterruptEntry                        ;
;---------------------------------------;
; IRQ0 interrupt serivce routine.
;
global ASM_PFX(InterruptEntry)
ASM_PFX(InterruptEntry):
  cli
  pushad
  ;
  ; 8259 IRQ0 EOI
  ;
  mov     al, 0x20
  out     0x20, al

  mov     eax, [TimerCallbackFunctionPtr]

  call    eax
  cli

  popad
  iretd

