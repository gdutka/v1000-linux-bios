/** @file

  Definition for IA32 processor

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

#ifndef _PROCESSOR_DEF_H_
#define _PROCESSOR_DEF_H_

#include <Protocol/Cpu.h>

#pragma pack(1)

typedef struct {
  UINT16  OffsetLow;
  UINT16  SegmentSelector;
  UINT16  Attributes;
  UINT16  OffsetHigh;
} INTERRUPT_GATE_DESCRIPTOR;

#pragma pack()

typedef struct {
  VOID  *Start;
  UINTN Size;
  UINTN FixOffset;
} INTERRUPT_HANDLER_TEMPLATE_MAP;

/**
  Return address map of interrupt handler template so that C code can generate
  interrupt handlers, and dynamically do address fix.

  @param AddressMap  Pointer to a buffer where the address map is returned.
**/
VOID
EFIAPI
GetTemplateAddressMap (
  OUT INTERRUPT_HANDLER_TEMPLATE_MAP *AddressMap
  );

/**
  Creates an IDT table starting at IdtTablPtr. It has IdtLimit/8 entries.
  Table is initialized to intxx where xx is from 00 to number of entries or
  100h, whichever is smaller. After table has been initialized the LIDT
  instruction is invoked.
 
  TableStart is the pointer to the callback table and is not used by 
  InitializedIdt but by commonEntry. CommonEntry handles all interrupts,
  does the context save and calls the callback entry, if non-NULL.
  It is the responsibility of the callback routine to do hardware EOIs.

  @param TableStart     Pointer to interrupt callback table.
  @param IdtTablePtr    Pointer to IDT table.
  @param IdtTableLimit  IDT Table limit (number of interrupt entries * 8).
**/
VOID
EFIAPI
InitializeIdt (
  IN EFI_CPU_INTERRUPT_HANDLER      *TableStart,
  IN UINTN                          *IdtTablePtr,
  IN UINT16                         IdtTableLimit
  );

#endif
