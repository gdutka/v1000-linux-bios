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

// Power resource defination

OperationRegion(PCI2, SystemMemory, 0xF000A000, 0x100)	//D1F2
Field(PCI2, DWordAcc, NoLock, Preserve) {
	offset (0x54),
	PMS2, 16
  }

OperationRegion(PCI3, SystemMemory, 0xF000B000, 0x100)	//D1F3
Field(PCI3, DWordAcc, NoLock, Preserve) {
	offset (0x54),
	PMS3, 16
  }

Scope(\_SB) {
  //
  //WLAM/BT
  //
  Name(ABF0, Buffer (5) {})
  Name(D0WL, 1)
  Name(D3WL, 0)

  PowerResource(P0WL, 0, 0) {
    Method(_STA) {
      TPST(MsWlanPr0StaMethod)
      Return(D0WL)
    }

    Method(_ON) {
      TPST(MsWlanPr0OnEntry)
      If(\_SB.PCI0.LPC0.ECOK()) {
        //device power on
        Acquire(\_SB.PCI0.LPC0.EC0.Z009, 0xFFFF)
        Store (\_SB.PCI0.LPC0.EC0.GPO6, Local0)	//POWER PIN
        Or(Local0, 0x04, Local1)
        Store (Local1, \_SB.PCI0.LPC0.EC0.GPO6)
        Release(\_SB.PCI0.LPC0.EC0.Z009)

        Sleep (100)
        Acquire(\_SB.PCI0.LPC0.EC0.Z009, 0xFFFF)
        Store (\_SB.PCI0.LPC0.EC0.GPO7, Local0)	//RESET#
        Or(Local0, 0x80, Local1)
        Store (Local1, \_SB.PCI0.LPC0.EC0.GPO7)
        Release(\_SB.PCI0.LPC0.EC0.Z009)

        //APPM
        CreateWordField(ABF0, 0, ASIZ)	 // Structure size field(2Byte)
        CreateField(ABF0, 16, 3, AFUN)	 // func field
        CreateField(ABF0, 19, 5, ADEV)	 // dev field
        CreateByteField(ABF0, 3, ABUS)		 // bus field
        CreateByteField(ABF0, 4, APPM)	 // 1=Enter_L23, 0=Exit from RC port
        Store(5, ASIZ)
        Store(0, ABUS)
        Store(1, ADEV)
        Store(2, AFUN)
        Store(0, APPM)   // 0=Exit from RC port
        Store(0xAAA3, P80H)
        \_SB.APPM (ABF0)
        Store(0xAAA4, P80H)

        Store(0x01, D0WL)
      }
      TPST(MsWlanPr0OnExit)
    }
    Method(_OFF) {
      TPST(MsWlanPr0OffEntry)
      If(\_SB.PCI0.LPC0.ECOK()) {
        //APPM
        CreateWordField(ABF0, 0, ASIZ)	 // Structure size field(2Byte)
        CreateField(ABF0, 16, 3, AFUN)	 // func field
        CreateField(ABF0, 19, 5, ADEV)	 // dev field
        CreateByteField(ABF0, 3, ABUS)		 // bus field
        CreateByteField(ABF0, 4, APPM)	 // 1=Enter_L23, 0=Exit from RC port
        Store(5, ASIZ)
        Store(0, ABUS)
        Store(1, ADEV)
        Store(2, AFUN)
        Store(1, APPM)  // 1=Enter_L23
        Store(0xAAA5, P80H)
        \_SB.APPM (ABF0)
        Store(0xAAA6, P80H)

        //device power off
        Acquire(\_SB.PCI0.LPC0.EC0.Z009, 0xFFFF)
        Store (\_SB.PCI0.LPC0.EC0.GPO7, Local0)	//RESET#
        And(Local0, 0x7F, Local1)
        Store (Local1, \_SB.PCI0.LPC0.EC0.GPO7)
        Release(\_SB.PCI0.LPC0.EC0.Z009)

        Sleep (100)
        Acquire(\_SB.PCI0.LPC0.EC0.Z009, 0xFFFF)
        Store (\_SB.PCI0.LPC0.EC0.GPO6, Local0)	//POWER PIN
        And(Local0, 0xFB, Local1)
        Store (Local1, \_SB.PCI0.LPC0.EC0.GPO6)
        Release(\_SB.PCI0.LPC0.EC0.Z009)

        Sleep (1)

        Store(0x00, D0WL)
	  }
	  TPST(MsWlanPr0OffExit)
	}
  }

  PowerResource(P3WL, 0, 0) {
	Method(_STA) {
	  TPST(MsWlanPr3StaMethod)
	  Return(D3WL)
	}
	Method(_ON) {
	  TPST(MsWlanPr3OnMethod)
	  Store(0x01, D3WL)
	}
	Method(_OFF) {
	  TPST(MsWlanPr3OffMethod)
	  Store(0x00, D3WL)
	}
  }

//
//BCM5762 LAN
//

Name(ABF1, Buffer (5) {})
Name(PBF1, Buffer (5) {})
Name(D0LA, 1)
Name(D3LA, 0)

PowerResource(P0LA, 0, 0) {
	Method(_STA) {
	  TPST(MsNicPr0StaMethod)
	  Return(D0LA)
	}
	Method(_ON) {
	  TPST(MsNicPr0OnEntry)
	  If(\_SB.PCI0.LPC0.ECOK()) {
	  //device power on

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

	  Store(0x01, D0LA)
	  }

	  TPST(MsNicPr0OnExit)
	}
	Method(_OFF) {
	  TPST(MsNicPr0OffEntry)

	  If(\_SB.PCI0.LPC0.ECOK()) {

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

	  Store(0x00, D0LA)
	  }

	  TPST(MsNicPr0OffExit)
	}
  }

  PowerResource(P3LA, 0, 0) {
	Method(_STA) {
	  TPST(MsNicPr3StaMethod)
	  Return(D3LA)
	}
	Method(_ON) {
	  TPST(MsNicPr3OnMethod)
	  Store(0x01, D3LA)
	}
	Method(_OFF) {
	  TPST(MsNicPr3OffMethod)
	  Store(0x00, D3LA)
	}
  }


//
//WWAN
//

Name(ABF2, Buffer (5) {})
Name(D0WW, 1)
Name(D3WW, 0)

PowerResource(P0WW, 0, 0) {
	Method(_STA) {
	  TPST(MsWwanPr0StaMethod)
	  Return(D0WW)
	}
	Method(_ON) {
	  TPST(MsWwanPr0OnEntry)
	  If(\_SB.PCI0.LPC0.ECOK()) {
	  //device power on
	  Acquire(\_SB.PCI0.LPC0.EC0.Z009, 0xFFFF)
	  //device power on
	  Store (\_SB.PCI0.LPC0.EC0.GPO6, Local0)	//POWER PIN
	  Or(Local0, 0x40, Local1)
	  Store (Local1, \_SB.PCI0.LPC0.EC0.GPO6)
	  Release(\_SB.PCI0.LPC0.EC0.Z009)

	  Sleep (100)
	  Acquire(\_SB.PCI0.LPC0.EC0.Z009, 0xFFFF)
	  Store (\_SB.PCI0.LPC0.EC0.GPO7, Local0)	//RESET#
	  Or(Local0, 0x20, Local1)
	  Store (Local1, \_SB.PCI0.LPC0.EC0.GPO7)
	  Release(\_SB.PCI0.LPC0.EC0.Z009)

	  //APPM
	  CreateWordField(ABF2, 0, ASIZ)	 // Structure size field(2Byte)
	  CreateField(ABF2, 16, 3, AFUN)	 // func field
	  CreateField(ABF2, 19, 5, ADEV)	 // dev field
	  CreateByteField(ABF2, 3, ABUS)		 // bus field
	  CreateByteField(ABF2, 4, APPM)	 // 1=Enter_L23, 0=Exit from RC port
	  Store(5, ASIZ)
	  Store(0, ABUS)
	  Store(1, ADEV)
	  Store(4, AFUN)
	  Store(0, APPM)   // 0=Exit from RC port
	  Store(0xAAA3, P80H)
	  \_SB.APPM (ABF2)
	  Store(0xAAA4, P80H)

	  Store(0x01, D0WW)
	  }
	  TPST(MsWwanPr0OnExit)
	}
	Method(_OFF) {
	  TPST(MsWwanPr0OffEntry)
	  If(\_SB.PCI0.LPC0.ECOK()) {

	  //APPM
	  CreateWordField(ABF2, 0, ASIZ)	 // Structure size field(2Byte)
	  CreateField(ABF2, 16, 3, AFUN)	 // func field
	  CreateField(ABF2, 19, 5, ADEV)	 // dev field
	  CreateByteField(ABF2, 3, ABUS)		 // bus field
	  CreateByteField(ABF2, 4, APPM)	 // 1=Enter_L23, 0=Exit from RC port
	  Store(5, ASIZ)
	  Store(0, ABUS)
	  Store(1, ADEV)
	  Store(4, AFUN)
	  Store(1, APPM)  // 1=Enter_L23
	  Store(0xAAA5, P80H)
	  \_SB.APPM (ABF2)
	  Store(0xAAA6, P80H)

	  //device power off
	  Acquire(\_SB.PCI0.LPC0.EC0.Z009, 0xFFFF)
	  Store (\_SB.PCI0.LPC0.EC0.GPO7, Local0)	//RESET#
	  And(Local0, 0xDF, Local1)
	  Store (Local1, \_SB.PCI0.LPC0.EC0.GPO7)
	  Release(\_SB.PCI0.LPC0.EC0.Z009)

	  Sleep (100)

	  Acquire(\_SB.PCI0.LPC0.EC0.Z009, 0xFFFF)
	  Store (\_SB.PCI0.LPC0.EC0.GPO6, Local0)	//POWER PIN
	  And(Local0, 0xBF, Local1)
	  Store (Local1, \_SB.PCI0.LPC0.EC0.GPO6)
	  Release(\_SB.PCI0.LPC0.EC0.Z009)

	  Sleep (1)

	  Store(0x00, D0WW)
	  }
	  TPST(MsWwanPr0OffExit)
	}
  }

  PowerResource(P3WW, 0, 0) {
	Method(_STA) {
	  TPST(MsWwanPr3StaMethod)
	  Return(D3WW)
	}
	Method(_ON) {
	  TPST(MsWwanPr3OnMethod)
	  Store(0x01, D3WW)
	}
	Method(_OFF) {
	  TPST(MsWwanPr3OffMethod)
	  Store(0x00, D3WW)
	}
  }


//
//NVME
//
/*
Name(D0ME, 1)
Name(D3ME, 0)

PowerResource(P0ME, 0, 0) {
	Method(_STA) {
	  TPST(MsNvmePr0StaMethod)
	  Return(D0ME)
	}
	Method(_ON) {
	  TPST(MsNvmePr0OnEntry)
	  //device power on
	  Store (1, \GE23)							 //RESET# AGPIO40
	  Store (0, \GV23)

	  Store (1, \GE40)							//POWER PIN AGPIO23
	  Store (1, \GV40)

	  Sleep (10)
	  Store (1, \GE23)							 //RESET# AGPIO40
	  Store (1, \GV23)

	  Store(0x01, D0ME)
	  TPST(MsNvmePr0OnExit)
	}
	Method(_OFF) {
	  TPST(MsNvmePr0OffEntry)
	  //device power off
	  Store (1, \GE23)							 //RESET# AGPIO40
	  Store (0, \GV23)

	  Store (1, \GE40)							//POWER PIN AGPIO23
	  Store (0, \GV40)

	  Sleep (10)

	  Store(0x00, D0ME)
	  TPST(MsNvmePr0OffExit)
	}
  }

  PowerResource(P3ME, 0, 0) {
	Method(_STA) {
	  TPST(MsNvmePr3StaMethod)
	  Return(D3ME)
	}
	Method(_ON) {
	  TPST(MsNvmePr3OnMethod)
	  Store(0x01, D3ME)
	}
	Method(_OFF) {
	  TPST(MsNvmePr3OffMethod)
	  Store(0x00, D3ME)
	}
  }
*/
}



