/** @file
  Usb Bus Peim definition

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

#ifndef _PEI_USB_BUS_H
#define _PEI_USB_BUS_H

#include <PiPei.h>
#include <Ppi/Stall.h>
#include <Ppi/Usb2HostController.h>
#include <Ppi/Usb3HostController.h>
#include <Ppi/UsbIo.h>
#include <Ppi/Usb3Io.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>


#define MAX_ENDPOINT              16
#define STALL_1_MILLI_SECOND      1000        // stall 1 ms
#define TIMEOUT_VALUE             3000

#define PEI_USB_DEVICE_SIGNATURE        SIGNATURE_32 ('U', 's', 'b', 'D')
typedef struct {
  UINTN                                 Signature;
  EFI_PEI_SERVICES                      **PeiServices;
  EFI_PEI_STALL_PPI                     *StallPpi;
  PEI_USB_IO_PPI                        UsbIoPpi;
  PEI_USB3_IO_PPI                       Usb3IoPpi;
  EFI_PEI_PPI_DESCRIPTOR                UsbIoPpiList;
  EFI_PEI_PPI_DESCRIPTOR                Usb3IoPpiList;
  PEI_USB3_HOST_CONTROLLER_PPI          *UsbHcPpi;
  UINT8                                 DeviceAddress;
  UINT8                                 MaxPacketSize0;
  UINT8                                 DeviceSpeed;
  UINT8                                 DataToggle;
  UINT8                                 IsHub;
  UINT8                                 DownStreamPortNo;
  UINT8                                 HubDepth;
  UINT8                                 MultiTT;
  UINTN                                 AllocateAddress;
  UINT8                                 ConfigurationData[1024];
  EFI_USB_CONFIG_DESCRIPTOR             *ConfigDesc;
  EFI_USB_INTERFACE_DESCRIPTOR          *InterfaceDesc;
  EFI_USB_ENDPOINT_DESCRIPTOR           *EndpointDesc[MAX_ENDPOINT];
  EFI_USB3_HC_TRANSACTION_TRANSLATOR    Translator;
  BOOLEAN                               IsUsb3HcPpiInstalled;
} PEI_USB_DEVICE;

#define PEI_USB_DEVICE_FROM_THIS(a)     CR (a, PEI_USB_DEVICE, UsbIoPpi, PEI_USB_DEVICE_SIGNATURE)
#define PEI_USB3_DEVICE_FROM_THIS(a)    CR (a, PEI_USB_DEVICE, Usb3IoPpi, PEI_USB_DEVICE_SIGNATURE)

typedef struct {
  LIST_ENTRY                            Link;
  PEI_USB2_HOST_CONTROLLER_PPI          *UsbHcPpi;
  PEI_USB3_HOST_CONTROLLER_PPI          Usb3HcPpi;
} PEI_USB_HC_TOKEN;

#define PEI_USB_HC_TOKEN_FROM_THIS(a)   BASE_CR (a, PEI_USB_HC_TOKEN, Usb3HcPpi)

/**

  Pei Usb Control Transfer

  @param  PeiServices           EFI_PEI_SERVICES
  @param  This                  PEI_USB3_IO_PPI
  @param  Request               Request packet
  @param  Direction             IN/OUT/NODATA
  @param  Timeout               Time out value in millisecond
  @param  Data                  Data buffer
  @param  DataLength            Data length

  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
EFIAPI
PeiUsbControlTransfer (
  IN     EFI_PEI_SERVICES               **PeiServices,
  IN     PEI_USB_IO_PPI                 *This,
  IN     EFI_USB_DEVICE_REQUEST         *Request,
  IN     EFI_USB_DATA_DIRECTION         Direction,
  IN     UINT32                         Timeout,
  IN OUT VOID                           *Data,      OPTIONAL
  IN     UINTN                          DataLength  OPTIONAL
  );

/**

  Pei Usb Bulk Transfer

  @param  PeiServices           EFI_PEI_SERVICES
  @param  This                  PEI_USB3_IO_PPI
  @param  DeviceEndpoint        Device Endpoint
  @param  Data                  Data buffer
  @param  DataLength            Data length
  @param  Timeout               Time out value in millisecond

  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
EFIAPI
PeiUsbBulkTransfer (
  IN     EFI_PEI_SERVICES               **PeiServices,
  IN     PEI_USB_IO_PPI                 *This,
  IN     UINT8                          DeviceEndpoint,
  IN OUT VOID                           *Data,
  IN OUT UINTN                          *DataLength,
  IN     UINTN                          Timeout
  );

/**

  Retrieves the interface Descriptor for that controller.

  @param  PeiServices           EFI_PEI_SERVICES
  @param  This                  Indicates the calling context.
  @param  InterfaceDescriptor   A pointer to the caller allocated USB interface
                                Descriptor.

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER
  @retval EFI_NOT_FOUND

**/
EFI_STATUS
EFIAPI
PeiUsbGetInterfaceDescriptor (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  PEI_USB_IO_PPI                    *This,
  OUT EFI_USB_INTERFACE_DESCRIPTOR      **InterfaceDescriptor
  );

/**

  Retrieves the endpoint Descriptor for a given endpoint.

  @param  PeiServices           EFI_PEI_SERVICES
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
PeiUsbGetEndpointDescriptor (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  PEI_USB_IO_PPI                    *This,
  IN  UINT8                             EndpointIndex,
  OUT EFI_USB_ENDPOINT_DESCRIPTOR       **EndpointDescriptor
  );

/**

  Pei Usb PortReset

  @param  PeiServices           EFI_PEI_SERVICES
  @param  This                  PEI_USB3_IO_PPI

  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
EFIAPI
PeiUsbPortReset (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN PEI_USB_IO_PPI                     *This
  );

/**

  Pei Usb Control Transfer

  @param  PeiServices           EFI_PEI_SERVICES
  @param  This                  PEI_USB3_IO_PPI
  @param  Request               Request packet
  @param  Direction             IN/OUT/NODATA
  @param  Timeout               Time out value in millisecond
  @param  Data                  Data buffer
  @param  DataLength            Data length

  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
EFIAPI
PeiUsb3ControlTransfer (
  IN     EFI_PEI_SERVICES               **PeiServices,
  IN     PEI_USB3_IO_PPI                *This,
  IN     EFI_USB_DEVICE_REQUEST         *Request,
  IN     EFI_USB_DATA_DIRECTION         Direction,
  IN     UINT32                         Timeout,
  IN OUT VOID                           *Data,      OPTIONAL
  IN     UINTN                          DataLength  OPTIONAL
  );

/**

  Pei Usb Bulk Transfer

  @param  PeiServices           EFI_PEI_SERVICES
  @param  This                  PEI_USB3_IO_PPI
  @param  DeviceEndpoint        Device Endpoint
  @param  Data                  Data buffer
  @param  DataLength            Data length
  @param  Timeout               Time out value in millisecond

  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
EFIAPI
PeiUsb3BulkTransfer (
  IN     EFI_PEI_SERVICES               **PeiServices,
  IN     PEI_USB3_IO_PPI                *This,
  IN     UINT8                          DeviceEndpoint,
  IN OUT VOID                           *Data,
  IN OUT UINTN                          *DataLength,
  IN     UINTN                          Timeout
  );

/**

  Pei Usb Sync Interrupt Transfer

  @param  PeiServices           EFI_PEI_SERVICES
  @param  This                  PEI_USB3_IO_PPI
  @param  DeviceEndpoint        Device Endpoint
  @param  Data                  Data buffer
  @param  DataLength            Data length
  @param  Timeout               Time out value in millisecond

  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
EFIAPI
PeiUsb3SyncInterruptTransfer (
  IN     EFI_PEI_SERVICES               **PeiServices,
  IN     PEI_USB3_IO_PPI                *This,
  IN     UINT8                          DeviceEndpoint,
  IN OUT VOID                           *Data,
  IN OUT UINTN                          *DataLength,
  IN     UINTN                          Timeout
  );

/**

  Pei Usb Async Interrupt Transfer

  @param  PeiServices           EFI_PEI_SERVICES
  @param  This                  PEI_USB3_IO_PPI
  @param  DeviceEndpoint        Device Endpoint
  @param  Data                  Data buffer
  @param  DataLength            Data length
  @param  InterruptCallBack     The Callback function.  This function is called if
                                          the asynchronous interrupt transfer is completed.
  @param  Context               Passed to InterruptCallback

  @retval EFI_SUCCESS           Success
  @retval EFI_INVALID_PARAMETER

**/
EFI_STATUS
EFIAPI
PeiUsb3AsyncInterruptTransfer (
  IN     EFI_PEI_SERVICES                 **PeiServices,
  IN     PEI_USB3_IO_PPI                  *This,
  IN     UINT8                            DeviceEndpoint,
  IN OUT VOID                             *Data,
  IN OUT UINTN                            *DataLength,
  IN     EFI_ASYNC_USB_TRANSFER_CALLBACK  InterruptCallBack ,
  IN     VOID                             *Context,
  IN     UINTN                            PollingInterval
  );

/**

  Retrieves the interface Descriptor for that controller.

  @param  PeiServices           EFI_PEI_SERVICES
  @param  This                  Indicates the calling context.
  @param  InterfaceDescriptor   A pointer to the caller allocated USB interface
                                Descriptor.

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER
  @retval EFI_NOT_FOUND

**/
EFI_STATUS
EFIAPI
PeiUsb3GetInterfaceDescriptor (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  PEI_USB3_IO_PPI                   *This,
  OUT EFI_USB_INTERFACE_DESCRIPTOR      **InterfaceDescriptor
  );

/**

  Retrieves the endpoint Descriptor for a given endpoint.

  @param  PeiServices           EFI_PEI_SERVICES
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
PeiUsb3GetEndpointDescriptor (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  PEI_USB3_IO_PPI                   *This,
  IN  UINT8                             EndpointIndex,
  OUT EFI_USB_ENDPOINT_DESCRIPTOR       **EndpointDescriptor
  );

/**

  Pei Usb PortReset

  @param  PeiServices           EFI_PEI_SERVICES
  @param  This                  PEI_USB3_IO_PPI

  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
EFIAPI
PeiUsb3PortReset (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN PEI_USB3_IO_PPI                    *This
  );

/**

  Clear Endpoint Halt

  @param  PeiServices           EFI_PEI_SERVICES
  @param  This                  PEI_USB3_IO_PPI
  @param  EndpointAddress       Endpoint Address

  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
EFIAPI
PeiUsb3ClearEndpointHalt (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN PEI_USB3_IO_PPI                    *This,
  IN UINT8                              EndpointAddress
  );

/**

  Reset RootPort

  @param  PeiUsbDevice          PEI_USB_DEVICE
  @param  PortNum               PortNum
  @param  RetryIndex            Retry times

**/
VOID
ResetRootPort (
  IN PEI_USB_DEVICE                     *PeiUsbDevice,
  IN UINT8                              PortNum
  );

#endif
