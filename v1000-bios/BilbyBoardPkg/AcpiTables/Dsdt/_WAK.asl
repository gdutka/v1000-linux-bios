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

#ifndef _WAK_
#define _WAK_

  Method(\_WAK, 1) {
    SWAK (Arg0)

    \_SB.AWAK (Arg0)

    If (Lor(LEqual(ARG0, 0x03), LEqual(ARG0, 0x04))) {
      If (LNotEqual(\GPIC,0)) {
        \_SB.DSPI()
        If (\NAPC) {
          \_SB.PCI0.NAPE()    // Clear D0F0xFC_x00[IoapicSbFeatureEn]
        }
      }
    }

    If (LEqual(ARG0, 0x03)) {
      \_SB.S80H(0xE3)
    }

    If (LEqual(ARG0, 0x04)) {
      \_SB.S80H(0xE4)
    }

    // EC EnableAcpi
    //Store(\_SB.PCI0.LPC0.EC0.STAS, Local0)
    //Or(Local0, 0x04, Local1)
    //Store(Local1, \_SB.PCI0.LPC0.EC0.STAS)

    //Store(0xFF, \_SB.ACAD.ACDC) //Update AC/DC status to SMU when wake.

    MWAK (Arg0)
    // TODO: To verify LID function after wake-up
    return (Package(){0,0})
  }

#endif
