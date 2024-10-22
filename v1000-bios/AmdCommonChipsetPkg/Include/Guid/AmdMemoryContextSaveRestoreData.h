/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _AMD_MEMORY_CONTEXT_SAVE_RESTORE_DATA_H_
#define _AMD_MEMORY_CONTEXT_SAVE_RESTORE_DATA_H_

#define AMD_MEMORY_CONTEXT_SAVE_RESTORE_DATA_GUID \
  {0x68299f11, 0xf7f2, 0x11e1, 0xaf, 0xa2, 0x2c, 0x41, 0x38, 0x0a, 0x0b, 0xaa}

#define AMD_MEMORY_CONTEXT_SAVE_RESTORE_VARIABLE_GUID \
  {0x96b3d61e, 0xf7f2, 0x11e1, 0xb7, 0x97, 0x2c, 0x41, 0x38, 0x0a, 0x0b, 0xaa}
  
#define AMD_MEMORY_CONTEXT_DATA_EXIST_VARIABLE_NAME  L"AmdMemContextExist"
#define AMD_MEMORY_CONTEXT_LAST_TRAINING_TIME_NAME  L"LastTrainingTime"

typedef struct _AMD_MEMORY_CONTEXT_SAVE_RESTORE_DATA {
  BOOLEAN  IsSystemCfgGood;
  BOOLEAN  IsWarmBoot;
  BOOLEAN  IsS4Resume;
  BOOLEAN  IsPostPass;
  BOOLEAN  IsSetupNoChange;
  BOOLEAN  HasMemContext;
  BOOLEAN  DoPeriodicRetraining;
  BOOLEAN  DoMemContextSaveRestore;
} AMD_MEMORY_CONTEXT_SAVE_RESTORE_DATA;

extern EFI_GUID gAmdMemoryContextSaveRestoreDataGuid;
extern EFI_GUID gAmdMemContextSaveRestoreVariableGuid;

#endif