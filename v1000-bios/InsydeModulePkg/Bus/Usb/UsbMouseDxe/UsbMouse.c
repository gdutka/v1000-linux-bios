/** @file

;******************************************************************************
;* Copyright (c) 2022, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "UsbMouse.h"
#include <PostCode.h>

//
// Prototypes
// Driver model protocol interface
//
EFI_STATUS
EFIAPI
UsbMouseDriverBindingEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  );

EFI_STATUS
EFIAPI
UsbMouseDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  );

EFI_STATUS
EFIAPI
UsbMouseDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  );

EFI_STATUS
EFIAPI
UsbMouseDriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN  EFI_HANDLE                    Controller,
  IN  UINTN                         NumberOfChildren,
  IN  EFI_HANDLE                    *ChildHandleBuffer
  );

EFI_STATUS
EFIAPI
UsbMouseUsbBindingSupported (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo,
  IN     EFI_USB_CORE_PROTOCOL          *UsbCore,
  IN     EFI_DEVICE_PATH_PROTOCOL       *DevicePath
  );

EFI_STATUS
EFIAPI
UsbMouseUsbBindingStart (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo,
  IN     EFI_USB_CORE_PROTOCOL          *UsbCore
  );

EFI_STATUS
EFIAPI
UsbMouseUsbBindingStop (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo,
  IN     VOID                           *Handle
  );

EFI_DRIVER_BINDING_PROTOCOL gUsbMouseDriverBinding = {
  UsbMouseDriverBindingSupported,
  UsbMouseDriverBindingStart,
  UsbMouseDriverBindingStop,
  0x10,
  NULL,
  NULL
};

//
// helper functions
//
STATIC
BOOLEAN
IsUsbMouse (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo
  );

STATIC
EFI_STATUS
InitializeUsbMouseDevice (
  IN  USB_MOUSE_DEV           *UsbMouseDevice
  );

STATIC
VOID
EFIAPI
UsbMouseWaitForInput (
  IN  EFI_EVENT               Event,
  IN  VOID                    *Context
  );

//
// Mouse interrupt handler
//
STATIC
EFI_STATUS
EFIAPI
OnMouseInterruptComplete (
  IN  VOID        *Data,
  IN  UINTN       DataLength,
  IN  VOID        *Context,
  IN  UINT32      Result
  );

//
// Mouse Protocol
//
STATIC
EFI_STATUS
EFIAPI
GetMouseState (
  IN   EFI_SIMPLE_POINTER_PROTOCOL  *This,
  OUT  EFI_SIMPLE_POINTER_STATE     *MouseState
  );

STATIC
EFI_STATUS
EFIAPI
UsbMouseReset (
  IN EFI_SIMPLE_POINTER_PROTOCOL    *This,
  IN BOOLEAN                        ExtendedVerification
  );

typedef struct {
  UINT16        IdVendor;
  UINT16        IdProduct;
} MOUSE_ID_TABLE;

STATIC MOUSE_ID_TABLE IncompatibleMouse[] = {
  { 0x1BCF, 0x0007 },   // KTMS258U
  { 0x04ca, 0x006d },   // Lite-on wireless mouse
  { 0x04F2, 0x1558 },   // Chicony touch pad with keyboard
  { 0x1532, 0x0029 },   // Razer DeathAdder mouse
  { 0x1D57, 0xAD03 },   // SI8028 mouse
  { 0x0e8f, 0x0020 },   // Pansignal Technology PS2 to USB cable
  { 0x04d9, 0xf105 },   // Thunderobot M303 USB mouse
  { 0x0000, 0x0000 }    // End of list
};

EFI_USB_CORE_PROTOCOL                   *mUsbCore;
BOOLEAN                                 mInSmram;

/**

  Retrieves the USB Device Descriptor.

  @param  This                  Indicates the calling context.
  @param  DeviceDescriptor      A pointer to the caller allocated USB Device
                                Descriptor.

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER
  @retval EFI_NOT_FOUND

**/
STATIC
EFI_STATUS
EFIAPI
UsbGetDeviceDescriptor (
  IN  EFI_USB_IO_PROTOCOL           *This,
  OUT EFI_USB_DEVICE_DESCRIPTOR     *DeviceDescriptor
  )
{
  EFI_STATUS                         Status;
  EFI_USB_IO_GET_DEVICE_DESCRIPTOR   UsbGetDeviceDescriptorFunc = This->UsbGetDeviceDescriptor;

  if (mInSmram) {
    Status = mUsbCore->AddressConvert (
                         SMM_ADDRESS,
                         (VOID*)(UINTN)UsbGetDeviceDescriptorFunc,
                         (VOID**)&UsbGetDeviceDescriptorFunc
                         );
    if (Status == EFI_NOT_FOUND) return Status;
  }
  return UsbGetDeviceDescriptorFunc (
           This,
           DeviceDescriptor
           );
}

/**

  Retrieves the interface Descriptor for that controller.

  @param  This                  Indicates the calling context.
  @param  InterfaceDescriptor   A pointer to the caller allocated USB interface
                                Descriptor.

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER
  @retval EFI_NOT_FOUND

**/
STATIC
EFI_STATUS
EFIAPI
UsbGetInterfaceDescriptor (
  IN  EFI_USB_IO_PROTOCOL              *This,
  OUT EFI_USB_INTERFACE_DESCRIPTOR     *InterfaceDescriptor
  )
{
  EFI_STATUS                            Status;
  EFI_USB_IO_GET_INTERFACE_DESCRIPTOR   UsbGetInterfaceDescriptorFunc = This->UsbGetInterfaceDescriptor;

  if (mInSmram) {
    Status = mUsbCore->AddressConvert (
                         SMM_ADDRESS,
                         (VOID*)(UINTN)UsbGetInterfaceDescriptorFunc,
                         (VOID**)&UsbGetInterfaceDescriptorFunc
                         );
    if (Status == EFI_NOT_FOUND) return Status;
  }
  return UsbGetInterfaceDescriptorFunc (
           This,
           InterfaceDescriptor
           );
}

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
STATIC
EFI_STATUS
EFIAPI
UsbGetEndpointDescriptor (
  IN  EFI_USB_IO_PROTOCOL             *This,
  IN  UINT8                           EndpointIndex,
  OUT EFI_USB_ENDPOINT_DESCRIPTOR     *EndpointDescriptor
  )
{
  EFI_STATUS                            Status;
  EFI_USB_IO_GET_ENDPOINT_DESCRIPTOR    UsbGetEndpointDescriptorFunc = This->UsbGetEndpointDescriptor;

  if (mInSmram) {
    Status = mUsbCore->AddressConvert (
                         SMM_ADDRESS,
                         (VOID*)(UINTN)UsbGetEndpointDescriptorFunc,
                         (VOID**)&UsbGetEndpointDescriptorFunc
                         );
    if (Status == EFI_NOT_FOUND) return Status;
  }
  return UsbGetEndpointDescriptorFunc (
           This,
           EndpointIndex,
           EndpointDescriptor
           );
}

/*++

  Usb Async Interrput Transfer

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
STATIC
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
  )
{
  EFI_STATUS                            Status;
  EFI_USB_IO_ASYNC_INTERRUPT_TRANSFER   UsbAsyncInterruptTransferFunc = This->UsbAsyncInterruptTransfer;
  
  if (mInSmram) {
    Status = mUsbCore->AddressConvert (
                         SMM_ADDRESS,
                         (VOID*)(UINTN)UsbAsyncInterruptTransferFunc,
                         (VOID**)&UsbAsyncInterruptTransferFunc
                         );
    if (Status == EFI_NOT_FOUND) return Status;
  }
  return UsbAsyncInterruptTransferFunc (
           This,
           DeviceEndpoint,
           IsNewTransfer,
           PollingInterval,
           DataLength,
           InterruptCallBack,
           Context
           );
}

/**

  Entry point for EFI drivers.

  @param  ImageHandle           EFI_HANDLE
  @param  SystemTable           EFI_SYSTEM_TABLE

  @retval EFI_SUCCESS
  @retval others

**/
EFI_STATUS
EFIAPI
UsbMouseDriverBindingEntryPoint (
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
               &gUsbMouseDriverBinding,
               ImageHandle,
               &gUsbMouseComponentName,
               &gUsbMouseComponentName2
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
                         UsbMouseUsbBindingSupported,
                         UsbMouseUsbBindingStart,
                         UsbMouseUsbBindingStop
                         );
  }
  return Status;
}

/**

  Test to see if this driver supports ControllerHandle. Any ControllerHandle
  that has UsbHcProtocol installed will be supported.

  @param  This                  Protocol instance pointer.
  @param  Controller            Handle of device to test
  @param  RemainingDevicePath   Not used

  @retval EFI_SUCCESS           This driver supports this device.
  @retval EFI_UNSUPPORTED       This driver does not support this device.

**/
EFI_STATUS
EFIAPI
UsbMouseDriverBindingSupported (
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
  OpenStatus = gBS->OpenProtocol (
                      Controller,
                      &gEfiUsbIoProtocolGuid,
                      (VOID **)&UsbIo,
                      This->DriverBindingHandle,
                      Controller,
                      EFI_OPEN_PROTOCOL_BY_DRIVER
                      );
  if (EFI_ERROR (OpenStatus) && (OpenStatus != EFI_ALREADY_STARTED)) {
    return EFI_UNSUPPORTED;
  }

  if (OpenStatus == EFI_ALREADY_STARTED) {
    return EFI_ALREADY_STARTED;
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
  // Use the USB I/O protocol interface to see the Controller is
  // the Mouse controller that can be managed by this driver.
  //
  Status = EFI_SUCCESS;
  if (!IsUsbMouse (UsbIo)) {
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

  Starting the Usb Bus Driver

  @param  This                  Protocol instance pointer.
  @param  Controller            Handle of device to test
  @param  RemainingDevicePath   Not used

  @retval EFI_SUCCESS           This driver supports this device.
  @retval EFI_UNSUPPORTED       This driver does not support this device.
  @retval EFI_DEVICE_ERROR      This driver cannot be started due to device error
  @retval EFI_OUT_OF_RESOURCES  Can't allocate memory resources
  @retval EFI_ALREADY_STARTED   Thios driver has been started

**/
EFI_STATUS
EFIAPI
UsbMouseDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                  Status;
  EFI_USB_IO_PROTOCOL         *UsbIo;
  EFI_USB_ENDPOINT_DESCRIPTOR EndpointDesc;
  USB_MOUSE_DEV               *UsbMouseDevice;
  UINT8                       EndpointNumber;
  UINT8                       Index;
  UINT8                       EndpointAddr;
  UINT8                       PollingInterval;
  UINT8                       PacketSize;
  BOOLEAN                     EndpointFound;
  USB_DEVICE                  UsbDevice;

  POST_CODE (BDS_CONNECT_USB_DEVICE);

  UsbMouseDevice  = NULL;
  Status          = EFI_SUCCESS;

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiUsbIoProtocolGuid,
                  (VOID **)&UsbIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }
  Status = mUsbCore->AllocateBuffer (
                       sizeof (USB_MOUSE_DEV),
                       ALIGNMENT_32,
                       (VOID **)&UsbMouseDevice
                       );
  if (EFI_ERROR(Status)) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  UsbMouseDevice->UsbIo               = UsbIo;
  UsbMouseDevice->Signature           = USB_MOUSE_DEV_SIGNATURE;

  //
  // Get the Device Path Protocol on Controller's handle
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &UsbMouseDevice->DevicePath,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }

  //
  // Report Status Code here since USB mouse will be detected next.
  //
  MouseReportStatusCode (
    UsbMouseDevice,
    EFI_PROGRESS_CODE,
    (EFI_PERIPHERAL_MOUSE | EFI_P_PC_PRESENCE_DETECT)
    );

  //
  // Get interface & endpoint descriptor
  //
  UsbGetInterfaceDescriptor (
    UsbIo,
    &UsbMouseDevice->InterfaceDescriptor
    );

  EndpointNumber = UsbMouseDevice->InterfaceDescriptor.NumEndpoints;

  for (Index = 0, EndpointFound = FALSE; Index < EndpointNumber; Index++) {
    UsbGetEndpointDescriptor (
      UsbIo,
      Index,
      &EndpointDesc
      );

    if ((EndpointDesc.Attributes & 0x03) == 0x03 && (EndpointDesc.EndpointAddress & 0x80)) {
      //
      // We only care interrupt-in endpoint here
      //
      UsbMouseDevice->IntEndpointDescriptor = EndpointDesc;
      EndpointFound = TRUE;
      break;
    }
  }

  if (!EndpointFound) {
    //
    // Report Status Code to indicate that there is no USB mouse
    //
    MouseReportStatusCode (
      UsbMouseDevice,
      EFI_ERROR_CODE | EFI_ERROR_MINOR,
      (EFI_PERIPHERAL_MOUSE | EFI_P_EC_NOT_DETECTED)
      );
    //
    // No interrupt endpoint, then error
    //
    Status = EFI_UNSUPPORTED;
    goto ErrorExit;
  }

  //
  // Report Status Code here since USB mouse has be detected.
  //
  MouseReportStatusCode (
    UsbMouseDevice,
    EFI_PROGRESS_CODE,
    (EFI_PERIPHERAL_MOUSE | EFI_P_PC_DETECTED)
    );

  Status = InitializeUsbMouseDevice (UsbMouseDevice);
  if (EFI_ERROR (Status)) {
    MouseReportStatusCode (
      UsbMouseDevice,
      EFI_ERROR_CODE | EFI_ERROR_MINOR,
      (EFI_PERIPHERAL_MOUSE | EFI_P_EC_INTERFACE_ERROR)
      );

    goto ErrorExit;
  }

  UsbMouseDevice->SimplePointerProtocol.GetState  = GetMouseState;
  UsbMouseDevice->SimplePointerProtocol.Reset     = UsbMouseReset;
  UsbMouseDevice->SimplePointerProtocol.Mode      = &UsbMouseDevice->Mode;

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_WAIT,
                  TPL_NOTIFY,
                  UsbMouseWaitForInput,
                  UsbMouseDevice,
                  &((UsbMouseDevice->SimplePointerProtocol).WaitForInput)
                  );
  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }

  Status = gBS->InstallProtocolInterface (
                  &Controller,
                  &gEfiSimplePointerProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &UsbMouseDevice->SimplePointerProtocol
                  );

  if (EFI_ERROR (Status)) {
    Status = EFI_DEVICE_ERROR;
    goto ErrorExit;
  }

  //
  // After Enabling Async Interrupt Transfer on this mouse Device
  // we will be able to get key data from it. Thus this is deemed as
  // the enable action of the mouse
  //

  MouseReportStatusCode (
    UsbMouseDevice,
    EFI_PROGRESS_CODE,
    (EFI_PERIPHERAL_MOUSE | EFI_P_PC_ENABLE)
    );

  //
  // submit async interrupt transfer
  //
  EndpointAddr    = UsbMouseDevice->IntEndpointDescriptor.EndpointAddress;
  PollingInterval = UsbMouseDevice->IntEndpointDescriptor.Interval;
  PacketSize      = (UINT8) (UsbMouseDevice->IntEndpointDescriptor.MaxPacketSize);

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
             OnMouseInterruptComplete,
             UsbMouseDevice
             );

  if (!EFI_ERROR (Status)) {

    UsbMouseDevice->ControllerNameTable = NULL;
    AddUnicodeString2 (
      LANGUAGE_CODE_ENGLISH_ISO639,
      gUsbMouseComponentName.SupportedLanguages,
      &UsbMouseDevice->ControllerNameTable,
      CONTROLLER_DRIVER_NAME,
      TRUE
      );
    AddUnicodeString2 (
      LANGUAGE_CODE_ENGLISH_RFC4646,
      gUsbMouseComponentName2.SupportedLanguages,
      &UsbMouseDevice->ControllerNameTable,
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
    UsbDevice.UsbHID.Instance = UsbMouseDevice;
    UsbDevice.UsbHID.SyncLED  = NULL;
    mUsbCore->InsertUsbDevice (
                &UsbDevice
                );
    //
    // Setup Smm address convert table for Smm security policy
    //
    mUsbCore->InsertAddressConvertTable (
                ACT_INSTANCE_BODY,
                UsbMouseDevice,
                sizeof (USB_MOUSE_DEV)
                );
    mUsbCore->InsertAddressConvertTable (
                ACT_INSTANCE_POINTER,
                &UsbMouseDevice->UsbIo,
                1
                );
    mUsbCore->InsertAddressConvertTable (
                ACT_INSTANCE_POINTER,
                &UsbMouseDevice->RecoveryPollingHandle,
                1
                );
    return EFI_SUCCESS;
  }

  //
  // If submit error, uninstall that interface
  //
  Status = EFI_DEVICE_ERROR;
  gBS->UninstallProtocolInterface (
        Controller,
        &gEfiSimplePointerProtocolGuid,
        &UsbMouseDevice->SimplePointerProtocol
        );

ErrorExit:
  if (EFI_ERROR (Status)) {
    gBS->CloseProtocol (
          Controller,
          &gEfiUsbIoProtocolGuid,
          This->DriverBindingHandle,
          Controller
          );

    if (UsbMouseDevice != NULL) {

      if ((UsbMouseDevice->SimplePointerProtocol).WaitForInput != NULL) {
        gBS->CloseEvent ((UsbMouseDevice->SimplePointerProtocol).WaitForInput);
      }
      mUsbCore->FreeBuffer (
                  sizeof (USB_MOUSE_DEV),
                  UsbMouseDevice
                  );
    }
  }

  return Status;
}

/**

  Stop this driver on ControllerHandle. Support stoping any child handles
  created by this driver.

  @param  This                  Protocol instance pointer.
  @param  Controller            Handle of device to stop driver on
  @param  NumberOfChildren      Number of Children in the ChildHandleBuffer
  @param  ChildHandleBuffer     List of handles for the children we need to stop.

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  @retval others

**/
EFI_STATUS
EFIAPI
UsbMouseDriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN  EFI_HANDLE                    Controller,
  IN  UINTN                         NumberOfChildren,
  IN  EFI_HANDLE                    *ChildHandleBuffer
  )
{
  EFI_STATUS                  Status;
  USB_MOUSE_DEV               *UsbMouseDevice;
  EFI_SIMPLE_POINTER_PROTOCOL *SimplePointerProtocol;
  EFI_USB_IO_PROTOCOL         *UsbIo;

  //
  // Get our context back.
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiSimplePointerProtocolGuid,
                  (VOID **)&SimplePointerProtocol,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  UsbMouseDevice = USB_MOUSE_DEV_FROM_MOUSE_PROTOCOL (SimplePointerProtocol);
  UsbIo   = UsbMouseDevice->UsbIo;
  //
  // Set the UsbIo to NULL to preventing async interrupt transfer restarted
  // by recovery handler after deleting the async interrupt transfer
  //
  UsbMouseDevice->UsbIo = NULL;

  gBS->CloseProtocol (
        Controller,
        &gEfiSimplePointerProtocolGuid,
        This->DriverBindingHandle,
        Controller
        );
  //
  // Remove the recovery handler before deleting the async interrupt transfer to preventing
  // async interrupt transfer restarted by recovery handler
  //
  mUsbCore->RemovePeriodicTimer(UsbMouseDevice->RecoveryPollingHandle);
  //
  // Uninstall the Asyn Interrupt Transfer from this device
  // will disable the mouse data input from this device
  //
  MouseReportStatusCode (
    UsbMouseDevice,
    EFI_PROGRESS_CODE,
    (EFI_PERIPHERAL_MOUSE | EFI_P_PC_DISABLE)
    );

  //
  // Delete Mouse Async Interrupt Transfer
  //
  UsbAsyncInterruptTransfer (
    UsbIo,
    UsbMouseDevice->IntEndpointDescriptor.EndpointAddress,
    FALSE,
    UsbMouseDevice->IntEndpointDescriptor.Interval,
    0,
    NULL,
    NULL
    );

  gBS->CloseEvent (UsbMouseDevice->SimplePointerProtocol.WaitForInput);

  Status = gBS->UninstallProtocolInterface (
                  Controller,
                  &gEfiSimplePointerProtocolGuid,
                  &UsbMouseDevice->SimplePointerProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  gBS->CloseProtocol (
         Controller,
         &gEfiUsbIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  if (UsbMouseDevice->ControllerNameTable) {
    FreeUnicodeStringTable (UsbMouseDevice->ControllerNameTable);
  }

  mUsbCore->FreeBuffer (
              sizeof (USB_MOUSE_DEV),
              UsbMouseDevice
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
              UsbMouseDevice
              );
  mUsbCore->RemoveAddressConvertTable (
              ACT_INSTANCE_POINTER,
              &UsbMouseDevice->UsbIo
              );
  mUsbCore->RemoveAddressConvertTable (
              ACT_INSTANCE_POINTER,
              &UsbMouseDevice->RecoveryPollingHandle
              );
  return EFI_SUCCESS;
}

/**

  Supported for UsbIo(called from UsbCore).

  @param  UsbIo                 UsbIo

  @retval EFI_UNSUPPORTED
  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
UsbMouseUsbBindingSupported (
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
  if (IsUsbMouse (UsbIo)) {
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
UsbMouseUsbBindingStart (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo,
  IN     EFI_USB_CORE_PROTOCOL          *UsbCore
  )
{
  EFI_STATUS                  Status;
  EFI_USB_ENDPOINT_DESCRIPTOR EndpointDesc;
  USB_MOUSE_DEV               *UsbMouseDevice;
  UINT8                       EndpointNumber;
  UINT8                       Index;
  UINT8                       EndpointAddr;
  UINT8                       PollingInterval;
  UINT8                       PacketSize;
  BOOLEAN                     EndpointFound;
  USB_DEVICE                  UsbDevice;

  POST_CODE (BDS_CONNECT_USB_DEVICE);

  UsbMouseDevice  = NULL;
  Status = mUsbCore->AllocateBuffer (
                       sizeof (USB_MOUSE_DEV),
                       ALIGNMENT_32,
                       (VOID **)&UsbMouseDevice
                       );
  if (EFI_ERROR(Status)) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  UsbMouseDevice->UsbIo               = UsbIo;
  UsbMouseDevice->Signature           = USB_MOUSE_DEV_SIGNATURE;
  //
  // Get interface & endpoint descriptor
  //
  UsbGetInterfaceDescriptor (
    UsbIo,
    &UsbMouseDevice->InterfaceDescriptor
    );

  EndpointNumber = UsbMouseDevice->InterfaceDescriptor.NumEndpoints;

  for (Index = 0, EndpointFound = FALSE; Index < EndpointNumber; Index++) {
    UsbGetEndpointDescriptor (
      UsbIo,
      Index,
      &EndpointDesc
      );

    if ((EndpointDesc.Attributes & 0x03) == 0x03 && (EndpointDesc.EndpointAddress & 0x80)) {
      //
      // We only care interrupt-in endpoint here
      //
      UsbMouseDevice->IntEndpointDescriptor = EndpointDesc;
      EndpointFound = TRUE;
      break;
    }
  }

  if (!EndpointFound) {
    //
    // No interrupt endpoint, then error
    //
    Status = EFI_UNSUPPORTED;
    goto ErrorExit;
  }

  Status = InitializeUsbMouseDevice (UsbMouseDevice);
  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }
  //
  // submit async interrupt transfer
  //
  EndpointAddr    = UsbMouseDevice->IntEndpointDescriptor.EndpointAddress;
  PollingInterval = UsbMouseDevice->IntEndpointDescriptor.Interval;
  PacketSize      = (UINT8) (UsbMouseDevice->IntEndpointDescriptor.MaxPacketSize);

  Status = UsbAsyncInterruptTransfer (
             UsbIo,
             EndpointAddr,
             TRUE,
             PollingInterval,
             PacketSize,
             OnMouseInterruptComplete,
             UsbMouseDevice
             );

  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }
  //
  // Insert UsbIoDevice into UsbCore for UsbLegacy
  //
  UsbDevice.UsbHID.Type     = USB_CORE_USB_HID;
  UsbDevice.UsbHID.UsbIo    = UsbIo;
  UsbDevice.UsbHID.Instance = UsbMouseDevice;
  UsbDevice.UsbHID.SyncLED  = NULL;
  mUsbCore->InsertUsbDevice (
              &UsbDevice
              );
  return EFI_SUCCESS;

ErrorExit:
  if (UsbMouseDevice != NULL) {
    mUsbCore->FreeBuffer (
                sizeof (USB_MOUSE_DEV),
                UsbMouseDevice
                );
  }
  return Status;
}

/**

  Supported for UsbIo(called from UsbCore).

  @param  Handle                The instance of UsbKeyboard allocated by UsbIoBindingStart

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
UsbMouseUsbBindingStop (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo,
  IN     VOID                           *Handle
  )
{
  USB_MOUSE_DEV               *UsbMouseDevice;
  UINTN                       Mode;

  UsbMouseDevice = (USB_MOUSE_DEV*)Handle;
  if (UsbIo != UsbMouseDevice->UsbIo) {
    //
    // Due to UsbCore may pass wrong Handle for us
    //
    return EFI_NOT_FOUND;
  }
  //
  // Remove the recovery handler before deleting the async interrupt transfer to preventing
  // async interrupt transfer restarted by recovery handler
  //
  mUsbCore->RemovePeriodicTimer(UsbMouseDevice->RecoveryPollingHandle);
  //
  // Delete Mouse Async Interrupt Transfer
  //
  UsbAsyncInterruptTransfer (
    UsbIo,
    UsbMouseDevice->IntEndpointDescriptor.EndpointAddress,
    FALSE,
    UsbMouseDevice->IntEndpointDescriptor.Interval,
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
                sizeof (USB_MOUSE_DEV),
                UsbMouseDevice
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

  Tell if a Usb Controller is a mouse

  @param  UsbIo                 UsbIo protocol pointer.
  @param  UsbCore               UsbCore protocol pointer

  @retval TRUE                  It is a mouse
  @retval FALSE                 It is not a mouse

**/
BOOLEAN
IsUsbMouse (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo
  )
{
  EFI_STATUS                    Status;
  EFI_USB_INTERFACE_DESCRIPTOR  InterfaceDescriptor;
  EFI_USB_HID_DESCRIPTOR        MouseHidDesc;
  UINT8                         *ReportDesc;
  UINT16                        DescriptorLength;
  BOOLEAN                       MouseDeviceFound;
  REPORT_FIELD_INFO             ReportFieldInfo;
  UINTN                         Index;

  //
  // Get the Default interface descriptor, now we only
  // suppose it is interface 1
  //
  Status = UsbGetInterfaceDescriptor (
             UsbIo,
             &InterfaceDescriptor
             );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  if (InterfaceDescriptor.InterfaceClass == CLASS_HID) {
    if (InterfaceDescriptor.InterfaceSubClass == SUBCLASS_BOOT &&
        InterfaceDescriptor.InterfaceProtocol == PROTOCOL_MOUSE) {
      return TRUE;
    }
    if (!((InterfaceDescriptor.InterfaceProtocol == PROTOCOL_MOUSE) ||
         (InterfaceDescriptor.InterfaceProtocol == PROTOCOL_NONE && InterfaceDescriptor.InterfaceSubClass == SUBCLASS_NONE))) {
      return FALSE;
    }
    Status = mUsbCore->UsbGetHidDescriptor (
                         UsbIo,
                         InterfaceDescriptor.InterfaceNumber,
                         &MouseHidDesc
                         );
    if (EFI_ERROR (Status) || (InterfaceDescriptor.InterfaceProtocol == PROTOCOL_NONE && MouseHidDesc.HidClassDesc[0].DescriptorType != 0x22)) {
      return FALSE;
    }
    DescriptorLength = MouseHidDesc.HidClassDesc[0].DescriptorLength;
    Status = mUsbCore->AllocateBuffer (
                         DescriptorLength,
                         ALIGNMENT_32,
                         (VOID **)&ReportDesc
                         );
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
    MouseDeviceFound = FALSE;
    for (Index = 0; Index < 3; Index ++) {
      Status = mUsbCore->UsbGetReportDescriptor (
                           UsbIo,
                           InterfaceDescriptor.InterfaceNumber,
                           DescriptorLength,
                           ReportDesc
                           );
      if (!EFI_ERROR (Status)) {
        break;
      } else if (Status == EFI_DEVICE_ERROR) {
        //
        // Stall 10ms to waiting for potential signal unstable
        //
        mUsbCore->Stall (10 * 1000);
      }
    }
    if (!EFI_ERROR (Status) &&
        (MatchHidDeviceType (ReportDesc, DescriptorLength, MOUSE_CLASS_CODE) ||
         MatchHidDeviceType (ReportDesc, DescriptorLength, POINTER_CLASS_CODE)) &&
        !MatchHidDeviceType (ReportDesc, DescriptorLength, TOUCH_SCREEN_CLASS_CODE) &&
        !MatchHidDeviceType (ReportDesc, DescriptorLength, PEN_CLASS_CODE)) {
      //
      // Check the X/Y value type, we only support the relative value due to it is requirement for SimplePointer protocol
      //
      ZeroMem (&ReportFieldInfo, sizeof (REPORT_FIELD_INFO));
      Status = ParseReportDescriptor (
                 ReportDesc,
                 DescriptorLength,
                 &ReportFieldInfo
                 );
      if (EFI_ERROR (Status)) {
        return FALSE;
      }
      //
      // Searching for qualified mouse report
      //
      for (Index = 0; Index < ReportFieldInfo.Total; Index ++) {
        if (ReportFieldInfo.ReportGroup[Index].DataValid &&
            ReportFieldInfo.ReportGroup[Index].DataAttr == ATTR_MOUSE_INPUT &&
            ReportFieldInfo.ReportGroup[Index].Data.Mouse.FieldX.ValueType == RELATIVE_VALUE &&
            ReportFieldInfo.ReportGroup[Index].Data.Mouse.FieldY.ValueType == RELATIVE_VALUE) {
          MouseDeviceFound = TRUE;
          break;
        }
      }
    }
    mUsbCore->FreeBuffer (
                DescriptorLength,
                ReportDesc
                );
    if (MouseDeviceFound) return TRUE;
  }
  return FALSE;
}

EFI_STATUS
SetDeviceMode (
  IN  USB_MOUSE_DEV       *UsbMouseDevice
  )
{
  EFI_STATUS              Status;
  REPORT_GROUP            *ReportGroup;
  UINT8                   *ReportData;

  if ((UsbMouseDevice == NULL) ||
      (UsbMouseDevice->UsbIo == NULL) ||
      (UsbMouseDevice->ReportFieldInfo.DeviceConfigurationReportGroup == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  ReportGroup = UsbMouseDevice->ReportFieldInfo.DeviceConfigurationReportGroup;
  //
  // Since Device Configuration must have its own report (see HUTRR34.pdf), Report ID must be
  // reported by the USB touch pad device and can not be the reserved value of 0.
  //
  if ((ReportGroup->Id == 0) || (!(ReportGroup->DataValid))) {
    return EFI_UNSUPPORTED;
  }
  Status = mUsbCore->AllocateBuffer (
                       ReportGroup->DataSize,
                       ALIGNMENT_32,
                       (VOID **)&ReportData
                       );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Set Device Mode to Mouse
  //
  ReportData[0] = ReportGroup->Id;

  Status = SetReportFieldValue (
             ReportData,
             ReportGroup->DataSize,
             &ReportGroup->Data.TouchPanelMode.FieldDeviceMode,
             HID_DIGITIZERS_DEVICE_MODE_MOUSE
             );
  if (EFI_ERROR (Status)) {
    goto EXIT;
  }
  if (ReportGroup->Data.TouchPanelMode.FieldDeviceId.DataValid) {
    //
    // Use a Device ID of 0 to configure all the report groups (i.e. top-level collections)
    // at the same time regardless of Device ID (see HUTRR34.pdf).
    //
    Status = SetReportFieldValue (
               ReportData,
               ReportGroup->DataSize,
               &ReportGroup->Data.TouchPanelMode.FieldDeviceId,
               0
               );
    if (EFI_ERROR (Status)) {
      goto EXIT;
    }
  }
  Status = mUsbCore->UsbSetReportRequest (
                       UsbMouseDevice->UsbIo,
                       UsbMouseDevice->InterfaceDescriptor.InterfaceNumber,
                       ReportGroup->Id,
                       HID_FEATURE_REPORT,
                       ReportGroup->DataSize,
                       ReportData
                       );
EXIT:
  mUsbCore->FreeBuffer (
              ReportGroup->DataSize,
              ReportData
              );
  return Status;
}


/**

  Initialize the Usb Mouse Device.

  @param  UsbMouseDevice        Device instance to be initialized

  @retval EFI_SUCCESS           Success
  @retval EFI_DEVICE_ERROR      Init error.
  @retval EFI_OUT_OF_RESOURCES  Can't allocate memory

**/
STATIC
EFI_STATUS
InitializeUsbMouseDevice (
  IN  USB_MOUSE_DEV           *UsbMouseDevice
  )
{
  EFI_USB_IO_PROTOCOL       *UsbIo;
  UINT8                     Protocol;
  BOOLEAN                   ProtocolChange;
  EFI_STATUS                Status;
  EFI_USB_HID_DESCRIPTOR    MouseHidDesc;
  UINT8                     *ReportDesc;
  EFI_USB_DEVICE_DESCRIPTOR DeviceDescriptor;
  UINTN                     Index;
  REPORT_GROUP              *ReportGroup;
  UINT16                    DescriptorLength;
  EFI_USB_CONFIG_DESCRIPTOR ConfigDesc;

  UsbIo            = UsbMouseDevice->UsbIo;
  DescriptorLength = 0;
  ReportDesc       = NULL;

  //
  // Get HID descriptor
  //
  Status = mUsbCore->UsbGetHidDescriptor (
                       UsbIo,
                       UsbMouseDevice->InterfaceDescriptor.InterfaceNumber,
                       &MouseHidDesc
                       );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get Report descriptor
  //
  if (MouseHidDesc.HidClassDesc[0].DescriptorType != 0x22) {
    return EFI_UNSUPPORTED;
  }
  if (UsbMouseDevice->InterfaceDescriptor.InterfaceNumber != 0) {
    //
    // Stall 5ms for non-first interface devices to workaround some
    // slow devices failed on accepting subsequently command issue
    //
    mUsbCore->Stall (5 * 1000);
  }
  //
  // Set appropriate protocol to the mouse interface
  //
  Protocol       = REPORT_PROTOCOL;
  ProtocolChange = FALSE;
  if (UsbMouseDevice->InterfaceDescriptor.InterfaceSubClass == SUBCLASS_BOOT) {
    Status = mUsbCore->UsbGetProtocolRequest (
                         UsbIo,
                         UsbMouseDevice->InterfaceDescriptor.InterfaceNumber,
                         &Protocol
                         );
    if (!EFI_ERROR (Status)) {
      //
      // Stall 2ms after GetProtocol command to workaround slow devices failed on
      // accepting subsequently command issue
      //
      mUsbCore->Stall (2 * 1000);
      if (Protocol == REPORT_PROTOCOL) {
        //
        // Apply boot protocol to multiple interface device to workaround some
        // multiple interface device unable to accept different protocol in each
        // of interface
        //
        Status = UsbIo->UsbGetConfigDescriptor (
                          UsbIo,
                          &ConfigDesc
                          );
        if (!EFI_ERROR (Status) && ConfigDesc.NumInterfaces > 1) {
          Protocol = BOOT_PROTOCOL;
          ProtocolChange = TRUE;
        }
      }
      if (Protocol == BOOT_PROTOCOL) {
        //
        // Get device descriptor for VID/PID for Incompatible BootProtocol Mouse detection
        //
        UsbGetDeviceDescriptor (
          UsbIo,
          &DeviceDescriptor
          );
        //
        // Incompatible BootProtocol Mouse detection
        //
        for (Index = 0; IncompatibleMouse[Index].IdVendor != 0; Index ++) {
          if (IncompatibleMouse[Index].IdVendor == DeviceDescriptor.IdVendor && IncompatibleMouse[Index].IdProduct == DeviceDescriptor.IdProduct) {
            Protocol = REPORT_PROTOCOL;
            ProtocolChange = TRUE;
            break;
          }
        }
      }
      if (ProtocolChange) {
        mUsbCore->UsbSetProtocolRequest (
                    UsbIo,
                    UsbMouseDevice->InterfaceDescriptor.InterfaceNumber,
                    Protocol
                    );
      }
    }
  }
  if (Protocol == REPORT_PROTOCOL) {
    //
    // Set to report protocol explicitly in case some device doesn't in report protocol
    //
    if (!ProtocolChange) {
      mUsbCore->UsbSetProtocolRequest (
                  UsbIo,
                  UsbMouseDevice->InterfaceDescriptor.InterfaceNumber,
                  REPORT_PROTOCOL
                  );
    }
    DescriptorLength = MouseHidDesc.HidClassDesc[0].DescriptorLength;
    Status = mUsbCore->AllocateBuffer (
                         DescriptorLength,
                         ALIGNMENT_32,
                         (VOID **)&ReportDesc
                         );

    if (EFI_ERROR(Status)) {
      return EFI_OUT_OF_RESOURCES;
    }

    Status = mUsbCore->UsbGetReportDescriptor (
                         UsbIo,
                         UsbMouseDevice->InterfaceDescriptor.InterfaceNumber,
                         DescriptorLength,
                         ReportDesc
                         );

    if (EFI_ERROR (Status)) {
      goto ErrorExit;
    }

    //
    // Parse report descriptor for set device mode to mouse
    //
    Status = ParseReportDescriptor (
               ReportDesc,
               DescriptorLength,
               &UsbMouseDevice->ReportFieldInfo
               );
    if (EFI_ERROR (Status)) {
      goto ErrorExit;
    }

    //
    // If Device Configuration feature report is found, this touch pad device has multiple operating
    // modes (mouse, single-input, multi-input, etc.).  Some devices only send "correct" report data
    // (i.e. absolute coordinates for digitizer as opposed to relative displacements for mouse) when
    // they are operating in mouse mode.  However, some other devices do not change operating
    // mode after SetDeviceMode (), but can still work with their report data; thus, we do not check
    // the return status of SetDeviceMode ().
    //
    if (UsbMouseDevice->ReportFieldInfo.DeviceConfigurationReportGroup != NULL) {
      SetDeviceMode (UsbMouseDevice);
    }
    //
    // Searching for mouse report
    //
    for (Index = 0, ReportGroup = NULL; Index < UsbMouseDevice->ReportFieldInfo.Total; Index ++) {
      if (UsbMouseDevice->ReportFieldInfo.ReportGroup[Index].DataValid &&
          (UsbMouseDevice->ReportFieldInfo.ReportGroup[Index].DataClass == MOUSE_CLASS_CODE ||
           UsbMouseDevice->ReportFieldInfo.ReportGroup[Index].DataClass == POINTER_CLASS_CODE) &&
          UsbMouseDevice->ReportFieldInfo.ReportGroup[Index].DataType == HID_MAIN_ITEM_TAG_INPUT &&
          UsbMouseDevice->ReportFieldInfo.ReportGroup[Index].DataSize > 0) {
        ReportGroup =  &UsbMouseDevice->ReportFieldInfo.ReportGroup[Index];
        break;
      }
    }
    if (ReportGroup == NULL) {
      Status = EFI_UNSUPPORTED;
      goto ErrorExit;
    }

  } else {
    //
    // Set ReportFieldInfo for boot protocol when target protocol is boot protocol
    //
    UsbMouseDevice->ReportFieldInfo.Total = 1;
    ReportGroup = &UsbMouseDevice->ReportFieldInfo.ReportGroup[0];
    ReportGroup->DataValid = TRUE;
    ReportGroup->DataClass = MOUSE_CLASS_CODE;
    ReportGroup->DataType  = HID_MAIN_ITEM_TAG_INPUT;
    ReportGroup->DataSize  = 3;
    ReportGroup->Id        = 0;
    ReportGroup->Data.Mouse.FieldButton1.DataValid = TRUE;
    ReportGroup->Data.Mouse.FieldButton1.BitLength = 1;
    ReportGroup->Data.Mouse.FieldButton1.BitOffset = 0;
    ReportGroup->Data.Mouse.FieldButton1.Min       = 0;
    ReportGroup->Data.Mouse.FieldButton1.Max       = 1;
    ReportGroup->Data.Mouse.FieldButton2.DataValid = TRUE;
    ReportGroup->Data.Mouse.FieldButton2.BitLength = 1;
    ReportGroup->Data.Mouse.FieldButton2.BitOffset = 1;
    ReportGroup->Data.Mouse.FieldButton2.Min       = 0;
    ReportGroup->Data.Mouse.FieldButton2.Max       = 1;
    ReportGroup->Data.Mouse.FieldX.DataValid       = TRUE;
    ReportGroup->Data.Mouse.FieldX.BitLength       = 8;
    ReportGroup->Data.Mouse.FieldX.BitOffset       = 8;
    ReportGroup->Data.Mouse.FieldX.Min             = 0x80;
    ReportGroup->Data.Mouse.FieldX.Max             = 0x7f;
    ReportGroup->Data.Mouse.FieldY.DataValid       = TRUE;
    ReportGroup->Data.Mouse.FieldY.BitLength       = 8;
    ReportGroup->Data.Mouse.FieldY.BitOffset       = 16;
    ReportGroup->Data.Mouse.FieldY.Min             = 0x80;
    ReportGroup->Data.Mouse.FieldY.Max             = 0x7f;
    ReportGroup->Data.Mouse.FieldZ.DataValid       = TRUE;
    ReportGroup->Data.Mouse.FieldZ.BitLength       = 8;
    ReportGroup->Data.Mouse.FieldZ.BitOffset       = 24;
    ReportGroup->Data.Mouse.FieldZ.Min             = 0x80;
    ReportGroup->Data.Mouse.FieldZ.Max             = 0x7f;
  }

  UsbMouseDevice->ReportId = ReportGroup->Id;
  if (ReportGroup->Data.Mouse.FieldButton1.DataValid) {
    UsbMouseDevice->Mode.LeftButton = TRUE;
  }
  if (ReportGroup->Data.Mouse.FieldButton2.DataValid) {
    UsbMouseDevice->Mode.RightButton = TRUE;
  }
  if (ReportGroup->Data.Mouse.FieldX.DataValid) {
    UsbMouseDevice->Mode.ResolutionX = 8;
  }
  if (ReportGroup->Data.Mouse.FieldY.DataValid) {
    UsbMouseDevice->Mode.ResolutionY = 8;
  }
  if (ReportGroup->Data.Mouse.FieldZ.DataValid) {
    UsbMouseDevice->Mode.ResolutionZ = 8;
  }
  //
  // Set indefinite Idle rate for USB Mouse
  //
  mUsbCore->UsbSetIdleRequest (
              UsbIo,
              UsbMouseDevice->InterfaceDescriptor.InterfaceNumber,
              0,
              0
              );
  if (UsbMouseDevice->RecoveryPollingHandle) {
    mUsbCore->RemovePeriodicTimer(UsbMouseDevice->RecoveryPollingHandle);
    UsbMouseDevice->RecoveryPollingHandle = NULL;
  }
ErrorExit:
  mUsbCore->FreeBuffer (
              DescriptorLength,
              ReportDesc
              );
  return Status;
}

#ifndef MIN_INT32
#define MIN_INT32  (((INT32) -2147483647) - 1)
#endif

INT32
PlusRelativeMovement (
  IN INT32             FirstInteger,
  IN INT32             SecondInteger
  )
{
  if (SecondInteger > 0) {
    if (FirstInteger > MAX_INT32 - SecondInteger) {
      FirstInteger = MAX_INT32;
    } else {
      FirstInteger += SecondInteger;
    }
  } else {
    if (FirstInteger > (INT32)(MIN_INT32 - SecondInteger)) {
      FirstInteger += SecondInteger;
    } else {
      FirstInteger = MIN_INT32;
    }
  }
  return FirstInteger;
}

/**

  It is called whenever there is data received from async interrupt
  transfer.

  @param  Data                  Data received.
  @param  DataLength            Length of Data
  @param  Context               Passed in context
  @param  Result                Async Interrupt Transfer result

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR

**/
STATIC
EFI_STATUS
EFIAPI
OnMouseInterruptComplete (
  IN  VOID        *Data,
  IN  UINTN       DataLength,
  IN  VOID        *Context,
  IN  UINT32      Result
  )
{
  USB_MOUSE_DEV         *UsbMouseDevice;
  EFI_USB_IO_PROTOCOL   *UsbIo;
  UINT8                 EndpointAddr;
  UINT32                UsbResult;
  UINTN                 Mode;
  UINTN                 Count;
  INT8                  MouseCode[4];
  UINTN                 Index;
  REPORT_GROUP          *ReportGroup;
  BOOLEAN               LeftButton;
  BOOLEAN               RightButton;
  UINTN                 BufferIn;
  INT32                 RelativeMovementX;
  INT32                 RelativeMovementY;
  INT32                 RelativeMovementZ;
  EFI_SIMPLE_POINTER_STATE      *MouseState;

  UsbMouseDevice  = (USB_MOUSE_DEV *) Context;
  UsbIo           = UsbMouseDevice->UsbIo;
  if (UsbIo == NULL) return EFI_DEVICE_ERROR;
  //
  // Get current mode
  //
  mUsbCore->GetMode(&Mode);
  //
  // Analyzes the Result and performs corresponding action.
  //
  if (Result != EFI_USB_NOERROR) {
    //
    // Some errors happen during the process
    //
    MouseReportStatusCode (
      UsbMouseDevice,
      EFI_ERROR_CODE | EFI_ERROR_MINOR,
      (EFI_PERIPHERAL_MOUSE | EFI_P_EC_INPUT_ERROR)
      );

    if ((Result & EFI_USB_ERR_STALL) == EFI_USB_ERR_STALL) {
      EndpointAddr = UsbMouseDevice->IntEndpointDescriptor.EndpointAddress;

      mUsbCore->UsbClearEndpointHalt (
                  UsbIo,
                  EndpointAddr,
                  &UsbResult
                  );
    }

    UsbAsyncInterruptTransfer (
      UsbIo,
      UsbMouseDevice->IntEndpointDescriptor.EndpointAddress,
      FALSE,
      0,
      0,
      NULL,
      NULL
      );
    //
    // Check is the device be detached
    //
    if (mUsbCore->CheckDeviceDetached(UsbIo) == EFI_SUCCESS) {
      return EFI_DEVICE_ERROR;
    }
    mUsbCore->InsertPeriodicTimer(
                USB_CORE_ONCE_TIMER,
                UsbMouseRecoveryHandler,
                UsbMouseDevice,
                EFI_USB_INTERRUPT_DELAY / 10000,
                &UsbMouseDevice->RecoveryPollingHandle
                );

    return EFI_DEVICE_ERROR;
  }
  if (DataLength == 0 || Data == NULL) {
    return EFI_SUCCESS;
  }
  ReportGroup = &UsbMouseDevice->ReportFieldInfo.ReportGroup[0];
  if (UsbMouseDevice->ReportId != 0) {
    for (Index = 0; Index < UsbMouseDevice->ReportFieldInfo.Total; Index++) {
      if ((UsbMouseDevice->ReportFieldInfo.ReportGroup[Index].Id == (*(UINT8*)Data)) &&
          (UsbMouseDevice->ReportFieldInfo.ReportGroup[Index].DataValid)) {
        ReportGroup = &UsbMouseDevice->ReportFieldInfo.ReportGroup[Index];
        break;
      }
    }
  }
  if (Mode != USB_CORE_RUNTIME_MODE) {
    UsbMouseDevice->StateChanged = TRUE;
    //
    // Check mouse Data
    //
    LeftButton  = UsbMouseDevice->State[UsbMouseDevice->BufferIn].LeftButton;
    RightButton = UsbMouseDevice->State[UsbMouseDevice->BufferIn].RightButton;
    UsbMouseDevice->State[UsbMouseDevice->BufferIn].LeftButton  = (BOOLEAN) GetReportFieldValue (Data, DataLength, &ReportGroup->Data.Mouse.FieldButton1);
    UsbMouseDevice->State[UsbMouseDevice->BufferIn].RightButton = (BOOLEAN) GetReportFieldValue (Data, DataLength, &ReportGroup->Data.Mouse.FieldButton2);

    RelativeMovementX = (INT8) GetReportFieldValue (Data, DataLength, &ReportGroup->Data.Mouse.FieldX);
    RelativeMovementY = (INT8) GetReportFieldValue (Data, DataLength, &ReportGroup->Data.Mouse.FieldY);
    RelativeMovementZ = 0;
    if (ReportGroup->Data.Mouse.FieldZ.DataValid) {
      RelativeMovementZ = (INT8) GetReportFieldValue (Data, DataLength, &ReportGroup->Data.Mouse.FieldZ);
    }

    if (LeftButton != UsbMouseDevice->State[UsbMouseDevice->BufferIn].LeftButton ||
        RightButton != UsbMouseDevice->State[UsbMouseDevice->BufferIn].RightButton) {
      //
      // Advance the internal buffer only when button state changed to reduce the dragging phenomenon
      //
      BufferIn = UsbMouseDevice->BufferIn + 1;
      if (BufferIn == STATE_BUFFER_SIZE) BufferIn = 0;
      if (BufferIn != UsbMouseDevice->BufferOut) {

        //
        // Copy last state to new position for use of check procedure of last button state
        //
        CopyMem (
          &UsbMouseDevice->State[BufferIn],
          &UsbMouseDevice->State[UsbMouseDevice->BufferIn],
          sizeof (EFI_SIMPLE_POINTER_STATE)
          );
        UsbMouseDevice->BufferIn = BufferIn;

        MouseState = &UsbMouseDevice->State[UsbMouseDevice->BufferIn];
        MouseState->RelativeMovementX = RelativeMovementX;
        MouseState->RelativeMovementY = RelativeMovementY;
        MouseState->RelativeMovementZ = RelativeMovementZ;
      }
    } else {
      MouseState = &UsbMouseDevice->State[UsbMouseDevice->BufferIn];
      MouseState->RelativeMovementX = PlusRelativeMovement (MouseState->RelativeMovementX, RelativeMovementX);
      MouseState->RelativeMovementY = PlusRelativeMovement (MouseState->RelativeMovementY, RelativeMovementY);
      MouseState->RelativeMovementZ = PlusRelativeMovement (MouseState->RelativeMovementZ, RelativeMovementZ);
    }
  } else if (mUsbCore->IsCsmEnabled () == EFI_SUCCESS) {
    //
    // Insert mouse KBC code for legacy mode
    //
    MouseCode[0] = (INT8)GetReportFieldValue (Data, DataLength, &ReportGroup->Data.Mouse.FieldButton1);
    MouseCode[0] |= ((INT8)GetReportFieldValue (Data, DataLength, &ReportGroup->Data.Mouse.FieldButton2)) << 1;
    MouseCode[0] |= 0x08;
    MouseCode[1] = (INT8)GetReportFieldValue (Data, DataLength, &ReportGroup->Data.Mouse.FieldX);
    MouseCode[2] = -((INT8)GetReportFieldValue (Data, DataLength, &ReportGroup->Data.Mouse.FieldY));
    if (MouseCode[1] < 0) MouseCode[0] |= 0x10;
    if (MouseCode[2] < 0) MouseCode[0] |= 0x20;
    MouseCode[3] = 0;
    Count = (EBDA(EBDA_PS2_AUX_TYPE) == 0x03) ? 4 : 3;
    mUsbCore->InsertKbcKeyCode(USB_CORE_MOUSE_CODE, (UINT8 *)MouseCode, Count);
  }
  return EFI_SUCCESS;
}

/**

  Get the mouse state, see SIMPLE POINTER PROTOCOL.

  @param  This                  Protocol instance pointer.
  @param  MouseState            Current mouse state

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  @retval EFI_NOT_READY

**/
STATIC
EFI_STATUS
EFIAPI
GetMouseState (
  IN   EFI_SIMPLE_POINTER_PROTOCOL  *This,
  OUT  EFI_SIMPLE_POINTER_STATE     *MouseState
  )
{
  USB_MOUSE_DEV *MouseDev;

  if (MouseState == NULL) {
    return EFI_DEVICE_ERROR;
  }
  MouseDev = USB_MOUSE_DEV_FROM_MOUSE_PROTOCOL (This);
  if (!MouseDev->StateChanged) {
    return EFI_NOT_READY;
  }
  CopyMem (
    MouseState,
    &MouseDev->State[MouseDev->BufferOut],
    sizeof (EFI_SIMPLE_POINTER_STATE)
    );

  ZeroMem (&MouseDev->State[MouseDev->BufferOut], sizeof (EFI_SIMPLE_POINTER_STATE));
  if (MouseDev->BufferOut != MouseDev->BufferIn) {
    MouseDev->BufferOut ++;
    if (MouseDev->BufferOut == STATE_BUFFER_SIZE) MouseDev->BufferOut = 0;
  }
  if (MouseDev->BufferOut == MouseDev->BufferIn) {
    MouseDev->StateChanged = FALSE;
  }
  return EFI_SUCCESS;
}

/**

  Reset the mouse device, see SIMPLE POINTER PROTOCOL.

  @param  This                  Protocol instance pointer.
  @param  ExtendedVerification  Ignored here

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
UsbMouseReset (
  IN EFI_SIMPLE_POINTER_PROTOCOL    *This,
  IN BOOLEAN                        ExtendedVerification
  )
{
  USB_MOUSE_DEV       *UsbMouseDevice;
  EFI_USB_IO_PROTOCOL *UsbIo;

  UsbMouseDevice  = USB_MOUSE_DEV_FROM_MOUSE_PROTOCOL (This);

  UsbIo           = UsbMouseDevice->UsbIo;

  MouseReportStatusCode (
    UsbMouseDevice,
    EFI_PROGRESS_CODE,
    (EFI_PERIPHERAL_MOUSE | EFI_P_PC_RESET)
    );

  ZeroMem (
    UsbMouseDevice->State,
    sizeof (EFI_SIMPLE_POINTER_STATE) * STATE_BUFFER_SIZE
    );
  UsbMouseDevice->StateChanged = FALSE;
  UsbMouseDevice->BufferIn     = 0;
  UsbMouseDevice->BufferOut    = 0;
  return EFI_SUCCESS;
}

/**

  Event notification function for SIMPLE_POINTER.WaitForInput event
  Signal the event if there is input from mouse

  @param  Event                 Wait Event
  @param  Context               Passed parameter to event handler

**/
STATIC
VOID
EFIAPI
UsbMouseWaitForInput (
  IN  EFI_EVENT               Event,
  IN  VOID                    *Context
  )
{
  USB_MOUSE_DEV *UsbMouseDevice;

  UsbMouseDevice = (USB_MOUSE_DEV *) Context;

  //
  // Someone is waiting on the mouse event, if there's
  // input from mouse, signal the event
  //
  if (UsbMouseDevice->StateChanged) {
    gBS->SignalEvent (Event);
  }
}

/**

  Timer handler for Delayed Recovery timer.

  @param  Event                 The Delayed Recovery event.
  @param  Context               Points to the USB_KB_DEV instance.

**/
VOID
EFIAPI
UsbMouseRecoveryHandler (
  IN    UINTN        Event,
  IN    VOID         *Context
  )
{
  USB_MOUSE_DEV       *UsbMouseDevice;
  EFI_USB_IO_PROTOCOL *UsbIo;

  UsbMouseDevice = (USB_MOUSE_DEV *) Context;
  UsbMouseDevice->RecoveryPollingHandle = NULL;
  UsbIo = UsbMouseDevice->UsbIo;
  if (UsbIo == NULL) return;
  UsbAsyncInterruptTransfer (
    UsbIo,
    UsbMouseDevice->IntEndpointDescriptor.EndpointAddress,
    TRUE,
    UsbMouseDevice->IntEndpointDescriptor.Interval,
    UsbMouseDevice->IntEndpointDescriptor.MaxPacketSize,
    OnMouseInterruptComplete,
    UsbMouseDevice
    );
}

/**

  Report Status Code in Usb Bot Driver

  @param  DevicePath            Use this to get Device Path
  @param  CodeType              Status Code Type
  @param  CodeValue             Status Code Value

**/
VOID
MouseReportStatusCode (
  IN USB_MOUSE_DEV             *UsbMouseDevice,
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
        UsbMouseDevice->DevicePath
        );
    }
  }
}
