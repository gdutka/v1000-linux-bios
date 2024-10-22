/* $NoKeywords:$ */
/**
 * @file
 *
 * AmdPspKvmMouse.h
 *
 * Contains definition for AmdPspKvmMouse
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _AMD_PSP_KVM_MOUSE_H_
#define _AMD_PSP_KVM_MOUSE_H_

#include <Protocol/SimplePointer.h>
#include <Protocol/AbsolutePointer.h>
#include <Protocol/PciIo.h>
#include <Protocol/DevicePath.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/TimerLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/PciLib.h>
#include <Library/DebugLib.h>


#include <AGESA.h>
#include <Filecode.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdPspBaseLibV2.h>
#include <Library/AmdPspMboxLibV2.h>
#include <Protocol/AmdPspKvmServiceProtocol.h>

#define KVM_LEFT_CLICK_MASK           0x1;
#define KVM_MIDDLE_CLICK_MASK         0x2;
#define KVM_RIGHT_CLICK_MASK          0x4;
#define KVM_SCROLL_UP_MASK            0x8;
#define KVM_SCROLL_DOWN_MASK          0x10;

// #define KVM_POINTER_STATE           EFI_SIMPLE_POINTER_STATE
#define KVM_MOUSE_BUFFER_SIZE         (32 * 1024) // 32KB Keyboard Buffer
#define KVM_MOUSE_DATA_BUFFER_SIZE    (KVM_MOUSE_BUFFER_SIZE - 2 * sizeof(UINT32))
#define KVM_MOUSE_MAX_COUNT           KVM_MOUSE_DATA_BUFFER_SIZE / sizeof(KVM_POINTER_STATE)

typedef struct {
  UINT32                              ButtonMask; //1=Left click, 2=Middle/Scroll click, 4=Right click, 8=Scroll up, 16=Scroll down
  UINT32                              MovementX;
  UINT32                              MovementY;
} KVM_POINTER_STATE;


typedef struct {
  UINT32                              WritePointer;
  UINT32                              ReadPointer;
  KVM_POINTER_STATE                   Data[KVM_MOUSE_MAX_COUNT];
} KVM_MOUSE_BUFFER;

#define KVM_MOUSE_DEV_SIGNATURE     SIGNATURE_32 ('k', 'v', 'm', 'm')

typedef struct {
  UINTN                               Signature;

  EFI_HANDLE                          Handle;
  EFI_SIMPLE_POINTER_PROTOCOL         SimplePointerProtocol;
  EFI_SIMPLE_POINTER_STATE            SimpleState;
  EFI_SIMPLE_POINTER_MODE             SimpleMode;

  EFI_ABSOLUTE_POINTER_PROTOCOL       AbsolutePointerProtocol;
  EFI_ABSOLUTE_POINTER_STATE          AbsoluteState;
  EFI_ABSOLUTE_POINTER_MODE           AbsoluteMode;

  BOOLEAN                             StateChanged;

  UINT8                               DataPackageSize;
  KVM_MOUSE_BUFFER                    *MRingBuffer;

  EFI_EVENT                           TimerEvent;

  EFI_UNICODE_STRING_TABLE            *ControllerNameTable;
} KVM_MOUSE_DEV;


KVM_MOUSE_DEV           mKvmMouseDev;
KVM_MOUSE_BUFFER        *mMRingBuffer;

EFI_STATUS
EFIAPI
KvmAbsoluteMouseDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  );

EFI_STATUS
EFIAPI
KvmAbsoluteMouseDriverStop (
  IN EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN EFI_HANDLE                    Controller,
  IN UINTN                         NumberOfChildren,
  IN EFI_HANDLE                    *ChildHandleBuffer
  );

EFI_STATUS
EFIAPI
KvmSimpleMouseDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  );


EFI_STATUS
EFIAPI
KvmSimpleMouseDriverStop (
  IN EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN EFI_HANDLE                    Controller,
  IN UINTN                         NumberOfChildren,
  IN EFI_HANDLE                    *ChildHandleBuffer
  );

BOOLEAN
IsKvmMouseBufEmpty (
  IN  KVM_MOUSE_BUFFER        *MRingBuffer
  );

EFI_STATUS
EFIAPI
InitializeMsRingBuffer (
  IN OUT KVM_MOUSE_BUFFER           **MRingBuffer
  );

VOID
EFIAPI
KvmGetNextPointer(
  IN OUT UINT32          *Pointer
  );

#endif
