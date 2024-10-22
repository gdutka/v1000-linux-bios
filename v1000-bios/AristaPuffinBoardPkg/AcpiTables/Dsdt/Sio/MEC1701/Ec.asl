/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

External(\_SB.ALIB, MethodObj) 

// ECOK - Return TRUE if EC Namespace accessible.
Method(ECOK,0)
{
  If (LEqual(\_SB.PCI0.LPC0.EC0.OKEC, 0x01))
  {
    Return (1)                      //return true
  }
  else
  {
    Return (0)                      //return false
  }
}

Device(EC0)
{
  Name(_HID,  EISAID("PNP0C09"))  // PNP ID

  Name(RHGP, 3)   // To be patched to Name(_GPE, 3) if HW_Reduced_ACPI is false
  Method(_STA,0,NotSerialized) {
    Return(0x0F)
  }

  Name(OKEC, Zero)

  // EC resources
  Method(_CRS, 0, NotSerialized) {
    Name(BUF0,ResourceTemplate() {
      IO (Decode16, 0x662, 0x662, 0x1, 0x1)
      IO (Decode16, 0x666, 0x666, 0x1, 0x1)
    })
    Name(BUF1,ResourceTemplate() {
      IO(Decode16,0x662,0x662,0x1,0x1)
      IO(Decode16,0x666,0x666,0x1,0x1)
      GpioInt (
        Edge,
        ActiveLow,
        ExclusiveAndWake,
        PullUp,
        0,
        "\\_SB.GPIO",
        0x00,
        ResourceConsumer,
        ,
      ) {22}    // GPIO Interrupt Connection resource for LPC_PME_L/GEVENT3
    })

    If(LEqual(RDHW, 0x0)) {   // HW_REDUCED_ACPI enabled?
      Return(BUF0)
    } Else {
      Return(BUF1)
    }
  }

  /* ------------------------------------------------------------------------*/
  // Name: _REG - This method is called by drivers to register installation
  //              and removal.  For example; OS calls _REG(3,1) to indicate
  //              to the ACPI BIOS that the Embedded Controller Driver is
  //              present and functional.
  //
  //              ARG0: 0=Memory
  //                    1=I/O
  //                    2=PCI Config
  //                    3=Embedded Controller
  //                    4=SMBus
  //
  //              ARG1: 0=Handler Not Connected
  //                    1=Handler Connected
  //
  Method(_REG, 2) {
    If (LEqual(Arg0, 0x03)) {
      // EC EnableAcpi
      Store(\_SB.PCI0.LPC0.EC0.STAS, Local0)
      Or(Local0, 0x04, Local1)
      Store(Local1, \_SB.PCI0.LPC0.EC0.STAS)

      Store(Arg1, OKEC)
      Notify(\_SB.ACAD, 0x80)
      Notify(\_SB.BATT, 0x80)
      Notify(\_SB.BATT, 0x81)   //Fix Battery information haven't display properly
      Notify(\_SB.LID, 0x80)
      if (OKEC) {
        ////DOCK INIT
        //Acquire(\_SB.PCI0.LPC0.EC0.Z009, 0xFFFF)
        //Store(\_SB.PCI0.LPC0.EC0.DKIN, Local0)
        //And(Local0, 0x02, Local1)
        //Release(\_SB.PCI0.LPC0.EC0.Z009)
        //If (LEqual(Local1, 0x02)) {
        //  \DOCK (1)
        //} Else {
        //  \DOCK (0)
        //}

        ////enable on Windows 7
        //if(And(LGreaterEqual(TPOS, 0x50), LNotEqual(ALST, 0))) {
        //  Acquire(\_SB.PCI0.LPC0.EC0.Z009, 0xFFFF)
        //  Store(\_SB.PCI0.LPC0.EC0.BATS, Local1)
        //  Store(Or(Local1, 0x20), \_SB.PCI0.LPC0.EC0.BATS)
        //  Release(\_SB.PCI0.LPC0.EC0.Z009)
        //}
      }
    }
  } // end of _REG

  // EC RAM fields
  #include "EcName.asl"       // EC Name Space
  #include "EcQevent.asl"     // _Qxx Methods

}// END device EC0

// Move to \_SB.I2CB bus.
//#ifndef LEGACY_FREE_SUPPORT
//    #include "ALSD.asl"
//#endif


