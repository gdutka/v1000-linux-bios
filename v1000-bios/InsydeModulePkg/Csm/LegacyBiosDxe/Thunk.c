/** @file
  Call into 16-bit BIOS code, Use AsmThunk16 function of BaseLib.

;******************************************************************************
;* Copyright (c) 2012 - 2018, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "LegacyBiosInterface.h"

THUNK_CONTEXT      mThunkContext;

/**
  Sets the counter value for Timer #0 in a legacy 8254 timer.

  @param  Count - The 16-bit counter value to program into Timer #0 of the legacy 8254 timer.

**/
VOID
SetPitCount (
  IN UINT16  Count
  )
{
  IoWrite8 (TIMER_CONTROL_PORT, TIMER0_CONTROL_WORD);
  IoWrite8 (TIMER0_COUNT_PORT, (UINT8) (Count & 0xFF));
  IoWrite8 (TIMER0_COUNT_PORT, (UINT8) ((Count>>8) & 0xFF));
}

/**
  Thunk to 16-bit real mode and execute a software interrupt with a vector
  of BiosInt. Regs will contain the 16-bit register context on entry and
  exit.

  @param  This    Protocol instance pointer.
  @param  BiosInt Processor interrupt vector to invoke
  @param  Regs    Register contexted passed into (and returned) from thunk to
                  16-bit mode

  @retval FALSE   Thunk completed, and there were no BIOS errors in the target code.
                  See Regs for status.
  @retval TRUE    There was a BIOS erro in the target code.

**/
BOOLEAN
EFIAPI
LegacyBiosInt86 (
  IN  EFI_LEGACY_BIOS_PROTOCOL      *This,
  IN  UINT8                         BiosInt,
  IN  EFI_IA32_REGISTER_SET         *Regs
  )
{
  UINT32  *VectorBase;

  Regs->X.Flags.Reserved1 = 1;
  Regs->X.Flags.Reserved2 = 0;
  Regs->X.Flags.Reserved3 = 0;
  Regs->X.Flags.Reserved4 = 0;
  Regs->X.Flags.IOPL      = 3;
  Regs->X.Flags.NT        = 0;
  Regs->X.Flags.IF        = 0;
  Regs->X.Flags.TF        = 0;
  Regs->X.Flags.CF        = 0;
  //
  // The base address of legacy interrupt vector table is 0.
  // We use this base address to get the legacy interrupt handler.
  //
  VectorBase              = (UINT32 *) (UINTN) (BiosInt * 4);  

  return InternalLegacyBiosFarCall (
           This,
           (UINT16) ((*VectorBase) >> 16),
           (UINT16) (*VectorBase),
           Regs,
           &Regs->X.Flags,
           sizeof (Regs->X.Flags)
           );
}

/**
  Thunk to 16-bit real mode and call Segment:Offset. Regs will contain the
  16-bit register context on entry and exit. Arguments can be passed on
  the Stack argument

  @param  This                   Protocol instance pointer.
  @param  Segment                Segemnt of 16-bit mode call
  @param  Offset                 Offset of 16-bit mdoe call
  @param  Regs                   Register contexted passed into (and returned) from
                                 thunk to  16-bit mode
  @param  Stack                  Caller allocated stack used to pass arguments
  @param  StackSize              Size of Stack in bytes

  @retval FALSE                  Thunk completed, and there were no BIOS errors in
                                 the target code. See Regs for status.
  @retval TRUE                   There was a BIOS erro in the target code.

**/
BOOLEAN
EFIAPI
LegacyBiosFarCall86 (
  IN  EFI_LEGACY_BIOS_PROTOCOL        *This,
  IN  UINT16                          Segment,
  IN  UINT16                          Offset,
  IN  EFI_IA32_REGISTER_SET           *Regs,
  IN  VOID                            *Stack,
  IN  UINTN                           StackSize
  )
{
  Regs->X.Flags.Reserved1 = 1;
  Regs->X.Flags.Reserved2 = 0;
  Regs->X.Flags.Reserved3 = 0;
  Regs->X.Flags.Reserved4 = 0;
  Regs->X.Flags.IOPL      = 3;
  Regs->X.Flags.NT        = 0;
  Regs->X.Flags.IF        = 1;
  Regs->X.Flags.TF        = 0;
  Regs->X.Flags.CF        = 0;

  return InternalLegacyBiosFarCall (This, Segment, Offset, Regs, Stack, StackSize);
}

/**
  Provide NULL interrupt handler which is used to check
  if there is more than one HW interrupt registers with the CPU AP.

  @param  InterruptType - The type of interrupt that occured
  @param  SystemContext - A pointer to the system context when the interrupt occured

**/
VOID
EFIAPI
LegacyBiosNullInterruptHandler (
  IN EFI_EXCEPTION_TYPE   InterruptType,
  IN EFI_SYSTEM_CONTEXT   SystemContext
  )
{
}

/**
  Thunk to 16-bit real mode and call Segment:Offset. Regs will contain the
  16-bit register context on entry and exit. Arguments can be passed on
  the Stack argument

  @param  This       Protocol instance pointer.
  @param  Segment    Segemnt of 16-bit mode call
  @param  Offset     Offset of 16-bit mdoe call
  @param  Regs       Register contexted passed into (and returned) from thunk to
                     16-bit mode
  @param  Stack      Caller allocated stack used to pass arguments
  @param  StackSize  Size of Stack in bytes

  @retval FALSE      Thunk completed, and there were no BIOS errors in the target code.
                     See Regs for status.
  @retval TRUE       There was a BIOS erro in the target code.

**/
BOOLEAN
EFIAPI
InternalLegacyBiosFarCall (
  IN  EFI_LEGACY_BIOS_PROTOCOL        *This,
  IN  UINT16                          Segment,
  IN  UINT16                          Offset,
  IN  EFI_IA32_REGISTER_SET           *Regs,
  IN  VOID                            *Stack,
  IN  UINTN                           StackSize
  )
{
  UINTN                 Status;
  LEGACY_BIOS_INSTANCE  *Private;
  UINT16                *Stack16;
  EFI_TPL               OriginalTpl;
  IA32_REGISTER_SET     ThunkRegSet;
  BOOLEAN               InterruptState;
  UINT32                BiosStart;

  Private = LEGACY_BIOS_INSTANCE_FROM_THIS (This);

  ZeroMem (&ThunkRegSet, sizeof (ThunkRegSet));
  ThunkRegSet.E.EDI   = Regs->E.EDI;
  ThunkRegSet.E.ESI   = Regs->E.ESI;
  ThunkRegSet.E.EBP   = Regs->E.EBP;
  ThunkRegSet.E.EBX   = Regs->E.EBX;
  ThunkRegSet.E.EDX   = Regs->E.EDX;
  //
  // Sometimes, ECX is used to pass in 32 bit data. For example, INT 1Ah, AX = B10Dh is
  // "PCI BIOS v2.0c + Write Configuration DWORD" and ECX has the dword to write.
  //
  ThunkRegSet.E.ECX   = Regs->E.ECX;
  ThunkRegSet.E.EAX   = Regs->E.EAX;
  ThunkRegSet.E.SS   = Regs->E.SS;
  ThunkRegSet.E.CS   = Regs->E.CS;
  ThunkRegSet.E.DS   = Regs->E.DS;
  ThunkRegSet.E.ES   = Regs->E.ES;

  CopyMem (&(ThunkRegSet.E.EFLAGS.UintN), &(Regs->X.Flags), sizeof (Regs->X.Flags));

  //
  // Clear the error flag; thunk code may set it. Stack16 should be the high address
  // Make Statk16 address the low 16 bit must be not zero.
  //
  Stack16 = (UINT16 *)((UINT8 *) mThunkContext.RealModeBuffer + OFFSET_OF (LOW_MEMORY_THUNK, Stack[0]) + LOW_STACK_SIZE - sizeof (UINT16));

  //
  // Save and disable interrupt of debug timer
  //
  InterruptState = SaveAndSetDebugTimerInterrupt (FALSE);

  //
  // The call to Legacy16 is a critical section to EFI
  //
  OriginalTpl = gBS->RaiseTPL (TPL_HIGH_LEVEL);

  //
  // Check to see if there is more than one HW interrupt registers with the CPU AP.
  // If there is, then ASSERT() since that is not compatible with the CSM because
  // interupts other than the Timer interrupt that was disabled above can not be
  // handled properly from real mode.
  //
  DEBUG_CODE (
    UINTN  Vector;
    UINTN  Count;

    for (Vector = 0x20, Count = 0; Vector < 0x100; Vector++) {
      Status = Private->Cpu->RegisterInterruptHandler (Private->Cpu, Vector, LegacyBiosNullInterruptHandler);
      if (Status == EFI_ALREADY_STARTED) {
        Count++;
      }
      if (Status == EFI_SUCCESS) {
        Private->Cpu->RegisterInterruptHandler (Private->Cpu, Vector, NULL);
      }
    }
    if (Count >= 2) {
      DEBUG ((EFI_D_ERROR, "ERROR: More than one HW interrupt active with CSM enabled\n"));
    }
    ASSERT (Count < 2);
  );

  if (Stack != NULL && StackSize != 0) {
    //
    // Copy Stack to low memory stack
    //
    Stack16 -= StackSize / sizeof (UINT16);
    CopyMem (Stack16, Stack, StackSize);
  }

  ThunkRegSet.E.SS   = (UINT16) ((UINTN) ((UINT8*)mThunkContext.RealModeBuffer + OFFSET_OF (LOW_MEMORY_THUNK, Stack[0])) >> 4);
  ThunkRegSet.E.ESP  = (UINT16) ((UINTN) ((UINT8*)Stack16 - ((UINTN)ThunkRegSet.E.SS << 4)));
  ThunkRegSet.E.CS   = Segment;
  ThunkRegSet.E.Eip  = Offset;

  mThunkContext.RealModeState      = &ThunkRegSet;

  //
  // Set Legacy16 state. 0x08, 0x70 is legacy 8259 vector bases.
  //
  Status = Private->Legacy8259->SetMode (Private->Legacy8259, Efi8259LegacyMode, NULL, NULL);
  ASSERT_EFI_ERROR (Status);

  AsmThunk16 (&mThunkContext);

  //
  // OPROM may allocate EBDA range by itself and change EBDA base and EBDA size.
  // Get the current EBDA base address, and compared with pre-allocate minimum
  // EBDA base address, if the current EBDA base address is smaller, it indicates
  // PcdEbdaReservedMemorySize should be adjusted to larger for more OPROMs.
  //
  DEBUG_CODE (
    {
      UINTN                 EbdaBaseAddress;
      UINTN                 ReservedEbdaBaseAddress;

      EbdaBaseAddress = (UINTN)(*(UINT16 *) (UINTN) 0x40E) << 4;
      ReservedEbdaBaseAddress = CONVENTIONAL_MEMORY_TOP - PcdGet32 (PcdEbdaReservedMemorySize);
      ASSERT (ReservedEbdaBaseAddress <= EbdaBaseAddress);
    }
  );

  if (Stack != NULL && StackSize != 0) {
    //
    // Copy low memory stack to Stack
    //
    CopyMem (Stack, Stack16, StackSize);
  }

  //
  // Restore protected mode interrupt state
  //
  Status = Private->Legacy8259->SetMode (Private->Legacy8259, Efi8259ProtectedMode, NULL, NULL);
  ASSERT_EFI_ERROR (Status);

  mThunkContext.RealModeState = NULL;

  //
  // End critical section
  //
  gBS->RestoreTPL (OriginalTpl);

  //
  // Restore interrupt of debug timer
  //
  SaveAndSetDebugTimerInterrupt (InterruptState);

  //
  // Update BiosStart upon CSM GetTableAddress function call to mark the actual CSM16 lower boundary
  //
  if (Segment == Private->Legacy16CallSegment &&
      Offset == Private->Legacy16CallOffset &&
      Regs->E.EAX == Legacy16GetTableAddress &&
      ThunkRegSet.X.AX == 0) {
    BiosStart = (UINT32)(ThunkRegSet.E.DS * 16 + ThunkRegSet.E.EBX);
    if (Private->BiosStart > BiosStart) {
      Private->BiosStart = BiosStart;
      Private->LegacyBiosImageSize = 0x100000 - Private->BiosStart;
    }
  }

  Regs->E.EDI      = ThunkRegSet.E.EDI;
  Regs->E.ESI      = ThunkRegSet.E.ESI;
  Regs->E.EBP      = ThunkRegSet.E.EBP;
  Regs->E.EBX      = ThunkRegSet.E.EBX;
  Regs->E.EDX      = ThunkRegSet.E.EDX;
  Regs->E.ECX      = ThunkRegSet.E.ECX;
  Regs->E.EAX      = ThunkRegSet.E.EAX;
  Regs->X.SS       = ThunkRegSet.E.SS;
  Regs->X.CS       = ThunkRegSet.E.CS;
  Regs->X.DS       = ThunkRegSet.E.DS;
  Regs->X.ES       = ThunkRegSet.E.ES;

  CopyMem (&(Regs->X.Flags), &(ThunkRegSet.E.EFLAGS.UintN), sizeof (Regs->X.Flags));

  return (BOOLEAN) (Regs->X.Flags.CF == 1);
}

/**
  Allocate memory < 1 MB and copy the thunker code into low memory. Se up
  all the descriptors.

  @param  Private                Private context for Legacy BIOS

  @retval EFI_SUCCESS            Should only pass.

**/
EFI_STATUS
LegacyBiosInitializeThunk (
  IN  LEGACY_BIOS_INSTANCE    *Private
  )
{
  EFI_STATUS              Status;
  EFI_PHYSICAL_ADDRESS    MemoryAddress;
  UINT8                   TimerVector;

  MemoryAddress   = (EFI_PHYSICAL_ADDRESS) (UINTN) Private->IntThunk;

  mThunkContext.RealModeBuffer     = (VOID *) (UINTN) (MemoryAddress + OFFSET_OF (LOW_MEMORY_THUNK, Code[0]));
  mThunkContext.RealModeBufferSize = LOW_CODE_SIZE;
  mThunkContext.ThunkAttributes    = THUNK_ATTRIBUTE_BIG_REAL_MODE | THUNK_ATTRIBUTE_DISABLE_A20_MASK_INT_15;

  AsmPrepareThunk16 (&mThunkContext);

  //
  // Get the interrupt vector number corresponding to IRQ0 from the 8259 driver
  //
  TimerVector = 0;
  Status = Private->Legacy8259->GetVector (Private->Legacy8259, Efi8259Irq0, &TimerVector);
  ASSERT_EFI_ERROR (Status);

  //
  // Check to see if the Timer AP has hooked the IRQ0 from the 8254 PIT
  //
  Status = Private->Cpu->RegisterInterruptHandler (
                           Private->Cpu,
                           TimerVector,
                           LegacyBiosNullInterruptHandler
                           );
  if (Status == EFI_SUCCESS) {
    //
    // If the Timer AP has not enabled the 8254 timer IRQ, then force the 8254 PIT
    // counter to 0, which is the CSM required rate of 54.9254 ms
    //
    Private->Cpu->RegisterInterruptHandler (
                    Private->Cpu,
                    TimerVector,
                    NULL
                    );
    SetPitCount (0);

    //
    // Save status that the Timer AP is not using the 8254 PIT
    //
    Private->TimerUses8254 = FALSE;
  } else if (Status == EFI_ALREADY_STARTED) {
    //
    // Save status that the Timer AP is using the 8254 PIT
    //
    Private->TimerUses8254 = TRUE;
  } else {
    //
    // Unexpected status from CPU AP RegisterInterruptHandler()
    //
    ASSERT (FALSE);
  }

  return EFI_SUCCESS;
}
