/** @file
  This code supports Iommu remap for AMD platform

;******************************************************************************
;* Copyright (c) 2022, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include <RemapIommuPei.h>

EFI_GUID              mIommuRemapNvmeGuid = {
  0xbce91a08, 0xbce6, 0x40e3, 0x89, 0xe0, 0x54, 0x7f, 0xd5, 0x21, 0x91, 0xae
};

EFI_GUID              mIommuRemapSmmCommBufNvmeGuid = {
  0xeed62def, 0xbb87, 0x43d9, 0xa8, 0x46, 0xac, 0x76, 0xb5, 0xd2, 0x3d, 0x90
};

EFI_GUID              mIommuRemapNvmeResourceNodeListGuid = {
  0x885b868b, 0x14db, 0x4362, 0xb2, 0x66, 0xa3, 0x5c, 0x2a, 0xd6, 0xcc, 0x16
};

EFI_GUID              mIommuRemapAhciGuid = {
  0xe7c41abf, 0x1e0c, 0x4aa1, 0xba, 0x2d, 0x27, 0x4d, 0xe, 0xb1, 0xe2, 0x58
};

EFI_GUID              mIommuRemapSmmCommBufAhciGuid = {
  0x42a5b701, 0xbc45, 0x496c, 0xbe, 0x1f, 0xdc, 0xfc, 0x3d, 0xb1, 0x06, 0x91
};

EFI_GUID              mIommuResourceAhciGuid = {
  0x27ef0f52, 0x0b33, 0x408a, 0x81, 0x9c, 0x70, 0xee, 0xc8, 0xfa, 0x8d, 0x85
};

/**
  Entry point of the notification callback function itself within the PEIM.
  It is to Re-map DMA address for AMD platform that support Iommu.

  @param  PeiServices      Indirect reference to the PEI Services Table.
  @param  NotifyDescriptor Address of the notification descriptor data structure.
  @param  Ppi              Address of the PPI that was installed.

  @return Status of the notification.
          The status code returned from this function is ignored.
**/
EFI_STATUS
EFIAPI
RemapIommuNvmeEndOfPeiNotify(
  IN EFI_PEI_SERVICES          **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN VOID                      *Ppi
  )
{
  EFI_STATUS                        Status;
  EFI_BOOT_MODE                     BootMode;
  UINTN                             IommuRemapVariableSize;
  IOMMU_RESOURCE_REMAP_NVME         *ResourceNode;
  EFI_PHYSICAL_ADDRESS              DeviceAddress;
  VOID                              *Mapping;
  UINT32                            Index;
  UINTN                             Size;
  UINT32                            Bytes;

  EFI_PEI_SMM_COMMUNICATION_PPI      *SmmCommunicationPpi;
  EFI_SMM_COMMUNICATE_HEADER         *CommHeader;
  NVME_SMM_PRE_MEM_INFO              *SmmCommBufferNvmeInfo;
  VOID                               *CommBufPtr;
  UINT8                              CommBuffer[sizeof(EFI_GUID) + sizeof(UINT64) + sizeof(EFI_PHYSICAL_ADDRESS)];
  UINTN                              CommSize;
  EFI_PHYSICAL_ADDRESS               ResourceNodeListAddress;
  UINT64                             MessageLength2;
  EFI_PHYSICAL_ADDRESS               *AddressBuffer;
  UINT8                              DummyData;

  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);
  if (BootMode != BOOT_ON_S3_RESUME) {
    return EFI_UNSUPPORTED;
  }

  IoMmuInit();
  if (mIoMmu == NULL) {
    return EFI_UNSUPPORTED;
  }

  Size = 0;
  Status = RestoreLockBox(&mIommuRemapNvmeResourceNodeListGuid, (VOID*) &ResourceNodeListAddress, &Size);
  if ((Status != EFI_BUFFER_TOO_SMALL) || (Size != sizeof(EFI_PHYSICAL_ADDRESS))) {
    return EFI_NOT_FOUND;
  } else {
    Status = RestoreLockBox(&mIommuRemapNvmeResourceNodeListGuid, (VOID*) &ResourceNodeListAddress, &Size);
    if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
    }
    ResourceNode = (IOMMU_RESOURCE_REMAP_NVME *) ((UINTN) ResourceNodeListAddress);
  }

  IommuRemapVariableSize = 0;
  Status = RestoreLockBox(&mIommuRemapNvmeGuid, &DummyData, &IommuRemapVariableSize);
  if (Status != EFI_BUFFER_TOO_SMALL) {
    return EFI_NOT_FOUND;
  } else {
    Status = RestoreLockBox(&mIommuRemapNvmeGuid, (VOID*) ResourceNode, &IommuRemapVariableSize);
    if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
    }
  }

  CommBufPtr = NULL;
  SmmCommBufferNvmeInfo = NULL;
  Size = 0;
  Status = RestoreLockBox(&mIommuRemapSmmCommBufNvmeGuid, &DummyData, &Size);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    SmmCommBufferNvmeInfo = AllocatePool (Size);
    if (SmmCommBufferNvmeInfo != NULL) {
      Status = RestoreLockBox(&mIommuRemapSmmCommBufNvmeGuid, (VOID*) SmmCommBufferNvmeInfo, &Size);
      if (!EFI_ERROR (Status)) {
        CommBufPtr = (VOID *) (UINTN) SmmCommBufferNvmeInfo->Start;
      }
    }
  }

  Size = IommuRemapVariableSize / sizeof(IOMMU_RESOURCE_REMAP_NVME);
  for (Index = 0; Index < Size; Index++) {
    Bytes = ResourceNode[Index].Bytes;
    Status = IoMmuMap (
               EdkiiIoMmuOperationBusMasterCommonBuffer,
               (VOID*)(UINTN)ResourceNode[Index].HostAddr,
                &Bytes,
                &DeviceAddress,
                &Mapping);
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }

    if (Bytes < ResourceNode[Index].Bytes) {
      return EFI_OUT_OF_RESOURCES;
    }

    ResourceNode[Index].DeviceAddr = DeviceAddress;
  }
  ResourceNode[Index].DeviceAddr = DEVICE_ADDR_OF_END_OF_RESOURCE_NOTE;
  ResourceNode[Index].Bytes = BYTES_OF_END_OF_RESOURCE_NOTE;
  ResourceNode[Index].HostAddr = HOST_ADDR_OF_END_OF_RESOURCE_NOTE;

  Status = PeiServicesLocatePpi (
             &gEfiPeiSmmCommunicationPpiGuid,
             0,
             NULL,
             (VOID **)&SmmCommunicationPpi
             );

  CommHeader = (EFI_SMM_COMMUNICATE_HEADER *)&CommBuffer[0];
  CopyMem (&CommHeader->HeaderGuid, &mIommuRemapNvmeGuid, sizeof(gEfiSmmLockBoxCommunicationGuid));
  MessageLength2 = sizeof(EFI_PHYSICAL_ADDRESS);
  CopyMem (&CommBuffer[OFFSET_OF (EFI_SMM_COMMUNICATE_HEADER, MessageLength)], &MessageLength2, sizeof(MessageLength2));
  AddressBuffer = (EFI_PHYSICAL_ADDRESS*) &CommBuffer[OFFSET_OF (EFI_SMM_COMMUNICATE_HEADER, MessageLength) + sizeof(UINT64)];

  //
  // Put the address of first resource node in the buffer
  //
  *AddressBuffer = (EFI_PHYSICAL_ADDRESS) (UINTN) ResourceNode;
  CommSize = sizeof(CommBuffer);
  if ((CommBufPtr != NULL) && (SmmCommBufferNvmeInfo->Size >= CommSize)) {
    CopyMem (CommBufPtr, CommBuffer, CommSize);
    Status = SmmCommunicationPpi->Communicate (
                                  SmmCommunicationPpi,
                                  CommBufPtr,
                                  &CommSize
                                  );
    ZeroMem (CommBufPtr, CommSize);
  } else {
    DEBUG ((DEBUG_ERROR, "Communication buffer error!\n"));
  }

  if (SmmCommBufferNvmeInfo != NULL) {
    FreePool (SmmCommBufferNvmeInfo);
  }

  return EFI_SUCCESS;
}

/**
  Entry point of the notification callback function itself within the PEIM.
  It is to Re-map DMA address for AMD platform that support Iommu.

  @param  PeiServices      Indirect reference to the PEI Services Table.
  @param  NotifyDescriptor Address of the notification descriptor data structure.
  @param  Ppi              Address of the PPI that was installed.

  @return Status of the notification.
          The status code returned from this function is ignored.
**/
EFI_STATUS
EFIAPI
RemapIommuAhciEndOfPeiNotify(
  IN EFI_PEI_SERVICES          **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN VOID                      *Ppi
  )
{
  EFI_STATUS                        Status;
  EFI_BOOT_MODE                     BootMode;
  UINTN                             IommuRemapVariableSize;
  IOMMU_RESOURCE_REMAP              *ResourceNode;
  IOMMU_RESOURCE_REMAP              *ResourceNodeBuffer;
  EFI_PHYSICAL_ADDRESS              DeviceAddress;
  VOID                              *Mapping;
  UINT32                            Index;
  UINTN                             Size;
  UINT32                            Bytes;

  EFI_PEI_SMM_COMMUNICATION_PPI      *SmmCommunicationPpi;
  EFI_SMM_COMMUNICATE_HEADER         *CommHeader;
  VOID                               *IommuBuffer;
  EFI_PHYSICAL_ADDRESS               BufferAddr;
  UINT8                              CommBuffer[sizeof(EFI_GUID) + sizeof(UINT64) + sizeof(EFI_PHYSICAL_ADDRESS)];
  UINTN                              CommSize;
  UINT64                             MessageLength2;  
  UINT32                             *Buffer;
  UINT8                              DummyData;    
  
  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);
  if (BootMode != BOOT_ON_S3_RESUME) {
    return EFI_UNSUPPORTED;
  }

  IoMmuInit();
  if (mIoMmu == NULL) {
    return EFI_UNSUPPORTED;
  }
  
  IommuRemapVariableSize = sizeof(ResourceNode);
  Status = RestoreLockBox(&mIommuRemapAhciGuid, &DummyData, &IommuRemapVariableSize);
  if (Status != EFI_BUFFER_TOO_SMALL) {
    return EFI_NOT_FOUND;
  } else {
    ResourceNodeBuffer = AllocatePool (IommuRemapVariableSize);
    if (ResourceNodeBuffer == NULL) {
      return EFI_NOT_FOUND;
    }

    Status = RestoreLockBox(&mIommuRemapAhciGuid, (VOID*)ResourceNodeBuffer, &IommuRemapVariableSize);
    if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
    }
  }

  Size = 0;
  ResourceNode = NULL;
  BufferAddr = 0;
  Status = RestoreLockBox (&mIommuResourceAhciGuid, &DummyData, &Size);
  if (Status == EFI_BUFFER_TOO_SMALL && Size == sizeof (EFI_PHYSICAL_ADDRESS)) {
    Status = RestoreLockBox (&mIommuResourceAhciGuid, &BufferAddr, &Size);
    if (!EFI_ERROR (Status)) {
      ResourceNode = (VOID *)(UINTN)BufferAddr;
      CopyMem (ResourceNode, ResourceNodeBuffer, IommuRemapVariableSize);
      FreePool (ResourceNodeBuffer);
    }
  }

  if (ResourceNode == NULL) {
    return EFI_SUCCESS;
  }

  Size = 0;
  IommuBuffer = NULL;
  BufferAddr = 0;
  Status = RestoreLockBox (&mIommuRemapSmmCommBufAhciGuid, &DummyData, &Size);
  if (Status == EFI_BUFFER_TOO_SMALL && Size == sizeof (EFI_PHYSICAL_ADDRESS)) {
    Status = RestoreLockBox (&mIommuRemapSmmCommBufAhciGuid, &BufferAddr, &Size);
    if (!EFI_ERROR (Status)) {
      IommuBuffer = (VOID *)(UINTN)BufferAddr;
    }
  }

  if (IommuBuffer == NULL) {
    return EFI_SUCCESS;
  }

  Size = IommuRemapVariableSize / sizeof(IOMMU_RESOURCE_REMAP);
  for (Index = 0; Index < Size; Index++) {
    Bytes = ResourceNode[Index].Bytes;
    Status = IoMmuMap (
                EdkiiIoMmuOperationBusMasterCommonBuffer, 
                (VOID*)(UINTN)ResourceNode[Index].HostAddr, 
                &Bytes, 
                &DeviceAddress, 
                &Mapping
                );
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }

    if (Bytes < ResourceNode[Index].Bytes) {
      return EFI_OUT_OF_RESOURCES;
    }
    
    ResourceNode[Index].DeviceAddr = DeviceAddress;
  }

  Status = PeiServicesLocatePpi (
              &gEfiPeiSmmCommunicationPpiGuid,
              0,
              NULL,
              (VOID **)&SmmCommunicationPpi
              );

  CommHeader = (EFI_SMM_COMMUNICATE_HEADER *)&CommBuffer[0];
  CopyMem (&CommHeader->HeaderGuid, &mIommuRemapAhciGuid, sizeof(gEfiSmmLockBoxCommunicationGuid));
  MessageLength2 = sizeof (EFI_PHYSICAL_ADDRESS);    
  CopyMem (&CommBuffer[OFFSET_OF (EFI_SMM_COMMUNICATE_HEADER, MessageLength)], &MessageLength2, sizeof(MessageLength2));        
  Buffer = (UINT32*)&CommBuffer[OFFSET_OF (EFI_SMM_COMMUNICATE_HEADER, MessageLength) + sizeof(UINT64)];

  //
  // Put the address of first resource node in the buffer
  //
  Buffer[0] = (UINTN)ResourceNode;
  CommSize = sizeof(CommBuffer);

  CopyMem (IommuBuffer, CommBuffer, CommSize);

  Status = SmmCommunicationPpi->Communicate (
                                  SmmCommunicationPpi,
                                  IommuBuffer,
                                  &CommSize
                                  );
  FreePool (IommuBuffer);
  FreePool (ResourceNode);
  
  return EFI_SUCCESS;
}



EFI_PEI_NOTIFY_DESCRIPTOR mRemapIommuNvmeEndOfPeiNotifyDesc = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiPostScriptTablePpiGuid,
  RemapIommuNvmeEndOfPeiNotify
};

EFI_PEI_NOTIFY_DESCRIPTOR mRemapIommuAhciEndOfPeiNotifyDesc = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiPostScriptTablePpiGuid,
  RemapIommuAhciEndOfPeiNotify
};


/**
  Main entry for this module.

  @param FileHandle             Handle of the file being invoked.
  @param PeiServices            Pointer to PEI Services table.

  @return Status from PeiServicesNotifyPpi.

**/
EFI_STATUS
EFIAPI
RemapIommuInit (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                    Status;
  Status = PeiServicesRegisterForShadow (FileHandle); 
  if (!EFI_ERROR (Status)) {
    return Status;
  }

  if (!PcdGetBool(PcdH2OIommuMapping)) {
    return EFI_SUCCESS;
  }
  
  Status = PeiServicesNotifyPpi (&mRemapIommuNvmeEndOfPeiNotifyDesc);
  ASSERT_EFI_ERROR (Status);

  Status = PeiServicesNotifyPpi (&mRemapIommuAhciEndOfPeiNotifyDesc);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

