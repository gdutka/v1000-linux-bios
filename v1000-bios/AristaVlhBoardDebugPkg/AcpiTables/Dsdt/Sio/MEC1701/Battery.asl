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

Device(BATT)
{
  Name(_HID,EISAID("PNP0C0A"))
  Name(_UID,0x01)
  Name(_PCL,Package(0x01){
    \_SB
  })
  Name(PBIF,Package(0x0D){
    0x01,        // Power Unit
    0x00,        // Design Capacity
    0x125C,      // Last Full Charge Capacity
    0x01,        // Battery Technology
    0x00,        // Design Voltage
    0x00,        // Design Capacity of Warning
    0x00,        // Design Capacity of Low
    0x00,        // Battery Capacity Granularity 1
    0x00,        // Battery Capacity Granularity 2
    "BA-05",     // Model Number
    "1",         // Serial Number
    "LION",      // Battery Type
    ""           // OEM Information
  })

  Name(PBIX,Package(0x14){
    0x00,        // 0x00, Revision         - BIX only
    0x01,        // 0x01, Power Unit
    0x00,        // 0x02, Design Capacity
    0x125C,      // 0x03, Last Full Charge Capacity
    0x01,        // 0x04, Battery Technology
    0x00,        // 0x05, Design Voltage
    0x00,        // 0x06, Design Capacity of Warning
    0x00,        // 0x07, Design Capacity of Low
    0x00000000,  // 0x08, Cycle Count        - BIX only
    0x00100000,  // 0x09, Measurement Accuracy    - BIX only
    0xFFFFFFFF,  // 0x0A, Max Sampling Time    - BIX only
    0xFFFFFFFF,  // 0x0B, Min Sampling Time    - BIX only
    0x0000EA60,  // 0x0C, Max Averaging Interval    - BIX only
    0x0000EA60,  // 0x0D, Min Averaging Interval    - BIX only
    0x00,        // 0x0E, Battery Capacity Granularity 1
    0x00,        // 0x0F, Battery Capacity Granularity 2
    "BA-05",     // 0x10, Model Number
    "1",         // 0x11, Serial Number
    "LION",      // 0x12, Battery Type
    ""           // 0x13, OEM Information
  })

#ifdef EC_ACPI_REGISTER_DUMP
  #include "EcDbg.asl"
#endif

  Method(_BTP,1,NotSerialized)
  {
    If(\_SB.PCI0.LPC0.ECOK())
    {
      if (LNot(Acquire(\_SB.PCI0.LPC0.EC0.Z009,300))) {

      if (LEqual(Arg0,0))
      {
        Store(0, Local0)
          Store(0, Local1)
      }
      Else
      {
        And(Arg0, 0xFF, Local0)
          ShiftRight(Arg0, 8, Local1)
          And(Local1, 0xFF, Local1)
      }

      Store (Local0, \_SB.PCI0.LPC0.EC0.BTPL)
      Store (Local1, \_SB.PCI0.LPC0.EC0.BTPH)

                      #ifdef EC_ACPI_REGISTER_DUMP
                      store ("_BTP", debug)
                      ECDB()
                      #endif

      Release(\_SB.PCI0.LPC0.EC0.Z009)
    }
    }
  }

  Name(PBST,Package(0x04){
    0x01,
    0x0A90,
    0x1000,
    0x2A30
  })

            Name(CALB, Buffer(0x10){})	// GNB-Platform BIOS interface buffer
            Name (BDBG, Buffer (256) {})

  Method(_STA,0,NotSerialized)
  {
    If(\_SB.PCI0.LPC0.ECOK())
    {
      if (LNot(Acquire(\_SB.PCI0.LPC0.EC0.Z009,300))) {

      Store(\_SB.PCI0.LPC0.EC0.CHSH,Local0)
      Release(\_SB.PCI0.LPC0.EC0.Z009)
      If(And(Local0,0x40))
      {
        Return(0x1F)
      }
      Else
      {
        Return(0x0F)
      }
      }

    }
      Return(0x1F)
    }
  Method(_BIF,0,NotSerialized)
  {
    If(\_SB.PCI0.LPC0.ECOK())
    {
      if (LNot(Acquire(\_SB.PCI0.LPC0.EC0.Z009,300))) {
      Store(\_SB.PCI0.LPC0.EC0.LFCL,Local2)
      Store(\_SB.PCI0.LPC0.EC0.LFCH,Local0)
      Multiply(Local0,0x0100,Local1)
      Add(Local1,Local2,Local0)
      Store(Local0,Index(PBIF,0x02))
      Store(\_SB.PCI0.LPC0.EC0.DECL,Local2)
      Store(\_SB.PCI0.LPC0.EC0.DECH,Local0)
      Multiply(Local0,0x0100,Local1)
      Add(Local1,Local2,Local0)
      Store(Local0,Index(PBIF,0x01))
      Store(\_SB.PCI0.LPC0.EC0.DEVL,Local2)
      Store(\_SB.PCI0.LPC0.EC0.DEVH,Local0)
      Multiply(Local0,0x0100,Local1)
      Add(Local1,Local2,Local0)
      Store(Local0,Index(PBIF,0x04))
      Store("",Index(PBIF,0x09))
      Store("",Index(PBIF,0x0B))
      Store(\_SB.PCI0.LPC0.EC0.BDNE,Index(PBIF,0x09))
      Store(\_SB.PCI0.LPC0.EC0.CHTY,Index(PBIF,0x0B))
      Store(\_SB.PCI0.LPC0.EC0.MANU,Index(PBIF,0x0C))
#ifdef EC_ACPI_REGISTER_DUMP
      store ("_BIF():", debug)
      ECDB()
#endif

      Release(\_SB.PCI0.LPC0.EC0.Z009)
    }
    }
    Return(PBIF)
  }

  Method(_BIX,0,NotSerialized)
  {
    If(\_SB.PCI0.LPC0.ECOK())
    {
      if (LNot(Acquire(\_SB.PCI0.LPC0.EC0.Z009,300))) {
      Store(\_SB.PCI0.LPC0.EC0.LFCL,Local2)
      Store(\_SB.PCI0.LPC0.EC0.LFCH,Local0)        // 0x03, Last Full Charge Capacity
      Multiply(Local0,0x0100,Local1)
      Add(Local1,Local2,Local0)
      Store(Local0,Index(PBIX,0x03))

      Store(\_SB.PCI0.LPC0.EC0.DECL,Local2)
      Store(\_SB.PCI0.LPC0.EC0.DECH,Local0)        // 0x02, Design Capacity
      Multiply(Local0,0x0100,Local1)
      Add(Local1,Local2,Local0)
      Store(Local0,Index(PBIX,0x02))

      Store(\_SB.PCI0.LPC0.EC0.DEVL,Local2)
      Store(\_SB.PCI0.LPC0.EC0.DEVH,Local0)        // 0x05, Design Voltage
      Multiply(Local0,0x0100,Local1)
      Add(Local1,Local2,Local0)
      Store(Local0,Index(PBIX,0x05))

      Store(\_SB.PCI0.LPC0.EC0.CCTL,Local2)
      Store(\_SB.PCI0.LPC0.EC0.CCTH,Local0)        // 0x08, Cycle Count        - BIX only
      Multiply(Local0,0x0100,Local1)
      Add(Local1,Local2,Local0)
      Store(Local0,Index(PBIX,0x08))

      Store(\_SB.PCI0.LPC0.EC0.MERL,Local2)
      Store(\_SB.PCI0.LPC0.EC0.MERH,Local0)        // 0x09, Measurement Accuracy    - BIX only
      Multiply(Local0,0x0100,Local1)
      Add(Local1,Local2,Local0)
      Store(Local0,Index(PBIX,0x09))

      Store("",Index(PBIX,0x10))
      Store("",Index(PBIX,0x12))
      Store(\_SB.PCI0.LPC0.EC0.BDNE,Index(PBIX,0x10))
      Store(\_SB.PCI0.LPC0.EC0.CHTY,Index(PBIX,0x12))
      Store(\_SB.PCI0.LPC0.EC0.MANU,Index(PBIX,0x13))

#ifdef EC_ACPI_REGISTER_DUMP
      store ("_BIX():", debug)
      ECDB()
#endif
      Release(\_SB.PCI0.LPC0.EC0.Z009)
    }
    }
    Return(PBIX)
  }

  Method(_BST,0,NotSerialized)
  {
                CreateWordField(CALB, 0, CLSZ)		// Call Buffer Size
                CreateByteField(CALB, 2, BTID)		// BatteryId
                CreateByteField(CALB, 3, PWRU)		// Power Unit, 0:mAh, 1:mWh
                CreateDWordField(CALB, 4, FCAP)		// Full  Capacity
                CreateDWordField(CALB, 8, RCAP)		// Remaining Capacity
                CreateDWordField(CALB, 12, BTVT)		// Battery voltage

                Store(0x10, CLSZ)				// Buffer Size fixed to 0x10
                Store(0x0, BTID)				// Battery ID fixed to 0x0
                Store(0, PWRU)				// Power Unit, 0-mAh

    If(\_SB.PCI0.LPC0.ECOK())
    {
      if (LNot(Acquire(\_SB.PCI0.LPC0.EC0.Z009,300))) {
      Store(0x00,Local2)
      Store(\_SB.PCI0.LPC0.EC0.CAPL,Local2)
      Store(\_SB.PCI0.LPC0.EC0.CAPH,Local0)
      Multiply(Local0,0x0100,Local1)
      Add(Local1,Local2,Local0)
      Store(Local0,Index(PBST,0x02))

      Store(Local0,RCAP)
      Store(\_SB.PCI0.LPC0.EC0.LFCH,Local0)
      Multiply(Local0,0x0100,Local1)
      Store(\_SB.PCI0.LPC0.EC0.LFCL,Local2)
      Add(Local1,Local2,Local0)
      Store(Local0,FCAP)

      Store(\_SB.PCI0.LPC0.EC0.BTCL,Local2)
      Store(\_SB.PCI0.LPC0.EC0.BTCH,Local0)
      Multiply(Local0,0x0100,Local1)
      Add(Local1,Local2,Local0)
      Store(\_SB.PCI0.LPC0.EC0.BATS,Local3)
      If(LEqual(And(Local3,0x01),0x00))
      {
        Store(And(Not(Local0),0x7FFF),Local0)
          Store(DeRefOf(Index(PBST,0x00)),Local1)
          Store(Or(0x01,And(Local1,0xFC)),Index(PBST,0x00))
      }
      Else
      {
        Store(DeRefOf(Index(PBST,0x00)),Local1)
          Store(Or(0x02,And(Local1,0xFC)),Index(PBST,0x00))
      }
      Store(Local0,Index(PBST,0x01))
      Store(\_SB.PCI0.LPC0.EC0.BTVL,Local2)
      Store(\_SB.PCI0.LPC0.EC0.BTVH,Local0)
      Multiply(Local0,0x0100,Local1)
      Add(Local1,Local2,Local0)
      Store(Local0,Index(PBST,0x03))

      Store(Local0,BTVT)

#ifdef EC_ACPI_REGISTER_DUMP
      store ("_BST():", debug)
      ECDB()
#endif
      Release(\_SB.PCI0.LPC0.EC0.Z009)
    }
    }
    Return(PBST)
  }
}
