/** @file

;******************************************************************************
;* Copyright (c) 2014 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _COMMON_DEFINITION_H_
#define _COMMON_DEFINITION_H_

#include <Protocol/H2OSecurityArch.h>

#define OPTION_ROM_LIST_OPTION_SIGNATURE       SIGNATURE_32 ('O', 'P', 'R', 'O')
#define SECURE_OPROM_CONTROL_PRIVATE_SIGNATURE SIGNATURE_32 ('S', 'O', 'P', 'C')

//
// Define the ROM type which contained in ROM image
//
//
// Define supported ROM type policy
//
#define UNSUPPORT_ALL_OPTION_ROM        0x00
#define SUPPORT_LEGACY_OPTION_ROM       0x01
#define SUPPORT_UNSIGNED_EFI_OPTION_ROM 0x02
#define SUPPORT_SIGNED_EFI_OPTION_ROM   0x03
#define SUPPORT_ALL_OPTION_ROM          0x04
#define INVALID_SUPPORT_OPTION_ROM      0x05

#define OP_ROM_ACTIVE                   0x01
#define OP_ROM_INACTIVE                 0x02

#define EFI_LIST_ENTRY                  LIST_ENTRY

#define OPTION_ROM_POLICY_AUTO          0x0
#define OPTION_ROM_POLICY_NONE          0x1
#define OPTION_ROM_POLICY_LEGACY        0x2
#define OPTION_ROM_POLICY_UNSIGNED_EFI  0x3
#define OPTION_ROM_POLICY_SIGNED_EFI    0x4
#define OPTION_ROM_POLICY_ALL           0x5

//
// Define the ROM tyep which contained in ROM image
//
#define HAVE_LEGACY_ROM                 0x01
#define HAVE_UNSIGNED_EFI_ROM           0x02
#define HAVE_SIGNED_EFI_ROM             0x04 

#define OPROM_CONTROL_HASH_SIZE         32

#pragma pack(push, 1)

typedef struct _OPTION_ROM_OPTION {
  UINT8           Attribute;
  UINT8           FoundRomType;
  UINT8           OpRomPolicy;
  UINT16          VendorId;
  UINT16          DeviceId;
  UINT8           HashValue[OPROM_CONTROL_HASH_SIZE];
  UINTN           DevicePathSize;
  UINT8           DevicePath[1];
} OPTION_ROM_OPTION;  
#pragma pack(pop)

typedef struct _OPTION_ROM_LIST_OPTION {
  UINT32                   Signature;
  EFI_LIST_ENTRY           Link;
  UINT16                   OptionNumber;
  OPTION_ROM_OPTION        OpRomOption;
} OPTION_ROM_LIST_OPTION; 

/**
  Validate the content of the option ROM option content from variable.

  @param[in]   *OpRomOption       - The point of option ROM option from OpRomXXXX variable.

  @retval EFI_SUCCESS             - The content is valid.
  @retval EFI_UNSUPPORTED         - The content is invalid.

**/
EFI_STATUS
EFIAPI
ValidateOpRomVariable (
  IN OPTION_ROM_OPTION            *OpRomOption
  );

/**
  Check if the device path is from PCI slot device.
  
  @param[in]  DevicePath        Pointer to the Device Path protocol of the option ROM.

  @retval     TRUE              The input device path is for PCI slot device.
              FALSE             The input device path is not for PCI slot device.

**/
BOOLEAN
EFIAPI
IsSupportedPciSlotDevicePath (
  IN CONST  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
  );

/**
  Set variables "OpRomXXXX" and "OpRomOrder" to describe the each OpRom entry.

  @param  VOID

  @retval EFI_SUCCESS            Function has completed successfully.
  @return Other                  Error occurred during execution.

**/
EFI_STATUS
EFIAPI
SetOpRomPolicies (
  VOID
  );

/**
  Get gH2ODeviceInfoTokenSpaceGuid PCD list and add to Rom list.

  @param[in] RomOptionList        - Pointer to the Option Rom list.
  @param[in] CurrentOptionRomNum  - Current Option Rom number.

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
GetDefaultOptionRomPolicyList (
  IN EFI_LIST_ENTRY      *RomOptionList,
  IN UINT16              *CurrentOptionRomNum
  );

#endif
