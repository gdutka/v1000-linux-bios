/** @file
  The Sata related definition block in ACPI Table for feature of Sata Freeze 

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


DefinitionBlock (
  "Sfrz.aml",                                   // Output file
  "SSDT",                                       // Signature
  0x01,                                         // SSDT Revision
  "Insyde",                                     // OEM ID
  "SataFrez",                                   // Table ID
  0x1                                           // OEM Revision
  )
{
  External(\_SB.PCI0.GP18.SATA, DeviceObj)

  Scope(\_SB.PCI0.GP18.SATA) {

    Device(PRT0) {
      Name(_ADR, 0xFFFF)
      Name(NOP, Buffer(0x7){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00})
      Name(SGTF, Buffer(0x7){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF5})
      //
      // for freeze feature
      //
      Method(_GTF, 0x0, NotSerialized) {
        if (FixedPcdGet8 (PcdSataFreezeByAsl)) {
          Return(SGTF)
        } Else {
          Return(NOP)
        }
      }
    }
  }
}
