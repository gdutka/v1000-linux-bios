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

#endif
