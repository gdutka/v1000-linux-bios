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


#ifndef _SNAP_CONSOLE_H_
#define _SNAP_CONSOLE_H_

#include <Uefi.h>
#include <Protocol/SimpleTextOut.h>


#define GLYPH_WIDTH         EFI_GLYPH_WIDTH
#define GLYPH_HEIGHT        EFI_GLYPH_HEIGHT

VOID
GraphicsGetBackground (
  UINTN   X,
  UINTN   Y,
  UINTN   Width,
  UINTN   Height,
  VOID    *Buffer
  );

VOID 
GraphicsPutBackground (
  UINTN   X,
  UINTN   Y,
  UINTN   Width,
  UINTN   Height,
  VOID    *Buffer
  );

VOID
DrawHorizontalLine (
  IN  UINTN       x,
  IN  UINTN       y,
  IN  UINTN       Len
  );

VOID
DrawVerticalLine (
  IN  UINTN       x,
  IN  UINTN       y,
  IN  UINTN       Len
  );

VOID
ScoPrintCharAt (
  UINTN     ScrX,
  UINTN     ScrY,
  CHAR16    Ch
  );

EFI_STATUS
ScoSetAttribute (
  IN UINTN    Attribute
  );

EFI_STATUS
ScoSetCursorPosition (
  IN UINTN Column,
  IN INTN Row
  );
  
EFI_STATUS
ScoOutputString (
  IN CHAR16      *String
  );

EFI_STATUS
ScoQueryMode (
  OUT UINTN     *Columns,
  OUT UINTN     *Rows
  );

EFI_STATUS
ScoEnableCursor (
  IN BOOLEAN    Enable
  );

EFI_STATUS
ScoGetModeInfo (
  IN INT32    *CursorColumn,
  IN INT32    *CursorRow,
  IN BOOLEAN  *CursorVisible,
  IN INT32    *Attribute
  );

EFI_STATUS
InitializeSnapConOut (
  VOID
  );

#endif // _SNAP_CONSOLE_H_
