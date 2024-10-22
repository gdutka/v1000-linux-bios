;******************************************************************************
;* Copyright (c) 2012 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
;------------------------------------------------------------------------------ ;
; Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
; This program and the accompanying materials
; are licensed and made available under the terms and conditions of the BSD License
; which accompanies this distribution.  The full text of the license may be found at
; http://opensource.org/licenses/bsd-license.php.
;
; THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
; WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
;
; Module Name:
;
;   AsmFuncs.Asm
;
; Abstract:
;
;   Assembly function to set segment selectors.
;
; Notes:
;
;------------------------------------------------------------------------------

SECTION .text

;------------------------------------------------------------------------------
; VOID
; EFIAPI
; AsmSetDataSelectors (
;   IN UINT16   SelectorValue
;   );
;------------------------------------------------------------------------------
global ASM_PFX(AsmSetDataSelectors)
ASM_PFX(AsmSetDataSelectors):
  mov     eax, [esp + 4]
o16 mov     ds, ax
o16 mov     es, ax
o16 mov     fs, ax
o16 mov     gs, ax
o16 mov     ss, ax
  ret

