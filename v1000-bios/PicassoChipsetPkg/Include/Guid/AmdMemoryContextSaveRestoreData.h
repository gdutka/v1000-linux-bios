/** @file
  The definition of AmdMemoryContextSaveRestoreDataGuid.

;*******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#ifndef _AMD_MEMORY_CONTEXT_SAVE_RESTORE_DATA_H_
#define _AMD_MEMORY_CONTEXT_SAVE_RESTORE_DATA_H_

#define AMD_MEMORY_CONTEXT_SAVE_RESTORE_DATA_GUID \
  { \
    0x966eb6fa, 0xeb61, 0x4f18, 0x87, 0x47, 0xe8, 0x27, 0x2e, 0xef, 0xa5, 0xbd \
  }

//
// Forward reference for pure ANSI compatability.
//
typedef struct _AMD_MEMORY_CONTEXT_SAVE_RESTORE_DATA  AMD_MEMORY_CONTEXT_SAVE_RESTORE_DATA;

struct _AMD_MEMORY_CONTEXT_SAVE_RESTORE_DATA {
  BOOLEAN  IsSystemCfgGood;
  BOOLEAN  IsWarmBoot;
  BOOLEAN  IsS4Resume;
  BOOLEAN  IsPostPass;
  BOOLEAN  IsSetupNoChange;
  BOOLEAN  HasMemContext;
  BOOLEAN  DoPeriodicRetraining;
  BOOLEAN  DoMemContextSaveRestore;
};

extern EFI_GUID gAmdMemoryContextSaveRestoreDataGuid;

#endif
