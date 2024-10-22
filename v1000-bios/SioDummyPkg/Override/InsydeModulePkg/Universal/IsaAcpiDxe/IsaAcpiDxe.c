/** @file
  ISA ACPI Driver

;******************************************************************************
;* Copyright (c) 2019, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "IsaAcpiDxe.h"
#include "IsaAcpi.h"

EFI_DRIVER_BINDING_PROTOCOL gIsaAcpiDriverBinding = {
  IsaAcpiDriverSupported,
  IsaAcpiDriverStart,
  IsaAcpiDriverStop,
  1,
  NULL,
  NULL
};

/**
  The entry point of IsaAcpiDxe.

  @param[in]  ImageHandle       Pointer to the loaded image protocol for this driver.
  @param[in]  SystemTable       Pointer to the EFI System Table.

  @retval     EFI_SUCCESS       Function complete successfully.

**/
EFI_STATUS
EFIAPI
IsaAcpiDriverEntryPoint (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  return EfiLibInstallDriverBinding (
           ImageHandle,
           SystemTable,
           &gIsaAcpiDriverBinding,
           ImageHandle
           );
}

/**
  Tests to see if this driver supports a given controller.

  @param[in]  This                 A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle     The handle of the controller to test. This handle
                                   must support a protocol interface that supplies
                                   an I/O abstraction to the driver.
  @param[in]  RemainingDevicePath  A pointer to the remaining portion of a device path.  This
                                   parameter is ignored by device drivers, and is optional for bus
                                   drivers. For bus drivers, if this parameter is not NULL, then
                                   the bus driver must determine if the bus controller specified
                                   by ControllerHandle and the child controller specified
                                   by RemainingDevicePath are both supported by this
                                   bus driver.

  @retval EFI_SUCCESS              The device specified by ControllerHandle and
                                   RemainingDevicePath is supported by the driver specified by This.
  @retval EFI_ALREADY_STARTED      The device specified by ControllerHandle and
                                   RemainingDevicePath is already being managed by the driver
                                   specified by This.
  @retval EFI_ACCESS_DENIED        The device specified by ControllerHandle and
                                   RemainingDevicePath is already being managed by a different
                                   driver or an application that requires exclusive access.
                                   Currently not implemented.
  @retval EFI_UNSUPPORTED          The device specified by ControllerHandle and
                                   RemainingDevicePath is not supported by the driver specified by This.

**/
EFI_STATUS
EFIAPI
IsaAcpiDriverSupported (
  IN  EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN  EFI_HANDLE                    Controller,
  IN  EFI_DEVICE_PATH_PROTOCOL      *RemainingDevicePath
  )
{
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *IsaBridgeDevicePath;
  ACPI_HID_DEVICE_PATH      *AcpiNode;
  PCI_DEVICE_PATH           *PciNode;
  UINT8                     IsaBridgeDevice;
  UINT8                     IsaBridgeFunction;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  PCI_TYPE00                Pci;

  //
  // Verify if there were an ISA bridge present on PCI bus
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID**) &IsaBridgeDevicePath,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status    = EFI_SUCCESS;
  AcpiNode  = (ACPI_HID_DEVICE_PATH *) IsaBridgeDevicePath;
  if (DevicePathNodeLength (&AcpiNode->Header) != sizeof (ACPI_HID_DEVICE_PATH) ||
      AcpiNode->Header.Type != ACPI_DEVICE_PATH ||
      AcpiNode->Header.SubType != ACPI_DP ||
      AcpiNode->HID != EISA_PNP_ID (0x0A03) ||
      AcpiNode->UID != 0
      )
  {
    Status = EFI_UNSUPPORTED;
  } else {
    IsaBridgeDevicePath = NextDevicePathNode (IsaBridgeDevicePath);
    PciNode             = (PCI_DEVICE_PATH *) IsaBridgeDevicePath;
    DxeCsSvcSetIsaInfo (&IsaBridgeDevice, &IsaBridgeFunction);

    if (DevicePathNodeLength (&PciNode->Header) != sizeof (PCI_DEVICE_PATH) ||
        PciNode->Header.Type != HARDWARE_DEVICE_PATH ||
        PciNode->Header.SubType != HW_PCI_DP ||
        PciNode->Function != IsaBridgeDevice ||
        PciNode->Device != IsaBridgeFunction
        )
    {
      Status = EFI_UNSUPPORTED;
    }
  }

  gBS->CloseProtocol (
         Controller,
         &gEfiDevicePathProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Verify if it were an ISA Bridge
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID**) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = PciIo->Pci.Read (
                    PciIo,
                    EfiPciIoWidthUint32,
                    0,
                    sizeof (PCI_TYPE00) / sizeof (UINT32),
                    &Pci
                    );
  if (!EFI_ERROR (Status)) {
    Status  = EFI_UNSUPPORTED;
    if ((Pci.Hdr.Command & (EFI_PCI_COMMAND_IO_SPACE | EFI_PCI_COMMAND_MEMORY_SPACE)) != 0) {
      if (IS_PCI_LPC (&Pci)) {
        Status = EFI_SUCCESS;
      } else {
        if (IS_CLASS2 (&Pci, PCI_CLASS_BRIDGE, PCI_CLASS_BRIDGE_ISA_PDECODE) &&
            Pci.Hdr.VendorId == V_INTEL_VENDOR_ID
            )
        {
          Status = EFI_SUCCESS;
        }
      }
    }
  }

  gBS->CloseProtocol (
         Controller,
         &gEfiPciIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  return Status;
}

/**
  Starts the device controller.

  @param[in]  This                 A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle     The handle of the controller to start. This handle
                                   must support a protocol interface that supplies
                                   an I/O abstraction to the driver.
  @param[in]  RemainingDevicePath  A pointer to the remaining portion of a device path.  This
                                   parameter is ignored by device drivers, and is optional for bus
                                   drivers. For a bus driver, if this parameter is NULL, then handles
                                   for all the children of Controller are created by this driver.
                                   If this parameter is not NULL and the first Device Path Node is
                                   not the End of Device Path Node, then only the handle for the
                                   child device specified by the first Device Path Node of
                                   RemainingDevicePath is created by this driver.
                                   If the first Device Path Node of RemainingDevicePath is
                                   the End of Device Path Node, no child handle is created by this
                                   driver.

  @retval EFI_SUCCESS              The device was started.
  @retval EFI_DEVICE_ERROR         The device could not be started due to a device error.Currently not implemented.
  @retval EFI_OUT_OF_RESOURCES     The request could not be completed due to a lack of resources.
  @retval Others                   The driver failded to start the device.

**/
EFI_STATUS
EFIAPI
IsaAcpiDriverStart (
  IN  EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN  EFI_HANDLE                    Controller,
  IN  EFI_DEVICE_PATH_PROTOCOL      *RemainingDevicePath
  )
{
  EFI_STATUS            Status;
  EFI_PCI_IO_PROTOCOL   *PciIo;
  H2O_ISA_BRIDGE        *IsaBridge;

  PostCode (BDS_START_ISA_ACPI_CONTROLLER);

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID**) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status) && Status != EFI_ALREADY_STARTED) {
    return Status;
  }

  IsaBridge = AllocateZeroPool (sizeof (H2O_ISA_BRIDGE));
  if (IsaBridge == NULL) {
    gBS->CloseProtocol (
           Controller,
           &gEfiPciIoProtocolGuid,
           This->DriverBindingHandle,
           Controller
           );
    return EFI_OUT_OF_RESOURCES;
  }

  IsaBridge->Signature                  = H2O_ISA_BRIDGE_SIGNATURE;
  IsaBridge->Handle                     = Controller;
  IsaBridge->PciIo                      = PciIo;
  IsaBridge->IsaAcpi.DeviceEnumerate    = IsaDeviceEnumerate;
  IsaBridge->IsaAcpi.SetPower           = IsaSetPower;
  IsaBridge->IsaAcpi.GetCurResource     = IsaGetCurrentResource;
  IsaBridge->IsaAcpi.GetPosResource     = IsaGetPossibleResource;
  IsaBridge->IsaAcpi.SetResource        = IsaSetResource;
  IsaBridge->IsaAcpi.EnableDevice       = IsaEnableDevice;
  IsaBridge->IsaAcpi.InitDevice         = IsaInitDevice;
  IsaBridge->IsaAcpi.InterfaceInit      = IsaInterfaceInit;
  IsaBridge->IsaAcpi.DriverStopCallback = IsaAcpiDriverStopCallback;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Controller,
                  &gEfiIsaAcpiProtocolGuid,
                  (VOID**) &IsaBridge->IsaAcpi,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    gBS->CloseProtocol (
           Controller,
           &gEfiPciIoProtocolGuid,
           This->DriverBindingHandle,
           Controller
           );
    FreePool (IsaBridge);
    return Status;
  }

  IsaInterfaceInit (NULL);

  return EFI_SUCCESS;
}

/**
  Stops the device controller.

  @param[in]  This              A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param[in]  ControllerHandle  A handle to the device being stopped. The handle must
                                support a bus specific I/O protocol for the driver
                                to use to stop the device.
  @param[in]  NumberOfChildren  The number of child device handles in ChildHandleBuffer.
  @param[in]  ChildHandleBuffer An array of child handles to be freed. May be NULL
                                if NumberOfChildren is 0.

  @retval EFI_SUCCESS           The device was stopped.
  @retval EFI_DEVICE_ERROR      The device could not be stopped due to a device error.

**/
EFI_STATUS
EFIAPI
IsaAcpiDriverStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN  EFI_HANDLE                     Controller,
  IN  UINTN                          NumberOfChildren,
  IN  EFI_HANDLE                     *ChildHandleBuffer
  )
{
  EFI_STATUS             Status;
  EFI_ISA_ACPI_PROTOCOL  *IsaAcpi;
  H2O_ISA_BRIDGE         *IsaBridge;

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiIsaAcpiProtocolGuid,
                  (VOID**) &IsaAcpi,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = IsaAcpi->DriverStopCallback (IsaAcpi);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  IsaBridge = H2O_ISA_BRIDGE_FROM_PROTOCOL (IsaAcpi);

  Status = gBS->UninstallProtocolInterface (
                  Controller,
                  &gEfiIsaAcpiProtocolGuid,
                  (VOID**) &IsaBridge->IsaAcpi
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  gBS->CloseProtocol (
         Controller,
         &gEfiPciIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );
  FreePool (IsaBridge);

  return EFI_SUCCESS;
}

