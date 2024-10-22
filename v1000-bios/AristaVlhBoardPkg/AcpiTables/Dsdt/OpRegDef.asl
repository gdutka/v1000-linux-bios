/** @file
  Operation region definition
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

#ifndef _OP_REG_ASI_
#define _OP_REG_ASI_

#define MMSO  0xFED80000
#define MMSG  0xFED81500
#define HPAD  0xFED00000

//External(\_SB.ALIB, MethodObj)
External(\_SB.APTS, MethodObj)
External(\_SB.AWAK, MethodObj)
External(\_SB.APPM, MethodObj)
External(MPTS, MethodObj)
External(MWAK, MethodObj)

External(AFN0, MethodObj)
External(AFN3, MethodObj)
External(AFN4, MethodObj)
External(AFN5, MethodObj)
External(AFN6, MethodObj)
External(AFN7, MethodObj)
External(\_SB.PCI0.VGA.LCD, DeviceObj)
External(\_SB.PCI0.GFX0.VGA.LCD, DeviceObj)

//
// 80 Port
//
OperationRegion (DBG0, SystemIO, 0x80, 0x1)
Field (DBG0, ByteAcc, NoLock, Preserve)
{
  IO80, 8
}

OperationRegion (DBG1, SystemIO, 0x80, 0x2)
Field (DBG1, WordAcc, NoLock, Preserve)
{
  P80H, 16
}

//
// Access CMOS range
//
OperationRegion (ACMS, SystemIO, 0x72, 0x2)
Field (ACMS, ByteAcc, NoLock, Preserve)
{
  ACMX, 8,
  ACMA, 8
}

IndexField(ACMX, ACMA, ByteAcc, NoLock, Preserve) {
  Offset(0xB9),
  IMEN, 8,
}

//
// SMI_CMD Port
//
OperationRegion (PSMI, SystemIO, PcdGet8 (PcdSoftwareSmiPort), 0x2)
Field (PSMI, ByteAcc, NoLock, Preserve)
{
  APMC, 8,
  APMD, 8
}

//
// Create an opregion for PM IO Registers
//
OperationRegion (PMRG, SystemIO, 0xCD6, 0x2)
Field (PMRG, ByteAcc, NoLock, Preserve)
{
  PMRI, 8,
  PMRD, 8
}
IndexField (PMRI, PMRD, ByteAcc, NoLock, Preserve)
{
  Offset(0x00),               // HpetEn
  ,6,
  HPEN, 1,
  Offset(0x60),               // AcpiPm1EvgBlk
  P1EB, 16,
  Offset(0xF0),
  ,3,
  RSTU, 1
}

//OperationRegion(GSMG, SystemMemory, MMSG, 0x3FF)
//Field(GSMG, AnyAcc, NoLock, Preserve)
//{
//}

OperationRegion(GSMM, SystemMemory, MMSO, 0x001000)
Field(GSMM, AnyAcc, NoLock, Preserve)
{
  Offset(0x0288),     // Slp_Type_event65
  ,1,
  CLPS,1,             // SMI status of Slp_Type
  Offset(0x02B0),     // SMI_REGB0
  ,2,
  SLPS,2,             // SMI SLP_TYPE
  Offset(0x03BB),     // WakePinEnable
  ,6,
  PWDE,1,             // enable wakeup from WAKE# pin.
  Offset(0x03E4),     // ABConfig
  BLNK,2,             // Blink Control
}

OperationRegion (P1E0, SystemIO, P1EB, 0x4)
Field (P1E0, ByteAcc, Nolock, Preserve)
{
  ,14,
  PEWS,1,
  WSTA,1,
  Offset(0x02),
  ,14,
  PEWD,1,
}

//
// ACPI IO operation region
//
OperationRegion (IOCC, SystemIO, 0x400, 0x80)
Field (IOCC, ByteAcc, NoLock, Preserve) {
  Offset (0x1),
  ,2,
  RTCS, 1,
}

Name (PRWP,Package(0x02){
  Zero,
  Zero
})
Method (GPRW,2,NotSerialized)
{
  Store (Arg0, Index (PRWP, 0x00))
  Store (Arg1, Index (PRWP, 0x01))
  If (LEqual (DAS3, 0x0))
  {
    If (LLessEqual (Arg1, 0x03))
    {
        Store (0x00, Index (PRWP, 0x01))
    }
  }
  Return (PRWP)
}

Method (SPTS, 1)
{
  //Set Blink LED according to sleep state, for S4/S5 leave it OFF always.
  if (LEqual(Arg0, 3))
  {
    Store(0x01, \BLNK)
  }
  if (Lor(LEqual(Arg0, 4), LEqual(Arg0, 5)))
  {
    Store(0x00, \BLNK)
  }

  If (LEqual(Arg0,3))
  {
    Store(zero, RSTU)     // Clear to make pci_rst not reset USB in S3
  }
  Store(One, CLPS)        // Clear SLP_SMI Status
  Store(One, SLPS)        // Enable SLP_SMI
  Store(PEWS, PEWS)       // Clear PciExpWakeStatus
}

Method (SWAK, 1)
{
  Store(0x03, \BLNK)    // Turn on Blink LED for S0 stateb

  If (LEqual(Arg0,3))
  {
      Store(one, RSTU)    // Set RSTU to make pci_rst reset USB in S3
  }
  Store(\PEWS, \PEWS)     // Clear PciExpWakeStatus
  Store(one, \PWDE)       // Set PciExpWakeDisEn
  Store(zero, \PEWD)      // Clear PciExpWakeDisEn
}

Method (TPST, 1, Serialized) {
    Add(Arg0, BiosAslTpbasevalue, Local0)
    OperationRegion(varM, SystemIO, 0x80, 0x4)
        Field(varM, DWordAcc, NoLock, Preserve) {
            varR, 32
        }
    Store (Local0, varR)
}

#endif
