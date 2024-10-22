/** @file
  Definition for bios region lock Protocol

;******************************************************************************
;* Copyright (c) 2015 - 2017, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _BIOS_REGION_LOCK_H_
#define _BIOS_REGION_LOCK_H_

typedef struct _BIOS_REGION_LOCK_PROTOCOL BIOS_REGION_LOCK_PROTOCOL;

// {E687E50B-C98D-4780-A7B0-E4C7C4AF7F69}
#define BIOS_REGION_LOCK_PROTOCOL_GUID \
 {0xe687e50b, 0xc98d, 0x4780, 0xa7, 0xb0, 0xe4, 0xc7, 0xc4, 0xaf, 0x7f, 0x69}

extern EFI_GUID gEfiBiosRegionLockProtocolGuid;

typedef enum {
  PROTECT_FVMAIN = 0,
  PROTECT_NV_COMMON_STORE,
  PROTECT_NV_COMMON_STORE_MICROCODE,
  PROTECT_NV_COMMON_STORE_OEM_DMI_STORE,
  PROTECT_NV_COMMON_STORE_RESERVED1,
  PROTECT_NV_COMMON_STORE_NV_BVDT,
  PROTECT_NVSTORAGE,
  PROTECT_NVSTORAGE_NV_FTW_WORKING,
  PROTECT_NVSTORAGE_NV_FTW_SPARE,
  PROTECT_FV_RECOVERY
} BIOS_REGION_TYPE;

typedef enum {
  BIOS_WRITE_PROTECT = 1,
  BIOS_READ_PROTECT  = 2,
  BIOS_READ_AND_WRITE_PROTECT = 3
} BIOS_PROTECT_TYPE;

typedef struct {
  UINT32                 Base;
  UINT32                 Size;
} BIOS_PROTECT_REGION;

typedef
EFI_STATUS
(EFIAPI *SET_REGION_BY_TYPE) (
  IN BIOS_REGION_LOCK_PROTOCOL  *This,
  IN BIOS_REGION_TYPE            Type
);

typedef
EFI_STATUS
(EFIAPI *SET_REGION_BY_ADDRESS) (
  IN BIOS_REGION_LOCK_PROTOCOL  *This,
  IN UINTN                       BaseAddress,
  IN UINTN                       Length
);

typedef
EFI_STATUS
(EFIAPI *CLEAR_REGION_BY_TYPE) (
  IN BIOS_REGION_LOCK_PROTOCOL  *This,
  IN BIOS_REGION_TYPE            Type
);

typedef
EFI_STATUS
(EFIAPI *CLEAR_REGION_BY_ADDRESS) (
  IN BIOS_REGION_LOCK_PROTOCOL  *This,
  IN UINTN                       BaseAddress,
  IN UINTN                       Length 
);

typedef
EFI_STATUS
(EFIAPI *LOCK) (
  IN BIOS_REGION_LOCK_PROTOCOL  *This
);

typedef struct _BIOS_REGION_LOCK_PROTOCOL {
  SET_REGION_BY_TYPE       SetRegionByType;
  SET_REGION_BY_ADDRESS    SetRegionByAddress;
  CLEAR_REGION_BY_TYPE     ClearRegionByType;
  CLEAR_REGION_BY_ADDRESS  ClearRegionByAddress;
  LOCK                     Lock;
} BIOS_REGION_LOCK_PROTOCOL;

#endif //_BIOS_REGION_LOCK_H_
