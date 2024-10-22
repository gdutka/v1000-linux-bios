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

#ifndef _IGP_
#define _IGP_

Device(VGA) {
  Name(_ADR, 0x00000000)

  Method(_STA) {
    Return(0x0F)
  }

  Name(DOSA, 0)       // ARGUMENT OF METHOD _DOS, DEFAULT TO 1 (BY BIOS)

  // _DOS(Enable/Disable Output Switching)
  // Arg0 b1:0
  Method(_DOS, 0x1, NotSerialized) {
    Store(Arg0, DOSA)
  }

  // DOD(Enumerate All Devices Attached to the Display Adapter)
  Method(_DOD, 0x0, NotSerialized) {
    Return (Package() {
      0x00010110,     // LCD
      0x00010210,     // DP/HDMI/DVI
      0x00010220,     // DP/HDMI/DVI
      0x00010230,     // DP/HDMI/DVI
      0x00010240,     // DP/HDMI/DVI
      0x00031000,     // The ID for front CAMF
      0x00032000      // for rear CAMR
    })
  }

  Device(LCD) {
    Name(_ADR, 0x110)

    //
    // table for _BCL
    //
    Name (BCLB, Package() {
      90,     // level when system has full power
      60,     // level when system is on battery
      // supported levels would sent to SBIOS for linear transformation to
      // silicon's capability
       2,  4,  6,  8, 10, 12, 14, 16, 18, 20,
      22, 24, 26, 28, 30, 32, 34, 36, 38, 40,
      42, 44, 46, 48, 50, 52, 54, 56, 58, 60,
      62, 64, 66, 68, 70, 72, 74, 76, 78, 80,
      82, 84, 86, 88, 90, 92, 94, 96, 98, 100
    })

    Method(_BCL,0) {
      Return(BCLB)
    }

    //
    // _BCM(Set the brightness Level)
    // Arg0:Desired brightness level
    //
    Method(_BCM, 0x1, NotSerialized) {
      // Brightness control by driver
      // Support ATIF Notify Function 7
      Divide(Multiply(Arg0, 0xFF), 100, Local1, Local0)
      AFN7(Local0)
      Store (Arg0, BRTL)
    }
  }

}

#endif
