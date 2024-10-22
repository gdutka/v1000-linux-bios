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

#ifndef _PIC_
#define _PIC_
  //
  // for determing PIC mode
  //
  Name(\PICM,Zero)
  Name(\GPIC,Zero)

  Method(\_PIC, 1, NotSerialized) {
    Store(Arg0,\PICM)
    Store(Arg0,\GPIC)
    If (PICM) {
      \_SB.DSPI()           // clear interrupt at 0C00h/0C01h
      If (\NAPC) {
        \_SB.PCI0.NAPE()    // Clear D0F0xFC_x00[IoapicSbFeatureEn]
      }
    }
  }
#endif
