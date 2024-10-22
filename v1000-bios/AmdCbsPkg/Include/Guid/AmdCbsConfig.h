/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _CBS_CONFIG_H_
#define _CBS_CONFIG_H_

#define FORMSET_ID_GUID_AMD_CBS \
  { \
    0xB04535E3, 0x3004, 0x4946, {0x9E, 0xB7, 0x14, 0x94, 0x28, 0x98, 0x30, 0x53} \
  }

#define CBS_SYSTEM_CONFIGURATION_GUID \
  { \
    0x3A997502, 0x647A, 0x4c82, {0x99, 0x8E, 0x52, 0xEF, 0x94, 0x86, 0xA2, 0x47} \
  }

extern EFI_GUID gCbsSystemConfigurationGuid;

#define AMD_SYSTEM_CONFIGURATION_GUID  \
  { \
    0x3A997502, 0x647A, 0x4c82, {0x99, 0x8E, 0x52, 0xEF, 0x94, 0x86, 0xA2, 0x47} \
  }

//[-start-190704-IB06070039-add]//
#define AMD_CBS_FORMSET_CLASS_GUID \
  { \
    0x5ead73ce, 0xcc14, 0x4ff7, {0xaa, 0x31, 0xa6, 0x64, 0x93, 0xc6, 0x15, 0x3d} \
  }
//[-end-190704-IB06070039-add]//

#define CBS_SYSTEM_CONFIGURATION_NAME L"AmdSetup"

#endif // _CBS_CONFIG_H_

