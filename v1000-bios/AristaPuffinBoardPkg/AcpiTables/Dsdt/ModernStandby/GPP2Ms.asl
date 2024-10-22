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

// GPP2 (Bus 0 Dev 1 Fn 3)
Scope(\_SB.PCI0.GPP2)
{
  OperationRegion(PCI3, SystemMemory, 0xF000B000, 0x100)  //D1F3
  Field(PCI3, DWordAcc, NoLock, Preserve) {
	  offset (0x54),
	  PMS3, 16
	}

  Name(ABF1, Buffer (5) {})
  Name(PBF1, Buffer (5) {})
	Name(PSTA, 1)
	
	PowerResource(PWRE, 0, 0) {
      Method(_STA) {
        TPST(MsNicPr0StaMethod)
        Return(PSTA)
      }
      Method(_ON) {
        TPST(MsNicPr0OnEntry)
        If (LEqual (PSTA, 0)) {
		  If(\_SB.PCI0.LPC0.ECOK()) {
		  //device power on
/*	
		  Acquire(\_SB.PCI0.LPC0.EC0.Z009, 0xFFFF)
		  Store (\_SB.PCI0.LPC0.EC0.GPO8, Local0)
		  Store(0xBBB1, P80H)
		  Or(Local0, 0x04, Local1)
		  Store(0xCCC1, P80H)
		  Store (Local1, \_SB.PCI0.LPC0.EC0.GPO8)
		  Release(\_SB.PCI0.LPC0.EC0.Z009)
	
		  Sleep (100)
		  Acquire(\_SB.PCI0.LPC0.EC0.Z009, 0xFFFF)
		  Store (\_SB.PCI0.LPC0.EC0.GPO7, Local0)
		  Or(Local0, 0x10, Local1)
		  Store (Local1, \_SB.PCI0.LPC0.EC0.GPO7)
		  Release(\_SB.PCI0.LPC0.EC0.Z009)
		  Sleep (100)
	
		  //ALIB FUNCTION6 HOTPLUG
		  CreateWordField(PBF1, 0, PSIZ)										  // Structure size field
		  CreateField(PBF1, 16, 3, FUCN)										  // func field
		  CreateField(PBF1, 19, 5, DEVN)											  // dev field
		  CreateByteField(PBF1, 3, BUSN)												  // bus field
		  CreateByteField(PBF1, 4, HTPS)										  // hotplug field
		  Store(5, PSIZ)
		  Store(0, BUSN)
		  Store(1, DEVN)
		  Store(3, FUCN)
		  Store(1, HTPS)
		  Store(0xAAA1, P80H)
		  \_SB.ALIB(6, PBF1)
		  Store(0xAAA2, P80H)
	
		  //APPM
		  CreateWordField(ABF1, 0, ASIZ)	 // Structure size field(2Byte)
		  CreateField(ABF1, 16, 3, AFUN)	 // func field
		  CreateField(ABF1, 19, 5, ADEV)	 // dev field
		  CreateByteField(ABF1, 3, ABUS)		 // bus field
		  CreateByteField(ABF1, 4, APPM)	 // 1=Enter_L23, 0=Exit from RC port
		  Store(5, ASIZ)
		  Store(0, ABUS)
		  Store(1, ADEV)
		  Store(3, AFUN)
		  Store(0, APPM)   // 0=Exit from RC port
		  Store(0xAAA3, P80H)
		  \_SB.APPM (ABF1)
		  Store(0xAAA4, P80H)
	*/
		  Store(0x01, PSTA)
		  }
        }
		  TPST(MsNicPr0OnExit)
       }
       Method(_OFF) {
        TPST(MsNicPr0OffEntry)
        If (LEqual (PSTA, 1)) {
		  If(\_SB.PCI0.LPC0.ECOK()) {
	/*
		  //APPM
		  CreateWordField(ABF1, 0, ASIZ)	 // Structure size field(2Byte)
		  CreateField(ABF1, 16, 3, AFUN)	 // func field
		  CreateField(ABF1, 19, 5, ADEV)	 // dev field
		  CreateByteField(ABF1, 3, ABUS)		 // bus field
		  CreateByteField(ABF1, 4, APPM)	 // 1=Enter_L23, 0=Exit from RC port
		  Store(5, ASIZ)
		  Store(0, ABUS)
		  Store(1, ADEV)
		  Store(3, AFUN)
		  Store(1, APPM)  // 1=Enter_L23
		  Store(0xAAA5, P80H)
		  \_SB.APPM (ABF1)
		  Store(0xAAA6, P80H)
	
		  //ALIB FUNCTION6 HOTPLUG
		  CreateWordField(PBF1, 0, PSIZ)										  // Structure size field
		  CreateField(PBF1, 16, 3, FUCN)										  // func field
		  CreateField(PBF1, 19, 5, DEVN)											  // dev field
		  CreateByteField(PBF1, 3, BUSN)												  // bus field
		  CreateByteField(PBF1, 4, HTPS)										  // hotplug field
		  Store(5, PSIZ)
		  Store(0, BUSN)
		  Store(1, DEVN)
		  Store(3, FUCN)
		  Store(0, HTPS)
		  Store(0xAAA7, P80H)
		  
		  Store (PMS3, Local2)
		  Store (And (Local2, 0x7FFC), PMS3)
		  
		  \_SB.ALIB(6, PBF1)
	
		  Store (And (Local2, 0x7FFF), PMS3)
			  
		  Store(0xAAA8, P80H)
	
		  //device power off
		  Acquire(\_SB.PCI0.LPC0.EC0.Z009, 0xFFFF)
		  Store (\_SB.PCI0.LPC0.EC0.GPO7, Local0)
		  And(Local0, 0xEF, Local1)
		  Store (Local1, \_SB.PCI0.LPC0.EC0.GPO7)
		  Release(\_SB.PCI0.LPC0.EC0.Z009)
	
		  Sleep (100)
		  Acquire(\_SB.PCI0.LPC0.EC0.Z009, 0xFFFF)
		  Store (\_SB.PCI0.LPC0.EC0.GPO8, Local0)
		  And(Local0, 0xFB, Local1)
		  Store (Local1, \_SB.PCI0.LPC0.EC0.GPO8)
		  Release(\_SB.PCI0.LPC0.EC0.Z009)
	
		  Sleep (100)
	*/
		  Store(0x00, PSTA)
		  }
        }
		  TPST(MsNicPr0OffExit)
      }
    }
  
    Name(MPR0, Package() {PWRE})
    Name(MPR2, Package() {PWRE})
    Name(MPR3, Package() {PWRE})
    Name (MS0W, 4)

 }// pcie port end

  Scope(\_SB.PCI0.GPP2.BCM5) {

	
  Method (RHRS, 0, NotSerialized)
  {
    Name (RBUF, ResourceTemplate ()
    {
      GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullNone, 0x0000, "\\_SB.GPIO", 0x00, ResourceConsumer, , ) {02}    // PCIE_WAK
      GpioInt (Edge, ActiveHigh, SharedAndWake, PullNone, 0x0000, "\\_SB.GPIO", 0x00, ResourceConsumer, , ) {172}	 // NbGppPme	
    })
    Return(RBUF)		  // Yes, return GPIO interrupt info
  }
  	Name (MS0W, 4)
  	Method(_RMV,0,NotSerialized) {
      Return(0x00)
    }
  }

