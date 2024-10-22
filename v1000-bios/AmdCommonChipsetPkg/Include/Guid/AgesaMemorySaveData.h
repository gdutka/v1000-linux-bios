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

#ifndef _AGESA_MEMORY_SAVE_DATA_GUID_H_
#define _AGESA_MEMORY_SAVE_DATA_GUID_H_

#define AGESA_MEMORY_SAVE_DATA_GUID \
  { \
    0x575E7FC3, 0xE067, 0x47D1, 0x83, 0x41, 0x7B, 0x8F, 0x1C, 0x28, 0xB0, 0x5 \
  }

#define AgesaMemorySaveDataVariable L"AgesaMemorySaveData"

extern EFI_GUID gAgesaMemorySaveDataGuid;

typedef struct {
  UINT64  NvSaveDataPtr;
  UINT64  NvSaveDataSize;
  UINT64  SaveDataPtr;
  UINT64  SaveDataSize;
} AGESA_MEMORY_SAVE_DATA;

#endif
