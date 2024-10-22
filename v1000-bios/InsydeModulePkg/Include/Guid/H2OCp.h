/** @file
  Definitions for H2O check point

;******************************************************************************
;* Copyright (c) 2023, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef __H2O_CHECK_POINT_H__
#define __H2O_CHECK_POINT_H__

#include <Uefi.h>
#include <Pi/PiBootMode.h>
#include <Pi/PiFirmwareVolume.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/Pci.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/HiiImage.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/SimplePointer.h>
#include <Protocol/AbsolutePointer.h>
#include <Protocol/OEMBadgingSupport.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Protocol/PciIo.h>
#include <Protocol/BusSpecificDriverOverride.h>
#include <Protocol/DevicePath.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/LoadFile2.h>
#include <Protocol/PciHotPlugInit.h>
#include <Protocol/Smbios.h>
#include <Protocol/H2OBdsServices.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/FormBrowser2.h>
#include <Guid/AuthenticatedVariableFormat.h>
#include <Library/FdSupportLib.h>
#include <IndustryStandard/Acpi62.h>

//
// Task behavior for H2O_CP_XXXX_DATA structure Status member
//
#define H2O_CP_TASK_NORMAL           0x00
#define H2O_CP_TASK_SKIP             0x01
#define H2O_CP_TASK_UPDATE           0x02
#define H2O_CP_TASK_BOOT_FAIL        0x03
#define H2O_CP_TASK_SKIP_UPDATE      0x04

#define H2O_CP_TASK_BREAK              0x80
#define H2O_CP_TASK_BREAK_NORMAL       (H2O_CP_TASK_BREAK | H2O_CP_TASK_NORMAL)
#define H2O_CP_TASK_BREAK_SKIP         (H2O_CP_TASK_BREAK | H2O_CP_TASK_SKIP)
#define H2O_CP_TASK_BREAK_UPDATE       (H2O_CP_TASK_BREAK | H2O_CP_TASK_UPDATE)
#define H2O_CP_TASK_BREAK_BOOT_FAIL    (H2O_CP_TASK_BREAK | H2O_CP_TASK_BOOT_FAIL)
#define H2O_CP_TASK_BREAK_SKIP_UPDATE  (H2O_CP_TASK_BREAK | H2O_CP_TASK_SKIP_UPDATE)
//
// Task behavior for H2O_BDS_CP_XXXX_DATA structure Status member
//
#define H2O_BDS_TASK_NORMAL         H2O_CP_TASK_NORMAL
#define H2O_BDS_TASK_SKIP           H2O_CP_TASK_SKIP
#define H2O_BDS_TASK_UPDATE         H2O_CP_TASK_UPDATE
#define H2O_BDS_TASK_BOOT_FAIL      H2O_CP_TASK_BOOT_FAIL

//
// Note: Please follow below policy to add new checkpoint.
// 1. Check points should be grouped 1st by phase (PEI, DXE, SMM or BDS)
//    If check points can be triggered in all phases, the phase name should be BASE. For example: post code checkpoint.
// 2. Where possible, check points should be grouped by common topic, such as the driver where they are
//    located or the technology they implement. For example: BdsDxe checkpoints together, or Setup
// 3. Checkpoints together. Where possible, check points should be in execution order. For example: "before"
//    checkpoints should appear before "after", "init" checkpoints before "shutdown" checkpoints.
//

//
// BASE CP GUID definitions
//
#define H2O_BASE_CP_POST_CODE_GUID \
  { \
    0x3cbdd55b, 0x9ad7, 0x4f86, 0x89, 0x57, 0x80, 0xe4, 0x81, 0x91, 0xfc, 0x28 \
  }

#define H2O_BASE_CP_VERIFY_FV_GUID \
  { \
    0xc975364e, 0xddd8, 0x4ef4, 0x94, 0x2c, 0x8a, 0xbd, 0xf5, 0xcb, 0x75, 0x74 \
  }

//
// PEI CP GUID definitions
//
#define H2O_PEI_CP_FORCE_VARIABLE_DEFAULTS_GUID \
  { \
    0x97796b25, 0x43bb, 0x4281, 0x9f, 0x05, 0x11, 0x0b, 0x86, 0xe3, 0x49, 0xa4 \
  }

#define H2O_PEI_CP_INIT_PLATFORM_STAGE_1_GUID \
  { \
    0x3b9c6cd9, 0xde0c, 0x4401, 0xa3, 0xc5, 0x12, 0x4b, 0x61, 0xd1, 0x9d, 0x3d \
  }

#define H2O_PEI_CP_SET_BOOT_MODE_BEFORE_GUID \
  { \
    0xedc49749, 0x1d69, 0x4d6a, 0xa6, 0xab, 0x27, 0x3f, 0x06, 0xf9, 0x74, 0x58 \
  }

#define H2O_PEI_CP_INIT_PLATFORM_STAGE_2_GUID \
  { \
    0x20b242dd, 0x0d63, 0x4949, 0xad, 0x0a, 0x1c, 0xfa, 0xf9, 0x3d, 0x12, 0x44 \
  }

#define H2O_PEI_CP_CRISIS_RECOVERY_PUBLISH_FV_GUID \
  { \
    0xdb38b706, 0x7ea2, 0x41f1, 0x90, 0x8a, 0xba, 0xbe, 0x73, 0xf7, 0x01, 0x45 \
  }

#define H2O_PEI_CP_CRISIS_RECOVERY_GET_PROTECT_TABLE_GUID \
  { \
    0xfbb92f55, 0x9dba, 0x4144, 0xaf, 0x42, 0xfc, 0x69, 0xe9, 0x24, 0x82, 0xa9 \
  }

#define H2O_PEI_CP_CRISIS_RECOVERY_FLASH_GUID \
  { \
    0x0e83fba0, 0xade2, 0x42cc, 0x90, 0xbe, 0x90, 0xe4, 0xee, 0x44, 0xba, 0x18 \
  }

#define H2O_PEI_CP_END_OF_PEI_BEFORE_GUID \
  { \
    0xc3caccf2, 0x6a32, 0x48b3, 0xb7, 0x4f, 0xae, 0x33, 0x56, 0xef, 0xe8, 0x04 \
  }

#define H2O_PEI_CP_END_OF_PEI_AFTER_GUID \
  { \
    0x53ef893c, 0x890f, 0x44a2, 0x8b, 0x37, 0x62, 0x1e, 0x1e, 0xd4, 0xaf, 0xde \
  }

//
// DXE CP GUID definitions
//
#define H2O_DXE_CP_WRITE_VARIABLE_DEFAULTS_GUID \
  { \
    0x549b7cc1, 0x2059, 0x4ba6, 0x90, 0x4a, 0xac, 0x26, 0xb9, 0x5f, 0xa6, 0x94 \
  }

#define H2O_DXE_CP_PCI_CHECK_SYSTEM_OPTION_ROM_GUID \
  { \
    0xaf9a2514, 0x474b, 0x40e8, 0xa8, 0xb5, 0x50, 0x6c, 0x0e, 0x5d, 0x48, 0x33 \
  }

#define H2O_DXE_CP_UPDATE_SMBIOS_RECORD_GUID \
  { \
    0x1b39dac9, 0xd4cd, 0x450a, 0xb7, 0xbb, 0x96, 0x86, 0x18, 0x51, 0x8e, 0x6d \
  }

#define H2O_DXE_CP_RESET_SYSTEM_GUID \
  { \
    0x4ec6f877, 0x13fc, 0x4adc, 0xaf, 0xe4, 0x14, 0x28, 0xcb, 0xc5, 0xf1, 0x77 \
  }

#define H2O_DXE_CP_PCI_GET_BRIDGE_INFO_GUID \
  { \
    0xdedf7529, 0xaa91, 0x4eb7, 0xb5, 0x7c, 0xcb, 0x4b, 0x01, 0x08, 0xbc, 0x67 \
  }

#define H2O_DXE_CP_PCI_HPC_GET_RESOURCE_PADDING_GUID \
  { \
    0x45df04f4, 0x596c, 0x4149, 0x8e, 0xd1, 0xba, 0xb1, 0x65, 0xf0, 0x23, 0xd0 \
  }

#define H2O_DXE_CP_UPDATE_ACPI_ESRT_TPM_FW_VERSION_GUID \
  { \
    0x5400031b, 0xe93d, 0x45bf, 0x90, 0x11, 0xed, 0x20, 0x09, 0x5f, 0x96, 0x6c \
  }

//
// SMM CP GUID definitions
//
#define H2O_SMM_CP_IHISI_AUTH_STATUS_GUID \
  { \
    0x725cff90, 0x6bb0, 0x4566, 0xbb, 0x12, 0xd9, 0x3d, 0x18, 0x62, 0xcf, 0x66 \
  }

#define H2O_SMM_CP_IHISI_AUTH_CHECK_GUID \
  { \
    0xd2a08ed5, 0xe171, 0x4492, 0x85, 0x7a, 0x2d, 0xf5, 0x8c, 0x96, 0x46, 0x86 \
  }

//
// BDS CP GUID definitions
//
#define H2O_BDS_CP_INIT_GUID \
  { \
    0xd74ef78e, 0xc75b, 0x4786, 0xba, 0x79, 0xd2, 0xc2, 0xb6, 0x72, 0x52, 0x50 \
  }

#define H2O_BDS_CP_END_OF_DXE_BEFORE_GUID \
  { \
    0x6adc6c22, 0x72e1, 0x410b, 0xa9, 0xf2, 0x9a, 0xc6, 0xed, 0xe9, 0xfe, 0x5e \
  }

#define H2O_BDS_CP_END_OF_DXE_AFTER_GUID \
  { \
    0x5b02c9e2, 0x7ce3, 0x403a, 0x99, 0x7c, 0xca, 0x10, 0xd8, 0x66, 0x50, 0x7d \
  }

#define H2O_BDS_CP_PLATFORM_INIT_GUID \
  { \
    0xe0f365e4, 0xa8c0, 0x4766, 0x99, 0x05, 0xb5, 0x34, 0xd7, 0x80, 0xf9, 0x03 \
  }

#define H2O_BDS_CP_DRIVER_CONNECT_BEFORE_GUID \
  { \
    0x3faf7df3, 0x9b55, 0x46f8, 0xad, 0xfa, 0x12, 0x8e, 0x9d, 0x90, 0x7a, 0x1c \
  }

#define H2O_BDS_CP_DRIVER_CONNECT_AFTER_GUID \
  { \
    0x64b0026a, 0x6886, 0x4ca3, 0xa4, 0xa8, 0x95, 0xed, 0x36, 0x68, 0x1f, 0x54 \
  }

#define H2O_BDS_CP_PCI_ENUM_BEGIN_GUID \
  { \
    0x88e3dfe8, 0x4c1c, 0x44c8, 0x94, 0xde, 0xcd, 0xdb, 0x17, 0x7f, 0x23, 0x6b \
  }

#define H2O_BDS_CP_PCI_ENUM_UPDATE_DEV_INFO_GUID \
 { \
    0x24678487, 0x87cc, 0x4f7e, 0x97, 0x98, 0xdd, 0xc5, 0x58, 0x29, 0x12, 0x82 \
 }

#define H2O_BDS_CP_PCI_ENUM_UPDATE_DEV_RESOURCES_GUID \
 { \
    0x82996373, 0xbc18, 0x4dc5, 0x93, 0xf2, 0xf9, 0xe1, 0x51, 0xf6, 0x9b, 0xa1 \
 }


#define H2O_BDS_CP_PCI_ENUM_COMPLETE_BEFORE_GUID \
  { \
    0x8e68f0ed, 0x1c72, 0x4786, 0xab, 0xaa, 0x09, 0x64, 0x94, 0x07, 0x27, 0x28 \
  }

#define H2O_BDS_CP_PCI_ENUM_COMPLETE_AFTER_GUID \
  { \
    0x9fac5ad2, 0xe513, 0x4d16, 0xa8, 0x88, 0x64, 0x70, 0x1e, 0x1d, 0x47, 0xb9 \
  }

#define H2O_BDS_CP_CON_IN_CONNECT_BEFORE_GUID \
  { \
    0x8a1a9798, 0xadfb, 0x4483, 0xa1, 0xc3, 0x29, 0x9d, 0xab, 0x99, 0x46, 0xdb \
  }

#define H2O_BDS_CP_CON_IN_CONNECT_AFTER_GUID \
  { \
    0xf32791c1, 0x6551, 0x449d, 0xbe, 0x5a, 0x99, 0xbe, 0x0, 0xe, 0xe6, 0x6b \
  }

#define H2O_BDS_CP_CON_OUT_CONNECT_BEFORE_GUID \
  { \
    0x68655f09, 0x464a, 0x4642, 0xa7, 0x22, 0xd1, 0x18, 0x93, 0xbd, 0x3f, 0x7b \
  }

#define H2O_BDS_CP_CON_OUT_CONNECT_AFTER_GUID \
  { \
    0xd2c4a682, 0xa6ef, 0x4410, 0xb1, 0x2d, 0x58, 0xd6, 0xdd, 0xc1, 0xf0, 0xb2 \
  }

#define H2O_BDS_CP_HOT_KEY_DETECTED_GUID \
  { \
    0x1168f79a, 0x7841, 0x417b, 0xad, 0x9a, 0x91, 0xbd, 0x3d, 0x57, 0x52, 0xe4 \
  }

#define H2O_BDS_CP_DXE_SMM_READY_TO_LOCK_BEFORE_GUID \
  { \
    0x191c1ba0, 0x18bb, 0x4ed5, 0x8b, 0x3, 0x45, 0xc8, 0x57, 0xc8, 0xb5, 0xa6 \
  }

#define H2O_BDS_CP_DXE_SMM_READY_TO_LOCK_AFTER_GUID \
  { \
    0xa03514ea, 0x127b, 0x4cd1, 0x86, 0xfa, 0xe4, 0xd3, 0xe6, 0xa, 0x6b, 0x92 \
  }

#define H2O_BDS_CP_DISPLAY_BEFORE_GUID \
  { \
    0xc5d30c2c, 0xd1f6, 0x401b, 0xbb, 0xd3, 0x74, 0xe6, 0xde, 0x1b, 0x4f, 0xf \
  }

#define H2O_BDS_CP_DISPLAY_STRING_BEFORE_GUID \
  { \
    0x9ba004e5, 0xf300, 0x453a, 0xb3, 0xbe, 0x3b, 0x7c, 0xeb, 0x21, 0xb7, 0x74 \
  }

#define H2O_BDS_CP_CONNECT_ALL_BEFORE_GUID \
  { \
    0x10b1ae5f, 0xf4d1, 0x4c15, 0x89, 0xd0, 0xa8, 0xc2, 0x67, 0x28, 0xf5, 0x80  \
  }

#define H2O_BDS_CP_NETWORK_UPDATE_MAC_ADDR_GUID \
  { \
    0x6efd5d42, 0x7951, 0x4396, 0xb2, 0x23, 0x7f, 0x6c, 0xdc, 0x60, 0x66, 0xd8 \
  }

#define H2O_BDS_CP_CONNECT_ALL_AFTER_GUID \
  { \
    0xadd30d8b, 0xda29, 0x40fe, 0x9b, 0x5e, 0x54, 0x3c, 0xda, 0xa3, 0x25, 0xe7 \
  }

#define H2O_BDS_CP_BOOT_DEVICE_ENUM_BEFORE_GUID \
  { \
    0xe94da155, 0x331a, 0x4944, 0xb8, 0x21, 0xe1, 0x68, 0xdd, 0x30, 0xdc, 0xbc  \
  }

#define H2O_BDS_CP_BOOT_DEVICE_REGISTER_LEGACY_BOOT_OPTION_GUID \
  { \
    0xd9787a56, 0x7282, 0x42e7, 0x80, 0x8b, 0x39, 0xb2, 0x18, 0x7a, 0x2a, 0xd1  \
  }

#define H2O_BDS_CP_BOOT_DEVICE_ENUM_CHECK_BOOT_OPTION_GUID \
  { \
    0x5a5c6d88, 0x3ad9, 0x4e43, 0xb1, 0xb7, 0x98, 0x31, 0x57, 0x42, 0xad, 0x4c  \
  }

#define H2O_BDS_CP_BOOT_DEVICE_REGISTER_EFI_BOOT_OPTION_GUID \
  { \
    0xfd68b83d, 0xc2dd, 0x4582, 0xb8, 0x0d, 0x43, 0xca, 0xc1, 0xa0, 0x36, 0x1f  \
  }

#define H2O_BDS_CP_BOOT_DEVICE_ENUM_AFTER_GUID \
  { \
    0x16bb17c5, 0x5757, 0x46fc, 0xa3, 0xef, 0xdb, 0x00, 0xbd, 0xc0, 0xab, 0xb1  \
  }

#define H2O_BDS_CP_RECOVERY_COMPLETE_GUID \
  { \
    0xb2d6ab51, 0x8f12, 0x49a6, 0x97, 0xa6, 0x8e, 0x41, 0x4c, 0x8b, 0xb3, 0x29 \
  }

#define H2O_BDS_CP_SYSPREP_GUID \
  { \
    0x1536c670, 0x7cb6, 0x437e, 0xa8, 0xca, 0xce, 0x06, 0xcf, 0xff, 0xe6, 0x3a \
  }

#define H2O_BDS_CP_OS_RECOVERY_GUID \
  { \
    0x4e66c7e8, 0x08f4, 0x49b1, 0x82, 0x11, 0xb7, 0x22, 0xd7, 0x24, 0x7e, 0xac \
  }

#define H2O_BDS_CP_PLATFORM_RECOVERY_GUID \
  { \
    0x0814d9f4, 0x4ef9, 0x4c8e, 0xb1, 0x90, 0x28, 0xe5, 0x63, 0x34, 0x90, 0xf2 \
  }

#define H2O_BDS_CP_BOOT_DEVICE_SELECT_GUID \
  { \
    0x633370f0, 0x4b8c, 0x4100, 0x84, 0xb7, 0x3b, 0x41, 0x8a, 0x5d, 0xc5, 0xba \
  }

#define H2O_BDS_CP_SEND_FORM_BEFORE_GUID \
  { \
    0xf513ba71, 0x652e, 0x41f9, 0x8c, 0x99, 0x48, 0x65, 0x5f, 0xb0, 0x40, 0x96 \
  }

#define H2O_BDS_CP_FORM_BROWSER_CALLBACK_BEFORE_GUID \
  { \
    0x7324b318, 0xd0a7, 0x4104, 0xa9, 0xff, 0xd6, 0xd1, 0x61, 0x00, 0x8e, 0xc0 \
  }

#define H2O_BDS_CP_FORM_BROWSER_CALLBACK_AFTER_GUID \
  { \
    0x557d5c05, 0x533f, 0x4bf7, 0x88, 0xb9, 0xa6, 0x7c, 0x53, 0x9e, 0xb9, 0x64 \
  }

#define H2O_BDS_CP_FORM_BROWSER_EXTRACT_CONFIG_BEFORE_GUID \
  { \
    0x872f7892, 0x02ab, 0x4345, 0x9b, 0x29, 0xe7, 0x15, 0x0f, 0xc0, 0x7b, 0x6f \
  }

#define H2O_BDS_CP_FORM_BROWSER_EXTRACT_CONFIG_AFTER_GUID \
  { \
    0xa4c351c6, 0xfdc2, 0x4e83, 0xb3, 0x9e, 0xdb, 0x58, 0x6e, 0x01, 0x93, 0xb6 \
  }

#define H2O_BDS_CP_FORM_BROWSER_ROUTE_CONFIG_BEFORE_GUID \
  { \
    0xf4ad3273, 0x7689, 0x418a, 0x92, 0x44, 0xb5, 0xa8, 0x14, 0x39, 0xd7, 0xfa \
  }

#define H2O_BDS_CP_FORM_BROWSER_ROUTE_CONFIG_AFTER_GUID \
  { \
    0xac77a0d8, 0x1c01, 0x48a0, 0xac, 0x67, 0xe0, 0x9f, 0x2b, 0x33, 0x17, 0xa0 \
  }

#define H2O_BDS_CP_FORM_BROWSER_IDLE_BEFORE_GUID \
  { \
    0x902e62c4, 0xe93b, 0x474c, 0x89, 0x0b, 0xe9, 0x39, 0xb6, 0x1c, 0x98, 0x77 \
  }

#define H2O_BDS_CP_FORM_BROWSER_IDLE_AFTER_GUID \
  { \
    0x65555e1e, 0x5883, 0x4d17, 0x97, 0xc5, 0x40, 0xf3, 0xac, 0x0c, 0xd4, 0xc6 \
  }

#define H2O_BDS_CP_FORM_BROWSER_USER_INPUT_GUID \
  { \
    0x4b01db42, 0x0284, 0x4754, 0xb4, 0x36, 0xa9, 0x9c, 0xa9, 0x7b, 0x50, 0x0a \
  }

#define H2O_BDS_CP_FORM_BROWSER_CHECK_PASSWORD_GUID \
  { \
    0x5e08f31e, 0x926d, 0x4834, 0xbc, 0x3a, 0x82, 0x9f, 0xda, 0x59, 0x96, 0xd2 \
  }

#define H2O_BDS_CP_FORM_BROWSER_UPDATE_PASSWORD_DIALOG_MESSAGE_GUID \
  { \
    0xaed40901, 0xe63b, 0x460f, 0x9c, 0xc8, 0x36, 0xdb, 0x7a, 0x71, 0xf7, 0x75 \
  }

#define H2O_BDS_CP_SEND_FORM_AFTER_GUID \
  { \
    0x9afa9a40, 0x2e13, 0x49b6, 0x99, 0x40, 0xd7, 0x4e, 0x35, 0xb5, 0xe5, 0x65 \
  }

#define H2O_BDS_CP_NO_BOOT_DEVICE_GUID \
  { \
    0xde22bc1a, 0x3499, 0x4207, 0xa0, 0xde, 0x3d, 0x70, 0x6a, 0xbe, 0x5f, 0xf1 \
  }

#define H2O_BDS_CP_READY_TO_BOOT_BEFORE_GUID \
  { \
    0x463ee173, 0x1485, 0x4749, 0xb7, 0x82, 0x91, 0x30, 0xdd, 0xf2, 0x2e, 0xaa \
  }

#define H2O_BDS_CP_UPDATE_ACPI_DESC_HDR_GUID \
  { \
    0xa6f4cc46, 0x892b, 0x4021, 0xba, 0xfb, 0xbf, 0x6d, 0x3b, 0xab, 0x1a, 0x96 \
  }

#define H2O_BDS_CP_UPDATE_ACPI_FACS_GUID \
  { \
    0x944a6f0b, 0xfa9a, 0x4ee3, 0x97, 0x03, 0x77, 0x29, 0xc5, 0x16, 0x07, 0xaf \
  }

#define H2O_BDS_CP_UPDATE_ACPI_SDEV_GUID \
  { \
    0xf9d1b3aa, 0xd8a1, 0x4a66, 0xa1, 0xf5, 0xab, 0x4a, 0xcc, 0x8b, 0xb5, 0x1d \
  }

#define H2O_BDS_CP_READY_TO_BOOT_AFTER_GUID \
  { \
    0xef208648, 0xb128, 0x4257, 0x9f, 0x21, 0xba, 0xaa, 0x51, 0x17, 0xef, 0xa8 \
  }

#define H2O_BDS_CP_CAPSULE_UPDATE_PROGRESS_GUID \
  { \
    0x66a9c3a7, 0x69be, 0x435c, 0x8a, 0xf1, 0x3e, 0x1d, 0xf4, 0x9, 0x9e, 0x66 \
  }

#define H2O_BDS_CP_UPDATE_MEMORY_TYPE_INFO_GUID \
  { \
    0x52581db1, 0x7880, 0x4d9a, 0xa2, 0xa1, 0x7b, 0x34, 0x44, 0x4e, 0x36, 0x33 \
  }

#define H2O_BDS_CP_BOOT_BEFORE_GUID \
  { \
    0xe7e316d5, 0xffb9, 0x4cec, 0xba, 0x82, 0x3b, 0x0, 0xe5, 0x3e, 0x12, 0xa8 \
  }

#define H2O_BDS_CP_CAPSULE_CHECK_UPDATE_CAPABILITY_GUID \
  { \
    0x8d2b6d59, 0x0adb, 0x4611, 0xb4, 0x1b, 0x22, 0xfa, 0x31, 0x78, 0x7a, 0x51 \
  }

#define H2O_BDS_CP_BOOT_AFTER_GUID \
  { \
    0x4173b6cd, 0x5711, 0x462c, 0x9c, 0x6e, 0x1f, 0x3c, 0x95, 0x49, 0xcc, 0x9f \
  }

#define H2O_BDS_CP_EXIT_BOOT_SERVICES_BEFORE_GUID \
  { \
    0x1dd55b9d, 0x3e0c, 0x41b1, 0xb0, 0xcf, 0x5b, 0xfd, 0x70, 0xb5, 0xc2, 0x12 \
  }

#define H2O_BDS_CP_LEGACY_PREPARE_TO_BOOT_BEFORE_GUID \
  { \
    0xf1bda499, 0x437f, 0x4058, 0xb8, 0xb2, 0xed, 0xb7, 0x87, 0x5a, 0x58, 0x58 \
  }

#define H2O_BDS_CP_LEGACY_PREPARE_TO_BOOT_AFTER_GUID \
  { \
    0xeed9baa0, 0xfc03, 0x4a3d, 0x92, 0xf2, 0x1b, 0x22, 0x71, 0x7f, 0xd0, 0x62 \
  }

#define H2O_BDS_CP_LEGACY_BOOT_BEFORE_GUID \
  { \
    0xf9afcfac, 0x68bb, 0x4c3c, 0x9e, 0xe1, 0x33, 0x5a, 0x91, 0x2b, 0x30, 0xb5 \
  }

#define H2O_BDS_CP_LEGACY_BOOT_AFTER_GUID \
  { \
    0xb96a8be, 0xb79b, 0x419e, 0x8b, 0x13, 0x70, 0x45, 0x37, 0xac, 0x3c, 0xdb \
  }

#define H2O_BDS_CP_BOOT_SUCCESS_GUID \
  { \
    0x75f1ff71, 0xd7bb, 0x4225, 0xa8, 0xc8, 0xab, 0x4c, 0xeb, 0xe6, 0x1b, 0x52 \
  }

#define H2O_BDS_CP_BOOT_FAILED_GUID \
  { \
    0xd4e5b97d, 0xb7de, 0x469b, 0xb7, 0xe3, 0x2a, 0x9c, 0x27, 0x6a, 0x11, 0xe8 \
  }

//
// BDS CP GUID definitions.
// NOTE: Below GUID defines are deprecated. Suggest using H2O_BDS_CP_XXX_YYY_GUID not Below
// H2O_BDS_CP_XXX_YYY_PROTOCOLGUID
//
#define H2O_BDS_CP_INIT_PROTOCOL_GUID                        H2O_BDS_CP_INIT_GUID
#define H2O_BDS_CP_END_OF_DXE_BEFORE_PROTOCOL_GUID           H2O_BDS_CP_END_OF_DXE_BEFORE_GUID
#define H2O_BDS_CP_END_OF_DXE_AFTER_PROTOCOL_GUID            H2O_BDS_CP_END_OF_DXE_AFTER_GUID
#define H2O_BDS_CP_DRIVER_CONNECT_BEFORE_PROTOCOL_GUID       H2O_BDS_CP_DRIVER_CONNECT_BEFORE_GUID
#define H2O_BDS_CP_DRIVER_CONNECT_AFTER_PROTOCOL_GUID        H2O_BDS_CP_DRIVER_CONNECT_AFTER_GUID
#define H2O_BDS_CP_CON_IN_CONNECT_BEFORE_PROTOCOL_GUID       H2O_BDS_CP_CON_IN_CONNECT_BEFORE_GUID
#define H2O_BDS_CP_CON_IN_CONNECT_AFTER_PROTOCOL_GUID        H2O_BDS_CP_CON_IN_CONNECT_AFTER_GUID
#define H2O_BDS_CP_CON_OUT_CONNECT_BEFORE_PROTOCOL_GUID      H2O_BDS_CP_CON_OUT_CONNECT_BEFORE_GUID
#define H2O_BDS_CP_CON_OUT_CONNECT_AFTER_PROTOCOL_GUID       H2O_BDS_CP_CON_OUT_CONNECT_AFTER_GUID
#define H2O_BDS_CP_DISPLAY_BEFORE_PROTOCOL_GUID              H2O_BDS_CP_DISPLAY_BEFORE_GUID
#define H2O_BDS_CP_DISPLAY_STRING_BEFORE_PROTOCOL_GUID       H2O_BDS_CP_DISPLAY_STRING_BEFORE_GUID
#define H2O_BDS_CP_CONNECT_ALL_BEFORE_PROTOCOL_GUID          H2O_BDS_CP_CONNECT_ALL_BEFORE_GUID
#define H2O_BDS_CP_CONNECT_ALL_AFTER_PROTOCOL_GUID           H2O_BDS_CP_CONNECT_ALL_AFTER_GUID
#define H2O_BDS_CP_SYSPREP_PROTOCOL_GUID                     H2O_BDS_CP_SYSPREP_GUID
#define H2O_BDS_CP_OS_RECOVERY_PROTOCOL_GUID                 H2O_BDS_CP_OS_RECOVERY_GUID
#define H2O_BDS_CP_PLATFORM_RECOVERY_PROTOCOL_GUID           H2O_BDS_CP_PLATFORM_RECOVERY_GUID
#define H2O_BDS_CP_BOOT_DEVICE_SELECT_PROTOCOL_GUID          H2O_BDS_CP_BOOT_DEVICE_SELECT_GUID
#define H2O_BDS_CP_NO_BOOT_DEVICE_PROTOCOL_GUID              H2O_BDS_CP_NO_BOOT_DEVICE_GUID
#define H2O_BDS_CP_READY_TO_BOOT_BEFORE_PROTOCOL_GUID        H2O_BDS_CP_READY_TO_BOOT_BEFORE_GUID
#define H2O_BDS_CP_READY_TO_BOOT_AFTER_PROTOCOL_GUID         H2O_BDS_CP_READY_TO_BOOT_AFTER_GUID
#define H2O_BDS_CP_BOOT_BEFORE_PROTOCOL_GUID                 H2O_BDS_CP_BOOT_BEFORE_GUID
#define H2O_BDS_CP_BOOT_AFTER_PROTOCOL_GUID                  H2O_BDS_CP_BOOT_AFTER_GUID
#define H2O_BDS_CP_EXIT_BOOT_SERVICES_BEFORE_PROTOCOL_GUID   H2O_BDS_CP_EXIT_BOOT_SERVICES_BEFORE_GUID
#define H2O_BDS_CP_BOOT_SUCCESS_PROTOCOL_GUID                H2O_BDS_CP_BOOT_SUCCESS_GUID
#define H2O_BDS_CP_BOOT_FAILED_PROTOCOL_GUID                 H2O_BDS_CP_BOOT_FAILED_GUID
#define H2O_BDS_CP_LEGACY_BOOT_AFTER_PROTOCOL_GUID           H2O_BDS_CP_LEGACY_BOOT_AFTER_GUID
#define H2O_BDS_CP_LEGACY_BOOT_BEFORE_PROTOCOL_GUID          H2O_BDS_CP_LEGACY_BOOT_BEFORE_GUID

// @Prompt Structure definition for checkpoint data
typedef struct _H2O_CP_DATA {
  UINT32 Size;
  UINT32 Status;
} H2O_CP_DATA;

//
// BASE CP structure definitions
//
// @Prompt Structure definition for gH2OBaseCpPostCodeGuid checkpoint
typedef struct _H2O_BASE_CP_POST_CODE_DATA {
  UINT32          Size;                 /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;               /// Enumerated value that specifies the kernel behavior.
  UINT32          PostCodeValue;        /// The 32-bit value to write to the POST card.
} H2O_BASE_CP_POST_CODE_DATA;

// @Prompt Structure definition for gH2OBaseCpVerifyFvGuid checkpoint
typedef struct _H2O_BASE_CP_VERIFY_FV_DATA {
  UINT32                Size;                 /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32                Status;               /// Enumerated value that specifies the kernel behavior.
  EFI_PHYSICAL_ADDRESS  FvBase;               /// Specifies Base address of FV image.
  UINT64                FvLength;             /// Specifies the length of FV image.
  BOOLEAN               Trusted;              /// BOOLEAN that specifies the FV is whether verified passed
} H2O_BASE_CP_VERIFY_FV_DATA;

//
// PEI CP structure definitions
//
// @Prompt Structure definition for gH2OPeiCpGetVariableStoreStatusGuid checkpoint
typedef struct _H2O_PEI_CP_FORCE_VARIABLE_DEFAULTS_DATA {
  UINT32                  Size;                   /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32                  Status;                 /// Enumerated value that specifies the kernel behavior.
  VARIABLE_STORE_HEADER   *VariableStoreHdr;      /// Pointer to the variable store header.
  BOOLEAN                 ForceVariableDefaults;  /// Status of Variable Store Region.
} H2O_PEI_CP_FORCE_VARIABLE_DEFAULTS_DATA;

// @Prompt Structure definition for gH2OPeiCpInitPlatformStage1Guid checkpoint
typedef struct _H2O_PEI_CP_INIT_PLATFORM_STAGE_1_DATA {
  UINT32          Size;                 /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;               /// Enumerated value that specifies the kernel behavior.
} H2O_PEI_CP_INIT_PLATFORM_STAGE_1_DATA;

// @Prompt Structure definition for gH2OPeiCpSetBootModeBeforeGuid checkpoint
typedef struct _H2O_PEI_CP_SET_BOOT_MODE_BEFORE_DATA {
  UINT32          Size;                 /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;               /// Enumerated value that specifies the kernel behavior.
  EFI_BOOT_MODE   BootMode;             /// On entry, specifies the boot mode that the kernel is planning to set.
                                        /// On exit, indicates the boot mode that the kernel should set (if the status
                                        /// is H2O_CP_TASK_UPDATE)
  BOOLEAN         CustomBootModePolicy; /// Specifies that the boot mode has been prioritized using the Default Policy
                                        /// (FALSE) or is the original value returned by the kernel or
                                        /// OemSvcChangeBootMode() (TRUE).
} H2O_PEI_CP_SET_BOOT_MODE_BEFORE_DATA;

// @Prompt Structure definition for gH2OPeiCpInitPlatformStage2Guid checkpoint
typedef struct _H2O_PEI_CP_INIT_PLATFORM_STAGE_2_DATA {
  UINT32          Size;                 /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;               /// Enumerated value that specifies the kernel behavior.
} H2O_PEI_CP_INIT_PLATFORM_STAGE_2_DATA;

// @Prompt Structure definition for gH2OPeiCpCrisisRecoveryPublishFvGuiid checkpoint
typedef struct _H2O_PEI_CP_CRISIS_RECOVERY_PUBLISH_FV_DATA {
     UINT32                      Size;                 /// Unsigned integer that specifies the size of this structure, in bytes.
     UINT32                      Status;               /// Enumerated value that specifies the kernel behavior.
  IN EFI_FIRMWARE_VOLUME_HEADER  *FvHeader;            /// Pointer to firmware volume header.
} H2O_PEI_CP_CRISIS_RECOVERY_PUBLISH_FV_DATA;


// @Prompt Structure definition for gH2OPeiCpCrisisRecoveryGetProtectTableGuid checkpoint
typedef struct _H2O_PEI_CP_CRISIS_RECOVERY_GET_PROTECT_TABLE_DATA {
  UINT32                 Size;                 /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32                 Status;               /// Enumerated value that specifies the kernel behavior.
  UINT32                 ProtectTableCount;    /// Unsigned integer that specifies the number of entries in the protected region table ProtectTable.
  FLASH_ENTRY            *ProtectTable;        /// Pointer to the protected region table.
} H2O_PEI_CP_CRISIS_RECOVERY_GET_PROTECT_TABLE_DATA;

// @Prompt Structure definition for gH2OPeiCpCrisisRecoveryFlashGuid checkpoint
typedef struct _H2O_PEI_CP_CRISIS_RECOVERY_FLASH_DATA {
  UINT32                 Size;                 /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32                 Status;               /// Enumerated value that specifies the kernel behavior.
  UINT32                 RecoveryCapsuleSize;  /// Unsigned integer that specifies the recovery capsule file size indicated by RecoveryCapsule.
  UINT8                  *RecoveryCapsule;     /// Pointer to the recovery capsule file.
} H2O_PEI_CP_CRISIS_RECOVERY_FLASH_DATA;

// @Prompt Structure definition for gH2OPeiCpEndOfPeiBeforeGuid checkpoint
typedef struct _H2O_PEI_CP_END_OF_PEI_BEFORE_DATA {
  UINT32          Size;                 /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;               /// Enumerated value that specifies the kernel behavior.
} H2O_PEI_CP_END_OF_PEI_BEFORE_DATA;

// @Prompt Structure definition for gH2OPeiCpEndOfPeiAfterGuid checkpoint
typedef struct _H2O_PEI_CP_END_OF_PEI_AFTER_DATA {
  UINT32          Size;                 /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;               /// Enumerated value that specifies the kernel behavior.
} H2O_PEI_CP_END_OF_PEI_AFTER_DATA;

//
// DXE CP structure definitions
//
// @Prompt Structure definition for gH2ODxeCpWriteVariableDefaultsGuid checkpoint
typedef struct _H2O_DXE_CP_WRITE_VARIABLE_DEFAULTS_DATA {
  UINT32          Size;                 /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;               /// Enumerated value that specifies the kernel behavior.
} H2O_DXE_CP_WRITE_VARIABLE_DEFAULTS_DATA;


// @Prompt Structure definition for gH2ODxeCpPciCheckSysOptionRomGuid checkpoint
typedef struct _H2O_DXE_CP_PCI_CHECK_SYSTEM_OPTION_ROM_DATA {
  UINT32          Size;                 /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;               /// Enumerated value that specifies the kernel behavior.
  EFI_GUID        FileName;             /// Option ROM file GUID.
  UINT16          VendorId;             /// Option ROM vendor ID.
  UINT16          DeviceId;             /// Option ROM device ID.
} H2O_DXE_CP_PCI_CHECK_SYSTEM_OPTION_ROM_DATA;

// @Prompt Structure definition for gH2ODxeCpUpdateSmbiosRecord checkpoint
typedef struct _H2O_DXE_CP_UPDATE_SMBIOS_RECORD_DATA {
  UINT32                    Size;                 /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32                    Status;               /// Enumerated value that specifies the kernel behavior.
  EFI_SMBIOS_TABLE_HEADER   *SmbiosTableHeader;   /// Pointer to the SMBIOS Table header
} H2O_DXE_CP_UPDATE_SMBIOS_RECORD_DATA;

// @Prompt Structure definition for gH2ODxeCpResetSystemGuid checkpoint
typedef struct _H2O_DXE_CP_RESET_SYSTEM_DATA {
  UINT32          Size;                 /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;               /// Enumerated value that specifies the kernel behavior.
  EFI_RESET_TYPE  ResetType;            /// The type of reset to perform.
  EFI_STATUS      ResetStatus;          /// The status code for the reset.
  UINT32          ResetDataSize;        /// The size, in bytes, of ResetData.
  VOID            *ResetData;           /// For a ResetType of EfiResetCold, EfiResetWarm, or
                                        /// EfiResetShutdown the data buffer starts with a Null-terminated
                                        /// string, optionally followed by additional binary data.
                                        /// The string is a description that the caller may use to further
                                        /// indicate the reason for the system reset. ResetData is only
                                        /// valid if ResetStatus is something other than EFI_SUCCESS
                                        /// unless the ResetType is EfiResetPlatformSpecific
                                        /// where a minimum amount of ResetData is always required.
} H2O_DXE_CP_RESET_SYSTEM_DATA;

#define IGNORE_DEVICE     0xFE
#define END_OF_TABLE      0xFF

typedef struct _HOT_PLUG_BRIDGE_INFO {
  UINT8   NodeUID;
  UINT8   DevNum;
  UINT8   FuncNum;
  UINT8   ReservedBusCount;
  UINT16  ReservedIoRange;
  UINT64  ReservedNonPrefetchableMmio;
  UINT64  AlignemntOfReservedNonPrefetchableMmio;
  UINT64  ReservedPrefetchableMmio;
  UINT64  AlignemntOfReservedPrefetchableMmio;
} HOT_PLUG_BRIDGE_INFO;

// @Prompt Structure definition for gH2ODxeCpPciHpcGetBridgeInfoGuid checkpoint
typedef struct _H2O_DXE_CP_PCI_HPC_GET_BRIDGE_INFO_DATA {
  UINT32                        Size;                                      /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32                        Status;                                    /// Enumerated value that specifies the kernel behavior.
  HOT_PLUG_BRIDGE_INFO          *HotPlugBridgeInfoTable;                   /// Pointer to start address of H2O_HOT_PLUG_BRIDGE_INFO array.
} H2O_DXE_CP_PCI_HPC_GET_BRIDGE_INFO_DATA;

// @Prompt Structure definition for gH2ODxeCpPciHpcGetResourcePaddingGuid checkpoint
typedef struct _H2O_DXE_CP_PCI_HPC_GET_RESOURCE_PADDING_DATA {
  UINT32                        Size;                                      /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32                        Status;                                    /// Enumerated value that specifies the kernel behavior.
  BOOLEAN                       Above4GbMmioSupported;                     /// Above 4G MMIO supported or not.
  BOOLEAN                       ResourcePaddingFound;                      /// Resoure padding found or not
  UINT64                        PciAddress;                                /// PCI address parameter and the format is EFI PCI Configuration Address
  EFI_DEVICE_PATH_PROTOCOL      *PciDevicePath;                            /// The Device Path to the Hot Plug Controller.
  UINT8                         ReservedBusCount;                          /// Reserved PCI bus number.
  UINT16                        ReservedIoRange;                           /// Reserved IO ranges in bytes.
  UINT64                        ReservedNonPrefetchableMmio;               /// Reserved non-prefetchable MMIO base address.
  UINT64                        AlignmentOfReservedNonPrefetchableMmio;    /// Alignment for reserved non-prefetchable MMIO base address.
  UINT64                        ReservedPrefetchableMmio;                  /// Reserved prefetchable MMIO base address.
  UINT64                        AlignmentOfReservedPrefetchableMmio;       /// Alignment for reserved prefetchable MMIO base address.
} H2O_DXE_CP_PCI_HPC_GET_RESOURCE_PADDING_DATA;

// @Prompt Structure definition for gH2ODxeCpUpdateAcpiEsrtTpmFwVersionGuid checkpoint
typedef struct _H2O_DXE_CP_UPDATE_ACPI_ESRT_TPM_FW_VERSION_DATA {
  IN     UINT32                 Size;                                      /// Unsigned integer that specifies the size of this structure, in bytes.
  IN OUT UINT32                 Status;                                    /// Enumerated value that specifies the kernel behavior.
  IN     UINT32                 TpmVersion;                                /// Enumerated value that specifies the revision of the TPM interface specification that this is TPM complies with.
  IN     UINT32                 ManufacturerId;                            /// Unsigned integer that specifies the vendor ID unique to each TPM manufacturer.
  IN     UINT64                 FwVersion;                                 /// Unsigned integer that specifies a TPM vendor-specific value indicating the version number of the firmware.
  IN OUT UINT32                 FwVersionInEsrt;                           /// Unsigned integer that specifies the firmware version field of ESRT which represents the current version of the firmware resource.
  IN OUT UINT32                 LowestSupportedFwVersionInEsrt;            /// Unsigned integer that specifies the lowest firmware resource version field of ESRT to which a firmware resource can be rolled back for the given system/device.
} H2O_DXE_CP_UPDATE_ACPI_ESRT_TPM_FW_VERSION_DATA;

#define H2O_DXE_CP_TPM_1_2      0
#define H2O_DXE_CP_TPM_2_0      1

//
// SMM CP structure definitions
//

// @Prompt Structure definition for gH2OSmmCpIhisiAuthStatusGuid checkpoint
typedef struct _H2O_SMM_CP_IHISI_AUTH_STATUS_DATA {
  IN     UINT32          Size;                     /// Unsigned integer that specifies the size of this structure, in bytes.
  IN OUT UINT32          Status;                   /// Enumerated value that specifies the kernel behavior.
  IN OUT UINT32          SupportedSecurityLevels;  /// Bitmask that specifies the security level to use when opening the IHISI session.
} H2O_SMM_CP_IHISI_AUTH_STATUS_DATA;

// @Prompt Structure definition for gH2OSmmCpIhisiAuthCheckGuid checkpoint
typedef struct _H2O_SMM_CP_IHISI_AUTH_CHECK_DATA {
  IN     UINT32          Size;             /// Unsigned integer that specifies the size of this structure, in bytes.
  IN OUT UINT32          Status;           /// Enumerated value that specifies the kernel behavior.
  IN     UINT32          SecurityLevel;    /// Bitmask that specifies the security level to use when opening the IHISI session.
  IN     VOID            *AuthData;        /// A pointer to a buffer that specifies the credential data associated with the security level.
  IN     UINT32          AuthDataSize;     /// An unsigned integer that specifies the size of the credential data in AuthData.
  IN OUT BOOLEAN         Authorized;       /// On entry, a Boolean that specifies the result of the Kernel authorization check.
                                           /// On exit, this indicates whether the security level should be allowed.
} H2O_SMM_CP_IHISI_AUTH_CHECK_DATA;
//
// BDS CP structure definitions
//
// @Prompt Structure definition for gH2OBdsCpInitGuid checkpoint
typedef struct _H2O_BDS_CP_INIT_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_INIT_DATA;

// @Prompt Structure definition for gH2OBdsCpEndOfDxeBeforeGuid checkpoint
typedef struct _H2O_BDS_CP_END_OF_DXE_BEFORE_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_END_OF_DXE_BEFORE_DATA;

// @Prompt Structure definition for gH2OBdsCpEndOfDxeAfterGuid checkpoint
typedef struct _H2O_BDS_CP_END_OF_DXE_AFTER_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_END_OF_DXE_AFTER_DATA;

// @Prompt Structure definition for gH2OBdsCpPlatformInitGuid checkpoint
typedef struct _H2O_BDS_CP_PLATFORM_INIT_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_PLATFORM_INIT_DATA;

// @Prompt Structure definition for gH2OBdsCpDriverConnectBeforeGuid checkpoint
typedef struct _H2O_BDS_CP_DRIVER_CONNECT_BEFORE_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_DRIVER_CONNECT_BEFORE_DATA;

// @Prompt Structure definition for gH2OBdsCpDriverConnectAfterGuid checkpoint
typedef struct _H2O_BDS_CP_DRIVER_CONNECT_AFTER_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_DRIVER_CONNECT_AFTER_DATA;

// @Prompt Structure definition for gH2OBdsCpPciEnumBeginGuid checkpoint
typedef struct _H2O_BDS_CP_PCI_ENUM_BEGIN_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_PCI_ENUM_BEGIN_DATA;

typedef enum {
  H2OPciBarTypeUnknown = 0,
  H2OPciBarTypeIo16,
  H2OPciBarTypeIo32,
  H2OPciBarTypeMem32,
  H2OPciBarTypePMem32,
  H2OPciBarTypeMem64,
  H2OPciBarTypePMem64,
  H2OPciBarTypeIo,
  H2OPciBarTypeMem,
  H2OPciBarTypeMaxType
} H2O_PCI_BAR_TYPE;

typedef struct _H2O_PCI_BAR {
  UINT64             BaseAddress;
  UINT64             Length;
  UINT64             Alignment;
  H2O_PCI_BAR_TYPE   BarType;
  BOOLEAN            BarTypeFixed;
  UINT16             Offset;
} H2O_PCI_BAR;

typedef struct _H2O_PCI_IO_DEVICE  H2O_PCI_IO_DEVICE;

struct _H2O_PCI_IO_DEVICE {
  UINT32                                    Signature;
  EFI_HANDLE                                Handle;
  EFI_PCI_IO_PROTOCOL                       PciIo;
  LIST_ENTRY                                Link;
  EFI_BUS_SPECIFIC_DRIVER_OVERRIDE_PROTOCOL PciDriverOverride;
  EFI_DEVICE_PATH_PROTOCOL                  *DevicePath;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL           *PciRootBridgeIo;
  EFI_LOAD_FILE2_PROTOCOL                   LoadFile2;
  PCI_TYPE00                                Pci;
  UINT8                                     BusNumber;
  UINT8                                     DeviceNumber;
  UINT8                                     FunctionNumber;
  H2O_PCI_BAR                               PciBar[PCI_MAX_BAR];
  H2O_PCI_BAR                               OptionRomBar;
  H2O_PCI_IO_DEVICE                         *Parent;
  LIST_ENTRY                                ChildList;
  BOOLEAN                                   Registered;
  BOOLEAN                                   Allocated;
  UINT64                                    Attributes;
  UINT64                                    Supports;
  UINT32                                    Decodes;
  BOOLEAN                                   EmbeddedRom;
  UINT64                                    RomSize;
  BOOLEAN                                   AllOpRomProcessed;
  BOOLEAN                                   BusOverride;
  LIST_ENTRY                                ReservedResourceList;
  LIST_ENTRY                                OptionRomDriverList;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR         *ResourcePaddingDescriptors;
  EFI_HPC_PADDING_ATTRIBUTES                PaddingAttributes;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR         *BusNumberRanges;
  BOOLEAN                                   IsPciExp;
  UINT8                                     PciExpressCapabilityOffset;
  UINT32                                    AriCapabilityOffset;
  UINT32                                    SrIovCapabilityOffset;
  UINT32                                    MrIovCapabilityOffset;
  H2O_PCI_BAR                               VfPciBar[PCI_MAX_BAR];
  UINT32                                    SystemPageSize;
  UINT16                                    InitialVFs;
  UINT16                                    ReservedBusNum;
  UINT16                                    BridgeIoAlignment;
};

// @Prompt Structure definition for gH2OBdsCpPciEnumUpdateDevInfoGuid checkpoint
typedef struct _H2O_BDS_CP_PCI_ENUM_UPDATE_DEV_INFO_DATA {
  UINT32              Size;           /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32              Status;         /// Unsigned integer that specifies the size of this structure, in bytes.
  H2O_PCI_IO_DEVICE   *PciDevice;     /// Points to information about the PCI device that was discovered during PCI enumeration.
} H2O_BDS_CP_PCI_ENUM_UPDATE_DEV_INFO_DATA;

// @Prompt Structure definition for gH2OBdsCpPciEnumUpdateDevResourcesGuid checkpoint
typedef struct _H2O_BDS_CP_PCI_ENUM_UPDATE_DEV_RESOURCES_DATA {
  UINT32              Size;           /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32              Status;         /// Unsigned integer that specifies the size of this structure, in bytes.
  H2O_PCI_IO_DEVICE   *PciDevice;     /// Points to information about the PCI device that was discovered during PCI enumeration.
} H2O_BDS_CP_PCI_ENUM_UPDATE_DEV_RESOURCES_DATA;

// @Prompt Structure definition for gH2OBdsCpPciEnumCompleteBeforeGuid checkpoint
typedef struct _H2O_BDS_CP_PCI_ENUM_COMPLETE_BEFORE_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_PCI_ENUM_COMPLETE_BEFORE_DATA;

// @Prompt Structure definition for gH2OBdsCpPciEnumCompleteAfterGuid checkpoint
typedef struct _H2O_BDS_CP_PCI_ENUM_COMPLETE_AFTER_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_PCI_ENUM_COMPLETE_AFTER_DATA;


// @Prompt Structure definition for gH2OBdsCpConInConnectBeforeGuid checkpoint
typedef struct _H2O_BDS_CP_CON_IN_CONNECT_BEFORE_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_CON_IN_CONNECT_BEFORE_DATA;

// @Prompt Structure definition for gH2OBdsCpConInConnectAfterGuid checkpoint
typedef struct _H2O_BDS_CP_CON_IN_CONNECT_AFTER_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_CON_IN_CONNECT_AFTER_DATA;

// @Prompt Structure definition for gH2OBdsCpConOutConnectBeforeGuid checkpoint
typedef struct _H2O_BDS_CP_CON_OUT_CONNECT_BEFORE_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_CON_OUT_CONNECT_BEFORE_DATA;

// @Prompt Structure definition for gH2OBdsCpConOutConnectAfterGuid checkpoint
typedef struct _H2O_BDS_CP_CON_OUT_CONNECT_AFTER_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_CON_OUT_CONNECT_AFTER_DATA;

// @Prompt Structure definition for gH2OBdsCpHotKeyDetectedGuid checkpoint
typedef struct _H2O_BDS_CP_HOT_KEY_DETECTED_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
  CONST CHAR8     *HotKey;
} H2O_BDS_CP_HOT_KEY_DETECTED_DATA;

// @Prompt Structure definition for gH2OBdsCpDxeSmmReadyToLockBeforeGuid checkpoint
typedef struct _H2O_BDS_CP_DXE_SMM_READY_TO_LOCK_BEFORE_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_DXE_SMM_READY_TO_LOCK_BEFORE_DATA;

// @Prompt Structure definition for gH2OBdsCpDxeSmmReadyToLockAfterGuid checkpoint
typedef struct _H2O_BDS_CP_DXE_SMM_READY_TO_LOCK_AFTER_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_DXE_SMM_READY_TO_LOCK_AFTER_DATA;

// @Prompt Structure definition for gH2OBdsCpDisplayBeforeGuid checkpoint
typedef struct _H2O_BDS_CP_DISPLAY_BEFORE_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
  UINT32          Features;     /// Bitmask that specifies the kernel's current policy concerning the primary display contents.
  EFI_IMAGE_INPUT *Image;       /// On input, this points to the read-only graphics image that the kernel intends to display on
                                /// the screen (if Features.H2O_BDS_CP_DISPLAY_LOGO is set) and place into the BGRT table
                                /// (if Features.H2O_BDS_CP_DISPLAY_BGRT is set). On output, this points to the graphics image
                                /// that the checkpoint handler will display as its final image.
} H2O_BDS_CP_DISPLAY_BEFORE_DATA;

//
// Bitmask that specifies the kernel's current policy concerning the primary display contents
//
#define H2O_BDS_CP_DISPLAY_LOGO          0x0000000000000001
#define H2O_BDS_CP_DISPLAY_INFO          0x0000000000000002
#define H2O_BDS_CP_DISPLAY_BGRT          0x0000000000000004

// @Prompt Structure definition for gH2OBdsCpDisplayStringBeforeGuid checkpoint
typedef struct _H2O_BDS_CP_DISPLAY_STRING_BEFORE_DATA {
  UINT32                             Size;               /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32                             Status;             /// Enumerated value that specifies the kernel behavior.
  EFI_OEM_BADGING_SUPPORT_PROTOCOL   *Badging;           /// Optional pointer to an instance of the EFI_OEM_BADGING_SUPPORT_PROTOCOL.
  BOOLEAN                            AfterSelect;        /// Boolean that specifies after user presses hotkey or not.
  UINT8                              SelectedStringNum;  /// Unsigned integer that specifies the string in rows that will be displayed.
} H2O_BDS_CP_DISPLAY_STRING_BEFORE_DATA;

// @Prompt Structure definition for gH2OBdsCpConnectAllBeforeGuid checkpoint
typedef struct _H2O_BDS_CP_CONNECT_ALL_BEFORE_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_CONNECT_ALL_BEFORE_DATA;

// @Prompt Structure definition for gH2OBdsCpNetworkUpdateMacAddrGuid checkpoint
typedef struct _H2O_BDS_CP_NETWORK_UPDATE_MAC_ADDR_DATA {
  IN     UINT32           Size;       /// Unsigned integer that specifies the size of this structure, in bytes.
  IN OUT UINT32           Status;     /// Enumerated value that specifies the kernel behavior.
  IN     EFI_HANDLE       NicHandle;  /// UNDI card device handle.
  IN OUT EFI_MAC_ADDRESS  NicMacAddr; /// Output is MAC address in all zero, INPUT MAC will replace the original MAC address on that device.
} H2O_BDS_CP_NETWORK_UPDATE_MAC_ADDR_DATA;

// @Prompt Structure definition for gH2OBdsCpConnectAllAfterGuid checkpoint
typedef struct _H2O_BDS_CP_CONNECT_ALL_AFTER_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_CONNECT_ALL_AFTER_DATA;

// @Prompt Structure definition for gH2OBdsCpBootDeviceEnumBeforeGuid checkpoint
typedef struct _H2O_BDS_CP_BOOT_DEVICE_ENUM_BEFORE_DATA {
  IN UINT32                         Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  IN UINT32                         Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_BOOT_DEVICE_ENUM_BEFORE_DATA;

// @Prompt Structure definition for gH2OBdsCpBootDeviceEnumCheckBootOptionGuid checkpoint
typedef struct _H2O_BDS_CP_BOOT_DEVICE_ENUM_CHECK_BOOT_OPTION_DATA {
  IN     UINT32                     Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  IN OUT UINT32                     Status;       /// Enumerated value that specifies the kernel behavior.
  IN     CONST H2O_BDS_LOAD_OPTION  *BootOption;
  IN OUT BOOLEAN                    Valid;        /// Boolean that specifies whether this boot option is valid or not.
} H2O_BDS_CP_BOOT_DEVICE_ENUM_CHECK_BOOT_OPTION_DATA;

// @Prompt Structure definition for gH2OBdsCpBootDeviceRegisterLegacyBootOptionGuid checkpoint
typedef struct _H2O_BDS_CP_BOOT_DEVICE_REGISTER_LEGACY_BOOT_OPTION_DATA {
  IN     UINT32                     Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  IN OUT UINT32                     Status;       /// Enumerated value that specifies the kernel behavior.
  IN     BBS_TABLE                  *BbsTable;    /// Pointer to BBS table of current legacy boot option.
  IN OUT H2O_BDS_LOAD_OPTION        *BootOption;
} H2O_BDS_CP_BOOT_DEVICE_REGISTER_LEGACY_BOOT_OPTION_DATA;

// @Prompt Structure definition for gH2OBdsCpBootDeviceRegisterEfiBootOptionGuid checkpoint
typedef struct _H2O_BDS_CP_BOOT_DEVICE_REGISTER_EFI_BOOT_OPTION_DATA {
  IN     UINT32                     Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  IN OUT UINT32                     Status;       /// Enumerated value that specifies the kernel behavior.
  IN OUT H2O_BDS_LOAD_OPTION        *BootOption;
} H2O_BDS_CP_BOOT_DEVICE_REGISTER_EFI_BOOT_OPTION_DATA;

// @Prompt Structure definition for gH2OBdsCpBootDeviceEnumAfterGuid checkpoint
typedef struct _H2O_BDS_CP_BOOT_DEVICE_ENUM_AFTER_DATA {
  IN UINT32                         Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  IN UINT32                         Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_BOOT_DEVICE_ENUM_AFTER_DATA;

#define  H2O_BDS_CP_RECOVERY_COMPLETE_DO_NOTHING                0x00
#define  H2O_BDS_CP_RECOVERY_COMPLETE_SHUTDOWN                  0x01
#define  H2O_BDS_CP_RECOVERY_COMPLETE_REBOOT                    0x02
#define  H2O_BDS_CP_RECOVERY_COMPLETE_WARM_RESET                0x03
#define  H2O_BDS_CP_RECOVERY_COMPLETE_PLATFORM_SPECIFIC_RESET   0x04


// @Prompt Structure definition for gH2OBdsCpRecoveryCompleteGuid checkpoint
typedef struct _H2O_BDS_CP_RECOVERY_COMPLETE_DATA {
  UINT32          Size;             /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;           /// Enumerated value that specifies the kernel behavior.
  UINT8           RequestAction;    /// Request action from Flash tool.
  UINT8           *ResetData;
  UINTN           ResetDataSize;
} H2O_BDS_CP_RECOVERY_COMPLETE_DATA;

// @Prompt Structure definition for gH2OBdsCpSysPrepGuid checkpoint
typedef struct _H2O_BDS_CP_SYSPREP_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
  LIST_ENTRY      *SysPrepList; /// Pointer to a pointer to the first load option in the System Preparation load option list.
} H2O_BDS_CP_SYSPREP_DATA;

// @Prompt Structure definition for gH2OBdsCpOsRecoveryGuid checkpoint
typedef struct _H2O_BDS_CP_OS_RECOVERY_DATA {
  UINT32          Size;              /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;            /// Enumerated value that specifies the kernel behavior.
  LIST_ENTRY      *OsRecoveryList;   /// Pointer to a pointer to the first load option in the OS Recovery load option list.
} H2O_BDS_CP_OS_RECOVERY_DATA;

// @Prompt Structure definition for gH2OBdsCpPlatformRecoveryGuid checkpoint
typedef struct _H2O_BDS_CP_PLATFORM_RECOVERY_DATA {
  UINT32          Size;                   /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;                 /// Enumerated value that specifies the kernel behavior.
  LIST_ENTRY      *PlatformRecoveryList;  /// Pointer to a pointer to the first load option in the Platform Recovery load option list.
} H2O_BDS_CP_PLATFORM_RECOVERY_DATA;

// @Prompt Structure definition for gH2OBdsCpBootDeviceSelectGuid checkpoint
typedef struct _H2O_BDS_CP_BOOT_DEVICE_SELECT_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
  LIST_ENTRY      **BootList;   /// Pointer to a pointer to the first load option in the Boot load option list.
} H2O_BDS_CP_BOOT_DEVICE_SELECT_DATA;

// @Prompt Structure definition for gH2OBdsCpSendFormBeforeGuid checkpoint
typedef struct _H2O_BDS_CP_SEND_FORM_BEFORE_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
  EFI_GUID        *FormSetGuid; /// points to the EFI_GUID which match the Guid field or one of the elements of the ClassId field in
                                /// the EFI_IFR_FORM_SET op-code. If FormsetGuid is NULL, then this function will display the form set
                                /// class EFI_HII_PLATFORM_SETUP_FORMSET_GUID.
  UINT16          FormId;       /// Specifies the identifier of the form
} H2O_BDS_CP_SEND_FORM_BEFORE_DATA;

// @Prompt Structure definition for gH2OBdsCpFormBrowserCallbackBeforeGuid checkpoint
typedef struct _H2O_BDS_CP_FORM_BROWSER_CALLBACK_BEFORE_DATA {
  IN     UINT32                     Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  IN OUT UINT32                     Status;       /// Enumerated value that specifies the kernel behavior.

  IN     EFI_BROWSER_ACTION         Action;
  IN     EFI_QUESTION_ID            QuestionId;
  IN     UINT8                      Type;
  IN OUT EFI_IFR_TYPE_VALUE         *Value;
  OUT    EFI_BROWSER_ACTION_REQUEST ActionRequest;
  IN OUT EFI_STATUS                 CallbackStatus;

  IN EFI_GUID                       FormsetGuid;
  IN EFI_FORM_ID                    FormId;
} H2O_BDS_CP_FORM_BROWSER_CALLBACK_BEFORE_DATA;

// @Prompt Structure definition for gH2OBdsCpFormBrowserCallbackAfterGuid checkpoint
typedef struct _H2O_BDS_CP_FORM_BROWSER_CALLBACK_AFTER_DATA {
  IN     UINT32                     Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  IN OUT UINT32                     Status;       /// Enumerated value that specifies the kernel behavior.

  IN     EFI_BROWSER_ACTION         Action;
  IN     EFI_QUESTION_ID            QuestionId;
  IN     UINT8                      Type;
  IN OUT EFI_IFR_TYPE_VALUE         *Value;
  IN OUT EFI_BROWSER_ACTION_REQUEST ActionRequest;
  IN OUT EFI_STATUS                 CallbackStatus;

  IN     EFI_GUID                   FormsetGuid;
  IN     EFI_FORM_ID                FormId;
} H2O_BDS_CP_FORM_BROWSER_CALLBACK_AFTER_DATA;

// @Prompt Structure definition for gH2OBdsCpFormBrowserExtractConfigBeforeGuid checkpoint
typedef struct _H2O_BDS_CP_FORM_BROWSER_EXTRACT_CONFIG_BEFORE_DATA {
  IN     UINT32                     Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  IN OUT UINT32                     Status;       /// Enumerated value that specifies the kernel behavior.

  IN     CONST CHAR16               *Request;
  IN OUT CHAR16                     *Progress;
  IN OUT CHAR16                     *Results;
  IN OUT EFI_STATUS                 CallbackStatus;
} H2O_BDS_CP_FORM_BROWSER_EXTRACT_CONFIG_BEFORE_DATA;

// @Prompt Structure definition for gH2OBdsCpFormBrowserExtractConfigAfterGuid checkpoint
typedef struct _H2O_BDS_CP_FORM_BROWSER_EXTRACT_CONFIG_AFTER_DATA {
  IN     UINT32                     Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  IN OUT UINT32                     Status;       /// Enumerated value that specifies the kernel behavior.

  IN     CONST CHAR16               *Request;
  IN OUT CHAR16                     *Progress;
  IN OUT CHAR16                     *Results;
  IN OUT EFI_STATUS                 CallbackStatus;
} H2O_BDS_CP_FORM_BROWSER_EXTRACT_CONFIG_AFTER_DATA;

// @Prompt Structure definition for gH2OBdsCpFormBrowserRouteConfigBeforeGuid checkpoint
typedef struct _H2O_BDS_CP_FORM_BROWSER_ROUTE_CONFIG_BEFORE_DATA {
  IN     UINT32                     Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  IN OUT UINT32                     Status;       /// Enumerated value that specifies the kernel behavior.

  IN OUT CHAR16                     *Configuration;
  IN OUT CHAR16                     *Progress;
  IN OUT EFI_STATUS                 CallbackStatus;
} H2O_BDS_CP_FORM_BROWSER_ROUTE_CONFIG_BEFORE_DATA;

// @Prompt Structure definition for gH2OBdsCpFormBrowserRouteConfigAfterGuid checkpoint
typedef struct _H2O_BDS_CP_FORM_BROWSER_ROUTE_CONFIG_AFTER_DATA {
  IN     UINT32                     Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  IN OUT UINT32                     Status;       /// Enumerated value that specifies the kernel behavior.

  IN     CONST CHAR16               *Configuration;
  IN OUT CHAR16                     *Progress;
  IN OUT EFI_STATUS                 CallbackStatus;
} H2O_BDS_CP_FORM_BROWSER_ROUTE_CONFIG_AFTER_DATA;

// @Prompt Structure definition for gH2OBdsCpFormBrowserIdleBeforeGuid checkpoint
typedef struct _H2O_BDS_CP_FORM_BROWSER_IDLE_BEFORE_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_FORM_BROWSER_IDLE_BEFORE_DATA;

// @Prompt Structure definition for gH2OBdsCpFormBrowserIdleAfterGuid checkpoint
typedef struct _H2O_BDS_CP_FORM_BROWSER_IDLE_AFTER_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_FORM_BROWSER_IDLE_AFTER_DATA;

// @Prompt Structure definition for gH2OBdsCpFormBrowserUserInputGuid checkpoint
typedef struct _H2O_BDS_CP_FORM_BROWSER_USER_INPUT_DATA {
  UINT32                            Size;           /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32                            Status;         /// Enumerated value that specifies the kernel behavior.

  UINT32                            UserInputType;  /// Enumerated value that specifies the user input type.
  EFI_KEY_DATA                      KeyData;        /// Key data is about to be sent to display engine.
  EFI_SIMPLE_POINTER_STATE          SimplePtrState; /// Simple pointer data is about to be sent to display engine.
  EFI_ABSOLUTE_POINTER_STATE        AbsPtrState;    /// Absolute pointer data is about to be sent to display engine.
} H2O_BDS_CP_FORM_BROWSER_USER_INPUT_DATA;

#define H2O_BDS_CP_FORM_BROWSER_USER_INPUT_KEYPRESS           0x00000001
#define H2O_BDS_CP_FORM_BROWSER_USER_INPUT_SIMPLE_POINTER     0x00000002
#define H2O_BDS_CP_FORM_BROWSER_USER_INPUT_ABSOLUTE_POINTER   0x00000003

// @Prompt Structure definition for gH2OBdsCpFormBrowserCheckPasswordGuid checkpoint
typedef struct _H2O_BDS_CP_FORM_BROWSER_CHECK_PASSWORD_DATA {
  IN       UINT32                   Size;                   /// Unsigned integer that specifies the size of this structure, in bytes.
  IN OUT   UINT32                   Status;                 /// Enumerated value that specifies the kernel behavior.

  IN       EFI_GUID                 FormsetGuid;            /// FormsetGuid of password question.
  IN       EFI_QUESTION_ID          QuestionId;             /// QuestionId of password question.
  IN CONST CHAR16                   *PasswordStr;           /// Password string
  IN OUT   EFI_STATUS               PasswordStrCheckStatus; /// Password string check status
} H2O_BDS_CP_FORM_BROWSER_CHECK_PASSWORD_DATA;

// @Prompt Structure definition for gH2OBdsCpFormBrowserUpdatePasswordDialogMessageGuid checkpoint
typedef struct _H2O_BDS_CP_FORM_BROWSER_UPDATE_PASSWORD_DIALOG_MESSAGE_DATA {
  IN     UINT32                     Size;                   /// Unsigned integer that specifies the size of this structure, in bytes.
  IN OUT UINT32                     Status;                 /// Enumerated value that specifies the kernel behavior.

  IN     EFI_STATUS                 PasswordStrCheckStatus; /// Password string check status
  IN OUT CHAR16                     *TitleStr;              /// Dialog title string
  IN OUT UINT32                     ButtonCount;            /// Dialog button count
  IN OUT UINT32                     BodyCount;              /// Dialog body string count
  IN OUT CHAR16                     **BodyStrArray;         /// Dialog body string array
} H2O_BDS_CP_FORM_BROWSER_UPDATE_PASSWORD_DIALOG_MESSAGE_DATA;

// @Prompt Structure definition for gH2OBdsCpSendFormAfterGuid checkpoint
typedef struct _H2O_BDS_CP_SEND_FORM_AFTER_DATA {
  UINT32          Size;           /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;         /// Enumerated value that specifies the kernel behavior.
  EFI_GUID        *FormSetGuid;   /// points to the EFI_GUID which match the Guid field or one of the elements of the ClassId field in
                                  /// the EFI_IFR_FORM_SET op-code. If FormsetGuid is NULL, then this function will display the form set
                                  /// class EFI_HII_PLATFORM_SETUP_FORMSET_GUID.
  UINT16          FormId;         /// Specifies the identifier of the form
  UINTN           ActionRequest;  /// The action recommended by the form.
  EFI_STATUS      ReturnStatus;   /// The return status from SendForm function.
} H2O_BDS_CP_SEND_FORM_AFTER_DATA;

// @Prompt Structure definition for gH2OBdsCpNoBootDeviceGuid checkpoint
typedef struct _H2O_BDS_CP_NO_BOOT_DEVICE_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_NO_BOOT_DEVICE_DATA;

// @Prompt Structure definition for gH2OBdsCpReadyToBootBeforeGuid checkpoint
typedef struct _H2O_BDS_CP_READY_TO_BOOT_BEFORE_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_READY_TO_BOOT_BEFORE_DATA;

// @Prompt Structure definition for gH2OBdsCpUpdateAcpiDescHdrGuid checkpoint
typedef struct _H2O_BDS_CP_UPDATE_ACPI_DESC_HDR_DATA {
  UINT32                       Size;             /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32                       Status;           /// Enumerated value that specifies the kernel behavior.
  EFI_ACPI_SDT_HEADER          *AcpiTableHeader; /// Pointer to the ACPI table header.
} H2O_BDS_CP_UPDATE_ACPI_DESC_HDR_DATA;

// @Prompt Structure definition for gH2OBdsCpUpdateAcpiFacsGuid checkpoint
typedef struct _H2O_BDS_CP_UPDATE_ACPI_FACS_DATA {
  UINT32                                        Size;      /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32                                        Status;    /// Enumerated value that specifies the kernel behavior.
  EFI_ACPI_1_0_FIRMWARE_ACPI_CONTROL_STRUCTURE  *AcpiFacs; /// Pointer to the ACPI FACS table.
} H2O_BDS_CP_UPDATE_ACPI_FACS_DATA;

// @Prompt Structure definition for gH2OBdsCpUpdateAcpiSdevGuid checkpoint
typedef struct _H2O_BDS_CP_UPDATE_ACPI_SDEV_DATA {
  UINT32                                Size;                     /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32                                Status;                   /// Enumerated value that specifies the kernel behavior.
  EFI_ACPI_6_2_SDEV_STRUCTURE_HEADER    *SdevStructureHeader;     /// Pointer to the Secure Device Structures header.
                                                                  /// Checkpoint handlers must check the SdevStructureSize to know
                                                                  /// the current size of secure device structures.
                                                                  /// If the buffer size is not enough to update secure device
                                                                  /// structures or the SdevStructureSize is zero, or the
                                                                  /// SdevStructureHeader is NULL, checkpoint handlers need to
                                                                  /// allocate new buffer of type EfiBootServicesData before
                                                                  /// updating the secure device structures. If the original
                                                                  /// buffer size is not zero, and original buffer is not NULL,
                                                                  /// checkpoint handlers need to free the original buffer by
                                                                  /// FreePool, and checkpoint handlers need update new allocate
                                                                  /// buffer size to SdevStructureSize and the new buffer pointer
                                                                  /// to SdevStructureHeader in the structure.
  UINT32                                SdevStructureSize;        /// The size of the buffer pointed to by Secure Device Structures, in bytes.
                                                                  /// The checkpoint handlers must update this.
} H2O_BDS_CP_UPDATE_ACPI_SDEV_DATA;

// @Prompt Structure definition for gH2OBdsCpReadyToBootAfterGuid checkpoint
typedef struct _H2O_BDS_CP_READY_TO_BOOT_AFTER_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_READY_TO_BOOT_AFTER_DATA;

// @Prompt Structure definition for gH2OBdsCpCapsuleUpdateProgressGuid checkpoint
typedef struct _H2O_BDS_CP_CAPSULE_UPDATE_PROGRESS_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
  EFI_GUID        FwClass;      /// The firmware class field contains a GUID that identifies a firmware component that can be updated.
  UINTN           Completion;   /// A value between 1 and 100 indicating the current completion progress of the firmware update.
} H2O_BDS_CP_CAPSULE_UPDATE_PROGRESS_DATA;


typedef struct _H2O_MEMORY_TYPE_INFORMATION {
  IN     UINT32  Type;                    ///< EFI memory type defined in UEFI specification.
  IN     UINT32  NumberOfPages;           ///< The pages of this type memory defined in memory information table.
  IN     UINT32  UsedNumberOfPages;       ///< The pages of this type memory really used by system.
  IN OUT UINT32  UpdatedNumberOfPages;    ///< The pages of this type memory will be updated.
} H2O_MEMORY_TYPE_INFORMATION;

// @Prompt Structure definition for gH2OBdsCpUpdateMemoryTypeInfoGuid checkpoint
typedef struct _H2O_BDS_CP_UPDATE_MEMORY_TYPE_INFO_DATA {
         UINT32                        Size;                 /// Unsigned integer that specifies the size of this structure, in bytes.
         UINT32                        Status;               /// Enumerated value that specifies the kernel behavior.
  IN     UINT32                        MemoryTypeInfoCount;  /// Unsigned integer that specifies the number of entries in the memory type information table.
  IN OUT H2O_MEMORY_TYPE_INFORMATION   *MemoryTypeInfoTable; /// Pointer to the memory type information table.
} H2O_BDS_CP_UPDATE_MEMORY_TYPE_INFO_DATA;

// @Prompt Structure definition for gH2OBdsCpExitBootServicesBeforeGuid checkpoint
typedef struct _H2O_BDS_CP_EXIT_BOOT_SERVICES_BEFORE_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_EXIT_BOOT_SERVICES_BEFORE_DATA;

// @Prompt Structure definition for gH2OBdsCpBootBeforeGuid checkpoint
typedef struct _H2O_BDS_CP_BOOT_BEFORE_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_BOOT_BEFORE_DATA;

// @Prompt Structure definition for gH2OBdsCpCapsuleCheckUpdateCapabilityGuid checkpoint
typedef struct _H2O_BDS_CP_CAPSULE_CHECK_UPDATE_CAPABILITY_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
  VOID            *Capsule;     /// Pointer to capsule data.
  UINTN           CapsuleSize;  /// Unsigned integer that specifies the size of capsule data, in bytes.
  UINT32          ErrorBits;    /// Bitmask that specifies which capsule check is failed.
} H2O_BDS_CP_CAPSULE_CHECK_UPDATE_CAPABILITY_DATA;

#define H2O_BDS_CP_CAPSULE_CHECK_ERROR_UNSUCCESSFUL           0x00000001
#define H2O_BDS_CP_CAPSULE_CHECK_ERROR_INSUFFICIENT_RESOURCES 0x00000002
#define H2O_BDS_CP_CAPSULE_CHECK_ERROR_INCORRECT_VERSION      0x00000004
#define H2O_BDS_CP_CAPSULE_CHECK_ERROR_INVALID_IMAGE_FORMAT   0x00000008
#define H2O_BDS_CP_CAPSULE_CHECK_ERROR_AUTHENTICATION         0x00000010
#define H2O_BDS_CP_CAPSULE_CHECK_ERROR_AC_NOT_CONNECTED       0x00000020
#define H2O_BDS_CP_CAPSULE_CHECK_ERROR_INSUFFICIENT_BATTERY   0x00000040

// @Prompt Structure definition for gH2OBdsCpBootAfterGuid checkpoint
typedef struct _H2O_BDS_CP_BOOT_AFTER_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_BOOT_AFTER_DATA;

// @Prompt Structure definition for gH2OBdsCpLegacyPrepareToBootBeforeGuid checkpoint
typedef struct _H2O_BDS_CP_LEGACY_PREPARE_TO_BOOT_BEFORE_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_LEGACY_PREPARE_TO_BOOT_BEFORE_DATA;

// @Prompt Structure definition for gH2OBdsCpLegacyPrepareToBootAfterGuid checkpoint
typedef struct _H2O_BDS_CP_LEGACY_PREPARE_TO_BOOT_AFTER_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_LEGACY_PREPARE_TO_BOOT_AFTER_DATA;

// @Prompt Structure definition for gH2OBdsCpLegacyBootBeforeGuid checkpoint
typedef struct _H2O_BDS_CP_LEGACY_BOOT_BEFORE_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_LEGACY_BOOT_BEFORE_DATA;

// @Prompt Structure definition for gH2OBdsCpLegacyBootAfterGuid checkpoint
typedef struct _H2O_BDS_CP_LEGACY_BOOT_AFTER_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_LEGACY_BOOT_AFTER_DATA;

// @Prompt Structure definition for gH2OBdsCpBootSuccessGuid checkpoint
typedef struct _H2O_BDS_CP_BOOT_SUCCESS_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
} H2O_BDS_CP_BOOT_SUCCESS_DATA;

// @Prompt Structure definition for gH2OBdsCpBootFailedGuid checkpoint
typedef struct _H2O_BDS_CP_BOOT_FAILED_DATA {
  UINT32          Size;         /// Unsigned integer that specifies the size of this structure, in bytes.
  UINT32          Status;       /// Enumerated value that specifies the kernel behavior.
  EFI_STATUS      ReturnStatus; /// Status returned from failed boot.
  CHAR16          *ExitData;    /// Pointer to exit data returned from the failed boot.
  UINTN           ExitDataSize; /// Pointer to size of exit data returned from failed boot.
} H2O_BDS_CP_BOOT_FAILED_DATA;

// @Prompt Structure definition for gH2OBdsCpEnterIncorrectSetupPasswordGuid checkpoint
typedef struct _H2O_BDS_CP_ENTER_INCORRECT_SETUP_PASSWORD_DATA {
          UINT32          Size;                            /// Unsigned integer that specifies the size of this structure, in bytes.
          UINT32          Status;                          /// Enumerated value that specifies the kernel behavior.
  IN OUT  UINTN           PasswordRetryCount;              /// Password retry counts.
  IN      EFI_STATUS      CheckPasswordStatus;             /// Status return from check password.
} H2O_BDS_CP_ENTER_INCORRECT_SETUP_PASSWORD_DATA;

typedef enum {
  PolicyColdReset              = EfiResetCold,                   /// 0
  PolicyWarmReset              = EfiResetWarm,                   /// 1
  PolicyShutdown               = EfiResetShutdown,               /// 2
  PolicyPlatformSpecificReset  = EfiResetPlatformSpecific,       /// 3
  PolicyNoAction,
  PolicyMaximum,
} CHECK_SETUP_PASSWORD_FAIL_POLICY;

// @Prompt Structure definition for gH2OBdsCpCheckSetupPasswordFailGuid checkpoint
typedef struct _H2O_BDS_CP_CHECK_SETUP_PASSWORD_FAIL_DATA {
          UINT32                                   Size;                            /// Unsigned integer that specifies the size of this structure, in bytes.
          UINT32                                   Status;                          /// Enumerated value that specifies the kernel behavior.
  IN OUT  CHECK_SETUP_PASSWORD_FAIL_POLICY         FailPolicy;                      /// Fail policy.
  IN OUT  UINT64                                   FreezeTime;                      /// The unit for freeze time is second.
  IN OUT  UINTN                                    PasswordRetryCount;              /// Password retry counts.
} H2O_BDS_CP_CHECK_SETUP_PASSWORD_FAIL_DATA;

//
// BASE CP GUID definitions
//
extern EFI_GUID gH2OBaseCpPostCodeGuid;
extern EFI_GUID gH2OBaseCpVerifyFvGuid;

//
// PEI CP GUID definitions
//
extern EFI_GUID gH2OPeiCpForceVariableDefaultsGuid;
extern EFI_GUID gH2OPeiCpInitPlatformStage1Guid;
extern EFI_GUID gH2OPeiCpSetBootModeBeforeGuid;
extern EFI_GUID gH2OPeiCpInitPlatformStage2Guid;
extern EFI_GUID gH2OPeiCpCrisisRecoveryPublishFvGuid;
extern EFI_GUID gH2OPeiCpCrisisRecoveryGetProtectTableGuid;
extern EFI_GUID gH2OPeiCpCrisisRecoveryFlashGuid;
extern EFI_GUID gH2OPeiCpEndOfPeiBeforeGuid;
extern EFI_GUID gH2OPeiCpEndOfPeiAfterGuid;


//
// DXE CP GUID definitions
//
extern EFI_GUID gH2ODxeCpWriteVariableDefaultsGuid;
extern EFI_GUID gH2ODxeCpPciCheckSysOptionRomGuid;
extern EFI_GUID gH2ODxeCpUpdateSmbiosRecord;
extern EFI_GUID gH2ODxeCpResetSystemGuid;
extern EFI_GUID gH2ODxeCpPciHpcGetBridgeInfoGuid;
extern EFI_GUID gH2ODxeCpPciHpcGetResourcePaddingGuid;
extern EFI_GUID gH2ODxeCpUpdateAcpiEsrtTpmFwVersionGuid;

//
// SMM CP GUID definitions
//
extern EFI_GUID gH2OSmmCpIhisiAuthStatusGuid;
extern EFI_GUID gH2OSmmCpIhisiAuthCheckGuid;

//
// BDS CP GUID definitions
//
extern EFI_GUID gH2OBdsCpInitGuid;
extern EFI_GUID gH2OBdsCpEndOfDxeBeforeGuid;
extern EFI_GUID gH2OBdsCpEndOfDxeAfterGuid;
extern EFI_GUID gH2OBdsCpPlatformInitGuid;
extern EFI_GUID gH2OBdsCpDriverConnectBeforeGuid;
extern EFI_GUID gH2OBdsCpDriverConnectAfterGuid;
extern EFI_GUID gH2OBdsCpPciEnumBeginGuid;
extern EFI_GUID gH2OBdsCpPciEnumUpdateDevInfoGuid;
extern EFI_GUID gH2OBdsCpPciEnumUpdateDevResourcesGuid;
extern EFI_GUID gH2OBdsCpPciEnumCompleteBeforeGuid;
extern EFI_GUID gH2OBdsCpPciEnumCompleteAfterGuid;
extern EFI_GUID gH2OBdsCpConInConnectBeforeGuid;
extern EFI_GUID gH2OBdsCpConInConnectAfterGuid;
extern EFI_GUID gH2OBdsCpConOutConnectBeforeGuid;
extern EFI_GUID gH2OBdsCpConOutConnectAfterGuid;
extern EFI_GUID gH2OBdsCpHotKeyDetectedGuid;
extern EFI_GUID gH2OBdsCpDxeSmmReadyToLockBeforeGuid;
extern EFI_GUID gH2OBdsCpDxeSmmReadyToLockAfterGuid;
extern EFI_GUID gH2OBdsCpDisplayBeforeGuid;
extern EFI_GUID gH2OBdsCpDisplayStringBeforeGuid;
extern EFI_GUID gH2OBdsCpConnectAllBeforeGuid;
extern EFI_GUID gH2OBdsCpNetworkUpdateMacAddrGuid;
extern EFI_GUID gH2OBdsCpConnectAllAfterGuid;
extern EFI_GUID gH2OBdsCpBootDeviceEnumBeforeGuid;
extern EFI_GUID gH2OBdsCpBootDeviceRegisterLegacyBootOptionGuid;
extern EFI_GUID gH2OBdsCpBootDeviceEnumCheckBootOptionGuid;
extern EFI_GUID gH2OBdsCpBootDeviceRegisterEfiBootOptionGuid;
extern EFI_GUID gH2OBdsCpBootDeviceEnumAfterGuid;
extern EFI_GUID gH2OBdsCpRecoveryCompleteGuid;
extern EFI_GUID gH2OBdsCpSysPrepGuid;
extern EFI_GUID gH2OBdsCpOsRecoveryGuid;
extern EFI_GUID gH2OBdsCpPlatformRecoveryGuid;
extern EFI_GUID gH2OBdsCpBootDeviceSelectGuid;
extern EFI_GUID gH2OBdsCpSendFormBeforeGuid;
extern EFI_GUID gH2OBdsCpFormBrowserCallbackBeforeGuid;
extern EFI_GUID gH2OBdsCpFormBrowserCallbackAfterGuid;
extern EFI_GUID gH2OBdsCpFormBrowserExtractConfigBeforeGuid;
extern EFI_GUID gH2OBdsCpFormBrowserExtractConfigAfterGuid;
extern EFI_GUID gH2OBdsCpFormBrowserRouteConfigBeforeGuid;
extern EFI_GUID gH2OBdsCpFormBrowserRouteConfigAfterGuid;
extern EFI_GUID gH2OBdsCpFormBrowserIdleBeforeGuid;
extern EFI_GUID gH2OBdsCpFormBrowserIdleAfterGuid;
extern EFI_GUID gH2OBdsCpFormBrowserUserInputGuid;
extern EFI_GUID gH2OBdsCpFormBrowserCheckPasswordGuid;
extern EFI_GUID gH2OBdsCpFormBrowserUpdatePasswordDialogMessageGuid;
extern EFI_GUID gH2OBdsCpSendFormAfterGuid;
extern EFI_GUID gH2OBdsCpNoBootDeviceGuid;
extern EFI_GUID gH2OBdsCpReadyToBootBeforeGuid;
extern EFI_GUID gH2OBdsCpUpdateAcpiDescHdrGuid;
extern EFI_GUID gH2OBdsCpUpdateAcpiFacsGuid;
extern EFI_GUID gH2OBdsCpUpdateAcpiSdevGuid;
extern EFI_GUID gH2OBdsCpReadyToBootAfterGuid;
extern EFI_GUID gH2OBdsCpCapsuleUpdateProgressGuid;
extern EFI_GUID gH2OBdsCpUpdateMemoryTypeInfoGuid;
extern EFI_GUID gH2OBdsCpBootBeforeGuid;
extern EFI_GUID gH2OBdsCpCapsuleCheckUpdateCapabilityGuid;
extern EFI_GUID gH2OBdsCpBootAfterGuid;
extern EFI_GUID gH2OBdsCpExitBootServicesBeforeGuid;
extern EFI_GUID gH2OBdsCpLegacyPrepareToBootBeforeGuid;
extern EFI_GUID gH2OBdsCpLegacyPrepareToBootAfterGuid;
extern EFI_GUID gH2OBdsCpLegacyBootBeforeGuid;
extern EFI_GUID gH2OBdsCpLegacyBootAfterGuid;
extern EFI_GUID gH2OBdsCpBootSuccessGuid;
extern EFI_GUID gH2OBdsCpBootFailedGuid;
extern EFI_GUID gH2OBdsCpEnterIncorrectSetupPasswordGuid;
extern EFI_GUID gH2OBdsCpCheckSetupPasswordFailGuid;

#endif
