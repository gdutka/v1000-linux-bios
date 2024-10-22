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

#ifndef _LID_
#define _LID_

Device (LID)
{
  Name(_HID, EISAID("PNP0C0D"))   // ID for LID
  Method(_LID)
  {
    If(\_SB.PCI0.LPC0.ECOK()) {
      Store(0xFF, Local0)
      if (LNot(Acquire(\_SB.PCI0.LPC0.EC0.Z009, 300))) {
        Store(\_SB.PCI0.LPC0.EC0.GPO3, Local0)
        Release(\_SB.PCI0.LPC0.EC0.Z009)
      }
      if (And(Local0, 0x08)) {
        Return(1)   // Lid Open
      } else {
        Return(0)   // Lid Closed
      }
    }
    Return(1)       // Lid Open
  }
}
#endif
