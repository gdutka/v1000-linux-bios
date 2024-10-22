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

// GPP1 (Bus 0 Dev 1 Fn 2)
Scope(\_SB.PCI0.GPP1)
{
  Name(PSTA, 1)
  
  PowerResource(PWRE, 0, 0) {
	Method(_STA) {
	  TPST(MsNicPr0StaMethod)
	  Return(PSTA)
	}
	Method(_ON) {
	  TPST(MsNicPr0OnEntry)
		Store(0x01, PSTA)
		TPST(MsNicPr0OnExit)
	 }
	 Method(_OFF) {
	  TPST(MsNicPr0OffEntry)
  
		Store(0x00, PSTA)
		TPST(MsNicPr0OffExit)
	}
  }
  
  Name(MPR0, Package() {PWRE})
  Name(MPR2, Package() {PWRE})
  Name(MPR3, Package() {PWRE})
  Name (MS0W, 4)
}

//WLAN d3-cold support
Scope(\_SB.PCI0.GPP1.DEV0)
{

	 Method (RHRS, 0, NotSerialized)
	 {
	   Name (RBUF, ResourceTemplate ()
	   {
		 GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullNone, 0x0000, "\\_SB.GPIO", 0x00, ResourceConsumer, , ) {14}	  // PCIE_WAK
		 GpioInt (Edge, ActiveHigh, SharedAndWake, PullNone, 0x0000, "\\_SB.GPIO", 0x00, ResourceConsumer, , ) {172}	// NbGppPme    
	   })
	   Return(RBUF) 		 // Yes, return GPIO interrupt info
	 }

     Name (MS0W, 4)
}

