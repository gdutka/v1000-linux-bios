/** @file
  The instance of VgaPostCodeLib Library

;******************************************************************************
;* Copyright (c) 2016 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <VgaPostCodeLibInternal.h>

/**
 This function uses GOP frame buffer to show the character
 on the specified position of screen.

 @param [in] GraphicsOutput   The GOP interface
 @param [in] FontInfo         The font information 
 @param [in] StartAddr        The start address which date will be written to frame buffer
 @param [in] Data             The character which will be shown
 
 @retval EFI_SUCCESS   Character was successfully shown on the specified position

**/
EFI_STATUS
EFIAPI
ShowChar (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput,
  IN FONT_INFO                    *FontInfo,
  IN UINT32                       StartAddr,
  IN CHAR8                        Data
  )
{
  UINTN  FrameBufferBase;
  UINT32 PixelsPerScanLine;
	UINT32 PixelPerByte;
  UINT8  *FontEntry;
	UINT8  Height;
  UINT8  Width;
  UINT8  Temp;
	UINT32 WriteData;
	UINT32 DestAddr;

  //
  // Get Mod Information
  //
  FrameBufferBase   = GraphicsOutput->Mode->FrameBufferBase;
  PixelsPerScanLine = GraphicsOutput->Mode->Info->PixelsPerScanLine;
  PixelPerByte      = 4;

  //
  // Get the font
  //
  FontEntry = FontInfo->FontTable + (Data * FontInfo->FontHeight);
  
  //
  // Write font to frame buffer
  //
	for (Height = 0; Height < FontInfo->FontHeight; Height++) {					
	  Temp = *(UINT8 *) (FontEntry + Height);	
    DestAddr = Height * (PixelsPerScanLine * PixelPerByte) + StartAddr;
	  for (Width = 0; Width < FontInfo->FontWidth; Width++) {
		  if (Temp & 0x80) {
		    WriteData = FontInfo->FG;
		  } else {
		    WriteData = FontInfo->BG;
		  }

      *(UINT32 *) (FrameBufferBase + DestAddr) = WriteData;
      DestAddr += PixelPerByte; 
		  Temp <<= 1;   
    }   
  }

  return EFI_SUCCESS;
}

/**
 This function uses GOP frame buffer to show the input string 
 on the specified position of screen.

 @param [in] GraphicsOutput   The GOP interface
 @param [in] DenstinationX    The X coordinate of position
 @param [in] DenstinationY    The Y coordinate of position
 @param [in] WordColor        The string word color
 @param [in] BackGroundColor  The strgig background color
 @param [in] String           The string which will be shown
 
 @retval EFI_SUCCESS   String was successfully shown on the specified position
 @return Others        Show string error

**/
EFI_STATUS
EFIAPI
ShowString (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput,
  IN UINT16                       DenstinationX,
  IN UINT16                       DenstinationY,
  IN UINT32                       WordColor,
  IN UINT32                       BackGroundColor,
  IN CHAR8                        *String
  )
{
  FONT_INFO                            FontInfo;  
  UINT32                               StrLen;
  UINT32                               Index; 
  UINT32                               StartAddr; 
  UINT32                               DestAddr;
	UINT32                               PixelPerByte;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *ModInfo;
  
  //
  // Get Mod Information
  //
  ModInfo = GraphicsOutput->Mode->Info;
  
  if (ModInfo->PixelFormat == PixelRedGreenBlueReserved8BitPerColor ||
      ModInfo->PixelFormat == PixelBlueGreenRedReserved8BitPerColor) {
    PixelPerByte = 4;
  } else {
    PixelPerByte = 0;
    return EFI_UNSUPPORTED;
  }
  
  //
  // Init Font Table
  //
  FontInfo.FontTable  = Font8X16;
  FontInfo.FontWidth  = 8;
  FontInfo.FontHeight = 16; 
  FontInfo.FG         = WordColor;
  FontInfo.BG         = BackGroundColor;

  //
  // Init StartAddr
  //
  StartAddr = (DenstinationY * FontInfo.FontHeight) * (ModInfo->PixelsPerScanLine * PixelPerByte) +
              (DenstinationX * FontInfo.FontWidth) * PixelPerByte;

  //
  // Write string to frame buffer
  //
  StrLen = (UINT32) AsciiStrLen (String);
  for (Index = 0; Index < StrLen; Index++) {
	  DestAddr = StartAddr + Index * FontInfo.FontWidth * PixelPerByte;
	  ShowChar (GraphicsOutput, &FontInfo, DestAddr, String[Index]);    
  }   
  
  return EFI_SUCCESS;
}

/**
 This function uses GOP frame buffer to show the input string 
 on the lower right corner of screen.

 @param [in] GraphicsOutput   The GOP interface
 @param [in] String           The string which will be shown
 
 @retval EFI_SUCCESS   String was successfully shown
 @return Others        Show string error

**/
EFI_STATUS
EFIAPI
ShowStringLowerRightCorner (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput,
  IN CHAR8                        *String,
  IN UINTN                        CharCount
  )
{
  UINT32     MaxCol;
  UINT32     MaxRow;
  EFI_STATUS Status; 
  
  //
  // Get max column and row of screen in 8X16 Font
  //
  MaxCol = GraphicsOutput->Mode->Info->HorizontalResolution / 8;
  MaxRow = GraphicsOutput->Mode->Info->VerticalResolution / 16;

  //
  // Show PostCode string on screen
  //
  Status = ShowString (
             GraphicsOutput,
             (UINT16) (MaxCol - CharCount - 2), 
             (UINT16) (MaxRow - 2), 
             RGB_WHITE, 
             RGB_BLACK, 
             String
             );
  
  return Status;
}

/**
 This function uses GOP frame buffer to show the PostCode string
 on the lower right corner of screen.

 @param [in] GraphicsOutput   The GOP interface
 @param [in] PostCode         The PostCode value of the PostCode string
 
 @retval EFI_SUCCESS   PostCode string was successfully shown
 @return Others        Show string error

**/
EFI_STATUS
EFIAPI
VgaShowPostCode (
  EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput,
  UINTN                        PostCode
  )
{ 
  CHAR8      String[BUFFER_SIZE];
  EFI_STATUS Status;
  UINTN      CharCount;
  
  //
  // Transfer PostCode to ASCII string
  // 
  CharCount = GetPostCodeString (
                ScreenDisplayStr,
                PostCode,
                BUFFER_SIZE,
                String
                );

  //
  // Print string on the lower right corner of screen
  //
  Status = ShowStringLowerRightCorner (GraphicsOutput, String, CharCount);

  return Status;
}

/**
 This function uses GOP frame buffer to clean the PostCode string 
 which was printed on the lower right corner of screen.

 @param [in] GraphicsOutput   The GOP interface

 @retval EFI_SUCCESS   PostCode string was successfully cleaned
 @return Others        Show string error

**/
EFI_STATUS
EFIAPI
VgaClearPostCode (
  EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput
  )
{
  CHAR8      String[BUFFER_SIZE];
  EFI_STATUS Status;
  UINTN      CharCount;
    
  //
  // Transfer string to ASCII string
  //  
  CharCount = GetPostCodeString (
                ScreenClearStr,
                0,
                BUFFER_SIZE,
                String
                );

  //
  // Print string on the lower right corner of screen
  //
  Status = ShowStringLowerRightCorner (GraphicsOutput, String, CharCount);

  return Status;  
}

