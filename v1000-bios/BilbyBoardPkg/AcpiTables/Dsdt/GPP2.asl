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

#ifndef _GPP2_
#define _GPP2_

// GPP2 (Bus 0 Dev 1 Fn 3)
Device(GPP2)
{

  Name(_ADR, 0x00010003)

  Method(_PRW,0,NotSerialized) {
    If (LEqual (WKPM, 0x01)) {
      Return (GPRW (0x8, 0x04))
    } Else {
      Return (GPRW (0x8, 0x00))
    }
  }

  #include "BusPRT/B3PRT.asl"

  Device(BCM5) {
    Name(_ADR, 0x00)
  }

}

#endif
