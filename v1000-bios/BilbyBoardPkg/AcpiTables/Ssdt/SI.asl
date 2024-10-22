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

#ifndef _SATI1_ASL_
#define _SATI1_ASL_

DefinitionBlock ("SI.aml", "SSDT", 1, "XXXXX", "OOOOO", 1)
{
    External(\_SB.PCI0, DeviceObj)
    External(TPOS)
    Scope(\_SB.PCI0)
    {
            Device(SATI)
            {
                Name(_ADR,0x00110000)
                Name(B5EN,0x00)
                Name(BA_5,0x00)
                Name(SBAR,0xFFFFFFFF)
                OperationRegion(SATX,PCI_Config,0x00,0x28)
                Field(SATX,AnyAcc,NoLock,Preserve)
                {
                    VIDI,32,
                    Offset(0x24),
                    BA05,32
                }
                Method(GBAA,0,Serialized)
                {
                    Store(BA05,BA_5)
                    If(LEqual(BA_5,0xFFFFFFFF))
                    {
                        Store(0x00,B5EN)
                        Return(SBAR)
                    }
                    Else
                    {
                        Store(0x01,B5EN)
                        Return(BA_5)
                    }
                }
                OperationRegion(BAR5,SystemMemory,GBAA(),0x1000)
                Field(BAR5,AnyAcc,NoLock,Preserve)
                {
                    Offset(0x120),
                    ,7,
                    PMBY,1,
                    Offset(0x128),
                    PMS0,4,
                    Offset(0x129),
                    PMS1,4,
                    Offset(0x12C),
                    DET0,4,
                    Offset(0x130),
                    Offset(0x132),
                    PRC0,1,
                    Offset(0x1A0),
                    ,7,
                    SMBY,1,
                    Offset(0x1A8),
                    SMS0,4,
                    Offset(0x1A9),
                    SMS1,4,
                    Offset(0x1AC),
                    DET1,4,
                    Offset(0x1B0),
                    Offset(0x1B2),
                    PRC1,1,
                    Offset(0x220),
                    ,7,
                    PSBY,1,
                    Offset(0x228),
                    PSS0,4,
                    Offset(0x229),
                    PSS1,4,
                    Offset(0x22C),
                    DET2,4,
                    Offset(0x230),
                    Offset(0x232),
                    PRC2,1,
                    Offset(0x2A0),
                    ,7,
                    SSBY,1,
                    Offset(0x2A8),
                    SSS0,4,
                    Offset(0x2A9),
                    SSS1,4,
                    Offset(0x2AC),
                    DET3,4,
                    Offset(0x2B0),
                    Offset(0x2B2),
                    PRC3,1
                }
                Method(_INI,0,NotSerialized)
                {
                    GBAA()
                    \_GPE._L1F()
                }
                Device(PRID)
                {
                    Name(_ADR,0x00)
                    Name(SPTM,Buffer(0x14){0x78,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,
                            0x78,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,
                            0x1F,0x00,0x00,0x00})
                    Method(_GTM,0,NotSerialized)
                    {
                        Return(SPTM)
                    }
                    Method(_STM,3,NotSerialized)
                    {
                        Store(Arg0,SPTM)
                    }
                    Name(PRIS,0x00)
                    Method(_PS0,0,NotSerialized)
                    {
                        GBAA()
                        If(LAnd(LOr(LGreaterEqual(TPOS,0x40),LEqual(TPOS,0x04)),\_SB.PCI0.SATI.B5EN))
                        {
                            If(\_SB.PCI0.SATI.PMS1)
                            {
                                Store(0x32,Local0)
                                While(LAnd(LEqual(\_SB.PCI0.SATI.PMBY,0x01),Local0))
                                {
                                    Sleep(0xFA)
                                    Decrement(Local0)
                                }
                            }
                            If(\_SB.PCI0.SATI.PSS1)
                            {
                                Store(0x32,Local0)
                                While(LAnd(LEqual(\_SB.PCI0.SATI.PSBY,0x01),Local0))
                                {
                                    Sleep(0xFA)
                                    Decrement(Local0)
                                }
                            }
                        }
                        Store(0x00,PRIS)
                    }
                    Method(_PS3,0,NotSerialized)
                    {
                        Store(0x03,PRIS)
                    }
                    Method(_PSC,0,NotSerialized)
                    {
                        Return(PRIS)
                    }
                    Device(P_D0)
                    {
                        Name(_ADR,0x00)
                        Method(_STA,0,NotSerialized)
                        {
                            GBAA()
                            If(LEqual(B5EN,0x00))
                            {
                                Return(0x00)
                            }
                            If(Not(LEqual(\_SB.PCI0.SATI.PMS1,0x00)))
                            {
                                Return(0x0F)
                            }
                            Else
                            {
                                Return(0x00)
                            }
                        }
                        Name(S12P,0x00)
                        Method(_PS0,0,NotSerialized)
                        {
                            GBAA()
                            If(LAnd(LAnd(LNot(LGreaterEqual(TPOS,0x40)),LNot(LEqual(TPOS,0x04))),\_SB.PCI0.SATI.B5EN))
                            {
                                Store(0x32,Local0)
                                While(LAnd(LEqual(\_SB.PCI0.SATI.PMBY,0x01),Local0))
                                {
                                    Sleep(0xFA)
                                    Decrement(Local0)
                                }
                            }
                            Store(0x00,S12P)
                        }
                        Method(_PS3,0,NotSerialized)
                        {
                            Store(0x03,S12P)
                        }
                        Method(_PSC,0,NotSerialized)
                        {
                            Return(S12P)
                        }
                        Method(_GTF,0,NotSerialized)
                        {
                            Store(Buffer(0x07){0x03,0x46,0x00,0x00,0x00,0xA0,0xEF},Local0)
                            Return(Local0)
                        }
                    }
                    Device(P_D1)
                    {
                        Name(_ADR,0x01)
                        Method(_STA,0,NotSerialized)
                        {
                            GBAA()
                            If(LEqual(B5EN,0x00))
                            {
                                Return(0x00)
                            }
                            If(Not(LEqual(\_SB.PCI0.SATI.SMS1,0x00)))
                            {
                                Return(0x0F)
                            }
                            Else
                            {
                                Return(0x00)
                            }
                        }
                        Name(S12P,0x00)
                        Method(_PS0,0,NotSerialized)
                        {
                            GBAA()
                            If(LAnd(LAnd(LNot(LGreaterEqual(TPOS,0x40)),LNot(LEqual(TPOS,0x04))),\_SB.PCI0.SATI.B5EN))
                            {
                                Store(0x32,Local0)
                                While(LAnd(LEqual(\_SB.PCI0.SATI.PSBY,0x01),Local0))
                                {
                                    Sleep(0xFA)
                                    Decrement(Local0)
                                }
                            }
                            Store(0x00,S12P)
                        }
                        Method(_PS3,0,NotSerialized)
                        {
                            Store(0x03,S12P)
                        }
                        Method(_PSC,0,NotSerialized)
                        {
                            Return(S12P)
                        }
                        Method(_GTF,0,NotSerialized)
                        {
                            Store(Buffer(0x07){0x03,0x46,0x00,0x00,0x00,0xA0,0xEF},Local0)
                            Return(Local0)
                        }
                    }
                }
                Device(SECD)
                {
                    Name(_ADR,0x01)
                    Name(SPTM,Buffer(0x14){0x78,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,
                            0x78,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,
                            0x1F,0x00,0x00,0x00})
                    Method(_GTM,0,NotSerialized)
                    {
                        Return(SPTM)
                    }
                    Method(_STM,3,NotSerialized)
                    {
                        Store(Arg0,SPTM)
                    }
                    Name(SECS,0x00)
                    Method(_PS0,0,NotSerialized)
                    {
                        GBAA()
                        If(LAnd(LOr(LGreaterEqual(TPOS,0x40),LEqual(TPOS,0x04)),\_SB.PCI0.SATI.B5EN))
                        {
                            If(\_SB.PCI0.SATI.SMS1)
                            {
                                Store(0x32,Local0)
                                While(LAnd(LEqual(\_SB.PCI0.SATI.SMBY,0x01),Local0))
                                {
                                    Sleep(0xFA)
                                    Decrement(Local0)
                                }
                            }
                            If(\_SB.PCI0.SATI.SSS1)
                            {
                                Store(0x32,Local0)
                                While(LAnd(LEqual(\_SB.PCI0.SATI.SSBY,0x01),Local0))
                                {
                                    Sleep(0xFA)
                                    Decrement(Local0)
                                }
                            }
                        }
                        Store(0x00,SECS)
                    }
                    Method(_PS3,0,NotSerialized)
                    {
                        Store(0x03,SECS)
                    }
                    Method(_PSC,0,NotSerialized)
                    {
                        Return(SECS)
                    }
                    Device(S_D0)
                    {
                        Name(_ADR,0x00)
                        Method(_STA,0,NotSerialized)
                        {
                            GBAA()
                            If(LEqual(B5EN,0x00))
                            {
                                Return(0x00)
                            }
                            If(Not(LEqual(\_SB.PCI0.SATI.PSS1,0x00)))
                            {
                                Return(0x0F)
                            }
                            Else
                            {
                                Return(0x00)
                            }
                        }
                        Name(S12P,0x00)
                        Method(_PS0,0,NotSerialized)
                        {
                            GBAA()
                            If(LAnd(LAnd(LNot(LGreaterEqual(TPOS,0x40)),LNot(LEqual(TPOS,0x04))),\_SB.PCI0.SATI.B5EN))
                            {
                                Store(0x32,Local0)
                                While(LAnd(LEqual(\_SB.PCI0.SATI.SMBY,0x01),Local0))
                                {
                                    Sleep(0xFA)
                                    Decrement(Local0)
                                }
                            }
                            Store(0x00,S12P)
                        }
                        Method(_PS3,0,NotSerialized)
                        {
                            Store(0x03,S12P)
                        }
                        Method(_PSC,0,NotSerialized)
                        {
                            Return(S12P)
                        }
                        Method(_GTF,0,NotSerialized)
                        {
                            Store(Buffer(0x07){0x03,0x46,0x00,0x00,0x00,0xA0,0xEF},Local0)
                            Return(Local0)
                        }
                    }
                    Device(S_D1)
                    {
                        Name(_ADR,0x01)
                        Method(_STA,0,NotSerialized)
                        {
                            GBAA()
                            If(LEqual(B5EN,0x00))
                            {
                                Return(0x00)
                            }
                            If(Not(LEqual(\_SB.PCI0.SATI.SSS0,0x00)))
                            {
                                Return(0x0F)
                            }
                            Else
                            {
                                Return(0x00)
                            }
                        }
                        Name(S12P,0x00)
                        Method(_PS0,0,NotSerialized)
                        {
                            GBAA()
                            If(LAnd(LAnd(LNot(LGreaterEqual(TPOS,0x40)),LNot(LEqual(TPOS,0x04))),\_SB.PCI0.SATI.B5EN))
                            {
                                Store(0x32,Local0)
                                While(LAnd(LEqual(\_SB.PCI0.SATI.SSBY,0x01),Local0))
                                {
                                    Sleep(0xFA)
                                    Decrement(Local0)
                                }
                            }
                            Store(0x00,S12P)
                        }
                        Method(_PS3,0,NotSerialized)
                        {
                            Store(0x03,S12P)
                        }
                        Method(_PSC,0,NotSerialized)
                        {
                            Return(S12P)
                        }
                        Method(_GTF,0,NotSerialized)
                        {
                            Store(Buffer(0x07){0x03,0x46,0x00,0x00,0x00,0xA0,0xEF},Local0)
                            Return(Local0)
                        }
                    }
                }
            }
            Scope(\_GPE)
            {
                Method(_L1F,0,NotSerialized)
                {
                    \_SB.PCI0.SATI.GBAA()
                    If(\_SB.PCI0.SATI.B5EN)
                    {
                        If(\_SB.PCI0.SATI.PRC0)
                        {
                            If(Not(LEqual(\_SB.PCI0.SATI.PMS1,0x00)))
                            {
                                Sleep(0x1E)
                            }
                            Notify(\_SB.PCI0.SATI.PRID.P_D0,0x01)
                            Store(One,\_SB.PCI0.SATI.PRC0)
                        }
                        If(\_SB.PCI0.SATI.PRC1)
                        {
                            If(Not(LEqual(\_SB.PCI0.SATI.SMS1,0x00)))
                            {
                                Sleep(0x1E)
                            }
                            Notify(\_SB.PCI0.SATI.SECD.S_D0,0x01)
                            Store(One,\_SB.PCI0.SATI.PRC1)
                        }
                        If(\_SB.PCI0.SATI.PRC2)
                        {
                            If(Not(LEqual(\_SB.PCI0.SATI.PSS1,0x00)))
                            {
                                Sleep(0x1E)
                            }
                            Notify(\_SB.PCI0.SATI.PRID.P_D1,0x01)
                            Store(One,\_SB.PCI0.SATI.PRC2)
                        }
                        If(\_SB.PCI0.SATI.PRC3)
                        {
                            If(Not(LEqual(\_SB.PCI0.SATI.SSS1,0x00)))
                            {
                                Sleep(0x1E)
                            }
                            Notify(\_SB.PCI0.SATI.SECD.S_D1,0x01)
                            Store(One,\_SB.PCI0.SATI.PRC3)
                        }
                    }
                }
            }
      }
}
#endif

