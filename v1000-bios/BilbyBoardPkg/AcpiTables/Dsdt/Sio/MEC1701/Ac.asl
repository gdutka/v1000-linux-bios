/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

Device(ACAD)
{
  Name(_HID,"ACPI0003")
    Name(_PCL,Package(0x01){
      \_SB
    })

  Name(XX00, Buffer(0x03){})
  Name(ACDC, 0xFF) // 0:DC, 1:AC, others: Invalid

  Method(_PSR,0,NotSerialized)
  {
    Store(0x01,Local0)
    If(\_SB.PCI0.LPC0.ECOK())
    {
      if (LNot(Acquire(\_SB.PCI0.LPC0.EC0.Z009,300))) {
        Store(\_SB.PCI0.LPC0.EC0.CHSH,Local0)
        ShiftRight(Local0,0x07,Local0)

        CreateWordField(XX00, 0, SSZE)          // Structure size field
        CreateByteField(XX00, 2, ACST)          // AC/DC field
        Store(3, SSZE)                          // table size 3
        
        If(LNotEqual(Local0, ACDC)) {
          If (Local0) {
            Store(0xECAC,P80H)
            AFN4(0x01)
            Store(0, ACST)          
          }
          Else
          {
            Store(0xECDC,P80H)
            AFN4(0x02)
            Store(1, ACST)
          }
          \_SB.ALIB(1, XX00)                      // notify AC/DC Status          
          Store(Local0, ACDC)          
        }                
        Release(\_SB.PCI0.LPC0.EC0.Z009)
      }
    }
      Return(Local0)
  }

  Method(_STA,0,NotSerialized)
  {
    Return(0x0F)
  }
}
