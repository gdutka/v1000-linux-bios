/** @file

;*******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, dispWWy, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

  Device(WWAN)
  {
  
   Name(_ADR, 0x00)
 
//   Method(_STA) {
//    if (LEqual(CNSB, 1)) {
//     Return(0x0F)
//    } else {
//     Return(0x00)
//    }
//   }

     Name(MPR0, Package() {P0WW})
     Name(MPR3, Package() {P3WW})

    Method(MS0W,0)
    {
       TPST(MsWwanS0wMethod)
       Return(4)
    }

    Method(MPS0,0)
    {
       TPST(MsWwanPs0Method)
    }

    Method(MPS3,0)
    {
       TPST(MsWwanPs3Method)
    }

  }
