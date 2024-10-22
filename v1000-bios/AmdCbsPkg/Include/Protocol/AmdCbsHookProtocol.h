/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef __CBS_HOOK_DXE_H__
#define __CBS_HOOK_DXE_H__

#define CBS_PROTOCOL_GUID \
  { \
    0xecad4a0,  0x75e9, 0x46d7, 0xa3, 0xa6, 0x81, 0xfe, 0x17, 0x3f, 0x2e, 0xca \
  }

typedef struct _CBS_DXE_PROTOCOL  CBS_DXE_PROTOCOL;

/// Function prototype
typedef EFI_STATUS (EFIAPI *CBS_HOOK_INTERFACE) (
  IN     EFI_BOOT_SERVICES    *BootServices,
  IN OUT VOID                 *AMD_PARAMS,
  IN     UINTN                TimePoint
);

///DXE Protocol Structure
struct _CBS_DXE_PROTOCOL {
  CBS_HOOK_INTERFACE            CbsHookInterface;
  UINT64                        PpTableAddr;
};

extern EFI_GUID gCbsProtocolGuid;

#endif // __CBS_HOOK_DXE_H__
