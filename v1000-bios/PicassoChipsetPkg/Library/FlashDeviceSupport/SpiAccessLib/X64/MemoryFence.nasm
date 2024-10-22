;; @file
; Provid Memfence X64 instruction for SpiAccessLib.
;
;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*

  SECTION .text

DEFAULT REL

;------------------------------------------------------------------------------
; VOID
; EFIAPI
; MemFence (
;   VOID
;   );
;------------------------------------------------------------------------------
;MemFence   PROC
global ASM_PFX(MemFence)
ASM_PFX(MemFence):
  mfence
  ret
;MemFence   ENDP

;END
