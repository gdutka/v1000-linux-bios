/** @file
  UEFI FMP capsule processor on firmware management protocol implementation 

;******************************************************************************
;* Copyright (c) 2012 - 2021, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "CapsuleProcessorDxe.h"

STATIC  CAPSULE_FMP_ENTRY       mCapsuleFmpEntry;

/**
  Populates the EFI_FIRMWARE_IMAGE_DESCRIPTOR structure in the private
  context structure.

  @param[in,out] Private  Pointer to the private context structure for the
                          Firmware Management Protocol instance.

**/
STATIC
VOID
PopulateDescriptor (
  IN OUT CAPSULE_FMP_ENTRY     *CapsuleFmpEntry
  )
{
  EFI_FIRMWARE_IMAGE_DESCRIPTOR *Descriptor;

  if (CapsuleFmpEntry->DescriptorPopulated) {
    return;
  }

  Descriptor = &CapsuleFmpEntry->Descriptor;

  CopyGuid (&Descriptor->ImageTypeId, &mCapsuleFmpEntry.ImageTypeGuid);
  Descriptor->ImageIndex                  = CapsuleFmpEntry->ImageIndex;
  Descriptor->ImageId                     = Descriptor->ImageIndex;
  Descriptor->ImageIdName                 = NULL;
  Descriptor->HardwareInstance            = 0;
  Descriptor->LowestSupportedImageVersion = 0;
  Descriptor->AttributesSupported         = (IMAGE_ATTRIBUTE_IMAGE_UPDATABLE | IMAGE_ATTRIBUTE_RESET_REQUIRED);
  Descriptor->AttributesSetting           = (IMAGE_ATTRIBUTE_IMAGE_UPDATABLE | IMAGE_ATTRIBUTE_RESET_REQUIRED);
  Descriptor->Compatibilities             = 0;
  Descriptor->Size                        = 0;
  Descriptor->Version                     = 0;
  Descriptor->LastAttemptVersion          = 0;
  Descriptor->LastAttemptStatus           = 0;

  //
  // Free the current version name.  Shouldn't really happen but this populate
  // function could be called multiple times (to refresh).
  //
  if (Descriptor->VersionName != NULL) {
    FreePool (Descriptor->VersionName);
    Descriptor->VersionName = NULL;
  }

  CapsuleFmpEntry->DescriptorPopulated = TRUE;
}

/**
  Checks if the firmware image is valid for the device.

  This function allows firmware update application to validate the firmware image without
  invoking the SetImage() first.

  @param[in]  This               A pointer to the EFI_FIRMWARE_MANAGEMENT_PROTOCOL instance.
  @param[in]  ImageIndex         A unique number identifying the firmware image(s) within the device.
                                 The number is between 1 and DescriptorCount.
  @param[in]  Image              Points to the new image.
  @param[in]  ImageSize          Size of the new image in bytes.
  @param[out] ImageUpdatable     Indicates if the new image is valid for update. It also provides,
                                 if available, additional information if the image is invalid.

  @retval EFI_SUCCESS            The image was successfully checked.
  @retval EFI_INVALID_PARAMETER  The Image was NULL.
  @retval EFI_UNSUPPORTED        The operation is not supported.
  @retval EFI_SECURITY_VIOLATION The operation could not be performed due to an authentication failure.

**/
STATIC
EFI_STATUS
CheckTheImage (
  IN     EFI_FIRMWARE_MANAGEMENT_PROTOCOL       *This,
  IN     UINT8                                  ImageIndex,
  IN     CONST VOID                             *Capsule,
  IN     UINTN                                  CapsuleSize,
  OUT    UINT32                                 *ImageUpdatable
  )
{
  EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER        *FmpCapsuleHeader;
  EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER  *ImageHeader;
  UINT8                                         *EndOfPayload;
  UINT64                                        *ItemOffsetList;
  UINT32                                        ItemNum;
  UINTN                                         Index;
  UINT32                                        CheckImageResult;
  UINT8                                         *Image;

  CheckImageResult = IMAGE_UPDATABLE_VALID;
  FmpCapsuleHeader = (EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER*)((UINT8*)Capsule + ((EFI_CAPSULE_HEADER*)Capsule)->HeaderSize);
  if (FmpCapsuleHeader->Version > EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER_INIT_VERSION) {
    CheckImageResult = IMAGE_UPDATABLE_INVALID_OLD;
    goto Exit;
  }
  ItemOffsetList = (UINT64 *)(FmpCapsuleHeader + 1);
  ItemNum = FmpCapsuleHeader->EmbeddedDriverCount + FmpCapsuleHeader->PayloadItemCount;
  if (ItemNum == FmpCapsuleHeader->EmbeddedDriverCount) {
    //
    // No payload element 
    //
    CheckImageResult = IMAGE_UPDATABLE_INVALID_TYPE;
    goto Exit;
  }
  if (FmpCapsuleHeader->PayloadItemCount != 0) {
    //
    // Check if the last payload is within capsule image range
    //
    ImageHeader  = (EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER *)((UINT8 *)FmpCapsuleHeader + ItemOffsetList[ItemNum - 1]);
    if (ImageHeader->Version >= EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER_INIT_VERSION) {
      Image = (UINT8 *)(ImageHeader + 1);
    } else {
      Image = (UINT8 *) ImageHeader + OFFSET_OF(EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER, UpdateHardwareInstance);
    }
    EndOfPayload = (UINT8*) (Image + ImageHeader->UpdateImageSize + ImageHeader->UpdateVendorCodeSize);
  } else {
    //
    // No driver & payload element in FMP
    //
    EndOfPayload = (UINT8*)(FmpCapsuleHeader + 1);
  }
  if (EndOfPayload != (UINT8*)Capsule + CapsuleSize) {
    CheckImageResult = IMAGE_UPDATABLE_INVALID_TYPE;
    goto Exit;
  }
  //
  // All the address in ItemOffsetList must be stored in ascending order
  //
  if (ItemNum >= 2) {
    for (Index = 0; Index < ItemNum - 1; Index++) {
      if (ItemOffsetList[Index] >= ItemOffsetList[Index + 1]) {
        CheckImageResult = IMAGE_UPDATABLE_INVALID_TYPE;
        goto Exit;
      }
    }
  }

Exit:
  if (ImageUpdatable) {
    *ImageUpdatable = CheckImageResult;
  }
  return EFI_SUCCESS;
}

/**
  Returns information about the current firmware image(s) of the device.

  This function allows a copy of the current firmware image to be created and saved.
  The saved copy could later been used, for example, in firmware image recovery or rollback.

  @param[in]      This               A pointer to the EFI_FIRMWARE_MANAGEMENT_PROTOCOL instance.
  @param[in, out] ImageInfoSize      A pointer to the size, in bytes, of the ImageInfo buffer.
                                     On input, this is the size of the buffer allocated by the caller.
                                     On output, it is the size of the buffer returned by the firmware
                                     if the buffer was large enough, or the size of the buffer needed
                                     to contain the image(s) information if the buffer was too small.
  @param[in, out] ImageInfo          A pointer to the buffer in which firmware places the current image(s)
                                     information. The information is an array of EFI_FIRMWARE_IMAGE_DESCRIPTORs.
  @param[out]     DescriptorVersion  A pointer to the location in which firmware returns the version number
                                     associated with the EFI_FIRMWARE_IMAGE_DESCRIPTOR.
  @param[out]     DescriptorCount    A pointer to the location in which firmware returns the number of
                                     descriptors or firmware images within this device.
  @param[out]     DescriptorSize     A pointer to the location in which firmware returns the size, in bytes,
                                     of an individual EFI_FIRMWARE_IMAGE_DESCRIPTOR.
  @param[out]     PackageVersion     A version number that represents all the firmware images in the device.
                                     The format is vendor specific and new version must have a greater value
                                     than the old version. If PackageVersion is not supported, the value is
                                     0xFFFFFFFF. A value of 0xFFFFFFFE indicates that package version comparison
                                     is to be performed using PackageVersionName. A value of 0xFFFFFFFD indicates
                                     that package version update is in progress.
  @param[out]     PackageVersionName A pointer to a pointer to a null-terminated string representing the
                                     package version name. The buffer is allocated by this function with
                                     AllocatePool(), and it is the caller's responsibility to free it with a call
                                     to FreePool().

  @retval EFI_SUCCESS                The device was successfully updated with the new image.
  @retval EFI_BUFFER_TOO_SMALL       The ImageInfo buffer was too small. The current buffer size
                                     needed to hold the image(s) information is returned in ImageInfoSize.
  @retval EFI_INVALID_PARAMETER      ImageInfoSize is NULL.
  @retval EFI_DEVICE_ERROR           Valid information could not be returned. Possible corrupted image.

**/
EFI_STATUS
EFIAPI
UefiFmpGetImageInfo (
  IN     EFI_FIRMWARE_MANAGEMENT_PROTOCOL  *This,
  IN OUT UINTN                             *ImageInfoSize,
  IN OUT EFI_FIRMWARE_IMAGE_DESCRIPTOR     *ImageInfo,
  OUT    UINT32                            *DescriptorVersion,
  OUT    UINT8                             *DescriptorCount,
  OUT    UINTN                             *DescriptorSize,
  OUT    UINT32                            *PackageVersion,
  OUT    CHAR16                            **PackageVersionName
  )
{
  //
  // Check for valid pointer
  //
  if (ImageInfoSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check the buffer size
  // NOTE: Check this first so caller can get the necessary memory size it must allocate.
  //
  if (*ImageInfoSize < (sizeof (EFI_FIRMWARE_IMAGE_DESCRIPTOR))) {
    *ImageInfoSize = sizeof (EFI_FIRMWARE_IMAGE_DESCRIPTOR);
    return EFI_BUFFER_TOO_SMALL;
  }

  //
  // Confirm that buffer isn't null
  //
  if ( (ImageInfo == NULL) || (DescriptorVersion == NULL) || (DescriptorCount == NULL) || (DescriptorSize == NULL)
       || (PackageVersion == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Set the size to whatever we need
  //
  *ImageInfoSize = sizeof (EFI_FIRMWARE_IMAGE_DESCRIPTOR);

  //
  // Make sure the descriptor has already been loaded or refreshed
  //
  PopulateDescriptor (&mCapsuleFmpEntry);

  //
  // Copy the image descriptor
  //
  CopyMem (ImageInfo, &mCapsuleFmpEntry.Descriptor, sizeof (EFI_FIRMWARE_IMAGE_DESCRIPTOR));

  *DescriptorVersion = EFI_FIRMWARE_IMAGE_DESCRIPTOR_VERSION;
  *DescriptorCount   = 1;
  *DescriptorSize    = sizeof (EFI_FIRMWARE_IMAGE_DESCRIPTOR);
  //
  // means unsupported
  //
  *PackageVersion = 0xFFFFFFFF;

  //
  // Do not update PackageVersionName since it is not supported in this instance.
  //

  return EFI_SUCCESS;
}

/**
  Checks if the firmware image is valid for the device.

  This function allows firmware update application to validate the firmware image without
  invoking the SetImage() first.

  @param[in]  This               A pointer to the EFI_FIRMWARE_MANAGEMENT_PROTOCOL instance.
  @param[in]  ImageIndex         A unique number identifying the firmware image(s) within the device.
                                 The number is between 1 and DescriptorCount.
  @param[in]  Image              Points to the new image.
  @param[in]  ImageSize          Size of the new image in bytes.
  @param[out] ImageUpdatable     Indicates if the new image is valid for update. It also provides,
                                 if available, additional information if the image is invalid.

  @retval EFI_SUCCESS            The image was successfully checked.
  @retval EFI_INVALID_PARAMETER  The Image was NULL.
  @retval EFI_UNSUPPORTED        The operation is not supported.
  @retval EFI_SECURITY_VIOLATION The operation could not be performed due to an authentication failure.

**/
EFI_STATUS
EFIAPI
UefiFmpCheckImage (
  IN     EFI_FIRMWARE_MANAGEMENT_PROTOCOL       *This,
  IN     UINT8                                  ImageIndex,
  IN     CONST VOID                             *Capsule,
  IN     UINTN                                  CapsuleSize,
  OUT    UINT32                                 *ImageUpdatable
  )
{
  EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER        *FmpCapsuleHeader;
  EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER  *ImageHeader;
  UINT64                                        *ItemOffsetList;
  UINT32                                        ItemNum;
  UINTN                                         Index;
  UINT32                                        CheckImageResult;
  UINTN                                         NumberOfHandles;
  EFI_HANDLE                                    *HandleBuffer;
  EFI_FIRMWARE_MANAGEMENT_PROTOCOL              *Fmp;
  EFI_STATUS                                    Status;
  UINTN                                         ImageInfoSize;
  EFI_FIRMWARE_IMAGE_DESCRIPTOR                 *FmpImageInfoBuf;
  EFI_FIRMWARE_IMAGE_DESCRIPTOR                 *TempFmpImageInfo;
  CHAR16                                        *PackageVersionName;
  UINT32                                        FmpImageInfoDescriptorVer;
  UINT8                                         FmpImageInfoCount;
  UINTN                                         DescriptorSize;
  UINT32                                        PackageVersion;
  UINTN                                         Index1;
  UINTN                                         Index2;
  UINT8                                         *Image;
  UINTN                                         MatchedNumberOfHandles;

  HandleBuffer = NULL;
  CheckImageResult = IMAGE_UPDATABLE_VALID;
  MatchedNumberOfHandles = 0;
  Status = CheckTheImage (
             This,
             ImageIndex,
             Capsule,
             CapsuleSize,
             &CheckImageResult
             );
  if (CheckImageResult != IMAGE_UPDATABLE_VALID) {
    goto Exit;
  }
  CheckImageResult = IMAGE_UPDATABLE_INVALID_TYPE;
  FmpCapsuleHeader = (EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER*)((UINT8*)Capsule + ((EFI_CAPSULE_HEADER*)Capsule)->HeaderSize);
  ItemOffsetList   = (UINT64 *)(FmpCapsuleHeader + 1);
  ItemNum          = FmpCapsuleHeader->EmbeddedDriverCount + FmpCapsuleHeader->PayloadItemCount;

  //
  // Check the image with the presented FMP instance.
  // Route payload to right FMP instance
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareManagementProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (!EFI_ERROR(Status)) {
    for(Index1 = 0; Index1 < NumberOfHandles; Index1++) {
      Status = gBS->HandleProtocol(
                      HandleBuffer[Index1],
                      &gEfiFirmwareManagementProtocolGuid,
                      (VOID **)&Fmp
                      );
      if (EFI_ERROR(Status)) {
        continue;
      }
      ImageInfoSize = 0;
      Status = Fmp->GetImageInfo (
                      Fmp,
                      &ImageInfoSize,
                      NULL,
                      NULL,
                      NULL,
                      NULL,
                      NULL,
                      NULL
                      );
      if (Status != EFI_BUFFER_TOO_SMALL) {
        continue;
      }
      FmpImageInfoBuf = NULL;
      FmpImageInfoBuf = AllocateZeroPool (ImageInfoSize);
      if (FmpImageInfoBuf == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Exit;
      }
      PackageVersionName = NULL;
      Status = Fmp->GetImageInfo (
                      Fmp,
                      &ImageInfoSize,               // ImageInfoSize
                      FmpImageInfoBuf,              // ImageInfo
                      &FmpImageInfoDescriptorVer,   // DescriptorVersion
                      &FmpImageInfoCount,           // DescriptorCount
                      &DescriptorSize,              // DescriptorSize
                      &PackageVersion,              // PackageVersion
                      &PackageVersionName           // PackageVersionName
                      );
      //
      // If FMP GetInformation interface failed, skip this resource
      //
      if (EFI_ERROR(Status)) {
        FreePool(FmpImageInfoBuf);
        continue;
      }
      if (PackageVersionName != NULL) {
        FreePool(PackageVersionName);
      }
      TempFmpImageInfo = FmpImageInfoBuf;
      for (Index2 = 0; Index2 < FmpImageInfoCount; Index2++) {
        //
        // Check all the payload entry in capsule payload list
        //
        for (Index = FmpCapsuleHeader->EmbeddedDriverCount; Index < ItemNum; Index++) {
          ImageHeader  = (EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER *)((UINT8 *)FmpCapsuleHeader + ItemOffsetList[Index]);
          if (ImageHeader->Version >= EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER_INIT_VERSION) {
            Image = (UINT8 *)(ImageHeader + 1);
          } else {
            Image = (UINT8 *)ImageHeader + OFFSET_OF(EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER, UpdateHardwareInstance);
          }
          if (CompareGuid(&ImageHeader->UpdateImageTypeId, &TempFmpImageInfo->ImageTypeId) &&
              ImageHeader->UpdateImageIndex == TempFmpImageInfo->ImageIndex) {
            Status = Fmp->CheckImage (
                            Fmp,
                            TempFmpImageInfo->ImageIndex,       // ImageIndex
                            (UINT8 *) Image,                    // Image
                            ImageHeader->UpdateImageSize,       // ImageSize
                            &CheckImageResult
                            );
            if (EFI_ERROR(Status)) {
              CheckImageResult = IMAGE_UPDATABLE_INVALID_TYPE;
              FreePool (FmpImageInfoBuf);
              goto Exit;
            } else {
              CheckImageResult = IMAGE_UPDATABLE_VALID;
            }
          }
        }
        //
        // Use DescriptorSize to move ImageInfo Pointer to stay compatible with different ImageInfo version
        //
        TempFmpImageInfo = (EFI_FIRMWARE_IMAGE_DESCRIPTOR *)((UINT8 *)TempFmpImageInfo + DescriptorSize);
      }
      FreePool (FmpImageInfoBuf);
    }
  }

Exit:
  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }
  if (ImageUpdatable) {
    *ImageUpdatable = CheckImageResult;
  }
  return EFI_SUCCESS;
}

/**
  Process UEFI FMP in capsule image

  This function updates the hardware with the new firmware image.
  This function returns EFI_UNSUPPORTED if the firmware image is not updatable.
  If the firmware image is updatable, the function should perform the following minimal validations
  before proceeding to do the firmware image update.
  - Validate the image authentication if image has attribute 
    IMAGE_ATTRIBUTE_AUTHENTICATION_REQUIRED.  The function returns 
    EFI_SECURITY_VIOLATION if the validation fails.
  - Validate the image is a supported image for this device.  The function returns EFI_ABORTED if 
    the image is unsupported.  The function can optionally provide more detailed information on 
    why the image is not a supported image.
  - Validate the data from VendorCode if not null.  Image validation must be performed before 
    VendorCode data validation.  VendorCode data is ignored or considered invalid if image 
    validation failed.  The function returns EFI_ABORTED if the data is invalid.
  
  VendorCode enables vendor to implement vendor-specific firmware image update policy.  Null if 
  the caller did not specify the policy or use the default policy.  As an example, vendor can implement 
  a policy to allow an option to force a firmware image update when the abort reason is due to the new 
  firmware image version is older than the current firmware image version or bad image checksum.  
  Sensitive operations such as those wiping the entire firmware image and render the device to be
  non-functional should be encoded in the image itself rather than passed with the VendorCode.
  AbortReason enables vendor to have the option to provide a more detailed description of the abort
  reason to the caller.

  @param[in]  This               A pointer to the EFI_FIRMWARE_MANAGEMENT_PROTOCOL instance.
  @param[in]  ImageIndex         A unique number identifying the firmware image(s) within the device.
                                 The number is between 1 and DescriptorCount.
  @param[in]  Image              Points to the new image.
  @param[in]  ImageSize          Size of the new image in bytes.
  @param[in]  VendorCode         This enables vendor to implement vendor-specific firmware image update policy.  
                                 Null indicates the caller did not specify the policy or use the default policy.
  @param[in]  Progress           A function used by the driver to report the progress of the firmware update.
  @param[out] AbortReason        A pointer to a pointer to a null-terminated string providing more
                                 details for the aborted operation. The buffer is allocated by this function 
                                 with AllocatePool(), and it is the caller's responsibility to free it with a
                                 call to FreePool().

  @retval EFI_SUCCESS            The device was successfully updated with the new image.
  @retval EFI_ABORTED            The operation is aborted.
  @retval EFI_INVALID_PARAMETER  The Image was NULL.
  @retval EFI_UNSUPPORTED        The operation is not supported.
  @retval EFI_SECURITY_VIOLATION The operation could not be performed due to an authentication failure.

**/
EFI_STATUS
EFIAPI 
UefiFmpSetImage (
  IN     EFI_FIRMWARE_MANAGEMENT_PROTOCOL               *This,
  IN     UINT8                                          ImageIndex,
  IN     CONST VOID                                     *Capsule,
  IN     UINTN                                          CapsuleSize,
  IN     CONST VOID                                     *VendorCode,
  IN     EFI_FIRMWARE_MANAGEMENT_UPDATE_IMAGE_PROGRESS  Progress,
  OUT    CHAR16                                         **AbortReason
  )
{
  EFI_STATUS                                    Status;
  UINT32                                        CheckImageResult;
  EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER        *FmpCapsuleHeader;
  EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER  *ImageHeader;
  EFI_HANDLE                                    ImageHandle;
  UINT64                                        *ItemOffsetList;
  UINT32                                        ItemNum;
  UINTN                                         Index;
  UINTN                                         ExitDataSize;
  EFI_HANDLE                                    *HandleBuffer;
  EFI_FIRMWARE_MANAGEMENT_PROTOCOL              *Fmp;
  UINTN                                         NumberOfHandles;
  UINTN                                         DescriptorSize;
  UINT8                                         FmpImageInfoCount;
  UINT32                                        FmpImageInfoDescriptorVer;
  UINTN                                         ImageInfoSize;
  UINT32                                        PackageVersion;
  CHAR16                                        *PackageVersionName;
  UINT8                                         *VendorCode1;
  CHAR16                                        *AbortReason1;
  EFI_FIRMWARE_IMAGE_DESCRIPTOR                 *FmpImageInfoBuf;
  EFI_FIRMWARE_IMAGE_DESCRIPTOR                 *TempFmpImageInfo;
  UINTN                                         DriverLen;
  UINTN                                         Index1;
  UINTN                                         Index2;
  MEMMAP_DEVICE_PATH                            MemMapNode;
  EFI_DEVICE_PATH_PROTOCOL                      *DriverDevicePath;
  UINT8                                         *Image;

  Status           = EFI_SUCCESS;
  HandleBuffer     = NULL;
  ExitDataSize     = 0;
  DriverDevicePath = NULL;
  //
  // Check Image before actual action to ensure the capsule is qualified
  //
  Status = CheckTheImage (
             This,
             ImageIndex,
             Capsule,
             CapsuleSize,
             &CheckImageResult
             );
  if (CheckImageResult != IMAGE_UPDATABLE_VALID) {
    return EFI_UNSUPPORTED;
  }
  FmpCapsuleHeader = (EFI_FIRMWARE_MANAGEMENT_CAPSULE_HEADER*)((UINT8*)Capsule + ((EFI_CAPSULE_HEADER*)Capsule)->HeaderSize);
  ItemOffsetList = (UINT64 *)(FmpCapsuleHeader + 1);
  ItemNum = FmpCapsuleHeader->EmbeddedDriverCount + FmpCapsuleHeader->PayloadItemCount;
  //
  // Capsule in which driver count and payload count are both zero is not processed.
  //
  if (ItemNum == 0) {
    return EFI_SUCCESS;
  }
  //
  // Try to load & start all the drivers within capsule 
  //
  SetDevicePathNodeLength (&MemMapNode.Header, sizeof (MemMapNode));
  MemMapNode.Header.Type     = HARDWARE_DEVICE_PATH;
  MemMapNode.Header.SubType  = HW_MEMMAP_DP;
  MemMapNode.MemoryType      = EfiBootServicesCode;
  MemMapNode.StartingAddress = (EFI_PHYSICAL_ADDRESS)(UINTN)Capsule;
  MemMapNode.EndingAddress   = (EFI_PHYSICAL_ADDRESS)(UINTN)((UINT8 *)Capsule + CapsuleSize - 1);
  DriverDevicePath = AppendDevicePathNode (NULL, &MemMapNode.Header);
  if (DriverDevicePath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  for (Index = 0; Index < FmpCapsuleHeader->EmbeddedDriverCount; Index++) {
    if (FmpCapsuleHeader->PayloadItemCount == 0 && Index == (UINTN)FmpCapsuleHeader->EmbeddedDriverCount - 1) {
      //
      // When driver is last element in the ItemOffsetList array, the driver size is calculated by reference CapsuleImageSize in EFI_CAPSULE_HEADER
      //
      DriverLen = CapsuleSize - ((EFI_CAPSULE_HEADER*)Capsule)->HeaderSize - (UINTN)ItemOffsetList[Index];
    } else {
      DriverLen = (UINTN)ItemOffsetList[Index + 1] - (UINTN)ItemOffsetList[Index];
    }
    Status = gBS->LoadImage(
                    FALSE,
                    gImageHandle,
                    DriverDevicePath,
                    (UINT8 *)FmpCapsuleHeader + ItemOffsetList[Index],
                    DriverLen,
                    &ImageHandle
                    );
    if (EFI_ERROR(Status)) {
      goto Exit;
    }
    Status = gBS->StartImage(
                    ImageHandle, 
                    &ExitDataSize, 
                    NULL
                    );
    if (EFI_ERROR(Status)) {
      DEBUG ((DEBUG_ERROR, "Driver Return Status = %r\n", Status));
      goto Exit;
    }
  }
  //
  // Route payload to right FMP instance
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareManagementProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (!EFI_ERROR(Status)) {
    for(Index1 = 0; Index1 < NumberOfHandles; Index1++) {
      Status = gBS->HandleProtocol(
                      HandleBuffer[Index1],
                      &gEfiFirmwareManagementProtocolGuid,
                      (VOID **)&Fmp
                      );
      if (EFI_ERROR(Status)) {
        continue;
      }
      ImageInfoSize = 0;
      Status = Fmp->GetImageInfo (
                      Fmp,
                      &ImageInfoSize,
                      NULL,
                      NULL,
                      NULL,
                      NULL,
                      NULL,
                      NULL
                      );
      if (Status != EFI_BUFFER_TOO_SMALL) {
        continue;
      }
      FmpImageInfoBuf = NULL;
      FmpImageInfoBuf = AllocateZeroPool (ImageInfoSize);
      if (FmpImageInfoBuf == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Exit;
      }
      PackageVersionName = NULL;
      Status = Fmp->GetImageInfo (
                      Fmp,
                      &ImageInfoSize,               // ImageInfoSize
                      FmpImageInfoBuf,              // ImageInfo
                      &FmpImageInfoDescriptorVer,   // DescriptorVersion
                      &FmpImageInfoCount,           // DescriptorCount
                      &DescriptorSize,              // DescriptorSize
                      &PackageVersion,              // PackageVersion
                      &PackageVersionName           // PackageVersionName
                      );
      //
      // If FMP GetInformation interface failed, skip this resource
      //
      if (EFI_ERROR(Status)) {
        FreePool(FmpImageInfoBuf);
        continue;
      }
      if (PackageVersionName != NULL) {
        FreePool(PackageVersionName);
      }
      TempFmpImageInfo = FmpImageInfoBuf;
      for (Index2 = 0; Index2 < FmpImageInfoCount; Index2++) {
        //
        // Check all the payload entry in capsule payload list 
        //
        for (Index = FmpCapsuleHeader->EmbeddedDriverCount; Index < ItemNum; Index++) {
          ImageHeader  = (EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER *)((UINT8 *)FmpCapsuleHeader + ItemOffsetList[Index]);
          if (CompareGuid(&ImageHeader->UpdateImageTypeId, &TempFmpImageInfo->ImageTypeId) &&
              ImageHeader->UpdateImageIndex == TempFmpImageInfo->ImageIndex) {
            AbortReason1 = NULL;
            VendorCode1 = (ImageHeader->UpdateVendorCodeSize == 0) ? NULL : (UINT8*)((UINT8*)(ImageHeader + 1) + ImageHeader->UpdateImageSize);
            if (ImageHeader->Version >= EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER_INIT_VERSION) {
              Image = (UINT8 *)(ImageHeader + 1);
            } else {
              Image = (UINT8 *)ImageHeader + OFFSET_OF(EFI_FIRMWARE_MANAGEMENT_CAPSULE_IMAGE_HEADER, UpdateHardwareInstance);
            }
            Status = Fmp->SetImage(
                            Fmp,
                            TempFmpImageInfo->ImageIndex,       // ImageIndex
                            (UINT8*)Image,                      // Image
                            ImageHeader->UpdateImageSize,       // ImageSize
                            VendorCode1,                        // VendorCode
                            Progress,                           // Progress
                            &AbortReason1                       // AbortReason
                            );
            if (AbortReason1 != NULL) {
              DEBUG ((EFI_D_ERROR, "%s\n", AbortReason1));
              FreePool(AbortReason1);
            }
          }
        }
        //
        // Use DescriptorSize to move ImageInfo Pointer to stay compatible with different ImageInfo version
        //
        TempFmpImageInfo = (EFI_FIRMWARE_IMAGE_DESCRIPTOR *)((UINT8 *)TempFmpImageInfo + DescriptorSize);
      }
      FreePool(FmpImageInfoBuf);
    }
  }
Exit:
  if (HandleBuffer != NULL) {
    FreePool(HandleBuffer);
  }
  if (DriverDevicePath != NULL) {
    FreePool(DriverDevicePath);
  }
  //
  // Need repopulate after SetImage is called to
  // update LastAttemptVersion and LastAttemptStatus.
  //
  mCapsuleFmpEntry.DescriptorPopulated = FALSE;
  return Status;
}

/**

  Installation of capsule in UEFI FMP processor

**/
EFI_STATUS
InstallCapsuleInUefiFmp (
  VOID
  )
{
  if ((PcdGet64 (PcdOsIndicationsSupported) & EFI_OS_INDICATIONS_FMP_CAPSULE_SUPPORTED) == 0) return EFI_UNSUPPORTED;
  mCapsuleFmp.CapsuleProcessorCount ++;
  mCapsuleFmpEntry.ImageIndex = (UINT8)mCapsuleFmp.CapsuleProcessorCount;
  CopyGuid (&mCapsuleFmpEntry.ImageTypeGuid, &gEfiFmpCapsuleGuid);
  mCapsuleFmpEntry.SetImage     = UefiFmpSetImage;
  mCapsuleFmpEntry.CheckImage   = UefiFmpCheckImage;
  mCapsuleFmpEntry.GetImageInfo = UefiFmpGetImageInfo;
  InsertTailList (&mCapsuleFmp.CapsuleProcessorListHead, (LIST_ENTRY*)&mCapsuleFmpEntry);
  return EFI_SUCCESS;
}