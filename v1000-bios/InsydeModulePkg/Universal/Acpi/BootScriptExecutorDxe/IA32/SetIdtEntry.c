/** @file
  Set a IDT entry for debug purpose

  Set a IDT entry for interrupt vector 3 for debug purpose for IA32 platform
;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/** @file
Copyright (c) 2006 - 2012, Intel Corporation. All rights reserved.<BR>

This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include "ScriptExecute.h"

/**
  Set a IDT entry for interrupt vector 3 for debug purpose.

  @param  AcpiS3Context  a pointer to a structure of ACPI_S3_CONTEXT

**/
VOID
SetIdtEntry (
  IN ACPI_S3_CONTEXT     *AcpiS3Context
  )
{
  IA32_IDT_GATE_DESCRIPTOR                      *IdtEntry;
  IA32_DESCRIPTOR                               *IdtDescriptor;
  UINTN                                         S3DebugBuffer;
  EFI_STATUS                                    Status;

  //
  // Restore IDT for debug
  //
  IdtDescriptor = (IA32_DESCRIPTOR *) (UINTN) (AcpiS3Context->IdtrProfile);
  AsmWriteIdtr (IdtDescriptor);

  //
  // Setup the default CPU exception handlers
  //
  Status = InitializeCpuExceptionHandlers (NULL);
  ASSERT_EFI_ERROR (Status);

  DEBUG_CODE (
    //
    // Update IDT entry INT3 if the instruction is valid in it
    //
    S3DebugBuffer = (UINTN) (AcpiS3Context->S3DebugBufferAddress);
    if (*(UINTN *)S3DebugBuffer != (UINTN) -1) {
      IdtEntry = (IA32_IDT_GATE_DESCRIPTOR *)(IdtDescriptor->Base + (3 * sizeof (IA32_IDT_GATE_DESCRIPTOR)));
      IdtEntry->Bits.OffsetLow      = (UINT16)S3DebugBuffer;
      IdtEntry->Bits.Selector       = (UINT16)AsmReadCs ();
      IdtEntry->Bits.Reserved_0     = 0;
      IdtEntry->Bits.GateType       = IA32_IDT_GATE_TYPE_INTERRUPT_32;
      IdtEntry->Bits.OffsetHigh     = (UINT16)(S3DebugBuffer >> 16);
    }
  );
}

