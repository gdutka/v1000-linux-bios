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

#ifndef _SDIO_
#define _SDIO_

  Device(SDIO) {
    Name(_ADR, 0x00140007)

        // BIOS will patch RHRS to _CRS if HW_REDUCED_ACPI is enabled
        //Method (RHRS, 0, NotSerialized)
        Method (_CRS, 0, NotSerialized)
        {
            Name (RBUF, ResourceTemplate ()
            {
                GpioInt (Level, ActiveLow, ExclusiveAndWake, PullUp, 0x0000, "\\_SB.GPIO", 0x00, ResourceConsumer, , )
                    {25}    // Int_iusb_wakeup0
            })
            Return(RBUF)        // Yes, return GPIO interrupt info
        }
  } // End Device(SDIO)

#endif
