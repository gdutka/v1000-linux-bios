/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

 #ifndef __CBS_BCT_SMM_PROTOCOL_H__
#define __CBS_BCT_SMM_PROTOCOL_H__

#define CBS_BCT_SMM_PROTOCOL_GUID \
  { \
    0x147f8bda, 0xda41, 0x45bb, {0xbc, 0x9b, 0x82, 0x7e, 0x72, 0xdc, 0x30, 0xa1} \
  }

typedef struct _CBS_BCT_SMM_PROTOCOL  CBS_BCT_SMM_PROTOCOL;

 EFI_STATUS
 EFIAPI
 InstallCbsBctSmmProtocol();

typedef EFI_STATUS (EFIAPI *CBS_BCT_SET_VARIABLE) (
  IN CBS_BCT_SMM_PROTOCOL    *This,
  IN UINT8               *IfrData
);

typedef EFI_STATUS (EFIAPI *CBS_BCT_WRITE_FLASH) ();

struct _CBS_BCT_SMM_PROTOCOL {
  CBS_BCT_SET_VARIABLE        CbsBctSetVariable;
  CBS_BCT_WRITE_FLASH         CbsBctWriteFlash;
};

extern EFI_GUID gCbsBctSmmProtocolGuid;

#endif // __CBS_BCT_SMM_PROTOCOL_H__
