;;******************************************************************************
;;* Copyright (c) 1983-2020, Insyde Software Corp. All Rights Reserved.
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
;;   CallSmmSecureBoot.nasm
;;
;; Abstract:
;;
;;   32 bit Sent SMI to call Secure Boot function
;;

  SECTION .text

;------------------------------------------------------------------------------
;  UINT8
;  SmmSecureBootCall (
;    IN     UINT8            *InPutBuff,  // ebp + 08h
;    IN     UINTN            DataSize,    // ebp + 0Ch
;    IN     UINT8            SubFunNum,   // ebp + 10h
;    IN     UINT16           SmiPort      // ebp + 14h
;    );
;------------------------------------------------------------------------------
;SmmSecureBootCall PROC       PUBLIC InPutBuff:DWORD, DataSize:DWORD, SubFunNum:BYTE, SmiPort:WORD
global ASM_PFX(SmmSecureBootCall)
ASM_PFX(SmmSecureBootCall):
  push  ebp
  mov   ebp, esp
  push  ebx
  push  ecx
  push  edx
  push  edi
  push  esi

  mov  ebx, [ebp + 0x0C] ; DataSize
  mov  esi, [ebp + 0x08] ; InPutBuff
  mov  al,  [ebp + 0x10] ; SubFunNum
  mov  ah,  al
  mov  al,  0xEC
  mov  dx,  [ebp + 0x14] ; SmiPort
  out  dx,  al

  pop esi
  pop edi
  pop edx
  pop ecx
  pop ebx
  leave
  ret

