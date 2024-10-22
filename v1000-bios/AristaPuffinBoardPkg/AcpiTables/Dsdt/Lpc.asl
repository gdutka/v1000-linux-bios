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

#ifndef _LPC_
#define _LPC_

Device(LPC0)
{
  Name(_ADR,0x140003)

  //
  // Legacy Method
  //
  #include "Mother.asl"

  //
  // Super I/O
  //
  //#include "Sio\MEC1701\Ec.asl"

  #include "Sio/Smsc/Smc1036.asl"

  Mutex(PSMX,0)
} // End of \_SB.PCI0.LPC0

#endif
