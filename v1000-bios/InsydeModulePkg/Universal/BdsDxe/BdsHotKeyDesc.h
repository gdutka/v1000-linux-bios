/** @file
  Header file for BDS hot key description.

;******************************************************************************
;* Copyright (c) 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _H2O_BDS_HOT_KEY_DESC_H_
#define _H2O_BDS_HOT_KEY_DESC_H_

#include "Bds.h"
#include "String.h"
#include <Library/RectLib.h>

#define IS_VALID_PERCENTAGE(x)              ((BOOLEAN) (((INT32) x >= 0) && ((INT32) x <= 100)))

#define ORIGIN_ACTION_MASK                  0xFF000000
#define ORIGIN_VALUE_MASK                   0x00FFFFFF

EFI_STATUS
EFIAPI
BdsServicesGetBootDisplayMode (
  IN  H2O_BDS_SERVICES_PROTOCOL             *This,
  OUT UINT8                                 *BootDisplayMode
  );

EFI_STATUS
EFIAPI
BdsServicesSetHotKeyDescText (
  IN H2O_BDS_SERVICES_PROTOCOL              *This,
  IN EFI_HANDLE                             HotKeyHandle,
  IN UINT8                                  BootDisplayMode,
  IN CONST CHAR16                           *BeforeDescText OPTIONAL,
  IN CONST CHAR16                           *AfterDescText OPTIONAL
  );

EFI_STATUS
EFIAPI
BdsServicesSetHotKeyDescPosn (
  IN H2O_BDS_SERVICES_PROTOCOL              *This,
  IN EFI_HANDLE                             HotKeyHandle,
  IN UINT8                                  BootDisplayMode,
  IN INT32                                  X,
  IN UINT32                                 XUnit,
  IN UINT32                                 *XOrigin OPTIONAL,
  IN INT32                                  Y,
  IN UINT32                                 YUnit,
  IN UINT32                                 *YOrigin OPTIONAL
  );

EFI_STATUS
EFIAPI
BdsServicesSetHotKeyDescColor (
  IN H2O_BDS_SERVICES_PROTOCOL              *This,
  IN EFI_HANDLE                             HotKeyHandle,
  IN UINT8                                  BootDisplayMode,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL          ForegroundColor,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL          BackgroundColor
  );

EFI_STATUS
EFIAPI
BdsServicesSetHotKeyDescFont (
  IN H2O_BDS_SERVICES_PROTOCOL              *This,
  IN EFI_HANDLE                             HotKeyHandle,
  IN UINT8                                  BootDisplayMode,
  IN CONST CHAR8                            *FontName OPTIONAL,
  IN UINT16                                 FontSize,
  IN EFI_HII_FONT_STYLE                     FontStyle
  );

VOID
DisplayHotKeyAfterUserSelection (
  VOID
  );

EFI_STATUS
BdsHotKeyDescInit (
  VOID
  );

extern H2O_HOT_KEY_DESCRIPTION         mDefaultHotKeyDescPosn;
extern H2O_HOT_KEY_DESCRIPTION         mDefaultHotKeyDescQuietPosn;
#endif
