/** @file
  Function prototype for USB Keyboard Driver

;******************************************************************************
;* Copyright (c) 2012 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

BOOLEAN
IsUsbKeyboard (
  IN  EFI_USB_IO_PROTOCOL       *UsbIo
  );

EFI_STATUS
InitUsbKeyboard (
  IN USB_KB_DEV   *UsbKeyboardDevice
  );

/**
  Find Key Descriptor in Key Convertion Table given its USB keycode.

  @param[in]  UsbKeyboardDevice   The USB_KB_DEV instance.
  @param[in]  KeyCode             USB Keycode.

  @return The Key Descriptor in Key Convertion Table.
          NULL means not found.

**/
EFI_KEY_DESCRIPTOR *
GetKeyDescriptor (
  IN USB_KB_DEV        *UsbKeyboardDevice,
  IN UINT8             KeyCode
  );

/**
  Find Non-Spacing key for given Key descriptor.

  @param[in]  UsbKeyboardDevice    The USB_KB_DEV instance.
  @param[in]  KeyDescriptor        Key descriptor.

  @return The Non-Spacing key corresponding to KeyDescriptor
          NULL means not found.

**/
USB_NS_KEY *
FindUsbNsKey (
  IN USB_KB_DEV          *UsbKeyboardDevice,
  IN EFI_KEY_DESCRIPTOR  *KeyDescriptor
  );

/**
  Find physical key definition for a given key descriptor.

  For a specified non-spacing key, there are a list of physical
  keys following it. This function traverses the list of
  physical keys and tries to find the physical key matching
  the KeyDescriptor.

  @param[in]  UsbNsKey          The non-spacing key information.
  @param[in]  KeyDescriptor     The key descriptor.

  @return The physical key definition.
          If no physical key is found, parameter KeyDescriptor is returned.

**/
EFI_KEY_DESCRIPTOR *
FindPhysicalKey (
  IN USB_NS_KEY          *UsbNsKey,
  IN EFI_KEY_DESCRIPTOR  *KeyDescriptor
  );

/**
  Initialize Key Convention Table by using default keyboard layout.

  @param[in, out]  UsbKeyboardDevice    The USB_KB_DEV instance.

  @retval EFI_SUCCESS          The default keyboard layout was installed successfully
  @retval Others               Failure to install default keyboard layout.
**/
EFI_STATUS
InstallDefaultKeyboardLayout (
   IN OUT USB_KB_DEV           *UsbKeyboardDevice
  );

/**
  Get current keyboard layout from HII database.

  @return Pointer to HII Keyboard Layout.
          NULL means failure occurred while trying to get keyboard layout.

**/
EFI_HII_KEYBOARD_LAYOUT *
GetCurrentKeyboardLayout (
  VOID
  );

/**
  The notification function for EFI_HII_SET_KEYBOARD_LAYOUT_EVENT_GUID.

  This function is registered to event of EFI_HII_SET_KEYBOARD_LAYOUT_EVENT_GUID
  group type, which will be triggered by EFI_HII_DATABASE_PROTOCOL.SetKeyboardLayout().
  It tries to get current keyboard layout from HII database.

  @param[in]  Event        Event being signaled.
  @param[in]  Context      Points to USB_KB_DEV instance.

**/
VOID
EFIAPI
SetKeyboardLayoutEvent (
  IN EFI_EVENT              Event,
  IN VOID                   *Context
  );

/**
  Destroy resources for keyboard layout.

  @param[in, out]  UsbKeyboardDevice    The USB_KB_DEV instance.

**/
VOID
ReleaseKeyboardLayoutResources (
  IN OUT USB_KB_DEV              *UsbKeyboardDevice
  );

/**
  Initialize USB keyboard layout.

  This function initializes Key Convertion Table for the USB keyboard device.
  It first tries to retrieve layout from HII database. If failed and default
  layout is enabled, then it just uses the default layout.

  @param[out]  UsbKeyboardDevice      The USB_KB_DEV instance.

  @retval EFI_SUCCESS            Initialization succeeded.
  @retval EFI_NOT_READY          Keyboard layout cannot be retrieve from HII
                                 database, and default layout is disabled.
  @retval Other                  Fail to register event to EFI_HII_SET_KEYBOARD_LAYOUT_EVENT_GUID group.

**/
EFI_STATUS
InitKeyboardLayout (
  OUT USB_KB_DEV   *UsbKeyboardDevice
  );

/**

  Retrieves the interface Descriptor for that controller.

  @param  This                  Indicates the calling context.
  @param  InterfaceDescriptor   A pointer to the caller allocated USB interface
                                Descriptor.

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER
  @retval EFI_NOT_FOUND

**/
EFI_STATUS
EFIAPI
UsbGetInterfaceDescriptor (
  IN  EFI_USB_IO_PROTOCOL              *This,
  OUT EFI_USB_INTERFACE_DESCRIPTOR     *InterfaceDescriptor
  );

/**
 
  Retrieves the endpoint Descriptor for a given endpoint.
 
  @param  This                  Indicates the calling context.
  @param  EndpointIndex         Indicates which endpoint descriptor to retrieve.
                                The valid range is 0..15.
  @param  EndpointDescriptor    A pointer to the caller allocated USB Endpoint
                                Descriptor of a USB controller.
 
  @retval EFI_SUCCESS           The endpoint descriptor was retrieved successfully.
  @retval EFI_INVALID_PARAMETER EndpointIndex is not valid.
                                EndpointDescriptor is NULL.
  @retval EFI_NOT_FOUND         The endpoint descriptor cannot be found.
                                The device may not be correctly configured.
 
**/
EFI_STATUS
EFIAPI
UsbGetEndpointDescriptor (
  IN  EFI_USB_IO_PROTOCOL             *This,
  IN  UINT8                           EndpointIndex,
  OUT EFI_USB_ENDPOINT_DESCRIPTOR     *EndpointDescriptor
  );

/*++

  Usb Async Interrupt Transfer

  @param  This                  Indicates calling context.
  @param  DeviceEndpoint        The destination USB device endpoint to which the
                                device request is being sent.
  @param  IsNewTransfer         If TRUE, a new transfer will be submitted to USB
                                controller.  If FALSE,  the interrupt transfer is
                                deleted from the device's interrupt transfer queue.
  @param  PollingInterval       Indicates the periodic rate, in milliseconds, that
                                the transfer is to be executed.
  @param  DataLength            Specifies the length, in bytes, of the data to be
                                received from the USB device.
  @param  InterruptCallBack     The Callback function.  This function is called if
                                the asynchronous interrupt transfer is completed.
  @param  Context               Passed to InterruptCallback

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER
  @retval EFI_OUT_OF_RESOURCES

--*/
EFI_STATUS
EFIAPI
UsbAsyncInterruptTransfer (
  IN EFI_USB_IO_PROTOCOL                 *This,
  IN UINT8                               DeviceEndpoint,
  IN BOOLEAN                             IsNewTransfer,
  IN UINTN                               PollingInterval, OPTIONAL
  IN UINTN                               DataLength, OPTIONAL
  IN EFI_ASYNC_USB_TRANSFER_CALLBACK     InterruptCallBack, OPTIONAL
  IN VOID                                *Context OPTIONAL
  );
  
EFI_STATUS
EFIAPI
KeyboardHandler (
  IN  VOID          *Data,
  IN  UINTN         DataLength,
  IN  VOID          *Context,
  IN  UINT32        Result
  );

VOID
EFIAPI
UsbKeyboardRecoveryHandler (
  IN    UINTN        Event,
  IN    VOID         *Context
  );

EFI_STATUS
UsbParseKey (
  IN OUT  USB_KB_DEV  *UsbKeyboardDevice,
  OUT     UINT8       *KeyChar
  );

EFI_STATUS
UsbKeyCodeToEFIScanCode (
  IN  USB_KB_DEV      *UsbKeyboardDevice,
  IN  UINT8           KeyChar,
  OUT EFI_INPUT_KEY   *Key
  );

USB_KB_DEV *
CreateUsbKeyboardDeviceBuffer (
  VOID
  );

VOID
InitQueue (
  IN OUT  USB_SIMPLE_QUEUE        *Queue,
  IN      UINTN                   ItemSize
  );

BOOLEAN
IsQueueEmpty (
  IN  USB_SIMPLE_QUEUE   *Queue
  );

BOOLEAN
IsQueueFull (
  IN  USB_SIMPLE_QUEUE   *Queue
  );

VOID
Enqueue (
  IN OUT  USB_SIMPLE_QUEUE *Queue,
  IN      VOID             *Item,
  IN      UINTN            ItemSize
  );

EFI_STATUS
Dequeue (
  IN OUT  USB_SIMPLE_QUEUE *Queue,
     OUT  VOID             *Item,
  IN      UINTN            ItemSize
  );

EFI_STATUS
InsertKeyCode (
  IN      USB_KB_DEV    *UsbKeyboardDevice,
  IN      UINT8         Key,
  IN      UINT8         Down
  );

VOID
EFIAPI
UsbKeyboardRepeatHandler (
  IN    UINTN        Event,
  IN    VOID         *Context
  );

EFI_STATUS
SetKeyLED (
  IN  USB_KB_DEV    *UsbKeyboardDevice
  );

VOID
EFIAPI
UsbKeyboardCheckLEDHandler (
  IN    UINTN        Event,
  IN    VOID         *Context
  );

/**
  Initialize the key state.

  @param  UsbKeyboardDevice     The USB_KB_DEV instance.
  @param  KeyState              A pointer to receive the key state information.
**/
VOID
InitializeKeyState (
  IN  USB_KB_DEV           *UsbKeyboardDevice,
  OUT EFI_KEY_STATE        *KeyState
  );

#endif
