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
Name(PR07, Package() {
//
Package(){ 0x0000FFFF, 0, LNKA, 0 },
Package(){ 0x0000FFFF, 1, LNKB, 0 },
Package(){ 0x0000FFFF, 2, LNKC, 0 },
Package(){ 0x0000FFFF, 3, LNKD, 0 },
})
Name(AR07, Package() {
//
Package(){ 0x0000FFFF, 0, 0, 16 },
Package(){ 0x0000FFFF, 1, 0, 17 },
Package(){ 0x0000FFFF, 2, 0, 18 },
Package(){ 0x0000FFFF, 3, 0, 19 },
})

Name(NR07, Package() {
//
Package(){ 0x0000FFFF, 0, 0, 48 },
Package(){ 0x0000FFFF, 1, 0, 49 },
Package(){ 0x0000FFFF, 2, 0, 50 },
Package(){ 0x0000FFFF, 3, 0, 51 },
})

Method(_PRT, 0) {
  If(PICM) {
    If (\NAPC) {
      Return(NR07)      // NB IOAPIC Enabled
    } Else {
      Return(AR07)      // NB IOAPIC Disabled
    }
  } Else {
    Return (PR07)       // PIC mode
  }
}
