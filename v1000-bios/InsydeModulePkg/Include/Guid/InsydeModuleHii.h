/** @file

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


#ifndef _INSYDEMODULE_HII_H
#define _INSYDEMODULE_HII_H

#define H2O_BDS_LOAD_OPTION_FRONT_PAGE_GUID          { 0xe0ba9b98, 0xdd2d, 0x4434, { 0xbb, 0x94, 0x59, 0x9c, 0xc9, 0xe4, 0x30, 0x5d } }
#define H2O_BDS_LOAD_OPTION_BOOT_MAINTENANCE_GUID    { 0x1c54c333, 0x24ff, 0x4506, { 0xa9, 0xd6, 0x0a, 0x62, 0x4e, 0x09, 0xae, 0x7e } }
#define H2O_BDS_LOAD_OPTION_BOOT_MANAGER_GUID        { 0x77e79a1e, 0xe1fb, 0x491f, { 0xa7, 0xc1, 0xfa, 0x1b, 0x54, 0x12, 0x53, 0x2a } }
#define H2O_BDS_LOAD_OPTION_DEVICE_MANAGER_GUID      { 0x8f1c1ac6, 0xfbc0, 0x4dab, { 0xa8, 0xbe, 0xb4, 0x12, 0xa1, 0x3c, 0x8b, 0x45 } }
#define H2O_BDS_LOAD_OPTION_SECURE_BOOT_MANAGER_GUID { 0x7517821f, 0xd9e1, 0x44c1, { 0xa7, 0x5a, 0xd0, 0x54, 0xce, 0xf3, 0xf8, 0xf8 } }

#pragma pack(1)
typedef struct {
  VENDOR_DEVICE_PATH                 VendorDevicePath;
  EFI_DEVICE_PATH_PROTOCOL           End;
} H2O_BDS_LOAD_OPTION_VENDOR_DEVICE_PATH;
#pragma pack()

///
/// GUIDed opcodes defined for H2O implementation.
///
#define H2O_IFR_EXT_GUID \
  { \
    0x38237648, 0x09cc, 0x47c4, {0x8b, 0x5f, 0xb0, 0x9f, 0x06, 0x89, 0x0d, 0xf7} \
  }

#pragma pack(1)

//
// For H2O_VALUE
//
#define H2O_VALUE_TYPE_UNDEFINED             0x0000
#define H2O_VALUE_TYPE_BOOLEAN               0x0001
#define H2O_VALUE_TYPE_UINT32                0x0002
#define H2O_VALUE_TYPE_INT32                 0x0003
#define H2O_VALUE_TYPE_PERCENTAGE            0x0004
#define H2O_VALUE_TYPE_UINT8                 0x0005
#define H2O_VALUE_TYPE_UINT16                0x0006
#define H2O_VALUE_TYPE_UINT64                0x0007
#define H2O_VALUE_TYPE_INT8                  0x0008
#define H2O_VALUE_TYPE_INT16                 0x0009
#define H2O_VALUE_TYPE_INT64                 0x000A
#define H2O_VALUE_TYPE_STR16                 0x000B
#define H2O_VALUE_TYPE_GUID                  0x000C
#define H2O_VALUE_TYPE_STRID                 0x000D
#define H2O_VALUE_TYPE_IMGID                 0x000E
#define H2O_VALUE_TYPE_BUFFER                0x000F
#define H2O_VALUE_TYPE_DATE                  0x0010
#define H2O_VALUE_TYPE_TIME                  0x0011
#define H2O_VALUE_TYPE_STR8                  0x0012
#define H2O_VALUE_TYPE_COLOR                 0x0013

typedef struct {
  UINT32                                     BufferSize;
  UINT8                                      *Buffer;
} H2O_BUFFER;

typedef struct {
  UINT16                                     Year;
  UINT8                                      Month;
  UINT8                                      Day;
} H2O_DATE;

typedef struct {
  UINT8                                      Hour;
  UINT8                                      Minute;
  UINT8                                      Second;
} H2O_TIME;

typedef struct {
  UINT32                                     Argb;
  UINT8                                      TextModeAttribute;
} H2O_COLOR;

typedef union {
  VOID                                       *Ptr;
  BOOLEAN                                    Bool;
  UINT8                                      Pct;
  CHAR8                                      C8;
  CHAR16                                     C16;
  UINT8                                      U8;
  UINT16                                     U16;
  UINT32                                     U32;
  UINT64                                     U64;
  INT8                                       I8;
  INT16                                      I16;
  INT32                                      I32;
  INT64                                      I64;
  CHAR8                                      *Str8;
  CHAR16                                     *Str16;
  EFI_GUID                                   Guid;
  H2O_DATE                                   Date;
  H2O_TIME                                   Time;
  H2O_BUFFER                                 Buffer;
  H2O_COLOR                                  Color;
} H2O_VALUE_TYPE_VALUE;

typedef struct {
  UINT32                                     Type;
  H2O_VALUE_TYPE_VALUE                       Value;
} H2O_VALUE;

///
/// H2O implementation extension opcodes, new extension can be added here later.
///
#define H2O_IFR_EXT_HOTKEY            0x0000
#define H2O_IFR_EXT_LAYOUT            0x0001
#define H2O_IFR_EXT_STYLE             0x0002
#define H2O_IFR_EXT_HELPIMAGE         0x0003
#define H2O_IFR_EXT_PROPERTY          0x0004
#define H2O_IFR_EXT_STYLE_REF         0x0005
#define H2O_IFR_EXT_TEXT              0x0006
#define H2O_IFR_EXT_LINK              0x0007
#define H2O_IFR_EXT_EXT_FLAGS         0x000C
#define H2O_IFR_EXT_INCLUDE_FORM_SET  0x000D
#define H2O_IFR_EXT_EXCLUDE_FORM_SET  0x000E
#define H2O_IFR_EXT_PCD               0x000F

//
// PCD definitions
//
typedef struct _H2O_IFR_GUID_PCD {
  EFI_IFR_OP_HEADER        Header;                  // Standard IFR opcode header, where OpCode is EFI_IFR_GUID_OP.
  EFI_GUID                 Guid;                    // The standard EFI_IFR_GUID header, where Guid is H2O_IFR_EXT_GUID.
  UINT16                   Function;                // Enumerated value that specifies the extended opcode, where Function is H2O_IFR_EXT_PCD.
  EFI_GUID                 PcdGuid;                 // GUID that, along with PcdTokenNumber, uniquely identifies the PCD.
  UINT32                   PcdTokenNumber;          // Unsigned integer that, along with PcdGuid, uniquely identifies the PCD.
} H2O_IFR_GUID_PCD;

//
// Link definitions
//
typedef struct _H2O_IFR_GUID_LINK {
  EFI_IFR_OP_HEADER   Header;
  ///
  /// H2O_IFR_EXT_GUID.
  ///
  EFI_GUID            Guid;
  ///
  /// H2O_IFR_EXT_LINK
  ///
  UINT16              Function;
} H2O_IFR_GUID_LINK;

///
/// HotKey opcode.
///
typedef struct _H2O_IFR_GUID_HOTKEY {
  EFI_IFR_OP_HEADER   Header;
  ///
  /// H2O_IFR_EXT_GUID.
  ///
  EFI_GUID            Guid;
  ///
  /// H2O_IFR_EXT_HOTKEY
  ///
  UINT16              Function;
  UINT8               HotKeyOffset;
} H2O_IFR_GUID_HOTKEY;

///
/// Layout opcode.
///
typedef struct _H2O_IFR_GUID_LAYOUT {
  EFI_IFR_OP_HEADER   Header;
  ///
  /// H2O_IFR_EXT_GUID.
  ///
  EFI_GUID            Guid;
  ///
  /// H2O_IFR_EXT_LAYOUT
  ///
  UINT16              Function;
  UINT16              LayoutId;
} H2O_IFR_GUID_LAYOUT;

///
/// Style opcode.
///
typedef struct _H2O_IFR_GUID_STYLE {
  EFI_IFR_OP_HEADER   Header;
  ///
  /// H2O_IFR_EXT_GUID.
  ///
  EFI_GUID            Guid;
  ///
  /// H2O_IFR_EXT_STYLE
  ///
  UINT16              Function;
  UINT8               Type;
  UINT8               ClassNameOffset;
  UINT8               PseudoClassOffset;
//CHAR8               Selector[1];
} H2O_IFR_GUID_STYLE;

///
/// Style type
///
#define H2O_IFR_STYLE_TYPE_LAYOUT      0x00
#define H2O_IFR_STYLE_TYPE_PANEL       0x01
#define H2O_IFR_STYLE_TYPE_FORM        0x02
#define H2O_IFR_STYLE_TYPE_FORMSET     0x03
#define H2O_IFR_STYLE_TYPE_ACTION      0x04
#define H2O_IFR_STYLE_TYPE_CHECKBOX    0x05
#define H2O_IFR_STYLE_TYPE_DATE        0x06
#define H2O_IFR_STYLE_TYPE_GOTO        0x07
#define H2O_IFR_STYLE_TYPE_ONEOF       0x08
#define H2O_IFR_STYLE_TYPE_NUMERIC     0x09
#define H2O_IFR_STYLE_TYPE_ORDEREDLIST 0x0a
#define H2O_IFR_STYLE_TYPE_PASSWORD    0x0b
#define H2O_IFR_STYLE_TYPE_STRING      0x0c
#define H2O_IFR_STYLE_TYPE_TIME        0x0d
#define H2O_IFR_STYLE_TYPE_TEXT        0x0e
#define H2O_IFR_STYLE_TYPE_SUBTITLE    0x0f
#define H2O_IFR_STYLE_TYPE_RESETBUTTON 0x10
#define H2O_IFR_STYLE_TYPE_BUTTON      0x11
#define H2O_IFR_STYLE_TYPE_HOTKEY      0x12
#define H2O_IFR_STYLE_TYPE_GROUP       0x13
#define H2O_IFR_STYLE_TYPE_SHEET       0xff

///
/// HelpImage opcode.
///
typedef struct _H2O_IFR_GUID_HELPIMAGE {
  EFI_IFR_OP_HEADER   Header;
  ///
  /// H2O_IFR_EXT_GUID.
  ///
  EFI_GUID            Guid;
  ///
  /// H2O_IFR_EXT_HELPIMAGE
  ///
  UINT16              Function;
  EFI_IMAGE_ID        HelpImage;
} H2O_IFR_GUID_HELPIMAGE;

///
/// Property opcode.
///
typedef struct _H2O_IFR_GUID_PROPERTY {
  EFI_IFR_OP_HEADER   Header;
  ///
  /// H2O_IFR_EXT_GUID.
  ///
  EFI_GUID            Guid;
  ///
  /// H2O_IFR_EXT_PROPERTY
  ///
  UINT16              Function;
  UINT32              ValueType;
  H2O_VALUE           H2OValue;
  UINT8               IdentifierOffset;
  UINT8               ValueOffset;
//CHAR8               Property[1];
} H2O_IFR_GUID_PROPERTY;

///
/// Property value type
///
typedef enum {
 H2O_PROPERTY_VALUE_TYPE_ANIMATION = 1,
 H2O_PROPERTY_VALUE_TYPE_BOOLEAN,
 H2O_PROPERTY_VALUE_TYPE_COLOR,
 H2O_PROPERTY_VALUE_TYPE_COORD,
 H2O_PROPERTY_VALUE_TYPE_ENUM,
 H2O_PROPERTY_VALUE_TYPE_ENUMS,
 H2O_PROPERTY_VALUE_TYPE_GUID,
 H2O_PROPERTY_VALUE_TYPE_VALUE,
 H2O_PROPERTY_VALUE_TYPE_ID,
 H2O_PROPERTY_VALUE_TYPE_IMAGE,
 H2O_PROPERTY_VALUE_TYPE_KEYWORD,
 H2O_PROPERTY_VALUE_TYPE_KEYWORDS,
 H2O_PROPERTY_VALUE_TYPE_OVERRIDE,
 H2O_PROPERTY_VALUE_TYPE_SIZE,
 H2O_PROPERTY_VALUE_TYPE_SIZES,
 H2O_PROPERTY_VALUE_TYPE_STRING,
 H2O_PROPERTY_VALUE_TYPE_SUBELEMENT,
 H2O_PROPERTY_VALUE_TYPE_TOKEN,
 H2O_PROPERTY_VALUE_TYPE_MAX
} H2O_PROPERTY_VALUE_TYPE;

typedef enum {
  DISPLAY_SUB_ELEMENT_NAME_NONE = 0,
  DISPLAY_SUB_ELEMENT_NAME_FORM_IMAGE,
  DISPLAY_SUB_ELEMENT_NAME_FORM_TITLE,
  DISPLAY_SUB_ELEMENT_NAME_FORMSET_HELP,
  DISPLAY_SUB_ELEMENT_NAME_FORMSET_HELP_IMAGE,
  DISPLAY_SUB_ELEMENT_NAME_FORMSET_IMAGE,
  DISPLAY_SUB_ELEMENT_NAME_FORMSET_TITLE,
  DISPLAY_SUB_ELEMENT_NAME_GROUP,
  DISPLAY_SUB_ELEMENT_NAME_HELP,
  DISPLAY_SUB_ELEMENT_NAME_HELP_IMAGE,
  DISPLAY_SUB_ELEMENT_NAME_IMAGE,
  DISPLAY_SUB_ELEMENT_NAME_IMG_0,
  DISPLAY_SUB_ELEMENT_NAME_IMG_1,
  DISPLAY_SUB_ELEMENT_NAME_IMG_2,
  DISPLAY_SUB_ELEMENT_NAME_IMG_3,
  DISPLAY_SUB_ELEMENT_NAME_IMG_4,
  DISPLAY_SUB_ELEMENT_NAME_IMG_5,
  DISPLAY_SUB_ELEMENT_NAME_IMG_6,
  DISPLAY_SUB_ELEMENT_NAME_IMG_7,
  DISPLAY_SUB_ELEMENT_NAME_IMG_8,
  DISPLAY_SUB_ELEMENT_NAME_IMG_9,
  DISPLAY_SUB_ELEMENT_NAME_PROMPT,
  DISPLAY_SUB_ELEMENT_NAME_STR_0,
  DISPLAY_SUB_ELEMENT_NAME_STR_1,
  DISPLAY_SUB_ELEMENT_NAME_STR_2,
  DISPLAY_SUB_ELEMENT_NAME_STR_3,
  DISPLAY_SUB_ELEMENT_NAME_STR_4,
  DISPLAY_SUB_ELEMENT_NAME_STR_5,
  DISPLAY_SUB_ELEMENT_NAME_STR_6,
  DISPLAY_SUB_ELEMENT_NAME_STR_7,
  DISPLAY_SUB_ELEMENT_NAME_STR_8,
  DISPLAY_SUB_ELEMENT_NAME_STR_9,
  DISPLAY_SUB_ELEMENT_NAME_TEXT2,
  DISPLAY_SUB_ELEMENT_NAME_VALUE,
  DISPLAY_SUB_ELEMENT_NAME_VALUE_IMAGE,
  DISPLAY_SUB_ELEMENT_NAME_MAX
} DISPLAY_SUB_ELEMENT_NAME;

///
/// Style Ref opcode.
///
typedef struct _H2O_IFR_GUID_STYLE_REF {
  EFI_IFR_OP_HEADER   Header;
  ///
  /// H2O_IFR_EXT_GUID.
  ///
  EFI_GUID            Guid;
  ///
  /// H2O_IFR_EXT_STYLE_REF
  ///
  UINT16              Function;
  UINT8               Type;
  UINT8               ClassNameOffset;
  UINT8               PseudoClassOffset;
//CHAR8               Selector[1];
} H2O_IFR_GUID_STYLE_REF;

///
/// Extended Text opcode.
///
typedef struct _H2O_IFR_GUID_TEXT {
  EFI_IFR_OP_HEADER   Header;
  ///
  /// H2O_IFR_EXT_GUID.
  ///
  EFI_GUID            Guid;
  ///
  /// H2O_IFR_EXT_TEXT
  ///
  UINT16              Function;
  EFI_STRING_ID       Text;
} H2O_IFR_GUID_TEXT;

///
/// Extended Flags opcode.
///
typedef struct _H2O_IFR_GUID_EXT_FLAGS {
  EFI_IFR_OP_HEADER   Header;
  ///
  /// H2O_IFR_EXT_GUID.
  ///
  EFI_GUID            Guid;
  ///
  /// H2O_IFR_EXT_EXT_FLAGS
  ///
  UINT16              Function;
  UINT32              ExtFlags;
} H2O_IFR_GUID_EXT_FLAGS;

//
// Extended Flags
//
#define H2O_IFR_EXT_FLAG_SETUP_CHANGE_LIST_IGNORE 0x00000001

///
/// Include Guid opcode for Link opcode.
///
typedef struct _H2O_IFR_GUID_INCLUDE_FORM_SET {
  EFI_IFR_OP_HEADER   Header;
  ///
  /// H2O_IFR_EXT_GUID.
  ///
  EFI_GUID            Guid;
  ///
  /// H2O_IFR_EXT_INCLUDE_FORM_SET
  ///
  UINT16              Function;
  EFI_GUID            IncludeGuid;
} H2O_IFR_GUID_INCLUDE_FORM_SET;

///
/// Exclude Guid opcode for Link opcode.
///
typedef struct _H2O_IFR_GUID_EXCLUDE_FORM_SET {
  EFI_IFR_OP_HEADER   Header;
  ///
  /// H2O_IFR_EXT_GUID.
  ///
  EFI_GUID            Guid;
  ///
  /// H2O_IFR_EXT_EXCLUDE_FORM_SET
  ///
  UINT16              Function;
  EFI_GUID            ExcludeGuid;
} H2O_IFR_GUID_EXCLUDE_FORM_SET;

typedef UINT16 H2O_PANEL_ID;
typedef UINT16 H2O_LAYOUT_ID;

//
// Hii Layout Package
//
typedef struct _H2O_HII_LAYOUT_PACKAGE_HDR {
  EFI_HII_GUID_PACKAGE_HDR  GuidPkgHeader;
  UINT8                     *LayoutPkgBuffer;
} H2O_HII_LAYOUT_PACKAGE_HDR;

#define H2O_HII_PACKAGE_LAYOUTS    0xe0

typedef struct _H2O_LAYOUT_PACKAGE_HDR {
  EFI_HII_PACKAGE_HEADER   Header;
  UINT32                   HdrSize;
  UINT32                   LayoutBlockOffset;
} H2O_LAYOUT_PACKAGE_HDR;

typedef struct _H2O_HII_LAYOUT_BLOCK {
  UINT8                    BlockType;
  UINT8                    BlockSize;
//UINT8                    BlockData[1];
} H2O_HII_LAYOUT_BLOCK;

#define H2O_HII_LIBT_LAYOUT_BEGIN  0x01
#define H2O_HII_LIBT_LAYOUT_END    0x02
#define H2O_HII_LIBT_PANEL_BEGIN   0x03
#define H2O_HII_LIBT_PANEL_END     0x04
#define H2O_HII_LIBT_STYLE_BEGIN   0x05
#define H2O_HII_LIBT_STYLE_END     0x06
#define H2O_HII_LIBT_PROPERTY      0x07
#define H2O_HII_LIBT_LAYOUT_DUP    0x08
#define H2O_HII_LIBT_PANEL_DUP     0x09
#define H2O_HII_LIBT_EXT2          0x10
#define H2O_HII_LIBT_EXT4          0x11
#define H2O_HII_LIBT_FORMAT        0x12
#define H2O_HII_LIBT_ENUM          0x13
#define H2O_HII_LIBT_KEYWORD       0x14

typedef struct _H2O_HII_LIBT_LAYOUT_BEGIN_BLOCK {
  H2O_HII_LAYOUT_BLOCK     Header;
  H2O_LAYOUT_ID            LayoutId;
  EFI_GUID                 DisplayEngineGuid[3];
} H2O_HII_LIBT_LAYOUT_BEGIN_BLOCK;

typedef struct _H2O_HII_LIBT_LAYOUT_END_BLOCK {
  H2O_HII_LAYOUT_BLOCK     Header;
} H2O_HII_LIBT_LAYOUT_END_BLOCK;

typedef UINT32 H2O_PANEL_TYPE;

#define H2O_PANEL_TYPE_SCREEN          0x0000
#define H2O_PANEL_TYPE_HOTKEY          0x0001
#define H2O_PANEL_TYPE_SETUP_MENU      0x0003
#define H2O_PANEL_TYPE_SETUP_PAGE      0x0004
#define H2O_PANEL_TYPE_OWNER_DRAW      0x0007
#define H2O_PANEL_TYPE_FORM            0x0008
#define H2O_PANEL_TYPE_DISPLAY_ELEMENT 0x0009
#define H2O_PANEL_TYPE_GUID            0xffff

typedef struct _H2O_HII_LIBT_PANEL_BEGIN_BLOCK {
  H2O_HII_LAYOUT_BLOCK     Header;
  H2O_PANEL_ID             PanelId;
  H2O_PANEL_TYPE           PanelType;
  EFI_GUID                 PanelTypeGuid;
  EFI_GUID                 TargetFormsetGuid;
  UINT16                   TargetFormId;
} H2O_HII_LIBT_PANEL_BEGIN_BLOCK;

typedef struct _H2O_HII_LIBT_PANEL_END_BLOCK {
  H2O_HII_LAYOUT_BLOCK     Header;
} H2O_HII_LIBT_PANEL_END_BLOCK;

typedef struct _H2O_HII_LIBT_STYLE_BEGIN_BLOCK {
  H2O_HII_LAYOUT_BLOCK     Header;
  UINT32                   Type;
  UINT8                    ClassNameOffset;
  UINT8                    PseudoClassOffset;
//CHAR8                    Selector[1];
} H2O_HII_LIBT_STYLE_BEGIN_BLOCK;

typedef struct _H2O_HII_LIBT_STYLE_END_BLOCK {
  H2O_HII_LAYOUT_BLOCK     Header;
} H2O_HII_LIBT_STYLE_END_BLOCK;

typedef struct _H2O_HII_LIBT_PROPERTY_BLOCK {
  H2O_HII_LAYOUT_BLOCK     Header;
  UINT32                   ValueType;
  UINT8                    IdentifierOffset;
  UINT8                    ValueOffset;
  H2O_VALUE                H2OValue;
//CHAR8                    Property[1];
} H2O_HII_LIBT_PROPERTY_BLOCK;

typedef struct _H2O_HII_LIBT_FORMAT_BLOCK {
  H2O_HII_LAYOUT_BLOCK     Header;
  UINT32                   StyleType;
  UINT32                   ValueType;
  UINT8                    IdentifierOffset;
} H2O_HII_LIBT_FORMAT_BLOCK;

typedef struct _H2O_HII_LIBT_ENUM_BLOCK {
  H2O_HII_LAYOUT_BLOCK     Header;
  UINT32                   ValueNumber;
  UINT8                    ValueOffset;
} H2O_HII_LIBT_ENUM_BLOCK;

typedef struct _H2O_HII_LIBT_KEYWORD_BLOCK {
  H2O_HII_LAYOUT_BLOCK     Header;
  UINT8                    ValueOffset;
} H2O_HII_LIBT_KEYWORD_BLOCK;

typedef struct _H2O_HII_LIBT_LAYOUT_DUP_BLOCK {
  H2O_HII_LAYOUT_BLOCK     Header;
  H2O_LAYOUT_ID            LayoutId;
  H2O_LAYOUT_ID            OldLayoutId;
} H2O_HII_LIBT_LAYOUT_DUP_BLOCK;

typedef struct _H2O_HII_LIBT_PANEL_DUP_BLOCK {
  H2O_HII_LAYOUT_BLOCK     Header;
  H2O_PANEL_ID             PanelId;
  H2O_LAYOUT_ID            OldLayoutId;
  H2O_PANEL_ID             OldPanelId;
} H2O_HII_LIBT_PANEL_DUP_BLOCK;

typedef struct _H2O_HII_LIBT_EXT2_BLOCK {
  H2O_HII_LAYOUT_BLOCK     Header;
  UINT8                    BlockType;
  UINT16                   BlockSize;
//UINT8                    BlockData[1];
} H2O_HII_LIBT_EXT2_BLOCK;

typedef struct _H2O_HII_LIBT_EXT4_BLOCK {
  H2O_HII_LAYOUT_BLOCK     Header;
  UINT8                    BlockType;
  UINT32                   BlockSize;
//UINT8                    BlockData[1];
} H2O_HII_LIBT_EXT4_BLOCK;

//
// ======= Vfr Tree =========
//
typedef UINT16 H2O_VFR_ID;
typedef UINT16 H2O_FORM_ID;
typedef UINT16 H2O_HOTKEY_ID;

#define H2O_HII_LIBT_EXT_VFR         0x81
#define H2O_HII_LIBT_EXT_VFR_END     0x82
#define H2O_HII_LIBT_EXT_FORMSET     0x83
#define H2O_HII_LIBT_EXT_FORMSET_END 0x84
#define H2O_HII_LIBT_EXT_FORM        0x85
#define H2O_HII_LIBT_EXT_FORM_END    0x86
#define H2O_HII_LIBT_EXT_HOTKEY      0x87
#define H2O_HII_LIBT_EXT_HOTKEY_END  0x88
#define H2O_HII_LIBT_EXT_IMPORT      0x89
#define H2O_HII_LIBT_EXT_IMPORT_END  0x8A
#define H2O_HII_LIBT_EXT_STATEMENT      0x8B
#define H2O_HII_LIBT_EXT_STATEMENT_END  0x8C

typedef struct _H2O_HII_LIBT_EXT_VFR_BLOCK {
  H2O_HII_LAYOUT_BLOCK     Header;
  H2O_VFR_ID               VfrId;
} H2O_HII_LIBT_EXT_VFR_BLOCK;

typedef struct _H2O_HII_LIBT_EXT_VFR_END_BLOCK {
  H2O_HII_LAYOUT_BLOCK     Header;
} H2O_HII_LIBT_EXT_VFR_END_BLOCK;

typedef struct _H2O_HII_LIBT_EXT_FORMSET_BLOCK {
  H2O_HII_LAYOUT_BLOCK     Header;
  EFI_GUID                 FormsetId;
  EFI_GUID                 ClassId[3];
  UINT32                   LayoutId;         // Optional layout identifier.
  BOOLEAN                  IsNew;            // Optional keyword.
  EFI_STRING_ID            Title;            // Optional string identifier. Mandatory if new is specified.
  EFI_STRING_ID            Help;             // Optional string identifier. Mandatory if new is specified.
  EFI_ANIMATION_ID         Animation;        // Optional animation identifier. Mandatory if new is specified.
  EFI_IMAGE_ID             Image;            // Optional image identifier. Mandatory if new is specified.
  EFI_IMAGE_ID             HelpImage;        // Optional image identifier. Mandatory if new is specified.
} H2O_HII_LIBT_EXT_FORMSET_BLOCK;

typedef struct _H2O_HII_LIBT_EXT_FORMSET_END_BLOCK {
  H2O_HII_LAYOUT_BLOCK     Header;
} H2O_HII_LIBT_EXT_FORMSET_END_BLOCK;

typedef struct _H2O_HII_LIBT_EXT_FORM_BLOCK {
  H2O_HII_LAYOUT_BLOCK     Header;
  H2O_FORM_ID              FormId;
  UINT32                   LayoutId;         // Optional layout identifier.
  EFI_STRING_ID            Title;            // Optional string identifier. Mandatory if new is specified.
  EFI_STRING_ID            Help;             // Optional string identifier. Mandatory if new is specified.
  EFI_ANIMATION_ID         Animation;        // Optional animation identifier. Mandatory if new is specified.
  EFI_IMAGE_ID             Image;            // Optional image identifier. Mandatory if new is specified.
  EFI_IMAGE_ID             HelpImage;        // Optional image identifier. Mandatory if new is specified.
} H2O_HII_LIBT_EXT_FORM_BLOCK;

typedef struct _H2O_HII_LIBT_EXT_FORM_END_BLOCK {
  H2O_HII_LAYOUT_BLOCK     Header;
} H2O_HII_LIBT_EXT_FORM_END_BLOCK;

typedef UINT16  H2O_GROUP_ID;
typedef UINT16  H2O_ACTION_ID;

typedef struct _H2O_HII_LIBT_EXT_HOTKEY_BLOCK {
  H2O_HII_LAYOUT_BLOCK     Header;
  H2O_HOTKEY_ID            HotkeyId;
  EFI_STRING_ID            Prompt;
  EFI_IMAGE_ID             Image;
  EFI_STRING_ID            Help;
  EFI_IMAGE_ID             HelpImage;
  H2O_GROUP_ID             Group;
  H2O_ACTION_ID            Action;
  EFI_DEFAULT_ID           ActionDefaultId;
  EFI_FORM_ID              ActionFormId;
  EFI_GUID                 ActionFormsetId;
  EFI_QUESTION_ID          ActionQuestionId;
  H2O_VALUE                ActionQuestionValue;
  UINT32                   ActionScope;
} H2O_HII_LIBT_EXT_HOTKEY_BLOCK;

#define H2O_HOT_KEY_ACTION_SHOW_HELP              0x00000001
#define H2O_HOT_KEY_ACTION_SELECT_PREVIOUS_ITEM   0x00000002
#define H2O_HOT_KEY_ACTION_SELECT_NEXT_ITEM       0x00000003
#define H2O_HOT_KEY_ACTION_SELECT_PREVIOUS_MENU   0x00000004
#define H2O_HOT_KEY_ACTION_SELECT_NEXT_MENU       0x00000005
#define H2O_HOT_KEY_ACTION_MODIFY_PREVIOUS_VALUE  0x00000006
#define H2O_HOT_KEY_ACTION_MODIFY_NEXT_VALUE      0x00000007
#define H2O_HOT_KEY_ACTION_DISCARD                0x00000008
#define H2O_HOT_KEY_ACTION_DISCARD_AND_EXIT       0x00000009
#define H2O_HOT_KEY_ACTION_LOAD_DEFAULT           0x0000000A
#define H2O_HOT_KEY_ACTION_SAVE                   0x0000000B
#define H2O_HOT_KEY_ACTION_SAVE_AND_EXIT          0x0000000C
#define H2O_HOT_KEY_ACTION_ENTER                  0x0000000D
#define H2O_HOT_KEY_ACTION_CALLBACK               0x0000000E
#define H2O_HOT_KEY_ACTION_GOTO                   0x0000000F
#define H2O_HOT_KEY_ACTION_SET_QUESTION_VALUE     0x00000010

#define H2O_HOT_KEY_ACTION_SCOPE_CURRENT          0x00000000
#define H2O_HOT_KEY_ACTION_SCOPE_ALL              0x00000001
#define H2O_HOT_KEY_ACTION_SCOPE_FORMSET          0x00000002
#define H2O_HOT_KEY_ACTION_SCOPE_FORM             0x00000003
#define H2O_HOT_KEY_ACTION_SCOPE_QUESTION         0x00000004

typedef struct _H2O_HII_LIBT_EXT_HOTKEY_END_BLOCK {
  H2O_HII_LAYOUT_BLOCK     Header;
} H2O_HII_LIBT_EXT_HOTKEY_END_BLOCK;

#define HII_H2O_HII_IMPORT_QUESTION          BIT0

typedef struct _H2O_HII_LIBT_EXT_IMPORT_BLOCK {
  H2O_HII_LAYOUT_BLOCK     Header;
  UINT16                   Flags;
  EFI_GUID                 TargetFormsetGuid;
  EFI_FORM_ID              TargetFormId;
  EFI_QUESTION_ID          TargetId;
  EFI_QUESTION_ID          LocalId;
  UINT16                   TargetVarstoreSize;
  EFI_GUID                 TargetVarstoreGuid;
  //CHAR8                  TargetVarstoreName[1];
} H2O_HII_LIBT_EXT_IMPORT_BLOCK;

#define H2O_HII_STATEMENT_QUESTION      BIT0

typedef struct _H2O_HII_LIBT_EXT_STATEMENT_BLOCK {
  H2O_HII_LAYOUT_BLOCK     Header;
  UINT16                   Flags;
  UINT16                   Position;
  UINT16                   StatementId;
  EFI_QUESTION_ID          QuestionId;
} H2O_HII_LIBT_EXT_STATEMENT_BLOCK;

typedef struct _H2O_HII_LIBT_EXT_STATEMENT_END_BLOCK {
  H2O_HII_LAYOUT_BLOCK     Header;
} H2O_HII_LIBT_EXT_STATEMENT_END_BLOCK;

#pragma pack()

extern EFI_GUID gH2OIfrExtGuid;
extern EFI_GUID gH2OBdsLoadOptionFrontPageGuid;
extern EFI_GUID gH2OBdsLoadOptionBootMaintenanceGuid;
extern EFI_GUID gH2OBdsLoadOptionBootManagerGuid;
extern EFI_GUID gH2OBdsLoadOptionDeviceManagerGuid;
extern EFI_GUID gH2OBdsLoadOptionSecureBootManagerGuid;
extern EFI_GUID gH2OBdsLoadOptionSetupUtilityAppFileGuid;

#endif

