/** @file
  USB Keyboard Driver

;******************************************************************************
;* Copyright (c) 2012 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "Efikey.h"
#include "Keyboard.h"

#include <PostCode.h>

//
// Prototypes
// Driver model protocol interface
//
EFI_STATUS
EFIAPI
UsbKeyboardDriverBindingEntryPoint (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  );

EFI_STATUS
EFIAPI
UsbKeyboardDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL        *This,
  IN EFI_HANDLE                         Controller,
  IN EFI_DEVICE_PATH_PROTOCOL           *RemainingDevicePath
  );

EFI_STATUS
EFIAPI
UsbKeyboardDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL        *This,
  IN EFI_HANDLE                         Controller,
  IN EFI_DEVICE_PATH_PROTOCOL           *RemainingDevicePath
  );

EFI_STATUS
EFIAPI
UsbKeyboardDriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL       *This,
  IN  EFI_HANDLE                        Controller,
  IN  UINTN                             NumberOfChildren,
  IN  EFI_HANDLE                        *ChildHandleBuffer
  );

EFI_STATUS
EFIAPI
UsbKeyboardUsbBindingSupported (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo,
  IN     EFI_USB_CORE_PROTOCOL          *UsbCore,
  IN     EFI_DEVICE_PATH_PROTOCOL       *DevicePath
  );

EFI_STATUS
EFIAPI
UsbKeyboardUsbBindingStart (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo,
  IN     EFI_USB_CORE_PROTOCOL          *UsbCore
  );

EFI_STATUS
EFIAPI
UsbKeyboardUsbBindingStop (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo,
  IN     VOID                           *Handle
  );

//
// Simple Text In Protocol Interface
//
STATIC
EFI_STATUS
EFIAPI
UsbKeyboardReset (
  IN  EFI_SIMPLE_TEXT_INPUT_PROTOCOL    *This,
  IN  BOOLEAN                           ExtendedVerification
  );

STATIC
EFI_STATUS
EFIAPI
UsbKeyboardReadKeyStroke (
  IN  EFI_SIMPLE_TEXT_INPUT_PROTOCOL    *This,
  OUT EFI_INPUT_KEY                     *Key
  );

STATIC
VOID
EFIAPI
UsbKeyboardWaitForKey (
  IN  EFI_EVENT               Event,
  IN  VOID                    *Context
  );

//
//  Helper functions
//
STATIC
EFI_STATUS
KbdFreeNotifyList (
  IN OUT LIST_ENTRY           *ListHead
  );

STATIC
BOOLEAN
IsKeyRegistered (
  IN EFI_KEY_DATA  *RegsiteredData,
  IN EFI_KEY_DATA  *InputData
  );

EFI_GUID gSimpleTextInExNotifyGuid = { \
  0x856f2def, 0x4e93, 0x4d6b, 0x94, 0xce, 0x1c, 0xfe, 0x47, 0x1, 0x3e, 0xa5 \
};

//
// Simple TextEx In Protocol Interface
//
EFI_STATUS
EFIAPI
UsbKeyboardResetEx (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN BOOLEAN                            ExtendedVerification
  );

EFI_STATUS
EFIAPI
UsbKeyboardReadKeyStrokeEx (
  IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
  OUT EFI_KEY_DATA                      *KeyData
  );

EFI_STATUS
EFIAPI
UsbKeyboardSetState (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN EFI_KEY_TOGGLE_STATE               *KeyToggleState
  );

EFI_STATUS
EFIAPI
UsbKeyboardRegisterKeyNotify (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN EFI_KEY_DATA                       *KeyData,
  IN EFI_KEY_NOTIFY_FUNCTION            KeyNotificationFunction,
  OUT EFI_HANDLE                        *NotifyHandle
  );

EFI_STATUS
EFIAPI
UsbKeyboardUnregisterKeyNotify (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN EFI_HANDLE                         NotificationHandle
  );

STATIC
VOID
UsbConnection (
  );

STATIC
VOID
EFIAPI
ReadyToBootNotifyFunction (
  IN     EFI_EVENT                      Event,
  IN     VOID                           *Context
  );

//
// USB Keyboard Driver Global Variables
//
EFI_DRIVER_BINDING_PROTOCOL gUsbKeyboardDriverBinding = {
  UsbKeyboardDriverBindingSupported,
  UsbKeyboardDriverBindingStart,
  UsbKeyboardDriverBindingStop,
  0x11,
  NULL,
  NULL
};

STATIC BOOLEAN                          mQualifierStatInstalled = FALSE;
STATIC EFI_EVENT                        mReadyToBootEvent       = NULL;
EFI_USB_CORE_PROTOCOL                   *mUsbCore;
BOOLEAN                                 mInSmram;

/**

  Driver Entry Point.

  @param  ImageHandle           EFI_HANDLE
  @param  SystemTable           EFI_SYSTEM_TABLE

  @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI
UsbKeyboardDriverBindingEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                    Status;
  //
  // Locate UsbCore protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiUsbCoreProtocolGuid,
                  NULL,
                  (VOID **)&mUsbCore
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  //
  // Register module in DXE/SMM instance
  //
  mUsbCore->ModuleRegistration (ImageHandle);
  //
  // Check the phase of instance
  //
  mUsbCore->IsInSmm (&mInSmram);
  if (!mInSmram) {
    //
    // Now in boot service, install protocols
    //
    Status = EfiLibInstallDriverBindingComponentName2 (
               ImageHandle,
               SystemTable,
               &gUsbKeyboardDriverBinding,
               ImageHandle,
               &gUsbKeyboardComponentName,
               &gUsbKeyboardComponentName2
               );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    //
    // Startup the smart USB connection
    //
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    ReadyToBootNotifyFunction,
                    NULL,
                    &gEfiEventReadyToBootGuid,
                    &mReadyToBootEvent
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  } else {
    //
    // Setup UsbCore for use by SMM instance
    //
    mUsbCore->AddressConvert (
                SMM_ADDRESS,
                (VOID*)(UINTN)mUsbCore,
                (VOID**)&mUsbCore
                );
    //
    // Register UsbBinding for legacy hot plug mechanism
    //
    Status = mUsbCore->RegisterUsbBindingProtocol(
                         UsbKeyboardUsbBindingSupported,
                         UsbKeyboardUsbBindingStart,
                         UsbKeyboardUsbBindingStop
                         );
  }
  return Status;
}

/**

  Supported.

  @param  This                  EFI_DRIVER_BINDING_PROTOCOL
  @param  Controller            Controller handle
  @param  RemainingDevicePath   EFI_DEVICE_PATH_PROTOCOL

  @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI
UsbKeyboardDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                    OpenStatus;
  EFI_USB_IO_PROTOCOL           *UsbIo;
  EFI_STATUS                    Status;
  EFI_DEV_PATH_PTR              Node;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  //
  // Check Device Path
  //
  if (RemainingDevicePath != NULL) {
    Node.DevPath = RemainingDevicePath;
    if (Node.DevPath->Type != MESSAGING_DEVICE_PATH ||
        Node.DevPath->SubType != MSG_USB_CLASS_DP   ||
        DevicePathNodeLength(Node.DevPath) != sizeof(USB_CLASS_DEVICE_PATH) ||
        Node.UsbClass->DeviceClass != CLASS_HID) {
      return EFI_UNSUPPORTED;
    }
  }
  //
  // Check if USB_IO protocol is attached on the controller handle.
  //
  OpenStatus = gBS->OpenProtocol (
                      Controller,
                      &gEfiUsbIoProtocolGuid,
                      (VOID **)&UsbIo,
                      This->DriverBindingHandle,
                      Controller,
                      EFI_OPEN_PROTOCOL_BY_DRIVER
                      );
  if (EFI_ERROR (OpenStatus)) {
    return OpenStatus;
  }

  //
  // Get the device path for CheckIgnoredDevice
  //
  Status = gBS->HandleProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **)&DevicePath
                  );
  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }
#if (KEEP_USBIO_FOR_IGNORED_DEVICE == 1)
  //
  // Filter out the USB devices which in the UsbIgnoreDevice list
  //
  Status = mUsbCore->CheckIgnoredDevice(DevicePath, UsbIo);
  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }
#endif
  //
  // Use the USB I/O protocol interface to check whether the Controller is
  // the Keyboard controller that can be managed by this driver.
  //
  Status = EFI_SUCCESS;

  if (!IsUsbKeyboard (UsbIo)) {
    Status = EFI_UNSUPPORTED;
  }
ErrorExit:
  gBS->CloseProtocol (
        Controller,
        &gEfiUsbIoProtocolGuid,
        This->DriverBindingHandle,
        Controller
        );

  return Status;
}

/**

  Start.

  @param  This                  EFI_DRIVER_BINDING_PROTOCOL
  @param  Controller            Controller handle
  @param  RemainingDevicePath   EFI_DEVICE_PATH_PROTOCOL

  @retval EFI_SUCCESS           Success
  @retval EFI_OUT_OF_RESOURCES  Can't allocate memory
  @retval EFI_UNSUPPORTED       The Start routine fail

**/
EFI_STATUS
EFIAPI
UsbKeyboardDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                    Status;
  EFI_USB_IO_PROTOCOL           *UsbIo;
  USB_KB_DEV                    *UsbKeyboardDevice;
  UINT8                         EndpointNumber;
  EFI_USB_ENDPOINT_DESCRIPTOR   EndpointDescriptor;
  UINT8                         Index;
  UINT8                         EndpointAddr;
  UINT8                         PollingInterval;
  UINT8                         PacketSize;
  BOOLEAN                       Found;
  USB_DEVICE                    UsbDevice;
  EFI_PS2_POLICY_PROTOCOL       *Ps2Policy;
  QUALIFIER_STAT                *QualifierStat;

  POST_CODE (BDS_CONNECT_USB_DEVICE);

  UsbKeyboardDevice = NULL;
  Found             = FALSE;

  //
  // Open USB_IO Protocol
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiUsbIoProtocolGuid,
                  (VOID **)&UsbIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  UsbKeyboardDevice = CreateUsbKeyboardDeviceBuffer ();
  if (UsbKeyboardDevice == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto close_protocol;
  }
  //
  // Get the Device Path Protocol on Controller's handle
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &UsbKeyboardDevice->DevicePath,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (EFI_ERROR (Status)) {
    goto free_kbdevice;
  }
  //
  // Initialize UsbKeyboardDevice
  //
  UsbKeyboardDevice->UsbIo   = UsbIo;
  //
  // Report that the usb keyboard is being enabled
  //
  KbdReportStatusCode (
    UsbKeyboardDevice,
    EFI_PROGRESS_CODE,
    (EFI_PERIPHERAL_KEYBOARD | EFI_P_PC_ENABLE)
    );

  //
  // This is pretty close to keyboard detection, so log progress
  //
  KbdReportStatusCode (
    UsbKeyboardDevice,
    EFI_PROGRESS_CODE,
    (EFI_PERIPHERAL_KEYBOARD | EFI_P_PC_PRESENCE_DETECT)
    );

  //
  // Get interface & endpoint descriptor
  //
  UsbGetInterfaceDescriptor (
    UsbIo,
    &UsbKeyboardDevice->InterfaceDescriptor
    );

  EndpointNumber = UsbKeyboardDevice->InterfaceDescriptor.NumEndpoints;

  for (Index = 0; Index < EndpointNumber; Index++) {

    UsbGetEndpointDescriptor (
      UsbIo,
      Index,
      &EndpointDescriptor
      );

    if ((EndpointDescriptor.Attributes & 0x03) == 0x03 && (EndpointDescriptor.EndpointAddress & 0x80)) {
      //
      // We only care interrupt-in endpoint here
      //
      CopyMem(&UsbKeyboardDevice->IntEndpointDescriptor, &EndpointDescriptor, sizeof(EndpointDescriptor));
      Found = TRUE;
      break;
    }
  }

  if (!Found) {
    //
    // Report Status Code to indicate that there is no USB keyboard
    //
    KbdReportStatusCode (
      UsbKeyboardDevice,
      EFI_ERROR_CODE | EFI_ERROR_MINOR,
      (EFI_PERIPHERAL_KEYBOARD | EFI_P_EC_NOT_DETECTED)
      );
    //
    // No interrupt endpoint found, then return unsupported.
    //
    Status = EFI_UNSUPPORTED;
    goto free_kbdevice;
  }
  //
  // Close smart USB connection due to device is starting connect
  //
  if (mReadyToBootEvent) {
    gBS->CloseEvent (mReadyToBootEvent);
    mReadyToBootEvent = NULL;
  }

  KbdReportStatusCode (
    UsbKeyboardDevice,
    EFI_PROGRESS_CODE,
    (EFI_PERIPHERAL_KEYBOARD | EFI_P_PC_DETECTED)
    );

  UsbKeyboardDevice->Signature                  = USB_KB_DEV_SIGNATURE;
  UsbKeyboardDevice->SimpleInput.Reset          = UsbKeyboardReset;
  UsbKeyboardDevice->SimpleInput.ReadKeyStroke  = UsbKeyboardReadKeyStroke;
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_WAIT,
                  TPL_NOTIFY,
                  UsbKeyboardWaitForKey,
                  UsbKeyboardDevice,
                  &(UsbKeyboardDevice->SimpleInput.WaitForKey)
                  );

  if (EFI_ERROR (Status)) {
    goto free_kbdevice;
  }

  UsbKeyboardDevice->SimpleInputEx.Reset               = UsbKeyboardResetEx;
  UsbKeyboardDevice->SimpleInputEx.ReadKeyStrokeEx     = UsbKeyboardReadKeyStrokeEx;
  UsbKeyboardDevice->SimpleInputEx.SetState            = UsbKeyboardSetState;
  UsbKeyboardDevice->SimpleInputEx.RegisterKeyNotify   = UsbKeyboardRegisterKeyNotify;
  UsbKeyboardDevice->SimpleInputEx.UnregisterKeyNotify = UsbKeyboardUnregisterKeyNotify;

  InitializeListHead (&UsbKeyboardDevice->NotifyList);

  Status = gBS->CreateEvent (
                  EVT_TIMER | EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  USBKeyboardTimerHandler,
                  UsbKeyboardDevice,
                  &UsbKeyboardDevice->TimerEvent
                  );
  if (EFI_ERROR (Status)) {
    goto close_event;
  }
  Status = gBS->SetTimer (UsbKeyboardDevice->TimerEvent, TimerPeriodic, KEYBOARD_TIMER_INTERVAL);
  if (EFI_ERROR (Status)) {
    goto close_timer_event;
  }

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_WAIT,
                  TPL_NOTIFY,
                  UsbKeyboardWaitForKey,
                  UsbKeyboardDevice,
                  &(UsbKeyboardDevice->SimpleInputEx.WaitForKeyEx)
                  );

  if (EFI_ERROR (Status)) {
    goto close_timer_event;
  }

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  KeyNotifyProcessHandler,
                  UsbKeyboardDevice,
                  &UsbKeyboardDevice->KeyNotifyProcessEvent
                  );
  if (EFI_ERROR (Status)) {
    goto close_event_ex;
  }

  //
  // Install Simple Text Input Protocol and Simple Text Input Ex Protocol
  // for the USB keyboard device.
  // USB keyboard is a hot plug device, and expected to work immediately
  // when plugging into system, other conventional console devices could
  // distinguish it by its device path.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Controller,
                  &gEfiSimpleTextInProtocolGuid,
                  &UsbKeyboardDevice->SimpleInput,
                  &gEfiSimpleTextInputExProtocolGuid,
                  &UsbKeyboardDevice->SimpleInputEx,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    goto close_notify_process_event;
  }

  UsbKeyboardDevice->ControllerHandle = Controller;
  Status = InitKeyboardLayout (UsbKeyboardDevice);
  if (EFI_ERROR (Status)) {
    gBS->UninstallMultipleProtocolInterfaces (
      Controller,
      &gEfiSimpleTextInProtocolGuid,
      &UsbKeyboardDevice->SimpleInput,
      &gEfiSimpleTextInputExProtocolGuid,
      &UsbKeyboardDevice->SimpleInputEx,
      NULL
      );
    goto close_notify_process_event;
  }

  //
  // In case the BDA_QUALIFIER_STAT has no updated by Ps2Keyboard/BiosKeyboard driver,
  // get Ps2Policy to synchronize with USB keyboard LED setting
  //
  Status = gBS->LocateProtocol (
                  &gEfiPs2PolicyProtocolGuid,
                  NULL,
                  (VOID **)&Ps2Policy
                  );
  if (!EFI_ERROR (Status) && !mQualifierStatInstalled) {
    if (mUsbCore->IsCsmEnabled() == EFI_SUCCESS) {
      //
      // Make synchronize with BDA qualifier stat
      //
      QualifierStat = (QUALIFIER_STAT*)(UINTN)(BDA_QUALIFIER_STAT);
      QualifierStat->ScrollLock = (Ps2Policy->KeyboardLight & EFI_KEYBOARD_SCROLLLOCK) ? 1 : 0;
      QualifierStat->NumLock    = (Ps2Policy->KeyboardLight & EFI_KEYBOARD_NUMLOCK) ? 1 : 0;
      QualifierStat->CapsLock   = (Ps2Policy->KeyboardLight & EFI_KEYBOARD_CAPSLOCK) ? 1 : 0;
      mQualifierStatInstalled   = TRUE;
    } else {
      UsbKeyboardDevice->CapsOn    = (Ps2Policy->KeyboardLight & EFI_KEYBOARD_CAPSLOCK) ? 1 : 0;
      UsbKeyboardDevice->NumLockOn = (Ps2Policy->KeyboardLight & EFI_KEYBOARD_NUMLOCK) ? 1 : 0;
      UsbKeyboardDevice->ScrollOn  = (Ps2Policy->KeyboardLight & EFI_KEYBOARD_SCROLLLOCK) ? 1 : 0;
    }
  }
  //
  // Reset USB Keyboard Device
  //
  Status = UsbKeyboardDevice->SimpleInputEx.Reset (
                                            &UsbKeyboardDevice->SimpleInputEx,
                                            TRUE
                                            );
  if (EFI_ERROR (Status)) {
    goto uninstall_protocol;
  }
  //
  // submit async interrupt transfer
  //
  EndpointAddr    = UsbKeyboardDevice->IntEndpointDescriptor.EndpointAddress;
  PollingInterval = UsbKeyboardDevice->IntEndpointDescriptor.Interval;
  PacketSize      = (UINT8) (UsbKeyboardDevice->IntEndpointDescriptor.MaxPacketSize);
  //
  // Remove previous instance in case it has been installed by legacy binding process
  //
  UsbAsyncInterruptTransfer (
    UsbIo,
    EndpointAddr,
    FALSE,
    PollingInterval,
    0,
    NULL,
    NULL
    );
  Status = UsbAsyncInterruptTransfer (
             UsbIo,
             EndpointAddr,
             TRUE,
             PollingInterval,
             PacketSize,
             KeyboardHandler,
             UsbKeyboardDevice
             );

  if (EFI_ERROR (Status)) {
    goto uninstall_protocol;
  }

  UsbKeyboardDevice->ControllerNameTable = NULL;
  AddUnicodeString2 (
    LANGUAGE_CODE_ENGLISH_ISO639,
    gUsbKeyboardComponentName.SupportedLanguages,
    &UsbKeyboardDevice->ControllerNameTable,
    CONTROLLER_DRIVER_NAME,
    TRUE
    );
  AddUnicodeString2 (
    LANGUAGE_CODE_ENGLISH_RFC4646,
    gUsbKeyboardComponentName2.SupportedLanguages,
    &UsbKeyboardDevice->ControllerNameTable,
    CONTROLLER_DRIVER_NAME,
    FALSE
    );
  //
  // Remove UsbIoDevice in case it has been installed by legacy binding process
  //
  mUsbCore->RemoveUsbDevice (
              UsbIo
              );
  //
  // Insert UsbIoDevice into UsbCore for UsbLegacy
  //
  UsbDevice.UsbHID.Type     = USB_CORE_USB_HID;
  UsbDevice.UsbHID.UsbIo    = UsbIo;
  UsbDevice.UsbHID.Instance = UsbKeyboardDevice;
  UsbDevice.UsbHID.SyncLED  = UsbKeyboardCheckLEDHandler;
  mUsbCore->InsertUsbDevice (
              &UsbDevice
              );
  //
  // Setup Smm address convert table for Smm security policy
  //
  mUsbCore->InsertAddressConvertTable (
              ACT_INSTANCE_BODY,
              UsbKeyboardDevice,
              USB_KB_DEV_BUFFER_SIZE
              );
  mUsbCore->InsertAddressConvertTable (
              ACT_INSTANCE_POINTER,
              &UsbKeyboardDevice->UsbIo,
              1
              );
  mUsbCore->InsertAddressConvertTable (
              ACT_INSTANCE_POINTER,
              &UsbKeyboardDevice->RepeatPollingHandle,
              1
              );
  mUsbCore->InsertAddressConvertTable (
              ACT_INSTANCE_POINTER,
              &UsbKeyboardDevice->RecoveryPollingHandle,
              1
              );
  return EFI_SUCCESS;

uninstall_protocol:
  gBS->UninstallMultipleProtocolInterfaces (
         Controller,
         &gEfiSimpleTextInProtocolGuid,
         &UsbKeyboardDevice->SimpleInput,
         &gEfiSimpleTextInputExProtocolGuid,
         &UsbKeyboardDevice->SimpleInputEx,
         NULL
         );
close_notify_process_event:
  gBS->CloseEvent (UsbKeyboardDevice->KeyNotifyProcessEvent);
close_event_ex:
  gBS->CloseEvent (UsbKeyboardDevice->SimpleInputEx.WaitForKeyEx);
  if (UsbKeyboardDevice->KeyboardLayoutEvent != NULL) {
    ReleaseKeyboardLayoutResources (UsbKeyboardDevice);
    gBS->CloseEvent (UsbKeyboardDevice->KeyboardLayoutEvent);
  }
close_timer_event:
  gBS->CloseEvent (UsbKeyboardDevice->TimerEvent);
close_event:
  gBS->CloseEvent (UsbKeyboardDevice->SimpleInput.WaitForKey);
free_kbdevice:
  mUsbCore->FreeBuffer (
              USB_KB_DEV_BUFFER_SIZE,
              UsbKeyboardDevice
              );
close_protocol:
  gBS->CloseProtocol (
         Controller,
         &gEfiUsbIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );
  return Status;
}

/**

  Stop.

  @param  This                  EFI_DRIVER_BINDING_PROTOCOL
  @param  Controller            Controller handle
  @param  NumberOfChildren      Child handle number
  @param  ChildHandleBuffer     Child handle buffer

  @retval EFI_SUCCESS           Success
  @retval EFI_UNSUPPORTED       Can't support

**/
EFI_STATUS
EFIAPI
UsbKeyboardDriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN  EFI_HANDLE                     Controller,
  IN  UINTN                          NumberOfChildren,
  IN  EFI_HANDLE                     *ChildHandleBuffer
  )
{
  EFI_STATUS                     Status;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL *SimpleInput;
  USB_KB_DEV                     *UsbKeyboardDevice;
  EFI_USB_IO_PROTOCOL            *UsbIo;
  UINT8                          Led;

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiSimpleTextInProtocolGuid,
                  (VOID **)&SimpleInput,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
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
    return EFI_UNSUPPORTED;
  }
  //
  // Get USB_KB_DEV instance.
  //
  UsbKeyboardDevice = USB_KB_DEV_FROM_THIS (SimpleInput);

  gBS->CloseProtocol (
        Controller,
        &gEfiSimpleTextInProtocolGuid,
        This->DriverBindingHandle,
        Controller
        );

  UsbIo   = UsbKeyboardDevice->UsbIo;
  //
  // free all the resources.
  //
  mUsbCore->RemovePeriodicTimer (UsbKeyboardDevice->RepeatPollingHandle);
  mUsbCore->RemovePeriodicTimer (UsbKeyboardDevice->RecoveryPollingHandle);

  gBS->CloseEvent (UsbKeyboardDevice->TimerEvent);
  gBS->CloseEvent (UsbKeyboardDevice->SimpleInput.WaitForKey);
  gBS->CloseEvent (UsbKeyboardDevice->SimpleInputEx.WaitForKeyEx);
  gBS->CloseEvent (UsbKeyboardDevice->KeyNotifyProcessEvent);
  KbdFreeNotifyList (&UsbKeyboardDevice->NotifyList);

  ReleaseKeyboardLayoutResources (UsbKeyboardDevice);
  gBS->CloseEvent (UsbKeyboardDevice->KeyboardLayoutEvent);

  //
  // Uninstall the Asyn Interrupt Transfer from this device
  // will disable the key data input from this device
  //
  KbdReportStatusCode (
    UsbKeyboardDevice,
    EFI_PROGRESS_CODE,
    (EFI_PERIPHERAL_KEYBOARD | EFI_P_PC_DISABLE)
    );
  //
  // Destroy asynchronous interrupt transfer
  //
  UsbAsyncInterruptTransfer (
    UsbIo,
    UsbKeyboardDevice->IntEndpointDescriptor.EndpointAddress,
    FALSE,
    UsbKeyboardDevice->IntEndpointDescriptor.Interval,
    0,
    NULL,
    NULL
    );
  if (UsbKeyboardDevice->LedSupported) {
    //
    // Clear the LED
    //
    Led = 0;
    mUsbCore->UsbSetReportRequest (
                UsbIo,
                UsbKeyboardDevice->InterfaceDescriptor.InterfaceNumber,
                0,
                HID_OUTPUT_REPORT,
                1,
                &Led
                );
  }
  gBS->CloseProtocol (
        Controller,
        &gEfiUsbIoProtocolGuid,
        This->DriverBindingHandle,
        Controller
        );
  Status = gBS->UninstallMultipleProtocolInterfaces (
                  Controller,
                  &gEfiSimpleTextInProtocolGuid,
                  &UsbKeyboardDevice->SimpleInput,
                  &gEfiSimpleTextInputExProtocolGuid,
                  &UsbKeyboardDevice->SimpleInputEx,
                  NULL
                  );
  if (UsbKeyboardDevice->ControllerNameTable != NULL) {
    FreeUnicodeStringTable (UsbKeyboardDevice->ControllerNameTable);
  }
  mUsbCore->FreeBuffer (
              USB_KB_DEV_BUFFER_SIZE,
              UsbKeyboardDevice
              );
  //
  // Remove UsbIoDevice from UsbCore for UsbLegacy
  //
  mUsbCore->RemoveUsbDevice (
              UsbIo
              );
  //
  // Remove Smm address convert table
  //
  mUsbCore->RemoveAddressConvertTable (
              ACT_INSTANCE_BODY,
              UsbKeyboardDevice
              );
  mUsbCore->RemoveAddressConvertTable (
              ACT_INSTANCE_POINTER,
              &UsbKeyboardDevice->UsbIo
              );
  mUsbCore->RemoveAddressConvertTable (
              ACT_INSTANCE_POINTER,
              &UsbKeyboardDevice->RepeatPollingHandle
              );
  mUsbCore->RemoveAddressConvertTable (
              ACT_INSTANCE_POINTER,
              &UsbKeyboardDevice->RecoveryPollingHandle
              );
  return Status;

}

/**

  Supported for UsbIo(called from UsbCore).

  @param  UsbIo                 UsbIo

  @retval EFI_UNSUPPORTED
  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
UsbKeyboardUsbBindingSupported (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo,
  IN     EFI_USB_CORE_PROTOCOL          *UsbCore,
  IN     EFI_DEVICE_PATH_PROTOCOL       *DevicePath
  )
{
  EFI_STATUS Status;
#if (KEEP_USBIO_FOR_IGNORED_DEVICE == 1)
  //
  // Filter out the USB devices which in the UsbIgnoreDevice list
  //
  Status = mUsbCore->CheckIgnoredDevice(DevicePath, UsbIo);
  if (EFI_ERROR (Status)) {
    return Status;
  }
#endif
  Status = EFI_UNSUPPORTED;
  if (IsUsbKeyboard (UsbIo)) {
    Status = EFI_SUCCESS;
  }
  return Status;
}

/**

  Supported for UsbIo(called from UsbCore).

  @param  UsbIo                 UsbIo
  @param  UsbCore               UsbCore
  @param  Handle                The instance of UsbKeyboard for output

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
UsbKeyboardUsbBindingStart (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo,
  IN     EFI_USB_CORE_PROTOCOL          *UsbCore
  )
{
  EFI_STATUS                    Status;
  USB_KB_DEV                    *UsbKeyboardDevice;
  UINT8                         EndpointNumber;
  EFI_USB_ENDPOINT_DESCRIPTOR   EndpointDescriptor;
  UINT8                         Index;
  UINT8                         EndpointAddr;
  UINT8                         PollingInterval;
  UINT8                         PacketSize;
  BOOLEAN                       Found;
  USB_DEVICE                    UsbDevice;

  POST_CODE (BDS_CONNECT_USB_DEVICE);

  Found = FALSE;
  UsbKeyboardDevice = CreateUsbKeyboardDeviceBuffer ();
  if (UsbKeyboardDevice == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Initialize UsbKeyboardDevice
  //
  UsbKeyboardDevice->UsbIo   = UsbIo;
  //
  // Get interface & endpoint descriptor
  //
  UsbGetInterfaceDescriptor (
    UsbIo,
    &UsbKeyboardDevice->InterfaceDescriptor
    );

  EndpointNumber = UsbKeyboardDevice->InterfaceDescriptor.NumEndpoints;

  for (Index = 0; Index < EndpointNumber; Index++) {

    UsbGetEndpointDescriptor (
      UsbIo,
      Index,
      &EndpointDescriptor
      );

    if ((EndpointDescriptor.Attributes & 0x03) == 0x03 && (EndpointDescriptor.EndpointAddress & 0x80)) {
      //
      // We only care interrupt-in endpoint here
      //
      UsbKeyboardDevice->IntEndpointDescriptor  = EndpointDescriptor;
      Found = TRUE;
      break;
    }
  }

  if (!Found) {
    //
    // No interrupt endpoint found, then return unsupported.
    //
    Status = EFI_UNSUPPORTED;
    goto free_kbdevice;
  }

  //
  // Reset USB Keyboard Device
  //
  Status = InitUsbKeyboard (UsbKeyboardDevice);

  if (EFI_ERROR (Status)) {
    goto free_kbdevice;
  }
  //
  // submit async interrupt transfer
  //
  EndpointAddr    = UsbKeyboardDevice->IntEndpointDescriptor.EndpointAddress;
  PollingInterval = UsbKeyboardDevice->IntEndpointDescriptor.Interval;
  PacketSize      = (UINT8) (UsbKeyboardDevice->IntEndpointDescriptor.MaxPacketSize);

  Status = UsbAsyncInterruptTransfer (
             UsbIo,
             EndpointAddr,
             TRUE,
             PollingInterval,
             PacketSize,
             KeyboardHandler,
             UsbKeyboardDevice
             );

  if (EFI_ERROR (Status)) {
    goto free_kbdevice;
  }
  //
  // Insert UsbIoDevice into UsbCore for UsbLegacy
  //
  UsbDevice.UsbHID.Type     = USB_CORE_USB_HID;
  UsbDevice.UsbHID.UsbIo    = UsbIo;
  UsbDevice.UsbHID.Instance = UsbKeyboardDevice;
  UsbDevice.UsbHID.SyncLED  = UsbKeyboardCheckLEDHandler;
  mUsbCore->InsertUsbDevice (
              &UsbDevice
              );
  return EFI_SUCCESS;

free_kbdevice:
  mUsbCore->FreeBuffer (
              USB_KB_DEV_BUFFER_SIZE,
              UsbKeyboardDevice
              );
  return Status;
}

/**

  Supported for UsbIo(called from UsbCore).

  @param  Handle                The instance of UsbKeyboard allocated by UsbIoBindingStart

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
UsbKeyboardUsbBindingStop (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo,
  IN     VOID                           *Handle
  )
{
  USB_KB_DEV                  *UsbKeyboardDevice;
  UINTN                       Mode;
  //
  // Get USB_KB_DEV instance.
  //
  UsbKeyboardDevice = Handle;
  if (UsbIo != UsbKeyboardDevice->UsbIo) {
    //
    // Due to UsbCore may pass wrong Handle for us
    //
    return EFI_NOT_FOUND;
  }
  //
  // free all the resources.
  //
  mUsbCore->RemovePeriodicTimer (UsbKeyboardDevice->RepeatPollingHandle);
  mUsbCore->RemovePeriodicTimer (UsbKeyboardDevice->RecoveryPollingHandle);
  //
  // Destroy asynchronous interrupt transfer
  //
  UsbAsyncInterruptTransfer (
    UsbIo,
    UsbKeyboardDevice->IntEndpointDescriptor.EndpointAddress,
    FALSE,
    UsbKeyboardDevice->IntEndpointDescriptor.Interval,
    0,
    NULL,
    NULL
    );
  //
  // Only destroy the instance when runtime mode because
  // the instance will be referenced by subsequently
  // binding stop function if system under POST time
  //
  mUsbCore->GetMode(&Mode);
  if (Mode == USB_CORE_RUNTIME_MODE) {
    mUsbCore->FreeBuffer (
                USB_KB_DEV_BUFFER_SIZE,
                UsbKeyboardDevice
                );
  }
  //
  // Remove UsbIoDevice from UsbCore for UsbLegacy
  //
  mUsbCore->RemoveUsbDevice (
              UsbIo
              );
  return EFI_SUCCESS;
}

/**

  Implements EFI_SIMPLE_TEXT_INPUT_PROTOCOL.Reset() function.

  @param  This                  The EFI_SIMPLE_TEXT_INPUT_PROTOCOL instance.
  @param  ExtendedVerification  Indicates that the driver may perform a more exhaustive
                                verification operation of the device during reset.

  @retval EFI_SUCCESS           Success
  @retval EFI_DEVICE_ERROR      Hardware Error

**/
EFI_STATUS
EFIAPI
UsbKeyboardReset (
  IN  EFI_SIMPLE_TEXT_INPUT_PROTOCOL    *This,
  IN  BOOLEAN                           ExtendedVerification
  )
{
  EFI_STATUS          Status;
  USB_KB_DEV          *UsbKeyboardDevice;
  EFI_USB_IO_PROTOCOL *UsbIo;

  UsbKeyboardDevice = USB_KB_DEV_FROM_THIS (This);

  UsbIo             = UsbKeyboardDevice->UsbIo;

  KbdReportStatusCode (
    UsbKeyboardDevice,
    EFI_PROGRESS_CODE,
    (EFI_PERIPHERAL_KEYBOARD | EFI_P_PC_RESET)
    );

  //
  // Non Exhaustive reset:
  // only reset private data structures.
  //
  if (!ExtendedVerification) {
    //
    // Clear the key buffer of this Usb keyboard
    //
    KbdReportStatusCode (
      UsbKeyboardDevice,
      EFI_PROGRESS_CODE,
      (EFI_PERIPHERAL_KEYBOARD | EFI_P_KEYBOARD_PC_CLEAR_BUFFER)
      );

    InitQueue (&UsbKeyboardDevice->UsbKeyQueue, sizeof (USB_KEY));
    InitQueue (&UsbKeyboardDevice->EfiKeyQueue, sizeof (EFI_KEY_DATA));
    InitQueue (&UsbKeyboardDevice->EfiKeyQueueForNotify, sizeof (EFI_KEY_DATA));

    return EFI_SUCCESS;
  }

  //
  // Exhaustive reset
  //
  Status = InitUsbKeyboard (UsbKeyboardDevice);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}


/**

  Reads the next keystroke from the input device. The WaitForKey Event can
  be used to test for existence of a keystroke via WaitForEvent () call.

  @param  UsbKeyboardDevice     The USB_KB_DEV instance.
  @param  KeyData               A pointer to a buffer that is filled in with the keystroke
                                state data for the key that was pressed.

  @retval EFI_SUCCESS           The keystroke information was returned.
  @retval EFI_NOT_READY         There was no keystroke data available.
  @retval EFI_DEVICE_ERROR      The keystroke information was not returned due to
                                hardware errors.
  @retval EFI_INVALID_PARAMETER KeyData is NULL.

**/
EFI_STATUS
EFIAPI
UsbKeyboardReadKeyStrokeWorker (
  IN USB_KB_DEV                     *UsbKeyboardDevice,
  OUT EFI_KEY_DATA                  *KeyData
  )
{
  if (KeyData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (IsQueueEmpty (&UsbKeyboardDevice->EfiKeyQueue)) {
    ZeroMem (&KeyData->Key, sizeof (KeyData->Key));
    InitializeKeyState (UsbKeyboardDevice, &KeyData->KeyState);
    return EFI_NOT_READY;
  }

  Dequeue (&UsbKeyboardDevice->EfiKeyQueue, KeyData, sizeof (*KeyData));

  return EFI_SUCCESS;
}

/**
  Timer handler to convert the key from USB.

  @param  Event                    Indicates the event that invoke this function.
  @param  Context                  Indicates the calling context.
**/
VOID
EFIAPI
USBKeyboardTimerHandler (
  IN  EFI_EVENT                 Event,
  IN  VOID                      *Context
  )
{
  EFI_STATUS                    Status;
  USB_KB_DEV                    *UsbKeyboardDevice;
  UINT8                         KeyCode;
  EFI_KEY_DATA                  KeyData;
  LIST_ENTRY                    *Link;
  LIST_ENTRY                    *NotifyList;
  KEYBOARD_CONSOLE_IN_EX_NOTIFY *CurrentNotify;

  UsbKeyboardDevice = (USB_KB_DEV *) Context;

  //
  // Fetch raw data from the USB keyboard buffer,
  // and translate it into USB keycode.
  //
  Status = UsbParseKey (UsbKeyboardDevice, &KeyCode);
  if (EFI_ERROR (Status)) {
    return ;
  }

  //
  // Translate saved USB keycode into EFI_INPUT_KEY
  //
  Status = UsbKeyCodeToEFIScanCode (UsbKeyboardDevice, KeyCode, &KeyData.Key);
  if (EFI_ERROR (Status)) {
    return;
  }

  CopyMem (&KeyData.KeyState, &UsbKeyboardDevice->KeyState, sizeof (KeyData.KeyState));

  //
  // Insert to the EFI Key queue
  //
  Enqueue (&UsbKeyboardDevice->EfiKeyQueue, &KeyData, sizeof (KeyData));

  //
  // Signal KeyNotify process event if this key pressed matches any key registered.
  //
  NotifyList = &UsbKeyboardDevice->NotifyList;
  for (Link = GetFirstNode (NotifyList); !IsNull (NotifyList, Link); Link = GetNextNode (NotifyList, Link)) {
    CurrentNotify = CR (Link, KEYBOARD_CONSOLE_IN_EX_NOTIFY, NotifyEntry, USB_KB_CONSOLE_IN_EX_NOTIFY_SIGNATURE);
    if (IsKeyRegistered (&CurrentNotify->KeyData, &KeyData)) {
      //
      // The key notification function needs to run at TPL_CALLBACK
      // while current TPL is TPL_NOTIFY. It will be invoked in
      // KeyNotifyProcessHandler() which runs at TPL_CALLBACK.
      //
      Enqueue (&UsbKeyboardDevice->EfiKeyQueueForNotify, &KeyData, sizeof (KeyData));
      gBS->SignalEvent (UsbKeyboardDevice->KeyNotifyProcessEvent);
      break;
    }
  }
}

/**

  Implements EFI_SIMPLE_TEXT_INPUT_PROTOCOL.ReadKeyStroke() function.

  @param  This                  The EFI_SIMPLE_TEXT_INPUT_PROTOCOL instance.
  @param  Key                   A pointer to a buffer that is filled in with the keystroke
                                information for the key that was pressed.

  @retval EFI_SUCCESS           Success

**/
STATIC
EFI_STATUS
EFIAPI
UsbKeyboardReadKeyStroke (
  IN  EFI_SIMPLE_TEXT_INPUT_PROTOCOL    *This,
  OUT EFI_INPUT_KEY                     *Key
  )
{
  USB_KB_DEV  *UsbKeyboardDevice;
  EFI_STATUS  Status;
  EFI_KEY_DATA    KeyData;

  ASSERT (Key != NULL);

  UsbKeyboardDevice = USB_KB_DEV_FROM_THIS (This);

  if (Key == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_SUCCESS;


  //
  // Considering if the partial keystroke is enabled, there maybe a partial
  // keystroke in the queue, so here skip the partial keystroke and get the
  // next key from the queue
  //
  while (1) {

    Status = UsbKeyboardReadKeyStrokeWorker (UsbKeyboardDevice, &KeyData);
    if (EFI_ERROR (Status)) {
      break;
    }

    //
    // SimpleTextIn Protocol doesn't support partial keystroke;
    //
    if (KeyData.Key.ScanCode == SCAN_NULL && KeyData.Key.UnicodeChar == CHAR_NULL) {
      continue;
    }

    //
    // Translate the CTRL-Alpha characters to their corresponding control value
    // (ctrl-a = 0x0001 through ctrl-Z = 0x001A)
    //
    if ((KeyData.KeyState.KeyShiftState & (EFI_LEFT_CONTROL_PRESSED | EFI_RIGHT_CONTROL_PRESSED)) != 0) {
      if (KeyData.Key.UnicodeChar >= L'a' && KeyData.Key.UnicodeChar <= L'z') {
        KeyData.Key.UnicodeChar = (CHAR16) (KeyData.Key.UnicodeChar - L'a' + 1);
      } else if (KeyData.Key.UnicodeChar >= L'A' && KeyData.Key.UnicodeChar <= L'Z') {
        KeyData.Key.UnicodeChar = (CHAR16) (KeyData.Key.UnicodeChar - L'A' + 1);
      } else if (KeyData.Key.UnicodeChar == '[' || KeyData.Key.UnicodeChar == '{') {
        KeyData.Key.ScanCode    = SCAN_ESC;
        KeyData.Key.UnicodeChar = CHAR_NULL;
      } else if (KeyData.Key.UnicodeChar == '\\' || KeyData.Key.UnicodeChar == '|') {
        KeyData.Key.UnicodeChar = 0x1C;
      } else if (KeyData.Key.UnicodeChar == ']' || KeyData.Key.UnicodeChar == '}') {
        KeyData.Key.UnicodeChar = 0x1D;
      } else if (KeyData.Key.UnicodeChar == '^' || KeyData.Key.UnicodeChar == '6') {
        KeyData.Key.UnicodeChar = 0x1E;
      } else if (KeyData.Key.UnicodeChar == '_' || KeyData.Key.UnicodeChar == '-') {
        KeyData.Key.UnicodeChar = 0x1F;
      }
    }

    CopyMem (Key, &KeyData.Key, sizeof (EFI_INPUT_KEY));
    break;
  }

  return Status;
}

/**

  Handler function for WaitForKey event.

  @param  Event                 Event to be signaled when a key is pressed.
  @param  Context               Points to USB_KB_DEV instance.

**/
STATIC
VOID
EFIAPI
UsbKeyboardWaitForKey (
  IN  EFI_EVENT               Event,
  IN  VOID                    *Context
  )
{
  USB_KB_DEV  *UsbKeyboardDevice;
  EFI_KEY_DATA KeyData;
  EFI_TPL      OldTpl;

  UsbKeyboardDevice = (USB_KB_DEV *) Context;

  //
  // Enter critical section
  //
  OldTpl = gBS->RaiseTPL (TPL_NOTIFY);

  //
  // WaitforKey doesn't support the partial key.
  // Considering if the partial keystroke is enabled, there maybe a partial
  // keystroke in the queue, so here skip the partial keystroke and get the
  // next key from the queue
  //
  while (!IsQueueEmpty (&UsbKeyboardDevice->EfiKeyQueue)) {
    //
    // If there is pending key, signal the event.
    //
    CopyMem (
      &KeyData,
      UsbKeyboardDevice->EfiKeyQueue.Buffer[UsbKeyboardDevice->EfiKeyQueue.Head],
      sizeof (EFI_KEY_DATA)
      );
    if (KeyData.Key.ScanCode == SCAN_NULL && KeyData.Key.UnicodeChar == CHAR_NULL) {
      Dequeue (&UsbKeyboardDevice->EfiKeyQueue, &KeyData, sizeof (EFI_KEY_DATA));
      continue;
    }
    gBS->SignalEvent (Event);
    break;
  }
  //
  // Leave critical section and return
  //
  gBS->RestoreTPL (OldTpl);
}

/**

  Report Status Code in Usb Bot Driver

  @param  DevicePath            Use this to get Device Path
  @param  CodeType              Status Code Type
  @param  CodeValue             Status Code Value

**/
VOID
KbdReportStatusCode (
  IN USB_KB_DEV                *UsbKeyboardDevice,
  IN EFI_STATUS_CODE_TYPE      CodeType,
  IN EFI_STATUS_CODE_VALUE     Value
  )
{
  UINTN      Mode;

  mUsbCore->GetMode(&Mode);
  if (Mode != USB_CORE_RUNTIME_MODE) {
    //
    // Only report status code during POST and non-SMM mode
    //
    if (!mInSmram) {
      ReportStatusCodeWithDevicePath (
        CodeType,
        Value,
        UsbKeyboardDevice->DevicePath
        );
    }
  }
}

/**

  Free notify list which used for notification of console in

  @param  ListHead              The list head

  @retval EFI_SUCCESS           Free the notify list successfully
  @retval EFI_INVALID_PARAMETER ListHead is invalid.

**/
STATIC
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
                   USB_KB_CONSOLE_IN_EX_NOTIFY_SIGNATURE
                   );
    RemoveEntryList (ListHead->ForwardLink);
    gBS->FreePool (NotifyNode);
  }

  return EFI_SUCCESS;
}

/**

  Check is the key been registered

  @param  RegsiteredData        A pointer to a buffer that is filled in with the keystroke
                                state data for the key that was registered.
  @param  InputData             A pointer to a buffer that is filled in with the keystroke
                                state data for the key that was pressed.

  @retval TRUE                  Key be pressed matches a registered key.
  @retval FLASE                 Match failed.

**/
STATIC
BOOLEAN
IsKeyRegistered (
  IN EFI_KEY_DATA  *RegsiteredData,
  IN EFI_KEY_DATA  *InputData
  )
{
  ASSERT (RegsiteredData != NULL && InputData != NULL);
  if ((RegsiteredData == NULL) || (InputData == NULL)) {
    return FALSE;
  }

  if ((RegsiteredData->Key.ScanCode    != InputData->Key.ScanCode) ||
      (RegsiteredData->Key.UnicodeChar != InputData->Key.UnicodeChar)) {
    return FALSE;
  }

  //
  // Assume KeyShiftState/KeyToggleState = 0 in Registered key data means these state could be ignored.
  //
  if (RegsiteredData->KeyState.KeyShiftState != 0 &&
      RegsiteredData->KeyState.KeyShiftState != InputData->KeyState.KeyShiftState) {
    return FALSE;
  }
  if (RegsiteredData->KeyState.KeyToggleState != 0 &&
      RegsiteredData->KeyState.KeyToggleState != InputData->KeyState.KeyToggleState) {
    return FALSE;
  }

  return TRUE;

}

/**

  Reset the input device and optionally run diagnostics

  @param  This                  Protocol instance pointer.
  @param  ExtendedVerification  Driver may perform diagnostics on reset.

  @retval EFI_SUCCESS           The device was reset.
  @retval EFI_DEVICE_ERROR      The device is not functioning properly and could
                                not be reset.

**/
EFI_STATUS
EFIAPI
UsbKeyboardResetEx (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN BOOLEAN                            ExtendedVerification
  )
{
  EFI_STATUS                Status;
  USB_KB_DEV                *UsbKeyboardDevice;
  EFI_TPL                   OldTpl;


  UsbKeyboardDevice = TEXT_INPUT_EX_USB_KB_DEV_FROM_THIS (This);

  Status = UsbKeyboardDevice->SimpleInput.Reset (&UsbKeyboardDevice->SimpleInput, ExtendedVerification);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  OldTpl = gBS->RaiseTPL (TPL_NOTIFY);
  UsbKeyboardDevice->KeyState.KeyShiftState  = EFI_SHIFT_STATE_VALID;
  UsbKeyboardDevice->KeyState.KeyToggleState = EFI_TOGGLE_STATE_VALID;
  gBS->RestoreTPL (OldTpl);

  return EFI_SUCCESS;

}

/**

  Reads the next keystroke from the input device. The WaitForKey Event can
  be used to test for existence of a keystroke via WaitForEvent () call.

  @param  This                  Protocol instance pointer.
  @param  KeyData               A pointer to a buffer that is filled in with the keystroke
                                state data for the key that was pressed.

  @retval EFI_SUCCESS           The keystroke information was returned.
  @retval EFI_NOT_READY         There was no keystroke data available.
  @retval EFI_DEVICE_ERROR      The keystroke information was not returned due to
                                hardware errors.
  @retval EFI_INVALID_PARAMETER KeyData is NULL.

**/
EFI_STATUS
EFIAPI
UsbKeyboardReadKeyStrokeEx (
  IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
  OUT EFI_KEY_DATA                      *KeyData
  )
{
  USB_KB_DEV                        *UsbKeyboardDevice;

  ASSERT (KeyData != NULL);

  UsbKeyboardDevice = TEXT_INPUT_EX_USB_KB_DEV_FROM_THIS (This);
  if (KeyData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  return UsbKeyboardReadKeyStrokeWorker (UsbKeyboardDevice, KeyData);
}

/**

  Set certain state for the input device.

  @param  This                  Protocol instance pointer.
  @param  KeyToggleState        A pointer to the EFI_KEY_TOGGLE_STATE to set the
                                state for the input device.

  @retval EFI_SUCCESS           The device state was set successfully.
  @retval EFI_DEVICE_ERROR      The device is not functioning correctly and could
                                not have the setting adjusted.
  @retval EFI_UNSUPPORTED       The device does not have the ability to set its state.
  @retval EFI_INVALID_PARAMETER KeyToggleState is NULL.

**/
EFI_STATUS
EFIAPI
UsbKeyboardSetState (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN EFI_KEY_TOGGLE_STATE               *KeyToggleState
  )
{
  USB_KB_DEV                        *UsbKeyboardDevice;

  if (KeyToggleState == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  UsbKeyboardDevice = TEXT_INPUT_EX_USB_KB_DEV_FROM_THIS (This);

  if (((UsbKeyboardDevice->KeyState.KeyToggleState & EFI_TOGGLE_STATE_VALID) != EFI_TOGGLE_STATE_VALID) ||
      ((*KeyToggleState & EFI_TOGGLE_STATE_VALID) != EFI_TOGGLE_STATE_VALID) ||
      (((*KeyToggleState & EFI_KEY_STATE_EXPOSED) == EFI_KEY_STATE_EXPOSED) && (mUsbCore->IsCsmEnabled() == EFI_SUCCESS))) {

    return EFI_UNSUPPORTED;
  }

  //
  // Update the status light
  //

  UsbKeyboardDevice->ScrollOn   = 0;
  UsbKeyboardDevice->NumLockOn  = 0;
  UsbKeyboardDevice->CapsOn     = 0;
  UsbKeyboardDevice->IsSupportPartialKey = 0;

  if ((*KeyToggleState & EFI_SCROLL_LOCK_ACTIVE) == EFI_SCROLL_LOCK_ACTIVE) {
    UsbKeyboardDevice->ScrollOn = 1;
  }
  if ((*KeyToggleState & EFI_NUM_LOCK_ACTIVE) == EFI_NUM_LOCK_ACTIVE) {
    UsbKeyboardDevice->NumLockOn = 1;
  }
  if ((*KeyToggleState & EFI_CAPS_LOCK_ACTIVE) == EFI_CAPS_LOCK_ACTIVE) {
    UsbKeyboardDevice->CapsOn = 1;
  }
  if ((*KeyToggleState & EFI_KEY_STATE_EXPOSED) == EFI_KEY_STATE_EXPOSED) {
    UsbKeyboardDevice->IsSupportPartialKey = 1;
  }

  SetKeyLED (UsbKeyboardDevice);

  UsbKeyboardDevice->KeyState.KeyToggleState = *KeyToggleState;

  return EFI_SUCCESS;

}

/**

  Register a notification function for a particular keystroke for the input device.

  @param  This                          Protocol instance pointer.
  @param  KeyData                       A pointer to a buffer that is filled in with the keystroke
                                        information data for the key that was pressed.
  @param  KeyNotificationFunction       Points to the function to be called when the key
                                        sequence is typed specified by KeyData.
  @param  NotifyHandle                  Points to the unique handle assigned to the registered notification.

  @retval EFI_SUCCESS                   The notification function was registered successfully.
  @retval EFI_OUT_OF_RESOURCES          Unable to allocate resources for necessary data structures.
  @retval EFI_INVALID_PARAMETER         KeyData or NotifyHandle is NULL.

**/
EFI_STATUS
EFIAPI
UsbKeyboardRegisterKeyNotify (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN EFI_KEY_DATA                       *KeyData,
  IN EFI_KEY_NOTIFY_FUNCTION            KeyNotificationFunction,
  OUT EFI_HANDLE                        *NotifyHandle
  )
{
  USB_KB_DEV                        *UsbKeyboardDevice;
  EFI_STATUS                        Status;
  KEYBOARD_CONSOLE_IN_EX_NOTIFY     *NewNotify;
  LIST_ENTRY                        *Link;
  KEYBOARD_CONSOLE_IN_EX_NOTIFY     *CurrentNotify;

  if (KeyData == NULL || NotifyHandle == NULL || KeyNotificationFunction == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  UsbKeyboardDevice = TEXT_INPUT_EX_USB_KB_DEV_FROM_THIS (This);

  //
  // Return EFI_SUCCESS if the (KeyData, NotificationFunction) is already registered.
  //
  for (Link = UsbKeyboardDevice->NotifyList.ForwardLink; Link != &UsbKeyboardDevice->NotifyList; Link = Link->ForwardLink) {
    CurrentNotify = CR (
                      Link,
                      KEYBOARD_CONSOLE_IN_EX_NOTIFY,
                      NotifyEntry,
                      USB_KB_CONSOLE_IN_EX_NOTIFY_SIGNATURE
                      );
    if (IsKeyRegistered (&CurrentNotify->KeyData, KeyData)) {
      if (CurrentNotify->KeyNotificationFn == KeyNotificationFunction) {
        *NotifyHandle = CurrentNotify->NotifyHandle;
        return EFI_SUCCESS;
      }
    }
  }

  //
  // Allocate resource to save the notification function
  //
  NewNotify = AllocateZeroPool (sizeof (KEYBOARD_CONSOLE_IN_EX_NOTIFY));
  if (NewNotify == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  NewNotify->Signature         = USB_KB_CONSOLE_IN_EX_NOTIFY_SIGNATURE;
  NewNotify->KeyNotificationFn = KeyNotificationFunction;
  CopyMem (&NewNotify->KeyData, KeyData, sizeof (EFI_KEY_DATA));
  InsertTailList (&UsbKeyboardDevice->NotifyList, &NewNotify->NotifyEntry);

  //
  // Use gSimpleTextInExNotifyGuid to get a valid EFI_HANDLE
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &NewNotify->NotifyHandle,
                  &gSimpleTextInExNotifyGuid,
                  NULL,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  *NotifyHandle = NewNotify->NotifyHandle;

  return EFI_SUCCESS;

}

/**

  Remove a registered notification function from a particular keystroke.

  @param  This                  Protocol instance pointer.
  @param  NotificationHandle    The handle of the notification function being unregistered.

  @retval EFI_SUCCESS           The notification function was unregistered successfully.
  @retval EFI_INVALID_PARAMETER The NotificationHandle is invalid.

**/
EFI_STATUS
EFIAPI
UsbKeyboardUnregisterKeyNotify (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN EFI_HANDLE                         NotificationHandle
  )
{
  USB_KB_DEV                        *UsbKeyboardDevice;
  EFI_STATUS                        Status;
  KEYBOARD_CONSOLE_IN_EX_NOTIFY     *CurrentNotify;
  LIST_ENTRY                        *Link;

  if (NotificationHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  UsbKeyboardDevice = TEXT_INPUT_EX_USB_KB_DEV_FROM_THIS (This);

  Status = gBS->OpenProtocol (
                  NotificationHandle,
                  &gSimpleTextInExNotifyGuid,
                  NULL,
                  NULL,
                  NULL,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return EFI_INVALID_PARAMETER;
  }

  for (Link = UsbKeyboardDevice->NotifyList.ForwardLink; Link != &UsbKeyboardDevice->NotifyList; Link = Link->ForwardLink) {
    CurrentNotify = CR (
                      Link,
                      KEYBOARD_CONSOLE_IN_EX_NOTIFY,
                      NotifyEntry,
                      USB_KB_CONSOLE_IN_EX_NOTIFY_SIGNATURE
                      );
    if (CurrentNotify->NotifyHandle == NotificationHandle) {
      //
      // Remove the notification function from NotifyList and free resources
      //
      RemoveEntryList (&CurrentNotify->NotifyEntry);
      Status = gBS->UninstallMultipleProtocolInterfaces (
                      CurrentNotify->NotifyHandle,
                      &gSimpleTextInExNotifyGuid,
                      NULL,
                      NULL
                      );
      ASSERT_EFI_ERROR (Status);
      gBS->FreePool (CurrentNotify);
      return EFI_SUCCESS;
    }
  }

  //
  // Cannot find the matching entry in database.
  //
  return EFI_INVALID_PARAMETER;
}

/**
  Process key notify.

  @param  Event                 Indicates the event that invoke this function.
  @param  Context               Indicates the calling context.
**/
VOID
EFIAPI
KeyNotifyProcessHandler (
  IN  EFI_EVENT                 Event,
  IN  VOID                      *Context
  )
{
  EFI_STATUS                    Status;
  USB_KB_DEV                    *UsbKeyboardDevice;
  EFI_KEY_DATA                  KeyData;
  LIST_ENTRY                    *Link;
  LIST_ENTRY                    *NotifyList;
  KEYBOARD_CONSOLE_IN_EX_NOTIFY *CurrentNotify;
  EFI_TPL                       OldTpl;

  UsbKeyboardDevice = (USB_KB_DEV *) Context;

  //
  // Invoke notification functions.
  //
  NotifyList = &UsbKeyboardDevice->NotifyList;
  while (TRUE) {
    //
    // Enter critical section
    //
    OldTpl = gBS->RaiseTPL (TPL_NOTIFY);
    Status = Dequeue (&UsbKeyboardDevice->EfiKeyQueueForNotify, &KeyData, sizeof (KeyData));
    //
    // Leave critical section
    //
    gBS->RestoreTPL (OldTpl);
    if (EFI_ERROR (Status)) {
      break;
    }
    for (Link = GetFirstNode (NotifyList); !IsNull (NotifyList, Link); Link = GetNextNode (NotifyList, Link)) {
      CurrentNotify = CR (Link, KEYBOARD_CONSOLE_IN_EX_NOTIFY, NotifyEntry, USB_KB_CONSOLE_IN_EX_NOTIFY_SIGNATURE);
      if (IsKeyRegistered (&CurrentNotify->KeyData, &KeyData)) {
        CurrentNotify->KeyNotificationFn (&KeyData);
      }
    }
  }
}

//
// Fake Simple Text In Protocol
//
STATIC
EFI_STATUS
EFIAPI
FakeKeyboardReset (
  IN  EFI_SIMPLE_TEXT_INPUT_PROTOCOL    *This,
  IN  BOOLEAN                           ExtendedVerification
  )
{
  return EFI_UNSUPPORTED;
}

STATIC
EFI_STATUS
EFIAPI
FakeKeyboardReadKeyStroke (
  IN  EFI_SIMPLE_TEXT_INPUT_PROTOCOL    *This,
  OUT EFI_INPUT_KEY                     *Key
  )
{
  UsbConnection ();
  return EFI_UNSUPPORTED;
}
//
// Fake Simple TextEx In Protocol Interface
//
EFI_STATUS
EFIAPI
FakeKeyboardResetEx (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN BOOLEAN                            ExtendedVerification
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
FakeKeyboardReadKeyStrokeEx (
  IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *This,
  OUT EFI_KEY_DATA                      *KeyData
  )
{
  UsbConnection ();
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
FakeKeyboardSetState (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN EFI_KEY_TOGGLE_STATE               *KeyToggleState
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
FakeKeyboardRegisterKeyNotify (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN EFI_KEY_DATA                       *KeyData,
  IN EFI_KEY_NOTIFY_FUNCTION            KeyNotificationFunction,
  OUT EFI_HANDLE                        *NotifyHandle
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
FakeKeyboardUnregisterKeyNotify (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *This,
  IN EFI_HANDLE                         NotificationHandle
  )
{
  return EFI_UNSUPPORTED;
}

#define EFI_USB_BUS_PROTOCOL_GUID \
  { \
    0x2B2F68CC, 0x0CD2, 0x44cf, 0x8E, 0x8B, 0xBB, 0xA2, 0x0B, 0x1B, 0x5B, 0x75 \
  }

STATIC FAKE_DEV                         mFakeDev = {
  NULL,
  {
    MESSAGING_DEVICE_PATH, MSG_USB_DP, sizeof(USB_DEVICE_PATH), 0, 0, 0,
    MESSAGING_DEVICE_PATH, MSG_USB_CLASS_DP, sizeof(USB_CLASS_DEVICE_PATH), 0, 0xFF, 0xFF, 0xFF, 0xFF, CLASS_HID, 0xFF, 0xFF,
    END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, END_DEVICE_PATH_LENGTH, 0
  },
  {
    FakeKeyboardReset,
    FakeKeyboardReadKeyStroke,
    NULL
  },
  {
    FakeKeyboardResetEx,
    FakeKeyboardReadKeyStrokeEx,
    NULL,
    FakeKeyboardSetState,
    FakeKeyboardRegisterKeyNotify,
    FakeKeyboardUnregisterKeyNotify,
  }
};

STATIC
VOID
UsbConnection (
  )
{
  EFI_STATUS            Status;
  EFI_PCI_IO_PROTOCOL   *PciIo;
  UINTN                 Index;
  UINTN                 HandleCount;
  EFI_HANDLE            *HandleBuffer;
  UINT8                 UsbClassCReg[3];
  LIST_ENTRY            ConnectList[3];
  LIST_ENTRY            *Handle;
  LIST_ENTRY            *List;
  //
  // Uninstall the fake SimpleTextIn/Ex protocols
  //
  gBS->UninstallMultipleProtocolInterfaces (
         mFakeDev.Handle,
         &gEfiDevicePathProtocolGuid,
         mFakeDev.UsbHidDevicePath + sizeof(USB_DEVICE_PATH),
         &gEfiSimpleTextInProtocolGuid,
         &mFakeDev.SimpleInput,
         &gEfiSimpleTextInputExProtocolGuid,
         &mFakeDev.SimpleInputEx,
         NULL
         );
  //
  // Connect the fake device for ConPlatform and ConSplitter
  //
  gBS->DisconnectController (
         mFakeDev.Handle,
         NULL,
         NULL
         );
  //
  // Connect USB stack
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return;
  }
  InitializeListHead (&ConnectList[0]);
  InitializeListHead (&ConnectList[1]);
  InitializeListHead (&ConnectList[2]);
  //
  // Enumerate PciIo to collect USB devices
  //
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **)&PciIo
                    );
    if (EFI_ERROR (Status)) {
      return;
    }
    Status = PciIo->Pci.Read (
                          PciIo,
                          EfiPciIoWidthUint8,
                          9,
                          3,
                          UsbClassCReg
                          );
    if (EFI_ERROR (Status)) {
      return;
    }
    //
    // Test whether the controller belongs to USB type
    //

    if (UsbClassCReg[2] == PCI_CLASS_SERIAL && UsbClassCReg[1] == PCI_CLASS_SERIAL_USB) {
      Handle = AllocatePool (sizeof (LIST_ENTRY) + sizeof (EFI_HANDLE));
      if (Handle == NULL) {
        return;
      }
      //
      // Sort the USB HC connection sequence as 3.0->2.0->1.1
      //
      *(EFI_HANDLE*) (Handle + 1) = HandleBuffer[Index];
      if (UsbClassCReg[0] == 0x30) List = &ConnectList[0];
      else if (UsbClassCReg[0] == 0x20) List = &ConnectList[1];
      else List = &ConnectList[2];
      InsertTailList (List, Handle);
    }
  }
  gBS->FreePool (HandleBuffer);
  for (Index = 0; Index < 3; Index ++) {
    while (!IsListEmpty (&ConnectList[Index])) {
      Handle = GetFirstNode (&ConnectList[Index]);
      gBS->ConnectController (
             *(EFI_HANDLE*) (Handle + 1),
             NULL,
             (EFI_DEVICE_PATH_PROTOCOL*) mFakeDev.UsbHidDevicePath,
             FALSE
             );
      RemoveEntryList (Handle);
      gBS->FreePool (Handle);
    }
  }
}

STATIC
VOID
EFIAPI
ReadyToBootNotifyFunction (
  IN     EFI_EVENT                      Event,
  IN     VOID                           *Context
  )
{
  EFI_STATUS Status;
  VOID       *Interface;
  EFI_GUID   UsbBusProtocolGuid = EFI_USB_BUS_PROTOCOL_GUID;

  gBS->CloseEvent (mReadyToBootEvent);
  mReadyToBootEvent = NULL;
  //
  // Detect is USB stack been started
  //
  Status = gBS->LocateProtocol (
                  &UsbBusProtocolGuid,
                  NULL,
                  (VOID **)&Interface
                  );
  if (Status == EFI_SUCCESS) {
    //
    // USB stack has started, quit directly
    //
    return;
  }
  //
  // Install fake simple text in protocol interface
  // for the usb keyboard device.
  // Usb keyboard is a hot plug device, and expected to work immediately
  // when plugging into system, so a HotPlugDeviceGuid is installed onto
  // the usb keyboard device handle, to distinguish it from other conventional
  // console devices.
  // NOTE: Device path should be the same as the short-form device path in ConIn variable that starts with MSG_USB_CLASS_DP.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mFakeDev.Handle,
                  &gEfiDevicePathProtocolGuid,
                  mFakeDev.UsbHidDevicePath + sizeof(USB_DEVICE_PATH),
                  &gEfiSimpleTextInProtocolGuid,
                  &mFakeDev.SimpleInput,
                  &gEfiSimpleTextInputExProtocolGuid,
                  &mFakeDev.SimpleInputEx,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return;
  }
  //
  // Connect the fake device for ConPlatform and ConSplitter
  //
  gBS->ConnectController (
         mFakeDev.Handle,
         NULL,
         NULL,
         FALSE
         );
}