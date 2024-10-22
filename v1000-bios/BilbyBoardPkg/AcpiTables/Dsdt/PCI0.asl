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

#ifndef _PCI0_
#define _PCI0_

Device(PCI0)
{
  Name(_HID,EISAID("PNP0A08"))            // PCI Express Bus
  Name(_CID,EISAID("PNP0A03"))
  Name(_UID,0x01)
  Name(_BBN,0x00)
  Name(_ADR, 0x00000000)                  // HW:dev#, LW:fnct#=0
  //
  // _INI method
  //
  #include "_INI.asl"

  Name(SUPP,0) // PCI _OSC Support Field value
  Name(CTRL,0) // PCI _OSC Control Field value
  Method(_OSC, 4) {
    // Note: Since these DwordFields are accessed from the else
    // condition of UUID check, they cannot be created inside the
    // If(LEqual .... ) scope.  If put inside the LEqual, it will
    // cause problem if the code ever executes the Else (unrecognized UUID)
    // condition.

    // Create DWord-adressable fields from the Capabilities Buffer
    CreateDWordField(Arg3,0,CDW1)
    CreateDWordField(Arg3,4,CDW2)
    CreateDWordField(Arg3,8,CDW3)

    // Check for proper UUID
    If(LEqual(Arg0,ToUUID("33DB4D5B-1FF7-401C-9657-7441C03DD766"))) {
      // Save Capabilities DWord2 & 3
      Store(CDW2,SUPP)
      Store(CDW3,CTRL)
      // Only allow native hot plug control if OS supports:
      // * ASPM
      // * Clock PM
      // * MSI/MSI-X
      If(LNotEqual(And(SUPP, 0x16), 0x16)) {
        And(CTRL,0x1E,CTRL) // Mask bit 0 (and undefined bits)
      }

      // Always allow native PME, AER (no dependencies)
      // Never allow SHPC (no SHPC controller in this system)
      And(CTRL,0x1D,CTRL)

      If(Not(And(CDW1,1))) { // Query flag clear?
        // Disable GPEs for features granted native control.
        If(And(CTRL,0x01)) { // Hot plug control granted?
          //Store(0,HPCE) // clear the hot plug SCI enable bit
          //Store(1,HPCS) // clear the hot plug SCI status bit
        }

        //                                               Native    Legacy
        // PMIO:0x74[24] - PcieNative            1         0
        // PMIO:0x74[25] - PcieWakMask        0         0
        // PMIO:0x74[27] - WakePinAsGevent  1         0
        // PMIO:0xBA[14] - WakePinEnable      0         1
        If (And(CTRL,0x04)) { // Native PME control granted?
          // Handled during POST according to user option
        }

        If(And(CTRL,0x10)) {// OS restoring PCIe cap structure?
          // Set status to not restore PCIe cap structure
          // upon resume from S3
        }
      }

      If(LNotEqual(Arg1,One)) {
        // Unknown revision
        Or(CDW1,0x08,CDW1)
      }

      If(LNotEqual(CDW3,CTRL)) {
        // Capabilities bits were masked
        Or(CDW1,0x10,CDW1)
      }

      // Update DWORD3 in the buffer
      Store(CTRL,CDW3)
      Return(Arg3)
    } Else {
      Or(CDW1,4,CDW1) // Unrecognized UUID
      Return(Arg3)
    }
  } // End _OSC

  Method(TOM) {
    Multiply(TOML, 0x10000, Local0)
    Multiply(TOMH, 0x1000000, Local1)
    Add(Local0, Local1, Local0)
    Return(Local0)
  } // END of TOM

  // Bridge _CRS
  Name(CRES, ResourceTemplate() {
    WORDBusNumber(
      ResourceProducer,
      MinFixed,               // _MIF
      MaxFixed,               // _MAF
      SubDecode,              // _DEC
      0x00,                   // _GRA
      0x00,                   // _MIN
      0x0FF,                  // _MAX
      0x00,                   // _TRA
      0x0100,                 // _LEN
      0x00, , WBS             // name of this descriptor
    )

    WORDIO(ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange, 0x0, 0x0, 0xCF7, 0x0, 0xCF8, 0x0, , BCF8)
    WORDIO(ResourceProducer, MinFixed, MaxFixed, PosDecode, EntireRange, 0x0, 0xD00, 0xFFFF, 0x0, 0xF300)
    DWordMemory(ResourceProducer, SubDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite, 0x0, 0xA0000, 0xBFFFF, 0x0, 0x20000, 0x0, , A000)
    DWordMemory(ResourceProducer, SubDecode, MinFixed, MaxFixed, Cacheable,  ReadOnly, 0x0, 0xC0000, 0xC3FFF, 0x0, 0x04000, 0x0, , C000)
    DWordMemory(ResourceProducer, SubDecode, MinFixed, MaxFixed, Cacheable,  ReadOnly, 0x0, 0xC4000, 0xC7FFF, 0x0, 0x04000, 0x0, , C400)
    DWordMemory(ResourceProducer, SubDecode, MinFixed, MaxFixed, NonCacheable, ReadOnly, 0x0, 0xC8000, 0xCBFFF, 0x0, 0x04000, 0x0, , C800)
    DWordMemory(ResourceProducer, SubDecode, MinFixed, MaxFixed, NonCacheable, ReadOnly, 0x0, 0xCC000, 0xCFFFF, 0x0, 0x04000, 0x0, , CC00)
    DWordMemory(ResourceProducer, SubDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite, 0x0, 0xD0000, 0xD3FFF, 0x0, 0x04000, 0x0, , D000)
    DWordMemory(ResourceProducer, SubDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite, 0x0, 0xD4000, 0xD7FFF, 0x0, 0x04000, 0x0, , D400)
    DWordMemory(ResourceProducer, SubDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite, 0x0, 0xD8000, 0xDBFFF, 0x0, 0x04000, 0x0, , D800)
    DWordMemory(ResourceProducer, SubDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite, 0x0, 0xDC000, 0xDFFFF, 0x0, 0x04000, 0x0, , DC00)
    DWordMemory(ResourceProducer, SubDecode, MinFixed, MaxFixed, Cacheable, ReadWrite, 0x0, 0xE0000, 0xE3FFF, 0x0, 0x04000, 0x0, , E000)
    DWordMemory(ResourceProducer, SubDecode, MinFixed, MaxFixed, Cacheable, ReadWrite, 0x0, 0xE4000, 0xE7FFF, 0x0, 0x04000, 0x0, , E400)
    DWordMemory(ResourceProducer, SubDecode, MinFixed, MaxFixed, Cacheable, ReadWrite, 0x0, 0xE8000, 0xEBFFF, 0x0, 0x04000, 0x0, , E800)
    DWordMemory(ResourceProducer, SubDecode, MinFixed, MaxFixed, Cacheable, ReadWrite, 0x0, 0xEC000, 0xEFFFF, 0x0, 0x04000, 0x0, , EC00)
    DWordMemory(ResourceProducer, SubDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite, 0x0, 0x80000000, 0xF7FFFFFF, 0x0, 0x78000000, 0x0, , BTS)
    //
    // To exclude TPM Area ( 0xFED40000 - 0xFED44FFF ) from PCI Host Bridge.
    //
//[-start-210507-IB06070107-modify]//
    DWordMemory(ResourceProducer, SubDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite, 0x0, 0xFC000000, 0xFEAFFFFF, 0x0, 0x2B00000, 0x0, , BTS1)
//[-end-210507-IB06070107-modify]//
//    DWordMemory(ResourceProducer, SubDecode, MinFixed, MaxFixed, NonCacheable, ReadWrite, 0x0, 0xFED45000, 0xFFFFFFFF, 0x0, 0x12BB000, 0x0, , BTS2)
    IO(Decode16, 0xCF8, 0xCF8, 0x1, 0x8, CF8)
//>>EnableAbove4GBMmio++
    QWORDMEMORY (             // Descriptor for HIGH PCI MMIO
            ResourceProducer, // bit 0 of general flags is 0
            PosDecode,
            MinFixed,         // Range is fixed
            MaxFixed,         // Range is fixed
            NonCacheable,
            ReadWrite,
            0x00000000,       // Granularity
            0,                // Min (Base)
            0,                // Max (Base + Length -1)
            0x00000000,       // Translation
            0                 // Range Length
            ,,,
            PM01
            )

    QWORDMEMORY (             // Descriptor for HIGH PCI MMIO
            ResourceProducer, // bit 0 of general flags is 0
            PosDecode,
            MinFixed,         // Range is fixed
            MaxFixed,         // Range is fixed
            NonCacheable,
            ReadWrite,
            0x00000000,       // Granularity
            0,                // Min (Base)
            0,                // Max (Base + Length -1)
            0x00000000,       // Translation
            0                 // Range Length
            ,,,
            PM02
            )
//<<EnableAbove4GBMmio++
  }) //End of PCI0.CRES

  Method(_CRS,0) {
    // the gap between the top of memory and the end of memory.
    CreateDWordField(CRES, ^BTS._MIN, BTMN)
    CreateDWordField(CRES, ^BTS._MAX, BTMX)
    CreateDWordField(CRES, ^BTS._LEN, BTLN)
    CreateDWordField(CRES, ^BTS1._MIN, BTN1)
    CreateDWordField(CRES, ^BTS1._MAX, BTX1)
    CreateDWordField(CRES, ^BTS1._LEN, BTL1)
    Store(\_SB.PCI0.TOM, BTMN)
    Subtract(PCBA, 1, BTMX)
    Subtract(PCBA, BTMN, BTLN)
    Add(PCBL, 1, BTN1)
    Subtract(BTX1, BTN1, BTL1)
    Add(BTL1, 1, BTL1)
////>>EnableAbove4GBMmio++
//  // High MMIO space
    If (LEqual(MM64,1)) {
      CreateQwordField(CRES, ^PM01._MIN, M1MN)
      CreateQwordField(CRES, ^PM01._MAX, M1MX)
      CreateQwordField(CRES, ^PM01._LEN, M1LN)
      //
      // Set 64bit MMIO resource Base and Length
      //
      Store (HMB1, M1MN) // HMB1, 64 bits MMIO base address
      Store (HMM1, M1MX) // HMM1, 64 bits MMIO limit
      Store (HML1, M1LN) // HML1, 64 bits MMIO length
//
      CreateQwordField(CRES, ^PM02._MIN, M2MN)
      CreateQwordField(CRES, ^PM02._MAX, M2MX)
      CreateQwordField(CRES, ^PM02._LEN, M2LN)
      //
      // Set 64bit MMIO resource Base and Length
      //
      Store (HMB2, M2MN) // HMB2, 64 bits MMIO base address
      Store (HMM2, M2MX) // HMM2, 64 bits MMIO limit
      Store (HML2, M2LN) // HML2, 64 bits MMIO length
//
    }
//<<EnableAbove4GBMmio++
    Return(CRES)
  } //End of PCI0._CRS

  Device(MEMR)
  {
    Name(_HID,EISAID("PNP0C02")) // Motherboard Resources
    Name(BAR3, 0xFFF00000)
    Name(MEM1,ResourceTemplate()
    {
      Memory32Fixed(ReadWrite,0x00000000,0x00000000)
      Memory32Fixed(ReadWrite,0x00000000,0x00000000)
      Memory32Fixed(ReadWrite,0x00000000,0x00000000)
    })
    Method(_CRS,0,NotSerialized)
    {
      CreateDwordField(MEM1,0x04,MB01)
      CreateDwordField(MEM1,0x08,ML01)
      CreateDwordField(MEM1,0x10,MB02)
      CreateDwordField(MEM1,0x14,ML02)
      CreateDwordField(MEM1,0x1C,MB03)
      CreateDwordField(MEM1,0x20,ML03)
      If(GPIC)
      {
        Store(0xFEC00000,MB01)
        Store(0xFEE00000,MB02)
        Store(0x1000,ML01)
        IF(\NAPC) {
          Add(ML01, 0x1000, ML01)
        }
        Store(0x1000,ML02)
      }
      If (LNotEqual (BAR3,0xFFF00000)) {
        Store (BAR3, MB03)
        Store (0x100000, ML03)
      }
      Return(MEM1)
    }
  }

  OperationRegion(NAPC, PCI_Config, 0xB8, 0x08)
  Field(NAPC,DWordAcc,NoLock,Preserve)
  {
    NAPX, 32,
    NAPD, 32
  }

  Mutex(NAPM, 0)
  Method(NAPE, 0) {
    Acquire(NAPM, 0xFFFF)
    Store(0x14300000, NAPX)
    Store(NAPD, Local0)
    And(Local0, 0xFFFFFFEF, Local0)
    Store(Local0, NAPD)
    Release(NAPM)
  }

  //
  // HOST Bridge PCI Routing Table (_PRT)
  //
  #include "_PRT.asl"

  //
  // PCIE P2P Bridge (Dev 1 Fn 1)
  //
  #include "GPP0.asl"

  //
  // PCIE P2P Bridge (Dev 1 Fn 2)
  //
  #include "GPP1.asl"

  //
  // PCIE P2P Bridge (Dev 1 Fn 3)
  //
  #include "GPP2.asl"

  //
  // PCIE P2P Bridge (Dev 1 Fn 4)
  //
  #include "GPP3.asl"

  //
  // PCIE P2P Bridge (Dev 1 Fn 5)
  //
  #include "GPP4.asl"

  //
  // PCIE P2P Bridge (Dev 1 Fn 6)
  //
  #include "GPP5.asl"

  //
  // PCIE P2P Bridge (Dev 1 Fn 7)
  //
  #include "GPP6.asl"

  //
  // PCIE PCIE GPP Bridge 0 to Bus A (Dev 8 Fn 1)
  //
  #include "GP17.asl"

  //
  // PCIE PCIE GPP Bridge 0 to Bus B (Dev 8 Fn 2)
  //
  #include "GP18.asl"

  //
  // High Precision Event Timer
  //
  #include "HPET.asl"

  //
  // SMBUS and ACPI Controller
  //
  #include "Smbs.asl"

  //
  // LPC CONTROLLER
  //
  #include "Lpc.asl"

  //Add Modern Standby support
#ifdef CONNECTED_STANDBY_SUPPORT
  #include "ModernStandby/GPP1Ms.asl"
  #include "ModernStandby/GPP2Ms.asl"
 // #include "ModernStandby/GPP3Ms.asl"
 // #include "ModernStandby/GPP6Ms.asl"
 // #include "ModernStandby/BlueTooth.asl"
  #include "ModernStandby/Xhci0Ms.asl"
  #include "ModernStandby/Xhci1Ms.asl"
#endif

} // End PCI Bus 0

#endif
