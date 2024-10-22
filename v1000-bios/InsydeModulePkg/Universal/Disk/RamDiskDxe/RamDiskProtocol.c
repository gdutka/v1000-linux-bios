/** @file
  The realization of EFI_RAM_DISK_PROTOCOL.

  Copyright (c) 2016 - 2019, Intel Corporation. All rights reserved.<BR>
  (C) Copyright 2016 Hewlett Packard Enterprise Development LP<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "RamDiskImpl.h"

RAM_DISK_PRIVATE_DATA mRamDiskPrivateDataTemplate = {
  RAM_DISK_PRIVATE_DATA_SIGNATURE,
  NULL
};

MEDIA_RAM_DISK_DEVICE_PATH  mRamDiskDeviceNodeTemplate = {
  {
    MEDIA_DEVICE_PATH,
    MEDIA_RAM_DISK_DP,
    {
      (UINT8) (sizeof (MEDIA_RAM_DISK_DEVICE_PATH)),
      (UINT8) ((sizeof (MEDIA_RAM_DISK_DEVICE_PATH)) >> 8)
    }
  }
};

extern EFI_RAM_DISK_REGISTER_RAMDISK    mRegisterRamDisk;
extern EFI_RAM_DISK_UNREGISTER_RAMDISK  mUnregisterRamDisk;
extern LIST_ENTRY  mRegisteredRamDisks;

BOOLEAN  mRamDiskSsdtTableKeyValid = FALSE;
UINTN    mRamDiskSsdtTableKey;


/**
  Initialize the RAM disk device node.

  @param[in]      PrivateData     Points to RAM disk private data.
  @param[in, out] RamDiskDevNode  Points to the RAM disk device node.

**/
VOID
RamDiskInitDeviceNode (
  IN     RAM_DISK_PRIVATE_DATA         *PrivateData,
  IN OUT MEDIA_RAM_DISK_DEVICE_PATH    *RamDiskDevNode
  )
{
  WriteUnaligned64 (
    (UINT64 *) &(RamDiskDevNode->StartingAddr[0]),
    (UINT64) PrivateData->StartingAddr
    );
  WriteUnaligned64 (
    (UINT64 *) &(RamDiskDevNode->EndingAddr[0]),
    (UINT64) PrivateData->StartingAddr + PrivateData->Size - 1
    );
  CopyGuid (&RamDiskDevNode->TypeGuid, &PrivateData->TypeGuid);
  RamDiskDevNode->Instance = PrivateData->InstanceNumber;
}


/**
  Register a RAM disk with specified address, size and type.

  @param[in]  RamDiskBase    The base address of registered RAM disk.
  @param[in]  RamDiskSize    The size of registered RAM disk.
  @param[in]  RamDiskType    The type of registered RAM disk. The GUID can be
                             any of the values defined in section 9.3.6.9, or a
                             vendor defined GUID.
  @param[in]  ParentDevicePath
                             Pointer to the parent device path. If there is no
                             parent device path then ParentDevicePath is NULL.
  @param[out] DevicePath     On return, points to a pointer to the device path
                             of the RAM disk device.
                             If ParentDevicePath is not NULL, the returned
                             DevicePath is created by appending a RAM disk node
                             to the parent device path. If ParentDevicePath is
                             NULL, the returned DevicePath is a RAM disk device
                             path without appending. This function is
                             responsible for allocating the buffer DevicePath
                             with the boot service AllocatePool().

  @retval EFI_SUCCESS             The RAM disk is registered successfully.
  @retval EFI_INVALID_PARAMETER   DevicePath or RamDiskType is NULL.
                                  RamDiskSize is 0.
  @retval EFI_ALREADY_STARTED     A Device Path Protocol instance to be created
                                  is already present in the handle database.
  @retval EFI_OUT_OF_RESOURCES    The RAM disk register operation fails due to
                                  resource limitation.

**/
EFI_STATUS
RamDiskRegiterHook (
  IN UINT64                       RamDiskBase,
  IN UINT64                       RamDiskSize,
  IN EFI_GUID                     *RamDiskType,
  IN EFI_DEVICE_PATH              *ParentDevicePath     OPTIONAL,
  OUT EFI_DEVICE_PATH_PROTOCOL    **DevicePath
  )
{
  EFI_STATUS                       Status;
  RAM_DISK_PRIVATE_DATA           *PrivateData;
  MEDIA_RAM_DISK_DEVICE_PATH      *RamDiskDevNode;

  Status = mRegisterRamDisk(
             RamDiskBase,
             RamDiskSize,
             RamDiskType,
             ParentDevicePath,
             DevicePath
             );

  if (EFI_ERROR(Status)){
    return Status;
  }

  //
  // Create a new RAM disk instance and initialize its private data
  //
  PrivateData = AllocateCopyPool (
                  sizeof (RAM_DISK_PRIVATE_DATA),
                  &mRamDiskPrivateDataTemplate
                  );
  if (NULL == PrivateData) {
    return EFI_OUT_OF_RESOURCES;
  }

  PrivateData->StartingAddr = RamDiskBase;
  PrivateData->Size         = RamDiskSize;
  CopyGuid (&PrivateData->TypeGuid, RamDiskType);
  InitializeListHead (&PrivateData->ThisInstance);

  //
  // Generate device path information for the registered RAM disk
  //
  RamDiskDevNode = AllocateCopyPool (
                     sizeof (MEDIA_RAM_DISK_DEVICE_PATH),
                     &mRamDiskDeviceNodeTemplate
                     );
  if (NULL == RamDiskDevNode) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  RamDiskInitDeviceNode (PrivateData, RamDiskDevNode);

  *DevicePath = AppendDevicePathNode (
                  ParentDevicePath,
                  (EFI_DEVICE_PATH_PROTOCOL *) RamDiskDevNode
                  );
  if (NULL == *DevicePath) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  PrivateData->DevicePath = *DevicePath;

  //
  // Insert the newly created one to the registered RAM disk list
  //
  InsertTailList (&mRegisteredRamDisks, &PrivateData->ThisInstance);

  FreePool (RamDiskDevNode);

  return Status;

ErrorExit:
  if (RamDiskDevNode != NULL) {
    FreePool (RamDiskDevNode);
  }

  if (PrivateData != NULL) {
    if (PrivateData->DevicePath) {
      FreePool (PrivateData->DevicePath);
    }

    FreePool (PrivateData);
  }

  return Status;

}


/**
  Unregister a RAM disk specified by DevicePath.

  @param[in] DevicePath      A pointer to the device path that describes a RAM
                             Disk device.

  @retval EFI_SUCCESS             The RAM disk is unregistered successfully.
  @retval EFI_INVALID_PARAMETER   DevicePath is NULL.
  @retval EFI_UNSUPPORTED         The device specified by DevicePath is not a
                                  valid ramdisk device path and not supported
                                  by the driver.
  @retval EFI_NOT_FOUND           The RAM disk pointed by DevicePath doesn't
                                  exist.

**/
EFI_STATUS
RamDiskUnregiterHook (
  IN  EFI_DEVICE_PATH_PROTOCOL    *DevicePath
  )
{

  LIST_ENTRY                      *Entry;
  LIST_ENTRY                      *NextEntry;
  UINT64                          StartingAddr;
  UINT64                          EndingAddr;
  EFI_DEVICE_PATH_PROTOCOL        *Header;
  MEDIA_RAM_DISK_DEVICE_PATH      *RamDiskDevNode;
  RAM_DISK_PRIVATE_DATA           *PrivateData;
  EFI_STATUS                      Status;

  Status = mUnregisterRamDisk(DevicePath);
  if (EFI_ERROR(Status)){
    return Status;
  }

  if (NULL == DevicePath) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Locate the RAM disk device node.
  //
  RamDiskDevNode = NULL;
  Header         = DevicePath;
  do {
    //
    // Test if the current device node is a RAM disk.
    //
    if ((MEDIA_DEVICE_PATH == Header->Type) &&
      (MEDIA_RAM_DISK_DP == Header->SubType)) {
      RamDiskDevNode = (MEDIA_RAM_DISK_DEVICE_PATH *) Header;

      break;
    }

    Header = NextDevicePathNode (Header);
  } while ((Header->Type != END_DEVICE_PATH_TYPE));

  if (NULL == RamDiskDevNode) {
    return EFI_UNSUPPORTED;
  }

  StartingAddr   = ReadUnaligned64 ((UINT64 *) &(RamDiskDevNode->StartingAddr[0]));
  EndingAddr     = ReadUnaligned64 ((UINT64 *) &(RamDiskDevNode->EndingAddr[0]));


  if (!IsListEmpty(&mRegisteredRamDisks)) {
    EFI_LIST_FOR_EACH_SAFE (Entry, NextEntry, &mRegisteredRamDisks) {
      PrivateData = RAM_DISK_PRIVATE_FROM_THIS (Entry);

      //
      // Unregister the RAM disk given by its starting address, ending address
      // and type guid.
      //
      if ((StartingAddr == PrivateData->StartingAddr) &&
          (EndingAddr == PrivateData->StartingAddr + PrivateData->Size - 1) &&
          (CompareGuid (&RamDiskDevNode->TypeGuid, &PrivateData->TypeGuid))) {

        RemoveEntryList (&PrivateData->ThisInstance);

        break;
      }
    }
   }
  return Status;
}
