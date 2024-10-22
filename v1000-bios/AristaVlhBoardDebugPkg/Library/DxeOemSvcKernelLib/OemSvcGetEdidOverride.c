/** @file
  Provide OEM to customize EdidOverride. .

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/DxeOemSvcKernelLib.h>

#include <Protocol/DevicePath.h>
#include <Protocol/EdidOverride.h>

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           AgpBridge;
  PCI_DEVICE_PATH           AgpDevice;
  ACPI_ADR_DEVICE_PATH      DisplayDevice;
  EFI_DEVICE_PATH_PROTOCOL  End;
} PLATFORM_ONBOARD_VGA_DEVICE_PATH;

//[-start-240426-IB18160212-modify-]//
//
// Platform specific on chip internal graphics device path
//
PLATFORM_ONBOARD_VGA_DEVICE_PATH mOnChipIgfxDevicePath = {
  {
    {ACPI_DEVICE_PATH,
    ACPI_DP,
    {(UINT8)(sizeof(ACPI_HID_DEVICE_PATH)),
    (UINT8)((sizeof(ACPI_HID_DEVICE_PATH)) >> 8)}},
    EISA_PNP_ID(0x0A03),
    0
  },
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
    ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_EXTERNAL_DIGITAL, 0, 0)
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {(sizeof (EFI_DEVICE_PATH_PROTOCOL)),
    0}
  }
};
//[-end-240426-IB18160212-modify-]//

//                       800x600    0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f
#define OEM_EDID_TABLE_DATA1       0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x05,0xA4,0x01,0x00,0x01,0x00,0x00,0x00, \
                                   0x01,0x13,0x01,0x03,0x80,0x24,0x1D,0x78,0x2A,0x16,0x76,0xA2,0x5A,0x4B,0x97,0x24, \
                                   0x18,0x4F,0x54,0xBF,0xEF,0x00,0x61,0x40,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01, \
                                   0x01,0x01,0x01,0x01,0x01,0x01,0x64,0x19,0x00,0x40,0x41,0x00,0x26,0x30,0x18,0x88, \
                                   0x36,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0xFF,0x00,0x30,0x30,0x30, \
                                   0x30,0x30,0x30,0x31,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x00,0x00,0x00,0xFD,0x00,0x32, \
                                   0x4B,0x1E,0x53,0x0E,0x00,0x0A,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xFC, \
                                   0x00,0x41,0x4D,0x44,0x30,0x30,0x31,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x0A,0x00,0x04,

UINT8 mEdidTableData[] = { OEM_EDID_TABLE_DATA1 };

/**
  Get OEM customization EdidOverride. 
  Customization EdidOverride provides OEM to define EdidOverride.

  @param[out]  *GetEdidOverride            A pointer to OEM_GET_EDID_OVERRIDE_DEFINITION

  @retval      EFI_UNSUPPORTED             Returns unsupported by default.
  @retval      EFI_SUCCESS                 Get EdidOverride information successful.
  @retval      EFI_MEDIA_CHANGED           The value of IN OUT parameter is changed. 
  @retval      Others                      Base on OEM design.
**/
EFI_STATUS
OemSvcGetEdidOverride (
  IN OUT OEM_GET_EDID_OVERRIDE_DEFINITION *GetEdidOverride
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  GetEdidOverride->Attributes     = EFI_EDID_OVERRIDE_DONT_OVERRIDE;
  GetEdidOverride->EdidSize       = sizeof (mEdidTableData);
  GetEdidOverride->EdidData       = mEdidTableData;
  GetEdidOverride->DisplayDevPath = (EFI_DEVICE_PATH_PROTOCOL *)&mOnChipIgfxDevicePath;

  return EFI_SUCCESS;
}

