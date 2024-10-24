/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/*++

Module Name:

  NonVolatileVariable.h

Abstract:

  This protocol will be installed when the non-volatile service initialized successfully.

--*/

#ifndef _NON_VOLATILE_VARIABLE_PROTOCOL_H_
#define _NON_VOLATILE_VARIABLE_PROTOCOL_H_

#define EFI_NON_VOLATILE_VARIABLE_PROTOCOL_GUID  \
  {0x173f9091, 0x44b6, 0x43be, 0x9d, 0x65, 0x98, 0x94, 0x7b, 0xd9, 0xb9, 0xd7}

#define EFI_READ_ONLY_VARIABLE_PROTOCOL_GUID  \
  {0x257a9418, 0xcb21, 0x4b4b, 0xb3, 0x92, 0xc9, 0xdc, 0x32, 0xc2, 0x75, 0x94}

#define EFI_EMU_NON_VOLATILE_VARIABLE_PROTOCOL_GUID  \
  {0x2b3820fb, 0x0f77, 0x4bec, 0x96, 0xe4, 0xcb, 0x26, 0xc0, 0xe9, 0xf5, 0x53}

typedef struct _NON_VOLATILE_VARIABLE_PROTOCOL NON_VOLATILE_VARIABLE_PROTOCOL;

/**
  Function to provide runtime type memory to store variable data which can be used to
  communicate with variable code in SMM.

  @param[out]    Buffer             Pointer to the return runtime type memory.
  @param[out]    BufferSize         The size of the return buffer.

  @retval  EFI_SUCCESS              Get runtime type memory successfully.
  @retval  EFI_NOT_FOUND            Cannot find runtime type memory.  
  @retval  EFI_INVALID_PARAMETER    Buffer is NULL or BufferSize is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *GET_RUNTIME_VARIABLE_BUFFER) (
  OUT    VOID        **Buffer,
  OUT     UINTN      *BufferSize
  );

struct _NON_VOLATILE_VARIABLE_PROTOCOL {
  GET_RUNTIME_VARIABLE_BUFFER              GetRuntimeVariableBuffer;
};

extern EFI_GUID gEfiNonVolatileVariableProtocolGuid;
extern EFI_GUID gEfiReadOnlyVariableProtocolGuid;
extern EFI_GUID gEfiEmuNonVolatileVariableProtocolGuid;

#endif
