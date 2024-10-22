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

#ifndef _PR_
#define _PR_
//
//  Processor Objects
//
Scope(\_PR) {
  Processor (C000, 0x00, 0x00000410, 0x06) {}
  Processor (C001, 0x01, 0x00000410, 0x06) {}
  Processor (C002, 0x02, 0x00000410, 0x06) {}
  Processor (C003, 0x03, 0x00000410, 0x06) {}
  Processor (C004, 0x04, 0x00000410, 0x06) {}
  Processor (C005, 0x05, 0x00000410, 0x06) {}
  Processor (C006, 0x06, 0x00000410, 0x06) {}
  Processor (C007, 0x07, 0x00000410, 0x06) {}
}

#endif
