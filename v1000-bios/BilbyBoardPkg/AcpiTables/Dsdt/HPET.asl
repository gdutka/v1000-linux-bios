/** @file

;*******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#ifndef _HPET_
#define _HPET_

Device(HPET)
{
  Name(_HID,EISAID("PNP0103"))
  Method(_STA,0,NotSerialized)
  {
    If (LEqual (HPEN,0x01))
    {
      //
      // For Window Vista or above.
      //
      IF (LGreaterEqual (OSTB, 0x40))
      {
        Return(0x0F)
      }
      //
      // To disable HPET MMIO space decoding, HPET IRQ output and HPET
      // timer0~timer2 MSI capability.
      //
      Store (0x0, HPEN)
        //
        // The device is present.
        //
        Return (0x01)
    }
    //
    // The device is present.
    //
    Return (0x01)
  }

  Method(_CRS,0,NotSerialized)
  {
    Name(BUF0,ResourceTemplate()
      {
      IRQNoFlags() {0}
      IRQNoFlags() {8}
      Memory32Fixed (ReadOnly, 0xFED00000, 0x00000400, FMRD)
      })

    CreateDWordField (BUF0, FMRD._BAS, HPEB)
      Store (HPAD, Local0)
      And (Local0, 0xFFFFFC00, HPEB)
      Return(BUF0)
  }
}

#endif
