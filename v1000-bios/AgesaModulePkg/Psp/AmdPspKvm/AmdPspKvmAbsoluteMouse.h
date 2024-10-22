/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _AMD_PSP_KVM_ABSOLUTE_MOUSE_H_
#define _AMD_PSP_KVM_ABSOLUTE_MOUSE_H_

#include "AmdPspKvmMouse.h"

#define KVM_MOUSE_DEV_FROM_THIS(a)  CR (a, KVM_MOUSE_DEV, AbsolutePointerProtocol, KVM_MOUSE_DEV_SIGNATURE)

EFI_STATUS
EFIAPI
KvmAbsoluteMouseReset (
  IN EFI_ABSOLUTE_POINTER_PROTOCOL    *This,
  IN BOOLEAN                        ExtendedVerification
  );

EFI_STATUS
EFIAPI
KvmAbsoluteMouseGetState (
  IN EFI_ABSOLUTE_POINTER_PROTOCOL    *This,
  IN OUT EFI_ABSOLUTE_POINTER_STATE   *State
  );

VOID
EFIAPI
KvmAbsoluteMouseWaitForInput (
  IN  EFI_EVENT               Event,
  IN  VOID                    *Context
  );

VOID
EFIAPI
KvmAbsoluteReadMouseState (
  IN KVM_MOUSE_DEV       *MouseDev
  );

extern KVM_MOUSE_DEV mKvmMouseDev;
extern KVM_MOUSE_BUFFER *mMRingBuffer;

#endif

