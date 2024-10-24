/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

DefinitionBlock (
  "LowPowerEthernet.aml", // Output file
  "SSDT",                 // Signature
  0x01,                   // SSDT Revision
  "AMD",                  // OEM ID
  "CPMLPETH",             // OEM Table ID
  0x1                     // OEM Revision
  )
{
  #define CPM_PCIE_LOW_POWER_ETHERNET_ASL

  #include "CmnDef.asi"
  #include "CmnExt.asi"
  #include "PostCode.asi"

  External (\_SB.PCI0.LPC0.EC0, DeviceObj)
  External (\_SB.PCI0.CpmLowPowerEthernetBridgeAslName.BCM5, DeviceObj)

  //
  // EC QEvent
  //
  Scope(\_SB.PCI0.LPC0.EC0) {
    Mutex(QMUX, 0)
    //
    // CpmLowPowerEthernetCablePlugOutMethod
    //
    Method(CpmLowPowerEthernetCablePlugOutMethod, 0x0, Serialized) {
      Acquire (QMUX, 0xFFFF)
      CpmSetPostCode (CpmTpLowPowerEthernetCablePlugOutBegin)
      Store (CpmReadTable (CpmLowPowerEthernetTable, CpmLowPowerEthernetLanLowPowerPin), Local2)
      If (LEqual (CpmReadGpio (Add (Local2, 0x200)), 0x0)) {
        If (CondRefOf (\_SB.PCI0.CpmLowPowerEthernetBridgeAslName.BCM5)) {
          Notify (\_SB.PCI0.CpmLowPowerEthernetBridgeAslName.BCM5, 0x03)
        }
      }
      CpmSetPostCode (CpmTpLowPowerEthernetCablePlugOutEnd)
      Release (QMUX)
    } // End of Method(CpmLowPowerEthernetCablePlugOutMethod, 0x0, Serialized)

    //
    // CpmLowPowerEthernetCablePlugInMethod
    //
    Method(CpmLowPowerEthernetCablePlugInMethod, 0x0, Serialized) {
      Acquire (QMUX, 0xFFFF)
      CpmSetPostCode (CpmTpLowPowerEthernetCablePlugInBegin)
      Store (CpmReadTable (CpmLowPowerEthernetTable, CpmLowPowerEthernetBridgeDevice), Local0)
      Store (CpmReadTable (CpmLowPowerEthernetTable, CpmLowPowerEthernetBridgeFunction), Local1)
      Store (CpmReadTable (CpmLowPowerEthernetTable, CpmLowPowerEthernetLanLowPowerPin), Local2)
      Store (CpmReadTable (CpmLowPowerEthernetTable, CpmLowPowerEthernetDeviceId), Local3)
      If (LEqual (CpmReadGpio (Add (Local2, 0x200)), 0x1)) {
        CpmWriteGpio (Add (Local2, 0x200), 0x0)
        Sleep (300)
        CpmSetDeviceClock (Local3, 1)           // Clock On
        Sleep (100)
        CpmSetDeviceReset (Local3, 0)           // Assert Reset
        Sleep (200)
        CpmSetDeviceReset (Local3, 1)           // De-assert Reset
        CpmPcieHotplug (Local0, Local1, 1, 1)
        Sleep (200)
        CpmSetDeviceClock (Local3, 2)           // Clock On
        If (CondRefOf (\_SB.PCI0.CpmLowPowerEthernetBridgeAslName.BCM5)) {
          Notify (\_SB.PCI0.CpmLowPowerEthernetBridgeAslName.BCM5, 0x00)
        }
      }
      CpmSetPostCode (CpmTpLowPowerEthernetCablePlugInEnd)
      Release (QMUX)
    } // End of Method(CpmLowPowerEthernetCablePlugInMethod, 0x0, Serialized)

  } // End of Scope(\_SB.PCI0.LPC0.EC0)

  Scope (\_SB.PCI0.CpmLowPowerEthernetBridgeAslName.BCM5) {
    Method (_EJ0, 1) {
      Acquire (\_SB.PCI0.LPC0.EC0.QMUX, 0xFFFF)
      CpmSetPostCode (CpmTpLowPowerEthernetEJ0Begin)
      Store (CpmReadTable (CpmLowPowerEthernetTable, CpmLowPowerEthernetBridgeDevice), Local0)
      Store (CpmReadTable (CpmLowPowerEthernetTable, CpmLowPowerEthernetBridgeFunction), Local1)
      Store (CpmReadTable (CpmLowPowerEthernetTable, CpmLowPowerEthernetLanLowPowerPin), Local2)
      Store (CpmReadTable (CpmLowPowerEthernetTable, CpmLowPowerEthernetDeviceId), Local3)
      CpmPcieHotplug (Local0, Local1, 0, 1)
      Sleep (100)
      Sleep (200)
      CpmWriteGpio (Add (Local2, 0x200), 0x1)
      Sleep (1000)
      CpmSetDeviceClock (Local3, 0)             // Clock Off
      CpmSetPostCode (CpmTpLowPowerEthernetEJ0End)
      Release (\_SB.PCI0.LPC0.EC0.QMUX)
    } // End of Method (_EJ0)
  } // End of Scope(\_SB.PCI0.CpmLowPowerEthernetBridgeAslName.BCM5)

  Scope (\_SB) {
    Method (CpmLowPowerEthernet_WAK, 1, Serialized) {
      CpmSetPostCode (CpmTpLowPowerEthernetWAKStart)
      Notify (\_SB.PCI0.CpmLowPowerEthernetBridgeAslName.BCM5, 0x00)
      CpmSetPostCode (CpmTpLowPowerEthernetWAKEnd)
    } // End of Method (CpmLowPowerEthernet_WAK)
  } // End of Scope(\_SB)
}

