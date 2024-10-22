;;******************************************************************************
;;* Copyright (c) 2023, Insyde Software Corp. All Rights Reserved.
;;*
;;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;;* transmit, broadcast, present, recite, release, license or otherwise exploit
;;* any part of this publication in any form, by any means, without the prior
;;* written permission of Insyde Software Corporation.
;;*
;;******************************************************************************
;;
;; Module Name:
;;
;;   CallSmmIhisi.nasm
;;
;; Abstract:
;;
;;   64 bit Sent SMI to call IHISI flash ROM part
;;

SEGMENT .text

;------------------------------------------------------------------------------
;  VOID
;  IhisiLibGenericSmi (
;    IN OUT     IHISI_LIB_SMI_DATA	        *SmiData       // rcx
;    );
;------------------------------------------------------------------------------
global ASM_PFX(IhisiLibGenericSmi)
ASM_PFX(IhisiLibGenericSmi):

  push    rax
  push    rbx
  push    rsi
  push    rdi
  push    rcx
  push    rdx
  push    r8
  push    r9

  mov     r8,  rcx
  mov     r9,  rdx

  mov     eax, dword [r8]
  mov     ebx, dword [r8 + 4]
  mov     ecx, dword [r8 + 8]
  mov     edx, dword [r8 + 12]
  mov     esi, dword [r8 + 16]
  mov     edi, dword [r8 + 20]

  out     dx, al

  mov     dword [r8],      eax
  mov     dword [r8 + 4],  ebx
  mov     dword [r8 + 8],  ecx
  mov     dword [r8 + 12], edx
  mov     dword [r8 + 16], esi
  mov     dword [r8 + 20], edi

  pop     r9
  pop     r8
  pop     rdx
  pop     rcx
  pop     rdi
  pop     rsi
  pop     rbx
  pop     rax
  ret
