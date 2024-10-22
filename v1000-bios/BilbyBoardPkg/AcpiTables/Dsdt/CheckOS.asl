/** @file
  Check OS version for MicroSoft OS
;*******************************************************************************
;* Copyright (c) 2013 - 2015, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#ifndef _CHECKOS_ASI_
#define _CHECKOS_ASI_

Method (SCMP, 2)
{
  Name (STG1, Buffer(80){})
  Name (STG2, Buffer(80){})

  Store (Arg0, STG1)
  Store (Arg1, STG2)

  If (LNotEqual(Sizeof(Arg0), Sizeof(Arg1)))
  {
    Return(Zero)
  }

  Store (Zero, Local0)
  While (LLess(Local0, Sizeof(Arg0)))
  {
    If(LNotEqual(Derefof(Index(STG1, Local0)), Derefof(Index(STG2, Local0))))
    {
      Return(Zero)
    }
    Increment(Local0)
  }
  Return(One)
}

Name (WNOS, 0)
Name (MYOS, Zero)
Name (HTTS, Zero)
Name (OSTB,Ones)
Name (TPOS, Zero)
Name (LINX, 0x00)
Name (OSSP, 0x00)

Method (SEQL,2,Serialized)
{
  Store (SizeOf(Arg0),Local0)
  Store (SizeOf(Arg1),Local1)
  If (LNot(LEqual(Local0,Local1)))
  {
    Return (Zero)
  }
  Name (BUF0,Buffer(Local0){})
  Store (Arg0,BUF0)
  Name (BUF1,Buffer(Local0){})
  Store (Arg1,BUF1)
  Store (Zero,Local2)
  While (LLess(Local2,Local0))
  {
    Store (DeRefOf(Index(BUF0,Local2)),Local3)
      Store (DeRefOf(Index(BUF1,Local2)),Local4)
      If (LNot(LEqual(Local3,Local4)))
      {
        Return (Zero)
      }
    Increment (Local2)
  }
  Return (One)
}

Method (OSTP,0,NotSerialized)
{
  If(LEqual(OSTB,Ones))
  {
    If(CondRefOf(\_OSI,Local0))
    {
      Store(0x00,OSTB)
      Store(0x00,TPOS)
      If(\_OSI("Windows 2001"))
      {
        Store(0x08,OSTB)
        Store(0x08,TPOS)
      }
      If(\_OSI("Windows 2001.1"))
      {
        Store(0x20,OSTB)
        Store(0x20,TPOS)
      }
      If(\_OSI("Windows 2001 SP1"))
      {
        Store(0x10,OSTB)
        Store(0x10,TPOS)
      }
      If(\_OSI("Windows 2001 SP2"))
      {
        Store(0x11,OSTB)
        Store(0x11,TPOS)
      }
      If(\_OSI("Windows 2001 SP3"))
      {
        Store(0x12,OSTB)
        Store(0x12,TPOS)
      }
      If(\_OSI("Windows 2006"))
      {
        Store(0x40,OSTB)
        Store(0x40,TPOS)
      }
      If(\_OSI("Windows 2006 SP1"))
      {
        Store(0x41,OSTB)
        Store(0x41,TPOS)
        Store(0x01, OSSP)
      }
      If(\_OSI("Windows 2009"))
      {
        Store(1, OSSP)
        Store(0x50, OSTB)
        Store(0x50, TPOS)
      }
      If(\_OSI("Windows 2012"))
      {
        Store(1, OSSP)
        Store(0x60, OSTB)       // OSTYPE_WINDOWS8
        Store(0x60, TPOS)
      }
      If(\_OSI("Windows 2013"))
      {
        Store(1, OSSP)
        Store(0x61, OSTB)       // OSTYPE_WINDOWS8.1
        Store(0x61, TPOS)
      }
      If(\_OSI("Windows 2015"))
      {
        Store(1, OSSP)
        Store(0x70, OSTB)       // OSTYPE_WINDOWS 10
        Store(0x70, TPOS)
      }
      If(\_OSI("Linux"))
      {
        Store(0x01,LINX)
        Store(0x80,OSTB)
        Store(0x80,TPOS)
      }
    }
    Else
    {
      If(CondRefOf(\_OS,Local0))
      {
        If(SEQL(\_OS,"Microsoft Windows"))
        {
          Store(0x01,OSTB)
          Store(0x01,TPOS)
        }
        Else
        {
          If(SEQL(\_OS,"Microsoft WindowsME: Millennium Edition"))
          {
            Store(0x02,OSTB)
            Store(0x02,TPOS)
          }
          Else
          {
            If(SEQL(\_OS,"Microsoft Windows NT"))
            {
              Store(0x04,OSTB)
              Store(0x04,TPOS)
            }
            Else
            {
              Store(0x00,OSTB)
              Store(0x00,TPOS)
            }
          }
        }
      }
      Else
      {
        Store(0x00,OSTB)
        Store(0x00,TPOS)
      }
    }
  }
  Return(OSTB)
}

#endif
