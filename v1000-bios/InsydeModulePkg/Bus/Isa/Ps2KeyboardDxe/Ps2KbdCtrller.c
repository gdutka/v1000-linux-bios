/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  Routines that access 8042 keyboard controller

Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Ps2Keyboard.h"

VOID
Ps2KeyboardCheckLEDHandler (
  IN KEYBOARD_CONSOLE_IN_DEV *ConsoleIn
  );

#define IS_MAKE_CODE(ScanCode)     ((BOOLEAN) (((ScanCode) & 0x80) == 0))
#define GET_MAKE_CODE(ScanCode)    ((ScanCode) & 0x7F)

#define ATTR_EXTEND_0_TRUE         BIT0
#define ATTR_EXTEND_0_FALSE        BIT1
#define ATTR_EXTEND_1_TRUE         BIT2
#define ATTR_EXTEND_1_FALSE        BIT3
#define ATTR_EXTEND_0_DONT_CARE    (ATTR_EXTEND_0_TRUE | ATTR_EXTEND_0_FALSE)
#define ATTR_EXTEND_1_DONT_CARE    (ATTR_EXTEND_1_TRUE | ATTR_EXTEND_1_FALSE)

struct {
  UINT8   ScanCode;
  UINT8   PrefixScanCodeAttr;
}
mEfiKeyToPs2KeyCodeConvertTable[] = {
  {SCANCODE_CTRL_MAKE       , ATTR_EXTEND_0_FALSE     | ATTR_EXTEND_1_FALSE    },  //  EfiKeyLCtrl
  {SCANCODE_LEFT_LOGO_MAKE  , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_FALSE    },  //  EfiKeyA0
  {SCANCODE_ALT_MAKE        , ATTR_EXTEND_0_FALSE     | ATTR_EXTEND_1_FALSE    },  //  EfiKeyLAlt
  {0x39                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeySpaceBar
  {SCANCODE_ALT_MAKE        , ATTR_EXTEND_0_TRUE      | ATTR_EXTEND_1_FALSE    },  //  EfiKeyA2
  {SCANCODE_RIGHT_LOGO_MAKE , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_FALSE    },  //  EfiKeyA3
  {SCANCODE_MENU_MAKE       , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_FALSE    },  //  EfiKeyA4
  {SCANCODE_CTRL_MAKE       , ATTR_EXTEND_0_TRUE      | ATTR_EXTEND_1_FALSE    },  //  EfiKeyRCtrl
  {0x4b                     , ATTR_EXTEND_0_TRUE      | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyLeftArrow
  {0x50                     , ATTR_EXTEND_0_TRUE      | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyDownArrow
  {0x4d                     , ATTR_EXTEND_0_TRUE      | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyRightArrow
  {0x52                     , ATTR_EXTEND_0_FALSE     | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyZero
  {0x53                     , ATTR_EXTEND_0_FALSE     | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyPeriod
  {0x1c                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyEnter
  {SCANCODE_LEFT_SHIFT_MAKE , ATTR_EXTEND_0_FALSE     | ATTR_EXTEND_1_FALSE    },  //  EfiKeyLShift
  {0x56                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyB0
  {0x2c                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyB1
  {0x2d                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyB2
  {0x2e                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyB3
  {0x2f                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyB4
  {0x30                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyB5
  {0x31                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyB6
  {0x32                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyB7
  {0x33                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyB8
  {0x34                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyB9
  {0x35                     , ATTR_EXTEND_0_FALSE     | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyB10
  {SCANCODE_RIGHT_SHIFT_MAKE, ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_FALSE    },  //  EfiKeyRShift
  {0x48                     , ATTR_EXTEND_0_TRUE      | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyUpArrow
  {0x4f                     , ATTR_EXTEND_0_FALSE     | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyOne
  {0x50                     , ATTR_EXTEND_0_FALSE     | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyTwo
  {0x51                     , ATTR_EXTEND_0_FALSE     | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyThree
  {SCANCODE_CAPS_LOCK_MAKE  , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_FALSE    },  //  EfiKeyCapsLock
  {0x1e                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyC1
  {0x1f                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyC2
  {0x20                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyC3
  {0x21                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyC4
  {0x22                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyC5
  {0x23                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyC6
  {0x24                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyC7
  {0x25                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyC8
  {0x26                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyC9
  {0x27                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyC10
  {0x28                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyC11
  {0x2b                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyC12 (ScanCode is the same as EfiKeyD13)
  {0x4b                     , ATTR_EXTEND_0_FALSE     | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyFour
  {0x4c                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyFive
  {0x4d                     , ATTR_EXTEND_0_FALSE     | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeySix
  {0x4e                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyPlus
  {0x0F                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyTab
  {0x10                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyD1
  {0x11                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyD2
  {0x12                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyD3
  {0x13                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyD4
  {0x14                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyD5
  {0x15                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyD6
  {0x16                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyD7
  {0x17                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyD8
  {0x18                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyD9
  {0x19                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyD10
  {0x1a                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyD11
  {0x1b                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyD12
  {0x2b                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyD13 (ScanCode is the same as EfiKeyC12)
  {0x53                     , ATTR_EXTEND_0_TRUE      | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyDel
  {0x4f                     , ATTR_EXTEND_0_TRUE      | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyEnd
  {0x51                     , ATTR_EXTEND_0_TRUE      | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyPgDn
  {0x47                     , ATTR_EXTEND_0_FALSE     | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeySeven
  {0x48                     , ATTR_EXTEND_0_FALSE     | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyEight
  {0x49                     , ATTR_EXTEND_0_FALSE     | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyNine
  {0x29                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyE0
  {0x02                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyE1
  {0x03                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyE2
  {0x04                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyE3
  {0x05                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyE4
  {0x06                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyE5
  {0x07                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyE6
  {0x08                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyE7
  {0x09                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyE8
  {0x0A                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyE9
  {0x0B                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyE10
  {0x0C                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyE11
  {0x0D                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyE12
  {0x0E                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyBackSpace
  {0x52                     , ATTR_EXTEND_0_TRUE      | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyIns
  {0x47                     , ATTR_EXTEND_0_TRUE      | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyHome
  {0x49                     , ATTR_EXTEND_0_TRUE      | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyPgUp
  {SCANCODE_NUM_LOCK_MAKE   , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_FALSE    },  //  EfiKeyNLck
  {0x35                     , ATTR_EXTEND_0_TRUE      | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeySlash
  {SCANCODE_SYS_REQ_MAKE    , ATTR_EXTEND_0_FALSE     | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyAsterisk
  {0x4a                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyMinus
  {0x01                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyEsc
  {0x3B                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyF1
  {0x3C                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyF2
  {0x3D                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyF3
  {0x3E                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyF4
  {0x3F                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyF5
  {0x40                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyF6
  {0x41                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyF7
  {0x42                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyF8
  {0x43                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyF9
  {0x44                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyF10
  {0x57                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyF11
  {0x58                     , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyF12
  {SCANCODE_LEFT_SHIFT_MAKE , ATTR_EXTEND_0_TRUE      | ATTR_EXTEND_1_DONT_CARE},  //  EfiKeyPrint (multiple scan codes)
  {SCANCODE_SCROLL_LOCK_MAKE, ATTR_EXTEND_0_FALSE     | ATTR_EXTEND_1_FALSE    },  //  EfiKeySLck
  {SCANCODE_NUM_LOCK_MAKE   , ATTR_EXTEND_0_DONT_CARE | ATTR_EXTEND_1_TRUE     },  //  EfiKeyPause (multiple scan codes)
};

//
// Keyboard modifier value to EFI Scan Code convertion table
// EFI Scan Code and the modifier values are defined in UEFI spec.
//
UINT8 mModifierValueToEfiScanCodeConvertionTable[] = {
  SCAN_NULL,       // EFI_NULL_MODIFIER
  SCAN_NULL,       // EFI_LEFT_CONTROL_MODIFIER
  SCAN_NULL,       // EFI_RIGHT_CONTROL_MODIFIER
  SCAN_NULL,       // EFI_LEFT_ALT_MODIFIER
  SCAN_NULL,       // EFI_RIGHT_ALT_MODIFIER
  SCAN_NULL,       // EFI_ALT_GR_MODIFIER
  SCAN_INSERT,     // EFI_INSERT_MODIFIER
  SCAN_DELETE,     // EFI_DELETE_MODIFIER
  SCAN_PAGE_DOWN,  // EFI_PAGE_DOWN_MODIFIER
  SCAN_PAGE_UP,    // EFI_PAGE_UP_MODIFIER
  SCAN_HOME,       // EFI_HOME_MODIFIER
  SCAN_END,        // EFI_END_MODIFIER
  SCAN_NULL,       // EFI_LEFT_SHIFT_MODIFIER
  SCAN_NULL,       // EFI_RIGHT_SHIFT_MODIFIER
  SCAN_NULL,       // EFI_CAPS_LOCK_MODIFIER
  SCAN_NULL,       // EFI_NUM_LOCK_MODIFIER
  SCAN_LEFT,       // EFI_LEFT_ARROW_MODIFIER
  SCAN_RIGHT,      // EFI_RIGHT_ARROW_MODIFIER
  SCAN_DOWN,       // EFI_DOWN_ARROW_MODIFIER
  SCAN_UP,         // EFI_UP_ARROW_MODIFIER
  SCAN_NULL,       // EFI_NS_KEY_MODIFIER
  SCAN_NULL,       // EFI_NS_KEY_DEPENDENCY_MODIFIER
  SCAN_F1,         // EFI_FUNCTION_KEY_ONE_MODIFIER
  SCAN_F2,         // EFI_FUNCTION_KEY_TWO_MODIFIER
  SCAN_F3,         // EFI_FUNCTION_KEY_THREE_MODIFIER
  SCAN_F4,         // EFI_FUNCTION_KEY_FOUR_MODIFIER
  SCAN_F5,         // EFI_FUNCTION_KEY_FIVE_MODIFIER
  SCAN_F6,         // EFI_FUNCTION_KEY_SIX_MODIFIER
  SCAN_F7,         // EFI_FUNCTION_KEY_SEVEN_MODIFIER
  SCAN_F8,         // EFI_FUNCTION_KEY_EIGHT_MODIFIER
  SCAN_F9,         // EFI_FUNCTION_KEY_NINE_MODIFIER
  SCAN_F10,        // EFI_FUNCTION_KEY_TEN_MODIFIER
  SCAN_F11,        // EFI_FUNCTION_KEY_ELEVEN_MODIFIER
  SCAN_F12,        // EFI_FUNCTION_KEY_TWELVE_MODIFIER
  //
  // For Partial Keystroke support
  //
  SCAN_NULL,       // EFI_PRINT_MODIFIER
  SCAN_NULL,       // EFI_SYS_REQUEST_MODIFIER
  SCAN_NULL,       // EFI_SCROLL_LOCK_MODIFIER
  SCAN_PAUSE,      // EFI_PAUSE_MODIFIER
  SCAN_NULL,       // EFI_BREAK_MODIFIER
  SCAN_NULL,       // EFI_LEFT_LOGO_MODIFIER
  SCAN_NULL,       // EFI_RIGHT_LOGO_MODIFER
  SCAN_NULL,       // EFI_MENU_MODIFER
};

STATIC
UINT8 CtrlIgnoreKey[] = {
    0x02,           // '1','!'
    0x04,           // '3','#'
    0x05,           // '4','$'
    0x06,           // '5','%'
    0x08,           // '7','&'
    0x09,           // '8','*'
    0x0A,           // '9','('
    0x0B,           // '0',')'
    0x0D,           // '=','+'
    0x27,           // ';',':'
    0x2B,           // '\'','"'
    0x29,           // '`','~'
    0x33,           // ',','<'
    0x34,           // '.','>'
    0x35            // '/','?'
};
STATIC
UINT8 AltIgnoreKey[] = {
    0xD3,           // 'Delete Forward'
    0xC8,           // 'Up Arrow'
    0xD0,           // 'Down Arrow'
    0xCB,           // 'Left Arrow'
    0xCD,           // 'Right Arrow'
    0x52            // 'keypad 0 Insert'
};

//
// The WaitForValue time out
//
UINTN  mWaitForValueTimeOut = KEYBOARD_WAITFORVALUE_TIMEOUT;

BOOLEAN          mEnableMouseInterface;



/**
  Return the count of scancode in the queue.

  @param Queue     Pointer to instance of SCAN_CODE_QUEUE.

  @return          Count of the scancode.
**/
UINTN
GetScancodeBufCount (
  IN SCAN_CODE_QUEUE       *Queue
  )
{
  if (Queue->Head <= Queue->Tail) {
    return Queue->Tail - Queue->Head;
  } else {
    return Queue->Tail + KEYBOARD_SCAN_CODE_MAX_COUNT - Queue->Head;
  }
}

/**
  Read several bytes from the scancode buffer without removing them.
  This function is called to see if there are enough bytes of scancode
  representing a single key.

  @param Queue     Pointer to instance of SCAN_CODE_QUEUE.
  @param Count     Number of bytes to be read
  @param Buf       Store the results

  @retval EFI_SUCCESS   success to scan the keyboard code
  @retval EFI_NOT_READY invalid parameter
**/
EFI_STATUS
GetScancodeBufHead (
  IN  SCAN_CODE_QUEUE        *Queue,
  IN  UINTN                  Count,
  OUT UINT8                  *Buf
  )
{
  UINTN                      Index;
  UINTN                      Pos;

  //
  // check the valid range of parameter 'Count'
  //
  if (GetScancodeBufCount (Queue) < Count) {
    return EFI_NOT_READY;
  }
  //
  // retrieve the values
  //
  for (Index = 0, Pos = Queue->Head; Index < Count; Index++, Pos = (Pos + 1) % KEYBOARD_SCAN_CODE_MAX_COUNT) {
    Buf[Index] = Queue->Buffer[Pos];
  }

  return EFI_SUCCESS;
}

/**

  Read & remove several bytes from the scancode buffer.
  This function is usually called after GetScancodeBufHead()

  @param Queue     Pointer to instance of SCAN_CODE_QUEUE.
  @param Count     Number of bytes to be read
  @param Buf       Store the results

  @retval EFI_SUCCESS success to scan the keyboard code
  @retval EFI_NOT_READY invalid parameter
**/
EFI_STATUS
PopScancodeBufHead (
  IN  SCAN_CODE_QUEUE       *Queue,
  IN  UINTN                 Count,
  OUT UINT8                 *Buf OPTIONAL
  )
{
  UINTN                     Index;

  //
  // Check the valid range of parameter 'Count'
  //
  if (GetScancodeBufCount (Queue) < Count) {
    return EFI_NOT_READY;
  }
  //
  // Retrieve and remove the values
  //
  for (Index = 0; Index < Count; Index++, Queue->Head = (Queue->Head + 1) % KEYBOARD_SCAN_CODE_MAX_COUNT) {
    if (Buf != NULL) {
      Buf[Index] = Queue->Buffer[Queue->Head];
    }
  }

  return EFI_SUCCESS;
}

/**
  Push one byte to the scancode buffer.

  @param Queue     Pointer to instance of SCAN_CODE_QUEUE.
  @param Scancode  The byte to push.
**/
VOID
PushScancodeBufTail (
  IN  SCAN_CODE_QUEUE       *Queue,
  IN  UINT8                 Scancode
  )
{
  if (GetScancodeBufCount (Queue) == KEYBOARD_SCAN_CODE_MAX_COUNT - 1) {
    PopScancodeBufHead (Queue, 1, NULL);
  }

  Queue->Buffer[Queue->Tail] = Scancode;
  Queue->Tail = (Queue->Tail + 1) % KEYBOARD_SCAN_CODE_MAX_COUNT;
}

/**
  Read data register .

  @param ConsoleIn Pointer to instance of KEYBOARD_CONSOLE_IN_DEV

  @return return the value

**/
UINT8
KeyReadDataRegister (
  IN KEYBOARD_CONSOLE_IN_DEV *ConsoleIn
  )

{
  EFI_ISA_IO_PROTOCOL                 *IsaIo;
  UINT8                               Data;

  //
  // Use IsaIo protocol to perform IO operations
  //
  IsaIo = ConsoleIn->IsaIo;

  IsaIo->Io.Read (
              IsaIo,
              EfiIsaIoWidthUint8,
              ConsoleIn->DataRegisterAddress,
              1,
              &Data
              );

  return Data;
}

/**
  Write data register.

  @param ConsoleIn Pointer to instance of KEYBOARD_CONSOLE_IN_DEV
  @param Data      value wanted to be written

**/
VOID
KeyWriteDataRegister (
  IN KEYBOARD_CONSOLE_IN_DEV *ConsoleIn,
  IN UINT8                   Data
  )
{
  ConsoleIn->IsaIo->Io.Write (
                         ConsoleIn->IsaIo,
                         EfiIsaIoWidthUint8,
                         ConsoleIn->DataRegisterAddress,
                         1,
                         &Data
                         );
}

/**
  Read status register.

  @param ConsoleIn  Pointer to instance of KEYBOARD_CONSOLE_IN_DEV

  @return value in status register

**/
UINT8
KeyReadStatusRegister (
  IN KEYBOARD_CONSOLE_IN_DEV *ConsoleIn
  )
{
  UINT8                               Data;
  ConsoleIn->IsaIo->Io.Read (
                         ConsoleIn->IsaIo,
                         EfiIsaIoWidthUint8,
                         ConsoleIn->StatusRegisterAddress,
                         1,
                         &Data
                         );
  return Data;
}

/**
  Write command register .

  @param ConsoleIn Pointer to instance of KEYBOARD_CONSOLE_IN_DEV
  @param Data      The value wanted to be written

**/
VOID
KeyWriteCommandRegister (
  IN KEYBOARD_CONSOLE_IN_DEV *ConsoleIn,
  IN UINT8                   Data
  )
{
  ConsoleIn->IsaIo->Io.Write (
                         ConsoleIn->IsaIo,
                         EfiIsaIoWidthUint8,
                         ConsoleIn->CommandRegisterAddress,
                         1,
                         &Data
                         );
}

/**
  Display error message.

  @param ConsoleIn Pointer to instance of KEYBOARD_CONSOLE_IN_DEV
  @param ErrMsg    Unicode string of error message

**/
VOID
KeyboardError (
  IN KEYBOARD_CONSOLE_IN_DEV *ConsoleIn,
  IN CHAR16                  *ErrMsg
  )
{
  ConsoleIn->KeyboardErr = TRUE;
}

/**
  Timer event handler: read a series of scancodes from 8042
  and put them into memory scancode buffer.
  it read as much scancodes to either fill
  the memory buffer or empty the keyboard buffer.
  It is registered as running under TPL_NOTIFY

  @param Event       The timer event
  @param Context     A KEYBOARD_CONSOLE_IN_DEV pointer

**/
VOID
EFIAPI
KeyboardTimerHandler (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )

{
  UINT8                   Data;
  EFI_TPL                 OldTpl;
  KEYBOARD_CONSOLE_IN_DEV *ConsoleIn;

  ConsoleIn = (KEYBOARD_CONSOLE_IN_DEV *) Context;

  //
  // Enter critical section
  //
  OldTpl = gBS->RaiseTPL (TPL_NOTIFY);

  if (((KEYBOARD_CONSOLE_IN_DEV *) Context)->KeyboardErr) {
    //
    // Leave critical section and return
    //
    gBS->RestoreTPL (OldTpl);
    return ;
  }

  //
  // To let KB driver support Hot plug, here should skip the 'resend' command  for the case that
  // KB is not connected to system. If KB is not connected to system, driver will find there's something
  // error in the following code and wait for the input buffer empty, this waiting time shoulb be short enough since
  // this is a NOTIFY TPL period function, or the system performance will degrade hardly when KB is not connected.
  // Just skip the 'resend' process simply.
  //

  while ((KeyReadStatusRegister (ConsoleIn) & (KEYBOARD_STATUS_REGISTER_TRANSMIT_TIMEOUT|KEYBOARD_STATUS_REGISTER_HAS_OUTPUT_DATA)) ==
      KEYBOARD_STATUS_REGISTER_HAS_OUTPUT_DATA
     ) {
    //
    // Stall 10 microsecond between the reading of KBC status and data port to make KBC has
    // enough time to prepare correct scan code in the data port
    //
    gBS->Stall (10);
    //
    // Read one byte of the scan code and store it into the memory buffer
    //
    Data = KeyReadDataRegister (ConsoleIn);
    PushScancodeBufTail (&ConsoleIn->ScancodeQueue, Data);
  }
  KeyGetchar (ConsoleIn);

  //
  // Leave critical section and return
  //
  gBS->RestoreTPL (OldTpl);
}

/**
  Read key value .

  @param ConsoleIn - Pointer to instance of KEYBOARD_CONSOLE_IN_DEV
  @param Data      - Pointer to outof buffer for keeping key value

  @retval EFI_TIMEOUT Status resigter time out
  @retval EFI_SUCCESS Success to read keyboard

**/
EFI_STATUS
KeyboardRead (
  IN KEYBOARD_CONSOLE_IN_DEV  *ConsoleIn,
  OUT UINT8                   *Data
  )

{
  UINT32  TimeOut;
  UINT32  RegFilled;

  TimeOut   = 0;
  RegFilled = 0;

  //
  // wait till output buffer full then perform the read
  //
  for (TimeOut = 0; TimeOut < KEYBOARD_TIMEOUT; TimeOut += 30) {
    if (KeyReadStatusRegister (ConsoleIn) & KEYBOARD_STATUS_REGISTER_HAS_OUTPUT_DATA) {
      RegFilled = 1;
      *Data     = KeyReadDataRegister (ConsoleIn);
      break;
    }

    MicroSecondDelay (30);
  }

  if (RegFilled == 0) {
    return EFI_TIMEOUT;
  }

  return EFI_SUCCESS;
}

/**
  write key to keyboard

  @param ConsoleIn Pointer to instance of KEYBOARD_CONSOLE_IN_DEV
  @param Data      value wanted to be written

  @retval EFI_TIMEOUT   The input buffer register is full for putting new value util timeout
  @retval EFI_SUCCESS   The new value is sucess put into input buffer register.

**/
EFI_STATUS
KeyboardWrite (
  IN KEYBOARD_CONSOLE_IN_DEV *ConsoleIn,
  IN UINT8                   Data
  )
{
  UINT32  TimeOut;
  UINT32  RegEmptied;

  TimeOut     = 0;
  RegEmptied  = 0;

  //
  // wait for input buffer empty
  //
  for (TimeOut = 0; TimeOut < KEYBOARD_TIMEOUT; TimeOut += 30) {
    if ((KeyReadStatusRegister (ConsoleIn) & 0x02) == 0) {
      RegEmptied = 1;
      break;
    }

    MicroSecondDelay (30);
  }

  if (RegEmptied == 0) {
    return EFI_TIMEOUT;
  }
  //
  // Write it
  //
  KeyWriteDataRegister (ConsoleIn, Data);

  return EFI_SUCCESS;
}

/**
  Issue keyboard command.

  @param ConsoleIn Pointer to instance of KEYBOARD_CONSOLE_IN_DEV
  @param Data      The buff holding the command

  @retval EFI_TIMEOUT Keyboard is not ready to issuing
  @retval EFI_SUCCESS Success to issue keyboard command

**/
EFI_STATUS
KeyboardCommand (
  IN KEYBOARD_CONSOLE_IN_DEV *ConsoleIn,
  IN UINT8                   Data
  )
{
  UINT32  TimeOut;
  UINT32  RegEmptied;

  TimeOut     = 0;
  RegEmptied  = 0;

  //
  // Wait For Input Buffer Empty
  //
  for (TimeOut = 0; TimeOut < KEYBOARD_TIMEOUT; TimeOut += 30) {
    if ((KeyReadStatusRegister (ConsoleIn) & 0x02) == 0) {
      RegEmptied = 1;
      break;
    }

    MicroSecondDelay (30);
  }

  if (RegEmptied == 0) {
    return EFI_TIMEOUT;
  }
  //
  // issue the command
  //
  KeyWriteCommandRegister (ConsoleIn, Data);

  //
  // Wait For Input Buffer Empty again
  //
  RegEmptied = 0;
  for (TimeOut = 0; TimeOut < KEYBOARD_TIMEOUT; TimeOut += 30) {
    if ((KeyReadStatusRegister (ConsoleIn) & 0x02) == 0) {
      RegEmptied = 1;
      break;
    }

    MicroSecondDelay (30);
  }

  if (RegEmptied == 0) {
    return EFI_TIMEOUT;
  }

  return EFI_SUCCESS;
}

/**
  wait for a specific value to be presented on
  8042 Data register by keyboard and then read it,
  used in keyboard commands ack

  @param ConsoleIn Pointer to instance of KEYBOARD_CONSOLE_IN_DEV
  @param Value     the value wanted to be waited.

  @retval EFI_TIMEOUT Fail to get specific value in given time
  @retval EFI_SUCCESS Success to get specific value in given time.

**/
EFI_STATUS
KeyboardWaitForValue (
  IN KEYBOARD_CONSOLE_IN_DEV *ConsoleIn,
  IN UINT8                   Value
  )
{
  UINT8   Data;
  UINT32  TimeOut;
  UINT32  SumTimeOut;
  UINT32  GotIt;

  GotIt       = 0;
  TimeOut     = 0;
  SumTimeOut  = 0;

  //
  // Make sure the initial value of 'Data' is different from 'Value'
  //
  Data = 0;
  if (Data == Value) {
    Data = 1;
  }
  //
  // Read from 8042 (multiple times if needed)
  // until the expected value appears
  // use SumTimeOut to control the iteration
  //
  while (1) {
    //
    // Perform a read
    //
    for (TimeOut = 0; TimeOut < KEYBOARD_TIMEOUT; TimeOut += 30) {
      if (KeyReadStatusRegister (ConsoleIn) & 0x01) {
        Data = KeyReadDataRegister (ConsoleIn);
        break;
      }

      MicroSecondDelay (30);
    }

    SumTimeOut += TimeOut;

    if (Data == Value) {
      GotIt = 1;
      break;
    }

    if (SumTimeOut >= mWaitForValueTimeOut) {
      break;
    }
  }
  //
  // Check results
  //
  if (GotIt == 1) {
    return EFI_SUCCESS;
  } else {
    return EFI_TIMEOUT;
  }

}

/**
  wait for a specific value to be presented on
  8042 Data register by keyboard and then read it,
  used in keyboard commands ack or failed ack

  @param[in]  ConsoleIn   The KEYBOARD_CONSOLE_IN_DEV instance pointer
  @param[in]  Values      The any of the values to be waited for.
  @param[in]  NumOfValues Number of values to be waited for.
  @param[out] Value       Output buffer to save the value which receive from data port and the value
                          is one of the "Values".

  @retval EFI_SUCCESS     Success to get any of specific values in given time.
  @retval EFI_TIMEOUT     Fail to get any of specific values in given time.
**/
STATIC
EFI_STATUS
KeyboardWaitForValues (
  IN  KEYBOARD_CONSOLE_IN_DEV *ConsoleIn,
  IN  UINT8                   *Values,
  IN  UINTN                   NumOfValues,
  OUT UINT8                   *Value
  )
{
  UINT8   Data;
  UINT32  TimeOut;
  UINT32  SumTimeOut;
  BOOLEAN ReceiveData;
  UINTN   Index;

  SumTimeOut  = 0;
  Data        = 0;
  ReceiveData = FALSE;
  //
  // Read from 8042 (multiple times if needed)
  // until the expected value appears
  // use SumTimeOut to control the iteration
  //
  while (TRUE) {
    //
    // Perform a read
    //
    for (TimeOut = 0; TimeOut < KEYBOARD_TIMEOUT; TimeOut += 30) {
      if (KeyReadStatusRegister (ConsoleIn) & 0x01) {
        Data = KeyReadDataRegister (ConsoleIn);
        ReceiveData = TRUE;
        break;
      }

      gBS->Stall (30);
    }

    SumTimeOut += TimeOut;

    if (ReceiveData) {
      for (Index = 0; Index < NumOfValues; Index++) {
        if(Values[Index] == Data) {
          *Value = Data;
          return EFI_SUCCESS;
        }
      }
      ReceiveData = FALSE;
    }

    if (SumTimeOut >= mWaitForValueTimeOut) {
      break;
    }
  }

  return EFI_TIMEOUT;
}

/**
  Show keyboard status lights according to
  indicators in ConsoleIn.

  @param ConsoleIn Pointer to instance of KEYBOARD_CONSOLE_IN_DEV

  @return status of updating keyboard register

**/
EFI_STATUS
UpdateStatusLights (
  IN KEYBOARD_CONSOLE_IN_DEV *ConsoleIn
  )
{
  EFI_STATUS  Status;
  UINT8       Command;
  UINT8       Values[2];
  UINT8       Value;

  //
  // Send keyboard command
  //
  Status = KeyboardWrite (ConsoleIn, 0xed);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // If system doesn't plug-in keyboard to PS/2 connector, some KBC will return KBC_CMDECHO_FAILED_ACK
  //
  Value = 0;
  Values[0] = KEYBOARD_8048_RETURN_8042_ACK;
  Values[1] = KEYBOARD_8048_RETURN_8042_FAILED_ACK;
  Status = KeyboardWaitForValues (ConsoleIn, Values, sizeof (Values), &Value);
  if (EFI_ERROR (Status) || Value == KEYBOARD_8048_RETURN_8042_FAILED_ACK) {
    return EFI_SUCCESS;
  }

  //
  // Light configuration
  //
  Command = 0;
  if (ConsoleIn->CapsLock) {
    Command |= 4;
  }

  if (ConsoleIn->NumLock) {
    Command |= 2;
  }

  if (ConsoleIn->ScrollLock) {
    Command |= 1;
  }

  Status = KeyboardWrite (ConsoleIn, Command);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = KeyboardWaitForValue (ConsoleIn, 0xfa);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "KeyboardWaitForValue() failed - %r\n", Status));
  }

  return EFI_SUCCESS;
}

/**
 Check if the scan code is the break code of EFI_KEY_STATE.KeyShiftState.

 @param[in] Extend1             Indicate if there is a prefix scan code E1
 @param[in] ScanCode            PS2 scan code

 @retval TRUE                   The scan code is the break code of EFI_KEY_STATE.KeyShiftState.
 @retval FALSE                  The scan code is not the break code of EFI_KEY_STATE.KeyShiftState.
**/
STATIC
BOOLEAN
IsBreakCodeOfEfiKeyShiftState (
  IN BOOLEAN                     Extend1,
  IN UINT16                      ScanCode
  )
{
  if (!Extend1 &&
      (ScanCode == SCANCODE_CTRL_BREAK             ||
       ScanCode == SCANCODE_ALT_BREAK              ||
       ScanCode == SCANCODE_LEFT_SHIFT_BREAK       ||
       ScanCode == SCANCODE_RIGHT_SHIFT_BREAK      ||
       ScanCode == SCANCODE_LEFT_LOGO_BREAK        ||
       ScanCode == SCANCODE_RIGHT_LOGO_BREAK       ||
       ScanCode == SCANCODE_MENU_BREAK             ||
       ScanCode == SCANCODE_SYS_REQ_BREAK_WITH_ALT ||
       ScanCode == SCANCODE_SYS_REQ_BREAK)) {
    return TRUE;
  }

  return FALSE;
}

/**
 Convert PS2 scan code to EFI_KEY value.

 @param[in] Extend0              Indicate if there is a prefix scan code E0
 @param[in] Extend1              Indicate if there is a prefix scan code E1
 @param[in] ScanCode             PS2 scan code

 @return The EFI_KEY value or oxFF if not found.
**/
STATIC
EFI_KEY
ConvertPs2ScanCodeToEfiKey (
  IN BOOLEAN                     Extend0,
  IN BOOLEAN                     Extend1,
  IN UINT16                      ScanCode
  )
{
  UINTN                          Index;

  if (Extend0 && ScanCode == SCANCODE_SCROLL_LOCK_MAKE) {
    //
    // PAUSE shares the same scancode as that of SCROLL except PAUSE (CTRL pressed) has E0 prefix
    //
    return EfiKeyPause;
  } else if ((!Extend1 && Extend0 && ScanCode == SCANCODE_SYS_REQ_MAKE) ||
             (!Extend1 && ScanCode == SCANCODE_SYS_REQ_MAKE_WITH_ALT)) {
    //
    // PRNT_SCRN shares the same scancode as that of Key Pad "*" except PRNT_SCRN has E0 prefix
    //
    return EfiKeyPrint;
  } else {
    //
    // Except the above special case, all others can be handled by convert table
    //
    for (Index = 0; Index < ARRAY_SIZE(mEfiKeyToPs2KeyCodeConvertTable); Index++) {
      if ((ScanCode == mEfiKeyToPs2KeyCodeConvertTable[Index].ScanCode) &&
          ((Extend0  && (mEfiKeyToPs2KeyCodeConvertTable[Index].PrefixScanCodeAttr & ATTR_EXTEND_0_TRUE ) != 0) ||
           (!Extend0 && (mEfiKeyToPs2KeyCodeConvertTable[Index].PrefixScanCodeAttr & ATTR_EXTEND_0_FALSE) != 0)) &&
          ((Extend1  && (mEfiKeyToPs2KeyCodeConvertTable[Index].PrefixScanCodeAttr & ATTR_EXTEND_1_TRUE ) != 0) ||
           (!Extend1 && (mEfiKeyToPs2KeyCodeConvertTable[Index].PrefixScanCodeAttr & ATTR_EXTEND_1_FALSE) != 0))) {
        return (EFI_KEY) Index;
      }
    }
  }

  return 0xFF;
}

/**
 Convert EFI_KEY value to corresponding key descriptor from input keyboard layout.

 @param[in] KbLayout            Pointer to keyboard layout
 @param[in] EfiKey              The EFI_KEY value.

 @return The pointer of key descriptor or NULL if not found.
**/
EFI_KEY_DESCRIPTOR *
ConvertEfiKeyToKeyDescriptor (
  IN EFI_HII_KEYBOARD_LAYOUT     *KbLayout,
  IN EFI_KEY                     EfiKey
  )
{
  EFI_KEY_DESCRIPTOR             *KeyDescriptor;
  UINTN                          Index;

  KeyDescriptor = (EFI_KEY_DESCRIPTOR *) (((UINT8 *) KbLayout) + sizeof (EFI_HII_KEYBOARD_LAYOUT));
  for (Index = 0; Index < KbLayout->DescriptorCount; Index++) {
    if (KeyDescriptor[Index].Key == EfiKey) {
      return &KeyDescriptor[Index];
    }

    //
    // For non-spacing key, skip following physical keys.
    //
    if (KeyDescriptor[Index].Modifier == EFI_NS_KEY_MODIFIER) {
      while (((Index + 1) < KbLayout->DescriptorCount) &&
             (KeyDescriptor[Index + 1].Modifier == EFI_NS_KEY_DEPENDENCY_MODIFIER)) {
        Index++;
      }
    }
  }

  return NULL;
}

/**
 Get corresponding physical key descriptor by non-spacing and current key descriptors.

 @param[in] KbLayout             Pointer to keyboard layout
 @param[in] CurrentNsKey         Pointer to non-spacing key descriptor
 @param[in] CurrentKeyDescriptor Pointer to current key descriptor

 @return The pointer of corresponding physical key descriptor or original key descriptor (i.e. CurrentKeyDescriptor) if not found.
**/
EFI_KEY_DESCRIPTOR *
Ps2GetNsPhysicalKey (
  IN EFI_HII_KEYBOARD_LAYOUT     *KbLayout,
  IN EFI_KEY_DESCRIPTOR          *CurrentNsKey,
  IN EFI_KEY_DESCRIPTOR          *CurrentKeyDescriptor
  )
{
  EFI_KEY_DESCRIPTOR             *KeyDescriptor;
  UINTN                          Index;

  KeyDescriptor = (EFI_KEY_DESCRIPTOR *) (((UINT8 *) KbLayout) + sizeof (EFI_HII_KEYBOARD_LAYOUT));
  for (Index = 0; Index < KbLayout->DescriptorCount; Index++, KeyDescriptor++) {
    if (KeyDescriptor != CurrentNsKey) {
      continue;
    }

    //
    // Find non-spacing key and then find the corresponding physical keys.
    //
    ASSERT (KeyDescriptor->Modifier == EFI_NS_KEY_MODIFIER);
    for (KeyDescriptor++, Index++; Index < KbLayout->DescriptorCount; KeyDescriptor++, Index++) {
      if (KeyDescriptor->Modifier != EFI_NS_KEY_DEPENDENCY_MODIFIER) {
        break;
      }

      if (KeyDescriptor->Key == CurrentKeyDescriptor->Key) {
        return KeyDescriptor;
      }
    }
    break;
  }

  //
  // No children definition matched, return original key
  //
  return CurrentKeyDescriptor;
}

/**
 Get key descriptor by PS2 scan code.

 @param[in] KbLayout             Pointer to keyboard layout
 @param[in] Extend0              Indicate if there is a prefix scan code E0
 @param[in] Extend1              Indicate if there is a prefix scan code E1
 @param[in] ScanCode             PS2 scan code

 @return The pointer of key descriptor or NULL if not found.
**/
EFI_KEY_DESCRIPTOR *
Ps2GetKeyDescriptor (
  IN EFI_HII_KEYBOARD_LAYOUT     *KbLayout,
  IN BOOLEAN                     Extend0,
  IN BOOLEAN                     Extend1,
  IN UINT16                      ScanCode
  )
{
  EFI_KEY                        EfiKey;
  EFI_KEY_DESCRIPTOR             *KeyDescriptor;

  if (KbLayout == NULL) {
    return NULL;
  }

  EfiKey = ConvertPs2ScanCodeToEfiKey (Extend0, Extend1, ScanCode);
  if (EfiKey == 0xFF) {
    return NULL;
  }

  KeyDescriptor = ConvertEfiKeyToKeyDescriptor (KbLayout, EfiKey);
  if (KeyDescriptor == NULL && EfiKey == EfiKeyC12) {
    //
    // Because it can not distinguish EfiKeyC12 and EfiKeyD13 (which PS2 scan codes are both 0x2b),
    // try to get the key descriptior of EfiKeyD13 if key descriptior of EfiKeyC12 is not found.
    //
    KeyDescriptor = ConvertEfiKeyToKeyDescriptor (KbLayout, EfiKeyD13);
  }

  return KeyDescriptor;
}

/**
 The notification function for EFI_HII_SET_KEYBOARD_LAYOUT_EVENT_GUID.

 This function is registered to event of EFI_HII_SET_KEYBOARD_LAYOUT_EVENT_GUID
 group type, which will be triggered by EFI_HII_DATABASE_PROTOCOL.SetKeyboardLayout().
 It tries to get curent keyboard layout from HII database.

 @param[in]  Event        Event being signaled.
 @param[in]  Context      Points to KEYBOARD_CONSOLE_IN_DEV instance.
**/
VOID
EFIAPI
Ps2SetKeyboardLayoutEvent (
  IN EFI_EVENT                   Event,
  IN VOID                        *Context
  )
{
  KEYBOARD_CONSOLE_IN_DEV        *ConsoleIn;
  EFI_HII_KEYBOARD_LAYOUT        *KeyboardLayout;

  ConsoleIn = (KEYBOARD_CONSOLE_IN_DEV *) Context;
  if (ConsoleIn->Signature != KEYBOARD_CONSOLE_IN_DEV_SIGNATURE) {
    return;
  }

  KeyboardLayout = HiiGetCurrentKeyboardLayout ();
  if (KeyboardLayout == NULL) {
    return;
  }

  if (ConsoleIn->CurrentKbLayout != NULL) {
    FreePool (ConsoleIn->CurrentKbLayout);
  }

  ConsoleIn->CurrentKbLayout = KeyboardLayout;
  ConsoleIn->CurrentNsKey    = NULL;
}

/**
  Initialize the key state.

  @param  ConsoleIn     The KEYBOARD_CONSOLE_IN_DEV instance.
  @param  KeyState      A pointer to receive the key state information.
**/
VOID
InitializeKeyState (
  IN  KEYBOARD_CONSOLE_IN_DEV *ConsoleIn,
  OUT EFI_KEY_STATE           *KeyState
  )
{
  KeyState->KeyShiftState  = EFI_SHIFT_STATE_VALID
                           | (ConsoleIn->LeftCtrl   ? EFI_LEFT_CONTROL_PRESSED  : 0)
                           | (ConsoleIn->RightCtrl  ? EFI_RIGHT_CONTROL_PRESSED : 0)
                           | (ConsoleIn->LeftAlt    ? EFI_LEFT_ALT_PRESSED      : 0)
                           | (ConsoleIn->RightAlt   ? EFI_RIGHT_ALT_PRESSED     : 0)
                           | (ConsoleIn->LeftShift  ? EFI_LEFT_SHIFT_PRESSED    : 0)
                           | (ConsoleIn->RightShift ? EFI_RIGHT_SHIFT_PRESSED   : 0)
                           | (ConsoleIn->LeftLogo   ? EFI_LEFT_LOGO_PRESSED     : 0)
                           | (ConsoleIn->RightLogo  ? EFI_RIGHT_LOGO_PRESSED    : 0)
                           | (ConsoleIn->Menu       ? EFI_MENU_KEY_PRESSED      : 0)
                           | (ConsoleIn->SysReq     ? EFI_SYS_REQ_PRESSED       : 0)
                           ;
  KeyState->KeyToggleState = EFI_TOGGLE_STATE_VALID
                           | (ConsoleIn->CapsLock   ? EFI_CAPS_LOCK_ACTIVE :   0)
                           | (ConsoleIn->NumLock    ? EFI_NUM_LOCK_ACTIVE :    0)
                           | (ConsoleIn->ScrollLock ? EFI_SCROLL_LOCK_ACTIVE : 0)
                           | (ConsoleIn->IsSupportPartialKey ? EFI_KEY_STATE_EXPOSED : 0)
                           ;
}

/**
  Get scancode from scancode buffer and translate into EFI-scancode and unicode defined by EFI spec.

  The function is always called in TPL_NOTIFY.

  @param ConsoleIn KEYBOARD_CONSOLE_IN_DEV instance pointer

**/
VOID
KeyGetchar (
  IN OUT KEYBOARD_CONSOLE_IN_DEV *ConsoleIn
  )
{
  EFI_STATUS                     Status;
  UINT16                         ScanCode;
  BOOLEAN                        Extend0;
  BOOLEAN                        Extend1;
  UINTN                          Index;
  EFI_KEY_DATA                   KeyData;
  LIST_ENTRY                     *Link;
  KEYBOARD_CONSOLE_IN_EX_NOTIFY  *CurrentNotify;
  //
  // 3 bytes most
  //
  UINT8                          ScancodeArr[3];
  UINT32                         ScancodeArrPos;
  STATIC BOOLEAN                 ClearUnexpectedLShift = FALSE;
  EFI_KEY_DESCRIPTOR             *KeyDescriptor;

  //
  // Check if there are enough bytes of scancode representing a single key
  // available in the buffer
  //
  while (TRUE) {
    Extend0        = FALSE;
    Extend1        = FALSE;
    ScancodeArrPos = 0;
    KeyDescriptor  = NULL;
    Status  = GetScancodeBufHead (&ConsoleIn->ScancodeQueue, ScancodeArrPos + 1, ScancodeArr);
    if (EFI_ERROR (Status)) {
      return ;
    }

    if (ScancodeArr[ScancodeArrPos] == SCANCODE_EXTENDED0) {
      //
      // E0 to look ahead 2 bytes
      //
      Extend0 = TRUE;
      ScancodeArrPos = 1;
      Status         = GetScancodeBufHead (&ConsoleIn->ScancodeQueue, ScancodeArrPos + 1, ScancodeArr);
      if (EFI_ERROR (Status)) {
        return ;
      }
    } else if (ScancodeArr[ScancodeArrPos] == SCANCODE_EXTENDED1) {
      //
      // E1 to look ahead 3 bytes
      //
      Extend1 = TRUE;
      ScancodeArrPos = 2;
      Status         = GetScancodeBufHead (&ConsoleIn->ScancodeQueue, ScancodeArrPos + 1, ScancodeArr);
      if (EFI_ERROR (Status)) {
        return ;
      }
    }
    //
    // if we reach this position, scancodes for a key is in buffer now,pop them
    //
    Status = PopScancodeBufHead (&ConsoleIn->ScancodeQueue, ScancodeArrPos + 1, ScancodeArr);
    ASSERT_EFI_ERROR (Status);

    //
    // store the last available byte, this byte of scancode will be checked
    //
    ScanCode = ScancodeArr[ScancodeArrPos];

    if (!Extend1) {
      switch (ScanCode) {

      case SCANCODE_LEFT_SHIFT_MAKE:
        if (Extend0) {
          //
          // To avoid recognize PRNT_SCRN key as a L_SHIFT key
          // because PRNT_SCRN key generates E0 followed by L_SHIFT scan code.
          // If it the second byte of the PRNT_ScRN skip it.
          //
          continue;
        }

      case SCANCODE_LEFT_SHIFT_BREAK:
        if (Extend0) {
          if (ClearUnexpectedLShift) {
            //
            // If receive scan codes 2A (LShift) E0 4B (left arrow) E0 4B (left arrow break) E0 AA, clear unexpected LShift bit because KBC lost to send E0 key.
            //
            ConsoleIn->LeftShift = FALSE;
          }
        }
        ClearUnexpectedLShift = FALSE;
        break;

      case SCANCODE_INSERT_MAKE:
      case SCANCODE_DELETE_MAKE:
      case SCANCODE_LEFT_ARROW_MAKE:
      case SCANCODE_HOME_MAKE:
      case SCANCODE_END_MAKE:
      case SCANCODE_UP_ARROW_MAKE:
      case SCANCODE_DOWN_ARROW_MAKE:
      case SCANCODE_PAGE_UP_MAKE:
      case SCANCODE_PAGE_DOWN_MAKE:
      case SCANCODE_RIGHT_ARROW_MAKE:
        if (Extend0) {
          if (ConsoleIn->LeftShift) {
            ClearUnexpectedLShift = TRUE;
          }
        }
        break;
      }
    }

    if ((ScanCode >= SCANCODE_MAX_MAKE) && !IsBreakCodeOfEfiKeyShiftState (Extend1, ScanCode)) {
      continue;
    }

    KeyDescriptor = Ps2GetKeyDescriptor (ConsoleIn->CurrentKbLayout, Extend0, Extend1, GET_MAKE_CODE(ScanCode));
    if (KeyDescriptor == NULL) {
      return;
    }

    switch (KeyDescriptor->Modifier) {

    case EFI_LEFT_CONTROL_MODIFIER:
      ConsoleIn->LeftCtrl = IS_MAKE_CODE(ScanCode);
      break;
    case EFI_RIGHT_CONTROL_MODIFIER:
      ConsoleIn->RightCtrl = IS_MAKE_CODE(ScanCode);
      break;
    case EFI_LEFT_SHIFT_MODIFIER:
      ConsoleIn->LeftShift = IS_MAKE_CODE(ScanCode);
      break;
    case EFI_RIGHT_SHIFT_MODIFIER:
      ConsoleIn->RightShift = IS_MAKE_CODE(ScanCode);
      break;
    case EFI_LEFT_ALT_MODIFIER:
      ConsoleIn->LeftAlt = IS_MAKE_CODE(ScanCode);
      break;
    case EFI_RIGHT_ALT_MODIFIER:
      ConsoleIn->RightAlt = IS_MAKE_CODE(ScanCode);
      break;
    case EFI_LEFT_LOGO_MODIFIER:
      ConsoleIn->LeftLogo = IS_MAKE_CODE(ScanCode);
      break;
    case EFI_RIGHT_LOGO_MODIFIER:
      ConsoleIn->RightLogo = IS_MAKE_CODE(ScanCode);
      break;
    case EFI_MENU_MODIFIER:
      ConsoleIn->Menu = IS_MAKE_CODE(ScanCode);
      break;
    case EFI_PRINT_MODIFIER:
    case EFI_SYS_REQUEST_MODIFIER:
      ConsoleIn->SysReq = IS_MAKE_CODE(ScanCode);
      break;
    case EFI_ALT_GR_MODIFIER:
      ConsoleIn->AltGrOn = IS_MAKE_CODE(ScanCode);
      break;
    case EFI_CAPS_LOCK_MODIFIER:
      ConsoleIn->CapsLock = (BOOLEAN)!ConsoleIn->CapsLock;
      Ps2KeyboardCheckLEDHandler (ConsoleIn);
      break;
    case EFI_NUM_LOCK_MODIFIER:
      ConsoleIn->NumLock = (BOOLEAN)!ConsoleIn->NumLock;
      Ps2KeyboardCheckLEDHandler (ConsoleIn);
      break;
    case EFI_SCROLL_LOCK_MODIFIER:
      ConsoleIn->ScrollLock = (BOOLEAN)!ConsoleIn->ScrollLock;
      Ps2KeyboardCheckLEDHandler (ConsoleIn);
      break;
    case EFI_NULL_MODIFIER:
      //
      // Special IME for French Layout keyboard
      //
      if (CompareGuid ((EFI_GUID *) (((UINT8 *) ConsoleIn->CurrentKbLayout) + sizeof (UINT16)), &gH2OUsbKeyboardFrenchLayoutKeyGuid)) {
        if (KeyDescriptor->Key == EfiKeyD11) {
          if (ConsoleIn->LeftShift || ConsoleIn->RightShift) {
            if (ConsoleIn->CircumflexAccentShiftedOn == 0) {
              ConsoleIn->CircumflexAccentShiftedOn = 1;
              continue;
            }
          } else {
            if (ConsoleIn->CircumflexAccentOn == 0) {
              ConsoleIn->CircumflexAccentOn = 1;
              continue;
            }
          }
        }
        if ((KeyDescriptor->Key == EfiKeyE2) && (ConsoleIn->AltGrOn)) {
          if (ConsoleIn->TildeOn == 0) {
            ConsoleIn->TildeOn = 1;
            continue;
          }
        }
        if ((KeyDescriptor->Key == EfiKeyE7) && (ConsoleIn->AltGrOn)) {
          if (ConsoleIn->GraveAccentOn == 0) {
            ConsoleIn->GraveAccentOn = 1;
            continue;
          }
        }
      }
      break;
    }

    //
    // If this is above the valid range, ignore it
    //
    if (ScanCode >= SCANCODE_MAX_MAKE) {
      continue;
    } else {
      break;
    }
  }

  //
  // Handle Ctrl+Alt+Del hotkey
  //
  if ((ConsoleIn->LeftCtrl || ConsoleIn->RightCtrl) &&
      (ConsoleIn->LeftAlt  || ConsoleIn->RightAlt ) &&
      KeyDescriptor->Modifier == EFI_DELETE_MODIFIER
     ) {
    gRT->ResetSystem (EfiResetWarm, EFI_SUCCESS, 0, NULL);
  }

  if (KeyDescriptor->Modifier == EFI_NS_KEY_MODIFIER) {
    //
    // If this is a dead key with EFI_NS_KEY_MODIFIER, then record it and return.
    //
    ConsoleIn->CurrentNsKey = KeyDescriptor;
    return;
  }

  if (ConsoleIn->CurrentNsKey != NULL) {
    //
    // If this keystroke follows a non-spacing key, then find the descriptor for corresponding
    // physical key.
    //
    KeyDescriptor = Ps2GetNsPhysicalKey (ConsoleIn->CurrentKbLayout, ConsoleIn->CurrentNsKey, KeyDescriptor);
    ConsoleIn->CurrentNsKey = NULL;
  }

  //
  // Make sure modifier of Key Descriptor is in the valid range according to UEFI spec.
  //
  if (KeyDescriptor->Modifier >= (sizeof (mModifierValueToEfiScanCodeConvertionTable) / sizeof (UINT8))) {
    return;
  }

  //
  // Save the Shift/Toggle state
  //
  InitializeKeyState (ConsoleIn, &KeyData.KeyState);
  KeyData.Key.ScanCode    = mModifierValueToEfiScanCodeConvertionTable[KeyDescriptor->Modifier];
  KeyData.Key.UnicodeChar = KeyDescriptor->Unicode;

  if ((KeyDescriptor->AffectedAttribute & EFI_AFFECTED_BY_STANDARD_SHIFT)!= 0) {
    if (ConsoleIn->LeftShift || ConsoleIn->RightShift) {
      KeyData.Key.UnicodeChar = KeyDescriptor->ShiftedUnicode;

      //
      // Need not return associated shift state if a class of printable characters that
      // are normally adjusted by shift modifiers. e.g. Shift Key + 'f' key = 'F'
      //
      if ((KeyDescriptor->Unicode != KeyDescriptor->ShiftedUnicode)) {
        KeyData.KeyState.KeyShiftState &= ~(EFI_LEFT_SHIFT_PRESSED | EFI_RIGHT_SHIFT_PRESSED);
      }

      if (ConsoleIn->AltGrOn) {
        KeyData.Key.UnicodeChar = KeyDescriptor->ShiftedAltGrUnicode;
      }
    } else {
      //
      // Shift off
      //
      KeyData.Key.UnicodeChar = KeyDescriptor->Unicode;

      if (ConsoleIn->AltGrOn) {
        KeyData.Key.UnicodeChar = KeyDescriptor->AltGrUnicode;
      }
    }
  }
  //
  // alphabetic key is affected by CapsLock State
  //
  if ((KeyDescriptor->AffectedAttribute & EFI_AFFECTED_BY_CAPS_LOCK) != 0) {
    if (ConsoleIn->CapsLock) {
      if (KeyData.Key.UnicodeChar == KeyDescriptor->Unicode) {
        KeyData.Key.UnicodeChar = KeyDescriptor->ShiftedUnicode;
      } else if (KeyData.Key.UnicodeChar == KeyDescriptor->ShiftedUnicode) {
        KeyData.Key.UnicodeChar = KeyDescriptor->Unicode;
      }
    }
  }

  //
  // Translate Unicode 0x1B (ESC) to EFI Scan Code
  //
  if (KeyData.Key.UnicodeChar == 0x1B && KeyData.Key.ScanCode == SCAN_NULL) {
    KeyData.Key.ScanCode    = SCAN_ESC;
    KeyData.Key.UnicodeChar = CHAR_NULL;
  }

  if (ConsoleIn->LeftAlt || ConsoleIn->RightAlt) {
    for (Index = 0; Index < sizeof (AltIgnoreKey); Index++) {
      if (ScanCode == AltIgnoreKey[Index]) {
        return;
      }
    }

    //
    // Alt + numpad 1~9
    // numpad 1~9 scancode greater than 0x47
    if ((ScanCode >= 0x47) &&
        (KeyData.Key.UnicodeChar >= '1' && KeyData.Key.UnicodeChar <= '9')) {
      KeyData.Key.UnicodeChar = KeyData.Key.UnicodeChar - '1' + 1;
    }

    //
    // Alt + (Up, Down, Left, Right)
    //
    if (KeyData.Key.ScanCode >= SCAN_UP && KeyData.Key.ScanCode <= SCAN_PAGE_DOWN) {
      KeyData.Key.ScanCode = SCAN_NULL;
    }
  }

  if (ConsoleIn->CircumflexAccentShiftedOn) {
    ConsoleIn->CircumflexAccentShiftedOn += 1;
    if (KeyData.Key.UnicodeChar == 'a') {
      KeyData.Key.UnicodeChar = 0xE4;
    } else if (KeyData.Key.UnicodeChar == 'e') {
      KeyData.Key.UnicodeChar = 0xEB;
    } else if (KeyData.Key.UnicodeChar == 'i') {
      KeyData.Key.UnicodeChar = 0xEF;
    } else if (KeyData.Key.UnicodeChar == 'o') {
      KeyData.Key.UnicodeChar = 0xF6;
    } else if (KeyData.Key.UnicodeChar == 'u') {
      KeyData.Key.UnicodeChar = 0xFC;
    } else if (KeyData.Key.UnicodeChar == 'y') {
      KeyData.Key.UnicodeChar = 0xFF;
    } else if (KeyData.Key.UnicodeChar == 'A') {
      KeyData.Key.UnicodeChar = 0xC4;
    } else if (KeyData.Key.UnicodeChar == 'E') {
      KeyData.Key.UnicodeChar = 0xCB;
    } else if (KeyData.Key.UnicodeChar == 'I') {
      KeyData.Key.UnicodeChar = 0xCF;
    } else if (KeyData.Key.UnicodeChar == 'O') {
      KeyData.Key.UnicodeChar = 0xD6;
    } else if (KeyData.Key.UnicodeChar == 'U') {
      KeyData.Key.UnicodeChar = 0xDC;
    } else {
      KeyData.Key.UnicodeChar = 0xA8;
    }
    if (ConsoleIn->CircumflexAccentShiftedOn > 3)
      ConsoleIn->CircumflexAccentShiftedOn = 0;
  }

  if (ConsoleIn->CircumflexAccentOn) {
      ConsoleIn->CircumflexAccentOn += 1;
    if (KeyData.Key.UnicodeChar == 'a') {
      KeyData.Key.UnicodeChar = 0xE2;
    } else if (KeyData.Key.UnicodeChar == 'e') {
      KeyData.Key.UnicodeChar = 0xEA;
    } else if (KeyData.Key.UnicodeChar == 'i') {
      KeyData.Key.UnicodeChar = 0xEE;
    } else if (KeyData.Key.UnicodeChar == 'o') {
      KeyData.Key.UnicodeChar = 0xF4;
    } else if (KeyData.Key.UnicodeChar == 'u') {
      KeyData.Key.UnicodeChar = 0xFB;
    } else if (KeyData.Key.UnicodeChar == 'A') {
      KeyData.Key.UnicodeChar = 0xC2;
    } else if (KeyData.Key.UnicodeChar == 'E') {
      KeyData.Key.UnicodeChar = 0xCA;
    } else if (KeyData.Key.UnicodeChar == 'I') {
      KeyData.Key.UnicodeChar = 0xCE;
    } else if (KeyData.Key.UnicodeChar == 'O') {
      KeyData.Key.UnicodeChar = 0xD4;
    } else if (KeyData.Key.UnicodeChar == 'U') {
      KeyData.Key.UnicodeChar = 0xDB;
    } else {
      KeyData.Key.UnicodeChar = '^';
    }
    if (ConsoleIn->CircumflexAccentOn > 3)
      ConsoleIn->CircumflexAccentOn = 0;
  }

  if (ConsoleIn->TildeOn) {
    ConsoleIn->TildeOn += 1;
    if (KeyData.Key.UnicodeChar == 'a') {
      KeyData.Key.UnicodeChar = 0xE3;
    } else if (KeyData.Key.UnicodeChar == 'A') {
      KeyData.Key.UnicodeChar = 0xC3;
    } else if (KeyData.Key.UnicodeChar == 'n') {
      KeyData.Key.UnicodeChar = 0xF1;
    } else if (KeyData.Key.UnicodeChar == 'N') {
      KeyData.Key.UnicodeChar = 0xD1;
    } else if (KeyData.Key.UnicodeChar == 'o') {
      KeyData.Key.UnicodeChar = 0xF5;
    } else if (KeyData.Key.UnicodeChar == 'O') {
      KeyData.Key.UnicodeChar = 0xD5;
    } else {
      KeyData.Key.UnicodeChar = '~';
    }
    if (ConsoleIn->TildeOn > 3)
      ConsoleIn->TildeOn = 0;
  }

  if (ConsoleIn->GraveAccentOn) {
    ConsoleIn->GraveAccentOn += 1;
    if (KeyData.Key.UnicodeChar == 'a') {
      KeyData.Key.UnicodeChar = 0xE0;
    } else if (KeyData.Key.UnicodeChar == 'e') {
      KeyData.Key.UnicodeChar = 0xE8;
    } else if (KeyData.Key.UnicodeChar == 'i') {
      KeyData.Key.UnicodeChar = 0xEC;
    } else if (KeyData.Key.UnicodeChar == 'o') {
      KeyData.Key.UnicodeChar = 0xF2;
    } else if (KeyData.Key.UnicodeChar == 'u') {
      KeyData.Key.UnicodeChar = 0xF9;
    } else if (KeyData.Key.UnicodeChar == 'A') {
      KeyData.Key.UnicodeChar = 0xC0;
    } else if (KeyData.Key.UnicodeChar == 'E') {
      KeyData.Key.UnicodeChar = 0xC8;
    } else if (KeyData.Key.UnicodeChar == 'I') {
      KeyData.Key.UnicodeChar = 0xCC;
    } else if (KeyData.Key.UnicodeChar == 'O') {
      KeyData.Key.UnicodeChar = 0xD2;
    } else if (KeyData.Key.UnicodeChar == 'U') {
      KeyData.Key.UnicodeChar = 0xD9;
    } else {
      KeyData.Key.UnicodeChar = 0x60;
    }
    if (ConsoleIn->GraveAccentOn> 3)
      ConsoleIn->GraveAccentOn = 0;
  }

  if ((KeyDescriptor->AffectedAttribute & EFI_AFFECTED_BY_NUM_LOCK) != 0) {
    //
    // For key affected by NumLock, if NumLock is on and Shift is not pressed, then it means
    // normal key, instead of original control key. So the ScanCode should be cleaned.
    // Otherwise, it means control key, so preserve the EFI Scan Code and clear the unicode keycode.
    //
    if ((ConsoleIn->NumLock) && (!(ConsoleIn->LeftShift || ConsoleIn->RightShift))) {
      KeyData.Key.ScanCode = SCAN_NULL;
    } else {
      KeyData.Key.UnicodeChar = CHAR_NULL;
    }
  }

  if (ConsoleIn->LeftCtrl || ConsoleIn->RightCtrl) {
    for (Index = 0; Index < sizeof (CtrlIgnoreKey); Index++) {
      if (ScanCode == CtrlIgnoreKey[Index]) {
         return;
      }
    }
  }

  if((Extend0) &&
     (KeyData.Key.UnicodeChar != 0x00) &&
     (KeyData.Key.UnicodeChar != 0x0d) &&
     (KeyData.Key.UnicodeChar!= 0x2f)) {
    KeyData.Key.UnicodeChar = 0x00;
  }
  //
  // If the key can not be converted then just return.
  //
  if (KeyData.Key.ScanCode == SCAN_NULL && KeyData.Key.UnicodeChar == CHAR_NULL) {
    if (!ConsoleIn->IsSupportPartialKey) {
      return ;
    }
  }

  //
  // Invoke notification functions if exist
  //
  for (Link = GetFirstNode (&ConsoleIn->NotifyList); !IsNull (&ConsoleIn->NotifyList, Link); Link = GetNextNode (&ConsoleIn->NotifyList, Link)) {
    CurrentNotify = CR (
                      Link,
                      KEYBOARD_CONSOLE_IN_EX_NOTIFY,
                      NotifyEntry,
                      KEYBOARD_CONSOLE_IN_EX_NOTIFY_SIGNATURE
                      );
    if (IsKeyRegistered (&CurrentNotify->KeyData, &KeyData)) {
      CurrentNotify->KeyNotificationFn (&KeyData);
    }
  }

  PushEfikeyBufTail (&ConsoleIn->EfiKeyQueue, &KeyData);
}

EFI_STATUS
CheckAuxStatus (
  OUT BOOLEAN                             *AuxEnable
  )
{
  EFI_STATUS              Status;
  EFI_HANDLE              *HandleBuffer;
  UINTN                   NumberOfHandles;
  UINTN                   Index;
  VOID                    *Protocol;

  *AuxEnable = FALSE;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimplePointerProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Index = 0; Index < NumberOfHandles; Index++) {

    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiIsaIoProtocolGuid,
                    &Protocol
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    *AuxEnable = TRUE;
    break;
  }

  FreePool (HandleBuffer);

  return EFI_SUCCESS;
}

/**
  Perform 8042 controller and keyboard Initialization.
  If ExtendedVerification is TRUE, do additional test for
  the keyboard interface

  @param ConsoleIn - KEYBOARD_CONSOLE_IN_DEV instance pointer
  @param ExtendedVerification - indicates a thorough initialization

  @retval EFI_DEVICE_ERROR Fail to init keyboard
  @retval EFI_SUCCESS      Success to init keyboard
**/
EFI_STATUS
InitKeyboard (
  IN OUT KEYBOARD_CONSOLE_IN_DEV *ConsoleIn,
  IN BOOLEAN                     ExtendedVerification
  )
{
  EFI_STATUS              Status;
  EFI_STATUS              Status1;
  UINT8                   CommandByte;
  EFI_PS2_POLICY_PROTOCOL *Ps2Policy;
  UINT32                  TryTime;
  BOOLEAN                 MouseEnable;

  Status                 = EFI_SUCCESS;
  mEnableMouseInterface  = TRUE;
  TryTime                = 0;

  //
  // Get Ps2 policy to set this
  //
  Ps2Policy = NULL;
  Status = gBS->LocateProtocol (
                  &gEfiPs2PolicyProtocolGuid,
                  NULL,
                  (VOID **) &Ps2Policy
                  );
  if (EFI_ERROR (Status)) {
    Ps2Policy = NULL;
  }

  REPORT_STATUS_CODE_WITH_DEVICE_PATH (
    EFI_PROGRESS_CODE,
    EFI_PERIPHERAL_KEYBOARD | EFI_P_KEYBOARD_PC_CLEAR_BUFFER,
    ConsoleIn->DevicePath
    );

  //
  // Perform a read to cleanup the Status Register's
  // output buffer full bits within MAX TRY times
  //
  if ((KeyReadStatusRegister (ConsoleIn) & KEYBOARD_STATUS_REGISTER_HAS_OUTPUT_DATA)) {
    while (!EFI_ERROR (Status) && TryTime < KEYBOARD_MAX_TRY) {
      Status = KeyboardRead (ConsoleIn, &CommandByte);
      TryTime ++;
    }
  }
  //
  // Exceed the max try times. The device may be error.
  //
  if (TryTime == KEYBOARD_MAX_TRY) {
  	Status = EFI_DEVICE_ERROR;
  	goto Done;
  }
  //
  // We should disable mouse interface during the initialization process
  // since mouse device output could block keyboard device output in the
  // 60H port of 8042 controller.
  //
  // So if we are not initializing 8042 controller for the
  // first time, we have to remember the previous mouse interface
  // enabling state
  //
  // Test the system flag in to determine whether this is the first
  // time initialization
  //
  if ((KeyReadStatusRegister (ConsoleIn) & KEYBOARD_STATUS_REGISTER_SYSTEM_FLAG) != 0) {
    if (!PcdGetBool (PcdFastPS2Detection)) {
      //
      // 8042 controller is already setup (by myself or by mouse driver):
      //   See whether mouse interface is already enabled
      //   which determines whether we should enable it later
      //
      //
      // Read the command byte of 8042 controller
      //
      Status = KeyboardCommand (ConsoleIn, KEYBOARD_8042_COMMAND_READ);
      if (EFI_ERROR (Status)) {
        KeyboardError (ConsoleIn, L"\n\r");
        goto Done;
      }

      Status = KeyboardRead (ConsoleIn, &CommandByte);
      if (EFI_ERROR (Status)) {
        KeyboardError (ConsoleIn, L"\n\r");
        goto Done;
      }
      //
      // Test the mouse enabling bit
      //
      if ((CommandByte & 0x20) != 0) {
        mEnableMouseInterface = FALSE;
      } else {
        mEnableMouseInterface = TRUE;
      }
    } else {
      mEnableMouseInterface = FALSE;
    }
  } else {
    //
    // 8042 controller is not setup yet:
    //   8042 controller selftest;
    //   Don't enable mouse interface later.
    //
    //
    // Disable keyboard and mouse interfaces
    //
    MouseEnable = FALSE;
    CheckAuxStatus (&MouseEnable);

    if (!PcdGetBool (PcdFastPS2Detection) && !MouseEnable) {
      Status = KeyboardCommand (ConsoleIn, KEYBOARD_8042_COMMAND_DISABLE_KEYBOARD_INTERFACE);
      if (EFI_ERROR (Status)) {
        KeyboardError (ConsoleIn, L"\n\r");
        goto Done;
      }

      Status = KeyboardCommand (ConsoleIn, KEYBOARD_8042_COMMAND_DISABLE_MOUSE_INTERFACE);
      if (EFI_ERROR (Status)) {
        KeyboardError (ConsoleIn, L"\n\r");
        goto Done;
      }

      REPORT_STATUS_CODE_WITH_DEVICE_PATH (
        EFI_PROGRESS_CODE,
        EFI_PERIPHERAL_KEYBOARD | EFI_P_KEYBOARD_PC_SELF_TEST,
        ConsoleIn->DevicePath
        );
      //
      // 8042 Controller Self Test
      //
      Status = KeyboardCommand (ConsoleIn, KEYBOARD_8042_COMMAND_CONTROLLER_SELF_TEST);
      if (EFI_ERROR (Status)) {
        KeyboardError (ConsoleIn, L"8042 controller command write error!\n\r");
        goto Done;
      }

      Status = KeyboardWaitForValue (ConsoleIn, 0x55);
      if (EFI_ERROR (Status)) {
        KeyboardError (ConsoleIn, L"8042 controller self test failed!\n\r");
        goto Done;
      }
    }
    //
    // Don't enable mouse interface later
    //
    mEnableMouseInterface = FALSE;
    if (MouseEnable) {
      mEnableMouseInterface = TRUE;
    }
  }

  if (Ps2Policy != NULL) {
    Ps2Policy->Ps2InitHardware (ConsoleIn->Handle);
  }
  //
  // Write 8042 Command Byte, set System Flag
  // While at the same time:
  //  1. disable mouse interface,
  //  2. enable kbd interface,
  //  3. enable PC/XT kbd translation mode
  //  4. enable mouse and kbd interrupts
  //
  //  ( Command Byte bits:
  //  7: Reserved
  //  6: PC/XT translation mode
  //  5: Disable Auxiliary device interface
  //  4: Disable keyboard interface
  //  3: Reserved
  //  2: System Flag
  //  1: Enable Auxiliary device interrupt
  //  0: Enable Keyboard interrupt )
  //
  Status = KeyboardCommand (ConsoleIn, KEYBOARD_8042_COMMAND_WRITE);
  if (EFI_ERROR (Status)) {
    KeyboardError (ConsoleIn, L"8042 controller command write error!\n\r");
    goto Done;
  }

  Status = KeyboardWrite (ConsoleIn, 0x67);
  if (EFI_ERROR (Status)) {
    KeyboardError (ConsoleIn, L"8042 controller data write error!\n\r");
    goto Done;
  }

  //
  // Clear Memory Scancode Buffer
  //
  ConsoleIn->ScancodeQueue.Head = 0;
  ConsoleIn->ScancodeQueue.Tail = 0;
  ConsoleIn->EfiKeyQueue.Head   = 0;
  ConsoleIn->EfiKeyQueue.Tail   = 0;

  //
  // Reset the status indicators
  //
  ConsoleIn->LeftCtrl   = FALSE;
  ConsoleIn->RightCtrl  = FALSE;
  ConsoleIn->LeftAlt    = FALSE;
  ConsoleIn->RightAlt   = FALSE;
  ConsoleIn->LeftShift  = FALSE;
  ConsoleIn->RightShift = FALSE;
  ConsoleIn->LeftLogo   = FALSE;
  ConsoleIn->RightLogo  = FALSE;
  ConsoleIn->Menu       = FALSE;
  ConsoleIn->SysReq     = FALSE;
  ConsoleIn->AltGrOn                   = FALSE;
  ConsoleIn->CircumflexAccentOn        = 0;
  ConsoleIn->CircumflexAccentShiftedOn = 0;
  ConsoleIn->GraveAccentOn             = 0;
  ConsoleIn->TildeOn                   = 0;
  ConsoleIn->CurrentNsKey              = NULL;

  ConsoleIn->IsSupportPartialKey = FALSE;
  //
  // For reseting keyboard is not mandatory before booting OS and sometimes keyboard responses very slow,
  // and to support KB hot plug, we need to let the InitKB succeed no matter whether there is a KB device connected
  // to system. So we only do the real reseting for keyboard when user asks and there is a real KB connected t system,
  // and normally during booting an OS, it's skipped.
  //
  if (ExtendedVerification && CheckKeyboardConnect (ConsoleIn)) {
    //
    // Additional verifications for keyboard interface
    //
    //
    // Keyboard Interface Test
    //
    Status = KeyboardCommand (ConsoleIn, KEYBOARD_8042_COMMAND_KEYBOARD_INTERFACE_SELF_TEST);
    if (EFI_ERROR (Status)) {
      KeyboardError (ConsoleIn, L"8042 controller command write error!\n\r");
      goto Done;
    }

    Status = KeyboardWaitForValue (ConsoleIn, 0x00);
    if (EFI_ERROR (Status)) {
      KeyboardError (
        ConsoleIn,
        L"Some specific value not aquired from 8042 controller!\n\r"
        );
      goto Done;
    }
    //
    // Keyboard reset with a BAT(Basic Assurance Test)
    //
    Status = KeyboardWrite (ConsoleIn, KEYBOARD_8048_COMMAND_RESET);
    if (EFI_ERROR (Status)) {
      KeyboardError (ConsoleIn, L"8042 controller data write error!\n\r");
      goto Done;
    }

    Status = KeyboardWaitForValue (ConsoleIn, KEYBOARD_8048_RETURN_8042_ACK);
    if (EFI_ERROR (Status)) {
      KeyboardError (ConsoleIn, L"Some specific value not aquired from 8042 controller!\n\r");
      goto Done;
    }
    //
    // wait for BAT completion code
    //
    mWaitForValueTimeOut  = KEYBOARD_BAT_TIMEOUT;

    Status                = KeyboardWaitForValue (ConsoleIn, KEYBOARD_8048_RETURN_8042_BAT_SUCCESS);
    if (EFI_ERROR (Status)) {
      KeyboardError (ConsoleIn, L"Keyboard self test failed!\n\r");
      goto Done;
    }

    mWaitForValueTimeOut = KEYBOARD_WAITFORVALUE_TIMEOUT;

    //
    // Set Keyboard to use Scan Code Set 2
    //
    Status = KeyboardWrite (ConsoleIn, KEYBOARD_8048_COMMAND_SELECT_SCAN_CODE_SET);
    if (EFI_ERROR (Status)) {
      KeyboardError (ConsoleIn, L"8042 controller data write error!\n\r");
      goto Done;
    }

    Status = KeyboardWaitForValue (ConsoleIn, KEYBOARD_8048_RETURN_8042_ACK);
    if (EFI_ERROR (Status)) {
      KeyboardError (ConsoleIn, L"Some specific value not aquired from 8042 controller!\n\r");
      goto Done;
    }

    Status = KeyboardWrite (ConsoleIn, 0x02);
    if (EFI_ERROR (Status)) {
      KeyboardError (ConsoleIn, L"8042 controller data write error!!\n\r");
      goto Done;
    }

    Status = KeyboardWaitForValue (ConsoleIn, KEYBOARD_8048_RETURN_8042_ACK);
    if (EFI_ERROR (Status)) {
      KeyboardError (ConsoleIn, L"Some specific value not aquired from 8042 controller!\n\r");
      goto Done;
    }

    //
    // Clear Keyboard Scancode Buffer
    //
    Status = KeyboardWrite (ConsoleIn, KEYBOARD_8048_COMMAND_CLEAR_OUTPUT_DATA);
    if (EFI_ERROR (Status)) {
      KeyboardError (ConsoleIn, L"8042 controller data write error!\n\r");
      goto Done;
    }

    Status = KeyboardWaitForValue (ConsoleIn, KEYBOARD_8048_RETURN_8042_ACK);
    if (EFI_ERROR (Status)) {
      KeyboardError (ConsoleIn, L"Some specific value not aquired from 8042 controller!\n\r");
      goto Done;
    }
  }

  //
  // Update Keyboard Lights
  //
  Status = UpdateStatusLights (ConsoleIn);
  if (EFI_ERROR (Status)) {
    KeyboardError (ConsoleIn, L"Update keyboard status lights error!\n\r");
    goto Done;
  }
  //
  // At last, we can now enable the mouse interface if appropriate
  //
Done:

  if (mEnableMouseInterface) {
    //
    // Enable mouse interface
    //
    Status1 = KeyboardCommand (ConsoleIn, KEYBOARD_8042_COMMAND_ENABLE_MOUSE_INTERFACE);
    if (EFI_ERROR (Status1)) {
      KeyboardError (ConsoleIn, L"8042 controller command write error!\n\r");
      return EFI_DEVICE_ERROR;
    }
  }

  if (!EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  } else {
    return EFI_DEVICE_ERROR;
  }

}

/**
  Disable the keyboard interface of the 8042 controller.

  @param ConsoleIn   The device instance

  @return status of issuing disable command

**/
EFI_STATUS
DisableKeyboard (
  IN KEYBOARD_CONSOLE_IN_DEV *ConsoleIn
  )
{
  EFI_STATUS  Status;

  //
  // Disable keyboard interface
  //
  Status = KeyboardCommand (ConsoleIn, KEYBOARD_8042_COMMAND_DISABLE_KEYBOARD_INTERFACE);
  if (EFI_ERROR (Status)) {
    KeyboardError (ConsoleIn, L"\n\r");
    return EFI_DEVICE_ERROR;
  }

  return Status;
}

/**
  Check whether there is Ps/2 Keyboard device in system by 0xF4 Keyboard Command
  If Keyboard receives 0xF4, it will respond with 'ACK'. If it doesn't respond, the device
  should not be in system.

  @param[in]  ConsoleIn             Keyboard Private Data Structure

  @retval     TRUE                  Keyboard in System.
  @retval     FALSE                 Keyboard not in System.
**/
BOOLEAN
EFIAPI
CheckKeyboardConnect (
  IN KEYBOARD_CONSOLE_IN_DEV *ConsoleIn
  )
{
  EFI_STATUS     Status;
  UINTN          WaitForValueTimeOutBcakup;
  UINT8          Values[2];
  UINT8          Value;

  Status = EFI_SUCCESS;
  //
  // clear previous data
  //
  while (KeyReadStatusRegister (ConsoleIn) & KBC_STSREG_VIA64_OUTB) {
    KeyReadDataRegister (ConsoleIn);
    gBS->Stall (15);
  }
  //
  // enable keyboard itself and wait for its ack
  // If can't receive ack, Keyboard should not be connected.
  //
  if (!PcdGetBool (PcdFastPS2Detection)) {
    Status = KeyboardWrite (
               ConsoleIn,
               KEYBOARD_KBEN
               );

    if (EFI_ERROR (Status)) {
      return FALSE;
    }
    //
    // wait for 1s
    //
    WaitForValueTimeOutBcakup = mWaitForValueTimeOut;
    mWaitForValueTimeOut = KEYBOARD_WAITFORVALUE_TIMEOUT;
    //
    // Also try to check failed ACK value to reduce waiting time.
    //
    Value = 0;
    Values[0] = KEYBOARD_8048_RETURN_8042_ACK;
    Values[1] = KEYBOARD_8048_RETURN_8042_FAILED_ACK;
    Status = KeyboardWaitForValues (
               ConsoleIn,
               Values,
               sizeof (Values),
               &Value
               );
    mWaitForValueTimeOut = WaitForValueTimeOutBcakup;

    if (EFI_ERROR (Status) || Value == KEYBOARD_8048_RETURN_8042_FAILED_ACK) {
      return FALSE;
    }

    return TRUE;
  } else {
    return TRUE;
  }
}

/**

Routine Description:

  Check whether Ps/2 Keyboard device ready

Arguments:

  ConsoleIn - the device instance

Returns:

  EFI_DEVICE_ERROR          - need to reset Keyboard.
  EFI_SUCCESS               - Keyboard ready
**/
EFI_STATUS
EFIAPI
CheckKeyboardStatus (
  IN OUT KEYBOARD_CONSOLE_IN_DEV *ConsoleIn
  )
{
  EFI_STATUS                                Status;
  UINT8                                     Values[2];
  UINT8                                     Value;

  Status = EFI_DEVICE_ERROR;
  //
  // First do keyboard reset sequence, if reset cmd success during PEI phase.
  //
  if (KeyReadStatusRegister (ConsoleIn) & KBC_STSREG_VIA64_OUTB) {
    //
    // KBC should return 0xAA to indicate BAT test successful. Some KBC will return
    // KBC self test OK to indicate test successful and doesn't plug-in keyboard.
    //
    Value = 0;
    Values[0] = KEYBOARD_8048_RETURN_8042_BAT_SUCCESS;
    Values[1] = KEYBOARD_8048_RETURN_8042_KBC_SELT_TEST_SUCCESS;
    Status = KeyboardWaitForValues (ConsoleIn, Values, sizeof (Values), &Value);
    if (!EFI_ERROR (Status) && Value == KEYBOARD_8048_RETURN_8042_BAT_SUCCESS) {

      //
      // Set keyboard scan code set = 02 (standard configuration)
      //
      Status = KeyboardWrite (ConsoleIn, KEYBOARD_8048_COMMAND_SELECT_SCAN_CODE_SET);
      //
      // Some KBC doesn't support this command and may return failed ACK data.
      //
      Values[0] = KEYBOARD_8048_RETURN_8042_ACK;
      Values[1] = KEYBOARD_8048_RETURN_8042_FAILED_ACK;
      Status = KeyboardWaitForValues (ConsoleIn, Values, sizeof (Values), &Value);
      if (EFI_ERROR (Status)) {
        Status    = EFI_DEVICE_ERROR;
        goto Exit;
      }

      if (Value == KEYBOARD_8048_RETURN_8042_ACK) {
        Status = KeyboardWrite (ConsoleIn, 0x02);
        Status = KeyboardWaitForValue (ConsoleIn, KEYBOARD_8048_RETURN_8042_ACK);
        if (EFI_ERROR (Status)) {
          Status    = EFI_DEVICE_ERROR;
          goto Exit;
        }
      }
      //
      // Enable keyboard itself (not via KBC) by writing CMD F4 via 60H
      //
      Status = KeyboardWrite (ConsoleIn, KEYBOARD_8048_COMMAND_CLEAR_OUTPUT_DATA);
      Value = 0;
      Status = KeyboardWaitForValues (ConsoleIn, Values, sizeof (Values), &Value);
      if (EFI_ERROR (Status)) {
        Status    = EFI_DEVICE_ERROR;
        goto Exit;
      }
      if (Value == KEYBOARD_8048_RETURN_8042_FAILED_ACK) {
        goto Exit;
      }
      //
      // Perform KB checking by writing ABh via 64H
      //
      Status = KeyboardCommand (ConsoleIn, KEYBOARD_8042_COMMAND_KEYBOARD_INTERFACE_SELF_TEST);
      Status = KeyboardWaitForValue (ConsoleIn, 0x00);
      if (EFI_ERROR (Status)) {
        Status    = EFI_DEVICE_ERROR;
        goto Exit;
      }

      return EFI_SUCCESS;
    }
  }
Exit:

  return Status;
}

/**
  Timer handler for Repeat Key timer.
  PS/2 and USB keyboard LEDs are synchronal.

  @param[in]  ConsoleIn             Keyboard Private Data Structure

**/
VOID
Ps2KeyboardCheckLEDHandler (
  IN KEYBOARD_CONSOLE_IN_DEV *ConsoleIn
  )
{

  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *ConSplitterEx;
  EFI_KEY_TOGGLE_STATE              KeyToggleState;
  EFI_STATUS                        Status;

  KeyToggleState = ConsoleIn->IsSupportPartialKey ? EFI_TOGGLE_STATE_VALID | EFI_KEY_STATE_EXPOSED : EFI_TOGGLE_STATE_VALID;

  if (ConsoleIn->CapsLock) {
    KeyToggleState |= EFI_CAPS_LOCK_ACTIVE;
  }

  if (ConsoleIn->NumLock) {
    KeyToggleState |= EFI_NUM_LOCK_ACTIVE;
  }

  if (ConsoleIn->ScrollLock) {
    KeyToggleState |= EFI_SCROLL_LOCK_ACTIVE;
  }

  if (gST->ConsoleInHandle != NULL) {
    Status = gBS->HandleProtocol (
                    gST->ConsoleInHandle,
                    &gEfiSimpleTextInputExProtocolGuid,
                    (VOID **) &ConSplitterEx
                    );
    if (!EFI_ERROR (Status)) {
      //
      // Update all of ConInEx device State.
      //
      Status = ConSplitterEx->SetState (ConSplitterEx, &KeyToggleState);
      if (Status == EFI_SUCCESS) return;
    }
  }
  //
  // Update Ps2Keyboard State.
  //
  ConsoleIn->ConInEx.SetState (&ConsoleIn->ConInEx, &KeyToggleState);
  return;
}
