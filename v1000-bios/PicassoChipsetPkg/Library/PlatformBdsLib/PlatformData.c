/** @file
  PlatformBdsLib

;******************************************************************************
;* Copyright (c) 2015, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/** 
  Defined the platform specific device path which will be used by
  platform Bbd to perform the platform policy connect.

Copyright (c) 2004 - 2008, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "BdsPlatform.h"


//
// Predefined platform root bridge
//
PLATFORM_ROOT_BRIDGE_DEVICE_PATH gPlatformRootBridge0 = {
  gPciRootBridge,
  gEndEntire
};

EFI_DEVICE_PATH_PROTOCOL* gPlatformRootBridges [] = {
  (EFI_DEVICE_PATH_PROTOCOL*)&gPlatformRootBridge0,
  NULL
};

//[-start-240426-IB18160212-modify-]//
//
// Platform specific keyboard device path
//
PLATFORM_KEYBOARD_DEVICE_PATH gKeyboardDevicePath = {
  gPciRootBridge,
  {
    {HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    {(UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8)}},
    0x03,
    0x14
  },
  {
    {ACPI_DEVICE_PATH,
    ACPI_DP,
    {(UINT8)(sizeof(ACPI_HID_DEVICE_PATH)),
    (UINT8)((sizeof(ACPI_HID_DEVICE_PATH)) >> 8)}},
    EISA_PNP_ID(0x0303),
    0
  },
  gEndEntire
};

//
// Platform specific PS2 Mouse device path
//
PLATFORM_MOUSE_DEVICE_PATH gMouseDevicePath = {
  gPciRootBridge,
  {
    {HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    {(UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8)}},
    0x03,
    0x14
  },
  {
    {ACPI_DEVICE_PATH,
    ACPI_DP,
    {(UINT8)(sizeof(ACPI_HID_DEVICE_PATH)),
    (UINT8)((sizeof(ACPI_HID_DEVICE_PATH)) >> 8)}},
    EISA_PNP_ID(0x0F03),
    0x00
  },
  gEndEntire
};

PLATFORM_USB_HID_DEVICE_PATH gUsbHidDevicePath = {
  {
    {MESSAGING_DEVICE_PATH,
    MSG_USB_CLASS_DP,
    {(UINT8)(sizeof (USB_CLASS_DEVICE_PATH)),
    (UINT8)((sizeof (USB_CLASS_DEVICE_PATH)) >> 8)}},
    0xffff,
    0xffff,
    0x03,
    0xff,
    0xff
  },
  gEndEntire
};

//
// Platform specific on chip internal graphics device path
//
PLATFORM_ONBOARD_VGA_DEVICE_PATH gOnChipIgfxDevicePath = {
  gPciRootBridge,
  {
    {HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    {(UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8)}},
    0x1,
    0x8   
  },
  {
    {HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    {(UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8)}},
    0,
    0
  },
  {
    {ACPI_DEVICE_PATH,
    ACPI_ADR_DP,
    {(UINT8) (sizeof (ACPI_ADR_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_ADR_DEVICE_PATH)) >> 8)}},
    ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_INTERNAL_DIGITAL, 0, 0)
  },
  gEndEntire
};

//
// Platform specific plug in PEG device path
//
PLATFORM_PLUG_IN_VGA_DEVICE_PATH gPlugInPegDevicePath0 = {
  gPciRootBridge,
  {
    {HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    {(UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8)}},
    0,
    0x1   
  },
  {
    {HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    {(UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8)}},
    0,
    0x5
  },
  {
    {ACPI_DEVICE_PATH,
    ACPI_ADR_DP,
    {(UINT8) (sizeof (ACPI_ADR_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_ADR_DEVICE_PATH)) >> 8)}},
    ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_EXTERNAL_DIGITAL, 0, 0)
  },
  gEndEntire
};

PLATFORM_PLUG_IN_VGA_DEVICE_PATH gPlugInPegDevicePath1 = {
  gPciRootBridge,
  {
    {HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    {(UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8)}},
    0x05,
    0x02   
  },
  {
    {HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    {(UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8)}},
    0,
    0
  },
  {
    {ACPI_DEVICE_PATH,
    ACPI_ADR_DP,
    {(UINT8) (sizeof (ACPI_ADR_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_ADR_DEVICE_PATH)) >> 8)}},
    ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_EXTERNAL_DIGITAL, 0, 0)
  },
  gEndEntire
};

//
// Platform specific plug in PCI VGA device path
//
PLATFORM_PLUG_IN_VGA_DEVICE_PATH gPlugInPciDevicePath0 = {
  gPciRootBridge,
  {
    {HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    {(UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8)}},
    0x03,
    0x01
  },
  {
    {HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    {(UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8)}},
    0,
    0
  },
  {
    {ACPI_DEVICE_PATH,
    ACPI_ADR_DP,
    {(UINT8) (sizeof (ACPI_ADR_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_ADR_DEVICE_PATH)) >> 8)}},
    ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_EXTERNAL_DIGITAL, 0, 0)
  },
  gEndEntire
};

PLATFORM_PLUG_IN_VGA_DEVICE_PATH gPlugInPciDevicePath1 = {
  gPciRootBridge,
  {
    {HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    {(UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8)}},
    0x04,
    0x01
  },
  {
    {HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    {(UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8)}},
    0,
    0
  },
  {
    {ACPI_DEVICE_PATH,
    ACPI_ADR_DP,
    {(UINT8) (sizeof (ACPI_ADR_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_ADR_DEVICE_PATH)) >> 8)}},
    ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_EXTERNAL_DIGITAL, 0, 0)
  },
  gEndEntire
};

PLATFORM_PLUG_IN_VGA_DEVICE_PATH gPlugInPciDevicePath2 = {
  gPciRootBridge,
  {
    {HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    {(UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8)}},
    0x05,
    0x01
  },
  {
    {HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    {(UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8)}},
    0,
    0
  },
  {
    {ACPI_DEVICE_PATH,
    ACPI_ADR_DP,
    {(UINT8) (sizeof (ACPI_ADR_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_ADR_DEVICE_PATH)) >> 8)}},
    ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_EXTERNAL_DIGITAL, 0, 0)
  },
  gEndEntire
};

PLATFORM_PLUG_IN_VGA_DEVICE_PATH gPlugInPciDevicePath3 = {
  gPciRootBridge,
  {
    {HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    {(UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8)}},
    0x06,
    0x01
  },
  {
    {HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    {(UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8)}},
    0,
    0
  },
  {
    {ACPI_DEVICE_PATH,
    ACPI_ADR_DP,
    {(UINT8) (sizeof (ACPI_ADR_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_ADR_DEVICE_PATH)) >> 8)}},
    ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_EXTERNAL_DIGITAL, 0, 0)
  },
  gEndEntire
};

//
// Platform specific SATA device path
//
PLATFORM_SATA_DEVICE_PATH gSataPort0DevicePath = {
  gPciRootBridge,
  {
    {HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    {(UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8)}},
    0x02,
    0x08
  },
  {
    {HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    {(UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8)}},
    0x00,
    0x00
  },
  {
    {MESSAGING_DEVICE_PATH,
    MSG_SATA_DP,
    {(UINT8) (sizeof (SATA_DEVICE_PATH)),
    (UINT8) ((sizeof (SATA_DEVICE_PATH)) >> 8)}},
    0,
    0,
    0
  },
  gEndEntire
};

//
// Platform specific serial device path
//
PLATFORM_ISA_SERIAL_DEVICE_PATH gSerialDevicePath = {
  gPciRootBridge,
  {
    {HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    {(UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8)}},
    0x03,
    0x14
  },
  {
    {ACPI_DEVICE_PATH,
    ACPI_DP,
    {(UINT8)(sizeof(ACPI_HID_DEVICE_PATH)),
    (UINT8)((sizeof(ACPI_HID_DEVICE_PATH)) >> 8)}},
    EISA_PNP_ID(0x0501),
    0
  },
  {
    {MESSAGING_DEVICE_PATH,
    MSG_UART_DP,
    {(UINT8)(sizeof(UART_DEVICE_PATH)),
    (UINT8)((sizeof(UART_DEVICE_PATH)) >> 8)}},
    0,
    115200,
    8,
    1,
    1
  },
  {
    {MESSAGING_DEVICE_PATH,
    MSG_VENDOR_DP,
    {(UINT8)(sizeof(VENDOR_DEVICE_PATH)),
    (UINT8)((sizeof(VENDOR_DEVICE_PATH)) >> 8)}},
    DEVICE_PATH_MESSAGING_PC_ANSI
  },
  gEndEntire
};

//
// Platform specific serial over lan device path
//
PLATFORM_ISA_SERIAL_OVER_LAN_DEVICE_PATH gSerialOverLANDevicePath = {
  gPciRootBridge,
  {
    {HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    {(UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8)}},
    0x2,
    0x2
  },
  {
    {HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    {(UINT8)(sizeof(PCI_DEVICE_PATH)),
    (UINT8)((sizeof(PCI_DEVICE_PATH)) >> 8)}},
    0x1,
    0x0
  },
  {
    {MESSAGING_DEVICE_PATH,
    MSG_UART_DP,
    {(UINT8)(sizeof(UART_DEVICE_PATH)),
    (UINT8)((sizeof(UART_DEVICE_PATH)) >> 8)}},
    0,
    9600,
    8,
    1,
    1
  },
  {
    {MESSAGING_DEVICE_PATH,
    MSG_VENDOR_DP,
    {(UINT8)(sizeof(VENDOR_DEVICE_PATH)),
    (UINT8)((sizeof(VENDOR_DEVICE_PATH)) >> 8)}},
    DEVICE_PATH_MESSAGING_PC_ANSI
  },
  gEndEntire
};

//
// Predefined platform default console device path
//
BDS_CONSOLE_CONNECT_ENTRY   gPlatformConsole[] = {
  {(EFI_DEVICE_PATH_PROTOCOL *) &gKeyboardDevicePath  , CONSOLE_IN},
  {(EFI_DEVICE_PATH_PROTOCOL *) &gMouseDevicePath     , CONSOLE_IN},
  {(EFI_DEVICE_PATH_PROTOCOL *) &gUsbHidDevicePath    , CONSOLE_IN},
  {(EFI_DEVICE_PATH_PROTOCOL *) &gOnChipIgfxDevicePath, CONSOLE_OUT},
  {(EFI_DEVICE_PATH_PROTOCOL *) &gPlugInPegDevicePath0, CONSOLE_OUT},
  {NULL, 0}
};

//
// All the possible platform on chip internal graphics device path
//
EFI_DEVICE_PATH_PROTOCOL* gPlatformAllPossibleIgfxConsole [] = {
  (EFI_DEVICE_PATH_PROTOCOL*) &gOnChipIgfxDevicePath, // IGD (D1/F0)
  NULL
};

//
// All the possible platform PEG device path
//
EFI_DEVICE_PATH_PROTOCOL* gPlatformAllPossiblePegConsole [] = {
  (EFI_DEVICE_PATH_PROTOCOL*) &gPlugInPegDevicePath0, // PEG bridge (0x00/0x01/0x01) VGA device (D1/F1), Highest priority
  (EFI_DEVICE_PATH_PROTOCOL*) &gPlugInPegDevicePath1, // PEG bridge (0x00/0x01/0x02) VGA device (D1/F2), Lowest priority
  NULL
};

//
// All the possible platform PCI graphics device path
//
EFI_DEVICE_PATH_PROTOCOL* gPlatformAllPossiblePciConsole [] = {
  (EFI_DEVICE_PATH_PROTOCOL*) &gPlugInPciDevicePath0, // SB PCI bridge (0x00/0x01/0x03) VGA device (D1/F3), Highest priority
  (EFI_DEVICE_PATH_PROTOCOL*) &gPlugInPciDevicePath1, // SB PCI bridge (0x00/0x01/0x04) VGA device (D1/F4)
  (EFI_DEVICE_PATH_PROTOCOL*) &gPlugInPciDevicePath2, // SB PCI bridge (0x00/0x01/0x05) VGA device (D1/F5)
  (EFI_DEVICE_PATH_PROTOCOL*) &gPlugInPciDevicePath3, // SB PCI bridge (0x00/0x01/0x06) VGA device (D1/F6), Lowest priority
  NULL
};

//
// Legacy hard disk boot option
//
LEGACY_HD_DEVICE_PATH gLegacyHd = {
  {
    {BBS_DEVICE_PATH,
    BBS_BBS_DP,
    {(UINT8)(sizeof(BBS_BBS_DEVICE_PATH)),
    (UINT8)((sizeof(BBS_BBS_DEVICE_PATH)) >> 8)}},
    BBS_TYPE_HARDDRIVE,
    0,
    {0}
  },
  gEndEntire
};

//
// Legacy cdrom boot option
//
LEGACY_HD_DEVICE_PATH gLegacyCdrom = {
  {
    {BBS_DEVICE_PATH,
    BBS_BBS_DP,
    {(UINT8)(sizeof(BBS_BBS_DEVICE_PATH)),
    (UINT8)((sizeof(BBS_BBS_DEVICE_PATH)) >> 8)}},
    BBS_TYPE_CDROM,
    0,
    {0}
  },
  gEndEntire
};
//[-end-240426-IB18160212-modify-]//

//
// Predefined platform specific perdict boot option
//
EFI_DEVICE_PATH_PROTOCOL* gPlatformBootOption [] = {
  (EFI_DEVICE_PATH_PROTOCOL*) &gLegacyHd,
  (EFI_DEVICE_PATH_PROTOCOL*) &gLegacyCdrom,
  NULL
};

//
// Predefined platform specific driver option
//
EFI_DEVICE_PATH_PROTOCOL* gPlatformDriverOption [] = {
  NULL
};

//
// Predefined platform connect sequence
//
EFI_DEVICE_PATH_PROTOCOL* gPlatformConnectSequence [] = {
  NULL
};

//
// Predefined platform connect SATA device
//
EFI_DEVICE_PATH_PROTOCOL* gPlatformConnectSata [] = {
  (EFI_DEVICE_PATH_PROTOCOL *) &gSataPort0DevicePath,
  NULL
};
