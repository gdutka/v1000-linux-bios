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

#ifndef _MOTHER_
#define _MOTHER_


Device(DMAC)
{                                       // DMA Controller
  Name(_HID, EISAID("PNP0200"))
  Name(_CRS, ResourceTemplate()
  {
  IO(Decode16, 0x0, 0x0, 0x1, 0x10)
  IO(Decode16, 0x81, 0x81, 0x1, 0xF)
  IO(Decode16, 0xC0, 0xC0, 0x1, 0x20)
  DMA(Compatibility, NotBusMaster, Transfer8_16) {4}
  })
} // End of DMAC

Device(COPR)
{                                       // Math Coprocessor
  Name(_HID, EISAID("PNP0C04"))
  Name(_CRS, ResourceTemplate()
  {
  IO(Decode16, 0xF0, 0xF0, 0x1, 0xF)
  IRQNoFlags(){0xD}
  })
} // End of MATH

Device(PIC)
{                                       // PIC Controller
  Name(_HID, EISAID("PNP0000"))
  Name(_CRS, ResourceTemplate()
  {
  IO(Decode16, 0x20, 0x20, 0x1, 0x2)
  IO(Decode16, 0xA0, 0xA0, 0x1, 0x2)
  IRQNoFlags(){0x2}
  })
} // End of PIC

Device(RTC)
{                                       // Real-Time Clock
  Name(_HID, EISAID("PNP0B00"))

  Name(BUF0,ResourceTemplate()
  {
  IO(Decode16,0x70,0x70,0x01,0x02)
  })

  Name(BUF1,ResourceTemplate()
  {
  IO(Decode16,0x70,0x70,0x01,0x02)
  IRQNoFlags() {8}
  })

  Method(_CRS,0,Serialized)
  {
    If (LEqual (HPEN,0x01))
    {
      Return(BUF0)
    }
    Return(BUF1)
  }
} // End of RTC

Device(SPKR)
{                                   // Speaker
  Name(_HID, EISAID("PNP0800"))

  Name(_CRS, ResourceTemplate()
  {
  IO(Decode16, 0x61, 0x61, 0x1, 0x1)
  })
} // End of SPKP

Device(TMR)
{                                   // System Timer
  Name(_HID, EISAID("PNP0100"))

  Name(BUF0,ResourceTemplate()
  {
  IO(Decode16, 0x40, 0x40, 0x1, 0x4)
  })

  Name(BUF1,ResourceTemplate()
  {
  IO(Decode16, 0x40, 0x40, 0x1, 0x4)
  IRQNoFlags(){0x0}
  })

  Method(_CRS,0,Serialized)
  {
    If (LEqual (HPEN,0x01))
    {
      Return(BUF0)
    }
    Return(BUF1)
  }
} // End of TIME

#ifndef LEGACY_FREE_SUPPORT
Device(KBC0)
{                                   // Keyboard Controller
  Name(_HID, EISAID("FUJ7401"))

  Name(_CID, EISAID("PNP0303"))

  Name(_CRS, ResourceTemplate()
  {
  IO(Decode16, 0x60, 0x60, 0x1, 0x1)
  IO(Decode16, 0x64, 0x64, 0x1, 0x1)
  IRQNoFlags() {1}
  })
} // End of KBC0

//Device(PS2M)
//{                                   // PS/2 Mouse
//  Name(_HID, EISAID("CIR0311"))
//
//  Name(_CID, EISAID("PNP0F13"))
//
//  Name(_CRS, ResourceTemplate()
//  {
//  IRQNoFlags() {12}
//  })
//} // End of PS2M
#endif

Device(SYSR)
{                                   // System Resource
  Name(_HID, EISAID("PNP0C02"))
  Name(_UID, 1)
  Name(_CRS, ResourceTemplate()
  {
  IO(Decode16, 0x10, 0x10, 0x1, 0x10)
  IO(Decode16, 0x72, 0x72, 0x1, 0x2)      // Second Bank RTC
  IO(Decode16, 0x80, 0x80, 0x1, 0x1)      // Port 80h
  IO(Decode16, 0xB0, 0xB0, 0x1, 0x2)      // SMI Cmd Port
  IO(Decode16, 0x92, 0x92, 0x1, 0x1)
  IO(Decode16, 0x400, 0x400, 0x01,0xd0)   // ACPI IO
  IO(Decode16, 0x4D0, 0x4D0, 0x1, 0x2)    // Interrupt Control Ports
  IO(Decode16, 0x4D6, 0x4D6, 0x1, 0x1)    // DMA2 Entend Ports
  IO(Decode16, 0xC00, 0xC00, 0x1, 0x2)    // PCI IRQ Routing
  IO(Decode16, 0xC14, 0xC14, 0x1, 0x1)
  IO(Decode16, 0xC50, 0xC50, 0x1, 0x3)    // CM & GPM Ports
  IO(Decode16, 0xC6C, 0xC6C, 0x1, 0x1)
  IO(Decode16, 0xC6F, 0xC6F, 0x1, 0x1)    // Rom Flash Enable
  IO(Decode16, 0xCD0, 0xCD0, 0x1, 0xc)    // PM IO
  })
} // End of SYSR

Device(MEM)
{
  Name(_HID, EISAID("PNP0C01"))   // Motherboard
  Name(MSRC, ResourceTemplate(){  // Resource
    Memory32Fixed(                      // BIOS
      ReadOnly,                   // _RW
      0xE0000,                    // _BAS
      0x20000                     // _LEN
      )
    Memory32Fixed(ReadWrite,0xFFF00000,0x100000,PART)

    // Reserving the memory for the ATI GART is mandatory.
    // See ATI BIOS Advisory Notices and BIOS Writer's Guide.

    // Reserved memory for Arista FPGA 
    Memory32Fixed (ReadWrite,
                   0xFEED0000,     // Address Base
                   0x00010000,     // Address Length
                  )
    })

  Method(_CRS)
  {
    // Reserving the memory for the ATI GART is mandatory.
    // See ATI BIOS Advisory Notices and BIOS Writer's Guide.
    CreateDwordField (MSRC, ^PART._LEN, PSIZ)
    CreateDwordField (MSRC, ^PART._BAS, PBAS)
    Store (ROMS, PSIZ)
    Subtract (ROMS, 1, Local0)
    Subtract (0xFFFFFFFF, Local0, PBAS)
    Return(MSRC)
  } // End of _CRS
} // End of PCI0.LPC0.MEM

#endif
