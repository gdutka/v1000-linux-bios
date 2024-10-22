/** @file
 DUMMY PS/2 Mouse

;******************************************************************************
;* Copyright (c) 2019 Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/

#if FixedPcdGetBool (PcdH2OSioDummyChip1KbcSupported)
#include <Uefi.h>
#include <SioPrivateDefine.h>
#include <Library/SioDummyPrivateLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <SioIsaDeviceContext.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/IsaAcpi.h>
#include <Protocol/SioInstancePrivateInfo.h>

/**
  Sets the power state of PS/2 Mouse.

  This services sets the power state of the PS/2 Mouse to the power state specified by On.
  TRUE denotes on, FALSE denotes off. If the power state is successfully set on the PS/2 Mouse,
  then EFI_SUCCESS is returned.

  @param[in] This     The pointer to the H2O_ISA_DEVICE_PROTOCOL instance.
  @param[in] On       TRUE denotes on, FALSE denotes off.

  @retval EFI_SUCCESS   Successfully set the power state of the PS/2 Mouse.
  @retval Other         The device could not be placed in the requested power state.

**/
EFI_STATUS
Ps2MsSetPower (
  IN  H2O_ISA_DEVICE_PROTOCOL   *This,
  IN  BOOLEAN                   On
  )
{
  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_UNSUPPORTED;
}

/**
  Retrieves the current set of resources associated with PS/2 Mouse.

  Retrieves the set of I/O, MMIO, DMA, and interrupt resources that is about to
  be assigned to the PS/2 Mouse.  These resources are returned in DeviceResource.

  @param[in]  This            The pointer to the H2O_ISA_DEVICE_PROTOCOL instance.
  @param[out] DeviceResource  The pointer to the current resource that is about to
                              be assigned to PS/2 Mouse.
                              The caller is responsible for freeing the buffer that
                              is allocated by callee.

  @retval EFI_SUCCESS    Successfully retrieved the current resource list.
  @retval EFI_NOT_FOUND  The resource list could not be retrieved.

**/
EFI_STATUS
Ps2MsGetCurrentResource (
  IN  H2O_ISA_DEVICE_PROTOCOL   *This,
  OUT H2O_ISA_DEVICE_RESOURCE   **DeviceResource
  )
{
  H2O_ISA_DEVICE_CONTEXT  *Context;

  if (This == NULL || DeviceResource == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Context = H2O_ISA_DEVICE_CONTEXT_FROM_PROTOCOL (This);
  (*DeviceResource) = (H2O_ISA_DEVICE_RESOURCE *) AllocateCopyPool (
                                                    sizeof (H2O_ISA_DEVICE_RESOURCE),
                                                    &Context->DeviceResource
                                                    );

  if ((*DeviceResource) == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  return EFI_SUCCESS;
}

/**
  Retrieves the set of possible resources that may be assigned to PS/2 Mouse
  with SetResource().

  Retrieves the possible sets of I/O, MMIO, DMA, and interrupt resources for the
  PS/2 Mouse. The sets are returned in DeviceResource.

  TODO: If IsaBusDxe needs the possible resource in the future, this function must
        be implemented to support that.

  @param[in]  This              The pointer to the H2O_ISA_DEVICE_PROTOCOL instance.
  @param[out] DeviceResource    The pointer to the returned list of resource lists.

  @retval EFI_UNSUPPORTED  This service is currently not supported.

**/
EFI_STATUS
Ps2MsGetPossibleResource (
  IN  H2O_ISA_DEVICE_PROTOCOL   *This,
  OUT H2O_ISA_DEVICE_RESOURCE   **DeviceResource,
  OUT UINT32                    *Uid
  )
{
  if (This == NULL || (*DeviceResource == NULL) || Uid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_UNSUPPORTED;
}

/**
  Assigns resources to PS/2 Mouse.

  Assigns the I/O, MMIO, DMA, and interrupt resources specified by DeviceResource
  to the PS/2 Mouse.

  @param[in] This             The pointer to the H2O_ISA_DEVICE_PROTOCOL instance.
  @param[in] DeviceResource   The pointer to a resources list that must be one of the
                              resource lists returned by GetPosResource() for the
                              PS/2 Mouse.

  @retval EFI_SUCCESS  Successfully set resources on the PS/2 Mouse.
  @retval Other        The resources could not be set for the PS/2 Mouse.

**/
EFI_STATUS
Ps2MsSetResource (
  IN  H2O_ISA_DEVICE_PROTOCOL   *This,
  IN  H2O_ISA_DEVICE_RESOURCE   *DeviceResource
  )
{
  return EFI_SUCCESS;
}

/**
  Enables or disables PS/2 Mouse.

  @param[in] This     The pointer to the H2O_ISA_DEVICE_PROTOCOL instance.
  @param[in] Enable   TRUE to enable the PS/2 Mouse.  FALSE to disable the PS/2 Mouse.

  @retval EFI_SUCCESS   Successfully enabled/disabled the PS/2 Mouse.
  @retval Other         The PS/2 Mouse could not be placed in the requested state.

**/
EFI_STATUS
Ps2MsEnableDevice (
  IN  H2O_ISA_DEVICE_PROTOCOL   *This,
  IN  BOOLEAN                   Enable
  )
{
  return EFI_SUCCESS;
}

/**
  Retrieves the device information that is associated with PS/2 Mouse.

  @param[in]  This          The pointer to the H2O_ISA_DEVICE_PROTOCOL instance.
  @param[out] DeviceInfo    The device information associated with PS/2 Mouse.
                            The caller is responsible for freeing the buffer that
                            is allocated by callee.

  @retval     EFI_SUCCESS   Successfully retrieved the device information.
  @retval     Other         The device information could not be retrieved.

**/
EFI_STATUS
Ps2MsGetDeviceInfo (
  IN  H2O_ISA_DEVICE_PROTOCOL   *This,
  OUT H2O_ISA_DEVICE_INFO       **DeviceInfo
  )
{
  H2O_ISA_DEVICE_CONTEXT  *Context;

  if (This == NULL || DeviceInfo == NULL) {
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
  Set UID field in device information associated with PS/2 Mouse.

  This function would check ChipInstance, ConfigPort and DeviceInstance in device
  information. If they matches this device, the UID would be updated.

  @param[in]  This          The pointer to the H2O_ISA_DEVICE_PROTOCOL instance.
  @param[in]  DeviceInfo    The pointer to device information associated with PS/2 Mouse.

  @retval     EFI_SUCCESS   Successfully retrieved the device information.
  @retval     Other         The UID could not be updated.

**/
EFI_STATUS
Ps2MsSetUid (
  IN  H2O_ISA_DEVICE_PROTOCOL   *This,
  IN  H2O_ISA_DEVICE_INFO       *DeviceInfo
  )
{
  H2O_ISA_DEVICE_CONTEXT  *Context;

  if (This == NULL || DeviceInfo == NULL) {
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
  Select alternative function of GPIO pins for PS/2 Mouse

  This function is about to be obsolete. If there's no implementation here,
  OEM team could configures it via DxeCustomizedTable by themselves.

  TODO: If this function needs another resource (e.g. Runtime I/O base address),
        the parameters could be expanded or modified on demand, and the caller
        has responsibility to provide such information.

  @param[in]  ConfigPort    Configuration DATA port.

**/
VOID
SioPs2MsSelectAltFunc (
  UINT16  ConfigPort
  )
{
}

/**
  Pre-configure PS/2 Mouse before being enumerated by IsaBusDxe.

  This function may configures additional settings about PS/2 Mouse, such as clock source,
  peripheral type, IRQ sharing or IRQ trigger mode.

  @param[in]  Resource      The pointer to configuration resource

  @retval     EFI_SUCCESS   Function complete successfully.
  @retval     other         Error occurs when configuring.

**/
EFI_STATUS
SioPs2MsPreConfig (
  IN  SIO_CONFIGURED_RESOURCE   *Resource
  )
{
  //
  // TODO: Add chip-specific code to configure additional settings.
  //

  SioPs2MsSelectAltFunc (Resource->ConfigPort);

  return EFI_SUCCESS;
}

/**
  Initialize DUMMY PS/2 Mouse.

  This function must do all following tasks:
  1. Initialize H2O ISA Device Context
  2. Install H2O ISA Device Protocol on Device Handle, IsaAcpiDxe needs it to collect
     information about this device.
  3. Update corresponding fields in SIO ISA Device Info associated with H2O ISA Device
     Info Protocol for SSDT driver.

  @param[in]  Resource      The pointer to the SIO_CONFIGURED_RESOURCE instance.

  @retval     EFI_SUCCESS   Function complete successfully.
  @retval     other         Error occurs.

**/
EFI_STATUS
InitializeSioPs2Ms (
  IN  SIO_CONFIGURED_RESOURCE   *Resource
  )
{
  EFI_STATUS                    Status;
  H2O_ISA_DEVICE_CONTEXT        *DeviceContext;


  Status = SioPs2MsPreConfig (Resource);
  if (EFI_ERROR (Status) && Status != EFI_UNSUPPORTED) {
    DEBUG ((EFI_D_ERROR, "InitializeSioPs2Ms() - SioPs2MsPreConfig failed: %r\n", Status));
    return Status;
  }

  DeviceContext = AllocateZeroPool (sizeof (H2O_ISA_DEVICE_CONTEXT));
  if (DeviceContext == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  DeviceContext->Signature                            = H2O_ISA_DEVICE_CONTEXT_SIGNATURE;

  DeviceContext->DeviceProtocol.Size                  = sizeof (H2O_ISA_DEVICE_PROTOCOL);
  DeviceContext->DeviceProtocol.SetPower              = Ps2MsSetPower;
  DeviceContext->DeviceProtocol.GetCurrentResource    = Ps2MsGetCurrentResource;
  DeviceContext->DeviceProtocol.GetPossibleResource   = Ps2MsGetPossibleResource;
  DeviceContext->DeviceProtocol.SetResource           = Ps2MsSetResource;
  DeviceContext->DeviceProtocol.EnableDevice          = Ps2MsEnableDevice;
  DeviceContext->DeviceProtocol.GetDeviceInfo         = Ps2MsGetDeviceInfo;
  DeviceContext->DeviceProtocol.SetUid                = Ps2MsSetUid;

  DeviceContext->DeviceInfo.ChipInstance              = Resource->ChipInstance;
  DeviceContext->DeviceInfo.ConfigPort                = Resource->ConfigPort;
  DeviceContext->DeviceInfo.DeviceInstance            = Resource->DevInstance;
  DeviceContext->DeviceInfo.Hid                       = EISA_PNP_ID (0x303);
  DeviceContext->DeviceInfo.Uid                       = GetIsaDeviceUid (
                                                          PcdGetPtr (PcdH2OSioDummyChip1IsaDeviceUid),
                                                          PcdGetSize (PcdH2OSioDummyChip1IsaDeviceUid),
                                                          SioPs2MouseDev,
                                                          Resource->DevInstance
                                                          );
  DeviceContext->DeviceInfo.IsDeviceExist             = TRUE;

  DeviceContext->DeviceResource.IoPort                = SIO_KBC_DATA_PORT;
  DeviceContext->DeviceResource.IrqNumber             = SIO_MOUSE_IRQ12;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &DeviceContext->Handle,
                  &gH2OIsaDeviceProtocolGuid,
                  &DeviceContext->DeviceProtocol,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "InitializeSioPs2Ms() - Failed to install protocol interface: %r\n", Status));
    FreePool (DeviceContext);
    return Status;
  }

  return EFI_SUCCESS;
}
#endif
