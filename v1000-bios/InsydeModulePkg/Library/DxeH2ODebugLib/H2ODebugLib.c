/** @file
  H2O Debug Library

;******************************************************************************
;* Copyright (c) 2012 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Protocol/DebugComm.h>

VOID*
DDTGetDebugComm (
  )
{
  VOID            *DebugComm = NULL;
#if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64)
  IA32_DESCRIPTOR Idt;
  UINT8           *IdtBase;
  UINT8           Int3Vector[sizeof (UINTN)];
  UINT8           *Ptr;
  
  AsmReadIdtr (&Idt);
  IdtBase = (UINT8*)(UINTN)Idt.Base;
#if defined(MDE_CPU_IA32)
  *(UINT16*)&Int3Vector[0] = *(UINT16*)&IdtBase[0x18];
  *(UINT16*)&Int3Vector[2] = *(UINT16*)&IdtBase[0x1e];
#else
  *(UINT16*)&Int3Vector[0] = *(UINT16*)&IdtBase[0x30];
  *(UINT16*)&Int3Vector[2] = *(UINT16*)&IdtBase[0x36];
  *(UINT32*)&Int3Vector[4] = *(UINT32*)&IdtBase[0x38];
#endif
  Ptr = *(UINT8**)(UINTN)Int3Vector;
  if (*(UINT32*)(UINTN)(Ptr - 0x10) == 0x44656267) {
    DebugComm = (VOID*)(UINTN)(*(UINT32*)(UINTN)(Ptr - 0x18));
  }
#endif
  return DebugComm;  
}

VOID
DDTPrint (
  IN CHAR8                        *Format,
  ...
  )
{
  H2O_DEBUG_COMM_PROTOCOL       *DebugComm;
  VA_LIST                       Args;

  DebugComm = DDTGetDebugComm();
  if (DebugComm) {
    VA_START (Args, Format);
    DebugComm->DDTPrint(DebugComm, Format, Args);
    VA_END (Args);
  }
}
