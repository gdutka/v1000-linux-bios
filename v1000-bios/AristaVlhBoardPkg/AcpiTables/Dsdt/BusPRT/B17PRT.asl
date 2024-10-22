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
Name(PR17, Package() {
// Bus A; Device 0; Function 0; Internal GPU
// Bus A; Device 0; Function 1; Display HD Audio Controller
// Bus A; Device 0; Function 2; Cryptographic Coprocessor
// Bus A; Device 0; Function 3; USB 3.1
// Bus A; Device 0; Function 4; USB 3.1
// Bus A; Device 0; Function 5; Audio Processor
// Bus A; Device 0; Function 6; Audio Processor ¡V HD Audio Controller
// Bus A; Device 0; Function 7; Sensor Fusion Hub
Package(){ 0x0000FFFF, 0, LNKE, 0 },
Package(){ 0x0000FFFF, 1, LNKF, 0 },
Package(){ 0x0000FFFF, 2, LNKG, 0 },
Package(){ 0x0000FFFF, 3, LNKH, 0 },
})
Name(AR17, Package() {
//
Package(){ 0x0000FFFF, 0, 0, 20 },
Package(){ 0x0000FFFF, 1, 0, 21 },
Package(){ 0x0000FFFF, 2, 0, 22 },
Package(){ 0x0000FFFF, 3, 0, 23 },
})

Name(NR17, Package() {
//
Package(){ 0x0000FFFF, 0, 0, 52 },
Package(){ 0x0000FFFF, 1, 0, 53 },
Package(){ 0x0000FFFF, 2, 0, 54 },
Package(){ 0x0000FFFF, 3, 0, 55 },
})

Method(_PRT, 0) {
  If(PICM) {
    If (\NAPC) {
      Return(NR17)      // NB IOAPIC Enabled
    } Else {
      Return(AR17)      // NB IOAPIC Disabled
    }
  } Else {
    Return (PR17)       // PIC mode
  }
}

