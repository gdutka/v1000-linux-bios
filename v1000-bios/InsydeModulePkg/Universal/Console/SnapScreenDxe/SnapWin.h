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

#ifndef _SNAP_WIN_H_
#define _SNAP_WIN_H_

#define WIN_MAX_WIDTH     128
#define WIN_MAX_HEIGHT     40
#define WIN_SCR_CENTER    -1
#define WIN_BORDER         1

#define TEXTOUT_CENTER    0x7FFF

typedef struct {
  INT16   X;
  INT16   Y;
  UINT16  Width;
  UINT16  Height;
} RECT;

typedef struct {
  RECT        Area;
  RECT        ClientArea;
  BOOLEAN     Border;
  BOOLEAN     IsShow;
  UINT16      CursorX;
  UINT16      CursorY;
  CHAR16      *Title;
  VOID        *TextBuffer;
  VOID        *GraphicBuffer;
} TWINDOW; 

VOID 
TWinGetCursor (
  TWINDOW    *Win,
  UINT16     *CursorX,
  UINT16     *CursorY
  );

VOID
TWinSetCursor (
  TWINDOW    *Win,
  UINT16     CursorX,
  UINT16     CursorY
  );
  
VOID
TWinSetAttribue (
  TWINDOW    *Win,
  UINTN      Attribute
  );

EFI_STATUS
TWinCreate (
  INT16       X,
  INT16       Y,
  UINT16      Width,
  UINT16      Height,
  BOOLEAN     Border,
  CHAR16      *Title,
  TWINDOW     **AWin
  );

EFI_STATUS
TWinDestroy (
  TWINDOW     *Win
  );

VOID
TWinClearArea (
  IN  TWINDOW    *Win,
  IN  RECT       *Rect,
  IN  UINTN       Attrib
  );

VOID
TWinPrintChar (
  TWINDOW   *Win,
  CHAR16      Ch
  );

EFI_STATUS
TWinPrint (
  TWINDOW     *Win,
  CHAR16      *Format,
  ...  
  );

EFI_STATUS
TWinPrintAt (
  TWINDOW   *Win,
  UINT16    X,
  UINT16    Y,
  CHAR16      *Format,
  ...  
  );
  
EFI_STATUS
TWinShow (
  TWINDOW    *Win
  );

VOID
TWinClearLine (
  TWINDOW  *Win,
  UINT16   LineNumber
  );

VOID
TWinTextOut (
  TWINDOW     *Win,
  INT16       X,
  INT16       Y,
  CHAR16      *Str
  );

#endif // _SNAP_WIN_H_
