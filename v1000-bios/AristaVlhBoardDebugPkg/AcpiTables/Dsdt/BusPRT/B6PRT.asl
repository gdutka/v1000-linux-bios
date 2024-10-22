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
Name(PR06, Package() {
//
Package(){ 0x0000FFFF, 0, LNKE, 0 },
Package(){ 0x0000FFFF, 1, LNKF, 0 },
Package(){ 0x0000FFFF, 2, LNKG, 0 },
Package(){ 0x0000FFFF, 3, LNKH, 0 },
})
Name(AR06, Package() {
//
Package(){ 0x0000FFFF, 0, 0, 20 },
Package(){ 0x0000FFFF, 1, 0, 21 },
Package(){ 0x0000FFFF, 2, 0, 22 },
Package(){ 0x0000FFFF, 3, 0, 23 },
})

Name(NR06, Package() {
//
Package(){ 0x0000FFFF, 0, 0, 44 },
Package(){ 0x0000FFFF, 1, 0, 45 },
Package(){ 0x0000FFFF, 2, 0, 46 },
Package(){ 0x0000FFFF, 3, 0, 47 },
})

Method(_PRT, 0) {
  If(PICM) {
    If (\NAPC) {
      Return(NR06)      // NB IOAPIC Enabled
    } Else {
      Return(AR06)      // NB IOAPIC Disabled
    }
  } Else {
    Return (PR06)       // PIC mode
  }
}
