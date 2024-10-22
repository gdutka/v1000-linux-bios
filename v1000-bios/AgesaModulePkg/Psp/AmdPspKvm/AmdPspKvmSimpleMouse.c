/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include "AmdPspKvmSimpleMouse.h"

STATIC BOOLEAN mSimpleFirstTimeReset = TRUE;

EFI_STATUS
EFIAPI
KvmToEfiSimplePointerState (
  IN KVM_POINTER_STATE              *KvmPointerState,
  OUT EFI_SIMPLE_POINTER_STATE      *EfiPointerState
)
{
  EfiPointerState->RelativeMovementX = KvmPointerState->MovementX;
  EfiPointerState->RelativeMovementY = KvmPointerState->MovementY;
  EfiPointerState->RelativeMovementZ = 0;
  EfiPointerState->LeftButton = (KvmPointerState->ButtonMask & 0x1) == 0x0 ? FALSE : TRUE;
  EfiPointerState->RightButton = (KvmPointerState->ButtonMask & 0x4) == 0x0 ? FALSE : TRUE;
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
InitializeMsRingBufferTestPattern (
  OUT KVM_MOUSE_BUFFER           *MRingBuffer
  )
{
  UINT32 i = 0;
  MRingBuffer->Data[i++].MovementX = 0x8000;
  MRingBuffer->Data[i++].MovementY = 0x8000;
  MRingBuffer->WritePointer = i;
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
KvmSimpleMouseDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                            Status;
  KVM_MOUSE_DEV                         *MouseDev;
  EFI_TPL                               OldTpl;

  Status = EFI_SUCCESS;
  IDS_HDT_CONSOLE_PSP_TRACE ("KvmMouseDriverStart >>\n");

  OldTpl = gBS->RaiseTPL (TPL_NOTIFY);
  MouseDev = &mKvmMouseDev;
  ZeroMem (MouseDev, sizeof (KVM_MOUSE_DEV));

  MouseDev->Signature       = KVM_MOUSE_DEV_SIGNATURE;
  MouseDev->Handle          = Controller;
  MouseDev->DataPackageSize = 3;

  MouseDev->SimpleMode.ResolutionX                = 4;
  MouseDev->SimpleMode.ResolutionY                = 4;
  MouseDev->SimpleMode.ResolutionZ                = 0;
  MouseDev->SimpleMode.LeftButton                 = TRUE;
  MouseDev->SimpleMode.RightButton                = TRUE;  
  
  MouseDev->SimplePointerProtocol.Reset     = KvmSimpleMouseReset;
  MouseDev->SimplePointerProtocol.GetState  = KvmSimpleMouseGetState;
  MouseDev->SimplePointerProtocol.Mode      = &(MouseDev->SimpleMode);

  InitializeMsRingBuffer(&MouseDev->MRingBuffer);
  mMRingBuffer = MouseDev->MRingBuffer;
  // InitializeMsRingBufferTestPattern (MouseDev->MRingBuffer);
  IDS_HDT_CONSOLE_PSP_TRACE ("Kvm Mouse Buffer: Max Count %x Ptr %x\n", KVM_MOUSE_MAX_COUNT, MouseDev->MRingBuffer);

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_WAIT,
                  TPL_NOTIFY,
                  KvmSimpleMouseWaitForInput,
                  MouseDev,
                  &((MouseDev->SimplePointerProtocol).WaitForInput)
                  );
  if (EFI_ERROR (Status)) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("KvmSimpleMouseWaitForInput Event Created\n");

  MouseDev->ControllerNameTable = NULL;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Controller,
                  &gEfiSimplePointerProtocolGuid,
                  &MouseDev->SimplePointerProtocol,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("EFI_SIMPLE_POINTER_PROTOCOL Installed\n");

  // PcdSet64S (PcdKvmConInHandle, (UINT64) (UINTN) Controller);
  // gST->ConsoleInHandle = Controller;
  IDS_HDT_CONSOLE_PSP_TRACE ("KvmMouseDriverStart<<\n");
  gBS->RestoreTPL (OldTpl);

  return EFI_SUCCESS;

ErrorExit:
  IDS_HDT_CONSOLE_PSP_TRACE ("Kvm Mouse Driver Error<<\n");

  if ((MouseDev != NULL) && (MouseDev->SimplePointerProtocol.WaitForInput != NULL)) {
    gBS->CloseEvent (MouseDev->SimplePointerProtocol.WaitForInput);
  }

  if ((MouseDev != NULL) && (MouseDev->TimerEvent != NULL)) {
    gBS->CloseEvent (MouseDev->TimerEvent);
  }

  if ((MouseDev != NULL) && (MouseDev->ControllerNameTable != NULL)) {
    FreeUnicodeStringTable (MouseDev->ControllerNameTable);
  }

  if (MouseDev != NULL) {
    FreePool (MouseDev);
  }
  gBS->RestoreTPL (OldTpl);

  return Status;
}

EFI_STATUS
EFIAPI
KvmSimpleMouseDriverStop (
  IN EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN EFI_HANDLE                    Controller,
  IN UINTN                         NumberOfChildren,
  IN EFI_HANDLE                    *ChildHandleBuffer
  )
{
  EFI_STATUS                  Status;
  EFI_SIMPLE_POINTER_PROTOCOL *SimplePointerProtocol;
  KVM_MOUSE_DEV               *MouseDev;

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiSimplePointerProtocolGuid,
                  (VOID **) &SimplePointerProtocol,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  MouseDev = KVM_MOUSE_DEV_FROM_THIS (SimplePointerProtocol);

  Status = gBS->UninstallProtocolInterface (
                  Controller,
                  &gEfiSimplePointerProtocolGuid,
                  &MouseDev->SimplePointerProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  gBS->CloseEvent (MouseDev->SimplePointerProtocol.WaitForInput);
  FreeUnicodeStringTable (MouseDev->ControllerNameTable);
  FreePool (MouseDev);

  gBS->CloseProtocol (
         Controller,
         &gEfiPciIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
KvmSimpleMouseReset (
  IN EFI_SIMPLE_POINTER_PROTOCOL    *This,
  IN BOOLEAN                        ExtendedVerification
  )
{
  // TODO
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
KvmSimpleMouseGetState (
  IN EFI_SIMPLE_POINTER_PROTOCOL    *This,
  IN OUT EFI_SIMPLE_POINTER_STATE   *State
  )
{
  KVM_MOUSE_DEV                     *MouseDev;
  EFI_STATUS                        Status;
  EFI_TPL                           OldTpl;

  Status = EFI_SUCCESS;
  MouseDev = KVM_MOUSE_DEV_FROM_THIS(This);

  if (mSimpleFirstTimeReset) {
    IDS_HDT_CONSOLE_PSP_TRACE ("First time reset Mouse State\n");
    Status = MouseDev->SimplePointerProtocol.Reset (
                         &MouseDev->SimplePointerProtocol,
                         TRUE
                         );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    mSimpleFirstTimeReset = FALSE;
  }

  OldTpl = gBS->RaiseTPL (TPL_NOTIFY);
  KvmSimpleReadMouseState(MouseDev);
  if (!MouseDev->StateChanged) {
    gBS->RestoreTPL (OldTpl);
    return EFI_NOT_READY;
  }
  CopyMem (State, &(MouseDev->SimpleState), sizeof (EFI_SIMPLE_POINTER_STATE));
  //
  // clear mouse state
  //
  MouseDev->SimpleState.RelativeMovementX = 0;
  MouseDev->SimpleState.RelativeMovementY = 0;
  MouseDev->SimpleState.RelativeMovementZ = 0;
  MouseDev->StateChanged            = FALSE;
  gBS->RestoreTPL (OldTpl);
  
  return EFI_SUCCESS;
}

VOID
EFIAPI
KvmSimpleMouseWaitForInput (
  IN  EFI_EVENT               Event,
  IN  VOID                    *Context
  )
{
  EFI_TPL       OldTpl;
  KVM_MOUSE_DEV *MouseDev;

  MouseDev = (KVM_MOUSE_DEV *) Context;
  OldTpl = gBS->RaiseTPL (TPL_NOTIFY);

  if (!IsKvmMouseBufEmpty(MouseDev->MRingBuffer)) {
    gBS->SignalEvent (Event);
  }
  gBS->RestoreTPL (OldTpl);
}

VOID
EFIAPI
KvmSimpleReadMouseState (
  IN KVM_MOUSE_DEV       *MouseDev
  )
{
  if (IsKvmMouseBufEmpty(MouseDev->MRingBuffer) || MouseDev->StateChanged) {
    return;
  }
  KvmToEfiSimplePointerState(&(MouseDev->MRingBuffer->Data[MouseDev->MRingBuffer->ReadPointer]),&(MouseDev->SimpleState));
  KvmGetNextPointer (&(MouseDev->MRingBuffer->ReadPointer));
  MouseDev->StateChanged = TRUE;
}

