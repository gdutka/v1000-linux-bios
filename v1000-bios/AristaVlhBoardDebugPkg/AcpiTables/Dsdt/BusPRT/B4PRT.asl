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

Name(PR04, Package() {
//
Package(){ 0x0000FFFF, 0, LNKE, 0 },
Package(){ 0x0000FFFF, 1, LNKF, 0 },
Package(){ 0x0000FFFF, 2, LNKG, 0 },
Package(){ 0x0000FFFF, 3, LNKH, 0 },
})
Name(AR04, Package() {
//
Package(){ 0x0000FFFF, 0, 0, 20 },
Package(){ 0x0000FFFF, 1, 0, 21 },
Package(){ 0x0000FFFF, 2, 0, 22 },
Package(){ 0x0000FFFF, 3, 0, 23 },
})

Name(NR04, Package() {
//
Package(){ 0x0000FFFF, 0, 0, 36 },
Package(){ 0x0000FFFF, 1, 0, 37 },
Package(){ 0x0000FFFF, 2, 0, 38 },
Package(){ 0x0000FFFF, 3, 0, 39 },
})

Method(_PRT, 0) {
  If(PICM) {
    If (\NAPC) {
      Return(NR04)      // NB IOAPIC Enabled
    } Else {
      Return(AR04)      // NB IOAPIC Disabled
    }
  } Else {
    Return (PR04)       // PIC mode
  }
}
