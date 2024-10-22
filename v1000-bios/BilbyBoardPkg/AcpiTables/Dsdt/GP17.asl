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

#ifndef _GP17_
#define _GP17_

// GP17 (Bus 0 Dev 8 Fn 1)
Device(GP17)
{

  Name(_ADR, 0x00080001)

  Method(_PRW,0,NotSerialized) {
    If (LEqual (WKPM, 0x01)) {
      Return (GPRW (0x19, 0x04))
    } Else {
      Return (GPRW (0x19, 0x00))
    }
  }


  #include "BusPRT/B17PRT.asl"
  #include "IGP.asl"
  #include "Psp.asl"
  #include "Azalia.asl"
  #include "xHci0.asl"
  #include "xHci1.asl"
  #include "Mp2.asl"
}

#endif

