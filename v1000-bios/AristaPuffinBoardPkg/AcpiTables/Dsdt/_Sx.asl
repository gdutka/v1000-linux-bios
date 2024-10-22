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

#ifndef _Sx_
#define _Sx_
//
// System Sleep States
//
Name (\_S0,Package (){0, 0, 0, 0})

#ifdef CONNECTED_STANDBY_SUPPORT
If (LEqual(CNSB, 0x0)) {
  If(LEqual(DAS3, 0x1))
  {
    Name (\_S3,Package (){3, 3, 0, 0})
  }
}
#else
If(LEqual(DAS3, 0x1)) {
    Name (\_S3,Package (){3, 3, 0, 0})
}

#endif
//Name (\_S4,Package (){4, 4, 0, 0})
Name (\_S5,Package (){5, 5, 0, 0})

#endif
