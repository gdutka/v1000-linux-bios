/** @file
  Provid Memfence IA32 instruction for SpiAccessLib.

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
VOID
EFIAPI
MemFence (
  VOID
  )
{
  _asm {
    mfence
  }
}

