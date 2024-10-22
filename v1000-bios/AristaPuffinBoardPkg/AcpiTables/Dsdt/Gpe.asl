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

#ifndef _GPE_ASI_
#define _GPE_ASI_

Scope(\_GPE) {
  Method(_L08, 0x00, Notserialized) {   // PCIe
    Notify (\_SB.PCI0.GPP0, 0x02)
    Notify (\_SB.PCI0.GPP1, 0x02)
    Notify (\_SB.PCI0.GPP2, 0x02)
    Notify (\_SB.PCI0.GPP3, 0x02)
    Notify (\_SB.PCI0.GPP4, 0x02)
    Notify (\_SB.PCI0.GPP5, 0x02)
    Notify (\_SB.PCI0.GP17, 0x02)
    Notify (\_SB.PCI0.GP18, 0x02)
    Notify (\_SB.PWRB, 0x2)
  }

  Method(_L13) {
    Notify (\_SB.PCI0.GPP3, 0x02)  //WWAN
  }

  Method(_L15) {
    Notify (\_SB.PCI0.GPP1, 0x02)  //DT SLOT
  }

  Method(_L19) {                        // XHCI
    Notify (\_SB.PCI0.GP17.XHC0,0x2)
    Notify (\_SB.PCI0.GP17.XHC1,0x2)
    Notify (\_SB.PWRB, 0x2)
  }

} //End of GPE

#endif
