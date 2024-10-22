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
Scope(\_SB.PCI0.GP17.XHC1.RHUB.PRT2) 
{
  Device(BUTH)
  {
  
   Name(_ADR, 0x00)
 
//   Method(_STA) {
//     if (LEqual(M2WL, 1)) {
//       Return(0x0F)
//     }else {
//       Return(0x00)
//     }
//   }

     Name(MPR0, Package() {P0WL})

     Name(MPR3, Package() {P3WL})

    Method(MS0W,0)
    {
       TPST(MsWlanS0wMethod)
       Return(4)
    }

    Method(MPS0,0)
    {
       TPST(MsWlanPs0Method)
    }

    Method(MPS3,0)
    {
       TPST(MsWlanPs3Method)
    }

  }
}
