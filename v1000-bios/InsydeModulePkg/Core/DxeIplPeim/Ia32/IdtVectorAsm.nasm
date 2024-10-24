;/** @file
;  IDT vector entry.
;******************************************************************************
;* Copyright (c) 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
;**/
;/** @file
;
;    IDT vector entry.
;
;  Copyright (c) 2007 - 2008, Intel Corporation. All rights reserved.<BR>
;  This program and the accompanying materials
;  are licensed and made available under the terms and conditions of the BSD License
;  which accompanies this distribution.  The full text of the license may be found at
;  http://opensource.org/licenses/bsd-license.php
;
;  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
;  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
;
;**/

    SECTION .text

;
;------------------------------------------------------------------------------
;  Generic IDT Vector Handlers for the Host.
;
;------------------------------------------------------------------------------

ALIGN   8
global ASM_PFX(AsmGetVectorTemplatInfo)
global ASM_PFX(AsmVectorFixup)
global ASM_PFX(VectorTemplateBase)
global ASM_PFX(VectorTemplateEnd)


global ASM_PFX(AsmVectorFixup)

@VectorTemplateBase:
        push  eax
        db    0x6a       ; push #VectorNumber
@VectorNum:
        db    0
        mov   eax, [ASM_PFX(CommonInterruptEntry)]
        jmp   eax
@VectorTemplateEnd:

global ASM_PFX(AsmGetVectorTemplatInfo)
ASM_PFX(AsmGetVectorTemplatInfo):
        mov   ecx, [esp + 4]
        mov   [ecx], dword @VectorTemplateBase
        mov   eax, (@VectorTemplateEnd - @VectorTemplateBase)
        ret

global ASM_PFX(AsmVectorFixup)
ASM_PFX(AsmVectorFixup):
        mov   eax, dword [esp + 8]
        mov   ecx, [esp + 4]
        mov   [ecx + (@VectorNum - @VectorTemplateBase)], al
        ret

;---------------------------------------;
; CommonInterruptEntry                  ;
;---------------------------------------;
; The follow algorithm is used for the common interrupt routine.

;
; +---------------------+ <-- 16-byte aligned ensured by processor
; +    Old SS           +
; +---------------------+
; +    Old RSP          +
; +---------------------+
; +    RFlags           +
; +---------------------+
; +    CS               +
; +---------------------+
; +    RIP              +
; +---------------------+
; +    Error Code       +
; +---------------------+
; +    Vector Number    +
; +---------------------+

global ASM_PFX(CommonInterruptEntry)
ASM_PFX(CommonInterruptEntry):
  cli

  jmp $

