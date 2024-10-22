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

#ifndef _LNKX_
#define _LNKX_

OperationRegion(PIRQ, SystemIO, 0xC00, 0x2)
Field(PIRQ, ByteAcc, NoLock, Preserve)
{
  PIDX, 8,        // Index port
  PDAT, 8         // Data port
}

IndexField(PIDX, PDAT, ByteAcc, NoLock, Preserve)
{
  PIRA, 8,        // INT A
  PIRB, 8,        // INT B
  PIRC, 8,        // INT C
  PIRD, 8,        // INT D
  PIRE, 8,        // INT E
  PIRF, 8,        // INT F
  PIRG, 8,        // INT G
  PIRH, 8,        // INT H
  Offset(0x0C),
  SIRA, 8,        // INTA from serial IRQ
  SIRB, 8,        // INTB from serial IRQ
  SIRC, 8,        // INTC from serial IRQ
  SIRD, 8,        // INTD from serial IRQ
  PIRS, 8,        // SCI
  Offset(0x13),
  HDAD, 8,        // HD Audio
  Offset(0x17),
  SDCL, 8,        // SD
  Offset(0x1A),
  SDIO, 8,        // SDIO
  Offset(0x30),
  USB1, 8,        // USB1 - Dev18 Func0
  Offset(0x34),
  USB3, 8,        // USB3 - Dev16 Func0
  Offset(0x41),
  SATA, 8,        // SATA
  Offset(0x62),
  GIOC, 8,        // GPIO
  Offset(0x70),
  I2C0, 8,        // I2C0
  I2C1, 8,        // I2C1
  I2C2, 8,        // I2C2
  I2C3, 8,        // I2C3
  URT0, 8,        // UART0
  URT1, 8         // UART1
}

OperationRegion(KBDD, SystemIO, 0x64, 0x01)
Field(KBDD, ByteAcc, NoLock, Preserve)
{
  PD64, 8         // Port 64h
}

Method(DSPI)
{
  INTA(0x1f)
  INTB(0x1f)
  INTC(0x1f)
  INTD(0x1f)
  Store(PD64, Local1)
  Store(0x1f, PIRE)
  Store(0x1f, PIRF)
  Store(0x1f, PIRG)
  Store(0x1f, PIRH)
}

Method(INTA,1)
{
  Store(ARG0, PIRA)
  If (PICM) {
    Store(ARG0, HDAD)
    Store(ARG0, SDCL)
  }
}

Method(INTB,1)
{
  Store(ARG0, PIRB)
}

Method(INTC,1)
{
  Store(ARG0, PIRC)
  If (PICM) {
    Store(ARG0, USB1)
    Store(ARG0, USB3)
  }
}

Method(INTD,1)
{
  Store(ARG0, PIRD)
  If (PICM)  {
    Store(ARG0, SATA)
  }
}

Name (PRS1,ResourceTemplate()
  {
  IRQ(Level,ActiveLow,Shared){3,5,6,10,11}
  })

Name(BUFA,ResourceTemplate()
  {
  IRQ(Level, ActiveLow, Shared ) {15}
  })

Device(LNKA)
{
  Name(_HID, EISAID("PNP0C0F"))
  Name(_UID, 1)
  Method(_STA,0) {
    If(LAnd(PIRA, LNotEqual(PIRA, 0x1f))) {
      Return (0xb)
    } Else {
      Return (0x9)
    }
  }

  Method(_PRS,0) {
    Return(PRS1)
  }

  Method(_DIS) {
    INTA(0x1f)
  }

  Method(_CRS,0) {
    CreateWordField(BUFA, 0x1, IRQX)
    ShiftLeft(1, PIRA, IRQX)
    Return(BUFA)
  }

  Method(_SRS, 1) {
    CreateWordField(ARG0, 0x01, IRA)    // IRQ Low Mask
    FindSetRightBit(IRA, Local0)
    Decrement(Local0)
    Store(Local0, PIRA)
  }
} // end Device LNKA

Device(LNKB)
{
  Name(_HID, EISAID("PNP0C0F"))
  Name(_UID, 2)
  Method(_STA,0) {
    If(LAnd(PIRB, LNotEqual(PIRB, 0x1f))) {
      Return (0xb)
    } Else {
      Return (0x9)
    }
  }

  Method(_PRS,0) {
    Return(PRS1)
  }

  Method(_DIS) {
    INTB(0x1f)
  }

  Method(_CRS,0) {
    CreateWordField(BUFA, 0x1, IRQX)
    ShiftLeft(1, PIRB, IRQX)
    Return(BUFA)
  }

  Method(_SRS, 1) {
    CreateWordField(ARG0, 0x01, IRA)    // IRQ Low Mask
    FindSetRightBit(IRA, Local0)
    Decrement(Local0)
    Store(Local0, PIRB)
  }
} // end Device LNKB

Device(LNKC)
{
  Name(_HID, EISAID("PNP0C0F"))
  Name(_UID, 3)
  Method(_STA,0) {
    If(LAnd(PIRC, LNotEqual(PIRC, 0x1f))) {
      Return (0xb)
    } Else {
      Return (0x9)
    }
  }

  Method(_PRS,0) {
    Return(PRS1)
  }

  Method(_DIS) {
    INTC(0x1f)
  }

  Method(_CRS,0) {
    CreateWordField(BUFA, 0x1, IRQX)
    ShiftLeft(1, PIRC, IRQX)
    Return(BUFA)
  }

  Method(_SRS, 1) {
    CreateWordField(ARG0, 0x01, IRA)    // IRQ Low Mask
    FindSetRightBit(IRA, Local0)
    Decrement(Local0)
    Store(Local0, PIRC)
  }
}// end Device LNKC

Device(LNKD)
{
  Name(_HID, EISAID("PNP0C0F"))
  Name(_UID, 4)
  Method(_STA,0) {
    If(LAnd(PIRD, LNotEqual(PIRD, 0x1f))) {
      Return (0xb)
    } Else {
      Return (0x9)
    }
  }

  Method(_PRS,0) {
    Return(PRS1)
  }

  Method(_DIS) {
    INTD(0x1f)
  }

  Method(_CRS,0) {
    CreateWordField(BUFA, 0x1, IRQX)
    ShiftLeft(1, PIRD, IRQX)
    Return(BUFA)
  }

  Method(_SRS, 1) {
    CreateWordField(ARG0, 0x01, IRA)    // IRQ Low Mask
    FindSetRightBit(IRA, Local0)
    Decrement(Local0)
    Store(Local0, PIRD)
  }
}// end Device LNKD

Device(LNKE)
{
  Name(_HID, EISAID("PNP0C0F"))
  Name(_UID, 5)
  Method(_STA,0) {
    If(LAnd(PIRE, LNotEqual(PIRE, 0x1f))) {
      Return (0xb)
    } Else {
      Return (0x9)
    }
  }

  Method(_PRS,0) {
    Return(PRS1)
  }

  Method(_DIS) {
    Store (0x1f, PIRE)
  }

  Method(_CRS,0) {
    CreateWordField(BUFA, 0x1, IRQX)
    ShiftLeft(1, PIRE, IRQX)
    Return(BUFA)
  }

  Method(_SRS, 1) {
    CreateWordField(ARG0, 0x01, IRA)    // IRQ Low Mask
    FindSetRightBit(IRA, Local0)
    Decrement(Local0)
    Store(Local0, PIRE)
  }
}// end Device LNKE

Device(LNKF)
{
  Name(_HID, EISAID("PNP0C0F"))
  Name(_UID, 6)
  Method(_STA,0) {
    If(LAnd(PIRF, LNotEqual(PIRF, 0x1f))) {
      Return (0xb)
    } Else {
      Return (0x9)
    }
  }

  Method(_PRS,0) {
    Return(PRS1)
  }

  Method(_DIS) {
    Store (0x1f, PIRF)
  }

  Method(_CRS,0) {
    CreateWordField(BUFA, 0x1, IRQX)
    ShiftLeft(1, PIRF, IRQX)
    Return(BUFA)
  }

  Method(_SRS, 1) {
    CreateWordField(ARG0, 0x01, IRA)    // IRQ Low Mask
    FindSetRightBit(IRA, Local0)
    Decrement(Local0)
    Store(Local0, PIRF)
  }
}// end Device LNKF

Device(LNKG)
{
  Name(_HID, EISAID("PNP0C0F"))
  Name(_UID, 7)
  Method(_STA,0) {
    If(LAnd(PIRG, LNotEqual(PIRG, 0x1f))) {
      Return (0xb)
    } Else {
      Return (0x9)
    }
  }

  Method(_PRS,0) {
    Return(PRS1)
  }

  Method(_DIS) {
    Store (0x1f, PIRG)
  }

  Method(_CRS,0) {
    CreateWordField(BUFA, 0x1, IRQX)
    ShiftLeft(1, PIRG, IRQX)
    Return(BUFA)
  }

  Method(_SRS, 1) {
    CreateWordField(ARG0, 0x01, IRA)    // IRQ Low Mask
    FindSetRightBit(IRA, Local0)
    Decrement(Local0)
    Store(Local0, PIRG)
  }
}// end Device LNKG

Device(LNKH)
{
  Name(_HID, EISAID("PNP0C0F"))
  Name(_UID, 8)
  Method(_STA,0) {
    If(LAnd(PIRH, LNotEqual(PIRH, 0x1f))) {
      Return (0xb)
    } Else {
      Return (0x9)
    }
  }

  Method(_PRS,0) {
    Return(PRS1)
  }

  Method(_DIS) {
    Store (0x1f, PIRH)
  }

  Method(_CRS,0) {
    CreateWordField(BUFA, 0x1, IRQX)
    ShiftLeft(1, PIRH, IRQX)
    Return(BUFA)
  }

  Method(_SRS, 1) {
    CreateWordField(ARG0, 0x01, IRA)    // IRQ Low Mask
    FindSetRightBit(IRA, Local0)
    Decrement(Local0)
    Store(Local0, PIRH)
  }
}// end Device LNKH

#endif
