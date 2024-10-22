/** @file
  The definition of AmdMemContextSaveRestoreVariableGuid.

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

#ifndef _AMD_MEMORY_CONTEXT_SAVE_RESTORE_VARIABLE_H_
#define _AMD_MEMORY_CONTEXT_SAVE_RESTORE_VARIABLE_H_

#define AMD_MEMORY_CONTEXT_SAVE_RESTORE_VARIABLE_GUID \
  { \
    0x52bc0af3, 0xbcb8, 0x4275, 0xbd, 0x96, 0x46, 0x6f, 0x55, 0xe4, 0x9d, 0x9c \
  }

#define AMD_MEMORY_CONTEXT_DATA_EXIST_VARIABLE_NAME  L"AmdMemContextExist"
#define AMD_MEMORY_CONTEXT_LAST_TRAINING_TIME_NAME   L"LastTrainingTime"

extern EFI_GUID gAmdMemContextSaveRestoreVariableGuid;

#endif
