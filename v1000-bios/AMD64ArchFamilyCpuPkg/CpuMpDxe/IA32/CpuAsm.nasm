;** @file
;
;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

  page    ,132
  title   CPU ARCHITECTURAL DXE PROTOCOL ASSEMBLY HOOKS
;------------------------------------------------------------------------------
;
; Copyright (c) 1999 - 2006, Intel Corporation. All rights reserved.<BR>
; This software and associated documentation (if any) is furnished
; under a license and may only be used or copied in accordance
; with the terms of the license. Except as permitted by such
; license, no part of this software or documentation may be
; reproduced, stored in a retrieval system, or transmitted in any
; form or by any means without the express written consent of
; Intel Corporation.
;
; Module Name:
;
;   CpuAsm.asm
; 
; Abstract:
; 
;   Assembly code that supports IA32 CPU architectural protocol
;
;------------------------------------------------------------------------------

SECTION .text

;ApMachineCheckHandler    PROC C    PUBLIC
global ASM_PFX(ApMachineCheckHandler)
ASM_PFX(ApMachineCheckHandler):
  ;
  ; Clear MCIP flag of IA32_MCG_STATUS register
  ;
  mov     ecx, 17ah
  rdmsr
  btr     eax, 2
  wrmsr

  iretd
  
;ApMachineCheckHandler ENDP

EXTERNDEF C ApMachineCheckHandlerEnd:BYTE

ApMachineCheckHandlerEnd LABEL BYTE

;------------------------------------------------------------------------------
;  VOID
;  C1eExceptionHandler (
;    VOID
;    )
;------------------------------------------------------------------------------
;_C1eExceptionHandler    proc    near public
global ASM_PFX(_C1eExceptionHandler)
ASM_PFX(_C1eExceptionHandler):
  push    ebp             ; C prolog
  mov     ebp, esp
  pushfd
  cli
  pushad

  ; Fix the return address on stack to skip offending
  ; code which caused the exception.
  mov    eax, DWORD PTR [ebp+8] 
  add    eax, 2
  mov    DWORD PTR [ebp+8], eax

  popad
  popfd
  pop    ebp
  add    esp, 4
  iretd
;_C1eExceptionHandler ENDP


;  end

