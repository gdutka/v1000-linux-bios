/** @file
  Process BDS hot key description.

;******************************************************************************
;* Copyright (c) 2016 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "Bds.h"

H2O_HOT_KEY_DESCRIPTION        mDefaultHotKeyDescPosn = {0, 0,
                                                         {0xFF, 0xFF, 0xFF, 0x00}, {0x00, 0x00, 0x00, 0x00},
                                                         NULL, EFI_GLYPH_HEIGHT, 0,
                                                         0, H2O_BDS_DISPLAY_UNIT_CHAR, H2O_BDS_DISPLAY_ORIGIN_X_LEFT,
                                                         0, H2O_BDS_DISPLAY_UNIT_CHAR, H2O_BDS_DISPLAY_ORIGIN_Y_BOTTOM
                                                         };
H2O_HOT_KEY_DESCRIPTION        mDefaultHotKeyDescQuietPosn = {0, 0,
                                                              {0xFF, 0xFF, 0xFF, 0x00}, {0x00, 0x00, 0x00, 0x00},
                                                              NULL, EFI_GLYPH_HEIGHT, 0,
                                                              0, H2O_BDS_DISPLAY_UNIT_CHAR, H2O_BDS_DISPLAY_ORIGIN_X_LEFT,
                                                              0, H2O_BDS_DISPLAY_UNIT_CHAR, H2O_BDS_DISPLAY_ORIGIN_Y_BOTTOM
                                                              };

STATIC EFI_HII_FONT_PROTOCOL   *mHiiFont                     = NULL;
STATIC BOOLEAN                 mIsConOutConnectAfterTriggered  = FALSE;
STATIC  EFI_GUID               mDisplayHotKeyAfterSelectGuid = { 0x3d21409e, 0x77b6, 0x4a47, { 0xbf, 0x0b, 0xcb, 0x05, 0xc2, 0x04, 0x65, 0x97 } };

typedef struct {
  EFI_GUID                     *TokenSpaceGuid;
  H2O_HOT_KEY_DESCRIPTION      *DefaultHotKeyDesc;
  UINTN                        HotKeyOptionOffset;
} PCD_HOT_KEY_DESC_INFO;

STATIC PCD_HOT_KEY_DESC_INFO   mPcdHotKeyDescInfoList[] = {
                                 {&gH2OBdsHotKeyDescGuid     , &mDefaultHotKeyDescPosn     , OFFSET_OF(H2O_HOT_KEY_OPTION, Description)     },
                                 {&gH2OBdsHotKeyDescQuietGuid, &mDefaultHotKeyDescQuietPosn, OFFSET_OF(H2O_HOT_KEY_OPTION, QuietDescription)},
                                 };

typedef
EFI_STATUS
(*DESCRIPTION_ATTRIBUTE_FUNCTION) (
  IN  CHAR8                            *NameAsciiStr,
  IN  CHAR8                            *ValueAsciiStr,
  OUT H2O_HOT_KEY_DESCRIPTION          *HotKeyDesc
  );

typedef struct {
  CHAR8                                *AttributeName;
  DESCRIPTION_ATTRIBUTE_FUNCTION       AttributeFn;
} DESCRIPTION_ATTRIBUTE_TABLE;


/**
 Check if system is in text mode or not.

 @retval TRUE       System is in text mode.
 @retval FALSE      System is not in text mode.
**/
BOOLEAN
IsTextMode (
  VOID
  )
{
  EFI_STATUS                           Status;
  KERNEL_CONFIGURATION                 KernelConfig;
  UINT8                                *DisableQuietBoot;

  Status = GetKernelConfiguration (&KernelConfig);
  if (EFI_ERROR (Status) || KernelConfig.QuietBoot == 0) {
    return TRUE;
  }

  Status = gBS->LocateProtocol (
                  &gEndOfDisableQuietBootGuid,
                  NULL,
                  (VOID **) &DisableQuietBoot
                  );
  if (!EFI_ERROR (Status)) {
    return TRUE;
  }

  return FALSE;
}

/**
 Check if input token number is exist in the input token number list.

 @param[in] TokenList                  Pointer to token number list
 @param[in] TokenListCount             The count of token number list
 @param[in] TokenNum                   Target token number

 @retval TRUE   Input ascii string is valid value string.
 @retval FALSE  Input ascii string is not valid value string.
**/
STATIC
BOOLEAN
IsTokenInList (
  IN UINTN                             *TokenList,
  IN UINTN                             TokenListCount,
  IN UINTN                             TokenNum
  )
{
  UINTN                                     Index;

  if (TokenList == NULL) {
    return FALSE;
  }

  for (Index = 0; Index < TokenListCount; Index++) {
    if (TokenList[Index] == TokenNum) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
 Check if boot display mode is valid or not.

 @param[in] BootDisplayMode            Boot display mode value

 @retval TRUE   Boot display mode is valid.
 @retval FALSE  Boot display mode is not valid.
**/
STATIC
BOOLEAN
IsValidBootDisplayMode (
  IN UINT8                             BootDisplayMode
  )
{
  if (BootDisplayMode == H2O_BDS_BOOT_DISPLAY_MODE_NORMAL ||
      BootDisplayMode == H2O_BDS_BOOT_DISPLAY_MODE_QUIET  ||
      BootDisplayMode == H2O_BDS_BOOT_DISPLAY_MODE_ALL) {
    return TRUE;
  }

  return FALSE;
}

/**
 Check if input value is valid position value.

 @param[in] Value                Position value
 @param[in] ValueUnit            Position value unit

 @retval TRUE   Input value is valid position.
 @retval FALSE  Input value is valid position.
**/
STATIC
BOOLEAN
IsValidPosn (
  IN INT32                             Value,
  IN UINT32                            ValueUnit
  )
{
  switch (ValueUnit) {

  case H2O_BDS_DISPLAY_UNIT_CHAR:
  case H2O_BDS_DISPLAY_UNIT_PIXEL:
    break;

  case H2O_BDS_DISPLAY_UNIT_PERCENT:
    if (Value > 100 || Value < -100) {
      return FALSE;
    }
    break;

  default:
    return FALSE;
  }

  return TRUE;
}

/**
 Check if it is valid XOrigin value.

 @param[in] XOriginValue               XOrigin value

 @retval TRUE   It is valid XOrigin value.
 @retval FALSE  It is not valid XOrigin value.
**/
STATIC
BOOLEAN
IsValidXOriginPosn (
  IN UINT32                            XOriginValue
  )
{
  switch (XOriginValue & ORIGIN_ACTION_MASK) {

  case H2O_BDS_DISPLAY_ORIGIN_X_DEFAULT_POSN:
  case H2O_BDS_DISPLAY_ORIGIN_X_CENTER:
  case H2O_BDS_DISPLAY_ORIGIN_X_DEFAULT:
    if ((XOriginValue & ORIGIN_VALUE_MASK) != 0) {
      return FALSE;
    }
    break;

  case H2O_BDS_DISPLAY_ORIGIN_X_LEFT:
  case H2O_BDS_DISPLAY_ORIGIN_X_RIGHT:
    if ((XOriginValue & ORIGIN_VALUE_MASK) > 100) {
      return FALSE;
    }
    break;

  default:
    return FALSE;
  }

  return TRUE;
}

/**
 Check if it is valid YOrigin value.

 @param[in] XOriginValue               YOrigin value

 @retval TRUE   It is valid YOrigin value.
 @retval FALSE  It is not valid YOrigin value.
**/
STATIC
BOOLEAN
IsValidYOriginPosn (
  IN UINT32                            YOriginValue
  )
{
  switch (YOriginValue & ORIGIN_ACTION_MASK) {

  case H2O_BDS_DISPLAY_ORIGIN_Y_DEFAULT_POSN:
  case H2O_BDS_DISPLAY_ORIGIN_Y_CENTER:
  case H2O_BDS_DISPLAY_ORIGIN_Y_DEFAULT:
    if ((YOriginValue & ORIGIN_VALUE_MASK) != 0) {
      return FALSE;
    }
    break;

  case H2O_BDS_DISPLAY_ORIGIN_Y_TOP:
  case H2O_BDS_DISPLAY_ORIGIN_Y_BOTTOM:
    if ((YOriginValue & ORIGIN_VALUE_MASK) > 100) {
      return FALSE;
    }
    break;

  default:
    return FALSE;
  }

  return TRUE;
}

/**
 Check if input ascii string is valid value string.

 @param[in] AsciiStr                  Pointer to ascii string
 @param[in] IsHex                     A Boolean that specifies string is hex or not

 @retval TRUE   Input ascii string is valid value string.
 @retval FALSE  Input ascii string is not valid value string.
**/
STATIC
BOOLEAN
IsValidValueAsciiStr (
  IN CHAR8                             *AsciiStr,
  IN BOOLEAN                           IsHex
  )
{
  CHAR8                                *AsciiStrPtr;

  if (AsciiStr == NULL || *AsciiStr == CHAR_NULL) {
    return FALSE;
  }

  AsciiStrPtr = AsciiStr;
  while (*AsciiStrPtr != CHAR_NULL) {
    if ((*AsciiStrPtr >= '0' && *AsciiStrPtr <= '9') ||
        (IsHex && ((*AsciiStrPtr >= 'a' && *AsciiStrPtr <= 'f') ||
                   (*AsciiStrPtr >= 'A' && *AsciiStrPtr <= 'F')))) {
      AsciiStrPtr++;
      continue;
    }
    return FALSE;
  }

  return TRUE;
}

/**
 Transfer ascii string to lower case.

 @param[in, out] AsciiStr              Pointer to ascii string
**/
STATIC
VOID
AsciiStrToLower (
  IN OUT CHAR8                         *AsciiStr
  )
{
  while (*AsciiStr != CHAR_NULL) {
    if (*AsciiStr >= 'A' && *AsciiStr <= 'Z') {
      *AsciiStr = (CHAR8) (*AsciiStr - 'A' + 'a');
    }
    AsciiStr++;
  }
}

/**
 Transfer GOP Blt color to simple text output color attribute.

 @param[in] GopForegroundColor         Pointer to GOP Blt foreground color
 @param[in] GopBackgroundColor         Pointer to GOP Blt background color

 @return Simple text output color attribute.
**/
STATIC
UINTN
GopColorToTextOutColor (
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL     *GopForegroundColor,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL     *GopBackgroundColor
  )
{
  UINT8                                TextOutForegroundColor;
  UINT8                                TextOutBackgroundColor;

  //
  // Refer from DisplayUpdateProgress() in MdeModulePkg.
  //
  TextOutForegroundColor = EFI_BLACK;
  if (GopForegroundColor->Blue >= 0x40) {
    TextOutForegroundColor |= EFI_BLUE;
  }
  if (GopForegroundColor->Green >= 0x40) {
    TextOutForegroundColor |= EFI_GREEN;
  }
  if (GopForegroundColor->Red >= 0x40) {
    TextOutForegroundColor |= EFI_RED;
  }
  if (GopForegroundColor->Blue >= 0xC0 || GopForegroundColor->Green >= 0xC0 || GopForegroundColor->Red >= 0xC0) {
    TextOutForegroundColor |= EFI_BRIGHT;
  }

  TextOutBackgroundColor = EFI_BLACK;
  if (GopBackgroundColor->Blue >= 0x40) {
    TextOutBackgroundColor |= EFI_BLUE;
  }
  if (GopBackgroundColor->Green >= 0x40) {
    TextOutBackgroundColor |= EFI_GREEN;
  }
  if (GopBackgroundColor->Red >= 0x40) {
    TextOutBackgroundColor |= EFI_RED;
  }

  return EFI_TEXT_ATTR (TextOutForegroundColor, TextOutBackgroundColor);
}

/**
 Transfer GOP Blt position to simple text output position.

 @param[in] GopPosn                    GOP Blt position
 @param[in] GopResolution              GOP Blt resolution
 @param[in] TextOutResolution          Simple text output resolution

 @return Simple text output position.
**/
STATIC
UINTN
GopPosnToTextOutPosn (
  IN UINTN                             GopPosn,
  IN UINTN                             GopResolution,
  IN UINTN                             TextOutResolution
  )
{
  UINTN                                Delta;

  if (GopResolution == 0 || TextOutResolution == 0) {
    return 0;
  }

  Delta = (GopResolution % TextOutResolution) / 2;

  if (GopPosn < Delta) {
    return 0;
  } else if (GopPosn >= (GopResolution - Delta)) {
    return TextOutResolution - 1;
  } else {
    return ((GopPosn - Delta) * TextOutResolution) / GopResolution;
  }
}

/**
 Normalize ascii string to remove prefix and suffix space characters and transfer character to lower case.

 @param[in, out] AsciiStr              Pointer to ascii string
**/
STATIC
VOID
NormalizeAsciiStr (
  IN OUT CHAR8                         *AsciiStr
  )
{
  UINTN                                Length;

  if (AsciiStr == NULL) {
    return;
  }

  Length = AsciiStrLen (AsciiStr);

  while (Length > 0 && AsciiStr[0] == ' ') {
    CopyMem (&AsciiStr[0], &AsciiStr[1], Length - 1);
    AsciiStr[Length - 1] = CHAR_NULL;
    Length--;
  }

  while (Length > 0 && AsciiStr[Length - 1] == ' ') {
    AsciiStr[Length - 1] = CHAR_NULL;
    Length--;
  }

  AsciiStrToLower (AsciiStr);
}

/**
 Get value by parsing ascii string.

 @param[in]  AsciiStr             Pointer to value ascii string
 @param[in]  UnitAsciiStr         Pointer to unit ascii string
 @param[out] Value                Pointer to value

 @retval EFI_SUCCESS              Get value successfully.
 @retval EFI_INVALID_PARAMETER    Input paramter in NULL.
 @retval EFI_ABORTED              Unit ascii string can not find in value ascii string or value ascii string is invalid.
**/
STATIC
EFI_STATUS
GetValueFromAsciiStr (
  IN  CHAR8                            *AsciiStr,
  IN  CHAR8                            *UnitAsciiStr OPTIONAL,
  OUT INT64                            *Value
  )
{
  CHAR8                                *AsciiStrPtr;
  BOOLEAN                              IsPositive;
  BOOLEAN                              IsHex;
  CHAR8                                *UnitAsciiStrPtr;
  CHAR8                                *ValueAsciiStr;
  CHAR8                                Buffer[30];
  UINTN                                ValueAsciiStrLen;
  UINTN                                ResultValue;

  if (AsciiStr == NULL || Value == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Syntax of string is ::== <signed-integer>[<UnitStr>]
  // For example: Return value should be 10 when String is "10%" and UnitStr is "%".
  //
  AsciiStrPtr = AsciiStr;
  if (*AsciiStrPtr == '-') {
    AsciiStrPtr++;
    IsPositive = FALSE;
  } else {
    IsPositive = TRUE;
  }

  if (AsciiStrPtr[0] == '0' && (AsciiStrPtr[1] == 'x' || AsciiStrPtr[1] == 'X')) {
    AsciiStrPtr += 2;
    IsHex = TRUE;
  } else {
    IsHex = FALSE;
  }

  if (UnitAsciiStr == NULL) {
    ValueAsciiStr = AsciiStrPtr;
  } else {
    UnitAsciiStrPtr = AsciiStrStr (AsciiStrPtr, UnitAsciiStr);
    if (UnitAsciiStrPtr == NULL || AsciiStrCmp (UnitAsciiStrPtr, UnitAsciiStr) != 0) {
      return EFI_ABORTED;
    }

    ValueAsciiStrLen = (UINTN) (UnitAsciiStrPtr - AsciiStrPtr);
    if (ValueAsciiStrLen + 1 > sizeof(Buffer)) {
      return EFI_ABORTED;
    }

    CopyMem (Buffer, AsciiStrPtr, ValueAsciiStrLen);
    Buffer[ValueAsciiStrLen] = CHAR_NULL;
    ValueAsciiStr = Buffer;
  }

  if (!IsValidValueAsciiStr (ValueAsciiStr, IsHex)) {
    return EFI_ABORTED;
  }

  ResultValue = IsHex ? AsciiStrHexToUintn (ValueAsciiStr) : AsciiStrDecimalToUintn (ValueAsciiStr);
  *Value      = IsPositive ? (INT64) ResultValue : -((INT64) ResultValue);

  return EFI_SUCCESS;
}

/**
 Get font display information.

 @param[in] HotKeyDesc                 Pointer to hot key description

 @return Pointer to font display information or NULL if input parameter is NULL or allocate memory failed.
**/
EFI_FONT_DISPLAY_INFO *
GetFontDisplayInfo (
  IN H2O_HOT_KEY_DESCRIPTION           *HotKeyDesc
  )
{
  EFI_FONT_DISPLAY_INFO                *FontDisplayInfo;
  UINTN                                FontNameAsciiStrLen;

  if (HotKeyDesc == NULL) {
    return NULL;
  }

  FontNameAsciiStrLen = (HotKeyDesc->FontName != NULL) ? AsciiStrLen (HotKeyDesc->FontName) : 0;
  FontDisplayInfo     = AllocateZeroPool (sizeof (EFI_FONT_DISPLAY_INFO) + (FontNameAsciiStrLen * sizeof (CHAR16)));
  if (FontDisplayInfo == NULL) {
    return NULL;
  }

  CopyMem (&FontDisplayInfo->ForegroundColor, &HotKeyDesc->ForegroundColor, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  CopyMem (&FontDisplayInfo->BackgroundColor, &HotKeyDesc->BackgroundColor, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  FontDisplayInfo->FontInfoMask = 0;
  FontDisplayInfo->FontInfo.FontSize  = HotKeyDesc->FontSize;
  FontDisplayInfo->FontInfo.FontStyle = HotKeyDesc->FontStyle;
  if (HotKeyDesc->FontName != NULL) {
    AsciiStrToUnicodeStrS (HotKeyDesc->FontName, FontDisplayInfo->FontInfo.FontName, FontNameAsciiStrLen + 1);
  }

  return FontDisplayInfo;
}

/**
 Get current screen resolution.

 @param[out] HorizontalResolution  Pointer to horizontal resolution
 @param[out] VerticalResolution    Pointer to vertical resolution

 @retval EFI_SUCCESS               Get resolution successfully.
 @retval EFI_INVALID_PARAMETER     Input paramter in NULL.
 @retval Other                     Get resolution failed.
**/
STATIC
EFI_STATUS
GetScreenResolution (
  OUT UINT32                           *HorizontalResolution,
  OUT UINT32                           *VerticalResolution
  )
{
  EFI_STATUS                           Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL         *GraphicsOutput;
  EFI_UGA_DRAW_PROTOCOL                *UgaDraw;
  UINT32                               ColorDepth;
  UINT32                               RefreshRate;

  if (HorizontalResolution == NULL || VerticalResolution == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->HandleProtocol (
                  gST->ConsoleOutHandle,
                  &gEfiGraphicsOutputProtocolGuid,
                  (VOID **) &GraphicsOutput
                  );
  if (!EFI_ERROR (Status)) {
    *HorizontalResolution = GraphicsOutput->Mode->Info->HorizontalResolution;
    *VerticalResolution   = GraphicsOutput->Mode->Info->VerticalResolution;
    return EFI_SUCCESS;
  }

  if (FeaturePcdGet (PcdUgaConsumeSupport)) {
    Status = gBS->HandleProtocol (
                    gST->ConsoleOutHandle,
                    &gEfiUgaDrawProtocolGuid,
                    (VOID **) &UgaDraw
                    );
    if (!EFI_ERROR (Status)) {
      return UgaDraw->GetMode (UgaDraw, HorizontalResolution, VerticalResolution, &ColorDepth, &RefreshRate);
    }
  }

  return Status;
}

/**
 Based on font information, calculate the display area of input string.

 @param[in]  DisplayString         Pointer to display string
 @param[in]  FontInfo              Pointer to font information
 @param[out] DisplayWidth          Pointer to the width of display area
 @param[out] DisplayHeight         Pointer to the height of display area

 @retval EFI_SUCCESS               Get resolution successfully.
 @retval EFI_INVALID_PARAMETER     Input paramter in NULL.
 @retval Other                     Get resolution failed.
**/
EFI_STATUS
GetDisplayArea (
  IN  CHAR16                           *DisplayString,
  IN  EFI_FONT_DISPLAY_INFO            *FontInfo,
  OUT UINT32                           *DisplayWidth,
  OUT UINT32                           *DisplayHeight
  )
{
  EFI_STATUS                           Status;
  UINT32                               HorizontalResolution;
  UINT32                               VerticalResolution;
  EFI_IMAGE_OUTPUT                     *Blt;
  EFI_HII_ROW_INFO                     *RowInfoArray;
  UINTN                                RowInfoArraySize;
  EFI_GRAPHICS_OUTPUT_PROTOCOL         *GraphicsOutput;
  UINTN                                Index;
  UINTN                                Width;
  UINTN                                Height;

  if (FontInfo == NULL || DisplayString == NULL || DisplayWidth == NULL || DisplayHeight == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **) &GraphicsOutput);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  HorizontalResolution = GraphicsOutput->Mode->Info->HorizontalResolution;
  VerticalResolution   = GraphicsOutput->Mode->Info->VerticalResolution;

  Blt = AllocateZeroPool (sizeof(EFI_IMAGE_OUTPUT));
  if (Blt == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Blt->Width        = (UINT16) HorizontalResolution;
  Blt->Height       = (UINT16) VerticalResolution;
  Blt->Image.Bitmap = AllocateZeroPool (HorizontalResolution * VerticalResolution * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  if (Blt->Image.Bitmap == NULL) {
    FreePool (Blt);
    return EFI_OUT_OF_RESOURCES;
  }

  RowInfoArray     = NULL;
  RowInfoArraySize = 0;
  Status = mHiiFont->StringToImage (
                       mHiiFont,
                       EFI_HII_IGNORE_IF_NO_GLYPH | EFI_HII_OUT_FLAG_CLIP |
                       EFI_HII_OUT_FLAG_CLIP_CLEAN_X | EFI_HII_OUT_FLAG_CLIP_CLEAN_Y,
                       DisplayString,
                       FontInfo,
                       &Blt,
                       0,
                       0,
                       &RowInfoArray,
                       &RowInfoArraySize,
                       NULL
                       );
  FreePool (Blt->Image.Bitmap);
  FreePool (Blt);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (RowInfoArray == NULL) {
    return EFI_NOT_FOUND;
  }

  Height = 0;
  Width  = 0;
  for (Index = 0; Index < RowInfoArraySize; Index++) {
    Height += RowInfoArray[Index].LineHeight;
    Width  = MAX (Width, RowInfoArray[Index].LineWidth);
  }

  *DisplayWidth  = (UINT32) Width;
  *DisplayHeight = (UINT32) Height;

  FreePool (RowInfoArray);
  return Status;
}

/**
 Get screen display region of input string.

 @param[in]  DisplayString         Pointer to display string
 @param[in]  HotKeyDesc            Pointer to hot key description
 @param[out] DisplayHeight         Pointer to screen region

 @retval EFI_SUCCESS               Get resolution successfully.
 @retval EFI_INVALID_PARAMETER     Input paramter in NULL.
 @retval EFI_ABORTED               Get font display info failed.
 @retval EFI_UNSUPPORTED           One of descrition attribute value is invalid.
 @retval Other                     Get resolution failed.
**/
STATIC
EFI_STATUS
GetDisplayRegion (
  IN  H2O_HOT_KEY_DESCRIPTION               *HotKeyDesc,
  IN  CHAR16                                *DisplayString,
  OUT RECT                                  *Region
  )
{
  EFI_STATUS                                Status;
  UINT32                                    StrDisplayWidth;
  UINT32                                    StrDisplayHeight;
  INT32                                     Offset;
  INT32                                     ResultX;
  INT32                                     ResultY;
  UINT32                                    HorizontalResolution;
  UINT32                                    VerticalResolution;
  UINT32                                    Percentage;
  UINT32                                    OriginAction;
  EFI_FONT_DISPLAY_INFO                     *FontDisplayInfo;

  if (HotKeyDesc == NULL || DisplayString == NULL || Region == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetScreenResolution (&HorizontalResolution, &VerticalResolution);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  FontDisplayInfo = GetFontDisplayInfo (HotKeyDesc);
  if (FontDisplayInfo == NULL) {
    return EFI_ABORTED;
  }

  //
  // Get X position
  //
  switch (HotKeyDesc->XUnit) {

  case H2O_BDS_DISPLAY_UNIT_CHAR:
    GetDisplayArea (L"M", FontDisplayInfo, &StrDisplayWidth, &StrDisplayHeight);
    ResultX = HotKeyDesc->X * ((INT32) (UINTN) StrDisplayWidth);
    break;

  case H2O_BDS_DISPLAY_UNIT_PIXEL:
    ResultX = HotKeyDesc->X;
    break;

  case H2O_BDS_DISPLAY_UNIT_PERCENT:
    ResultX = (INT32) (DivU64x32 (MultU64x32 ((UINT64) HorizontalResolution, (UINT32) HotKeyDesc->X), 100));
    break;

  default:
    FreePool (FontDisplayInfo);
    return EFI_UNSUPPORTED;
  }

  GetDisplayArea (DisplayString, FontDisplayInfo, &StrDisplayWidth, &StrDisplayHeight);
  FreePool (FontDisplayInfo);

  OriginAction = HotKeyDesc->XOrigin & ORIGIN_ACTION_MASK;
  Percentage   = HotKeyDesc->XOrigin & ORIGIN_VALUE_MASK;
  switch (OriginAction) {

  case H2O_BDS_DISPLAY_ORIGIN_X_LEFT:
    if (!IS_VALID_PERCENTAGE(Percentage)) {
      return EFI_UNSUPPORTED;
    }

    Offset = (INT32) (DivU64x32 (MultU64x32 ((UINT64) HorizontalResolution, Percentage), 100));
    break;

  case H2O_BDS_DISPLAY_ORIGIN_X_CENTER:
    if (Percentage != 0) {
      return EFI_UNSUPPORTED;
    }
    Offset  = (INT32) (DivU64x32 ((UINT64) HorizontalResolution, 2));
    Offset -= (StrDisplayWidth / 2);
    break;

  case H2O_BDS_DISPLAY_ORIGIN_X_RIGHT:
    if (!IS_VALID_PERCENTAGE(Percentage)) {
      return EFI_UNSUPPORTED;
    }

    Offset = HorizontalResolution - (INT32) (DivU64x32 (MultU64x32 ((UINT64) HorizontalResolution, Percentage), 100));
    Offset -= StrDisplayWidth;
    break;

  default:
    return EFI_UNSUPPORTED;
  }

  ResultX += Offset;
  if (ResultX + StrDisplayWidth > (INT32) HorizontalResolution) {
    ResultX = HorizontalResolution - StrDisplayWidth;
  }
  if (ResultX < 0) {
    ResultX = 0;
  }

  //
  // Get Y position
  //
  switch (HotKeyDesc->YUnit) {

  case H2O_BDS_DISPLAY_UNIT_CHAR:
    ResultY = (HotKeyDesc->Y * HotKeyDesc->FontSize);
    break;

  case H2O_BDS_DISPLAY_UNIT_PIXEL:
    ResultY = HotKeyDesc->Y;
    break;

  case H2O_BDS_DISPLAY_UNIT_PERCENT:
    ResultY = (INT32) (DivU64x32 (MultU64x32 ((UINT64) HotKeyDesc->Y, VerticalResolution), 100));
    break;

  default:
    return EFI_UNSUPPORTED;
  }

  OriginAction = HotKeyDesc->YOrigin & ORIGIN_ACTION_MASK;
  Percentage   = HotKeyDesc->YOrigin & ORIGIN_VALUE_MASK;
  switch (OriginAction) {

  case H2O_BDS_DISPLAY_ORIGIN_Y_TOP:
    if (!IS_VALID_PERCENTAGE(Percentage)) {
      return EFI_UNSUPPORTED;
    }

    Offset = (INT32) (DivU64x32 (MultU64x32 ((UINT64) VerticalResolution, Percentage), 100));
    break;

  case H2O_BDS_DISPLAY_ORIGIN_Y_CENTER:
    if (Percentage != 0) {
      return EFI_UNSUPPORTED;
    }

    Offset = (INT32) (DivU64x32 ((UINT64) VerticalResolution, 2));
    break;

  case H2O_BDS_DISPLAY_ORIGIN_Y_BOTTOM:
    if (!IS_VALID_PERCENTAGE(Percentage)) {
      return EFI_UNSUPPORTED;
    }

    Offset = VerticalResolution - (INT32) (DivU64x32 (MultU64x32 ((UINT64) VerticalResolution, Percentage), 100));
    Offset -= HotKeyDesc->FontSize;
    break;

  default:
    return EFI_UNSUPPORTED;
  }

  ResultY += Offset;
  if (ResultY + HotKeyDesc->FontSize > (INT32) VerticalResolution) {
    ResultY = VerticalResolution - HotKeyDesc->FontSize;
  }
  if (ResultY < 0) {
    ResultY = 0;
  }

  Region->left   = (INT32) ResultX;
  Region->top    = (INT32) ResultY;
  Region->right  = (INT32) (ResultX + StrDisplayWidth);
  Region->bottom = (INT32) (ResultY + StrDisplayHeight);

  return EFI_SUCCESS;
}

/**
 Get the hot key description of specific display mode by handle.

 @param[in] HotKeyHandle           Hot key description handle
 @param[in] BootDisplayMode        Boot display mode

 @return Pointer to hot key description or NULL if input parameter is NULL or handle is invalid.
**/
STATIC
H2O_HOT_KEY_DESCRIPTION *
GetHotKeyDesc (
  IN EFI_HANDLE                             HotKeyHandle,
  IN UINT8                                  BootDisplayMode
  )
{
  H2O_HOT_KEY_OPTION                        *HotKeyOption;

  if (HotKeyHandle == NULL) {
    return NULL;
  }

  HotKeyOption = GetHotKeyOption (HotKeyHandle);
  if (HotKeyOption == NULL) {
    return NULL;
  }

  switch (BootDisplayMode) {

  case H2O_BDS_BOOT_DISPLAY_MODE_NORMAL:
    return &HotKeyOption->Description;

  case H2O_BDS_BOOT_DISPLAY_MODE_QUIET:
    return &HotKeyOption->QuietDescription;
  }

  return NULL;
}

/**
 Attribute X Y process function to update (X, XUnit) or (Y, Yunit) value in hot key description.

 @param[in]  NameAsciiStr         Pointer to attribute name ascii string
 @param[in]  ValueAsciiStr        Pointer to attribute value ascii string
 @param[out] HotKeyDesc           Pointer to hot key description

 @retval EFI_SUCCESS              Update attribute successfully.
 @retval EFI_ABORTED              Value ascii string is invalid.
**/
STATIC
EFI_STATUS
DescAttributeFnXY (
  IN  CHAR8                            *NameAsciiStr,
  IN  CHAR8                            *ValueAsciiStr,
  OUT H2O_HOT_KEY_DESCRIPTION          *HotKeyDesc
  )
{
  INT64                                Value;
  UINT32                               DisplayUnit;

  ASSERT (NameAsciiStr  != NULL);
  ASSERT (ValueAsciiStr != NULL);
  ASSERT (HotKeyDesc    != NULL);

  if (GetValueFromAsciiStr (ValueAsciiStr, "em", &Value) == EFI_SUCCESS) {
    DisplayUnit = H2O_BDS_DISPLAY_UNIT_CHAR;
  } else if (GetValueFromAsciiStr (ValueAsciiStr, "px", &Value) == EFI_SUCCESS) {
    DisplayUnit = H2O_BDS_DISPLAY_UNIT_PIXEL;
  } else if (GetValueFromAsciiStr (ValueAsciiStr, "%", &Value) == EFI_SUCCESS) {
    DisplayUnit = H2O_BDS_DISPLAY_UNIT_PERCENT;
  } else {
    return EFI_ABORTED;
  }

  if (NameAsciiStr[0] == 'x') {
    HotKeyDesc->X     = (INT32) Value;
    HotKeyDesc->XUnit = DisplayUnit;
  } else {
    HotKeyDesc->Y     = (INT32) Value;
    HotKeyDesc->YUnit = DisplayUnit;
  }

  return EFI_SUCCESS;
}

/**
 Attribute XOrigin process function to update value in hot key description.

 @param[in]  NameAsciiStr         Pointer to attribute name ascii string
 @param[in]  ValueAsciiStr        Pointer to attribute value ascii string
 @param[out] HotKeyDesc           Pointer to hot key description

 @retval EFI_SUCCESS              Update attribute successfully.
 @retval EFI_ABORTED              Value ascii string is invalid.
**/
STATIC
EFI_STATUS
DescAttributeFnXOrigin (
  IN  CHAR8                            *NameAsciiStr,
  IN  CHAR8                            *ValueAsciiStr,
  OUT H2O_HOT_KEY_DESCRIPTION          *HotKeyDesc
  )
{
  INT64                                Value;
  EFI_STATUS                           Status;

  ASSERT (NameAsciiStr  != NULL);
  ASSERT (ValueAsciiStr != NULL);
  ASSERT (HotKeyDesc    != NULL);

  if (AsciiStrCmp (ValueAsciiStr, "left") == 0) {
    Value = H2O_BDS_DISPLAY_ORIGIN_X_LEFT;
  } else if (AsciiStrCmp (ValueAsciiStr, "center") == 0) {
    Value = H2O_BDS_DISPLAY_ORIGIN_X_CENTER;
  } else if (AsciiStrCmp (ValueAsciiStr, "right") == 0) {
    Value = H2O_BDS_DISPLAY_ORIGIN_X_RIGHT;
  } else if (AsciiStrnCmp (ValueAsciiStr, "left(", (sizeof("left(") - 1)) == 0) {
    Status = GetValueFromAsciiStr (&ValueAsciiStr[5], "%)", &Value);
    if (EFI_ERROR (Status) || !IS_VALID_PERCENTAGE(Value)) {
      return EFI_ABORTED;
    }
    Value = H2O_BDS_DISPLAY_ORIGIN_X_LEFT_PERCENT(Value);
  } else if (AsciiStrnCmp (ValueAsciiStr, "right(", (sizeof("right(") - 1)) == 0) {
    Status = GetValueFromAsciiStr (&ValueAsciiStr[6], "%)", &Value);
    if (EFI_ERROR (Status) || !IS_VALID_PERCENTAGE(Value)) {
      return EFI_ABORTED;
    }
    Value = H2O_BDS_DISPLAY_ORIGIN_X_RIGHT_PERCENT(Value);
  } else {
    Status = GetValueFromAsciiStr (ValueAsciiStr, "%", &Value);
    if (EFI_ERROR (Status) || !IS_VALID_PERCENTAGE(Value)) {
      return EFI_ABORTED;
    }
    Value = H2O_BDS_DISPLAY_ORIGIN_X_LEFT_PERCENT(Value);
  }

  HotKeyDesc->XOrigin = (UINT32) Value;

  return EFI_SUCCESS;
}

/**
 Attribute YOrigin process function to update value in hot key description.

 @param[in]  NameAsciiStr         Pointer to attribute name ascii string
 @param[in]  ValueAsciiStr        Pointer to attribute value ascii string
 @param[out] HotKeyDesc           Pointer to hot key description

 @retval EFI_SUCCESS              Update attribute successfully.
 @retval EFI_ABORTED              Value ascii string is invalid.
**/
STATIC
EFI_STATUS
DescAttributeFnYOrigin (
  IN  CHAR8                            *NameAsciiStr,
  IN  CHAR8                            *ValueAsciiStr,
  OUT H2O_HOT_KEY_DESCRIPTION          *HotKeyDesc
  )
{
  INT64                                Value;
  EFI_STATUS                           Status;

  ASSERT (NameAsciiStr  != NULL);
  ASSERT (ValueAsciiStr != NULL);
  ASSERT (HotKeyDesc    != NULL);

  if (AsciiStrCmp (ValueAsciiStr, "top") == 0) {
    Value = H2O_BDS_DISPLAY_ORIGIN_Y_TOP;
  } else if (AsciiStrCmp (ValueAsciiStr, "center") == 0) {
    Value = H2O_BDS_DISPLAY_ORIGIN_Y_CENTER;
  } else if (AsciiStrCmp (ValueAsciiStr, "bottom") == 0) {
    Value = H2O_BDS_DISPLAY_ORIGIN_Y_BOTTOM;
  } else if (AsciiStrnCmp (ValueAsciiStr, "top(", (sizeof("top(") - 1)) == 0) {
    Status = GetValueFromAsciiStr (&ValueAsciiStr[4], "%)", &Value);
    if (EFI_ERROR (Status) || !IS_VALID_PERCENTAGE(Value)) {
      return EFI_ABORTED;
    }
    Value = H2O_BDS_DISPLAY_ORIGIN_Y_TOP_PERCENT(Value);
  } else if (AsciiStrnCmp (ValueAsciiStr, "bottom(", (sizeof("bottom(") - 1)) == 0) {
    Status = GetValueFromAsciiStr (&ValueAsciiStr[7], "%)", &Value);
    if (EFI_ERROR (Status) || !IS_VALID_PERCENTAGE(Value)) {
      return EFI_ABORTED;
    }
    Value = H2O_BDS_DISPLAY_ORIGIN_Y_BOTTOM_PERCENT(Value);
  } else {
    Status = GetValueFromAsciiStr (ValueAsciiStr, "%", &Value);
    if (EFI_ERROR (Status) || !IS_VALID_PERCENTAGE(Value)) {
      return EFI_ABORTED;
    }
    Value = H2O_BDS_DISPLAY_ORIGIN_Y_TOP_PERCENT(Value);
  }

  HotKeyDesc->YOrigin = (UINT32) Value;

  return EFI_SUCCESS;
}

/**
 Attribute FontName process function to update value in hot key description.

 @param[in]  NameAsciiStr         Pointer to attribute name ascii string
 @param[in]  ValueAsciiStr        Pointer to attribute value ascii string
 @param[out] HotKeyDesc           Pointer to hot key description

 @retval EFI_SUCCESS              Update attribute successfully.
**/
STATIC
EFI_STATUS
DescAttributeFnFont (
  IN  CHAR8                            *NameAsciiStr,
  IN  CHAR8                            *ValueAsciiStr,
  OUT H2O_HOT_KEY_DESCRIPTION          *HotKeyDesc
  )
{
  ASSERT (NameAsciiStr  != NULL);
  ASSERT (ValueAsciiStr != NULL);
  ASSERT (HotKeyDesc    != NULL);

  HotKeyDesc->FontName = AllocateCopyPool (AsciiStrSize (ValueAsciiStr), ValueAsciiStr);

  return EFI_SUCCESS;
}

/**
 Attribute FontSize process function to update value in hot key description.

 @param[in]  NameAsciiStr         Pointer to attribute name ascii string
 @param[in]  ValueAsciiStr        Pointer to attribute value ascii string
 @param[out] HotKeyDesc           Pointer to hot key description

 @retval EFI_SUCCESS              Update attribute successfully.
 @retval EFI_ABORTED              Value ascii string is invalid.
**/
STATIC
EFI_STATUS
DescAttributeFnFontSize (
  IN  CHAR8                            *NameAsciiStr,
  IN  CHAR8                            *ValueAsciiStr,
  OUT H2O_HOT_KEY_DESCRIPTION          *HotKeyDesc
  )
{
  EFI_STATUS                           Status;
  INT64                                Value;

  ASSERT (NameAsciiStr  != NULL);
  ASSERT (ValueAsciiStr != NULL);
  ASSERT (HotKeyDesc    != NULL);

  Status = GetValueFromAsciiStr (ValueAsciiStr, NULL, &Value);
  if (EFI_ERROR (Status) || Value <= 0) {
    return EFI_ABORTED;
  }

  HotKeyDesc->FontSize = (UINT16) (UINTN) Value;

  return EFI_SUCCESS;
}

/**
 Attribute FontStyle process function to update value in hot key description.

 @param[in]  NameAsciiStr         Pointer to attribute name ascii string
 @param[in]  ValueAsciiStr        Pointer to attribute value ascii string
 @param[out] HotKeyDesc           Pointer to hot key description

 @retval EFI_SUCCESS              Update attribute successfully.
 @retval EFI_ABORTED              Value ascii string is invalid.
**/
STATIC
EFI_STATUS
DescAttributeFnFontStyle (
  IN  CHAR8                            *NameAsciiStr,
  IN  CHAR8                            *ValueAsciiStr,
  OUT H2O_HOT_KEY_DESCRIPTION          *HotKeyDesc
  )
{
  EFI_HII_FONT_STYLE                   FontStyle;

  ASSERT (NameAsciiStr  != NULL);
  ASSERT (ValueAsciiStr != NULL);
  ASSERT (HotKeyDesc    != NULL);

  if (AsciiStrCmp (ValueAsciiStr, "bold") == 0) {
    FontStyle = EFI_HII_FONT_STYLE_BOLD;
  } else if (AsciiStrCmp (ValueAsciiStr, "italic") == 0) {
    FontStyle = EFI_HII_FONT_STYLE_ITALIC;
  } else if (AsciiStrCmp (ValueAsciiStr, "normal") == 0) {
    FontStyle = EFI_HII_FONT_STYLE_NORMAL;
  } else if (AsciiStrCmp (ValueAsciiStr, "underline") == 0) {
    FontStyle = EFI_HII_FONT_STYLE_UNDERLINE;
  } else {
    return EFI_ABORTED;
  }

  HotKeyDesc->FontStyle |= FontStyle;

  return EFI_SUCCESS;
}

/**
 Attribute ForegroundColor BackgroundColor process function to update value in hot key description.

 @param[in]  NameAsciiStr         Pointer to attribute name ascii string
 @param[in]  ValueAsciiStr        Pointer to attribute value ascii string
 @param[out] HotKeyDesc           Pointer to hot key description

 @retval EFI_SUCCESS              Update attribute successfully.
 @retval EFI_ABORTED              Value ascii string is invalid.
**/
STATIC
EFI_STATUS
DescAttributeFnForeBackColor (
  IN  CHAR8                            *NameAsciiStr,
  IN  CHAR8                            *ValueAsciiStr,
  OUT H2O_HOT_KEY_DESCRIPTION          *HotKeyDesc
  )
{
  EFI_STATUS                           Status;
  INT64                                Value;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL        *Color;

  ASSERT (NameAsciiStr  != NULL);
  ASSERT (ValueAsciiStr != NULL);
  ASSERT (HotKeyDesc    != NULL);

  Status = GetValueFromAsciiStr (ValueAsciiStr, NULL, &Value);
  if (EFI_ERROR (Status) || Value < 0) {
    return EFI_ABORTED;
  }

  Color           = (NameAsciiStr[0] == 'f') ? &HotKeyDesc->ForegroundColor : &HotKeyDesc->BackgroundColor;
  Color->Blue     = (UINT8) (Value         & 0xFF);
  Color->Green    = (UINT8) ((Value >>  8) & 0xFF);
  Color->Red      = (UINT8) ((Value >> 16) & 0xFF);
  Color->Reserved = (UINT8) ((Value >> 24) & 0xFF);

  return EFI_SUCCESS;
}

DESCRIPTION_ATTRIBUTE_TABLE       mDescAttributeTable[] = {{"x"        , DescAttributeFnXY           },
                                                           {"y"        , DescAttributeFnXY           },
                                                           {"xorigin"  , DescAttributeFnXOrigin      },
                                                           {"yorigin"  , DescAttributeFnYOrigin      },
                                                           {"font"     , DescAttributeFnFont         },
                                                           {"fontsize" , DescAttributeFnFontSize     },
                                                           {"fontstyle", DescAttributeFnFontStyle    },
                                                           {"forecolor", DescAttributeFnForeBackColor},
                                                           {"backcolor", DescAttributeFnForeBackColor},
                                                           };

/**
 Update attribute into hot key description by parsing attribute ascii string.

 @param[in]  AttributeAsciiStr    Pointer to attribute ascii string
 @param[out] HotKeyDesc           Pointer to hot key description

 @retval EFI_SUCCESS              Update attribute successfully.
 @retval EFI_INVALID_PARAMETER    Input parameter is NULL.
 @retval EFI_OUT_OF_RESOURCES     Allocate memory failed.
**/
STATIC
EFI_STATUS
HotKeyDescUpdateAttributes (
  IN  CHAR8                            *AttributeAsciiStr,
  OUT H2O_HOT_KEY_DESCRIPTION          *HotKeyDesc
  )
{
  CHAR8                                *AsciiStrBuffer;
  CHAR8                                *CurrAttrAsciiStr;
  CHAR8                                *NextAttrAsciiStr;
  CHAR8                                *NameAsciiStr;
  CHAR8                                *ValueAsciiStr;
  UINTN                                Index;

  if (AttributeAsciiStr == NULL || HotKeyDesc == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  AsciiStrBuffer = AllocateCopyPool (AsciiStrSize (AttributeAsciiStr), AttributeAsciiStr);
  if (AsciiStrBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Extract attributes by parsing description string in format L"name1=value1;name2=value2;..."
  //
  NextAttrAsciiStr = AsciiStrBuffer;
  while (NextAttrAsciiStr != NULL) {
    CurrAttrAsciiStr = NextAttrAsciiStr;
    NextAttrAsciiStr = AsciiStrStr (NextAttrAsciiStr, ";");
    if (NextAttrAsciiStr != NULL) {
      *NextAttrAsciiStr = CHAR_NULL;
      NextAttrAsciiStr++;
    }

    NameAsciiStr  = CurrAttrAsciiStr;
    ValueAsciiStr = AsciiStrStr (NameAsciiStr, "=");
    if (ValueAsciiStr == NULL) {
      continue;
    }
    *ValueAsciiStr = CHAR_NULL;
    ValueAsciiStr++;

    NormalizeAsciiStr (NameAsciiStr);
    NormalizeAsciiStr (ValueAsciiStr);

    for (Index = 0; Index < sizeof(mDescAttributeTable) / sizeof(DESCRIPTION_ATTRIBUTE_TABLE); Index++) {
      if (AsciiStrCmp (mDescAttributeTable[Index].AttributeName, NameAsciiStr) == 0) {
        mDescAttributeTable[Index].AttributeFn (NameAsciiStr, ValueAsciiStr, HotKeyDesc);
        break;
      }
    }
  }

  FreePool (AsciiStrBuffer);

  return EFI_SUCCESS;
}

/**
 Print string at specific position in text only console output device.

 @param[in] GopX                       GOP position X value
 @param[in] GopY                       GOP position Y value
 @param[in] GopResolutionX             GOP resolution X value
 @param[in] GopResolutionY             GOP resolution Y value
 @param[in] String                     Pointer to display string
 @param[in] FontDisplayInfo            Pointer to font information
**/
STATIC
VOID
PrintStrAtTextOnlyConOutDev (
  IN UINTN                             GopX,
  IN UINTN                             GopY,
  IN UINTN                             GopResolutionX,
  IN UINTN                             GopResolutionY,
  IN CHAR16                            *String,
  IN EFI_FONT_DISPLAY_INFO             *FontDisplayInfo
  )
{
  EFI_STATUS                           Status;
  EFI_HANDLE                           *HandleBuf;
  UINTN                                NoHandles;
  UINTN                                Index;
  VOID                                 *Instance;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL      *SimpleTextOut;
  UINTN                                TextOutResolutionX;
  UINTN                                TextOutResolutionY;
  UINTN                                TextOutX;
  UINTN                                TextOutY;
  UINTN                                Attribute;
  EFI_SIMPLE_TEXT_OUTPUT_MODE          OrgMode;

  Status =  gBS->LocateHandleBuffer (
                   ByProtocol,
                   &gEfiConsoleOutDeviceGuid,
                   NULL,
                   &NoHandles,
                   &HandleBuf
                   );
  if (EFI_ERROR (Status)) {
    return;
  }

  for (Index = 0; Index < NoHandles; Index++) {
    Status = gBS->HandleProtocol (HandleBuf[Index], &gEfiGraphicsOutputProtocolGuid, &Instance);
    if (!EFI_ERROR (Status)) {
      continue;
    }
    Status = gBS->HandleProtocol (HandleBuf[Index], &gEfiDevicePathProtocolGuid, &Instance);
    if (EFI_ERROR (Status)) {
      continue;
    }
    Status = gBS->HandleProtocol (HandleBuf[Index], &gEfiSimpleTextOutProtocolGuid, (VOID **) &SimpleTextOut);
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = SimpleTextOut->QueryMode (SimpleTextOut, (UINTN) SimpleTextOut->Mode->Mode, &TextOutResolutionX, &TextOutResolutionY);
    if (EFI_ERROR (Status)) {
      continue;
    }
    CopyMem (&OrgMode, SimpleTextOut->Mode, sizeof(EFI_SIMPLE_TEXT_OUTPUT_MODE));

    Attribute = GopColorToTextOutColor (&FontDisplayInfo->ForegroundColor, &FontDisplayInfo->BackgroundColor);
    TextOutX  = GopPosnToTextOutPosn (GopX + EFI_GLYPH_WIDTH / 2                   , GopResolutionX, TextOutResolutionX);
    TextOutY  = GopPosnToTextOutPosn (GopY + FontDisplayInfo->FontInfo.FontSize / 2, GopResolutionY, TextOutResolutionY);

    if (OrgMode.CursorVisible) {
      SimpleTextOut->EnableCursor (SimpleTextOut, FALSE);
    }
    SimpleTextOut->SetCursorPosition (SimpleTextOut, TextOutX, TextOutY);
    SimpleTextOut->SetAttribute (SimpleTextOut, Attribute);
    SimpleTextOut->OutputString (SimpleTextOut, String);
    SimpleTextOut->SetAttribute (SimpleTextOut, (UINTN) OrgMode.Attribute);
    SimpleTextOut->SetCursorPosition (SimpleTextOut, (UINTN) OrgMode.CursorColumn, (UINTN) OrgMode.CursorRow);
    if (OrgMode.CursorVisible) {
      SimpleTextOut->EnableCursor (SimpleTextOut, TRUE);
    }
  }

  FreePool (HandleBuf);
}

/**
 Print string at specific position.

 @param[in] X                          Position X value
 @param[in] Y                          Position Y value
 @param[in] String                     Pointer to display string
 @param[in] FontDisplayInfo            Pointer to font information

 @retval EFI_SUCCESS                   Print string successfully.
 @retval EFI_INVALID_PARAMETER         Input paramter in NULL.
 @retval Other                         Locate graphics output protocol failed or display function return failed.
**/
STATIC
EFI_STATUS
PrintStrAt (
  IN UINTN                             X,
  IN UINTN                             Y,
  IN CHAR16                            *String,
  IN EFI_FONT_DISPLAY_INFO             *FontDisplayInfo
  )
{
  EFI_STATUS                           Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL         *GraphicsOutput;
  UINT32                               HorizontalResolution;
  UINT32                               VerticalResolution;
  EFI_IMAGE_OUTPUT                     *Blt;
  EFI_HII_ROW_INFO                     *RowInfoArray;
  UINTN                                RowInfoArraySize;

  if (String == NULL || FontDisplayInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **) &GraphicsOutput);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  HorizontalResolution = GraphicsOutput->Mode->Info->HorizontalResolution;
  VerticalResolution   = GraphicsOutput->Mode->Info->VerticalResolution;

  Blt = (EFI_IMAGE_OUTPUT *) AllocateZeroPool (sizeof(EFI_IMAGE_OUTPUT));
  if (Blt == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Blt->Width        = (UINT16) (HorizontalResolution);
  Blt->Height       = (UINT16) (VerticalResolution);
  Blt->Image.Screen = GraphicsOutput;
  RowInfoArray      = NULL;
  RowInfoArraySize  = 0;
  Status = mHiiFont->StringToImage (
                       mHiiFont,
                       EFI_HII_IGNORE_IF_NO_GLYPH | EFI_HII_OUT_FLAG_CLIP |
                       EFI_HII_OUT_FLAG_CLIP_CLEAN_X | EFI_HII_OUT_FLAG_CLIP_CLEAN_Y | EFI_HII_DIRECT_TO_SCREEN,
                       String,
                       FontDisplayInfo,
                       &Blt,
                       X,
                       Y,
                       &RowInfoArray,
                       &RowInfoArraySize,
                       NULL
                       );
  FreePool (Blt);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (RowInfoArray != NULL) {
    FreePool (RowInfoArray);
  }

  PrintStrAtTextOnlyConOutDev (X, Y, HorizontalResolution, VerticalResolution, String, FontDisplayInfo);
  return EFI_SUCCESS;
}

/**
 Record specific screen region in blt buffer.

 @param[in]  Region                    Pointer to specific screen region
 @param[out] Bitmap                    Pointer to blt buffer

 @retval EFI_SUCCESS                   Record screen successfully.
 @retval EFI_INVALID_PARAMETER         Input paramter in NULL.
 @retval EFI_OUT_OF_RESOURCES          Allocate memory failed.
 @retval Other                         Locate graphics output protocol failed or blt function return failed.
**/
STATIC
EFI_STATUS
RecordScreen (
  IN  RECT                             *Region,
  OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL    **Bitmap
  )
{
  EFI_STATUS                           Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL         *GraphicsOutput;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL        *BltBuf;
  UINTN                                Width;
  UINTN                                Height;

  if (Region == NULL || Bitmap == NULL || IsRectEmpty (Region)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **) &GraphicsOutput);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Width  = Region->right  - Region->left;
  Height = Region->bottom - Region->top;
  BltBuf = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) AllocateZeroPool (Width * Height * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  if (BltBuf == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = GraphicsOutput->Blt (
                             GraphicsOutput,
                             BltBuf,
                             EfiBltVideoToBltBuffer,
                             Region->left,
                             Region->top,
                             0,
                             0,
                             Width,
                             Height,
                             0
                             );
  if (EFI_ERROR (Status)) {
    FreePool (BltBuf);
    return Status;
  }

  *Bitmap = BltBuf;
  return EFI_SUCCESS;
}

/**
 Restore specific screen region from input blt buffer.

 @param[in] Region                     Pointer to specific screen region
 @param[in] Bitmap                     Pointer to blt buffer

 @retval EFI_SUCCESS                   Record screen successfully.
 @retval EFI_INVALID_PARAMETER         Input paramter in NULL.
 @retval EFI_OUT_OF_RESOURCES          Allocate memory failed.
 @retval Other                         Locate graphics output protocol failed or blt function return failed.
**/
EFI_STATUS
RestoreScreen (
  IN RECT                              *Region,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL     *Bitmap
  )
{
  EFI_STATUS                           Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL         *GraphicsOutput;

  if (Region == NULL || Bitmap == NULL || IsRectEmpty (Region)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **) &GraphicsOutput);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return GraphicsOutput->Blt (
                           GraphicsOutput,
                           Bitmap,
                           EfiBltBufferToVideo,
                           0,
                           0,
                           (UINTN) Region->left,
                           (UINTN) Region->top,
                           (UINTN) (Region->right  - Region->left),
                           (UINTN) (Region->bottom - Region->top),
                           0
                           );
}

/**
 Clear hot key description strings at ready to boot.

 @param[in] Event           A pointer to the Event that triggered the callback
 @param[in] Context         A pointer to event context
**/
VOID
EFIAPI
ReadyToBootEventFn (
  IN EFI_EVENT                         Event,
  IN VOID                              *Context
  )
{
  EFI_STATUS                           Status;
  UINT8                                BootDisplayMode;
  LIST_ENTRY                           *Link;
  H2O_HOT_KEY_OPTION                   *HotKeyOption;
  H2O_HOT_KEY_DESCRIPTION              *HotKeyDesc;

  gBS->CloseEvent (Event);

  if (!IsGopReady ()) {
    return;
  }

  Status = gBdsServices->GetBootDisplayMode (gBdsServices, &BootDisplayMode);
  if (EFI_ERROR (Status)) {
    BootDisplayMode = H2O_BDS_BOOT_DISPLAY_MODE_QUIET;
  }

  Link = GetFirstNode (&mBdsHotKeyList);
  while (!IsNull (&mBdsHotKeyList, Link)) {
    HotKeyOption = H2O_HOT_KEY_OPTION_FROM_LINK (Link);
    Link         = GetNextNode (&mBdsHotKeyList, Link);

    HotKeyDesc = (BootDisplayMode == H2O_BDS_BOOT_DISPLAY_MODE_QUIET) ? &HotKeyOption->QuietDescription : &HotKeyOption->Description;
    if (HotKeyDesc == NULL) {
      continue;
    }

    if (HotKeyDesc->OrgBltBuffer != NULL) {
      RestoreScreen (&HotKeyDesc->DisplayRegion, HotKeyDesc->OrgBltBuffer);

      FreePool (HotKeyDesc->OrgBltBuffer);
      HotKeyDesc->OrgBltBuffer = NULL;
      SetRectEmpty (&HotKeyDesc->DisplayRegion);
    }
  }
}

/**
 Display hot key strings after user press hot key.

 @param[in] Event                      The event that is signaled.
 @param[in] Context                    Pointer to the context data.
**/
STATIC
VOID
DisplayHotKeyAfterUserSelectionNotifyFn (
  IN EFI_EVENT                              Event,
  IN VOID                                   *Context
  )
{
  EFI_STATUS                                Status;
  UINT8                                     *Instance;
  UINT8                                     BootDisplayMode;
  LIST_ENTRY                                *Link;
  H2O_HOT_KEY_OPTION                        *HotKeyOption;
  H2O_HOT_KEY_DESCRIPTION                   *HotKeyDesc;
  CHAR16                                    *String;
  EFI_FONT_DISPLAY_INFO                     *FontDisplayInfo;

  Status = gBS->LocateProtocol (&mDisplayHotKeyAfterSelectGuid, NULL, (VOID **) &Instance);
  if (EFI_ERROR (Status)) {
    return;
  }

  if (!IsGopReady ()) {
    return;
  }

  Status = gBdsServices->GetBootDisplayMode (gBdsServices, &BootDisplayMode);
  if (EFI_ERROR (Status)) {
    BootDisplayMode = H2O_BDS_BOOT_DISPLAY_MODE_QUIET;
  }

  Link = GetFirstNode (&mBdsHotKeyList);
  while (!IsNull (&mBdsHotKeyList, Link)) {
    HotKeyOption = H2O_HOT_KEY_OPTION_FROM_LINK (Link);
    Link         = GetNextNode (&mBdsHotKeyList, Link);

    HotKeyDesc = (BootDisplayMode == H2O_BDS_BOOT_DISPLAY_MODE_QUIET) ? &HotKeyOption->QuietDescription : &HotKeyOption->Description;
    if (HotKeyDesc == NULL) {
      continue;
    }

    if (HotKeyDesc->OrgBltBuffer != NULL) {
      RestoreScreen (&HotKeyDesc->DisplayRegion, HotKeyDesc->OrgBltBuffer);

      FreePool (HotKeyDesc->OrgBltBuffer);
      HotKeyDesc->OrgBltBuffer = NULL;
      SetRectEmpty (&HotKeyDesc->DisplayRegion);
    }

    if (HotKeyOption != mSelectedHotKeyOption) {
      continue;
    }
    if (HotKeyDesc->AfterDescStrId == 0) {
      continue;
    }

    FontDisplayInfo = GetFontDisplayInfo (HotKeyDesc);
    if (FontDisplayInfo == NULL) {
      continue;
    }

    String = GetStringById (HotKeyDesc->AfterDescStrId);
    if (String == NULL) {
      continue;
    }
    if (*String == CHAR_NULL) {
      FreePool (String);
      continue;
    }

    Status = GetDisplayRegion (HotKeyDesc, String, &HotKeyDesc->DisplayRegion);
    if (EFI_ERROR (Status)) {
      FreePool (String);
      continue;
    }

    PrintStrAt (
      HotKeyDesc->DisplayRegion.left,
      HotKeyDesc->DisplayRegion.top,
      String,
      FontDisplayInfo
      );

    FreePool (String);
    FreePool (FontDisplayInfo);
  }
}

/**
 ConOut connect after handler to trigger DisplayStringBefore checkpoint if hot key is detected before this checkpoint.

 @param[in] Event           A pointer to the Event that triggered the callback.
 @param[in] Handle          Checkpoint handle.
**/
STATIC
VOID
EFIAPI
ConOutConnectAfterHandler (
  IN EFI_EVENT                              Event,
  IN H2O_CP_HANDLE                          Handle
  )
{
  H2OCpUnregisterHandler (Handle);

  mIsConOutConnectAfterTriggered = TRUE;

  if (mSelectedHotKeyOption != NULL) {
    //
    // Hot key is detected before ConOutConnectAfter checkpoint, delay to trigger DisplayStringBefore checkpoint here.
    //
    TriggerCpDisplayStringBefore (NULL, TRUE, 0);
  }
}

/**
 Install protocol to trigger display hot key strings after user press hot key.
**/
VOID
DisplayHotKeyAfterUserSelection (
  VOID
  )
{
  if (mIsConOutConnectAfterTriggered) {
    TriggerCpDisplayStringBefore (NULL, TRUE, 0);
  }
}

/**
 Display hot key strings before user press hot key.
**/
STATIC
VOID
EFIAPI
DisplayHotKeyBeforeUserSelection (
  VOID
  )
{
  EFI_STATUS                                Status;
  UINT8                                     BootDisplayMode;
  LIST_ENTRY                                *Link;
  H2O_HOT_KEY_OPTION                        *HotKeyOption;
  H2O_HOT_KEY_DESCRIPTION                   *HotKeyDesc;
  CHAR16                                    *String;
  EFI_FONT_DISPLAY_INFO                     *FontDisplayInfo;
  KERNEL_CONFIGURATION                      KernelConfig;

  if (!IsGopReady ()) {
    return;
  }

  GetKernelConfiguration (&KernelConfig);
  if (!FeaturePcdGet (PcdDisplayOemHotkeyString) && (KernelConfig.QuietBoot) && (KernelConfig.BootType == EFI_BOOT_TYPE)) {
    return;
  }

  Status = gBdsServices->GetBootDisplayMode (gBdsServices, &BootDisplayMode);
  if (EFI_ERROR (Status)) {
    BootDisplayMode = H2O_BDS_BOOT_DISPLAY_MODE_QUIET;
  }

  Link = GetFirstNode (&mBdsHotKeyList);
  while (!IsNull (&mBdsHotKeyList, Link)) {
    HotKeyOption = H2O_HOT_KEY_OPTION_FROM_LINK (Link);
    Link         = GetNextNode (&mBdsHotKeyList, Link);

    HotKeyDesc = (BootDisplayMode == H2O_BDS_BOOT_DISPLAY_MODE_QUIET) ? &HotKeyOption->QuietDescription : &HotKeyOption->Description;
    if (HotKeyDesc == NULL) {
      continue;
    }
    if (HotKeyDesc->BeforeDescStrId == 0) {
      continue;
    }

    FontDisplayInfo = GetFontDisplayInfo (HotKeyDesc);
    if (FontDisplayInfo == NULL) {
      continue;
    }

    String = GetStringById (HotKeyDesc->BeforeDescStrId);
    if (String == NULL) {
      continue;
    }
    if (*String == CHAR_NULL) {
      FreePool (String);
      continue;
    }

    Status = GetDisplayRegion (HotKeyDesc, String, &HotKeyDesc->DisplayRegion);
    if (EFI_ERROR (Status)) {
      FreePool (String);
      continue;
    }

    RecordScreen (&HotKeyDesc->DisplayRegion, &HotKeyDesc->OrgBltBuffer);

    PrintStrAt (
      HotKeyDesc->DisplayRegion.left,
      HotKeyDesc->DisplayRegion.top,
      String,
      FontDisplayInfo
      );

    FreePool (String);
    FreePool (FontDisplayInfo);
  }
}

/**
 Display strings before handler to display before/after string based on H2O_BDS_CP_DISPLAY_STRING_BEFORE_DATA.AfterSelect.

 @param[in] Event           A pointer to the Event that triggered the callback.
 @param[in] Handle          Checkpoint handle.
**/
STATIC
VOID
EFIAPI
DisplayStringBeforeHandler (
  IN EFI_EVENT                              Event,
  IN H2O_CP_HANDLE                          Handle
  )
{
  EFI_STATUS                                Status;
  H2O_BDS_CP_DISPLAY_STRING_BEFORE_DATA     *BdsDisplayStringBeforeData;
  EFI_GUID                                  BdsDisplayStringBeforeGuid;
  EFI_HANDLE                                ProtocolHandle;

  Status = H2OCpLookup (Handle, (VOID **) &BdsDisplayStringBeforeData, &BdsDisplayStringBeforeGuid);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Checkpoint Data Not Found: %x (%r)\n", Handle, Status));
    DEBUG ((DEBUG_ERROR, "   %a\n", __FUNCTION__));
    return;
  }
  if (BdsDisplayStringBeforeData->Status == H2O_BDS_TASK_SKIP) {
    return;
  }

  if (BdsDisplayStringBeforeData->AfterSelect) {
    ProtocolHandle = NULL;
    gBS->InstallProtocolInterface (
           &ProtocolHandle,
           &mDisplayHotKeyAfterSelectGuid,
           EFI_NATIVE_INTERFACE,
           NULL
           );
  } else {
    DisplayHotKeyBeforeUserSelection ();
  }
}

/**
 Enumerate PCD hot key description.
**/
VOID
PcdHotKeyDescEnumeration (
  VOID
  )
{
  UINTN                                Index;
  EFI_GUID                             *TokenSpaceGuid;
  EFI_STATUS                           Status;
  UINTN                                *TokenNumList;
  UINTN                                TokenNumCount;
  LIST_ENTRY                           *Link;
  CHAR8                                *PcdAsciiStr;
  UINTN                                PcdAsciiStrSize;
  UINTN                                Size;
  H2O_HOT_KEY_OPTION                   *HotKeyOption;
  H2O_HOT_KEY_DESCRIPTION              *HotKeyDesc;

  //
  // Enumeratie hot key description from the PCD of token space GUID for every boot display mode.
  //
  for (Index = 0; Index < sizeof(mPcdHotKeyDescInfoList) / sizeof(PCD_HOT_KEY_DESC_INFO); Index++) {
    TokenSpaceGuid = mPcdHotKeyDescInfoList[Index].TokenSpaceGuid;

    Status = GetTokenNumList (TokenSpaceGuid, &TokenNumList, &TokenNumCount);
    if (EFI_ERROR (Status) || TokenNumList == NULL) {
      continue;
    }

    Link = GetFirstNode (&mBdsHotKeyList);
    while (!IsNull (&mBdsHotKeyList, Link)) {
      HotKeyOption = H2O_HOT_KEY_OPTION_FROM_LINK (Link);
      Link         = GetNextNode (&mBdsHotKeyList, Link);
      if (HotKeyOption->Type != PCD_HOT_KEY) {
        continue;
      }

      HotKeyDesc = (H2O_HOT_KEY_DESCRIPTION *) (((UINT8 *) HotKeyOption) + mPcdHotKeyDescInfoList[Index].HotKeyOptionOffset);
      CopyMem (
        HotKeyDesc,
        mPcdHotKeyDescInfoList[Index].DefaultHotKeyDesc,
        sizeof(H2O_HOT_KEY_DESCRIPTION)
        );

      if (!IsTokenInList (TokenNumList, TokenNumCount, HotKeyOption->PcdTokenNum)) {
        continue;
      }

      PcdAsciiStr     = (CHAR8 *) LibPcdGetExPtr (TokenSpaceGuid, HotKeyOption->PcdTokenNum);
      PcdAsciiStrSize = LibPcdGetExSize (TokenSpaceGuid, HotKeyOption->PcdTokenNum);
      if (PcdAsciiStr == NULL || PcdAsciiStrSize == 0) {
        continue;
      }


      //
      // PCD data is in following format "BEFORE_STRING_TOKEN\0AFTER_STRING_TOKEN\0Attribute ASCII Strings\0".
      // So, update data in following order before string ID, after string ID and attributes.
      //
      HiiGetStringIdByName (BdsDxeStringsToIds, PcdAsciiStr, &HotKeyDesc->BeforeDescStrId);
      Size = AsciiStrSize (PcdAsciiStr);
      if (PcdAsciiStrSize <= Size) {
        continue;
      }
      PcdAsciiStr     += Size;
      PcdAsciiStrSize -= Size;

      HiiGetStringIdByName (BdsDxeStringsToIds, PcdAsciiStr, &HotKeyDesc->AfterDescStrId);
      Size = AsciiStrSize (PcdAsciiStr);
      if (PcdAsciiStrSize <= Size) {
        continue;
      }
      PcdAsciiStr += Size;

      HotKeyDescUpdateAttributes (PcdAsciiStr, HotKeyDesc);
    }

    FreePool (TokenNumList);
  }
}

/**
 Initialize BDS hot key description.

 @retval EFI_SUCCESS    Initialize BDS hot key description successfully.
 @retval Other          Locate protocol failed.
**/
EFI_STATUS
BdsHotKeyDescInit (
  VOID
  )
{
  EFI_STATUS                           Status;
  EFI_EVENT                            Event;
  VOID                                 *Registration;
  H2O_CP_HANDLE                        CpHandle;

  Status = gBS->LocateProtocol (&gEfiHiiFontProtocolGuid, NULL, (VOID **) &mHiiFont);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  HotKeyDescUpdateAttributes ((CHAR8 *) PcdGetPtr (PcdH2OBdsDefaultHotKeyDescPosn)     , &mDefaultHotKeyDescPosn);
  HotKeyDescUpdateAttributes ((CHAR8 *) PcdGetPtr (PcdH2OBdsDefaultHotKeyDescQuietPosn), &mDefaultHotKeyDescQuietPosn);

  PcdHotKeyDescEnumeration ();

  //
  // Register protocol notify to clear hot key description strings.
  //
  EfiCreateEventReadyToBootEx (
    TPL_CALLBACK,
    ReadyToBootEventFn,
    NULL,
    &Event
    );

  //
  // To prevent from gH2OBdsCpDisplayStringBeforeGuid checkpoint doesn't work properly.
  // (will be called in TPL_NOTIFY TPL).
  // Register a callback function using 5 TPL to make sure the checkpoint can work properly.
  //
  EfiCreateProtocolNotifyEvent (
    &mDisplayHotKeyAfterSelectGuid,
    TPL_APPLICATION + 1,
    DisplayHotKeyAfterUserSelectionNotifyFn,
    NULL,
    &Registration
    );
  if (FeaturePcdGet (PcdH2OBdsCpDisplayStringBeforeSupported)) {
    Status = H2OCpRegisterHandler (
               &gH2OBdsCpDisplayStringBeforeGuid,
               DisplayStringBeforeHandler,
               H2O_CP_LOW,
               &CpHandle
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Checkpoint Register Fail: %g (%r)\n", &gH2OBdsCpDisplayStringBeforeGuid, Status));
      return Status;
    }
    DEBUG ((DEBUG_INFO, "Checkpoint Registered: %g (%r)\n", &gH2OBdsCpDisplayStringBeforeGuid, Status));
  }
  if (FeaturePcdGet (PcdH2OBdsCpConOutConnectAfterSupported)) {
    Status = H2OCpRegisterHandler (
               &gH2OBdsCpConOutConnectAfterGuid,
               ConOutConnectAfterHandler,
               H2O_CP_LOW,
               &CpHandle
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Checkpoint Register Fail: %g (%r)\n", &gH2OBdsCpConOutConnectAfterGuid, Status));
      return Status;
    }
    DEBUG ((DEBUG_INFO, "Checkpoint Registered: %g (%r)\n", &gH2OBdsCpConOutConnectAfterGuid, Status));
  }

  return EFI_SUCCESS;
}

/**
 Get the current boot display mode, either normal or quiet.

 @param[in]  This                A pointer to the current instance of this protocol.
 @param[out] BootDisplayMode     On exit, returns whether the boot display is in
                                 text mode (H2O_BDS_BOOT_DISPLAY_NORMAL) or graphics mode (H2O_BDS_BOOT_DISPLAY_QUIET).

 @retval EFI_SUCCESS             Function completed successfully.
 @retval EFI_INVALID_PARAMETER   This or BootDisplayMode is NULL.
**/
EFI_STATUS
EFIAPI
BdsServicesGetBootDisplayMode (
  IN  H2O_BDS_SERVICES_PROTOCOL        *This,
  OUT UINT8                            *BootDisplayMode
  )
{
  if (This == NULL || BootDisplayMode == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *BootDisplayMode = IsTextMode () ? H2O_BDS_BOOT_DISPLAY_MODE_NORMAL : H2O_BDS_BOOT_DISPLAY_MODE_QUIET;

  return EFI_SUCCESS;
}

/**
 Change the hot key description for a specific hot key.

 @param[in] This                 A pointer to the current instance of this protocol.
 @param[in] HotKeyHandle         Handle of a previously registered hot key.
 @param[in] BootDisplayMode      Enumerated value that specifies the boot display mode for which these descriptions are
                                 valid. Valid values are: H2O_BDS_BOOT_DISPLAY_MODE_NORMAL - Text
                                                          H2O_BDS_BOOT_DISPLAY_MODE_QUIET  - Graphics
                                                          H2O_BDS_BOOT_DISPLAY_MODE_ALL    - Text & Graphics
 @param[in] BeforeDescText       Pointer to an optional null-terminated string that specifies the text that will be
                                 displayed before the hot key has been detected. A NULL specifies that nothing will
                                 be displayed.
 @param[in] AfterDescText        Pointer to an optional null-terminated string that specifies the text that will be
                                 displayed after the hot key has been detected. A NULL specifies that nothing will be
                                 displayed.

 @retval EFI_SUCCESS             Function completed successfully.
 @retval EFI_INVALID_PARAMETER   HotKeyHandle specifies an invalid handle or BootDisplayMode specifies an invalid boot
                                 display mode for this platform.
 @retval EFI_NOT_FOUND           Hot key option is not found for HotKeyHandle.
**/
EFI_STATUS
EFIAPI
BdsServicesSetHotKeyDescText (
  IN H2O_BDS_SERVICES_PROTOCOL         *This,
  IN EFI_HANDLE                        HotKeyHandle,
  IN UINT8                             BootDisplayMode,
  IN CONST CHAR16                      *BeforeDescText OPTIONAL,
  IN CONST CHAR16                      *AfterDescText OPTIONAL
  )
{
  EFI_STATUS                           Status1;
  EFI_STATUS                           Status2;
  H2O_HOT_KEY_DESCRIPTION              *HotKeyDesc;

  if (This == NULL || HotKeyHandle == NULL || !IsValidBootDisplayMode (BootDisplayMode)) {
    return EFI_INVALID_PARAMETER;
  }

  if (BootDisplayMode == H2O_BDS_BOOT_DISPLAY_MODE_ALL) {
    Status1 = BdsServicesSetHotKeyDescText (This, HotKeyHandle, H2O_BDS_BOOT_DISPLAY_MODE_NORMAL, BeforeDescText, AfterDescText);
    Status2 = BdsServicesSetHotKeyDescText (This, HotKeyHandle, H2O_BDS_BOOT_DISPLAY_MODE_QUIET , BeforeDescText, AfterDescText);
    return EFI_ERROR (Status1) ? Status1 : Status2;
  }

  HotKeyDesc = GetHotKeyDesc (HotKeyHandle, BootDisplayMode);
  if (HotKeyDesc == NULL) {
    return EFI_NOT_FOUND;
  }

  if (BeforeDescText != NULL) {
    HotKeyDesc->BeforeDescStrId = HiiSetString (gStringPackHandle, HotKeyDesc->BeforeDescStrId, (CONST EFI_STRING) BeforeDescText, NULL);
  }
  if (AfterDescText != NULL) {
    HotKeyDesc->AfterDescStrId = HiiSetString (gStringPackHandle, HotKeyDesc->AfterDescStrId, (CONST EFI_STRING) AfterDescText, NULL);
  }

  return EFI_SUCCESS;
}

/**
 Change the hot key position for a specific hot key.

 @param[in] This                A pointer to the current instance of this protocol.
 @param[in] HotKeyHandle        Handle of a previously registered hot key.
 @param[in] BootDisplayMode     Enumerated value that specifies the boot display mode for which these descriptions are
                                valid. Valid values are: H2O_BDS_BOOT_DISPLAY_MODE_NORMAL - Text
                                                         H2O_BDS_BOOT_DISPLAY_MODE_QUIET  - Graphics
                                                         H2O_BDS_BOOT_DISPLAY_MODE_ALL    - Text & Graphics
 @param[in] X        Signed integer value that specifies the X offset, relative to the X origin, in unites specified by XUnit.
 @param[in] XUnit    Enumerated value that specifies the unit of measurement associated with X. Valid values are:
                       H2O_BDS_DISPLAY_UNIT_CHAR    - X is measured in character cells of the display font.
                       H2O_BDS_DISPLAY_UNIT_PIXEL   - X is measured in pixels.
                       H2O_BDS_DISPLAY_UNIT_PERCENT - X is measured in percentage of the screen width.
 @param[in] XOrigin  Enumerated value that specifies the horizontal origin location. Associated with the Description
                     Horizontal Origin and Description Horizontal Origin Offset. Valid values are:
                       H2O_BDS_DISPLAY_X_ORIGIN_LEFT -X is measured from the left edge.
                       H2O_BDS_DISPLAY_X_ORIGIN_CENTER - X is measured from the center of the screen, less half of the
                                                         width of the string.
                       H2O_BDS_DISPLAY_X_ORIGIN_RIGHT -X is measured from the right edge, less the width of the string.
                       H2O_BDS_DISPLAY_X_ORIGIN_DEFAULT -Use the default X origin.
                       H2O_BDS_DISPLAY_X_ORIGIN_DEFAULT_POSN -Use the default X position, unit and origin.
                       H2O_BDS_DISPLAY_X_ORIGIN_LEFT_PERCENT(percent) - X is measured from the left edge of the screen
                                                                        + percent% of the screen width. If percent is
                                                                        equal to 0, then this is equivalent to
                                                                        H2O_BDS_DISPLAY_X_ORIGIN_LEFT.
                       H2O_BDS_DISPLAY_X_ORIGIN_RIGHT_PERCENT(percent) - X is measured from the right edge of the screen,
                                                                         less the width of the string, less the percent%
                                                                         of the screen width. If percent is equal to 0,
                                                                         then this is equivalent to H2O_BDS_DISPLAY_X_ORIGIN_RIGHT.
                     Signed integer value that specifies the Y offset, relative to the Y origin, in unites specified by YUnit.
 @param[in] Y        Enumerated value that specifies the unit of measurement associated with Y. Valid values are:
                       H2O_BDS_DISPLAY_UNIT_CHAR    -Y is measured in character cells of the display font.
                       H2O_BDS_DISPLAY_UNIT_PIXEL   -Y is measured in pixels.
                       H2O_BDS_DISPLAY_UNIT_PERCENT -Y is measured in percentage of the screen width.
 @param[in] YUnit    Enumerated value that specifies the vertical origin location. Associated with the Description
                     Vertical Origin and Description Vertical Origin Offset. Valid values are:
                       H2O_BDS_DISPLAY_Y_ORIGIN_TOP -Y is measured from the top edge.
                       H2O_BDS_DISPLAY_Y_ORIGIN_CENTER - Y is measured from the center of the screen, less half of the
                                                         height of the string.
                       H2O_BDS_DISPLAY_Y_ORIGIN_RIGHT -Y is measured from the bottom edge, less the height of the string.
                       H2O_BDS_DISPLAY_Y_ORIGIN_DEFAULT -Use the default Y origin.
                       H2O_BDS_DISPLAY_Y_ORIGIN_DEFAULT_POSN - Use the default Y position, unit and origin.
                       H2O_BDS_DISPLAY_X_ORIGIN_TOP_PERCENT(percent) - Y is measured from the top edge of the screen +
                                                                       percent% of the screen height. If percent is equal
                                                                       to 0, then this is equivalent to H2O_BDS_DISPLAY_X_ORIGIN_TOP.
                       H2O_BDS_DISPLAY_X_ORIGIN_BOTTOM_PERCENT(percent) - Y is measured from the bottom edge, less the
                                                                          height of the string, less the percent% of the
                                                                          screen height. If percent is equal to 0, then
                                                                          this is equivalent to H2O_BDS_DISPLAY_X_ORIGIN_RIGHT.
 @param[in] YOrigin  Enumerated value that specifies the vertical origin location.

 @retval EFI_SUCCESS             This function has completed successfully.
 @retval EFI_INVALID_PARAMETER   HotKeyHandle specifies an invalid handle or BootDisplayMode specifies an invalid boot
                                 display mode for this platform.
 @retval EFI_NOT_FOUND           Hot key option is not found for HotKeyHandle.
**/
EFI_STATUS
EFIAPI
BdsServicesSetHotKeyDescPosn (
  IN H2O_BDS_SERVICES_PROTOCOL         *This,
  IN EFI_HANDLE                        HotKeyHandle,
  IN UINT8                             BootDisplayMode,
  IN INT32                             X,
  IN UINT32                            XUnit,
  IN UINT32                            *XOrigin OPTIONAL,
  IN INT32                             Y,
  IN UINT32                            YUnit,
  IN UINT32                            *YOrigin OPTIONAL
  )
{
  EFI_STATUS                           Status1;
  EFI_STATUS                           Status2;
  H2O_HOT_KEY_DESCRIPTION              *HotKeyDesc;

  if (This == NULL || HotKeyHandle == NULL || !IsValidBootDisplayMode (BootDisplayMode) ||
      !IsValidPosn (X, XUnit) || !IsValidPosn (Y, YUnit)) {
    return EFI_INVALID_PARAMETER;
  }
  if ((XOrigin != NULL && !IsValidXOriginPosn (*XOrigin)) ||
      (YOrigin != NULL && !IsValidYOriginPosn (*YOrigin))) {
    return EFI_INVALID_PARAMETER;
  }

  if (BootDisplayMode == H2O_BDS_BOOT_DISPLAY_MODE_ALL) {
    Status1 = BdsServicesSetHotKeyDescPosn (This, HotKeyHandle, H2O_BDS_BOOT_DISPLAY_MODE_NORMAL, X, XUnit, XOrigin, Y, YUnit, YOrigin);
    Status2 = BdsServicesSetHotKeyDescPosn (This, HotKeyHandle, H2O_BDS_BOOT_DISPLAY_MODE_QUIET , X, XUnit, XOrigin, Y, YUnit, YOrigin);
    return EFI_ERROR (Status1) ? Status1 : Status2;
  }

  HotKeyDesc = GetHotKeyDesc (HotKeyHandle, BootDisplayMode);
  if (HotKeyDesc == NULL) {
    return EFI_NOT_FOUND;
  }

  HotKeyDesc->X     = X;
  HotKeyDesc->XUnit = XUnit;
  HotKeyDesc->Y     = Y;
  HotKeyDesc->YUnit = YUnit;
  if (XOrigin != NULL) {
    HotKeyDesc->XOrigin = *XOrigin;
  }
  if (YOrigin != NULL) {
    HotKeyDesc->YOrigin = *YOrigin;
  }

  return EFI_SUCCESS;
}

/**
 Change the hot key color for a specific hot key description.

 @param[in] This                 A pointer to the current instance of this protocol.
 @param[in] HotKeyHandle         Handle of a previously registered hot key.
 @param[in] BootDisplayMode      Enumerated value that specifies the boot display mode for which these descriptions are
                                 valid. Valid values are: H2O_BDS_BOOT_DISPLAY_MODE_NORMAL - Text
                                                          H2O_BDS_BOOT_DISPLAY_MODE_QUIET  - Graphics
                                                          H2O_BDS_BOOT_DISPLAY_MODE_ALL    - Text & Graphics
 @param[in] ForegroundColor      The color of the "on" pixels in the font characters.
 @param[in] BackgroundColor      The color of the "off" pixels in the font characters.

 @retval EFI_SUCCESS             Function completed successfully.
 @retval EFI_INVALID_PARAMETER   HotKeyHandle specifies an invalid handle.
 @retval EFI_NOT_FOUND           Hot key option is not found for HotKeyHandle.
**/
EFI_STATUS
EFIAPI
BdsServicesSetHotKeyDescColor (
  IN H2O_BDS_SERVICES_PROTOCOL         *This,
  IN EFI_HANDLE                        HotKeyHandle,
  IN UINT8                             BootDisplayMode,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL     ForegroundColor,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL     BackgroundColor
  )
{
  EFI_STATUS                           Status1;
  EFI_STATUS                           Status2;
  H2O_HOT_KEY_DESCRIPTION              *HotKeyDesc;

  if (This == NULL || HotKeyHandle == NULL || !IsValidBootDisplayMode (BootDisplayMode)) {
    return EFI_INVALID_PARAMETER;
  }

  if (BootDisplayMode == H2O_BDS_BOOT_DISPLAY_MODE_ALL) {
    Status1 = BdsServicesSetHotKeyDescColor (This, HotKeyHandle, H2O_BDS_BOOT_DISPLAY_MODE_NORMAL, ForegroundColor, BackgroundColor);
    Status2 = BdsServicesSetHotKeyDescColor (This, HotKeyHandle, H2O_BDS_BOOT_DISPLAY_MODE_QUIET , ForegroundColor, BackgroundColor);
    return EFI_ERROR (Status1) ? Status1 : Status2;
  }

  HotKeyDesc = GetHotKeyDesc (HotKeyHandle, BootDisplayMode);
  if (HotKeyDesc == NULL) {
    return EFI_NOT_FOUND;
  }

  CopyMem (&HotKeyDesc->ForegroundColor, &ForegroundColor, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  CopyMem (&HotKeyDesc->BackgroundColor, &BackgroundColor, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

  return EFI_SUCCESS;
}

/**
 Change the hot key font for a specific hot key description.

 @param[in] This                 A pointer to the current instance of this protocol.
 @param[in] HotKeyHandle         Handle of a previously registered hot key.
 @param[in] BootDisplayMode      Enumerated value that specifies the boot display mode for which these descriptions are
                                 valid. Valid values are: H2O_BDS_BOOT_DISPLAY_MODE_NORMAL - Text
                                                          H2O_BDS_BOOT_DISPLAY_MODE_QUIET  - Graphics
                                                          H2O_BDS_BOOT_DISPLAY_MODE_ALL    - Text & Graphics
 @param[in] FontName             Pointer to optional null-terminated ASCII string that specifies the name of the font
                                 to use for the display of these strings. A NULL specifies that the default font name,
                                 font size and style will be used.
 @param[in] FontSize             Unsigned integer that specifies the height of the font to use for the display of these
                                 strings. If FontName is NULL, then this is ignored.
 @param[in] FontStyle            Bitmask that specifies the style of the font to use for the display of these strings.
                                 If FontName is NULL, then this is ignored

 @retval EFI_SUCCESS             Function completed successfully.
 @retval EFI_INVALID_PARAMETER   HotKeyHandle specifies an invalid handle.
 @retval EFI_NOT_FOUND           Hot key option is not found for HotKeyHandle.
**/
EFI_STATUS
EFIAPI
BdsServicesSetHotKeyDescFont (
  IN H2O_BDS_SERVICES_PROTOCOL         *This,
  IN EFI_HANDLE                        HotKeyHandle,
  IN UINT8                             BootDisplayMode,
  IN CONST CHAR8                       *FontName OPTIONAL,
  IN UINT16                            FontSize,
  IN EFI_HII_FONT_STYLE                FontStyle
  )
{
  EFI_STATUS                           Status1;
  EFI_STATUS                           Status2;
  H2O_HOT_KEY_DESCRIPTION              *HotKeyDesc;

  if (This == NULL || HotKeyHandle == NULL || !IsValidBootDisplayMode (BootDisplayMode)) {
    return EFI_INVALID_PARAMETER;
  }

  if (BootDisplayMode == H2O_BDS_BOOT_DISPLAY_MODE_ALL) {
    Status1 = BdsServicesSetHotKeyDescFont (This, HotKeyHandle, H2O_BDS_BOOT_DISPLAY_MODE_NORMAL, FontName, FontSize, FontStyle);
    Status2 = BdsServicesSetHotKeyDescFont (This, HotKeyHandle, H2O_BDS_BOOT_DISPLAY_MODE_QUIET , FontName, FontSize, FontStyle);
    return EFI_ERROR (Status1) ? Status1 : Status2;
  }

  HotKeyDesc = GetHotKeyDesc (HotKeyHandle, BootDisplayMode);
  if (HotKeyDesc == NULL) {
    return EFI_NOT_FOUND;
  }

  if (HotKeyDesc->FontName != NULL) {
    FreePool (HotKeyDesc->FontName);
  }

  if (FontName == NULL) {
    HotKeyDesc->FontName  = NULL;
    HotKeyDesc->FontSize  = EFI_GLYPH_HEIGHT;
    HotKeyDesc->FontStyle = 0;
  } else {
    HotKeyDesc->FontName  = AllocateCopyPool (AsciiStrSize (FontName), FontName);
    HotKeyDesc->FontSize  = FontSize;
    HotKeyDesc->FontStyle = FontStyle;
  }

  return EFI_SUCCESS;
}

