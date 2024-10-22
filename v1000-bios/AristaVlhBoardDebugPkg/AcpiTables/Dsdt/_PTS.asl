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

#ifndef _PTS_
#define _PTS_

#define UNLOCK_SPI_STATE                0x90
External(\_SB.TPM2.PTS, MethodObj)

Method(\_PTS, 1) {
  SPTS (Arg0)
  If (LEqual(ARG0, 0x01)) {
    \_SB.S80H(0x51)
  }
  If (LEqual(ARG0, 0x03)) {
    \_SB.S80H(0x53)
    Store(One,\SLPS)
  }
  If (LEqual(ARG0, 0x04)) {
    \_SB.S80H(0x54)
    Store(One,\SLPS)
    Store(One, RSTU)
  }
  If (LEqual(ARG0, 0x05)) {
    \_SB.S80H(0x55)
    //
    // Trigger SMI to clear SPI lock bits before reboot and shutdown.
    //
    Store(UNLOCK_SPI_STATE, BCMD)
    \_SB.BSMI(0)  // Trigger SMI
    \_SB.GSMI(0x03)
  }
  
#ifdef CONNECTED_STANDBY_SUPPORT
  If (LEqual(CNSB, 0x01)) {
	Store(0x0, \_SB.PCI0.LPC0.EC0.STAS)   // Disable EC AcpiEnable
  }
#endif

  //
  // Check whether TPM Module support is included in the project or not
  //
  If(CondRefOf(\_SB.TPM2.PTS)) 
  {
    //
    // Call TPM PTS method
    //
    \_SB.TPM2.PTS (Arg0)
  }


  \_SB.APTS (Arg0)

  MPTS (Arg0)
} //End of \_PTS

#endif
