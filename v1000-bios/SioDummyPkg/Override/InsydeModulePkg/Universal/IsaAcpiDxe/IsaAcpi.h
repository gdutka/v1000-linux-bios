/** @file
  Declarations for IsaAcpi.c

;******************************************************************************
;* Copyright (c) 2013 - 2019, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _ISA_ACPI_H_
#define _ISA_ACPI_H_

#include <Uefi.h>

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/SortLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/DevicePath.h>
#include <Protocol/IsaIo.h>

#include <Protocol/H2OIsaDevice.h>

#define MAX_AVAILABLE_UID   0xFF

#define DEVICE_ID_TO_ORDER_KEY(Hid, Uid) (UINT64) (\
  (UINT64)(Uid)   | \
  ((UINT64)(Hid)  << 32))

#define DEVICE_INFO_TO_ORDER_KEY(Hid, ConfigPort, ChipInstance, DeviceInstance) (UINT64) (\
  (UINT64)(DeviceInstance)          | \
  ((UINT64)(ChipInstance)   << 8)   | \
  ((UINT64)(ConfigPort)     << 16)  | \
  ((UINT64)(Hid)            << 32))

typedef struct {
  H2O_ISA_DEVICE_PROTOCOL     *DeviceProtocol;
  EFI_ISA_ACPI_RESOURCE_LIST  ResourceList;
} H2O_ISA_DEVICE;

typedef struct {
  EFI_ISA_ACPI_RESOURCE IsaIo;
  EFI_ISA_ACPI_RESOURCE IsaIrq;
  EFI_ISA_ACPI_RESOURCE IsaNull;
} ISA_ACPI_IO_IRQ_RESOURCE;

typedef struct {
  EFI_ISA_ACPI_RESOURCE IsaIo;
  EFI_ISA_ACPI_RESOURCE IsaIrq;
  EFI_ISA_ACPI_RESOURCE IsaDma;
  EFI_ISA_ACPI_RESOURCE IsaNull;
} ISA_ACPI_IO_IRQ_DMA_RESOURCE;

/**
  Enumerates the ISA devices on LPC interface.

  This service allows all the ISA devices on LPC interface to be enumerated.  If
  Device is a pointer to a NULL value, then the first ISA device on LPC interface
  is returned in Device and EFI_SUCCESS is returned.  If Device is a pointer
  to a value that was returned on a prior call to DeviceEnumerate(), then the next
  ISA device on LPC interface is returned in Device and EFI_SUCCESS is returned.
  If Device is a pointer to the last ISA device on LPC interface, then
  EFI_NOT_FOUND is returned.

  @param[in]  This     The pointer to the EFI_ISA_ACPI_PROTOCOL instance.
  @param[out] Device   The pointer to an ISA device named by ACPI HID/UID.

  @retval EFI_SUCCESS    The next ISA device on LPC interface was returned.
  @retval EFI_NOT_FOUND  No device found.

**/
EFI_STATUS
EFIAPI
IsaDeviceEnumerate (
  IN  EFI_ISA_ACPI_PROTOCOL   *This,
  OUT EFI_ISA_ACPI_DEVICE_ID  **Device
  );

/**
  Retrieves the current set of resources associated with an ISA device.

  Retrieves the set of I/O, MMIO, DMA, and interrupt resources currently
  assigned to the ISA device specified by Device.  These resources
  are returned in ResourceList.

  @param[in]  This          The pointer to the EFI_ISA_ACPI_PROTOCOL instance.
  @param[in]  Device        The pointer to an ISA device named by ACPI HID/UID.
  @param[out] ResourceList  The pointer to the current resource list for Device.

  @retval EFI_SUCCESS    Successfully retrieved the current resource list.
  @retval EFI_NOT_FOUND  The resource list could not be retrieved.

**/
EFI_STATUS
EFIAPI
IsaGetCurrentResource (
  IN  EFI_ISA_ACPI_PROTOCOL       *This,
  IN  EFI_ISA_ACPI_DEVICE_ID      *Device,
  OUT EFI_ISA_ACPI_RESOURCE_LIST  **ResourceList
  );

/**
  Initializes an ISA device, so that it can be used.  This service must be called
  before SetResource(), EnableDevice(), or SetPower() will behave as expected.

  @param[in] This     The pointer to the EFI_ISA_ACPI_PROTOCOL instance.
  @param[in] Device   The pointer to an ISA device named by ACPI HID/UID.

  @retval EFI_SUCCESS   Successfully initialized an ISA device.
  @retval Other         The ISA device could not be initialized.

**/
EFI_STATUS
EFIAPI
IsaInitDevice (
  IN  EFI_ISA_ACPI_PROTOCOL   *This,
  IN  EFI_ISA_ACPI_DEVICE_ID  *Device
  );

/**
  Assigns resources to an ISA device.

  Assigns the I/O, MMIO, DMA, and interrupt resources specified by ResourceList
  to the ISA device specified by Device.  ResourceList must match a resource list
  returned by GetPosResource() for the same ISA device.

  @param[in] This           The pointer to the EFI_ISA_ACPI_PROTOCOL instance.
  @param[in] Device         The pointer to an ISA device named by ACPI HID/UID.
  @param[in] ResourceList   The pointer to a resources list that must be one of the
                            resource lists returned by GetPosResource() for the
                            ISA device specified by Device.

  @retval EFI_SUCCESS  Successfully set resources on the ISA device.
  @retval Other        The resources could not be set for the ISA device.

**/
EFI_STATUS
EFIAPI
IsaSetResource (
  IN  EFI_ISA_ACPI_PROTOCOL       *This,
  IN  EFI_ISA_ACPI_DEVICE_ID      *Device,
  IN  EFI_ISA_ACPI_RESOURCE_LIST  *ResourceList
  );

/**
  Sets the power state of an ISA device.

  This services sets the power state of the ISA device specified by Device to
  the power state specified by On.  TRUE denotes on, FALSE denotes off.
  If the power state is successfully set on the ISA device, then
  EFI_SUCCESS is returned.

  @param[in] This     The pointer to the EFI_ISA_ACPI_PROTOCOL instance.
  @param[in] Device   The pointer to an ISA device named by ACPI HID/UID.
  @param[in] On       TRUE denotes on, FALSE denotes off.

  @retval EFI_SUCCESS   Successfully set the power state of the ISA device.
  @retval Other         The ISA device could not be placed in the requested power state.

**/
EFI_STATUS
EFIAPI
IsaSetPower (
  IN  EFI_ISA_ACPI_PROTOCOL   *This,
  IN  EFI_ISA_ACPI_DEVICE_ID  *Device,
  IN  BOOLEAN                 On
  );

/**
  Enables or disables an ISA device.

  @param[in] This     The pointer to the EFI_ISA_ACPI_PROTOCOL instance.
  @param[in] Device   The pointer to the ISA device to enable/disable.
  @param[in] Enable   TRUE to enable the ISA device.  FALSE to disable the
                      ISA device.

  @retval EFI_SUCCESS   Successfully enabled/disabled the ISA device.
  @retval Other         The ISA device could not be placed in the requested state.

**/
EFI_STATUS
EFIAPI
IsaEnableDevice (
  IN  EFI_ISA_ACPI_PROTOCOL   *This,
  IN  EFI_ISA_ACPI_DEVICE_ID  *Device,
  IN  BOOLEAN                 Enable
  );

/**
  Retrieves the set of possible resources that may be assigned to an ISA device
  with SetResource().

  Retrieves the possible sets of I/O, MMIO, DMA, and interrupt resources for the
  ISA device specified by Device.  The sets are returned in ResourceList.

  TODO: If IsaBusDxe needs the possible resource in the future, this function must
        be implmented to support that.

  @param[in]  This           The pointer to the EFI_ISA_ACPI_PROTOCOL instance.
  @param[in]  Device         The pointer to an ISA device named by ACPI HID/UID.
  @param[out] ResourceList   The pointer to the returned list of resource lists.

  @retval EFI_UNSUPPORTED  This service is currently not supported.

**/
EFI_STATUS
EFIAPI
IsaGetPossibleResource (
  IN  EFI_ISA_ACPI_PROTOCOL       *This,
  IN  EFI_ISA_ACPI_DEVICE_ID      *Device,
  OUT EFI_ISA_ACPI_RESOURCE_LIST  **ResourceList
  );

/**
  Initializes all the H/W states required for the ISA devices on LPC interface
  to be enumerated and managed by the rest of the services in this prorotol.
  This service must be called before any of the other services in this
  protocol will function as expected.

  @param[in] This  The pointer to the EFI_ISA_ACPI_PROTOCOL instance.

  @retval EFI_SUCCESS   Successfully initialized all required hardware states.
  @retval Other         The ISA interface could not be initialized.

**/
EFI_STATUS
EFIAPI
IsaInterfaceInit (
  IN  EFI_ISA_ACPI_PROTOCOL   *This
  );

/**
  This function provides an interface to free allocated programming resources in
  ISA Controller driver, cause it may not be a driver-model driver.

  @param[in] This  The pointer to the EFI_ISA_ACPI_PROTOCOL instance.

  @retval EFI_SUCCESS   Successfully free allocated programming resources.
  @retval Other         Something error happened.

**/
EFI_STATUS
EFIAPI
IsaAcpiDriverStopCallback (
  IN  EFI_ISA_ACPI_PROTOCOL   *This
  );

#endif