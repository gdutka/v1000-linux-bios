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

  Device(NVME)
  {
  
   Name(_ADR, 0x00)
 /*
//   Method(_STA) {
 //    if (LEqual(CNSB, 1)) {
 //      Return(0x0F)
  //   }else {
 //      Return(0x00)
//     }
  // }

     Name(MPR0, Package() {P0ME})

     Name(MPR3, Package() {P3ME})

    Method(MS0W,0)
    {
       TPST(MsNvmeS0wMethod)
       Return(4)
    }

    Method(MPS0,0)
    {
       TPST(MsNvmePs0Method)
    }

    Method(MPS3,0)
    {
       TPST(MsNvmePs3Method)
    }
*/
  }
