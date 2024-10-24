/** @file
 French Keyboard layout data.

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "FrenchKeyboardLayout.h"

GLOBAL_REMOVE_IF_UNREFERENCED H2O_KEYBOARD_LAYOUT_FRENCH_PACK_BIN  mH2OKeyboardLayoutFrenchBin = {
  sizeof (H2O_KEYBOARD_LAYOUT_FRENCH_PACK_BIN),   // Binary size

  //
  // EFI_HII_PACKAGE_HEADER
  //
  {
    sizeof (H2O_KEYBOARD_LAYOUT_FRENCH_PACK_BIN) - sizeof (UINT32),
    EFI_HII_PACKAGE_KEYBOARD_LAYOUT
  },
  1,  // LayoutCount
  //
  // EFI_HII_KEYBOARD_LAYOUT
  //
  sizeof (H2O_KEYBOARD_LAYOUT_FRENCH_PACK_BIN) - sizeof (UINT32) - sizeof (EFI_HII_PACKAGE_HEADER) - sizeof (UINT16), // LayoutLength
  H2O_USB_KEYBOARD_FRENCH_LAYOUT_KEY_GUID,  // KeyGuid
  sizeof (UINT16) + sizeof (EFI_GUID) + sizeof (UINT32) + sizeof (UINT8) + (H2O_KEYBOARD_LAYOUT_FRENCH_KEY_COUNT * sizeof (EFI_KEY_DESCRIPTOR)), // LayoutDescriptorStringOffset
  H2O_KEYBOARD_LAYOUT_FRENCH_KEY_COUNT, // DescriptorCount
  {
    //
    // EFI_KEY_DESCRIPTOR (total number is H2O_KEYBOARD_LAYOUT_FRENCH_KEY_COUNT)
    //
    {EfiKeyD1,         'a',      'A',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_CAPS_LOCK},
    {EfiKeyB5,         'b',      'B',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_CAPS_LOCK},
    {EfiKeyB3,         'c',      'C',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_CAPS_LOCK},
    {EfiKeyC3,         'd',      'D',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_CAPS_LOCK},
    {EfiKeyD3,         'e',      'E',  0x20AC,   0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_CAPS_LOCK},
    {EfiKeyC4,         'f',      'F',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_CAPS_LOCK},
    {EfiKeyC5,         'g',      'G',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_CAPS_LOCK},
    {EfiKeyC6,         'h',      'H',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_CAPS_LOCK},
    {EfiKeyD8,         'i',      'I',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_CAPS_LOCK},
    {EfiKeyC7,         'j',      'J',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_CAPS_LOCK},
    {EfiKeyC8,         'k',      'K',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_CAPS_LOCK},
    {EfiKeyC9,         'l',      'L',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_CAPS_LOCK},
    {EfiKeyC10,        'm',      'M',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_CAPS_LOCK},
    {EfiKeyB6,         'n',      'N',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_CAPS_LOCK},
    {EfiKeyD9,         'o',      'O',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_CAPS_LOCK},
    {EfiKeyD10,        'p',      'P',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_CAPS_LOCK},
    {EfiKeyC1,         'q',      'Q',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_CAPS_LOCK},
    {EfiKeyD4,         'r',      'R',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_CAPS_LOCK},
    {EfiKeyC2,         's',      'S',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_CAPS_LOCK},
    {EfiKeyD5,         't',      'T',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_CAPS_LOCK},
    {EfiKeyD7,         'u',      'U',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_CAPS_LOCK},
    {EfiKeyB4,         'v',      'V',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_CAPS_LOCK},
    {EfiKeyB1,         'w',      'W',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_CAPS_LOCK},
    {EfiKeyB2,         'x',      'X',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_CAPS_LOCK},
    {EfiKeyD6,         'y',      'Y',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_CAPS_LOCK},
    {EfiKeyD2,         'z',      'Z',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_CAPS_LOCK},
    {EfiKeyE1,         '&',      '1',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT},
    {EfiKeyE2,         0xE9,     '2',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT},
    {EfiKeyE3,         '"',      '3',  '#',      0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT},
    {EfiKeyE4,         '\'',     '4',  '{',      0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT},
    {EfiKeyE5,         '(',      '5',  '[',      0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT},
    {EfiKeyE6,         '-',      '6',  '|',      0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT},
    {EfiKeyE7,         0xE8,     '7',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT},
    {EfiKeyE8,         '_',      '8',  '\\',     0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT},
    {EfiKeyE9,         0xE7,     '9',  '^',      0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT},
    {EfiKeyE10,        0xE0,     '0',  '@',      0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT},
    {EfiKeyEnter,      0x0d,     0x0d, 0,        0,  EFI_NULL_MODIFIER,   0},
    {EfiKeyEsc,        0x1b,     0x1b, 0,        0,  EFI_NULL_MODIFIER,   0},
    {EfiKeyBackSpace,  0x08,     0x08, 0,        0,  EFI_NULL_MODIFIER,   0},
    {EfiKeyTab,        0x09,     0x09, 0,        0,  EFI_NULL_MODIFIER,   0},
    {EfiKeySpaceBar,   ' ',      ' ',  0,        0,  EFI_NULL_MODIFIER,   0},
    {EfiKeyE11,        ')',      0xB0, ']',      0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT},
    {EfiKeyE12,        '=',      '+',  '}',      0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT},
    {EfiKeyD11,        0x00,     0x00, 0,        0,  EFI_NULL_MODIFIER,          0},
    {EfiKeyD12,        '$',      0xA3, 0xA4,     0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT},
    {EfiKeyC12,        '*' ,     0xB5, 0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT},
    {EfiKeyC11,        0xF9,     '%',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT},
    {EfiKeyE0,         0xB2,     0x00, 0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT},
    {EfiKeyB0,         '<',      '>',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT},
    {EfiKeyB7,         ',',      '?',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT},
    {EfiKeyB8,         ';',      '.',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT},
    {EfiKeyB9,         ':',      '/',  0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT},
    {EfiKeyB10,        '!',      0xA7, 0,        0,  EFI_NULL_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT},
    {EfiKeyCapsLock,   0x00,     0x00, 0,        0,  EFI_CAPS_LOCK_MODIFIER,            0},
    {EfiKeyF1,         0x00,     0x00, 0,        0,  EFI_FUNCTION_KEY_ONE_MODIFIER,     0},
    {EfiKeyF2,         0x00,     0x00, 0,        0,  EFI_FUNCTION_KEY_TWO_MODIFIER,     0},
    {EfiKeyF3,         0x00,     0x00, 0,        0,  EFI_FUNCTION_KEY_THREE_MODIFIER,   0},
    {EfiKeyF4,         0x00,     0x00, 0,        0,  EFI_FUNCTION_KEY_FOUR_MODIFIER,    0},
    {EfiKeyF5,         0x00,     0x00, 0,        0,  EFI_FUNCTION_KEY_FIVE_MODIFIER,    0},
    {EfiKeyF6,         0x00,     0x00, 0,        0,  EFI_FUNCTION_KEY_SIX_MODIFIER,     0},
    {EfiKeyF7,         0x00,     0x00, 0,        0,  EFI_FUNCTION_KEY_SEVEN_MODIFIER,   0},
    {EfiKeyF8,         0x00,     0x00, 0,        0,  EFI_FUNCTION_KEY_EIGHT_MODIFIER,   0},
    {EfiKeyF9,         0x00,     0x00, 0,        0,  EFI_FUNCTION_KEY_NINE_MODIFIER,    0},
    {EfiKeyF10,        0x00,     0x00, 0,        0,  EFI_FUNCTION_KEY_TEN_MODIFIER,     0},
    {EfiKeyF11,        0x00,     0x00, 0,        0,  EFI_FUNCTION_KEY_ELEVEN_MODIFIER,  0},
    {EfiKeyF12,        0x00,     0x00, 0,        0,  EFI_FUNCTION_KEY_TWELVE_MODIFIER,  0},
    {EfiKeyPrint,      0x00,     0x00, 0,        0,  EFI_PRINT_MODIFIER,                0},
    {EfiKeySLck,       0x00,     0x00, 0,        0,  EFI_SCROLL_LOCK_MODIFIER,          0},
    {EfiKeyPause,      0x00,     0x00, 0,        0,  EFI_PAUSE_MODIFIER,                0},
    {EfiKeyIns,        0x00,     0x00, 0,        0,  EFI_INSERT_MODIFIER,               0},
    {EfiKeyHome,       0x00,     0x00, 0,        0,  EFI_HOME_MODIFIER,                 0},
    {EfiKeyPgUp,       0x00,     0x00, 0,        0,  EFI_PAGE_UP_MODIFIER,              0},
    {EfiKeyDel,        0x00,     0x00, 0,        0,  EFI_DELETE_MODIFIER,               0},
    {EfiKeyEnd,        0x00,     0x00, 0,        0,  EFI_END_MODIFIER,                  0},
    {EfiKeyPgDn,       0x00,     0x00, 0,        0,  EFI_PAGE_DOWN_MODIFIER,            0},
    {EfiKeyRightArrow, 0x00,     0x00, 0,        0,  EFI_RIGHT_ARROW_MODIFIER,          0},
    {EfiKeyLeftArrow,  0x00,     0x00, 0,        0,  EFI_LEFT_ARROW_MODIFIER,           0},
    {EfiKeyDownArrow,  0x00,     0x00, 0,        0,  EFI_DOWN_ARROW_MODIFIER,           0},
    {EfiKeyUpArrow,    0x00,     0x00, 0,        0,  EFI_UP_ARROW_MODIFIER,             0},
    {EfiKeyNLck,       0x00,     0x00, 0,        0,  EFI_NUM_LOCK_MODIFIER,             0},
    {EfiKeySlash,      '/',      '/',  0,        0,  EFI_NULL_MODIFIER,                 0},
    {EfiKeyAsterisk,   '*',      '*',  0,        0,  EFI_NULL_MODIFIER,                 0},
    {EfiKeyMinus,      '-',      '-',  0,        0,  EFI_NULL_MODIFIER,                 0},
    {EfiKeyPlus,       '+',      '+',  0,        0,  EFI_NULL_MODIFIER,                 0},
    {EfiKeyEnter,      0x0d,     0x0d, 0,        0,  EFI_NULL_MODIFIER,                 0},
    {EfiKeyOne,        '1',      '1',  0,        0,  EFI_END_MODIFIER,         EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_NUM_LOCK},
    {EfiKeyTwo,        '2',      '2',  0,        0,  EFI_DOWN_ARROW_MODIFIER,  EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_NUM_LOCK},
    {EfiKeyThree,      '3',      '3',  0,        0,  EFI_PAGE_DOWN_MODIFIER,   EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_NUM_LOCK},
    {EfiKeyFour,       '4',      '4',  0,        0,  EFI_LEFT_ARROW_MODIFIER,  EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_NUM_LOCK},
    {EfiKeyFive,       '5',      '5',  0,        0,  EFI_NULL_MODIFIER,        EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_NUM_LOCK},
    {EfiKeySix,        '6',      '6',  0,        0,  EFI_RIGHT_ARROW_MODIFIER, EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_NUM_LOCK},
    {EfiKeySeven,      '7',      '7',  0,        0,  EFI_HOME_MODIFIER,        EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_NUM_LOCK},
    {EfiKeyEight,      '8',      '8',  0,        0,  EFI_UP_ARROW_MODIFIER,    EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_NUM_LOCK},
    {EfiKeyNine,       '9',      '9',  0,        0,  EFI_PAGE_UP_MODIFIER,     EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_NUM_LOCK},
    {EfiKeyZero,       '0',      '0',  0,        0,  EFI_INSERT_MODIFIER,      EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_NUM_LOCK},
    {EfiKeyPeriod,     '.',      '.',  0,        0,  EFI_DELETE_MODIFIER,      EFI_AFFECTED_BY_STANDARD_SHIFT | EFI_AFFECTED_BY_NUM_LOCK},
    {EfiKeyA4,         0x00,     0x00, 0,        0,  EFI_MENU_MODIFIER,            0},
    {EfiKeyLCtrl,      0,        0,    0,        0,  EFI_LEFT_CONTROL_MODIFIER,    0},
    {EfiKeyLShift,     0,        0,    0,        0,  EFI_LEFT_SHIFT_MODIFIER,      0},
    {EfiKeyLAlt,       0,        0,    0,        0,  EFI_LEFT_ALT_MODIFIER,        0},
    {EfiKeyA0,         0,        0,    0,        0,  EFI_LEFT_LOGO_MODIFIER,       0},
    {EfiKeyRCtrl,      0,        0,    0,        0,  EFI_RIGHT_CONTROL_MODIFIER,   0},
    {EfiKeyRShift,     0,        0,    0,        0,  EFI_RIGHT_SHIFT_MODIFIER,     0},
    {EfiKeyA2,         0,        0,    0,        0,  EFI_ALT_GR_MODIFIER,          0},
    {EfiKeyA3,         0,        0,    0,        0,  EFI_RIGHT_LOGO_MODIFIER,      0},
  },
  1,                          // DescriptionCount
  {'f', 'r', '-', 'F', 'R'},  // RFC4646 language code
  ' ',                        // Space
  {H2O_KEYBOARD_LAYOUT_FRENCH_DESCRIPTION_STR}, // DescriptionString[]
};


