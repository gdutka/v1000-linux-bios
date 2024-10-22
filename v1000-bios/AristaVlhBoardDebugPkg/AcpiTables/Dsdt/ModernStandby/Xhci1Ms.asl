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

// xhci1
Scope(\_SB.PCI0.GP17.XHC1)
{

  // BIOS will patch RHRS to _CRS if HW_REDUCED_ACPI is enabled
  Method (RHRS, 0, NotSerialized)
  {
      Name (RBUF, ResourceTemplate ()
      {
          GpioInt (Level, ActiveHigh, ExclusiveAndWake, PullNone, 0x0000, "\\_SB.GPIO", 0x00, ResourceConsumer, , )
             {59}    // Int_usb_xhc_1_acpi_pme
      })
      Return(RBUF)        // Yes, return GPIO interrupt info
  }

  Name(D0U1, 1)
  Name(D3U1, 0)

  PowerResource(P0U1, 0, 0) {
      Method(_STA) {
	  	TPST(MsXhci1Pr0StaMethod)
        Return(D0U1)
      }
      Method(_ON) {
	  	TPST(MsXhci1Pr0OnMethod)
        Store(0x01, D0U1)
      }
      Method(_OFF) {
	  	TPST(MsXhci1Pr0OffMethod)
      	Store(0x00, D0U1)
      }
    }
  
  PowerResource(P3U1, 0, 0) {
      Method(_STA) {
	  	TPST(MsXhci1Pr3StaMethod)
        Return(D3U1)
      }
      Method(_ON) {
	  	TPST(MsXhci1Pr3OnMethod)
        Store(0x01, D3U1)
      }
      Method(_OFF) {
	  	TPST(MsXhci1Pr3OffMethod)
        Store(0x00, D3U1)
      }
    }


     Name(_PR0, Package() {P0U1})

     Name(_PR3, Package() {P3U1})

    Method(_S0W,0)
    {
       TPST(MsXhci1S0wMethod)
       Return(4)
    }
    Method(_PS0,0)
    {
       TPST(MsXhci1Ps0Method)
    }
    Method(_PS3,0)
    {
       TPST(MsXhci1Ps3Method)
    }
}
