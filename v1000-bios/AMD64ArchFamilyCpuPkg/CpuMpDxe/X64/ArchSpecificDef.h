/** @file

  Definition for AMD64 processor.

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

#ifndef _ARCH_SPECIFIC_DEF_H_
#define _ARCH_SPECIFIC_DEF_H_

#include <PiDxe.h>
#include <Protocol/Cpu.h>

#pragma pack(1)

typedef struct {
  UINT16    Offset15To0;
  UINT16    SegmentSelector;
  UINT16    Attributes;
  UINT16    Offset31To16;
  UINT32    Offset63To32;
  UINT32    Reserved;
} INTERRUPT_GATE_DESCRIPTOR;

#pragma pack()

typedef struct {
  UINT8 *RendezvousFunnelAddress;
  UINTN PModeEntryOffset;
  UINTN FlatJumpOffset;
  UINTN LModeEntryOffset;
  UINTN LongJumpOffset;
  UINTN Size;
  UINTN ProtectedModeEntryPointOffset;
} MP_ASSEMBLY_ADDRESS_MAP;

/**
  Get address map of RendezvousFunnelProc.

  This function gets address map of RendezvousFunnelProc.

  @param  AddressMap   Output buffer for address map information

**/
VOID
EFIAPI
AsmGetAddressMap (
  OUT MP_ASSEMBLY_ADDRESS_MAP    *AddressMap
  );

#endif
