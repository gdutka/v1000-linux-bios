/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include "AmdPspKvm.h"

extern EFI_GUID gEfiConsoleInDeviceGuid;

STATIC KVM_KEYBOARD_CONSOLE_IN_DEV mKvmKbConInDev;
KB_RING_BUFFER *mKbRingBuffer = NULL;


EFI_STATUS
KbdFreeNotifyList (
  IN OUT LIST_ENTRY           *ListHead
  )
{
  KVM_KEYBOARD_CONSOLE_IN_EX_NOTIFY *NotifyNode;

  if (ListHead == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  while (!IsListEmpty (ListHead)) {
    NotifyNode = CR (
                   ListHead->ForwardLink,
                   KVM_KEYBOARD_CONSOLE_IN_EX_NOTIFY,
                   NotifyEntry,
                   KVM_KEYBOARD_CONSOLE_IN_EX_NOTIFY_SIGNATURE
                   );
    RemoveEntryList (ListHead->ForwardLink);
    gBS->FreePool (NotifyNode);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
InitializeKbdRingBuffer (
  OUT KB_RING_BUFFER            **KbBufferPtr
  )
{
  *KbBufferPtr = mKbRingBuffer;
  return EFI_SUCCESS;
}

// Hello world
EFI_STATUS
EFIAPI
InitializeKbdRingBufferTestPattern (
  OUT KB_RING_BUFFER            *KbBuffer
  )
{
  UINT32 i = 0;
  // KbBuffer->Data[i++].KeySym = 0x48;
  // KbBuffer->Data[i++].KeySym = 0x48;
  // KbBuffer->Data[i++].KeySym = 0x65;
  // KbBuffer->Data[i++].KeySym = 0x6c;
  // KbBuffer->Data[i++].KeySym = 0x6c;
  // KbBuffer->Data[i++].KeySym = 0x6F;
  // KbBuffer->Data[i++].KeySym = 0x20;
  // KbBuffer->Data[i++].KeySym = 0x57;
  // KbBuffer->Data[i++].KeySym = 0x6F;
  // KbBuffer->Data[i++].KeySym = 0x72;
  // KbBuffer->Data[i++].KeySym = 0x6c;
  // KbBuffer->Data[i++].KeySym = 0x64;
  // KbBuffer->Data[i++].KeySym = 0x21;
  // KbBuffer->Data[i++].KeySym = KVM_ENTER;
  KbBuffer->Data[i++].KeySym = 0x61;
  KbBuffer->Data[i++].KeySym = 0x65;
  KbBuffer->Data[i++].KeySym = 0x78;
  KbBuffer->Data[i++].KeySym = 0x69;
  KbBuffer->Data[i++].KeySym = 0x74;
  KbBuffer->Data[i++].KeySym = KVM_ENTER;
  KbBuffer->Data[i++].KeySym = KVM_DOWN;
  KbBuffer->Data[i++].KeySym = KVM_DOWN;
  KbBuffer->WritePointer = i;
  return EFI_SUCCESS;
}
// EFI_STATUS
// EFIAPI
// InitializeRingBufferTestPattern (
//   OUT KB_RING_BUFFER            *KbBuffer
//   )
// {
//   KbBuffer->WritePointer = 6;
//   KbBuffer->Data[0].KeySym = 0x61;
//   KbBuffer->Data[1].KeySym = 0x65;
//   KbBuffer->Data[2].KeySym = 0x78;
//   KbBuffer->Data[3].KeySym = 0x69;
//   KbBuffer->Data[4].KeySym = 0x74;
//   KbBuffer->Data[5].KeySym = KVM_ENTER;
//   return EFI_SUCCESS;
// }

EFI_STATUS
EFIAPI
KvmKbdControllerDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                                Status;
  EFI_STATUS_CODE_VALUE                     StatusCode;
  KVM_KEYBOARD_CONSOLE_IN_DEV               *ConsoleIn;
  EFI_TPL                                   OldTpl;

  IDS_HDT_CONSOLE_PSP_TRACE ("KvmKbdControllerDriverStart\n");
  OldTpl = gBS->RaiseTPL (TPL_NOTIFY);
  //
  // Allocate private data
  //
  ConsoleIn = &mKvmKbConInDev;
  ZeroMem (ConsoleIn, sizeof (KVM_KEYBOARD_CONSOLE_IN_DEV));
  //
  // Setup the device instance
  //
  ConsoleIn->Signature              = KVM_KEYBOARD_CONSOLE_IN_DEV_SIGNATURE;
  ConsoleIn->Handle                 = Controller;
  ConsoleIn->KeyboardErr            = FALSE;
  (ConsoleIn->ConIn).Reset          = KvmKeyboardEfiReset;
  (ConsoleIn->ConIn).ReadKeyStroke  = KvmKeyboardReadKeyStroke;


  ConsoleIn->ConInEx.Reset               = KvmKeyboardEfiResetEx;
  ConsoleIn->ConInEx.ReadKeyStrokeEx     = KvmKeyboardReadKeyStrokeEx;
  ConsoleIn->ConInEx.SetState            = KvmKeyboardSetState;
  ConsoleIn->ConInEx.RegisterKeyNotify   = KvmKeyboardRegisterKeyNotify;
  ConsoleIn->ConInEx.UnregisterKeyNotify = KvmKeyboardUnregisterKeyNotify;
  ConsoleIn->KvmKeyState.KeyShiftState   = 0x0;
  ConsoleIn->KvmKeyState.KeyToggleState  = 0x0;

  InitializeKbdRingBuffer(&ConsoleIn->KvmKeyQueue.KbBuffer);
  mKbRingBuffer = ConsoleIn->KvmKeyQueue.KbBuffer;
  // InitializeKbdRingBufferTestPattern (ConsoleIn->KvmKeyQueue.KbBuffer);
  InitializeListHead (&ConsoleIn->NotifyList);
  IDS_HDT_CONSOLE_PSP_TRACE ("KbBuffer %x\n", ConsoleIn->KvmKeyQueue.KbBuffer);

  //
  // Setup the WaitForKey event
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_WAIT,
                  TPL_NOTIFY,
                  KvmKeyboardWaitForKey,
                  ConsoleIn,
                  &((ConsoleIn->ConIn).WaitForKey)
                  );
  if (EFI_ERROR (Status)) {
    Status      = EFI_OUT_OF_RESOURCES;
    StatusCode  = EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_CONTROLLER_ERROR;
    goto ErrorExit;
  }
  //
  // Setup the WaitForKeyEx event
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_WAIT,
                  TPL_NOTIFY,
                  KvmKeyboardWaitForKeyEx,
                  ConsoleIn,
                  &((ConsoleIn->ConInEx).WaitForKeyEx)
                  );
  if (EFI_ERROR (Status)) {
    Status      = EFI_OUT_OF_RESOURCES;
    StatusCode  = EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_CONTROLLER_ERROR;
    goto ErrorExit;
  }
  //
  // Install protocol interfaces for the keyboard device.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Controller,
                  &gEfiSimpleTextInProtocolGuid,
                  &ConsoleIn->ConIn,
                  &gEfiSimpleTextInputExProtocolGuid,
                  &ConsoleIn->ConInEx,
                  &gEfiConsoleInDeviceGuid,
                  NULL,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    StatusCode = EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_CONTROLLER_ERROR;
    goto ErrorExit;
  }

  // PcdSet64S (PcdKvmConIn, (UINT64) (UINTN) &ConsoleIn->ConIn);
  // gST->ConIn = &(ConsoleIn->ConIn);
  // gST->ConsoleInHandle = Controller;
  gBS->RestoreTPL (OldTpl);
  return Status;

ErrorExit:
  //
  // Report error code
  //

  if ((ConsoleIn != NULL) && (ConsoleIn->ConIn.WaitForKey != NULL)) {
    gBS->CloseEvent (ConsoleIn->ConIn.WaitForKey);
  }
  if ((ConsoleIn != NULL) && (ConsoleIn->ConInEx.WaitForKeyEx != NULL)) {
    gBS->CloseEvent (ConsoleIn->ConInEx.WaitForKeyEx);
  }
  KbdFreeNotifyList (&ConsoleIn->NotifyList);

  if (ConsoleIn != NULL) {
    gBS->FreePool (ConsoleIn);
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("InitializeKvmKeyboard ERROR <<\n");
  gBS->RestoreTPL (OldTpl);
  return Status;
}

EFI_STATUS
EFIAPI
KvmKbdControllerDriverStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN  EFI_HANDLE                     Controller,
  IN  UINTN                          NumberOfChildren,
  IN  EFI_HANDLE                     *ChildHandleBuffer
  )
{
  EFI_STATUS                     Status;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ConIn;
  KVM_KEYBOARD_CONSOLE_IN_DEV    *ConsoleIn;

  //
  // Disable Keyboard
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiSimpleTextInProtocolGuid,
                  (VOID **) &ConIn,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiSimpleTextInputExProtocolGuid,
                  NULL,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ConsoleIn = KVM_KEYBOARD_CONSOLE_IN_DEV_FROM_THIS (ConIn);
  Status = gBS->FreePool(ConsoleIn->KvmKeyQueue.KbBuffer);
  Status = gBS->UninstallMultipleProtocolInterfaces (
                  Controller,
                  &gEfiSimpleTextInProtocolGuid,
                  &ConsoleIn->ConIn,
                  &gEfiSimpleTextInputExProtocolGuid,
                  &ConsoleIn->ConInEx,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if ((ConsoleIn->ConIn).WaitForKey != NULL) {
    gBS->CloseEvent ((ConsoleIn->ConIn).WaitForKey);
    (ConsoleIn->ConIn).WaitForKey = NULL;
  }
  if (ConsoleIn->ConInEx.WaitForKeyEx != NULL) {
    gBS->CloseEvent (ConsoleIn->ConInEx.WaitForKeyEx);
    ConsoleIn->ConInEx.WaitForKeyEx = NULL;
  }
  KbdFreeNotifyList (&ConsoleIn->NotifyList);
  gBS->FreePool (ConsoleIn);

  return EFI_SUCCESS;
}

