/* $NoKeywords:$ */
/**
 * @file
 *
 * Platform PCIe Complex Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdNbioSmuV9Pei
 * @e \$Revision: 312065 $   @e \$Date: 2015-01-30 04:23:05 -0600 (Fri, 30 Jan 2015) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include <PiPei.h>
#include <AmdPcieComplex.h>
#include <AmdServerHotplug.h>
#include <Protocol/NbioHotplugDesc.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>


/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

HOTPLUG_DESCRIPTOR    HotplugDescriptor[] = {
  {
      0,
      HOTPLUG_ENGINE_DATA_INITIALIZER (0,      // Start Lane
                                       3,      // End Lane
                                       0,       // Socket Number
                                       4)       // Slot number
      PCIE_HOTPLUG_INITIALIZER_MAPPING (HotplugPresenceDetect,   //Hotplug Type
                                        0,      // 0 = No Gpio Descriptor
                                        0,      // 0 = No Reset Descriptor
                                        1,      // 1 = Port Active - this is a valid descriptor
                                        0,      // 1 = Master/Slave APU
                                        0,      // 0 = Die number this slot is connected to
                                        0,      // Alternate Slot number
                                        0)      // Primary/secondary for SSD only
      PCIE_HOTPLUG_INITIALIZER_FUNCTION (1,     // Gpio Bit Select
                                         0,     // GPIO Byte Mapping
                                         0,     // GPIO Device Mapping Ext
                                         7,  // GPIO Device Mapping
                                         1,     // Device Type 1 = 9535
                                         0,     // Bus Segment (No 9545 Present)
                                         0)  // Function Mask
      PCIE_HOTPLUG_INITIALIZER_NO_RESET()
      PCIE_HOTPLUG_INITIALIZER_NO_GPIO()
  },
  {
      0,
      HOTPLUG_ENGINE_DATA_INITIALIZER (4,      // Start Lane
                                       7,      // End Lane
                                       0,       // Socket Number
                                       5)       // Slot number
      PCIE_HOTPLUG_INITIALIZER_MAPPING (HotplugPresenceDetect,   //Hotplug Type
                                        0,      // 0 = No Gpio Descriptor
                                        0,      // 0 = No Reset Descriptor
                                        1,      // 1 = Port Active - this is a valid descriptor
                                        0,      // 1 = Master/Slave APU
                                        0,      // 0 = Die number this slot is connected to
                                        0,      // Alternate Slot number
                                        0)      // Primary/secondary for SSD only
      PCIE_HOTPLUG_INITIALIZER_FUNCTION (0,     // Gpio Bit Select
                                         1,     // GPIO Byte Mapping
                                         0,     // GPIO Device Mapping Ext
                                         7,  // GPIO Device Mapping
                                         1,     // Device Type 1 = 9535
                                         0,     // Bus Segment (No 9545 Present)
                                         0)  // Function Mask
      PCIE_HOTPLUG_INITIALIZER_NO_RESET()
      PCIE_HOTPLUG_INITIALIZER_NO_GPIO()
  },
  {
      0,
      HOTPLUG_ENGINE_DATA_INITIALIZER (8,      // Start Lane
                                       11,      // End Lane
                                       0,       // Socket Number
                                       6)       // Slot number
      PCIE_HOTPLUG_INITIALIZER_MAPPING (HotplugPresenceDetect,   //Hotplug Type
                                        0,      // 0 = No Gpio Descriptor
                                        0,      // 0 = No Reset Descriptor
                                        1,      // 1 = Port Active - this is a valid descriptor
                                        0,      // 1 = Master/Slave APU
                                        0,      // 0 = Die number this slot is connected to
                                        0,      // Alternate Slot number
                                        0)      // Primary/secondary for SSD only
      PCIE_HOTPLUG_INITIALIZER_FUNCTION (1,     // Gpio Bit Select
                                         0,     // GPIO Byte Mapping
                                         0,     // GPIO Device Mapping Ext
                                         7,  // GPIO Device Mapping
                                         1,     // Device Type 1 = 9535
                                         2,     // Bus Segment (No 9545 Present)
                                         0)  // Function Mask
      PCIE_HOTPLUG_INITIALIZER_NO_RESET()
      PCIE_HOTPLUG_INITIALIZER_NO_GPIO()
  },
  {
      0,
      HOTPLUG_ENGINE_DATA_INITIALIZER (12,      // Start Lane
                                       15,      // End Lane
                                       0,       // Socket Number
                                       7)       // Slot number
      PCIE_HOTPLUG_INITIALIZER_MAPPING (HotplugPresenceDetect,   //Hotplug Type
                                        0,      // 0 = No Gpio Descriptor
                                        0,      // 0 = No Reset Descriptor
                                        1,      // 1 = Port Active - this is a valid descriptor
                                        0,      // 1 = Master/Slave APU
                                        0,      // 0 = Die number this slot is connected to
                                        0,      // Alternate Slot number
                                        0)      // Primary/secondary for SSD only
      PCIE_HOTPLUG_INITIALIZER_FUNCTION (0,     // Gpio Bit Select
                                         1,     // GPIO Byte Mapping
                                         0,     // GPIO Device Mapping Ext
                                         7,  // GPIO Device Mapping
                                         1,     // Device Type 1 = 9535
                                         2,     // Bus Segment (No 9545 Present)
                                         0)  // Function Mask
      PCIE_HOTPLUG_INITIALIZER_NO_RESET()
      PCIE_HOTPLUG_INITIALIZER_NO_GPIO()
  },
  {
    0,
    HOTPLUG_ENGINE_DATA_INITIALIZER (80, 95, 0, 3)
    PCIE_HOTPLUG_INITIALIZER_MAPPING (HotplugExpressModule, 0, 0, 1, 1, 2, 0, 0)
    PCIE_HOTPLUG_INITIALIZER_FUNCTION (0, 0, 0, 3, 1, 7, 0)
    PCIE_HOTPLUG_INITIALIZER_NO_RESET ()
    PCIE_HOTPLUG_INITIALIZER_NO_GPIO ()
  },
  {
      0,
      HOTPLUG_ENGINE_DATA_INITIALIZER (0,       // Start Lane
                                       3,       // End Lane
                                       1,       // Socket Number
                                       8)       // Slot number
      PCIE_HOTPLUG_INITIALIZER_MAPPING (HotplugEnterpriseSsd,   //Hotplug Type
                                        0,      // 0 = No Gpio Descriptor
                                        0,      // 0 = No Reset Descriptor
                                        1,      // 1 = Port Active - this is a valid descriptor
                                        0,      // 1 = Master/Slave APU
                                        0,      // 0 = Die number this slot is connected to
                                        0,      // Alternate Slot number
                                        0)      // Primary/secondary for SSD only
      PCIE_HOTPLUG_INITIALIZER_FUNCTION (1,     // Gpio Bit Select
                                         0,     // GPIO Byte Mapping
                                         2,     // GPIO Device Mapping Ext
                                         1,     // GPIO Device Mapping
                                         1,     // Device Type 1 = 9535
                                         1,     // Bus Segment (No 9545 Present)
                                         0)     // Function Mask
      PCIE_HOTPLUG_INITIALIZER_NO_RESET()
      PCIE_HOTPLUG_INITIALIZER_NO_GPIO()
  },
  {
      0,
      HOTPLUG_ENGINE_DATA_INITIALIZER (4,       // Start Lane
                                       7,       // End Lane
                                       1,       // Socket Number
                                       9)       // Slot number
      PCIE_HOTPLUG_INITIALIZER_MAPPING (HotplugEnterpriseSsd,   //Hotplug Type
                                        0,      // 0 = No Gpio Descriptor
                                        0,      // 0 = No Reset Descriptor
                                        1,      // 1 = Port Active - this is a valid descriptor
                                        0,      // 1 = Master/Slave APU
                                        0,      // 0 = Die number this slot is connected to
                                        0,      // Alternate Slot number
                                        0)      // Primary/secondary for SSD only
      PCIE_HOTPLUG_INITIALIZER_FUNCTION (1,     // Gpio Bit Select
                                         1,     // GPIO Byte Mapping
                                         2,
                                         1,     // GPIO Device Mapping
                                         1,     // Device Type 1 = 9535
                                         1,     // Bus Segment (No 9545 Present)
                                         0)     // Function Mask
      PCIE_HOTPLUG_INITIALIZER_NO_RESET()
      PCIE_HOTPLUG_INITIALIZER_NO_GPIO()
  },
  {
      0,
      HOTPLUG_ENGINE_DATA_INITIALIZER (8,       // Start Lane
                                       11,      // End Lane
                                       1,       // Socket Number
                                       10)      // Slot number
      PCIE_HOTPLUG_INITIALIZER_MAPPING (HotplugEnterpriseSsd,   //Hotplug Type
                                        0,      // 0 = No Gpio Descriptor
                                        0,      // 0 = No Reset Descriptor
                                        1,      // 1 = Port Active - this is a valid descriptor
                                        0,      // 1 = Master/Slave APU
                                        0,      // 0 = Die number this slot is connected to
                                        0,      // Alternate Slot number
                                        0)      // Primary/secondary for SSD only
      PCIE_HOTPLUG_INITIALIZER_FUNCTION (1,     // Gpio Bit Select
                                         0,     // GPIO Byte Mapping
                                         2,     // GPIO Device Mapping Ext
                                         2,     // GPIO Device Mapping
                                         1,     // Device Type 1 = 9535
                                         1,     // Bus Segment (No 9545 Present)
                                         0)     // Function Mask
      PCIE_HOTPLUG_INITIALIZER_NO_RESET()
      PCIE_HOTPLUG_INITIALIZER_NO_GPIO()
  },
  {
      DESCRIPTOR_TERMINATE_LIST,
      HOTPLUG_ENGINE_DATA_INITIALIZER (12,      // Start Lane
                                       15,      // End Lane
                                       1,       // Socket Number
                                       11)      // Slot number
      PCIE_HOTPLUG_INITIALIZER_MAPPING (HotplugEnterpriseSsd,   //Hotplug Type
                                        0,      // 0 = No Gpio Descriptor
                                        0,      // 0 = No Reset Descriptor
                                        1,      // 1 = Port Active - this is a valid descriptor
                                        0,      // 1 = Master/Slave APU
                                        0,      // 0 = Die number this slot is connected to
                                        0,      // Alternate Slot number
                                        0)      // Primary/secondary for SSD only
      PCIE_HOTPLUG_INITIALIZER_FUNCTION (1,     // Gpio Bit Select
                                         1,     // GPIO Byte Mapping
                                         2,     // GPIO Device Mapping Ext
                                         2,     // GPIO Device Mapping
                                         1,     // Device Type 1 = 9535
                                         1,     // Bus Segment (No 9545 Present)
                                         0)     // Function Mask
      PCIE_HOTPLUG_INITIALIZER_NO_RESET()
      PCIE_HOTPLUG_INITIALIZER_NO_GPIO()
  },
};

STATIC NBIO_HOTPLUG_DESC_PROTOCOL mHotplugDescriptorProtocol = {
  AMD_NBIO_HOTPLUG_DESC_VERSION,  ///< revision
  HotplugDescriptor
};

EFI_STATUS
EFIAPI
HotplugDescEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_HANDLE                       Handle;
  EFI_STATUS                       Status;

  Handle = NULL;
  // Fixup HOTPLUG_DESCRIPTOR here
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gAmdHotplugDescProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mHotplugDescriptorProtocol
                  );

  return Status;
}

