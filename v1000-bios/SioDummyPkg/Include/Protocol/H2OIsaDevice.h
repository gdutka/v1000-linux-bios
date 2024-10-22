/** @file
  H2O ISA Device Protocol

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

#ifndef _H2O_ISA_DEVICE_H_
#define _H2O_ISA_DEVICE_H_

#include <Uefi.h>

///
/// The Global ID for the H2O ISA Device Protocol.
///
#define H2O_ISA_DEVICE_PROTOCOL_GUID \
  { \
    0x25744fe3, 0x604a, 0x4c51, { 0x91, 0x2f, 0xdb, 0x2f, 0x91, 0xd3, 0xa1, 0xa5 } \
  }

typedef struct _H2O_ISA_DEVICE_PROTOCOL   H2O_ISA_DEVICE_PROTOCOL;

typedef struct {
  UINT8     ChipInstance;
  UINT16    ConfigPort;
  UINT8     DeviceInstance;
  UINT32    Hid;
  UINT32    Uid;
  BOOLEAN   IsDeviceExist;
} H2O_ISA_DEVICE_INFO;

typedef struct {
  UINT16  IoPort;
  UINT8   IrqNumber;
  UINT8   DmaChannel;
} H2O_ISA_DEVICE_RESOURCE;

/**
  Sets the power state of ISA device.

  This services sets the power state of the ISA device to the power state specified by On.
  TRUE denotes on, FALSE denotes off. If the power state is successfully set on the ISA device,
  then EFI_SUCCESS is returned.

  @param[in] This     The pointer to the H2O_ISA_DEVICE_PROTOCOL instance.
  @param[in] On       TRUE denotes on, FALSE denotes off.

  @retval EFI_SUCCESS   Successfully set the power state of the ISA device.
  @retval Other         The device could not be placed in the requested power state.

**/
typedef
EFI_STATUS
(EFIAPI *H2O_ISA_DEVICE_SET_POWER) (
  IN  H2O_ISA_DEVICE_PROTOCOL   *This,
  IN  BOOLEAN                   On
  );

/**
  Retrieves the current set of resources associated with ISA device.

  Retrieves the set of I/O, MMIO, DMA, and interrupt resources that is about to
  be assigned to the ISA device.  These resources are returned in DeviceResource.

  @param[in]  This            The pointer to the H2O_ISA_DEVICE_PROTOCOL instance.
  @param[out] DeviceResource  The pointer to the current resource that is about to
                              be assigned to ISA device.
                              The caller is responsible for freeing the buffer that
                              is allocated by callee.

  @retval EFI_SUCCESS    Successfully retrieved the current resource list.
  @retval EFI_NOT_FOUND  The resource list could not be retrieved.

**/
typedef
EFI_STATUS
(EFIAPI *H2O_ISA_DEVICE_GET_CURRENT_RESOURCE) (
  IN  H2O_ISA_DEVICE_PROTOCOL   *This,
  OUT H2O_ISA_DEVICE_RESOURCE   **DeviceResource
  );

/**
  Retrieves the set of possible resources that may be assigned to ISA device
  with SetResource().

  Retrieves the possible sets of I/O, MMIO, DMA, and interrupt resources for the
  ISA device. The sets are returned in DeviceResource.

  TODO: If IsaBusDxe needs the possible resource in the future, this function must
        be implemented to support that.

  @param[in]  This              The pointer to the H2O_ISA_DEVICE_PROTOCOL instance.
  @param[out] DeviceResource    The pointer to the returned list of resource lists.

  @retval EFI_UNSUPPORTED  This service is currently not supported.

**/
typedef
EFI_STATUS
(EFIAPI *H2O_ISA_DEVICE_GET_POSSIBLE_RESOURCE) (
  IN  H2O_ISA_DEVICE_PROTOCOL   *This,
  OUT H2O_ISA_DEVICE_RESOURCE   **DeviceResource,
  OUT UINT32                    *Uid
  );

/**
  Assigns resources to ISA device.

  Assigns the I/O, MMIO, DMA, and interrupt resources specified by DeviceResource
  to the ISA device.

  @param[in] This             The pointer to the H2O_ISA_DEVICE_PROTOCOL instance.
  @param[in] DeviceResource   The pointer to a resources list that must be one of the
                              resource lists returned by GetPosResource() for the
                              ISA device.

  @retval EFI_SUCCESS  Successfully set resources on the ISA device.
  @retval Other        The resources could not be set for the ISA device.

**/
typedef
EFI_STATUS
(EFIAPI *H2O_ISA_DEVICE_SET_RESOURCE) (
  IN  H2O_ISA_DEVICE_PROTOCOL   *This,
  IN  H2O_ISA_DEVICE_RESOURCE   *DeviceResource
  );

/**
  Enables or disables ISA device.

  @param[in] This     The pointer to the H2O_ISA_DEVICE_PROTOCOL instance.
  @param[in] Enable   TRUE to enable the ISA device.  FALSE to disable the ISA device.

  @retval EFI_SUCCESS   Successfully enabled/disabled the ISA device.
  @retval Other         The ISA device could not be placed in the requested state.

**/
typedef
EFI_STATUS
(EFIAPI *H2O_ISA_DEVICE_ENABLE_DEVICE) (
  IN  H2O_ISA_DEVICE_PROTOCOL   *This,
  IN  BOOLEAN                   Enable
  );

/**
  Retrieves the device information that is associated with ISA device.

  @param[in]  This        The pointer to the H2O_ISA_DEVICE_PROTOCOL instance.
  @param[out] DeviceInfo  The device information associated with ISA device.
                          The caller is responsible for freeing the buffer that
                          is allocated by callee.

  @retval EFI_SUCCESS   Successfully retrieved the device information.
  @retval Other         The device information could not be retrieved.

**/
typedef
EFI_STATUS
(EFIAPI *H2O_ISA_DEVICE_GET_DEVICE_INFO) (
  IN  H2O_ISA_DEVICE_PROTOCOL   *This,
  OUT H2O_ISA_DEVICE_INFO       **DeviceInfo
  );

/**
  Set UID field in device information associated with ISA device.

  This function would check ChipInstance, ConfigPort and DeviceInstance in device
  information. If they matches this device, the UID would be updated

  @param[in] This        The pointer to the H2O_ISA_DEVICE_PROTOCOL instance.
  @param[in] DeviceInfo  The pointer to device information associated with ISA device.

  @retval EFI_SUCCESS   Successfully retrieved the device information.
  @retval Other         The UID could not be updated.

**/
typedef
EFI_STATUS
(EFIAPI *H2O_ISA_DEVICE_SET_UID) (
  IN  H2O_ISA_DEVICE_PROTOCOL   *This,
  IN  H2O_ISA_DEVICE_INFO       *DeviceInfo
  );

struct _H2O_ISA_DEVICE_PROTOCOL {
  UINT32                                Size;
  H2O_ISA_DEVICE_SET_POWER              SetPower;
  H2O_ISA_DEVICE_GET_CURRENT_RESOURCE   GetCurrentResource;
  H2O_ISA_DEVICE_GET_POSSIBLE_RESOURCE  GetPossibleResource;
  H2O_ISA_DEVICE_SET_RESOURCE           SetResource;
  H2O_ISA_DEVICE_ENABLE_DEVICE          EnableDevice;
  H2O_ISA_DEVICE_GET_DEVICE_INFO        GetDeviceInfo;
  H2O_ISA_DEVICE_SET_UID                SetUid;
};

extern EFI_GUID gH2OIsaDeviceProtocolGuid;

#endif