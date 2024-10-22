/** @file
  Firmware authentication routines
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

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/RetrieveSpecificFfsInFv.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseCryptLib.h>
#include <Guid/FirmwareFileSystem2.h>
#include <Ppi/FirmwareAuthentication.h>

#define MEMORY_SIZE_4K                    0x1000
#define EFI_CERT_TYPE_RSA2048_SHA256_SIZE 256
#define EFI_CERT_TYPE_RSA1024_SHA256_SIZE 128

CONST STATIC UINT8 mRsaE[] = { 0x01, 0x00, 0x01 };

//
// Signature of Certification, for not having signature in machine code of this library, assign value one by one byte
//
STATIC CHAR8         mCertSignature[] = {'$', '_', 'I', 'F', 'L', 'A','S','H', '_', 'B', 'I', 'O', 'S', 'C', 'E', 'R'};

/**
 SHA256 HASH calculation

 @param [in]   Message          The message data to be calculated
 @param [in]   MessageSize      The size in byte of the message data
 @param [out]  Digest           The caclulated HASH digest

 @retval EFI_SUCCESS            The HASH value is calculated
 @retval EFI_SECURITY_VIOLATION  Failed to calculate the HASH

**/
EFI_STATUS
CalculateSha256Hash (
  IN UINT8                      *Message,
  IN UINTN                      MessageSize,
  OUT UINT8                     *Digest
  )
{
  VOID       *HashCtx;
  UINTN      CtxSize;
  EFI_STATUS Status;

  SetMem (Digest, SHA256_DIGEST_SIZE, 0);
  CtxSize = Sha256GetContextSize ();
  HashCtx = NULL;
  HashCtx = AllocatePool (CtxSize);
  if (HashCtx == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (!Sha256Init (HashCtx)) {
    Status = EFI_SECURITY_VIOLATION;
    goto Done;
  }
  if(!Sha256Update (HashCtx, Message, MessageSize)) {
    Status = EFI_SECURITY_VIOLATION;
    goto Done;
  }
  if(!Sha256Final (HashCtx, Digest)) {
    Status = EFI_SECURITY_VIOLATION;
  } else {
    Status = EFI_SUCCESS;
  }

Done:
  FreePool (HashCtx);
  return Status;
}

/**
 SHA1 HASH calculation

 @param [in]   Message          The message data to be calculated
 @param [in]   MessageSize      The size in byte of the message data
 @param [out]  Digest           The caclulated HASH digest

 @retval EFI_SUCCESS            The HASH value is calculated
 @retval EFI_SECURITY_VIOLATION  Failed to calculate the HASH

**/
EFI_STATUS
EFIAPI
CalculateSha1Hash (
  IN UINT8                      *Message,
  IN UINTN                      MessageSize,
  OUT UINT8                     *Digest
  )
{
  VOID       *HashCtx;
  UINTN      CtxSize;
  EFI_STATUS Status;

  SetMem (Digest, SHA1_DIGEST_SIZE, 0);
  CtxSize = Sha1GetContextSize ();
  HashCtx = NULL;
  HashCtx = AllocatePool (CtxSize);
  if (HashCtx == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (!Sha1Init (HashCtx)) {
    Status = EFI_SECURITY_VIOLATION;
    goto Done;
  }
  if(!Sha1Update (HashCtx, Message, MessageSize)) {
    Status = EFI_SECURITY_VIOLATION;
    goto Done;
  }
  if(!Sha1Final (HashCtx, Digest)) {
    Status = EFI_SECURITY_VIOLATION;
  } else {
    Status = EFI_SUCCESS;
  }

Done:
  FreePool (HashCtx);
  return Status;
}

/**
  Update the data of the unsigned FV region.

  @param  ImageBuffer             The image address
  @param  ImageSize               The size of the image

  @retval EFI_SUCCESS             update succuessfully
  @retval EFI_NOT_FOUND           cannot find the unsigned FV region

**/
EFI_STATUS
UnsignedFvRegionUpdate (
  IN UINT8                      *ImageBuffer,
  IN UINTN                      ImageSize
  )
{
  UINTN                         Index;
  UINTN                         Index2;
  EFI_FIRMWARE_VOLUME_HEADER    *FvHeaderPtr;
  EFI_FFS_FILE_HEADER           *FileHeaderPtr;
  UINT32                        FileLength;
  BOOLEAN                       FileGuidFound;
  UINT8                         *SkipRegionAddress;
  UINTN                         SkipRegionSize;

  FvHeaderPtr   = NULL;
  FileHeaderPtr = NULL;
  FileGuidFound = FALSE;

  //
  // Find the skip region from the image, preserve it before modifying it.
  //
  for (Index = 0; Index < ImageSize; Index += ALIGHMENT_SIZE) {
    FvHeaderPtr = (EFI_FIRMWARE_VOLUME_HEADER *)(ImageBuffer + Index);
    if ((FvHeaderPtr->Signature == EFI_FVH_SIGNATURE) &&
        (CompareGuid (&FvHeaderPtr->FileSystemGuid, &gEfiFirmwareFileSystem2Guid))) {
      Index2 = 0;
      while (Index2 < FvHeaderPtr->FvLength) {
        FileHeaderPtr = (EFI_FFS_FILE_HEADER *)(ImageBuffer + Index + FvHeaderPtr->HeaderLength + Index2);
        if (CompareGuid (PcdGetPtr (PcdUnsignedFvKeyFile), &FileHeaderPtr->Name) == TRUE) {
          FileGuidFound = TRUE;
          break;
        } else {
          FileLength = *(UINT32 *)(FileHeaderPtr->Size) & 0x00FFFFFF;
          Index2 += FileLength;
        }
      }
    }
    if (FileGuidFound) {
      break;
    }
  }
  if (!FileGuidFound) {
    return EFI_NOT_FOUND;
  }

  SkipRegionAddress = (UINT8 *)FvHeaderPtr;
  SkipRegionSize = (UINTN)(FvHeaderPtr->FvLength);
  SetMem (SkipRegionAddress, SkipRegionSize, 0xFF);

  return EFI_SUCCESS;
}


/**
  Check if the image is a PE image

  @param  ImageBuffer             The image data


  @retval TRUE                    The image binary is a PE formated image
  @retval FALSE                   The image binary is not a PE formated image

**/
BOOLEAN
FileIsPeImage (
  IN UINT8      *ImageBuffer
  )
{
  ASSERT (ImageBuffer != NULL);
  return ((CHAR8)ImageBuffer[0] == 'M') && ((CHAR8)ImageBuffer[1] == 'Z');
}

/**
  Retrive firmware binary from a PE formated image (standalone flash utility)

  @param[in] FirmwareFileData   PE32 file image buffer
  @param[in] FirmwareFileSize   File size of the recovery firmware file
  @param[out] BiosImageOffset   BIOS image offset from the firmware file beginning
  @param[out] BiosImageSize     BIOS image size
  @param[out] SignatureOffset   RSA signature offset from the firmware file beginning
  @param[out] SignatureSize     RSA signature size

  @retval EFI_SUCCESS           The firmware binary is successfully retrieved
  @retval EFI_INVALID_PARAMETER The given FirmwareBin or FirmwareSize are NULL pointers
  @retval EFI_NOT_FOUND         Unable to find BIOS image or signature data
**/
EFI_STATUS
RetrieveFirmwareFromPeImage (
  IN UINT8      *FirmwareFileData,
  IN UINTN      FirmwareFileSize,
  OUT UINTN     *BiosImageOffset,
  OUT UINTN     *BiosImageSize,
  OUT UINTN     *SignatureOffset,
  OUT UINTN     *SignatureSize
  )
{
  UINTN         Index;
  ISFLASH_DATA_REGION_HEADER  *DataRegion;
  ISFLASH_DATA_REGION_HEADER  *Cert;


  if ((FirmwareFileData == NULL) || (FirmwareFileSize == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Search for BIOS image
  //
  for (Index = 0; Index < FirmwareFileSize - ISFLASH_TAG_SIZE; Index++) {
    if (CompareMem (FirmwareFileData + Index, ISFLASH_BIOS_IMAGE_TAG, ISFLASH_TAG_SIZE) == 0) {
      break;
    }
  }
  if (Index == FirmwareFileSize - ISFLASH_TAG_SIZE) {
    return EFI_NOT_FOUND;
  }

  //
  // Copy BIOS image to the start of FirmwareBin pointer
  //
  DataRegion = (ISFLASH_DATA_REGION_HEADER *)(FirmwareFileData + Index);
  *BiosImageOffset = Index + sizeof (ISFLASH_DATA_REGION_HEADER);
  *BiosImageSize = DataRegion->DataSize;
  if (*BiosImageOffset + *BiosImageSize > FirmwareFileSize) {
    return EFI_NOT_FOUND;
  }

  //
  // Search for RSA-2048, RSA-1024 signature
  //
  for (Index = 0;Index < FirmwareFileSize - ISFLASH_TAG_SIZE; Index++) {
    Cert = (ISFLASH_DATA_REGION_HEADER*)(FirmwareFileData + Index);
    if (CompareMem (Cert, mCertSignature, ISFLASH_TAG_SIZE) == 0 && \
    ( Cert->AllocatedSize >= Cert->DataSize ) && \
    ( (Cert->DataSize >= EFI_CERT_TYPE_RSA1024_SHA256_SIZE))) {
      break;
    }
  }

  if (Index == FirmwareFileSize - ISFLASH_TAG_SIZE) {
    return EFI_NOT_FOUND;
  }



  //
  // Append signature to the BIOS image
  //
  DataRegion       = (ISFLASH_DATA_REGION_HEADER *)(FirmwareFileData + Index);
  *SignatureOffset = Index + sizeof (ISFLASH_DATA_REGION_HEADER);
  *SignatureSize   = DataRegion->DataSize;
  if (*SignatureOffset + *SignatureSize > FirmwareFileSize) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/**
 Helper function to verify signature by using input public key

  @param [in]   PublicKey         Pointer to input public key.
  @param [in]   PublicKeySize     Public key size in bytes.
  @param [in]   Signature         Pointer to input signature.
  @param [in]   SignatureSize     Signature buffer size in bytes.
  @param [in]   Digest            Pointer to input digest.
  @param [in]   DigestSize        Digest size in bytes.

  @retval EFI_SUCCESS             The signature verification is successful
  @retval EFI_SECURITY_VIOLATION  Failed to verify the firmware
**/
STATIC
EFI_STATUS
VerifyImageByPublicKey (
  IN  UINT8        *PublicKey,
  IN  UINTN        PublicKeySize,
  IN  UINT8        *Signature,
  IN  UINTN        SignatureSize,
  IN  UINT8        *Digest,
  IN  UINTN        DigestSize
  )
{
  VOID           *Rsa;
  EFI_STATUS     Status;

  Rsa = RsaNew ();
  if (Rsa == NULL) {
    return EFI_SECURITY_VIOLATION;
  }

  Status = EFI_SECURITY_VIOLATION;
  if(!RsaSetKey (Rsa, RsaKeyN, PublicKey, PublicKeySize)) {
    goto Done;
  }
  if(!RsaSetKey (Rsa, RsaKeyE, mRsaE, sizeof (mRsaE))) {
    goto Done;
  }

  if (RsaPkcs1Verify (Rsa, Digest, DigestSize, Signature, SignatureSize)) {
    Status = EFI_SUCCESS;
  }

Done:
  RsaFree (Rsa);
  return Status;
}


/**
 Firmware verification with RSA2048-SHA256

 @param [in]   FirmwareFileData  Firmware file data buffer
 @param [in]   FirmwareFileSize  The firmware file size including signature

 @retval EFI_SUCCESS            The firmware verification is successful
 @retval EFI_OUT_OF_RESOURCES   Out of resources
 @retval EFI_SECURITY_VIOLATION  Failed to verify the firmware

**/
EFI_STATUS
VerifyFirmware (
  IN UINT8                      *FirmwareFileData,
  IN UINTN                      FirmwareFileSize
  )
{
  EFI_STATUS Status;
  EFI_STATUS StatusSha1;
  EFI_STATUS StatusSha256;
  UINT8      *Signature;
  UINT8      *AllPublickKeys;
  UINTN      TotalKeySize;
  UINT8      *PublicKey;
  UINTN      PublicKeySize;
  UINT8      DigestSha256[SHA256_DIGEST_SIZE];
  UINT8      DigestSha1[SHA1_DIGEST_SIZE];
  UINT8      *BiosImage;
  UINTN      BiosImageOffset;
  UINTN      BiosImageSize;
  UINTN      SignatureOffset;
  UINTN      SignatureSize;
  UINT8      *BiosImageBackup;
  UINTN      Offset;

  BiosImageBackup = NULL;

  if (FirmwareFileData == NULL || FirmwareFileSize == 0) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Initial setting for BIOS image and signature information
  //
  BiosImageOffset = 0;
  BiosImageSize   = FirmwareFileSize - EFI_CERT_TYPE_RSA2048_SHA256_SIZE;
  SignatureOffset = FirmwareFileSize - EFI_CERT_TYPE_RSA2048_SHA256_SIZE;
  SignatureSize   = EFI_CERT_TYPE_RSA2048_SHA256_SIZE;

  if (!FeaturePcdGet (PcdSecureFlashSupported)) {
    return EFI_SUCCESS;
  }
  //
  // If firmware file is an executable secure flash image,
  // update BIOS image and signature information
  //
  if (FileIsPeImage (FirmwareFileData)) {
    Status = RetrieveFirmwareFromPeImage (
               FirmwareFileData,
               FirmwareFileSize,
               &BiosImageOffset,
               &BiosImageSize,
               &SignatureOffset,
               &SignatureSize
               );
    if (EFI_ERROR (Status)) {
      ASSERT_EFI_ERROR (Status);
      return EFI_SECURITY_VIOLATION;
    }
  }
  BiosImage = FirmwareFileData + BiosImageOffset;
  Signature = FirmwareFileData + SignatureOffset;

  if (PcdGetBool (PcdUnsignedFvSupported)) {
    BiosImageBackup = AllocatePages ((BiosImageSize / MEMORY_SIZE_4K) + 1);
    if (BiosImageBackup == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (BiosImageBackup, BiosImage, BiosImageSize);
    UnsignedFvRegionUpdate (BiosImage, BiosImageSize);
  }

  if (PcdGetBool (PcdH2OCapsuleSignatureDatabaseSupported)) {
    Status = RetrieveSpecificFfsInFv (PcdGetPtr (PcdSecureFlashPublicKeyFile), (VOID **)&AllPublickKeys, &TotalKeySize);
    if (EFI_ERROR (Status)) {
      goto Done;
    }
    StatusSha256 = CalculateSha256Hash (BiosImage, BiosImageSize, DigestSha256);
    StatusSha1   = CalculateSha1Hash (BiosImage, BiosImageSize, DigestSha1);
    Status       = EFI_SECURITY_VIOLATION;
    for (Offset = 0; Offset < TotalKeySize; Offset += (PublicKeySize + sizeof (UINT32))) {
      PublicKey      = AllPublickKeys + Offset + sizeof (UINT32);
      PublicKeySize  = *((UINT32 *)(AllPublickKeys + Offset));
      if (!EFI_ERROR (StatusSha256)) {
        Status = VerifyImageByPublicKey (PublicKey, PublicKeySize, Signature, SignatureSize, DigestSha256, SHA256_DIGEST_SIZE);
      }
      if (!EFI_ERROR (StatusSha1) && EFI_ERROR (Status)) {
        Status = VerifyImageByPublicKey (PublicKey, PublicKeySize, Signature, SignatureSize, DigestSha1, SHA1_DIGEST_SIZE);
      }
      if (!EFI_ERROR (Status)) {
        break;
      }
    }
  } else {
    Status = RetrieveSpecificFfsInFv (PcdGetPtr (PcdSecureFlashPublicKeyFile), (VOID **)&PublicKey, &PublicKeySize);
    if (EFI_ERROR (Status)) {
      goto Done;
    }
    Status = EFI_SECURITY_VIOLATION;
    StatusSha256 = CalculateSha256Hash (BiosImage, BiosImageSize, DigestSha256);
    if (!EFI_ERROR (StatusSha256)) {
      Status = VerifyImageByPublicKey (PublicKey, PublicKeySize, Signature, SignatureSize, DigestSha256, SHA256_DIGEST_SIZE);
      if (!EFI_ERROR (Status)) {
        goto Done;
      }
    }
    StatusSha1   = CalculateSha1Hash (BiosImage, BiosImageSize, DigestSha1);
    if (!EFI_ERROR (StatusSha1)) {
      Status = VerifyImageByPublicKey (PublicKey, PublicKeySize, Signature, SignatureSize, DigestSha1, SHA1_DIGEST_SIZE);
    }
  }

Done:
  if (PcdGetBool (PcdUnsignedFvSupported)) {
    CopyMem (BiosImage, BiosImageBackup, BiosImageSize);
    FreePool (BiosImageBackup);
  }
  return Status;
}
