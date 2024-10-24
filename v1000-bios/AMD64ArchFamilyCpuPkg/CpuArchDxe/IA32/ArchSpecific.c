/** @file

  Memory Operation Functions for IA32 Architecture.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "Cpu.h"
#include "MemoryAttribute.h"

extern EFI_CPU_INTERRUPT_HANDLER   mExternalVectorTable[];

INTERRUPT_HANDLER_TEMPLATE_MAP mTemplateMap;
INTERRUPT_GATE_DESCRIPTOR      *mIdtTable = NULL;
VOID                           *mInterruptHandler = NULL;
INTERRUPT_GATE_DESCRIPTOR      *mOrigIdtEntry = NULL;
UINT16                         mOrigIdtEntryCount = 0;

UINT64   mValidMtrrAddressMask = MTRR_LIB_CACHE_VALID_ADDRESS;
UINT64   mValidMtrrBitsMask    = MTRR_LIB_MSR_VALID_MASK;

/**
  Creates an IDT and a new GDT in RAM.

  This function creates an IDT and a new GDT in RAM.

**/
VOID
InitializeDescriptorTables (
  VOID
  )
{
  UINT16                          CodeSegment;
  INTERRUPT_GATE_DESCRIPTOR       *IdtEntry;
  UINTN                           Index;
  INTERRUPT_GATE_DESCRIPTOR       *IdtTable;
  UINT8                           *InterruptHandler;
  IA32_DESCRIPTOR                 Idtr;

  //
  // Load Global Descriptor Table and update segment selectors
  //
  InitializeGdt ();

  //
  // Create Interrupt Descriptor Table
  //
  IdtTable = AllocateZeroPool (sizeof (INTERRUPT_GATE_DESCRIPTOR) * INTERRUPT_VECTOR_NUMBER);

  GetTemplateAddressMap (&mTemplateMap);
  InterruptHandler  = AllocatePool (mTemplateMap.Size * INTERRUPT_VECTOR_NUMBER);

  CodeSegment       = AsmReadCs ();

  //
  // Get original IDT address and size and save original IDT entry.
  //
  AsmReadIdtr ((IA32_DESCRIPTOR *) &Idtr);
  mOrigIdtEntryCount = (UINT16) ((Idtr.Limit + 1) / sizeof (INTERRUPT_GATE_DESCRIPTOR));
  ASSERT (mOrigIdtEntryCount <= INTERRUPT_VECTOR_NUMBER);
  mOrigIdtEntry = AllocateCopyPool (Idtr.Limit + 1, (VOID *) Idtr.Base);
  ASSERT (mOrigIdtEntry != NULL);

  //
  // Copy original IDT entry.
  //
  IdtEntry = IdtTable;
  CopyMem (IdtEntry, (VOID *) Idtr.Base, Idtr.Limit + 1);

  //
  // Save IDT table pointer and Interrupt table pointer
  //
  mIdtTable         = IdtTable;
  mInterruptHandler = InterruptHandler;

  for (Index = 0; Index < INTERRUPT_VECTOR_NUMBER; Index ++) {
    //
    // Update all IDT entries to use current CS value.
    //
    IdtEntry[Index].SegmentSelector = CodeSegment;
    if (Index < mOrigIdtEntryCount) {
      //
      // Skip original IDT entry.
      //
      continue;
    }
    //
    // Set the address of interrupt handler to the rest IDT entry.
    //
    SetInterruptDescriptorTableHandlerAddress (Index);
  }

  InitializeIdt (
    &(mExternalVectorTable[0]),
    (UINTN *) IdtTable,
    (UINT16) (sizeof (INTERRUPT_GATE_DESCRIPTOR) * INTERRUPT_VECTOR_NUMBER)
    );

  //
  // Initialize Exception Handlers
  //
  InitializeException (mOrigIdtEntryCount);
}

/**
  Set Interrupt Descriptor Table Handler Address.

  @param Index        The Index of the interrupt descriptor table handle.

**/
VOID
SetInterruptDescriptorTableHandlerAddress (
  IN UINTN       Index
  )
{
  UINTN          ExceptionHandle;

  //
  // Get the address of handler for entry
  //
  ExceptionHandle = (UINTN)mInterruptHandler + Index * mTemplateMap.Size;
  CopyMem ((VOID *)ExceptionHandle, mTemplateMap.Start, mTemplateMap.Size);
  *(UINT32 *) (ExceptionHandle + mTemplateMap.FixOffset) = Index;

  mIdtTable[Index].OffsetLow  = (UINT16) ExceptionHandle;
  mIdtTable[Index].Attributes = INTERRUPT_GATE_ATTRIBUTE;
  //
  // 8e00;
  //
  mIdtTable[Index].OffsetHigh = (UINT16) (ExceptionHandle >> 16);
}

/**
  Restore original Interrupt Descriptor Table Handler Address.

  @param Index        The Index of the interrupt descriptor table handle.

**/
VOID
RestoreInterruptDescriptorTableHandlerAddress (
  IN UINTN       Index
  )
{
  if (Index < mOrigIdtEntryCount) {
    mIdtTable[Index].OffsetLow  = mOrigIdtEntry[Index].OffsetLow;
    mIdtTable[Index].OffsetHigh = mOrigIdtEntry[Index].OffsetHigh;
  }
}
