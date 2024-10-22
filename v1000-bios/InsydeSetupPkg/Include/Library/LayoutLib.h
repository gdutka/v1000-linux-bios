/** @file
 Structure and function definition for Layout Library

;******************************************************************************
;* Copyright (c) 2013 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _LAYOUT_LIB_H_
#define _LAYOUT_LIB_H_

#include <Uefi.h>
#include <Protocol/H2OFormBrowser.h>
#include <Protocol/H2ODisplayEngine.h>
#include <Protocol/HiiFont.h>

#include <Library/RectLib.h>

typedef enum {
  KEYWORD_ALIGN_CONTENT_FLEX_START = 0,
  KEYWORD_ALIGN_CONTENT_FLEX_END,
  KEYWORD_ALIGN_CONTENT_CENTER,
  KEYWORD_ALIGN_CONTENT_SPACE_BETWEEN,
  KEYWORD_ALIGN_CONTENT_SPACE_AROUND,
  KEYWORD_ALIGN_CONTENT_STRETCH,
  KEYWORD_ALIGN_CONTENT_MAX
} KEYWORD_ALIGN_CONTENT;

typedef enum {
  KEYWORD_ALIGN_ITEMS_FLEX_START = 0,
  KEYWORD_ALIGN_ITEMS_FLEX_END,
  KEYWORD_ALIGN_ITEMS_CENTER,
  KEYWORD_ALIGN_ITEMS_STRETCH,
  KEYWORD_ALIGN_ITEMS_MAX
} KEYWORD_ALIGN_ITEMS;

typedef enum {
  KEYWORD_BACKGROUND_REPEAT_NO_REPEAT = 0,
  KEYWORD_BACKGROUND_REPEAT_REPEAT_X,
  KEYWORD_BACKGROUND_REPEAT_REPEAT_Y,
  KEYWORD_BACKGROUND_REPEAT_REPEAT,
  KEYWORD_BACKGROUND_REPEAT_MAX
} KEYWORD_BACKGROUND_REPEAT;

typedef enum {
  KEYWORD_BORDER_SHADOW_NONE = 0,
  KEYWORD_BORDER_SHADOW_OUTSET,
  KEYWORD_BORDER_SHADOW_INSET,
  KEYWORD_BORDER_SHADOW_MAX
} KEYWORD_BORDER_SHADOW;

typedef enum {
  KEYWORD_BORDER_STYLE_NONE = 0,
  KEYWORD_BORDER_STYLE_SOLID,
  KEYWORD_BORDER_STYLE_DOTTED,
  KEYWORD_BORDER_STYLE_DASHED,
  KEYWORD_BORDER_STYLE_DOUBLE,
  KEYWORD_BORDER_STYLE_GROOVE,
  KEYWORD_BORDER_STYLE_RIDGE,
  KEYWORD_BORDER_STYLE_INSET,
  KEYWORD_BORDER_STYLE_OUTSET,
  KEYWORD_BORDER_STYLE_HIDDEN,
  KEYWORD_BORDER_STYLE_MAX
} KEYWORD_BORDER_STYLE;

typedef enum {
  KEYWORD_DISPLAY_NONE = 0,
  KEYWORD_DISPLAY_INLINE,
  KEYWORD_DISPLAY_BLOCK,
  KEYWORD_DISPLAY_FLEX,
  KEYWORD_DISPLAY_MAX
} KEYWORD_DISPLAY;

typedef enum {
  KEYWORD_FILTER_NONE = 0,
  KEYWORD_FILTER_BRIGHTNESS,
  KEYWORD_FILTER_GRAYSCALE,
  KEYWORD_FILTER_MAX
} KEYWORD_FILTER;

typedef enum {
  KEYWORD_FLEX_DIRECTION_NONE = 0,
  KEYWORD_FLEX_DIRECTION_ROW,
  KEYWORD_FLEX_DIRECTION_COLUMN,
  KEYWORD_FLEX_DIRECTION_MAX
} KEYWORD_FLEX_DIRECTION;

typedef enum {
  KEYWORD_FLEX_WRAP_NOWRAP = 0,
  KEYWORD_FLEX_WRAP_WRAP,
  KEYWORD_FLEX_WRAP_MAX
} KEYWORD_FLEX_WRAP;

typedef enum {
  KEYWORD_JUSTIFY_CONTENT_FLEX_START = 0,
  KEYWORD_JUSTIFY_CONTENT_FLEX_END,
  KEYWORD_JUSTIFY_CONTENT_CENTER,
  KEYWORD_JUSTIFY_CONTENT_SPACE_BETWEEN,
  KEYWORD_JUSTIFY_CONTENT_SPACE_AROUND,
  KEYWORD_JUSTIFY_CONTENT_MAX
} KEYWORD_JUSTIFY_CONTENT;

typedef enum {
  KEYWORD_LINEAR_GRADIENT_NONE = 0,
  KEYWORD_LINEAR_GRADIENT_DIRECTION_LEFT,
  KEYWORD_LINEAR_GRADIENT_DIRECTION_RIGHT,
  KEYWORD_LINEAR_GRADIENT_DIRECTION_TOP,
  KEYWORD_LINEAR_GRADIENT_DIRECTION_BOTTOM,
  KEYWORD_LINEAR_GRADIENT_COLOR_START,
  KEYWORD_LINEAR_GRADIENT_COLOR_STOP,
  KEYWORD_LINEAR_GRADIENT_COLOR_END,
  KEYWORD_LINEAR_GRADIENT_MAX
} KEYWORD_LINEAR_GRADIENT;

typedef enum {
  KEYWORD_OVERFLOW_AUTO = 0,
  KEYWORD_OVERFLOW_VISIBLE,
  KEYWORD_OVERFLOW_HIDDEN,
  KEYWORD_OVERFLOW_SCROLL,
  KEYWORD_OVERFLOW_MAX
} KEYWORD_OVERFLOW;

typedef enum {
  KEYWORD_POSITION_ABSOLUTE = 0,
  KEYWORD_POSITION_RELATIVE,
  KEYWORD_POSITION_FIXED,
  KEYWORD_POSITION_STATIC,
  KEYWORD_POSITION_MAX
} KEYWORD_POSITION;

typedef enum {
  KEYWORD_TEXT_ALIGN_LEFT = 0,
  KEYWORD_TEXT_ALIGN_RIGHT,
  KEYWORD_TEXT_ALIGN_CENTER,
  KEYWORD_TEXT_ALIGN_JUSTIFY,
  KEYWORD_TEXT_ALIGN_MAX
} KEYWORD_TEXT_ALIGN;

typedef enum {
  KEYWORD_TEXT_CONTROL_ELLIPSIS = 0,
  KEYWORD_TEXT_CONTROL_NOWRAP,
  KEYWORD_TEXT_CONTROL_WRAP,
  KEYWORD_TEXT_CONTROL_TRUNCATE,
  KEYWORD_TEXT_CONTROL_MAX
} KEYWORD_TEXT_CONTROL;

typedef enum {
  KEYWORD_VISIBILITY_VISIBLE = 0,
  KEYWORD_VISIBILITY_HIDDEN,
  KEYWORD_VISIBILITY_INHERIT,
  KEYWORD_VISIBILITY_MAX
} KEYWORD_VISIBILITY;

typedef struct _RGB_ATTR {
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL                Rgb;
  UINT32                                       Attribute;
} RGB_ATTR;

typedef struct _H2O_COLOR_INFO {
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL                Rgb;
  UINT32                                       Attribute;
} H2O_COLOR_INFO;

typedef struct _H2O_STRING_INFO {
  RECT                                         StringField;
  CHAR16                                       *String;
} H2O_STRING_INFO;

typedef struct _H2O_IMAGE_INFO {
  RECT                                         ImageField;

  CHAR16                                       *ImageMapStr;
  EFI_IMAGE_ID                                 ImageId;
  EFI_ANIMATION_ID                             AnimationId;
  EFI_EVENT                                    AnimationRefreshEvent;

  EFI_GRAPHICS_OUTPUT_BLT_PIXEL                *CurrentBlt;
  UINT32                                       CurrentBltWidth;
  UINT32                                       CurrentBltHeight;
} H2O_IMAGE_INFO;

typedef struct _H2O_CSS_LENGTH {
  UINT8                                        Type;
  union {
    //
    // Relative
    //
    UINT32                                     Px; ///< Pixels
    UINT32                                     Em; ///< Relative to font size
    UINT32                                     Ex; ///< Relative to Lowercase x height
    UINT32                                     Percentage;
    BOOLEAN                                    Auto;
    //
    // Absolute
    //
    UINT32                                     In; ///< Inch
    UINT32                                     Cm; ///< Centimeter
    UINT32                                     Mm; ///< Millimeter
    UINT32                                     Pt; ///< points, 1 pt = 1/72 in
    UINT32                                     Pc; ///< picas, 1 pc = 12 pt
  } Value;
} H2O_CSS_LENGTH;

typedef struct {
  UINT32                                       Type;
  H2O_VALUE                                    H2OValue;
  union {
    H2O_CSS_LENGTH                             Length;
    H2O_COLOR_INFO                             Color;
    H2O_IMAGE_INFO                             Image;
    RECT                                       Rect;
  } Value;
} H2O_PROPERTY_VALUE;

typedef struct _H2O_PANEL_INFO H2O_PANEL_INFO;
typedef struct _H2O_LAYOUT_INFO H2O_LAYOUT_INFO;

#define H2O_PROPERTY_INFO_NODE_SIGNATURE SIGNATURE_32 ('P', 'R', 'O', 'P')
typedef struct _H2O_PROPERTY_INFO {
  UINT32                                       Signature;
  LIST_ENTRY                                   Link;
  UINT32                                       ValueType;
  H2O_VALUE                                    H2OValue;
  CHAR8                                        *IdentifierStr;
  CHAR8                                        *ValueStr;
} H2O_PROPERTY_INFO;
#define H2O_PROPERTY_INFO_NODE_FROM_LINK(a) CR (a, H2O_PROPERTY_INFO, Link, H2O_PROPERTY_INFO_NODE_SIGNATURE)

#define H2O_STYLE_INFO_NODE_SIGNATURE SIGNATURE_32 ('S', 'T', 'Y', 'L')
typedef struct _H2O_STYLE_INFO {
  UINT32                                       Signature;
  LIST_ENTRY                                   Link;

  UINT32                                       StyleType;          ///< From styletypekeyword. See H2O_IFR_STYLE_TYPE_xxx.
  CHAR8                                        *ClassName;
  UINT32                                       PseudoClass;        ///< See H2O_STYLE_PSEUDO_CLASS_xxxx.

  LIST_ENTRY                                   PropertyListHead;
} H2O_STYLE_INFO;
#define H2O_STYLE_INFO_NODE_FROM_LINK(a) CR (a, H2O_STYLE_INFO, Link, H2O_STYLE_INFO_NODE_SIGNATURE)

#define H2O_IFR_STYLE_TYPE_VFR_TREE_FLAG       0x80

typedef enum {
  H2O_STYLE_PSEUDO_CLASS_NORMAL = 0,
  H2O_STYLE_PSEUDO_CLASS_SELECTABLE,
  H2O_STYLE_PSEUDO_CLASS_GRAYOUT,
  H2O_STYLE_PSEUDO_CLASS_HIGHLIGHT,
  H2O_STYLE_PSEUDO_CLASS_LINK,
  H2O_STYLE_PSEUDO_CLASS_VISITED,
  H2O_STYLE_PSEUDO_CLASS_HOVER,
  H2O_STYLE_PSEUDO_CLASS_FOCUS,
  H2O_STYLE_PSEUDO_CLASS_ACTIVE,
  H2O_STYLE_PSEUDO_CLASS_ENABLED,
  H2O_STYLE_PSEUDO_CLASS_DISABLED,
  H2O_STYLE_PSEUDO_CLASS_NOT,
  H2O_STYLE_PSEUDO_CLASS_HELP,
  H2O_STYLE_PSEUDO_CLASS_MAX
} H2O_STYLE_PSEUDO_CLASS;

typedef enum {
  H2O_CONTROL_ID_DIALOG_TITLE = 1,
  H2O_CONTROL_ID_DIALOG_BODY,
  H2O_CONTROL_ID_DIALOG_BODY_INPUT,
  H2O_CONTROL_ID_DIALOG_BUTTON,
  H2O_CONTROL_ID_MAX
} H2O_CONTROL_ID;

typedef struct _H2O_CONTROL_INFO {
  UINT32                                       ControlId;
  H2O_STYLE_INFO                               ControlStyle;

  RECT                                         ControlField;
  RECT                                         ControlRelativeField; ///< from panel

  H2O_STRING_INFO                              Text;
  H2O_STRING_INFO                              HelpText;
  H2O_STRING_INFO                              ValueStrInfo;
  H2O_IMAGE_INFO                               BackgroundImage;
  EFI_IMAGE_INPUT                              HelpImage;
  EFI_HII_VALUE                                HiiValue;

  H2O_PANEL_INFO                               *ParentPanel;
  BOOLEAN                                      Editable;
  //
  // Statement
  //
  H2O_PAGE_ID                                  PageId;
  H2O_STATEMENT_ID                             StatementId;
  EFI_QUESTION_ID                              QuestionId;
  UINT8                                        Operand;
  EFI_IFR_OP_HEADER                            *IfrOpCode;
  BOOLEAN                                      Visible;
  BOOLEAN                                      Selectable;
  BOOLEAN                                      Modifiable;

  UINT64                                       Minimum;
  UINT64                                       Maximum;
  UINT64                                       Step;

  //
  // Display item sequence for date/time
  //  Date:      Month/Day/Year
  //  Sequence:  0     1   2
  //
  //  Time:      Hour : Minute : Second
  //  Sequence:  0      1        2
  //
  UINT8                                        Sequence;
} H2O_CONTROL_INFO;

typedef struct _H2O_CONTROL_LIST {
  UINT32                                       Count;
  H2O_CONTROL_INFO                             *ControlArray;
} H2O_CONTROL_LIST;

#define H2O_PANEL_INFO_NODE_SIGNATURE SIGNATURE_32 ('P', 'A', 'N', 'E')
struct _H2O_PANEL_INFO {
  UINT32                                       Signature;
  LIST_ENTRY                                   Link;
  UINT32                                       PanelId;
  UINT32                                       PanelType; ///< See H2O_PANEL_TYPE_xxx.
  EFI_GUID                                     PanelTypeGuid;

  RECT                                         PanelField;
  RECT                                         PanelRelField; ///< for page-up and page-down

  H2O_IMAGE_INFO                               ContentsImage;
  H2O_IMAGE_INFO                               BackgroundImage;

  UINT32                                       OrderFlag; ///< See H2O_PANEL_ORDER_FLAG_xxxx.
  BOOLEAN                                      WithScrollBarVertical;
  BOOLEAN                                      WithScrollBarHorizontal;

  H2O_LAYOUT_INFO                              *ParentLayout;
  LIST_ENTRY                                   StyleListHead;
  H2O_CONTROL_LIST                             ControlList;
} ;
#define H2O_PANEL_INFO_NODE_FROM_LINK(a) CR (a, H2O_PANEL_INFO, Link, H2O_PANEL_INFO_NODE_SIGNATURE)

#define H2O_PANEL_ORDER_FLAG_HORIZONTAL_FIRST  0x00000001 ///< Default Vertical First Order
#define H2O_PANEL_ORDER_FLAG_NO_SCROLL_BAR     0x00000002
#define H2O_PANEL_ORDER_FLAG_JUSTIFIED         0x00000004
#define H2O_PANEL_ORDER_FLAG_MINIMUM           0x00000008

#define DISPLAY_NONE                           0x00000000 ///< Default
#define DISPLAY_LEFT                           0x00000001
#define DISPLAY_TOP                            0x00000002
#define DISPLAY_RIGHT                          0x00000004
#define DISPLAY_BOTTOM                         0x00000008

typedef struct _H2O_PANEL_LIST {
  UINT32                                       Count;
  H2O_PANEL_INFO                               *PanelArray;
} H2O_PANEL_LIST;

#define H2O_LAYOUT_INFO_NODE_SIGNATURE SIGNATURE_32 ('L', 'A', 'Y', 'O')
struct _H2O_LAYOUT_INFO {
  UINT32                                       Signature;
  LIST_ENTRY                                   Link;
  UINT32                                       LayoutId;
  EFI_GUID                                     DisplayEngineGuid[3];
  UINT32                                       Revision;
  LIST_ENTRY                                   PanelListHead;
  LIST_ENTRY                                   HotkeyListHead;
  LIST_ENTRY                                   StyleListHead;
};
#define H2O_LAYOUT_INFO_NODE_FROM_LINK(a) CR (a, H2O_LAYOUT_INFO, Link, H2O_LAYOUT_INFO_NODE_SIGNATURE)

typedef enum {
  LAYOUT_REVISION_UNKNOWN = 0, // not init
  LAYOUT_REVISION_1,           // visibility property value type is boolean, layout id is a property
  LAYOUT_REVISION_2,           // visibility property value type is keyword, layout id is an attribute
  LAYOUT_REVISION_MAX
} LAYOUT_REVISION;

typedef struct _H2O_VFR_INFO H2O_VFR_INFO;
typedef struct _H2O_FORMSET_INFO H2O_FORMSET_INFO;
typedef struct _H2O_FORM_INFO H2O_FORM_INFO;
typedef struct _H2O_STATEMENT_INFO H2O_STATEMENT_INFO;
typedef struct _H2O_HOTKEY_INFO H2O_HOTKEY_INFO;
typedef struct _H2O_IMPORT_INFO H2O_IMPORT_INFO;

#define H2O_HOTKEY_INFO_NODE_SIGNATURE SIGNATURE_32 ('H', 'O', 'T', 'K')
struct _H2O_HOTKEY_INFO {
  UINT32                                       Signature;
  LIST_ENTRY                                   Link;
  H2O_HII_LIBT_EXT_HOTKEY_BLOCK                *HotkeyBlock;
  LIST_ENTRY                                   StyleListHead;
};
#define H2O_HOTKEY_INFO_NODE_FROM_LINK(a) CR (a, H2O_HOTKEY_INFO, Link, H2O_HOTKEY_INFO_NODE_SIGNATURE)

#define H2O_IMPORT_INFO_NODE_SIGNATURE SIGNATURE_32 ('I', 'M', 'P', 'O')
struct _H2O_IMPORT_INFO {
  UINT32                                       Signature;
  LIST_ENTRY                                   Link;
  UINT16                                       Flags;
  EFI_GUID                                     TargetFormsetGuid;
  EFI_FORM_ID                                  TargetFormId;
  EFI_QUESTION_ID                              TargetId;
  EFI_QUESTION_ID                              LocalId;
};
#define H2O_IMPORT_INFO_NODE_FROM_LINK(a) CR (a, H2O_IMPORT_INFO, Link, H2O_IMPORT_INFO_NODE_SIGNATURE)

#define H2O_STATEMENT_INFO_NODE_SIGNATURE SIGNATURE_32 ('H', 'S', 'I', 'N')
struct _H2O_STATEMENT_INFO {
  UINT32                                       Signature;
  LIST_ENTRY                                   Link;
  BOOLEAN                                      IsQuestion;
  UINT16                                       Position;
  UINT16                                       StatementId;
  EFI_QUESTION_ID                              QuestionId;
  LIST_ENTRY                                   HotkeyListHead;
  LIST_ENTRY                                   StyleListHead;
};
#define H2O_STATEMENT_INFO_NODE_FROM_LINK(a) CR (a, H2O_STATEMENT_INFO, Link, H2O_STATEMENT_INFO_NODE_SIGNATURE)

#define H2O_FORM_INFO_NODE_SIGNATURE SIGNATURE_32 ('F', 'O', 'R', 'M')
struct _H2O_FORM_INFO {
  UINT32                                       Signature;
  LIST_ENTRY                                   Link;
  H2O_HII_LIBT_EXT_FORM_BLOCK                  *FormBlock;
  LIST_ENTRY                                   HotkeyListHead;
  LIST_ENTRY                                   StyleListHead;
  LIST_ENTRY                                   ImportListHead;
  LIST_ENTRY                                   StatementListHead;
};
#define H2O_FORM_INFO_NODE_FROM_LINK(a) CR (a, H2O_FORM_INFO, Link, H2O_FORM_INFO_NODE_SIGNATURE)

#define H2O_FORMSET_INFO_NODE_SIGNATURE SIGNATURE_32 ('F', 'M', 'S', 'T')
struct _H2O_FORMSET_INFO {
  UINT32                                       Signature;
  LIST_ENTRY                                   Link;
  EFI_GUID                                     FormsetId;
  H2O_HII_LIBT_EXT_FORMSET_BLOCK               *FormsetBlock;
  LIST_ENTRY                                   FormListHead;
  LIST_ENTRY                                   HotkeyListHead;
  LIST_ENTRY                                   StyleListHead;
};
#define H2O_FORMSET_INFO_NODE_FROM_LINK(a) CR (a, H2O_FORMSET_INFO, Link, H2O_FORMSET_INFO_NODE_SIGNATURE)

#define H2O_VFR_INFO_NODE_SIGNATURE SIGNATURE_32 ('V', 'F', 'R', 'I')
struct _H2O_VFR_INFO {
  UINT32                                       Signature;
  LIST_ENTRY                                   Link;
  UINT32                                       VfrId;
  LIST_ENTRY                                   FormsetListHead;
};
#define H2O_VFR_INFO_NODE_FROM_LINK(a) CR (a, H2O_VFR_INFO, Link, H2O_VFR_INFO_NODE_SIGNATURE)

typedef struct {
  CHAR16              *KeyStr;
  CHAR16              *PromptStrTokenName;
  CHAR16              *HelpStrTokenName;
  EFI_IMAGE_INPUT     *Image;
  BOOLEAN             Display;
  UINT8               GroupId;
  EFI_GUID            FormSetGuid;
  UINT16              FormId;
  UINT16              QuestionId;
  UINT32              HotKeyAction;
  UINT16              HotKeyDefaultId;
  EFI_GUID            HotKeyTargetFormSetGuid;
  UINT16              HotKeyTargetFormId;
  UINT16              HotKeyTargetQuestionId;
  EFI_HII_VALUE       HotKeyHiiValue;
} H2O_HOTKEY;

#define IMPORT_INFO_FLAG_IS_QUESTION(Flag) ((BOOLEAN) ((Flag & HII_H2O_HII_IMPORT_QUESTION) == HII_H2O_HII_IMPORT_QUESTION))

typedef struct {
  UINT16              Flags;
  EFI_GUID            SrcFormSetGuid;
  UINT16              SrcFormId;
  UINT16              SrcId;

  UINT16              DstFormId;
  UINT16              DstId;
} IMPORT_INFO;

EFI_STATUS
GetImportInfoList (
  IN  EFI_GUID                                *FormsetGuid,
  OUT IMPORT_INFO                             **ImportInfoList,
  OUT UINT16                                  *ImportInfoCount
  );

EFI_STATUS
GetFormsetLayoutByGuid (
  IN  EFI_GUID                                *FormsetGuid,
  OUT H2O_FORMSET_INFO                        **Formset
  );

H2O_FORM_INFO *
GetFormLayoutByFormId (
  IN H2O_FORMSET_INFO                         *Formset,
  IN UINT32                                   FormId
  );

H2O_STATEMENT_INFO *
GetStatementLayoutByQuestionId (
  IN H2O_FORMSET_INFO                            *Formset,
  IN H2O_FORM_INFO                               *FormInfo,
  IN BOOLEAN                                     IsQuestion,
  IN UINT16                                      Id
  );

EFI_STATUS
GetPropertyFromStyleList (
  IN  LIST_ENTRY                              *StyleListHead,
  IN  UINT32                                  StyleType,
  IN  UINT32                                  PseudoClass,
  IN  CHAR8                                   *IdentifierStr,
  IN  RECT                                    *RelativeField OPTIONAL,
  OUT H2O_PROPERTY_INFO                       **Property,
  OUT H2O_PROPERTY_VALUE                      *PropValue
  );

EFI_STATUS
BltCopy(
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *Blt,
  IN     UINTN                           BltStartX,
  IN     UINTN                           BltStartY,
  IN     UINTN                           BltWidth,
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *DestBlt,
  IN     UINTN                           DestStartX,
  IN     UINTN                           DestStartY,
  IN     UINTN                           DestWidth,
  IN     UINTN                           DestHeight,
  IN     UINTN                           CopyWidth,
  IN     UINTN                           CopyHeight
  );

EFI_STATUS
BltCombine(
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL   **DestBlt,
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *BgBlt,
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *FgBlt,
  IN     UINTN                           BltWidth,
  IN     UINTN                           BltHeight
  );

EFI_STATUS
BltTransparent(
  IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL   **DestBlt,
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *SrcBlt,
  IN     UINTN                           BltWidth,
  IN     UINTN                           BltHeight
  );

EFI_STATUS
CreateImageByColor (
  IN     EFI_GRAPHICS_OUTPUT_BLT_PIXEL       *Rgb,
  IN     RECT                                *Field,
  IN OUT EFI_IMAGE_INPUT                     *ImageInput
  );

EFI_STATUS
GetLayoutById (
  IN     UINT32                                  LayoutId,
  IN     EFI_GUID                                *DisplayEngineGuid,
  OUT    H2O_LAYOUT_INFO                         **Layout
  );

EFI_STATUS
EFIAPI
GetLayoutIdByGuid (
  IN     EFI_GUID                                *FormsetGuid,
  OUT    UINT32                                  *LayoutId
  );

EFI_STATUS
GetImageById (
  IN     UINT32                                ImageId,
  OUT    EFI_IMAGE_INPUT                       *ImageInput
  );

EFI_STATUS
SeparateStringByFont (
  IN     CHAR16                                *String,
  IN     EFI_FONT_DISPLAY_INFO                 *FontDisplayInfo,
  IN     UINT32                                LimitWidth,
  OUT    EFI_IMAGE_OUTPUT                      **CharImageOutArray OPTIONAL,
  OUT    UINT32                                *SeparateStrCount,
  OUT    UINT32                                **SeparateStrOffsetArray OPTIONAL,
  OUT    UINT32                                **SeparateStrWidthArray OPTIONAL,
  OUT    UINTN                                 **SeparateStrArray
  );

EFI_STATUS
GetStringImageByFont (
  IN     CHAR16                                *String,
  IN     RECT                                  *Field,
  IN     EFI_FONT_DISPLAY_INFO                 *FontDisplayInfo,
  OUT    EFI_IMAGE_OUTPUT                      **ImageOutput
  );

BOOLEAN
IsVisibility (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     H2O_VFR_PROPERTY                      *VfrProperty,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass
  );

EFI_STATUS
GetPanelField (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     H2O_VFR_PROPERTY                      *VfrProperty,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  IN     RECT                                  *RelativeField,
  OUT    RECT                                  *PanelField
  );

EFI_STATUS
GetForegroundColor (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     H2O_VFR_PROPERTY                      *VfrProperty,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  OUT    H2O_COLOR_INFO                        *Color
  );

EFI_STATUS
GetBackgroundColor (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     H2O_VFR_PROPERTY                      *VfrProperty,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  OUT    H2O_COLOR_INFO                        *Color
  );

EFI_STATUS
GetPanelColorAttribute (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     H2O_VFR_PROPERTY                      *VfrProperty,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  OUT    UINT32                                *PanelColorAttribute
  );

UINT32
GetBorderWidth (
  IN     H2O_PANEL_INFO                        *Panel,
  IN     H2O_VFR_PROPERTY                      *VfrProperty,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass
  );

EFI_STATUS
GetHeight(
  IN     H2O_PANEL_INFO                        *Panel,
  IN     H2O_VFR_PROPERTY                      *VfrProperty,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  IN     RECT                                  *RelativeField,
  OUT    INT32                                 *Height
  );

EFI_STATUS
GetWidth(
  IN     H2O_PANEL_INFO                        *Panel,
  IN     H2O_VFR_PROPERTY                      *VfrProperty,
  IN     UINT32                                StyleType,
  IN     UINT32                                PseudoClass,
  IN     RECT                                  *RelativeField,
  OUT    INT32                                 *Width
  );

EFI_STATUS
GetHotKeyList (
  IN  EFI_GUID                                *FormsetGuid,
  IN  H2O_FORM_ID                             FormId,
  OUT UINT32                                  *HotKeyInfoCount,
  OUT HOT_KEY_INFO                            **HotKeyInfo
  );

EFI_STATUS
GetHotKeyListByQuestion (
  IN  EFI_GUID                                *FormsetGuid,
  IN  H2O_FORM_ID                             FormId,
  IN  BOOLEAN                                 IsQuestion,
  IN  UINT16                                  Id,
  OUT UINT32                                  *HotKeyInfoCount,
  OUT HOT_KEY_INFO                            **HotKeyInfo
  );

EFI_STATUS
GetHotKeyListByLayoutInfo (
  IN  H2O_LAYOUT_INFO                          *LayoutInfo,
  IN  EFI_GUID                                 *FormsetGuid,
  IN  H2O_FORM_ID                              FormId,
  IN  EFI_QUESTION_ID                          QuestionId,
  OUT UINT32                                   *HotKeyInfoCount,
  OUT HOT_KEY_INFO                             **HotKeyInfo
  );

H2O_PANEL_INFO *
GetPanelInfo (
  IN H2O_LAYOUT_INFO                           *LayoutInfo,
  IN UINT32                                    PanelId
  );

UINT32
GetStyleTypeByOpCode (
  IN    UINT8                                  OpCode
  );

VOID
SafeFreePool(
  IN   VOID                                **Buffer
  );

UINT32
GetPseudoClassByStatement (
  IN H2O_FORM_BROWSER_S                        *Statement
  );

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
  );

EFI_STATUS
GetVfcfFormsetPropertyValue (
  IN  EFI_GUID                                 *FormsetGuid,
  IN  UINT32                                   StyleType,
  IN  UINT32                                   PseudoClass,
  IN  CHAR8                                    *IdentifierStr,
  IN  RECT                                     *RelativeField OPTIONAL,
  OUT H2O_PROPERTY_INFO                        **PropertyInfo,
  OUT H2O_PROPERTY_VALUE                       *PropertyValue
  );

EFI_STATUS
GetVfcfFormPropertyValue (
  IN  EFI_GUID                                 *FormsetGuid,
  IN  UINT16                                   FormId,
  IN  UINT32                                   StyleType,
  IN  UINT32                                   PseudoClass,
  IN  CHAR8                                    *IdentifierStr,
  IN  RECT                                     *RelativeField OPTIONAL,
  OUT H2O_PROPERTY_INFO                        **PropertyInfo,
  OUT H2O_PROPERTY_VALUE                       *PropertyValue
  );

EFI_STATUS
GetVfcfStatementPropertyValue (
  IN  EFI_GUID                                 *FormsetGuid,
  IN  BOOLEAN                                  IsQuestion,
  IN  UINT16                                   Id,
  IN  UINT32                                   StyleType,
  IN  UINT32                                   PseudoClass,
  IN  CHAR8                                    *IdentifierStr,
  IN  RECT                                     *RelativeField OPTIONAL,
  OUT H2O_PROPERTY_INFO                        **PropertyInfo,
  OUT H2O_PROPERTY_VALUE                       *PropertyValue
  );

BOOLEAN
HaveVfcfFormsetInfo (
  IN EFI_GUID                                  *FormsetGuid
  );

#endif

