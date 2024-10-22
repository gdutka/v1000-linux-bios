/** @file

;******************************************************************************
;* Copyright (c) 2013 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "LayoutSupportLib.h"

typedef struct {
  UINT16                                       Value;
  CHAR8                                        *Str;
} KEYWORD_MAP;

KEYWORD_MAP                                    mSubElementNameMap[] = {
  {DISPLAY_SUB_ELEMENT_NAME_FORM_IMAGE        , "form-image"        },
  {DISPLAY_SUB_ELEMENT_NAME_FORM_TITLE        , "form-title"        },
  {DISPLAY_SUB_ELEMENT_NAME_FORMSET_HELP      , "formset-help"      },
  {DISPLAY_SUB_ELEMENT_NAME_FORMSET_HELP_IMAGE, "formset-help-image"},
  {DISPLAY_SUB_ELEMENT_NAME_FORMSET_IMAGE     , "formset-image"     },
  {DISPLAY_SUB_ELEMENT_NAME_FORMSET_TITLE     , "formset-title"     },
  {DISPLAY_SUB_ELEMENT_NAME_HELP_IMAGE        , "help-image"        },
  {DISPLAY_SUB_ELEMENT_NAME_VALUE_IMAGE       , "value-image"       },
  {DISPLAY_SUB_ELEMENT_NAME_GROUP             , "group"             },
  {DISPLAY_SUB_ELEMENT_NAME_HELP              , "help"              },
  {DISPLAY_SUB_ELEMENT_NAME_IMAGE             , "image"             },
  {DISPLAY_SUB_ELEMENT_NAME_IMG_0             , "img0"              },
  {DISPLAY_SUB_ELEMENT_NAME_IMG_1             , "img1"              },
  {DISPLAY_SUB_ELEMENT_NAME_IMG_2             , "img2"              },
  {DISPLAY_SUB_ELEMENT_NAME_IMG_3             , "img3"              },
  {DISPLAY_SUB_ELEMENT_NAME_IMG_4             , "img4"              },
  {DISPLAY_SUB_ELEMENT_NAME_IMG_5             , "img5"              },
  {DISPLAY_SUB_ELEMENT_NAME_IMG_6             , "img6"              },
  {DISPLAY_SUB_ELEMENT_NAME_IMG_7             , "img7"              },
  {DISPLAY_SUB_ELEMENT_NAME_IMG_8             , "img8"              },
  {DISPLAY_SUB_ELEMENT_NAME_IMG_9             , "img9"              },
  {DISPLAY_SUB_ELEMENT_NAME_PROMPT            , "prompt"            },
  {DISPLAY_SUB_ELEMENT_NAME_STR_0             , "str0"              },
  {DISPLAY_SUB_ELEMENT_NAME_STR_1             , "str1"              },
  {DISPLAY_SUB_ELEMENT_NAME_STR_2             , "str2"              },
  {DISPLAY_SUB_ELEMENT_NAME_STR_3             , "str3"              },
  {DISPLAY_SUB_ELEMENT_NAME_STR_4             , "str4"              },
  {DISPLAY_SUB_ELEMENT_NAME_STR_5             , "str5"              },
  {DISPLAY_SUB_ELEMENT_NAME_STR_6             , "str6"              },
  {DISPLAY_SUB_ELEMENT_NAME_STR_7             , "str7"              },
  {DISPLAY_SUB_ELEMENT_NAME_STR_8             , "str8"              },
  {DISPLAY_SUB_ELEMENT_NAME_STR_9             , "str9"              },
  {DISPLAY_SUB_ELEMENT_NAME_TEXT2             , "text2"             },
  {DISPLAY_SUB_ELEMENT_NAME_VALUE             , "value"             },
  {DISPLAY_SUB_ELEMENT_NAME_NONE              , NULL                }
};

KEYWORD_MAP                                   mKeywordAlignContentMap[] = {
  {KEYWORD_ALIGN_CONTENT_FLEX_START           , "flex-start"        },
  {KEYWORD_ALIGN_CONTENT_FLEX_END             , "flex-start"        },
  {KEYWORD_ALIGN_CONTENT_CENTER               , "center"            },
  {KEYWORD_ALIGN_CONTENT_SPACE_BETWEEN        , "space-between"     },
  {KEYWORD_ALIGN_CONTENT_SPACE_AROUND         , "space-around"      },
  {KEYWORD_ALIGN_CONTENT_STRETCH              , "stretch"           },
  {0                                          , NULL                }
};

KEYWORD_MAP                                   mKeywordAlignItemsMap[] = {
  {KEYWORD_ALIGN_ITEMS_FLEX_START             , "flex-start"        },
  {KEYWORD_ALIGN_ITEMS_FLEX_END               , "flex-start"        },
  {KEYWORD_ALIGN_ITEMS_CENTER                 , "center"            },
  {KEYWORD_ALIGN_ITEMS_STRETCH                , "stretch"           },
  {0                                          , NULL                }
};

KEYWORD_MAP                                   mKeywordBackgroundRepeatMap[] = {
  {KEYWORD_BACKGROUND_REPEAT_NO_REPEAT        , "no-repeat"         },
  {KEYWORD_BACKGROUND_REPEAT_REPEAT_X         , "repeat-y"          },
  {KEYWORD_BACKGROUND_REPEAT_REPEAT_Y         , "repeat-x"          },
  {KEYWORD_BACKGROUND_REPEAT_REPEAT           , "repeat"            },
  {0                                          , NULL                }
};

KEYWORD_MAP                                   mKeywordBorderShadowMap[] = {
  {KEYWORD_BORDER_SHADOW_NONE                 , "none"              },
  {KEYWORD_BORDER_SHADOW_OUTSET               , "outset"            },
  {KEYWORD_BORDER_SHADOW_INSET                , "inset"             },
  {0                                          , NULL                }
};

KEYWORD_MAP                                   mKeywordBorderStyleMap[] = {
  {KEYWORD_BORDER_STYLE_NONE                  , "none"              },
  {KEYWORD_BORDER_STYLE_SOLID                 , "solid"             },
  {KEYWORD_BORDER_STYLE_DOTTED                , "dotted"            },
  {KEYWORD_BORDER_STYLE_DASHED                , "dashed"            },
  {KEYWORD_BORDER_STYLE_DOUBLE                , "double"            },
  {KEYWORD_BORDER_STYLE_GROOVE                , "groove"            },
  {KEYWORD_BORDER_STYLE_RIDGE                 , "ridge"             },
  {KEYWORD_BORDER_STYLE_INSET                 , "inset"             },
  {KEYWORD_BORDER_STYLE_OUTSET                , "outset"            },
  {KEYWORD_BORDER_STYLE_HIDDEN                , "hidden"            },
  {0                                          , NULL                }
};

KEYWORD_MAP                                   mKeywordDisplayMap[] = {
  {KEYWORD_DISPLAY_NONE                       , "none"              },
  {KEYWORD_DISPLAY_INLINE                     , "inline"            },
  {KEYWORD_DISPLAY_BLOCK                      , "block"             },
  {KEYWORD_DISPLAY_FLEX                       , "flex"              },
  {0                                          , NULL                }
};

KEYWORD_MAP                                   mKeywordFilterMap[] = {
  {KEYWORD_FILTER_NONE                        , "none"              },
  {KEYWORD_FILTER_BRIGHTNESS                  , "brightness"        },
  {KEYWORD_FILTER_GRAYSCALE                   , "grayscale"         },
  {0                                          , NULL                }
};

KEYWORD_MAP                                   mKeywordFlexDirectionMap[] = {
  {KEYWORD_FLEX_DIRECTION_NONE                , "none"              },
  {KEYWORD_FLEX_DIRECTION_ROW                 , "row"               },
  {KEYWORD_FLEX_DIRECTION_COLUMN              , "column"            },
  {0                                          , NULL                }
};

KEYWORD_MAP                                   mKeywordFlexWrapMap[] = {
  {KEYWORD_FLEX_WRAP_NOWRAP                   , "nowrap"            },
  {KEYWORD_FLEX_WRAP_WRAP                     , "wrap"              },
  {0                                          , NULL                }
};

KEYWORD_MAP                                   mKeywordJustifyContentMap[] = {
  {KEYWORD_JUSTIFY_CONTENT_FLEX_START         , "flex-start"        },
  {KEYWORD_JUSTIFY_CONTENT_FLEX_END           , "flex-start"        },
  {KEYWORD_JUSTIFY_CONTENT_CENTER             , "center"            },
  {KEYWORD_JUSTIFY_CONTENT_SPACE_BETWEEN      , "space-between"     },
  {KEYWORD_JUSTIFY_CONTENT_SPACE_AROUND       , "space-around"      },
  {0                                          , NULL                }
};

KEYWORD_MAP                                   mKeywordLinearGradientMap[] = {
  {KEYWORD_LINEAR_GRADIENT_NONE               , "none"              },
  {KEYWORD_LINEAR_GRADIENT_DIRECTION_LEFT     , "direction-left"    },
  {KEYWORD_LINEAR_GRADIENT_DIRECTION_RIGHT    , "direction-right"   },
  {KEYWORD_LINEAR_GRADIENT_DIRECTION_TOP      , "direction-top"     },
  {KEYWORD_LINEAR_GRADIENT_DIRECTION_BOTTOM   , "direction-bottom"  },
  {KEYWORD_LINEAR_GRADIENT_COLOR_START        , "color-start"       },
  {KEYWORD_LINEAR_GRADIENT_COLOR_STOP         , "color-stop"        },
  {KEYWORD_LINEAR_GRADIENT_COLOR_END          , "color-end"         },
  {0                                          , NULL                }
};

KEYWORD_MAP                                   mKeywordOverflowMap[] = {
  {KEYWORD_OVERFLOW_AUTO                      , "auto"              },
  {KEYWORD_OVERFLOW_VISIBLE                   , "visible"           },
  {KEYWORD_OVERFLOW_HIDDEN                    , "hidden"            },
  {KEYWORD_OVERFLOW_SCROLL                    , "scroll"            },
  {0                                          , NULL                }
};

KEYWORD_MAP                                   mKeywordPositionMap[] = {
  {KEYWORD_POSITION_ABSOLUTE                  , "absolute"          },
  {KEYWORD_POSITION_RELATIVE                  , "relative"          },
  {KEYWORD_POSITION_FIXED                     , "static"            },
  {KEYWORD_POSITION_STATIC                    , "fixed"             },
  {0                                          , NULL                }
};

KEYWORD_MAP                                   mKeywordTextAlignMap[] = {
  {KEYWORD_TEXT_ALIGN_LEFT                    , "left"              },
  {KEYWORD_TEXT_ALIGN_RIGHT                   , "right"            },
  {KEYWORD_TEXT_ALIGN_CENTER                  , "center"             },
  {KEYWORD_TEXT_ALIGN_JUSTIFY                 , "justify"           },
  {0                                          , NULL                }
};

KEYWORD_MAP                                   mKeywordTextControlMap[] = {
  {KEYWORD_TEXT_CONTROL_ELLIPSIS              , "ellipsis"          },
  {KEYWORD_TEXT_CONTROL_NOWRAP                , "nowrap"            },
  {KEYWORD_TEXT_CONTROL_WRAP                  , "wrap"              },
  {KEYWORD_TEXT_CONTROL_TRUNCATE              , "truncate"          },
  {0                                          , NULL                }
};

KEYWORD_MAP                                   mKeywordVisibilityMap[] = {
  {KEYWORD_VISIBILITY_VISIBLE                 , "visible"           },
  {KEYWORD_VISIBILITY_HIDDEN                  , "hidden"            },
  {KEYWORD_VISIBILITY_INHERIT                 , "inherit"           },
  {0                                          , NULL                }
};

STATIC
UINT16
GetKeywordValueByStr (
  IN KEYWORD_MAP                                 *Map,
  IN CHAR8                                       *Str
  )
{
  UINT32                                         Index;

  Index = 0;
  while (Map [Index].Str != NULL) {
    if (AsciiStrStr (Str, Map[Index].Str) != NULL) {
      return Map[Index].Value;
    }
    Index ++;
  }

  return DISPLAY_SUB_ELEMENT_NAME_NONE;
}

STATIC
VOID
BubbleSortHotkey (
  IN     UINT32                               HotKeyCount,
  IN OUT HOT_KEY_INFO                         *HotKeyList
  )
{
  UINT32                                      Index;
  UINT32                                      ArrayIndex;
  HOT_KEY_INFO                                Temp;

  if (HotKeyCount == 0 || HotKeyList == NULL) {
    return;
  }

  for (Index = 0; Index < HotKeyCount - 1; Index++) {
    for (ArrayIndex = 1; ArrayIndex < HotKeyCount; ArrayIndex++) {
      if (HotKeyList[ArrayIndex - 1].HotkeyId > HotKeyList[ArrayIndex].HotkeyId) {
        CopyMem (&Temp, &HotKeyList[ArrayIndex - 1], sizeof (HOT_KEY_INFO));
        CopyMem (&HotKeyList[ArrayIndex - 1], &HotKeyList[ArrayIndex], sizeof (HOT_KEY_INFO));
        CopyMem (&HotKeyList[ArrayIndex], &Temp, sizeof (HOT_KEY_INFO));
      }
    }
  }
}

STATIC
EFI_STATUS
ConvertToHiiValue (
  IN  H2O_VALUE                               *H2OValue,
  OUT EFI_HII_VALUE                           *HiiValue
  )
{
  EFI_STATUS                                   Status;
  CHAR16                                       *String;
  UINTN                                        StringSize;
  LAYOUT_DATABASE_PROTOCOL                     *LayoutDatabase;

  if (H2OValue == NULL || HiiValue == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (HiiValue, sizeof(EFI_HII_VALUE));

  switch (H2OValue->Type) {

  case H2O_VALUE_TYPE_BOOLEAN:
    HiiValue->Type    = EFI_IFR_TYPE_BOOLEAN;
    HiiValue->Value.b = H2OValue->Value.Bool;
    break;

  case H2O_VALUE_TYPE_UINT8:
    HiiValue->Type     = EFI_IFR_TYPE_NUM_SIZE_8;
    HiiValue->Value.u8 = H2OValue->Value.U8;
    break;

  case H2O_VALUE_TYPE_UINT16:
    HiiValue->Type      = EFI_IFR_TYPE_NUM_SIZE_16;
    HiiValue->Value.u16 = H2OValue->Value.U16;
    break;

  case H2O_VALUE_TYPE_UINT32:
    HiiValue->Type      = EFI_IFR_TYPE_NUM_SIZE_32;
    HiiValue->Value.u32 = H2OValue->Value.U32;
    break;

  case H2O_VALUE_TYPE_UINT64:
    HiiValue->Type      = EFI_IFR_TYPE_NUM_SIZE_64;
    HiiValue->Value.u64 = H2OValue->Value.U64;
    break;

  case H2O_VALUE_TYPE_STR16:
    HiiValue->Type      = EFI_IFR_TYPE_STRING;
    HiiValue->BufferLen = (UINT16) StrSize (H2OValue->Value.Str16);
    HiiValue->Buffer    = AllocateCopyPool ((UINTN) HiiValue->BufferLen, H2OValue->Value.Str16);
    break;

  case H2O_VALUE_TYPE_STRID:
    Status = gBS->LocateProtocol (&gLayoutDatabaseProtocolGuid, NULL, (VOID **) &LayoutDatabase);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    HiiValue->Type      = EFI_IFR_TYPE_STRING;
    String = HiiGetString (LayoutDatabase->ImagePkgHiiHandle, H2OValue->Value.U16, NULL);
    if (String == NULL) {
      return EFI_NOT_FOUND;
    }
    StringSize = StrSize (String);

    HiiValue->Value.string = (EFI_STRING_ID) H2OValue->Value.U16;
    HiiValue->BufferLen    = (UINT16) StringSize + sizeof (EFI_HII_HANDLE);
    HiiValue->Buffer       = AllocateZeroPool (HiiValue->BufferLen);
    if (HiiValue->Buffer == NULL) {
      FreePool (String);
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (HiiValue->Buffer, String, StringSize);
    CopyMem (&HiiValue->Buffer[StringSize], &LayoutDatabase->ImagePkgHiiHandle, sizeof (EFI_HII_HANDLE));
    FreePool (String);
    break;

  case H2O_VALUE_TYPE_BUFFER:
    HiiValue->Type      = EFI_IFR_TYPE_BUFFER;
    HiiValue->BufferLen = (UINT16) H2OValue->Value.Buffer.BufferSize;
    HiiValue->Buffer    = AllocateCopyPool ((UINTN) HiiValue->BufferLen, H2OValue->Value.Buffer.Buffer);
    break;

  case H2O_VALUE_TYPE_TIME:
    HiiValue->Type              = EFI_IFR_TYPE_TIME;
    HiiValue->Value.time.Hour   = H2OValue->Value.Time.Hour;
    HiiValue->Value.time.Minute = H2OValue->Value.Time.Minute;
    HiiValue->Value.time.Second = H2OValue->Value.Time.Second;
    break;

  case H2O_VALUE_TYPE_DATE:
    HiiValue->Type             = EFI_IFR_TYPE_DATE;
    HiiValue->Value.date.Year  = H2OValue->Value.Date.Year;
    HiiValue->Value.date.Month = H2OValue->Value.Date.Month;
    HiiValue->Value.date.Day   = H2OValue->Value.Date.Day;
    break;

  default:
    ASSERT (FALSE);
    break;
  }

  return EFI_SUCCESS;
}

STATIC
UINT32
Abs (
  IN     UINT32                              First,
  IN     UINT32                              Second
  )
{
  UINT32                                     Result;

  Result = 0;
  if (First >= Second) {
    Result = First - Second;
  } else {
    Result = Second - First;
  }

  return Result;
}

STATIC
UINT32
ColorGap (
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL       *Color1,
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL       *Color2
  )
{
  UINT32                                     Result;
  UINT32                                     GapR;
  UINT32                                     GapG;
  UINT32                                     GapB;

  GapR = Abs ((UINT32)Color1->Red, (UINT32)Color2->Red);
  GapG = Abs ((UINT32)Color1->Green, (UINT32)Color2->Green);
  GapB = Abs ((UINT32)Color1->Blue, (UINT32)Color2->Blue);
  Result = GapR * GapR + GapG * GapG + GapB * GapB;

  return Result;
}

STATIC
UINT32
RgbToAttribute (
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL       *Rgb
  )
{
  UINT32                                     Index;
  UINT32                                     Gap;
  UINT32                                     MinGap;
  UINT32                                     MinIndex;
  UINT32                                     Attribute;
  RGB_ATTR                                   RgbAttr[] = {
                                               //
                                               // B    G    R   reserved
                                               //
                                               {{0x00, 0x00, 0x00, 0xff}, EFI_BLACK       },
                                               {{0x98, 0x00, 0x00, 0xff}, EFI_LIGHTBLUE   },
                                               {{0x00, 0x98, 0x00, 0xff}, EFI_LIGHTGREEN  },
                                               {{0x98, 0x98, 0x00, 0xff}, EFI_LIGHTCYAN   },
                                               {{0x00, 0x00, 0x98, 0xff}, EFI_LIGHTRED    },
                                               {{0x98, 0x00, 0x98, 0xff}, EFI_MAGENTA     },
                                               {{0x00, 0x98, 0x98, 0xff}, EFI_BROWN       },
                                               {{0x98, 0x98, 0x98, 0xff}, EFI_LIGHTGRAY   },
                                               {{0x30, 0x30, 0x30, 0xff}, EFI_DARKGRAY    },
                                               {{0xff, 0x00, 0x00, 0xff}, EFI_BLUE        },
                                               {{0x00, 0xff, 0x00, 0xff}, EFI_GREEN       },
                                               {{0xff, 0xff, 0x00, 0xff}, EFI_CYAN        },
                                               {{0x00, 0x00, 0xff, 0xff}, EFI_RED         },
                                               {{0xff, 0x00, 0xff, 0xff}, EFI_LIGHTMAGENTA},
                                               {{0x00, 0xff, 0xff, 0xff}, EFI_YELLOW      },
                                               {{0xff, 0xff, 0xff, 0xff}, EFI_WHITE       },
                                               {{0x00, 0x00, 0x00, 0x00}, EFI_BLACK       }
                                             };

  MinGap = 0;
  MinIndex = 0;
  for (Index = 0; RgbAttr[Index].Rgb.Reserved != 0; Index ++) {
    Gap = ColorGap (Rgb, &RgbAttr[Index].Rgb);
    if (Gap <= MinGap) {
      //
      // Get Most Closest Attribute
      //
      MinGap = Gap;
      MinIndex = Index;
      if (MinGap == 0) {
        //
        // Same Color
        //
        break;
      }
    }
  }
  Attribute = RgbAttr[MinIndex].Attribute;

  return Attribute;
}

STATIC
EFI_STATUS
ParseH2OValue (
  IN     H2O_PROPERTY_INFO                     *Property,
  OUT    H2O_PROPERTY_VALUE                    *PropValue
  )
{
  CopyMem (&PropValue->H2OValue, &Property->H2OValue, sizeof (H2O_VALUE));

  switch (Property->H2OValue.Type) {

  case H2O_VALUE_TYPE_STR16:
    PropValue->H2OValue.Value.Str16 = (CHAR16 *)Property->ValueStr;
    break;

  case H2O_VALUE_TYPE_BUFFER:
    PropValue->H2OValue.Value.Buffer.Buffer = (UINT8 *)Property->ValueStr;
    break;

  default:
    break;
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
ParseSizeValue (
  IN     INT32                                 RelativeSize,
  IN     H2O_PROPERTY_INFO                     *Property,
  OUT    H2O_PROPERTY_VALUE                    *PropValue
  )
{
  if (Property->H2OValue.Type == H2O_VALUE_TYPE_PERCENTAGE) {
    PropValue->H2OValue.Type = H2O_VALUE_TYPE_UINT32;
    PropValue->H2OValue.Value.U32 = (UINT32)RelativeSize * Property->H2OValue.Value.U32 / 100;
  } else if (Property->H2OValue.Type == H2O_VALUE_TYPE_INT32 || Property->H2OValue.Type == H2O_VALUE_TYPE_INT64) {
    PropValue->H2OValue.Type = H2O_VALUE_TYPE_UINT32;
    if (Property->H2OValue.Value.I32 < 0) {
      PropValue->H2OValue.Value.U32 = (UINT32)(RelativeSize + Property->H2OValue.Value.I32);
    } else {
      PropValue->H2OValue.Value.U32 = (UINT32)(Property->H2OValue.Value.I32);
    }
  } else {
    CopyMem (&PropValue->H2OValue, &Property->H2OValue, sizeof (H2O_VALUE));
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
ParseCoordValue (
  IN     RECT                                  *RelativeField,
  IN     H2O_PROPERTY_INFO                     *Property,
  OUT    H2O_PROPERTY_VALUE                    *PropValue
  )
{
  INT32                                        RelativeSize;

  RelativeSize = 1;
  if (RelativeField != NULL) {
    if (AsciiStrStr (Property->IdentifierStr, "left") != NULL ||
      AsciiStrStr (Property->IdentifierStr, "right") != NULL) {
      RelativeSize = RelativeField->right - RelativeField->left;
    } else if (AsciiStrStr (Property->IdentifierStr, "top") != NULL ||
      AsciiStrStr (Property->IdentifierStr, "bottom") != NULL) {
      RelativeSize = RelativeField->bottom - RelativeField->top;
    }
  }

  ParseSizeValue (RelativeSize, Property, PropValue);

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
ParseColorValue (
  IN     H2O_PROPERTY_INFO                     *Property,
  OUT    H2O_PROPERTY_VALUE                    *PropValue
  )
{
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL                Rgb;
  UINT32                                       Attribute;
  UINT32                                       RgbNumber;
  UINT32                                       TempAttribute;
  CHAR8                                        *StrPtr;
  BOOLEAN                                      Background;

  StrPtr = Property->ValueStr;
  TempAttribute = 0;

  Background = FALSE;
  if (AsciiStrStr (Property->IdentifierStr, "background") != NULL) {
    Background = TRUE;
  }

  if (Property->H2OValue.Type == H2O_VALUE_TYPE_UINT32) {
    RgbNumber = (UINT32)Property->H2OValue.Value.U32;
    CopyMem (&Rgb, &RgbNumber, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
    TempAttribute = RgbToAttribute (&Rgb);
  } else if (Property->H2OValue.Type == H2O_VALUE_TYPE_COLOR) {
    CopyMem (&PropValue->Value.Color.Rgb, &Property->H2OValue.Value.Color.Argb, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
    PropValue->Value.Color.Attribute = Property->H2OValue.Value.Color.TextModeAttribute;
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }

  Attribute = 0;
  if (Background) {
    if (TempAttribute == EFI_WHITE) {
      TempAttribute = EFI_LIGHTGRAY;
    }
    TempAttribute <<= 4;
    Attribute &= 0x0f;
    Attribute |= TempAttribute;
  } else {
    Attribute &= 0xf0;
    Attribute |= TempAttribute;
  }

  CopyMem (&PropValue->Value.Color.Rgb, &Rgb, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  PropValue->Value.Color.Attribute = Attribute;

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
ParseImageValue (
  IN     H2O_PROPERTY_INFO                     *Property,
  OUT    H2O_PROPERTY_VALUE                    *PropValue
  )
{
  if (Property->H2OValue.Type == H2O_VALUE_TYPE_STR16) {
    PropValue->Value.Image.ImageMapStr = (CHAR16 *)Property->ValueStr;
  } else {
    CopyMem (&PropValue->H2OValue, &Property->H2OValue, sizeof (H2O_VALUE));
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
ParseProperty (
  IN     RECT                                  *RelativeField OPTIONAL,
  IN     H2O_PROPERTY_INFO                     *Property,
  OUT    H2O_PROPERTY_VALUE                    *PropValue
  )
{
  ZeroMem (PropValue, sizeof (H2O_PROPERTY_VALUE));

  PropValue->Type = Property->ValueType;
  ParseH2OValue (Property, PropValue);

  switch (Property->ValueType){

  case H2O_PROPERTY_VALUE_TYPE_SIZE:
  case H2O_PROPERTY_VALUE_TYPE_SIZES:
    ParseSizeValue (1, Property, PropValue);
    break;

  case H2O_PROPERTY_VALUE_TYPE_COORD:
    ParseCoordValue (RelativeField, Property, PropValue);
    break;

  case H2O_PROPERTY_VALUE_TYPE_COLOR:
    ParseColorValue (Property, PropValue);
    break;

  case H2O_PROPERTY_VALUE_TYPE_IMAGE:
  case H2O_PROPERTY_VALUE_TYPE_ANIMATION:
    ParseImageValue (Property, PropValue);
    break;

  case H2O_PROPERTY_VALUE_TYPE_BOOLEAN:
  case H2O_PROPERTY_VALUE_TYPE_ID:
  case H2O_PROPERTY_VALUE_TYPE_TOKEN:
  case H2O_PROPERTY_VALUE_TYPE_ENUM:
  case H2O_PROPERTY_VALUE_TYPE_ENUMS:
  case H2O_PROPERTY_VALUE_TYPE_KEYWORD:
  case H2O_PROPERTY_VALUE_TYPE_KEYWORDS:
  case H2O_PROPERTY_VALUE_TYPE_STRING:
  case H2O_PROPERTY_VALUE_TYPE_GUID:
  case H2O_PROPERTY_VALUE_TYPE_VALUE:
  case H2O_PROPERTY_VALUE_TYPE_OVERRIDE:
  case H2O_PROPERTY_VALUE_TYPE_SUBELEMENT:
    break;

  default:
    //
    // Mismatched IdentifierStr String
    //
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
GetPropertyFromPropertyList (
  IN     LIST_ENTRY                            *PropertyListHead,
  IN     CHAR8                                 *IdentifierStr,
  IN     RECT                                  *RelativeField OPTIONAL,
  OUT    H2O_PROPERTY_INFO                     **Property,
  OUT    H2O_PROPERTY_VALUE                    *PropValue
  )
{
  EFI_STATUS                                   Status;
  LIST_ENTRY                                   *Link;
  H2O_PROPERTY_INFO                            *CurrentProperty;


  if (PropertyListHead == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (IdentifierStr == NULL || Property == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Link = PropertyListHead;
  if (IsNull (Link, Link->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  Status = EFI_NOT_FOUND;

  *Property = NULL;
  CurrentProperty = NULL;
  do {
    Link = Link->ForwardLink;
    CurrentProperty = H2O_PROPERTY_INFO_NODE_FROM_LINK (Link);

    if (AsciiStrCmp (CurrentProperty->IdentifierStr, IdentifierStr) == 0) {
      ParseProperty (RelativeField, CurrentProperty, PropValue);
      *Property = CurrentProperty;
      Status = EFI_SUCCESS;
    }
  } while (!IsNodeAtEnd (PropertyListHead, Link));

  return Status;
}

EFI_STATUS
GetPropertyFromStyleList (
  IN     LIST_ENTRY                            *StyleListHead,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  IN     CHAR8                                 *IdentifierStr,
  IN     RECT                                  *RelativeField OPTIONAL,
  OUT    H2O_PROPERTY_INFO                     **Property,
  OUT    H2O_PROPERTY_VALUE                    *PropValue
  )
{
  EFI_STATUS                                   Status;
  LIST_ENTRY                                   *Link;
  H2O_STYLE_INFO                               *CurrentStyle;

  H2O_PROPERTY_INFO                            *TempProperty;
  UINT8                                        Priority;
  UINT8                                        PriorityOld;
  BOOLEAN                                      Changed;


  if (StyleListHead == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (IdentifierStr == NULL || Property == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Link = StyleListHead;
  if (IsNull (Link, Link->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  Status = EFI_NOT_FOUND;
  Priority = 0xff;
  PriorityOld = Priority;
  Changed = FALSE;
  *Property = NULL;

  CurrentStyle = NULL;
  do {
    Link = Link->ForwardLink;
    CurrentStyle = H2O_STYLE_INFO_NODE_FROM_LINK (Link);

    if ((CurrentStyle->StyleType & H2O_IFR_STYLE_TYPE_VFR_TREE_FLAG) != 0) {
      //
      // If StyleType is from vfr...endvfr, the formsetguid and formid and hotkeyid are already checked.
      //
      Priority = 0;
      Changed = TRUE;
    } else if (StyleType != 0 && CurrentStyle->StyleType != 0 && StyleType != CurrentStyle->StyleType) {
      continue;
    }

    if ((PseudoClass != 0 && CurrentStyle->PseudoClass != 0 && PseudoClass != CurrentStyle->PseudoClass) ||
        (PseudoClass == 0 && CurrentStyle->PseudoClass != 0)) {
      continue;
    }

    if (StyleType != 0 && CurrentStyle->StyleType != 0 && PseudoClass != 0 && CurrentStyle->PseudoClass != 0) {
      if (StyleType == CurrentStyle->StyleType && (PseudoClass == CurrentStyle->PseudoClass)) {
        //
        // ".ClassName StyleType : PseudoClass" priority is the highest
        //
        if (Priority >= 1) {
          Priority = 1;
          Changed = TRUE;
        }
      }
    } else if (StyleType != 0 && CurrentStyle->StyleType != 0) {
      if (StyleType == CurrentStyle->StyleType) {
        //
        // ".ClassName StyleType" priority is the second
        //
        if (Priority >= 2) {
          Priority = 2;
          Changed = TRUE;
        }
      }
    } else if (PseudoClass != 0 && CurrentStyle->PseudoClass != 0) {
      if (PseudoClass == CurrentStyle->PseudoClass) {
        //
        // ".ClassName : PseudoClass" priority is the third
        //
        if (Priority >= 3) {
          Priority = 3;
          Changed = TRUE;
        }
      }
    } else if (CurrentStyle->StyleType == 0 && CurrentStyle->PseudoClass == 0) {
      //
      // ".ClassName" priority is the last
      //
      if (Priority >= 4) {
        Priority = 4;
        Changed = TRUE;
      }
    }

    if (Changed) {
      Status = GetPropertyFromPropertyList (&CurrentStyle->PropertyListHead, IdentifierStr, RelativeField, &TempProperty, PropValue);
      if (Status == EFI_SUCCESS) {
        *Property = TempProperty;
        PriorityOld = Priority;
      } else {
        Priority = PriorityOld;
      }
      Changed = FALSE;
    }
  } while (!IsNodeAtEnd (StyleListHead, Link));

  if (*Property == NULL) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
GetPropertyFromHotkeyList (
  IN     LIST_ENTRY                            *HotkeyListHead,
  IN     UINT32                                HotkeyId,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  IN     CHAR8                                 *IdentifierStr,
  IN     RECT                                  *RelativeField OPTIONAL,
  OUT    H2O_PROPERTY_INFO                     **Property,
  OUT    H2O_PROPERTY_VALUE                    *PropValue
  )
{
  EFI_STATUS                                   Status;
  LIST_ENTRY                                   *HotkeyLink;
  H2O_HOTKEY_INFO                              *CurrentHotkey;

  if (HotkeyId == 0) {
    return EFI_INVALID_PARAMETER;
  }

  if (IdentifierStr == NULL || Property == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  HotkeyLink = HotkeyListHead;
  if (IsNull (HotkeyLink, HotkeyLink->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  do {
    HotkeyLink = HotkeyLink->ForwardLink;
    CurrentHotkey = H2O_HOTKEY_INFO_NODE_FROM_LINK (HotkeyLink);

    if (CurrentHotkey->HotkeyBlock->HotkeyId == HotkeyId) {
      //
      // Style of hotkey
      //
      Status = GetPropertyFromStyleList (&CurrentHotkey->StyleListHead, StyleType, PseudoClass, IdentifierStr, RelativeField, Property, PropValue);
      if (Status == EFI_SUCCESS) {
        return EFI_SUCCESS;
      }
    }
  } while (!IsNodeAtEnd (HotkeyListHead, HotkeyLink));

  return EFI_NOT_FOUND;
}

STATIC
EFI_STATUS
GetPropertyFromStatementInVfrTree (
  IN     LIST_ENTRY                            *StatementListHead,
  IN     BOOLEAN                               IsQuestion,
  IN     UINT16                                Id,
  IN     UINT32                                HotkeyId,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  IN     CHAR8                                 *IdentifierStr,
  IN     RECT                                  *RelativeField OPTIONAL,
  OUT    H2O_PROPERTY_INFO                     **Property,
  OUT    H2O_PROPERTY_VALUE                    *PropValue
  )
{
  EFI_STATUS                                   Status;
  LIST_ENTRY                                   *StatementLink;
  H2O_STATEMENT_INFO                           *CurrentStatement;

  if (IdentifierStr == NULL || Property == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  StatementLink = StatementListHead;
  if (IsNull (StatementLink, StatementLink->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  do {
    StatementLink    = StatementLink->ForwardLink;
    CurrentStatement = H2O_STATEMENT_INFO_NODE_FROM_LINK (StatementLink);

    if (CurrentStatement->IsQuestion == IsQuestion &&
        CurrentStatement->QuestionId == Id) {
      //
      // Hotkey of statement
      //
      Status = GetPropertyFromHotkeyList (&CurrentStatement->HotkeyListHead, HotkeyId, StyleType, PseudoClass, IdentifierStr, RelativeField, Property, PropValue);
      if (Status == EFI_SUCCESS) {
        return EFI_SUCCESS;
      }
      //
      // Style of statement
      //
      Status = GetPropertyFromStyleList (&CurrentStatement->StyleListHead, StyleType, PseudoClass, IdentifierStr, RelativeField, Property, PropValue);
      if (Status == EFI_SUCCESS) {
        return EFI_SUCCESS;
      }
    }
  } while (!IsNodeAtEnd (StatementListHead, StatementLink));

  return EFI_NOT_FOUND;
}

STATIC
EFI_STATUS
GetPropertyFromFormInVfrTree (
  IN     LIST_ENTRY                            *FormListHead,
  IN     UINT32                                FormId,
  IN     BOOLEAN                               IsQuestion,
  IN     UINT16                                Id,
  IN     UINT32                                HotkeyId,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  IN     CHAR8                                 *IdentifierStr,
  IN     RECT                                  *RelativeField OPTIONAL,
  OUT    H2O_PROPERTY_INFO                     **Property,
  OUT    H2O_PROPERTY_VALUE                    *PropValue
  )
{
  EFI_STATUS                                   Status;
  LIST_ENTRY                                   *FormLink;
  H2O_FORM_INFO                                *CurrentForm;

  if (FormId == 0) {
    return EFI_INVALID_PARAMETER;
  }

  if (IdentifierStr == NULL || Property == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  FormLink = FormListHead;
  if (IsNull (FormLink, FormLink->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  do {
    FormLink = FormLink->ForwardLink;
    CurrentForm = H2O_FORM_INFO_NODE_FROM_LINK (FormLink);

    if (CurrentForm->FormBlock->FormId == FormId) {
      //
      // Statement
      //
      if ((IsQuestion && Id != 0) || !IsQuestion) {
        Status = GetPropertyFromStatementInVfrTree (&CurrentForm->StatementListHead, IsQuestion, Id, HotkeyId, StyleType, PseudoClass, IdentifierStr, RelativeField, Property, PropValue);
        if (Status == EFI_SUCCESS) {
          return EFI_SUCCESS;
        }
      }
      //
      // Hotkey of form
      //
      Status = GetPropertyFromHotkeyList (&CurrentForm->HotkeyListHead, HotkeyId, StyleType, PseudoClass, IdentifierStr, RelativeField, Property, PropValue);
      if (Status == EFI_SUCCESS) {
        return EFI_SUCCESS;
      }
      //
      // Style of form
      //
      Status = GetPropertyFromStyleList (&CurrentForm->StyleListHead, StyleType, PseudoClass, IdentifierStr, RelativeField, Property, PropValue);
      if (Status == EFI_SUCCESS) {
        return EFI_SUCCESS;
      }
    }
  } while (!IsNodeAtEnd (FormListHead, FormLink));

  return EFI_NOT_FOUND;
}

STATIC
EFI_STATUS
GetPropertyFromFormsetInVfrTree (
  IN     LIST_ENTRY                            *FormsetListHead,
  IN     EFI_GUID                              *FormsetGuid,
  IN     UINT32                                FormId,
  IN     BOOLEAN                               IsQuestion,
  IN     UINT16                                Id,
  IN     UINT32                                HotkeyId,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  IN     CHAR8                                 *IdentifierStr,
  IN     RECT                                  *RelativeField OPTIONAL,
  OUT    H2O_PROPERTY_INFO                     **Property,
  OUT    H2O_PROPERTY_VALUE                    *PropValue
  )
{
  EFI_STATUS                                   Status;
  LIST_ENTRY                                   *FormsetLink;
  H2O_FORMSET_INFO                             *CurrentFormset;

  if (FormsetGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (IdentifierStr == NULL || Property == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  FormsetLink = FormsetListHead;
  if (IsNull (FormsetLink, FormsetLink->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  do {
    FormsetLink = FormsetLink->ForwardLink;
    CurrentFormset = H2O_FORMSET_INFO_NODE_FROM_LINK (FormsetLink);

    if (CompareGuid (&(CurrentFormset->FormsetId), FormsetGuid)) {
      //
      // Form
      //
      Status = GetPropertyFromFormInVfrTree (&CurrentFormset->FormListHead, FormId, IsQuestion, Id, HotkeyId, StyleType, PseudoClass, IdentifierStr, RelativeField, Property, PropValue);
      if (Status == EFI_SUCCESS) {
        return EFI_SUCCESS;
      }
      //
      // Hotkey of formset
      //
      Status = GetPropertyFromHotkeyList (&CurrentFormset->HotkeyListHead, HotkeyId, StyleType, PseudoClass, IdentifierStr, RelativeField, Property, PropValue);
      if (Status == EFI_SUCCESS) {
        return EFI_SUCCESS;
      }
      //
      // Style of formset
      //
      Status = GetPropertyFromStyleList (&CurrentFormset->StyleListHead, StyleType, PseudoClass, IdentifierStr, RelativeField, Property, PropValue);
      if (Status == EFI_SUCCESS) {
        return EFI_SUCCESS;
      }
    }
  } while (!IsNodeAtEnd (FormsetListHead, FormsetLink));

  return EFI_NOT_FOUND;
}

EFI_STATUS
GetPropertyFromVfrInVfrTree (
  IN     LIST_ENTRY                            *VfrListHead,
  IN     EFI_GUID                              *FormsetGuid,
  IN     UINT32                                FormId,
  IN     BOOLEAN                               IsQuestion,
  IN     UINT16                                Id,
  IN     UINT32                                HotkeyId,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  IN     CHAR8                                 *IdentifierStr,
  IN     RECT                                  *RelativeField OPTIONAL,
  OUT    H2O_PROPERTY_INFO                     **Property,
  OUT    H2O_PROPERTY_VALUE                    *PropValue
  )
{
  EFI_STATUS                                   Status;
  LIST_ENTRY                                   *VfrLink;
  H2O_VFR_INFO                                 *CurrentVfr;

  if (FormsetGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (IdentifierStr == NULL || Property == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  VfrLink = VfrListHead;
  if (IsNull (VfrLink, VfrLink->ForwardLink)) {
    return EFI_NOT_FOUND;
  }

  do {
    VfrLink = VfrLink->ForwardLink;
    CurrentVfr = H2O_VFR_INFO_NODE_FROM_LINK (VfrLink);

    //
    // Formset
    //
    Status = GetPropertyFromFormsetInVfrTree (&CurrentVfr->FormsetListHead, FormsetGuid, FormId, IsQuestion, Id, HotkeyId, StyleType, PseudoClass, IdentifierStr, RelativeField, Property, PropValue);
    if (Status == EFI_SUCCESS) {
      return EFI_SUCCESS;
    }
  } while (!IsNodeAtEnd (VfrListHead, VfrLink));

  return EFI_NOT_FOUND;
}

EFI_STATUS
GetPropertyFromVfrInVfcf (
  IN     EFI_GUID                              *FormsetGuid,
  IN     UINT32                                FormId,
  IN     BOOLEAN                               IsQuestion,
  IN     UINT16                                Id,
  IN     UINT32                                HotkeyId,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  IN     CHAR8                                 *IdentifierStr,
  IN     RECT                                  *RelativeField OPTIONAL,
  OUT    H2O_PROPERTY_INFO                     **Property,
  OUT    H2O_PROPERTY_VALUE                    *PropValue
  )
{
  EFI_STATUS                                   Status;
  LAYOUT_DATABASE_PROTOCOL                     *LayoutDatabase;

  if (FormsetGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (IdentifierStr == NULL || Property == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_NOT_FOUND;

  Status = gBS->LocateProtocol (&gLayoutDatabaseProtocolGuid, NULL, (VOID **) &LayoutDatabase);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Vfr
  //
  Status = GetPropertyFromVfrInVfrTree (LayoutDatabase->VfrListHead, FormsetGuid, FormId, IsQuestion, Id, HotkeyId, StyleType, PseudoClass, IdentifierStr, RelativeField, Property, PropValue);
  if (Status == EFI_SUCCESS) {
    return EFI_SUCCESS;
  }

  return Status;
}

STATIC
EFI_STATUS
GetPropertyFromPanel (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     UINT32                                HotkeyId,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  IN     CHAR8                                 *IdentifierStr,
  IN     RECT                                  *RelativeField OPTIONAL,
  OUT    H2O_PROPERTY_INFO                     **Property,
  OUT    H2O_PROPERTY_VALUE                    *PropValue
  )
{
  EFI_STATUS                                   Status;

  if (Panel == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (IdentifierStr == NULL || Property == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_NOT_FOUND;

  //
  // Panel itself style
  //
  Status = GetPropertyFromStyleList (&Panel->StyleListHead, StyleType, PseudoClass, IdentifierStr, RelativeField, Property, PropValue);
  if (Status == EFI_SUCCESS) {
    return EFI_SUCCESS;
  }

  //
  // Panel's parent layout style
  //
  Status = GetPropertyFromStyleList (&Panel->ParentLayout->StyleListHead, StyleType, PseudoClass, IdentifierStr, RelativeField, Property, PropValue);

  //
  // Hotkey of panel's parent layout
  //
  Status = GetPropertyFromHotkeyList (&Panel->ParentLayout->HotkeyListHead, HotkeyId, StyleType, PseudoClass, IdentifierStr, RelativeField, Property, PropValue);
  if (Status == EFI_SUCCESS) {
    return EFI_SUCCESS;
  }

  return Status;
}

EFI_STATUS
GetPropertyFromVfrAndPanel (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     H2O_VFR_PROPERTY                      *VfrProperty,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  IN     CHAR8                                 *IdentifierStr,
  IN     RECT                                  *RelativeField OPTIONAL,
  OUT    H2O_PROPERTY_INFO                     **Property,
  OUT    H2O_PROPERTY_VALUE                    *PropValue
  )
{
  EFI_STATUS                                   Status;
  H2O_VFR_PROPERTY                             EmptyVfrProperty;

  if (IdentifierStr == NULL || Property == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (VfrProperty == NULL) {
    ZeroMem (&EmptyVfrProperty, sizeof (H2O_VFR_PROPERTY));
    VfrProperty = &EmptyVfrProperty;
  }

  Status = EFI_NOT_FOUND;

  //
  // Priority: Vfr Option > Vfr Statement > Vfr Form > Vfr Formset > Vfr in Vfcf > Vfcf
  //
  Status = GetPropertyFromPropertyList (VfrProperty->OptionPropList, IdentifierStr, RelativeField, Property, PropValue);
  if (Status == EFI_SUCCESS) {
    return EFI_SUCCESS;
  }

  Status = GetPropertyFromPropertyList (VfrProperty->StatementPropList, IdentifierStr, RelativeField, Property, PropValue);
  if (Status == EFI_SUCCESS) {
    return EFI_SUCCESS;
  }

  Status = GetPropertyFromPropertyList (VfrProperty->FormPropList, IdentifierStr, RelativeField, Property, PropValue);
  if (Status == EFI_SUCCESS) {
    return EFI_SUCCESS;
  }

  Status = GetPropertyFromPropertyList (VfrProperty->FormsetPropList, IdentifierStr, RelativeField, Property, PropValue);
  if (Status == EFI_SUCCESS) {
    return EFI_SUCCESS;
  }

  Status = GetPropertyFromVfrInVfcf (&VfrProperty->FormsetGuid, VfrProperty->FormId, TRUE, 0, VfrProperty->HotkeyId, StyleType, PseudoClass, IdentifierStr, RelativeField, Property, PropValue);
  if (Status == EFI_SUCCESS) {
    return EFI_SUCCESS;
  }

  if (Panel != NULL) {
    Status = GetPropertyFromPanel (Panel, VfrProperty->HotkeyId, StyleType, PseudoClass, IdentifierStr, RelativeField, Property, PropValue);
  }

  return Status;
}

BOOLEAN
IsVisibility (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     H2O_VFR_PROPERTY                      *VfrProperty,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass
  )
{
  EFI_STATUS                                   Status;
  H2O_PROPERTY_INFO                            *Property;
  H2O_PROPERTY_VALUE                           PropValue;
  UINT16                                       KeywordValue;

  if (Panel == NULL) {
    return FALSE;
  }

  Status = EFI_NOT_FOUND;

  Status = GetPropertyFromVfrAndPanel (Panel, VfrProperty, StyleType, PseudoClass, "visibility", NULL, &Property, &PropValue);
  if (!EFI_ERROR (Status)) {
    if (PropValue.Type == H2O_PROPERTY_VALUE_TYPE_KEYWORD) {
      KeywordValue = GetKeywordValueByStr (mKeywordVisibilityMap, Property->ValueStr);
      if (KeywordValue == KEYWORD_VISIBILITY_HIDDEN) {
        return FALSE;
      }
    } else if (!PropValue.H2OValue.Value.Bool) {
      return FALSE;
    }
  }

  return TRUE;
}

EFI_STATUS
GetPanelField (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     H2O_VFR_PROPERTY                      *VfrProperty,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  IN     RECT                                  *RelativeField,
  OUT    RECT                                  *PanelField
  )
{
  EFI_STATUS                                   Status;
  H2O_PROPERTY_INFO                            *Property;
  H2O_PROPERTY_VALUE                           PropValue;
  INT32                                        Left;
  INT32                                        Top;
  INT32                                        Right;
  INT32                                        Bottom;
  INT32                                        Width;
  INT32                                        Height;
  BOOLEAN                                      HasLeft;
  BOOLEAN                                      HasTop;
  BOOLEAN                                      HasRight;
  BOOLEAN                                      HasBottom;
  BOOLEAN                                      HasWidth;
  BOOLEAN                                      HasHeight;
  RECT                                         ResultRect;
  INT32                                        RelativeWidth;
  INT32                                        RelativeHeight;

  if (Panel == NULL || PanelField == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Left   = 0;
  Top    = 0;
  Right  = 0;
  Bottom = 0;
  Width  = 0;
  Height = 0;

  HasLeft   = FALSE;
  HasTop    = FALSE;
  HasRight  = FALSE;
  HasBottom = FALSE;
  HasWidth  = FALSE;
  HasHeight = FALSE;

  Status = GetPropertyFromVfrAndPanel (Panel, VfrProperty, StyleType, PseudoClass, "left", RelativeField, &Property, &PropValue);
  if (!EFI_ERROR (Status)) {
    HasLeft = TRUE;
    Left = PropValue.H2OValue.Value.I32;
  }

  Status = GetPropertyFromVfrAndPanel (Panel, VfrProperty, StyleType, PseudoClass, "top", RelativeField, &Property, &PropValue);
  if (!EFI_ERROR (Status)) {
    HasTop = TRUE;
    Top = PropValue.H2OValue.Value.I32;
  }

  Status = GetPropertyFromVfrAndPanel (Panel, VfrProperty, StyleType, PseudoClass, "right", RelativeField, &Property, &PropValue);
  if (!EFI_ERROR (Status)) {
    HasRight = TRUE;
    Right = PropValue.H2OValue.Value.I32;
  }

  Status = GetPropertyFromVfrAndPanel (Panel, VfrProperty, StyleType, PseudoClass, "bottom", RelativeField, &Property, &PropValue);
  if (!EFI_ERROR (Status)) {
    HasBottom = TRUE;
    Bottom = PropValue.H2OValue.Value.I32;
  }

  Status = GetPropertyFromVfrAndPanel (Panel, VfrProperty, StyleType, PseudoClass, "width", RelativeField, &Property, &PropValue);
  if (!EFI_ERROR (Status) && (PropValue.H2OValue.Value.I32 > 0)) {
    HasWidth = TRUE;
    Width = PropValue.H2OValue.Value.I32;
  }

  Status = GetPropertyFromVfrAndPanel (Panel, VfrProperty, StyleType, PseudoClass, "height", RelativeField, &Property, &PropValue);
  if (!EFI_ERROR (Status) && (PropValue.H2OValue.Value.I32 > 0)) {
    HasHeight = TRUE;
    Height = PropValue.H2OValue.Value.I32;
  }

  RelativeWidth = RelativeField->right - RelativeField->left;
  RelativeHeight = RelativeField->bottom - RelativeField->top;

  ZeroMem (&ResultRect, sizeof (RECT));

  if (HasLeft) {
    ResultRect.left     = Left;
    if (Panel->ParentLayout->Revision >= LAYOUT_REVISION_2 && Left > 0) {
      ResultRect.left   ++;
    }
  }
  if (HasTop) {
    ResultRect.top      = Top;
  }
  if (HasRight) {
    if (Panel->ParentLayout->Revision < LAYOUT_REVISION_2) {
      ResultRect.right  = Right;
    } else {
      ResultRect.right  = RelativeWidth - Right;
    }
  }
  if (HasBottom) {
    if (Panel->ParentLayout->Revision < LAYOUT_REVISION_2) {
      ResultRect.bottom = Bottom;
    } else {
      ResultRect.bottom = RelativeHeight - Bottom;
    }
  }
  if (HasWidth) {
    if (!HasLeft && HasRight) {
      ResultRect.left   = ResultRect.right - Width + 1;
    } else {
      ResultRect.right  = ResultRect.left + Width - 1;
    }
  }
  if (HasHeight) {
    if (!HasTop && HasBottom) {
      ResultRect.top    = ResultRect.bottom - Height + 1;
    } else {
      ResultRect.bottom = ResultRect.top + Height - 1;
    }
  }

  if (ResultRect.left < RelativeField->left || ResultRect.left >= RelativeField->right) {
    ResultRect.left = RelativeField->left;
  }
  if (ResultRect.right <= RelativeField->left || ResultRect.right > RelativeField->right) {
    ResultRect.right = RelativeField->right;
  }
  if (ResultRect.top < RelativeField->top || ResultRect.top >= RelativeField->bottom) {
    ResultRect.top = RelativeField->top;
  }
  if (ResultRect.bottom <= RelativeField->top || ResultRect.bottom > RelativeField->bottom) {
    ResultRect.bottom = RelativeField->bottom;
  }

  CopyRect (PanelField, &ResultRect);

  return EFI_SUCCESS;
}

EFI_STATUS
GetForegroundColor (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     H2O_VFR_PROPERTY                      *VfrProperty,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  OUT    H2O_COLOR_INFO                        *Color
  )
{
  EFI_STATUS                                   Status;
  H2O_PROPERTY_INFO                            *Property;
  H2O_PROPERTY_VALUE                           PropValue;

  if (Panel == NULL || Color == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Panel->ParentLayout->Revision < LAYOUT_REVISION_2) {
    Status = GetPropertyFromVfrAndPanel (Panel, VfrProperty, StyleType, PseudoClass, "foreground-color", NULL, &Property, &PropValue);
  } else {
    Status = GetPropertyFromVfrAndPanel (Panel, VfrProperty, StyleType, PseudoClass, "color", NULL, &Property, &PropValue);
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }
  CopyMem (Color, &PropValue.Value.Color, sizeof (H2O_COLOR_INFO));

  return EFI_SUCCESS;
}

EFI_STATUS
GetBackgroundColor (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     H2O_VFR_PROPERTY                      *VfrProperty,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  OUT    H2O_COLOR_INFO                        *Color
  )
{
  EFI_STATUS                                   Status;
  H2O_PROPERTY_INFO                            *Property;
  H2O_PROPERTY_VALUE                           PropValue;

  if (Panel == NULL || Color == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetPropertyFromVfrAndPanel (Panel, VfrProperty, StyleType, PseudoClass, "background-color", NULL, &Property, &PropValue);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  CopyMem (Color, &PropValue.Value.Color, sizeof (H2O_COLOR_INFO));

  return EFI_SUCCESS;
}

EFI_STATUS
GetPanelColorAttribute (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     H2O_VFR_PROPERTY                      *VfrProperty,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  OUT    UINT32                                *PanelColorAttribute
  )
{
  EFI_STATUS                                   Status;
  H2O_COLOR_INFO                               Color;
  UINT32                                       Attribute;

  Attribute = 0;

  Status = GetForegroundColor (Panel, VfrProperty, StyleType, PseudoClass, &Color);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Attribute |= Color.Attribute;

  Status = GetBackgroundColor (Panel, VfrProperty, StyleType, PseudoClass, &Color);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Attribute |= Color.Attribute;

  *PanelColorAttribute = Attribute;

  return EFI_SUCCESS;
}

UINT32
GetBorderWidth (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     H2O_VFR_PROPERTY                      *VfrProperty,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass
  )
{
  EFI_STATUS                                   Status;
  H2O_PROPERTY_INFO                            *Property;
  H2O_PROPERTY_VALUE                           PropValue;

  if (Panel == NULL) {
    return 0;
  }

  Status = GetPropertyFromVfrAndPanel (Panel, VfrProperty, StyleType, PseudoClass, "border-width", NULL, &Property, &PropValue);
  if (EFI_ERROR (Status)) {
    //
    // Not found, default border width is 0
    //
    return 0;
  }

  return PropValue.H2OValue.Value.U32;
}

EFI_STATUS
GetHeight(
  IN     H2O_PANEL_INFO                        *Panel,
  IN     H2O_VFR_PROPERTY                      *VfrProperty,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  IN     RECT                                  *RelativeField,
  OUT    INT32                                 *Height
  )
{
  EFI_STATUS                                   Status;
  H2O_PROPERTY_INFO                            *Property;
  H2O_PROPERTY_VALUE                           PropValue;

  if (Panel == NULL || Height == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetPropertyFromVfrAndPanel (Panel, VfrProperty, StyleType, PseudoClass, "height", RelativeField, &Property, &PropValue);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  *Height = PropValue.H2OValue.Value.U32;

  return EFI_SUCCESS;
}

EFI_STATUS
GetWidth(
  IN     H2O_PANEL_INFO                        *Panel,
  IN     H2O_VFR_PROPERTY                      *VfrProperty,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  IN     RECT                                  *RelativeField,
  OUT    INT32                                 *Width
  )
{
  EFI_STATUS                                   Status;
  H2O_PROPERTY_INFO                            *Property;
  H2O_PROPERTY_VALUE                           PropValue;

  if (Panel == NULL || Width == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetPropertyFromVfrAndPanel (Panel, VfrProperty, StyleType, PseudoClass, "width", RelativeField, &Property, &PropValue);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  *Width = PropValue.H2OValue.Value.U32;

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
GetPropertyOfHotkey (
  IN  LIST_ENTRY                               *StyleListHead,
  IN  CHAR8                                    *IdentifierStr,
  IN  UINT32                                   BufferSize,
  OUT UINT8                                    *Buffer
  )
{
  EFI_STATUS                                   Status;
  CHAR16                                       *Str;
  CHAR8                                        *AsciiStr;
  UINTN                                        AsciiStringSize;
  EFI_IMAGE_INPUT                              *Image;
  EFI_HII_VALUE                                HiiValue;
  H2O_PROPERTY_INFO                            *PropertyInfo;
  H2O_PROPERTY_VALUE                           PropertyValue;
  LAYOUT_DATABASE_PROTOCOL                     *LayoutDatabase;
  UINT32                                       ResultSize;
  UINT8                                        *ResultPtr;

  Status = GetPropertyFromStyleList (
             StyleListHead,
             H2O_IFR_STYLE_TYPE_HOTKEY,
             H2O_STYLE_PSEUDO_CLASS_NORMAL,
             IdentifierStr,
             NULL,
             &PropertyInfo,
             &PropertyValue
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  switch (PropertyValue.Type) {

  case H2O_PROPERTY_VALUE_TYPE_BOOLEAN:
    ResultSize = sizeof (BOOLEAN);
    ResultPtr  = (UINT8 *) &PropertyValue.H2OValue.Value.Bool;
    break;

  case H2O_PROPERTY_VALUE_TYPE_STRING:
    if (PropertyValue.H2OValue.Type == H2O_VALUE_TYPE_STR16) {
      Str = AllocateCopyPool (StrSize (PropertyValue.H2OValue.Value.Str16), PropertyValue.H2OValue.Value.Str16);
      if (Str == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
    } else {
      AsciiStringSize = AsciiStrLen (PropertyInfo->ValueStr) + 1;
      Str = (CHAR16 *)AllocateZeroPool (AsciiStringSize * sizeof (CHAR16));
      if (Str == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      AsciiStrToUnicodeStrS (PropertyInfo->ValueStr, Str, AsciiStringSize);
    }

    ResultSize = sizeof (CHAR16 *);
    ResultPtr  = (UINT8 *) &Str;
    break;

  case H2O_PROPERTY_VALUE_TYPE_TOKEN:
    Status = gBS->LocateProtocol (&gLayoutDatabaseProtocolGuid, NULL, (VOID **) &LayoutDatabase);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (PropertyValue.H2OValue.Type == H2O_VALUE_TYPE_STRID && PropertyValue.H2OValue.Value.U16 != 0) {
      Str = HiiGetString (LayoutDatabase->ImagePkgHiiHandle, PropertyValue.H2OValue.Value.U16, NULL);
      if (Str == NULL) {
        return EFI_NOT_FOUND;
      }
      ResultSize = sizeof (CHAR16 *);
      ResultPtr  = (UINT8 *) &Str;
    } else if (PropertyValue.H2OValue.Type == H2O_VALUE_TYPE_IMGID) {
      Image = AllocateZeroPool (sizeof(EFI_IMAGE_INPUT));
      if (Image == NULL) {
        return EFI_NOT_FOUND;
      }
      gHiiImage->GetImage (gHiiImage, LayoutDatabase->ImagePkgHiiHandle, PropertyValue.H2OValue.Value.U16, Image);

      ResultSize = sizeof (EFI_IMAGE_INPUT *);
      ResultPtr  = (UINT8 *) &Image;
    } else {
      return EFI_UNSUPPORTED;
    }
    break;

  case H2O_PROPERTY_VALUE_TYPE_GUID:
    ResultSize = sizeof (EFI_GUID);
    ResultPtr  = (UINT8 *) &PropertyValue.H2OValue.Value.Guid;
    break;

  case H2O_PROPERTY_VALUE_TYPE_ID:
    switch (PropertyValue.H2OValue.Type) {

    case H2O_VALUE_TYPE_UINT8:
      ResultSize = MIN(sizeof (UINT8), BufferSize);
      ResultPtr  = (UINT8 *) &PropertyValue.H2OValue.Value.U8;
      break;

    case H2O_VALUE_TYPE_UINT16:
      ResultSize = MIN(sizeof (UINT16), BufferSize);
      ResultPtr  = (UINT8 *) &PropertyValue.H2OValue.Value.U16;
      break;

    case H2O_VALUE_TYPE_UINT32:
      ResultSize = MIN(sizeof (UINT32), BufferSize);
      ResultPtr  = (UINT8 *) &PropertyValue.H2OValue.Value.U32;
      break;

    case H2O_VALUE_TYPE_UINT64:
      ResultSize = MIN(sizeof (UINT64), BufferSize);
      ResultPtr  = (UINT8 *) &PropertyValue.H2OValue.Value.U64;
      break;

    default:
      return EFI_UNSUPPORTED;
    }
    break;

  case H2O_PROPERTY_VALUE_TYPE_VALUE:
    Status = ConvertToHiiValue  (&PropertyValue.H2OValue, &HiiValue);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    ResultSize = sizeof (EFI_HII_VALUE);
    ResultPtr  = (UINT8 *) &HiiValue;
    break;

  case H2O_PROPERTY_VALUE_TYPE_KEYWORD:
    AsciiStringSize = AsciiStrSize (PropertyInfo->ValueStr) + 1;
    AsciiStr = (CHAR8 *)AllocateZeroPool (AsciiStringSize);
    if (AsciiStr == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    AsciiStrCpyS (AsciiStr, AsciiStringSize, PropertyInfo->ValueStr);
    ResultSize = sizeof (CHAR8 *);
    ResultPtr  = (UINT8 *) &AsciiStr;
    break;

  default:
     return EFI_UNSUPPORTED;
  }


  if (BufferSize < ResultSize) {
    return EFI_BUFFER_TOO_SMALL;
  }

  ZeroMem (Buffer, BufferSize);
  CopyMem (Buffer, ResultPtr, ResultSize);

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
CompatibleHotkey (
  IN     H2O_HOTKEY_INFO                      *H2OHotKey,
  IN OUT HOT_KEY_INFO                         *Hotkey
  )
{
  EFI_STATUS                                  Status;
  BOOLEAN                                     BooleanValue;
  CHAR16                                      *Str;
  UINT64                                      Num;
  EFI_GUID                                    Guid;
  EFI_IMAGE_INPUT                             *Img;
  EFI_HII_VALUE                               HiiValue;

  if (H2OHotKey == NULL || Hotkey == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetPropertyOfHotkey (&H2OHotKey->StyleListHead, "enable", sizeof (BOOLEAN), (UINT8 *) &BooleanValue);
  if (!EFI_ERROR (Status)) {
    Hotkey->Display = BooleanValue;
  }

  Status = GetPropertyOfHotkey (&H2OHotKey->StyleListHead, "prompt-string-token", sizeof(CHAR16 *), (UINT8 *) &Str);
  if (!EFI_ERROR (Status)) {
    SafeFreePool (&Hotkey->Mark);
    Hotkey->Mark = Str;
  }

  Status = GetPropertyOfHotkey (&H2OHotKey->StyleListHead, "help-string-token", sizeof(CHAR16 *), (UINT8 *) &Str);
  if (!EFI_ERROR (Status)) {
    SafeFreePool (&Hotkey->String);
    Hotkey->String = Str;
  }

  Status = GetPropertyOfHotkey (&H2OHotKey->StyleListHead, "prompt-image-token", sizeof(EFI_IMAGE_INPUT *), (UINT8 *) &Img);
  if (!EFI_ERROR (Status)) {
    SafeFreePool (&Hotkey->ImageBuffer);
    Hotkey->ImageBuffer = Img;
  }

  Status = GetPropertyOfHotkey (&H2OHotKey->StyleListHead, "group-id", sizeof(UINT8), (UINT8 *) &Num);
  if (!EFI_ERROR (Status)) {
    Hotkey->GroupId = (UINT8) Num;
  }

  Status = GetPropertyOfHotkey (&H2OHotKey->StyleListHead, "key-action", sizeof(UINT32), (UINT8 *) &Num);
  if (!EFI_ERROR (Status)) {
    Hotkey->HotKeyAction = (UINT32)Num;
  }

  Status = GetPropertyOfHotkey (&H2OHotKey->StyleListHead, "action-defaultid", sizeof(UINT16), (UINT8 *) &Num);
  if (!EFI_ERROR (Status)) {
    Hotkey->HotKeyDefaultId = (UINT16)Num;
  }

  Status = GetPropertyOfHotkey (&H2OHotKey->StyleListHead, "action-target-formsetguid", sizeof(EFI_GUID), (UINT8 *) &Guid);
  if (!EFI_ERROR (Status)) {
    CopyMem (&Hotkey->HotKeyTargetFormSetGuid, &Guid, sizeof (EFI_GUID));
  }

  Status = GetPropertyOfHotkey (&H2OHotKey->StyleListHead, "action-target-formid", sizeof(UINT16), (UINT8 *) &Num);
  if (!EFI_ERROR (Status)) {
    Hotkey->HotKeyTargetFormId = (UINT16)Num;
  }

  Status = GetPropertyOfHotkey (&H2OHotKey->StyleListHead, "action-target-questionid", sizeof(UINT16), (UINT8 *) &Num);
  if (!EFI_ERROR (Status)) {
    Hotkey->HotKeyTargetQuestionId = (UINT16)Num;
  }

  Status = GetPropertyOfHotkey (&H2OHotKey->StyleListHead, "action-target-questionvalue", sizeof(EFI_HII_VALUE), (UINT8 *) &HiiValue);
  if (!EFI_ERROR (Status)) {
    CopyMem (&Hotkey->HotKeyHiiValue, &HiiValue, sizeof (EFI_HII_VALUE));
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
PrepareHotkey (
  IN     LIST_ENTRY                           *VfcfHotKeyListHead,
  IN OUT UINT32                               *HotKeyCount,
  IN OUT HOT_KEY_INFO                         **HotKeyList
  )
{
  EFI_STATUS                                  Status;
  LAYOUT_DATABASE_PROTOCOL                    *LayoutDatabase;
  H2O_KEY_DESC_PROTOCOL                       *KeyDesc;
  EFI_HII_HANDLE                              HiiHandle;
  H2O_HII_LIBT_EXT_HOTKEY_BLOCK               *HotkeyBlock;
  HOT_KEY_INFO                                *Hotkey;
  CHAR16                                      *Str;
  UINT16                                      KeywordValue;
  UINT16                                      GroupId;
  EFI_IMAGE_INPUT                             *Image;
  H2O_PROPERTY_INFO                            *PropertyInfo;
  H2O_PROPERTY_VALUE                           PropertyValue;
  LIST_ENTRY                                  *HotKeyLink;
  H2O_HOTKEY_INFO                             *H2OHotKey;
  HOT_KEY_INFO                                *List;
  UINT32                                      Count;
  UINT32                                      Index;

  if (VfcfHotKeyListHead == NULL || HotKeyCount == NULL || HotKeyList == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (IsListEmpty (VfcfHotKeyListHead)) {
    return EFI_SUCCESS;
  }

  Status = gBS->LocateProtocol (&gLayoutDatabaseProtocolGuid, NULL, (VOID **) &LayoutDatabase);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  HiiHandle = LayoutDatabase->ImagePkgHiiHandle;

  Status = gBS->LocateProtocol (&gH2OKeyDescProtocolGuid, NULL, (VOID **) &KeyDesc);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  List  = *HotKeyList;
  Count = *HotKeyCount;

  for (HotKeyLink = GetFirstNode (VfcfHotKeyListHead); !IsNull (VfcfHotKeyListHead, HotKeyLink); HotKeyLink = GetNextNode (VfcfHotKeyListHead, HotKeyLink)) {
    H2OHotKey   = H2O_HOTKEY_INFO_NODE_FROM_LINK (HotKeyLink);
    HotkeyBlock = H2OHotKey->HotkeyBlock;
    if (HotkeyBlock->Action >= HotKeyMax) {
      continue;
    }

    Hotkey = NULL;
    for (Index = 0; Index < Count; Index++) {
      if (List[Index].HotkeyId == HotkeyBlock->HotkeyId) {
        Hotkey = &List[Index];
        break;
      }
    }
    if (Hotkey == NULL) {
      List = ReallocatePool (sizeof (HOT_KEY_INFO) * Count, sizeof (HOT_KEY_INFO) * (Count + 1), List);
      if (List == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      Hotkey           = &List[Count++];
      Hotkey->Display  = TRUE;
      Hotkey->HotkeyId = HotkeyBlock->HotkeyId;
    }

    Status = GetPropertyOfHotkey (&H2OHotKey->StyleListHead, "key-string", sizeof(CHAR16 *), (UINT8 *) &Str);
    if (!EFI_ERROR (Status)) {
      KeyDesc->KeyFromString (KeyDesc, Str, &Hotkey->KeyData);
      FreePool (Str);
    }

    if (HotkeyBlock->Prompt != 0) {
      SafeFreePool (&Hotkey->Mark);
      Hotkey->Mark = HiiGetString (LayoutDatabase->ImagePkgHiiHandle, HotkeyBlock->Prompt, NULL);
    }
    if (HotkeyBlock->Help != 0) {
      SafeFreePool (&Hotkey->String);
      Hotkey->String = HiiGetString (LayoutDatabase->ImagePkgHiiHandle, HotkeyBlock->Help, NULL);
    }
    if (HotkeyBlock->Action != 0) {
      Hotkey->HotKeyAction = (HOT_KEY_ACTION)HotkeyBlock->Action;
    }
    if (HotkeyBlock->Group != 0) {
      Hotkey->GroupId = (UINT8)HotkeyBlock->Group;
    }

    Status = GetPropertyOfHotkey (&H2OHotKey->StyleListHead, "group-id", sizeof(UINT16), (UINT8 *) &GroupId);
    if (!EFI_ERROR (Status)) {
      Hotkey->GroupId = (UINT8)GroupId;
    }

    if (HotkeyBlock->Image != 0) {
      Image = AllocateZeroPool (sizeof(EFI_IMAGE_INPUT));
      if (Image != NULL) {
        Status = gHiiImage->GetImage (gHiiImage, LayoutDatabase->ImagePkgHiiHandle, HotkeyBlock->Image, Image);
        if (!EFI_ERROR (Status)) {
          Hotkey->ImageBuffer = Image;
        } else {
          Hotkey->ImageBuffer = NULL;
          FreePool (Image);
        }
      }
    }

    Status = GetPropertyFromStyleList (&H2OHotKey->StyleListHead, H2O_IFR_STYLE_TYPE_HOTKEY, H2O_STYLE_PSEUDO_CLASS_NORMAL, "visibility", NULL, &PropertyInfo, &PropertyValue);
    if (!EFI_ERROR (Status)) {
      if (PropertyValue.Type == H2O_PROPERTY_VALUE_TYPE_KEYWORD) {
        KeywordValue = GetKeywordValueByStr (mKeywordVisibilityMap, PropertyInfo->ValueStr);
        if (KeywordValue == KEYWORD_VISIBILITY_HIDDEN) {
          Hotkey->Display = FALSE;
        }
      } else if (!PropertyValue.H2OValue.Value.Bool) {
        Hotkey->Display = FALSE;
      }
    }

    if (HotkeyBlock->ActionDefaultId != 0) {
      Hotkey->HotKeyDefaultId = HotkeyBlock->ActionDefaultId;
    }
    if (!IsZeroGuid (&HotkeyBlock->ActionFormsetId)) {
      CopyGuid (&Hotkey->HotKeyTargetFormSetGuid, &HotkeyBlock->ActionFormsetId);
    }
    if (HotkeyBlock->ActionFormId != 0) {
      Hotkey->HotKeyTargetFormId = HotkeyBlock->ActionFormId;
    }
    if (HotkeyBlock->ActionQuestionId != 0) {
      Hotkey->HotKeyTargetQuestionId = HotkeyBlock->ActionQuestionId;
    }
    if (HotkeyBlock->ActionQuestionValue.Type != H2O_VALUE_TYPE_UNDEFINED) {
      ConvertToHiiValue (&HotkeyBlock->ActionQuestionValue, &Hotkey->HotKeyHiiValue);
    }

    CompatibleHotkey (H2OHotKey, Hotkey);
  }

  *HotKeyList  = List;
  *HotKeyCount = Count;
  return EFI_SUCCESS;
}

EFI_STATUS
GetHotKeyList (
  IN  EFI_GUID                                *FormsetGuid,
  IN  H2O_FORM_ID                             FormId,
  OUT UINT32                                  *HotKeyInfoCount,
  OUT HOT_KEY_INFO                            **HotKeyInfo
  )
{
  EFI_STATUS                                  Status;

  Status = GetHotKeyListByQuestion (FormsetGuid, FormId, TRUE, 0, HotKeyInfoCount, HotKeyInfo);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
GetHotKeyListByQuestion (
  IN  EFI_GUID                         *FormsetGuid,
  IN  H2O_FORM_ID                      FormId,
  IN  BOOLEAN                          IsQuestion,
  IN  UINT16                           Id,
  OUT UINT32                           *HotKeyInfoCount,
  OUT HOT_KEY_INFO                     **HotKeyInfo
  )
{
  EFI_STATUS                           Status;
  UINT32                               HotKeyCount;
  HOT_KEY_INFO                         *HotkeyList;
  UINT32                               Index;
  LIST_ENTRY                           *LayoutListHead;
  LIST_ENTRY                           *VfrListHead;
  LIST_ENTRY                           *VfrLink;
  H2O_VFR_INFO                         *VfrInfo;
  LIST_ENTRY                           *FormsetListHead;
  LIST_ENTRY                           *FormsetLink;
  H2O_FORMSET_INFO                     *FormsetInfo;
  LIST_ENTRY                           *FormListHead;
  LIST_ENTRY                           *FormLink;
  H2O_FORM_INFO                        *FormInfo;
  LIST_ENTRY                           *StatementListHead;
  LIST_ENTRY                           *StatementLink;
  H2O_STATEMENT_INFO                   *StatementInfo;
  UINT8                                EnumerateOrder[] = {H2O_HII_LIBT_EXT_FORMSET,
                                                           H2O_HII_LIBT_EXT_FORM,
                                                           H2O_HII_LIBT_EXT_STATEMENT};

  if (FormsetGuid == NULL || HotKeyInfoCount == NULL || HotKeyInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetLayoutTreeAndVfrTree (&LayoutListHead, &VfrListHead);
  if (EFI_ERROR (Status) || VfrListHead == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Enumerate all hot keys in sequence and the later has higher priority.
  //
  HotKeyCount = 0;
  HotkeyList  = NULL;
  for (Index = 0; Index < sizeof (EnumerateOrder); Index++) {
    if ((EnumerateOrder[Index] == H2O_HII_LIBT_EXT_FORM      && FormId == 0) ||
        (EnumerateOrder[Index] == H2O_HII_LIBT_EXT_STATEMENT && (FormId == 0 || (IsQuestion && Id == 0)))) {
      continue;
    }

    for (VfrLink = GetFirstNode (VfrListHead); !IsNull (VfrListHead, VfrLink); VfrLink = GetNextNode (VfrListHead, VfrLink)) {
      VfrInfo = H2O_VFR_INFO_NODE_FROM_LINK (VfrLink);

      FormsetListHead = &VfrInfo->FormsetListHead;
      for (FormsetLink = GetFirstNode (FormsetListHead); !IsNull (FormsetListHead, FormsetLink); FormsetLink = GetNextNode (FormsetListHead, FormsetLink)) {
        FormsetInfo = H2O_FORMSET_INFO_NODE_FROM_LINK (FormsetLink);
        if (!CompareGuid (&FormsetInfo->FormsetId, FormsetGuid)) {
          continue;
        }
        if (EnumerateOrder[Index] == H2O_HII_LIBT_EXT_FORMSET) {
          PrepareHotkey (&FormsetInfo->HotkeyListHead, &HotKeyCount, &HotkeyList);
          continue;
        }

        FormListHead = &FormsetInfo->FormListHead;
        for (FormLink = GetFirstNode (FormListHead); !IsNull (FormListHead, FormLink); FormLink = GetNextNode (FormListHead, FormLink)) {
          FormInfo = H2O_FORM_INFO_NODE_FROM_LINK (FormLink);
          if (FormInfo->FormBlock->FormId != FormId) {
            continue;
          }
          if (EnumerateOrder[Index] == H2O_HII_LIBT_EXT_FORM) {
            PrepareHotkey (&FormInfo->HotkeyListHead, &HotKeyCount, &HotkeyList);
            continue;
          }

          StatementListHead = &FormInfo->StatementListHead;
          for (StatementLink = GetFirstNode (StatementListHead); !IsNull (StatementListHead, StatementLink); StatementLink = GetNextNode (StatementListHead, StatementLink)) {
            StatementInfo = H2O_STATEMENT_INFO_NODE_FROM_LINK (StatementLink);
            if (StatementInfo->IsQuestion != IsQuestion ||
                StatementInfo->QuestionId != Id) {
              continue;
            }
            if (EnumerateOrder[Index] == H2O_HII_LIBT_EXT_STATEMENT) {
              PrepareHotkey (&StatementInfo->HotkeyListHead, &HotKeyCount, &HotkeyList);
            }
          }
        }
      }
    }
  }

  //
  // Remove invalid hot key from list.
  //
  Index = 0;
  while (Index < HotKeyCount) {
    if (HotkeyList[Index].KeyData.Key.ScanCode == SCAN_NULL &&
        HotkeyList[Index].KeyData.Key.UnicodeChar == CHAR_NULL) {
      CopyMem (&HotkeyList[Index], &HotkeyList[Index + 1], (HotKeyCount - Index - 1) * sizeof (HOT_KEY_INFO));
      HotKeyCount--;
      continue;
    }
    Index++;
  }
  if (HotKeyCount == 0) {
    SafeFreePool (&HotkeyList);
    return EFI_NOT_FOUND;
  }

  BubbleSortHotkey (HotKeyCount, HotkeyList);

  //
  // Always append an empty HOT_KEY_INFO in the end of list for H2O_FORM_BROWSER_P.HotKeyInfo consumer (i.e. IS_END_OF_HOT_KEY_INFO).
  //
  *HotKeyInfoCount = HotKeyCount;
  *HotKeyInfo      = ReallocatePool (HotKeyCount * sizeof (HOT_KEY_INFO), (HotKeyCount + 1) * sizeof (HOT_KEY_INFO), HotkeyList);

  return EFI_SUCCESS;
}

EFI_STATUS
GetHotKeyListByLayoutInfo (
  IN  H2O_LAYOUT_INFO                          *LayoutInfo,
  IN  EFI_GUID                                 *FormsetGuid,
  IN  H2O_FORM_ID                              FormId,
  IN  EFI_QUESTION_ID                          QuestionId,
  OUT UINT32                                   *HotKeyInfoCount,
  OUT HOT_KEY_INFO                             **HotKeyInfo
  )
{
  EFI_STATUS                                  Status;

  Status = GetHotKeyListByQuestion (FormsetGuid, FormId, (BOOLEAN)(QuestionId != 0), QuestionId, HotKeyInfoCount, HotKeyInfo);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

H2O_PANEL_INFO *
GetPanelInfo (
  IN H2O_LAYOUT_INFO                           *LayoutInfo,
  IN UINT32                                    PanelId
  )
{
  LIST_ENTRY                                   *PanelLink;
  H2O_PANEL_INFO                               *Panel;

  ASSERT (LayoutInfo != NULL);
  if (LayoutInfo == NULL) {
    return NULL;
  }

  PanelLink = &LayoutInfo->PanelListHead;
  if (IsNull (PanelLink, PanelLink->ForwardLink)) {
    return NULL;
  }

  do {
    PanelLink = PanelLink->ForwardLink;
    Panel     = H2O_PANEL_INFO_NODE_FROM_LINK (PanelLink);

    if (Panel->PanelId == PanelId) {
      return Panel;
    }
  } while (!IsNodeAtEnd (&LayoutInfo->PanelListHead, PanelLink));

  return NULL;
}

UINT32
GetPseudoClassByStatement (
  IN H2O_FORM_BROWSER_S                        *Statement
  )
{
  UINT32                                       PseudoClass;

  PseudoClass = H2O_STYLE_PSEUDO_CLASS_NORMAL;
  if (Statement->Selectable) {
    PseudoClass = H2O_STYLE_PSEUDO_CLASS_SELECTABLE;
  }
  if (Statement->GrayedOut) {
    PseudoClass = H2O_STYLE_PSEUDO_CLASS_GRAYOUT;
  }
  if (Statement->Locked) {
    PseudoClass = H2O_STYLE_PSEUDO_CLASS_GRAYOUT;
  }
  if (Statement->ReadOnly) {
    PseudoClass = H2O_STYLE_PSEUDO_CLASS_GRAYOUT;
  }
  return PseudoClass;
}

