/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _AMD_GNBLATE_PROTOCOL_H_
#define _AMD_GNBLATE_PROTOCOL_H_

#define AMD_GNBLATE_PROTOCOL_REVISION   0x00

typedef struct _DXE_AMD_GNBLATE_INIT_COMPLETE_PROTOCOL DXE_AMD_GNBLATE_INIT_COMPLETE_PROTOCOL;

/// AMD GNBLATE DXE Initialization Complete Protocol
struct _DXE_AMD_GNBLATE_INIT_COMPLETE_PROTOCOL {
  UINTN    Revision;               ///< Revision Number
};

// Current Protocol revision

extern EFI_GUID gAmdGnbLateDxeInitCompleteProtocolGuid;

#endif // _AMD_GNBLATE_PROTOCOL_H_

