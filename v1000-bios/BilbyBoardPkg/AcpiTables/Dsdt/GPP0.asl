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

#ifndef _GPP0_
#define _GPP0_

// GPP0 (Bus 0 Dev 1 Fn 1)
Device(GPP0)
{

  Name(_ADR, 0x00010001)

  Method(_PRW,0,NotSerialized) {
    If (LEqual (WKPM, 0x01)) {
      Return (GPRW (0x8, 0x04))
    } Else {
      Return (GPRW (0x8, 0x00))
    }
  }

  #include "BusPRT/B1PRT.asl"

  Device(VGA)
  {
    Name(_ADR, 0x00)

    //
    // ACPI Video related extension function.
    //
    #include "VideoExt.asl"

    Method(_RMV,0,NotSerialized) {
      // _RMV - Removable Device
      //
      // Indicates to OSPM whether the device can be
      // removed while the system is in the working state
      // (in other words, any device that only supports
      // surprise-style removal). Any such removable
      // device that does not have _LCK or _EJx control
      // methods must have an _RMV object. This allows
      // OSPM to indicate to the user that the device can
      // be removed and to provide a way for shutting down
      // the device before removing it. Reevaluated after
      // a device-check notification.
      //
      //  IN: None
      // OUT: Integer  0 = Device cannot be removed.
      //                    1 = Device can be removed.
      //
      Return(0x00)
    }
  }

  Device(HDAU)
  {
    Name(_ADR, 0x01)

    Method(_RMV,0,NotSerialized) {
      // _RMV - Removable Device
      //
      // Indicates to OSPM whether the device can be
      // removed while the system is in the working state
      // (in other words, any device that only supports
      // surprise-style removal). Any such removable
      // device that does not have _LCK or _EJx control
      // methods must have an _RMV object. This allows
      // OSPM to indicate to the user that the device can
      // be removed and to provide a way for shutting down
      // the device before removing it. Reevaluated after
      // a device-check notification.
      //
      //  IN: None
      // OUT: Integer  0 = Device cannot be removed.
      //                    1 = Device can be removed.
      //
      Return(0x00)
    }
  }
}

#endif
