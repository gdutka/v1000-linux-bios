/** @file
 Function definition for the CMOS library.

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

#include <Library/IoLib.h>
#include <Library/AmdExtCmosLib.h>

#define AMD_EXT_CMOS_INDEX           0x72
#define AMD_EXT_CMOS_DATA            0x73
#define INTERRUPT_FLAG               BIT9

VOID
CmosDisableInterrupt (
  VOID
  );

VOID
CmosEableInterrupt (
  VOID
  );

UINT16
CmosGetCpuFlags (
  VOID
  );

/**
 Read an 8-bit value from the address offset of CMOS.

 @param[in] Address  Address offset of CMOS.

 @return An 8-bit value in the address offset of CMOS.
**/
UINT8
AmdReadExtCmos8 (
  IN UINT8                                 Address
  )
{
  UINT16        Eflags;
  UINT8         Value;

  Eflags = CmosGetCpuFlags ();
  CmosDisableInterrupt ();

  IoWrite8 (AMD_EXT_CMOS_INDEX, Address);
  Value = IoRead8 (AMD_EXT_CMOS_DATA);

  if (Eflags & INTERRUPT_FLAG) {
    CmosEableInterrupt ();
  }

  return Value;
}

/**
 Write an 8-bit value to the address offset of CMOS.

 @param[in] Address  Address offset of CMOS.
 @param[in] Data     Data written into CMOS.
**/
VOID
AmdWriteExtCmos8 (
  IN UINT8                                 Address,
  IN UINT8                                 Data
  )
{
  UINT16        Eflags;

  Eflags = CmosGetCpuFlags ();
  CmosDisableInterrupt ();

  IoWrite8 (AMD_EXT_CMOS_INDEX, Address);
  IoWrite8 (AMD_EXT_CMOS_DATA, Data);

  if (Eflags & INTERRUPT_FLAG) {
    CmosEableInterrupt ();
  }
  return;
}

/**
 Read a 16-bit value from the address offset of CMOS.

 @param[in] Address  Address offset of CMOS.

 @return A 16-bit value in the address offset of CMOS.
**/
UINT16
AmdReadExtCmos16 (
  IN UINT8                                 Address
  )
{
  UINT16   Temp16 = 0;
  UINT8    Temp8 = 0;

  Temp8 = AmdReadExtCmos8 (Address + 1);
  Temp16 = (UINT16) (Temp8 << 8);

  Temp8 = AmdReadExtCmos8 (Address);
  Temp16 = (UINT16) (Temp16 + Temp8);

  return Temp16;
}

/**
 Write a 16-bit value to the address offset of CMOS.

 @param[in] Address  Address offset of CMOS.
 @param[in] Data     Data written into CMOS.
**/
VOID
AmdWriteExtCmos16 (
  IN UINT8                                 Address,
  IN UINT16                                Data
  )
{
  UINT8    Temp8 = 0;

  Temp8 = (UINT8) (Data & 0x00FF);
  AmdWriteExtCmos8 (Address, Temp8);

  Temp8 = (UINT8) ((Data & 0xFF00)>>8);
  AmdWriteExtCmos8 (Address + 1, Temp8);

  return;
}

/**
 Read a 32-bit value from the address offset of CMOS.

 @param[in] Address  Address offset of CMOS.

 @return A 32-bit value in the address offset of CMOS.
**/
UINT32
AmdReadExtCmos32 (
  IN UINT8                                 Address
  )
{
  UINT32   Temp32 = 0;
  UINT8    Temp8 = 0;

  Temp8 = AmdReadExtCmos8 (Address + 3);
  Temp32 = (UINT32) (Temp8 << 24);

  Temp8 = AmdReadExtCmos8 (Address + 2);
  Temp32 = Temp32 + (UINT32) (Temp8 << 16);

  Temp8 = AmdReadExtCmos8 (Address + 1);
  Temp32 = Temp32 + (UINT32) (Temp8 << 8);

  Temp8 = AmdReadExtCmos8 (Address);
  Temp32 = (UINT32) (Temp32 + Temp8);

  return Temp32;
}

/**
 Write a 32-bit value to the address offset of CMOS.

 @param[in] Address  Address offset of CMOS.
 @param[in] Data     Data written into CMOS.
**/
VOID
AmdWriteExtCmos32 (
  IN UINT8                                 Address,
  IN UINT32                                Data
  )
{
  UINT8    Temp8 = 0;

  Temp8 = (UINT8)(Data&0x000000FF);
  AmdWriteExtCmos8 (Address, Temp8);

  Temp8 = (UINT8)((Data&0x0000FF00)>>8);
  AmdWriteExtCmos8 (Address + 1, Temp8);

  Temp8 = (UINT8)((Data&0x00FF0000)>>16);
  AmdWriteExtCmos8 (Address + 2, Temp8);

  Temp8 = (UINT8)((Data&0xFF000000)>>24);
  AmdWriteExtCmos8 (Address + 3, Temp8);

  return;
}
