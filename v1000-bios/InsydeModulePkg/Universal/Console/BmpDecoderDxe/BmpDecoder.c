/** @file
  This code supports a the private implementation
  of the BMP Decoder protocol

;******************************************************************************
;* Copyright (c) 2023, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "BmpDecoder.h"

BMP_DECODER_INSTANCE    mPrivateData = {
  BMP_DECODER_INSTANCE_SIGNATURE,
  NULL,
  { BmpDecoderDecodeImage }
};

#define SIZE_4G     0x100000000
//
// BMP compression type.
//
#define BMP_UNCOMPRESSION_TYPE 0
#define BMP_RLE8BIT_TYPE       1
#define BMP_RLE4BIT_TYPE       2

//
// RLE Mode.
//
#define ENCODE_MODE_END_OF_LINE   0x00
#define ENCODE_MODE_END_OF_BITMAP 0x01
#define ENCODE_MODE_DELTA         0x02
#define ABSOLUTE_MODE_FLAG        0x00

#define RGB555_TO_RGB888(color) ((color) << 3 | ((color) >> 2))


/**
 Convert a BMP image data with RLE4 compression type to a GOP blt buffer.

 @param[out] BltBuffer          Pointer to output GOP blt buffer
 @param[in]  RLE4Image          Pointer to BMP image data with RLE4 compression type
 @param[in]  RLE4ImageSize      Size of BMP image data
 @param[in]  BmpColorMap        Pointer to BMP color map
 @param[in]  BmpColorMapNum     Number of BMP color map
 @param[in]  BmpHeader          Pointer to BMP image header

 @retval EFI_SUCCESS            Convert successfully
 @retval EFI_INVALID_PARAMETER  Input parameter is NULL or BltBufferSize is too small.
 @retval EFI_UNSUPPORTED        BMP height or width is zero or BMP image data is invalid
**/
EFI_STATUS
RLE4ToBlt (
  OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer,
  IN  UINT8                         *RLE4Image,
  IN  UINTN                         RLE4ImageSize,
  IN  BMP_COLOR_MAP                 *BmpColorMap,
  IN  UINTN                         BmpColorMapNum,
  IN  BMP_IMAGE_HEADER              *BmpHeader
  )
{
  UINT8         FirstByte;
  UINT8         SecondByte;
  UINTN         Index;
  UINTN         Height;
  UINTN         Width;
  BOOLEAN       EndOfLine;
  BOOLEAN       EndOfBMP;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Blt;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBufferEnd;
  UINT8                         *RLE4ImageEnd;

  if (BltBuffer == NULL || RLE4Image == NULL || RLE4ImageSize == 0 || BmpHeader == NULL || (BmpColorMapNum != 0 && BmpColorMap == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if ((BmpHeader->PixelHeight == 0) || (BmpHeader->PixelWidth == 0)) {
    return EFI_UNSUPPORTED;
  }

  BltBufferEnd = BltBuffer + (BmpHeader->PixelHeight * BmpHeader->PixelWidth);
  RLE4ImageEnd = RLE4Image + RLE4ImageSize;
  EndOfLine = FALSE;
  EndOfBMP = FALSE;
  Height = 0;

  while (Height < BmpHeader->PixelHeight && EndOfBMP == FALSE) {

    Blt = &BltBuffer[(BmpHeader->PixelHeight - Height - 1) * BmpHeader->PixelWidth];
    Width = 0;
    EndOfLine = FALSE;

    while (Width <= BmpHeader->PixelWidth && EndOfLine == FALSE) {

      //
      // Get a WORD each time
      //
      if (RLE4Image + 2 > RLE4ImageEnd) {
        return EFI_UNSUPPORTED;
      }
      FirstByte = *RLE4Image;
      RLE4Image++;
      SecondByte = *RLE4Image;
      RLE4Image++;

      if (FirstByte > ABSOLUTE_MODE_FLAG) {
        UINT8 Nibble[2];
        Nibble[0] = (SecondByte & 0x0F);
        Nibble[1] = (SecondByte & 0xF0) >>4;

        if ((Blt + FirstByte > BltBufferEnd) ||
            (Nibble[1] >= BmpColorMapNum) ||
            (FirstByte > 1 && Nibble[0] >= BmpColorMapNum)) {
          return EFI_UNSUPPORTED;
        }
        for(Index=0; Index<FirstByte; Index++, Width++, Blt++) {
          //
          // The left-most pixel is in the more significant nibble.
          //
          Blt->Red    = BmpColorMap[Nibble[1-Index%2]].Red;
          Blt->Green  = BmpColorMap[Nibble[1-Index%2]].Green;
          Blt->Blue   = BmpColorMap[Nibble[1-Index%2]].Blue;
        }
      } else { // FirstByte is zero...
        switch (SecondByte) {
          case ENCODE_MODE_END_OF_LINE:
            EndOfLine = TRUE;
            break;

          case ENCODE_MODE_END_OF_BITMAP:
            EndOfBMP = TRUE;
            EndOfLine = TRUE;
            break;

          case ENCODE_MODE_DELTA:
            if (RLE4Image + 2 > RLE4ImageEnd) {
              return EFI_UNSUPPORTED;
            }
            FirstByte = *RLE4Image;
            RLE4Image++;
            SecondByte= *RLE4Image;
            RLE4Image++;
            Width = Width + FirstByte;
            Height = Height + SecondByte;
            if (Height >= BmpHeader->PixelHeight) {
              return EFI_UNSUPPORTED;
            }
            Blt = &BltBuffer[((BmpHeader->PixelHeight - Height - 1) * BmpHeader->PixelWidth) + (Width)];
            break;

          default:
            //
            // Absolute mode. (This block has no compression)
            //
            {
              UINT16  ByteToBeRead = (((SecondByte-1)/4)+1)*2;

              if (RLE4Image + ByteToBeRead > RLE4ImageEnd ||
                  Blt + MIN (SecondByte, (ByteToBeRead * 2)) > BltBufferEnd) {
                return EFI_UNSUPPORTED;
              }
              for (Index = 0; Index < ByteToBeRead; Index++) {
                UINT8 NibbleHigh = ((*RLE4Image) & 0xF0) >> 4;
                UINT8 NibbleLow  = (*RLE4Image & 0x0F);
                RLE4Image++;

                if(SecondByte != 0) {
                  if (NibbleHigh >= BmpColorMapNum) {
                    return EFI_UNSUPPORTED;
                  }
                  Blt->Red    = BmpColorMap[NibbleHigh].Red;
                  Blt->Green  = BmpColorMap[NibbleHigh].Green;
                  Blt->Blue   = BmpColorMap[NibbleHigh].Blue;
                  Width++;
                  Blt++;
                  SecondByte--;
                }

                if(SecondByte != 0) {
                  if (NibbleLow >= BmpColorMapNum) {
                    return EFI_UNSUPPORTED;
                  }
                  Blt->Red    = BmpColorMap[NibbleLow].Red;
                  Blt->Green  = BmpColorMap[NibbleLow].Green;
                  Blt->Blue   = BmpColorMap[NibbleLow].Blue;
                  Width++;
                  Blt++;
                  SecondByte--;
                }
              }
            }
            break;
        }
      }
    }
    Height++;
  }
  return EFI_SUCCESS;
}

/**
 Convert a BMP image data with RLE8 compression type to a GOP blt buffer.

 @param[out] BltBuffer          Pointer to output GOP blt buffer
 @param[in]  RLE8Image          Pointer to BMP image data with RLE8 compression type
 @param[in]  RLE8ImageSize      Size of BMP image data
 @param[in]  BmpColorMap        Pointer to BMP color map
 @param[in]  BmpColorMapNum     Number of BMP color map
 @param[in]  BmpHeader          Pointer to BMP image header

 @retval EFI_SUCCESS            Convert successfully
 @retval EFI_INVALID_PARAMETER  Input parameter is NULL or BltBufferSize is too small.
 @retval EFI_UNSUPPORTED        BMP height or width is zero or BMP image data is invalid
**/
EFI_STATUS
RLE8ToBlt (
  OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer,
  IN  UINT8                         *RLE8Image,
  IN  UINTN                         RLE8ImageSize,
  IN  BMP_COLOR_MAP                 *BmpColorMap,
  IN  UINTN                         BmpColorMapNum,
  IN  BMP_IMAGE_HEADER              *BmpHeader
  )
{
  UINT8         FirstByte;
  UINT8         SecondByte;
  UINT8         EachValue;
  UINTN         Index;
  UINTN         Height;
  UINTN         Width;
  BOOLEAN       EndOfLine;
  BOOLEAN       EndOfBMP;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Blt;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBufferEnd;
  UINT8                         *RLE8ImageEnd;

  if (BltBuffer == NULL || RLE8Image == NULL || RLE8ImageSize == 0 || BmpHeader == NULL || (BmpColorMapNum != 0 && BmpColorMap == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if ((BmpHeader->PixelHeight == 0) || (BmpHeader->PixelWidth == 0)) {
    return EFI_UNSUPPORTED;
  }

  BltBufferEnd = BltBuffer + (BmpHeader->PixelHeight * BmpHeader->PixelWidth);
  RLE8ImageEnd = RLE8Image + RLE8ImageSize;
  EndOfLine = FALSE;
  EndOfBMP = FALSE;
  Height = 0;

  while (Height < BmpHeader->PixelHeight && EndOfBMP == FALSE) {

    Blt = &BltBuffer[(BmpHeader->PixelHeight - Height - 1) * BmpHeader->PixelWidth];
    Width = 0;
    EndOfLine = FALSE;

    while (Width <= BmpHeader->PixelWidth && EndOfLine == FALSE) {
      if (RLE8Image + 2 > RLE8ImageEnd) {
        return EFI_UNSUPPORTED;
      }
      FirstByte = *RLE8Image;
      RLE8Image++;
      SecondByte = *RLE8Image;
      RLE8Image++;

      if (FirstByte > ABSOLUTE_MODE_FLAG) {
        if (Blt + FirstByte > BltBufferEnd || SecondByte >= BmpColorMapNum) {
          return EFI_UNSUPPORTED;
        }
        for (Index = 0; Index < FirstByte; Index++, Width++, Blt++) {
          Blt->Red    = BmpColorMap[SecondByte].Red;
          Blt->Green  = BmpColorMap[SecondByte].Green;
          Blt->Blue   = BmpColorMap[SecondByte].Blue;
        }
      } else {
        switch (SecondByte) {

        case ENCODE_MODE_END_OF_LINE:
          EndOfLine = TRUE;
          break;

        case ENCODE_MODE_END_OF_BITMAP:
          EndOfBMP = TRUE;
          EndOfLine = TRUE;
          break;

        case ENCODE_MODE_DELTA:
          if (RLE8Image + 2 > RLE8ImageEnd) {
            return EFI_UNSUPPORTED;
          }
          FirstByte = *RLE8Image;
          RLE8Image++;
          SecondByte= *RLE8Image;
          RLE8Image++;
          Width = Width + FirstByte;
          Height = Height + SecondByte;
          if (Height >= BmpHeader->PixelHeight) {
            return EFI_UNSUPPORTED;
          }
          Blt = &BltBuffer[((BmpHeader->PixelHeight - Height - 1) * BmpHeader->PixelWidth) + (Width)];
          break;

        default:
          //
          // Absolute mode.
          //
          if (RLE8Image + SecondByte > RLE8ImageEnd ||
              Blt + SecondByte > BltBufferEnd) {
            return EFI_UNSUPPORTED;
          }
          for (Index = 0; Index < SecondByte; Index++, Width++, Blt++) {
            EachValue = *RLE8Image;
            RLE8Image++;
            if (EachValue >= BmpColorMapNum) {
              return EFI_UNSUPPORTED;
            }
            Blt->Red    = BmpColorMap[EachValue].Red;
            Blt->Green  = BmpColorMap[EachValue].Green;
            Blt->Blue   = BmpColorMap[EachValue].Blue;
          }
          //
          // align on word boundary
          //
          if (Index % 2 == 1) {
            RLE8Image++;
          }
          break;
        }
      }
    }
    Height++;
  }
  return EFI_SUCCESS;
}

/**
 Convert a *.BMP graphics image to a GOP/UGA blt buffer. If a NULL Blt buffer
 is passed in a GopBlt buffer will be allocated by this routine. If a GopBlt
 buffer is passed in it will be used if it is big enough.

 @param[in]  This               Pointer to BMP protocol instance
 @param[in]  ImageData          Pointer to BMP file
 @param[in]  ImageDataSize      Number of bytes in BMP file
 @param[out] DecodedData        Buffer containing GOP version of BMP file
 @param[out] DecodedDataSize    Size of DecodedData in bytes.
 @param[out] Height             Height of DecodedData/ImageData in pixels
 @param[out] Width              Width of DecodedData/ImageData in pixels

 @retval EFI_SUCCESS            GopBlt and GopBltSize are returned.
 @retval EFI_UNSUPPORTED        BmpImage is not a valid *.BMP image
 @retval EFI_BUFFER_TOO_SMALL   The passed in GopBlt buffer is not big enough.
                                GopBltSize will contain the required size.
 @retval EFI_OUT_OF_RESOURCES   No enough buffer to allocate

**/
EFI_STATUS
EFIAPI
BmpDecoderDecodeImage (
  IN     EFI_BMP_DECODER_PROTOCOL     *This,
  IN     UINT8                         *ImageData,
  IN     UINTN                         ImageDataSize,
     OUT UINT8                         **DecodedData,
     OUT UINTN                         *DecodedDataSize,
     OUT UINTN                         *Height,
     OUT UINTN                         *Width
  )
{
  UINT8                         *Image;
  UINT8                         *ImageHeader;
  BMP_IMAGE_HEADER              *BmpHeader;
  BMP_COLOR_MAP                 *BmpColorMap;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BltBuffer;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Blt;
  UINT64                        BltBufferSize;
  UINTN                         Index;
  UINTN                         HeightIndex;
  UINTN                         WidthIndex;
  UINTN                         ImageIndex;
  UINT32                        DataSizePerLine;
  BOOLEAN                       IsAllocated;
  UINT32                        ColorMapNum;
  UINT64                        DataSize;
  EFI_STATUS                    Status;

  if ((ImageData == NULL) || (DecodedData == NULL) || (DecodedDataSize == NULL) || (Height == NULL) || (Width == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (sizeof (BMP_IMAGE_HEADER) > ImageDataSize) {
    return EFI_INVALID_PARAMETER;
  }

  BmpHeader = (BMP_IMAGE_HEADER *) ImageData;
  if (BmpHeader->CharB != 'B' || BmpHeader->CharM != 'M') {
    return EFI_UNSUPPORTED;
  }

  if (   BmpHeader->CompressionType != BMP_UNCOMPRESSION_TYPE
      && BmpHeader->CompressionType != BMP_RLE8BIT_TYPE
      && BmpHeader->CompressionType != BMP_RLE4BIT_TYPE ) {
    return EFI_UNSUPPORTED;
  }

  if ((BmpHeader->PixelHeight == 0) || (BmpHeader->PixelWidth == 0)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Only support BITMAPINFOHEADER format.
  // BITMAPFILEHEADER + BITMAPINFOHEADER = BMP_IMAGE_HEADER
  //
  if (BmpHeader->HeaderSize != sizeof (BMP_IMAGE_HEADER) - OFFSET_OF(BMP_IMAGE_HEADER, HeaderSize)) {
    return EFI_UNSUPPORTED;
  }

  //
  // The data size in each line must be 4 byte alignment.
  //
  DataSize = MultU64x32 ((UINT64) BmpHeader->PixelWidth, BmpHeader->BitPerPixel);
  if (DataSize > MAX_UINT32) {
    return EFI_UNSUPPORTED;
  }
  DataSize += 31;
  if (DataSize > MAX_UINT32) {
    return EFI_UNSUPPORTED;
  }
  DataSizePerLine = (((UINT32) DataSize) >> 3) &(~0x3);

  DataSize = MultU64x32 ((UINT64) DataSizePerLine, BmpHeader->PixelHeight);
  if (DataSize > MAX_UINT32) {
    return EFI_INVALID_PARAMETER;
  }
  if ((ImageDataSize < BmpHeader->ImageOffset) ||
      (ImageDataSize - BmpHeader->ImageOffset < (UINT32) DataSize && BmpHeader->CompressionType == BMP_UNCOMPRESSION_TYPE)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Calculate Color Map offset in the image.
  //
  Image       = ImageData;
  BmpColorMap = (BMP_COLOR_MAP *) (Image + sizeof (BMP_IMAGE_HEADER));
  if (BmpHeader->ImageOffset < sizeof (BMP_IMAGE_HEADER)) {
    return EFI_INVALID_PARAMETER;
  }

  ColorMapNum = 0;
  if (BmpHeader->ImageOffset > sizeof (BMP_IMAGE_HEADER)) {
    switch (BmpHeader->BitPerPixel) {

    case 1:
      ColorMapNum = 2;
      break;
    case 4:
      ColorMapNum = 16;
      break;
    case 8:
      ColorMapNum = 256;
      break;
    default:
      ColorMapNum = 0;
      break;
    }

    if (BmpHeader->NumberOfColors != 0) {
      ColorMapNum = BmpHeader->NumberOfColors;
    }

    //
    // BMP file may has padding data between the bmp header section and the bmp data section.
    //
    if (BmpHeader->ImageOffset - sizeof (BMP_IMAGE_HEADER) < sizeof (BMP_COLOR_MAP) * ColorMapNum) {
      return EFI_INVALID_PARAMETER;
    }
  }

  //
  // Calculate graphics image data address in the image
  //
  Image         = ((UINT8 *) ImageData) + BmpHeader->ImageOffset;
  ImageHeader   = Image;

  //
  // Calculate the BltBuffer needed size.
  //
  BltBufferSize = MultU64x32 ((UINT64) BmpHeader->PixelWidth, BmpHeader->PixelHeight);
  //
  // Ensure the BltBufferSize * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) doesn't overflow
  //
  if (BltBufferSize > DivU64x32 ((UINTN) ~0, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL))) {
    return EFI_UNSUPPORTED;
  }
  BltBufferSize = MultU64x32 (BltBufferSize, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  if (BltBufferSize >= SIZE_4G) {
     //
     // The buffer size extends the limitation
     //
     return EFI_UNSUPPORTED;
  }

  IsAllocated   = FALSE;
  if (*DecodedData == NULL) {
    *DecodedDataSize = (UINTN) BltBufferSize;
    *DecodedData     = AllocatePool (*DecodedDataSize);
    IsAllocated = TRUE;
    if (*DecodedData == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  } else {
    if (*DecodedDataSize < (UINTN) BltBufferSize) {
      *DecodedDataSize = (UINTN) BltBufferSize;

      return EFI_BUFFER_TOO_SMALL;
    }
  }

  *Width   = BmpHeader->PixelWidth;
  *Height  = BmpHeader->PixelHeight;
  //
  // Convert image from BMP to Blt buffer format
  //
  BltBuffer = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) *DecodedData;
  //
  // RLE8 decode.
  //
  if (BmpHeader->CompressionType == BMP_RLE8BIT_TYPE) {
    gBS->SetMem (BltBuffer, (UINTN)BltBufferSize, 0x00);
    Status = RLE8ToBlt (BltBuffer, Image, ImageDataSize - BmpHeader->ImageOffset, BmpColorMap, ColorMapNum, BmpHeader);
    if (EFI_ERROR (Status)) {
      goto Error;
    }
    return EFI_SUCCESS;
  }
  //
  // RLE4 decode.
  //
  if(BmpHeader->CompressionType == BMP_RLE4BIT_TYPE) {
    gBS->SetMem (BltBuffer, (UINTN)BltBufferSize, 0x00);
    Status = RLE4ToBlt (BltBuffer, Image, ImageDataSize - BmpHeader->ImageOffset, BmpColorMap, ColorMapNum, BmpHeader);
    if (EFI_ERROR (Status)) {
      goto Error;
    }
    return EFI_SUCCESS;
  }
  //
  // BMP decode.
  //
  for (HeightIndex = 0; HeightIndex < BmpHeader->PixelHeight; HeightIndex++) {
    Blt = &BltBuffer[(BmpHeader->PixelHeight - HeightIndex - 1) * BmpHeader->PixelWidth];
    for (WidthIndex = 0; WidthIndex < BmpHeader->PixelWidth; WidthIndex++, Image++, Blt++) {
      switch (BmpHeader->BitPerPixel) {

      case 1:
        //
        // Convert 1-bit (2 colors) BMP to 24-bit color
        //
        for (Index = 0; Index < 8 && WidthIndex < BmpHeader->PixelWidth; Index++) {
          if ((UINT32) (((*Image) >> (7 - Index)) & 0x1) >= ColorMapNum) {
            goto Error;
          }
          Blt->Red    = BmpColorMap[((*Image) >> (7 - Index)) & 0x1].Red;
          Blt->Green  = BmpColorMap[((*Image) >> (7 - Index)) & 0x1].Green;
          Blt->Blue   = BmpColorMap[((*Image) >> (7 - Index)) & 0x1].Blue;
          Blt++;
          WidthIndex++;
        }

        Blt --;
        WidthIndex --;
        break;

      case 4:
        //
        // Convert 4-bit (16 colors) BMP Palette to 24-bit color
        //
        Index       = (*Image) >> 4;
        if ((UINT32) Index >= ColorMapNum) {
          goto Error;
        }
        Blt->Red    = BmpColorMap[Index].Red;
        Blt->Green  = BmpColorMap[Index].Green;
        Blt->Blue   = BmpColorMap[Index].Blue;
        if (WidthIndex < (BmpHeader->PixelWidth - 1)) {
          Blt++;
          WidthIndex++;
          Index       = (*Image) & 0x0f;
          if ((UINT32) Index >= ColorMapNum) {
            goto Error;
          }
          Blt->Red    = BmpColorMap[Index].Red;
          Blt->Green  = BmpColorMap[Index].Green;
          Blt->Blue   = BmpColorMap[Index].Blue;
        }
        break;

      case 8:
        //
        // Convert 8-bit (256 colors) BMP Palette to 24-bit color
        //
        if ((UINT32) *Image >= ColorMapNum) {
          goto Error;
        }
        Blt->Red    = BmpColorMap[*Image].Red;
        Blt->Green  = BmpColorMap[*Image].Green;
        Blt->Blue   = BmpColorMap[*Image].Blue;
        break;

      case 16:
        Blt->Blue  = RGB555_TO_RGB888 (((*((UINT16 *) Image))      ) & 0x1F);
        Blt->Green = RGB555_TO_RGB888 (((*((UINT16 *) Image)) >> 5 ) & 0x1F);
        Blt->Red   = RGB555_TO_RGB888 (((*((UINT16 *) Image)) >> 10) & 0x1F);
        Image++;
        break;

      case 24:
        Blt->Blue   = *Image++;
        Blt->Green  = *Image++;
        Blt->Red    = *Image;
        break;

      case 32:
        Blt->Blue   = *Image++;
        Blt->Green  = *Image++;
        Blt->Red    = *Image++;
        break;

      default:
        goto Error;
      }

    }

    ImageIndex = (UINTN) (Image - ImageHeader);
    if ((ImageIndex % 4) != 0) {
      //
      // Bmp Image starts each row on a 32-bit boundary!
      //
      Image = Image + (4 - (ImageIndex % 4));
    }
  }

  return EFI_SUCCESS;

Error:
  if (IsAllocated) {
   FreePool (*DecodedData);
   *DecodedData = NULL;
  }
  return EFI_UNSUPPORTED;
}

/**
 Install Driver to produce PCX Decoder protocol.

 @param [in]   ImageHandle
 @param [in]   SystemTable

 @retval EFI_SUCCESS            PCX Decoder protocol installed
 @return Other        No protocol installed, unload driver.

**/
EFI_STATUS
EFIAPI
BmpDecoderInstall (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS              Status;

  Status = gBS->InstallProtocolInterface (
                  &mPrivateData.Handle,
                  &gEfiBmpDecoderProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mPrivateData.BmpDecoder
                  );
  return Status;
}
