/** @file
  PEI USB helper function header

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

#ifndef _PEI_USB_HELPER_H
#define _PEI_USB_HELPER_H

/**

  Pei Usb Get Descriptor

  @param  PeiServices           EFI_PEI_SERVICES
  @param  UsbIoPpi              PEI_USB3_IO_PPI
  @param  Value                 Request Value
  @param  Index                 Request Index
  @param  DescriptorLength      Descriptor Length
  @param  Descriptor            Descriptor

  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
PeiUsbGetDescriptor (
  IN  EFI_PEI_SERVICES         **PeiServices,
  IN  PEI_USB3_IO_PPI          *UsbIoPpi,
  IN  UINT16                   Value,
  IN  UINT16                   Index,
  IN  UINT16                   DescriptorLength,
  OUT VOID                     *Descriptor
  );

/**

  Pei Set Address

  @param  PeiServices           EFI_PEI_SERVICES
  @param  UsbIoPpi              PEI_USB3_IO_PPI
  @param  AddressValue          AddressValue
 
  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
PeiUsbSetDeviceAddress (
  IN EFI_PEI_SERVICES         **PeiServices,
  IN PEI_USB3_IO_PPI          *UsbIoPpi,
  IN UINT16                   AddressValue
  );

/**

  Pei Usb Clear Device Feature

  @param  PeiServices           EFI_PEI_SERVICES
  @param  UsbIoPpi              PEI_USB3_IO_PPI
  @param  Recipient             Device/Interface/Endpoint
  @param  Value                 Request Value
  @param  Target                Request Index

  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
PeiUsbClearDeviceFeature (
  IN EFI_PEI_SERVICES         **PeiServices,
  IN PEI_USB3_IO_PPI          *UsbIoPpi,
  IN UINT8                    Recipient,
  IN UINT16                   Value,
  IN UINT16                   Target
  );

/**

  Pei Usb Set Configuration to 1

  @param  PeiServices           EFI_PEI_SERVICES
  @param  UsbIoPpi              PEI_USB3_IO_PPI
 
  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
PeiUsbSetConfiguration (
  IN EFI_PEI_SERVICES         **PeiServices,
  IN PEI_USB3_IO_PPI          *UsbIoPpi
  );

/**

  Tell if there is a device connected to that port according to
  the Port Status.

  @param  PortStatus            The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortConnect (
  IN UINT16  PortStatus
  );

/**

  Tell if the connected device is a low device.

  @param  PortStatus            The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortLowSpeedDeviceAttached (
  IN UINT16  PortStatus
  );

/**

  Tell if there is a Connect Change status in that port.

  @param  PortChangeStatus      The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortConnectChange (
  IN UINT16  PortChangeStatus
  );

/**

  Tell if there is a Port Reset Change status in that port.

  @param  PortChangeStatus      The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortResetChange (
  IN UINT16  PortChangeStatus
  );

/**

  Initiate a USB control transfer using a specific USB Host controller on the USB bus

  @param  PeiServices           The pointer to the PEI Services Table.
  @param  This                  The pointer to this instance of the PEI_USB3_HOST_CONTROLLER_PPI.
  @param  DeviceAddress         Represents the address of the target device on the USB.
  @param  DeviceSpeed           Indicates device speed.
  @param  MaximumPacketLength   Indicates the maximum packet size that the default control transfer
                                endpoint is capable of sending or receiving.
  @param  Request               A pointer to the USB device request that will be sent to the USB device.
  @param  TransferDirection     Specifies the data direction for the transfer. There are three values available:
                                EfiUsbDataIn, EfiUsbDataOut and EfiUsbNoData.
  @param  Data                  A pointer to the buffer of data that will be transmitted to USB device or
                                received from USB device.
  @param  DataLength            On input, indicates the size, in bytes, of the data buffer specified by Data.
                                On output, indicates the amount of data actually transferred.
  @param  TimeOut               Indicates the maximum time, in milliseconds, that the transfer is allowed to complete.
  @param  Translator            A pointer to the transaction translator data.
  @param  TransferResult        A pointer to the detailed result information generated by this control transfer.

  @retval EFI_SUCCESS           The control transfer was completed successfully.
  @retval EFI_DEVICE_ERROR      The control transfer failed due to host controller or device error.
  @retval EFI_INVALID_PARAMETER Some parameters are invalid.
  @retval EFI_OUT_OF_RESOURCES  The control transfer could not be completed due to a lack of resources.
  @retval EFI_TIMEOUT           The control transfer failed due to timeout.

**/
EFI_STATUS
EFIAPI
Usb3HcControlTransfer (
  IN     EFI_PEI_SERVICES                       **PeiServices,
  IN     PEI_USB3_HOST_CONTROLLER_PPI           *This,
  IN     UINT8                                  DeviceAddress,
  IN     UINT8                                  DeviceSpeed,
  IN     UINTN                                  MaximumPacketLength,
  IN     USB_DEVICE_REQUEST                     *Request,
  IN     EFI_USB_DATA_DIRECTION                 TransferDirection,
  IN OUT VOID                                   *Data OPTIONAL,
  IN OUT UINTN                                  *DataLength OPTIONAL,
  IN     UINTN                                  TimeOut,
  IN     EFI_USB3_HC_TRANSACTION_TRANSLATOR     *Translator,
  OUT    UINT32                                 *TransferResult
  );

/**

  Initiate a USB bulk transfer using a specific USB Host controller on the USB bus

  @param  PeiServices           The pointer to the PEI Services Table.
  @param  This                  The pointer to this instance of the PEI_USB3_HOST_CONTROLLER_PPI.
  @param  DeviceAddress         Represents the address of the target device on the USB.
  @param  EndPointAddress       The combination of an endpoint number and
                                an endpoint direction of the target USB device.
  @param  DeviceSpeed           Indicates device speed.
  @param  MaximumPacketLength   Indicates the maximum packet size the target
                                endpoint is capable of sending or receiving.
  @param  Data                  Array of pointers to the buffers of data
                                that will be transmitted to USB device or
                                received from USB device.
  @param  DataLength            When input, indicates the size, in bytes, of
                                the data buffers specified by Data. When output,
                                indicates the data size actually transferred.
  @param  DataToggle            A pointer to the data toggle value.
  @param  TimeOut               Indicates the maximum time, in milliseconds,
                                in which the transfer is allowed to complete.
  @param  Translator            A pointer to the transaction translator data.
  @param  TransferResult        A pointer to the detailed result information of the bulk transfer.

  @retval EFI_SUCCESS           The bulk transfer was completed successfully.
  @retval EFI_DEVICE_ERROR      The bulk transfer failed due to host controller or device error.
                                Caller should check TransferResult for detailed error information.
  @retval EFI_INVALID_PARAMETER Some parameters are invalid.
  @retval EFI_OUT_OF_RESOURCES  The bulk transfer could not be submitted due to a lack of resources.
  @retval EFI_TIMEOUT           The bulk transfer failed due to timeout

**/
EFI_STATUS
EFIAPI
Usb3HcBulkTransfer (
  IN     EFI_PEI_SERVICES                       **PeiServices,
  IN     PEI_USB3_HOST_CONTROLLER_PPI           *This,
  IN     UINT8                                  DeviceAddress,
  IN     UINT8                                  EndPointAddress,
  IN     UINT8                                  DeviceSpeed,
  IN     UINTN                                  MaximumPacketLength,
  IN OUT VOID                                   *Data[EFI_USB_MAX_BULK_BUFFER_NUM],
  IN OUT UINTN                                  *DataLength,
  IN OUT UINT8                                  *DataToggle,
  IN     UINTN                                  TimeOut,
  IN     EFI_USB3_HC_TRANSACTION_TRANSLATOR     *Translator,
  OUT    UINT32                                 *TransferResult
  );

/**

  Initiate a USB interrupt transfer using a specific USB Host controller on the USB bus

  @param  PeiServices           The pointer to the PEI Services Table.
  @param  This                  The pointer to this instance of the PEI_USB3_HOST_CONTROLLER_PPI.
  @param  DeviceAddress         Represents the address of the target device on the USB.
  @param  EndPointAddress       The combination of an endpoint number and
                                an endpoint direction of the target USB device.
  @param  DeviceSpeed           Indicates device speed.
  @param  MaximumPacketLength   Indicates the maximum packet size the target
                                endpoint is capable of sending or receiving.
  @param  Data                  Array of pointers to the buffers of data
                                that will be transmitted to USB device or
                                received from USB device.
  @param  DataLength            When input, indicates the size, in bytes, of
                                the data buffers specified by Data. When output,
                                indicates the data size actually transferred.
  @param  DataToggle            A pointer to the data toggle value.
  @param  TimeOut               Indicates the maximum time, in milliseconds,
                                in which the transfer is allowed to complete.
  @param  Translator            A pointer to the transaction translator data.
  @param  CallBackFunction      The Callback function.This function is called at the
                                rate specified by PollingInterval.This parameter is
                                only required when request asynchronous interrupt transfer.
  @param  Context               The context that is passed to the CallBackFunction.
                                his is an optional parameter and may be NULL.
  @param  PollingInterval       Indicates the interval, in milliseconds, that the
                                asynchronous interrupt transfer is polled.
                                This parameter is required request asynchronous interrupt
                                transfer.
  @param  TransferResult        A pointer to the detailed result information of the sync interrupt transfer.

  @retval EFI_SUCCESS           The sync interrupt transfer was completed successfully.
  @retval EFI_DEVICE_ERROR      The sync interrupt transfer failed due to host controller or device error.
                                Caller should check TransferResult for detailed error information.
  @retval EFI_INVALID_PARAMETER Some parameters are invalid.
  @retval EFI_OUT_OF_RESOURCES  The sync interrupt transfer could not be submitted due to a lack of resources.
  @retval EFI_TIMEOUT           The sync interrupt transfer failed due to timeout.

**/
EFI_STATUS
EFIAPI
Usb3HcInterruptTransfer (
  IN     EFI_PEI_SERVICES                       **PeiServices,
  IN     PEI_USB3_HOST_CONTROLLER_PPI           *This,
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

/**

  Retrieves the number of root hub ports

  @param  PeiServices           The pointer to the PEI Services Table.
  @param  This                  The pointer to this instance of the PEI_USB3_HOST_CONTROLLER_PPI.
  @param  PortNumber            The pointer to the number of the root hub ports.

  @retval EFI_SUCCESS           The port number was retrieved successfully.
  @retval EFI_INVALID_PARAMETER PortNumber is NULL.

**/
EFI_STATUS
EFIAPI
Usb3HcGetRootHubPortNumber (
  IN     EFI_PEI_SERVICES                       **PeiServices,
  IN     PEI_USB3_HOST_CONTROLLER_PPI           *This,
  OUT    UINT8                                  *PortNumber
  );

/**

  Retrieves the current status of a USB root hub port.

  @param  PeiServices           The pointer to the PEI Services Table.
  @param  This                  The pointer to this instance of the PEI_USB3_HOST_CONTROLLER_PPI.
  @param  PortNumber            Specifies the root hub port from which the status is
                                to be retrieved. This value is zero based.
  @param  PortStatus            A pointer to the current port status bits and port status change bits.

  @retval EFI_SUCCESS           The status of the USB root hub port specified by
                                PortNumber was returned in PortStatus.
  @retval EFI_INVALID_PARAMETER PortNumber is invalid.

**/
EFI_STATUS
EFIAPI
Usb3HcGetRootHubPortStatus (
  IN     EFI_PEI_SERVICES                       **PeiServices,
  IN     PEI_USB3_HOST_CONTROLLER_PPI           *This,
  IN     UINT8                                  PortNumber,
  OUT    EFI_USB_PORT_STATUS                    *PortStatus
  );

/**

  Sets a feature for the specified root hub port.

  @param  PeiServices           The pointer to the PEI Services Table.
  @param  This                  The pointer to this instance of the PEI_USB3_HOST_CONTROLLER_PPI.
  @param  PortNumber            Specifies the root hub port whose feature is requested
                                to be set. This value is zero based.
  @param  PortFeature           Indicates the feature selector associated with the feature set request.

  @retval EFI_SUCCESS           The feature specified by PortFeature was set for
                                the USB root hub port specified by PortNumber.
  @retval EFI_INVALID_PARAMETER PortNumber is invalid or PortFeature is invalid
                                for this function.
  @retval EFI_TIMEOUT           The time out occurred

**/
EFI_STATUS
EFIAPI
Usb3HcSetRootHubPortFeature (
  IN     EFI_PEI_SERVICES                       **PeiServices,
  IN     PEI_USB3_HOST_CONTROLLER_PPI           *This,
  IN     UINT8                                  PortNumber,
  IN     EFI_USB_PORT_FEATURE                   PortFeature
  );

/**

  Clears a feature for the specified root hub port.

  @param  PeiServices           The pointer to the PEI Services Table.
  @param  This                  The pointer to this instance of the PEI_USB3_HOST_CONTROLLER_PPI.
  @param  PortNumber            Specifies the root hub port whose feature is requested to be cleared.
  @param  PortFeature           Indicates the feature selector associated with the feature clear request.

  @retval EFI_SUCCESS           The feature specified by PortFeature was cleared
                                for the USB root hub port specified by PortNumber.
  @retval EFI_INVALID_PARAMETER PortNumber is invalid or PortFeature is invalid.

**/
EFI_STATUS
EFIAPI
Usb3HcClearRootHubPortFeature (
  IN     EFI_PEI_SERVICES                       **PeiServices,
  IN     PEI_USB3_HOST_CONTROLLER_PPI           *This,
  IN     UINT8                                  PortNumber,
  IN     EFI_USB_PORT_FEATURE                   PortFeature
  );

#endif
