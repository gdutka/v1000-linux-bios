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

#ifndef _AGESA_MEMORY_SAVE_DATA_PROTOCOL_H_
#define _AGESA_MEMORY_SAVE_DATA_PROTOCOL_H_

typedef struct _AGESA_MEMORY_SAVE_DATA_PROTOCOL AGESA_MEMORY_SAVE_DATA_PROTOCOL;

#define AGESA_MEMORY_SAVE_DATA_PROTOCOL_GUID \
  { 0x155523FE, 0x54EB, 0x4BC9, 0xB9, 0x17, 0xB5, 0xAB, 0x73, 0x56, 0x5C, 0x64 }

extern EFI_GUID gAgesaMemorySaveDataProtocolGuid;

typedef
EFI_STATUS
(EFIAPI *DO_AGESA_MEMORY_SAVE_DATA) (
  IN       UINT8                        S3DataType,
  IN       UINTN                        DataSize,
  IN       VOID                         *Data
  );

typedef struct _AGESA_MEMORY_SAVE_DATA_PROTOCOL {
  DO_AGESA_MEMORY_SAVE_DATA      DoAgesaMemorySaveData;
};

#endif
