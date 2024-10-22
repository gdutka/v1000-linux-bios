/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include "AmdPspKvmMouse.h"

KVM_MOUSE_DEV           mKvmMouseDev;
KVM_MOUSE_BUFFER        *mMRingBuffer = NULL;

BOOLEAN
IsKvmMouseBufEmpty (
  IN  KVM_MOUSE_BUFFER        *MRingBuffer
  )
{
  return (BOOLEAN) (MRingBuffer->WritePointer == MRingBuffer->ReadPointer);
}

EFI_STATUS
EFIAPI
InitializeMsRingBuffer (
  IN OUT KVM_MOUSE_BUFFER           **MRingBuffer
  )
{
  *MRingBuffer = mMRingBuffer;
  return EFI_SUCCESS;
}

VOID
EFIAPI
KvmGetNextPointer(
  IN OUT UINT32          *Pointer
  )
{
  *Pointer = ((*Pointer + 1) == KVM_MOUSE_MAX_COUNT) ? 0 : *Pointer + 1;
}
