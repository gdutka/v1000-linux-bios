//;******************************************************************************
//;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//; 
//; Module Name:
//; 
//;   AmdMemContextSaveRestoreProtocol.h
//; 
//; Abstract:
//; 
//;   Protocol definition for AMD_MEM_CONTEXT_SAVE_RESTORE_PROTOCOL.
//; 

#ifndef _AMD_MEM_CONTEXT_SAVE_RESTORE_PROTOCOL_H_
#define _AMD_MEM_CONTEXT_SAVE_RESTORE_PROTOCOL_H_
//
// Forward reference for ANSI C compatibility
//
typedef struct _AMD_MEM_CONTEXT_SAVE_RESTORE_PROTOCOL AMD_MEM_CONTEXT_SAVE_RESTORE_PROTOCOL;

//
// Global ID for the Sx SMI Protocol
//
#define AMD_MEM_CONTEXT_SAVE_RESTORE_PROTOCOL_GUID  \
  { 0xbd3bb9de, 0x6c03, 0x4609, 0xbd, 0xa9, 0x86, 0x1c, 0xc4, 0x48, 0x83, 0x01 }

//
// UPDATE_MEM_CONTEXT_SAVE_RESTORE_CMOS_FLAG
//
typedef
EFI_STATUS
(EFIAPI *UPDATE_MEM_CONTEXT_SAVE_RESTORE_CMOS_FLAG) (
  IN  UINT8   Flag
  );

typedef
BOOLEAN 
(EFIAPI *RESTORE_VOLATILE_MEM_ADDRESS) (
  UINTN    *BufferPtrValue
  );

typedef struct _AMD_MEM_CONTEXT_SAVE_RESTORE_PROTOCOL {
  UPDATE_MEM_CONTEXT_SAVE_RESTORE_CMOS_FLAG      UpdateMemContextSaveRestoreCmosFlag;
  RESTORE_VOLATILE_MEM_ADDRESS                   RestoreVolatileMemAddress;
};

extern EFI_GUID gAmdMemContextSaveRestoreProtocolGuid;

#endif