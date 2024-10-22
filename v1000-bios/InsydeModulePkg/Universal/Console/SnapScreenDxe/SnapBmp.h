/** @file
  SnapScree driver for capature screen image to BMP file.

;******************************************************************************
;* Copyright (c) 2012 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#ifndef _SNAP_BMP_FILE_
#define _SNAP_BMP_FILE_

#include <Uefi.h>
#include <IndustryStandard/Bmp.h>
#include <Protocol/GraphicsOutput.h>

#define EFI_FILE_STRING_LENGTH  255
#define MAX_FILE_INFO_SIZE      sizeof(EFI_FILE_INFO)+ EFI_FILE_STRING_LENGTH
#define MAX_VGA_MODE_WIDTH      0
#define MAX_VGA_MODE_HEIGHT     0

// TimeFileName example : 20120904144503.BMP  (18 char + 1 NULL)
#define BMP_TIME_FILE_NAME_SIZE  (19*sizeof(CHAR16))

//
//  EFI_FIELD_OFFSET - returns the byte offset to a field within a structure
//
#define EFI_FIELD_OFFSET(TYPE,Field) ((UINTN)(&(((TYPE *) 0)->Field)))


typedef struct _BMP_FILE {
  EFI_HANDLE                        FsHandle;
  CHAR16                            *FileName;
  BMP_IMAGE_HEADER                  *BmpHeader;
  UINT8                             *BmpImage;
} BMP_FILE;

EFI_STATUS
ObtainBmpFileNameByTime (
  CHAR16    *FileName,
  UINTN     Size
  );

EFI_STATUS
BmpCreate (
  EFI_HANDLE    FsHandle,
  CHAR16        *FileName,
  BMP_FILE      **ABmpFile
  );


EFI_STATUS
BmpBltBufferToImage (
  BMP_FILE                        *Bmp,
  UINTN                           Width,
  UINTN                           Height,
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *BltBuffer,
  UINTN                           BufferWidth
  );

EFI_STATUS
BmpScreenToImage (
  BMP_FILE      *Bmp,
  UINTN         X,
  UINTN         Y,
  UINTN         Width,
  UINTN         Height
  );

EFI_STATUS
BmpSaveToFile (
  BMP_FILE      *Bmp
  );

EFI_STATUS
BmpDestroy (
  BMP_FILE      *Bmp
  );

#endif //  _SNAP_BMP_FILE_
