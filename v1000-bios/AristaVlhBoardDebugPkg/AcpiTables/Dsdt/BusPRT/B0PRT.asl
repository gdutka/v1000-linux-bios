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

Name(PR00, Package() {

// Device 1 Func 1:7 --GPP[0:6]
Package(){ 0x0001FFFF, 0, LNKA, 0 },
Package(){ 0x0001FFFF, 1, LNKB, 0 },
Package(){ 0x0001FFFF, 2, LNKC, 0 },
Package(){ 0x0001FFFF, 3, LNKD, 0 },

// Device 8 Func 1:2 --Internal GPP [1:0]
Package(){ 0x0008FFFF, 0, LNKD, 0 },
Package(){ 0x0008FFFF, 1, LNKE, 0 },

// FCH On-Chip
Package(){ 0x0014FFFF, 0, LNKA, 0 },
Package(){ 0x0014FFFF, 1, LNKB, 0 },
Package(){ 0x0014FFFF, 2, LNKC, 0 },
Package(){ 0x0014FFFF, 3, LNKD, 0 },
})

Name(AR00, Package() {

// Device 1 Func 1:7 --GPP[0:6]
Package(){ 0x0001FFFF, 0, 0, 16 },
Package(){ 0x0001FFFF, 1, 0, 17 },
Package(){ 0x0001FFFF, 2, 0, 18 },
Package(){ 0x0001FFFF, 3, 0, 19 },

// Device 8 Func 1:2 --Internal GPP [1:0]
Package(){ 0x0008FFFF, 0, 0, 19 },
Package(){ 0x0008FFFF, 1, 0, 20 },

// FCH On-Chip
Package(){ 0x0014FFFF, 0, 0, 16 },
Package(){ 0x0014FFFF, 1, 0, 17 },
Package(){ 0x0014FFFF, 2, 0, 18 },
Package(){ 0x0014FFFF, 3, 0, 19 },
})

Name(NR00, Package() {

// Device 1 Func 1:7 --GPP[0:6]
Package(){ 0x0001FFFF, 0, 0, 40 },
Package(){ 0x0001FFFF, 1, 0, 41 },
Package(){ 0x0001FFFF, 2, 0, 42 },
Package(){ 0x0001FFFF, 3, 0, 43 },

// Device 8 Func 1:2 --Internal GPP [1:0]
Package(){ 0x0008FFFF, 0, 0, 43 },
Package(){ 0x0008FFFF, 1, 0, 36 },

// FCH On-Chip
Package(){ 0x0014FFFF, 0, 0, 16 },
Package(){ 0x0014FFFF, 1, 0, 17 },
Package(){ 0x0014FFFF, 2, 0, 18 },
Package(){ 0x0014FFFF, 3, 0, 19 },
})

Method(_PRT, 0) {
  If(PICM) {
    If (\NAPC) {
      Return(NR00)      // NB IOAPIC Enabled
    } Else {
      Return(AR00)      // NB IOAPIC Disabled
    }
  } Else {
    Return (PR00)       // PIC mode
  }
}
