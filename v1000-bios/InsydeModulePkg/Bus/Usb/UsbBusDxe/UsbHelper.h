/** @file
  Helper functions for USB

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

#ifndef _USB_HELPER_H
#define _USB_HELPER_H

#define USB_HOST_CONTROLLER_DEVICE_SIGNATURE    SIGNATURE_32 ('u', 'h', 'c', '3')

typedef struct {
  UINTN                                 Signature;
  EFI_USB_IO_PROTOCOL                   *UsbIo;
  EFI_USB2_HC_PROTOCOL                  *Usb2Hc;
  EFI_USB3_HC_PROTOCOL                  Usb3Hc;
  EFI_EVENT                             TimerEvent;
  VOID                                  *TimerContext;
  EFI_USB3_HC_CALLBACK                  TimerCallback;
} USB_HOST_CONTROLLER_DEVICE;

#define USB_HOST_CONTROLLER_DEVICE_FROM_THIS(a)         CR (a, USB_HOST_CONTROLLER_DEVICE, Usb3Hc, USB_HOST_CONTROLLER_DEVICE_SIGNATURE)

#pragma pack(1)

typedef struct {
  UINT8   Length;
  UINT8   DescriptorType;
  UINT16  SupportedLanID[1];
} EFI_USB_SUPPORTED_LANGUAGES;

#pragma pack()

EFI_STATUS
UsbGetStringtable (
  IN  USB_IO_DEVICE                     *UsbIoDev
  );

EFI_STATUS
UsbGetAllConfigurations (
  IN  USB_IO_DEVICE                     *UsbIoDev
  );

EFI_STATUS
UsbSetConfiguration (
  IN  USB_IO_DEVICE                     *UsbIoDev,
  IN  UINTN                             ConfigurationValue
  );

EFI_STATUS
UsbSetDefaultConfiguration (
  IN  USB_IO_DEVICE                     *UsbIoDev
  );

VOID
UsbDestroyAllConfiguration (
  IN  USB_IO_DEVICE                     *UsbIoDev
  );

BOOLEAN
IsPortConnect (
  IN UINT16                             PortStatus
  );

BOOLEAN
IsPortEnable (
  IN UINT16                             PortStatus
  );

BOOLEAN
IsPortInReset (
  IN UINT16                             PortStatus
  );

BOOLEAN
IsPortPowerApplied (
  IN UINT16                             PortStatus
  );

BOOLEAN
IsPortLowSpeedDeviceAttached (
  IN UINT16                             PortStatus
  );

BOOLEAN
IsPortSuspend (
  IN UINT16                             PortStatus
  );

BOOLEAN
IsPortConnectChange (
  IN UINT16                             PortChangeStatus
  );

BOOLEAN
IsPortEnableDisableChange (
  IN UINT16                             PortChangeStatus
  );

BOOLEAN
IsPortResetChange (
  IN UINT16                             PortChangeStatus
  );

BOOLEAN
IsPortSuspendChange (
  IN UINT16                             PortChangeStatus
  );

EFI_STATUS
IsDeviceDisconnected (
  IN USB_IO_CONTROLLER_DEVICE           *UsbIoController,
  IN OUT BOOLEAN                        *Disconnected
  );

UINT8
GetFirstAlternateSetting (
  IN EFI_USB_IO_PROTOCOL                *This
  );

VOID
SetFirstAlternateSetting (
  IN EFI_USB_IO_PROTOCOL                *This
  );

EFI_USB_HID_DESCRIPTOR*
GetHidDescriptor (
  IN EFI_USB_IO_PROTOCOL                *This
  );

INTERFACE_DESC_LIST_ENTRY*
FindInterfaceListEntry (
  IN EFI_USB_IO_PROTOCOL                *This
  );

ENDPOINT_DESC_LIST_ENTRY*
FindEndPointListEntry (
  IN EFI_USB_IO_PROTOCOL                *This,
  IN UINT8                              EndPointAddress
  );

VOID
SetDataToggleBit (
  IN EFI_USB_IO_PROTOCOL                *UsbIo,
  IN UINT8                              EndpointAddr,
  IN UINT8                              DataToggle
  );

EFI_STATUS
UsbSetDeviceAddress (
  IN  EFI_USB_IO_PROTOCOL               *UsbIo,
  IN  UINT16                            AddressValue,
  OUT UINT32                            *Status
  );

EFI_STATUS
EFIAPI
ControlTransfer (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     UINT8                                  DeviceAddress,
  IN     UINT8                                  DeviceSpeed,
  IN     UINTN                                  MaximumPacketLength,
  IN     EFI_USB_DEVICE_REQUEST                 *Request,
  IN     EFI_USB_DATA_DIRECTION                 TransferDirection,
  IN OUT VOID                                   *Data       OPTIONAL,
  IN OUT UINTN                                  *DataLength OPTIONAL,
  IN     UINTN                                  TimeOut,
  IN     EFI_USB3_HC_TRANSACTION_TRANSLATOR     *Translator,
  OUT    UINT32                                 *TransferResult
  );

EFI_STATUS
EFIAPI
BulkTransfer (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     UINT8                                  DeviceAddress,
  IN     UINT8                                  EndPointAddress,
  IN     UINT8                                  DeviceSpeed,
  IN     UINTN                                  MaximumPacketLength,
  IN     UINT8                                  DataBuffersNumber,
  IN OUT VOID                                   *Data[EFI_USB_MAX_BULK_BUFFER_NUM],
  IN OUT UINTN                                  *DataLength,
  IN OUT UINT8                                  *DataToggle,
  IN     UINTN                                  TimeOut,
  IN     EFI_USB3_HC_TRANSACTION_TRANSLATOR     *Translator,
  IN     EFI_ASYNC_USB_TRANSFER_CALLBACK        CallBackFunction OPTIONAL,
  IN     VOID                                   *Context         OPTIONAL,
  OUT    UINT32                                 *TransferResult
  );

EFI_STATUS
EFIAPI
InterruptTransfer (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     UINT8                                  DeviceAddress,
  IN     UINT8                                  EndPointAddress,
  IN     UINT8                                  DeviceSpeed,
  IN     UINTN                                  MaximumPacketLength,
  IN OUT VOID                                   *Data,
  IN OUT UINTN                                  *DataLength,
  IN OUT UINT8                                  *DataToggle,
  IN     UINTN                                  TimeOut,
  IN     EFI_USB3_HC_TRANSACTION_TRANSLATOR     *Translator,
  IN     EFI_ASYNC_USB_TRANSFER_CALLBACK        CallBackFunction OPTIONAL,
  IN     VOID                                   *Context         OPTIONAL,
  IN     UINTN                                  PollingInterval  OPTIONAL,
  OUT    UINT32                                 *TransferResult
  );

EFI_STATUS
EFIAPI
CancelIo (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     UINT8                                  DeviceAddress,
  IN     UINT8                                  EndPointAddress,
  IN     UINTN                                  PollingInterval,
  IN OUT UINT8                                  *DataToggle
  );

EFI_STATUS
EFIAPI
GetRootHubPortStatus (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     UINT8                                  PortNumber,
  OUT    EFI_USB_PORT_STATUS                    *PortStatus
  );

EFI_STATUS
EFIAPI
SetRootHubPortFeature (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     UINT8                                  PortNumber,
  IN     EFI_USB_PORT_FEATURE                   PortFeature
  );

EFI_STATUS
EFIAPI
ClearRootHubPortFeature (
  IN     EFI_USB3_HC_PROTOCOL                   *This,
  IN     UINT8                                  PortNumber,
  IN     EFI_USB_PORT_FEATURE                   PortFeature
  );

/**

  Install Usb3Hc for Usb2Hc only driver

  @param  UsbIo                 EFI_USB_IO_PROTOCOL
  @param  Usb2Hc                EFI_USB2_HC_PROTOCOL

  @retval The installed EFI_USB3_HC_PROTOCOL

**/
EFI_USB3_HC_PROTOCOL*
InstallUsb3HcThunk (
  IN  EFI_USB_IO_PROTOCOL       *UsbIo,
  IN  EFI_USB2_HC_PROTOCOL      *Usb2Hc
  );

/**

  Uninstall Usb3Hc for Usb2Hc only driver

  @param  Usb2Hc                EFI_USB2_HC_PROTOCOL

  @retval The installed EFI_USB3_HC_PROTOCOL

**/
VOID
UninstallUsb3HcThunk (
  IN  EFI_USB3_HC_PROTOCOL      *Usb3Hc
  );

#endif
