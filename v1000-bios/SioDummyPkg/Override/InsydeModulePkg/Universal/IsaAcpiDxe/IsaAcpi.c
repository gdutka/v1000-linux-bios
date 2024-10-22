/** @file
  ISA ACPI Protocol Implementation

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

extern VOID InitDmaController (VOID);

BOOLEAN         mIsDmaCompatibleDevicePresented = FALSE;
BOOLEAN         mIsDmaControllerInitialized     = FALSE;
H2O_ISA_DEVICE  *mIsaDevices;
UINTN           mDeviceCount;

/**
  Initialize mIsaDevices and mDeviceCount.

  @return EFI_SUCCESS   The operation completed successfully.
  @return others        Some error occurred during initialization.

**/
EFI_STATUS
InitIsaDevices (
  VOID
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  *HandleBuffer;
  UINTN       HandleBufferCount;
  UINTN       Index;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gH2OIsaDeviceProtocolGuid,
                  NULL,
                  &HandleBufferCount,
                  &HandleBuffer
                  );

  if (Status == EFI_NOT_FOUND) {
    DEBUG ((EFI_D_INFO, "InitIsaDevices() - No ISA device is detected.\n"));
    return Status;
  }

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "InitIsaDevices() - Failed to locate handle buffer: %r\n", Status));
    return Status;
  }

  mDeviceCount  = HandleBufferCount;
  mIsaDevices   = AllocateZeroPool (mDeviceCount * sizeof (H2O_ISA_DEVICE));
  if (mIsaDevices == NULL) {
    FreePool (HandleBuffer);
    return EFI_OUT_OF_RESOURCES;
  } 

  for (Index = 0; Index < mDeviceCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gH2OIsaDeviceProtocolGuid,
                    (VOID**) &mIsaDevices[Index].DeviceProtocol
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "InitIsaDevices() - Failed to get protocol from handle: %r\n", Status));
      FreePool (HandleBuffer);
      FreePool (mIsaDevices);
      mDeviceCount = 0;
      return Status;
    }
  }

  FreePool (HandleBuffer);

  return EFI_SUCCESS;
}

/**
  Helper function to initialize common resource for an resource item.

  The prototype of ResourceItem must be ISA_ACPI_IO_IRQ_RESOURCE or
  ISA_ACPI_IO_IRQ_DMA_RESOURCE.

  @param[in]  Hid             Device's HID
  @param[in]  DeviceResource  The pointer to H2O_ISA_DEVICE_RESOURCE data structure.
  @param[out] ResourceItem    The pointer to EFI_ISA_ACPI_RESOURCE data structure.

**/
VOID
InitCommonResource (
  IN  UINT32                    Hid,
  IN  H2O_ISA_DEVICE_RESOURCE   *DeviceResource,
  OUT EFI_ISA_ACPI_RESOURCE     *ResourceItem
  )
{
  ((ISA_ACPI_IO_IRQ_RESOURCE*) ResourceItem)->IsaIo.Type       = EfiIsaAcpiResourceIo;
  ((ISA_ACPI_IO_IRQ_RESOURCE*) ResourceItem)->IsaIo.StartRange = DeviceResource->IoPort;
  if (Hid == EISA_PNP_ID (0x303) || Hid == EISA_PNP_ID (0xF03)) {
    ((ISA_ACPI_IO_IRQ_RESOURCE*) ResourceItem)->IsaIo.EndRange = DeviceResource->IoPort + 4;
  } else {
    ((ISA_ACPI_IO_IRQ_RESOURCE*) ResourceItem)->IsaIo.EndRange = DeviceResource->IoPort | 7;
  }
  ((ISA_ACPI_IO_IRQ_RESOURCE*) ResourceItem)->IsaIo.Attribute  = 0;

  ((ISA_ACPI_IO_IRQ_RESOURCE*) ResourceItem)->IsaIrq.Type       = EfiIsaAcpiResourceInterrupt;
  ((ISA_ACPI_IO_IRQ_RESOURCE*) ResourceItem)->IsaIrq.StartRange = DeviceResource->IrqNumber;
  ((ISA_ACPI_IO_IRQ_RESOURCE*) ResourceItem)->IsaIrq.EndRange   = 0;
  ((ISA_ACPI_IO_IRQ_RESOURCE*) ResourceItem)->IsaIrq.Attribute  = 0;
}

/**
  Helper function to initialize DMA resource for an resource item.

  The prototype of ResourceItem must be ISA_ACPI_IO_IRQ_DMA_RESOURCE.

  @param[in]  DeviceResource  The pointer to H2O_ISA_DEVICE_RESOURCE data structure.
  @param[out] ResourceItem    The pointer to EFI_ISA_ACPI_RESOURCE data structure.

**/
VOID
InitDmaResource (
  IN  H2O_ISA_DEVICE_RESOURCE   *DeviceResource,
  OUT EFI_ISA_ACPI_RESOURCE     *ResourceItem
  )
{
  ((ISA_ACPI_IO_IRQ_DMA_RESOURCE*) ResourceItem)->IsaDma.Type       = EfiIsaAcpiResourceDma;
  ((ISA_ACPI_IO_IRQ_DMA_RESOURCE*) ResourceItem)->IsaDma.StartRange = DeviceResource->DmaChannel;
  ((ISA_ACPI_IO_IRQ_DMA_RESOURCE*) ResourceItem)->IsaDma.EndRange   = 0;
  ((ISA_ACPI_IO_IRQ_DMA_RESOURCE*) ResourceItem)->IsaDma.Attribute  =
    (UINT32) (EFI_ISA_IO_SLAVE_DMA_ATTRIBUTE_SPEED_COMPATIBLE |
    EFI_ISA_IO_SLAVE_DMA_ATTRIBUTE_WIDTH_8 |
    EFI_ISA_IO_SLAVE_DMA_ATTRIBUTE_SINGLE_MODE
    );
}

/**
  Initialize ResourceList for all devices in mIsaDevices.

  @return EFI_SUCCESS           The operation completed successfully.
  @return EFI_OUT_OF_RESOURCE   Insufficient memory for initialization.

**/
EFI_STATUS
InitIsaDeviceResources (
  VOID
  )
{
  UINTN                     Index;
  H2O_ISA_DEVICE_PROTOCOL   *DeviceProtocol;
  H2O_ISA_DEVICE_INFO       *DeviceInfo;
  H2O_ISA_DEVICE_RESOURCE   *DeviceResource;
  EFI_ISA_ACPI_RESOURCE     *ResourceItem;
  EFI_ISA_ACPI_RESOURCE     ResourceItemNull;
  EFI_STATUS                Status;

  DeviceInfo      = NULL;
  DeviceResource  = NULL;
  ResourceItem    = NULL;

  ResourceItemNull.Type       = EfiIsaAcpiResourceEndOfList;
  ResourceItemNull.StartRange = 0;
  ResourceItemNull.EndRange   = 0;
  ResourceItemNull.Attribute  = 0;

  for (Index = 0; Index < mDeviceCount; Index++) {
    DeviceProtocol = mIsaDevices[Index].DeviceProtocol;

    Status = DeviceProtocol->GetDeviceInfo (DeviceProtocol, &DeviceInfo);
    if (!EFI_ERROR (Status)) {
      Status = DeviceProtocol->GetCurrentResource (DeviceProtocol, &DeviceResource);
    }
    if (EFI_ERROR (Status)) {
      return Status;
    }

    switch (DeviceInfo->Hid) {

    case EISA_PNP_ID (0x401):
    case EISA_PNP_ID (0x604):
      mIsDmaCompatibleDevicePresented = TRUE;

      ResourceItem = AllocateZeroPool (sizeof (ISA_ACPI_IO_IRQ_DMA_RESOURCE));
      if (ResourceItem == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto ErrorExit;
      }

      InitCommonResource (DeviceInfo->Hid, DeviceResource, ResourceItem);
      InitDmaResource (DeviceResource, ResourceItem);
      CopyMem (
        &((ISA_ACPI_IO_IRQ_DMA_RESOURCE*) ResourceItem)->IsaNull,
        &ResourceItemNull,
        sizeof (EFI_ISA_ACPI_RESOURCE)
        );
      break;

    case EISA_PNP_ID (0x303):
    case EISA_PNP_ID (0x501):
    case EISA_PNP_ID (0x510):
    case EISA_PNP_ID (0xF03):
    case EISA_PNP_ID (0xB02F):
      ResourceItem = AllocateZeroPool (sizeof (ISA_ACPI_IO_IRQ_RESOURCE));
      if (ResourceItem == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto ErrorExit;
      }

      InitCommonResource (DeviceInfo->Hid, DeviceResource, ResourceItem);
      CopyMem (
        &((ISA_ACPI_IO_IRQ_RESOURCE*) ResourceItem)->IsaNull,
        &ResourceItemNull,
        sizeof (EFI_ISA_ACPI_RESOURCE)
        );
      break;
    }   // End of switch (DeviceInfo->Hid)

    mIsaDevices[Index].ResourceList.Device.HID    = DeviceInfo->Hid;
    mIsaDevices[Index].ResourceList.Device.UID    = DeviceInfo->Uid;
    mIsaDevices[Index].ResourceList.ResourceItem  = ResourceItem;

    FreePool (DeviceInfo);
    DeviceInfo = NULL;
    FreePool (DeviceResource);
  }

  return EFI_SUCCESS;

ErrorExit:
  DEBUG ((EFI_D_ERROR, "InitIsaDeviceResources() - Fail to init: %r\n", Status));
  FreePool (DeviceInfo);
  FreePool (DeviceResource);
  return Status;
}

/**
  Check if any UID of each device type is conflict or not.

  This function must be called only after the device list is ordered.

  @return TRUE    Conflict
  @return FALSE   No conflict.

**/
BOOLEAN
IsUidConflict (
  VOID
  )
{
  UINTN     Index;
  UINT32    CurHid;
  UINT32    PrevHid;
  BOOLEAN   IsOccupied[MAX_AVAILABLE_UID + 1];
  UINT32    Uid;

  ZeroMem (IsOccupied,  sizeof (IsOccupied));

  PrevHid = 0;
  for (Index = 0; Index < mDeviceCount; Index++) {
    CurHid = mIsaDevices[Index].ResourceList.Device.HID;
    if (CurHid != PrevHid) {
      ZeroMem (IsOccupied,  sizeof (IsOccupied));
    }
    PrevHid = CurHid;

    Uid = mIsaDevices[Index].ResourceList.Device.UID;
    if (Uid <= MAX_AVAILABLE_UID) {
      if (!IsOccupied[Uid]) {
        IsOccupied[Uid] = TRUE;
      } else {
        DEBUG ((
          EFI_D_INFO,
          "IsUidConflict() - PNP%04x device UID conflict, please check PcdH2OSioXXXChipXIsaDeviceUid.\n",
          CurHid
          ));
        return TRUE;
      }
    }
  }

  return FALSE;
}

/**
  Configure UID automatically for the devices that doesn't provide a valid UID.

  This function must be called only after the device list is ordered and any UID
  any UID of each device type must be no conflict.

**/
VOID
AutoConfigUid (
  VOID
  )
{
  UINTN                     Index1;
  UINTN                     Index2;
  UINT32                    CurHid;
  UINT32                    PrevHid;
  UINT8                     Uid;
  H2O_ISA_DEVICE_PROTOCOL   *DeviceProtocol;
  H2O_ISA_DEVICE_INFO       *DeviceInfo;
  EFI_STATUS                Status;

  PrevHid = 0;
  Uid     = 0;

  for (Index1 = 0; Index1 < mDeviceCount; Index1++) {
    if (mIsaDevices[Index1].ResourceList.Device.UID <= MAX_AVAILABLE_UID) {
      continue;
    }

    CurHid = mIsaDevices[Index1].ResourceList.Device.HID;
    if (CurHid != PrevHid) {
      Uid = 0;
    }
    PrevHid = CurHid;

    for (Index2 = 0; Index2 < mDeviceCount; Index2++) {
      //
      // Check devices with the same HID
      //
      if (mIsaDevices[Index2].ResourceList.Device.HID != CurHid) {
        continue;
      }
      if (Uid == mIsaDevices[Index2].ResourceList.Device.UID) {
        Uid++;
      }
    }
    mIsaDevices[Index1].ResourceList.Device.UID = Uid++;

    DeviceProtocol = mIsaDevices[Index1].DeviceProtocol;
    if (DeviceProtocol == NULL) {
      if (CurHid != EISA_PNP_ID (0x200)) {
        DEBUG ((EFI_D_INFO, "AutoConfigUid() - This device doesn't provide an protocol interface.\n"));
      }
      continue;
    }

    Status = DeviceProtocol->GetDeviceInfo (DeviceProtocol, &DeviceInfo);

    if (!EFI_ERROR (Status)) {
      DeviceInfo->Uid = mIsaDevices[Index1].ResourceList.Device.UID;
      DeviceProtocol->SetUid (DeviceProtocol, DeviceInfo);
    }
  }
}

/**
  Function to compare 2 device by its HID, config. port, chip instance and device instance.

  @param[in] Buffer1  Pointer to device to compare (H2O_ISA_DEVICE*).
  @param[in] Buffer2  Pointer to second device to compare (H2O_ISA_DEVICE*).

  @retval 0     Both devices have the same weight.
  @return < 0   Buffer1 has a lower weight than Buffer2.
  @return > 0   Buffer1 has a higher weight than Buffer2.

**/
INTN
EFIAPI
DeviceCompare (
  IN  CONST VOID  *Buffer1,
  IN  CONST VOID  *Buffer2
  )
{
  H2O_ISA_DEVICE        *Device1;
  H2O_ISA_DEVICE        *Device2;
  H2O_ISA_DEVICE_INFO   *DeviceInfo1;
  H2O_ISA_DEVICE_INFO   *DeviceInfo2;
  UINT64                OrderKey1;
  UINT64                OrderKey2;

  Device1   = ((H2O_ISA_DEVICE*) Buffer1);
  Device2   = ((H2O_ISA_DEVICE*) Buffer2);

  Device1->DeviceProtocol->GetDeviceInfo (Device1->DeviceProtocol, &DeviceInfo1);
  Device2->DeviceProtocol->GetDeviceInfo (Device2->DeviceProtocol, &DeviceInfo2);

  OrderKey1 = DEVICE_INFO_TO_ORDER_KEY (
                DeviceInfo1->Hid,
                DeviceInfo1->ConfigPort,
                DeviceInfo1->ChipInstance,
                DeviceInfo1->DeviceInstance
                );
  OrderKey2 = DEVICE_INFO_TO_ORDER_KEY (
                DeviceInfo2->Hid,
                DeviceInfo2->ConfigPort,
                DeviceInfo2->ChipInstance,
                DeviceInfo2->DeviceInstance
                );

  if (OrderKey1 == OrderKey2) {
    return 0;
  } else {
    if (OrderKey1 < OrderKey2) {
      return -1;
    }
    return 1;
  }
}

/**
  Function to compare 2 device by its HID and UID.

  @param[in] Buffer1  Pointer to device to compare (H2O_ISA_DEVICE*).
  @param[in] Buffer2  Pointer to second device to compare (H2O_ISA_DEVICE*).

  @retval 0     Both devices have the same weight.
  @return < 0   Buffer1 has a lower weight than Buffer2.
  @return > 0   Buffer1 has a higher weight than Buffer2.

**/
INTN
EFIAPI
DeviceIdCompare (
  IN  CONST VOID  *Buffer1,
  IN  CONST VOID  *Buffer2
  )
{
  H2O_ISA_DEVICE  *Device1;
  H2O_ISA_DEVICE  *Device2;
  UINT64          OrderKey1;
  UINT64          OrderKey2;

  Device1   = ((H2O_ISA_DEVICE*) Buffer1);
  Device2   = ((H2O_ISA_DEVICE*) Buffer2);

  OrderKey1 = DEVICE_ID_TO_ORDER_KEY (
                Device1->ResourceList.Device.HID,
                Device1->ResourceList.Device.UID
                );
  OrderKey2 = DEVICE_ID_TO_ORDER_KEY (
                Device2->ResourceList.Device.HID,
                Device2->ResourceList.Device.UID
                );

  if (OrderKey1 == OrderKey2) {
    return 0;
  } else {
    if (OrderKey1 < OrderKey2) {
      return -1;
    }
    return 1;
  }
}

/**
  Get device's index in device list by its HID and UID.

  @param[in]  DeviceId      The pointer to a specific HID and UID composition.
  @param[out] DeviceIndex   Device's index in device list.

  @retval EFI_SUCCESS     Device was found.
  @retval EFI_NOT_FOUND   Device was not found.

**/
EFI_STATUS
GetDeviceIndexById (
  IN  EFI_ISA_ACPI_DEVICE_ID  *DeviceId,
  OUT UINTN                   *DeviceIndex
  )
{
  UINTN   Index;

  for (Index = 0; Index < mDeviceCount; Index++) {
    if (mIsaDevices[Index].ResourceList.Device.HID == DeviceId->HID &&
        mIsaDevices[Index].ResourceList.Device.UID == DeviceId->UID
        )
    {
      (*DeviceIndex) = Index;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**
  Enumerates the ISA devices on LPC interface.

  This service allows all the ISA devices on LPC interface to be enumerated.  If
  Device is a pointer to a NULL value, then the first ISA device on LPC interface
  is returned in Device and EFI_SUCCESS is returned.  If Device is a pointer
  to a value that was returned on a prior call to DeviceEnumerate(), then the next
  ISA device on LPC interface is returned in Device and EFI_SUCCESS is returned.
  If Device is a pointer to the last ISA device on the ISA bus, then
  EFI_NOT_FOUND is returned.

  @param[in]  This     The pointer to the EFI_ISA_ACPI_PROTOCOL instance.
  @param[out] Device   The pointer to an ISA device named by ACPI HID/UID.

  @retval EFI_SUCCESS    The next ISA controller on the ISA bus was returned.
  @retval EFI_NOT_FOUND  No device found.

**/
EFI_STATUS
EFIAPI
IsaDeviceEnumerate (
  IN  EFI_ISA_ACPI_PROTOCOL   *This,
  OUT EFI_ISA_ACPI_DEVICE_ID  **DeviceId
  )
{
  EFI_STATUS  Status;
  UINTN       DeviceIndex;

  if (This == NULL || DeviceId == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((*DeviceId) == NULL) {
    //
    // return first device
    //
    (*DeviceId) = &mIsaDevices[0].ResourceList.Device;
  } else {
    Status = GetDeviceIndexById ((*DeviceId), &DeviceIndex);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (DeviceIndex == mDeviceCount - 1) {
      return EFI_NOT_FOUND;
    }

    (*DeviceId) = &mIsaDevices[DeviceIndex + 1].ResourceList.Device;
  }

  return EFI_SUCCESS;
}

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
  IN  EFI_ISA_ACPI_DEVICE_ID      *DeviceId,
  OUT EFI_ISA_ACPI_RESOURCE_LIST  **ResourceList
  )
{
  EFI_STATUS  Status;
  UINTN       DeviceIndex;

  if (This == NULL || DeviceId == NULL || ResourceList == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetDeviceIndexById (DeviceId, &DeviceIndex);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  (*ResourceList) = &mIsaDevices[DeviceIndex].ResourceList;

  return EFI_SUCCESS;
}

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
  IN  EFI_ISA_ACPI_DEVICE_ID  *DeviceId
  )
{
  if ((DeviceId->HID == EISA_PNP_ID (0x401) || DeviceId->HID == EISA_PNP_ID (0x604)) &&
      !mIsDmaControllerInitialized
      )
  {
    InitDmaController ();
    mIsDmaControllerInitialized = TRUE;

    return EFI_SUCCESS;
  }

  return EFI_UNSUPPORTED;
}

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
  IN  EFI_ISA_ACPI_DEVICE_ID      *DeviceId,
  IN  EFI_ISA_ACPI_RESOURCE_LIST  *ResourceList
  )
{
  H2O_ISA_DEVICE_RESOURCE   DeviceResource;
  EFI_STATUS                Status;
  UINTN                     DeviceIndex;
  H2O_ISA_DEVICE_PROTOCOL   *DeviceProtocol;

  if (This == NULL || DeviceId == NULL || ResourceList == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (ResourceList->ResourceItem == NULL) {
    return EFI_UNSUPPORTED;
  }

  DeviceResource.IoPort      = (UINT16) ResourceList->ResourceItem[0].StartRange;
  DeviceResource.IrqNumber   = (UINT8) ResourceList->ResourceItem[1].StartRange;
  DeviceResource.DmaChannel  = (UINT8) ResourceList->ResourceItem[2].StartRange;

  Status = GetDeviceIndexById (DeviceId, &DeviceIndex);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DeviceProtocol = mIsaDevices[DeviceIndex].DeviceProtocol;
  if (DeviceProtocol != NULL) {
    return DeviceProtocol->SetResource (DeviceProtocol, &DeviceResource);
  }

  return EFI_SUCCESS;
}

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
  IN  EFI_ISA_ACPI_DEVICE_ID  *DeviceId,
  IN  BOOLEAN                 On
  )
{
  EFI_STATUS                Status;
  UINTN                     DeviceIndex;
  H2O_ISA_DEVICE_PROTOCOL   *DeviceProtocol;

  if (This == NULL || DeviceId == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetDeviceIndexById (DeviceId, &DeviceIndex);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DeviceProtocol = mIsaDevices[DeviceIndex].DeviceProtocol;
  if (DeviceProtocol != NULL) {
    return DeviceProtocol->SetPower (DeviceProtocol, On);
  }

  return EFI_SUCCESS;
}

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
  IN  EFI_ISA_ACPI_DEVICE_ID  *DeviceId,
  IN  BOOLEAN                 Enable
  )
{
  EFI_STATUS                Status;
  UINTN                     DeviceIndex;
  H2O_ISA_DEVICE_PROTOCOL   *DeviceProtocol;

  if (This == NULL || DeviceId == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetDeviceIndexById (DeviceId, &DeviceIndex);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DeviceProtocol = mIsaDevices[DeviceIndex].DeviceProtocol;
  if (DeviceProtocol != NULL) {
    return DeviceProtocol->EnableDevice (DeviceProtocol, Enable);
  }

  return EFI_SUCCESS;
}

/**
  Retrieves the set of possible resources that may be assigned to an ISA device
  with SetResource().

  Retrieves the possible sets of I/O, MMIO, DMA, and interrupt resources for the
  ISA device specified by Device.  The sets are returned in ResourceList.

  TODO: If IsaBusDxe needs the possible resource in the future, this function must
        be implemented to support that.

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
  )
{
  //
  // Not supported yet
  //
  return EFI_UNSUPPORTED;
}

/**
  Initializes all the H/W states required for the ISA devices on LPC interface
  to be enumerated and managed by the rest of the services in this protocol.
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
  )
{
  EFI_STATUS      Status;
  UINTN           TempDeviceCount;
  H2O_ISA_DEVICE  *TempIsaDevices;
  UINTN           Index;

  //
  // Following initialization flow must be executed once only,
  // because this function may be called several times during POST.
  //
  if (This == NULL) {
    PublishOldPolicyIsaDevices ();

    Status = InitIsaDevices ();
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = InitIsaDeviceResources ();
    if (EFI_ERROR (Status)) {
      return Status;
    }

    PerformQuickSort (mIsaDevices, mDeviceCount, sizeof (H2O_ISA_DEVICE), DeviceCompare);

    if (IsUidConflict ()) {
      FreePool (mIsaDevices);
      mIsaDevices   = NULL;
      mDeviceCount  = 0;

      RevokeOldPolicyIsaDevices ();

      ASSERT (FALSE);
      return EFI_ABORTED;
    }

    if (mIsDmaCompatibleDevicePresented) {
      //
      // add DMA controller to the tail of device list
      //
      TempDeviceCount = mDeviceCount + 1;
      TempIsaDevices  = AllocateZeroPool (TempDeviceCount * sizeof (H2O_ISA_DEVICE));
      if (TempIsaDevices != NULL) {
        CopyMem (TempIsaDevices, mIsaDevices, mDeviceCount * sizeof (H2O_ISA_DEVICE));
        for (Index = 0; Index < TempDeviceCount; Index++) {
          if (TempIsaDevices[Index].DeviceProtocol == NULL) {
            TempIsaDevices[Index].ResourceList.Device.HID   = EISA_PNP_ID (0x200);
            TempIsaDevices[Index].ResourceList.Device.UID   = 0xFFFF;     // for auto-config
          }
        }

        FreePool (mIsaDevices);
        mIsaDevices  = TempIsaDevices;
        mDeviceCount = TempDeviceCount;
      }
    }

    AutoConfigUid ();
    PerformQuickSort (mIsaDevices, mDeviceCount, sizeof (H2O_ISA_DEVICE), DeviceIdCompare);
  }

  return EFI_SUCCESS;
}

/**
  This function provides an interface to free allocated programming resources in
  ISA Controller driver, cause it may not be a driver-model driver.

  This function will free mIsaDevices and reset mDeviceCount to zero.

  @param[in] This  The pointer to the EFI_ISA_ACPI_PROTOCOL instance.

  @retval EFI_SUCCESS   Successfully free allocated programming resources.
  @retval Other         Something error happened.

**/
EFI_STATUS
EFIAPI
IsaAcpiDriverStopCallback (
  IN  EFI_ISA_ACPI_PROTOCOL   *This
  )
{
  DEBUG ((EFI_D_INFO, "IsaAcpiDriverStopCallback() - Start\n"));
  
  if (mIsaDevices != NULL) {
    FreePool (mIsaDevices);
    mIsaDevices   = NULL;
    mDeviceCount  = 0;
  }

  RevokeOldPolicyIsaDevices ();
  
  DEBUG ((EFI_D_INFO, "IsaAcpiDriverStopCallback() - End\n"));

  return EFI_SUCCESS;
}
