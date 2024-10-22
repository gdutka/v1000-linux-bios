/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _CBS_SETUP_PROTOCOL_H_
#define _CBS_SETUP_PROTOCOL_H_

#define CBS_SETUP_PROTOCOL_GUID \
  { \
    0x5083736, 0xbcab, 0x4474, 0x8d, 0xb8, 0xfe, 0xea, 0x0, 0x1c, 0xe, 0x53 \
  }

typedef struct _CBS_SETUP_PROTOCOL  CBS_SETUP_PROTOCOL;

/// Function prototype
typedef EFI_STATUS (EFIAPI *CBS_SETUP_LOADDEFAULT) (
  IN  CBS_SETUP_PROTOCOL  *This
);

///CBS SETUP Protocol Structure
struct _CBS_SETUP_PROTOCOL {
  CBS_SETUP_LOADDEFAULT         CbsSetupLoadDefault;
};

extern EFI_GUID gCbsSetupProtocolGuid;

#endif // _CBS_SETUP_PROTOCOL_H_
