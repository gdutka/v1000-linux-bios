/** @file
  Function prototype for I2C Keyboard Driver

;******************************************************************************
;* Copyright (c) 2014 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _I2C_KEYBOARD_H
#define _I2C_KEYBOARD_H

EFI_STATUS
EFIAPI
KeyboardHandler (
  IN  VOID              *Data,
  IN  UINTN             DataLength,
  IN  VOID              *Context,
  IN  UINT32            Result
  );

VOID
KeyboardRepeatHandler (
  IN  EFI_EVENT         Event,
  IN  VOID              *Context
  );

VOID
InitQueue (
  IN OUT  I2C_SIMPLE_QUEUE   *Queue,
  IN      UINTN              ItemSize
  );

VOID
DestroyQueue (
  IN OUT I2C_SIMPLE_QUEUE   *Queue
  );

BOOLEAN
IsQueueEmpty (
  IN  I2C_SIMPLE_QUEUE   *Queue
  );

BOOLEAN
IsQueueFull (
  IN  I2C_SIMPLE_QUEUE   *Queue
  );

VOID
Enqueue (
  IN OUT  I2C_SIMPLE_QUEUE *Queue,
  IN      VOID             *Item,
  IN      UINTN            ItemSize
  );

EFI_STATUS
Dequeue (
  IN OUT  I2C_SIMPLE_QUEUE *Queue,
     OUT  VOID             *Item,
  IN      UINTN            ItemSize
  );

EFI_STATUS
ParseKey (
  IN  I2C_KB_DEV        *KbDev,
  OUT UINT8             *KeyChar
  );

EFI_STATUS
KeyCodeToEFIScanCode (
  IN  I2C_KB_DEV        *KbDev,
  IN  UINT8             KeyChar,
  OUT EFI_INPUT_KEY     *Key
  );

EFI_STATUS
InsertKeyCode (
  IN  I2C_KB_DEV        *KbDev,
  IN  UINT8             Key,
  IN  UINT8             Down
  );

EFI_STATUS
SetKeyLED (
  IN  I2C_KB_DEV        *KbDev
  );

VOID
SyncKbdLed (
  IN  I2C_KB_DEV        *KbDev
  );

/**
  Initialize the key state.

  @param  UsbKeyboardDevice     The USB_KB_DEV instance.
  @param  KeyState              A pointer to receive the key state information.
**/
VOID
InitializeKeyState (
  IN  I2C_KB_DEV        *KbDev,
  OUT EFI_KEY_STATE     *KeyState
  );

#endif
