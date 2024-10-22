/** @file
  The instance of PostCodeConfigLib Library
  This file is X64 architecture implement

;******************************************************************************
;* Copyright (c) 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/PostCodeConfigLib.h>

/**
 Register a new handler to the specified IDT entry.
 The Handler address is 64 bit.
 
 @param [in] VectorNumber   The vector number of IDT entry
 @param [in] Handler        The new handler

**/
VOID
EFIAPI
RegisterIdtHandler (
  IN UINT8  VectorNumber,
  IN UINT64 Handler
  )
{
  IA32_DESCRIPTOR           Idtr;
  INTERRUPT_GATE_DESCRIPTOR *IdtBase;

  //
  // Get the IDT table address
  //
  AsmReadIdtr (&Idtr);
  IdtBase = (INTERRUPT_GATE_DESCRIPTOR*) Idtr.Base;

  //
  // Assign the Handler address to the specified entry of IDT
  //
  IdtBase[VectorNumber].Offset15To0     = (UINT16) Handler;
  IdtBase[VectorNumber].SegmentSelector = AsmReadCs ();
  IdtBase[VectorNumber].Attributes      = INTERRUPT_GATE_ATTRIBUTE;
  IdtBase[VectorNumber].Offset31To16    = (UINT16) (Handler >> 16);
  IdtBase[VectorNumber].Offset63To32    = (UINT32) (Handler >> 32);
}

