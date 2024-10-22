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

// GPP6 (Bus 0 Dev 1 Fn 7)
Scope(\_SB.PCI0.GPP6)
{
/*
  Method (RHRS, 0, NotSerialized)
  {
    Name (RBUF, ResourceTemplate ()
    {
      GpioInt (Edge, ActiveLow, SharedAndWake, PullNone, 0x0000, "\\_SB.GPIO", 0x00, ResourceConsumer, , ) {02}    // PCIE_WAK
      GpioInt (Edge, ActiveHigh, SharedAndWake, PullNone, 0x0000, "\\_SB.GPIO", 0x00, ResourceConsumer, , ) {172}	 // NbGppPme	
    })
    Return(RBUF)		  // Yes, return GPIO interrupt info
  }

  Name(D0GP, 1)
  Name(D3GP, 0)

  PowerResource(P0GP, 0, 0) {
      Method(_STA) {
        TPST(MsGpp6Pr0StaMethod)
        Return(D0GP)
      }
      Method(_ON) {
        TPST(MsGpp6Pr0OnMethod)
        Store(0x01, D0GP)
      }
      Method(_OFF) {
        TPST(MsGpp6Pr0OffMethod)
      	Store(0x00, D0GP)
      }
    }
    PowerResource(P3GP, 0, 0) {
      Method(_STA) {
        TPST(MsGpp6Pr3StaMethod)
        Return(D3GP)
      }
      Method(_ON) {
        TPST(MsGpp6Pr3OnMethod)
        Store(0x01, D3GP)
      }
      Method(_OFF) {
        TPST(MsGpp6Pr3OffMethod)
        Store(0x00, D3GP)
      }
    }

     Name(MPR0, Package() {P0GP})
     Name(MPR3, Package() {P3GP})

    Method(MS0W,0)
    {
       TPST(MsGpp6S0wMethod)
       Return(4)
    }
    Method(MPS0,0)
    {
       TPST(MsGpp6Ps0Method)
    }
    Method(MPS3,0)
    {
       TPST(MsGpp6Ps3Method)
    }
*/
  #include "Nvme.asl"

}

