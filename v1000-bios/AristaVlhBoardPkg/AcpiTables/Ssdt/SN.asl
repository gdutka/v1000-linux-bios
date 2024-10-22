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

#ifndef _SATN_ASL_
#define _SATN_ASL_

DefinitionBlock ("SN.aml", "SSDT", 1, "XXXXX", "OOOOO", 1)
{
    External(\_SB.PCI0, DeviceObj)
    External(TPOS)
    Scope(\_SB.PCI0)
    {
            Device(SATN)
            {
                Name(_ADR,0x00110000)
            }
      }
}
#endif

