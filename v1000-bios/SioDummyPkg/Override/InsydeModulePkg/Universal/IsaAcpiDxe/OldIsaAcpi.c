/** @file
  Old ISA ACPI Protocol Implementation

;******************************************************************************
;* Copyright (c) 2019 - 2020, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "IsaAcpi.h"
#include "OldIsaAcpi.h"

STATIC H2O_OLD_ISA_PROTOCOL   mOldIsaProtocols[] = {
  { EISA_PNP_ID (0x303),  &gEfiIsaPnp303DeviceProtocolGuid,   NULL, 0 },
  { EISA_PNP_ID (0x401),  &gEfiIsaPnp401DeviceProtocolGuid,   NULL, 0 },
  { EISA_PNP_ID (0x501),  &gEfiIsaPnp501DeviceProtocolGuid,   NULL, 0 },
  { EISA_PNP_ID (0x510),  &gEfiIsaPnp510DeviceProtocolGuid,   NULL, 0 },
  { EISA_PNP_ID (0x604),  &gEfiIsaPnp604DeviceProtocolGuid,   NULL, 0 },
  { EISA_PNP_ID (0xB02F), &gEfiIsaPnpB02fDeviceProtocolGuid,  NULL, 0 },
  { EISA_PNP_ID (0xF03),  &gEfiIsaPnpF03DeviceProtocolGuid,   NULL, 0 }
};

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
EFI_STATUS
IsaDeviceSetPower (
  IN  H2O_ISA_DEVICE_PROTOCOL   *This,
  IN  BOOLEAN                   On
  )
{
  H2O_ISA_DEVICE_CONTEXT  *Context;
  H2O_OLD_ISA_DEVICE      *Device;

  Context = H2O_ISA_DEVICE_CONTEXT_FROM_PROTOCOL (This);
  Device  = H2O_OLD_ISA_DEVICE_FROM_CONTEXT (Context);

  return Device->FunctionPtr->SetPower (On);
}

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
EFI_STATUS
IsaDeviceGetCurrentResource (
  IN  H2O_ISA_DEVICE_PROTOCOL   *This,
  OUT H2O_ISA_DEVICE_RESOURCE   **DeviceResource
  )
{
  H2O_ISA_DEVICE_CONTEXT    *Context;
  H2O_OLD_ISA_DEVICE        *Device;
  H2O_ISA_DEVICE_RESOURCE   *TempDeviceResource;
  EFI_STATUS                Status;

  Context = H2O_ISA_DEVICE_CONTEXT_FROM_PROTOCOL (This);
  Device  = H2O_OLD_ISA_DEVICE_FROM_CONTEXT (Context);

  Status = Device->FunctionPtr->GetCurrentResource (&TempDeviceResource);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  (*DeviceResource) = AllocateCopyPool (sizeof (H2O_ISA_DEVICE_RESOURCE), TempDeviceResource);
  if ((*DeviceResource) == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  return EFI_SUCCESS;
}

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
EFI_STATUS
IsaDeviceGetPossibleResource (
  IN  H2O_ISA_DEVICE_PROTOCOL   *This,
  OUT H2O_ISA_DEVICE_RESOURCE   **DeviceResource,
  OUT UINT32                    *Uid
  )
{
  H2O_ISA_DEVICE_CONTEXT  *Context;
  H2O_OLD_ISA_DEVICE      *Device;

  Context = H2O_ISA_DEVICE_CONTEXT_FROM_PROTOCOL (This);
  Device  = H2O_OLD_ISA_DEVICE_FROM_CONTEXT (Context);

  return Device->FunctionPtr->GetPossibleResource (DeviceResource, Uid);
}

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
EFI_STATUS
IsaDeviceSetResource (
  IN  H2O_ISA_DEVICE_PROTOCOL   *This,
  IN  H2O_ISA_DEVICE_RESOURCE   *DeviceResource
  )
{
  H2O_ISA_DEVICE_CONTEXT  *Context;
  H2O_OLD_ISA_DEVICE      *Device;

  Context = H2O_ISA_DEVICE_CONTEXT_FROM_PROTOCOL (This);
  Device  = H2O_OLD_ISA_DEVICE_FROM_CONTEXT (Context);

  return Device->FunctionPtr->SetResource (DeviceResource);
}

/**
  Enables or disables ISA device.

  @param[in] This     The pointer to the H2O_ISA_DEVICE_PROTOCOL instance.
  @param[in] Enable   TRUE to enable the ISA device.  FALSE to disable the ISA device.

  @retval EFI_SUCCESS   Successfully enabled/disabled the ISA device.
  @retval Other         The ISA device could not be placed in the requested state.

**/
EFI_STATUS
IsaDeviceEnableDevice (
  IN  H2O_ISA_DEVICE_PROTOCOL   *This,
  IN  BOOLEAN                   Enable
  )
{
  H2O_ISA_DEVICE_CONTEXT  *Context;
  H2O_OLD_ISA_DEVICE      *Device;

  Context = H2O_ISA_DEVICE_CONTEXT_FROM_PROTOCOL (This);
  Device  = H2O_OLD_ISA_DEVICE_FROM_CONTEXT (Context);

  return Device->FunctionPtr->EnableDevice (Enable);
}

/**
  Retrieves the device information that is associated with ISA device.

  @param[in]  This        The pointer to the H2O_ISA_DEVICE_PROTOCOL instance.
  @param[out] DeviceInfo  The device information associated with ISA device.
                          The caller is responsible for freeing the buffer that
                          is allocated by callee.

  @retval EFI_SUCCESS   Successfully retrieved the device information.
  @retval Other         The device information could not be retrieved.

**/
EFI_STATUS
IsaDeviceGetDeviceInfo (
  IN  H2O_ISA_DEVICE_PROTOCOL   *This,
  OUT H2O_ISA_DEVICE_INFO       **DeviceInfo
  )
{
  H2O_ISA_DEVICE_CONTEXT  *Context;

  if (DeviceInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Context = H2O_ISA_DEVICE_CONTEXT_FROM_PROTOCOL (This);
  (*DeviceInfo) = (H2O_ISA_DEVICE_INFO *) AllocateCopyPool (sizeof (H2O_ISA_DEVICE_INFO), &Context->DeviceInfo);

  if (*DeviceInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  return EFI_SUCCESS;
}

/**
  Set UID field in device information associated with ISA device.

  This function would check ChipInstance, ConfigPort and DeviceInstance in device
  information. If they matches this device, the UID would be updated

  @param[in] This        The pointer to the H2O_ISA_DEVICE_PROTOCOL instance.
  @param[in] DeviceInfo  The pointer to device information associated with ISA device.

  @retval EFI_SUCCESS   Successfully retrieved the device information.
  @retval Other         The UID could not be updated.

**/
EFI_STATUS
IsaDeviceSetUid (
  IN  H2O_ISA_DEVICE_PROTOCOL   *This,
  IN  H2O_ISA_DEVICE_INFO       *DeviceInfo
  )
{
  H2O_ISA_DEVICE_CONTEXT  *Context;

  if (DeviceInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Context = H2O_ISA_DEVICE_CONTEXT_FROM_PROTOCOL (This);

  if (DeviceInfo->ChipInstance != Context->DeviceInfo.ChipInstance ||
      DeviceInfo->ConfigPort != Context->DeviceInfo.ConfigPort ||
      DeviceInfo->DeviceInstance != Context->DeviceInfo.DeviceInstance ||
      DeviceInfo->Hid != Context->DeviceInfo.Hid
      )
  {
    return EFI_INVALID_PARAMETER;
  }

  Context->DeviceInfo.Uid = DeviceInfo->Uid;

  return EFI_SUCCESS;
}

/**
  Publish old-policy ISA devices.

  Term "old-policy" means the old interface associated with gEfiIsaPnpXXXDeviceProtocolGuid
  which is about to be obsolete. This function would convert its interface to a new one.

  @retval EFI_SUCCESS   Successfully publishes old-policy ISA devices.
  @retval Other         The old-policy ISA devices could not be published.

**/
EFI_STATUS
PublishOldPolicyIsaDevices (
  VOID
  )
{
  EFI_STATUS                Status;
  UINTN                     ProtocolIndex;
  EFI_HANDLE                *HandleBuffer;
  UINTN                     HandleBufferCount;
  UINTN                     HandleIndex;
  H2O_OLD_ISA_DEVICE        *Devices;
  EFI_ISA_DEVICE_RESOURCE   *DeviceResource;
  UINT32                    Uid;

  for (ProtocolIndex = 0; ProtocolIndex < sizeof (mOldIsaProtocols) / sizeof (H2O_OLD_ISA_PROTOCOL); ProtocolIndex++) {
    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    mOldIsaProtocols[ProtocolIndex].Guid,
                    NULL,
                    &HandleBufferCount,
                    &HandleBuffer
                    );
    if (Status == EFI_NOT_FOUND) {
      continue;
    }

    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "PublishOldPolicyIsaDevices() - Failed to locate handle buffer: %r\n", Status));
      return Status;
    }

    Devices = AllocateZeroPool (HandleBufferCount * sizeof (H2O_OLD_ISA_DEVICE));
    if (Devices == NULL) {
      DEBUG ((EFI_D_ERROR, "Allocate pool error\n"));
      FreePool (HandleBuffer);
      return EFI_OUT_OF_RESOURCES;
    } 
    for (HandleIndex = 0; HandleIndex < HandleBufferCount; HandleIndex++) {
      Status = gBS->HandleProtocol (
                      HandleBuffer[HandleIndex],
                      mOldIsaProtocols[ProtocolIndex].Guid,
                      (VOID**) &Devices[HandleIndex].FunctionPtr
                      );
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "PublishOldPolicyIsaDevices() - Failed to get protocol from handle: %r\n", Status));
        return Status;
      }

      DeviceResource  = NULL;
      Uid             = MAX_AVAILABLE_UID;
      Status = Devices[HandleIndex].FunctionPtr->GetPossibleResource (&DeviceResource, &Uid);
      ASSERT_EFI_ERROR (Status);
      if (DeviceResource == NULL || Uid > MAX_AVAILABLE_UID) {
        DEBUG ((EFI_D_WARN, "PublishOldPolicyIsaDevices() - Failed to get possible resource from device"));
        continue;
      }

      if (DeviceResource->DmaChannel == NULL_ID) {
        continue;
      }

      Devices[HandleIndex].Signature                                          = H2O_OLD_ISA_DEVICE_SIGNATURE;

      Devices[HandleIndex].DeviceContext.Signature                            = H2O_ISA_DEVICE_CONTEXT_SIGNATURE;

      Devices[HandleIndex].DeviceContext.DeviceProtocol.Size                  = sizeof (H2O_ISA_DEVICE_PROTOCOL);
      Devices[HandleIndex].DeviceContext.DeviceProtocol.SetPower              = IsaDeviceSetPower;
      Devices[HandleIndex].DeviceContext.DeviceProtocol.GetCurrentResource    = IsaDeviceGetCurrentResource;
      Devices[HandleIndex].DeviceContext.DeviceProtocol.GetPossibleResource   = IsaDeviceGetPossibleResource;
      Devices[HandleIndex].DeviceContext.DeviceProtocol.SetResource           = IsaDeviceSetResource;
      Devices[HandleIndex].DeviceContext.DeviceProtocol.EnableDevice          = IsaDeviceEnableDevice;
      Devices[HandleIndex].DeviceContext.DeviceProtocol.GetDeviceInfo         = IsaDeviceGetDeviceInfo;
      Devices[HandleIndex].DeviceContext.DeviceProtocol.SetUid                = IsaDeviceSetUid;

      Devices[HandleIndex].DeviceContext.DeviceInfo.IsDeviceExist             = TRUE;
      Devices[HandleIndex].DeviceContext.DeviceInfo.Hid                       = mOldIsaProtocols[ProtocolIndex].Hid;
      Devices[HandleIndex].DeviceContext.DeviceInfo.Uid                       = 0xFFFF;   // force auto-config

      Devices[HandleIndex].DeviceContext.DeviceResource.IoPort                = DeviceResource->IoPort;
      Devices[HandleIndex].DeviceContext.DeviceResource.IrqNumber             = DeviceResource->IrqNumber;
      Devices[HandleIndex].DeviceContext.DeviceResource.DmaChannel            = DeviceResource->DmaChannel;

      Status = gBS->InstallMultipleProtocolInterfaces (
                      &Devices[HandleIndex].DeviceContext.Handle,
                      &gH2OIsaDeviceProtocolGuid,
                      &Devices[HandleIndex].DeviceContext.DeviceProtocol,
                      NULL
                      );
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "PublishOldPolicyIsaDevices() - Failed to install protocol interface: %r\n", Status));
        return Status;
      }
    }

    mOldIsaProtocols[ProtocolIndex].Devices     = Devices;
    mOldIsaProtocols[ProtocolIndex].DeviceCount = HandleBufferCount;

    FreePool (HandleBuffer);
  }

  return EFI_SUCCESS;
}

/**
  Revoke old-policy ISA devices.

  Term "old-policy" means the old interface associated with gEfiIsaPnpXXXDeviceProtocolGuid
  which is about to be obsolete. This function would uninstall protocol interface for all
  old-policy ISA devices and free new interfaces created by PublishOldPolicyIsaDevices().
  This function should be called only when error happens.

  @retval EFI_SUCCESS   Successfully revokes old-policy ISA devices.
  @retval Other         The old-policy ISA devices could not be revoked.

**/
EFI_STATUS
RevokeOldPolicyIsaDevices (
  VOID
  )
{
  EFI_STATUS          Status;
  UINTN               ProtocolIndex;
  H2O_OLD_ISA_DEVICE  *Devices;
  UINTN               DeviceCount;
  UINTN               DeviceIndex;

  for (ProtocolIndex = 0; ProtocolIndex < sizeof (mOldIsaProtocols) / sizeof (H2O_OLD_ISA_PROTOCOL); ProtocolIndex++) {
    Devices  = mOldIsaProtocols[ProtocolIndex].Devices;
    DeviceCount = mOldIsaProtocols[ProtocolIndex].DeviceCount;

    if (Devices != NULL) {
      for (DeviceIndex = 0; DeviceIndex < DeviceCount; DeviceIndex ++) {
        Status = gBS->UninstallMultipleProtocolInterfaces (
                        Devices[DeviceIndex].DeviceContext.Handle,
                        &gH2OIsaDeviceProtocolGuid,
                        &Devices[DeviceIndex].DeviceContext.DeviceProtocol,
                        NULL
                        );
        if (EFI_ERROR (Status)) {
          DEBUG ((EFI_D_ERROR, "RevokeOldPolicyIsaDevices() - Failed to uninstall protocol interface: %r\n", Status));
          return Status;
        }
      }

      FreePool (mOldIsaProtocols[ProtocolIndex].Devices);
      mOldIsaProtocols[ProtocolIndex].Devices     = NULL;
      mOldIsaProtocols[ProtocolIndex].DeviceCount = 0;
    }
  }

  return EFI_SUCCESS;
}