/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

//0x24 - DOCK is connected
//0x25 - DOCK is disconnected
//0x26 - Lid Open, LID_SWITCH L to H
//0x27 - Lid Close, LID_SWITCH H to L
//0x35 - AC Power in/out
//0x39 - Battery in/out
//0x3E - Battery Trip Point
//0x40 - (Runtime generation in CPM) Odd plugin falling edge
//0x41 - (Runtime generation in CPM) Odd plugin rising edge
//0x42 - (Runtime generation in CPM) Odd DA falling edge
//0x43 - (Runtime generation in CPM) Odd DA rising edge
//0x45 - Battery charge done

#define QEVENT_DEBUG

#ifdef  QEVENT_DEBUG
#define QEVENT_DEBUG_DELAY      1
#else
#define QEVENT_DEBUG_DELAY      1
#endif

//
//Dock is connected
//
//Method(_Q24,0,NotSerialized)
//{
//#ifdef  QEVENT_DEBUG
//  Store(0xEC24, P80H)
//#endif
//  \DOCK (0)
//}

//
//Dock is disconnected
//
//Method(_Q25,0,NotSerialized)
//{
//#ifdef  QEVENT_DEBUG
//  Store(0xEC25, P80H)
//#endif
//  \DOCK (1)
//}

//
//Lid open
//
Method(_Q26,0,NotSerialized)
{
#ifdef  QEVENT_DEBUG
  Store(0xEC26, P80H)
#endif
  Notify(\_SB.LID, 0x80)
}

//
//Lid close
//
Method(_Q27,0,NotSerialized)
{
#ifdef  QEVENT_DEBUG
  Store(0xEC27, P80H)
#endif
  Notify(\_SB.LID, 0x80)
}

//
//AC Power Insertion/Removal
//
Method(_Q35,0,NotSerialized)
{
#ifdef  QEVENT_DEBUG
  Store(0xEC35, P80H)
#endif
  if (LNot(Acquire(\_SB.PCI0.LPC0.EC0.QEVT, 300))) {
    Notify(\_SB.ACAD, 0x80)
    Notify(\_SB.BATT, 0x80)
    Release(\_SB.PCI0.LPC0.EC0.QEVT)
  }
}

//
// Battery Insertion
//
Method(_Q39,0,NotSerialized)
{
#ifdef  QEVENT_DEBUG
  Store(0xEC39, P80H)
#endif
  if (LNot(Acquire(\_SB.PCI0.LPC0.EC0.QEVT,300))) {
    Notify(\_SB.ACAD, 0x80)
    Notify(\_SB.BATT, 0x80)
    Notify(\_SB.BATT, 0x81)
    Release(\_SB.PCI0.LPC0.EC0.QEVT)
  }
}

//
// Battery Trip Point
//
Method(_Q3E,0,NotSerialized)
{
#ifdef  QEVENT_DEBUG
  Store(0xEC3E, P80H)
#endif
  // Write 1 to bit 7 of ECRAMxED to clear the event.
  if (LNot(Acquire(\_SB.PCI0.LPC0.EC0.QEVT,300))) {
    Store(\_SB.PCI0.LPC0.EC0.BATS, Local0)
    Or(Local0, 0x80, Local0)
    Store(Local0, \_SB.PCI0.LPC0.EC0.BATS)
    Release(\_SB.PCI0.LPC0.EC0.QEVT)
  }

  Notify(\_SB.BATT, 0x80)
}

//
// Battery charge done
//
Method(_Q45,0,NotSerialized)
{
#ifdef  QEVENT_DEBUG
  Store(0xEC45, P80H)
#endif

  Notify(\_SB.BATT, 0x80)
}

Name(DPTI, Buffer(0x7){})

//
// DPTC
//
Method(_Q49,0,NotSerialized)
{
#ifdef  QEVENT_DEBUG
  Store(0xEC49, P80H)
#endif
  If (LEqual(DPTC,0x01)) {
    CreateWordField(DPTI, 0, SSZE)        // Structure size field
    CreateByteField(DPTI, 2, MSID)
    CreateByteField(DPTI, 3, DECI)
    CreateByteField(DPTI, 4, INTE)
    CreateByteField(DPTI, 5, EP00)
    CreateByteField(DPTI, 6, EP01)


    Store(0x7, SSZE)                      // table size 7
    Store(0xA, MSID)
    Store(\_SB.PCI0.LPC0.EC0.MDEC,DECI)   // decimal part
    Store(\_SB.PCI0.LPC0.EC0.MINT,INTE)   // integer part
    Store(0x0, EP00)
    Store(0x0, EP01)

    \_SB.ALIB(0xC, DPTI)
  }
}

