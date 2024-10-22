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

#ifndef _INI_
#define _INI_

  Method(_INI) {
    If (LNotEqual(GPIC,0)) {
      \_SB.DSPI()
      If (\NAPC) {
        \_SB.PCI0.NAPE()    // Clear D0F0xFC_x00[IoapicSbFeatureEn]
      }
    }
    \OSTP()
  }

#endif
