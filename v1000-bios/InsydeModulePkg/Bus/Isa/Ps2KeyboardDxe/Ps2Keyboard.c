/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2021, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**

  PS/2 Keyboard driver. Routines that interacts with callers,
  conforming to EFI driver model

Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Ps2Keyboard.h"

STATIC BOOLEAN    mPS2KeyboardCheck = TRUE;
STATIC LIST_ENTRY mConsoleInDeviceList = INITIALIZE_LIST_HEAD_VARIABLE (mConsoleInDeviceList);
//
// Function prototypes
//
/**
  Test controller is a keyboard Controller.

  @param This                 Pointer of EFI_DRIVER_BINDING_PROTOCOL
  @param Controller           driver's controller
  @param RemainingDevicePath  children device path

  @retval EFI_UNSUPPORTED controller is not floppy disk
  @retval EFI_SUCCESS     controller is floppy disk
**/
EFI_STATUS
EFIAPI
KbdControllerDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  );

/**
  Create KEYBOARD_CONSOLE_IN_DEV instance on controller.

  @param This         Pointer of EFI_DRIVER_BINDING_PROTOCOL
  @param Controller   driver controller handle
  @param RemainingDevicePath Children's device path

  @retval whether success to create floppy control instance.
**/
EFI_STATUS
EFIAPI
KbdControllerDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  );

/**
  Stop this driver on ControllerHandle. Support stoping any child handles
  created by this driver.

  @param  This              Protocol instance pointer.
  @param  ControllerHandle  Handle of device to stop driver on
  @param  NumberOfChildren  Number of Handles in ChildHandleBuffer. If number of
                            children is zero stop the entire bus driver.
  @param  ChildHandleBuffer List of Child Handles to Stop.

  @retval EFI_SUCCESS       This driver is removed ControllerHandle
  @retval other             This driver was not removed from this device

**/
EFI_STATUS
EFIAPI
KbdControllerDriverStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN  EFI_HANDLE                     Controller,
  IN  UINTN                          NumberOfChildren,
  IN  EFI_HANDLE                     *ChildHandleBuffer
  );

/**
  Free the waiting key notify list.

  @param ListHead  Pointer to list head

  @retval EFI_INVALID_PARAMETER  ListHead is NULL
  @retval EFI_SUCCESS            Sucess to free NotifyList
**/
EFI_STATUS
KbdFreeNotifyList (
  IN OUT LIST_ENTRY           *ListHead
  );

//
// DriverBinding Protocol Instance
//
EFI_DRIVER_BINDING_PROTOCOL gKeyboardControllerDriver = {
  KbdControllerDriverSupported,
  KbdControllerDriverStart,
  KbdControllerDriverStop,
  0xa,
  NULL,
  NULL
};

/**
 Set PS2 KB timer event TPL to NOTIFY - 1 at boot before checkpoint and resume it back to NOTIFY at boot after checkpoint.
 It can prevent from key (read by this PS2 KB driver) occasionally lost in pre-OS application dialog which
 application's kerboard driver will read key from KBC in timer event at NOTIFY level.

 @param[in] Event                      The Event this notify function registered to.
 @param[in] Handle                     The handle associated with a previously registered checkpoint handler.
**/
STATIC
VOID
EFIAPI
KbdControllerBootCpHandler (
  IN EFI_EVENT                         Event,
  IN H2O_CP_HANDLE                     Handle
  )
{
  LIST_ENTRY                           *Link;
  KEYBOARD_CONSOLE_IN_DEV              *ConsoleIn;
  EFI_STATUS                           Status;
  EFI_TPL                              TimerNotifyTpl;
  VOID                                 *CpData;
  EFI_GUID                             CpGuid;

  Status = H2OCpLookup (Handle, &CpData, &CpGuid);
  if (!EFI_ERROR (Status) && CompareGuid (&CpGuid, &gH2OBdsCpBootBeforeGuid)) {
    TimerNotifyTpl = TPL_NOTIFY - 1;
  } else {
    TimerNotifyTpl = TPL_NOTIFY;
  }

  for (Link = GetFirstNode (&mConsoleInDeviceList); !IsNull (&mConsoleInDeviceList, Link); Link = GetNextNode (&mConsoleInDeviceList, Link)) {
    ConsoleIn = KEYBOARD_CONSOLE_IN_DEV_FROM_LINK (Link);
    if (ConsoleIn->TimerEvent == NULL) {
      continue;
    }
    gBS->CloseEvent (ConsoleIn->TimerEvent);
    ConsoleIn->TimerEvent = NULL;

    Status = gBS->CreateEvent (
                    EVT_TIMER | EVT_NOTIFY_SIGNAL,
                    TimerNotifyTpl,
                    KeyboardTimerHandler,
                    ConsoleIn,
                    &ConsoleIn->TimerEvent
                    );
    if (!EFI_ERROR (Status)) {
      gBS->SetTimer (ConsoleIn->TimerEvent, TimerPeriodic, KEYBOARD_TIMER_INTERVAL);
    }
  }
}

/**
  Test controller is a keyboard Controller.

  @param This                 Pointer of EFI_DRIVER_BINDING_PROTOCOL
  @param Controller           driver's controller
  @param RemainingDevicePath  children device path

  @retval EFI_UNSUPPORTED controller is not floppy disk
  @retval EFI_SUCCESS     controller is floppy disk
**/
EFI_STATUS
EFIAPI
KbdControllerDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                          Status;
  EFI_ISA_IO_PROTOCOL                 *IsaIo;
  EFI_LEGACY_BIOS_PROTOCOL            *LegacyBios;
  UINT8                               Data;

  Data = 0;
  //
  // See if the Legacy BIOS Protocol is available
  //
  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  (VOID **) &LegacyBios
                  );

  if (!EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  //
  // Open the IO Abstraction(s) needed to perform the supported test
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiIsaIoProtocolGuid,
                  (VOID **) &IsaIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Use the ISA I/O Protocol to see if Controller is the Keyboard controller
  //
  if (IsaIo->ResourceList->Device.HID != EISA_PNP_ID (0x303) || IsaIo->ResourceList->Device.UID != 0) {
    Status = EFI_UNSUPPORTED;
  } else {
    Status = IsaIo->Io.Read (
                      IsaIo,
                      EfiIsaIoWidthUint8,
                      KEYBOARD_8042_STATUS_REGISTER,
                      1,
                      &Data
                      );
    if ((EFI_ERROR (Status)) || (Data == 0xFF)) {
      //
      //if status register is FF then KBC is missing
      //
      Status =  EFI_UNSUPPORTED;
    }
  }
  //
  // Close the I/O Abstraction(s) used to perform the supported test
  //
  gBS->CloseProtocol (
         Controller,
         &gEfiIsaIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  return Status;
}

/**
  Create KEYBOARD_CONSOLE_IN_DEV instance on controller.

  @param This         Pointer of EFI_DRIVER_BINDING_PROTOCOL
  @param Controller   driver controller handle
  @param RemainingDevicePath Children's device path

  @retval whether success to create floppy control instance.
**/
EFI_STATUS
EFIAPI
KbdControllerDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                                Status;
  EFI_STATUS                                Status1;
  EFI_ISA_IO_PROTOCOL                       *IsaIo;
  KEYBOARD_CONSOLE_IN_DEV                   *ConsoleIn;
  UINT8                                     Data;
  EFI_STATUS_CODE_VALUE                     StatusCode;
  EFI_DEVICE_PATH_PROTOCOL                  *ParentDevicePath;
  BOOLEAN                                   DoKeyboardReset;
  VOID                                      *HobList;
  UINT8                                     *HobBuffer;
  EFI_PS2_POLICY_PROTOCOL                   *Ps2Policy;

  StatusCode = 0;
  HobList    = NULL;
  HobBuffer  = NULL;

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &ParentDevicePath,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Report that the keyboard is being enabled
  //
  REPORT_STATUS_CODE_WITH_DEVICE_PATH (
    EFI_PROGRESS_CODE,
    EFI_PERIPHERAL_KEYBOARD | EFI_P_PC_ENABLE,
    ParentDevicePath
    );

  //
  // Get the ISA I/O Protocol on Controller's handle
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiIsaIoProtocolGuid,
                  (VOID **) &IsaIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    gBS->CloseProtocol (
           Controller,
           &gEfiDevicePathProtocolGuid,
           This->DriverBindingHandle,
           Controller
           );
    return EFI_INVALID_PARAMETER;
  }
  //
  // Allocate private data
  //
  ConsoleIn = AllocateZeroPool (sizeof (KEYBOARD_CONSOLE_IN_DEV));
  if (ConsoleIn == NULL) {
    Status      = EFI_OUT_OF_RESOURCES;
    StatusCode  = EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_CONTROLLER_ERROR;
    goto ErrorExit;
  }
  //
  // Setup the device instance
  //
  ConsoleIn->Signature              = KEYBOARD_CONSOLE_IN_DEV_SIGNATURE;
  ConsoleIn->Handle                 = Controller;
  (ConsoleIn->ConIn).Reset          = KeyboardEfiReset;
  (ConsoleIn->ConIn).ReadKeyStroke  = KeyboardReadKeyStroke;
  ConsoleIn->DataRegisterAddress    = KEYBOARD_8042_DATA_REGISTER;
  ConsoleIn->StatusRegisterAddress  = KEYBOARD_8042_STATUS_REGISTER;
  ConsoleIn->CommandRegisterAddress = KEYBOARD_8042_COMMAND_REGISTER;
  ConsoleIn->IsaIo                  = IsaIo;
  ConsoleIn->DevicePath             = ParentDevicePath;

  ConsoleIn->ConInEx.Reset               = KeyboardEfiResetEx;
  ConsoleIn->ConInEx.ReadKeyStrokeEx     = KeyboardReadKeyStrokeEx;
  ConsoleIn->ConInEx.SetState            = KeyboardSetState;
  ConsoleIn->ConInEx.RegisterKeyNotify   = KeyboardRegisterKeyNotify;
  ConsoleIn->ConInEx.UnregisterKeyNotify = KeyboardUnregisterKeyNotify;

  InitializeListHead (&ConsoleIn->NotifyList);
  //
  // Policy change:
  // Check HOB and get reset cmd ACK while first time calling BiosKeyboardReset, because this check is used to
  // skip keyboard full reset if system do keyboard full reset in PEI phase.
  //
  DoKeyboardReset = FeaturePcdGet (PcdPs2KbdExtendedVerification);
  if (mPS2KeyboardCheck) {
    mPS2KeyboardCheck = FALSE;
    HobList = GetHobList ();
    HobBuffer = GetNextGuidHob ((CONST EFI_GUID*)&gKbcInitializedGuid, HobList);
    if (HobBuffer != NULL) {
      Status = CheckKeyboardStatus (ConsoleIn);
      if (!EFI_ERROR (Status)) {
        DoKeyboardReset = FALSE;
      }
    }

    if (FeaturePcdGet (PcdH2OBdsCpBootBeforeSupported)) {
      H2O_CP_HANDLE                        CpHandle;

      Status = H2OCpRegisterHandler (
                 &gH2OBdsCpBootBeforeGuid,
                 KbdControllerBootCpHandler,
                 H2O_CP_MEDIUM,
                 &CpHandle
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Checkpoint Register Fail: %g (%r)\n", &gH2OBdsCpBootBeforeGuid, Status));
        Status      = EFI_OUT_OF_RESOURCES;
        StatusCode  = EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_CONTROLLER_ERROR;
        goto ErrorExit;
      }
      DEBUG ((DEBUG_INFO, "Checkpoint Registered: %g (%r)\n", &gH2OBdsCpBootBeforeGuid, Status));
    }
    if (FeaturePcdGet (PcdH2OBdsCpBootAfterSupported)) {
      H2O_CP_HANDLE                        CpHandle;

      Status = H2OCpRegisterHandler (
                 &gH2OBdsCpBootAfterGuid,
                 KbdControllerBootCpHandler,
                 H2O_CP_MEDIUM,
                 &CpHandle
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Checkpoint Register Fail: %g (%r)\n", &gH2OBdsCpBootAfterGuid, Status));
        Status      = EFI_OUT_OF_RESOURCES;
        StatusCode  = EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_CONTROLLER_ERROR;
        goto ErrorExit;
      }
      DEBUG ((DEBUG_INFO, "Checkpoint Registered: %g (%r)\n", &gH2OBdsCpBootAfterGuid, Status));
    }
  }

  //
  // Setup the WaitForKey event
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_WAIT,
                  TPL_NOTIFY,
                  KeyboardWaitForKey,
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
                  KeyboardWaitForKeyEx,
                  ConsoleIn,
                  &(ConsoleIn->ConInEx.WaitForKeyEx)
                  );
  if (EFI_ERROR (Status)) {
    Status      = EFI_OUT_OF_RESOURCES;
    StatusCode  = EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_CONTROLLER_ERROR;
    goto ErrorExit;
  }
  // Setup a periodic timer, used for reading keystrokes at a fixed interval
  //
  Status = gBS->CreateEvent (
                  EVT_TIMER | EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  KeyboardTimerHandler,
                  ConsoleIn,
                  &ConsoleIn->TimerEvent
                  );
  if (EFI_ERROR (Status)) {
    Status      = EFI_OUT_OF_RESOURCES;
    StatusCode  = EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_CONTROLLER_ERROR;
    goto ErrorExit;
  }

  Status = gBS->SetTimer (
                  ConsoleIn->TimerEvent,
                  TimerPeriodic,
                  KEYBOARD_TIMER_INTERVAL
                  );
  if (EFI_ERROR (Status)) {
    Status      = EFI_OUT_OF_RESOURCES;
    StatusCode  = EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_CONTROLLER_ERROR;
    goto ErrorExit;
  }

  ConsoleIn->CurrentNsKey    = NULL;
  ConsoleIn->CurrentKbLayout = HiiGetCurrentKeyboardLayout ();
  //
  // Register event to EFI_HII_SET_KEYBOARD_LAYOUT_EVENT_GUID group,
  // which will be triggered by EFI_HII_DATABASE_PROTOCOL.SetKeyboardLayout().
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  Ps2SetKeyboardLayoutEvent,
                  ConsoleIn,
                  &gEfiHiiKeyBoardLayoutGuid,
                  &ConsoleIn->KeyboardLayoutEvent
                  );
  if (EFI_ERROR (Status)) {
    Status      = EFI_OUT_OF_RESOURCES;
    StatusCode  = EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_CONTROLLER_ERROR;
    goto ErrorExit;
  }

  REPORT_STATUS_CODE_WITH_DEVICE_PATH (
    EFI_PROGRESS_CODE,
    EFI_PERIPHERAL_KEYBOARD | EFI_P_PC_PRESENCE_DETECT,
    ParentDevicePath
    );

  //
  // Initialize Keyboard Lights from PS2 policy
  //
  ConsoleIn->CapsLock   = FALSE;
  ConsoleIn->NumLock    = FALSE;
  ConsoleIn->ScrollLock = FALSE;

  Status = gBS->LocateProtocol (
                  &gEfiPs2PolicyProtocolGuid,
                  NULL,
                  (VOID **) &Ps2Policy
                  );
  if (!EFI_ERROR (Status)) {
    if ((Ps2Policy->KeyboardLight & EFI_KEYBOARD_CAPSLOCK) == EFI_KEYBOARD_CAPSLOCK) {
      ConsoleIn->CapsLock = TRUE;
    }

    if ((Ps2Policy->KeyboardLight & EFI_KEYBOARD_NUMLOCK) == EFI_KEYBOARD_NUMLOCK) {
      ConsoleIn->NumLock = TRUE;
    }

    if ((Ps2Policy->KeyboardLight & EFI_KEYBOARD_SCROLLLOCK) == EFI_KEYBOARD_SCROLLLOCK) {
      ConsoleIn->ScrollLock = TRUE;
    }
  }

  //
  // Reset the keyboard device
  //
  Status = ConsoleIn->ConInEx.Reset (&ConsoleIn->ConInEx, DoKeyboardReset);
  if (EFI_ERROR (Status)) {
    Status      = EFI_DEVICE_ERROR;
    StatusCode  = EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_NOT_DETECTED;
    goto ErrorExit;
  }

  ConsoleIn->ControllerNameTable = NULL;
  AddUnicodeString2 (
    "eng",
    gPs2KeyboardComponentName.SupportedLanguages,
    &ConsoleIn->ControllerNameTable,
    L"PS/2 Keyboard Device",
    TRUE
    );
  AddUnicodeString2 (
    "en",
    gPs2KeyboardComponentName2.SupportedLanguages,
    &ConsoleIn->ControllerNameTable,
    L"PS/2 Keyboard Device",
    FALSE
    );


  //
  // Install protocol interfaces for the keyboard device.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Controller,
                  &gEfiSimpleTextInProtocolGuid,
                  &ConsoleIn->ConIn,
                  &gEfiSimpleTextInputExProtocolGuid,
                  &ConsoleIn->ConInEx,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    StatusCode = EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_CONTROLLER_ERROR;
    goto ErrorExit;
  }

  InsertTailList (&mConsoleInDeviceList, &ConsoleIn->Link);
  return Status;

ErrorExit:
  //
  // Report error code
  //
  if (StatusCode != 0) {
    REPORT_STATUS_CODE_WITH_DEVICE_PATH (
      EFI_ERROR_CODE | EFI_ERROR_MINOR,
      StatusCode,
      ParentDevicePath
      );
  }

  if ((ConsoleIn != NULL) && (ConsoleIn->ConIn.WaitForKey != NULL)) {
    gBS->CloseEvent (ConsoleIn->ConIn.WaitForKey);
  }

  if ((ConsoleIn != NULL) && (ConsoleIn->TimerEvent != NULL)) {
    gBS->CloseEvent (ConsoleIn->TimerEvent);
  }
  if ((ConsoleIn != NULL) && (ConsoleIn->ConInEx.WaitForKeyEx != NULL)) {
    gBS->CloseEvent (ConsoleIn->ConInEx.WaitForKeyEx);
  }
  if ((ConsoleIn != NULL) && (ConsoleIn->KeyboardLayoutEvent != NULL)) {
    gBS->CloseEvent (ConsoleIn->KeyboardLayoutEvent);
  }
  if ((ConsoleIn != NULL) && (ConsoleIn->CurrentKbLayout != NULL)) {
    FreePool(ConsoleIn->CurrentKbLayout);
  }

  if (ConsoleIn != NULL) {
    KbdFreeNotifyList (&ConsoleIn->NotifyList);
  }

  if ((ConsoleIn != NULL) && (ConsoleIn->ControllerNameTable != NULL)) {
    FreeUnicodeStringTable (ConsoleIn->ControllerNameTable);
  }
  //
  // Since there will be no timer handler for keyboard input any more,
  // exhaust input data just in case there is still keyboard data left
  //
  if (ConsoleIn != NULL) {
    Status1 = EFI_SUCCESS;
    while (!EFI_ERROR (Status1) && (Status != EFI_DEVICE_ERROR)) {
      Status1 = KeyboardRead (ConsoleIn, &Data);;
    }
  }

  if (ConsoleIn != NULL) {
    FreePool (ConsoleIn);
  }

  gBS->CloseProtocol (
         Controller,
         &gEfiDevicePathProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  gBS->CloseProtocol (
         Controller,
         &gEfiIsaIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  return Status;
}

/**
  Stop this driver on ControllerHandle. Support stoping any child handles
  created by this driver.

  @param  This              Protocol instance pointer.
  @param  ControllerHandle  Handle of device to stop driver on
  @param  NumberOfChildren  Number of Handles in ChildHandleBuffer. If number of
                            children is zero stop the entire bus driver.
  @param  ChildHandleBuffer List of Child Handles to Stop.

  @retval EFI_SUCCESS       This driver is removed ControllerHandle
  @retval other             This driver was not removed from this device

**/
EFI_STATUS
EFIAPI
KbdControllerDriverStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN  EFI_HANDLE                     Controller,
  IN  UINTN                          NumberOfChildren,
  IN  EFI_HANDLE                     *ChildHandleBuffer
  )
{
  EFI_STATUS                     Status;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL *ConIn;
  KEYBOARD_CONSOLE_IN_DEV        *ConsoleIn;
  UINT8                          Data;

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

  ConsoleIn = KEYBOARD_CONSOLE_IN_DEV_FROM_THIS (ConIn);

  //
  // Report that the keyboard is being disabled
  //
  REPORT_STATUS_CODE_WITH_DEVICE_PATH (
    EFI_PROGRESS_CODE,
    EFI_PERIPHERAL_KEYBOARD | EFI_P_PC_DISABLE,
    ConsoleIn->DevicePath
    );

  RemoveEntryList (&ConsoleIn->Link);
  if (ConsoleIn->TimerEvent != NULL) {
    gBS->CloseEvent (ConsoleIn->TimerEvent);
    ConsoleIn->TimerEvent = NULL;
  }
  if (ConsoleIn->KeyboardLayoutEvent != NULL) {
    gBS->CloseEvent (ConsoleIn->KeyboardLayoutEvent);
  }
  if (ConsoleIn->CurrentKbLayout != NULL) {
    FreePool(ConsoleIn->CurrentKbLayout);
  }
  //
  // Disable the keyboard interface
  //
  Status = DisableKeyboard (ConsoleIn);

  //
  // Since there will be no timer handler for keyboard input any more,
  // exhaust input data just in case there is still keyboard data left
  //
  Status = EFI_SUCCESS;
  while (!EFI_ERROR (Status)) {
    Status = KeyboardRead (ConsoleIn, &Data);;
  }
  //
  // Uninstall the SimpleTextIn and SimpleTextInEx protocols
  //
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

  gBS->CloseProtocol (
         Controller,
         &gEfiDevicePathProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  gBS->CloseProtocol (
         Controller,
         &gEfiIsaIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  //
  // Free other resources
  //
  if ((ConsoleIn->ConIn).WaitForKey != NULL) {
    gBS->CloseEvent ((ConsoleIn->ConIn).WaitForKey);
    (ConsoleIn->ConIn).WaitForKey = NULL;
  }
  if (ConsoleIn->ConInEx.WaitForKeyEx != NULL) {
    gBS->CloseEvent (ConsoleIn->ConInEx.WaitForKeyEx);
    ConsoleIn->ConInEx.WaitForKeyEx = NULL;
  }
  KbdFreeNotifyList (&ConsoleIn->NotifyList);
  FreeUnicodeStringTable (ConsoleIn->ControllerNameTable);
  FreePool (ConsoleIn);

  return EFI_SUCCESS;
}

/**
  Free the waiting key notify list.

  @param ListHead  Pointer to list head

  @retval EFI_INVALID_PARAMETER  ListHead is NULL
  @retval EFI_SUCCESS            Sucess to free NotifyList
**/
EFI_STATUS
KbdFreeNotifyList (
  IN OUT LIST_ENTRY           *ListHead
  )
{
  KEYBOARD_CONSOLE_IN_EX_NOTIFY *NotifyNode;

  if (ListHead == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  while (!IsListEmpty (ListHead)) {
    NotifyNode = CR (
                   ListHead->ForwardLink,
                   KEYBOARD_CONSOLE_IN_EX_NOTIFY,
                   NotifyEntry,
                   KEYBOARD_CONSOLE_IN_EX_NOTIFY_SIGNATURE
                   );
    RemoveEntryList (ListHead->ForwardLink);
    FreePool (NotifyNode);
  }

  return EFI_SUCCESS;
}

/**
  The module Entry Point for module Ps2Keyboard.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
InitializePs2Keyboard(
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS              Status;

  //
  // Install driver model protocol(s).
  //
  Status = EfiLibInstallDriverBindingComponentName2 (
             ImageHandle,
             SystemTable,
             &gKeyboardControllerDriver,
             ImageHandle,
             &gPs2KeyboardComponentName,
             &gPs2KeyboardComponentName2
             );
  ASSERT_EFI_ERROR (Status);


  return Status;
}

