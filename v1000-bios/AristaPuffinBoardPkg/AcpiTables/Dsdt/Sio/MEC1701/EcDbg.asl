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

#define STORE_ORG_DATA
#ifdef STRING_OUTPUT
Name (ECBF, Buffer (256) {0})
#endif

Method(ECDB,0,NotSerialized)
{
    Acquire(\_SB.PCI0.LPC0.EC0.Z009,0xFFFF)
#ifdef STRING_OUTPUT
    store(Concatenate (" Battery Device Name : ", ToString (\_SB.PCI0.LPC0.EC0.BDNE), ECBF) ,debug)
    store(Concatenate (" Chemistry : ", ToString (\_SB.PCI0.LPC0.EC0.CHTY), ECBF) ,debug)
    store(Concatenate (" Manufaturer name : ", ToString (\_SB.PCI0.LPC0.EC0.MANU), ECBF) ,debug)
    ShiftLeft(\_SB.PCI0.LPC0.EC0.BTPH, 8, Local0)
    Or(Local0, \_SB.PCI0.LPC0.EC0.BTPL, Local0)
    store(Concatenate (" Battery Trip Point : ", ToHexString (Local0), ECBF) ,debug)
    ShiftLeft(\_SB.PCI0.LPC0.EC0.MERH, 8, Local0)
    Or(Local0, \_SB.PCI0.LPC0.EC0.MERL, Local0)
    store(Concatenate (" MaxError : ", ToHexString (Local0), ECBF) ,debug)
    ShiftLeft(\_SB.PCI0.LPC0.EC0.CCTH, 8, Local0)
    Or(Local0, \_SB.PCI0.LPC0.EC0.CCTL, Local0)
    store(Concatenate (" CycleCount : ", ToHexString (Local0), ECBF) ,debug)
    store(Concatenate (" Miscellaneous Status and Control : ", ToHexString (\_SB.PCI0.LPC0.EC0.STAS), ECBF) ,debug)
    store(Concatenate (" Brightness : ", ToHexString (\_SB.PCI0.LPC0.EC0.BRTL), ECBF) ,debug)
    store(Concatenate (" Battery status : ", ToHexString (\_SB.PCI0.LPC0.EC0.BATS), ECBF) ,debug)
    ShiftLeft(\_SB.PCI0.LPC0.EC0.CAPH, 8, Local0)
    Or(Local0, \_SB.PCI0.LPC0.EC0.CAPL, Local0)
    store(Concatenate (" Remaining capacity : ", ToHexString (Local0), ECBF) ,debug)
    ShiftLeft(\_SB.PCI0.LPC0.EC0.BTVH, 8, Local0)
    Or(Local0, \_SB.PCI0.LPC0.EC0.BTVL, Local0)
    store(Concatenate (" Terminal voltage : ", ToHexString (Local0), ECBF) ,debug)
    ShiftLeft(\_SB.PCI0.LPC0.EC0.BTCH, 8, Local0)
    Or(Local0, \_SB.PCI0.LPC0.EC0.BTCL, Local0)
    store(Concatenate (" Terminal current : ", ToHexString (Local0), ECBF) ,debug)
    ShiftLeft(\_SB.PCI0.LPC0.EC0.DECH, 8, Local0)
    Or(Local0, \_SB.PCI0.LPC0.EC0.DECL, Local0)
    store(Concatenate (" DesignCapacity : ", ToHexString (Local0), ECBF) ,debug)
    ShiftLeft(\_SB.PCI0.LPC0.EC0.DEVH, 8, Local0)
    Or(Local0, \_SB.PCI0.LPC0.EC0.DEVL, Local0)
    store(Concatenate (" DesignVoltage : ", ToHexString (Local0), ECBF) ,debug)
    ShiftLeft(\_SB.PCI0.LPC0.EC0.LFCH, 8, Local0)
    Or(Local0, \_SB.PCI0.LPC0.EC0.LFCL, Local0)
    store(Concatenate (" Fully charged capacity : ", ToHexString (Local0), ECBF) ,debug)
    store(Concatenate (" Smart charger status : ", ToHexString (\_SB.PCI0.LPC0.EC0.CHSH), ECBF) ,debug)
#else
#ifdef ORG_DATA_OUTPUT
    store(" Battery Device Name : ",debug)
    store(\_SB.PCI0.LPC0.EC0.BDNE, debug)

    store(" Chemistry : ", debug)
    store(\_SB.PCI0.LPC0.EC0.CHTY, debug)

    store(" Manufaturer name : ", debug)
    store(\_SB.PCI0.LPC0.EC0.MANU, debug)

    store(" Battery Trip Point : ", debug)
    ShiftLeft(\_SB.PCI0.LPC0.EC0.BTPH, 8, Local0)
    Or(Local0, \_SB.PCI0.LPC0.EC0.BTPL, Local0)
    store(Local0 ,debug)

    store(" MaxError : ", debug)
    ShiftLeft(\_SB.PCI0.LPC0.EC0.MERH, 8, Local0)
    Or(Local0, \_SB.PCI0.LPC0.EC0.MERL, Local0)
    store(Local0 ,debug)

    store(" CycleCount : ", debug)
    ShiftLeft(\_SB.PCI0.LPC0.EC0.CCTH, 8, Local0)
    Or(Local0, \_SB.PCI0.LPC0.EC0.CCTL, Local0)
    store(Local0, debug)

    store(" Miscellaneous Status and Control : ", debug)
    store(\_SB.PCI0.LPC0.EC0.STAS,debug)

    store(" Brightness : ", debug)
    store(\_SB.PCI0.LPC0.EC0.BRTL, debug)

    store(" Battery status : ", debug)
    store(\_SB.PCI0.LPC0.EC0.BATS, debug)

    store(" Remaining capacity : ", debug)
    ShiftLeft(\_SB.PCI0.LPC0.EC0.CAPH, 8, Local0)
    Or(Local0, \_SB.PCI0.LPC0.EC0.CAPL, Local0)
    store(Local0, debug)

    store(" Terminal voltage : ", debug)
    ShiftLeft(\_SB.PCI0.LPC0.EC0.BTVH, 8, Local0)
    Or(Local0, \_SB.PCI0.LPC0.EC0.BTVL, Local0)
    store(Local0, debug)

    store(" Terminal current : ", debug)
    ShiftLeft(\_SB.PCI0.LPC0.EC0.BTCH, 8, Local0)
    Or(Local0, \_SB.PCI0.LPC0.EC0.BTCL, Local0)
    store(Local0, debug)

    store(" DesignCapacity : ", debug)
    ShiftLeft(\_SB.PCI0.LPC0.EC0.DECH, 8, Local0)
    Or(Local0, \_SB.PCI0.LPC0.EC0.DECL, Local0)
    store(Local0, debug)

    store(" DesignVoltage : ", debug)
    ShiftLeft(\_SB.PCI0.LPC0.EC0.DEVH, 8, Local0)
    Or(Local0, \_SB.PCI0.LPC0.EC0.DEVL, Local0)
    store(Local0, debug)

    store(" Fully charged capacity : ", debug)
    ShiftLeft(\_SB.PCI0.LPC0.EC0.LFCH, 8, Local0)
    Or(Local0, \_SB.PCI0.LPC0.EC0.LFCL, Local0)
    store(Local0, debug)

    store(" Smart charger status : ", debug)
    store(\_SB.PCI0.LPC0.EC0.CHSH, debug)
#endif
#ifdef STORE_ORG_DATA
    store(0, ETPH)
    store(0, ETPL)
    store(0, EAPH)
    store(0, EAPL)
    store(0, EFCH)
    store(0, EFCL)
    store(0, EATS)
    store(0, EHSH)

    store(\_SB.PCI0.LPC0.EC0.BTPL, ETPL)
    store(\_SB.PCI0.LPC0.EC0.BTPH, ETPH)
    store(\_SB.PCI0.LPC0.EC0.CAPL, EAPL)
    store(\_SB.PCI0.LPC0.EC0.CAPH, EAPH)
    store(\_SB.PCI0.LPC0.EC0.LFCL, EFCL)
    store(\_SB.PCI0.LPC0.EC0.LFCH, EFCH)
    store(\_SB.PCI0.LPC0.EC0.BATS, EATS)
    store(\_SB.PCI0.LPC0.EC0.CHSH, EHSH)
#endif
#endif
    Release(\_SB.PCI0.LPC0.EC0.Z009)
}