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
;;   32 bit Sent SMI to call IHISI flash ROM part
;;

  SECTION .text

;------------------------------------------------------------------------------
;  VOID
;  IhisiLibGenericSmi (
;    IN OUT     IHISI_LIB_SMI_DATA	        *SmiData  // ebp + 08h
;    );
;------------------------------------------------------------------------------
global ASM_PFX(IhisiLibGenericSmi)
ASM_PFX(IhisiLibGenericSmi):
  push    ebp
  mov     ebp, esp
  push    eax
  push    ebx
  push    esi
  push    edi
  push    ecx
  push    edx

  mov     eax, [ebp + 0x08]
  mov     ebx, [eax + 4]
  mov     ecx, [eax + 8]
  mov     edx, [eax + 12]
  mov     esi, [eax + 16]
  mov     edi, [eax + 20]
  mov     eax, [eax]

  out     dx, al

  mov     eax, [ebp + 0x08]
  mov     [eax + 4],  ebx
  mov     [eax + 8],  ecx
  mov     [eax + 12], edx
  mov     [eax + 16], esi
  mov     [eax + 20], edi

  pop     edx
  pop     ecx
  pop     edi
  pop     esi
  pop     ebx
  pop     eax
  leave
  ret

