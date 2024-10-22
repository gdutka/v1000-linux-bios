/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _AMD_PSP_KVM_SIMPLE_MOUSE_H_
#define _AMD_PSP_KVM_SIMPLE_MOUSE_H_

#include "AmdPspKvmMouse.h"


#define KVM_MOUSE_DEV_FROM_THIS(a)  CR (a, KVM_MOUSE_DEV, SimplePointerProtocol, KVM_MOUSE_DEV_SIGNATURE)

EFI_STATUS
EFIAPI
KvmSimpleMouseReset (
  IN EFI_SIMPLE_POINTER_PROTOCOL    *This,
  IN BOOLEAN                        ExtendedVerification
  );

EFI_STATUS
EFIAPI
KvmSimpleMouseGetState (
  IN EFI_SIMPLE_POINTER_PROTOCOL    *This,
  IN OUT EFI_SIMPLE_POINTER_STATE   *State
  );

VOID
EFIAPI
KvmSimpleMouseWaitForInput (
  IN  EFI_EVENT               Event,
  IN  VOID                    *Context
  );

VOID
EFIAPI
KvmSimpleReadMouseState (
  IN KVM_MOUSE_DEV       *MouseDev
  );

extern KVM_MOUSE_DEV mKvmMouseDev;
extern KVM_MOUSE_BUFFER *mMRingBuffer;

#endif

