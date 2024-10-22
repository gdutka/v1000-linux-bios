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

#ifndef _PWRB_
#define _PWRB_

Device(PWRB) {                          // Start Power button device
  // Sleep button Hardware Device ID
  Name(_HID, EISAID("PNP0C0C"))
  Method(_STA, 0)
  {                                   // return status of device
    Return(0x0B)
  }
} //End of \_SB.PWRB


#endif
