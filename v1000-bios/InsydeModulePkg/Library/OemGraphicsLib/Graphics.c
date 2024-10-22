/** @file
  Support for Basic Graphics operations.

;******************************************************************************
;* Copyright (c) 2012 - 2020, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "Library/OemGraphicsLib.h"
#include <IndustryStandard/Bmp.h>
#include <Protocol/JpegDecoder.h>
#include <Protocol/PcxDecoder.h>
#include <Protocol/TgaDecoder.h>
#include <Protocol/GifDecoder.h>
#include <Protocol/BmpDecoder.h>
#include <Protocol/PngDecoder.h>
#include <Protocol/BootLogo.h>
#include <Protocol/UgaDraw.h>
#include <Protocol/EndOfDisableQuietBoot.h>
#include <Protocol/ConsoleControl.h>
#include <Protocol/DynamicHotKey.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <Library/VariableLib.h>
#include <Library/H2OCpLib.h>
#include <Guid/Jpeg.h>
#include <Guid/ConsoleOutDevice.h>
#include <Guid/H2OBdsCheckPoint.h>
#include <Guid/EsrtCrbFirmwareGuid.h>
#include <Guid/ImageAuthentication.h>
#include <Guid/EfiSystemResourceTable.h>
#include <Protocol/Hash2.h>
#include <VesaBiosExtensions.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/FlashRegionLib.h>

#define SIZE_OF_QA_TEST_KEY         0x311
STATIC UINT8    mQACertificateHashData[SHA256_DIGEST_SIZE] = {
           0xF1, 0x47, 0x00, 0xFC, 0xCA, 0x6A, 0xED, 0x9C, 0x5D, 0x33, 0xF3, 0x56, 0xE3, 0xF8, 0xA3, 0x41,
           0x3B, 0xED, 0xA7, 0x6D, 0xAC, 0xAA, 0x13, 0x24, 0xB0, 0x0E, 0x3D, 0x0A, 0x05, 0x44, 0x6b, 0x18
           };

#define IS_VALID_PERCENTAGE_VALUE(x)  (ABS(x) <= 100)

typedef struct {
  UINT8         *Blt;
  UINTN         ImageWidth;
  UINTN         ImageHeight;
  UINTN         DestinationX;
  UINTN         DestinationY;
  INT8          Priority;
} LOGO_INFO_DATA;

//
//  Record the current VGA Mode from EFI code
//
#define EFI_CURRENT_VGA_MODE_ADDRESS                      0x4A3
//
//  Record the current VGA Mode from VGA OpRom
//
#define CURRENT_VGA_MODE_ADDRESS                          0x449

//
// Priority definition of Logo.
//
#define LOWEST_LOGO_PRIORITY                              -128

STATIC UINTN                                  mAutoPlaceStrDestY = (UINTN) (-1);
STATIC UINTN                                  mAutoPlaceStrCount;


/**
  Get GOP protocol instance.

  @param[out]  GraphicsOutput     Graphics Output Protocol instance pointer.

  @retval EFI_SUCCESS             Get GOP protocol instance success.
  @retval Others                  Failed to get GOP protocol instance.
**/
EFI_STATUS
GetBootGop (
  OUT EFI_GRAPHICS_OUTPUT_PROTOCOL     **GraphicsOutput
  )
{
  EFI_STATUS                           Status;

  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **) GraphicsOutput);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}


BOOLEAN
IsModeSync (
  VOID
);

EFI_STATUS
BgrtDecodeImageToBlt (
  IN       UINT8                           *ImageData,
  IN       UINTN                           ImageSize,
  IN       EFI_BADGING_SUPPORT_FORMAT      ImageFormat,
  IN OUT   UINT8                           **Blt,
  IN OUT   UINTN                           *Width,
  IN OUT   UINTN                           *Height
  )
{
  EFI_STATUS                                    Status;
  UINTN                                         BltSize;
  EFI_JPEG_DECODER_PROTOCOL                     *JpegDecoder;
  EFI_PCX_DECODER_PROTOCOL                      *PcxDecoder;
  EFI_TGA_DECODER_PROTOCOL                      *TgaDecoder;
  EFI_GIF_DECODER_PROTOCOL                      *GifDecoder;
  EFI_BMP_DECODER_PROTOCOL                      *BmpDecoder;
  EFI_PNG_DECODER_PROTOCOL                      *PngDecoder;
  BOOLEAN                                       HasAlphaChannel;
  TGA_FORMAT                                    TgaFormate;
  EFI_JPEG_DECODER_STATUS                       DecoderStatus;
  ANIMATION                                     *Animation;

  switch (ImageFormat) {

  case EfiBadgingSupportFormatBMP:
    Status = gBS->LocateProtocol (&gEfiBmpDecoderProtocolGuid, NULL, (VOID **) &BmpDecoder);
    if (EFI_ERROR (Status)) {
      break;
    }
    Status = BmpDecoder->DecodeImage (
                           BmpDecoder,
                           ImageData,
                           ImageSize,
                           Blt,
                           &BltSize,
                           Height,
                           Width
                           );
    break;

  case EfiBadgingSupportFormatTGA:
    Status = gBS->LocateProtocol (&gEfiTgaDecoderProtocolGuid, NULL, (VOID **)&TgaDecoder);
    if (EFI_ERROR (Status)) {
      break;
    }
    Status = TgaDecoder->DecodeImage (
                           TgaDecoder,
                           ImageData,
                           ImageSize,
                           Blt,
                           &BltSize,
                           Height,
                           Width,
                           &TgaFormate,
                           &HasAlphaChannel
                           );
    break;

  case EfiBadgingSupportFormatJPEG:
    Status = gBS->LocateProtocol (&gEfiJpegDecoderProtocolGuid, NULL, (VOID **)&JpegDecoder);
    if (EFI_ERROR (Status)) {
      break;
    }
    Status = JpegDecoder->DecodeImage (
                            JpegDecoder,
                            ImageData,
                            ImageSize,
                            Blt,
                            &BltSize,
                            Height,
                            Width,
                            &DecoderStatus
                            );
    break;

  case EfiBadgingSupportFormatPCX:
    Status = gBS->LocateProtocol (&gEfiPcxDecoderProtocolGuid, NULL, (VOID **)&PcxDecoder);
    if (EFI_ERROR (Status)) {
      break;
    }
    Status = PcxDecoder->DecodeImage (
                           PcxDecoder,
                           ImageData,
                           ImageSize,
                           Blt,
                           &BltSize,
                           Height,
                           Width
                           );
    break;

  case EfiBadgingSupportFormatPNG:
    Status = gBS->LocateProtocol (&gEfiPngDecoderProtocolGuid, NULL, (VOID **)&PngDecoder);
    if (EFI_ERROR (Status)) {
      break;
    }
    Status = PngDecoder->DecodeImage (
                           PngDecoder,
                           ImageData,
                           ImageSize,
                           Blt,
                           &BltSize,
                           Height,
                           Width
                           );
    break;

  case EfiBadgingSupportFormatGIF:
    Status = gBS->LocateProtocol (&gEfiGifDecoderProtocolGuid, NULL, (VOID **)&GifDecoder);
    if (EFI_ERROR (Status)) {
      break;
    }
    //
    // BGRT only support the first image if .gif file.
    //
    Status = GifDecoder->CreateAnimationFromMem (
                           GifDecoder,
                           ImageData,
                           ImageSize,
                           NULL,
                           &Animation
                           );
    if (!EFI_ERROR (Status)) {
      *Width  = Animation->Width;
      *Height = Animation->Height;

      BltSize = Animation->Width * Animation->Height * sizeof (EFI_UGA_PIXEL);
      *Blt = AllocateZeroPool (BltSize);
      if (*Blt != NULL) {
        CopyMem (*Blt, Animation->Frames->Bitmap, BltSize);
        GifDecoder->DestroyAnimation (GifDecoder, Animation);
      } else {
        Status = EFI_OUT_OF_RESOURCES;
      }
    }
    break;

  default:
    Status = EFI_UNSUPPORTED;
    break;
  }

  return Status;
}

EFI_STATUS
BgrtGetImageByBadging (
  IN OUT  UINT8                          **ImageData,
  IN OUT  UINTN                          *ImageSize,
  IN OUT  EFI_BADGING_SUPPORT_FORMAT     *ImageFormat
  )
{
  EFI_STATUS                                    Status;
  UINT32                                        Instance;
  EFI_BADGING_SUPPORT_DISPLAY_ATTRIBUTE         Attribute;
  UINTN                                         CoordinateX;
  UINTN                                         CoordinateY;
  EFI_BADGING_SUPPORT_IMAGE_TYPE                Type;
  EFI_OEM_BADGING_SUPPORT_PROTOCOL              *Badging;

  Status  = gBS->LocateProtocol (&gEfiOEMBadgingSupportProtocolGuid, NULL, (VOID **)&Badging);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Instance   = 0;
  Type       = EfiBadgingSupportImageBoot;

  Status = Badging->GetImage (
                      Badging,
                      &Instance,
                      &Type,
                      ImageFormat,
                      ImageData,
                      ImageSize,
                      &Attribute,
                      &CoordinateX,
                      &CoordinateY
                      );
  return Status;
}

/**
  Get logo pos X and Y value.

  @param[in]  LogoRegionId        Pointer to region logo ID.
  @param[out] PosX                Pointer to logo image position X
  @param[out] PosY                Pointer to logo image position Y

  @retval EFI_SUCCESS             Get logo pos X and Y value successfully.
  @retval EFI_INVALID_PARAMETER   Input parameter is NULL or logo position is invalid percentage value.
**/
STATIC
EFI_STATUS
GetLogoPosXY (
  IN  H2O_FLASH_MAP_REGION_LOGO_ID           *LogoRegionId,
  OUT INT16                                  *PosX,
  OUT INT16                                  *PosY
  )
{
  EFI_STATUS                                 Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL               *GraphicsOutput;
  INT32                                      PositionX;
  INT32                                      PositionY;

  if (LogoRegionId == NULL || PosX == NULL || PosY == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetBootGop (&GraphicsOutput);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (((LogoRegionId->Flags) & H2O_FLASH_MAP_REGION_LOGO_POS_X_PERCENTAGE) == H2O_FLASH_MAP_REGION_LOGO_POS_X_PERCENTAGE) {
    if (!IS_VALID_PERCENTAGE_VALUE(LogoRegionId->PosX)) {
      return EFI_INVALID_PARAMETER;
    }
    PositionX = ((INT32) LogoRegionId->PosX * (INT32) GraphicsOutput->Mode->Info->HorizontalResolution) / 100;
  } else {
    PositionX = (INT32) LogoRegionId->PosX;
  }

  if (((LogoRegionId->Flags) & H2O_FLASH_MAP_REGION_LOGO_POS_Y_PERCENTAGE) == H2O_FLASH_MAP_REGION_LOGO_POS_Y_PERCENTAGE) {
    if (!IS_VALID_PERCENTAGE_VALUE(LogoRegionId->PosY)) {
      return EFI_INVALID_PARAMETER;
    }
    PositionY = ((INT32) LogoRegionId->PosY * (INT32) GraphicsOutput->Mode->Info->VerticalResolution) / 100;
  } else {
    PositionY = (INT32) LogoRegionId->PosY;
  }

  *PosX = (INT16) PositionX;
  *PosY = (INT16) PositionY;
  return EFI_SUCCESS;
}

/**
  SHA256 HASH calculation

  @param [in]   Message          The message data to be calculated
  @param [in]   MessageSize      The size in byte of the message data
  @param [out]  Digest           The caclulated HASH digest

  @retval EFI_SUCCESS            The HASH value is calculated
  @retval EFI_SECURITY_VIOLATION  Failed to calculate the HASH

**/
EFI_STATUS
CalculateHash (
  IN UINT8                      *Message,
  IN UINTN                      MessageSize,
  OUT UINT8                     *Digest
  )
{
  EFI_STATUS                    Status;
  EFI_HASH2_PROTOCOL            *Hash2Protocol;
  UINTN                         CaseIndex;


  CaseIndex = 0;
  Status = gBS->LocateProtocol (
                  &gEfiHash2ProtocolGuid,
                  NULL,
                  (VOID **) &Hash2Protocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SetMem (Digest, SHA256_DIGEST_SIZE, 0);

  Status = Hash2Protocol->Hash (
                            Hash2Protocol,
                            &gEfiHashAlgorithmSha256Guid,
                            Message,
                            MessageSize,
                            (EFI_HASH2_OUTPUT *) Digest
                            );
  if (EFI_ERROR (Status)) {
    return EFI_ABORTED;
  }

  return Status;
}

/**
  Check if there is a QA test key in Factory Copy.

  @retval TRUE   - found the QA test certificate
  @retval FALSE  - did not found the QA test certificate

**/
BOOLEAN
CheckCertFromFactoryCopy (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINT8                         *VarBuffer;
  UINTN                         VariableSize;
  EFI_SIGNATURE_LIST            *SignatureList;
  EFI_SIGNATURE_DATA            *SignatureData;
  UINTN                         Offset;
  UINT8                         Digest[SHA256_DIGEST_SIZE];

  VarBuffer = NULL;
  Status = CommonGetVariableDataAndSize (
             L"dbDefault",
             &gEfiGlobalVariableGuid,
             &VariableSize,
             (VOID **) &VarBuffer
             );
  if (EFI_ERROR (Status) || VarBuffer == NULL) {
    return FALSE;
  }

  Offset = 0;
  while (Offset < VariableSize) {
    SignatureList = (EFI_SIGNATURE_LIST *) (VarBuffer + Offset);
    SignatureData = (EFI_SIGNATURE_DATA *) ((UINT8 *)SignatureList + sizeof (EFI_SIGNATURE_LIST));
    //
    // Compare the certificate size first, if the size doesn't match, skip to the next one.
    //
    if ((SignatureList->SignatureSize - sizeof (EFI_GUID)) == SIZE_OF_QA_TEST_KEY) {
      Status = CalculateHash (SignatureData->SignatureData, SIZE_OF_QA_TEST_KEY, Digest);
      if (CompareMem (Digest, mQACertificateHashData, SHA256_DIGEST_SIZE) == 0) {
        FreePool (VarBuffer);
        return TRUE;
      }
    }
    Offset += SignatureList->SignatureListSize;
  }

  if (VarBuffer != NULL) {
    FreePool (VarBuffer);
  }
  return FALSE;
}

/**
  Check if there is a QA test key. QA test key cannot be used in both FFS and factory copy default.

  @retval TRUE   - found the QA test certificate
  @retval FALSE  - did not found the QA test certificate

**/
BOOLEAN
CheckCertificate (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINT8                         *FileBuffer;
  UINTN                         FileSize;
  UINT8                         Digest[SHA256_DIGEST_SIZE];
  EFI_SIGNATURE_LIST            *SignatureList;
  EFI_SIGNATURE_DATA            *SignatureData;
  UINTN                         Offset;

  if (CheckCertFromFactoryCopy ()) {
    return TRUE;
  }
  //
  // Check the certificate from FFS
  //
  FileBuffer = NULL;
  Status = GetSectionFromAnyFv (PcdGetPtr (PcdSecureFlashCertificateFile), EFI_SECTION_RAW, 0, (VOID **)&FileBuffer, &FileSize);
  if (FileBuffer == NULL) {
    return FALSE;
  }

  if (PcdGetBool (PcdH2OCapsuleSignatureDatabaseSupported)) {
    for (Offset = 0; Offset + sizeof (EFI_SIGNATURE_LIST) < FileSize; Offset += SignatureList->SignatureListSize) {
      SignatureList = (EFI_SIGNATURE_LIST *) (FileBuffer + Offset);
      SignatureData = (EFI_SIGNATURE_DATA *) ((UINT8 *)SignatureList + sizeof (EFI_SIGNATURE_LIST));
      if (SignatureList->SignatureListSize  < sizeof (EFI_SIGNATURE_LIST)) {
        break;
      }
      if ((SignatureList->SignatureSize - sizeof (EFI_GUID)) != SIZE_OF_QA_TEST_KEY) {
        continue;
      }
      Status = CalculateHash (SignatureData->SignatureData, SIZE_OF_QA_TEST_KEY, Digest);
      if (CompareMem (Digest, mQACertificateHashData, SHA256_DIGEST_SIZE) == 0) {
        FreePool (FileBuffer);
        return TRUE;
      }
    }
  } else {
    if (FileSize == SIZE_OF_QA_TEST_KEY) {
      Status = CalculateHash (FileBuffer, FileSize, Digest);
      if (CompareMem (Digest, mQACertificateHashData, SHA256_DIGEST_SIZE) == 0) {
        FreePool (FileBuffer);
        return TRUE;
      }
    }
  }
  FreePool (FileBuffer);
  return FALSE;
}

/**
 Check if BDS hot key is detected or not.

 @retval TRUE   - BDS hot key is detected
 @retval FALSE  - BDS hot key is not detected yet.
**/
STATIC
BOOLEAN
OemGraphicsIsHotKeyDetected (
  VOID
  )
{
  EFI_STATUS                                  Status;
  H2O_BDS_CP_DISPLAY_STRING_BEFORE_DATA       *BdsDisplayStringBeforeData;

  Status = gBS->LocateProtocol (&gH2OBdsCpDisplayStringBeforeGuid, NULL, (VOID **) &BdsDisplayStringBeforeData);
  if (!EFI_ERROR (Status)) {
    return BdsDisplayStringBeforeData->AfterSelect;
  }

  return FALSE;
}

/**
  Check if ESRT Firmware Guid is the default CRB firmware guid

  @retval TRUE   - ESRT Firmware Guid is the default CRB firmware guid
  @retval FALSE  - ESRT Firmware Guid is not the default CRB firmware guid

**/
BOOLEAN
IsEsrtCrbFirmwareGuid (
  VOID
){

  EFI_STATUS                    Status;
  EFI_SYSTEM_RESOURCE_TABLE     *Esrt;
  UINT8                         Index;

  Status = EfiGetSystemConfigurationTable (&gEfiSystemResourceTableGuid, (VOID **)&Esrt);
  if (EFI_ERROR(Status) || Esrt == NULL){
    return FALSE;
  }

  for (Index = 0; Index < Esrt->FirmwareResourceCount; Index++) {
    if (CompareGuid (&Esrt->FirmwareResources[Index].FirmwareClass, &gEsrtCrbFirmwareGuid)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  Check if need to replace logo image by warning image.

  @retval TRUE           Need to replace logo image by warning image.
  @retval FALSE          No need to replace logo image by warning image.
**/
BOOLEAN
IsReplacedByWarningLogo (
  VOID
  )
{
  if (PcdGetBool(PcdH2OWarningLogoSupported) && (IsEsrtCrbFirmwareGuid () || CheckCertificate ())) {
    return TRUE;
  }

  return FALSE;
}

/**
  Check if the BGRT feature is enabled or not.

  @retval TRUE           BGRT feature is enabled.
  @retval FALSE          BGRT feature is disabled.
**/
STATIC
BOOLEAN
IsBgrtEnabled (
  VOID
  )
{
  EFI_BOOT_LOGO_PROTOCOL                     *BootLogo;
  EFI_STATUS                                 Status;

  Status = gBS->LocateProtocol (&gEfiBootLogoProtocolGuid, NULL, (VOID **) &BootLogo);
  return (!EFI_ERROR (Status)) ? TRUE : FALSE;
}

/**
  Get corresponding badging support format value.

  @param[in] Format            Logo file format

  @return The corresponding badging support format value or unknown value if not found.
**/
STATIC
EFI_BADGING_SUPPORT_FORMAT
GetBadgingSupportFormat (
  IN UINT8               Format
  )
{
  switch (Format) {

  case H2O_FLASH_MAP_REGION_LOGO_FORMAT_BMP:
    return EfiBadgingSupportFormatBMP;

  case H2O_FLASH_MAP_REGION_LOGO_FORMAT_JPEG:
    return EfiBadgingSupportFormatJPEG;

  case H2O_FLASH_MAP_REGION_LOGO_FORMAT_GIF:
    return EfiBadgingSupportFormatGIF;

  case H2O_FLASH_MAP_REGION_LOGO_FORMAT_PNG:
  default:
    return EfiBadgingSupportFormatUnknown;
  }
}

/**
  Using the current display resolution and image size, calculate the image display coordinates.

  @param[in]  ImageWidth        Image width
  @param[in]  ImageHeight       Image height
  @param[in]  PosX              The image position X.
  @param[in]  PosX              The image position Y.
  @param[in]  OriginX           Logo image original position X
  @param[in]  OriginY           Logo image original position Y
  @param[out] CoordinateX       The image destination coordinate X.
  @param[out] CoordinateY       The image destination coordinate Y.

  @retval EFI_SUCCESS           Get image coordinate successfully.
  @retval EFI_INVALID_PARAMETER OriginX or OriginY value is invalid.
  @retval Other                 Failed to get GOP protocol instance
**/
STATIC
EFI_STATUS
GetLogoCoordinate (
  IN   UINTN                                 ImageWidth,
  IN   UINTN                                 ImageHeight,
  IN   INT16                                 PosX,
  IN   INT16                                 PosY,
  IN   UINT8                                 OriginX,
  IN   UINT8                                 OriginY,
  OUT  UINTN                                 *CoordinateX,
  OUT  UINTN                                 *CoordinateY
  )
{
  EFI_STATUS                                 Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL               *GraphicsOutput;
  UINTN                                      ResolutionX;
  UINTN                                      ResolutionY;


  *CoordinateX = 0;
  *CoordinateY = 0;

  Status = GetBootGop (&GraphicsOutput);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ResolutionX = GraphicsOutput->Mode->Info->HorizontalResolution;
  ResolutionY = GraphicsOutput->Mode->Info->VerticalResolution;

  switch (OriginX) {

  case H2O_FLASH_MAP_REGION_LOGO_ORIGIN_X_LEFT:
    *CoordinateX = 0;
    break;

  case H2O_FLASH_MAP_REGION_LOGO_ORIGIN_X_RIGHT:
    *CoordinateX = ResolutionX - ImageWidth;
    break;

  case H2O_FLASH_MAP_REGION_LOGO_ORIGIN_X_CENTER:
    *CoordinateX = (ResolutionX - ImageWidth) / 2;
    break;

  default:
    return EFI_INVALID_PARAMETER;
  }

  switch (OriginY) {

  case H2O_FLASH_MAP_REGION_LOGO_ORIGIN_Y_TOP:
    *CoordinateY = 0;
    break;

  case H2O_FLASH_MAP_REGION_LOGO_ORIGIN_Y_BOTTOM:
    *CoordinateY = ResolutionY - ImageHeight;
    break;

  case H2O_FLASH_MAP_REGION_LOGO_ORIGIN_Y_CENTER:
    *CoordinateY = (ResolutionY - ImageHeight) / 2;
    break;

  default:
    return EFI_INVALID_PARAMETER;
  }

  *CoordinateX = *CoordinateX + (UINTN) PosX;
  *CoordinateY = *CoordinateY + (UINTN) PosY;
  return EFI_SUCCESS;
}

/**
  Add logo info into list.

  @param[in]      ImageData     Pointer to the logo image
  @param[in]      ImageSize     Logo image size
  @param[in]      PosX          Logo image position X
  @param[in]      PosY          Logo image position Y
  @param[in]      OriginX       Logo image original position X
  @param[in]      OriginY       Logo image original position Y
  @param[in]      Format        Logo file format
  @param[in]      Priority      Specifies the relative order of display for logos.
  @param[in, out] InfoList      Double pointer to the logo info list
  @param[in, out] InfoCount     Pointer to the logo info count

  @retval EFI_SUCCESS           Add logo info into list successfully.
  @retval EFI_OUT_OF_RESOURCES  Unable to allocate memory for logo info.
  @retval Other                 Failed to decode image or get coordinate position.
**/
STATIC
EFI_STATUS
AddLogoInfo (
  IN     UINT8                              *ImageData,
  IN     UINTN                              ImageSize,
  IN     INT16                              PosX,
  IN     INT16                              PosY,
  IN     UINT8                              OriginX,
  IN     UINT8                              OriginY,
  IN     UINT8                              Format,
  IN     INT8                               Priority,
  IN OUT LOGO_INFO_DATA                     **InfoList,
  IN OUT UINTN                              *InfoCount
  )
{
  EFI_STATUS                                Status;
  UINT8                                     *Blt;
  UINTN                                     ImageWidth;
  UINTN                                     ImageHeight;
  UINTN                                     DestinationX;
  UINTN                                     DestinationY;
  UINTN                                     Count;
  LOGO_INFO_DATA                            *NewInfoList;
  UINTN                                     Index;

  Blt = NULL;
  Status = BgrtDecodeImageToBlt (
             ImageData,
             ImageSize,
             GetBadgingSupportFormat (Format),
             &Blt,
             &ImageWidth,
             &ImageHeight
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = GetLogoCoordinate (
             ImageWidth,
             ImageHeight,
             PosX,
             PosY,
             OriginX,
             OriginY,
             &DestinationX,
             &DestinationY
             );
  if (EFI_ERROR (Status)) {
    FreePool (Blt);
    return Status;
  }

  Count       = *InfoCount;
  NewInfoList = ReallocatePool (
                  sizeof (LOGO_INFO_DATA) * Count,
                  sizeof (LOGO_INFO_DATA) * (Count + 1),
                  *InfoList
                  );
  if (NewInfoList == NULL) {
    FreePool (Blt);
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < Count; Index++) {
    if (Priority > NewInfoList[Index].Priority) {
      break;
    }
  }

  CopyMem (&NewInfoList[Index + 1], &NewInfoList[Index], sizeof (LOGO_INFO_DATA) * (Count - Index));

  NewInfoList[Index].Blt          = Blt;
  NewInfoList[Index].ImageWidth   = ImageWidth;
  NewInfoList[Index].ImageHeight  = ImageHeight;
  NewInfoList[Index].DestinationX = DestinationX;
  NewInfoList[Index].DestinationY = DestinationY;
  NewInfoList[Index].Priority     = Priority;

  *InfoList  = NewInfoList;
  *InfoCount = Count + 1;
  return EFI_SUCCESS;
}

/**
  Enumerate all logo images from FDM.

  @param[out] LogoInfoList           Double pointer to the logo info list
  @param[out] LogoInfoCount          Pointer to the count of logo info list

  @retval EFI_SUCCESS                Enumerate all logo images from FDM successfully.
  @retval EFI_NOT_FOUND              No available logo in FDM.
  @retval Other                      Failed to get logo region type from FDM.
**/
STATIC
EFI_STATUS
EnumerateAllBootLogoFromFdm (
  OUT LOGO_INFO_DATA               **LogoInfoList,
  OUT UINTN                        *LogoInfoCount
  )
{
  EFI_STATUS                       Status;
  H2O_FLASH_MAP_REGION_LOGO_ID     *LogoRegionId;
  UINT8                            LogoRegionCount;
  UINT8                            BoardId;
  UINT8                            LogoIndex;
  UINT8                            RegionId[FDM_ENTRY_REGION_ID_SIZE];
  UINT64                           RegionOffset;
  UINT64                           RegionSize;
  UINT32                           RegionAttr;
  UINT8                            OriginX;
  UINT8                            OriginY;
  UINT8                            Format;
  UINT8                            LogoId;
  UINT32                           Index;
  INT16                            PosX;
  INT16                            PosY;
  UINT8                            *ImageData;
  UINTN                            ImageSize;
  BOOLEAN                          ReplaceFirstLogo;
  LOGO_INFO_DATA                   *InfoList;
  UINTN                            InfoCount;

  Status = FdmGetNCount (&gH2OFlashMapRegionLogoGuid, &LogoRegionCount);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  BoardId          = PcdGet8(PcdH2OBoardId);
  InfoList         = NULL;
  InfoCount        = 0;
  ReplaceFirstLogo = (BOOLEAN) (IsBgrtEnabled() && IsReplacedByWarningLogo());

  //
  // Get all logo images by enumerating flash device map.
  //
  for (LogoIndex = 1; LogoIndex <= LogoRegionCount; LogoIndex++) {
    Status = FdmGetNAt (&gH2OFlashMapRegionLogoGuid, LogoIndex, RegionId, &RegionOffset, &RegionSize, &RegionAttr);
    if (EFI_ERROR (Status)) {
      continue;
    }

    LogoRegionId = (H2O_FLASH_MAP_REGION_LOGO_ID *) RegionId;
    LogoId       = LogoRegionId->Id;
    if (LogoId != H2O_FLASH_MAP_REGION_LOGO_ID_BOOT) {
      continue;
    }

    for (Index = 0; Index < SUPPORT_BOARD_COUNT; Index++) {
      if (LogoRegionId->Boards[Index] == H2O_FLASH_MAP_REGION_LOGO_BOARDS_END) {
        Index = SUPPORT_BOARD_COUNT;
        break;
      }
      if (LogoRegionId->Boards[Index] == 0 ||
          LogoRegionId->Boards[Index] == BoardId) {
        break;
      }
    }
    if (Index == SUPPORT_BOARD_COUNT) {
      continue;
    }

    Status = GetLogoPosXY (LogoRegionId, &PosX, &PosY);
    if (EFI_ERROR(Status)) {
      continue;
    }
    OriginX   = LogoRegionId->Flags & H2O_FLASH_MAP_REGION_LOGO_ORIGIN_X_MASK;
    OriginY   = LogoRegionId->Flags & H2O_FLASH_MAP_REGION_LOGO_ORIGIN_Y_MASK;
    Format    = LogoRegionId->Flags & H2O_FLASH_MAP_REGION_LOGO_FORMAT_MASK;
    ImageData = ((UINT8 *)(UINTN) FdmGetBaseAddr()) + RegionOffset;
    ImageSize = (UINTN) RegionSize;

    if (ReplaceFirstLogo) {
      ReplaceFirstLogo = FALSE;
      //
      // Get warning logo from FV.
      //
      Status = GetSectionFromAnyFv ((EFI_GUID *) PcdGetPtr (PcdWarningLogoFile), EFI_SECTION_RAW, 0, (VOID **)&ImageData, &ImageSize);
      if (!EFI_ERROR(Status)) {
        Format = H2O_FLASH_MAP_REGION_LOGO_FORMAT_JPEG;
      }
    }

    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcUpdateLogo \n"));
    Status = OemSvcUpdateLogo (
               &LogoId,
               &PosX,
               &PosY,
               &OriginX,
               &OriginY,
               &Format,
               &ImageData,
               (UINT32 *) &ImageSize
               );
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcUpdateLogo Status: %r\n", Status));
    if (Status == EFI_SUCCESS || LogoId != H2O_FLASH_MAP_REGION_LOGO_ID_BOOT) {
      continue;
    }

    AddLogoInfo (
      ImageData,
      ImageSize,
      PosX,
      PosY,
      OriginX,
      OriginY,
      Format,
      LogoRegionId->Priority,
      &InfoList,
      &InfoCount
      );
  }

  //
  // Find all boot logos from OemSvcGetLogo()
  //
  Index = 0;
  while (TRUE) {
    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcGetLogo \n"));
    Status = OemSvcGetLogo (
               H2O_FLASH_MAP_REGION_LOGO_ID_BOOT,
               Index,
               &PosX,
               &PosY,
               &OriginX,
               &OriginY,
               &Format,
               &ImageData,
               (UINT32 *) &ImageSize
               );
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcGetLogo Status: %r\n", Status));
    if (Status == EFI_UNSUPPORTED) {
      break;
    }

    Index++;
    if (Status == EFI_SUCCESS) {
      continue;
    }
    if (Status != EFI_MEDIA_CHANGED) {
      break;
    }

    AddLogoInfo (
      ImageData,
      ImageSize,
      PosX,
      PosY,
      OriginX,
      OriginY,
      Format,
      LOWEST_LOGO_PRIORITY,
      &InfoList,
      &InfoCount
      );
  }

  if (InfoCount == 0) {
    return EFI_NOT_FOUND;
  }

  *LogoInfoList  = InfoList;
  *LogoInfoCount = InfoCount;
  return EFI_SUCCESS;
}

/**
  Get logo image from FDM.

  @param[out] BltBuffer         Pointer to the blt buffer of logo image
  @param[out] DestinationX      The image destination coordinate X.
  @param[out] DestinationY      The image destination coordinate Y.
  @param[out] Width             The image width size by pixel.
  @param[out] Height            The image height size by pixel.

  @retval EFI_SUCCESS           Get logo image from FDM successfully.
  @retval EFI_OUT_OF_RESOURCES  Unable to allocate memory for BLT buffer.
  @retval Other                 Failed to get logo info from FDM, failed to locate GOP protocol, or
                                blt function return fail.
**/
EFI_STATUS
GetLogoImageFromFdm (
  OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL     **BltBuffer,
  OUT UINTN                             *DestinationX,
  OUT UINTN                             *DestinationY,
  OUT UINTN                             *Width,
  OUT UINTN                             *Height
  )
{
  EFI_STATUS                            Status;
  LOGO_INFO_DATA                        *LogoInfoList;
  UINTN                                 LogoInfoCount;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL         *NewBltBuffer;
  UINTN                                 Index;
  UINTN                                 StartX;
  UINTN                                 StartY;
  UINTN                                 EndX;
  UINTN                                 EndY;
  UINTN                                 ImageWidth;
  UINTN                                 ImageHeight;
  UINTN                                 DestX;
  UINTN                                 DestY;
  EFI_GRAPHICS_OUTPUT_PROTOCOL          *GraphicsOutput;
  UINTN                                 Line;

  LogoInfoList  = NULL;
  LogoInfoCount = 0;

  Status = EnumerateAllBootLogoFromFdm (&LogoInfoList, &LogoInfoCount);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (LogoInfoCount == 1) {
    *DestinationX = LogoInfoList[0].DestinationX;
    *DestinationY = LogoInfoList[0].DestinationY;
    *Width        = LogoInfoList[0].ImageWidth;
    *Height       = LogoInfoList[0].ImageHeight;
    *BltBuffer    = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) LogoInfoList[0].Blt;
    LogoInfoList[0].Blt = NULL;
    goto Done;
  }

  //
  // Merge multiple logo images into BLT buffer
  //
  StartX = (UINTN) -1;
  StartY = (UINTN) -1;
  EndX   = 0;
  EndY   = 0;
  for (Index = 0; Index < LogoInfoCount; Index++) {
    StartX = MIN (LogoInfoList[Index].DestinationX, StartX);
    StartY = MIN (LogoInfoList[Index].DestinationY, StartY);
    EndX   = MAX ((LogoInfoList[Index].DestinationX + LogoInfoList[Index].ImageWidth) , EndX);
    EndY   = MAX ((LogoInfoList[Index].DestinationY + LogoInfoList[Index].ImageHeight), EndY);
  }
  DestX       = StartX;
  DestY       = StartY;
  ImageWidth  = EndX - StartX;
  ImageHeight = EndY - StartY;

  Status = GetBootGop (&GraphicsOutput);
  if (EFI_ERROR(Status)) {
    goto Done;
  }

  NewBltBuffer = AllocateZeroPool ((ImageWidth * ImageHeight) * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  if (NewBltBuffer == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }

  Status = GraphicsOutput->Blt (
                             GraphicsOutput,
                             NewBltBuffer,
                             EfiBltVideoToBltBuffer,
                             DestX,
                             DestY,
                             0,
                             0,
                             ImageWidth,
                             ImageHeight,
                             ImageWidth * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                             );
  if (EFI_ERROR(Status)) {
    goto Done;
  }

  for (Index = 0; Index < LogoInfoCount; Index++) {
    StartX  = LogoInfoList[Index].DestinationX - DestX;
    StartY  = LogoInfoList[Index].DestinationY - DestY;

    for (Line = 0; Line < LogoInfoList[Index].ImageHeight; Line++) {
      CopyMem (
        NewBltBuffer + (ImageWidth * (StartY + Line) + StartX),
        LogoInfoList[Index].Blt + LogoInfoList[Index].ImageWidth * Line * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL),
        LogoInfoList[Index].ImageWidth * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
        );
    }
  }

  *BltBuffer    = NewBltBuffer;
  *DestinationX = DestX;
  *DestinationY = DestY;
  *Width        = ImageWidth;
  *Height       = ImageHeight;

Done:
  if (LogoInfoList != NULL) {
    for (Index = 0; Index < LogoInfoCount; Index++) {
      if (LogoInfoList[Index].Blt != NULL) {
        FreePool (LogoInfoList[Index].Blt);
      }
    }
    FreePool (LogoInfoList);
  }
  return Status;
}

VOID
BgrtGetLogoCoordinate (
  IN   UINTN      ResolutionX,
  IN   UINTN      ResolutionY,
  IN   UINTN      ImageWidth,
  IN   UINTN      ImageHeight,
  OUT  UINTN      *CoordinateX,
  OUT  UINTN      *CoordinateY
  )
{
  if (CoordinateX == NULL || CoordinateY == NULL) {
    return;
  }

  //
  // According to BGRT requirement, the logo must place at 38.2% from top of screen
  // and the width/height should be less than 40% of screen.
  //
  if ((ImageWidth >= ((ResolutionX * 40) / 100)) || (ImageHeight >= ((ResolutionY * 40) / 100))) {
    DEBUG ((EFI_D_ERROR, "Warning! The logo location did not meet BGRT requirement.\n"));
  }  //
  // Current setting: Image on the middle of the screen, and 38.2% from top.
  //
  *CoordinateX = (UINTN)(ResolutionX - ImageWidth) / 2;
  *CoordinateY = (UINTN)((ResolutionY * 382 / 1000) - (ImageHeight / 2));
}

/**
  According to current resolution a image size to get the image destination coordinate.

  @param[in]  ImageWidth        The image width size by pixel.
  @param[in]  ImageHeight       The image height size by pixel.
  @param[out] CoordinateX       The image destination coordinate X.
  @param[out] CoordinateY       The image destination coordinate Y.

  @retval EFI_SUCCESS           Get image coordinate successfully.
  @retval EFI_NOT_FOUND         Cannot find gEfiGraphicsOutputProtocolGuid protocol.
**/
STATIC
EFI_STATUS
InternalBgrtGetLogoCoordinate (
  IN   UINTN      ImageWidth,
  IN   UINTN      ImageHeight,
  OUT  UINTN      *CoordinateX,
  OUT  UINTN      *CoordinateY
  )
{
  EFI_STATUS                       Status;
  UINTN                            ResolutionX;
  UINTN                            ResolutionY;
  EFI_GRAPHICS_OUTPUT_PROTOCOL     *GraphicsOutput;

  Status = GetBootGop (&GraphicsOutput);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ResolutionX  = GraphicsOutput->Mode->Info->HorizontalResolution;
  ResolutionY  = GraphicsOutput->Mode->Info->VerticalResolution;

  BgrtGetLogoCoordinate (
    GraphicsOutput->Mode->Info->HorizontalResolution,
    GraphicsOutput->Mode->Info->VerticalResolution,
    ImageWidth,
    ImageHeight,
    CoordinateX,
    CoordinateY
    );
  return EFI_SUCCESS;
}

VOID
EFIAPI
BgrtClearOemStringReadyToBootEvent (
  IN EFI_EVENT                Event,
  IN VOID                     *Context
  )
{
  EFI_OEM_BADGING_SUPPORT_PROTOCOL  *Badging;
  EFI_GRAPHICS_OUTPUT_PROTOCOL      *GraphicsOutput;
  UINTN                             StringCount;
  UINTN                             Index;
  EFI_STATUS                        Status;
  UINTN                             LocX;
  UINTN                             LocY;
  EFI_UGA_PIXEL                     Foreground;
  EFI_UGA_PIXEL                     Background;
  CHAR16                            *StringData;
  UINTN                             MinLocY;
  UINTN                             MaxLocY;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL     Blt;
  BOOLEAN                           Found;

  MinLocY = 0xFFFFFFFF;
  MaxLocY = 0;
  Found   = FALSE;

  gBS->CloseEvent (Event);

  Status  = gBS->LocateProtocol (&gEfiOEMBadgingSupportProtocolGuid, NULL, (VOID **)&Badging);
  if (EFI_ERROR (Status)) {
    return;
  }

  Status = GetBootGop (&GraphicsOutput);
  if (EFI_ERROR (Status)) {
    return;
  }

  Badging->GetStringCount (Badging, &StringCount);

  if (StringCount) {
    for (Index = 0; Index < StringCount; Index++) {
      if (Badging->GetOemString (Badging, Index, FALSE, 0, &StringData, &LocX, &LocY, &Foreground, &Background)) {
        Found = TRUE;
        FreePool (StringData);
        if (LocX == (UINTN) (-1) || LocY == (UINTN) (-1)) {
          continue;
        }
        if (MinLocY > LocY) {
          MinLocY = LocY;
        }
        if (LocY > MaxLocY) {
          MaxLocY = LocY;
        }
      }
    }

    if (!Found) {
      return;
    }

    SetMem (&Blt, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0x00);
    //
    // First clear string which position is defined by OEM, then clear auto place strings.
    //
    if (MinLocY != 0xFFFFFFFF) {
      Status = GraphicsOutput->Blt (
                 GraphicsOutput,
                 &Blt,
                 EfiBltVideoFill,
                 0,
                 0,
                 (UINTN) 0,
                 (UINTN) MinLocY,
                 GraphicsOutput->Mode->Info->HorizontalResolution,
                 //
                 // GLYPH_HEIGHT is 19.
                 //
                 (MaxLocY - MinLocY + 20),
                 0
                 );
    }
    if (mAutoPlaceStrCount != 0 && mAutoPlaceStrDestY != (UINTN) (-1)) {
      Status = GraphicsOutput->Blt (
                                 GraphicsOutput,
                                 &Blt,
                                 EfiBltVideoFill,
                                 0,
                                 0,
                                 (UINTN) 0,
                                 (UINTN) mAutoPlaceStrDestY,
                                 GraphicsOutput->Mode->Info->HorizontalResolution,
                                 mAutoPlaceStrCount * EFI_GLYPH_HEIGHT,
                                 0
                                 );
      mAutoPlaceStrCount = 0;
      mAutoPlaceStrDestY = (UINTN) (-1);
    }
  }
  return;
}


/**
  Get BGRT image information from firmware volume.

  @param[out] BltBuffer         BGRT image blt.
  @param[out] DestinationX      The image destination coordinate X.
  @param[out] DestinationY      The image destination coordinate Y.
  @param[out] Width             The image width size by pixel.
  @param[out] Height            The image height size by pixel.

  @retval EFI_SUCCESS           Get BGRT image information successfully.
  @retval EFI_INVALID_PARAMETER Any input parameter is invalid.
  @retval EFI_OUT_OF_RESOURCES  Unable to allocate memory for BLT buffer.
  @retval EFI_NOT_FOUND         Cannot find BGRT image.
  @retval EFI_UNSUPPORTED       Doesn't support output image coordinate.
**/
STATIC
EFI_STATUS
BgrtGetBootImageInfoFromFv (
  OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL     **BltBuffer,
  OUT UINTN                             *DestinationX,
  OUT UINTN                             *DestinationY,
  OUT UINTN                             *Width,
  OUT UINTN                             *Height
  )
{
  EFI_STATUS                      Status;
  UINT8                           *ImageData;
  UINTN                           ImageSize;
  EFI_BADGING_SUPPORT_FORMAT      BadgingImageFormat;
  UINTN                           ImageWidth;
  UINTN                           ImageHeight;
  UINT8                           *Blt;

  if (BltBuffer == NULL || DestinationX == NULL || DestinationY == NULL ||
      Width == NULL || Height == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Blt = NULL;
  if (PcdGetBool (PcdH2OBdsOemBadgingSupported)) {
    Status = BgrtGetImageByBadging (&ImageData, &ImageSize, &BadgingImageFormat);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = BgrtDecodeImageToBlt (
               ImageData,
               ImageSize,
               BadgingImageFormat,
               &Blt,
               &ImageWidth,
               &ImageHeight
               );
    FreePool (ImageData);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    Status = InternalBgrtGetLogoCoordinate (ImageWidth, ImageHeight, DestinationX, DestinationY);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  } else {
    Status = GetLogoImageFromFdm ((EFI_GRAPHICS_OUTPUT_BLT_PIXEL  **) &Blt, DestinationX, DestinationY, &ImageWidth, &ImageHeight);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }
  if (((*(INTN*)DestinationX) < 0) || ((*(INTN*)DestinationY) < 0)) {
    return EFI_UNSUPPORTED;
  }

  *BltBuffer = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL*) Blt;
  *Width     = ImageWidth;
  *Height    = ImageHeight;
  return Status;
}

/**
  Get BGRT image information from input image

  @param[in]  Image             Input EFI_IMAGE_INPUT instance.
  @param[out] BltBuffer         BGRT image blt.
  @param[out] DestinationX      The image destination coordinate X.
  @param[out] DestinationY      The image destination coordinate Y.
  @param[out] Width             The image width size by pixel.
  @param[out] Height            The image height size by pixel.

  @retval EFI_SUCCESS           Get BGRT image information successfully.
  @retval EFI_INVALID_PARAMETER Any input parameter is invalid.
  @retval EFI_OUT_OF_RESOURCES  Unable to allocate memory for BLT buffer.
  @retval EFI_UNSUPPORTED       Doesn't support output image coordinate.
**/
STATIC
EFI_STATUS
BgrtGetBootImageInfoFromImageBuffer (
  IN  EFI_IMAGE_INPUT                   *Image,
  OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL     **BltBuffer,
  OUT UINTN                             *DestinationX,
  OUT UINTN                             *DestinationY,
  OUT UINTN                             *Width,
  OUT UINTN                             *Height
  )
{
  EFI_STATUS                      Status;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *Blt;
  UINTN                           BltBufferSize;

  if (Image == NULL || Image->Bitmap == NULL || Image->Width == 0 || Image->Height == 0 ||
      BltBuffer == NULL || DestinationX == NULL || DestinationY == NULL || Width == NULL || Height == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  BltBufferSize = (UINTN)Image->Width * Image->Height * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
  Blt = AllocatePool (BltBufferSize);
  if (Blt == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CopyMem (Blt, Image->Bitmap, BltBufferSize);


  Status = InternalBgrtGetLogoCoordinate ((UINTN) Image->Width, (UINTN) Image->Height, DestinationX, DestinationY);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (((*(INTN*) DestinationX) < 0) || ((*(INTN*) DestinationY) < 0)) {
    return EFI_UNSUPPORTED;
  }

  *BltBuffer = Blt;
  *Width     = (UINTN) Image->Width;
  *Height    = (UINTN) Image->Height;
  return Status;
}

/**
  Get BGRT image information

  @param[out] BltBuffer         BGRT image blt.
  @param[out] DestinationX      The image destination coordinate X.
  @param[out] DestinationY      The image destination coordinate Y.
  @param[out] Width             The image width size by pixel.
  @param[out] Height            The image height size by pixel.

  @retval EFI_SUCCESS           Get BGRT image information successfully.
  @retval EFI_INVALID_PARAMETER Any input parameter is invalid.
  @retval EFI_OUT_OF_RESOURCES  Unable to allocate memory for BLT buffer.
  @retval EFI_NOT_FOUND         Cannot find BGRT image.
  @retval EFI_UNSUPPORTED       Doesn't support output image coordinate.
**/
EFI_STATUS
BgrtGetBootImageInfo (
  OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL     **BltBuffer,
  OUT UINTN                             *DestinationX,
  OUT UINTN                             *DestinationY,
  OUT UINTN                             *Width,
  OUT UINTN                             *Height
  )
{
  EFI_STATUS                           Status;
  H2O_BDS_CP_DISPLAY_BEFORE_DATA      *BdsDisplayBeforData;

  Status = gBS->LocateProtocol (
                  &gH2OBdsCpDisplayBeforeGuid,
                  NULL,
                  (VOID **) &BdsDisplayBeforData
                  );
  if (BdsDisplayBeforData != NULL && (BdsDisplayBeforData->Features & H2O_BDS_CP_DISPLAY_BGRT) != 0 &&
      BdsDisplayBeforData->Image != NULL) {
    Status = BgrtGetBootImageInfoFromImageBuffer (
               BdsDisplayBeforData->Image,
               BltBuffer,
               DestinationX,
               DestinationY,
               Width,
               Height
               );
  } else {
    Status = BgrtGetBootImageInfoFromFv (
               BltBuffer,
               DestinationX,
               DestinationY,
               Width,
               Height
               );
  }

  return Status;
}

EFI_STATUS
EFIAPI
BgrtEnableQuietBoot (
  EFI_BOOT_LOGO_PROTOCOL              *SetBootLogo
  )
{
  EFI_STATUS                           Status;
  EFI_EVENT                            ReadyToBootEvent;
  EFI_GRAPHICS_OUTPUT_PROTOCOL         *GraphicsOutput;
  EFI_CONSOLE_CONTROL_PROTOCOL         *ConsoleControl;
  EFI_OEM_BADGING_SUPPORT_PROTOCOL     *Badging;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL        *BltBuffer;
  UINTN                                DestinationX;
  UINTN                                DestinationY;
  UINTN                                Width;
  UINTN                                Height;
  EFI_STATUS                           ShowStringStatus;
  H2O_BDS_CP_DISPLAY_BEFORE_DATA      *BdsDisplayBeforData;

  BltBuffer    = NULL;
  DestinationX = 0;
  DestinationY = 0;
  Width        = 0;
  Height       = 0;

  Status = gBS->LocateProtocol (
                  &gH2OBdsCpDisplayBeforeGuid,
                  NULL,
                  (VOID **) &BdsDisplayBeforData
                  );
  if (!EFI_ERROR (Status) && BdsDisplayBeforData->Status == H2O_BDS_TASK_SKIP) {
    return EFI_SUCCESS;
  }

  if (SetBootLogo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // The procedure of BgrtEnableQuietBoot:
  //
  // 1. Get Boot image by OemBadgingSupport with Type == EfiBadgingSupportImageBoot.
  // 2. Update Boot image to BGRT table.
  // 3. Show logo. (According to the bgrt spec, the background always black)
  // 4. Show OEM String.
  // 5. Clear OEM String by ReadyToBoot Event.
  //
  Status = BgrtGetBootImageInfo (
             &BltBuffer,
             &DestinationX,
             &DestinationY,
             &Width,
             &Height
             );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // BIOS show logo.
  //
  Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, (VOID **)&ConsoleControl);
  if (!EFI_ERROR (Status)) {
    //
    // switch to the Graphical mode.
    //
    ConsoleControl->SetMode (ConsoleControl, EfiConsoleControlScreenGraphics);
  } else {
    gST->ConOut->ClearScreen (gST->ConOut);
  }

  //
  // Erase Cursor from screen
  //
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);

  Status = GetBootGop (&GraphicsOutput);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = GraphicsOutput->Blt (
                             GraphicsOutput,
                             BltBuffer,
                             EfiBltBufferToVideo,
                             0,
                             0,
                             DestinationX,
                             DestinationY,
                             Width,
                             Height,
                             Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                             );
  ASSERT_EFI_ERROR (Status);
  Status = SetBootLogo->SetBootLogo (
                          SetBootLogo,
                          BltBuffer,
                          DestinationX,
                          DestinationY,
                          Width,
                          Height
                          );
  ASSERT_EFI_ERROR (Status);
  //
  // Y vaule of auto place OEM badging string starts at y value of image bottom plus 2 glyph heigh.
  //
  mAutoPlaceStrDestY = DestinationY + Height + 2 * EFI_GLYPH_HEIGHT;
  //
  // show OemString.
  //
  Status  = gBS->LocateProtocol (&gEfiOEMBadgingSupportProtocolGuid, NULL, (VOID **)&Badging);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  ShowStringStatus = ShowOemString (Badging, OemGraphicsIsHotKeyDetected (), 0);

  if (EFI_ERROR (ShowStringStatus)) {
    if (BltBuffer != NULL) {
      FreePool (BltBuffer);
    }
    return Status;
  }
  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             BgrtClearOemStringReadyToBootEvent,
             NULL,
             &ReadyToBootEvent
             );

  if (EFI_ERROR (Status)) {
    gBS->CloseEvent (ReadyToBootEvent);
  }

  if (BltBuffer != NULL) {
    FreePool (BltBuffer);
  }

  return Status;
}

EFI_STATUS
LockKeyboards (
  IN  CHAR16    *Password
  )
{
  EFI_STATUS                    Status;
  EFI_CONSOLE_CONTROL_PROTOCOL  *ConsoleControl;

  Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, (VOID **)&ConsoleControl);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  Status = ConsoleControl->LockStdIn (ConsoleControl, Password);
  return Status;
}

STATIC
EFI_STATUS
DiaplayMessage(
  IN     UINTN          PosX,
  IN     UINTN          PosY,
  IN     EFI_UGA_PIXEL  ForegroundRGB,
  IN     EFI_UGA_PIXEL  BackgroundRGB,
  IN     CHAR16         *TmpStr
  )
{
  EFI_STATUS                    Status;
  EFI_UGA_DRAW_PROTOCOL         *UgaDraw;
  UINT32                        SizeOfX;
  UINT32                        SizeOfY;
  UINT32                        ColorDepth;
  UINT32                        RefreshRate;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL Foreground;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL Background;
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput;

  UgaDraw = NULL;
  GraphicsOutput = NULL;
  CopyMem (&Foreground, &ForegroundRGB, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  CopyMem (&Background, &BackgroundRGB, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

  Status = GetBootGop (&GraphicsOutput);
  if (EFI_ERROR (Status)) {
    GraphicsOutput = NULL;
    Status = gBS->HandleProtocol (
                    gST->ConsoleOutHandle,
                    &gEfiUgaDrawProtocolGuid,
                    (VOID **)&UgaDraw
                    );
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }
  }

  if (GraphicsOutput == NULL && UgaDraw == NULL) {
    return EFI_UNSUPPORTED;
  }

  if (GraphicsOutput != NULL) {
    SizeOfX = GraphicsOutput->Mode->Info->HorizontalResolution;
    SizeOfY = GraphicsOutput->Mode->Info->VerticalResolution;
  } else {
    Status = UgaDraw->GetMode (
                        UgaDraw,
                        &SizeOfX,
                        &SizeOfY,
                        &ColorDepth,
                        &RefreshRate
                        );
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }
  }

  //
  // If X or Y value of OEM badging string is -1, system will auto place this string.
  //
  if ((PosX == (UINTN) (-1) || PosY == (UINTN) (-1))) {
    if (mAutoPlaceStrDestY == (UINTN) (-1)) {
      return EFI_UNSUPPORTED;
    }
    PosX = (SizeOfX - StrLen (TmpStr) * EFI_GLYPH_WIDTH) / 2;
    PosY = mAutoPlaceStrDestY + mAutoPlaceStrCount * EFI_GLYPH_HEIGHT;
    mAutoPlaceStrCount++;
  }

  if (PosX > SizeOfX || PosY > SizeOfY) {
    ASSERT (0);
    return EFI_UNSUPPORTED;
  }

  if (*TmpStr != CHAR_NULL) {
    PrintXY (
      PosX,
      PosY,
      &Foreground,
      &Background,
      TmpStr
      );
  }
  return EFI_SUCCESS;
}

/**
  This function uses to check GOP hardware device is whether ready.

  @retval TRUE        GOP hardware device is ready.
  @retval FALSE       GOP hardware device isn't ready.

**/
BOOLEAN
IsGopReady (
  VOID
  )
{
  EFI_STATUS                            Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL          *GraphicsOutput;
  UINTN                                 SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  *Info;

  Status = GetBootGop (&GraphicsOutput);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  Status = GraphicsOutput->QueryMode (
                             GraphicsOutput,
                             GraphicsOutput->Mode->Mode,
                             &SizeOfInfo,
                             &Info
                             );
  if (!EFI_ERROR (Status)) {
    FreePool (Info);
    return TRUE;
  }
  return FALSE;
}


/**
  Internal function to remove all of previous installed checkpoint and then
  use previous installed checkpoint data to install checkpoint again.

  @param[in] Checkpoint    The number ID of checkpoint.

  @retval EFI_SUCCESS      Reinstall checkpoint successfully.
  @retval EFI_NOT_FOUND    The checkpoint hasn't been triggered.
**/
STATIC
EFI_STATUS
RemovePreviousInstalledCp (
  IN  EFI_GUID *CheckPoint
  )
{
  EFI_STATUS       Status;
  UINTN            NoHandles;
  EFI_HANDLE       *Buffer;
  UINTN            Index;
  VOID             *Interface;


  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  CheckPoint,
                  NULL,
                  &NoHandles,
                  &Buffer
                  );
  if (EFI_ERROR (Status) || NoHandles == 0) {
    return EFI_SUCCESS;
  }

  for (Index = 0; Index < NoHandles; Index++) {
    Status = gBS->HandleProtocol (
                    Buffer[Index],
                    CheckPoint,
                    (VOID **) &Interface
                    );
    ASSERT (!EFI_ERROR (Status));
    Status = gBS->UninstallProtocolInterface (
                    Buffer[Index],
                    CheckPoint,
                    Interface
                    );
    FreePool (Interface);
  }
  FreePool (Buffer);
  return Status;
}

/**
  Internal function to initalize H2O_BDS_CP_DISPLAY_STRING_BEFORE_DATA data.

  @param[in]  Badging             Input EFI_OEM_BADGING_SUPPORT_PROTOCOL interface.
  @param[in]  AfterSelect         Boolean value for hotkey is pressed (true) or not(false).
  @param[in]  SelectedStringNum   Specific string token number to display.
  @param[out] CpData              A Pointer to H2O_BDS_CP_DISPLAY_STRING_BEFORE_DATA instance to initialized.

  @retval EFI_SUCCESS             Init H2O_BDS_CP_DISPLAY_STRING_BEFORE_DATA data successfully.
  @retval EFI_INVALID_PARAMETER   CpData is NULL.
  @retval EFI_OUT_OF_RESOURCES    Allocate memory for H2O_BDS_CP_DISPLAY_STRING_BEFORE_DATA interface failed.
**/
STATIC
EFI_STATUS
InitBeforeDisplayStringData (
  IN  EFI_OEM_BADGING_SUPPORT_PROTOCOL             *Badging,
  IN BOOLEAN                                       AfterSelect,
  IN UINT8                                         SelectedStringNum,
  OUT H2O_BDS_CP_DISPLAY_STRING_BEFORE_DATA        **CpData
  )
{
  H2O_BDS_CP_DISPLAY_STRING_BEFORE_DATA   *BdsDisplayStringBeforeData;

  if (CpData == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  BdsDisplayStringBeforeData = AllocateZeroPool (sizeof (H2O_BDS_CP_DISPLAY_STRING_BEFORE_DATA));
  if (BdsDisplayStringBeforeData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  ZeroMem (BdsDisplayStringBeforeData, sizeof (H2O_BDS_CP_DISPLAY_STRING_BEFORE_DATA));
  BdsDisplayStringBeforeData->Size              = sizeof (H2O_BDS_CP_DISPLAY_STRING_BEFORE_DATA);
  BdsDisplayStringBeforeData->Status            = H2O_BDS_TASK_NORMAL;
  BdsDisplayStringBeforeData->Badging           = Badging;
  BdsDisplayStringBeforeData->AfterSelect       = AfterSelect;
  BdsDisplayStringBeforeData->SelectedStringNum = SelectedStringNum;
  *CpData = BdsDisplayStringBeforeData;
  return EFI_SUCCESS;
}
/**
  Initalize H2O_BDS_CP_DISPLAY_STRING_BEFORE_DATA
  data and trigger gH2OBdsCpDisplayStringBeforeGuid checkpoint.

  @retval EFI_SUCCESS             Trigger gH2OBdsCpDisplayStringBeforeGuid checkpoint successfully.
  @retval EFI_OUT_OF_RESOURCES    Allocate memory to initialize checkpoint data failed.
  @return Other                   Other error occurred while triggering gH2OBdsCpDisplayStringBeforeGuid
                                  checkpoint.
**/
EFI_STATUS
TriggerCpDisplayStringBefore (
  IN EFI_OEM_BADGING_SUPPORT_PROTOCOL      *Badging,
  IN BOOLEAN                               AfterSelect,
  IN UINT8                                 SelectedStringNum
  )
{
  EFI_STATUS                                  Status;
  H2O_BDS_CP_DISPLAY_STRING_BEFORE_DATA       *BdsDisplayStringBeforeData;
  EFI_HANDLE                                  Handle;

  RemovePreviousInstalledCp (&gH2OBdsCpDisplayStringBeforeGuid);
  Status = InitBeforeDisplayStringData (Badging, AfterSelect, SelectedStringNum, &BdsDisplayStringBeforeData);
  if (Status != EFI_SUCCESS) {
    return Status;
  }
  //
  // Install checpoint data to protocol for later usage.
  //
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  (EFI_GUID *) &gH2OBdsCpDisplayStringBeforeGuid,
                  EFI_NATIVE_INTERFACE,
                  (VOID *) BdsDisplayStringBeforeData
                  );
  DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpDisplayStringBeforeGuid));
  H2OCpTrigger (&gH2OBdsCpDisplayStringBeforeGuid, BdsDisplayStringBeforeData);
  DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", BdsDisplayStringBeforeData->Status));
  return EFI_SUCCESS;
}

EFI_STATUS
ShowOemString (
  IN EFI_OEM_BADGING_SUPPORT_PROTOCOL      *Badging,
  IN BOOLEAN                               AfterSelect,
  IN UINT8                                 SelectedStringNum
  )
{
  EFI_STATUS                                  Status;
  UINTN                                       LocX;
  UINTN                                       LocY;
  EFI_UGA_PIXEL                               Foreground;
  EFI_UGA_PIXEL                               Background;
  CHAR16                                      *StringData;
  UINTN                                       StringCount;
  UINTN                                       Index;
  H2O_BDS_CP_DISPLAY_STRING_BEFORE_DATA       *BdsDisplayStringBeforeData;


  if (!IsGopReady ()) {
    return EFI_NOT_STARTED;
  }
  if (FeaturePcdGet (PcdH2OBdsCpDisplayStringBeforeSupported)) {
    TriggerCpDisplayStringBefore (Badging, AfterSelect, SelectedStringNum);
    Status = gBS->LocateProtocol (
                    &gH2OBdsCpDisplayStringBeforeGuid,
                    NULL,
                    (VOID **) &BdsDisplayStringBeforeData
                    );
    if (!EFI_ERROR (Status) && BdsDisplayStringBeforeData->Status == H2O_BDS_TASK_SKIP) {
      return Status;
    }
  }
  LocX = 0;
  LocY = 0;
  StringCount = 0;
  Index = 0;

  Status = EFI_SUCCESS;
  if (FeaturePcdGet (PcdDynamicHotKeySupported)) {
    DYNAMIC_HOTKEY_PROTOCOL        *DynamicHotKey;

	DynamicHotKey = NULL;
    Status = gBS->LocateProtocol (&gDynamicHotKeyProtocolGuid, NULL, (VOID **)&DynamicHotKey);
    if (!EFI_ERROR (Status)) {
      //
      //  Get POST string count
      //
      Status = DynamicHotKey->GetDynamicStringCount (DynamicHotKey, AfterSelect, &StringCount);
      if (!EFI_ERROR (Status)) {
        for (Index = 0 ; Index < StringCount ; Index++) {
          //
          //  Get POST string
          //
          Status = DynamicHotKey->GetDynamicString (DynamicHotKey, Index, AfterSelect, &StringData, &LocX, &LocY);
          if (!EFI_ERROR (Status)) {
            //
            //  Get POST string foreground and background color
            //
            DynamicHotKey->GetDynamicStringColor (Index, AfterSelect, &Foreground, &Background);
            DynamicHotKey->AdjustStringPosition (DynamicHotKey, FALSE, TRUE, mAutoPlaceStrDestY, StringData, &LocX, &LocY);
            DiaplayMessage (LocX, LocY, Foreground, Background, StringData);
            FreePool (StringData);
          }
        }
        DynamicHotKey->AdjustStringPosition (DynamicHotKey, TRUE, TRUE, 0, NULL, NULL, NULL);
      }
    }
  } else {
    ZeroMem (&Foreground, sizeof (EFI_UGA_PIXEL));
    Status = Badging->GetStringCount(Badging, &StringCount);
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }

    mAutoPlaceStrCount = 0;
    Status = EFI_UNSUPPORTED;

    for ( Index = 0 ; Index < StringCount ; Index++ ) {
      if (Badging->GetOemString(Badging, Index, AfterSelect, SelectedStringNum, &StringData, &LocX, &LocY, &Foreground, &Background) ) {
        if (TextOnlyConsole ()) {
          DisplayMessageToConsoleRedirection (StringData);
        }
        DiaplayMessage (
                  LocX,
                  LocY,
                  Foreground,
                  Background,
                  StringData
                  );
        FreePool (StringData);
        Status = EFI_SUCCESS;
      }
    }
  }

  return Status;
}

STATIC
EFI_STATUS
SetTextModeBySimpleTextOut (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *SimpleTextOut,
  IN UINT32                             NewColumns,
  IN UINT32                             NewRows
  )
{
  EFI_STATUS                            Status;
  UINTN                                 Index;
  UINTN                                 Columns;
  UINTN                                 Rows;

  Status = SimpleTextOut->QueryMode (SimpleTextOut, SimpleTextOut->Mode->Mode, &Columns, &Rows);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (Columns == NewColumns && Rows == NewRows) {
    return EFI_SUCCESS;
  }

  for (Index = 0; Index < (UINTN)(SimpleTextOut->Mode->MaxMode); Index++) {
    Status = SimpleTextOut->QueryMode (SimpleTextOut, Index, &Columns, &Rows);
    if (!EFI_ERROR (Status) && Columns == NewColumns && Rows == NewRows) {
      return SimpleTextOut->SetMode (SimpleTextOut, Index);
    }
  }

  DEBUG ((DEBUG_INFO, "Warning! No corresponding resolution for text mode\n"));
  return EFI_UNSUPPORTED;
}

VOID
SetModeByGraphicOutput (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
  IN  UINT32                        SizeOfX,
  IN  UINT32                        SizeOfY
  )
{
  UINT32                                Index;
  UINT32                                OriginalMode;
  UINTN                                 SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  *Info;
  UINT8                                 *EFIBdaVGAMode;
  UINT8                                 *BdaVGAMode;
  EFI_STATUS                            Status;
  EFI_LEGACY_BIOS_PROTOCOL              *LegacyBios;

  EFIBdaVGAMode = (UINT8 *)(UINTN)EFI_CURRENT_VGA_MODE_ADDRESS;
  BdaVGAMode    = (UINT8 *)(UINTN)CURRENT_VGA_MODE_ADDRESS;

  OriginalMode = GraphicsOutput->Mode->Mode;
  GraphicsOutput->QueryMode (GraphicsOutput, OriginalMode, &SizeOfInfo, &Info);

  if (Info->HorizontalResolution != SizeOfX || Info->VerticalResolution != SizeOfY || !IsModeSync ()) {

    Status = EFI_SUCCESS;
    for (Index = 0; Index < GraphicsOutput->Mode->MaxMode; Index++) {
      GraphicsOutput->QueryMode (GraphicsOutput, Index, &SizeOfInfo, &Info);
      if (SizeOfX == Info->HorizontalResolution &&
          SizeOfY == Info->VerticalResolution) {
          Status = GraphicsOutput->SetMode (GraphicsOutput, Index);
          FreePool (Info);
          break;
      }
      FreePool (Info);
      Info = NULL;
    }

    //
    // Cannot find suitable GOP mode, set mode with default resolution.
    //
    if (EFI_ERROR (Status) || (Index == GraphicsOutput->Mode->MaxMode)) {
      for (Index = 0; Index < GraphicsOutput->Mode->MaxMode; Index++) {
        GraphicsOutput->QueryMode (GraphicsOutput, Index, &SizeOfInfo, &Info);
        if (Info->HorizontalResolution == PcdGet32 (PcdDefaultHorizontalResolution) &&
            Info->VerticalResolution == PcdGet32 (PcdDefaultVerticalResolution)) {
            Status = GraphicsOutput->SetMode (GraphicsOutput, Index);
            FreePool (Info);
            Info = NULL;
            break;
        }
        FreePool (Info);
        Info = NULL;
      }
    }

    if (EFI_ERROR (Status) || (Index == GraphicsOutput->Mode->MaxMode)) {
      for (Index = 0; Index < GraphicsOutput->Mode->MaxMode; Index++) {
        Status = GraphicsOutput->QueryMode (GraphicsOutput, Index, &SizeOfInfo, &Info);
        if (Info->HorizontalResolution == 640 &&
            Info->VerticalResolution == 480) {
            Status = GraphicsOutput->SetMode (GraphicsOutput, Index);
            break;
        }
      }
    }

    ASSERT_EFI_ERROR (Status);

  } else {
    Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **) &LegacyBios);
    if (!EFI_ERROR (Status)) {
      if (*EFIBdaVGAMode != *BdaVGAMode) {
        GraphicsOutput->SetMode (GraphicsOutput, OriginalMode);
      }
    }
  }
}

EFI_STATUS
OnEndOfDisableQuietBoot (
  VOID
  )
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        Handle;
  EFI_HANDLE                        *HandleBuffer;
  UINTN                             NumberOfHandles;
  UINTN                             Index;
  VOID                              *Interface;

  //
  // To prevent from memory leak, uninstall other gEndOfDisableQuietBootGuid.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEndOfDisableQuietBootGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < NumberOfHandles; Index++) {
      Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gEndOfDisableQuietBootGuid,
                      (VOID **) &Interface
                      );
      Status = gBS->UninstallProtocolInterface (
                      HandleBuffer[Index],
                      &gEndOfDisableQuietBootGuid,
                      Interface
                      );
    }
    FreePool (HandleBuffer);
  }

  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEndOfDisableQuietBootGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );

  return EFI_SUCCESS;
}

/**
  Consume by Bgrt driver to get the logo infomation.
  In some platform won't called EnableQuietBoot to update the Bgrt logo info,
  if the logo info won't update when ReadyToBoot event trigger, the Bgrt driver will
  get the logo info automatically by this function.

  @retval EFI_SUCCESS              Get boot logo info success.

**/
EFI_STATUS
LogoLibSetBootLogo (
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_BOOT_LOGO_PROTOCOL         *BootLogoProtocol;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL  *BltBuffer = NULL;
  UINTN                          DestinationX = 0;
  UINTN                          DestinationY = 0;
  UINTN                          Width = 0;
  UINTN                          Height = 0;

  Status = gBS->LocateProtocol (&gEfiBootLogoProtocolGuid, NULL, (VOID **)&BootLogoProtocol);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = BgrtGetBootImageInfo (
             &BltBuffer,
             &DestinationX,
             &DestinationY,
             &Width,
             &Height
             );
  if (!EFI_ERROR(Status)) {
    Status = BootLogoProtocol->SetBootLogo (
                                 BootLogoProtocol,
                                 BltBuffer,
                                 DestinationX,
                                 DestinationY,
                                 Width,
                                 Height
                                 );
    if (BltBuffer != NULL) {
      FreePool (BltBuffer);
    }
  }

  return Status;
}

/**
  Internal function to check whether the logo image is in H2O_BDS_CP_DISPLAY_BEFORE_DATA
  logo member.

  @retval TRUE        Logo image is in H2O_BDS_CP_DISPLAY_BEFORE_DATA logo member.
  @retval FALSE       Logo image isn't in H2O_BDS_CP_DISPLAY_BEFORE_DATA logo member.
**/
BOOLEAN
IsLogoInDisplayBeforeProtocol (
  VOID
  )
{
  H2O_BDS_CP_DISPLAY_BEFORE_DATA              *BdsDisplayBeforData;
  EFI_STATUS                                   Status;

  Status = gBS->LocateProtocol (
                  &gH2OBdsCpDisplayBeforeGuid,
                  NULL,
                  (VOID **) &BdsDisplayBeforData
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  if ((BdsDisplayBeforData->Features & H2O_BDS_CP_DISPLAY_LOGO) == 0 || BdsDisplayBeforData->Image == NULL) {
    return FALSE;
  }

  return TRUE;
}

/**
  Internal function to set default OEM string location.

  @retval EFI_SUCCESS   Set OEM string location successfully.
  @return Other         Any error occurred while setting OEM string location.
**/
EFI_STATUS
SetOemStringLocation (
  VOID
  )
{
  EFI_STATUS                          Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL        *GraphicsOutput;
  EFI_UGA_DRAW_PROTOCOL               *UgaDraw;
  UINT32                              SizeOfX;
  UINT32                              SizeOfY;
  UINT32                              ColorDepth;
  UINT32                              RefreshRate;

  UgaDraw        = NULL;
  GraphicsOutput = NULL;
  SizeOfY        = (UINT32) (-1);

  Status = GetBootGop (&GraphicsOutput);
  if (EFI_ERROR (Status) && FeaturePcdGet (PcdUgaConsumeSupport)) {
    Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiUgaDrawProtocolGuid, (VOID **) &UgaDraw);
  }

  if (GraphicsOutput != NULL) {
    SizeOfY = GraphicsOutput->Mode->Info->VerticalResolution;
  } else if (UgaDraw != NULL) {
    Status = UgaDraw->GetMode (UgaDraw, &SizeOfX, &SizeOfY, &ColorDepth, &RefreshRate);
  }

  if (!EFI_ERROR (Status)) {
    mAutoPlaceStrDestY = SizeOfY / 2;
  }
  return Status;

}

/**
  Use SystemTable Conout to stop video based Simple Text Out consoles from going
  to the video device. Put up LogoFile on every video device that is a console.

  @param[in]  LogoFile   File name of logo to display on the center of the screen.

  @retval EFI_SUCCESS     ConsoleControl has been flipped to graphics and logo displayed.
  @retval EFI_UNSUPPORTED Logo not found

**/
EFI_STATUS
EnableQuietBoot (
  IN  EFI_GUID  *LogoFile
  )
{
  EFI_STATUS                                    Status;
  EFI_OEM_BADGING_SUPPORT_PROTOCOL              *Badging;
  UINT32                                        SizeOfX;
  UINT32                                        SizeOfY;
  INTN                                          DestX;
  INTN                                          DestY;
  UINT8                                         *ImageData;
  UINTN                                         ImageSize;
  UINTN                                         BltSize;
  UINT32                                        Instance;
  EFI_BADGING_SUPPORT_FORMAT                    Format;
  EFI_BADGING_SUPPORT_DISPLAY_ATTRIBUTE         Attribute;
  UINTN                                         CoordinateX;
  UINTN                                         CoordinateY;
  UINTN                                         Height;
  UINTN                                         Width;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL                 *Blt;
  EFI_UGA_DRAW_PROTOCOL                         *UgaDraw;
  UINT32                                        ColorDepth;
  UINT32                                        RefreshRate;
  EFI_GRAPHICS_OUTPUT_PROTOCOL                  *GraphicsOutput;
  EFI_BADGING_SUPPORT_IMAGE_TYPE                Type;
  EFI_BADGING_SUPPORT_DISPLAY_ATTRIBUTE         BadgeAttribute;
  UINTN                                         BadgeCoordinateX;
  UINTN                                         BadgeCoordinateY;
  BOOLEAN                                       OverrideBadgeLocation;
  EFI_JPEG_DECODER_PROTOCOL                     *JpegDecoder;
  EFI_PCX_DECODER_PROTOCOL                      *PcxDecoder;
  EFI_JPEG_DECODER_STATUS                       DecoderStatus;
  JPG_BADGE_COMMENT                             *pJpegBadgeComment;
  UINTN                                         Index;
  UINTN                                         LocX;
  UINTN                                         LocY;
  UINT8                                         *VideoBuffer;
  TGA_FORMAT                                    TgaFormate;
  EFI_TGA_DECODER_PROTOCOL                      *TgaDecoder;
  BOOLEAN                                       HasAlphaChannel;
  EFI_GIF_DECODER_PROTOCOL                      *GifDecoder;
  ANIMATION                                     *Animation;
  ANIMATION_REFRESH_ENTRY                       AnimationRefreshEntry;
  EFI_BOOT_LOGO_PROTOCOL                        *SetBootLogo;
  EFI_CONSOLE_CONTROL_PROTOCOL                  *ConsoleControl;
  EFI_CONSOLE_CONTROL_SCREEN_MODE               ScreenMode;
  H2O_BDS_CP_DISPLAY_BEFORE_DATA               *BdsDisplayBeforData;
  EFI_BMP_DECODER_PROTOCOL                      *BmpDecoder;
  EFI_PNG_DECODER_PROTOCOL                      *PngDecoder;

  Status = gBS->LocateProtocol (
                  &gH2OBdsCpDisplayBeforeGuid,
                  NULL,
                  (VOID **) &BdsDisplayBeforData
                  );
  if (!EFI_ERROR (Status) && BdsDisplayBeforData->Status == H2O_BDS_TASK_SKIP) {
    SetOemStringLocation ();
    return EFI_SUCCESS;
  }

  SetBootLogo = NULL;
  Status = gBS->LocateProtocol (&gEfiBootLogoProtocolGuid, NULL, (VOID **)&SetBootLogo);
  if (((BdsDisplayBeforData != NULL && (BdsDisplayBeforData->Features & H2O_BDS_CP_DISPLAY_BGRT) != 0) || BdsDisplayBeforData == NULL)
      && !EFI_ERROR(Status)) {
    //
    // Bgrt feature supported, it has the different showing logo policy.
    //
    Status = BgrtEnableQuietBoot (SetBootLogo);
    if (!EFI_ERROR(Status)) {
      return Status;
    }
  }

  UgaDraw = NULL;
  ColorDepth = 0;
  RefreshRate = 0;
  //
  // Try to open GOP first
  //
  Status = GetBootGop (&GraphicsOutput);
  if (EFI_ERROR (Status) && FeaturePcdGet (PcdUgaConsumeSupport)) {
    GraphicsOutput = NULL;
    //
    // Open GOP failed, try to open UGA
    //
    Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiUgaDrawProtocolGuid, (VOID **) &UgaDraw);
  }
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  Badging = NULL;
  Status  = gBS->LocateProtocol (&gEfiOEMBadgingSupportProtocolGuid, NULL, (VOID **)&Badging);

  JpegDecoder = NULL;
  Status = gBS->LocateProtocol (&gEfiJpegDecoderProtocolGuid, NULL, (VOID **)&JpegDecoder);

  PcxDecoder = NULL;
  Status = gBS->LocateProtocol (&gEfiPcxDecoderProtocolGuid, NULL, (VOID **)&PcxDecoder);

  TgaDecoder = NULL;
  Status = gBS->LocateProtocol (&gEfiTgaDecoderProtocolGuid, NULL, (VOID **)&TgaDecoder);

  GifDecoder = NULL;
  Status = gBS->LocateProtocol (&gEfiGifDecoderProtocolGuid, NULL, (VOID **)&GifDecoder);

  BmpDecoder = NULL;
  Status = gBS->LocateProtocol (&gEfiBmpDecoderProtocolGuid, NULL, (VOID **) &BmpDecoder);

  PngDecoder = NULL;
  Status = gBS->LocateProtocol (&gEfiPngDecoderProtocolGuid, NULL, (VOID **)&PngDecoder);

  Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, (VOID **)&ConsoleControl);
  if (!EFI_ERROR (Status)) {
    ConsoleControl->SetMode (ConsoleControl, EfiConsoleControlScreenGraphics);
  } else {
    ConsoleControl = NULL;
    gST->ConOut->ClearScreen (gST->ConOut);
  }

  //
  // Erase Cursor from screen
  //
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);

  if (GraphicsOutput != NULL) {
    SizeOfX = GraphicsOutput->Mode->Info->HorizontalResolution;
    SizeOfY = GraphicsOutput->Mode->Info->VerticalResolution;

  } else if (UgaDraw != NULL && FeaturePcdGet (PcdUgaConsumeSupport)) {
    Status = UgaDraw->GetMode (UgaDraw, &SizeOfX, &SizeOfY, &ColorDepth, &RefreshRate);
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }
  } else {
    return EFI_UNSUPPORTED;
  }

  //
  // Initialize the Badge location and attributes
  //
  BadgeAttribute = EfiBadgingSupportDisplayAttributeCustomized;
  BadgeCoordinateX = 0;
  BadgeCoordinateY = 0;
  OverrideBadgeLocation = FALSE;
  Height = 0;
  Width = 0;

  //
  // Set the request for the first image of type Logo
  //
  Instance = 0;
  Type = EfiBadgingSupportImageLogo;
  Format = EfiBadgingSupportFormatBMP;

  while (1) {
    ImageData       = NULL;
    ImageSize       = 0;
    TgaFormate      = UnsupportedTgaFormat;
    HasAlphaChannel = FALSE;
    Animation       = NULL;
    if (IsLogoInDisplayBeforeProtocol ()) {
        if (BdsDisplayBeforData == NULL || BdsDisplayBeforData->Image == NULL) {
          return EFI_UNSUPPORTED;
        }
        CoordinateX = 0;
        CoordinateY = 0;
        Attribute   = EfiBadgingSupportDisplayAttributeCenter;
        Height      = BdsDisplayBeforData->Image->Height;
        Width       = BdsDisplayBeforData->Image->Width;
        Blt         = BdsDisplayBeforData->Image->Bitmap;
        BltSize     = Height * Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);

    } else if (!PcdGetBool (PcdH2OBdsOemBadgingSupported)) {
      Attribute = EfiBadgingSupportDisplayAttributeLeftTop;
      Status = GetLogoImageFromFdm (&Blt, &CoordinateX, &CoordinateY, &Width, &Height);
      if (EFI_ERROR (Status)) {
        Blt         = NULL;
        BltSize     = 0;
        CoordinateX = 0;
        CoordinateY = 0;
      } else {
        BltSize = Height * Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
      }
    } else {
      if (Badging != NULL) {
        Status = Badging->GetImage (
                            Badging,
                            &Instance,
                            &Type,
                            &Format,
                            &ImageData,
                            &ImageSize,
                            &Attribute,
                            &CoordinateX,
                            &CoordinateY
                            );
        if (EFI_ERROR (Status)) {
          //
          // After the final logo, reset the instance counter and switch to type Badge
          // After the final badge exit the function
          //
          if (Type == EfiBadgingSupportImageLogo) {
            Type = EfiBadgingSupportImageBadge;
            Instance = 0;
            continue;
          }

          return Status;
        }

        switch (Type) {

        case EfiBadgingSupportImageLogo:
          switch (Format) {

          case EfiBadgingSupportFormatBMP:
            //
            // Upon finding the BMP Logo, save the Badge location from the Reserved values
            //  in the BMP Header
            //
            BadgeCoordinateX = ((BMP_IMAGE_HEADER *)ImageData)->Reserved[0];
            BadgeCoordinateY = ((BMP_IMAGE_HEADER *)ImageData)->Reserved[1];
            OverrideBadgeLocation = TRUE;
            break;

          case EfiBadgingSupportFormatTGA:
            //
            // Upon finding the TGA Logo, save the Badge location from the XOrigin
            // and YOrigin valuesin the TGA Header
            //
            BadgeCoordinateX = ((TGA_IMAGE_HEADER *)ImageData)->XOrigin;
            BadgeCoordinateY = ((TGA_IMAGE_HEADER *)ImageData)->YOrigin;
            OverrideBadgeLocation = TRUE;
            break;
          case EfiBadgingSupportFormatJPEG:
            //
            // Upon finding the JPG Logo, save the Badge location from the $LOC Comment
            //  in the JPG stream
            //
            for (Index = 0; Index < ImageSize; Index++) {
              if (*((UINT8 *)(ImageData + Index)) == 0xFF &&
                  *((UINT8 *)(ImageData + Index + 1)) == 0xFE) {
                pJpegBadgeComment = (JPG_BADGE_COMMENT *)((UINT8 *)(ImageData + Index));
                if (pJpegBadgeComment->Signature == BADGE_SIGNATURE) {
                  BadgeCoordinateX = pJpegBadgeComment->BadgeXLocation;
                  BadgeCoordinateY = pJpegBadgeComment->BadgeYLocation;
                  OverrideBadgeLocation = TRUE;
                  break;
                }
              }
            }
            break;

          case EfiBadgingSupportFormatPCX:
          case EfiBadgingSupportFormatGIF:
          case EfiBadgingSupportFormatPNG:
            //
            // Not Support override badging location right now.
            //
            break;

          default:
            FreePool (ImageData);
            continue;
          }
          break;


        case EfiBadgingSupportImageBadge:
          if (OverrideBadgeLocation) {
            //
            // It needn't override attribute. If override this, all of the EfiBadgingSupportImageBadge type
            // pictures will be displayed in the same location (base on BadgeAttribute setting.
            //
            CoordinateX = BadgeCoordinateX;
            CoordinateY = BadgeCoordinateY;
          }
          break;

        default:
          return EFI_INVALID_PARAMETER;
        }
      } else {
        Status = GetSectionFromAnyFv (LogoFile, EFI_SECTION_RAW, 0, (VOID **) &ImageData, &ImageSize);
        if (EFI_ERROR (Status)) {
          return EFI_UNSUPPORTED;
        }

        CoordinateX = 0;
        CoordinateY = 0;
        Attribute   = EfiBadgingSupportDisplayAttributeCenter;
      }

      Blt = NULL;
      BltSize = 0;

      switch (Format) {

      case EfiBadgingSupportFormatBMP:
        if (BmpDecoder == NULL) {
          Status = EFI_UNSUPPORTED;
          break;
        }
        Status = BmpDecoder->DecodeImage (
                               BmpDecoder,
                               ImageData,
                               ImageSize,
                               (UINT8 **) &Blt,
                               &BltSize,
                               &Height,
                               &Width
                               );
        break;

      case EfiBadgingSupportFormatTGA:
        if (TgaDecoder == NULL) {
          Status = EFI_UNSUPPORTED;
          break;
        }
        Status = TgaDecoder->DecodeImage (
                                   TgaDecoder,
                                   ImageData,
                                   ImageSize,
                                   (UINT8 **)&Blt,
                                   &BltSize,
                                   &Height,
                                   &Width,
                                   &TgaFormate,
                                   &HasAlphaChannel
                                   );
        break;
      case EfiBadgingSupportFormatJPEG:
        if (JpegDecoder == NULL) {
          Status = EFI_UNSUPPORTED;
          break;
        }

        Status = JpegDecoder->DecodeImage (
                        JpegDecoder,
                        ImageData,
                        ImageSize,
                        (UINT8 **)&Blt,
                        &BltSize,
                        &Height,
                        &Width,
                        &DecoderStatus
                        );
        break;

      case EfiBadgingSupportFormatPCX:
        if (PcxDecoder == NULL) {
          Status = EFI_UNSUPPORTED;
          break;
        }

        Status = PcxDecoder->DecodeImage (
                        PcxDecoder,
                        ImageData,
                        ImageSize,
                        (UINT8 **)&Blt,
                        &BltSize,
                        &Height,
                        &Width
                        );
        break;

      case EfiBadgingSupportFormatPNG:
        if (PngDecoder == NULL) {
          Status = EFI_UNSUPPORTED;
          break;
        }

        Status = PngDecoder->DecodeImage (
                               PngDecoder,
                               ImageData,
                               ImageSize,
                               (UINT8 **) &Blt,
                               &BltSize,
                               &Height,
                               &Width
                               );
        break;

      case EfiBadgingSupportFormatGIF:
        if (GifDecoder == NULL) {
          Status = EFI_UNSUPPORTED;
          break;

        }

        Status = GifDecoder->CreateAnimationFromMem (
                               GifDecoder,
                               ImageData,
                               ImageSize,
                               NULL,
                               &Animation
                               );
        if (!EFI_ERROR (Status)) {
          Width  = Animation->Width;
          Height = Animation->Height;
        }
        break;

      default:
        Status = EFI_UNSUPPORTED;
        break;
      }

      if (EFI_ERROR (Status)) {
        FreePool (ImageData);

        if (Badging == NULL) {
          return Status;
        } else {
          continue;
        }
      }
    }
    //
    // Determine and change the video resolution based on the logo
    //

    if ((Type == EfiBadgingSupportImageLogo) && (Badging != NULL)) {
      if (Badging->OemVideoModeScrStrXY (Badging, OemSupportedVideoMode, (UINT32)Width, (UINT32)Height, &LocX, &LocY))  {
        SizeOfX = (UINT32)Width;
        SizeOfY = (UINT32)Height;
        if (GraphicsOutput != NULL) {
          //
          // BUGBUG: sync resolution between SimpleTextOut and GraphicsOutput, but it may not find the corresponding text mode and make set mode failed.
          //
          SetTextModeBySimpleTextOut (gST->ConOut, SizeOfX / EFI_GLYPH_WIDTH, SizeOfY / EFI_GLYPH_HEIGHT);
          SetModeByGraphicOutput (GraphicsOutput, SizeOfX, SizeOfY);
        } else if (UgaDraw != NULL && FeaturePcdGet (PcdUgaConsumeSupport)) {
          Status = UgaDraw->SetMode (UgaDraw, SizeOfX, SizeOfY, ColorDepth, RefreshRate);
          if (EFI_ERROR (Status)) {
            FreePool (ImageData);
            return EFI_UNSUPPORTED;
          }
        }
      }
    }

    switch (Attribute) {

    case EfiBadgingSupportDisplayAttributeLeftTop:
      DestX = CoordinateX;
      DestY = CoordinateY;
      break;

    case EfiBadgingSupportDisplayAttributeCenterTop:
      DestX = (SizeOfX - Width) / 2;
      DestY = CoordinateY;
      break;

    case EfiBadgingSupportDisplayAttributeRightTop:
      DestX = (SizeOfX - Width - CoordinateX);
      DestY = CoordinateY;;
      break;

    case EfiBadgingSupportDisplayAttributeCenterRight:
      DestX = (SizeOfX - Width - CoordinateX);
      DestY = (SizeOfY - Height) / 2;
      break;

    case EfiBadgingSupportDisplayAttributeRightBottom:
      DestX = (SizeOfX - Width - CoordinateX);
      DestY = (SizeOfY - Height - CoordinateY);
      break;

    case EfiBadgingSupportDisplayAttributeCenterBottom:
      DestX = (SizeOfX - Width) / 2;
      DestY = (SizeOfY - Height - CoordinateY);
      break;

    case EfiBadgingSupportDisplayAttributeLeftBottom:
      DestX = CoordinateX;
      DestY = (SizeOfY - Height - CoordinateY);
      break;

    case EfiBadgingSupportDisplayAttributeCenterLeft:
      DestX = CoordinateX;
      DestY = (SizeOfY - Height) / 2;
      break;

    case EfiBadgingSupportDisplayAttributeCenter:
      DestX = (SizeOfX - Width) / 2;
      DestY = (SizeOfY - Height) / 2;
      break;

    default:
      DestX = CoordinateX;
      DestY = CoordinateY;
      break;
    }

    if ((DestX >= 0) && (DestY >= 0)) {
      if (GraphicsOutput != NULL) {
        if (Format == EfiBadgingSupportFormatGIF && GifDecoder != NULL && Animation != NULL) {
            AnimationRefreshEntry.Animation  = Animation;
            AnimationRefreshEntry.Data       = NULL;
            AnimationRefreshEntry.X          = DestX;
            AnimationRefreshEntry.Y          = DestY;
            AnimationRefreshEntry.BltWidth   = SizeOfX;
            AnimationRefreshEntry.BltHeight  = SizeOfY;
            AnimationRefreshEntry.Current    = NULL;
            AnimationRefreshEntry.RecordTick = 0;
            AnimationRefreshEntry.AutoLoop   = FALSE;
            AnimationRefreshEntry.Status     = ANIM_STATUS_PLAY;

            //
            // Only one picture, don't play animation
            //
            if (Animation->Frames != NULL && Animation->Frames->Next == NULL) {
              Status = GifDecoder->NextAnimationFrame (
                                     GifDecoder,
                                     &AnimationRefreshEntry,
                                     GraphicsOutput
                                     );

            } else {
              while (1) {
                if (ConsoleControl != NULL) {
                  ConsoleControl->GetMode (ConsoleControl, &ScreenMode, NULL, NULL);
                  if (ScreenMode == EfiConsoleControlScreenText) {
                    break;
                  }
                }
                Status = GifDecoder->RefreshAnimation (
                                       GifDecoder,
                                       &AnimationRefreshEntry,
                                       GraphicsOutput,
                                       0
                                       );
                if (EFI_ERROR (Status)) {
                  break;
                }
                //
                // wait 0.001 second
                //
                gBS->Stall (1000);
              };
            }
            GifDecoder->DestroyAnimation (GifDecoder, Animation);

        } else {
          if (Format == EfiBadgingSupportFormatTGA && Blt != NULL && TgaDecoder != NULL) {
            if (HasAlphaChannel) {
              VideoBuffer  = AllocatePool (BltSize);
              Status = GraphicsOutput->Blt (
                                  GraphicsOutput,
                                  (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) VideoBuffer,
                                  EfiBltVideoToBltBuffer,
                                  (UINTN) DestX,
                                  (UINTN) DestY,
                                  0,
                                  0,
                                  Width,
                                  Height,
                                  Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                                  );
              Status = TgaDecoder->CalculateBltImage (
                                         TgaDecoder,
                                         (UINT8 *) Blt,
                                         BltSize,
                                         VideoBuffer,
                                         BltSize,
                                         TgaFormate
                                         );
              FreePool (Blt);
              Blt = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) VideoBuffer;
            }
          }
          if (Blt != NULL) {
            Status = GraphicsOutput->Blt (
                                GraphicsOutput,
                                Blt,
                                EfiBltBufferToVideo,
                                0,
                                0,
                                (UINTN) DestX,
                                (UINTN) DestY,
                                Width,
                                Height,
                                Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                                );
          }
        }
      } else if (UgaDraw != NULL && FeaturePcdGet (PcdUgaConsumeSupport)) {
        if (Format == EfiBadgingSupportFormatTGA && Blt != NULL && TgaDecoder != NULL) {
          if (HasAlphaChannel) {
            VideoBuffer  = AllocatePool (BltSize);
            Status = UgaDraw->Blt (
                              UgaDraw,
                              (EFI_UGA_PIXEL *) VideoBuffer,
                              EfiBltVideoToBltBuffer,
                              (UINTN) DestX,
                              (UINTN) DestY,
                              0,
                              0,
                              Width,
                              Height,
                              Width * sizeof (EFI_UGA_PIXEL)
                              );
            Status = TgaDecoder->CalculateBltImage (
                                       TgaDecoder,
                                       (UINT8 *) Blt,
                                       BltSize,
                                       VideoBuffer,
                                       BltSize,
                                       TgaFormate
                                       );
            FreePool (Blt);
            Blt = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) VideoBuffer;
          }
        }
        if (Blt != NULL) {
          Status = UgaDraw->Blt (
                              UgaDraw,
                              (EFI_UGA_PIXEL *) Blt,
                              EfiUgaBltBufferToVideo,
                              0,
                              0,
                              (UINTN) DestX,
                              (UINTN) DestY,
                              Width,
                              Height,
                              Width * sizeof (EFI_UGA_PIXEL)
                              );
        }
      } else {
        Status = EFI_UNSUPPORTED;
      }
    }
    //
    //Display Oem String Message
    //
    if ((Type == EfiBadgingSupportImageLogo) && (Badging != NULL)) {
      mAutoPlaceStrDestY = SizeOfY / 2;
      ShowOemString(Badging, OemGraphicsIsHotKeyDetected (), 0);
    }

    FreePool (ImageData);
    if (Blt != NULL) {
      FreePool (Blt);
    }

    if (Badging == NULL || IsLogoInDisplayBeforeProtocol () || !PcdGetBool (PcdH2OBdsOemBadgingSupported)) {
      break;
    }
  }

  return Status;
}

/**
  Use SystemTable Conout to turn on video based Simple Text Out consoles. The
  Simple Text Out screens will now be synced up with all non video output devices

  @retval EFI_SUCCESS     UGA devices are back in text mode and synced up.

**/
EFI_STATUS
DisableQuietBoot (
  VOID
  )
{
  EFI_STATUS                      Status;
  EFI_UGA_DRAW_PROTOCOL           *UgaDraw;
  EFI_GRAPHICS_OUTPUT_PROTOCOL    *GraphicsOutput;
  EFI_CONSOLE_CONTROL_PROTOCOL    *ConsoleControl;
  UINTN                           MaxMode;
  UINT32                          ModeNumber;
  UINT32                          ScuResolutionX;
  UINT32                          ScuResolutionY;
  UINTN                           Columns;
  UINTN                           Rows;

  UgaDraw = NULL;
  GraphicsOutput = NULL;

  Status = GetBootGop (&GraphicsOutput);
  if (EFI_ERROR (Status)) {
    GraphicsOutput = NULL;
    Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiUgaDrawProtocolGuid, (VOID **)&UgaDraw);
    if (EFI_ERROR (Status)) {
      OnEndOfDisableQuietBoot ();

      return EFI_UNSUPPORTED;
    }
  }

  ConsoleControl = NULL;

  Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, (VOID **)&ConsoleControl);
  if (!EFI_ERROR (Status)) {
    Status = ConsoleControl->SetMode (ConsoleControl, EfiConsoleControlScreenText);
  }


  Status = GetPreferredResolution (
             gST->ConsoleOutHandle,
             &ScuResolutionX,
             &ScuResolutionY,
             NULL,
             NULL,
             NULL
             );

  MaxMode    = gST->ConOut->Mode->MaxMode;
  ModeNumber = (UINT32) MaxMode;
  if (!EFI_ERROR (Status)) {
    for (ModeNumber = 2; ModeNumber < MaxMode; ModeNumber++) {
      //
      // Based on scu resoltuion, find the text mode that enables full screen text view.
      //
      gST->ConOut->QueryMode (gST->ConOut, ModeNumber, &Columns, &Rows);
      if ((ScuResolutionX / EFI_GLYPH_WIDTH == Columns) &&
          (ScuResolutionY / EFI_GLYPH_HEIGHT == Rows) ) {
        Status = gST->ConOut->SetMode (gST->ConOut, ModeNumber);
        break;
      }
    }
  }

  //
  // When one video care with two display output, and plug-in two display,
  // the GOP.SetMode function maybe fail
  //
  if (EFI_ERROR (Status)) {
    for (ModeNumber = 0; ModeNumber < MaxMode; ModeNumber++) {
      gST->ConOut->QueryMode (gST->ConOut, ModeNumber, &Columns, &Rows);
      if ((PcdGet32 (PcdDefaultHorizontalResolution) / EFI_GLYPH_WIDTH == Columns) &&
          (PcdGet32 (PcdDefaultVerticalResolution) / EFI_GLYPH_HEIGHT == Rows)) {
        Status = gST->ConOut->SetMode (gST->ConOut, ModeNumber);
        break;
      }
    }
  }

  if (EFI_ERROR (Status) || ModeNumber == MaxMode) {
    gST->ConOut->SetMode (gST->ConOut, 0);
  }

  if (ConsoleControl == NULL) {
    gST->ConOut->ClearScreen (gST->ConOut);
  }
  //
  // Enable Cursor on Screen
  //
  gST->ConOut->EnableCursor (gST->ConOut, TRUE);

  OnEndOfDisableQuietBoot ();

  return Status;
}

/*++

Routine Description:
  Retrieve native resolution of a panel by reading its Edid information.

Arguments:
  *EdidDiscovered       - A pointer to Edid discovered protocol
  *ResolutionX          - A pointer to native horizontal resolution of a panel.
  *ResolutionY          - A pointer to native vertical resolution of a panel.

Returns:
  EFI_SUCCESS           - The function runs correctly.
  EFI_INVALID_PARAMETER - Invalid parameter.

-*/
EFI_STATUS
GetResolutionByEdid (
  IN  EFI_EDID_DISCOVERED_PROTOCOL      *EdidDiscovered,
  OUT UINT32                            *ResolutionX,
  OUT UINT32                            *ResolutionY
  )
{
  UINT8                                  TempBufferH;
  UINT8                                  TempBufferL;
  UINT32                                 NativeResolutionX;
  UINT32                                 NativeResolutionY;
  VESA_BIOS_EXTENSIONS_EDID_DATA_BLOCK   *EdidDataBlock;

  ASSERT (EdidDiscovered != NULL);
  ASSERT (ResolutionX != NULL);
  ASSERT (ResolutionY != NULL);
  if ((EdidDiscovered == NULL) || (ResolutionX == NULL) || (ResolutionY == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (EdidDiscovered->Edid == NULL) {
    return EFI_UNSUPPORTED;
  }
  EdidDataBlock = (VESA_BIOS_EXTENSIONS_EDID_DATA_BLOCK *) EdidDiscovered->Edid;

  TempBufferH = EdidDataBlock->DetailedTimingDescriptions[4];
  TempBufferL = EdidDataBlock->DetailedTimingDescriptions[2];
  NativeResolutionX = ((((TempBufferH>>4)&0x0F) * 256) + TempBufferL) & 0x0FFF;

  TempBufferH = EdidDataBlock->DetailedTimingDescriptions[7];
  TempBufferL = EdidDataBlock->DetailedTimingDescriptions[5];
  NativeResolutionY = ((((TempBufferH>>4)&0x0F) * 256) + TempBufferL) & 0x0FFF;

  *ResolutionX = NativeResolutionX;
  *ResolutionY = NativeResolutionY;

  return EFI_SUCCESS;
}

BOOLEAN
IsModeSync (
  VOID
)
{
  EFI_GRAPHICS_OUTPUT_PROTOCOL            *ConOutGraphicsOutput;
  EFI_GRAPHICS_OUTPUT_PROTOCOL            *GraphicsOutput;
  EFI_HANDLE                              *HandleBuffer;
  UINTN                                   NumberOfHandles;
  UINTN                                   Index;
  EFI_DEVICE_PATH_PROTOCOL                *GopDevicePath;
  EFI_STATUS                              Status;
  BOOLEAN                                 SyncStatus;

  SyncStatus = TRUE;

  Status = GetBootGop (&ConOutGraphicsOutput);
  if (EFI_ERROR (Status)) {
    return SyncStatus;
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiGraphicsOutputProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return SyncStatus;
  }

  for (Index = 0; Index < NumberOfHandles; Index++) {

    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID*)&GopDevicePath
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiGraphicsOutputProtocolGuid,
                    (VOID **) &GraphicsOutput
                    );
    if (EFI_ERROR(Status)) {
      continue;
    }

    if (ConOutGraphicsOutput->Mode->Info->HorizontalResolution !=
        GraphicsOutput->Mode->Info->HorizontalResolution ||
        ConOutGraphicsOutput->Mode->Info->VerticalResolution!=
        GraphicsOutput->Mode->Info->VerticalResolution) {
      SyncStatus = FALSE;
    }
  }

  FreePool (HandleBuffer);
  return SyncStatus;
}

/**
  Calculate GCD(Greatest Common Divisor) value

  @param[in] A              First value
  @param[in] B              Second value

  @return UINT32            The GCD value

**/
UINT32
CalculateGcd (
  UINT32 A,
  UINT32 B
  )
{
  UINT32 Temp;

  while (B != 0) {
    Temp = A % B;
    A = B;
    B = Temp;
  }
  return A;
}

/**
  Search current specific mode supported the user defined resolution
  for the Graphics Console device based on Graphics Output Protocol.

  @param[in] GraphicsOutput         Graphics Output Protocol instance pointer.
  @param[in]  HorizontalResolution  User defined horizontal resolution
  @param[in]  VerticalResolution    User defined vertical resolution.
  @param[out]  GopModeNum           Current specific mode to be check.
  @param[out]  ResolutionX          Preffered resoltion X
  @param[out]  ResolutionY          Preffered resoltion Y

  @retval EFI_SUCCESS               The mode is supported.
  @retval EFI_NOT_FOUND             The specific mode is out of range of graphics
                                    device supported.
  @retval EFI_INVALID_PARAMETER     Invalid parameter.

**/
EFI_STATUS
FindNearestGopMode (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL   *GraphicsOutput,
  IN  UINT32                         HorizontalResolution,
  IN  UINT32                         VerticalResolution,
  OUT UINT32                         *GopModeNum,
  OUT UINT32                         *ResolutionX,
  OUT UINT32                         *ResolutionY
  )
{
  EFI_STATUS                            Status;
  UINT32                                SelResolutionX;
  UINT32                                SelResolutionY;
  UINT32                                SelModeNum;
  UINT32                                ModeNum;
  UINT32                                MaxMode;
  UINTN                                 SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  *Info;
  UINT32                                RatioX;
  UINT32                                RatioY;
  UINT32                                GcdValue;
  UINTN                                 Count;

  ASSERT (GraphicsOutput != NULL);
  ASSERT (GopModeNum != NULL);
  ASSERT (ResolutionX != NULL);
  ASSERT (ResolutionY != NULL);
  if ((GraphicsOutput == NULL) ||
      (GopModeNum == NULL) ||
      (ResolutionX == NULL) ||
      (ResolutionY == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  ASSERT (HorizontalResolution != 0 || VerticalResolution != 0);

  if (GraphicsOutput == NULL ||
      HorizontalResolution == 0 ||
      VerticalResolution == 0) {
    return EFI_INVALID_PARAMETER;
  }

  MaxMode  = GraphicsOutput->Mode->MaxMode;
  GcdValue = CalculateGcd (HorizontalResolution, VerticalResolution);
  RatioX   = HorizontalResolution / GcdValue;
  RatioY   = VerticalResolution / GcdValue;

  //
  // count == 0, search same aspect ratio
  // count == 1, search all aspect ratio
  //
  for (Count = 0; Count < 2; Count++) {

    SelModeNum     = MaxMode;
    SelResolutionX = (UINT32)-1;
    SelResolutionY = (UINT32)-1;
    for (ModeNum = 0; ModeNum < MaxMode; ModeNum++) {
      Info = NULL;
      Status = GraphicsOutput->QueryMode (
                                 GraphicsOutput,
                                 ModeNum,
                                 &SizeOfInfo,
                                 &Info
                                 );
      if (EFI_ERROR (Status) ||
          Info->HorizontalResolution < HorizontalResolution ||
          Info->VerticalResolution < VerticalResolution) {
        FreePool (Info);
        continue;
      }

      if (Count == 0 &&
          ((Info->HorizontalResolution / GcdValue != RatioX) ||
          (Info->VerticalResolution / GcdValue != RatioY))) {
        FreePool (Info);
        continue;
      }

      if ((Info->HorizontalResolution < SelResolutionX) ||
          ((Info->HorizontalResolution == SelResolutionX) &&
           (Info->VerticalResolution <= SelResolutionY))) {
        SelModeNum     = ModeNum;
        SelResolutionX = Info->HorizontalResolution;
        SelResolutionY = Info->VerticalResolution;
        FreePool (Info);
      }
    }

    if (SelModeNum != MaxMode) {
      *GopModeNum  = SelModeNum;
      *ResolutionX = SelResolutionX;
      *ResolutionY = SelResolutionY;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}


/**
  Check if the current specific mode supported the user defined resolution
  for the Graphics Console device based on Graphics Output Protocol.

  If yes, set the graphic devcice's current mode to this specific mode.

  @param  GraphicsOutput        Graphics Output Protocol instance pointer.
  @param  HorizontalResolution  User defined horizontal resolution
  @param  VerticalResolution    User defined vertical resolution.
  @param  CurrentModeNumber     Current specific mode to be check.

  @retval EFI_SUCCESS       The mode is supported.
  @retval EFI_UNSUPPORTED   The specific mode is out of range of graphics
                            device supported.
  @retval other             The specific mode does not support user defined
                            resolution or failed to set the current mode to the
                            specific mode on graphics device.

**/
EFI_STATUS
CheckModeSupported (
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
  IN  UINT32                    HorizontalResolution,
  IN  UINT32                    VerticalResolution,
  OUT UINT32                    *CurrentModeNumber
  )
{
  UINT32     ModeNumber;
  EFI_STATUS Status;
  UINTN      SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
  UINT32     MaxMode;

  ASSERT (CurrentModeNumber != NULL);
  if (CurrentModeNumber == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status  = EFI_SUCCESS;
  MaxMode = GraphicsOutput->Mode->MaxMode;

  for (ModeNumber = 0; ModeNumber < MaxMode; ModeNumber++) {
    Info = NULL;
    Status = GraphicsOutput->QueryMode (
                       GraphicsOutput,
                       ModeNumber,
                       &SizeOfInfo,
                       &Info
                       );
    if (!EFI_ERROR (Status)) {
      if ((Info->HorizontalResolution == HorizontalResolution) &&
          (Info->VerticalResolution == VerticalResolution)) {
        FreePool (Info);
        break;
      }
      FreePool (Info);
    }
  }

  if (ModeNumber == MaxMode) {
    return EFI_UNSUPPORTED;
  }

  *CurrentModeNumber = ModeNumber;
  return Status;
}

/*++
 Get GOP resolution by specific priority.
   1. Input preferred resolution.
   2. Nearest resolution of PCD default resolution
   3. Resolution of GOP mode 0

 @param[in]  GraphicsOutput         Instance of console out handle
 @param[in]  PreferredResolutionX   Preferred resolution X
 @param[in]  PreferredResolutionY   Preferred resolution Y
 @param[out] ResultResolutionX      Pointer to result resolution X
 @param[out] ResultResolutionY      Pointer to result resolution Y
 @param[out] ResultModeNum          Pointer to result GOP mode

 @retval EFI_SUCCESS                Get GOP resolution successfully.
 @retval EFI_INVALID_PARAMETER      Input parameter is NULL.
 @retval EFI_NOT_FOUND              Failed to get a valid GOP resolution.
**/
STATIC
EFI_STATUS
GetResolution (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL       *GraphicsOutput,
  IN  UINT32                             PreferredResolutionX,
  IN  UINT32                             PreferredResolutionY,
  OUT UINT32                             *ResultResolutionX,
  OUT UINT32                             *ResultResolutionY,
  OUT UINT32                             *ResultModeNum OPTIONAL
  )
{
  EFI_STATUS                             Status;
  UINT32                                 ResolutionX;
  UINT32                                 ResolutionY;
  UINT32                                 ModeNum;
  UINTN                                  SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION   *Info;

  if (GraphicsOutput == NULL || ResultResolutionX == NULL || ResultResolutionY == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ResolutionX = 0;
  ResolutionY = 0;
  ModeNum     = 0;

  if (PreferredResolutionX != 0 && PreferredResolutionY != 0) {
    Status = CheckModeSupported (
               GraphicsOutput,
               PreferredResolutionX,
               PreferredResolutionY,
               &ModeNum
               );
    if (!EFI_ERROR (Status)) {
      ResolutionX = PreferredResolutionX;
      ResolutionY = PreferredResolutionY;
      goto Done;
    }
  }

  Status = FindNearestGopMode (
             GraphicsOutput,
             PcdGet32 (PcdDefaultHorizontalResolution),
             PcdGet32 (PcdDefaultVerticalResolution),
             &ModeNum,
             &ResolutionX,
             &ResolutionY
             );
  if (!EFI_ERROR (Status)) {
    goto Done;
  }

  ModeNum = 0;
  Status = GraphicsOutput->QueryMode (
                             GraphicsOutput,
                             ModeNum,
                             &SizeOfInfo,
                             &Info
                             );
  if (!EFI_ERROR (Status)) {
    ResolutionX = Info->HorizontalResolution;
    ResolutionY = Info->VerticalResolution;
    FreePool (Info);
    goto Done;
  }

  return EFI_NOT_FOUND;

Done:
  *ResultResolutionX = ResolutionX;
  *ResultResolutionY = ResolutionY;
  if (ResultModeNum != NULL) {
    *ResultModeNum = ModeNum;
  }
  return EFI_SUCCESS;
}

/*++

 Retrieve preferred resolution of a GOP.

 @param[in]  GraphicsOutput         Instance of console out handle
 @param[out] ScuResolutionX         X axis of text (SCU) resoliton
 @param[out] ScuResolutionY         Y axis of text (SCU) resoliton
 @param[out] GraphicsResoltionX     X axis of graphics resoliton
 @param[out] GraphicsResoltionY     Y axis of graphics resoliton

 @retval EFI_SUCCESS  Function complete successfully.

**/
EFI_STATUS
GetPreferredResolution (
  IN  EFI_HANDLE                        ConOutHandle,
  OUT UINT32                            *ScuResolutionX OPTIONAL,
  OUT UINT32                            *ScuResolutionY OPTIONAL,
  OUT UINT32                            *GraphicsGopModeNumber OPTIONAL,
  OUT UINT32                            *GraphicsResoltionX OPTIONAL,
  OUT UINT32                            *GraphicsResoltionY OPTIONAL
  )
{
  EFI_STATUS                             Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL           *GraphicsOutput;
  EFI_EDID_DISCOVERED_PROTOCOL           *EdidDiscovered;
  OEM_LOGO_RESOLUTION_DEFINITION         *OemLogoResolution;
  OEM_LOGO_RESOLUTION_DEFINITION         *OemLogoResolutionPtr;
  UINT32                                 NativeResolutionX;
  UINT32                                 NativeResolutionY;
  UINT32                                 ModeNumber;

  ASSERT (ConOutHandle != NULL);

  if (ConOutHandle == NULL ||
      ((ScuResolutionX != NULL && ScuResolutionY == NULL) ||
       (ScuResolutionX == NULL && ScuResolutionY != NULL)) ||
      ((GraphicsResoltionX != NULL && GraphicsResoltionY == NULL) ||
       (GraphicsResoltionX == NULL && GraphicsResoltionY != NULL))) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->HandleProtocol (
                  ConOutHandle,
                  &gEfiGraphicsOutputProtocolGuid,
                  (VOID **)&GraphicsOutput
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  OemLogoResolution = AllocateCopyPool (PcdGetSize (PcdDefaultLogoResolution), PcdGetPtr (PcdDefaultLogoResolution));
  if (OemLogoResolution == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  OemLogoResolutionPtr = OemLogoResolution;

  //
  // According to Window 8 logo requirement (System.Client.Firmware.UEFI.GOP)
  // If Edid mode is not supported, UEFI must select an alternate mode that matches
  // the same aspect ratio as the native resolution of the display.
  // If the display device does not provide an EDID, UEFI must set a mode of
  // 1024 x 768
  //
  Status = gBS->HandleProtocol (
                  ConOutHandle,
                  &gEfiEdidDiscoveredProtocolGuid,
                  (VOID **)&EdidDiscovered
                  );
  if (!EFI_ERROR (Status)) {
    Status = GetResolutionByEdid (EdidDiscovered, &NativeResolutionX, &NativeResolutionY);
    if (!EFI_ERROR (Status)) {
      Status = FindNearestGopMode (
                 GraphicsOutput,
                 NativeResolutionX,
                 NativeResolutionY,
                 &ModeNumber,
                 &NativeResolutionX,
                 &NativeResolutionY
                 );
      if (!EFI_ERROR (Status)) {
        OemLogoResolutionPtr->LogoResolutionX = NativeResolutionX;
        OemLogoResolutionPtr->LogoResolutionY = NativeResolutionY;
      }
    }
  }

  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcLogoResolution \n"));
  Status = OemSvcLogoResolution (&OemLogoResolutionPtr);
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcLogoResolution Status: %r\n", Status));

  Status = EFI_SUCCESS;
  if (GraphicsResoltionX != NULL && GraphicsResoltionY != NULL) {
    Status = GetResolution (
               GraphicsOutput,
               (OemLogoResolutionPtr == NULL) ? 0 : OemLogoResolutionPtr->LogoResolutionX,
               (OemLogoResolutionPtr == NULL) ? 0 : OemLogoResolutionPtr->LogoResolutionY,
               GraphicsResoltionX,
               GraphicsResoltionY,
               GraphicsGopModeNumber
               );
    if (EFI_ERROR (Status)) {
      goto Done;
    }
  }

  if (ScuResolutionX != NULL && ScuResolutionY != NULL) {
    Status = GetResolution (
               GraphicsOutput,
               (OemLogoResolutionPtr == NULL) ? 0 : OemLogoResolutionPtr->ScuResolutionX,
               (OemLogoResolutionPtr == NULL) ? 0 : OemLogoResolutionPtr->ScuResolutionY,
               ScuResolutionX,
               ScuResolutionY,
               NULL
               );
    if (EFI_ERROR (Status)) {
      goto Done;
    }
  }

Done:
  FreePool (OemLogoResolution);
  return Status;
}

BOOLEAN
TextOnlyConsole (
  VOID
  )
{
  EFI_DEVICE_PATH_PROTOCOL         *RemainingDevicePath;
  EFI_DEVICE_PATH_PROTOCOL         *NextDevPathInstance;
  EFI_STATUS                       Status;
  EFI_HANDLE                       Handle;
  EFI_GRAPHICS_OUTPUT_PROTOCOL     *Gop;
  BOOLEAN                          FoundTextBasedCon;
  UINTN                            Size;

  //
  //init local
  //
  RemainingDevicePath = NULL;
  Gop = NULL;
  FoundTextBasedCon = FALSE;
  NextDevPathInstance = NULL;

  RemainingDevicePath = OemGraphicsGetAllActiveConOutDevPath ();
  do {
    //
    //find all output console handles
    //
    NextDevPathInstance = OemGraphicsGetNextDevicePathInstance (&RemainingDevicePath, &Size);
    if (NextDevPathInstance != NULL) {
      gBS->LocateDevicePath (&gEfiDevicePathProtocolGuid, &NextDevPathInstance, &Handle);
      Status = gBS->HandleProtocol (
                      Handle,
                      &gEfiGraphicsOutputProtocolGuid,
                      (VOID*)&Gop
                      );
       if (EFI_ERROR (Status)) {
         //
         //found text-based console
         //
         FoundTextBasedCon = TRUE;
         break;
       }
    }
  } while (RemainingDevicePath != NULL);

  return FoundTextBasedCon;
}


VOID
DisplayMessageToConsoleRedirection (
  IN     CHAR16   *LogoStr
  )
{
  EFI_DEVICE_PATH_PROTOCOL         *RemainingDevicePath;
  EFI_DEVICE_PATH_PROTOCOL         *NextDevPathInstance;
  EFI_STATUS                       Status;
  EFI_HANDLE                       Handle;
  EFI_GRAPHICS_OUTPUT_PROTOCOL     *Gop;
  BOOLEAN                          FoundTextBasedCon;
  UINTN                            Size;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *TextOutProto;

  //
  //init local
  //
  RemainingDevicePath = NULL;
  Gop = NULL;
  FoundTextBasedCon = FALSE;
  NextDevPathInstance = NULL;

  RemainingDevicePath = OemGraphicsGetAllActiveConOutDevPath ();
  do {
    //
    //find all output console handles
    //
    NextDevPathInstance = OemGraphicsGetNextDevicePathInstance (&RemainingDevicePath, &Size);
    if (NextDevPathInstance != NULL) {
      gBS->LocateDevicePath (&gEfiDevicePathProtocolGuid, &NextDevPathInstance, &Handle);
      Status = gBS->HandleProtocol (
                      Handle,
                      &gEfiGraphicsOutputProtocolGuid,
                      (VOID*)&Gop
                      );
       if (EFI_ERROR (Status)) {
         //
         //found text-based console
         //
         Status = gBS->HandleProtocol (
                         Handle,
                         &gEfiSimpleTextOutProtocolGuid,
                         (VOID*) &TextOutProto
                         );
         if (!EFI_ERROR (Status)) {
           TextOutProto->SetCursorPosition (TextOutProto, 0, 0);
           TextOutProto->OutputString (TextOutProto, LogoStr);
         }
       }
    }
  } while (RemainingDevicePath != NULL);

}

VOID *
OemGraphicsGetAllActiveConOutDevPath (
  VOID
  )
{
  UINTN                      NoHandles;
  EFI_HANDLE                 *HandleBuf;
  EFI_DEVICE_PATH_PROTOCOL   *DevPath;
  EFI_DEVICE_PATH_PROTOCOL   *ConsoleDevPath;
  EFI_DEVICE_PATH_PROTOCOL   *TempDevPath;
  EFI_STATUS                 Status;
  UINT8                      Index;

  //
  //init local
  //
  NoHandles = 0;
  HandleBuf = NULL;
  DevPath = NULL;
  ConsoleDevPath = NULL;
  TempDevPath = NULL;

  Status =  gBS->LocateHandleBuffer (
              ByProtocol,
              &gEfiConsoleOutDeviceGuid,
              NULL,
              &NoHandles,
              &HandleBuf
              );
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  for (Index = 0; Index < NoHandles; Index++) {
    Status =  gBS->HandleProtocol (
                HandleBuf [Index],
                &gEfiDevicePathProtocolGuid,
                (VOID **)&DevPath
                );
    if (EFI_ERROR (Status) || (DevPath == NULL)) {
      continue;
    }

    TempDevPath = ConsoleDevPath;
    ConsoleDevPath = AppendDevicePathInstance (ConsoleDevPath, DevPath);
    if (TempDevPath != NULL) {
      FreePool (TempDevPath);
    }
  }

  FreePool (HandleBuf);
  return ConsoleDevPath;
}


EFI_DEVICE_PATH_PROTOCOL *
EFIAPI
OemGraphicsGetNextDevicePathInstance (
  IN OUT EFI_DEVICE_PATH_PROTOCOL    **DevicePath,
  OUT UINTN                          *Size
  )
/*++

Routine Description:
  Function retrieves the next device path instance from a device path data structure.

Arguments:
  DevicePath           - A pointer to a device path data structure.

  Size                 - A pointer to the size of a device path instance in bytes.

Returns:

  This function returns a pointer to the current device path instance.
  In addition, it returns the size in bytes of the current device path instance in Size,
  and a pointer to the next device path instance in DevicePath.
  If there are no more device path instances in DevicePath, then DevicePath will be set to NULL.

--*/
{
  EFI_DEVICE_PATH_PROTOCOL  *DevPath;
  EFI_DEVICE_PATH_PROTOCOL  *ReturnValue;
  UINT8                     Temp;

  ASSERT (Size != NULL);
  if (Size == NULL) {
    return NULL;
  }

  if (DevicePath == NULL || *DevicePath == NULL) {
    *Size = 0;
    return NULL;
  }

  if (!OemGraphicsIsDevicePathValid (*DevicePath, 0)) {
    return NULL;
  }

  //
  // Find the end of the device path instance
  //
  DevPath = *DevicePath;
  while (!OemGraphicsIsDevicePathEndType (DevPath)) {
    DevPath = OemGraphicsNextDevicePathNode (DevPath);
  }

  //
  // Compute the size of the device path instance
  //
  *Size = ((UINTN) DevPath - (UINTN) (*DevicePath)) + sizeof (EFI_DEVICE_PATH_PROTOCOL);

  //
  // Make a copy and return the device path instance
  //
  Temp              = DevPath->SubType;
  DevPath->SubType  = END_ENTIRE_DEVICE_PATH_SUBTYPE;
  ReturnValue       = OemGraphicsDuplicateDevicePath (*DevicePath);
  DevPath->SubType  = Temp;

  //
  // If DevPath is the end of an entire device path, then another instance
  // does not follow, so *DevicePath is set to NULL.
  //
  if (OemGraphicsDevicePathSubType (DevPath) == END_ENTIRE_DEVICE_PATH_SUBTYPE) {
    *DevicePath = NULL;
  } else {
    *DevicePath = OemGraphicsNextDevicePathNode (DevPath);
  }

  return ReturnValue;
}

EFI_DEVICE_PATH_PROTOCOL *
EFIAPI
OemGraphicsDuplicateDevicePath (
  IN CONST EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
/*++

Routine Description:

  Duplicate a device path structure.

Arguments:

  DevicePath  - The device path to duplicated.

Returns:

  The duplicated device path.

--*/
{
  UINTN                     Size;

  //
  // Compute the size
  //
  Size = OemGraphicsGetDevicePathSize (DevicePath);
  if (Size == 0) {
    return NULL;
  }

  //
  // Allocate space for duplicate device path
  //

  return AllocateCopyPool (Size, DevicePath);
}

UINTN
EFIAPI
OemGraphicsGetDevicePathSize (
  IN CONST EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
/*++

Routine Description:

  Calculate the space size of a device path.

Arguments:

  DevicePath  - A specified device path

Returns:

  The size.

--*/
{
  CONST EFI_DEVICE_PATH_PROTOCOL  *Start;

  if (DevicePath == NULL) {
    return 0;
  }

  if (!OemGraphicsIsDevicePathValid (DevicePath, 0)) {
    return 0;
  }

  //
  // Search for the end of the device path structure
  //
  Start = DevicePath;
  while (!OemGraphicsIsDevicePathEnd (DevicePath)) {
    DevicePath = OemGraphicsNextDevicePathNode (DevicePath);
  }

  //
  // Compute the size and add back in the size of the end device path structure
  //
  return ((UINTN) DevicePath - (UINTN) Start) + OemGraphicsDevicePathNodeLength (DevicePath);
}

BOOLEAN
EFIAPI
OemGraphicsIsDevicePathValid (
  IN CONST EFI_DEVICE_PATH_PROTOCOL *DevicePath,
  IN       UINTN                    MaxSize
  )
{
  UINTN Count;
  UINTN Size;
  UINTN NodeLength;

  ASSERT (DevicePath != NULL);
  if (DevicePath == NULL) {
    return FALSE;
  }

  for (Count = 0, Size = 0; !OemGraphicsIsDevicePathEnd (DevicePath); DevicePath = OemGraphicsNextDevicePathNode (DevicePath)) {
    NodeLength = OemGraphicsDevicePathNodeLength (DevicePath);
    if (NodeLength < sizeof (EFI_DEVICE_PATH_PROTOCOL)) {
      return FALSE;
    }

    if (MaxSize > 0) {
      Size += NodeLength;
      if (Size + END_DEVICE_PATH_LENGTH > MaxSize) {
        return FALSE;
      }
    }

    if (PcdGet32 (PcdMaximumDevicePathNodeCount) > 0) {
      Count++;
      if (Count >= PcdGet32 (PcdMaximumDevicePathNodeCount)) {
        return FALSE;
      }
    }
  }

  //
  // Only return TRUE when the End Device Path node is valid.
  //
  return (BOOLEAN) (OemGraphicsDevicePathNodeLength (DevicePath) == END_DEVICE_PATH_LENGTH);
}

BOOLEAN
EFIAPI
OemGraphicsIsDevicePathEndType (
  IN CONST VOID  *Node
  )
{
  ASSERT (Node != NULL);
  if (Node == NULL) {
    return FALSE;
  }
  return (BOOLEAN) (OemGraphicsDevicePathType (Node) == END_DEVICE_PATH_TYPE);
}

EFI_DEVICE_PATH_PROTOCOL *
EFIAPI
OemGraphicsNextDevicePathNode (
  IN CONST VOID  *Node
  )
{
  ASSERT (Node != NULL);
  return (EFI_DEVICE_PATH_PROTOCOL *)((UINT8 *)(Node) + OemGraphicsDevicePathNodeLength(Node));
}

UINT8
EFIAPI
OemGraphicsDevicePathSubType (
  IN CONST VOID  *Node
  )
{
  ASSERT (Node != NULL);
  if (Node == NULL) {
    return 0;
  }
  return ((EFI_DEVICE_PATH_PROTOCOL *)(Node))->SubType;
}

BOOLEAN
EFIAPI
OemGraphicsIsDevicePathEnd (
  IN CONST VOID  *Node
  )
{
  ASSERT (Node != NULL);
  if (Node == NULL) {
    return FALSE;
  }
  return (BOOLEAN) (OemGraphicsIsDevicePathEndType (Node) && OemGraphicsDevicePathSubType(Node) == END_ENTIRE_DEVICE_PATH_SUBTYPE);
}

UINTN
EFIAPI
OemGraphicsDevicePathNodeLength (
  IN CONST VOID  *Node
  )
{
  UINTN Length;

  ASSERT (Node != NULL);
  Length = ReadUnaligned16 ((UINT16 *)&((EFI_DEVICE_PATH_PROTOCOL *)(Node))->Length[0]);
  ASSERT (Length >= sizeof (EFI_DEVICE_PATH_PROTOCOL));
  return Length;
}

UINT8
EFIAPI
OemGraphicsDevicePathType (
  IN CONST VOID  *Node
  )
{
  ASSERT (Node != NULL);
  if (Node == NULL) {
    return 0;
  }
  return ((EFI_DEVICE_PATH_PROTOCOL *)(Node))->Type;
}
